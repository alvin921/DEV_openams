#include "pack_audioplayer.hi"



static void	__detail_change_info(audioplayer_glob_t *glob, t_HWindow handle)
{
	t_HGDI gdi = wgt_get_client_gdi(handle);
	
	t_HMedia codec;
	const char *fname;
	g_ds_GotoByIndex(glob->hdb_filelist, glob->filelist_cursor);
	fname = g_ds_GetString(glob->hdb_filelist, FILELIST_DBFIELD_PATH);
	codec = g_media_new_for_file(fname);
	if(g_object_valid(codec)){
		t_MediaInfoA *info = (t_MediaInfoA*)g_media_info(codec);
		gdi_printf(gdi, "Path: %s\n"
			"Channel: %d\n"
			"BitsPerSample: %d\n"
			"Sample Rate: %d\n"
			"bps: %d\n"
			"duration: %d\n"
			, fname, info->channel, info->bits_per_sample, info->sample_rate, info->bps, info->duration);
		if(info->type == AUD_FORMAT_MP3){
			t_Mp3ExtInfo *extinfo = g_media_extinfo(codec);
			gdi_printf(gdi, "Title: %s\n"
				"Artist: %s\n"
				, extinfo->title, extinfo->artist);
		}
		g_object_unref_0(codec);
	}else{
		gdi_printf(gdi, "Path: %s\n", fname);
	}
	g_object_unref(gdi);
}

error_t	audioplayer_DetailWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	audioplayer_glob_t *glob = task_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
		{
			t_HWidget sk;
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			sk	= wnd_get_skbar(handle);
			if(g_object_valid(sk)){
				sk_set_text(sk, SK_LSK, NULL, RESID_STR_OK);
				sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
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
			
			__detail_change_info(glob, handle);

			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam != MMI_KEY_PRESS)
				break;
			switch(lparam){
				case MVK_LEFT:
					if(!g_ds_Bof(glob->hdb_filelist)){
						//listctrl_next(wnd_get_widget(wgt_parent_window(h), RESID_LISTCTRL_FILELIST), -1, TRUE);
						glob->filelist_cursor--;
						wgt_redraw(handle, FALSE);
					}
					ret = SUCCESS;
					break;
				case MVK_RIGHT:
					if(!g_ds_Eof(glob->hdb_filelist)){
						//listctrl_next(wnd_get_widget(wgt_parent_window(h), RESID_LISTCTRL_FILELIST), +1, TRUE);
						glob->filelist_cursor++;
						wgt_redraw(handle, FALSE);
					}
					ret = SUCCESS;
					break;
			}
			break;
	}
	return ret;
}


