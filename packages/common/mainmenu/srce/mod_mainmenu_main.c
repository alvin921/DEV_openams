#include "pack_mainmenu.hi"


error_t	mainmenu_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t handled = NOT_HANDLED;

	switch(evt){
		case WINDOW_OnOpen:
			{
				t_HImage image;
				t_HList gallery;
				t_HDataSource ds;
				gui_rect_t client;
				gu16 clflags;

				wgt_enable_attr(handle, WND_ATTR_DEFAULT);
				ds = DSAppStore_New(NULL, NULL);
				AppStore_AddByMenuTree(DSAppStore_GetRootNode(ds), prod_menu_tree_open(NULL, "/"));
				if(wnd_has_title(handle)){
					t_AppStoreDir *dir = DSAppStore_GetCWD(ds);
					if(dir){
						wnd_set_caption_by_resid(handle, BRD_HANDLE_SELF, dir->text);
						wnd_set_icon_by_resid(handle, BRD_HANDLE_SELF, dir->icon);
					}
				}
				
				gallery = wnd_get_widget(handle, RESID_LIST_MAINMENU);
				listctrl_attach_datasource(gallery, ds);
				g_object_unref((t_HObject)ds);
				wgt_get_client_rect((t_HWidget)handle, &client);
				wgt_set_bound(gallery, 0, 0, client.dx, client.dy);

				listctrl_set_viewtype(gallery, GUI_VIEWTYPE_ICON_TEXT_BOTTOM);
				clflags = CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE;
#if (LCD_WIDTH == 240) && (LCD_HEIGHT == 320)
				listctrl_set_rows(gallery, 0, 60, 0);
				listctrl_set_cols(gallery, 0, 80);
#elif (LCD_WIDTH == 320) && (LCD_HEIGHT == 240)
				listctrl_set_rows(gallery, 3, 0, 0);
				listctrl_set_cols(gallery, 4, 0);
#elif (LCD_WIDTH == 176) && (LCD_HEIGHT == 220)
				listctrl_set_rows(gallery, 3, 0, 0);
				listctrl_set_cols(gallery, 4, 0);
#elif (LCD_WIDTH == 220) && (LCD_HEIGHT == 176)
				listctrl_set_rows(gallery, 3, 0, 0);
				listctrl_set_cols(gallery, 4, 0);
#elif (LCD_WIDTH == 128) && (LCD_HEIGHT == 160)
				listctrl_set_viewtype(gallery, GUI_VIEWTYPE_ICON_TEXT_TITLE);
				listctrl_set_rows(gallery, 4, 0, 0);
				listctrl_set_cols(gallery, 3, 0);
#endif
				listctrl_set_flags(gallery, clflags);
#if defined(FTR_SOFT_KEY)
				{
					t_SKHandle sk;
					sk = wnd_get_skbar(handle);
					if(g_object_valid(sk)){
						sk_set_text(sk, SK_LSK, "OK", RESID_STR_OK);
						sk_set_text(sk, SK_RSK, "Back", RESID_STR_BACK);
					}
				}
#endif
			}
			handled = SUCCESS;
			break;
		case WINDOW_OnClose:
			handled = SUCCESS;
			break;
		case WINDOW_OnDraw:
			break;
		case WIDGET_OnKeyEvent:
			//if(sparam == MVK_END && lparam == MMI_KEY_PRESS){
			//	wnd_send_close(handle, 0);
			//	handled = SUCCESS;
			//}
			break;
	}

	return handled;
}

