#include "pack_audioplayer.hi"

enum {
	AP_FOP_IDX_PLAY = 0,
	AP_FOP_IDX_NEW_PLAYLIST,
	AP_FOP_IDX_ADD_TO_PLAYLIST,
	AP_FOP_IDX_PLAYLIST1,
	AP_FOP_IDX_PLAYLIST2,
	
};

static void	dsource_Redraw(t_HDataSource h, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight)
{
	audioplayer_glob_t *glob = task_get_data();
	const char *name = NULL;
	int x = 0;

	if(!g_object_valid(gdi))
		return;
	if(index == AP_FOP_IDX_PLAY){
		name = brd_get_string(BRD_HANDLE_SELF, RESID_STR_PLAY, "Play");
	}else if(index == AP_FOP_IDX_ADD_TO_PLAYLIST){
		name = brd_get_string(BRD_HANDLE_SELF, RESID_STR_ADD_TO_PLAYLIST, "");
	}else if(index == AP_FOP_IDX_NEW_PLAYLIST){
		name = brd_get_string(BRD_HANDLE_SELF, RESID_STR_NEW_PLAYLIST, "");
	}else if(index-AP_FOP_IDX_PLAYLIST1 < g_ds_GetRecordNum(glob->hdb_playlist)){
		resid_t pic_resid;
		const char *pic;
		int icon_w, icon_h;
		
		g_ds_GotoByIndex(glob->hdb_playlist, index-AP_FOP_IDX_PLAYLIST1);
		name = g_ds_GetString(glob->hdb_playlist, PLAYLIST_DBFIELD_NAME);
		x += 10;
	}
	gdi_draw_text_in(gdi, x, 0, width-x, height, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
}

static int	dsource_GetCount(t_HDataSource h)
{
	audioplayer_glob_t *glob = task_get_data();
	return g_ds_GetRecordNum(glob->hdb_playlist)+AP_FOP_IDX_PLAYLIST1;
}

static gboolean	dsource_CanFocused(t_HDataSource h, int index)
{
	return (index == AP_FOP_IDX_ADD_TO_PLAYLIST)?FALSE:TRUE;
}

static VTBL(datasource) dsourceVtbl = {
	(PFNONCREATE)base_NullCreate
	, (PFNONDESTROY)base_NullDestroy

	, NULL
	, dsource_Redraw
	, dsource_GetCount
	, dsource_CanFocused
	, NULL
	, NULL
};

static CLASS_DECLARE(dsource, t_DataSource, dsourceVtbl);



static error_t __on_playlist_hook(t_HWidget h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	audioplayer_glob_t *glob = task_get_data();
	t_HWindow win = wgt_parent_window(h);
	switch(evt){
		case WIDGET_OnSelected:
			{
				int cursor = (int)sparam;//listctrl_get_cursor(listctrl);

				//audioplayer_ShowViewWindow(sparam);
				if(cursor == AP_FOP_IDX_PLAY){
					wnd_send_close(win, 0);
				}else if(cursor == AP_FOP_IDX_NEW_PLAYLIST){
					gui_rect_t rect;
					t_HWidget widget = wnd_get_widget(win, RESID_EDITOR_NEW_PLAYLIST);
					listctrl_rect_by_index(h, cursor, &rect);
					wgt_set_bound(widget, 0, 0, rect.dx-80, rect.dy);
					wgt_popup(h, widget, rect.x+80, rect.y, guiRight|guiBottom);
				}else if(cursor >= AP_FOP_IDX_PLAYLIST1){
					g_ds_GotoByIndex(glob->hdb_playlist, cursor-AP_FOP_IDX_PLAYLIST1);
					wnd_send_close(win, g_ds_CurrentID(glob->hdb_playlist));
				}
			}
			ret = SUCCESS;
			break;
	}
	return ret;
}

static error_t __on_editor_hook(t_HWidget h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	audioplayer_glob_t *glob = task_get_data();
	switch(evt){
		case WIDGET_OnKillFocus:
			{
				#if 0
				t_HWindow win = wgt_parent_window(h);
				t_HWidget widget = wnd_get_widget(win, RESID_LISTCTRL_AUDIO_OPEN);
				wgt_set_focus(widget, TAB_NONE);
				#endif
				wgt_set_visible(h, FALSE);
				ret = SUCCESS;
			}
			break;
		case WIDGET_OnSelected:
			{
				char name[AP_PLAYLIST_NAME_MAX+1];
				if(editor_get_text(h, name, sizeof(name))){
					t_HDataSet hdb = glob->hdb_playlist;
					t_DBRecordID rid;
#if 0	//2 不检查列表是否存在，直接添加				
					g_ds_CondSet1(hdb, PLAYLIST_DBFIELD_NAME, DBREL_EQ, name);
					g_ds_Query(hdb, FALSE);
					if(!g_ds_Empty(hdb)){
						msgbox_show(NULL, _CSK(OK), 0, 0, "<%s> exists!", name);
					}else
#endif
					{
						g_ds_AddNew(hdb);
						g_ds_SetString(hdb, PLAYLIST_DBFIELD_NAME, name);
						rid = g_ds_Update(hdb);
						if(rid == DB_RID_NULL){
							msgbox_show(NULL, _CSK(OK), 0, 0, "Error to create playlist");
						}else{
							wnd_send_close(wgt_parent_window(h), rid);
						}
					}
				}
			}
			ret = SUCCESS;
			break;
	}
	return ret;
}

error_t	audioplayer_OpenFileWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	audioplayer_glob_t *glob = task_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
		{
			t_HList list, widget;
			t_HDataSource ds;
			
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			list = wnd_get_widget(handle, RESID_LISTCTRL_AUDIO_OPEN);
			ds = (t_HDataSource)g_object_new0(&class_dsource);
			listctrl_attach_datasource(list, ds);
			g_object_unref_0(ds);
			listctrl_set_viewtype(list, GUI_VIEWTYPE_NORMAL);
			listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM);
			listctrl_set_item_margin(list, 5, 5, 0, 0);
			wgt_hook_register(list, WIDGET_OnSelected, __on_playlist_hook, NULL);

			widget = wnd_get_widget(handle, RESID_EDITOR_NEW_PLAYLIST);
			editor_set_nchar_max(widget, AP_PLAYLIST_NAME_MAX);
			editor_set_type(widget, EDITOR_TYPE_TEXT, FALSE);
			wgt_hook_register(widget, WIDGET_OnSelected|WIDGET_OnKillFocus, __on_editor_hook, NULL);
			
			widget = wnd_get_skbar(handle);
			sk_set_text(widget, SK_LSK, NULL, RESID_STR_OK);
			sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
			
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
			if(sparam == MMI_KEY_UP){
				if(lparam == MVK_RSK){
					wnd_send_close(handle, -1);
					ret = SUCCESS;
				}
			}
			break;
	}
	return ret;
}


