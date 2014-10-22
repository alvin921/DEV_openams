#include "pack_audioplayer.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "audioplayer_brd_le.h"
#else
#include "audioplayer_brd_be.h"
#endif
#endif


static void	audioplayer_glob_destroy(audioplayer_glob_t *glob)
{
	if(glob){
	
		g_object_unref(glob->hdb_playlist);
		g_object_unref(glob->hdb_filelist);

		FREEIF(glob);
	}
}

static audioplayer_glob_t *	audioplayer_glob_new(t_sparam sparam, t_lparam lparam)
{
	audioplayer_glob_t *glob;

	db_audplayer_init();
	
	glob = MALLOC(sizeof(audioplayer_glob_t));
	memset(glob, 0, sizeof(audioplayer_glob_t));

	glob->hdb_playlist = g_ds_open(AUDPLAYER_DB_FNAME, AP_DBTAB_PLAYLIST);
	glob->hdb_filelist = g_ds_open(AUDPLAYER_DB_FNAME, AP_DBTAB_FILELIST);

	glob->playlist_cursor = -1;
	glob->filelist_cursor = -1;
	glob->ap_state = AP_STATE_IDLE;
	glob->playing_playlist_cursor = -1;
	glob->playing_filelist_cursor = -1;

	mplayer_recent_read(&glob->mp_recent);
	glob->recent_dirt = 0;
	return glob;
}

static int	audioplayer_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	audioplayer_glob_t *glob;
	switch(id){
		case MOD_ID_MAIN:
			glob = audioplayer_glob_new(sparam, lparam);
			app_set_data(glob);
			
			if(wnd_show(wnd_create_by_resid(audioplayer_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN)))
				ret = app_loop(NULL);
			
			audioplayer_glob_destroy(glob);
			break;
		case MOD_ID_FILE_OPEN:
			{
				const char *fname = (const char *)lparam;
				t_HWindow h;
				int result;
				t_HMedia codec;
				t_DBRecordID rid_playlist, rid_filelist;

				//2 Check codec
				if((codec = g_media_new_for_file(fname)) == NULL){
					msgbox_show(NULL, _CSK(OK), 0, 0, "Invalid format");
					task_exit(0);
					return SUCCESS;
				}
				g_object_unref_0(codec);
				
				glob = audioplayer_glob_new(sparam, lparam);
				app_set_data(glob);
				
				h = wnd_create_by_resid(audioplayer_OpenFileWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_AUDIO_OPEN);
				result = wnd_modal(h);
				
				if(result < 0){
					//2 Cancel
					ret = 0;
				}else if(result == 0){
					//2 Play it!
					h = wnd_create_by_resid(audioplayer_PlayerWndMsgHandler, sparam, lparam, brd_get(PACK_GUID_SELF), RESID_WIN_PLAYER);
					wnd_show(h);
					ret = app_loop(NULL);
				}else{
					//2 Add to playlist(result)
					rid_playlist = result;
					rid_filelist = db_add_file_to_playlist(glob->hdb_filelist, (char*)fname, glob->hdb_playlist, rid_playlist);
					if(rid_filelist == DB_RID_NULL){
						msgbox_show(NULL, _CSK(OK), 0, 0, "Error to add to playlist");
						ret = 0;
					}else{
						glob->ap_view = AP_VIEW_FILELIST;
						h = wnd_create_by_resid(audioplayer_MainWndMsgHandler, rid_playlist, rid_filelist, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN);
						wnd_show(h);
						ret = app_loop(NULL);
					}
				}
				audioplayer_glob_destroy(glob);
			}
			break;
	}
	return ret;
}

static t_AmsModule		pack_audioplayer_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		8*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	{
		MOD_ID_FILE_OPEN, 		// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		8*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	MOD_INFO_END
};

extern t_GClass class_apbox;

static error_t		pack_audioplayer_OnLoad(void)
{
	wgt_register_class(&class_apbox);
	return SUCCESS;
}

static error_t		pack_audioplayer_OnUnload(void)
{
	wgt_unregister_class(&class_apbox);
	return SUCCESS;
}

t_AmsPackage pack_info_audioplayer = {
	PACK_GUID_SELF
	, pack_audioplayer_OnLoad	// OnLoad
	, pack_audioplayer_OnUnload	// OnUnload
	, audioplayer_AppEntry

	, pack_audioplayer_modules
	
#if defined(TARGET_WIN)
	, audioplayer_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_audioplayer;
}

#endif


