#include "pack_audioplayer.hi"


#if (LCD_WIDTH == 240) && (LCD_HEIGHT == 320)
#elif (LCD_WIDTH == 320) && (LCD_HEIGHT == 240)
#elif (LCD_WIDTH == 176) && (LCD_HEIGHT == 220)
#elif (LCD_WIDTH == 220) && (LCD_HEIGHT == 176)
#elif (LCD_WIDTH == 128) && (LCD_HEIGHT == 160)
#elif (LCD_WIDTH == 160) && (LCD_HEIGHT == 128)
#endif

#define	A_ROW_H	16
#define	P_HEIGHT	10
#define	B_WIDTH	40
#define	B_HEIGHT	40


static t_ApboxConfig g_apbox_attr = {
	//2 Image RESID
	{
		RESID_IMG_APBOX_BG,
		RESID_IMG_PROGRESS,
		RESID_IMG_VOLUME,
		RESID_IMG_PREV,
		RESID_IMG_NEXT,
		RESID_IMG_PLAY,
		RESID_IMG_PAUSE,
		RESID_IMG_REPEAT,
		RESID_IMG_UP_DOWN,
	},
	//2 Region Bound
	{
		//2 <title>
#if defined(FTR_APBOX_TIME_PAST_REMAIN)
		{40				, 0 		, LCD_WIDTH-80	, A_ROW_H},
#else
		{0				, 0 		, LCD_WIDTH-80	, A_ROW_H},
#endif
		//2 progress
		{0, 17, LCD_WIDTH, 13},
		//2 volume
#if (LCD_WIDTH==128)
		{55, 29, 52, 11},
#elif (LCD_WIDTH==176)
		{75, 31, 73, 11},
#else
		{103, 36, 98, 17},
#endif
		//2 <time>
#if defined(FTR_APBOX_TIME_PAST_REMAIN)
		{0	, 0 		, 40	, A_ROW_H},
		{LCD_WIDTH-40	, 0 		, 40	, A_ROW_H},
#else
		{LCD_WIDTH-80	, 0 		, 80	, A_ROW_H},
#endif		
#if (LCD_WIDTH==128)
			{2, 30, 14, 9},	//2 Button: <prev>
			{20, 28, 12, 13},	//2 Button: <play>
			{37, 30, 14, 9},	//2 Button: <next>
			{109, 31, 9, 7},	//2 Button: <repeat>
			{118, 31, 9, 7},	//2 Button: <detail>
#elif (LCD_WIDTH==176)
			{3, 31, 19, 11}, //2 Button: <prev>
			{28, 28, 17, 17},	//2 Button: <play>
			{51, 31, 19, 11},	//2 Button: <next>
			{150, 32, 11, 9},	//2 Button: <repeat>
			{162, 32, 13, 9},	//2 Button: <detail>
#else
			{4, 37, 25, 15},	//2 Button: <prev>
			{38, 33, 23, 23},	//2 Button: <play>
			{70, 37, 25, 15},	//2 Button: <next>
			{205, 38, 15, 13},	//2 Button: <repeat>
			{222, 38, 16, 11},	//2 Button: <detail>
#endif
	},
	LCD_WIDTH, 
#if (LCD_WIDTH==128)
	44,		//2 bg height
	1, 124,	//2 progress 
	1, 50,	//2 volume
#elif (LCD_WIDTH==176)
	48, 	//2 bg height
	3, 170, //2 progress 
	2, 69,	//2 volume
#else
	60,		//2 bg height
	4, 232,	//2 progress 
	4, 90,	//2 volume
#endif
#if !defined(FTR_NO_INPUT_KEY)
	MVK_4,	MVK_5,	MVK_6,	// prev, play, next
	MVK_STAR,	MVK_HASH,	// vol -/+
	MVK_0,		// mode
	MVK_1,		// detail
#endif

};


static char *	audplayer_song_name_callback(void *userdata, gu8 mode, gu8 repeat)
{
	t_HWidget apbox = userdata;
	t_HWindow handle = wgt_parent_window(apbox);
	audioplayer_glob_t *glob = app_get_data();
	int count, index;
	const char *fname;

	if(app_current_module() == MOD_ID_FILE_OPEN && wgt_get_resid(handle) == RESID_WIN_PLAYER){
		if(mode == AP_PLAY_CURRENT)
			return NULL;
		return g_str_dup((char *)wgt_get_tag(handle));
	}

	if(glob->playing_playlist_cursor == LIST_RECENT){
		count = glob->mp_recent.count;
	}else{
		count = g_ds_GetRecordNum(glob->hdb_filelist);
	}
	if(count <= 0)
		return NULL;
	index = glob->playing_filelist_cursor;
	if(mode == AP_PLAY_PREV){
		index--;
		if(index < 0)
			index = count-1;
	}else if(mode == AP_PLAY_NEXT){
		index++;
		if(index >= count)
			index = 0;
	}else if(mode == AP_PLAY_CURRENT){
		//2 根据当前repeat模式来选择下一首
		if(repeat == AP_REPEAT_NONE){
			//3 顺序加载
			index++;
			if(index >= count)
				return NULL;
		}else if(repeat == AP_REPEAT_ALL){
			index++;
			if(index >= count)
				index = 0;
		}else if(repeat == AP_REPEAT_ONE){
			;
		}else{
		}
	}
	if(index < 0){
		if(glob->playing_playlist_cursor < 0)
			glob->playing_playlist_cursor = glob->playlist_cursor;
		index = 0;
	}
	glob->playing_filelist_cursor = index;
	if(glob->playing_playlist_cursor == LIST_RECENT){
		g_ds_Goto(glob->hdb_filelist, glob->mp_recent.recids[index]);
	}else{
		g_ds_GotoByIndex(glob->hdb_filelist, index);
	}
	g_printf("Playing <%s>", g_ds_GetString(glob->hdb_filelist, FILELIST_DBFIELD_PATH));
	fname = g_ds_GetString(glob->hdb_filelist, FILELIST_DBFIELD_PATH);
	return g_str_dup(fname);
}


static void	audplayer_state_change_callback(void *userdata, gu8 state, int errcode)
{
	t_HWidget apbox = userdata;
	t_HWindow handle = wgt_parent_window(apbox);
	audioplayer_glob_t *glob = app_get_data();
	gu8 repeat = apbox_get_repeat(apbox);

	if(app_current_module() == MOD_ID_FILE_OPEN && wgt_get_resid(handle) == RESID_WIN_PLAYER){
		//2 不能stop,因为在idle->play时也会执行到这里
		//apbox_stop(apbox, TRUE);
		return;
	}
	glob->ap_state = apbox_get_state(apbox);
#if 0
if(glob->ap_view == AP_VIEW_FILELIST){
	t_HWidget list = wnd_get_widget(handle, RESID_LISTCTRL_FILELIST);
	if(wgt_is_focused(list))
#if defined(FTR_FILELIST_SHOW_BACK_UP)
		listctrl_redraw_item(list, glob->playing_filelist_cursor+1);
#else
		listctrl_redraw_item(list, glob->playing_filelist_cursor);
#endif
}
#else
//	if(glob->ap_view == AP_VIEW_FILELIST && glob->playing_playlist_cursor == glob->playlist_cursor){
//		t_HWidget list = wnd_get_widget(handle, RESID_LISTCTRL_FILELIST);
//		listctrl_set_cursor(list, glob->playing_filelist_cursor, TRUE);
//	}
#endif
}

void	wgt_apbox_init(t_HWidget widget, audioplayer_glob_t *glob)
{
	//2 <Initialization>---<filelist>
	if(g_object_valid(widget)){
		apbox_bind(widget, &g_apbox_attr, (apbox_get_song_name_callback_t)audplayer_song_name_callback, audplayer_state_change_callback, widget);
	}
}

error_t	audioplayer_PlayerWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	audioplayer_glob_t *glob = task_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
		{
			t_HList widget, sk;
			gui_rect_t client;
			if(app_current_module() == MOD_ID_FILE_OPEN){
				char *fname = g_str_dup((char *)lparam);
				wgt_set_tag(handle, fname);
			}

			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			
			widget = wnd_get_widget(handle, RESID_APBOX_PLAYER);
			wgt_get_client_rect((t_HWidget)handle, &client);
			//wgt_set_bound(widget, 0, 0, client.dx, client.dy);
			
#if defined(FTR_APBOX_BOTTOM)
			wgt_set_position(widget, 0, client.dy-wgt_bound_dy(widget));
#else
			wgt_set_position(widget, 0, 0);
#endif
			if(g_object_valid(widget)){
#if !defined(FTR_NO_INPUT_KEY)
				wgt_disable_attr(widget, CTRL_ATTR_TABSTOP);
#endif
				apbox_bind(widget, &g_apbox_attr, audplayer_song_name_callback, audplayer_state_change_callback, widget);
				apbox_play(widget, AP_PLAY_NONE);
			}
			
			sk	= wnd_get_skbar(handle);
			if(g_object_valid(sk)){
				//sk_set_text(sk, SK_LSK, NULL, RESID_STR_OPTION);
				sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
			}
			ret = SUCCESS;
		}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			FREEIF(wgt_get_tag(handle));
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
			if(sparam == MMI_KEY_UP && lparam == MVK_RSK){
				if(glob->ap_state == AP_STATE_IDLE){
					wnd_send_close(handle, 0);
					glob->player = NULL;
				}else{
					wgt_hide(handle);
				}
				ret = SUCCESS;
			}
			break;
	}
	return ret;
}


