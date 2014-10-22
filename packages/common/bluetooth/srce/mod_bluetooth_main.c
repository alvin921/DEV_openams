#include "pack_bluetooth.hi"

#if defined(FTR_IPHONE_STYLE)
#define	X_OFFSET	5
#define	Y_OFFSET	5
#else
#define	X_OFFSET	0
#define	Y_OFFSET	0
#endif


//2 refer to ------support_profile_tbl[]
const t_BtProfileID ga_supported_profiles[] = {
	BT_UUID_HFP_AG,
	BT_UUID_A2DP,
	BT_UUID_AVRCP,
};

void	bt_evt_callback(t_sparam sparam, t_lparam lparam, void *userdata)
{
	bluetooth_glob_t *glob = app_get_data();
	t_HWindow wnd = userdata;
	t_HWidget listctrl = wnd_get_widget(wnd, RESID_LIST_DEVICE);
	switch(sparam){
		case BT_IND_PROGRESS:
			if(lparam >= 100)
				msgbox_show(NULL, 0, 1000, 0, brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_DONE, ""));
			else
				msgbox_show(NULL, 0, 0, 0, brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_PLEASE_WAIT, ""));
			break;
		case BT_IND_INQUIRY_BEGIN:
			wgt_set_caption_by_resid(wnd_get_widget(wnd, RESID_BTN_SEARCH), brd_get(PACK_GUID_SELF), RESID_STR_BT_CANCEL_SEARCH);
			wgt_redraw(wnd_get_widget(wnd, RESID_BTN_SEARCH), FALSE);
			listctrl_set_count(listctrl, bt_dev_count());
			wgt_set_client_origin(listctrl, 0, 0);
			wgt_redraw(listctrl, FALSE);
			break;
		case BT_IND_INQUIRY:
			listctrl_set_count(listctrl, bt_dev_count());
			wgt_redraw(listctrl, FALSE);
			break;
		case BT_IND_INQUIRY_END:
			glob->searching = FALSE;
			wgt_set_caption_by_resid(wnd_get_widget(wnd, RESID_BTN_SEARCH), brd_get(PACK_GUID_SELF), RESID_STR_BT_SEARCH);
			wgt_redraw(wnd_get_widget(wnd, RESID_BTN_SEARCH), FALSE);
			if(lparam){
				;//msgbox_show(NULL, _CSK(OK), "Found <%d> device%s", lparam, lparam>1?"s":"");
				wgt_set_focus(wnd_get_widget(wnd, RESID_LIST_DEVICE), TAB_NONE);
			}else
				msgbox_show(NULL, _CSK(OK), 0, 0, "Device not found!");
			break;
		case BT_IND_PIN:
			//2 底层上来配对请求，暂时默认回复0000
			{
				t_BtIdent id = lparam;
				t_BtDevice *dev = bt_dev_get_info(lparam);
				msgbox_show(NULL, 0, 0, 0, brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_BT_PAIRING, ""));
				bt_dev_pair(id, "0000");
			}
			break;
		case BT_IND_PIN_ACK:
			listctrl_set_count(listctrl, bt_dev_count());
			wgt_redraw(listctrl, FALSE);
			msgbox_show(NULL, 0, 1000, 0, brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_BT_PAIR_OK, ""));
			break;
		case BT_IND_PIN_NCK:
			msgbox_show(NULL, 0, 1000, 0, brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_BT_PAIR_KO, ""));
			break;
		case BT_IND_BOND:
			//2 Bonding成功
			{
				t_BtIdent id = lparam;
				t_BtDevice *dev = bt_dev_get_info(lparam);
				//bt_dev_sdc(id);
			}
			break;
		case BT_IND_SDC_END:
			//2 Service discovery 成功
			{
				t_BtIdent id = lparam;
				t_BtDevice *dev = bt_dev_get_info(lparam);
				//bt_dev_connect(id);
			}
			break;
		case BT_IND_CONNECT:
			listctrl_redraw_item(listctrl, lparam);
			break;
		case BT_IND_CONNECTING:
			msgbox_show(NULL, 0, 0, 0, brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_BT_CONNECTING, ""));
			break;
		case BT_IND_DISCONNECTING:
			msgbox_show(NULL, 0, 0, 0, brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_BT_DISCONNECTING, ""));
			break;
		case BT_IND_DISCONNECT:
			listctrl_redraw_item(listctrl, lparam);
			break;
		case BT_IND_PROF_CONNECT:
		case BT_IND_PROF_DISCONNECT:
			{
					t_BtIdent id = lparam>>16;
					t_BtProfileID profile = lparam&0xffff;
					t_BtDevice *dev = bt_dev_get_info(id);

					if(wgt_get_resid(wnd) == RESID_WIN_MAIN){
						//2 主界面
						char str[100];
						
						if(sparam == BT_IND_PROF_CONNECT){
							sprintf(str, "%s %s", bt_profile_string(profile), brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_BT_CONNECTED, ""));
						}else{
							sprintf(str, "%s %s", bt_profile_string(profile), brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_BT_DISCONNECTED, ""));
						}
						msgbox_show(NULL, 0, 1500, 0, str);
					}else{
						t_ViewBtWData *wdata = (t_ViewBtWData *)wgt_get_tag(wnd);
						int index;
						for(index = 0; index < dev->n_profiles; index++){
							if(dev->profiles[index] == profile){
								listctrl_redraw_item(wnd_get_widget(wnd, RESID_BTVIEW_LIST_PROFILE), index);
								break;
							}
						}
						listctrl_redraw_item(wnd_get_widget(wnd, RESID_BTVIEW_LIST_OVERVIEW), LIST_IDX_STATE);
						//if(wdata->id == BT_IDENT_SELF){
						//}
						if(sparam == BT_IND_PROF_CONNECT){
							t_HWidget btn = wnd_get_widget(wnd, RESID_BTVIEW_BTN_DISCONNECT);
							if(!wgt_is_enabled(btn)){
								wgt_enable_attr(btn, CTRL_ATTR_ENABLED);
								wgt_redraw(btn, FALSE);
							}
						}else if(wdata->id != BT_IDENT_SELF){
							t_HWidget btn = wnd_get_widget(wnd, RESID_BTVIEW_BTN_CONNECT);
							if(!wgt_is_enabled(btn)){
								wgt_enable_attr(btn, CTRL_ATTR_ENABLED);
								wgt_redraw(btn, FALSE);
							}
							if(!BIT_TEST(dev->state, BTDEV_STATE_CONNECTED)){
								btn = wnd_get_widget(wnd, RESID_BTVIEW_BTN_DISCONNECT);
								wgt_disable_attr(btn, CTRL_ATTR_ENABLED);
								wgt_redraw(btn, FALSE);
							}
						}
					}
			}
			break;
	}
}

static void	__btview_callback(t_sparam sparam, t_lparam lparam, void *userdata)
{
	bluetooth_glob_t *glob = app_get_data();
	t_HWindow wnd = userdata;
	switch(sparam){
		case BT_IND_PROF_CONNECT:
		case BT_IND_PROF_DISCONNECT:
			{
				t_BtIdent id = lparam>>16;
				t_BtProfileID profile = lparam&0xffff;
				t_BtDevice *dev = bt_dev_get_info(id);
			}
			break;
	}
}

static error_t	__widget_on_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_HWindow wnd = userdata;
	bluetooth_glob_t *glob = app_get_data();
	t_HWidget widget;
	switch(evt){
		case WIDGET_OnSelected:
			switch(wgt_get_resid(handle)){
				case RESID_ONOFF_POWER:
					if(onoff_get_value(handle)){
						//2 power on
						//1 BT power on sequence:
						/*
						1. IOCMD_BT_ON()
						2. IOCMD_BT_SET_SCAN_MODE(0)
						3. IOCMD_BT_GET_ADDR()
						4. IOCMD_BT_SET_NAME()
						5. IOCMD_BT_SET_AUTH(TRUE)
						6. IOCMD_BT_PROFILE_ACTIVATE()	// all profiles
						7. IOCMD_BT_SET_SCAN_MODE(BT_PAGE_SCAN_ON|BT_INQUIRY_SCAN_ON)
						8. allow ACL link
						9. inquiry
						*/
						if(bt_power_on(TRUE) == SUCCESS){
							widget = wnd_get_widget(wnd, RESID_BTN_SEARCH);
							wgt_enable_attr(widget, CTRL_ATTR_ENABLED);
							widget = wnd_get_widget(wnd, RESID_ONOFF_VISIBLE);
							wgt_enable_attr(widget, CTRL_ATTR_ENABLED);
							//wgt_redraw(widget, FALSE);
						}
					}else{
						//2 power off
						bt_power_on(FALSE);
						widget = wnd_get_widget(wnd, RESID_ONOFF_VISIBLE);
						wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
						wgt_freeze(widget);
						onoff_set_value(widget, FALSE);
						wgt_unfreeze(widget);
						wgt_redraw(widget, FALSE);
					}
					ret = SUCCESS;
					break;
				case RESID_ONOFF_VISIBLE:
					bt_set_visible(onoff_get_value(handle));
					break;
				case RESID_BTN_SEARCH:
					if(!bt_is_power_on())
						break;
					if(glob->searching){
						//2 cancel searching
						bt_inquiry_end(TRUE);
						glob->searching = FALSE;
						//wgt_set_caption_by_resid(handle, brd_get(PACK_GUID_SELF), RESID_STR_SEARCH);
						//wgt_redraw(handle, FALSE);
					}else{
						//2 do searching
						glob->searching = TRUE;
						//wgt_set_caption_by_resid(handle, brd_get(PACK_GUID_SELF), RESID_STR_CANCEL_SEARCH);
						//wgt_redraw(handle, FALSE);
						bt_inquiry(BT_DEVICE_MAX, 60, BT_COD_INQUIRY_ANY);
					}
					break;
			}
			break;
	}
}

#define LINE_X_MARGIN	2
#define LINE_HEIGHT		prod_ui_list_item_height()

static void	__inquiry_list_redraw(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	t_HWidget listctrl = userdata;
	bluetooth_glob_t *glob = task_get_data();
	t_BtIdent id = bt_dev_get_by_index(index);
	t_BtDevice *dev = bt_dev_get_info(id);

	
	const char *name;
	t_HImage icon = NULL;
	int x;

	icon = bt_get_cod_icon(dev->cod);
		
	name = dev->name;
	if(g_object_valid(icon)){
		gdi_draw_image_in(gdi, 0, 0, height, height, icon, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE | IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
	}
	g_object_unref(icon);

	width -= height+2;
	x = height+2;
	gdi_draw_text_in(gdi, x, 0, width, height, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);

	name = bt_get_state_string(dev->state);
	gdi_draw_text_in(gdi, x, 0, width-height, height, name, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
	if(prod_ui_ts_support()){
		icon = brd_get_image(brd_get(PACK_GUID_SELF), RESID_ICON_RARROW);
		gdi_draw_image_in(gdi, x+width-LINE_X_MARGIN-height, 0, height, height, icon, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
		g_object_unref(icon);
	}
}

static gboolean	__show_btdev_detail(int index)
{
	if(index >= 0){
		t_BtIdent id = bt_dev_get_by_index(index);
		t_BtDevice *dev = bt_dev_get_info(id);
		if(BIT_TEST(dev->state, BTDEV_STATE_PAIRED))
			wnd_show(wnd_create(bluetooth_ViewWndMsgHandler, id, 0));
		return TRUE;
	}
	return FALSE;
}

static error_t	__listctrl_on_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	bluetooth_glob_t *glob = app_get_data();
	t_HWindow wnd = userdata;
	switch(evt){
		case WIDGET_OnSelected:
			if(glob->searching){
				//2 cancel searching
				t_HWidget btn = wnd_get_widget(wnd, RESID_BTN_SEARCH);
				bt_inquiry_end(TRUE);
				glob->searching = FALSE;
				wgt_set_caption_by_resid(btn, brd_get(PACK_GUID_SELF), RESID_STR_BT_SEARCH);
				wgt_redraw(btn, FALSE);
			}
			{
				t_BtIdent id = bt_dev_get_by_index(sparam);
				t_BtDevice *dev = bt_dev_get_info(id);
				if(dev){
					if(BIT_TEST(dev->state, BTDEV_STATE_CONNECTED))
						;
					else if(BIT_TEST(dev->state, BTDEV_STATE_PAIRED))
						bt_dev_connect(id);
					else if(BIT_TEST(dev->state, BTDEV_STATE_NORMAL))
						bt_dev_bond(id);
				}
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			//2 只允许查看已配对设备信息
			if(sparam == MMI_KEY_UP && lparam == MVK_RIGHT){
				if(__show_btdev_detail(listctrl_get_cursor(handle)))
					ret = SUCCESS;
			}
			break;
#if defined(FTR_TOUCH_SCREEN)//FTR_NO_INPUT_KEY
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
							__show_btdev_detail(index);
							ret = SUCCESS;
						}
						break;
					case MMI_PEN_PRESS:
						break;
				}
			}
#endif
	}
	return ret;
}

error_t	bluetooth_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	bluetooth_glob_t *glob = app_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			wgt_enable_attr(handle, WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_TITLE|WND_ATTR_HAS_STATUS|WND_ATTR_HAS_SK);
			//bt_init(ga_supported_profiles, ARRAY_SIZE(ga_supported_profiles));
			bt_register_notification(bt_evt_callback, handle);
			
			{
				t_HWidget widget;
				int h = prod_ui_list_item_height();
				gui_rect_t client;
				coord_t y = Y_OFFSET;

				wgt_get_client_rect((t_HWidget)handle, &client);
				
				widget = wnd_get_widget(handle, RESID_ONOFF_POWER);
				wgt_hook_register(widget, WIDGET_OnSelected, __widget_on_hook, handle);
				wgt_set_bound(widget, X_OFFSET, y, lcd_get_width()-2*X_OFFSET, h);
#if defined(FTR_IPHONE_STYLE)
				y += h-1;
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftTop|guiRightTop);
#else
				y += h;
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom);
#endif
				if(bt_is_power_on())
					onoff_set_value(widget, TRUE);

				widget = wnd_get_widget(handle, RESID_ONOFF_VISIBLE);
				wgt_hook_register(widget, WIDGET_OnSelected, __widget_on_hook, handle);
				wgt_set_bound(widget, X_OFFSET, y, lcd_get_width()-2*X_OFFSET, h);
				y += h+Y_OFFSET;
#if defined(FTR_IPHONE_STYLE)
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiLeftBottom|guiRightBottom);
#else
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom);
#endif
				if(bt_is_power_on())
					onoff_set_value(widget, TRUE);
				
				widget = wnd_get_widget(handle, RESID_BTN_SEARCH);
				if(!bt_is_power_on())
				wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
				wgt_hook_register(widget, WIDGET_OnSelected, __widget_on_hook, handle);
				wgt_set_bound(widget, X_OFFSET, y, lcd_get_width()-2*X_OFFSET, h);
				y += h+Y_OFFSET;
#if defined(FTR_IPHONE_STYLE)
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiFourCorner);
#else
				wgt_set_border(widget, BORDER_TYPE_LINE, 1, guiBottom);
#endif

				widget = wnd_get_widget(handle, RESID_LIST_DEVICE);
				wgt_hook_register(widget, WIDGET_OnSelected|WIDGET_OnKeyEvent|WIDGET_OnPenEvent, __listctrl_on_hook, handle);
				wgt_set_bound(widget, X_OFFSET, y, lcd_get_width()-2*X_OFFSET, client.dy-y-Y_OFFSET);
#if defined(FTR_IPHONE_STYLE)
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiFourCorner);
#else
				wgt_set_border(widget, BORDER_TYPE_NONE, 0, 0);
#endif
				listctrl_attach(widget, __inquiry_list_redraw, widget);
				listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_AUTOTAB_UD);
				listctrl_set_count(widget, bt_dev_count());

				widget = wnd_get_skbar(handle);
				if(g_object_valid(widget)){
					sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
					sk_set_text(widget, SK_LSK, NULL, RESID_STR_DETAIL);
				}
			}
			
			break;
		case WINDOW_OnClose:
			if(glob->searching){
				bt_inquiry_end(TRUE);
				glob->searching = FALSE;
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			bt_register_notification(bt_evt_callback, handle);
			break;
		case WINDOW_OnDraw:
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_UP && lparam == MVK_LSK){
				wnd_show(wnd_create(bluetooth_ViewWndMsgHandler, BT_IDENT_SELF, 0));
				ret = SUCCESS;
			}
			break;
	}
	return ret;
}

