#define EMAIL_POP3_C


#include "pack_email.hi"

static t_SockConnectHandle g_pop3_sock = NULL;


#define case_x(err, msg)		case err: return msg
#if 0
char * POP3_errorMsg(int eCode)
{
	switch(eCode){
		case_x(POP_E_RESOLVE, "连接服务器失败");
		case_x(POP_E_CONNECT, "连接服务器失败");
		case_x(POP_E_MEMORY, "系统无可用内存");
		case_x(POP_E_PARAM, "接口参数有误");
		case_x(POP_E_NOOP, "NOOP命令失败: 可能POP3服务器异常");
		case_x(POP_E_RSET, "RSET命令失败: 可能POP3服务器异常");
		case_x(POP_E_USER, "USER命令失败: 可能POP3服务器异常");
		case_x(POP_E_PASS, "登录失败: 可能用户名或口令不正确或者用户正在其它地方登录");
		case_x(POP_E_APOP, "登录失败: 可能用户名或口令不正确或者用户正在其它地方登录");
		case_x(POP_E_STAT, "STAT命令失败: 可能POP3服务器异常");
		case_x(POP_E_QUIT, "QUIT命令失败: 可能POP3服务器异常");
		case_x(POP_E_USER_CANCEL, "用户取消收取邮件");
		case_x(POP_E_MODEM_CLOSE, "Modem已经关闭,操作中止");
		case_x(POP_E_COMMUNICATE, "通信中断");
		case_x(POP_E_NO_SUCH, "该邮件不存在");
		case_x(POP_E_NO_HEADER, "邮件语法错误: 没有邮件头");
		case_x(POP_E_SYNTAX, "邮件语法错误: 没有结束符");
		case_x(POP_E_NOT_SUPPORT, "命令不支持");
		case_x(POP_E_TIMEOUT, "网络访问超时");
		case_x(POP_E_TRUNC, "邮件过大，被截断！");
		case_x(POP_E_DISKFULL, "存储空间不够接受本封邮件,请先清理本地邮箱");
	}
	return (char *)"";
}
#endif

#define	B_INT	BIT_0	//2 参数是整数
#define	B_STR	BIT_1	//2 参数是字符串

typedef const struct {
	gu8	cmd;
	gu8	flag;
	char str[6];
}t_Pop3Command;

static t_Pop3Command ga_pop3_cmd[] = {
	{POP3_USER		, B_STR		, "USER"},
	{POP3_PASS		, B_STR		, "PASS"},
	{POP3_NOOP		, 0			, "NOOP"},
	{POP3_RSET		, 0			, "RSET"},
	{POP3_STAT		, 0			, "STAT"},
	{POP3_UIDL		, 0			, "UIDL"},
	{POP3_LIST		, B_INT		, "LIST"},
	{POP3_RETR		, B_INT		, "RETR"},
	{POP3_TOP		, B_INT		, "TOP"},
	{POP3_DELE		, B_INT		, "DELE"},
	{POP3_QUIT		, 0			, "QUIT"},
};

t_Pop3Command * __pop3_cmd_get(gu8 cmd)
{
	int i;
	for(i = 0; i < ARRAY_SIZE(ga_pop3_cmd); i++)
		if(ga_pop3_cmd[i].cmd == cmd)
			return &ga_pop3_cmd[i];
	return NULL;
}



gu8 g_pop3_state = POP3_NONE;
static gboolean g_pop3_ack_more = FALSE;	//2 TRUE表示第一行ack，还有后续，比如retr命令字会返回邮件体

static t_EmailTxRxCallback g_pop3_cb = NULL;
static t_HWindow g_pop3_win = NULL;

#define	POP3_REPORT_MSG(MSG)		if(g_pop3_cb)g_pop3_cb(g_pop3_win, POP3_REPORT, POP3_NONE, MSG, -1)
#define	POP3_NOTIFY(evt)		if(g_pop3_cb)g_pop3_cb(g_pop3_win, evt, POP3_NONE, NULL, 0)

#define	POP3_NCK(cmd, msg, len)			if(g_pop3_cb)g_pop3_cb(g_pop3_win, POP3_CMD_NCK, cmd, msg, len)
#define	POP3_ACK( cmd, msg, len)		if(g_pop3_cb)g_pop3_cb(g_pop3_win, POP3_CMD_ACK, cmd, msg, len)
#define	POP3_ACK_BODY( cmd, msg, len)	if(g_pop3_cb)g_pop3_cb(g_pop3_win, POP3_CMD_ACK_BODY, cmd, msg, len)


#define	ACK_TOKEN	"+OK"
#define 	NCK_TOKEN	"-ERR"

static char g_pop3_ack[128];
static int g_pop3_ack_len = 0;

void	pop3_set_callback(t_EmailTxRxCallback callback, t_HWindow handle)
{
	g_pop3_cb = callback;
	g_pop3_win = handle;
}

int	pop3_send(gu8 cmd, int i_arg, char *s_arg)
{
	int ret;
	char buf[60];
	t_Pop3Command *p = __pop3_cmd_get(cmd);

	if(p == NULL)
		return EBADPARM;

	if(p->flag & B_INT)
		sprintf(buf, "%s %d\r\n", p->str, i_arg);
	else if((p->flag & B_STR) && s_arg && *s_arg)
		sprintf(buf, "%s %s\r\n", p->str, s_arg);
	else
		sprintf(buf, "%s\r\n", p->str);
	if(cmd == POP3_TOP)
		strcat(buf, " 0");
	
	ret = sockconn_write(g_pop3_sock, buf, strlen(buf));
	g_pop3_ack_more = FALSE;
	g_pop3_ack_len = 0;
	if(ret >= 0){
		g_pop3_state = cmd;
		
		sprintf(buf, "%s----OK", p->str);
		POP3_NOTIFY(buf);
	}else{
		sprintf(buf, "%s----ERROR", p->str);
		POP3_NOTIFY(buf);
	}
	return ret;
}

static void	pop3_connect_cb(void *data, int error)
{
	char buf[40];
	if(error == SUCCESS){
		sprintf(buf, "Connect mail servier SUCCESS", error);
		POP3_REPORT_MSG(buf);
		POP3_NOTIFY(POP3_CONNECT_OK);
	}else{
		sprintf(buf, "Connect mail servier failed: %d", error);
		POP3_REPORT_MSG(buf);
		POP3_NOTIFY(POP3_CONNECT_FAILED);
		sockconn_destroy(g_pop3_sock);
		g_pop3_sock = NULL;
		g_pop3_state = POP3_NONE;
	}
		
}
static void	pop3_err_cb(void *data, int error)
{
	email_glob_t *glob = data;
	t_Pop3Command *p;
	char buf[40];
	
	switch(g_pop3_state){
		case POP3_NONE:
			break;
		case POP3_CONNECT:
		default:
			p = __pop3_cmd_get(g_pop3_state);
			if(p)
				sprintf(buf, "%s error: %d", p->str, error);
			//2 pop3_quit必须删除，否则会导致递归，因为sock已关闭，pop3_quit会写sock失败，又会调用err_cb，从而递归
			//pop3_quit();
			POP3_REPORT_MSG(buf);
			POP3_NOTIFY(POP3_CLOSED);
			pop3_close();
			break;
	}
	g_pop3_state = POP3_NONE;	
}

static void	pop3_read_cb(void *data, gu8 *_buffer, int len)
{
	email_glob_t *glob = data;
	char *buffer = (char*)_buffer;
	char msg[40];
	gu8 cmd;

	if(g_pop3_state == POP3_NONE)
		return;
	
	if(!g_pop3_ack_more){
		char *p = strchr((char*)buffer, '\n');
		if(p){
			strncpy(&g_pop3_ack[g_pop3_ack_len], buffer, p-buffer+1);
			g_pop3_ack_len += p-buffer+1;
			g_pop3_ack[g_pop3_ack_len] = 0;

			len -= p-buffer+1;
			buffer = p+1;
		}else{
			strncpy(&g_pop3_ack[g_pop3_ack_len], buffer, len);
			g_pop3_ack_len += len;
			g_pop3_ack[g_pop3_ack_len] = 0;
			return;
		}

		//2 ack第一行完整收取，开始解析
		g_pop3_ack_more = TRUE;

		//g_printf(g_pop3_ack);
		POP3_REPORT_MSG(g_pop3_ack);
		if(g_strnicmp(g_pop3_ack, ACK_TOKEN, strlen(ACK_TOKEN)) == 0){
			//2 收到ACK
			POP3_ACK(g_pop3_state, g_pop3_ack, g_pop3_ack_len);
		}else if(g_strnicmp(g_pop3_ack, NCK_TOKEN, strlen(NCK_TOKEN)) == 0){
			//2 收到NCK
			POP3_NCK(g_pop3_state, g_pop3_ack, g_pop3_ack_len);
		}
		if(g_pop3_state == POP3_QUIT){
			pop3_close();
		}
	}
	if(len)
		POP3_ACK_BODY(g_pop3_state, buffer, len);
}

void	pop3_close(void)
{
	if(g_pop3_sock){
		sockconn_disconnect(g_pop3_sock);
		sockconn_destroy(g_pop3_sock);
		g_pop3_sock = NULL;
	}
	g_pop3_state = POP3_NONE;
}

int pop3_connect(char *server)
{
	int ret;
	g_pop3_sock = sockconn_create("pop3", "tcp", 2048, app_get_data(), pop3_connect_cb, pop3_read_cb, pop3_err_cb);
	g_printf("g_pop3_sock = %x", g_pop3_sock);
	if(g_pop3_sock == NULL)
		return ESOCK_CREATE;
	g_pop3_ack_len = 0;
	g_pop3_ack_more = FALSE;
	ret = sockconn_connect(g_pop3_sock, server, 110);
	g_printf("sockconn_connect return: %d", ret);
	if(ret == SUCCESS)
		g_pop3_state = POP3_CONNECT;
	return ret;
}

//2 ACK: "+OK <count> <allmailsize>\n"
int pop3_stat_parse(char *msg, int len, int *count)
{
	*count = atoi(&msg[4]);
	return SUCCESS;;
}

//2 +OK\r\n
//2 <index> <uidl>\r\n
//2 <index> <uidl>\r\n
//2 <index> <uidl>\r\n
//2 <index> <uidl>\r\n
//2 .\r\n
int pop3_uidl_parse(char *msg, int len, int *count, char **uidl)
{
	if(count)
		*count = atoi(msg);
	while(isdigit(*msg))
		msg++;
	while(isspace(*msg))
		msg++;
	msg = g_str_trim(msg);
	if(uidl)*uidl = g_str_dup(msg);
	return SUCCESS;;
}
#if 0
int pop3_uidl_parse(char *msg, int len, char *uidl, int *count)
{
	while(1){
		if(DaemonUserCancelled())
			return POP_E_USER_CANCEL;
		q = eoln(p, &skip);
		if(q == NULL){
			if(p != buf){
				recvlen = recvlen-(p-buf);
				MemMove(buf, p, recvlen+1);
				p = buf;
			}
			ret = POP3_read(global->sfd, &buf[recvlen], buflen-recvlen);
			if(ret < 0)
				return ret;
			recvlen += ret;
			//printk("%d\tbytes received\n", ret);
			continue;
		}
		*q = 0;
		uidl[n].index = StrAToI(p);
		while(isdigit(*p))
			p++;
		while(isspace(*p))
			p++;
		StrNCopy(uidl[n].uid, p, UIDL_LEN);
		uidl[n++].uid[UIDL_LEN] = 0;
		q += skip;
		if(*q == '.'){
			if(StrCompare(q, (char *)".\r\n") != 0)
				timeout_read(global->sfd, RECVTIMEOUT, buf, buflen);
			break;
		}
		p = q;
	}
	*count = n;
}
#endif

int pop3_list_parse(char *msg, int len, int *mailsize)
{
	char *p;

	p = &msg[3];
	while(isspace(*p))
		p++;
	while(isdigit(*p))
		p++;
	while(isspace(*p))
		p++;
	*mailsize = atoi(p);
	return SUCCESS;
}

#define H_TOKEN_FROM			(char *)"From:"
#define H_TOKEN_SUBJECT		(char *)"Subject:"
#define H_TOKEN_DATE			(char *)"Date:"
#define H_TOKEN_TO				(char *)"To:"
#define H_TOKEN_CC				(char *)"Cc:"
#define H_TOKEN_BCC				(char *)"Bcc:"
#define H_TOKEN_CTYPE			(char *)"Content-Type:"
#define H_TOKEN_PRIORITY		(char *)"X-Priority:"
#define H_TOKEN_RETPATH		(char *)"Return-Path:"
#define H_TOKEN_REPLYTO		(char *)"Reply-To:"

int	__get_head_type(char *str)
{
	if(g_strnicmp(str, H_TOKEN_FROM, strlen(H_TOKEN_FROM)) == 0)
		return H_FROM;
	if(g_strnicmp(str, H_TOKEN_SUBJECT, strlen(H_TOKEN_SUBJECT)) == 0)
		return H_SUBJECT;
	if(g_strnicmp(str, H_TOKEN_DATE, strlen(H_TOKEN_DATE)) == 0)
		return H_DATE;
	if(g_strnicmp(str, H_TOKEN_TO, strlen(H_TOKEN_TO)) == 0)
		return H_TO;
	if(g_strnicmp(str, H_TOKEN_CC, strlen(H_TOKEN_CC)) == 0)
		return H_CC;
	if(g_strnicmp(str, H_TOKEN_BCC, strlen(H_TOKEN_BCC)) == 0)
		return H_BCC;
	if(g_strnicmp(str, H_TOKEN_CTYPE, strlen(H_TOKEN_CTYPE)) == 0)
		return H_CTYPE;
	if(g_strnicmp(str, H_TOKEN_PRIORITY, strlen(H_TOKEN_PRIORITY)) == 0)
		return H_PRIORITY;
	if(g_strnicmp(str, H_TOKEN_RETPATH, strlen(H_TOKEN_RETPATH)) == 0)
		return H_RETPATH;
	if(g_strnicmp(str, H_TOKEN_REPLYTO, strlen(H_TOKEN_REPLYTO)) == 0)
		return H_REPLYTO;
	return H_NONE;
}

char *	strnchr(char *buf, int len, gu8 ch)
{
	int i;
	for(i = 0; i < len; i++)
		if((gu8)buf[i] == ch)
			return &buf[i];
	return NULL;
}

static const char * strMonth[] = {
	(char *)"Jan",
	(char *)"Feb",
	(char *)"Mar",
	(char *)"Apr",
	(char *)"May",
	(char *)"Jun",
	(char *)"Jul",
	(char *)"Aug",
	(char *)"Sep",
	(char *)"Oct",
	(char *)"Nov",
	(char *)"Dec"
};

int FindMonth(char *str)
{
	int i;
	for(i = 0; i < 12; i++)
		if(g_strnicmp(str, strMonth[i], 3) == 0)
			return i;
	return 0;	
}

/*
* RFC822 Time: (Tue, )15 Oct 2003 08:45:09
* 其中星期是可选的
*/

char* rfc822_time2str(char *string, datetime_t *dt)
{
	sprintf((char *)string, "%d %s %d %.2d:%.2d:%.2d", dt->dt_day, (char *)strMonth[dt->dt_month?(dt->dt_month-1):0], dt->dt_year, dt->dt_hour, dt->dt_minute, dt->dt_second);
	return string;
}

datetime_t *rfc822_str2time(char *string, datetime_t *dt)
{
	char *p;

	p = strchr(string, (char)',');
	if(p)
		p++;
	else
		p = string;
	while(isspace(*p))
		p++;
	
	dt->dt_day = (atoi(p));

	while(isdigit(*p))
		p++;
	while(isspace(*p))
		p++;
	dt->dt_month = FindMonth(p)+1;

	while(!isspace(*p))
		p++;
	while(isspace(*p))
		p++;
	dt->dt_year = atoi(p);

	while(isdigit(*p))
		p++;
	while(isspace(*p))
		p++;
	dt->dt_hour = atoi(p);

	while(isdigit(*p))
		p++;
	while(!isdigit(*p))
		p++;
	dt->dt_minute = atoi(p);
	
	while(isdigit(*p))
		p++;
	while(!isdigit(*p))
		p++;
	dt->dt_second = atoi(p);

	return dt;
}

char * eoln(const char *str, int *skip_bytes)
{
	char *p;

	p = strstr(str, (char *)"\n");
	if(p){
		if(*(p - 1) == '\r'){
			if(skip_bytes)
				*skip_bytes = 2;
			p--;
		}else if(skip_bytes){
			*skip_bytes = 1;
		}
	}
	return p;
}

char *	StrNStr (const char *str, int len, const char *token)
{
    const char *p1, *p2, *tmp = str;
    char c1, c2;
    
    for(;;)
    {
        p1 = str; p2 = token;
        while((c2 = *p2++) != '\0')
        {
            do{
            	if(p1 - tmp >= len)
            		return NULL;
                if ((c1 = *p1++) == '\0')
                    return NULL;
            } while(c1 != c2);
        }
        p2--;
        if(p2-token == p1-str) 
            return (char *)str;
        str = p1 - (p2-token);
    }
    return NULL;
}

