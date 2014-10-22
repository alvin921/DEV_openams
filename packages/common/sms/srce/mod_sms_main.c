#include "pack_sms.hi"

#if defined(FTR_COMM_SESSION_SUPPORT)

static void	__csl_notify(t_HDataSet hds, t_DBRecordID rid, void *userdata)
{
	t_HWindow handle = (t_HWindow)userdata;
	//sms_glob_t *glob = task_get_data();
	if(g_object_valid(hds)){
		g_ds_CondSet1(hds, CSESSION_DBFIELD_TYPE, DBREL_EQ, COMM_TYPE_SMS);
		g_ds_OrderBy(hds, CSESSION_DBFIELD_TIME, FALSE, FALSE);
		g_ds_Query(hds, TRUE);

		if(wgt_is_focused(handle)){
			wgt_redraw(wnd_get_widget(handle, RESID_LIST), FALSE);
		}

	}
}

static error_t	__csl_list_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	sms_glob_t *glob = task_get_data();
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnKeyEvent:
			switch(lparam){
				case MVK_OK:
					if(sparam == MMI_KEY_LONG_PRESS && g_ds_GetRecordNum(glob->hds_cs)){
						if(msgbox_show(NULL, MBB_YES_NO, 0, 0, brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_DEL, "")) == MBB_YES){
							t_ContactID sid;

							g_ds_GotoByIndex(glob->hds_cs, listctrl_get_cursor(handle));
							sid = g_ds_GetValue(glob->hds_cs, CSESSION_DBFIELD_SID);
							g_ds_CondSet1(glob->ds, SMS_DBFIELD_SID, DBREL_EQ, sid);
							g_ds_Query(glob->ds, TRUE);
							while(!g_ds_Empty(glob->ds)){
								g_ds_First(glob->ds);
								g_ds_Delete(glob->ds);
							}
							g_ds_Delete(glob->hds_cs);

							wgt_redraw(handle, FALSE);
						}
						ret = SUCCESS;
					}
					break;
			}
			break;
	}
	return ret;
}

static void	csl_redraw_record(t_HDataSet hds, int index, t_HGDI gdi, t_GuiViewType viewtype, int width, int height, gboolean hilight)
{
	t_ContactInfo *cinfo;
	char *name, *number;
	datetime_t curr_dt;
	dos_datetime_t dt;
	char dt_string[14];
	t_HImage icon = NULL;
	int tw, th;
	t_CSessionID sid;
	t_UITheme *theme = ui_theme_get();
	coord_t x=1;

	g_ds_GotoByIndex(hds, index);
	number = g_ds_GetString(hds, CSESSION_DBFIELD_NUMBER);
	sid = g_ds_GetValue(hds, CSESSION_DBFIELD_SID);
	name = number;

	cinfo = contact_get_info(sid);
	if(cinfo){
		name = cinfo->name;
		if(cinfo->pic_resid){
			icon = brd_get_image("contact", cinfo->pic_resid);
		}else if(cinfo->pic_fname && *cinfo->pic_fname){
			icon = image_load_from_file(cinfo->pic_fname);
		}
		if(g_object_valid(icon)){
			gdi_draw_image_in(gdi, 0, 0, height, height, icon, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE | IDF_HALIGN_STRETCH | IDF_VALIGN_STRETCH);
			width -= height;
			x = height;
			g_object_unref(icon);
		}
	}
	dt = g_ds_GetValue(hds, CSESSION_DBFIELD_TIME);
	//dostime_to_datetime(g_ds_GetValue(hds, CSESSION_DBFIELD_TIME), &dt);

g_printf("[%d] number=%s", index, number);

	if(VIEWTYPE_IS_NORMAL(viewtype)){
		sms_glob_t *glob = task_get_data();
		int line_dy;
		
#define X_MARGIN	10
		//2 Line 1: <name>(<N>) 					 <time>
		//3 1.1 <name>
#if defined(FTR_IPHONE_STYLE)
		gdi_set_color(gdi, RGB_BLACK);
		line_dy = height/3;
#else
		if(hilight)
			gdi_set_color(gdi, theme->hilight_fg_color);
		else
			gdi_set_color(gdi, theme->fg_color);
		line_dy = height/2;
#endif
		gdi_draw_text_in(gdi, x, 0, width, line_dy, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
#if (LCD_WIDTH <= 128)
		//3 2.1 <N>
		gdi_set_color(gdi, RGB_GRAY);
		sprintf(dt_string, "(%d)", g_ds_GetValue(hds, CSESSION_DBFIELD_COUNT));
		gdi_draw_text_in(gdi, x, line_dy, width, line_dy, dt_string, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
		//3 2.2 <time>
		gdi_set_color(gdi, RGB_BLUE);
		datetime_current(&curr_dt);
		if(curr_dt.dt_year == DOSDATE_Y(dt>>16) && curr_dt.dt_month == DOSDATE_M(dt>>16) && curr_dt.dt_day == DOSDATE_D(dt>>16))
			sprintf(dt_string, "%02d:%02d", DOSTIME_H(dt), DOSTIME_M(dt));
		else
			sprintf(dt_string, "%02d-%02d", DOSDATE_M(dt>>16), DOSDATE_D(dt>>16));
		gdi_draw_text_in(gdi, x, line_dy, width-X_MARGIN, line_dy, dt_string, -1, CHARSET_US_ASCII, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
#else
		//3 1.2 <N>
		gdi_set_color(gdi, RGB_GRAY);
		sprintf(dt_string, "(%d)", g_ds_GetValue(hds, CSESSION_DBFIELD_COUNT));
		gdi_draw_text(gdi, dt_string, -1, CHARSET_UTF8);
		//3 1.3 <time>
		gdi_set_color(gdi, RGB_BLUE);
		datetime_current(&curr_dt);
		if(curr_dt.dt_year == DOSDATE_Y(dt>>16) && curr_dt.dt_month == DOSDATE_M(dt>>16) && curr_dt.dt_day == DOSDATE_D(dt>>16))
			sprintf(dt_string, "%02d:%02d", DOSTIME_H(dt), DOSTIME_M(dt));
		else
			sprintf(dt_string, "%02d-%02d", DOSDATE_M(dt>>16), DOSDATE_D(dt>>16));
		gdi_draw_text_in(gdi, x, 0, width-X_MARGIN, line_dy, dt_string, -1, CHARSET_US_ASCII, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
		
		//2 Line2&3: content
		g_ds_OrderBy(glob->ds, SMS_DBFIELD_TIME, FALSE, FALSE);
		g_ds_CondSet1(glob->ds, SMS_DBFIELD_SID, DBREL_EQ, sid);
		g_ds_Query(glob->ds, TRUE);
		
		if(g_ds_GetRecordNum(glob->ds)){
			gu8 dcs, *data, type;
			gu16 size;
		
			g_ds_First(glob->ds);
			type = g_ds_GetValue(glob->ds, SMS_DBFIELD_TYPE);
			dcs = g_ds_GetValue(glob->ds, SMS_DBFIELD_DCS);
			data = g_ds_GetBinary(glob->ds, SMS_DBFIELD_DATA, &size);
		
			gdi_set_color(gdi, RGB_GRAY);
			if(dcs == DCS_7BIT){
				th = gdi_draw_text_in(gdi, x, line_dy, width-X_MARGIN, line_dy, (const char *)data, size, CHARSET_US_ASCII, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
#if defined(FTR_IPHONE_STYLE)
				if(th < size)
					gdi_draw_text_in(gdi, x, line_dy*2, width-X_MARGIN, line_dy, (const char *)(data+th), size-th, CHARSET_US_ASCII, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
#endif
			}else if(dcs == DCS_UCS2){
				th = gdi_draw_utext_in(gdi, x, line_dy, width-X_MARGIN, line_dy, (const ucs2_t *)data, size>>1, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
#if defined(FTR_IPHONE_STYLE)
				if(th < size/2)
					gdi_draw_utext_in(gdi, x, line_dy*2, width-X_MARGIN, line_dy, (const ucs2_t *)(data+2*th), (size>>1)-th, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
#endif
			}else{
				gdi_draw_text_in(gdi, x, line_dy, width-X_MARGIN, line_dy, "<unknown>", -1, CHARSET_US_ASCII, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
			}
		}
#endif
	}
	contact_free_info(cinfo);
}

void	csl_view_record(t_HDataSet hds, int index)
{
	//glob->cursor = index;
	g_ds_GotoByIndex(hds, index);
#if defined(SMALL_LCD)
	if(g_ds_GetValue(hds, CSESSION_DBFIELD_COUNT) == 1){
		sms_glob_t *glob = app_get_data();
		t_CSessionID cid = g_ds_GetValue(glob->hds_cs, CSESSION_DBFIELD_SID);
		
		g_ds_OrderBy(glob->ds, SMS_DBFIELD_TIME, TRUE, FALSE);
		g_ds_CondSet1(glob->ds, SMS_DBFIELD_SID, DBREL_EQ, cid);
		g_ds_Query(glob->ds, TRUE);
		wnd_show(wnd_create(sms_ViewWndMsgHandler, 0, glob->ds));
	}else
#endif
		wnd_show(wnd_create(sms_ViewSessionWndMsgHandler, g_ds_CurrentID(hds), hds));
}
#endif




static void	LskNewCallback(t_HWidget sk, void *data)
{
	t_HWindow handle = (t_HWindow)data;
	//2 主界面的LSK用于创建新信息
	if(wgt_get_resid(handle) == RESID_WIN_MAIN){
		wnd_new_message(WITH_NONE, NULL);
	}
}

error_t	sms_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	sms_glob_t *glob = task_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
			{
				t_HList list;
				gui_rect_t client;
				
				wgt_enable_attr(handle, WND_ATTR_DEFAULT);
				wgt_get_client_rect((t_HWidget)handle, &client);
				list = wnd_get_widget(handle, RESID_LIST);
				wgt_set_bound(list, 0, 0, client.dx, client.dy);
				listctrl_set_viewtype(list, GUI_VIEWTYPE_NORMAL);
				listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE|CLFLAG_ENABLE_ITEM_FSHOW);
				listctrl_set_cols(list, 1, 0);
#if defined(FTR_COMM_SESSION_SUPPORT)
				g_ds_register_notify(glob->hds_cs, __csl_notify, handle);
				g_ds_bind(glob->hds_cs, csl_redraw_record, csl_view_record);
				listctrl_attach_db(list, glob->hds_cs);
				wgt_hook_register(list, WIDGET_OnKeyEvent, __csl_list_hook, NULL);
#if defined(FTR_IPHONE_STYLE)
				listctrl_set_rows(list, 0, 40, 0);
#else
				listctrl_set_rows(list, 0, prod_ui_list_item_height()*2, 0);
#endif
#else
				g_printf("sms_count=%d", g_ds_GetRecordNum(glob->ds));
				listctrl_set_rows(list, 0, prod_ui_list_item_height()*2, 0);
				g_ds_bind(glob->ds, db_redraw_record, db_view_record);
				listctrl_attach_db(list, glob->ds);
#endif
				{
					t_HWidget sk = wnd_get_skbar(handle);
					//t_HWidget menu;
				
					//menu = wnd_get_widget(handle, RESID_MENU_OPTION);
					//menu_bind(menu, miContact, list);
					//menu_SetItemPopup(menu, RESID_STR_OPERATION, IDENT_BY_ID, contact_ms_listctrl_new(glob, handle));
					//g_object_unref(list);
					if(g_object_valid(sk)){
						sk_set_text(sk, SK_LSK, NULL, RESID_STR_NEW_SMS);
						sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
						sk_set_callback(sk, SK_LSK, LskNewCallback, handle);
						//list = sms_ms_listctrl_new(glob, handle);
						//sk_set_popup(sk, SK_LSK, list);
						//g_object_unref(list);
					}
				}
				ret = SUCCESS;
			}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			ret = SUCCESS;
			break;
	}
	return ret;
}

