
#define DB_AUDPLAYER_C

#include "ams.h"
#include "db_audioplayer.h"

gboolean	mplayer_recent_read(t_MPlayerSaved *pRecent)
{
	int fd;
	if(pRecent)
		memset(pRecent, 0, sizeof(t_MPlayerSaved));
	
	if(fsi_stat(MPLAYER_RECENT_FNAME, NULL)){
		int i;
		fd = fsi_open(MPLAYER_RECENT_FNAME, _O_CREAT|_O_RDWR, _S_IREAD|_S_IWRITE);
		if(fd < 0)
			return FALSE;
		fsi_write_u32(fd, 0, TARGET_ENDIAN);
		for(i = 0; i < MPLAYER_RECENT_MAX; i++)
			fsi_write_u16(fd, 0, TARGET_ENDIAN);
		fsi_close(fd);
		return TRUE;
	}
	fd = fsi_open(MPLAYER_RECENT_FNAME, _O_RDONLY, _S_IREAD|_S_IWRITE);
	if(fd < 0)
		return FALSE;
	if(pRecent)
		fsi_read(fd, pRecent, sizeof(t_MPlayerSaved));
	fsi_close(fd);
	return TRUE;
}


void	mplayer_recent_write(t_MPlayerSaved *pRecent)
{
	if(pRecent){
		int fd;
		fd = fsi_open(MPLAYER_RECENT_FNAME, _O_RDWR, _S_IREAD|_S_IWRITE);
		if(fd >= 0){
			fsi_write(fd, pRecent, sizeof(t_MPlayerSaved));
			fsi_close(fd);
		}
	}
}

//2 如果recent数据有更新则返回TRUE
gboolean	mplayer_recent_add(t_MPlayerSaved *pRecent, t_DBRecordID fid)
{
	if(pRecent && fid != DB_RID_NULL){
		//2 如果fid已存在recent中
		int i, j;
		for(i = 0; i < pRecent->count; i++){
			if(pRecent->recids[i] == fid){
				if( i == pRecent->count-1)
					return FALSE;
				for(j = i; j < pRecent->count-1; j++)
					pRecent->recids[j] = pRecent->recids[j+1];
				pRecent->recids[pRecent->count-1] = fid;
				return TRUE;
			}
		}
		//2 recent已满
		if(pRecent->count == MPLAYER_RECENT_MAX){
			for(i = 0; i < MPLAYER_RECENT_MAX-1; i++)
				pRecent->recids[i] = pRecent->recids[i+1];
			pRecent->count--;
		}
		//2 加入recent中
		pRecent->recids[pRecent->count++] = fid;
		return TRUE;
	}
	return FALSE;
}

//2 如果recent数据有更新则返回TRUE
gboolean	mplayer_recent_remove(t_MPlayerSaved *pRecent, t_DBRecordID fid)
{
	if(pRecent && fid != DB_RID_NULL){
		//2 如果fid已存在recent中
		int i, j;
		for(i = 0; i < pRecent->count; i++){
			if(pRecent->recids[i] == fid){
				for(j = i; j < pRecent->count-1; j++)
					pRecent->recids[j] = pRecent->recids[j+1];
				pRecent->recids[pRecent->count-1] = DB_RID_NULL;;
				pRecent->count--;
				return TRUE;
			}
		}
	}
	return FALSE;
}

//2 如果recent数据有更新则返回TRUE
gboolean	mplayer_recent_remove_idx(t_MPlayerSaved *pRecent, int idx)
{
	if(pRecent && idx >= 0 && idx < pRecent->count){
		int j;
		for(j = idx; j < pRecent->count-1; j++)
			pRecent->recids[j] = pRecent->recids[j+1];
		pRecent->recids[pRecent->count-1] = DB_RID_NULL;;
		pRecent->count--;
		return TRUE;
	}
	return FALSE;
}

void		db_audplayer_init(void)
{
	if(fsi_stat(AUDPLAYER_DB_FNAME, NULL)){
		t_HDB h;
		t_HDataSet hdb_playlist, hdb_filelist;
		t_DBRecordID rid;
		
		g_printf("db file not exists, create it");
		g_db_create(AUDPLAYER_DB_FNAME);
		h = g_db_open(AUDPLAYER_DB_FNAME);
		if(g_object_valid(h)){
			g_db_create_table(h, AP_DBTAB_PLAYLIST, AP_PLAYLIST_NUM_MAX, playlist_db_fields, ARRAY_SIZE(playlist_db_fields));
			g_db_create_table(h, AP_DBTAB_FILELIST, AP_FILELIST_NUM_MAX, filelist_db_fields, ARRAY_SIZE(filelist_db_fields));

			hdb_playlist = g_db_open_table(h, AP_DBTAB_PLAYLIST);
			hdb_filelist = g_db_open_table(h, AP_DBTAB_FILELIST);
			g_printf("playlist=%x, filelist=%x", hdb_playlist, hdb_filelist);
			if(g_object_valid(hdb_playlist)){
				g_ds_AddNew(hdb_playlist);
				g_ds_SetString(hdb_playlist, PLAYLIST_DBFIELD_NAME, "Default");
				rid = g_ds_Update(hdb_playlist);
				g_printf("rid(Default)=%d", rid);
				g_ds_AddNew(hdb_playlist);
				g_ds_SetString(hdb_playlist, PLAYLIST_DBFIELD_NAME, "Favorate");
				rid = g_ds_Update(hdb_playlist);
				g_printf("rid(Favorate)=%d", rid);
			}
			g_object_unref(hdb_playlist);
			g_object_unref(hdb_filelist);
			g_object_unref(h);
		}

	}else{
		g_printf("db file exists");
	}
}

t_DBRecordID	db_add_file_to_playlist(t_HDataSet hdb_filelist, char *fname, t_HDataSet hdb_playlist, t_DBRecordID rid)
{
	t_HMedia codec = g_media_new_for_file(fname);
	t_DBRecordID rid_filelist = DB_RID_NULL;
	
	if(g_object_valid(codec)){
		char *artist, *title;
		if(!MEDIA_IS_IMAGE(g_media_type(codec))){
		}
		g_printf("Add file(%s)", fname);
		g_ds_AddNew(hdb_filelist);
		g_ds_SetValue(hdb_filelist, FILELIST_DBFIELD_LIST, rid);
		g_ds_SetString(hdb_filelist, FILELIST_DBFIELD_PATH, fname);
		g_ds_SetValue(hdb_filelist, FILELIST_DBFIELD_TYPE, g_media_type(codec));
		g_ds_SetValue(hdb_filelist, FILELIST_DBFIELD_FAVORATE, 0);
		g_ds_SetValue(hdb_filelist, FILELIST_DBFIELD_DURATION, g_media_duration(codec));
		artist = title = NULL;
		if(g_media_type(codec) == AUD_FORMAT_MP3){
			t_Mp3ExtInfo *extinfo = g_media_extinfo(codec);
			if(extinfo){
				if(extinfo->artist[0])
					artist = extinfo->artist;
				if(extinfo->title[0])
					title = extinfo->title;
			}
		}
		if(title == NULL){
			title = fname + strlen(fname);
			while((*(title-1) != '/' && *(title-1) != '\\') && title > fname)
				title--;
		}
		if(artist == NULL)
			g_ds_SetString(hdb_filelist, FILELIST_DBFIELD_TITLE, title);
		else{
			char *tmp = MALLOC(strlen(artist)+strlen(title)+2);
			sprintf(tmp, "%s-%s", artist, title);
			g_ds_SetString(hdb_filelist, FILELIST_DBFIELD_TITLE, tmp);
			FREE(tmp);
		}

		rid_filelist = g_ds_Update(hdb_filelist);
		if(rid_filelist != DB_RID_NULL){
			g_printf("OK add to playlist: %d", rid_filelist);
			g_ds_Goto(hdb_playlist, rid);
			g_ds_SetValue(hdb_playlist, PLAYLIST_DBFIELD_COUNT, g_ds_GetValue(hdb_playlist, PLAYLIST_DBFIELD_COUNT)+1);
			g_ds_Update(hdb_playlist);
		}else{
			g_printf("ERROR add to playlist");
		}
		g_object_unref_0(codec);
	}
	return rid_filelist;
}

int	db_add_dir_to_playlist(t_HDataSet hdb_filelist, char *path, t_HDataSet hdb_playlist, t_DBRecordID rid)
{
	int num = 0;
	char *fn = path+strlen(path)-1;
	char *f;
	void *odh;
	
	while(*fn == '/' || *fn == '\\'){
		*fn-- = 0;
	}
	fn++;
	odh = fsi_opendir(path);
	g_printf("fsi_opendir(%s):%s\r\n", path, odh?"OK":"ERR");
	if(odh){
		dentry_stat_t stat;
	
		*fn++ = '/';
		while(f = fsi_readdir(odh)){
			if(strcmp(f, ".") == 0) continue;
			if(strcmp(f, "..") == 0) continue;
			//if(name[0] == '.') continue;
			
			strcpy(fn, f);
			fsi_stat(path, &stat);
			g_printf("mode=%x", stat.mode);
	
			if(stat.mode & F_MODE_DIR)
				continue;
			if(db_add_file_to_playlist(hdb_filelist, path, hdb_playlist, rid) != DB_RID_NULL)
				num++;
		}
		fsi_closedir(odh);
		//listctrl_set_cursor(wnd_get_widget(wnd, RESID_LISTCTRL_FILELIST), glob->playlist_cursor, TRUE);
	}
	return num;
}

int	db_delete_playlist(t_HDataSet hdb_playlist, t_DBRecordID rid)
{
	int num = 0;
	if(g_ds_Goto(hdb_playlist, rid) == SUCCESS){
		g_ds_Delete(hdb_playlist);
		num ++;
	}
	return num;
}

int	db_remove_filelist(t_HDataSet hdb_filelist, t_DBRecordID rid)
{
	int num = 0;
	g_ds_CondSet1(hdb_filelist, FILELIST_DBFIELD_LIST, DBREL_EQ, rid);
	g_ds_Query(hdb_filelist, TRUE);
	g_ds_First(hdb_filelist);
	while(!g_ds_Empty(hdb_filelist)){
		g_ds_Delete(hdb_filelist);
		num++;
	}
	return num;
}

int	db_remove_invalid_files(t_HDataSet hdb_filelist)
{
	int num = 0;
	
	if(!g_ds_Empty(hdb_filelist)){
		char *fname;
		t_HMedia codec;
		
		g_ds_First(hdb_filelist);
		do{
			fname = g_ds_GetString(hdb_filelist, FILELIST_DBFIELD_PATH);
			codec = g_media_new_for_file(fname);
			if(g_object_valid(codec)){
				g_object_unref(codec);
				g_ds_Next(hdb_filelist);
			}else{
				num++;
				g_ds_Delete(hdb_filelist);
			}
		}while(!g_ds_Eof(hdb_filelist));
	}
	return num;
}

int	db_remove_file(t_HDataSet hdb, t_DBRecordID rid, t_HDataSet hdb_playlist)
{
	int num = 0;
	if(g_ds_Goto(hdb, rid) == SUCCESS){
		//2 首先更新所在playlist的数目
		t_DBRecordID playlist = g_ds_GetValue(hdb, FILELIST_DBFIELD_LIST);
		g_ds_Goto(hdb_playlist, playlist);
		g_ds_SetValue(hdb_playlist, PLAYLIST_DBFIELD_COUNT, g_ds_GetValue(hdb_playlist, PLAYLIST_DBFIELD_COUNT)-1);
		g_ds_Update(hdb_playlist);
		
		g_ds_Delete(hdb);
		num ++;

	}
	return num;
}

int	db_delete_file(t_HDataSet hdb, t_DBRecordID rid, t_HDataSet hdb_playlist)
{
	int num = 0;
	if(g_ds_Goto(hdb, rid) == SUCCESS){
		char *fname = g_ds_GetString(hdb, FILELIST_DBFIELD_PATH);
		fsi_unlink(fname);

		num = db_remove_file(hdb, rid, hdb_playlist);
	}
	return num;
}


#undef DB_AUDPLAYER_C

