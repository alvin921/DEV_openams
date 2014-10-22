#include "pack_contact.hi"




error_t	contact_SettingWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	
	switch(evt){
		case WINDOW_OnOpen:
			{
				g_idx_t id = sparam;
				t_HList list;

				wgt_enable_attr(handle, WND_ATTR_DEFAULT);
				wnd_set_caption(handle, "ViewContact");
				
#if defined(FTR_SOFT_KEY)
				{
					t_SKHandle sk = wnd_get_skbar(handle);
					if(g_object_valid(sk)){
						sk_set_text(sk, SK_LSK, "Advance", 0);
						sk_set_text(sk, SK_RSK, "Back", 0);
					}
				}
#endif
			}
			
			
			ret = SUCCESS;
			break;
		case WINDOW_OnClose:
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			{
				t_HGDI gdi = wgt_get_client_gdi((t_HWidget)handle);
				if(gdi){
					int x, y;
					
					gdi_set_color(gdi, RGB_WHITE);

					
					g_object_unref(gdi);
				}
			}

			ret = SUCCESS;
			break;
	}
	return ret;
}


