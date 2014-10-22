#include "pack_call.hi"
#include "wgt_editor.h"


//2 
void		call_DialerQuery(t_PanelWndData *wdata, t_HWidget listctrl, char *number)
{
	if(g_object_valid(listctrl)){
		int i;

		wdata->total_count = 0;
		for(i = 0; i < wdata->n_ms_phone; i++){
			if(wdata->vtbl[i] && wdata->vtbl[i]->Query){
				wdata->vtbl[i]->Query(wdata->vtbl[i], number);
				wdata->count[i] = g_ds_GetRecordNum(wdata->vtbl[i]->hds);
			}else{
				wdata->count[i] = 0;
			}
			wdata->total_count += wdata->count[i];
		}
		listctrl_set_count(listctrl, wdata->total_count);
		listctrl_set_cursor(listctrl, 0, FALSE);
	}
}

void	__show_phone(t_PanelWndData *wdata, t_HWindow handle)
{
	t_HImage image;
	t_HGDI gdi;
	int i;
	coord_t x, y;
	int iw, ih, bh;

	gdi = wgt_get_client_gdi((t_HWidget)handle);
	image = brd_get_image(BRD_HANDLE_SELF, RESID_IMG_DIALER_BG);
	bh = image_get_height(image);
	gdi_draw_image_at(gdi, 0, 0, image, 0, 0, -1, -1, 1.0, 1.0);
	g_object_unref_0(image);

	x = y = 0;
	for(i = 0; i < wdata->len; i++){
		switch(wdata->strPhone[i]){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				image = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_IMG_DIGIT, wdata->strPhone[i]-'0'));
				break;
			case '*':
				image = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_IMG_DIGIT, 11));
				break;
			case '#':
				image = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_IMG_DIGIT, 12));
				break;
			case 'p':
				break;
			case 'w':
				break;
			default:
				image = NULL;
		}
		if(g_object_valid(image)){
			iw = image_get_width(image);
			if(y == 0){
				gui_rect_t rt;
				wgt_get_client_rect((t_HWidget)handle, &rt);
				y = (bh - image_get_height(image))>>1;
			}
			gdi_draw_image_at(gdi, x, y, image, 0, 0, -1, -1, 1.0, 1.0);
			g_object_unref(image);
			x += iw;
		}else{
			font_measure_uchar(wdata->strPhone[i], &iw, &ih, NULL);
			if(y == 0){
				gui_rect_t rt;
				wgt_get_client_rect((t_HWidget)handle, &rt);
				y = (bh - ih)>>1;
			}
			gdi_move_to(gdi, x, y);
			gdi_draw_uchar(gdi, wdata->strPhone[i]);
			x += iw;
		}
	}
	g_object_unref(gdi);
}

void	on_ime_add_char(t_HWidget handle, gu8 mode, ucs2_t ch)
{
	t_HWindow wnd = (t_HWindow)handle;
	t_PanelWndData *wdata = (t_PanelWndData *)wgt_get_tag(handle);
	t_HWidget listctrl = wnd_get_widget(wnd, RESID_LIST_DIALER);

	if(wdata->len < CONTACT_NUMBER_MAX){
		if(ch >= '0' && ch <= '9' || ch == '*' || ch == '#' || ch == 'p' || ch == 'w'){
			wdata->strPhone[wdata->len++] = ch;
			wdata->strPhone[wdata->len] = 0;
			if(wdata->len == 1){
				sk_set_text(wnd_get_skbar(handle), SK_RSK, NULL, RESID_STR_DELETE);
			}
			call_DialerQuery(wdata, listctrl, wdata->strPhone);
			wnd_redraw(wnd, NULL);
		}
	}
}


static	gboolean	__GetItemIndex(t_PanelWndData *wdata, int index, int *pi, int *po)
{
	int i;
	for(i = 0; i < wdata->n_ms_phone; i++){
		if(index < wdata->count[i]){
			if(pi)*pi = i;
			if(po)*po = index;
			return TRUE;
		}
		index -= wdata->count[i];
	}
	return FALSE;
}

static char * __GetPhoneNumber(t_PanelWndData *wdata, t_HWindow handle)
{
	if(wdata->total_count > 0){
		int idx, off;
		if(__GetItemIndex(wdata, listctrl_get_cursor(wnd_get_widget(handle, RESID_LIST_DIALER)), &idx, &off)){
			if(wdata->vtbl[idx] && wdata->vtbl[idx]->GetItemNumber)
				return wdata->vtbl[idx]->GetItemNumber(wdata->vtbl[idx], off);
		}
	}else{
		return wdata->strPhone;
	}
	return NULL;
}

static void	__ms_listctrl_on_select(t_HDataSource h, const char *guid, t_AmsModuleId module, void *userdata)
{
	char * number;
	t_HWindow handle;

	handle = wgt_parent_window(h->list);
	number = __GetPhoneNumber((t_PanelWndData*)wgt_get_tag(handle), handle);
	if(number && *number)
		pack_call(guid, module, (void*)number, strlen(number)+1, FALSE, NULL);
	else
		msgbox_show(NULL, _CSK(OK), 0, 0, "Please input number!");
}

static void	dialer_RedrawItem(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, t_PanelWndData *wdata)
{
	int idx, off;
	if(__GetItemIndex(wdata, index, &idx, &off)){
		g_ds_RedrawRecord(wdata->vtbl[idx]->hds, off, gdi, viewtype, width, height, hilight);
	}
}

error_t	call_PanelWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	call_glob_t *glob = app_get_data();
	t_PanelWndData *wdata = (t_PanelWndData *)wgt_get_tag(handle);
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			{
				t_HWidget widget, sk;
				int i;

				wgt_enable_attr(handle, WND_ATTR_CLOSE_APP|WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_SK|WND_ATTR_HAS_STATUS);
				wdata = MALLOC(sizeof(t_PanelWndData));
				memset(wdata, 0, sizeof(t_PanelWndData));
				wgt_set_tag(handle, wdata);

				wdata->n_ms_phone = mime_service_query(MIME_TYPE_CONTACT, METHOD_NAME_QUERY_INTERFACE, NULL, wdata->ms_phone, ARRAY_SIZE(wdata->ms_phone), 0, TRUE);
				for(i = 0; i < wdata->n_ms_phone; i++){
					//mime_service_exec(MIME_TYPE_CONTACT, METHOD_NAME_QUERY_INTERFACE, wdata->ms_phone[i].guid, &wdata->vtbl[i], sizeof(t_ContactMSInterface), TRUE);
					pack_call_func(wdata->ms_phone[i].guid, wdata->ms_phone[i].module, &wdata->vtbl[i], sizeof(t_ContactMSInterface));
				}
				widget = wnd_get_widget(handle, RESID_LIST_DIALER);
				if(g_object_valid(widget)){
					listctrl_attach(widget, (listctrl_redraw_func_t)dialer_RedrawItem, wdata);
					listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
					listctrl_set_flags(widget, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM);
				}

				widget = wnd_get_widget(handle, RESID_LIST_OPERATION);
				if(g_object_valid(widget)){
					t_HDataSet ds;
					
					ds = DSAppStore_New(__ms_listctrl_on_select, glob);
					AppStore_AddByMime(DSAppStore_GetRootNode(ds), MIME_TYPE_PHONE, NULL);
					listctrl_attach_datasource(widget, ds);
					g_object_unref(ds);

					listctrl_set_viewtype(widget, GUI_VIEWTYPE_ICON_TEXT_BOTTOM);
					listctrl_set_flags(widget, CLFLAG_SCROLLING_HORIZONTAL|CLFLAG_SCROLLING_PAGE);

#if (LCD_WIDTH == 240) && (LCD_HEIGHT == 320)
					listctrl_set_rows(widget, 0, 60, 0);
					listctrl_set_cols(widget, 0, 80);
#elif (LCD_WIDTH == 320) && (LCD_HEIGHT == 240)
					listctrl_set_rows(widget, 0, 80, 0);
					listctrl_set_cols(widget, 0, 60);
#elif (LCD_WIDTH == 176) && (LCD_HEIGHT == 220)
					listctrl_set_rows(widget, 3, 0, 0);
					listctrl_set_cols(widget, 4, 0);
#elif (LCD_WIDTH == 220) && (LCD_HEIGHT == 176)
					listctrl_set_rows(widget, 4, 0, 0);
					listctrl_set_cols(widget, 3, 0);
#elif (LCD_WIDTH == 128) && (LCD_HEIGHT == 160)
					listctrl_set_rows(widget, 3, 0, 0);
					listctrl_set_cols(widget, 3, 0);
#endif
					wgt_enable_attr(widget, CTRL_ATTR_POPUP);
				}

				sk = wnd_get_skbar(handle);
				if(g_object_valid(sk)){
					sk_set_text(sk, SK_LSK, NULL, RESID_STR_OPTION);
					sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
					sk_set_popup(sk, SK_LSK, widget);
				}
			}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			if(wdata){
				int i;
			
				wdata->total_count = 0;
				for(i = 0; i < wdata->n_ms_phone; i++){
					contact_msi_destroy(wdata->vtbl[i]);
				}
				FREE(wdata);
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			ime_agent_activate((t_HWidget)handle);
			ime_set(ime_query(NULL, LANG_123, "123"), DIGIT_TYPE_PHONE);
			ime_enable_navigation(FALSE);
			break;
		case WIDGET_OnKillFocus:
			ime_deactivate_agent();
			break;
		case WIDGET_OnMessage:
			if(sparam == EVT_IME_ADD_CHAR){
				t_AmosMsg *msg = (t_AmosMsg *)lparam;
				on_ime_add_char(handle, msg->sparam, msg->lparam);
				ret = SUCCESS;
			}
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			__show_phone(wdata, handle);
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_DOWN)
				break;
			switch(lparam){
				case MVK_RSK:
					if(sparam == MMI_KEY_UP){	//2 仅在无号码时自动退出
						if(wdata->len == 0)
							wnd_send_close(handle, 0);
						else
							return SUCCESS;
					}
					if(wdata->len == 0)
						return SUCCESS;
					if(sparam == MMI_KEY_LONG_PRESS){
						wdata->len = 0;
					}else{
						wdata->len--;
					}
					wdata->strPhone[wdata->len] = 0;
					if(wdata->len == 0){
						sk_set_text(wnd_get_skbar(handle), SK_RSK, NULL, RESID_STR_BACK);
					}
					call_DialerQuery(wdata, wnd_get_widget(handle, RESID_LIST_DIALER), wdata->strPhone);
					wnd_redraw(handle, NULL);
					ret = SUCCESS;
					break;
				case MVK_SEND1:
				case MVK_SEND2:
				case MVK_SEND3:
				case MVK_SEND4:
					if(sparam != MMI_KEY_UP)
						break;
					if(prod_sim_status(lparam-MVK_SEND1) != SIM_CARD_NORMAL){
						msgbox_show(NULL, _CSK(OK), 0, 0, "SIM %d not inserted!", lparam-MVK_SEND1+1);
						break;
					}
				case MVK_SEND:
					if(sparam != MMI_KEY_UP)
						break;
					if(wdata->total_count > 0){
						int idx, off;
						if(__GetItemIndex(wdata, listctrl_get_cursor(wnd_get_widget(handle, RESID_LIST_DIALER)), &idx, &off)){
							if(wdata->vtbl[idx] && wdata->vtbl[idx]->GetItemNumber){
								if(lparam == MVK_SEND)
									call_LoadOutgoing(0xff, wdata->vtbl[idx]->GetItemNumber(wdata->vtbl[idx], off), 0);
								else
									call_LoadOutgoing(lparam-MVK_SEND1, wdata->vtbl[idx]->GetItemNumber(wdata->vtbl[idx], off), 0);
							}
						}
					}else{
						if(lparam == MVK_SEND)
							call_LoadOutgoing(0xff, wdata->strPhone, 0);
						else
							call_LoadOutgoing(lparam-MVK_SEND1, wdata->strPhone, 0);
					}
					wnd_send_close(handle, 0);
					ret = SUCCESS;
					break;
				
				case MVK_END:
					if(sparam == MMI_KEY_UP)
					wnd_send_close(handle, 0);
					ret = SUCCESS;
					break;
			}
			break;
	}
	return ret;
}

