#include "pack_desktop.hi"

static error_t	__on_win_open(t_HWindow handle)
{
	t_HImage image;
	t_HList gallery;
	t_HDataSource ds;

	wgt_enable_attr(handle, WND_ATTR_HAS_STATUS|WND_ATTR_IS_DESKTOP);
	
	ds = DSAppStore_New(NULL, NULL);
	AppStore_AddByMenuTree(DSAppStore_GetRootNode(ds), prod_menu_tree_open(NULL, "/"));

	gallery = wnd_get_widget(handle, RESID_LIST_MAIN_MENU);
	listctrl_attach_datasource(gallery, ds);
	g_object_unref((t_HObject)ds);

	listctrl_set_flags(gallery, CLFLAG_SCROLLING_HORIZONTAL|CLFLAG_SCROLLING_PAGE|CLFLAG_HILIGHT_SHOW_MASK|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE);
	listctrl_set_viewtype(gallery, GUI_VIEWTYPE_ICON_TEXT_BOTTOM);
	listctrl_set_rows(gallery, 0, 71, 0);
	listctrl_set_cols(gallery, 4, 0);

	gallery = wnd_get_widget(handle, RESID_LIST_SHORTCUT);
	if(g_object_valid(gallery)){
		ds = DSAppStore_New(NULL, NULL);
		AppStore_AddByMenuTree(DSAppStore_GetRootNode(ds), prod_shortcut_open());
		listctrl_attach_datasource(gallery, ds);
		g_object_unref((t_HObject)ds);

		listctrl_set_flags((t_HList)gallery, CLFLAG_HILIGHT_SHOW_MASK|CLFLAG_DISABLE_VLINE|CLFLAG_DISABLE_HLINE);
		listctrl_set_viewtype(gallery, GUI_VIEWTYPE_ICON_TEXT_BOTTOM);
		listctrl_set_rows(gallery, 0, 71, 0);
		listctrl_set_cols(gallery, 4, 0);
	}
	
	return SUCCESS;
}

error_t	desktop_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t handled = NOT_HANDLED;
	t_DesktopGlobData *glob = task_get_data();

	switch(evt){
		case WINDOW_OnOpen:
			handled = __on_win_open(handle);
			break;
		case WINDOW_OnClose:
			handled = SUCCESS;
			break;
		case WINDOW_OnDraw:
			g_printf("idle:WINDOW_OnDraw");
			#if 0
			timer_start(glob->timer_id, 1000, TIMER_MODE_ONCE);
			__idle_main_draw_datetime(handle, TRUE);
			#endif
			break;
	}

	return handled;
}

