
#if 0	
#define DLL_EXPORT_explorer
#endif

#include "pack_explorer.hi"
#include "pack_sys.h"
#include "wgt_combobox.h"
#include "ds_explorer.h"

//1 ==================【File Explorer Control】==================


//2 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

static void __explorer_show_cwd(t_Widget *handle, gboolean defer);


enum {
	STORAGE_PHONE,
	STORAGE_TFLASH,

	STORAGE_COUNT
};
enum {
	VIEW_LIST,
	VIEW_ICON,

	VIEW_N
};

static const char *gaStorage[STORAGE_COUNT] = {"C:", "X:"};

static error_t	__combo_storage_on_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_HWidget dentry = wgt_get_parent(handle);
	t_HWidget listctrl = wgt_get_child(dentry, RESID_DENTRY_LIST, FALSE);
	if(!g_object_valid(listctrl))return SUCCESS;
	switch(evt){
		case WIDGET_OnSelChanged:
			if(lparam < STORAGE_COUNT){
				t_HDataSource ds = listctrl_get_datasource(listctrl);
				char *path = DSExplorer_GetPath(ds);
				if(path && toupper(*path) != gaStorage[lparam][0])
					DSExplorer_SetPath(ds, gaStorage[lparam]);
				wgt_set_focus(listctrl, TAB_NONE);
				listctrl_refresh_datasource(listctrl, TRUE);
				__explorer_show_cwd(dentry, FALSE);
				ret = SUCCESS;
			}
			break;
	}
	return ret;
}

static error_t	__combo_view_on_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_HWidget dentry = wgt_get_parent(handle);
	t_HWidget listctrl = wgt_get_child(dentry, RESID_DENTRY_LIST, FALSE);
	switch(evt){
		case WIDGET_OnSelChanged:
			switch(lparam){
				case VIEW_LIST:
					listctrl_set_viewtype(listctrl, DENTRY_VIEWTYPE_LIST);
					listctrl_set_rows(listctrl, 0, prod_ui_list_item_height(), 0);
					listctrl_set_cols(listctrl, 1, 0);
					listctrl_set_flags(listctrl, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE|CLFLAG_AUTOTAB_LR|CLFLAG_AUTOTAB_UD);
					wgt_set_focus(listctrl, TAB_NONE);
					wgt_redraw(listctrl, FALSE);
					break;
				case VIEW_ICON:
					listctrl_set_viewtype(listctrl, DENTRY_VIEWTYPE_ICON);
					#if (LCD_WIDTH==128)
					listctrl_set_rows(listctrl, 3, 0, 0);
					listctrl_set_cols(listctrl, 3, 0);
					#else
					listctrl_set_rows(listctrl, 4, 0, 0);
					listctrl_set_cols(listctrl, 4, 0);
					#endif
					listctrl_set_flags(listctrl, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE|CLFLAG_AUTOTAB_LR|CLFLAG_AUTOTAB_UD);
					wgt_set_focus(listctrl, TAB_NONE);
					wgt_redraw(listctrl, FALSE);
					break;
			}
			ret = SUCCESS;
			break;
	}
	return ret;
}

static void	__explorer_ms_on_select(t_HDataSource handle, const char *guid, t_AmsModuleId module, void *userdata)
{
	t_HDataSource ds_explorer = userdata;
	const char *path = DSExplorer_GetPath(ds_explorer);
	const char *fname = DSExplorer_GetFname(ds_explorer);
	char *fullname = MALLOC(strlen(path)+1+strlen(fname)+1);
	sprintf(fullname, "%s/%s", path, fname);
	pack_call(guid, module, fullname, strlen(fullname)+1, FALSE, NULL);
	FREE(fullname);
}

static error_t __explorer_on_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnKeyEvent:
			if(lparam == MVK_OK && sparam == MMI_KEY_LONG_PRESS){
				t_HDataSource ds = listctrl_get_datasource(handle);
				const char *path = DSExplorer_GetPath(ds);
				const char *fname = DSExplorer_GetFname(ds);
				dentry_stat_t stat;
				char *fullname;

				if(!path || !fname || *path == 0 || *fname == 0)
					break;
				fullname = MALLOC(strlen(path)+1+strlen(fname)+1);
				sprintf(fullname, "%s/%s", path, fname);
				if(fsi_stat(fullname, &stat) == SUCCESS){
					t_HWindow parent = wgt_get_parent(handle);
					if(stat.mode & F_MODE_DIR){
						wgt_popup(handle, wgt_get_child(parent, RESID_DENTRY_MENU_DIROPT, FALSE), 0, 0, guiRight);
					}else{
						t_HWidget popup;
						char mime[MIME_NAME_MAX+1];
						char *p;
					
						strcpy(mime, "file/");
						p = strrchr(fname, '.');
						if(p)
							strcat(mime, p+1);

						popup = wgt_get_child(parent, RESID_DENTRY_OPEN, FALSE);
						listctrl_attach_mime_method(popup, mime, METHOD_NAME_OPEN, __explorer_ms_on_select, ds);

						wgt_popup(handle, wgt_get_child(parent, RESID_DENTRY_MENU_FILEOPT, FALSE), 0, 0, guiRight);
					}
					ret = SUCCESS;
				}
				FREE(fullname);
			}
			break;
		default:
			break;
	}
	return ret;
}

static void	__dentry_on_change(t_HWidget handle, const char *path)
{
	__explorer_show_cwd(wgt_get_parent(handle), FALSE);
}

#define BTN_COLOR_1 MAKE_RGB(0x23,0x41,0x56)
#define BTN_COLOR_2 MAKE_RGB(0x67,0x9a,0xc0)

#define	CAPTION_H	20

#define	COMBO_S_WIDTH		40
#define	COMBO_V_WIDTH		40

static void __explorer_show_cwd(t_Widget *handle, gboolean defer)
{
	t_HGDI gdi = wgt_get_gdi(handle);
	coord_t x = COMBO_S_WIDTH;
	coord_t y = 0;
	gu16 dx = wgt_bound_dx(handle)-COMBO_S_WIDTH-COMBO_V_WIDTH;
	gu16 dy = CAPTION_H;
	int w;
	
	char *title = DSExplorer_GetPath(listctrl_get_datasource(wgt_get_child(handle, RESID_DENTRY_LIST, FALSE)));

	if(!defer)
		gdi_clear(gdi, x, y, dx, dy, guiNoCorner);
	//2 skip "X:", start with '/'
	title += 2;
	//2 不显示全路径
	title = strrchr(title, '/');
	if(title)
		title++;
	else
		title = "/";
	gdi_set_color(gdi, RGB_WHITE);
	gdi_measure_text(gdi, title, -1, CHARSET_UTF8, -1, &w, NULL, NULL);
	gdi_draw_text_in(gdi, x, y, dx, dy, title, -1, CHARSET_UTF8, IDF_HALIGN_BESTFIT|IDF_VALIGN_MIDDLE);

	if(!defer)
		gdi_blt(gdi, x, y, dx, dy);
	g_object_unref(gdi);
}

static void	CmdOpenDir(t_HMenu handle, void *data)
{
	t_HWidget dentry = data;
	if(g_object_valid(dentry)){
		wgt_send_msg(dentry, EVT_SELECT, 0, 0, 0);
	}
}
static void	CmdRename(t_HMenu handle, void *data)
{
	t_HWidget dentry = data;
	if(g_object_valid(dentry)){
		msgbox_show(NULL, _LSK(OK), 0, 0, "Not finished!");
	}
}
static void	CmdBack(t_HMenu handle, void *data)
{
	t_HWidget dentry = data;
	wnd_send_close(wgt_parent_window(handle), 0);
}

static t_MenuItem miDirOperation[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_OPEN, CmdOpenDir),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_RENAME, CmdRename),
	MITEM_END
};

static t_MenuItem miFileOperation[] = {
	MITEM_SUBMENU(RESID_UNDEF, RESID_STR_OPEN, NULL),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_RENAME, CmdRename),
	MITEM_END
};

static error_t		__ctrl_OnCreate(t_Widget* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	t_HWidget widget, menu;
	t_HDataSource hds;
	//2 NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP|CTRL_ATTR_CONTAINER);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	wgt_set_bg_fill_solid(pme, BTN_COLOR_2);
	
	//2 Listctrl: dentry----放在最前定义是为了默认焦点
	widget = wgt_create((t_HWidget)pme, "listctrl");
	wgt_set_resid(widget, RESID_DENTRY_LIST);
	wgt_set_border(widget, BORDER_TYPE_FRAME, 1, 0);
	wgt_set_bg_fill_solid(widget, ui_theme_get()->bg_color);

	listctrl_attach_explorer(widget, NULL, NULL, DENTRY_FLAG_DEFAULT, NULL, __dentry_on_change);
	listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_AUTOTAB_LR|CLFLAG_AUTOTAB_UD);
	wgt_hook_register(widget, WIDGET_OnKeyEvent, (t_WidgetHookFunc)__explorer_on_hook, NULL);

	//2 ComboBox: storage
	widget = wgt_create((t_HWidget)pme, "combobox");
	wgt_set_resid(widget, RESID_DENTRY_COMBO_STORAGE);
	wgt_set_border(widget, BORDER_TYPE_FRAME, 1, 0);
	wgt_hook_register(widget, WIDGET_OnSelChanged, (t_WidgetHookFunc)__combo_storage_on_hook, NULL);

	hds = listctrl_get_datasource(combobox_get_listctrl(widget));
	DSIconText_SetAlign(hds, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
	DSIconText_NewItem(hds);
	DSIconText_NewItem(hds);
	DSIconText_SetItemText(hds, STORAGE_PHONE, IDENT_BY_POSITION, NULL, brd_get(PACK_GUID_SELF), RESID_STR_PHONE);
	DSIconText_SetItemText(hds, STORAGE_TFLASH, IDENT_BY_POSITION, NULL, brd_get(PACK_GUID_SELF), RESID_STR_TFLASH);
	
	combobox_set_cursor(widget, STORAGE_PHONE);

	//2 ComboBox: View
	widget = wgt_create((t_HWidget)pme, "combobox");
	wgt_set_resid(widget, RESID_DENTRY_COMBO_VIEW);
	wgt_set_border(widget, BORDER_TYPE_FRAME, 1, 0);
	wgt_hook_register(widget, WIDGET_OnSelChanged, (t_WidgetHookFunc)__combo_view_on_hook, NULL);

	hds = listctrl_get_datasource(combobox_get_listctrl(widget));
	DSIconText_SetAlign(hds, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
	DSIconText_NewItem(hds);
	DSIconText_NewItem(hds);
	DSIconText_SetItemText(hds, VIEW_LIST, IDENT_BY_POSITION, NULL, brd_get(PACK_GUID_SELF), RESID_STR_LIST_VIEW);
	DSIconText_SetItemText(hds, VIEW_ICON, IDENT_BY_POSITION, NULL, brd_get(PACK_GUID_SELF), RESID_STR_ICON_VIEW);
	
	combobox_set_cursor(widget, VIEW_LIST);

	//2 Menus	
	widget = wgt_create((t_HWidget)pme, "listctrl");
	wgt_set_resid(widget, RESID_DENTRY_OPEN);
	wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
	listctrl_set_flags((t_HList)widget, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM|CLFLAG_AUTO_RESIZE);

	menu = wgt_create((t_HWidget)pme, "menu");
	wgt_set_resid(menu, RESID_DENTRY_MENU_DIROPT);
	menu_bind(menu, miDirOperation, pme);
	wgt_disable_attr(menu, CTRL_ATTR_VISIBLE);
	menu_SetItemPopup(menu, RESID_STR_OPEN, IDENT_BY_ID, widget);
	
	menu = wgt_create((t_HWidget)pme, "menu");
	wgt_set_resid(menu, RESID_DENTRY_MENU_FILEOPT);
	menu_bind(menu, miFileOperation, pme);
	wgt_disable_attr(menu, CTRL_ATTR_VISIBLE);
	menu_SetItemPopup(menu, RESID_STR_OPEN, IDENT_BY_ID, widget);
	
	return SUCCESS;
}

static void	__ctrl_OnDestroy(t_Widget* pme)
{
	g_object_inherit_destroy(pme);
}

static error_t __ctrl_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_Widget *pme = (t_Widget *)handle;
	switch(evt){
		case WIDGET_OnDraw:
			if(wgt_test_attr(pme, CTRL_ATTR_MODIFIED)){
				t_HWidget child;
				int i;
				for(i = 0; i < g_vector_size(handle->vect_child); i++){
					child = g_vector_element_at(handle->vect_child, i);
					switch(wgt_get_resid(child)){
						case RESID_DENTRY_COMBO_STORAGE:
							wgt_set_bound(child, 1, 1, COMBO_S_WIDTH-2, CAPTION_H-2);
							break;
						case RESID_DENTRY_COMBO_VIEW:
							wgt_set_bound(child, wgt_bound_dx(handle)-COMBO_V_WIDTH+1, 1, COMBO_V_WIDTH-2, CAPTION_H-2);
							break;
						case RESID_DENTRY_LIST:
							wgt_set_bound(child, 0, CAPTION_H, wgt_bound_dx(handle), wgt_bound_dy(handle)-CAPTION_H);
							break;
					}
				}
			}
			container_redraw(handle, TRUE);

			__explorer_show_cwd(handle, TRUE);
			
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


static VTBL(widget) explorerVtbl = {
	(PFNONCREATE)__ctrl_OnCreate
	, (PFNONDESTROY)__ctrl_OnDestroy

	, __ctrl_OnEvent
	, NULL
};

CLASS_DECLARE(explorer, t_Widget, explorerVtbl);


// TODO: add functions for your own widget.  You MUST export them in dll.def

void		wgt_explorer_set(t_HWidget handle, const char *path, const char *filter, gu32 flags, f_DentrySelect select)
{
	if(g_object_valid(handle) && g_object_class(handle) == &class_explorer){
		t_HWidget widget;
		t_HDataSource ds;
		
		widget = wgt_get_child(handle, RESID_DENTRY_LIST, FALSE);
		ds = listctrl_get_datasource(widget);
		DSExplorer_Bind(ds, select, __dentry_on_change);
		DSExplorer_SetPath(ds, path);
		DSExplorer_SetFilter(ds, filter, flags);
		
		path = DSExplorer_GetPath(ds);
		
		widget = wgt_get_child(handle, RESID_DENTRY_COMBO_STORAGE, FALSE);
		wgt_enable_attr(widget, CTRL_ATTR_FROZEN);
		if(*path == 'C' || *path == 'c')
			combobox_set_cursor(widget, STORAGE_PHONE);
		else if(*path == 'X' || *path == 'x')
			combobox_set_cursor(widget, STORAGE_TFLASH);
		wgt_disable_attr(widget, CTRL_ATTR_FROZEN);
		
		wgt_redraw(handle, FALSE);
	}
}

#undef PARENT_CLASS



