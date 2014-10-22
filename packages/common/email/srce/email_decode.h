#ifndef EMAIL_DECODE_H
#define EMAIL_DECODE_H

#include "ams.h"


typedef struct {
	char		*fname;
	int		fsize;
	gu32	offset;
}t_AttachInfo;

typedef struct {
	char		*text;
	int		text_len;
	int		text_charset;
	char		*html;
	int		html_len;
	t_AttachInfo *attach;
	int		attach_num;
}t_EmailBody;


BEGIN_DECLARATION
	
void	email_body_free(t_EmailBody *eb);
int	email_get_content(t_HDataSet hds, t_DBRecordID mailid, gu32 mailsize, gu32 blocksize, t_EmailBody *eb);

END_DECLARATION

#endif

