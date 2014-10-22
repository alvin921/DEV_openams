/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  OEMToolbar.c

SERVICES:  OEM Toolbar Control Interface

GENERAL DESCRIPTION:  Provides support for IToolbar services
in OEM.

PUBLIC CLASSES AND STATIC FUNCTIONS: IToolbar

        Copyright ?2005 MAS Incorporated.
               All Rights Reserved.

=====================================================*/
	
/*=============================================================================
	
$DateTime: 2004/11/06 16:41:00 $
$Author: Wang MinGang $

					  EDIT HISTORY FOR FILE
	
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when		 who	 what, where, why
----------	 ---	 ----------------------------------------------------------
=============================================================================*/



/*=========================================================================
List控件特性描述:
1) list view: 主体是text
	1.1) 每行一个item(icon+text或者单text), 每行等高
	1.2) 可以添加属性如check/radio, 分别对应多选/单选
	1.3) hilight状态, item icon如果是动画, 则运行动画, 否则显示静态图片
	1.4) 暂时不支持: 允许设置hilight item占用两行, 这样text就有两个, detailTxt只在hilight显示

	高级特性:适用于数据比较多, 一次性加载浪费内存切加载慢, 比如数据库
	1.4) 允许用户设置数据源和显示方式, 提供最大限度灵活度, 将其实现为DBList
	
2) icon view: 主体是icon
	2.1) 每个item包括icon+text
	2.2) 每个item显示区域为正方形, cell方式显示
	2.3) 如果item icon为动画, 则hilight时显示动画

===========================================================================*/



/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
	

#include "wgt_listctrl.h"

static void	__listitem_Free(t_IconTextItem *item)
{	
	if(item){
		FREEIF(item->text);
		g_object_unref(item->image);
	}
}

typedef struct {
	t_DataSource	common;
	g_array_t		*items;	// t_IconTextItem
	gu32		flags;
}t_DSIconText;


#define	_GetItem(h, idx)		(DS_VALID(h)?g_array_element(((t_DSIconText*)(h))->items, idx) : NULL)

static error_t		DS_OnCreate(t_DSIconText* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4);
static void	DS_OnDestroy(t_DSIconText* pme);
#define ICON_TEXT_SPLIT_WIDTH	2
#define	ICON_TEXT_MARGIN_WIDTH	2
static gboolean	DS_Measure(t_HDataSource h, t_GuiViewType viewtype, int index, int *width, int *height);
static void	DS_Redraw(t_HDataSource h, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight);
static int	DS_GetCount(t_HDataSource h);

static VTBL(datasource) ddsVtbl = {
	(PFNONCREATE)DS_OnCreate
	, (PFNONDESTROY)DS_OnDestroy

	, DS_Measure
	, DS_Redraw
	, DS_GetCount
	, NULL		// CanFocused
	, NULL		// HookEvent
	, NULL		// refresh
};


CLASS_DECLARE(DSIconText, t_DSIconText,  ddsVtbl);

#define	DS_VALID(ds)	(g_object_valid(ds) && g_object_class(ds) == &class_DSIconText)

static error_t		DS_OnCreate(t_DSIconText* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	pme->items = g_array_new(sizeof(t_IconTextItem), 4, ARRAY_TYPE_DYN_ALIGN, 4);
	pme->flags = IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE;
	return SUCCESS;
}

static void	DS_OnDestroy(t_DSIconText* pme)
{
	g_array_destroy(pme->items, (GFreeFunc)__listitem_Free);
}

static gboolean	DS_Measure(t_HDataSource h, t_GuiViewType viewtype, int index, int *width, int *height)
{
	t_IconTextItem *item = _GetItem(h, index);
	if(item){
		t_HImage icon = item->image;
		const char *text = brd_get_string(item->brd, item->resid, (const char *)item->text);
		int iw, ih, tw, th;

		iw = ih = tw = th = 0;
		if(text){
			font_measure_string(text, -1, CHARSET_UNDEF, -1, &tw, &th, NULL);
		}
		if(g_object_valid(item->image)){
			iw = image_get_width(item->image);
			ih = image_get_height(item->image);
		}
		
		if(viewtype == GUI_VIEWTYPE_NORMAL){
			if(width)*width = tw+iw+ICON_TEXT_SPLIT_WIDTH;
			if(height)*height = MAX(th, ih);
		}else if(viewtype == GUI_VIEWTYPE_ICON_ONLY || viewtype == GUI_VIEWTYPE_ICON_TEXT_TITLE){
			if(width)*width = iw;
			if(height)*height = ih;
		}else if(viewtype == GUI_VIEWTYPE_ICON_TEXT_TOP || viewtype == GUI_VIEWTYPE_ICON_TEXT_BOTTOM){
			if(width)*width = MAX(tw, iw);
			if(height)*height = th+ih+ICON_TEXT_SPLIT_WIDTH;
		}else{
			if(width)*width = 0;
			if(height)*height = 0;
		}
	}
	return TRUE;
}

static void	DS_Redraw(t_HDataSource h, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight)
{
	t_IconTextItem *item = _GetItem(h, index);
	if(item){
		t_HImage icon = item->image;
		const char *text = brd_get_string(item->brd, item->resid, (const char *)item->text);

		if(viewtype == GUI_VIEWTYPE_NORMAL){
			gdi_draw_icon_text(gdi, icon, text, width, height, guiRight, ((t_DSIconText*)h)->flags);
		}else if(viewtype == GUI_VIEWTYPE_ICON_ONLY || viewtype == GUI_VIEWTYPE_ICON_TEXT_TITLE){
			gdi_draw_image_in(gdi, 0, 0, height, height, icon, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
		}else if(viewtype == GUI_VIEWTYPE_ICON_TEXT_TOP){
			gdi_draw_icon_text(gdi, icon, text, width, height, guiTop, ((t_DSIconText*)h)->flags);
		}else if(viewtype == GUI_VIEWTYPE_ICON_TEXT_BOTTOM){
			gdi_draw_icon_text(gdi, icon, text, width, height, guiBottom, ((t_DSIconText*)h)->flags);
		}else{
		}
	}
}

static int	DS_GetCount(t_HDataSource h)
{
	t_DSIconText* pme = (t_DSIconText*)h;
	return g_array_size(pme->items);
}


g_idx_t		DSIconText_NewItem(t_HDataSource h)
{
	g_idx_t idx = IDX_UNDEF;
	if(DS_VALID(h)){
		t_DSIconText* ds = (t_DSIconText*)h;
		
		idx = g_array_append(ds->items, NULL);
		if(g_object_valid(DS_LISTCTRL(h)))
			wgt_enable_attr(DS_LISTCTRL(h), CTRL_ATTR_MODIFIED);
	}
	return idx;
}
static int	DSIconText_Find(t_DSIconText *pme, int id)
{
	int i;
	t_IconTextItem *item;
	for(i = 0; i < g_array_size(pme->items); i++){
		item = g_array_element(pme->items, i);
		if(item->id == id)
			return i;
	}
	return -1;
}

error_t 	DSIconText_DeleteItem(t_HDataSource h, int id, t_IdentType flags)
{
	error_t ret = EBADPARM;
	if(DS_VALID(h)){
		t_DSIconText* ds = (t_DSIconText*)h;
		
		if(flags & IDENT_BY_ID)
			id = DSIconText_Find(ds, id);
		if(id >= 0 && id < g_array_size(ds->items)){
			g_array_del(ds->items, id, (GFreeFunc)__listitem_Free);
			if(g_object_valid(DS_LISTCTRL(h))){
				if(((t_CommonListCtrl *)(DS_LISTCTRL(h)))->cursor >= g_array_size(ds->items))
					((t_CommonListCtrl *)(DS_LISTCTRL(h)))->cursor = g_array_size(ds->items)-1;
				wgt_enable_attr(DS_LISTCTRL(h), CTRL_ATTR_MODIFIED);
			}
			ret = SUCCESS;
		}
	}
	return ret;
}

error_t 	DSIconText_DeleteAll (t_HDataSource h)
{
	error_t ret = EBADPARM;
	if(DS_VALID(h)){
		t_DSIconText* ds = (t_DSIconText*)h;
		
		g_array_reset(ds->items, (GFreeFunc)__listitem_Free);
		if(g_object_valid(DS_LISTCTRL(h))){
			((t_CommonListCtrl *)(DS_LISTCTRL(h)))->cursor = IDX_UNDEF;
			wgt_enable_attr(DS_LISTCTRL(h), CTRL_ATTR_MODIFIED);
		}
		ret = SUCCESS;
	}
	return ret;
}

t_IconTextItem *	DSIconText_GetItem(t_HDataSource h, int id, t_IdentType flags)
{
	t_IconTextItem *item = NULL;
	if(DS_VALID(h)){
		t_DSIconText* ds = (t_DSIconText*)h;
		if(flags & IDENT_BY_ID)
			id = DSIconText_Find(ds, id);
		item = _GetItem(ds, id);
	}
	return item;
}

error_t	DSIconText_SetItemText(t_HDataSource h, int id, t_IdentType flags, const char *text, BrdHandle brd, resid_t resid)
{
	error_t ret = EBADPARM;
	if(DS_VALID(h)){
		t_DSIconText* ds = (t_DSIconText*)h;
		if(flags & IDENT_BY_ID)
			id = DSIconText_Find(ds, id);
		ret = IconTextItem_SetText(g_array_element(ds->items, id), text, brd, resid);
	}
	return ret;
}

error_t	DSIconText_SetItemID(t_HDataSource h, int idx, gu32 tag)
{
	error_t ret = EBADPARM;
	t_IconTextItem *item = _GetItem(h, idx);
	if(item){
		item->id = tag;
		ret = SUCCESS;
	}
	return ret;
}

gu32 	DSIconText_GetItemID(t_HDataSource h, int idx)
{
	t_IconTextItem *item = _GetItem(h, idx);
	return (item)? item->id : -1;
}

error_t	DSIconText_SetItemImage(t_HDataSource h, int id, t_IdentType flags, t_HImage image, BrdHandle brd, resid_t resid)
{
	error_t ret = EBADPARM;
	if(DS_VALID(h)){
		t_DSIconText* ds = (t_DSIconText*)h;
		if(flags & IDENT_BY_ID)
			id = DSIconText_Find(ds, id);
		ret = IconTextItem_SetImage(g_array_element(ds->items, id), image, brd, resid);
	}
	return ret;
}

void		DSIconText_SetAlign(t_HDataSource h, gu32 flags)
{
	if(DS_VALID(h)){
		t_DSIconText* ds = (t_DSIconText*)h;
		ds->flags = flags;
	}
}
error_t	IconTextItem_SetText(t_IconTextItem *item, const char *text, BrdHandle brd, resid_t resid)
{
	error_t ret = EBADPARM;
	if(item){
		FREEIF(item->text);
		item->brd = brd;
		item->resid = resid;
		item->text = g_str_dup(text);
		ret = SUCCESS;
	}
	return ret;
}

error_t	IconTextItem_SetImage(t_IconTextItem *item, t_HImage image, BrdHandle brd, resid_t resid)
{
	error_t ret = EBADPARM;
	if(item){
		g_object_unref_0(item->image);
		if(g_object_valid(image)){
			item->image = g_object_ref(image);
		}else{
			item->image = brd_get_image(brd, resid);
		}
		ret = SUCCESS;
	}
	return ret;
}

