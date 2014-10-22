#include "pack_settings.hi"

enum {
	RESID_NW_ONOFF = RESID_CTRL_DYNAMIC,

	RESID_NW_EDITOR_APN,
	RESID_NW_EDITOR_USER,
	RESID_NW_EDITOR_PASS,
};

static error_t	__nw_widget_on_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_HWindow wnd = userdata;
	setting_glob_t *glob = app_get_data();
	t_HWidget widget;
	gu8 sim = wgt_get_tag(wnd);
	char f[20];
	char *apn, *user, *pass, *tmp;

	switch(wgt_get_resid(handle)){
		case RESID_NW_ONOFF:
			if(evt == WIDGET_OnSelected){
				sprintf(f, "%s%d", SYS_NVRAM_APN, sim);
				apn = sys_nvram_get_string(f);
				sprintf(f, "%s%d", SYS_NVRAM_APN_USER, sim);
				user = sys_nvram_get_string(f);
				sprintf(f, "%s%d", SYS_NVRAM_APN_PASS, sim);
				pass = sys_nvram_get_string(f);
		
				msgbox_show(NULL, 0, 500, 0, onoff_get_value(handle)?"Network switching ON":"Network switching OFF");
				sprintf(f, "%s%d", SYS_NVRAM_NETWORK, sim);
				if(onoff_get_value(handle)){
					if(sys_nw_start_gprs(sim, apn, user, pass, INFINITE) == SUCCESS){
						sys_nvram_set_integer(f, onoff_get_value(handle));
					}else{
						msgbox_show(NULL, 0, 500, 0, "Network switching ON Failed");
						onoff_set_value(handle, FALSE);
					}
				}else{
					if(sys_nw_stop_gprs(sim, INFINITE) == NULL){
						sys_nvram_set_integer(f, onoff_get_value(handle));
					}else{
						msgbox_show(NULL, 0, 500, 0, "Network switching OFF Failed");
						onoff_set_value(handle, TRUE);
					}
				}
			}
			ret = SUCCESS;
			break;
		case RESID_NW_EDITOR_APN:
			if(evt == WIDGET_OnKillFocus || evt == WIDGET_OnSelected){
				sprintf(f, "%s%d", SYS_NVRAM_APN, sim);
				apn = sys_nvram_get_string(f);
				tmp = editor_get_text_dup(handle);
				if(apn == NULL || g_stricmp(apn, tmp)){
					sys_nvram_set_string(f, tmp);
				}
				FREEIF(tmp);
				ret = SUCCESS;
			}
			break;
		case RESID_NW_EDITOR_USER:
			if(evt == WIDGET_OnKillFocus || evt == WIDGET_OnSelected){
				sprintf(f, "%s%d", SYS_NVRAM_APN_USER, sim);
				user = sys_nvram_get_string(f);
				tmp = editor_get_text_dup(handle);
				if(user == NULL || g_stricmp(user, tmp)){
					sys_nvram_set_string(f, tmp);
				}
				FREEIF(tmp);
				ret = SUCCESS;
			}
			break;
		case RESID_NW_EDITOR_PASS:
			if(evt == WIDGET_OnKillFocus || evt == WIDGET_OnSelected){
				sprintf(f, "%s%d", SYS_NVRAM_APN_PASS, sim);
				pass = sys_nvram_get_string(f);
				tmp = editor_get_text_dup(handle);
				if(pass == NULL || g_stricmp(pass, tmp)){
					sys_nvram_set_string(f, tmp);
				}
				FREEIF(tmp);
				ret = SUCCESS;
			}
			break;
	}
	return ret;
}


//2 sparam=<sim>
static error_t	settings_NetworkSubWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
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
				gu8 sim = sparam;
				gu8 sim_max = prod_get_sim_slots();
				char f[20];

				wgt_set_tag(handle, sim);
				
				wgt_get_client_rect((t_HWidget)handle, &client);

				//2 Network
				widget = wgt_create(handle, "onoff");
				wgt_set_resid(widget, RESID_NW_ONOFF);
				wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_STR_NETWORK);
				wgt_hook_register(widget, WIDGET_OnSelected, __nw_widget_on_hook, handle);
				wgt_set_bound(widget, X_OFFSET, y, lcd_get_width()-2*X_OFFSET, h);
#if defined(FTR_IPHONE_STYLE)
				y += h-1;
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftTop|guiRightTop);
#else
				y += h;
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom|guiTop);
#endif
				sprintf(f, "%s%d", SYS_NVRAM_NETWORK, sim);
				if(sys_nvram_get_integer(f))
					onoff_set_value(widget, TRUE);

				//2 Editor: APN
				widget = wgt_create(handle, "editor");
				wgt_set_resid(widget, RESID_NW_EDITOR_APN);
				wgt_set_caption(widget, "APN: ");
				wgt_hook_register(widget, WIDGET_OnSelected|WIDGET_OnKillFocus, __nw_widget_on_hook, handle);
				wgt_set_bound(widget, X_OFFSET, y, lcd_get_width()-2*X_OFFSET, h);
				y += h+Y_OFFSET;
#if defined(FTR_IPHONE_STYLE)
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftBottom|guiRightBottom);
#else
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom);
#endif
				sprintf(f, "%s%d", SYS_NVRAM_APN, sim);
				editor_set_text(widget, sys_nvram_get_string(f));
				
				//2 Editor: APN USER
				widget = wgt_create(handle, "editor");
				wgt_set_resid(widget, RESID_NW_EDITOR_USER);
				wgt_set_caption(widget, "USER: ");
				wgt_hook_register(widget, WIDGET_OnSelected|WIDGET_OnKillFocus, __nw_widget_on_hook, handle);
				wgt_set_bound(widget, X_OFFSET, y, lcd_get_width()-2*X_OFFSET, h);
				y += h+Y_OFFSET;
#if defined(FTR_IPHONE_STYLE)
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftBottom|guiRightBottom);
#else
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom);
#endif
				sprintf(f, "%s%d", SYS_NVRAM_APN_USER, sim);
				editor_set_text(widget, sys_nvram_get_string(f));
				
				//2 Editor: APN Password
				widget = wgt_create(handle, "editor");
				wgt_set_resid(widget, RESID_NW_EDITOR_PASS);
				wgt_set_caption(widget, "Password: ");
				wgt_hook_register(widget, WIDGET_OnSelected|WIDGET_OnKillFocus, __nw_widget_on_hook, handle);
				wgt_set_bound(widget, X_OFFSET, y, lcd_get_width()-2*X_OFFSET, h);
				y += h+Y_OFFSET;
#if defined(FTR_IPHONE_STYLE)
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftBottom|guiRightBottom);
#else
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom);
#endif
				sprintf(f, "%s%d", SYS_NVRAM_APN_PASS, sim);
				editor_set_text(widget, sys_nvram_get_string(f));
				
				//2 Softkey
				widget = wnd_get_skbar(handle);
				if(g_object_valid(widget)){
					sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
					//sk_set_text(widget, SK_LSK, NULL, RESID_STR_DETAIL);
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




static void miRedrawHintNetwork(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	t_HMenu menu = userdata;
	gu8 sim = menu_GetItemID(menu, index)-RESID_STR_NETWORK1;
	char *str;
	int iw, ih, x, y;
	
	if(sys_nw_gprs_started(sim))
		str = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_OPENED, "Opened");
	else
		str = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_CLOSED, "Closed");
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
	gu8 sim = menu_GetItemID(menu, listctrl_get_cursor(menu))-RESID_STR_NETWORK1;
	wnd_show(wnd_create(settings_NetworkSubWndMsgHandler, sim, 0));
}

static t_MenuItem miSettingsNetwork[] = {
	MITEM_HINT(0	, RESID_STR_NETWORK1	, miRedrawHintNetwork	, CmdNetwork),
	MITEM_HINT(0	, RESID_STR_NETWORK2	, miRedrawHintNetwork	, CmdNetwork),
	MITEM_HINT(0	, RESID_STR_NETWORK3	, miRedrawHintNetwork	, CmdNetwork),
	MITEM_HINT(0	, RESID_STR_NETWORK4	, miRedrawHintNetwork	, CmdNetwork),
	MITEM_END
};


enum {
	RESID_MENU_SETTING = RESID_CTRL_DYNAMIC,

};
error_t	settings_NetworkWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
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

				//2 Network
				n = 0;
				for(sim = 0; sim < sim_max; sim++){
					if(prod_sim_status(sim) == SIM_CARD_NORMAL){
						n++;
					}
				}
				widget = wgt_create(handle, "menu");
				wgt_set_resid(widget, RESID_MENU_SETTING);
				menu_bind(widget, miSettingsNetwork, handle);
				listctrl_set_flags(widget, CLFLAG_AUTOTAB_UD|CLFLAG_CURSOR_NODRAW_ONKILLFOCUS|CLFLAG_CURSOR_AUTOCHG_ONSETFOCUS);
#if defined(FTR_IPHONE_STYLE)
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftBottom|guiRightBottom);
#else
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom);
#endif
				wgt_set_bound(widget, X_OFFSET, y, lcd_get_width()-2*X_OFFSET, h*n);
				y += h*n+Y_OFFSET;
				
				for(sim = 0; sim < SIM_MAX; sim++){
					if(prod_sim_status(sim) == SIM_CARD_NORMAL){
						char f[20];
						menu_SetItemVisible(widget, sim, IDENT_BY_POSITION, TRUE);
						sprintf(f, "%s%d", SYS_NVRAM_FLIGHTMODE, sim);
						if(sys_nvram_get_integer(f))
							menu_EnableItem(widget, sim+RESID_STR_NETWORK1, IDENT_BY_ID, FALSE);
					}else{
						menu_SetItemVisible(widget, sim, IDENT_BY_POSITION, FALSE);
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

