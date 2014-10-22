#include "pack_AmpManager.hi"


static void	CmdListView(t_HMenu handle, void *data)
{
	t_HList list = data;
	if(g_object_valid(list)){
		t_HWindow wnd = wgt_parent_window(list);
		listctrl_set_viewtype(list, GUI_VIEWTYPE_NORMAL);
		listctrl_set_rows(list, 0, prod_ui_list_item_height(), 0);
		listctrl_set_cols(list, 1, 0);
		listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM);
		sk_set_popup(wnd_get_skbar(wnd), SK_LSK, wnd_get_widget(wnd, RESID_MENU_LISTVIEW));
		wgt_redraw(list, FALSE);
	}
}

static void	CmdCellView(t_HMenu handle, void *data)
{
	t_HList list = data;
	if(g_object_valid(list)){
		t_HWindow wnd = wgt_parent_window(list);
		listctrl_set_viewtype(list, GUI_VIEWTYPE_ICON_TEXT_BOTTOM);
		listctrl_set_rows(list, 0, 60, 60);
		listctrl_set_cols(list, 0, 80);
		listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE);
		sk_set_popup(wnd_get_skbar(wnd), SK_LSK, wnd_get_widget(wnd, RESID_MENU_CELLVIEW));
		wgt_redraw(list, FALSE);
	}
}

static void	CmdRun(t_HMenu handle, void *data)
{
	t_HList list = data;
	DSInstall_run(listctrl_get_datasource(list), listctrl_get_cursor(list));
}
static void	CmdUninstall(t_HMenu handle, void *data)
{
	t_HList list = data;
	DSInstall_uninstall(listctrl_get_datasource(list), listctrl_get_cursor(list));
}

static void	CmdOpenFile(t_HMenu handle, void *data)
{
	t_HList list = data;
	char fname[AMOS_PATH_MAX+1];
	if(FileDlg_Show(fname, sizeof(fname), "x:", "*.amp", OFD_FILE_OPEN)){
		AmpManager_ShowInfoWindow((const char*)fname);
	}
}


static t_MenuItem miCellViewOptions[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_LISTVIEW, CmdListView),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_RUN, CmdRun),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_UNINSTALL, CmdUninstall),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_OPEN, CmdOpenFile),
	MITEM_END
};

static t_MenuItem miListViewOptions[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_CELLVIEW, CmdCellView),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_RUN, CmdRun),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_UNINSTALL, CmdUninstall),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_OPEN, CmdOpenFile),
	MITEM_END
};

error_t	AmpManager_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_AmpManagerData *glob = app_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			{
				t_HList list;
				t_HMenu menu;
			
				wgt_enable_attr(handle, WND_ATTR_DEFAULT);
				list = (t_HList)wnd_get_widget(handle, RESID_LIST_PACK);
				if(g_object_valid(list)){
					gui_rect_t rc;
					t_HDataSource ds;
					wgt_get_client_rect((t_HWidget)handle, &rc);
					wgt_set_bound((t_HWidget)list, 0, 0, rc.dx, rc.dy);
			
					ds = DSInstall_new();
					listctrl_attach_datasource(list, ds);
					g_object_unref((t_HObject)ds);
			
					listctrl_set_viewtype(list, GUI_VIEWTYPE_NORMAL);
					listctrl_set_rows(list, 0, prod_ui_list_item_height(), 0);
					listctrl_set_cols(list, 1, 0);
					listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM);
				}
				menu = wnd_get_widget(handle, RESID_MENU_LISTVIEW);
				if(g_object_valid(menu)){
					menu_bind(menu, miListViewOptions, list);
				}
				menu = wnd_get_widget(handle, RESID_MENU_CELLVIEW);
				if(g_object_valid(menu)){
					menu_bind(menu, miCellViewOptions, list);
				}
#if defined(FTR_SOFT_KEY)
				{
					t_SKHandle sk;
					sk = wnd_get_skbar(handle);
					if(g_object_valid(sk)){
						sk_set_text(sk, SK_LSK, "Options", RESID_STR_OPTION);
						sk_set_popup(sk, SK_LSK, wnd_get_widget(handle, RESID_MENU_LISTVIEW));
			
						sk_set_text(sk, SK_RSK, "Back", RESID_STR_BACK);
					}
				}
#else
#endif
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
		case WIDGET_OnMessage:
			{
				t_AmosMsg *msg = (t_AmosMsg *)lparam;
				t_HList list;
				switch(msg->evt){
					case MSG_AMP_REFRESH:
						list = (t_HList)wnd_get_widget(handle, RESID_LIST_PACK);
						if(g_object_valid(list)){
							listctrl_refresh_datasource(list, TRUE);
						}
						ret = SUCCESS;
						break;
				}
			}
	}
	return ret;
}

