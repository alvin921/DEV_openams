
#if 0	
#define DLL_EXPORT_calllog
#endif

#include "pack_calllog.hi"
#include "pack_sys.h"

//1 ==================【CallLog Control for Specified Contact】==================
#if defined(FTR_COMM_SESSION_SUPPORT)
//2 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

enum {
	RESID_BTN_CALL = RESID_CTRL_DYNAMIC,
	RESID_BTN_MORE,
	RESID_LIST_ALL,
};

typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	t_HDataSet	hdb;
	t_CSessionID	sid;
	char *number;
	t_ContactInfo *cinfo;
}t_ContactCallLogCtrl;

void	db_calllog_redraw_record(t_HDataSet hdb, int index, t_HGDI gdi, t_GuiViewType viewtype, int width, int height, gboolean hilight)
{
	datetime_t curr_dt;
	dos_datetime_t dt;
	char dt_string[14];
	resid_t resid;
	gu8 type;
	coord_t x, y;
	char *string;

	g_ds_GotoByIndex(hdb, index);
	dt = g_ds_GetValue(hdb, CALL_DBFIELD_TIME);
	type = g_ds_GetValue(hdb, CALL_DBFIELD_TYPE);
	if(type == CALL_LOG_OUTGOING)
		resid = RESID_STR_OUTGOING_CALL;
	else
		resid = RESID_STR_INCOMING_CALL;

	x = 5;
	//2 呼入/	呼出
	string = brd_get_string(brd_get(PACK_GUID_SELF), resid, "");
	gdi_draw_text_in(gdi, x, 0, width-x, height, string, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
	gdi_draw_text(gdi, "    ", 4, CHARSET_UTF8);
	//2 日期或时间
	datetime_current(&curr_dt);
	gdi_printf(gdi, "%02d-%02d %02d:%02d", DOSDATE_M(dt>>16), DOSDATE_D(dt>>16), DOSTIME_H(dt), DOSTIME_M(dt));
	gdi_draw_text(gdi, "    ", 4, CHARSET_UTF8);
	//2 持续时间或
	if(type == CALL_LOG_MISSED){
		string = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_MISSED_CALL, "");
		gdi_draw_text(gdi, string, -1, CHARSET_UTF8);
	}else if(type == CALL_LOG_REJECTED){
		string = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_REJECTED_CALL, "");
		gdi_draw_text(gdi, string, -1, CHARSET_UTF8);
	}else{
		dt = g_ds_GetValue(hdb, CALL_DBFIELD_DURATION);
		if(type == CALL_LOG_OUTGOING && dt == 0){
			string = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_CANCEL, "");
			gdi_draw_text(gdi, string, -1, CHARSET_UTF8);
		}else{
			if(dt >= 3600){
				gdi_printf(gdi, "%d%s", dt/3600, brd_get_string(BRD_HANDLE_SELF, RESID_STR_HOUR, "h"));
				dt %= 3600;
			}
			if(dt >= 60){
				gdi_printf(gdi, "%d%s", dt/60, brd_get_string(BRD_HANDLE_SELF, RESID_STR_MINUTE, "m"));
				dt %= 60;
			}
			gdi_printf(gdi, "%d%s", dt, brd_get_string(BRD_HANDLE_SELF, RESID_STR_SECOND, "s"));
		}
	}
}

void	wgt_calllog_attach_contact(t_HWidget handle, t_CSessionID sid)
{
	t_ContactCallLogCtrl * pme = (t_ContactCallLogCtrl *)handle;
	if(g_object_valid(handle) && pme->sid != sid){
		if(pme->cinfo == NULL && pme->number)
			FREE(pme->number);
		if(sid != CSESSION_NULL){
			g_ds_CondSet1(pme->hdb, CALL_DBFIELD_SID, DBREL_EQ, sid);
			g_ds_Query(pme->hdb, (pme->sid == CSESSION_NULL)?FALSE:TRUE);
		}else{
			g_ds_CondClear(pme->hdb);
			g_ds_Query(pme->hdb, TRUE);
		}
		pme->sid = sid;
		pme->cinfo = contact_get_info(sid);
		if(pme->cinfo)
			pme->number = pme->cinfo->number;
		else if(!g_ds_Empty(pme->hdb)){
			g_ds_First(pme->hdb);
			pme->number = g_str_dup(g_ds_GetString(pme->hdb, CALL_DBFIELD_NUMBER));
		}
		wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
		wgt_redraw(handle, FALSE);
	}
}
void	wgt_calllog_contact_dial(t_HWidget handle)
{
	t_ContactCallLogCtrl * pme = (t_ContactCallLogCtrl *)handle;
	if(g_object_valid(handle) && pme->sid != CSESSION_NULL){
		call_dialing(pme->number, pme->sid);
	}
}
void	wgt_calllog_contact_more_op(t_HWidget handle)
{
	t_ContactCallLogCtrl * pme = (t_ContactCallLogCtrl *)handle;
	if(g_object_valid(handle) && pme->sid != CSESSION_NULL){
		dlg_contact_op_show(PACK_GUID_SELF, pme->sid, pme->number);
	}
}

static error_t	__widget_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_ContactCallLogCtrl *pme = (t_ContactCallLogCtrl *)wgt_get_parent(handle);
	switch(evt){
#if defined(FTR_NO_INPUT_KEY)
		case WIDGET_OnSelected:
			switch(wgt_get_resid(handle)){
				case RESID_BTN_CALL:
					//call_dialing(g_ds_GetString(pme->hdb, CALL_DBFIELD_NUMBER), g_ds_GetValue(pme->hdb, CALL_DBFIELD_SID));
					call_dialing(pme->number, pme->sid);
					break;
				case RESID_BTN_MORE:
					//g_ds_GotoByIndex(pme->hdb, 0);
					//dlg_contact_op_show(PACK_GUID_SELF, pme->sid, g_ds_GetString(pme->hdb, CALL_DBFIELD_NUMBER));
					dlg_contact_op_show(PACK_GUID_SELF, pme->sid, pme->number);
					break;
			}
			ret = SUCCESS;
			break;
#endif
	}
	return ret;
}

#define BTN_COLOR_1 MAKE_RGB(0x23,0x41,0x56)
#define BTN_COLOR_2 MAKE_RGB(0x67,0x9a,0xc0)
static error_t		__viewctrl_OnCreate(t_ContactCallLogCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	t_HWidget widget;
	//2 NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP|CTRL_ATTR_CONTAINER);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	pme->hdb = calllog_db_open();
	if(g_object_valid(pme->hdb)){
		g_ds_bind(pme->hdb, db_calllog_redraw_record, NULL);
		g_ds_OrderBy(pme->hdb, CALL_DBFIELD_TIME, FALSE, TRUE);
	}
	pme->sid = CSESSION_NULL;
	pme->cinfo = NULL;//contact_get_info(wdata->sid);
	wgt_set_bg_fill_solid((t_HWidget)pme, BTN_COLOR_2);
	//wgt_set_bg_fill(handle, GDI_FILL_MODE_SOLID, 0xff679ac0, 0xff679ac0);
	
#if defined(FTR_NO_INPUT_KEY)
	//2 创建button: <Call>
	widget = wgt_create((t_HWidget)pme, CLSID_button);
	wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_STR_CALL);
	wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftTop|guiLeftBottom);
	//wgt_set_bg_fill(widget, GDI_FILL_MODE_GRADIENT_H, BTN_COLOR_1, BTN_COLOR_2);
	wgt_set_bg_fill_solid(widget, BTN_COLOR_2);
	wgt_set_resid(widget, RESID_BTN_CALL);
	wgt_hook_register(widget, WIDGET_OnSelected, __widget_hook, NULL);
	//2 创建button: <More>
	widget = wgt_create((t_HWidget)pme, CLSID_button);
	wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_STR_MORE);
	wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiRightTop|guiRightBottom);
	//wgt_set_bg_fill(widget, GDI_FILL_MODE_GRADIENT_H, BTN_COLOR_1, BTN_COLOR_2);
	wgt_set_bg_fill_solid(widget, BTN_COLOR_2);
	wgt_set_resid(widget, RESID_BTN_MORE);
	wgt_hook_register(widget, WIDGET_OnSelected, __widget_hook, NULL);
#endif
	//2 创建listctrl
	widget = wgt_create((t_HWidget)pme, CLSID_LISTCTRL);
	wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
	wgt_set_bg_fill_solid(widget, ui_theme_get()->bg_color);
	wgt_set_border(widget, BORDER_TYPE_LINE, 0, guiTop);
	listctrl_attach_db(widget, pme->hdb);
	listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
	listctrl_set_rows(widget, 0, prod_ui_list_item_height(), 0);
	listctrl_set_cols(widget, 1, 0);
	listctrl_set_flags(widget, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM);
	wgt_set_resid(widget, RESID_LIST_ALL);
	wgt_hook_register(widget, WIDGET_OnKeyEvent, __widget_hook, NULL);
	return SUCCESS;
}

static void	__viewctrl_OnDestroy(t_ContactCallLogCtrl* pme)
{
	if(pme->cinfo == NULL && pme->number)
		FREE(pme->number);
	g_object_unref(pme->hdb);
	contact_free_info(pme->cinfo);
	g_object_inherit_destroy(pme);
}

static error_t __viewctrl_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_ContactCallLogCtrl *pme = (t_ContactCallLogCtrl *)handle;
	switch(evt){
		case WIDGET_OnDraw:
			if(wgt_test_attr(pme, CTRL_ATTR_MODIFIED)){
				t_HWidget child;
				int i;
				wgt_set_caption(wgt_parent_window(handle), pme->cinfo?pme->cinfo->name:pme->number);
				for(i = 0; i < g_vector_size(handle->vect_child); i++){
					child = g_vector_element_at(handle->vect_child, i);
					switch(wgt_get_resid(child)){
#if defined(FTR_NO_INPUT_KEY)
						case RESID_BTN_CALL:
							wgt_set_bound(child, lcd_get_width()/2-BTN_WIDTH, BTN_Y_OFFSET, BTN_WIDTH, BTN_HEIGHT);
							break;
						case RESID_BTN_MORE:
							wgt_set_bound(child, lcd_get_width()/2-1, BTN_Y_OFFSET, BTN_WIDTH, BTN_HEIGHT);
							break;
#endif
						case RESID_LIST_ALL:
#if defined(FTR_NO_INPUT_KEY)
							wgt_set_bound(child, 0, LIST_Y_OFFSET, wgt_bound_dx(handle), wgt_bound_dy(handle)-LIST_Y_OFFSET);
#else
							wgt_set_bound(child, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
#endif
							break;
					}
				}
			}
			container_redraw(handle, TRUE);
			{
				t_HGDI gdi = wgt_get_client_gdi(handle);

				gdi_set_color(gdi, RGB_DKGRAY);
				gdi_hline(gdi, 0, 0, wgt_bound_dx(handle)-1);
				gdi_set_color(gdi, RGB_LTGRAY);
				gdi_hline(gdi, 1, 0, wgt_bound_dx(handle)-1);
				g_object_unref(gdi);
			}
			break;

		case WIDGET_OnSetFocus:
			break;
		case WIDGET_OnKillFocus:
			break;
		case WIDGET_OnSelected:
			break;
			
#if !defined(FTR_NO_INPUT_KEY)
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_UP){
				if(lparam == MVK_SEND){
					g_ds_GotoByIndex(pme->hdb, 0);
					call_dialing(g_ds_GetString(pme->hdb, CALL_DBFIELD_NUMBER), pme->sid);
					ret = SUCCESS;
				}else if(lparam == MVK_LSK || lparam == MVK_OK){
					g_ds_GotoByIndex(pme->hdb, 0);
					dlg_contact_op_show(PACK_GUID_SELF, pme->sid, g_ds_GetString(pme->hdb, CALL_DBFIELD_NUMBER));
					ret = SUCCESS;
				}
			}
			break;
#endif

#if defined(FTR_TOUCH_SCREEN)
			case WIDGET_OnPenEvent:
			{
				coord_t x0, y0;
				PEN_XY(lparam, x0, y0);
				switch(PEN_TYPE(lparam)){
					case MMI_PEN_DOWN:
						break;
					case MMI_PEN_UP:
						break;
					case MMI_PEN_PRESS:
						break;
					case MMI_PEN_LONG_PRESS:
						break;
					case MMI_PEN_MOVE_IN:
						break;
					case MMI_PEN_MOVE_OUT:
						break;
					case MMI_PEN_MOVE:
						break;
				}
				break;
			}
#endif
	}
	if(ret == NOT_HANDLED){
		ret = g_widget_inherit_OnEvent(handle, evt, sparam, lparam);
	}
	return ret;
}


static VTBL(widget) ContactCalllogVtbl = {
	(PFNONCREATE)__viewctrl_OnCreate
	, (PFNONDESTROY)__viewctrl_OnDestroy

	, __viewctrl_OnEvent
	, NULL
};

CLASS_DECLARE(calllogview, t_ContactCallLogCtrl, ContactCalllogVtbl);



// TODO: add functions for your own widget.  You MUST export them in dll.def


#undef PARENT_CLASS

#endif


//1 ==================【CallLog Control for CSession】==================

#if defined(FTR_COMM_SESSION_SUPPORT)
//2 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

enum {
	RESID_BTN_ALL = RESID_CTRL_DYNAMIC,
	RESID_BTN_MISSED,
	RESID_LIST_MAIN,
};

typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	t_HDataSet	hdb;	//2 CSession
}t_CallLogCtrl;

void __cs_view_detail(t_HDataSet hds, int index)
{
	if(!g_ds_Empty(hds) && g_ds_GotoByIndex(hds, index) == SUCCESS){
		t_CSessionID sid;
		
		g_ds_GotoByIndex(hds, index);
		sid = g_ds_GetValue(hds, CSESSION_DBFIELD_SID);
		wnd_show(wnd_create(calllog_ViewSessionWndMsgHandler, BY_SID, sid));
	}
}

void cs_view_record(t_HDataSet hds, int index)
{
#if defined(FTR_NO_INPUT_KEY)
	if(!g_ds_Empty(hds) && g_ds_GotoByIndex(hds, index) == SUCCESS){
		call_dialing(g_ds_GetString(hds, CSESSION_DBFIELD_NUMBER), g_ds_GetValue(hds, CSESSION_DBFIELD_SID));
	}
#else
	__cs_view_detail(hds, index);
#endif
}

void	cs_redraw_record(t_HDataSet hds, int index, t_HGDI gdi, t_GuiViewType viewtype, int width, int height, gboolean hilight)
{
	t_ContactInfo *cinfo;
	char *name, *number;
	t_CSessionID sid;
	datetime_t curr_dt, dt;
	char dt_string[14];
	resid_t pic_resid;
	t_HImage icon = NULL;
	int icon_w, icon_h, tw, th;

	g_ds_GotoByIndex(hds, index);
	number = g_ds_GetString(hds, CSESSION_DBFIELD_NUMBER);
	if(number == NULL)
		number = "";
	sid = g_ds_GetValue(hds, CSESSION_DBFIELD_SID);
	cinfo = contact_get_info(sid);
	if(cinfo)
		name = cinfo->name;
	else
		name = number;
		

	dostime_to_datetime(g_ds_GetValue(hds, CSESSION_DBFIELD_TIME), &dt);
	icon = brd_get_image(brd_get(PACK_GUID_SELF), RESID_ICON_RARROW);
	{
		coord_t x=0;
		if(VIEWTYPE_IS_NORMAL(viewtype)){
#if defined(FTR_IPHONE_STYLE)
			//2 Line 1:<name>  (<count>)						<time> <advance...>
			//2 Line 2: <number type>
			resid_t resid;
			gu32 old;

			datetime_current(&curr_dt);
			if(curr_dt.dt_year == dt.dt_year && curr_dt.dt_month == dt.dt_month && curr_dt.dt_day == dt.dt_day)
				sprintf(dt_string, "%02d:%02d", dt.dt_hour, dt.dt_minute);
			else
				sprintf(dt_string, "%02d-%02d", dt.dt_month, dt.dt_day);
			gdi_measure_text(gdi, dt_string, 5, CHARSET_US_ASCII, -1, &tw, &th, NULL);

			gdi_draw_text_at(gdi, width-LINE_MARGIN-height-tw, (height-th)>>1, dt_string, -1, CHARSET_US_ASCII);
			gdi_draw_image_in(gdi, width-LINE_MARGIN-height, 0, height, height, icon, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
			width -= LINE_MARGIN+height+tw;

			width -= LINE_MARGIN;
			gdi_draw_text_in(gdi, LINE_MARGIN, 0, width, height/2, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
			old = gdi_set_color(gdi, RGB_GRAY);
			sprintf(dt_string, "  (%d)", g_ds_GetValue(hds, CSESSION_DBFIELD_COUNT));
			gdi_draw_text(gdi, dt_string, -1, CHARSET_UTF8);
			gdi_set_color(gdi, old);

			switch(sid&0xff){
				case CONTACT_F_NUMBER_FAX:
					resid = RESID_STR_NUMBER_FAX;
					break;
				case CONTACT_F_NUMBER_HOME:
					resid = RESID_STR_NUMBER_HOME;
					break;
				case CONTACT_F_NUMBER_MOBILE:
					resid = RESID_STR_NUMBER_MOBILE;
					break;
				case CONTACT_F_NUMBER_OFFICE:
					resid = RESID_STR_NUMBER_OFFICE;
					break;
				case CONTACT_F_NUMBER_OTHER:
					resid = RESID_STR_NUMBER_OTHER;
					break;
				default:
					resid = RESID_STR_NUMBER_UNKNOWN;
					break;
			}
			name = brd_get_string(BRD_HANDLE_SELF, resid, "Unknown");
			gdi_draw_text_in(gdi, LINE_MARGIN, height/2, width, height/2, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
#elif defined(FTR_CALLLOG_VIEW_2LINE)
			//2 Line 1:<name>					   (<count>)
			//2 Line 2: <time>
			gdi_draw_text_in(gdi, x, 0, width, height/2, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
			sprintf(dt_string, "(%d)", g_ds_GetValue(hds, CSESSION_DBFIELD_COUNT));
			gdi_draw_text_in(gdi, x, 0, width, height/2, dt_string, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
			sprintf(dt_string, "%02d-%02d %02d:%02d", dt.dt_month, dt.dt_day, dt.dt_hour, dt.dt_minute);
			gdi_draw_text_in(gdi, x, height/2, width, height/2, dt_string, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
#else
			//2 <name> (<count>)          <time>
			datetime_current(&curr_dt);
			if(curr_dt.dt_year == dt.dt_year && curr_dt.dt_month == dt.dt_month && curr_dt.dt_day == dt.dt_day)
				sprintf(dt_string, "%02d:%02d", dt.dt_hour, dt.dt_minute);
			else
				sprintf(dt_string, "%02d-%02d", dt.dt_month, dt.dt_day);
			gdi_measure_text(gdi, dt_string, 5, CHARSET_US_ASCII, -1, &tw, &th, NULL);
			gdi_draw_text_at(gdi, x+width-tw, (height-th)>>1, dt_string, -1, CHARSET_US_ASCII);
			width -= tw;
			gdi_draw_text_in(gdi, x, 0, width, height, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
			gdi_move(gdi, 4, 0);
			sprintf(dt_string, "(%d)", g_ds_GetValue(hds, CSESSION_DBFIELD_COUNT));
			gdi_draw_text(gdi, dt_string, -1, CHARSET_US_ASCII);
#endif
		}
	}
	g_object_unref(icon);
	contact_free_info(cinfo);
}

static error_t	__list_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	switch(evt){
#if !defined(FTR_NO_INPUT_KEY)
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_UP){
				t_CallLogCtrl *pme = (t_CallLogCtrl *)wgt_get_parent(handle);
				if(lparam == MVK_RIGHT){
					__cs_view_detail(pme->hdb, listctrl_get_cursor(handle));
					ret = SUCCESS;
				}else if(lparam == MVK_LSK || lparam == MVK_SEND){
					int cursor = listctrl_get_cursor(handle);
					if(!g_ds_Empty(pme->hdb) && g_ds_GotoByIndex(pme->hdb, cursor) == SUCCESS){
						t_CSessionID sid;
						char *number;
						
						sid = g_ds_GetValue(pme->hdb, CSESSION_DBFIELD_SID);
						number = g_ds_GetString(pme->hdb, CSESSION_DBFIELD_NUMBER);
						call_dialing(number, sid);
					}
					ret = SUCCESS;
				}
			}
			break;
#endif
#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
			{
				coord_t x0, y0;
				int index;
				PEN_XY(lparam, x0, y0);
				switch(PEN_TYPE(lparam)){
					case MMI_PEN_DOWN:
						index = listctrl_index_by_point(handle, &x0, &y0, NULL, NULL);
						if(index >= 0){
							gui_rect_t rc;
							wgt_get_client_rect(handle, &rc);
							if(x0 >= rc.dx-LINE_MARGIN-LINE_HEIGHT){
								wgt_set_tag(handle, index+1);
								wgt_disable_attr(handle, CTRL_ATTR_HSCROLLABLE);
								wgt_disable_attr(handle, CTRL_ATTR_VSCROLLABLE);
								ret = SUCCESS;
							}
						}
						break;
					case MMI_PEN_UP:
						index = wgt_get_tag(handle)-1;
						if(index >= 0){
							t_CallLogCtrl *pme = (t_CallLogCtrl *)wgt_get_parent(handle);
							wgt_set_tag(handle, 0);
							wgt_enable_attr(handle, CTRL_ATTR_HSCROLLABLE);
							wgt_enable_attr(handle, CTRL_ATTR_VSCROLLABLE);
							__cs_view_detail(pme->hdb, index);
							ret = SUCCESS;
						}
						break;
					case MMI_PEN_PRESS:
						break;
				}
			}	
			break;
#endif
	}
	return ret;
}


static void	__calllog_ds_notify(t_HDataSet hds, t_DBRecordID rid, void *userdata)
{
	t_CallLogCtrl *pme = (t_CallLogCtrl *)userdata;

	g_ds_Query(hds, TRUE);

	wgt_enable_attr(pme, CTRL_ATTR_MODIFIED);
	wgt_redraw((t_HWidget)pme, FALSE);
}

static error_t		__ctrl_OnCreate(t_CallLogCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	t_HWidget widget;
	//2 NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP|CTRL_ATTR_CONTAINER);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
#if defined(FTR_COMM_SESSION_SUPPORT)
	pme->hdb = comm_session_db_open();
	if(g_object_valid(pme->hdb)){
		g_ds_bind(pme->hdb, cs_redraw_record, cs_view_record);
		g_ds_CondSet1(pme->hdb, CSESSION_DBFIELD_TYPE, DBREL_EQ, COMM_TYPE_CALL_LOG);
		g_ds_OrderBy(pme->hdb, CSESSION_DBFIELD_TIME, FALSE, FALSE);
		g_ds_Query(pme->hdb, FALSE);

		g_ds_register_notify(pme->hdb, __calllog_ds_notify, pme);
	}
#endif

	wgt_set_bg_fill_solid(pme, BTN_COLOR_2);
	//2 Listctrl: All Calls/Missed Calls
	widget = wgt_create((t_HWidget)pme, "listctrl");
	wgt_set_border(widget, BORDER_TYPE_LINE, 0, guiTop);
	wgt_set_bg_fill_solid(widget, ui_theme_get()->bg_color);
	wgt_set_resid(widget, RESID_LIST_MAIN);

	listctrl_attach_db(widget, pme->hdb);
	listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
	listctrl_set_rows(widget, 0, prod_ui_list_item_height(), 0);
	listctrl_set_cols(widget, 1, 0);
	listctrl_set_flags(widget, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE|CLFLAG_AUTOTAB_UD|CLFLAG_CURSOR_NODRAW_ONKILLFOCUS);
	wgt_hook_register(widget, WIDGET_OnKeyEvent|WIDGET_OnPenEvent, __list_hook, NULL);
	//2 Button: All Calls
	widget = wgt_create((t_HWidget)pme, "button");
	wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_STR_ALL_CALL);
	wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftTop|guiLeftBottom);
	wgt_set_bg_fill_solid(widget, BTN_COLOR_2);
	wgt_set_resid(widget, RESID_BTN_ALL);
	//2 Button: Missed Calls
	widget = wgt_create((t_HWidget)pme, "button");
	wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_STR_MISSED_CALL);
	wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiRightTop|guiRightBottom);
	wgt_set_bg_fill_solid(widget, BTN_COLOR_2);
	wgt_set_resid(widget, RESID_BTN_MISSED);
	return SUCCESS;
}

static void	__ctrl_OnDestroy(t_CallLogCtrl* pme)
{
	int i;

	g_object_unref(pme->hdb);
	g_object_inherit_destroy(pme);
}

static error_t __ctrl_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_CallLogCtrl *pme = (t_CallLogCtrl *)handle;
	switch(evt){
		case WIDGET_OnDraw:
			if(wgt_test_attr(pme, CTRL_ATTR_MODIFIED)){
				t_HWidget child;
				int i;
				for(i = 0; i < g_vector_size(handle->vect_child); i++){
					child = g_vector_element_at(handle->vect_child, i);
					switch(wgt_get_resid(child)){
						case RESID_BTN_ALL:
							wgt_set_bound(child, wgt_bound_dx(handle)/2-BTN_WIDTH, BTN_Y_OFFSET, BTN_WIDTH, BTN_HEIGHT);
							break;
						case RESID_BTN_MISSED:
							wgt_set_bound(child, wgt_bound_dx(handle)/2-1, BTN_Y_OFFSET, BTN_WIDTH, BTN_HEIGHT);
							break;
						case RESID_LIST_MAIN:
							wgt_set_bound(child, 0, LIST_Y_OFFSET, wgt_bound_dx(handle), wgt_bound_dy(handle)-LIST_Y_OFFSET);
							break;
					}
				}
			}
			container_redraw(handle, TRUE);
			break;

		case WIDGET_OnSetFocus:
			break;
		case WIDGET_OnKillFocus:
			break;
		case WIDGET_OnSelected:
			break;
			
		case WIDGET_OnKeyEvent:
			break;

#if defined(FTR_TOUCH_SCREEN)
			case WIDGET_OnPenEvent:
			{
				coord_t x0, y0;
				PEN_XY(lparam, x0, y0);
				switch(PEN_TYPE(lparam)){
					case MMI_PEN_DOWN:
						break;
					case MMI_PEN_UP:
						break;
					case MMI_PEN_PRESS:
						break;
					case MMI_PEN_LONG_PRESS:
						break;
					case MMI_PEN_MOVE_IN:
						break;
					case MMI_PEN_MOVE_OUT:
						break;
					case MMI_PEN_MOVE:
						break;
				}
				break;
			}
#endif
	}
	if(ret == NOT_HANDLED){
		ret = g_widget_inherit_OnEvent(handle, evt, sparam, lparam);
	}
	return ret;
}


static VTBL(widget) calllogVtbl = {
	(PFNONCREATE)__ctrl_OnCreate
	, (PFNONDESTROY)__ctrl_OnDestroy

	, __ctrl_OnEvent
	, NULL
};

CLASS_DECLARE(calllog, t_CallLogCtrl, calllogVtbl);


// TODO: add functions for your own widget.  You MUST export them in dll.def


#undef PARENT_CLASS

//2 <sparam>=BY_SID|BY_NUMBER
//2 <lparam>=<sid> or <number>
void	wgt_calllog_attach(t_HWidget widget, t_sparam sparam, t_lparam lparam)
{
	if(g_object_valid(widget) && sparam != BY_NONE){
		t_CallLogCtrl *pme = (t_CallLogCtrl*)widget;
		if(sparam == BY_SID){
			//2 查询指定contact的所有记录
			g_ds_CondSet2(pme->hdb, CALL_DBFIELD_SID, DBREL_BT, lparam, BOOL_AND, CALL_DBFIELD_SID, DBREL_LT, (lparam&0xffff0000)|0xff);
		}else if(sparam == BY_NUMBER){
			g_ds_CondSet1(pme->hdb, CALL_DBFIELD_NUMBER, DBREL_HAS, (char *)lparam);
		}
		g_ds_Query(pme->hdb, FALSE);
		wgt_enable_attr(widget, CTRL_ATTR_MODIFIED);
	}
}


#else


void	db_calllog_redraw_record(t_HDataSet hdb, int index, t_HGDI gdi, t_GuiViewType viewtype, int width, int height, gboolean hilight)
{
	const char *number;
	datetime_t curr_dt;
	dos_datetime_t dt;
	char dt_string[14];
	resid_t pic_resid;
	t_HImage icon = NULL;
	int icon_w, icon_h, tw, th;

	g_ds_GotoByIndex(hdb, index);
	number = g_ds_GetString(hdb, CALL_DBFIELD_NUMBER);
	dt = g_ds_GetValue(hdb, CALL_DBFIELD_TIME);

	//pic = g_ds_GetString(pme->hdb, CONTACT_DBFIELD_PICTURE);
	switch(g_ds_GetValue(hdb, CALL_DBFIELD_TYPE)){
		case CALL_LOG_OUTGOING:
			pic_resid = RESID_IMG_OUTGOING_CALL;
			break;
		case CALL_LOG_INCOMING:
			pic_resid = RESID_IMG_INCOMING_CALL;
			break;
		case CALL_LOG_MISSED:
			pic_resid = RESID_IMG_MISSED_CALL;
			break;
		case CALL_LOG_REJECTED:
			pic_resid = RESID_IMG_REJECTED_CALL;
			break;
		default:
			pic_resid = RESID_UNDEF;
			break;
	}
	icon = brd_get_image(brd_get(PACK_GUID_SELF), pic_resid);
	icon_w = icon_h = 0;
	if(g_object_valid(icon)){
		icon_w = image_get_width(icon);
		icon_h = image_get_height(icon);
	}
	{
		coord_t x;
		if(g_object_valid(icon)){
			gdi_draw_image_in(gdi, 0, 0, height, height, icon, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE | IDF_HALIGN_STRETCH | IDF_VALIGN_STRETCH);
		}
		width -= height+1;
		x = height+1;
		if(VIEWTYPE_IS_NORMAL(viewtype)){
#if defined(FTR_CALLLOG_VIEW_2LINE)
			//2 Line1: number
			gdi_draw_text_in(gdi, x, 0, width, height/2, number, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
			//2 Line2: time                  duration
			sprintf(dt_string, "%02d-%02d %02d:%02d", DOSDATE_M(dt>>16), DOSDATE_D(dt>>16), DOSTIME_H(dt), DOSTIME_M(dt));
			gdi_draw_text_in(gdi, x, height/2, width, height/2, dt_string, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);

			dt = g_ds_GetValue(hdb, CALL_DBFIELD_DURATION);
			if(dt >= 3600)
				sprintf(dt_string, "%02d:%02d:%02d", dt/3600, (dt%3600)/60, dt%60);
			else
				sprintf(dt_string, "%02d:%02d", dt/60, dt%60);
			gdi_draw_text_in(gdi, x, height/2, width, height/2, dt_string, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
#else
			//2 <number>        <time>
			datetime_current(&curr_dt);
			if(curr_dt.dt_year == DOSDATE_Y(dt>>16) && curr_dt.dt_month == DOSDATE_M(dt>>16) && curr_dt.dt_day == DOSDATE_D(dt>>16))
				sprintf(dt_string, "%02d:%02d", DOSTIME_H(dt), DOSTIME_M(dt));
			else
				sprintf(dt_string, "%02d-%02d", DOSDATE_M(dt>>16), DOSDATE_D(dt>>16));
			gdi_measure_text(gdi, dt_string, 5, CHARSET_US_ASCII, -1, &tw, &th, NULL);
			gdi_draw_text_at(gdi, x+width-tw, (height-th)>>1, dt_string, -1, CHARSET_US_ASCII);
			width -= tw;
			gdi_draw_text_in(gdi, x, 0, width, height, number, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
#endif
		}
	}
	g_object_unref(icon);
}

void db_calllog_view_record(t_HDataSet hds, int index)
{
	wnd_show(wnd_create_by_resid(calllog_ViewWndMsgHandler, 0, index, brd_get(PACK_GUID_SELF), RESID_WIN_VIEW));
}

#endif

