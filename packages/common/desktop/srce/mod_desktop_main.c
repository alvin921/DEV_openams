#include "pack_desktop.hi"
#include "wgt_listctrl.h"
#include "wgt_softkey.h"

static error_t	__on_win_open(t_DesktopGlobData *glob, t_HWindow handle)
{
	t_HImage image;
	t_HList gallery;
	t_HDataSource ds;

	wgt_enable_attr(handle, WND_ATTR_HAS_STATUS|WND_ATTR_HAS_SK|WND_ATTR_IS_DESKTOP);
	//wgt_disable_caption(handle);

	gallery = wnd_get_widget(handle, RESID_LIST_SHORTCUT);
	if(g_object_valid(gallery)){
		ds = DSAppStore_New(NULL, NULL);
		AppStore_AddByMenuTree(DSAppStore_GetRootNode(ds), prod_shortcut_open());

		listctrl_set_viewtype(gallery, GUI_VIEWTYPE_ICON_ONLY);
		listctrl_set_flags((t_HList)gallery, CLFLAG_SCROLLING_HORIZONTAL|CLFLAG_SCROLLING_PAGE|CLFLAG_AUTOTAB_UD|CLFLAG_CURSOR_NODRAW_ONKILLFOCUS|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE);
#if (LCD_WIDTH == 240) && (LCD_HEIGHT == 320)
		DSAppStore_SetIconSize(ds, 28, 28);
		listctrl_set_rows(gallery, 0, 36, 0);
		listctrl_set_cols(gallery, 0, 36);
#elif (LCD_WIDTH == 320) && (LCD_HEIGHT == 240)
		DSAppStore_SetIconSize(ds, 28, 28);
		listctrl_set_rows(gallery, 0, 36, 0);
		listctrl_set_cols(gallery, 0, 36);
#elif (LCD_WIDTH == 220) && (LCD_HEIGHT == 176)
		DSAppStore_SetIconSize(ds, 28, 28);
		listctrl_set_rows(gallery, 0, 30, 0);
		listctrl_set_cols(gallery, 0, 30);
#elif (LCD_WIDTH == 176) && (LCD_HEIGHT == 220)
		DSAppStore_SetIconSize(ds, 28, 28);
		listctrl_set_rows(gallery, 0, 30, 0);
		listctrl_set_cols(gallery, 0, 30);
#elif (LCD_WIDTH == 128) && (LCD_HEIGHT == 160)
		DSAppStore_SetIconSize(ds, 20, 20);
		listctrl_set_rows(gallery, 0, 24, 0);
		listctrl_set_cols(gallery, 0, 24);
#endif
		listctrl_attach_datasource(gallery, ds);
		g_object_unref((t_HObject)ds);

	}
#if defined(FTR_IDLE_TODAY_DISPLAY)
	gallery = wnd_get_widget(handle, RESID_LIST_TODAY);
	if(g_object_valid(gallery)){
		ds = DSToday_Create();
		listctrl_attach_datasource(gallery, ds);
		g_object_unref((t_HObject)ds);

		listctrl_refresh_datasource(gallery, FALSE);
		listctrl_set_viewtype(gallery, GUI_VIEWTYPE_NORMAL);
		listctrl_set_flags((t_HList)gallery, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM|CLFLAG_AUTOTAB_UD|CLFLAG_CURSOR_AUTOCHG_ONSETFOCUS|CLFLAG_CURSOR_NODRAW_ONKILLFOCUS|CLFLAG_DISABLE_VLINE);
	}
#endif
#if defined(FTR_SOFT_KEY)
		{
			t_SKHandle sk;
			sk = wnd_get_skbar(handle);
			if(g_object_valid(sk)){
				sk_set_app(sk, SK_LSK, "mainmenu", 0);
				sk_set_app(sk, SK_RSK, "taskmng", 0);
			}
		}
#endif
	return SUCCESS;
}

error_t	desktop_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t handled = NOT_HANDLED;
	t_DesktopGlobData *glob = task_get_data();

	switch(evt){
		case WINDOW_OnOpen:
			handled = __on_win_open(glob, handle);
			break;
		case WINDOW_OnClose:
			handled = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			handled = SUCCESS;
			break;
		case WIDGET_OnKillFocus:
			handled = SUCCESS;
			break;
		case WINDOW_OnDraw:
			g_printf("desktop:WINDOW_OnDraw");
#if defined(FTR_IDLE_TODAY_DISPLAY)
			{
				t_HList gallery;
				gallery = wnd_get_widget(handle, RESID_LIST_TODAY);
				listctrl_refresh_datasource(gallery, TRUE);
			}
#endif
			break;
		case WIDGET_OnKeyEvent:
			if(sparam != MMI_KEY_PRESS && sparam != MMI_KEY_REPEAT)
				break;
			if(MVK_IS_DIGIT(lparam)){
				t_MimeMethodInfo info;
				t_TaskID tid;
				if(mime_service_query(MIME_TYPE_DIALER, METHOD_NAME_OPEN, NULL, &info, 1, 0, FALSE) < 1){
					msgbox_show(NULL, _CSK(OK), 0, 0, "No dialer provided");
					break;
				}
				if(pack_call(info.guid, info.module, NULL, 0, FALSE, &tid) == SUCCESS){
					msg_send_s(tid, NULL, EVT_KEY, sparam, lparam);
				}
				
				handled = SUCCESS;
			}
			break;
		case WINDOW_OnMessage:
		if(lparam){
			t_AmosMsg *msg = (t_AmosMsg *)lparam;
		}
	}

	return handled;
}

