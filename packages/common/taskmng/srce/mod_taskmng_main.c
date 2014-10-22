#include "pack_taskmng.hi"

static void	CmdCellView(t_HWidget handle, void *data);

static void	CmdListView(t_HWidget handle, void *data)
{
	t_HList list = data;
	if(g_object_valid(list)){
		t_HWindow wnd = wgt_parent_window(list);

		listctrl_set_viewtype(list, GUI_VIEWTYPE_NORMAL);
		listctrl_set_rows(list, 0, prod_ui_list_item_height(), 0);
		listctrl_set_cols(list, 1, 0);
		listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM);
#if defined(FTR_NO_INPUT_KEY)
		sk_set_text(handle, SK_LSK, NULL, RESID_STR_CELLVIEW);
		sk_set_callback(handle, SK_LSK, CmdCellView, data);
		wgt_redraw(handle, FALSE);
#else
		sk_set_popup(wnd_get_skbar(wnd), SK_LSK, wnd_get_widget(wnd, RESID_MENU_LISTVIEW));
#endif
		wgt_client_origin_x(list) = 0;
		wgt_client_origin_y(list) = 0;
		wgt_redraw(list, FALSE);
	}
}

static void	CmdCellView(t_HWidget handle, void *data)
{
	t_HList list = data;
	if(g_object_valid(list)){
		t_HWindow wnd = wgt_parent_window(list);

		listctrl_set_viewtype(list, GUI_VIEWTYPE_ICON);
		listctrl_set_item_margin(list, 0, 0, 0, 2);
#if defined(FTR_IPHONE_STYLE)
		listctrl_set_rows(list, 0, 68, 0);
		listctrl_set_cols(list, 0, 60);
#elif (LCD_WIDTH == 240) && (LCD_HEIGHT == 320)
		listctrl_set_rows(list, 0, 68, 0);
		listctrl_set_cols(list, 0, 60);
#elif (LCD_WIDTH == 320) && (LCD_HEIGHT == 240)
		listctrl_set_rows(list, 0, 60, 0);
		listctrl_set_cols(list, 0, 68);
#elif (LCD_WIDTH == 220) && (LCD_HEIGHT == 176)
		listctrl_set_rows(list, 0, 36, 0);
		listctrl_set_cols(list, 0, 36);
#elif (LCD_WIDTH == 176) && (LCD_HEIGHT == 220)
		listctrl_set_rows(list, 4, 0, 0);
		listctrl_set_cols(list, 4, 0);
#elif (LCD_WIDTH == 128) && (LCD_HEIGHT == 160)
		listctrl_set_rows(list, 3, 0, 0);
		listctrl_set_cols(list, 3, 0);
#endif

#if defined(FTR_IPHONE_STYLE)
		listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE|CLFLAG_HILIGHT_SHOW_MASK|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE);
#else
		listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE);
#endif
#if defined(FTR_NO_INPUT_KEY)
		sk_set_text(handle, SK_LSK, NULL, RESID_STR_LISTVIEW);
		sk_set_callback(handle, SK_LSK, CmdListView, data);
		wgt_redraw(handle, FALSE);
#else
		sk_set_popup(wnd_get_skbar(wnd), SK_LSK, wnd_get_widget(wnd, RESID_MENU_CELLVIEW));
#endif
		wgt_client_origin_x(list) = 0;
		wgt_client_origin_y(list) = 0;
		wgt_redraw(list, FALSE);
	}
}

static void	CmdActivate(t_HWidget handle, void *data)
{
	t_HList list = data;
	DSTask_activate(listctrl_get_datasource(list), listctrl_get_cursor(list));
}

static void	CmdKill(t_HWidget handle, void *data)
{
	t_HList list = data;
	DSTask_kill(listctrl_get_datasource(list), listctrl_get_cursor(list));
}

#if !defined(FTR_NO_INPUT_KEY)
static t_MenuItem miCellViewOptions[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_LISTVIEW, CmdListView),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_ACTIVATE, CmdActivate),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_KILL, CmdKill),
	MITEM_END
};

static t_MenuItem miListViewOptions[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_CELLVIEW, CmdCellView),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_ACTIVATE, CmdActivate),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_KILL, CmdKill),
	MITEM_END
};
#endif

#if defined(FTR_NO_INPUT_KEY)
#define LINE_HEIGHT		prod_ui_list_item_height()
static error_t	__list_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnPenEvent:
			{
				coord_t x0, y0;
				int index;
				PEN_XY(lparam, x0, y0);
				if(VIEWTYPE_IS_ICON(listctrl_get_viewtype(handle)))
					break;
				switch(PEN_TYPE(lparam)){
					case MMI_PEN_DOWN:
						index = listctrl_index_by_point(handle, &x0, &y0, NULL, NULL);
						if(index >= 0){
							gui_rect_t rc;
							wgt_get_client_rect(handle, &rc);
							if(x0 >= rc.dx-LINE_X_MARGIN-LINE_HEIGHT){
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
							wgt_set_tag(handle, 0);
							wgt_enable_attr(handle, CTRL_ATTR_HSCROLLABLE);
							wgt_enable_attr(handle, CTRL_ATTR_VSCROLLABLE);
							DSTask_kill(listctrl_get_datasource(handle), index);
							ret = SUCCESS;
						}
						break;
					case MMI_PEN_PRESS:
						break;
				}
			}	
			break;
	}
	return ret;
}
#endif

void	__win_open(t_HWindow handle)
{
	t_HList list;
	t_HMenu menu;
	t_HDataSource ds;
	gui_rect_t rc;

	wgt_enable_attr(handle, WND_ATTR_DEFAULT);
#if defined(FTR_IPHONE_STYLE)
	wgt_set_bg_fill_solid(handle, MAKE_RGB(192,192,255));
#endif
	list = (t_HList)wnd_get_widget(handle, RESID_LIST_TASK);
	if(g_object_valid(list)){
		wgt_get_client_rect((t_HWidget)handle, &rc);
		wgt_set_bound((t_HWidget)list, 0, 0, rc.dx, rc.dy);
#if defined(FTR_IPHONE_STYLE)
		wgt_set_bg_fill_solid(list, MAKE_RGB(192,192,255));
#endif
		
		ds = DSTask_new();
		listctrl_attach_datasource(list, ds);
		g_object_unref((t_HObject)ds);

		listctrl_set_viewtype(list, GUI_VIEWTYPE_NORMAL);
		listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM);
#if defined(FTR_NO_INPUT_KEY)
		wgt_hook_register(list, WIDGET_OnPenEvent, __list_hook, NULL);
#endif
	}

#if !defined(FTR_NO_INPUT_KEY)
	menu = wnd_get_widget(handle, RESID_MENU_LISTVIEW);
	if(g_object_valid(menu)){
		menu_bind(menu, miListViewOptions, list);
	}
	menu = wnd_get_widget(handle, RESID_MENU_CELLVIEW);
	if(g_object_valid(menu)){
		menu_bind(menu, miCellViewOptions, list);
	}
#endif

#if defined(FTR_SOFT_KEY)
	{
		t_SKHandle sk = wnd_get_skbar(handle);
		if(g_object_valid(sk)){
#if defined(FTR_NO_INPUT_KEY)
			sk_set_text(sk, SK_LSK, NULL, RESID_STR_CELLVIEW);
			sk_set_callback(sk, SK_LSK, CmdCellView, list);
#else
			sk_set_text(sk, SK_LSK, "Options", RESID_STR_OPTION);
			sk_set_popup(sk, SK_LSK, wnd_get_widget(handle, RESID_MENU_LISTVIEW));
#endif
			sk_set_text(sk, SK_RSK, "Back", RESID_STR_BACK);
		}
	}
#endif
}

error_t	taskmng_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			__win_open(handle);
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			g_printf("EVT_WND_PAINT\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_PRESS && lparam == MVK_END){
				app_end(TID_SELF, 0);
				ret = SUCCESS;
			}
			break;
		case WINDOW_OnMessage:
			{
				t_AmosMsg *msg = (t_AmosMsg *)lparam;
				switch(sparam){
					case EVT_APP_NOTIFY:
					{
						// sparam: tid; lparam: state
						t_HList list = (t_HList)wnd_get_widget(handle, RESID_LIST_TASK);
						t_HDataSource ds = listctrl_get_datasource(list);
						if(msg->lparam == TASK_STATE_NONE){
							DSTask_remove(ds, msg->sparam);
						}else if(msg->lparam == TASK_STATE_READY){
							DSTask_add(ds, msg->sparam);
						}
						ret = SUCCESS;
					}
						break;
				}
			}
	}
	return ret;
}

