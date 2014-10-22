
#include "pack_email.hi"

enum{
	SEG_UNDEF,
	SEG_MULTIPART,
	SEG_MIXED = SEG_MULTIPART,
	SEG_RELATED,
	SEG_ALTERNATIVE,
	SEG_META,
	SEG_TEXT = SEG_META,
	SEG_HTML,
	SEG_APPLICATION,
	SEG_IMAGE,
	SEG_AUDIO,
	SEG_VIDEO,
};
enum{
	MIME_ENCODING_NONE,
	MIME_ENCODING_BASE64,
	MIME_ENCODING_QP,
};


typedef struct {
	char		*boundary;
	g_vector_t	*vect;
}t_MimeSegmentMultipart;

typedef struct {
	gu32	start;
	gu32	size;
	char		*name;	//2 only for attachment
 }t_MimeSegmentMeta;

typedef struct tagMimeType t_MimeSegment;
struct tagMimeType{
	gu8		type;
	gu8		lastline;
	gu8		encoding;	//2 only for SEG_META
	gu8		charset;		//2 only for SEG_META
	
	t_MimeSegment *parent;	//2 NULL表示toplevel
	union{
		t_MimeSegmentMultipart multipart;
		t_MimeSegmentMeta meta;
	}u;
 };



#define DECOMPOSE_FAIL_COMMENT		"\r\n\r\n这是一封HTML格式邮件，请在客户端浏览该邮件"

#define ENCODING_LEN			40
#define DISPOSITION_LEN			20
#define BOUNDARY_LEN			100
#define CHARSET_LEN				20
#define CONT_NAME_LEN			100
#define CONT_FNAME_LEN			100


enum {
	CH_NONE,
	CH_TYPE,
	CH_ENCODING,
	CH_DISPOSITION,

	CH_BODY,
	CH_EOF,

};
#define CH_TOKEN_TYPE			(char *)"Content-Type:"
#define CH_TOKEN_ENCODING		(char *)"Content-Transfer-Encoding:"
#define CH_TOKEN_DISPOSITION	(char *)"Content-Disposition:"

//2 for Content-Type
#define CH_TOKEN_NAME			(char *)"name="
#define CH_TOKEN_BOUND			(char *)"boundary="
#define CH_TOKEN_CHARSET		(char *)"charset="
//2 for Content-Disposition
#define CH_TOKEN_FNAME			(char *)"filename="


static int	__get_mime_head_type(char *str)
{
	if(g_strnicmp(str, CH_TOKEN_TYPE, strlen(CH_TOKEN_TYPE)) == 0)
		return CH_TYPE;
	if(g_strnicmp(str, CH_TOKEN_ENCODING, strlen(CH_TOKEN_ENCODING)) == 0)
		return CH_ENCODING;
	if(g_strnicmp(str, CH_TOKEN_DISPOSITION, strlen(CH_TOKEN_DISPOSITION)) == 0)
		return CH_DISPOSITION;
	return CH_NONE;
}

//2 将输入分离成两个字符串，返回属性值
static char *parse_mime_header_attr(char *buf)
{
	char *v, *p1, *p2;
	p1 = strchr(buf, '=');
	if(p1 == NULL)
		return NULL;
	*p1++=0;
	
	v = strchr(p1, '\"');
	if(v == NULL){
		//2 没有引号，直接将尾部的;去掉
		p2 = p1+strlen(p1)-1;
		while(p2>p1&&(*p2 == ';' || isspace(*p2)))
			*p2--=0;
	}else{
		p1 = v+1;
		p2 = strchr(p1, '\"');
		if(p2)
			*p2=0;
	}
	v = g_str_trim(p1);
	return v;
}

#define LINE_MAX	1024
typedef struct {
	gu32	mailsize;
	gu32	blocksize;
	
	char *line;	//2 
	gu16	line_max;
	gu16	linelen;	//2 line[]中有效数据长度，因为我们顺序加载block，某行可能分别在两个block中
	int parselen;

	t_MimeSegment *root;
	t_MimeSegment *current;
	//2 
	t_MimeSegment *seg_text;
	t_MimeSegment *seg_html;
	g_vector_t *vect_attach;
}t_MimeParser;

#define SEG_CHILDREN(seg)	((t_MimeSegment*)(seg))->u.multipart.vect
#define	MIME_SEG_IS_META(seg)		((seg)->type >= SEG_META)

t_MimeSegment *mime_segment_new(t_MimeSegment *parent)
{
	t_MimeSegment *seg;
	
	seg = (t_MimeSegment*)MALLOC(sizeof(t_MimeSegment));
	memset(seg, 0, sizeof(t_MimeSegment));
	if(parent){
		g_vector_add(SEG_CHILDREN(parent), seg, NULL);
	}
	seg->parent = parent;
	return seg;
}

void	mime_segment_free(t_MimeSegment *seg)
{
	if(seg){
		if(MIME_SEG_IS_META(seg)){
			FREEIF(seg->u.meta.name);
		}else{
			FREEIF(seg->u.multipart.boundary);
			g_vector_destroy(SEG_CHILDREN(seg), (GFreeFunc)mime_segment_free);
		}
		FREE(seg);
	}
}

static t_CharsetID __get_charset(char *charset)
{
	if(g_stricmp(charset, "gb2312") == 0)
		return CHARSET_GB2312;
	else if(g_stricmp(charset, "gbk") == 0)
		return CHARSET_GB2312;
	else if(g_stricmp(charset, "big5") == 0)
		return CHARSET_BIG5;
	else if(g_stricmp(charset, "UTF-8") == 0)
		return CHARSET_UTF8;
	else
		return CHARSET_UNDEF;
}

//2 已经解析的字节数存储在parser->parselen
int parse_mime_header(t_MimeParser *parser, char *line)
{
	char	*start, *temp;
	
	if(line[0] == '\n' || line[0] == '\r' && line[1] == '\n'){
		parser->current->lastline = CH_BODY;
		//2 记录meta segment的body起始地址
		if(parser->current->type >= SEG_META)
			parser->current->u.meta.start = parser->parselen;
		return SUCCESS;
	}
	if(isspace(line[0])){
		start = g_str_trim(line);
		switch(parser->current->lastline){
			case CH_TYPE:
				temp = parse_mime_header_attr(start);
				if(temp){
					if(g_stricmp(start, "boundary") == 0)
						parser->current->u.multipart.boundary = g_str_dup(temp);
					else if(g_stricmp(start, "charset") == 0)
						parser->current->charset = __get_charset(temp);
					else if(g_stricmp(start, "name") == 0)
						parser->current->u.meta.name = rfc2047_decode_simple(temp);
				}
				break;
			case CH_DISPOSITION:
				temp = parse_mime_header_attr(start);
				if(temp){
					if(g_stricmp(start, "filename") == 0 && parser->current->u.meta.name == NULL)
						parser->current->u.meta.name = rfc2047_decode_simple(temp);
				}
				break;
		}
		return SUCCESS;
	}
	parser->current->lastline = __get_mime_head_type(line);
	if(parser->current->lastline == CH_NONE){
		//2 暂时不支持的字段，直接忽略
		return SUCCESS;
	}
	start = strchr(line, ':');
	start = g_str_trim(start+1);
	switch(parser->current->lastline){
		case CH_TYPE:
			temp = strchr(start, ';');
			if(temp)
				*temp++ = 0;
			//2 创建新的Segment
			if(g_strnicmp(start, "multipart", strlen("multipart")) == 0){
				SEG_CHILDREN(parser->current) = g_vector_new(4, 4, VECTOR_TYPE_DYN_ALIGN);
				if(strstr(start, "/mixed"))
					parser->current->type = SEG_MIXED;
				else if(strstr(start, "/related"))
					parser->current->type = SEG_RELATED;
				else if(strstr(start, "/alternative"))
					parser->current->type = SEG_ALTERNATIVE;
				else
					g_raise("syntax error: unknown content-type(%s)", start);
			}else{
				if(g_strnicmp(start, "text/plain", strlen("text/plain")) == 0){
					parser->current->type = SEG_TEXT;
					parser->seg_text = parser->current;
				}else if(g_strnicmp(start, "text/html", strlen("text/html")) == 0){
					parser->current->type = SEG_HTML;
					parser->seg_html = parser->current;
				}else if(g_strnicmp(start, "application", strlen("application")) == 0){
					parser->current->type = SEG_APPLICATION;
					if(parser->vect_attach == NULL)
						parser->vect_attach = g_vector_new(4, 4, VECTOR_TYPE_DYN_ALIGN);
					g_vector_add(parser->vect_attach, parser->current, NULL);
				}else if(g_strnicmp(start, "image/", strlen("image/")) == 0){
					parser->current->type = SEG_IMAGE;
				}else if(g_strnicmp(start, "audio/", strlen("audio/")) == 0){
					parser->current->type = SEG_AUDIO;
				}else if(g_strnicmp(start, "video/", strlen("video/")) == 0)
					parser->current->type = SEG_VIDEO;
			}
			
			if(temp && *temp){	// followed by boundary or charset
				start = g_str_trim(temp);
				temp = parse_mime_header_attr(start);
				if(temp){
					if(g_stricmp(start, "boundary") == 0)
						parser->current->u.multipart.boundary = g_str_dup(temp);
					else if(g_stricmp(start, "charset") == 0)
						parser->current->charset = __get_charset(temp);
					else if(g_stricmp(start, "name") == 0)
						parser->current->u.meta.name = rfc2047_decode_simple(temp);
				}
			}
			break;
		case CH_ENCODING:
			if(parser->current == NULL || !MIME_SEG_IS_META(parser->current))
				break;
			if(g_stricmp(start, (char *)"base64") == 0)
				parser->current->encoding = MIME_ENCODING_BASE64;
			else if(g_stricmp(start, (char *)"quoted-printable") == 0)
				parser->current->encoding = MIME_ENCODING_QP;
			break;
		case CH_DISPOSITION:
			temp = strchr(start, ';');
			if(temp)
				*temp++ = 0;
			if(g_stricmp(start, "attachment") == 0 || g_stricmp(start, "inline") == 0){
			}
			if(temp && *temp){	// followed by boundary or charset
				start = g_str_trim(temp);
				temp = parse_mime_header_attr(start);
				if(temp){
					if(g_stricmp(start, "filename") == 0 && parser->current->u.meta.name == NULL)
						parser->current->u.meta.name = rfc2047_decode_simple(temp);
				}
			}
	}
	return SUCCESS;
}

int	mime_parser_scan(t_MimeParser *parser, char *buf, int len)
{
	int ret;
	char *crlf;
	int	plen, linelen;
	char	*start, *temp;

	while(len > 0 && parser->current){
		crlf = strnchr(buf, len, '\n');
		if(crlf == NULL){
			//2 没找到行尾结束符，表示后续数据在下一个block中或丢失
			memcpy(parser->line, buf, len);
			parser->linelen = len;
			return EMAIL_E_CORRUPT;
		}
		//2 加载一行数据到line中缓存
		linelen = crlf - buf + 1;
		if(linelen + parser->linelen > parser->line_max){
			gu8 *line;
			parser->line_max = linelen+parser->linelen;
			line = MALLOC(parser->line_max+1);
			if(parser->line && parser->linelen){
				memcpy(line, parser->line, parser->linelen);
			}
			FREEIF(parser->line);
			parser->line = line;
		}
		memcpy(&parser->line[parser->linelen], buf, linelen);
		parser->linelen += linelen;
		parser->line[parser->linelen] = 0;
		
		buf += linelen;
		len -= linelen;

		parser->parselen += parser->linelen;
		parser->linelen = 0; //2 复位，表示line中没有暂存数据

		if(parser->current->lastline < CH_BODY){
			//2 header还没解析完毕
			ret = parse_mime_header(parser, parser->line);
		}else{
			if(parser->current->type < SEG_META/* && parser->current->lastline == CH_BODY*/){
				//2 对于multipart segment, 还没检测到segment的开始
				start = strstr(parser->line, parser->current->u.multipart.boundary);
				if(start && parser->line == start-2 && *(start-1)=='-' && *(start-2)=='-'){
					if(start[strlen(parser->current->u.multipart.boundary)]!='-'){
						//2 分隔符
						//parser->current->lastline = CH_SEGMENT;
						parser->current = mime_segment_new(parser->current);
					}else{
						//2 结束符
						parser->current = parser->current->parent;
					}
					continue;
				}
			}
			//2 检测是否碰到parent segment的boundary
			if(parser->current->parent){
				start = strstr(parser->line, parser->current->parent->u.multipart.boundary);
				if(start && parser->line == start-2 && *(start-1)=='-' && *(start-2)=='-'){
					if(start[strlen(parser->current->parent->u.multipart.boundary)]!='-'){
						//2 分隔符
						parser->current = mime_segment_new(parser->current->parent);
					}else{
						//2 结束符
						parser->current = parser->current->parent;
					}
					continue;
				}
			}
			//2 未检测到boundary，先暂时记录meta segment大小为当前解析到的位置，以防数据有被截断
			if(parser->current->type >= SEG_META){
				parser->current->u.meta.size = parser->parselen-parser->current->u.meta.start;
			}
		}
	}
	return SUCCESS;

}

t_MimeParser *		mime_parser_new(void)
{
	t_MimeParser *parser = MALLOC(sizeof(t_MimeParser));
	if(parser){
		memset(parser, 0, sizeof(t_MimeParser));
		parser->root = mime_segment_new(NULL);
		parser->current = parser->root;
	}
	return parser;
}
void		mime_parser_free(t_MimeParser *parser)
{
	if(parser){
		g_vector_destroy(parser->vect_attach, NULL);
		mime_segment_free(parser->root);
		FREEIF(parser->line);
		FREE(parser);
	}
}

char *	mime_decode_seg(t_HDataSet hds, gu32 blocksize, gu32 seg_start, gu32 seg_size, gu8 encoding, int *plen)
{
	int block_s = seg_start/blocksize;
	int block_e = (seg_size+seg_start-1)/blocksize;
	char *text;
	int text_len = 0;
	
	gu8 *data;
	gu16 size;
	int len, off, i;

	text = MALLOC(seg_size);
	text_len = 0;
	
	for(i = block_s; i <= block_e; i++){
		g_ds_GotoByIndex(hds, i);
		data = g_ds_GetBinary(hds, CONTENT_DBF_CONTENT, &size);

		if(i == block_s){
			off = seg_start%blocksize;
		}else{
			off = 0;
		}
		if(block_s == block_e)
			len = seg_size;
		else if(i == block_s)
			len = size-off;
		else if(i == block_e)
			len = (seg_size+seg_start)%blocksize;
		else
			len = size;
		
		if(encoding == MIME_ENCODING_BASE64)
			len = Base64_decode(data+off, len, text+text_len);
		else if(encoding == MIME_ENCODING_QP)
			len = QP_decode(data+off, len, text+text_len);
		else
			memcpy(text+text_len, data+off, len);
		text_len += len;
	}
	if(plen)*plen = text_len;
	return text;
}

int	mime_parser_decode(t_HDataSet hds, t_MimeParser *parser, t_EmailBody *eb)
{
	if(parser->seg_text){
		eb->text = mime_decode_seg(hds, parser->blocksize, parser->seg_text->u.meta.start, parser->seg_text->u.meta.size, parser->seg_text->encoding, &eb->text_len);
		eb->text_charset = parser->seg_text->charset;
	}
	if(parser->seg_html)
		eb->html = mime_decode_seg(hds, parser->blocksize, parser->seg_html->u.meta.start, parser->seg_html->u.meta.size, parser->seg_html->encoding, &eb->html_len);
	eb->attach_num = g_vector_size(parser->vect_attach);
	if(parser->vect_attach && eb->attach_num){
		int i;
		t_MimeSegment *att;
		eb->attach = MALLOC(eb->attach_num * sizeof(t_AttachInfo));
		for(i = 0; i < g_vector_size(parser->vect_attach); i++){
			att = g_vector_element_at(parser->vect_attach, i);
			eb->attach[i].fname = g_str_dup(att->u.meta.name);
			eb->attach[i].fsize = att->u.meta.size;
			eb->attach[i].offset = att->u.meta.start;
		}
	}
}

void	email_body_free(t_EmailBody *eb)
{
	if(eb){
		int i;
		for(i = 0; i < eb->attach_num; i++)
			FREEIF(eb->attach[i].fname);
		FREEIF(eb->text);
		FREEIF(eb->html);
		FREEIF(eb->attach);
		eb->attach_num = 0;
		eb->html_len = 0;
	}
}

int	email_get_content(t_HDataSet hds, t_DBRecordID mailid, gu32 mailsize, gu32 blocksize, t_EmailBody *eb)
{
	int num = ALIGN_DIV(mailsize, blocksize);
	int index;
	gu8 *data;
	gu16 size;
	t_MimeParser *parser;

	if(num != g_ds_GetRecordNum(hds))
		return EMAIL_E_CORRUPT;
	if(eb)memset(eb, 0, sizeof(t_EmailBody));
	parser = mime_parser_new();
	parser->mailsize = mailsize;
	parser->blocksize = blocksize;
	
	for(index = 0; index < num; index++){
		g_ds_GotoByIndex(hds, index);
		g_raise_if_fail((index+1) == g_ds_GetValue(hds, CONTENT_DBF_BLOCK), "EMAIL_E_CORRUPT");
		data = g_ds_GetBinary(hds, CONTENT_DBF_CONTENT, &size);

		mime_parser_scan(parser, data, size);
	}

	mime_parser_decode(hds, parser, eb);
	mime_parser_free(parser);
	return SUCCESS;
}




