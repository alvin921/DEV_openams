#include "pack_simlock.hi"

error_t	simlock_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_simlockData *glob = app_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			{
				t_HGDI gdi = wgt_get_client_gdi((t_HWidget)handle);
				if(gdi){
					gdi_set_color(gdi, RGB_RED);
					gdi_draw_text_at(gdi, 30, 30, "Hello World!", -1, CHARSET_UTF8);
					g_object_unref(gdi);
				}
			}
			ret = SUCCESS;
			break;
	}
	return ret;
}

