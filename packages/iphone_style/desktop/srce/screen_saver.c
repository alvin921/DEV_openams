#include "pack_desktop.hi"


error_t	screensaver_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WINDOW_OnOpen:
			wgt_enable_attr(handle, WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_STATUS);
			{
				t_HWidget widget = wgt_slider_create((t_HWidget)handle, SLIDER_TO_UNLOCK);
				gui_rect_t client;
				wgt_get_client_rect((t_HWidget)handle, &client);
				wgt_set_bound(widget, 0, client.dy-60, client.dx, 60);
			}
			amos_pen_enable_slide(FALSE);
			break;
		case WINDOW_OnClose:
			amos_pen_enable_slide(TRUE);
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			switch(lparam){
				case MVK_END:
					ret = SUCCESS; //2 ·ÀÖ¹home¼üÍË»Øµ½idle
					break;
			}
			break;
	}
	return ret;
}

