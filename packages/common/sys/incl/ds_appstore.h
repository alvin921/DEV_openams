#ifndef DS_APPSTORE_H
#define DS_APPSTORE_H


#include "list_datasource.h"

#include "ams.h"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_LISTCTRL)
__declspec(dllimport) extern t_GClass	 class_DSAppStore;
#else
extern t_GClass class_DSAppStore;
#endif

enum {
	gallery_text_none
	, gallery_text_pop
	, gallery_text_title
	, gallery_text_cell

	, gallery_text_show_max
};

enum {
	APPSTORE_ENTRY_NULL
	, APPSTORE_ENTRY_APP
	, APPSTORE_ENTRY_DIR
};

typedef struct {	// 8 bytes
	gu8		type;
	MC_FILL_STRUCT1;
	t_AmsModuleId	module;
	const char *guid;
}t_AppStoreApp;

typedef struct {	// 12 bytes
	gu8		type;
	gu8		level;	// 0 for root
	MC_FILL_STRUCT2;
	resid_t	icon;
	resid_t	text;
	g_array_t	*children;
}t_AppStoreDir;

typedef union{
	gu8		type;
	t_AppStoreApp	app;
	t_AppStoreDir	dir;
}t_AppStoreEntry;

typedef t_AppStoreEntry *t_AppStoreNode;

typedef void (*appstore_select_func_t)(t_HDataSource handle, const char *guid, t_AmsModuleId module, void *userdata);

BEGIN_DECLARATION


t_HDataSource	DSAppStore_New(appstore_select_func_t func, void *userdata);
t_AppStoreDir* DSAppStore_GetRootNode(t_HDataSource h);
t_AppStoreDir* DSAppStore_GetCWD(t_HDataSource h);
t_AppStoreNode DSAppStore_GetNode(t_HDataSource h, int index);
void		DSAppStore_SetIconSize(t_HDataSource h, int icon_width, int icon_height);
t_AppStoreDir * AppStore_AddDir(t_AppStoreDir * node, resid_t icon, resid_t text);
t_AppStoreApp* AppStore_AddApp(t_AppStoreDir *node, const char *guid, t_AmsModuleId module);


//2 return number of app added
int	AppStore_AddByMime(t_AppStoreDir *dir, const char *mime, const char *method);
int	AppStore_AddByMenuTree(t_AppStoreDir *dir, t_MenuTreeNode node);



void	listctrl_attach_mime_method(t_HWidget handle, const char *mime, const char *method, appstore_select_func_t func, void *userdata);

END_DECLARATION


#endif /* DS_APPSTORE_H */


