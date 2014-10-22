#define EMAIL_DB_C

#include "ams.h"
#include "email_db.h"



void		db_email_init(void)
{
	if(fsi_stat(EMAIL_DB_PATH, NULL)){
		fsi_mkdir(EMAIL_DB_PATH);
	}
	if(fsi_stat(EMAIL_DB_FNAME, NULL)){
		t_HDB h;
		
		g_printf("db file not exists, create it");
		g_db_create(EMAIL_DB_FNAME);
		h = g_db_open(EMAIL_DB_FNAME);
		if(g_object_valid(h)){
			g_db_create_table(h, EMAIL_TAB_HEADER, TABHEAD_MAX, head_db_fields, ARRAY_SIZE(head_db_fields));
			g_db_create_table(h, EMAIL_TAB_CONTENT, 0, content_db_fields, ARRAY_SIZE(content_db_fields));
			g_db_create_table(h, EMAIL_TAB_UIDL, EMAIL_NUM_MAX, uidl_db_fields, ARRAY_SIZE(uidl_db_fields));
			g_object_unref(h);
		}

	}else{
		g_printf("db file exists");
	}
}

t_DBRecordID db_email_head_new(t_HDataSet hds, t_EmailHeader *head)
{
	g_ds_AddNew(hds);
	g_ds_SetValue(hds, HEAD_DBF_FOLDER, 0);
	g_ds_SetValue(hds, HEAD_DBF_READ, head->read);
	g_ds_SetValue(hds, HEAD_DBF_PRIORITY, head->priority);
	g_ds_SetValue(hds, HEAD_DBF_BLOCKSIZE, EMAIL_BLOCK_SIZE);
	g_ds_SetValue(hds, HEAD_DBF_MAILSIZE, head->mailsize);
	g_ds_SetValue(hds, HEAD_DBF_RECVSIZE, head->recvsize);
	g_ds_SetValue(hds, HEAD_DBF_TIME, head->time);
	g_ds_SetString(hds, HEAD_DBF_UIDL, head->uidl);
	g_ds_SetString(hds, HEAD_DBF_CTYPE, head->ctype);
	g_ds_SetString(hds, HEAD_DBF_SUBJECT, head->subject);
	g_ds_SetString(hds, HEAD_DBF_FROM, head->from);
	g_ds_SetString(hds, HEAD_DBF_TO, head->to);
	g_ds_SetString(hds, HEAD_DBF_CC, head->cc);
	g_ds_SetString(hds, HEAD_DBF_BCC, head->bcc);
	g_ds_SetString(hds, HEAD_DBF_REPLYTO, head->reply_to);

	return g_ds_Update(hds);
}

void	email_head_free(t_EmailHeader *head)
{
	if(head){
		FREEIF(head->uidl);
		FREEIF(head->ctype);
		FREEIF(head->subject);
		FREEIF(head->from);
		FREEIF(head->to);
		FREEIF(head->cc);
		FREEIF(head->bcc);
		FREEIF(head->reply_to);
	}
}



