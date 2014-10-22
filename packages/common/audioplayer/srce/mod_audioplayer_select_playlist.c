#include "pack_audioplayer.hi"

static void	db_audioplayer_redraw_playlist(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, t_HDataSet hdb)
{
	const char *name;
	const char *pic;
	t_HImage icon = NULL;

	g_ds_GotoByIndex(hdb, index);
	name = g_ds_GetString(hdb, PLAYLIST_DBFIELD_NAME);
	{
		int x;
		if(g_object_valid(icon)){
			gdi_draw_image_in(gdi, 0, 0, height, height, icon, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE | IDF_HALIGN_STRETCH | IDF_VALIGN_STRETCH);
		}
		width -= height+2;
		x = height+2;
		gdi_draw_text_in(gdi, x, 0, width, height, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
	}
	g_object_unref(icon);
}

error_t	audioplayer_PlaylistWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	audioplayer_glob_t *glob = task_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
		{
			t_HList list;

			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			list = wnd_get_widget(handle, RESID_LISTCTRL_SELECT_PLAYLIST);
			listctrl_set_count(list, g_ds_GetRecordNum(glob->hdb_playlist));
			listctrl_attach(list, (listctrl_redraw_func_t)db_audioplayer_redraw_playlist, glob->hdb_playlist);
			listctrl_set_viewtype(list, GUI_VIEWTYPE_NORMAL);
			listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM);
			
			{
				t_HWidget sk = wnd_get_skbar(handle);
				if(g_object_valid(sk)){
					sk_set_text(sk, SK_LSK, NULL, RESID_STR_OK);
					sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
				}
			}
			ret = SUCCESS;
		}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			
			//gdi_draw_text_at(wgt_gdi(handle), "Hello World!", -1, 30, 30, RGB_WHITE, RGB_BLACK, TRUE);
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_PRESS){
				if(lparam == MVK_RSK){
					wnd_send_close(handle, DB_RID_NULL);
					ret = SUCCESS;
				}else if(lparam == MVK_LSK || lparam == MVK_OK){
					g_ds_GotoByIndex(glob->hdb_playlist, listctrl_get_cursor(wnd_get_widget(handle, RESID_LISTCTRL_SELECT_PLAYLIST)));
					wnd_send_close(handle, g_ds_CurrentID(glob->hdb_playlist));
					ret = SUCCESS;
				}
			}
			break;
	}
	return ret;
}


