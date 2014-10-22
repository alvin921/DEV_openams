#ifndef EMAIL_DB_H
#define EMAIL_DB_H

#include "ams.h"


#define	EMAIL_DB_PATH		"x:/email"
#define	EMAIL_DB_FNAME	EMAIL_DB_PATH"/email"
#define	EMAIL_TAB_HEADER		"header"
#define	EMAIL_TAB_CONTENT	"content"
#define	EMAIL_TAB_UIDL		"uidl"



typedef struct {
	gu8		folder;
	gu8		read:1;
	gu8		priority:3;
	gu16	blocksize;		//2 邮件content分块存储，每个block大小
	gu32	mailsize;
	gu32	recvsize;
	dos_datetime_t	time;
	char *uidl;
	char	*ctype;
	char *subject;
	char * from;
	char * to;
	char * cc;
	char * bcc;
	char * reply_to;
}t_EmailHeader;

#define	EMAIL_NUM_MAX		1000
#define	TABHEAD_MAX		EMAIL_NUM_MAX
#define	TABCONTENT_MAX	(EMAIL_NUM_MAX*5)

#define	EMAIL_BLOCK_SIZE	(5120)

//2 TABHEAD各字段定义
#define	HEAD_DBF_FOLDER	"folder"
#define	HEAD_DBF_READ		"read"
#define	HEAD_DBF_PRIORITY		"priority"
#define	HEAD_DBF_BLOCKSIZE	"blocksize"
#define	HEAD_DBF_MAILSIZE		"totalsize"
#define	HEAD_DBF_RECVSIZE		"recvsize"	//2 已收
#define	HEAD_DBF_TIME			"time"		//2 时间戳
#define	HEAD_DBF_UIDL			"uidl"
#define	HEAD_DBF_CTYPE		"content_type"
#define	HEAD_DBF_SUBJECT		"subject"
#define	HEAD_DBF_FROM		"from"
#define	HEAD_DBF_TO		"to"
#define	HEAD_DBF_CC		"cc"
#define	HEAD_DBF_BCC		"bcc"
#define	HEAD_DBF_REPLYTO	"reply_to"

//2 TABContent各字段定义
#define	CONTENT_DBF_MAILID	"mailid"
#define	CONTENT_DBF_BLOCK	"block"
#define	CONTENT_DBF_CONTENT	"content"


//2 TABUidl各字段定义
#define	UIDL_DBF_MAIL		"mail"	//2 正在接收邮件的id，接收完毕则清0，如果未清0表示异常中断须重收
#define	UIDL_DBF_UIDL		"uidl"





#if defined(EMAIL_DB_C)

static const t_DBFieldInfo		head_db_fields[] = {
	{HEAD_DBF_FOLDER		, DBFIELD_TYPE_BYTE		, FALSE	, 0},
	{HEAD_DBF_READ			, DBFIELD_TYPE_BYTE 		, FALSE	, 0},	// count
	{HEAD_DBF_PRIORITY		, DBFIELD_TYPE_BYTE 		, FALSE , 0},	// count
	{HEAD_DBF_BLOCKSIZE	, DBFIELD_TYPE_WORD 	, FALSE	, 0},	// count
	{HEAD_DBF_MAILSIZE		, DBFIELD_TYPE_DWORD	, FALSE	, 0},	// count
	{HEAD_DBF_RECVSIZE		, DBFIELD_TYPE_DWORD	, FALSE	, 0},	// count
	{HEAD_DBF_TIME			, DBFIELD_TYPE_DATETIME	, FALSE	, 0},	// count
	{HEAD_DBF_UIDL			, DBFIELD_TYPE_STRING	, FALSE	, 0},	// count
	{HEAD_DBF_CTYPE		, DBFIELD_TYPE_STRING	, FALSE	, 0},	// count
	{HEAD_DBF_SUBJECT		, DBFIELD_TYPE_STRING	, FALSE	, 0},	// count
	{HEAD_DBF_FROM			, DBFIELD_TYPE_STRING	, FALSE	, 0},	// count
	{HEAD_DBF_TO			, DBFIELD_TYPE_STRING	, FALSE	, 0},	// count
	{HEAD_DBF_CC			, DBFIELD_TYPE_STRING	, FALSE	, 0},	// count
	{HEAD_DBF_BCC			, DBFIELD_TYPE_STRING	, FALSE	, 0},	// count
	{HEAD_DBF_REPLYTO		, DBFIELD_TYPE_STRING	, FALSE	, 0},	// count
};

static const t_DBFieldInfo		content_db_fields[] = {
	{CONTENT_DBF_MAILID	, DBFIELD_TYPE_WORD		, FALSE	, 0},	// tabhead记录号
	{CONTENT_DBF_BLOCK		, DBFIELD_TYPE_WORD		, FALSE	, 0},	// playlist记录号
	{CONTENT_DBF_CONTENT	, DBFIELD_TYPE_STRING	, FALSE	, 0},	// full path name
};

static const t_DBFieldInfo		uidl_db_fields[] = {
	{UIDL_DBF_MAIL		, DBFIELD_TYPE_WORD, FALSE	, 0},	// tabhead记录号
	{UIDL_DBF_UIDL		, DBFIELD_TYPE_STRING	, FALSE	, 0},	// full path name
};

#endif


BEGIN_DECLARATION

void		db_email_init(void);
t_DBRecordID db_email_head_new(t_HDataSet hds, t_EmailHeader *head);
void	email_head_free(t_EmailHeader *head);


END_DECLARATION



#endif /* EMAIL_DB_H */

