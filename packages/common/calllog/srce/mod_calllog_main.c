#include "pack_calllog.hi"

#if defined(FTR_COMM_SESSION_SUPPORT)

//2 sparam = BY_SID; lparam = <cid>
error_t	calllog_ViewSessionWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_HWidget view = (t_HWidget)wgt_get_tag(handle);
	switch(evt){
		case WINDOW_OnOpen:
		{
			t_HWidget widget;
			gui_rect_t client;

			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			widget = wgt_create((t_HWidget)handle, "calllogview");
			wgt_get_client_rect((t_HWidget)handle, &client);
			wgt_set_bound(widget, 0, 0, client.dx, client.dy);
			wgt_set_border(widget, BORDER_TYPE_NONE, 0, 0);
			
			wgt_calllog_attach_contact(widget, lparam);
			wgt_set_tag(handle, widget);
			
#if defined(FTR_SOFT_KEY)
			widget = wnd_get_skbar(handle);
			if(g_object_valid(widget)){
#if !defined(FTR_NO_INPUT_KEY)
				sk_set_text(widget, SK_LSK, NULL, RESID_STR_MORE);
#endif
				sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
			}
#endif
			ret = SUCCESS;
		}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			//gdi_draw_text_at(wgt_gdi(handle), "Hello World!", -1, 30, 30, RGB_WHITE, RGB_BLACK, TRUE);
			ret = SUCCESS;
			break;
#if 0 //2 calllogview控件已经处理了
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_UP){
				switch(lparam){
					case MVK_LSK:
						wgt_calllog_contact_more_op(view);
						ret = SUCCESS;
						break;
					case MVK_SEND:
						wgt_calllog_contact_dial(view);
						ret = SUCCESS;
						break;
				}
			}
			break;
#endif
	}
	return ret;
}
#else

static const resid_t resid_call_types[] = {
	RESID_STR_OUTGOING_CALL,
	RESID_STR_INCOMING_CALL,
	RESID_STR_MISSED_CALL,
	RESID_STR_REJECTED_CALL
};

static void		__show_detail(calllog_glob_t *glob, t_HWindow handle, gboolean redraw)
{
	t_HWidget widget;
	gu32 dt, d, t;

	g_ds_GotoByIndex(glob->hdb, glob->cursor);

	widget = wnd_get_widget(handle, RESID_LABEL_NUMBER);
	wgt_set_caption_by_resid(widget, BRD_HANDLE_SELF, resid_call_types[g_ds_GetValue(glob->hdb, CALL_DBFIELD_TYPE)]);
	label_set_text(widget, g_ds_GetString(glob->hdb, CALL_DBFIELD_NUMBER));
	
	dt = g_ds_GetValue(glob->hdb, CALL_DBFIELD_TIME);
	d = dt>>16;
	t = dt&0xFFFF;
	label_set_text(wnd_get_widget(handle, RESID_LABEL_TIME), "%04d-%02d-%02d %02d:%02d", DOSDATE_Y(d), DOSDATE_M(d), DOSDATE_D(d), DOSTIME_H(t), DOSTIME_M(t), DOSTIME_S(t));
	
	dt = g_ds_GetValue(glob->hdb, CALL_DBFIELD_DURATION);
	if(dt >= 3600)
		label_set_text(wnd_get_widget(handle, RESID_LABEL_DURATION), "%02d:%02d:%02d", dt/3600, (dt%3600)/60, dt%60);
	else
		label_set_text(wnd_get_widget(handle, RESID_LABEL_DURATION), "%02d:%02d", dt/60, dt%60);

	if(redraw){
		wgt_client_origin_x(handle) = 0;
		wgt_client_origin_y(handle) = 0;
		wnd_redraw(handle, NULL);
	}
}

error_t	calllog_ViewWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	calllog_glob_t *glob = task_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			__show_detail(glob, handle, FALSE);
			{
#if !defined(FTR_NO_INPUT_KEY) && defined(FTR_SOFT_KEY)
				t_HWidget sk = wnd_get_skbar(handle);
				t_HList list;
				
				if(g_object_valid(sk)){
					sk_set_text(sk, SK_LSK, NULL, RESID_STR_OPTION);
					sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
					list = calllog_ms_listctrl_new(glob, handle);
					sk_set_popup(sk, SK_LSK, list);
					g_object_unref(list);
				}
#endif
				ret = SUCCESS;
			}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			//gdi_draw_text_at(wgt_gdi(handle), "Hello World!", -1, 30, 30, RGB_WHITE, RGB_BLACK, TRUE);
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_PRESS && lparam == MVK_SEND)
				;//pack_call("call", MOD_ID_MAIN, (gu8*)glob->items[glob->cursor].CellNo, strlen(glob->items[glob->cursor].CellNo)+1, TRUE, NULL);
			else if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
				switch(lparam){
					case MVK_LEFT:
						if(!g_ds_Bof(glob->hdb)){
							glob->cursor--;
							g_ds_Prev(glob->hdb);
							__show_detail(glob, handle, TRUE);
						}
						ret = SUCCESS;
						break;
					case MVK_RIGHT:
						if(!g_ds_Eof(glob->hdb)){
							glob->cursor++;
							g_ds_Next(glob->hdb);
							__show_detail(glob, handle, TRUE);
						}
						ret = SUCCESS;
						break;
				}
			}
	}
	return ret;
}

#endif

error_t	calllog_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	
	switch(evt){
		case WINDOW_OnOpen:
		{
			t_HWidget widget;
			gui_rect_t client;

#if !defined(FTR_NO_INPUT_KEY) && defined(FTR_SOFT_KEY)
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
#else
			wgt_enable_attr(handle, WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_STATUS|WND_ATTR_HAS_TITLE);
#endif
			widget = wgt_create((t_HWidget)handle, "calllog");
			wgt_get_client_rect((t_HWidget)handle, &client);
			wgt_set_bound(widget, 0, 0, client.dx, client.dy);
			wgt_set_border(widget, BORDER_TYPE_NONE, 0, 0);

			wgt_calllog_attach(widget, sparam, lparam);

#if !defined(FTR_NO_INPUT_KEY) && defined(FTR_SOFT_KEY)
			widget = wnd_get_skbar(handle);
			if(g_object_valid(widget)){
				sk_set_text(widget, SK_LSK, NULL, RESID_STR_CALL);
				sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
			}
#endif
			ret = SUCCESS;
		}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			//listctrl_set_cursor(wnd_get_widget(handle, RESID_LIST), glob->cursor, FALSE);
			break;
		case WIDGET_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			//gdi_draw_text_at(wgt_gdi(handle), "Hello World!", -1, 30, 30, RGB_WHITE, RGB_BLACK, TRUE);
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_PRESS && lparam == MVK_SEND)
				;//pack_call("call", MOD_ID_MAIN, (gu8*)glob->items[glob->cursor].CellNo, strlen(glob->items[glob->cursor].CellNo)+1, TRUE, NULL);
	}
	return ret;
}


