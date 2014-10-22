#include "pack_NES.hi"

enum {
	RESID_NESCTRL = RESID_CTRL_DYNAMIC,
};

static void __sk_callback(t_HWidget sk, void *data)
{
	t_HWindow handle = wgt_parent_window(sk);
	int id = (int)data;
	t_HWidget widget = wnd_get_widget(handle, RESID_NESCTRL);
	int state = nesctrl_state(widget);

	if(g_object_valid(handle)){
		if(id == SK_RSK){
			if(state == NES_STATE_IDLE){
				if(msgbox_show(NULL, _LSK(YES)|_RSK(NO), 0, 0, brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_BACK, "")) == MBB_YES){
					wnd_send_close(handle, 0);
				}
			}else{
				nesctrl_stop(widget);
				sk_set_text(sk, SK_LSK, "Open", RESID_STR_OPEN);
				sk_set_text(sk, SK_RSK, "Exit", RESID_STR_BACK);
				wgt_redraw(sk, FALSE);
			}
		}else if(id == SK_LSK){
			if(state == NES_STATE_IDLE){
				char fname[AMOS_PATH_MAX+1];
				if(FileDlg_Show(fname, sizeof(fname), "x:/nes", "*.nes", OFD_FILE_OPEN)){
					if(nesctrl_run(widget, fname, TRUE)){
						sk_set_text(sk, SK_LSK, "Pause", RESID_STR_PAUSE);
						sk_set_text(sk, SK_RSK, "Close", RESID_STR_CLOSE);
						wgt_redraw(sk, FALSE);
					}else{
						msgbox_show(NULL, _CSK(OK), 0, 0, brd_get_string(BRD_HANDLE_SELF, RESID_STR_FILE_FORMAT, "format error"));
					}
				}
			}else if(state == NES_STATE_RUNNING){
				nesctrl_pause(widget);
				sk_set_text(sk, SK_LSK, "Resume", RESID_STR_RESUME);
				wgt_redraw(sk, FALSE);
			}else if(state == NES_STATE_PAUSE){
				nesctrl_resume(widget);
				sk_set_text(sk, SK_LSK, "Pause", RESID_STR_PAUSE);
				wgt_redraw(sk, FALSE);
			}
		}
	}
}

error_t	nes_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_HWidget widget;
	
	switch(evt){
		case WINDOW_OnOpen:
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			wgt_set_bg_fill_solid(handle, 0); //2 ȫ͸
			{
				gui_rect_t client;
				t_SKHandle sk = wnd_get_skbar(handle);

				wgt_get_client_rect((t_HWidget)handle, &client);
				
				//2 NesCtrl
				widget = wgt_create((t_HWidget)handle, "nesctrl");
				wgt_set_resid(widget, RESID_NESCTRL);
				wgt_set_bound(widget, 0, 0, client.dx, client.dy);

				if(lparam){
					nesctrl_run(widget, (const char *)lparam, FALSE);
					sk_set_text(sk, SK_LSK, "Pause", RESID_STR_PAUSE);
					sk_set_text(sk, SK_RSK, "Close", RESID_STR_CLOSE);
				}else{
					sk_set_text(sk, SK_LSK, "Open", RESID_STR_OPEN);
					sk_set_text(sk, SK_RSK, "Exit", RESID_STR_BACK);
				}
				sk_set_callback(sk, SK_LSK, (t_WidgetCallback)__sk_callback, (void*)SK_LSK);
				sk_set_callback(sk, SK_RSK, (t_WidgetCallback)__sk_callback, (void*)SK_RSK);
			}
			
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			switch(lparam){
				case MVK_OK:
					ret = SUCCESS;
					break;
			}
			break;
		case WIDGET_OnSetFocus:
			break;
		case WIDGET_OnKillFocus:
			break;
		case WINDOW_OnDraw:
			ret = SUCCESS;
			break;
			
	}
	return ret;
}

