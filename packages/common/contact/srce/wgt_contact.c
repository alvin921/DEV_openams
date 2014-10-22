
#if 0	
#define DLL_EXPORT_contact
#endif

#include "pack_contact.hi"
#include "pack_sys.h"


//1 ==================¡¾Contact Control for CSession¡¿==================

#if defined(FTR_COMM_SESSION_SUPPORT)
#endif

//2 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

enum {
	RESID_BTN_REFRESH = RESID_CTRL_DYNAMIC,
	RESID_BTN_ADD,
	RESID_LIST_MAIN,
};

static void	__show_contact_csession(t_HGDI gdi, int width, int height, int x, t_ContactID rid)
{
	//3 Line 1: name	<n_sms><n_calllog>...
	//3 Line 2: number
	t_HDataSet hds_cs=NULL;
	int i, w;
	char count[8];
	t_HImage icon;
	
	hds_cs = comm_session_db_open();
	if(g_object_valid(hds_cs)){
		g_ds_CondSet2(hds_cs, CSESSION_DBFIELD_SID, DBREL_BT, CSESSION_ID(rid, 0), BOOL_AND, CSESSION_DBFIELD_SID, DBREL_LT, CSESSION_ID(rid, 0xff));
		g_ds_Query(hds_cs, FALSE);
	}
	//2 Line 1
	if(g_object_valid(hds_cs) && !g_ds_Empty(hds_cs)){
		for(i = 0; i < g_ds_GetRecordNum(hds_cs); i++){
			g_ds_GotoByIndex(hds_cs, i);
			sprintf(count, "(%d)", g_ds_GetValue(hds_cs, CSESSION_DBFIELD_COUNT));
			icon = pack_GetIcon(g_ds_GetValue(hds_cs, CSESSION_DBFIELD_TYPE));
			gdi_measure_text(gdi, count, strlen(count), CHARSET_US_ASCII, -1, &w, NULL, NULL);
			if(g_object_valid(icon))
				w += height;
			gdi_move_to(gdi, x+width-w, 0);
			gdi_draw_icon_text(gdi, icon, count, w, height, guiRight, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
			width -= w;
			g_object_unref(icon);
		}
	}
	g_object_unref(hds_cs);
}

void	db_contact_redraw_record(t_HDataSet hdb, int index, t_HGDI gdi, t_GuiViewType viewtype, int width, int height, gboolean hilight)
{
	const char *name, *number;
	resid_t pic_resid;
	const char *pic;
	t_HImage icon = NULL;

	g_ds_GotoByIndex(hdb, index);
	name = g_ds_GetString(hdb, CONTACT_DBFIELD_NAME);
	pic = g_ds_GetString(hdb, CONTACT_DBFIELD_PICTURE);
	number = g_ds_GetString(hdb, CONTACT_DBFIELD_NUMBER);
	if(g_str_checki(pic)){
		icon = brd_get_image(brd_get(PACK_GUID_SELF), g_str2int(pic));
	}else if(pic && *pic){
		icon = image_load_from_file(pic);
	}else{
#if !defined(FTR_IPHONE_STYLE)
		icon = brd_get_image(brd_get(PACK_GUID_SELF), RESID_ICON_APPLICATION);
#endif
	}
	
	if(viewtype == GUI_VIEWTYPE_ICON_TEXT_TOP){
		gdi_draw_icon_text(gdi, icon, name, width, height, guiTop, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
	}else if(viewtype == GUI_VIEWTYPE_ICON_TEXT_BOTTOM){
		gdi_draw_icon_text(gdi, icon, name, width, height, guiBottom, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
	}else if(viewtype == GUI_VIEWTYPE_NORMAL){
		int x;
		if(g_object_valid(icon)){
			gdi_draw_image_in(gdi, 0, 0, height, height, icon, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE | IDF_HALIGN_STRETCH | IDF_VALIGN_STRETCH);
			g_object_unref(icon);
		}
		width -= height+2;
		x = height+2;
		
		gdi_draw_text_in(gdi, x, 0, width, height, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
		__show_contact_csession(gdi, width, height, x, g_ds_CurrentID(hdb));
#if defined(FTR_IPHONE_STYLE)
#endif
	}
}

void	db_contact_view_record(t_HDataSet hds, int index)
{
	contact_ShowViewWindow(index, hds);
}

static error_t	__list_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	switch(evt){
#if !defined(FTR_NO_INPUT_KEY)
		case WIDGET_OnKeyEvent:
			if(lparam == MVK_RIGHT){
				if(sparam == MMI_KEY_UP){
					t_ContactCtrl *pme = (t_ContactCtrl *)wgt_get_parent(handle);
					db_contact_view_record(pme->hdb, listctrl_get_cursor(handle));
				}
				ret = SUCCESS;
			}else if(sparam == MMI_KEY_PRESS && lparam == MVK_LSK){
				t_ContactCtrl *pme = (t_ContactCtrl *)wgt_get_parent(handle);
				contact_AddNewWindow(pme->hdb);
				ret = SUCCESS;
			}
			break;
#endif
#if 0//defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
			{
				coord_t x0, y0;
				PEN_XY(lparam, x0, y0);
				int index;
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
							t_ContactCtrl *pme = (t_ContactCtrl *)wgt_get_parent(handle);
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


static error_t	__widget_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_ContactCtrl *pme = (t_ContactCtrl *)wgt_get_parent(handle);
	switch(evt){
#if defined(FTR_NO_INPUT_KEY)
		case WIDGET_OnSelected:
			switch(wgt_get_resid(handle)){
				case RESID_BTN_REFRESH:
					break;
				case RESID_BTN_ADD:
					contact_AddNewWindow(pme->hdb);
					break;
			}
			ret = SUCCESS;
			break;
#endif
	}
	return ret;
}


static void	__db_contact_notify(t_HDataSet hds, t_DBRecordID rid, void *userdata)
{
	t_ContactCtrl* pme = (t_HWindow)userdata;
	g_ds_Query(hds, TRUE);
	listctrl_refresh_datasource(wgt_get_child((t_HWidget)pme, RESID_LIST_MAIN, FALSE), FALSE);
}

#define BTN_COLOR_1 MAKE_RGB(0x23,0x41,0x56)
#define BTN_COLOR_2 MAKE_RGB(0x67,0x9a,0xc0)
static error_t		__ctrl_OnCreate(t_ContactCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	t_HWidget widget;
	//2 NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP|CTRL_ATTR_CONTAINER);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	pme->hdb = g_ds_open(CONTACT_DB_FNAME, CONTACT_DBTAB_LIST);
	if(g_object_valid(pme->hdb)){
		g_ds_register_notify(pme->hdb, __db_contact_notify, pme);
		g_ds_bind(pme->hdb, db_contact_redraw_record, db_contact_view_record);
	}

#if defined(FTR_IPHONE_STYLE)
	wgt_set_bg_fill_solid(pme, BTN_COLOR_2);
#endif

#if defined(FTR_NO_INPUT_KEY)
	//2 Button: Refresh
	widget = wgt_create((t_HWidget)pme, "button");
	wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_STR_CANCEL);
	wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftTop|guiLeftBottom);
	wgt_set_bg_fill_solid(widget, BTN_COLOR_2);
	wgt_set_resid(widget, RESID_BTN_REFRESH);
	//2 Button: Add Contact
	widget = wgt_create((t_HWidget)pme, "button");
	//wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_STR_ADD_CONTACT);
	wgt_set_icon_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_ICON_ADD);
	wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiRightTop|guiRightBottom);
	wgt_set_bg_fill_solid(widget, BTN_COLOR_2);
	wgt_set_resid(widget, RESID_BTN_ADD);
	wgt_hook_register(widget, WIDGET_OnSelected, __widget_hook, NULL);
#endif
	//2 Listctrl: Contacts
	widget = wgt_create((t_HWidget)pme, "listctrl");
	wgt_set_border(widget, BORDER_TYPE_NONE, 0, 0);
#if defined(FTR_IPHONE_STYLE)
	wgt_set_bg_fill_solid(widget, RGB_WHITE);
#endif
	wgt_set_resid(widget, RESID_LIST_MAIN);

	listctrl_attach_db(widget, pme->hdb);
	//listctrl_set_listview(widget, CALL_VIEWTYPE_2LINE_IPHONE, LINE_HEIGHT, LINE_HEIGHT, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE);
	listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
	listctrl_set_rows(widget, 0, prod_ui_list_item_height(), 0);
	listctrl_set_cols(widget, 1, 0);
	listctrl_set_flags(widget, CLFLAG_SCROLLING_VERTICAL);
	wgt_hook_register(widget, WIDGET_OnKeyEvent|WIDGET_OnPenEvent, __list_hook, NULL);
	return SUCCESS;
}

static void	__ctrl_OnDestroy(t_ContactCtrl* pme)
{
	g_object_unref(pme->hdb);
	g_object_inherit_destroy(pme);
}

static error_t __ctrl_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_ContactCtrl *pme = (t_ContactCtrl *)handle;
	switch(evt){
		case WIDGET_OnDraw:
			if(wgt_test_attr(pme, CTRL_ATTR_MODIFIED)){
				t_HWidget child;
				int i;
				for(i = 0; i < g_vector_size(handle->vect_child); i++){
					child = g_vector_element_at(handle->vect_child, i);
					switch(wgt_get_resid(child)){
#if defined(FTR_NO_INPUT_KEY)
						case RESID_BTN_REFRESH:
							wgt_set_bound(child, BTN_X_OFFSET, BTN_Y_OFFSET, BTN_WIDTH, BTN_HEIGHT);
							break;
						case RESID_BTN_ADD:
							wgt_set_bound(child, wgt_bound_dx(handle)-BTN_X_OFFSET-BTN_WIDTH, BTN_Y_OFFSET, BTN_WIDTH, BTN_HEIGHT);
							break;
#endif
						case RESID_LIST_MAIN:
							wgt_set_bound(child, 0, LIST_Y_OFFSET, wgt_bound_dx(handle), wgt_bound_dy(handle)-LIST_Y_OFFSET);
							break;
					}
				}
			}
			container_redraw(handle, TRUE);
#if defined(FTR_NO_INPUT_KEY)
			{
				t_HGDI gdi = wgt_get_gdi(handle);
				char *title = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_TITLE, "");
				gdi_set_color(gdi, RGB_WHITE);
				gdi_draw_text_in(gdi, 0, 0, wgt_bound_dx(handle), LIST_Y_OFFSET, title, -1, CHARSET_UTF8, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
				g_object_unref(gdi);
			}
#endif
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


static VTBL(widget) contactVtbl = {
	(PFNONCREATE)__ctrl_OnCreate
	, (PFNONDESTROY)__ctrl_OnDestroy

	, __ctrl_OnEvent
	, NULL
};

CLASS_DECLARE(contact, t_ContactCtrl, contactVtbl);


// TODO: add functions for your own widget.  You MUST export them in dll.def


#undef PARENT_CLASS

void	wgt_contact_attach(t_HWidget widget, t_sparam sparam, t_lparam lparam)
{
	if(g_object_valid(widget) && sparam != BY_NONE){
		t_ContactCtrl *pme = (t_ContactCtrl*)widget;
		if(sparam == BY_NUMBER){
			g_ds_CondSet1(pme->hdb, CONTACT_DBFIELD_NUMBER, DBREL_HAS, (char *)lparam);
			g_ds_Query(pme->hdb, FALSE);
			wgt_enable_attr(widget, CTRL_ATTR_MODIFIED);
		}
	}
}



