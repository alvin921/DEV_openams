#include "pack_email.hi"

static void __sk_callback(t_HWidget sk, void *data)
{
	t_HWindow handle = wgt_parent_window(sk);
	int id = (int)data;
	if(g_object_valid(handle)){
		if(id == SK_RSK){
			if(msgbox_show(NULL, MBB_YES_NO, 0, 0, brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_SAVE, NULL)) == MBB_YES){
				id = SK_LSK;
			}
		}
		if(id == SK_LSK){
			t_EmailUser *puser = email_userinfo_load();
			t_HWidget widget;
			
			//2 check name
			widget = wnd_get_widget(handle, RESID_EDITOR_EMAIL);
			if(editor_get_nchars(widget) <= 0){
				msgbox_show(NULL, _CSK(OK), 0, 0, "%s %s", brd_get_string(BRD_HANDLE_SELF, RESID_STR_EMAIL, NULL), brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_NEEDED, NULL));
				return;
			}
			//2 check pop3 host
			widget = wnd_get_widget(handle, RESID_EDITOR_POP3_HOST);
			if(editor_get_nchars(widget) <= 0){
				msgbox_show(NULL, _CSK(OK), 0, 0, "%s %s", brd_get_string(BRD_HANDLE_SELF, RESID_STR_POP3_HOST, NULL), brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_NEEDED, NULL));
				return;
			}
			//2 check pop3 user
			widget = wnd_get_widget(handle, RESID_EDITOR_POP3_USER);
			if(editor_get_nchars(widget) <= 0){
				msgbox_show(NULL, _CSK(OK), 0, 0, "%s %s", brd_get_string(BRD_HANDLE_SELF, RESID_STR_USER, NULL), brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_NEEDED, NULL));
				return;
			}
			//2 check pop3 passwd
			widget = wnd_get_widget(handle, RESID_EDITOR_POP3_USER);
			if(editor_get_nchars(widget) <= 0){
				msgbox_show(NULL, _CSK(OK), 0, 0, "%s %s", brd_get_string(BRD_HANDLE_SELF, RESID_STR_PASSWD, NULL), brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_NEEDED, NULL));
				return;
			}
			editor_get_text(wnd_get_widget(handle, RESID_EDITOR_NAME), puser->display_name, DISPLAY_NAME_MAX+1);
			editor_get_text(wnd_get_widget(handle, RESID_EDITOR_EMAIL), puser->email, EMAIL_LENGTH_MAX+1);
			editor_get_text(wnd_get_widget(handle, RESID_EDITOR_POP3_HOST), puser->pop3_server, HOST_NAME_MAX+1);
			editor_get_text(wnd_get_widget(handle, RESID_EDITOR_POP3_USER), puser->pop3_user, HOST_USER_MAX+1);
			editor_get_text(wnd_get_widget(handle, RESID_EDITOR_POP3_PASS), puser->pop3_pass, HOST_PASS_MAX+1);
			editor_get_text(wnd_get_widget(handle, RESID_EDITOR_SMTP_HOST), puser->smtp_server, HOST_NAME_MAX+1);
			editor_get_text(wnd_get_widget(handle, RESID_EDITOR_SMTP_USER), puser->smtp_user, HOST_USER_MAX+1);
			editor_get_text(wnd_get_widget(handle, RESID_EDITOR_SMTP_PASS), puser->smtp_pass, HOST_PASS_MAX+1);
			email_userinfo_store();

			//2  如果第一次，则自动加载主界面
			if(wgt_get_tag(handle))
				wnd_show(wnd_create_by_resid(email_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN));
		}
		wnd_send_close(handle, 0);
	}
}

//2 lparam==1, 表示无用户信息，启动第一次加载
error_t	email_UserProfileWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	email_glob_t *glob = app_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
			{
				t_HWidget widget;
				t_EmailUser *puser = glob->user;

				wgt_set_tag(handle, lparam);
				
				wgt_enable_attr(handle, WND_ATTR_DEFAULT|CTRL_ATTR_VSCROLLABLE);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_NAME);
				wgt_set_border(widget, BORDER_TYPE_LINE, 0, guiTop);
				editor_set_type(widget, EDITOR_TYPE_TEXT, FALSE);
				editor_set_nbytes_max(widget, DISPLAY_NAME_MAX);
				editor_set_text(widget, puser->display_name);

				widget = wnd_get_widget(handle, RESID_EDITOR_EMAIL);
				editor_set_type(widget, EDITOR_TYPE_TEXT, TRUE);
				editor_set_nbytes_max(widget, EMAIL_LENGTH_MAX);
				editor_set_text(widget, puser->email);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_POP3_HOST);
				editor_set_type(widget, EDITOR_TYPE_TEXT, TRUE);
				editor_set_nbytes_max(widget, HOST_NAME_MAX);
				editor_set_text(widget, puser->pop3_server);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_POP3_USER);
				editor_set_type(widget, EDITOR_TYPE_TEXT, TRUE);
				editor_set_nbytes_max(widget, HOST_USER_MAX);
				editor_set_text(widget, puser->pop3_user);

				widget = wnd_get_widget(handle, RESID_EDITOR_POP3_PASS);
				editor_set_type(widget, EDITOR_TYPE_TEXT, TRUE);
				editor_set_nbytes_max(widget, HOST_PASS_MAX);
				editor_set_text(widget, puser->pop3_pass);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_SMTP_HOST);
				editor_set_type(widget, EDITOR_TYPE_TEXT, TRUE);
				editor_set_nbytes_max(widget, HOST_NAME_MAX);
				editor_set_text(widget, puser->smtp_server);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_SMTP_USER);
				editor_set_type(widget, EDITOR_TYPE_TEXT, TRUE);
				editor_set_nbytes_max(widget, HOST_USER_MAX);
				editor_set_text(widget, puser->smtp_user);

				widget = wnd_get_widget(handle, RESID_EDITOR_SMTP_PASS);
				editor_set_type(widget, EDITOR_TYPE_TEXT, TRUE);
				editor_set_nbytes_max(widget, HOST_PASS_MAX);
				editor_set_text(widget, puser->smtp_pass);

#if defined(FTR_SOFT_KEY)
				{
					t_SKHandle sk = wnd_get_skbar(handle);
					if(g_object_valid(sk)){
						sk_set_text(sk, SK_LSK, NULL, RESID_STR_SAVE);
						sk_set_callback(sk, SK_LSK, (t_WidgetCallback)__sk_callback, (void *)SK_LSK);
						sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
						sk_set_callback(sk, SK_RSK, (t_WidgetCallback)__sk_callback, (void *)SK_RSK);
					}
				}
#endif
			}
			
			
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			break;
		case WINDOW_OnClose:
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			ret = SUCCESS;
			break;
	}
	return ret;
}

