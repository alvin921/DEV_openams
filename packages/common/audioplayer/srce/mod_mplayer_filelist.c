#include "pack_audioplayer.hi"




//1 *********************************************************************************
//2 <File List>


void	__filelist_refresh(audioplayer_glob_t *glob, t_HList list, int cursor, gboolean reload, gboolean redraw)
{
	int num;

	if(glob->playlist_cursor == LIST_ALL){
		if(reload){
			g_ds_CondClear(glob->hdb_filelist);
			g_ds_Query(glob->hdb_filelist, TRUE);
		}
		num = g_ds_GetRecordNum(glob->hdb_filelist);
	}else if(glob->playlist_cursor == LIST_RECENT){
		num = glob->mp_recent.count;
	}else{
		if(reload){
			t_DBRecordID rid;
			g_ds_GotoByIndex(glob->hdb_playlist, glob->playlist_cursor-LIST_USER);
			rid = g_ds_CurrentID(glob->hdb_playlist);
			g_ds_CondSet1(glob->hdb_filelist, FILELIST_DBFIELD_LIST, DBREL_EQ, rid);
			g_ds_Query(glob->hdb_filelist, TRUE);
		}
		num = g_ds_GetRecordNum(glob->hdb_filelist);
	}
#if defined(FTR_FILELIST_SHOW_BACK_UP)
	num ++;
#endif
	if(cursor >= num)
		cursor = num-1;
	listctrl_set_count(list, num);
	listctrl_set_cursor(list, cursor, FALSE);
	if(wgt_is_visible(list)){
		if(redraw)
			wgt_redraw(list, FALSE);
	}
}


void	CmdAddFile(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();
	char fname[AMOS_PATH_MAX+1];
	char *fn;

	if(FileDlg_Show(fname, sizeof(fname), "x:", "*", OFD_FILE_OPEN)){
		t_DBRecordID rid;

		g_ds_GotoByIndex(glob->hdb_playlist, glob->playlist_cursor-LIST_USER);
		rid = g_ds_CurrentID(glob->hdb_playlist);
		rid = db_add_file_to_playlist(glob->hdb_filelist, fname, glob->hdb_playlist, rid);
		if(rid == DB_RID_NULL){
			msgbox_show(NULL, _CSK(OK), 0, 0, "<%s> unsupported audio format!", fname);
		}else{
			__filelist_refresh(glob, wnd_get_widget(wnd, RESID_LISTCTRL_FILELIST), g_ds_RecordIndex(glob->hdb_filelist, rid), FALSE, TRUE);
		}
	}
}

void	CmdAddDir(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();
	char fname[AMOS_PATH_MAX+1];

	if(FileDlg_Show(fname, sizeof(fname), "x:", "*", OFD_DIR_OPEN)){
		t_DBRecordID rid;

		g_ds_GotoByIndex(glob->hdb_playlist, glob->playlist_cursor);
		rid = g_ds_CurrentID(glob->hdb_playlist);

		if(db_add_dir_to_playlist(glob->hdb_filelist, fname, glob->hdb_playlist, rid) > 0){
			__filelist_refresh(glob, wnd_get_widget(wnd, RESID_LISTCTRL_FILELIST), glob->filelist_cursor, FALSE, TRUE);
		}
	}
}

static void	CmdAddFavorate(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();

}

static void	CmdCleanupPlaylist(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();

	if(msgbox_show(NULL, _LSK(YES)|_RSK(CANCEL), 0, 0, "Sure to delete the playlist and all its files?") == MBB_YES){
		t_DBRecordID rid;
		g_ds_GotoByIndex(glob->hdb_playlist, glob->playlist_cursor);
		rid = g_ds_CurrentID(glob->hdb_playlist);
		
		if(db_remove_filelist(glob->hdb_filelist, rid) > 0){
			__filelist_refresh(glob, wnd_get_widget(wnd, RESID_LISTCTRL_FILELIST), glob->filelist_cursor, FALSE, TRUE);
		}
	}
}

static void	CmdRemoveInvalidFiles(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();
	if(db_remove_invalid_files(glob->hdb_filelist) > 0){
		__filelist_refresh(glob, wnd_get_widget(wnd, RESID_LISTCTRL_FILELIST), glob->filelist_cursor, FALSE, TRUE);
	}
}

static void	CmdRemoveFile(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();
	t_DBRecordID rid;

	g_ds_GotoByIndex(glob->hdb_filelist, glob->filelist_cursor);
	rid = g_ds_CurrentID(glob->hdb_filelist);

	if(db_remove_file(glob->hdb_filelist, rid, glob->hdb_playlist)){
		__filelist_refresh(glob, wnd_get_widget(wnd, RESID_LISTCTRL_FILELIST), glob->filelist_cursor, FALSE, TRUE);
	}
}

static void	CmdDeleteFile(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();
	t_DBRecordID rid;

	g_ds_GotoByIndex(glob->hdb_filelist, glob->filelist_cursor);
	rid = g_ds_CurrentID(glob->hdb_filelist);

	if(db_delete_file(glob->hdb_filelist, rid, glob->hdb_playlist)){
		__filelist_refresh(glob, wnd_get_widget(wnd, RESID_LISTCTRL_FILELIST), glob->filelist_cursor, FALSE, TRUE);
	}
}

void	CmdConvertFile(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();
	const char *fname;
	char *newfname, *r;
	int len;

	g_ds_GotoByIndex(glob->hdb_filelist, glob->filelist_cursor);
	fname = g_ds_GetString(glob->hdb_filelist, FILELIST_DBFIELD_PATH);
	len = strlen(fname);
	newfname = MALLOC(strlen(fname)+5);
	strcpy(newfname, fname);
	r = strrchr(newfname, '.');
	if(r == NULL)
		r = newfname+len;
	strcpy(r, ".wav");
	if(aud_file_to_wavfile(fname, newfname) < 0){
		msgbox_show(NULL, _CSK(OK), 0, 0, "Error to convert file");
	}
	FREE(newfname);
}

void	CmdLocateFile(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();

	g_ds_GotoByIndex(glob->hdb_filelist, glob->filelist_cursor);
	FileDlg_Locate(g_ds_GetString(glob->hdb_filelist, FILELIST_DBFIELD_PATH));
}

void	CmdViewDetail(t_HMenu handle, void *data)
{
	wnd_show(wnd_create_by_resid(audioplayer_DetailWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_VIEW_DETAIL));
}

void	CmdEditInfo(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	audioplayer_glob_t *glob = app_get_data();

}

void	CmdAdd2Playlist(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	t_HWindow h;
	audioplayer_glob_t *glob = app_get_data();
	const char *fname;

	t_DBRecordID rid;

	h = wnd_create_by_resid(audioplayer_PlaylistWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_SELECT_PLAYLIST);
	rid = wnd_modal(h);
	if(rid != DB_RID_NULL){
		g_ds_GotoByIndex(glob->hdb_playlist, glob->playlist_cursor);
		if(rid != g_ds_CurrentID(glob->hdb_playlist)){
			g_ds_GotoByIndex(glob->hdb_filelist, glob->filelist_cursor);
			fname = g_ds_GetString(glob->hdb_filelist, FILELIST_DBFIELD_PATH);
			
			rid = db_add_file_to_playlist(glob->hdb_filelist, (char*)fname, glob->hdb_playlist, rid);
			if(rid == DB_RID_NULL){
				msgbox_show(NULL, _CSK(OK), 0, 0, "<%s> unsupported audio format!", fname);
			//}else{
			//	listctrl_set_cursor(wnd_get_widget(wnd, RESID_LISTCTRL_FILELIST), g_ds_RecordIndex(glob->hdb_filelist, rid), TRUE);
			}
		}
	}

}

t_MenuItem miFilelist[] = {
#if !defined(FTR_NO_INPUT_KEY)
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_ADD_FILES, CmdAddFile),			//2 添加文件
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_ADD_DIR, CmdAddDir),				//2 添加目录
#endif
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_ADD_TO_PLAYLIST, CmdAdd2Playlist), 	//2 加入播放列表
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_CLEANUP_PLAYLIST, CmdCleanupPlaylist), 
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_REMOVE_INVALID_FILES, CmdRemoveInvalidFiles),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_REMOVE_FILE, CmdRemoveFile),		//2 移除文件
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_DELETE_FILE, CmdDeleteFile),		//2 删除文件
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_CONVERT_FILE, CmdConvertFile),	//2 转换文件格式
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_LOCATE_FILE, CmdLocateFile), 		//2 打开歌曲目录
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_VIEW_DETAIL, CmdViewDetail),		//2 浏览歌曲信息
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_EDIT_FILE_INFO, CmdEditInfo),		//2 修改歌曲信息
	MITEM_END
};

static error_t __on_filelist_hook(t_HWidget h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	audioplayer_glob_t *glob = task_get_data();
	t_HWindow win = wgt_parent_window(h);
	switch(evt){
		case WIDGET_OnSelChanged:
			{
				t_DBRecordID rid;
				
				glob->filelist_cursor = lparam;
				#if defined(FTR_FILELIST_SHOW_BACK_UP)
				glob->filelist_cursor--;
				#endif
			}
			
			ret = SUCCESS;
			break;
		case WIDGET_OnSelected:
#if defined(FTR_FILELIST_SHOW_BACK_UP)
			if(sparam == 0){
				__ap_view_change(glob, win, TRUE);
				break;
			}
#endif
			if(listctrl_get_count(h)){
				gboolean toplay = FALSE;
				if(glob->playing_playlist_cursor != glob->playlist_cursor || glob->playing_filelist_cursor != sparam)
					toplay = TRUE;
				
				if(glob->playing_playlist_cursor == glob->playlist_cursor && glob->playing_filelist_cursor != sparam){
					//2 重绘之前播放的文件
#if defined(FTR_FILELIST_SHOW_BACK_UP)
					listctrl_redraw_item(h, glob->playing_filelist_cursor+1);
#else
					listctrl_redraw_item(h, glob->playing_filelist_cursor);
#endif
				}
				glob->playing_playlist_cursor = glob->playlist_cursor;
				glob->playing_filelist_cursor = sparam;
#if defined(FTR_FILELIST_SHOW_BACK_UP)
				glob->playing_filelist_cursor--;
#endif

				listctrl_redraw_item(h, sparam);
				//2 创建并加载播放界面
				if(!g_object_valid(glob->player)){
					glob->player = wnd_create_by_resid(audioplayer_PlayerWndMsgHandler, sparam, lparam, brd_get(PACK_GUID_SELF), RESID_WIN_PLAYER);
					wnd_show(glob->player);
				}else{
					wnd_show(glob->player);
					wnd_set_focus(glob->player);
					if(toplay)
						apbox_play(wnd_get_widget(glob->player, RESID_APBOX_PLAYER), AP_PLAY_NONE);
				}
			}
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_LONG_PRESS){
				//2 Popup menu
				if(lparam == MVK_OK || lparam == MVK_LSK){
					if(/*glob->playlist_cursor >= LIST_USER || */listctrl_get_count(h)){
						gui_rect_t rt;
						listctrl_rect_by_index(h, listctrl_get_cursor(h), &rt);
						wgt_popup(h, wnd_get_widget(win, RESID_MENU_FILELIST), rt.x+rt.dx/2, rt.y+rt.dy, guiBottom|guiRight);
						
					}
					ret = SUCCESS;
				}
			}
			break;
#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
			if(PEN_TYPE(lparam) == MMI_PEN_LONG_PRESS){
				coord_t x0, y0;
				PEN_XY(lparam, x0, y0);
				if(/*glob->playlist_cursor >= LIST_USER || */listctrl_get_count(h)){
					wgt_popup(h, wnd_get_widget(win, RESID_MENU_FILELIST), x0, y0, guiBottom|guiRight);
				}
				ret = SUCCESS;
			}
			break;
#endif
	}
	return ret;
}

static void	__audioplayer_redraw_filelist(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata)
{
	t_HWidget listctrl = userdata;
	audioplayer_glob_t *glob = task_get_data();
	t_HDataSet hdb = glob->hdb_filelist;
	const char *name, *path;
	gu32 duration;
	t_HImage icon = NULL;

#if defined(FTR_FILELIST_SHOW_BACK_UP)
	if(index == 0){
		name = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_TO_PLAYLIST, "To Playlist");
		gdi_draw_text_in(gdi, height+2, 0, width-height-2, height, name, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
		return;
	}
	index--;
#endif
	if(glob->playlist_cursor == LIST_RECENT){
		g_ds_Goto(hdb, glob->mp_recent.recids[index]);
	}else{
		g_ds_GotoByIndex(hdb, index);
	}

	path = g_ds_GetString(hdb, FILELIST_DBFIELD_PATH);
	name = g_ds_GetString(hdb, FILELIST_DBFIELD_TITLE);;
	duration = g_ds_GetValue(hdb, FILELIST_DBFIELD_DURATION);
	duration /= 1000;
	if(duration == 0)
		duration = 1;

	{
		int x;
		char str[12];
		
		if(glob->ap_state != AP_STATE_IDLE && glob->playing_playlist_cursor == glob->playlist_cursor && glob->playing_filelist_cursor == index){
			icon = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_IMG_PLAYING, (glob->ap_state==AP_STATE_PLAYING)?1:0));
			if(g_object_valid(icon)){
				gdi_draw_image_in(gdi, 0, 0, height, height, icon, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
			}
		}
		if(duration >= 60*60)
			sprintf(str, "%02d:%02d:%02d", duration/60/60, (duration/60)%60, duration%60);
		else
			sprintf(str, "%02d:%02d", duration/60, duration%60);
		width -= height+2;
		x = height+2;
		gdi_measure_text(gdi, str, 5, CHARSET_US_ASCII, width, &duration, NULL, NULL);

		width -= duration;
		gdi_draw_text_in(gdi, x, 0, width, height, name, -1, CHARSET_UNDEF, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
		x += width;
		width = duration;
		gdi_draw_text_in(gdi, x, 0, width, height, str, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
	}
	g_object_unref(icon);
}

void	wgt_filelist_init(t_HWidget widget, audioplayer_glob_t *glob)
{
	//2 <Initialization>---<filelist>
	listctrl_attach(widget, __audioplayer_redraw_filelist, widget);
	wgt_hook_register(widget, WIDGET_OnSelChanged|WIDGET_OnSelected|WIDGET_OnKeyEvent|WIDGET_OnPenEvent, __on_filelist_hook, NULL);
	listctrl_set_viewtype(widget, GUI_VIEWTYPE_NORMAL);
	listctrl_set_flags(widget, CLFLAG_AUTOTAB_LR|CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM);
	if(glob->ap_view == AP_VIEW_FILELIST){
		wgt_enable_attr(widget, CTRL_ATTR_VISIBLE);
	}else{
		wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
	}
	__filelist_refresh(glob, widget, glob->filelist_cursor, TRUE, FALSE);
}

