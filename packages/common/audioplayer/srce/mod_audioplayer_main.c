#include "pack_audioplayer.hi"

#define	FTR_ADD_NEW_IN_PLAYLIST__________N


static const char*	__audioplayer_get_playlist_name(audioplayer_glob_t *glob, int index, int *pCount)
{
	t_HDataSet hdb = glob->hdb_playlist;
	int count = g_ds_GetRecordNum(hdb);
	int N;
	
	const char *name;

	if(index == LIST_ALL){
		name = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_LIST_ALL, "");
		if(pCount){
			g_ds_CondClear(glob->hdb_filelist);
			g_ds_Query(glob->hdb_filelist, TRUE);
			*pCount = g_ds_GetRecordNum(glob->hdb_filelist);
		}
	}else if(index == LIST_RECENT){
		name = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_LIST_RECENTS, "");
		if(pCount)*pCount = glob->mp_recent.count;
#if defined(FTR_ADD_NEW_IN_PLAYLIST)
	}else if(index == LIST_USER+count){
		//2 Add New
		name = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_NEW_PLAYLIST, "");
		if(pCount)*pCount = 0;
	}else if(index == LIST_USER+count+1){
		//2 Import
		name = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_IMPORT_PLAYLIST, "");
		if(pCount)*pCount = 0;
#endif
	}else{
		g_ds_GotoByIndex(hdb, index-LIST_USER);
		if(index == LIST_DEFAULT)
			name = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_LIST_DEFAULT, "");
		else if(index == LIST_FAVORATE)
			name = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_LIST_FAVORATES, "");
		else
			name = g_ds_GetString(hdb, PLAYLIST_DBFIELD_NAME);
		if(pCount)*pCount = g_ds_GetValue(hdb, PLAYLIST_DBFIELD_COUNT);
	}
	return name;
}

static void	__sk_initialize(audioplayer_glob_t *glob, t_HWindow handle)
{
	t_HWidget sk = wnd_get_skbar(handle);
	if(g_object_valid(sk)){
		t_HWidget menu;
		sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
		
		if(glob->ap_view == AP_VIEW_PLAYLIST){
#if defined(FTR_NO_INPUT_KEY)
			sk_set_text(sk, SK_LSK, NULL, RESID_STR_ADD_NEW);
			menu = wnd_get_widget(handle, RESID_MENU_LSK);
			sk_set_popup(sk, SK_LSK, menu);
			menu_SetItemVisible(menu, RESID_STR_ADD_FILES, IDENT_BY_ID, FALSE);
			menu_SetItemVisible(menu, RESID_STR_ADD_DIR, IDENT_BY_ID, FALSE);
			menu_SetItemVisible(menu, RESID_STR_NEW_PLAYLIST, IDENT_BY_ID, TRUE);
			menu_SetItemVisible(menu, RESID_STR_IMPORT_PLAYLIST, IDENT_BY_ID, TRUE);
#else
			sk_set_text(sk, SK_LSK, NULL, RESID_STR_OPTION);
			sk_set_popup(sk, SK_LSK, wnd_get_widget(handle, RESID_MENU_PLAYLIST));
#endif
			wgt_set_caption_by_resid((t_HWidget)handle, brd_get(PACK_GUID_SELF), RESID_APP_TITLE);
			wgt_redraw(sk, FALSE);
		}else if(glob->ap_view == AP_VIEW_FILELIST){
			t_HWidget listctrl = wnd_get_widget(handle, RESID_LISTCTRL_FILELIST);
			
			#if defined(FTR_NO_INPUT_KEY)
			if(glob->playlist_cursor >= LIST_USER){
				sk_set_text(sk, SK_LSK, NULL, RESID_STR_ADD_NEW);
				menu = wnd_get_widget(handle, RESID_MENU_LSK);
				sk_set_popup(sk, SK_LSK, menu);
				menu_SetItemVisible(menu, RESID_STR_ADD_FILES, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_ADD_DIR, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_NEW_PLAYLIST, IDENT_BY_ID, FALSE);
				menu_SetItemVisible(menu, RESID_STR_IMPORT_PLAYLIST, IDENT_BY_ID, FALSE);
			}else{
				sk_set_text(sk, SK_LSK, NULL, RESID_UNDEF);
				//sk_set_popup(sk, SK_LSK, NULL);
			}
			#else
			sk_set_text(sk, SK_LSK, NULL, RESID_STR_OPTION);
			menu = wnd_get_widget(handle, RESID_MENU_FILELIST);
			sk_set_popup(sk, SK_LSK, menu);
			if(glob->playlist_cursor >= LIST_USER){
				menu_SetItemVisible(menu, RESID_STR_ADD_FILES, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_ADD_DIR, IDENT_BY_ID, TRUE);
			}else{
				menu_SetItemVisible(menu, RESID_STR_ADD_FILES, IDENT_BY_ID, FALSE);
				menu_SetItemVisible(menu, RESID_STR_ADD_DIR, IDENT_BY_ID, FALSE);
			}
			if(listctrl_get_count(listctrl)){
				menu_SetItemVisible(menu, RESID_STR_ADD_TO_PLAYLIST, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_CLEANUP_PLAYLIST, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_REMOVE_INVALID_FILES, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_REMOVE_FILE, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_DELETE_FILE, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_CONVERT_FILE, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_LOCATE_FILE, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_VIEW_DETAIL, IDENT_BY_ID, TRUE);
				menu_SetItemVisible(menu, RESID_STR_EDIT_FILE_INFO, IDENT_BY_ID, TRUE);
			}else{
				menu_SetItemVisible(menu, RESID_STR_ADD_TO_PLAYLIST, IDENT_BY_ID, FALSE);
				menu_SetItemVisible(menu, RESID_STR_CLEANUP_PLAYLIST, IDENT_BY_ID, FALSE);
				menu_SetItemVisible(menu, RESID_STR_REMOVE_INVALID_FILES, IDENT_BY_ID, FALSE);
				menu_SetItemVisible(menu, RESID_STR_REMOVE_FILE, IDENT_BY_ID, FALSE);
				menu_SetItemVisible(menu, RESID_STR_DELETE_FILE, IDENT_BY_ID, FALSE);
				menu_SetItemVisible(menu, RESID_STR_CONVERT_FILE, IDENT_BY_ID, FALSE);
				menu_SetItemVisible(menu, RESID_STR_LOCATE_FILE, IDENT_BY_ID, FALSE);
				menu_SetItemVisible(menu, RESID_STR_VIEW_DETAIL, IDENT_BY_ID, FALSE);
				menu_SetItemVisible(menu, RESID_STR_EDIT_FILE_INFO, IDENT_BY_ID, FALSE);
			}
			#endif
			wgt_set_caption((t_HWidget)handle, __audioplayer_get_playlist_name(glob, glob->playlist_cursor, NULL));
			wgt_redraw(sk, FALSE);
		}else if(glob->ap_view == AP_VIEW_PLAYER){
#if 0
			sk_set_text(sk, SK_LSK, NULL, RESID_STR_OPTION);
			sk_set_popup(sk, SK_LSK, wnd_get_widget(handle, RESID_MENU_MAIN_PLAYER));
			wgt_redraw(sk, FALSE);
#endif
		}
	}
}

void	__filelist_refresh(audioplayer_glob_t *glob, t_HList list, int cursor, gboolean reload, gboolean redraw);
void	__ap_view_change(audioplayer_glob_t *glob, t_HWindow win, gu8 back)
{
	t_HWidget widget;
	switch(glob->ap_view){
		case AP_VIEW_PLAYLIST:
			if(!back){
				widget = wnd_get_widget(win, RESID_LISTCTRL_PLAYLIST);
				wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
				widget = wnd_get_widget(win, RESID_LISTCTRL_FILELIST);
				wgt_enable_attr(widget, CTRL_ATTR_VISIBLE);
				wgt_set_focus(widget, TAB_NONE);
				glob->ap_view = AP_VIEW_FILELIST;
				__filelist_refresh(glob, widget, glob->filelist_cursor, TRUE, TRUE);
				__sk_initialize(glob, win);
			}
			break;
		case AP_VIEW_FILELIST:
			if(back){
				widget = wnd_get_widget(win, RESID_LISTCTRL_FILELIST);
				wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
				widget = wnd_get_widget(win, RESID_LISTCTRL_PLAYLIST);
				wgt_enable_attr(widget, CTRL_ATTR_VISIBLE);
				wgt_set_focus(widget, TAB_NONE);
				wgt_redraw(widget, FALSE);
				glob->ap_view = AP_VIEW_PLAYLIST;
				__sk_initialize(glob, win);
			}else{
				glob->ap_view = AP_VIEW_PLAYER;
			}
			break;
		case AP_VIEW_PLAYER:
			if(back){
				if(g_object_valid(glob->player))
					wgt_hide(glob->player);
				glob->ap_view = AP_VIEW_FILELIST;
			}
			break;
	}
}

static void	__playlist_refresh(audioplayer_glob_t *glob, t_HList list, int cursor, gboolean redraw)
{
	int num = g_ds_GetRecordNum(glob->hdb_playlist);
	if(cursor >= num)
		cursor = num-1;
#if defined(FTR_ADD_NEW_IN_PLAYLIST)	
	listctrl_set_count(list, num+LIST_USER+2);
#else
	listctrl_set_count(list, num+LIST_USER);
#endif
	listctrl_set_cursor(list, cursor, FALSE);
	if(wgt_is_visible(list) && redraw){
		wgt_redraw(list, FALSE);
	}
}

//1 *********************************************************************************
//2 <Play List>

static void	CmdNewPlaylist(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();
	char name[AP_PLAYLIST_NAME_MAX+1];

	if(g_ds_GetRecordNum(glob->hdb_playlist) >= AP_PLAYLIST_NUM_MAX){
		msgbox_show(NULL, _CSK(OK), 0, 0, "Playlist full!");
		return;
	}
	strcpy(name, "");
	if(dlg_editor_show(name, sizeof(name), brd_get_string(BRD_HANDLE_SELF, RESID_STR_NEW_PLAYLIST, NULL), NULL, IDF_TEXT_SINGLE)){
		t_DBRecordID rid;
		g_ds_AddNew(glob->hdb_playlist);
		g_ds_SetString(glob->hdb_playlist, PLAYLIST_DBFIELD_NAME, name);
		rid = g_ds_Update(glob->hdb_playlist);
		if(rid == DB_RID_NULL){
			msgbox_show(NULL, _CSK(OK), 0, 0, "New Playlist failed");
			return;
		}
		__playlist_refresh(glob, wnd_get_widget(wnd, RESID_LISTCTRL_PLAYLIST), g_ds_RecordIndex(glob->hdb_playlist, rid), TRUE);
	}
}

static void	CmdImportPlaylist(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();
	char fname[AMOS_PATH_MAX+1];

	if(g_ds_GetRecordNum(glob->hdb_playlist) >= AP_PLAYLIST_NUM_MAX){
		msgbox_show(NULL, _CSK(OK), 0, 0, "Playlist full!");
		return;
	}
	if(FileDlg_Show(fname, sizeof(fname), "x:", "*", OFD_DIR_OPEN)){
		char *fn = fname+strlen(fname)-1;
		char *f;
		t_DBRecordID rid;
		t_HList list;

		while(*fn == '/' || *fn == '\\'){
			*fn-- = 0;
		}
		f = fn;
		while(*f != '/' && *f != '\\' && f > fname)
			f--;
		if(*f == '/' || *f == '\\')
			f++;
		g_ds_AddNew(glob->hdb_playlist);
		g_ds_SetString(glob->hdb_playlist, PLAYLIST_DBFIELD_NAME, f);
		rid = g_ds_Update(glob->hdb_playlist);
		if(rid == DB_RID_NULL){
			msgbox_show(NULL, _CSK(OK), 0, 0, "New Playlist failed");
			return;
		}
		//glob->playlist_cursor = g_ds_RecordIndex(glob->hdb_playlist, rid);

		db_add_dir_to_playlist(glob->hdb_filelist, fname, glob->hdb_playlist, rid);
		__playlist_refresh(glob, wnd_get_widget(wnd, RESID_LISTCTRL_PLAYLIST), g_ds_RecordIndex(glob->hdb_playlist, rid), TRUE);
	}
}

static void	CmdRenamePlaylist(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();
	const char *name;
	char new_name[AP_PLAYLIST_NAME_MAX+1];
	t_DBRecordID rid;
	t_HWidget editor;

	g_ds_GotoByIndex(glob->hdb_playlist, glob->playlist_cursor-LIST_USER);
	name = g_ds_GetString(glob->hdb_playlist, PLAYLIST_DBFIELD_NAME);
	strcpy(new_name, name);
	if(dlg_editor_show(new_name, sizeof(new_name), brd_get_string(BRD_HANDLE_SELF, RESID_STR_RENAME_PLAYLIST, NULL), NULL, 0)){
		if(g_stricmp(new_name, name) == 0)
			return;
		g_ds_SetString(glob->hdb_playlist, PLAYLIST_DBFIELD_NAME, new_name);
		g_ds_Update(glob->hdb_playlist);
		__playlist_refresh(glob, wnd_get_widget(wnd, RESID_LISTCTRL_PLAYLIST), glob->playlist_cursor, TRUE);
	}
}

static void	CmdDeletePlaylist(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();

	if(msgbox_show(NULL, _LSK(YES)|_RSK(CANCEL), 0, 0, "Sure to delete the playlist and all its files?")){
		t_HList listctrl = wnd_get_widget(wnd, RESID_LISTCTRL_PLAYLIST);
		int index = glob->playlist_cursor-LIST_USER;
		t_DBRecordID rid;

		g_ds_GotoByIndex(glob->hdb_playlist, index);
		rid = g_ds_CurrentID(glob->hdb_playlist);
		
		db_remove_filelist(glob->hdb_filelist, rid);
		if(db_delete_playlist(glob->hdb_playlist, rid) > 0){
			__playlist_refresh(glob, listctrl, index, TRUE);
		}
	}
}

t_MenuItem miPlaylist[] = {
#if !defined(FTR_NO_INPUT_KEY)
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_NEW_PLAYLIST, CmdNewPlaylist),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_IMPORT_PLAYLIST, CmdImportPlaylist),
#endif
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_RENAME_PLAYLIST, CmdRenamePlaylist),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_DELETE_PLAYLIST, CmdDeletePlaylist), 
	MITEM_END
};

#if defined(FTR_NO_INPUT_KEY)
void	CmdAddFile(t_HMenu handle, void *data);
void	CmdAddDir(t_HMenu handle, void *data);
t_MenuItem miLSK[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_NEW_PLAYLIST, CmdNewPlaylist),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_IMPORT_PLAYLIST, CmdImportPlaylist),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_ADD_FILES, CmdAddFile), 		//2 添加文件
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_ADD_DIR, CmdAddDir),				//2 添加目录
	MITEM_END
};
#endif

static void	__audioplayer_redraw_playlist(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	t_HWidget listctrl = userdata;
	audioplayer_glob_t *glob = task_get_data();
	t_HDataSet hdb = glob->hdb_playlist;
	int count = g_ds_GetRecordNum(hdb);
	int N;
	
	const char *name;
	t_HImage icon = NULL;
	int x;

	width -= height+2;
	x = height+2;

	N = -1;

	name = __audioplayer_get_playlist_name(glob, index, &N);
	if(g_object_valid(icon)){
		gdi_draw_image_in(gdi, 0, 0, height, height, icon, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE | IDF_HALIGN_STRETCH | IDF_VALIGN_STRETCH);
	}
	g_object_unref(icon);

	gdi_draw_text_in(gdi, x, 0, width, height, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);

	if(N >= 0){
		char str_N[8];
		gu32 old;
		
		old = gdi_set_color(gdi, RGB_GRAY);
		sprintf(str_N, "  (%d)", N);
		gdi_draw_text(gdi, str_N, -1, CHARSET_UTF8);
		gdi_set_color(gdi, old);
		if(prod_ui_ts_support()){
			icon = brd_get_image(brd_get(PACK_GUID_SELF), RESID_ICON_RARROW);
			gdi_draw_image_in(gdi, x+width-2-height, 0, height, height, icon, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
			g_object_unref(icon);
		}
	}
}

static error_t __on_playlist_hook(t_HWidget h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	audioplayer_glob_t *glob = task_get_data();
	t_HWindow win = wgt_parent_window(h);
	switch(evt){
		case WIDGET_OnKeyEvent:
			if(lparam == MVK_OK || lparam == MVK_LSK){
				if(sparam == MMI_KEY_LONG_PRESS){
					int cursor = listctrl_get_cursor(h);
					if(cursor > LIST_FAVORATE/* && cursor < listctrl_get_count(h)-2*/){
						gui_rect_t rt;
						listctrl_rect_by_index(h, cursor, &rt);
						wgt_popup(h, wnd_get_widget(win, RESID_MENU_PLAYLIST), rt.x+rt.dx/2, rt.y+rt.dy, guiBottom|guiRight);
					}
					ret = SUCCESS;
				}
			}
			break;
#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
			if(PEN_TYPE(lparam) == MMI_PEN_LONG_PRESS){
				int cursor = listctrl_get_cursor(h);
				coord_t x0, y0;
				PEN_XY(lparam, x0, y0);
				if(cursor > LIST_FAVORATE/* && cursor < listctrl_get_count(h)-2*/){
					wgt_popup(h, wnd_get_widget(win, RESID_MENU_PLAYLIST), x0, y0, guiBottom|guiRight);
				}
				ret = SUCCESS;
			}
			break;
#endif
		case WIDGET_OnSelected:
			{
#if defined(FTR_ADD_NEW_IN_PLAYLIST)
				int count = listctrl_get_count(h);

				if(sparam == count-2){
					//2 Add New
					CmdNewPlaylist(NULL, win);
					break;
				}else if(sparam == count-1){
					//2 Import
					CmdImportPlaylist(NULL, win);
					break;
				}else
#endif
				{
					glob->playlist_cursor = (int)sparam;//listctrl_get_cursor(listctrl);
					__ap_view_change(glob, win, FALSE);
				}
			}
			break;
	}
	return ret;
}


void	wgt_playlist_init(t_HWidget widget, audioplayer_glob_t *glob)
{
	//2 <Initialization>---<playlist>
	listctrl_attach(widget, __audioplayer_redraw_playlist, widget);
	wgt_hook_register(widget, WIDGET_OnSelChanged|WIDGET_OnSelected|WIDGET_OnKeyEvent|WIDGET_OnPenEvent, __on_playlist_hook, NULL);
	listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
	listctrl_set_rows(widget, 0, prod_ui_list_item_height(), 0);
	listctrl_set_cols(widget, 1, 0);
	listctrl_set_flags(widget, CLFLAG_AUTOTAB_LR|CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM);
	if(glob->ap_view == AP_VIEW_PLAYLIST){
		wgt_enable_attr(widget, CTRL_ATTR_VISIBLE);
	}else{
		wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
	}
	__playlist_refresh(glob, widget, glob->playlist_cursor, FALSE);
}


//2 <AP_VIEW_PLAYLIST>: 	<sparam>=<playlist_rid>, 0 default, 主界面
//2 <AP_VIEW_FILELIST>: 	<sparam>=<playlist_rid>, <lparam>=<filelist_rid>，加入到播放列表
//2 <AP_VIEW_PLAYER>: 	<sparam>=<playlist_rid>, <lparam>=<filelist_rid>，加入到播放列表并播放
error_t	audioplayer_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	audioplayer_glob_t *glob = task_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
		{
			t_HList widget;
			gui_rect_t client;

			wgt_enable_attr(handle, WND_ATTR_DEFAULT);

			//2 <playlist>
			if(sparam == DB_RID_NULL){
				glob->playlist_cursor = 0;
			}else{
				glob->playlist_cursor = g_ds_RecordIndex(glob->hdb_playlist, sparam)+LIST_USER;
			}
			//2 <filelist>
			glob->filelist_rid = lparam;

			wgt_get_client_rect((t_HWidget)handle, &client);

			//2 <Initialization>---<playlist>
			widget = wnd_get_widget(handle, RESID_LISTCTRL_PLAYLIST);
			#if defined(FTR_APBOX_BOTTOM)
			wgt_set_border(widget, BORDER_TYPE_LINE, 0, guiTop);
			wgt_set_bound(widget, 0, 0, client.dx, client.dy);
			#else
			wgt_set_bound(widget, 0, 0, client.dx, client.dy);
			#endif
			wgt_playlist_init(widget, glob);
			
			//2 <Initialization>---<filelist>
			widget = wnd_get_widget(handle, RESID_LISTCTRL_FILELIST);
			#if defined(FTR_APBOX_BOTTOM)
			wgt_set_border(widget, BORDER_TYPE_LINE, 0, guiTop);
			wgt_set_bound(widget, 0, 0, client.dx, client.dy);
			#else
			wgt_set_bound(widget, 0, 0, client.dx, client.dy);
			#endif
			wgt_filelist_init(widget, glob);

			//2 <Initialization>--menus
			#if defined(FTR_NO_INPUT_KEY)
			widget = wgt_create((t_HWidget)handle, "menu");
			wgt_set_resid(widget, RESID_MENU_LSK);
			wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
			menu_bind(widget, miLSK, handle);
			#endif

			menu_bind(wnd_get_widget(handle, RESID_MENU_PLAYLIST), miPlaylist, handle);
			menu_bind(wnd_get_widget(handle, RESID_MENU_FILELIST), miFilelist, handle);

			__sk_initialize(glob, handle);

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
				if(lparam == MVK_LSK){
					//if(glob->ap_view == AP_VIEW_FILELIST){
					//	__ap_view_change(glob, handle, FALSE);
					//	ret = SUCCESS;
					//}
					#if 0
					t_HWidget apbox = wnd_get_widget(handle, RESID_APBOX_MAIN);
					if(!wgt_is_visible(apbox)){
						wgt_popup(wnd_get_skbar(handle), apbox, 0, 0, guiTop);
						ret = SUCCESS;
					}
					#endif
				}else if(lparam == MVK_RSK){
					//2 如果在filelist界面且播放视频，则停止播放并退回到playlist
					 if(glob->ap_view == AP_VIEW_FILELIST ||glob->ap_view == AP_VIEW_PLAYER ){
						 __ap_view_change(glob, handle, TRUE);
						 ret = SUCCESS;
					}
				}
				
			}
			break;
	}
	return ret;
}

