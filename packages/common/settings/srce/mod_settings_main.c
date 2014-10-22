#include "pack_settings.hi"


enum {
	RESID_ONOFF_FLIGHTMODE = RESID_CTRL_DYNAMIC,

	RESID_ONOFF_FLIGHTMODE_LAST = RESID_ONOFF_FLIGHTMODE+SIM_MAX,
	
	RESID_MENU_SETTING,

};


static error_t	__flightmode_widget_on_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_HWindow wnd = userdata;
	setting_glob_t *glob = app_get_data();
	t_HWidget widget;
	gu8 sim;
	char f[20];
	switch(evt){
		case WIDGET_OnSelected:
			switch(wgt_get_resid(handle)){
				case RESID_ONOFF_FLIGHTMODE:
				case RESID_ONOFF_FLIGHTMODE+1:
				case RESID_ONOFF_FLIGHTMODE+2:
				case RESID_ONOFF_FLIGHTMODE+3:
					sim = wgt_get_resid(handle)-RESID_ONOFF_FLIGHTMODE;
					msgbox_show(NULL, 0, 500, 0, onoff_get_value(handle)?"Flight mode switching ON":"Flight mode switching OFF");
					sprintf(f, "%s%d", SYS_NVRAM_FLIGHTMODE, sim);
					if(onoff_get_value(handle)){
						if(sys_nw_close(sim) == SUCCESS){
							sys_nvram_set_integer(f, onoff_get_value(handle));
						}else{
							onoff_set_value(handle, FALSE);
						}
					}else{
						if(sys_nw_open(sim) == SUCCESS){
							sys_nvram_set_integer(f, onoff_get_value(handle));
						}else{
							onoff_set_value(handle, TRUE);
						}
					}
					ret = SUCCESS;
					break;
			}
			break;
	}
	return ret;
}

static error_t	settings_FlightmodeWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
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
#if defined(FTR_IPHONE_STYLE)
	wgt_set_bg_fill_solid(handle, RGB_WHITE);
#endif
				//2 Flight Mode
				for(sim = 0; sim < sim_max; sim++){
					if(prod_sim_status(sim) != SIM_CARD_ABSENT){
						widget = wgt_create(handle, "onoff");
						wgt_set_resid(widget, RESID_ONOFF_FLIGHTMODE+sim);
						wgt_set_caption(widget, "%s SIM%d", brd_get_string(PACK_GUID_SELF, RESID_STR_FLIGHT_MODE, "Flight Mode"), sim+1);
						wgt_hook_register(widget, WIDGET_OnSelected, __flightmode_widget_on_hook, handle);
						wgt_set_bound(widget, X_OFFSET, y, lcd_get_width()-2*X_OFFSET, h);
#if defined(FTR_IPHONE_STYLE)
						y += h-1;
						wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftTop|guiRightTop);
#else
						y += h;
						wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom|guiTop);
						wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom);
#endif
						sprintf(f, "%s%d", SYS_NVRAM_FLIGHTMODE, sim);
						if(sys_nvram_get_integer(f))
							onoff_set_value(widget, TRUE);
					}
				}

				//2 Softkey
				widget = wnd_get_skbar(handle);
				if(g_object_valid(widget)){
					sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
					sk_set_text(widget, SK_LSK, NULL, RESID_STR_OK);
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

static void miRedrawHintFlightmode(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	t_HMenu menu = userdata;
	gu8 sim_max = prod_get_sim_slots();
	gu8 sim;
	char *str;
	int iw, ih, x, y;
	char f[20];

	str = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_OPENED, "Opened");
	for(sim = 0; sim < sim_max; sim++){
		if(prod_sim_status(sim) != SIM_CARD_ABSENT){
			sprintf(f, "%s%d", SYS_NVRAM_FLIGHTMODE, sim);
			if(!sys_nvram_get_integer(f)){
				str = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_CLOSED, "Closed");
				break;
			}
		}
	}
	gdi_measure_text(gdi, str, -1, CHARSET_UTF8, width, &iw, &ih, NULL);

	x = width-iw-2-2-2;
	y = (height-ih)/2;
	gdi_draw_text_at(gdi, x, y, str, -1, CHARSET_UTF8);

	//2 绘制>
	x += iw+2;
	gdi_move_to(gdi, x, y);
	gdi_line_to(gdi, x+2, height/2);
	gdi_line_to(gdi, x, height-y);
}

static void		CmdFlightmode(t_HMenu menu, void *data)
{
	wnd_show(wnd_create(settings_FlightmodeWndMsgHandler, 0, 0));
}

static void miRedrawHintNetwork(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	t_HMenu menu = userdata;
	gu8 sim_max = prod_get_sim_slots();
	gu8 sim;
	char *str;
	int iw, ih, x, y;

	str = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_CLOSED, "Closed");
	for(sim = 0; sim < sim_max; sim++){
		if(sys_nw_gprs_started(sim)){
			str = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_OPENED, "Opened");
			break;
		}
	}
	gdi_measure_text(gdi, str, -1, CHARSET_UTF8, width, &iw, &ih, NULL);

	x = width-iw-2-2-2;
	y = (height-ih)/2;
	gdi_draw_text_at(gdi, x, y, str, -1, CHARSET_UTF8);
	x += iw+2;

	gdi_move_to(gdi, x, y);
	gdi_line_to(gdi, x+2, height/2);
	gdi_line_to(gdi, x, height-y);
}

static void		CmdNetwork(t_HMenu menu, void *data)
{
	wnd_show(wnd_create(settings_NetworkWndMsgHandler, 0, 0));
}

static void miRedrawHintLanguage(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	t_HMenu menu = userdata;
	int iw, ih, x, y;
	t_LangName *lang = lang_get(lang_get_current());

	gdi_measure_text(gdi, lang->comment, -1, CHARSET_UNDEF, width, &iw, &ih, NULL);

	x = width-iw-2-2-2;
	y = (height-ih)/2;
	gdi_draw_text_at(gdi, x, y, lang->comment, -1, CHARSET_UNDEF);
	x += iw+2;

	gdi_move_to(gdi, x, y);
	gdi_line_to(gdi, x+2, height/2);
	gdi_line_to(gdi, x, height-y);
}

static void		CmdLanguage(t_HMenu menu, void *data)
{
	wnd_show(wnd_create(settings_LanguageWndMsgHandler, 0, 0));
}

static t_MenuItem miSettings[] = {
	MITEM_HINT(0	, RESID_STR_FLIGHT_MODE	, miRedrawHintFlightmode	, CmdFlightmode),
	MITEM_HINT(0	, RESID_STR_NETWORK 	, miRedrawHintNetwork	, CmdNetwork),
	MITEM_HINT(0	, RESID_STR_LANGUAGE	, miRedrawHintLanguage	, CmdLanguage),
	MITEM_END
};


error_t	settings_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	setting_glob_t *glob = app_get_data();
	error_t ret = NOT_HANDLED;
	
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
#if defined(FTR_IPHONE_STYLE)
			wgt_set_bg_fill_solid(handle, RGB_WHITE);
#endif

			{
				t_HWidget widget;
				gui_rect_t client;
				coord_t y = Y_OFFSET;

				wgt_get_client_rect((t_HWidget)handle, &client);

				widget = wgt_create(handle, "menu");
				wgt_set_resid(widget, RESID_MENU_SETTING);
				menu_bind(widget, miSettings, handle);
				listctrl_set_flags(widget, /*CLFLAG_AUTOTAB_UD|*/CLFLAG_CURSOR_NODRAW_ONKILLFOCUS|CLFLAG_CURSOR_AUTOCHG_ONSETFOCUS);
#if defined(FTR_IPHONE_STYLE)
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiFourCorner);
#else
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom|guiTop);
#endif
				wgt_set_bound(widget, X_OFFSET, y, client.dx-2*X_OFFSET, client.dy);
				//2 飞行模式则禁止设置网络
				if(sys_nw_is_flightmode())
					menu_EnableItem(widget, RESID_STR_NETWORK, IDENT_BY_ID, FALSE);

				//2 Softkey
				widget = wnd_get_skbar(handle);
				if(g_object_valid(widget)){
					sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
					sk_set_text(widget, SK_LSK, NULL, RESID_STR_OK);
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
			//2 飞行模式则禁止设置网络
			if(sys_nw_is_flightmode())
				menu_EnableItem(wnd_get_widget(handle, RESID_MENU_SETTING), RESID_STR_NETWORK, IDENT_BY_ID, FALSE);
			else
				menu_EnableItem(wnd_get_widget(handle, RESID_MENU_SETTING), RESID_STR_NETWORK, IDENT_BY_ID, TRUE);
			ret = SUCCESS;
			break;
	}
	return ret;
}
