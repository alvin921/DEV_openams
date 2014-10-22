#ifndef DB_PLAYLIST_H
#define DB_PLAYLIST_H

#include "dbase.h"

#define	MPLAYER_RECENT_FNAME		"c:/mplayer.dat"

#define	MPLAYER_RECENT_MAX	20
typedef struct {
	int		playing_playlist_cursor;
	int		playing_filelist_cursor;
	gu32	time;	//2 上次播放时间，ms为单位
	
	gu8		repeat;
	gu8		shuffle;
	gu8		modified;
	gu8		count;
	t_DBRecordID		recids[MPLAYER_RECENT_MAX];
}t_MPlayerSaved;

#define	AUDPLAYER_DB_FNAME		"c:/audplayer.adb"
#define	AP_DBTAB_PLAYLIST			"playlist"
#define	AP_DBTAB_FILELIST			"filelist"

#define	AP_PLAYLIST_NUM_MAX		20
#define	AP_PLAYLIST_NAME_MAX		31

#define	PLAYLIST_DBFIELD_NAME	"name"
#define	PLAYLIST_DBFIELD_COUNT	"count"

#define	AP_FILELIST_NUM_MAX		500

#define	FILELIST_DBFIELD_LIST		"list"
#define	FILELIST_DBFIELD_PATH		"path"
#define	FILELIST_DBFIELD_TYPE		"type"
#define	FILELIST_DBFIELD_FAVORATE		"favorate"
#define	FILELIST_DBFIELD_TITLE		"title"
#define	FILELIST_DBFIELD_DURATION	"duration"


#if defined(DB_AUDPLAYER_C)

static const t_DBFieldInfo		playlist_db_fields[] = {
	{PLAYLIST_DBFIELD_NAME		, DBFIELD_TYPE_STRING	, FALSE	, AP_PLAYLIST_NAME_MAX+1},
	//2 如下信息暂存以加速访问
	{PLAYLIST_DBFIELD_COUNT		, DBFIELD_TYPE_WORD 	, FALSE	, 0},	// count
};

static const t_DBFieldInfo		filelist_db_fields[] = {
	{FILELIST_DBFIELD_LIST		, DBFIELD_TYPE_WORD		, FALSE	, 0},	// playlist记录号
	{FILELIST_DBFIELD_PATH		, DBFIELD_TYPE_STRING	, FALSE	, 0},	// full path name
	//2 如下信息暂存以加速访问
	{FILELIST_DBFIELD_TYPE		, DBFIELD_TYPE_BYTE		, FALSE	, 0},	// audio type: used to choose codec
	{FILELIST_DBFIELD_FAVORATE	, DBFIELD_TYPE_BYTE 		, FALSE , 0},	// favorate
	{FILELIST_DBFIELD_TITLE		, DBFIELD_TYPE_STRING	, FALSE	, AUD_TITLE_MAX+1},	// name
	{FILELIST_DBFIELD_DURATION	, DBFIELD_TYPE_DWORD	, FALSE	, 0},	// duration: MS
};

#endif

BEGIN_DECLARATION



gboolean	mplayer_recent_read(t_MPlayerSaved *pRecent);
void		mplayer_recent_write(t_MPlayerSaved *pRecent);
//2 如果recent数据有更新则返回TRUE
gboolean	mplayer_recent_add(t_MPlayerSaved *pRecent, t_DBRecordID fid);
//2 如果recent数据有更新则返回TRUE
gboolean	mplayer_recent_remove(t_MPlayerSaved *pRecent, t_DBRecordID fid);
//2 如果recent数据有更新则返回TRUE
gboolean	mplayer_recent_remove_idx(t_MPlayerSaved *pRecent, int idx);


void		db_audplayer_init(void);
t_DBRecordID	db_add_file_to_playlist(t_HDataSet hdb_filelist, char *fname, t_HDataSet hdb_playlist, t_DBRecordID rid);
int	db_add_dir_to_playlist(t_HDataSet hdb_filelist, char *path, t_HDataSet hdb_playlist, t_DBRecordID rid);
int	db_remove_filelist(t_HDataSet hdb_filelist, t_DBRecordID rid);
int	db_remove_invalid_files(t_HDataSet hdb_filelist);
int	db_remove_file(t_HDataSet hdb, t_DBRecordID rid, t_HDataSet hdb_playlist);
int	db_delete_file(t_HDataSet hdb, t_DBRecordID rid, t_HDataSet hdb_playlist);


END_DECLARATION

#endif
