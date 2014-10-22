	
/*=============================================================================
	
$DateTime: 2010/06/06 16:41:00 $
$Author: Wang MinGang $

					  EDIT HISTORY FOR FILE
	
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when		 who	 what, where, why
----------	 ---	 ----------------------------------------------------------
=============================================================================*/





/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
	

#include "ams.h"
#include "wgt_listctrl.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/




#define APPSTORE_CONTEXT_MAX	10

typedef struct {
	const char *guid;
	t_AmsModuleId	module;
	gu8		count;
	MC_FILL_STRUCT1;
	t_TodayMSInterface	context;
}t_DSTodayItem;

typedef struct {
	t_DataSource	common;

	t_DSTodayItem *items;
	gs16		num;
}t_DSToday;

static void	DS_OnDestroy(t_DSToday* pme)
{
	if(pme->items){
		int index;
		for(index = 0; index < pme->num; index++){
			if(pme->items[index].context && pme->items[index].context->Destroy)
				pme->items[index].context->Destroy(pme->items[index].context);
		}
		FREE(pme->items);
	}
}

static void	DS_Refresh(t_HDataSource h)
{
	t_DSToday *pme = (t_DSToday*)h;
	t_MenuTreeNode node;
	int index;
	
	DS_OnDestroy(pme);

	node = prod_today_open();
	pme->num = prod_get_menu_count(node);
	if(pme->num){
		pme->items = MALLOC(pme->num * sizeof(t_DSTodayItem));
		memset(pme->items, 0, pme->num * sizeof(t_DSTodayItem));
		for(index = 0; index < pme->num; index++){
			t_MenuTreeNode child;
			t_MenuTreeNodeAttr attr;
			
			child = prod_get_menu_node(node, index);
			if(prod_get_menu_node_attr(child, &attr) == SUCCESS && attr.type == MTREE_NODE_APP){
				pme->items[index].guid = attr.name;
				pme->items[index].module = attr.module;
				pme->items[index].count = 1;
				mime_service_exec_func(MIME_TYPE_TODAY, METHOD_NAME_QUERY_INTERFACE, attr.name, &pme->items[index].context, sizeof(t_TodayMSInterface));
				if(pme->items[index].context && pme->items[index].context->GetItemCount)
					pme->items[index].count += pme->items[index].context->GetItemCount(pme->items[index].context);
			}
		}
	}
}

static error_t		DS_OnCreate(t_DSToday* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	return SUCCESS;
}

static	gboolean	__DS_GetItemIndex(t_DSToday *pme, int index, int *pi, int *po)
{
	int i;
	for(i = 0; i < pme->num; i++){
		if(index < pme->items[i].count){
			if(pi)*pi = i;
			if(po)*po = index;
			return TRUE;
		}
		index -= pme->items[i].count;
	}
	return FALSE;
}

static void	DS_Redraw(t_HDataSource h, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight)
{
	t_DSToday* pme = (t_DSToday *)h;
	int item_idx, item_off;
	t_DSTodayItem *item;
	t_HImage image = NULL;
	char title[GUID_NAME_MAX+1];
	int x;
 
	if(!__DS_GetItemIndex(pme, index, &item_idx, &item_off))
		return;
	item = &pme->items[item_idx];
	if(item->guid == NULL)
		return;
	image = pack_GetIcon(item->guid);
	pack_GetModuleTitle(item->guid, item->module, title, sizeof(title));

	x = 2;
	width -= x;
	//strcpy(title, "<N/A>");
	if(item_off == 0){
		if(g_object_valid(image)){
			gdi_draw_image_in(gdi, x, 0, height, height, image, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
		}
		x += height+2;
		width -= height+2;
		gdi_draw_text_in(gdi, x, 0, width, height, title, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
		if(item->context && item->context->GetDesc && item->context->GetDesc(item->context, title, sizeof(title))){
			gdi_draw_text(gdi, " - ", 3, CHARSET_UTF8);
			gdi_draw_text(gdi, title, -1, CHARSET_UTF8);
		}
	}else{ // sub items
		x += height+2;
		width -= height+2;
		if(item->context && item->context->GetItemDesc && item->context->GetItemDesc(item->context, item_off-1, title, sizeof(title))){
			gdi_draw_text_in(gdi, x, 0, width, height, title, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
		}
	}
	g_object_unref(image);
}

static int	DS_GetCount(t_HDataSource h)
{
	int index, count;
	t_DSToday *pme = (t_DSToday *)h;
	count = 0;
	for(index = 0; index < pme->num; index++){
		count += pme->items[index].count;
	}
	return count;
}

static error_t DS_on_event(t_HDataSource h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	t_DSToday* pme = (t_DSToday *)h;
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnSelected:
			{
				int item_idx, item_off;
				t_DSTodayItem *item;
				
				if(!__DS_GetItemIndex(pme, sparam, &item_idx, &item_off))
					return;
				item = &pme->items[item_idx];
				if(item->guid == NULL)
					return;
				if(item_off == 0){
					if(item->context && item->context->OnSelect)
						item->context->OnSelect(item->context);
					else
						pack_call(item->guid, item->module, NULL, 0, FALSE, NULL);
				}else{
					if(item->context && item->context->OnItemSelect)
						item->context->OnItemSelect(item->context, item_off-1);
				}
				ret = SUCCESS;
			}
			break;
			
		case WIDGET_OnKeyEvent:
#if !defined(FTR_NO_INPUT_KEY)
#endif
			break;

#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
		{
			coord_t x0, y0;
			PEN_XY(lparam, x0, y0);
			switch(PEN_TYPE(lparam)){
				case MMI_PEN_LONG_PRESS:
					//2 PenLongPress就表示没有PenMove
					{
						int row, col;
						int index = listctrl_index_by_point(h->list, &x0, &y0, &row, &col);
					}
					break;
				case MMI_PEN_DOWN:
					if(0){
						int row, col;
						int index = listctrl_index_by_point(h->list, &x0, &y0, &row, &col);
					}
					break;
			}
			break;
		}
#endif
	}
	return ret;
}


static VTBL(datasource) dsourceVtbl = {
	(PFNONCREATE)DS_OnCreate
	, (PFNONDESTROY)DS_OnDestroy

	, NULL		// Measure
	, DS_Redraw
	, DS_GetCount
	, NULL		// CanFocused
	, DS_on_event	// HookEvent
	, DS_Refresh
};

CLASS_DECLARE(DSToday, t_DSToday, dsourceVtbl);

t_HDataSource	DSToday_Create(void)
{
	t_HDataSource ds = NULL;
	
	ds = (t_HDataSource)g_object_new0(&class_DSToday);

	return ds;
}

