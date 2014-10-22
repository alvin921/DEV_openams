#include "pack_email.hi"

typedef struct {
	t_HDataSet	hds;
}t_EmailMainWData;

static void	__email_notify(t_HDataSet hds, t_DBRecordID rid, void *userdata)
{
	t_HWindow handle = (t_HWindow)userdata;
	//email_glob_t *glob = task_get_data();
	if(g_object_valid(hds)){
		g_ds_OrderBy(hds, HEAD_DBF_TIME, FALSE, FALSE);
		g_ds_Query(hds, TRUE);

		if(wgt_is_focused(handle)){
			wgt_redraw(wnd_get_widget(handle, RESID_LIST_MAIN), FALSE);
		}

	}
}

#define	X_OFF	12

static void	db_email_redraw_record(t_HDataSet hds, int index, t_HGDI gdi, t_GuiViewType viewtype, int width, int height, gboolean hilight)
{
	char *from, *subject;
	gu8 flag;
	datetime_t curr_dt;
	dos_datetime_t dt;
	char dt_string[14];
	t_UITheme *theme = ui_theme_get();

	g_ds_GotoByIndex(hds, index);
	from = g_ds_GetString(hds, HEAD_DBF_FROM);
	subject = g_ds_GetString(hds, HEAD_DBF_SUBJECT);
	dt = g_ds_GetValue(hds, HEAD_DBF_TIME);

	g_printf("[%d] from=%s", index, from);

	if(VIEWTYPE_IS_NORMAL(viewtype)){
	#define X_MARGIN	10
		//2 Line 1: <name>						<time>
		gdi_set_color(gdi, theme->fg_color);
		gdi_draw_text_in(gdi, X_OFF, 0, width-X_OFF-30, height/2, from, -1, CHARSET_UNDEF, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
		gdi_set_color(gdi, RGB_BLUE);
		datetime_current(&curr_dt);
		if(curr_dt.dt_year == DOSDATE_Y(dt>>16) && curr_dt.dt_month == DOSDATE_M(dt>>16) && curr_dt.dt_day == DOSDATE_D(dt>>16))
			sprintf(dt_string, "%02d:%02d", DOSTIME_H(dt), DOSTIME_M(dt));
		else
			sprintf(dt_string, "%02d-%02d", DOSDATE_M(dt>>16), DOSDATE_D(dt>>16));
		gdi_draw_text_in(gdi, X_OFF, 0, width-X_OFF, height/2, dt_string, -1, CHARSET_US_ASCII, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
	
		//2 Line2: subject
		gdi_set_color(gdi, RGB_GRAY);
		gdi_draw_text_in(gdi, X_OFF, height/2, width-X_OFF, height/2, subject, -1, CHARSET_UNDEF, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
	}
}

void	db_email_view_record(t_HDataSet hds, int index)
{
	email_glob_t *glob = app_get_data();
	glob->view_index = index;	
	wnd_show(wnd_create(email_ViewWndMsgHandler, index, 0));
}

static void __uidl_list_free(email_glob_t *glob)
{
	if(glob->mail.uidl_list){
		int n;
		for(n = 0; n < glob->mail.count; n++){
			FREEIF(glob->mail.uidl_list[n].uidl);
		}
		FREE(glob->mail.uidl_list);
		glob->mail.uidl_list = NULL;
		glob->mail.count = 0;
	}
}

static void	LskRecvMail(t_HWidget sk, void *data)
{
	email_glob_t *glob = app_get_data();
	t_HWindow handle = (t_HWindow)data;
	if(g_pop3_state == POP3_NONE){
		int ret;
		if(!sys_nw_gprs_started(SIM_ANY)){
			msgbox_show(NULL, _LSK(OK), INFINITE, MBB_OK, "GPRS not open");
			return;
		}
		ret = pop3_connect(glob->user->pop3_server);
		if(ret != SUCCESS)
			msgbox_show(NULL, _LSK(OK), INFINITE, MBB_OK, "pop3 connect ERROR");
		else{
			sk_set_text(sk, SK_LSK, NULL, RESID_STR_CANCEL);
			wgt_redraw(sk, FALSE);
		}
	}else{
		pop3_close();
		email_head_free(&glob->mail.head);
		__uidl_list_free(glob);
		sk_set_text(sk, SK_LSK, NULL, RESID_STR_RECVMAIL);
		wgt_redraw(sk, FALSE);
	}

}

static void	__OnRecvDone(t_HWindow handle, gboolean send_quit)
{
	email_glob_t *glob = app_get_data();
	t_HWidget widget;

	if(send_quit)
		pop3_quit();

	email_head_free(&glob->mail.head);
	__uidl_list_free(glob);

	widget = wnd_get_skbar(handle);
	sk_set_text(widget, SK_LSK, NULL, RESID_STR_RECVMAIL);
	wgt_redraw(widget, FALSE);

	widget = wnd_get_widget(handle, RESID_LABEL_INFO);
	if(listctrl_get_count(handle))
		label_set_text(widget, "%d/%d", listctrl_get_cursor(handle)+1, listctrl_get_count(handle));
	else
		label_set_text(widget, "0/0");
	wgt_redraw(widget, FALSE);
}

static error_t	__email_list_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	email_glob_t *glob = app_get_data();
	t_HWindow win = userdata;
	t_EmailMainWData *wdata = (t_EmailMainWData *)wgt_get_tag(win);
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnSelChanged:
			{
				t_HWidget widget = wnd_get_widget(win, RESID_LABEL_INFO);
				label_set_text(widget, "%d/%d", lparam+1, listctrl_get_count(handle));
				wgt_redraw(widget, FALSE);
			}
			break;
		case WIDGET_OnKeyEvent:
			switch(lparam){
				case MVK_OK:
					if(sparam == MMI_KEY_LONG_PRESS && g_ds_GetRecordNum(glob->hds_head)){
						if(msgbox_show(NULL, MBB_YES_NO, 0, 0, brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_DEL, "")) == MBB_YES){
							t_ContactID sid;

							g_ds_Delete(glob->hds_head);

							wgt_redraw(handle, FALSE);
						}
						ret = SUCCESS;
					}
					break;
			}
			break;
	}
	return ret;
}
extern int	__get_head_type(char *str);

//2 返回已经解析的字节数，
int pop3_retr_parse_header(t_RecvMail*mail, char *buf, int len)
{
	char *crlf;
	int	linelen, plen;
	char	*start, *temp;
	char *line = NULL;
	int line_max = 0;
	
	plen = 0;
	while(len > 0){
		
		if(buf[0] == '\n' || buf[0] == '\r' && buf[1] == '\n'){
			mail->parse_step = H_BODY;
			plen += (buf[0] == '\n')?1:2;
			break;
		}
		crlf = strnchr(buf, len, '\n');
		if(crlf == NULL){
			//2 没找到行尾结束符，表示数据未接收完全
			break;
		}
		linelen = crlf - buf + 1;
		plen += linelen;

		if(linelen > line_max){
			FREEIF(line);
			line_max = linelen;
			line = MALLOC(line_max+1);
		}
		strncpy(line, buf, linelen);
		line[linelen] = 0;
		
		buf += linelen;
		len -= linelen;
		
		if(isspace(line[0])){
			start = g_str_trim(line);
			switch(mail->parse_step){
				case H_TO:
					temp = (char *)rfc2047_decode_simple((const char *)start);
					if(temp && temp[0] != 0)
						mail->head.to = g_str_append(mail->head.to, temp);
					FREEIF(temp);
					break;
				case H_CC:
					temp = (char *)rfc2047_decode_simple((const char *)start);
					if(temp && temp[0] != 0)
						mail->head.cc = g_str_append(mail->head.cc, temp);
					FREEIF(temp);
					break;
				case H_BCC:
					temp = (char *)rfc2047_decode_simple((const char *)start);
					if(temp && temp[0] != 0)
						mail->head.bcc = g_str_append(mail->head.bcc, temp);
					FREEIF(temp);
					break;
				case H_SUBJECT:
					temp = (char *)rfc2047_decode_simple((const char *)start);
					if(temp && temp[0] != 0)
						mail->head.subject = g_str_append(mail->head.subject, temp);
					FREEIF(temp);
					break;
				case H_CTYPE:
					break;
			}
			continue;
		}
		mail->parse_step = __get_head_type(line);
		if(mail->parse_step == H_NONE){
			//2 暂时不支持的字段
			continue;
		}
		start = strchr(line, ':');
		start = g_str_trim(start+1);
		switch(mail->parse_step){
			case H_FROM:
				temp = (char *)rfc2047_decode_simple((const char *)start);
				mail->head.from = temp;
				break;
			case H_SUBJECT:
				temp = (char *)rfc2047_decode_simple((const char *)start);
				mail->head.subject = temp;
				break;
			case H_DATE:
				{
					datetime_t dt;
					rfc822_str2time(start, &dt);
					mail->head.time = datetime_to_dostime(&dt);
				}
				break;
			case H_TO:
				temp = (char *)rfc2047_decode_simple((const char *)start);
				if(temp && temp[0] != 0)
					mail->head.to = g_str_append(mail->head.to, temp);
				FREEIF(temp);
				break;
			case H_CC:
				temp = (char *)rfc2047_decode_simple((const char *)start);
				if(temp && temp[0] != 0)
					mail->head.cc = g_str_append(mail->head.cc, temp);
				FREEIF(temp);
				break;
			case H_BCC:
				temp = (char *)rfc2047_decode_simple((const char *)start);
				if(temp && temp[0] != 0)
					mail->head.bcc = g_str_append(mail->head.bcc, temp);
				FREEIF(temp);
				break;
			case H_CTYPE:
				temp = strchr(start, ';');
				if(temp)
					*temp = 0;
				//将来试图用这个来判断是否有附件,如果有附件，这个值应该是multipart/mixed
				mail->head.ctype = g_str_dup(start);
				break;
			case H_PRIORITY:
				mail->head.priority = atoi(start);
				if(mail->head.priority == 0)
					mail->head.priority = 3;
				break;
			case H_RETPATH:
				if(mail->head.reply_to == NULL)
					mail->head.reply_to = g_str_dup(start);
				break;
			case H_REPLYTO:
				FREEIF(mail->head.reply_to);
				mail->head.reply_to = g_str_dup(start);
				break;
		}
	}
	FREEIF(line);
	return plen;
}
static void pop3_callback(t_HWindow handle, gu8 evt, gu8 cmd, char *msg, int len)
{
	email_glob_t *glob = app_get_data();
	t_HWidget label = wnd_get_widget(handle, RESID_LABEL_INFO);
	switch(evt){
		case POP3_REPORT:
			g_printf(msg);
			//msgbox_show(NULL, 0, 1000, 0, msg);
			break;
		case POP3_CONNECT_OK:
			label_set_text(label, "Connect to <%s> SUCCESS", glob->user->pop3_server);
			wgt_redraw(label, FALSE);
			break;
		case POP3_CONNECT_FAILED:
			label_set_text(label, "Connect to <%s> FAIED", glob->user->pop3_server);
			wgt_redraw(label, FALSE);
			break;
		case POP3_CLOSED:
			label_set_text(label, "Socket Closed");
			wgt_redraw(label, FALSE);
			amos_sleep(500);
			__OnRecvDone(handle, FALSE);
			break;
		case POP3_CMD_NCK:
			switch(cmd){
				case POP3_NOOP:
				case POP3_USER:
				case POP3_PASS:
				case POP3_STAT:
				case POP3_RSET:
					__OnRecvDone(handle, TRUE);
					break;
			}
			break;
		case POP3_CMD_ACK:
			if(cmd == POP3_QUIT){
			}else if(cmd == POP3_CONNECT){
				label_set_text(label, "USER %s", glob->user->pop3_user);
				wgt_redraw(label, FALSE);
				pop3_user(glob->user->pop3_user);
			}else if(cmd == POP3_USER){
				label_set_text(label, "PASS authentication");
				wgt_redraw(label, FALSE);
				pop3_pass(glob->user->pop3_pass);
			}else if(cmd == POP3_PASS){
				label_set_text(label, "PASS authentication: SUCCESS");
				wgt_redraw(label, FALSE);
				amos_sleep(50);
				label_set_text(label, "STAT");
				wgt_redraw(label, FALSE);
				
				pop3_stat();
			}else if(cmd == POP3_STAT){
				//2 解析STAT命令的ack
				pop3_stat_parse(msg, len, &glob->mail.count);
				if(glob->mail.count == 0){
					label_set_text(label, "No Mail");
					wgt_redraw(label, FALSE);
					//msgbox_show(NULL, 0, 1000, 0, "No Mail");
					__OnRecvDone(handle, TRUE);
				}else{
					label_set_text(label, "Checking message list...");
					wgt_redraw(label, FALSE);

					pop3_uidl();
				}
			}else if(cmd == POP3_UIDL){
				//2 解析UIDL命令的ack
				glob->mail.uidl_list = MALLOC(glob->mail.count*sizeof(t_EmailUidl));
				memset(glob->mail.uidl_list, 0, glob->mail.count*sizeof(t_EmailUidl));
 				glob->mail.buflen = 0; //2 因为要借用buf[]来暂存未解析的字符
 				glob->mail.parse_index = 0;
			}else if(cmd == POP3_LIST){
				//2 解析LIST命令的ack
				pop3_list_parse(msg, len, &glob->mail.head.mailsize);
				label_set_text(label, "Receiving %d/%d: 0/%d", glob->mail.parse_index+1, glob->mail.count, glob->mail.head.mailsize);
				wgt_redraw(label, FALSE);
				pop3_retr(glob->mail.uidl_list[glob->mail.parse_index].index);
			}else if(cmd == POP3_RETR){
				//2 解析RETR命令的ack
				glob->mail.rxsize = 0;
				glob->mail.buflen = 0;
				glob->mail.block = 0;
				glob->mail.mailid = 0;
				glob->mail.parse_step = H_NONE;
				glob->mail.parsed_head_len = 0;
			}
			break;
		case POP3_CMD_ACK_BODY:
			if(cmd == POP3_UIDL){
				//2 按行解析uidl列表，buf[]中暂存未解析的字符
				while(len > 0){
					char * p;
					int n, i;
					gboolean found;
					p = strnchr(msg, len, '\n');
					if(p == NULL){
						//2 没找到行尾结束符，放到buf[]中暂存
						memcpy(&glob->mail.buf[glob->mail.buflen], msg, len);
						glob->mail.buflen += len;
						break;
					}
					//2 加载一行数据到buf[] 中缓存
					n = p - msg + 1;
					memcpy(&glob->mail.buf[glob->mail.buflen], msg, n);
					msg += n;
					len -= n;

					n += glob->mail.buflen;
					glob->mail.buf[n] = 0;
					glob->mail.buflen = 0;

					p = glob->mail.buf;
					if(p[0] == '.' && (p[1] == '\r' || p[1] == '\n')){
						//2 uidl列表结束，需要跟本地uidl列表做比较
						g_ds_First(glob->hds_uidl);
						while(!g_ds_Eof(glob->hds_uidl)){
							found = FALSE;
							p = g_ds_GetString(glob->hds_uidl, UIDL_DBF_UIDL);
							n = g_ds_GetValue(glob->hds_uidl, UIDL_DBF_MAIL);
							if(n){
								//2 邮件没接收完，删除已接收部分
								g_ds_CondSet1(glob->hds_cont, CONTENT_DBF_MAILID, DBREL_EQ, n);
								g_ds_Query(glob->hds_cont, TRUE);
								g_ds_First(glob->hds_cont);
								while(!g_ds_Eof(glob->hds_cont))
									g_ds_Delete(glob->hds_cont);
								
								if(g_ds_Goto(glob->hds_head, n) == SUCCESS)
									g_ds_Delete(glob->hds_head);
								
								g_ds_SetValue(glob->hds_uidl, UIDL_DBF_MAIL, 0);
								g_ds_Update(glob->hds_uidl);
							}
							for(n = 0; n < glob->mail.count; n++){
								if(g_stricmp(p, glob->mail.uidl_list[n].uidl) == 0){
									//2 找到相同uidl，表示该邮件已经收取下来了
									int i;
									FREEIF(glob->mail.uidl_list[n].uidl);
									for(i = n+1; i < glob->mail.count; i++){
										glob->mail.uidl_list[i-1].index = glob->mail.uidl_list[i].index;
										glob->mail.uidl_list[i-1].uidl = glob->mail.uidl_list[i].uidl;
									}
									glob->mail.count--;
									found = TRUE;
									break;
								}
							}
							if(found){
								g_ds_Next(glob->hds_uidl);
							}else{
								//2 邮件服务器上未找到该uidl，表示已经删除，本地无须再存
								g_ds_Delete(glob->hds_uidl);
							}
						}

						//2 然后开始接收邮件
						if(glob->mail.count == 0){
							label_set_text(label, "No Mail");
							wgt_redraw(label, FALSE);
							//msgbox_show(NULL, 0, 1000, 0, "No Mail");
							__OnRecvDone(handle, TRUE);
						}else{
							//2 从第一封开始收取
							label_set_text(label, "Message count = %d", glob->mail.count);
							wgt_redraw(label, FALSE);
						
							memset(&glob->mail.head, 0, sizeof(t_EmailHeader));
							glob->mail.parse_index = 0;
							label_set_text(label, "Receiving %d/%d", glob->mail.parse_index+1, glob->mail.count);
							wgt_redraw(label, FALSE);
							pop3_list(glob->mail.uidl_list[glob->mail.parse_index].index);
						}
					}else{
						//2 解析出index和uidl
						pop3_uidl_parse(p, n, &glob->mail.uidl_list[glob->mail.parse_index].index, &glob->mail.uidl_list[glob->mail.parse_index].uidl);
						glob->mail.parse_index++;
					}
					
				}
			}else
			if(cmd == POP3_RETR){
				int n = EMAIL_BLOCK_SIZE-glob->mail.buflen;
				gboolean save = FALSE;
				glob->mail.rxsize += len;
				
				label_set_text(label, "Receiving %d/%d: %d/%d", glob->mail.parse_index, glob->mail.count, glob->mail.rxsize, glob->mail.head.mailsize);
				wgt_redraw(label, FALSE);

				if(n >= len)
					n = len;
				memcpy(&glob->mail.buf[glob->mail.buflen], msg, n);
				msg += n;
				len -= n;
				glob->mail.buflen += n;
				if(glob->mail.parse_step != H_BODY){
					n = pop3_retr_parse_header(&glob->mail, &glob->mail.buf[glob->mail.parsed_head_len], glob->mail.buflen-glob->mail.parsed_head_len);
					glob->mail.parsed_head_len += n;
					if(glob->mail.parse_step == H_BODY){
						//2 邮件头解析完毕
						glob->mail.mailid = db_email_head_new(glob->hds_head, &glob->mail.head);
						if(glob->mail.mailid != DB_RID_NULL){
							g_ds_AddNew(glob->hds_uidl);
							g_ds_SetString(glob->hds_uidl, UIDL_DBF_UIDL, glob->mail.uidl_list[glob->mail.parse_index].uidl);
							g_ds_SetValue(glob->hds_uidl, UIDL_DBF_MAIL, glob->mail.mailid);
							g_ds_Update(glob->hds_uidl);
						}else{
							//2 空间满，无法接收邮件
							label_set_text(label, "Disk Full, only (%d) messages received", glob->mail.parse_index);
							wgt_redraw(label, FALSE);
							__OnRecvDone(handle, TRUE);
							return;
						}
					}
				}
				if(glob->mail.rxsize >= glob->mail.head.mailsize){
					//2 接收完毕，把最后的结束符"\r\n.\r\n"去掉
					glob->mail.buflen -= 5;
					save = TRUE;

					g_ds_SetValue(glob->hds_uidl, UIDL_DBF_MAIL, 0);
					g_ds_Update(glob->hds_uidl);
					
					label_set_text(label, "Receiving %d/%d SUCCESS, mailsize=%d", glob->mail.parse_index+1, glob->mail.count, glob->mail.head.mailsize);
					wgt_redraw(label, FALSE);
					email_head_free(&glob->mail.head);
					//2 开始收取下一封email
					if(glob->mail.parse_index+1 < glob->mail.count){
						glob->mail.parse_index++;
						label_set_text(label, "Receiving %d/%d", glob->mail.parse_index, glob->mail.count);
						wgt_redraw(label, FALSE);
						pop3_list(glob->mail.uidl_list[glob->mail.parse_index].index);
					}else{
						//3 全部邮件接收完毕
						label_set_text(label, "All <%d> messages received", glob->mail.count);
						wgt_redraw(label, FALSE);
						__OnRecvDone(handle, TRUE);
					}
				}else if(glob->mail.buflen == EMAIL_BLOCK_SIZE){
					save = TRUE;
				}
				if(save){
					glob->mail.block++;
					g_ds_AddNew(glob->hds_cont);
					g_ds_SetValue(glob->hds_cont, CONTENT_DBF_MAILID, glob->mail.mailid);
					g_ds_SetValue(glob->hds_cont, CONTENT_DBF_BLOCK, glob->mail.block);
					g_ds_SetBinary(glob->hds_cont, CONTENT_DBF_CONTENT, glob->mail.buf, glob->mail.buflen);
					g_ds_Update(glob->hds_cont);
					glob->mail.buflen = 0;
				}
				if(len){
					memcpy(glob->mail.buf, msg, len);
					glob->mail.buflen = len;
				}
			}
			break;
	}
}


#define	LABEL_INFO_HEIGHT		30

error_t	email_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	email_glob_t *glob = app_get_data();
	t_EmailMainWData *wdata = (t_EmailMainWData*)wgt_get_tag(handle);
	
	switch(evt){
		case WINDOW_OnOpen:
			{
				t_HWidget widget;
				gui_rect_t client;
				
				wgt_enable_attr(handle, WND_ATTR_DEFAULT);
				//wgt_set_bg_fill_solid((t_HWidget)handle, ui_theme_get()->bg_color);

				wgt_get_client_rect((t_HWidget)handle, &client);
				
				wdata = MALLOC(sizeof(t_EmailMainWData));
				memset(wdata, 0, sizeof(t_EmailMainWData));
				wgt_set_tag(handle, wdata);

				pop3_set_callback(pop3_callback, handle);

				
				widget = wnd_get_widget(handle, RESID_LIST_MAIN);
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiTop|guiBottom);
				wgt_set_bound(widget, 0, 0, client.dx, client.dy-LABEL_INFO_HEIGHT);
				g_ds_register_notify(glob->hds_head, __email_notify, handle);
				g_ds_bind(glob->hds_head, db_email_redraw_record, db_email_view_record);
				listctrl_attach_db(widget, glob->hds_head);
				wgt_hook_register(widget, WIDGET_OnKeyEvent|WIDGET_OnSelChanged, __email_list_hook, handle);
#if defined(FTR_IPHONE_STYLE)
				listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
				listctrl_set_rows(widget, 0, 40, 0);
				listctrl_set_cols(widget, 1, 0);
#else
				listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
				listctrl_set_rows(widget, 0, 40, 0);
				listctrl_set_cols(widget, 1, 0);
#endif
				listctrl_set_flags(widget, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM|CLFLAG_ENABLE_ITEM_FSHOW);

				
				widget = wnd_get_widget(handle, RESID_LABEL_INFO);
				wgt_set_bound(widget, 0, client.dy-LABEL_INFO_HEIGHT, client.dx, LABEL_INFO_HEIGHT);
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiTop|guiBottom);
				{
					t_HWidget sk = wnd_get_skbar(handle);
					//t_HWidget menu;
				
					//menu = wnd_get_widget(handle, RESID_MENU_OPTION);
					//menu_bind(menu, miContact, widget);
					//menu_SetItemPopup(menu, RESID_STR_OPERATION, IDENT_BY_ID, contact_ms_listctrl_new(glob, handle));
					//g_object_unref(widget);
					if(g_object_valid(sk)){
						sk_set_text(sk, SK_LSK, NULL, RESID_STR_RECVMAIL);
						sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
						sk_set_callback(sk, SK_LSK, LskRecvMail, handle);
						//widget = sms_ms_listctrl_new(glob, handle);
						//sk_set_popup(sk, SK_LSK, widget);
						//g_object_unref(widget);
					}
				}
				ret = SUCCESS;
			}
			break;
		case WINDOW_OnClose:
			if(wdata){
				g_object_unref_0(wdata->hds);
				FREE(wdata);
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			listctrl_set_cursor(wnd_get_widget(handle, RESID_LIST_MAIN), glob->view_index, FALSE);
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			ret = SUCCESS;
			break;
	}
	return ret;
}

