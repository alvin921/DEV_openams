#include "pack_desktop.hi"

static error_t	__button_on_click(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	wnd_send_close(wgt_parent_window(handle), 0);
	return SUCCESS;
}

error_t	shutdown_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;

	switch(evt){
		case WINDOW_OnOpen:
			wgt_enable_attr(handle, WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_STATUS);
			{
				t_HWidget widget = wgt_slider_create((t_HWidget)handle, SLIDER_TO_POWEROFF);
				gui_rect_t client;
				wgt_get_client_rect((t_HWidget)handle, &client);

				wgt_set_bound(widget, 0, 10, wgt_bound_dx(handle), 60);

				widget = wgt_create((t_HWidget)handle, "button");
				wgt_set_bound(widget, 20, client.dy-40, 200, 30);
				wgt_set_caption_by_resid(widget, BRD_HANDLE_SELF, RESID_STR_CANCEL);
				wgt_set_bg_fill(widget, GDI_FILL_MODE_2COLOR_H, MAKE_RGB(224,224,224), MAKE_RGB(0xc0,0xc0,0xc0));
				wgt_set_border(widget, BORDER_TYPE_3DIN, 1, guiFourCorner);
				wgt_hook_register(widget, WIDGET_OnSelected, __button_on_click, NULL);

			}

			amos_pen_enable_slide(FALSE);
			break;
		case WINDOW_OnClose:
			amos_pen_enable_slide(TRUE);
			ret = SUCCESS;
			break;
		case WIDGET_OnPreDraw:
			{
				t_HGDI gdi = wgt_get_client_gdi((t_HWidget)handle);
				gui_rect_t client;
				wgt_get_client_rect((t_HWidget)handle, &client);
				gdi_set_color(gdi, 0xd0404040);
				gdi_rectangle_fill(gdi, 0, client.dy-60, client.dx, 60, 0);
				g_object_unref(gdi);
			}
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

