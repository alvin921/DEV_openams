#include "pack_contact.hi"

static void	__redraw_phone(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	contact_number_t *wdata = userdata;
	resid_t resid = RESID_UNDEF;
	const char *text;
	int tw, th;
	int i;
	for(i = 0; i < CONTACT_F_NUMBER_N; i++){
		if(wdata->number[i][0] != 0){
			if(index == 0)
				break;
			index--;
		}
	}
	if(i < CONTACT_F_NUMBER_N)
		resid = ga_phone_type_resid[i];
	else
		resid = RESID_STR_NUMBER_UNKNOWN;
	text = brd_get_string(BRD_HANDLE_SELF, resid, NULL);
	gdi_measure_text(gdi, text, -1, CHARSET_UTF8, -1, &tw, &th, NULL);
	gdi_draw_text_in(gdi, 0, 0, tw, height, text, -1, CHARSET_UTF8, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
	gdi_draw_text_in(gdi, tw, 0, width-tw, height, wdata->number[i], -1, CHARSET_UTF8, IDF_HALIGN_RIGHT|IDF_VALIGN_MIDDLE);
}

//2 <sparam>=count, <lparam>=(contact_number_t *)
error_t	contact_SelectPhoneWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	contact_number_t *wdata = (contact_number_t *)wgt_get_tag(handle);
	
	switch(evt){
		case WINDOW_OnOpen:
			{
				t_HList list;

				wgt_set_tag(handle, lparam);
				wgt_enable_attr(handle, WND_ATTR_DEFAULT);
				wgt_set_caption_by_resid((t_HWidget)handle, brd_get(PACK_GUID_SELF), RESID_STR_SELECT_PHONE);
				wdata = (contact_number_t *)lparam;

				list = wnd_get_widget(handle, RESID_LIST_PHONE);
				if(g_object_valid(list)){
					//wgt_hook_register(list, WIDGET_OnSelected, __listctrl_hook);
					listctrl_set_count(list, sparam);
					listctrl_attach(list, __redraw_phone, wdata);
				}
#if defined(FTR_SOFT_KEY)
				{
					t_SKHandle sk = wnd_get_skbar(handle);
					if(g_object_valid(sk)){
						sk_set_text(sk, SK_LSK, NULL, RESID_STR_OK);
						sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
					}
				}
#endif
			}
			
			
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
				switch(lparam){
					case MVK_LSK:
					case MVK_OK:
						{
							t_HList list = wnd_get_widget(handle, RESID_LIST_PHONE);
							int cursor = listctrl_get_cursor(list);
							if(cursor >= 0){
								int i;
								for(i = 0; i < CONTACT_F_NUMBER_N; i++){
									if(wdata->number[i][0] != 0){
										if(cursor == 0){
											wnd_send_close(handle, i);
											break;
										}
										cursor--;
									}
								}
							}
						}
						ret = SUCCESS;
						break;
					case MVK_RSK:
						wnd_send_close(handle, -1);
						ret = SUCCESS;
						break;
				}
			}
			break;
		case WINDOW_OnClose:
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			ret = SUCCESS;
			break;
	}
	return ret;
}


