#ifndef WGT_menu_H
#define WGT_menu_H


#include "ams.h"
#include "wgt_listctrl.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_MENU	"menu"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_menu)
__declspec(dllimport) extern t_GClass	 class_menu;
#else
extern t_GClass class_menu;
#endif

typedef t_HWidget	t_HMenu;

typedef const struct tagMenuItem t_MenuItem;
enum {
	MITEM_TYPE_NORMAL
	, MITEM_TYPE_SPLIT
	, MITEM_TYPE_SUBMENU
	
	, MITEM_TYPE_UNDEF = 0xFF
};

struct tagMenuItem{
	gu8		type;	// TRUE or FALSE
	MC_FILL_STRUCT3;
	resid_t 	imgId;	// optional
	resid_t	txtId;	// mandatory, and taken as ID
	listctrl_redraw_func_t hint_redraw;	//2 绘制菜单项的hint信息
	union {
		t_WidgetCallback	command;
		t_MenuItem *sub; // for submenu
	}u;
};
#define	u_command	u.command
#define	u_sub	u.sub

#define	MITEM_NORMAL(image, text, cmd)		{MITEM_TYPE_NORMAL, image, text, NULL, {cmd}}
#define	MITEM_HINT(image, text, hint, cmd)		{MITEM_TYPE_NORMAL, image, text, hint, {cmd}}
#define	MITEM_SUBMENU(image, text, submenu)	{MITEM_TYPE_SUBMENU, image, text, NULL, {submenu}}
#define	MITEM_SPLIT			{MITEM_TYPE_SPLIT, 0, 0, NULL, {NULL}}
#define	MITEM_END			{MITEM_TYPE_UNDEF, 0, 0, NULL, {NULL}}


//2 MENU_OnSelected first thrown to window, if not handled, then to menu itself
#define	MENU_OnSelected	WIDGET_OnSelected	// for window(sparam=index, lparam=menu), for menu(lparam=id)


BEGIN_DECLARATION

error_t	menu_bind(t_HMenu handle, t_MenuItem *items, void *data);
t_HMenu	menu_GetSubMenu(t_HMenu handle, int id, t_IdentType flags);
error_t	menu_EnableItem(t_HMenu handle, int id, t_IdentType flags, gboolean enabled);
gu32	menu_GetItemID(t_HMenu handle, int index);
error_t	menu_SetItemVisible(t_HMenu handle, int id, t_IdentType flags, gboolean visible);
error_t	menu_SetItemPopup(t_HMenu handle, int id, t_IdentType flags, t_HWidget popup);


END_DECLARATION


#endif /* WGT_menu_H */


