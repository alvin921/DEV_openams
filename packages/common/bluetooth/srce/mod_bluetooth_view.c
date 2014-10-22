#include "pack_bluetooth.hi"




#define	MARGIN_X	5
#define	BLOCK_SPLIT_Y	5
#define	BLOCK_DY	30

#define	BLOCK_TITLE_DX	60
#define	BLOCK_TITLE_COLOR	RGB_LTBLUE

#define	PROFILE_DY		40

#define	PROFILE_PIC_X		MARGIN_X
#define	PROFILE_PIC_Y		BLOCK_SPLIT_Y
#define	PROFILE_PIC_DX		PROFILE_DY
#define	PROFILE_PIC_DY		PROFILE_DY

#define	PROFILE_NAME_X	(MARGIN_X+PROFILE_DY+MARGIN_X)
#define	PROFILE_NAME_Y	PROFILE_PIC_Y

void	__listctrl_overview_redraw(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	t_ViewBtWData *wdata = userdata;
	
	if(index == LIST_IDX_NAME){
		t_HImage icon = bt_get_cod_icon(wdata->dev->cod);
		gdi_draw_image_in(gdi, MARGIN_X, 1, height-2, height-2, icon, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
		gdi_draw_text_in(gdi, MARGIN_X+height, 0, width-MARGIN_X-height, height, wdata->dev->name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
		g_object_unref(icon);
	}else if(index == LIST_IDX_ADDR){
		char str[20];
		gu8 *p = wdata->dev->addr;
		sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", p[5], p[4], p[3], p[2], p[1], p[0]);
		gdi_draw_text_in(gdi, MARGIN_X, 0, width-MARGIN_X, height, str, -1, CHARSET_UTF8, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
	}else if(index == LIST_IDX_STATE){
		gdi_draw_text_in(gdi, MARGIN_X, 0, width-MARGIN_X, height, bt_get_state_string(wdata->dev->state), -1, CHARSET_UTF8, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
	}
}

static void	__listctrl_profile_redraw(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	t_ViewBtWData *wdata = userdata;
	t_BtDevice *dev = wdata->dev;
	t_BtProfileID profile = 0;
	gu8 state;
	const char *str;
	resid_t resid[] = {RESID_UNDEF, RESID_STR_BT_ACTIVATED, RESID_STR_BT_CONNECTED};

	if(dev->n_profiles > 0){
		profile = dev->profiles[index];
	}
	str = bt_profile_string(profile);
	state = bt_dev_profile_state(dev->id, profile);
	gdi_draw_text_in(gdi, MARGIN_X, 0, width-MARGIN_X, height, str, -1, CHARSET_UTF8, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
	if(state){
		str = brd_get_string(brd_get(PACK_GUID_SELF), resid[state], "");
		gdi_draw_text_in(gdi, MARGIN_X, 0, width-MARGIN_X-MARGIN_X, height, str, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT|IDF_VALIGN_MIDDLE);
	}
}

static error_t	__widget_on_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	bluetooth_glob_t *glob = app_get_data();
	t_HWidget wnd = userdata;
	t_ViewBtWData *wdata = (t_ViewBtWData *)wgt_get_tag(wnd);
	
	switch(evt){
		case WIDGET_OnSelected:
			switch(wgt_get_resid(handle)){
				case RESID_BTVIEW_BTN_REFRESH:
					break;
				case RESID_BTVIEW_BTN_CONNECT:
					if(wdata->id != BT_IDENT_SELF && BIT_TEST(wdata->dev->state, BTDEV_STATE_PAIRED)){
						if(bt_dev_connect(wdata->id) == SUCCESS){
							wgt_disable_attr(handle, CTRL_ATTR_ENABLED);
							wgt_redraw(handle, FALSE);
							handle = wnd_get_widget(wnd, RESID_BTVIEW_BTN_DISCONNECT);
							wgt_enable_attr(handle, CTRL_ATTR_ENABLED);
							wgt_redraw(handle, FALSE);
						}
					}
					break;
				case RESID_BTVIEW_BTN_DISCONNECT:
					if(BIT_TEST(wdata->dev->state, BTDEV_STATE_CONNECTED)){
						bt_dev_disconnect(wdata->id);
						//wgt_set_caption_by_resid(handle, brd_get(PACK_GUID_SELF), RESID_STR_BT_CONNECT);
						wgt_disable_attr(handle, CTRL_ATTR_ENABLED);
						wgt_redraw(handle, FALSE);
						if(wdata->id != BT_IDENT_SELF){
							handle = wnd_get_widget(wnd, RESID_BTVIEW_BTN_CONNECT);
							wgt_enable_attr(handle, CTRL_ATTR_ENABLED);
							wgt_redraw(handle, FALSE);
						}
					}
					break;
				case RESID_BTVIEW_LIST_PROFILE:
					{
						int cursor = listctrl_get_cursor(handle);
						if(cursor >= 0 && cursor < wdata->dev->n_profiles){
							gu8 state = bt_dev_profile_state(wdata->id, wdata->dev->profiles[cursor]);
							char str[100];
							if(state == BT_PROF_CONNECTED){
								sprintf(str, "%s %s?", brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_BT_DISCONNECT_PROMPT, ""), bt_profile_string(wdata->dev->profiles[cursor]));
								if(msgbox_show(NULL, _LSK(YES)|_RSK(NO), 0, 0, str) == MBB_YES)
									bt_dev_profile_disconnect(wdata->id, wdata->dev->profiles[cursor]);
							}else{
								sprintf(str, "%s %s?", brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_BT_CONNECT_PROMPT, ""), bt_profile_string(wdata->dev->profiles[cursor]));
								if(msgbox_show(NULL, _LSK(YES)|_RSK(NO), 0, 0, str) == MBB_YES)
									bt_dev_profile_connect(wdata->id, wdata->dev->profiles[cursor]);
							}
						}
					}
					break;
			}
			break;
	}
}

static void __sk_callback(t_HWidget sk, void *data)
{
	t_HWindow handle = data;
	t_ViewBtWData *wdata = (t_ViewBtWData *)wgt_get_tag(handle);

	if(wdata->id != BT_IDENT_SELF){
		bt_dev_disconnect(wdata->id);
		bt_dev_debond(wdata->id);
		
		wnd_send_close(handle, 0);
	}
}

//2 sparam=<ID>, BT_IDENT_SELF表示显示本机信息
error_t	bluetooth_ViewWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_ViewBtWData *wdata = (t_ViewBtWData *)wgt_get_tag(handle);
	
	switch(evt){
		case WINDOW_OnOpen:
			{
				t_HWidget widget;
				int w = lcd_get_width()-MARGIN_X*2;
				coord_t x = MARGIN_X;
				coord_t y = BLOCK_SPLIT_Y;
				
				bt_register_notification(bt_evt_callback, handle);
				wdata = MALLOC(sizeof(t_ViewBtWData));
				memset(wdata, 0, sizeof(t_ViewBtWData));
				
				wgt_enable_attr(handle, WND_ATTR_DEFAULT|CTRL_ATTR_VSCROLLABLE);
				wdata->id = sparam;
				wdata->dev = bt_dev_get_info(wdata->id);

				wgt_set_tag(handle, wdata);
#if defined(FTR_SOFT_KEY)
				widget = wnd_get_skbar(handle);
				if(g_object_valid(widget)){
					sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
					if(wdata->id != BT_IDENT_SELF){
						sk_set_text(widget, SK_LSK, NULL, RESID_STR_BT_DEBOND);
						sk_set_callback(widget, SK_LSK, __sk_callback, handle);
					}
				}
#endif
				wgt_set_caption((t_HWidget)handle, wdata->dev->name);
				//wgt_set_bg_fill_solid(handle, RGB_LTGRAY);

				//2 Overview
				widget = wgt_create((t_HWidget)handle, CLSID_LISTCTRL);
				wgt_set_resid(widget, RESID_BTVIEW_LIST_OVERVIEW);
				wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
				wgt_set_bound(widget, x, y, w, prod_ui_list_item_height()*LIST_OVERVIEW_NUM);
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiFourCorner);
				listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_AUTOTAB_UD);
				listctrl_attach(widget, __listctrl_overview_redraw, wdata);
				listctrl_set_count(widget, LIST_OVERVIEW_NUM);
				y += wgt_bound_dy(widget)+BLOCK_SPLIT_Y;
				
				//2 Button-refresh
				widget = wgt_create((t_HWidget)handle, CLSID_button);
				wgt_set_resid(widget, RESID_BTVIEW_BTN_REFRESH);
				wgt_hook_register(widget, WIDGET_OnSelected, __widget_on_hook, handle);
				wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_STR_BT_REFRESH_PROF);
				wgt_set_bound(widget, x, y, w/3-MARGIN_X, prod_ui_list_item_height());
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiFourCorner);
				if(wdata->id == BT_IDENT_SELF/* || BIT_TEST(wdata->dev->state, BTDEV_STATE_CONNECTED)*/){
					//2  有可能个别profile connected，此时任然可以connect其它profile
					wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
				}
				
				//2 Button-connect
				widget = wgt_create((t_HWidget)handle, CLSID_button);
				wgt_set_resid(widget, RESID_BTVIEW_BTN_CONNECT);
				wgt_hook_register(widget, WIDGET_OnSelected, __widget_on_hook, handle);
				wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_STR_BT_CONNECT);
				wgt_set_bound(widget, x+w/3+MARGIN_X, y, w/3-MARGIN_X, prod_ui_list_item_height());
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiFourCorner);
				if(wdata->id == BT_IDENT_SELF/* || BIT_TEST(wdata->dev->state, BTDEV_STATE_CONNECTED)*/){
					//2  有可能个别profile connected，此时任然可以connect其它profile
					wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
				}
				
				//2 Button-disconnect
				widget = wgt_create((t_HWidget)handle, CLSID_button);
				wgt_set_resid(widget, RESID_BTVIEW_BTN_DISCONNECT);
				wgt_hook_register(widget, WIDGET_OnSelected, __widget_on_hook, handle);
				wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_STR_BT_DISCONNECT);
				wgt_set_bound(widget, x+2*w/3+MARGIN_X, y, w/3-MARGIN_X, prod_ui_list_item_height());
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiFourCorner);
				y += wgt_bound_dy(widget)+BLOCK_SPLIT_Y;

				if(!BIT_TEST(wdata->dev->state, BTDEV_STATE_CONNECTED)){
					wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
				}

				//2 Profiles List
				widget = wgt_create((t_HWidget)handle, CLSID_LISTCTRL);
				wgt_set_resid(widget, RESID_BTVIEW_LIST_PROFILE);
				wgt_hook_register(widget, WIDGET_OnSelected, __widget_on_hook, handle);
				wgt_set_bound(widget, x, y, w, prod_ui_list_item_height()*MAX(1, wdata->dev->n_profiles));
				wgt_set_border(widget, BORDER_TYPE_FRAME, 1, guiFourCorner);
				listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_AUTOTAB_UD);
				listctrl_attach(widget, __listctrl_profile_redraw, wdata);
				listctrl_set_count(widget, MAX(1, wdata->dev->n_profiles));
				y += wgt_bound_dy(widget)+BLOCK_SPLIT_Y;

			}
			
			
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			break;
		case WINDOW_OnClose:
			FREEIF(wdata);
			break;
		case WINDOW_OnDraw:
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			bt_register_notification(bt_evt_callback, handle);
			break;
#if defined(FTR_TOUCH_SCREEN) && defined(FTR_IPHONE_STYLE)
		case WIDGET_OnPenEvent:
			{
				coord_t x0, y0;
				int index;
				PEN_XY(lparam, x0, y0);
				switch(PEN_TYPE(lparam)){
					case MMI_PEN_DOWN:
						break;
					case MMI_PEN_UP:
						
						break;
					case MMI_PEN_MOVE:
						
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


