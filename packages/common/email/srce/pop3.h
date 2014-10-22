#ifndef POP3_H
#define POP3_H


#include "ams.h"
//#include "pop3.h"


enum {
	POP3_NONE,
	POP3_CONNECT,
	POP3_USER,
	POP3_PASS,
	POP3_NOOP,
	POP3_RSET,
	POP3_STAT,
	POP3_UIDL,
	POP3_LIST,
	POP3_RETR,
	POP3_TOP,
	POP3_DELE,
	POP3_QUIT,

	POP3_CMD_N,
};

extern gu8 g_pop3_state;

enum{
	
	POP3_REPORT,	//2 report message

	//2 事件通知，无参数
	POP3_CONNECT_OK,	
	POP3_CONNECT_FAILED,
	POP3_CLOSED,

	//2 命令响应，后面带参数
	POP3_CMD_NCK,
	POP3_CMD_ACK,
	POP3_CMD_ACK_BODY,
};

typedef void	(*t_EmailTxRxCallback)(t_HWindow handle, gu8 evt, gu8 cmd, char *msg, int len);


#define	pop3_noop()			pop3_send(POP3_NOOP, 0, NULL)
#define	pop3_rset()			pop3_send(POP3_RSET, 0, NULL)
#define	pop3_user(user)		pop3_send(POP3_USER, 0, user)
#define	pop3_pass(pass)		pop3_send(POP3_PASS, 0, pass)
#define	pop3_stat()			pop3_send(POP3_STAT, 0, NULL)
#define	pop3_uidl()			pop3_send(POP3_UIDL, 0, NULL)
#define	pop3_list(index)		pop3_send(POP3_LIST, index, NULL)
#define	pop3_retr(index)		pop3_send(POP3_RETR, index, NULL)
#define	pop3_dele(index)	pop3_send(POP3_DELE, index, NULL)
#define	pop3_top(index)		pop3_send(POP3_TOP, index, NULL)
#define	pop3_quit()			pop3_send(POP3_QUIT, 0, NULL)

enum {
	H_NONE,
	H_FROM,
	H_SUBJECT,
	H_DATE,
	H_TO,
	H_CC,
	H_BCC,
	H_CTYPE,
	H_PRIORITY,
	H_RETPATH,
	H_REPLYTO,


	H_BODY
	
};


BEGIN_DECLARATION

void	pop3_set_callback(t_EmailTxRxCallback callback, t_HWindow handle);
int	pop3_send(gu8 cmd, int i_arg, char *s_arg);
int pop3_connect(char *server);
int pop3_stat_parse(char *msg, int len, int *count);
int pop3_uidl_parse(char *msg, int len, int *index, char **uidl);
int pop3_list_parse(char *msg, int len, int *mailsize);
void	pop3_close(void);

END_DECLARATION





#endif

