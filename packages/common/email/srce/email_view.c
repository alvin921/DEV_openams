#include "pack_email.hi"

#define X_OFF	5
#define Y_LINE	16
void		__show_email(t_HWindow handle, gboolean defer)
{
	email_glob_t *glob = app_get_data();
	char *from = g_ds_GetString(glob->hds_head, HEAD_DBF_FROM);
	char *to = g_ds_GetString(glob->hds_head, HEAD_DBF_TO);
	char *cc = g_ds_GetString(glob->hds_head, HEAD_DBF_CC);
	char *subject = g_ds_GetString(glob->hds_head, HEAD_DBF_SUBJECT);
	datetime_t dt;
	t_HGDI gdi = wgt_get_client_gdi((t_HWidget)handle);
	gui_rect_t client;
	coord_t x, y;
	char msg[40];
	int height;
	
	if(!g_object_valid(gdi))
		return;
	wgt_get_client_rect((t_HWidget)handle, &client);
	if(!defer)
		gdi_clear(gdi, 0, 0, client.dx, client.dy, 0);
	dostime_to_datetime(g_ds_GetValue(glob->hds_head, HEAD_DBF_TIME), &dt);
	//2 From: <xxx>
	x = X_OFF;
	y = 0;
	gdi_draw_text_in(gdi, x, y, LCD_WIDTH-x, Y_LINE, "From: ", -1, CHARSET_US_ASCII, IDF_VALIGN_MIDDLE|IDF_HALIGN_LEFT);
	gdi_draw_text(gdi, from, -1, CHARSET_UNDEF);
	//2 TO: <xxx>
	y += Y_LINE;
	gdi_draw_text_in(gdi, x, y, LCD_WIDTH-x, Y_LINE, "To: ", -1, CHARSET_US_ASCII, IDF_VALIGN_MIDDLE|IDF_HALIGN_LEFT);
	gdi_draw_text(gdi, to, -1, CHARSET_UNDEF);
	//2 TO: <xxx>
	if(cc){
		y += Y_LINE;
		gdi_draw_text_in(gdi, x, y, LCD_WIDTH-x, Y_LINE, "Cc: ", -1, CHARSET_US_ASCII, IDF_VALIGN_MIDDLE|IDF_HALIGN_LEFT);
		gdi_draw_text(gdi, cc, -1, CHARSET_UNDEF);
	}
	//2 Subject
	y += Y_LINE;
	gdi_draw_text_in(gdi, x, y, LCD_WIDTH-x, Y_LINE, "Subject: ", -1, CHARSET_US_ASCII, IDF_VALIGN_MIDDLE|IDF_HALIGN_LEFT);
	gdi_draw_text(gdi, subject, -1, CHARSET_UNDEF);
	//2 Time
	y += Y_LINE;
	rfc822_time2str(msg, &dt);
	gdi_draw_text_in(gdi, x, y, LCD_WIDTH-x, Y_LINE, "Time: ", -1, CHARSET_US_ASCII, IDF_VALIGN_MIDDLE|IDF_HALIGN_LEFT);
	gdi_draw_text(gdi, msg, -1, CHARSET_UNDEF);
	y += Y_LINE;
	{
		t_ColorRasterOp rop;
		rop = gdi_set_rop(gdi, IMG_RO_NOT);
		gdi_hline(gdi, y, x, LCD_WIDTH-x);
		gdi_set_rop(gdi, rop);
	}
	gdi_draw_text_in(gdi, x, y+Y_LINE, client.dx-x*2, 0x7000, glob->body.text, glob->body.text_len, glob->body.text_charset, IDF_TEXT_AUTO_WRAP|IDF_TEXT_MULTILINE|IDF_HALIGN_LEFT|IDF_VALIGN_TOP);

	if(!defer)
		gdi_blt(gdi, 0, 0, client.dx, client.dy);
	g_object_unref(gdi);
}

void	__load_email(email_glob_t *glob, t_HWindow handle, int act, gboolean redraw)
{
	int index;
	int mailsize;
	int blocksize;
	int mailid;

	index = glob->view_index+act;
	if(index < 0 || index >= g_ds_GetRecordNum(glob->hds_head))
		return;
	glob->view_index = index;
	
	g_ds_GotoByIndex(glob->hds_head, glob->view_index);
	mailid = g_ds_CurrentID(glob->hds_head);
	mailsize = g_ds_GetValue(glob->hds_head, HEAD_DBF_MAILSIZE);
	blocksize = g_ds_GetValue(glob->hds_head, HEAD_DBF_BLOCKSIZE); 
	if(blocksize == 0)
		blocksize = EMAIL_BLOCK_SIZE;
	
	g_ds_CondSet1(glob->hds_cont, CONTENT_DBF_MAILID, DBREL_EQ, mailid);
	g_ds_OrderBy(glob->hds_cont, CONTENT_DBF_BLOCK, TRUE, FALSE);
	g_ds_Query(glob->hds_cont, TRUE);
	
	email_body_free(&glob->body);
	email_get_content(glob->hds_cont, mailid, mailsize, blocksize, &glob->body);
	
	wgt_client_origin_x(handle) = wgt_client_origin_y(handle) = 0;
	wgt_client_drawing_dy(handle) = Y_LINE*10+font_measure_string_multiline(glob->body.text, glob->body.text_len, glob->body.text_charset, LCD_WIDTH-X_OFF*2, NULL, NULL);
	
	if(redraw)
		__show_email(handle, FALSE);
}

//3 sparam=<rid>, lparam=<hdb>
error_t	email_ViewWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	email_glob_t *glob = app_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
			{
				t_HWidget widget;
				gui_rect_t client;
				
				wgt_enable_attr(handle, WND_ATTR_DEFAULT|CTRL_ATTR_VSCROLLABLE);

				__load_email(glob, handle, 0, FALSE);
#if defined(FTR_SOFT_KEY)
				{
					t_SKHandle sk = wnd_get_skbar(handle);
					if(g_object_valid(sk)){
						sk_set_text(sk, SK_LSK, NULL, RESID_STR_OK);
						sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
					}
				}
#endif
			}
			
			
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
				if(lparam == MVK_LEFT){
					__load_email(glob, handle, -1, TRUE);
					ret = SUCCESS;
				}else if(lparam == MVK_RIGHT){
					__load_email(glob, handle, 1, TRUE);
					ret = SUCCESS;
				}
			}
			break;
		case WINDOW_OnClose:
			email_body_free(&glob->body);
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			__show_email(handle, TRUE);
			ret = SUCCESS;
			break;
	}
	return ret;
}

