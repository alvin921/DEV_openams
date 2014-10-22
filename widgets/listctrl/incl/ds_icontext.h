#ifndef DS_ICONTEXT_H
#define DS_ICONTEXT_H

#include "list_datasource.h"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_LISTCTRL)
__declspec(dllimport) extern t_GClass	 class_DSIconText;
#else
extern t_GClass class_DSIconText;
#endif


typedef struct {
	g_tag_t	id;
	
	BrdHandle	brd;
	MC_FILL_STRUCT2;
	resid_t		resid;
	char *		text;
	
	t_HImage	image;
}t_IconTextItem;

#define IconTextItem_SetID(item, v)	do{if(item)(item)->id = v;}while(0)


BEGIN_DECLARATION

error_t	IconTextItem_SetText(t_IconTextItem *item, const char *text, BrdHandle brd, resid_t resid);
error_t	IconTextItem_SetImage(t_IconTextItem *item, t_HImage image, BrdHandle brd, resid_t resid);

g_idx_t	DSIconText_NewItem(t_HDataSource h);
error_t 	DSIconText_DeleteItem(t_HDataSource h, int id, t_IdentType flags);
error_t 	DSIconText_DeleteAll (t_HDataSource h);
error_t	DSIconText_SetItemText(t_HDataSource h, int id, t_IdentType flags, const char *text, BrdHandle brd, resid_t resid);
error_t	DSIconText_SetItemID(t_HDataSource h, int idx, gu32 tag);
gu32 	DSIconText_GetItemID(t_HDataSource h, int idx);
error_t	DSIconText_SetItemImage(t_HDataSource h, int id, t_IdentType flags, t_HImage image, BrdHandle brd, resid_t resid);
//2 设置item显示属性, 默认是IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE
void		DSIconText_SetAlign(t_HDataSource h, gu32 flags);
t_IconTextItem *	DSIconText_GetItem(t_HDataSource h, int id, t_IdentType flags);

END_DECLARATION

#endif	/* DS_ICONTEXT_H */

