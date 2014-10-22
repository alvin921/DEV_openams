#define MOD_CONTACT_MAIN_C

#include "pack_contact.hi"

#define	RESID_CTRL_CONTACT	(RESID_CTRL_DYNAMIC+100)
error_t	contact_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_HWidget widget;
	
	switch(evt){
		case WINDOW_OnOpen:
		{
			gui_rect_t client;

#if defined(FTR_SOFT_KEY) && !defined(FTR_NO_INPUT_KEY)
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
#else
			wgt_enable_attr(handle, WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_STATUS|WND_ATTR_HAS_TITLE);
#endif
			widget = wgt_create((t_HWidget)handle, "contact");
			wgt_get_client_rect((t_HWidget)handle, &client);
			wgt_set_bound(widget, 0, 0, client.dx, client.dy);
			wgt_set_border(widget, BORDER_TYPE_NONE, 0, 0);
			wgt_set_resid(widget, RESID_CTRL_CONTACT);
			wgt_contact_attach(widget, sparam, lparam);

#if defined(FTR_SOFT_KEY) && !defined(FTR_NO_INPUT_KEY)
			widget = wnd_get_skbar(handle);
			if(g_object_valid(widget)){
				sk_set_text(widget, SK_LSK, NULL, RESID_STR_ADD_CONTACT);
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
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			break;
	}
	return ret;
}

#undef MOD_CONTACT_MAIN_C

