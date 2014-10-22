#include "pack_sms.hi"
#include "wgt_button.h"


void	__display_sms_content(t_HDataSet hds, t_HGDI gdi, coord_t x, coord_t y, gu16 width, gu16 height, gu32 flags)
{
	gu8 dcs, *data, type;
	gu16 size;
	
	type = g_ds_GetValue(hds, SMS_DBFIELD_TYPE);
	dcs = g_ds_GetValue(hds, SMS_DBFIELD_DCS);
	data = g_ds_GetBinary(hds, SMS_DBFIELD_DATA, &size);
	
	gdi_set_color(gdi, RGB_GRAY);
	if(dcs == DCS_7BIT){
		gdi_draw_text_in(gdi, x, y, width, height, (const char *)data, size, CHARSET_US_ASCII, flags);
	}else if(dcs == DCS_UCS2){
		gdi_draw_utext_in(gdi, x, y, width, height, (const ucs2_t *)data, size>>1, flags);
	}else{
		gdi_draw_text_in(gdi, x, y, width, height, "<unknown>", -1, CHARSET_US_ASCII, flags);
	}
}


#if !defined(FTR_COMM_SESSION_SUPPORT) || defined(SMALL_LCD)

static void	CmdViewContact(t_HMenu handle, void *data)
{
	t_HWindow win = data;
	sms_glob_t *glob = task_get_data();

	char *number = g_ds_GetString(glob->ds, SMS_DBFIELD_NUMBER);
	t_CSessionID sid = g_ds_GetValue(glob->ds, SMS_DBFIELD_SID);
	dlg_contact_op_show(PACK_GUID_SELF, sid, number);
}

static void	CmdViewCopyText(t_HMenu handle, void *data)
{
	t_HWindow win = data;
}
static void	CmdViewReply(t_HMenu handle, void *data)
{
	t_HWindow win = data;
	sms_glob_t *glob = task_get_data();
	char *number;

	//g_ds_GotoByIndex(glob->ds, glob->view_index);
	number = g_ds_GetString(glob->ds, SMS_DBFIELD_NUMBER);
	wnd_new_message(WITH_TO, number);
}

static void	CmdViewForward(t_HMenu handle, void *data)
{
	t_HWindow win = data;
	sms_glob_t *glob = task_get_data();

	gu8 dcs, *out, *cont;
	gu16 size;

	//g_ds_GotoByIndex(glob->ds, glob->view_index);
	dcs = g_ds_GetValue(glob->ds, SMS_DBFIELD_DCS);
	cont = g_ds_GetBinary(glob->ds, SMS_DBFIELD_DATA, &size);
	if(dcs == DCS_UCS2){
		int tmp = ustring_to_utf8((const ucs2_t *)cont, size>>1, NULL, -1);
		out = MALLOC(tmp+1);
		ustring_to_utf8((const ucs2_t *)cont, size>>1, out, -1);
		wnd_new_message(WITH_CONTENT, out);
		FREE(out);
	}else{
		wnd_new_message(WITH_CONTENT, cont);
	}
}

static void	CmdViewDelete(t_HMenu handle, void *data)
{
	t_HWindow win = data;
	sms_glob_t *glob = task_get_data();
	
	//g_ds_GotoByIndex(glob->ds, glob->view_index);
	g_ds_Delete(glob->ds);
	wnd_send_close(win, 0);
}

static t_MenuItem miSmsViewOptions[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_OPEN_CONTACT, CmdViewContact),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_COPY, CmdViewCopyText),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_REPLY, CmdViewReply),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_FORWARD, CmdViewForward),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_DELETE, CmdViewDelete),
	MITEM_END
};



#define X_OFF	5
#define Y_LINE	16
static void		__show_sms(t_HWindow handle, gboolean defer)
{
	sms_glob_t *glob = app_get_data();
	t_HGDI gdi = wgt_get_client_gdi((t_HWidget)handle);
	gui_rect_t client;
	coord_t x, y;
	datetime_t dt;
	char msg[40];
	t_CSessionID sid;
	t_ContactInfo *cinfo;
	char *sender;
	
	gu8 dcs, *data, type;
	gu16 size;
	
	if(!g_object_valid(gdi))
		return;
	wgt_get_client_rect((t_HWidget)handle, &client);
	if(!defer)
		gdi_clear(gdi, 0, 0, client.dx, client.dy, 0);

	gdi_set_color(gdi, ui_theme_get()->fg_color);
	//2 display <Sender>
	x = X_OFF;
	y = 0;
	sid = g_ds_GetValue(glob->ds, SMS_DBFIELD_SID);
	cinfo = contact_get_info(sid);
	if(cinfo){
		sender = cinfo->name;
	}else{
		sender = g_ds_GetString(glob->ds, SMS_DBFIELD_NUMBER);
	}
	gdi_draw_text_in(gdi, x, y, client.dx-x, Y_LINE, sender, -1, CHARSET_UNDEF, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
	//2 display <date time>
	y += Y_LINE;
	dostime_to_datetime(g_ds_GetValue(glob->ds, SMS_DBFIELD_TIME), &dt);
	sprintf(msg, "%04d-%02d-%02d %02d:%02d, SIM%d", dt.dt_year, dt.dt_month, dt.dt_day, dt.dt_hour, dt.dt_minute, 
			g_ds_GetValue(glob->ds, SMS_DBFIELD_SIM)+1);
	gdi_draw_text_in(gdi, x, y, client.dx-x, Y_LINE, msg, -1, CHARSET_US_ASCII, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
	//2 display ------------------------------------
	y += Y_LINE;
	{
		t_ColorRasterOp rop;
		rop = gdi_set_rop(gdi, IMG_RO_NOT);
		gdi_hline(gdi, y, x, client.dx-x);
		gdi_set_rop(gdi, rop);
	}
	//2 display content
	type = g_ds_GetValue(glob->ds, SMS_DBFIELD_TYPE);
	dcs = g_ds_GetValue(glob->ds, SMS_DBFIELD_DCS);
	data = g_ds_GetBinary(glob->ds, SMS_DBFIELD_DATA, &size);
	//gdi_set_color(gdi, RGB_BLACK);
	if(dcs == DCS_7BIT){
		gdi_draw_text_in(gdi, x, y, client.dx-X_OFF*2, 0x7000, (const char *)data, size, CHARSET_US_ASCII, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE|IDF_TEXT_MULTILINE);
	}else if(dcs == DCS_UCS2){
		gdi_draw_utext_in(gdi, x, y, client.dx-X_OFF*2, 0x7000, (const ucs2_t *)data, size>>1, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE|IDF_TEXT_MULTILINE);
	}else{
		gdi_draw_text_in(gdi, x, y, client.dx-X_OFF*2, 0x7000, "<unknown>", -1, CHARSET_US_ASCII, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE|IDF_TEXT_MULTILINE);
	}


	contact_free_info(cinfo);


	if(!defer)
		gdi_blt(gdi, 0, 0, client.dx, client.dy);
	g_object_unref(gdi);
}

void	__load_sms(sms_glob_t *glob, t_HWindow handle, int act, gboolean redraw)
{
	int index;

	gu8 dcs, *data;
	gu16 size;
	int ww, hh;
	gui_rect_t client;

	index = glob->view_index+act;
	if(index < 0 || index >= g_ds_GetRecordNum(glob->ds))
		return;
	glob->view_index = index;

	wgt_get_client_rect((t_HWidget)handle, &client);
	
	g_ds_GotoByIndex(glob->ds, glob->view_index);

	//2 display bound & content
	dcs = g_ds_GetValue(glob->ds, SMS_DBFIELD_DCS);
	data = g_ds_GetBinary(glob->ds, SMS_DBFIELD_DATA, &size);

	if(dcs == DCS_7BIT){
		hh = font_measure_string_multiline((const char *)data, size, CHARSET_US_ASCII, client.dx-X_OFF*2, &ww, NULL);
	}else if(dcs == DCS_UCS2){
		hh = font_measure_ustring_multiline((const ucs2_t *)data, size>>1, client.dx-X_OFF*2, &ww, NULL);
	}else{
		hh = font_measure_string_multiline("<unknown>", -1, CHARSET_US_ASCII, client.dx-X_OFF*2, &ww, NULL);
	}
	
	wgt_client_origin_x(handle) = wgt_client_origin_y(handle) = 0;
	wgt_client_drawing_dy(handle) = Y_LINE*2+hh;
	
	if(redraw)
		__show_sms(handle, FALSE);
}

error_t	sms_ViewWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	sms_glob_t *glob = task_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
			{
				wgt_enable_attr(handle, WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_STATUS|WND_ATTR_HAS_SK|CTRL_ATTR_VSCROLLABLE);

				glob->view_index = sparam;
				__load_sms(glob, handle, 0, FALSE);
				{
					t_HWidget sk = wnd_get_skbar(handle);
					t_HWidget menu;

					//2 menu popup for LISTCTRL
					menu = wgt_create((t_HWidget)handle, CLSID_MENU);
					wgt_set_resid(menu, RESID_MENU_POPUP);
					wgt_disable_attr(menu, CTRL_ATTR_VISIBLE);
					//listctrl_set_rows(menu, 1, 20, 0);
					//listctrl_set_cols(menu, 0, 0);
					listctrl_set_viewtype(menu, GUI_VIEWTYPE_NORMAL);
					listctrl_set_flags(menu, listctrl_get_flags(menu)|CLFLAG_SCROLLING_VERTICAL);
					menu_bind(menu, miSmsViewOptions, handle);
					
					sk_set_text(sk, SK_LSK, NULL, RESID_STR_OPTION);
					sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
					sk_set_popup(sk, SK_LSK, menu);
				}
				ret = SUCCESS;
			}
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
				if(lparam == MVK_LEFT){
					__load_sms(glob, handle, -1, TRUE);
					ret = SUCCESS;
				}else if(lparam == MVK_RIGHT){
					__load_sms(glob, handle, 1, TRUE);
					ret = SUCCESS;
				}
			}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			__show_sms(handle, TRUE);
			ret = SUCCESS;
			break;
	}
	return ret;
}

//2 如果定义了FTR_COMM_SESSION_SUPPORT表示是小屏session列表，只需在title显示contact，
//2 否则须在每条记录都显示contact
void	db_redraw_record(t_HDataSet hdb, int index, t_HGDI gdi, t_GuiViewType viewtype, int width, int height, gboolean hilight)
{
	const char *number;
	char name[40];
	datetime_t curr_dt;
	dos_datetime_t dt;
	char dt_string[14];
	int  tw, th;

	g_ds_GotoByIndex(hdb, index);
	number = g_ds_GetString(hdb, SMS_DBFIELD_NUMBER);
	contact_query_by_number(number, name, sizeof(name));
	dt = g_ds_GetValue(hdb, SMS_DBFIELD_TIME);
	g_printf("[%d] number=%s", index, number);

	if(name[0] == 0)
		strcpy(name, number);
	{
		coord_t x=1;
		width -= 1;
		if(VIEWTYPE_IS_NORMAL(viewtype)){

#if defined(FTR_SMS_VIEW_2LINE)
#if !defined(FTR_COMM_SESSION_SUPPORT)
			gdi_draw_text_in(gdi, x, 0, width, height/2, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
#endif
			sprintf(dt_string, "%02d-%02d %02d:%02d", DOSDATE_M(dt>>16), DOSDATE_D(dt>>16), DOSTIME_H(dt), DOSTIME_M(dt));
			gdi_draw_text_in(gdi, x, 0, width, height/2, dt_string, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
			__display_sms_content(hdb, gdi, x, height/2, width, height/2, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
#else
			datetime_current(&curr_dt);
			if(curr_dt.dt_year == DOSDATE_Y(dt>>16) && curr_dt.dt_month == DOSDATE_M(dt>>16) && curr_dt.dt_day == DOSDATE_D(dt>>16))
				sprintf(dt_string, "%02d:%02d", DOSTIME_H(dt), DOSTIME_M(dt));
			else
				sprintf(dt_string, "%02d-%02d", DOSDATE_M(dt>>16), DOSDATE_D(dt>>16));
			gdi_measure_text(gdi, dt_string, 5, CHARSET_US_ASCII, -1, &tw, &th, NULL);
			gdi_draw_text_at(gdi, x+width-tw, (height-th)>>1, dt_string, -1, CHARSET_US_ASCII);
			width -= tw;
			gdi_draw_text_in(gdi, x, 0, width, height, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
#endif
		}
	}
}

void	db_view_record(t_HDataSet hds, int index)
{
	g_ds_GotoByIndex(hds, index);
	wnd_show(wnd_create(sms_ViewWndMsgHandler, index, hds));
}

#endif


#if defined(FTR_COMM_SESSION_SUPPORT)

#define	SMS_CONTENT_MAX_BYTES		600
typedef struct {
	t_HDataSet	hds_cs;
	t_DBRecordID	cs_rid;
	MC_FILL_STRUCT2;
	t_CSessionID cid;
	char * number;
	t_ContactInfo *cinfo;
	char 	*title;
	char msg[SMS_CONTENT_MAX_BYTES];
}t_SmsSessionWData;


static void	__ds_notify(t_HDataSet hds, t_DBRecordID rid, void *userdata)
{
	t_HWindow handle = (t_HWindow)userdata;
	t_SmsSessionWData *wdata = (t_SmsSessionWData*)wgt_get_tag(handle);
	gu16 rec_num;

#if defined(SMALL_LCD)
	g_ds_OrderBy(hds, SMS_DBFIELD_TIME, FALSE, FALSE);
#else
	g_ds_OrderBy(hds, SMS_DBFIELD_TIME, TRUE, FALSE);
#endif
	g_ds_CondSet1(hds, SMS_DBFIELD_SID, DBREL_EQ, wdata->cid);
	g_ds_Query(hds, TRUE);

	rec_num = g_ds_GetRecordNum(hds);
#if !defined(SMALL_LCD)
	if(rec_num)
#endif
	{
		t_HList list = wnd_get_widget(handle, RESID_LIST_VIEW);
		int index;
		listctrl_refresh_datasource(list, FALSE);
		
		if(rid != DB_RID_NULL){
			index = g_ds_RecordIndex(hds, rid);
		}else{
			index = rec_num-1;
		}
#if !defined(SMALL_LCD)
		index = index*2+1;
#endif
		listctrl_set_cursor(list, index, FALSE);
		if(rid != DB_RID_NULL)
			wgt_redraw(list, FALSE);
	}
	if(rec_num == 0){
		wnd_send_close(handle, 0);
	}
}


#if defined(SMALL_LCD)

static void	CmdReply(t_HMenu handle, void *data)
{
	t_HWindow win = data;
	t_HList	list = wnd_get_widget(win, RESID_LIST_VIEW);
	sms_glob_t *glob = task_get_data();
	t_SmsSessionWData *wdata = (t_SmsSessionWData*)wgt_get_tag(win);
	char *number;

	g_ds_GotoByIndex(glob->ds, listctrl_get_cursor(list));
	number = g_ds_GetString(glob->ds, SMS_DBFIELD_NUMBER);
	wnd_new_message(WITH_TO, number);
}
static void	CmdContact(t_HMenu handle, void *data)
{
	t_HWindow win = data;
	sms_glob_t *glob = task_get_data();
	t_SmsSessionWData *wdata = (t_SmsSessionWData*)wgt_get_tag(win);

	dlg_contact_op_show(PACK_GUID_SELF, wdata->cid, wdata->number);
}

static void	CmdDelete(t_HMenu handle, void *data)
{
	t_HWindow win = data;
	sms_glob_t *glob = task_get_data();
	t_SmsSessionWData *wdata = (t_SmsSessionWData*)wgt_get_tag(win);
	t_HList	list = wnd_get_widget(win, RESID_LIST_VIEW);
	
	if(listctrl_get_count(list)){
		g_ds_GotoByIndex(glob->ds, listctrl_get_cursor(list));
		g_ds_Delete(glob->ds);

		//2 因为sms删除底层会通知，从而__ds_notify被调用
		//wgt_enable_attr(list, CTRL_ATTR_MODIFIED);
		//wgt_redraw(list, FALSE);
	}
}

static t_MenuItem miSmsSessionOptions[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_OPEN_CONTACT, CmdContact),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_REPLY, CmdReply),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_DELETE, CmdDelete),
	MITEM_END
};



//3 sparam=<cs_rid>, lparam=<hdb_cs>

#define	LIST_BTN_HEIGHT	20
error_t	sms_ViewSessionWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	sms_glob_t *glob = task_get_data();
	t_SmsSessionWData *wdata = (t_SmsSessionWData*)wgt_get_tag(handle);
	t_MimeMethodInfo info;
	t_HWidget widget;
	char title[40];
	switch(evt){
		case WINDOW_OnOpen:
		{
			gui_rect_t client;
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);

			wdata = MALLOC(sizeof(t_SmsSessionWData));
			memset(wdata, 0, sizeof(t_SmsSessionWData));
			wdata->hds_cs = (t_HDataSet)lparam;
			wdata->cs_rid = sparam;
			if(g_object_valid(wdata->hds_cs)){
				g_object_ref(wdata->hds_cs);
			}else{
				wdata->hds_cs = comm_session_db_open();
			}
			g_ds_Goto(wdata->hds_cs, wdata->cs_rid);
			wdata->cid = g_ds_GetValue(glob->hds_cs, CSESSION_DBFIELD_SID);
			//2 之所以不能直接保存指针，因为外部对CommSessionDB的改变会触发notify，不能确保此前保存的指针现在指向何处
			wdata->number = g_str_dup(g_ds_GetString(wdata->hds_cs, CSESSION_DBFIELD_NUMBER));
			wdata->cinfo = contact_get_info(wdata->cid);
			wgt_set_tag(handle, wdata);
			
			//2 Window Title
			if(wdata->cinfo){
				wdata->title = g_str_dup(wdata->cinfo->name);
			}else{
				wdata->title = g_str_dup(wdata->number);
			}
			wgt_set_caption(handle, wdata->title);

			wgt_get_client_rect((t_HWidget)handle, &client);

			//3 LISTCTRL for SMS
			widget = wgt_create((t_HWidget)handle, CLSID_LISTCTRL);
			wgt_set_resid(widget, RESID_LIST_VIEW);
			wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiTop);
			wgt_set_bound(widget, 0, 0, client.dx, client.dy);
			wgt_set_bg_fill_solid(widget, 0);
			listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_HILIGHT_SHOW_CUSTOM|CLFLAG_ENABLE_ITEM_FSHOW|CLFLAG_AUTOTAB_LR|CLFLAG_CURSOR_NODRAW_ONKILLFOCUS);
			listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
			listctrl_set_rows(widget, 0, prod_ui_list_item_height()*2, 0);
			listctrl_set_cols(widget, 1, 0);
			listctrl_set_item_margin(widget, LINE_X_MARGIN, LINE_X_MARGIN, LINE_Y_SPLIT, LINE_Y_SPLIT);
			//wgt_hook_register(widget, WIDGET_OnDraw|WIDGET_OnKeyEvent, PanelHooker);
			g_ds_register_notify(glob->ds, __ds_notify, handle);
			__ds_notify(glob->ds, DB_RID_NULL, handle);
			
			g_ds_bind(glob->ds, db_redraw_record, db_view_record);
			listctrl_attach_db(widget, glob->ds);
			
			//2 menu popup for LISTCTRL
			widget = wgt_create((t_HWidget)handle, CLSID_MENU);
			wgt_set_resid(widget, RESID_MENU_POPUP);
			wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
			//listctrl_set_rows(widget, 1, 20, 0);
			//listctrl_set_cols(widget, 0, 0);
			listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
			listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_DISABLE_HLINE|CLFLAG_SCROLLING_VERTICAL);
			menu_bind(widget, miSmsSessionOptions, handle);

			{
				t_HWidget sk = wnd_get_skbar(handle);
				if(g_object_valid(sk)){
					sk_set_text(sk, SK_LSK, NULL, RESID_STR_OPTION);
					sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
					sk_set_popup(sk, SK_LSK, widget);
				}
			}
			ret = SUCCESS;
		}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			if(wdata){
				FREEIF(wdata->title);
				FREEIF(wdata->number);
				g_object_unref(wdata->hds_cs);
				contact_free_info(wdata->cinfo);
				FREE(wdata);
			}
			g_ds_unregister_notify(glob->ds);
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			if(IS_UNSAVED_CONTACT(wdata->cid)){
				char name[40];
				t_CSessionID cid;
				//2 之所以要重新Goto一下, 是因为hds_cs数据库有变更, 记录被重新刷新过
				g_ds_Goto(wdata->hds_cs, wdata->cs_rid);
				cid = contact_query_by_number(wdata->number, name, sizeof(name));
				if(cid != UNKNOWN_CONTACT && wdata->cid != cid){
					wdata->cid = cid;
					contact_free_info(wdata->cinfo);
					wdata->cinfo = contact_get_info(wdata->cid);
					wgt_set_caption(handle, name);
				}
				ret = SUCCESS;
			}
			break;
		case WIDGET_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(lparam == MVK_SEND && sparam == MMI_KEY_PRESS)
				;//pack_call("call", MOD_ID_MAIN, (gu8*)glob->items[glob->cursor].CellNo, strlen(glob->items[glob->cursor].CellNo)+1, TRUE, NULL);
	}
	return ret;
}

#else
static void	CmdCopyText(t_HMenu handle, void *data)
{
	t_HWindow win = data;
}
static void	CmdResend(t_HMenu handle, void *data)
{
	t_HWindow win = data;
	sms_glob_t *glob = task_get_data();
	t_SmsSessionWData *wdata = (t_SmsSessionWData*)wgt_get_tag(win);

	t_HList	list = wnd_get_widget(win, RESID_LIST_VIEW);
	g_ds_GotoByIndex(glob->ds, listctrl_get_cursor(list));

	sms_resend(g_ds_CurrentID(glob->ds), FALSE, NULL, win);
}
static void	CmdMore(t_HMenu handle, void *data)
{
	t_HWindow win = data;
}

static t_MenuItem miSmsSessionOptions[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_COPY, CmdCopyText),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_RESEND, CmdResend), //2 for MO sms
	//MITEM_NORMAL(RESID_UNDEF, RESID_STR_MORE, CmdMore),
	MITEM_END
};

static void	_SmsMoCallback(gu8 state, t_CSessionID cid, const char *number, void *userdata)
{
	t_HWindow handle = userdata;
	t_SmsSessionWData *wdata = (t_SmsSessionWData*)wgt_get_tag(handle);
	t_HWidget widget = wnd_get_skbar(handle);
	switch(state){
		case SMS_MO_STATE_BEGIN:
			wgt_set_caption((t_HWidget)handle, "TX begin......");
			wgt_redraw(widget, FALSE);
			break;
		case SMS_MO_STATE_SENDING:
			wgt_set_caption((t_HWidget)handle, "state tx: cid=0x%x, number=%s", cid, number);
			wgt_redraw(widget, FALSE);
			break;
		case SMS_MO_STATE_SENT_OK:
			wgt_set_caption((t_HWidget)handle, "TX OK: cid=0x%x, number=%s", cid, number);
			wgt_redraw(widget, FALSE);
			break;
		case SMS_MO_STATE_SENT_KO:
			wgt_set_caption((t_HWidget)handle, "TX ERR: cid=0x%x, number=%s", cid, number);
			wgt_redraw(widget, FALSE);
			break;
		case SMS_MO_STATE_END:
			wgt_set_caption((t_HWidget)handle, "TX end......");
			wgt_redraw(widget, FALSE);
			amos_sleep(1000);
			wgt_set_caption((t_HWidget)handle, wdata->title);
			wgt_redraw(widget, FALSE);
			break;
		case SMS_MO_STATE_ERROR:
			wgt_set_caption((t_HWidget)handle, "TX error end......");
			wgt_redraw(widget, FALSE);
			amos_sleep(1000);
			wgt_set_caption((t_HWidget)handle, wdata->title);
			wgt_redraw(widget, FALSE);
			break;
	}
}

enum {
	BY_CONTACT,
	BY_SMS,

	BY_COUNT,
};
error_t	__list_btn_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	sms_glob_t *glob = task_get_data();
	t_HWindow win = wgt_parent_window(handle);
	t_SmsSessionWData *wdata = (t_SmsSessionWData*)wgt_get_tag(win);
	switch(DSIconText_GetItemID(listctrl_get_datasource(handle), sparam)){
		case BY_CONTACT:
			dlg_contact_op_show(PACK_GUID_SELF, wdata->cid, wdata->number);
			ret = SUCCESS;
			break;
		case BY_SMS:
			if(wgt_get_resid(win) == RESID_WIN_MAIN){
				wnd_new_message(WITH_NONE, NULL);
			}
			else{
				BrdHandle brd = brd_get(PACK_GUID_SELF);
				int len;
				wdata->msg[0] = 0;
				len = dlg_editor_show(wdata->msg, SMS_CONTENT_MAX_BYTES, 
					brd_get_string(brd, RESID_STR_NEW_SMS, NULL), 
					brd_get_string(brd, RESID_STR_SEND, NULL), IDF_TEXT_MULTILINE|IDF_TEXT_AUTO_WRAP);
				if(len){
					char *number;
					g_ds_Goto(wdata->hds_cs, wdata->cs_rid);
					number = g_ds_GetString(wdata->hds_cs, SMS_DBFIELD_NUMBER);
					sms_send(wdata->msg, len, DCS_7BIT, number, 0, FALSE, _SmsMoCallback, win, TRUE);
				}
			}
			break;
	}
	return ret;
}

static void	ViewLskCallback(t_HWidget sk, void *data)
{
	t_HWindow handle = (t_HWindow)data;
	//2 浏览界面的LSK用做"编辑"
	
		sms_glob_t *glob = task_get_data();
		t_SmsSessionWData *wdata = (t_SmsSessionWData*)wgt_get_tag(handle);
		wnd_show(wnd_create(sms_ViewSessionDelWndMsgHandler, wdata->cid, wdata->hds_cs));
}

//3 sparam=<cs_rid>, lparam=<hdb_cs>

#define	LIST_BTN_HEIGHT	20
error_t	sms_ViewSessionWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	sms_glob_t *glob = task_get_data();
	t_SmsSessionWData *wdata = (t_SmsSessionWData*)wgt_get_tag(handle);
	t_HDataSource ds;
	t_MimeMethodInfo info;
	t_HWidget widget;
	char title[40];
	switch(evt){
		case WINDOW_OnOpen:
		{
			gui_rect_t client;
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			//wgt_set_bg_fill_solid(handle, MAKE_RGB(192,192,255));

			wdata = MALLOC(sizeof(t_SmsSessionWData));
			memset(wdata, 0, sizeof(t_SmsSessionWData));
			wdata->hds_cs = (t_HDataSet)lparam;
			wdata->cs_rid = sparam;
			if(g_object_valid(wdata->hds_cs)){
				g_object_ref(wdata->hds_cs);
			}else{
				wdata->hds_cs = comm_session_db_open();
			}
			g_ds_Goto(wdata->hds_cs, wdata->cs_rid);
			wdata->cid = g_ds_GetValue(glob->hds_cs, CSESSION_DBFIELD_SID);
			//2 之所以不能直接保存指针，因为外部对CommSessionDB的改变会触发notify，不能确保此前保存的指针现在指向何处
			wdata->number = g_str_dup(g_ds_GetString(wdata->hds_cs, CSESSION_DBFIELD_NUMBER));
			wdata->cinfo = contact_get_info(wdata->cid);
			wgt_set_tag(handle, wdata);
			
			//2 Window Title
			if(wdata->cinfo){
				wdata->title = g_str_dup(wdata->cinfo->name);
			}else{
				wdata->title = g_str_dup(wdata->number);
			}
			wgt_set_caption(handle, wdata->title);

			wgt_get_client_rect((t_HWidget)handle, &client);

			{
				g_idx_t idx;


				//2 LISTCTRL--------Action: Call&Calllog&Contact
				widget = wgt_create((t_HWidget)handle, CLSID_LISTCTRL);
				wgt_set_resid(widget, RESID_LIST_BUTTON);
				wgt_set_bound(widget, 0, 0, client.dx, LIST_BTN_HEIGHT);
				wgt_hook_register(widget, WIDGET_OnSelected, __list_btn_hook, NULL);
				wgt_set_bg_fill_solid(widget, 0);
				listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_DISABLE_HLINE|CLFLAG_AUTOTAB_UD|CLFLAG_CURSOR_NODRAW_ONKILLFOCUS);
				listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
				listctrl_set_rows(widget, 1, 0, 0);
				listctrl_set_cols(widget, BY_COUNT, 0);
				
				ds = listctrl_get_datasource(widget);
				DSIconText_SetAlign(ds, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
				//4 1. -------------------Contact
				idx = DSIconText_NewItem(ds);
				DSIconText_SetItemID(ds, idx, BY_CONTACT);
				DSIconText_SetItemText(ds, idx, IDENT_BY_POSITION, NULL, brd_get(PACK_GUID_SELF), RESID_STR_OPEN_CONTACT);
				//4 2.  -------------------New SMS
				idx = DSIconText_NewItem(ds);
				DSIconText_SetItemID(ds, idx, BY_SMS);
				DSIconText_SetItemText(ds, idx, IDENT_BY_POSITION, NULL, brd_get(PACK_GUID_SELF), RESID_STR_NEW_SMS);
			}

			//3 LISTCTRL for SMS
			widget = wgt_create((t_HWidget)handle, CLSID_LISTCTRL);
			wgt_set_resid(widget, RESID_LIST_VIEW);
			wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiTop);
			wgt_set_bound(widget, 0, LIST_BTN_HEIGHT, client.dx, client.dy-LIST_BTN_HEIGHT);
			wgt_set_bg_fill_solid(widget, 0);
			ds = ds_sms_new(glob->ds);
			listctrl_attach_datasource(widget, ds);
			g_object_unref(ds);
			listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_DISABLE_HLINE|CLFLAG_HILIGHT_SHOW_CUSTOM|CLFLAG_ENABLE_ITEM_FSHOW|CLFLAG_AUTOTAB_LR|CLFLAG_AUTOTAB_UD|CLFLAG_CURSOR_NODRAW_ONKILLFOCUS);
			listctrl_set_viewtype(widget, GUI_VIEWTYPE_CUSTOM);
			listctrl_set_rows(widget, 0, 0, 0);
			listctrl_set_cols(widget, 1, 0);
			listctrl_set_item_margin(widget, LINE_X_MARGIN, LINE_X_MARGIN, LINE_Y_SPLIT, LINE_Y_SPLIT);
			//wgt_hook_register(widget, WIDGET_OnDraw|WIDGET_OnKeyEvent, PanelHooker);
			g_ds_register_notify(glob->ds, __ds_notify, handle);
			__ds_notify(glob->ds, DB_RID_NULL, handle);
			//2 menu popup for LISTCTRL
			widget = wgt_create((t_HWidget)handle, CLSID_MENU);
			wgt_set_resid(widget, RESID_MENU_POPUP);
			wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
			listctrl_set_rows(widget, 1, 20, 0);
			listctrl_set_cols(widget, 0, 0);
			listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
			listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_DISABLE_HLINE|CLFLAG_SCROLLING_HORIZONTAL);
			menu_bind(widget, miSmsSessionOptions, handle);

			{
				t_HWidget sk = wnd_get_skbar(handle);
				if(g_object_valid(sk)){
					sk_set_text(sk, SK_LSK, NULL, RESID_STR_EDIT);
					sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
					sk_set_callback(sk, SK_LSK, ViewLskCallback, handle);
				}
			}
			ret = SUCCESS;
		}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			if(wdata){
				FREEIF(wdata->title);
				FREEIF(wdata->number);
				g_object_unref(wdata->hds_cs);
				contact_free_info(wdata->cinfo);
				FREE(wdata);
			}
			g_ds_unregister_notify(glob->ds);
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			if(IS_UNSAVED_CONTACT(wdata->cid)){
				char name[40];
				t_CSessionID cid;
				//2 之所以要重新Goto一下, 是因为hds_cs数据库有变更, 记录被重新刷新过
				g_ds_Goto(wdata->hds_cs, wdata->cs_rid);
				cid = contact_query_by_number(wdata->number, name, sizeof(name));
				if(cid != UNKNOWN_CONTACT && wdata->cid != cid){
					wdata->cid = cid;
					contact_free_info(wdata->cinfo);
					wdata->cinfo = contact_get_info(wdata->cid);
					wgt_set_caption(handle, name);
				}
				ret = SUCCESS;
			}
			break;
		case WIDGET_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(lparam == MVK_SEND && sparam == MMI_KEY_PRESS)
				;//pack_call("call", MOD_ID_MAIN, (gu8*)glob->items[glob->cursor].CellNo, strlen(glob->items[glob->cursor].CellNo)+1, TRUE, NULL);
	}
	return ret;
}


//1 =================================================

enum {
	RESID_LIST_MAIN = RESID_CTRL_DYNAMIC,
	RESID_BTN_DEL,
	RESID_BTN_FWD,
};
typedef struct {
	t_HDataSet	hds_cs;
	t_DBRecordID	cs_rid;
	MC_FILL_STRUCT2;
	t_CSessionID cid;
	t_ContactInfo *cinfo;
	char 	*title;
}t_SmsSessionDelWData;

static void	ViewDelSKCallback(t_HWidget sk, void *data)
{
	t_HWindow handle = (t_HWindow)wgt_parent_window(sk);
	int kk = (int)data;
	if(kk == SK_LSK){
		//2 Cancel
		wnd_send_close(handle, 0);
	}else if(kk == SK_RSK){
		//2 Delete/Delete All
		if(msgbox_show("", _LSK(YES)|_RSK(CANCEL), 0, 0, brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_DELALL, "")) == MBB_YES){
			sms_glob_t *glob = task_get_data();
			t_SmsSessionDelWData *wdata = (t_SmsSessionDelWData *)wgt_get_tag(handle);
			

			g_ds_First(glob->ds);
			while(!g_ds_Empty(glob->ds))
				g_ds_Delete(glob->ds);
			//2 因为sms删除底层会通知，从而__ds_notify被调用
			//t_HList list = wnd_get_widget(win, RESID_LIST_VIEW);
			//wgt_enable_attr(list, CTRL_ATTR_MODIFIED);
			//wgt_redraw(list, FALSE);
			wnd_send_close(handle, 0);
		}
	}
}

static error_t	list_delete_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	sms_glob_t *glob = task_get_data();
	t_HWindow win = wgt_parent_window(handle);
	t_HWidget button;
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnSelected:
			{
				int num = listctrl_get_num_select(handle);
				button = wnd_get_widget(win, RESID_BTN_DEL);
				if(num > 0){
					wgt_set_caption(button, "%s  (%d)", brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_DELETE, ""), num);
					wgt_enable_attr(button, CTRL_ATTR_ENABLED);
				}else{
					wgt_set_caption_by_resid(button, brd_get(PACK_GUID_SELF), RESID_STR_DELETE);
					wgt_disable_attr(button, CTRL_ATTR_ENABLED);
				}
				wgt_redraw(button, FALSE);
				
				button = wnd_get_widget(win, RESID_BTN_FWD);
				if(num > 0){
					wgt_set_caption(button, "%s  (%d)", brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_FORWARD, ""), num);
					wgt_enable_attr(button, CTRL_ATTR_ENABLED);
				}else{
					wgt_set_caption_by_resid(button, brd_get(PACK_GUID_SELF), RESID_STR_FORWARD);
					wgt_disable_attr(button, CTRL_ATTR_ENABLED);
				}
				wgt_redraw(button, FALSE);
			}
			ret = SUCCESS;
	}
	return ret;
}

static error_t	btn_delete_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	sms_glob_t *glob = task_get_data();
	t_HWindow win = wgt_parent_window(handle);
	t_HWidget listctrl;
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnSelected:
			{
				listctrl = wnd_get_widget(win, RESID_LIST_MAIN);
				if(msgbox_show(NULL, _LSK(YES)|_RSK(CANCEL), 0, 0, brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_PROMPT_DEL, "")) == MBB_YES){
					//2 delete selected sms: 不能根据索引来删除，必须根据sms记录号，因为这是不变的
					ds_sms_delete(listctrl_get_datasource(listctrl));
					wnd_send_close(win, 0);
				}else{
				}
			}
			ret = SUCCESS;
	}
	return ret;
}

static error_t	btn_forward_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	sms_glob_t *glob = task_get_data();
	t_HWindow win = wgt_parent_window(handle);
	t_HWidget listctrl;
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnSelected:
			{
				listctrl = wnd_get_widget(win, RESID_LIST_MAIN);
				ds_sms_forward(listctrl_get_datasource(listctrl));
				wnd_send_close(win, 0);
			}
			ret = SUCCESS;
	}
	return ret;
}

//3 sparam=<cs_rid>, lparam=<hdb_cs>
error_t	sms_ViewSessionDelWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	sms_glob_t *glob = task_get_data();
	t_SmsSessionDelWData *wdata = (t_SmsSessionDelWData*)wgt_get_tag(handle);
	t_HDataSource ds;
	t_MimeMethodInfo info;
	t_HWidget widget;
	char title[40];
	switch(evt){
		case WINDOW_OnOpen:
		{
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			wgt_set_bg_fill_solid(handle, MAKE_RGB(192,192,255));

			wdata = MALLOC(sizeof(t_SmsSessionDelWData));
			memset(wdata, 0, sizeof(t_SmsSessionDelWData));
			wdata->hds_cs = (t_HDataSet)lparam;
			wdata->cs_rid = sparam;
			g_object_ref(wdata->hds_cs);
			g_ds_Goto(wdata->hds_cs, wdata->cs_rid);
			wdata->cid = g_ds_GetValue(glob->hds_cs, CSESSION_DBFIELD_SID);
			wdata->cinfo = contact_get_info(wdata->cid);
			wgt_set_tag(handle, wdata);
			
			//2 Window Title
			if(wdata->cinfo){
				wdata->title = g_str_dup(wdata->cinfo->name);
			}else{
				char *number = g_ds_GetString(wdata->hds_cs, SMS_DBFIELD_NUMBER);
				wdata->title = g_str_dup(number);
			}
			wgt_set_caption(handle, wdata->title);
			
			{
				#define	BOTTOM		40
				#define	BTN_HEIGHT		30
				#define	MARGIN_X	10
				
				gui_rect_t client;
				wgt_get_client_rect((t_HWidget)handle, &client);
				
				//3 LISTCTRL for SMS
				widget = wgt_create((t_HWidget)handle, CLSID_LISTCTRL);
				wgt_set_resid(widget, RESID_LIST_MAIN);
				wgt_set_bound(widget, 0, 0, client.dx, client.dy-BOTTOM);
				wgt_set_bg_fill_solid(widget, 0);
				ds = ds_sms_new(glob->ds);
				listctrl_attach_datasource(widget, ds);
				g_object_unref(ds);
				listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_HILIGHT_SHOW_CUSTOM|CLFLAG_AUTOTAB_UD|CLFLAG_AUTOTAB_LR);
				listctrl_set_viewtype(widget, GUI_VIEWTYPE_CUSTOM);
				listctrl_set_style(widget, LISTCTRL_STYLE_CHECK);
				listctrl_set_rows(widget, 0, 0, 0);
				listctrl_set_cols(widget, 1, 0);
				listctrl_set_item_margin(widget, LINE_X_MARGIN, LINE_X_MARGIN, LINE_Y_SPLIT, LINE_Y_SPLIT);
				wgt_hook_register(widget, WIDGET_OnSelected, list_delete_hook, NULL);
				//listctrl_refresh_datasource(widget, FALSE);
				listctrl_set_cursor(widget, listctrl_get_count(widget)-1, FALSE);


				//3 BUTTON for DELETE
				widget = wgt_create((t_HWidget)handle, CLSID_button);
				wgt_set_resid(widget, RESID_BTN_DEL);
				wgt_set_bound(widget, MARGIN_X, client.dy-BOTTOM+(BOTTOM-BTN_HEIGHT)/2, (client.dx-MARGIN_X*4)/2, BTN_HEIGHT);
				wgt_set_caption_by_resid(widget, BRD_HANDLE_SELF, RESID_STR_DELETE);
				wgt_set_text_align(widget, guiRight);
				wgt_set_border(widget, BORDER_TYPE_3DIN, 1, guiFourCorner);
				wgt_set_bg_fill(widget, GDI_FILL_MODE_2COLOR_H, MAKE_RGB(0xe0,0,0), MAKE_RGB(0xb0,0,0));
				wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
				wgt_hook_register(widget, WIDGET_OnSelected, btn_delete_hook, NULL);
				//3 BUTTON for FORWARD
				widget = wgt_create((t_HWidget)handle, CLSID_button);
				wgt_set_resid(widget, RESID_BTN_FWD);
				wgt_set_bound(widget, client.dx/2+MARGIN_X, client.dy-BOTTOM+(BOTTOM-BTN_HEIGHT)/2, (client.dx-MARGIN_X*4)/2, BTN_HEIGHT);
				wgt_set_caption_by_resid(widget, BRD_HANDLE_SELF, RESID_STR_FORWARD);
				wgt_set_text_align(widget, guiRight);
				wgt_set_border(widget, BORDER_TYPE_3DIN, 1, guiFourCorner);
				wgt_set_bg_fill(widget, GDI_FILL_MODE_2COLOR_H, MAKE_RGB(0xe0,0,0), MAKE_RGB(0xb0,0,0));
				wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
				wgt_hook_register(widget, WIDGET_OnSelected, btn_forward_hook, NULL);
			}
			{
				t_HWidget sk = wnd_get_skbar(handle);
				if(g_object_valid(sk)){
					sk_set_text(sk, SK_LSK, NULL, RESID_STR_CANCEL);
					sk_set_text(sk, SK_RSK, NULL, RESID_STR_DELALL);
					sk_set_callback(sk, SK_LSK, ViewDelSKCallback, (void *)SK_LSK);
					sk_set_callback(sk, SK_RSK, ViewDelSKCallback, (void *)SK_RSK);
				}
			}
			ret = SUCCESS;
		}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			if(wdata){
				FREEIF(wdata->title);
				g_object_unref(wdata->hds_cs);
				contact_free_info(wdata->cinfo);
				FREE(wdata);
			}
			g_ds_unregister_notify(glob->ds);
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			if(IS_UNSAVED_CONTACT(wdata->cid)){
				char name[40];
				char *number;
				t_ContactID cid;
				//2 之所以要重新Goto一下, 是因为hds_cs数据库有变更, 记录被重新刷新过
				g_ds_Goto(wdata->hds_cs, wdata->cs_rid);
				number = g_ds_GetString(glob->hds_cs, SMS_DBFIELD_NUMBER);
				cid = contact_query_by_number(number, name, sizeof(name));
				if(cid != UNKNOWN_CONTACT && wdata->cid != cid){
					wdata->cid = cid;
					g_ds_SetValue(glob->hds_cs, CSESSION_DBFIELD_SID, cid);
					g_ds_Update(glob->hds_cs);
					g_ds_First(glob->ds);
					while(!g_ds_Eof(glob->ds)){
						g_ds_SetValue(glob->ds, SMS_DBFIELD_SID, cid);
						g_ds_Update(glob->ds);
						g_ds_Next(glob->ds);
					}
					
					wgt_set_caption(handle, name);
				}
				ret = SUCCESS;
			}
			break;
		case WIDGET_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(lparam == MVK_SEND && sparam == MMI_KEY_PRESS)
				;//pack_call("call", MOD_ID_MAIN, (gu8*)glob->items[glob->cursor].CellNo, strlen(glob->items[glob->cursor].CellNo)+1, TRUE, NULL);
	}
	return ret;
}

#endif /* SMALL_LCD */
#endif

