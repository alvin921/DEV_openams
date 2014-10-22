#include "pack_settings.hi"

enum {
	RESID_LIST_LANG = RESID_CTRL_DYNAMIC,
};

static error_t	__lang_widget_on_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_HWindow wnd = userdata;
	setting_glob_t *glob = app_get_data();
	switch(evt){
		case WIDGET_OnSelected:
			{
				t_LangID lang = lang_supported_get(sparam);
				lang_set_current(lang);
				wnd_send_close(wnd, 0);
			}
			ret = SUCCESS;
			break;
	}
	return ret;
}

static void __list_redraw_lang(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	t_LangName *lang = lang_get(lang_supported_get(index));
	if(lang){
		gdi_draw_text_in(gdi, 8, 0, width-8, height, lang->comment, -1, CHARSET_UNDEF, IDF_VALIGN_MIDDLE|IDF_HALIGN_LEFT);
	}
}

error_t	settings_LanguageWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	setting_glob_t *glob = app_get_data();
	error_t ret = NOT_HANDLED;
	
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			{
				t_HWidget widget;
				int h = prod_ui_list_item_height();
				gui_rect_t client;
				coord_t y = Y_OFFSET;
				gu8 sim, n;
				gu8 sim_max = prod_get_sim_slots();
				char f[20];

				wgt_get_client_rect((t_HWidget)handle, &client);

				//2 Language List
				widget = wgt_create(handle, "listctrl");
				wgt_set_resid(widget, RESID_LIST_LANG);
				wgt_hook_register(widget, WIDGET_OnSelected, __lang_widget_on_hook, handle);
				listctrl_set_style(widget, LISTCTRL_STYLE_RADIO);
				listctrl_set_flags(widget, CLFLAG_AUTOTAB_UD|CLFLAG_CURSOR_NODRAW_ONKILLFOCUS|CLFLAG_CURSOR_AUTOCHG_ONSETFOCUS);
#if defined(FTR_IPHONE_STYLE)
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftBottom|guiRightBottom);
#else
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom);
#endif
				wgt_set_bound(widget, 0, 0, client.dx, client.dy);
				{
					t_LangID cur = lang_get_current();
					int count = lang_supported_num();
					int i;
					
					listctrl_set_count(widget, count);
					for(i = 0; i < count; i++){
						if(cur == lang_supported_get(i)){
							listctrl_set_select(widget, i, TRUE);
							listctrl_set_cursor(widget, i, FALSE);
							break;
						}
					}
				}
				listctrl_attach(widget, __list_redraw_lang, handle);
				//2 Softkey
				widget = wnd_get_skbar(handle);
				if(g_object_valid(widget)){
					sk_set_text(widget, SK_LSK, NULL, RESID_STR_OK);
					sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
				}
			}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			//gdi_draw_text_at(, "Hello World!", -1, 30, 30, RGB_WHITE, RGB_BLACK, TRUE);
			ret = SUCCESS;
			break;
	}
	return ret;
}
