	
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
	
#define DLL_EXPORT_menu

#include "wgt_menu.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/


#undef	PARENT_CLASS
#define	PARENT_CLASS		(&class_listctrl)

#define	MITEM_MARGIN	0
#define	MITEM_DEFAULT_HEIGHT	13

#define	MITEM_MAX		12

#define	MITEM_FLAG_ENABLED		BIT_1
#define	MITEM_FLAG_VISIBLE		BIT_2
typedef struct {
	t_DataSource	common;
	t_MenuItem*	items;
	gs8			count;
	gs8			real_count;
	MC_FILL_STRUCT2;
	gu8			real_array[MITEM_MAX];
	gu8			flags[MITEM_MAX];
	t_HMenu		submenu[MITEM_MAX];
	void *data;
}t_MenuSource;

static error_t		dsource_OnCreate(t_MenuSource* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	pme->items = NULL;
	pme->count = 0;
	return SUCCESS;
}

static void	dsource_OnDestroy(t_MenuSource* pme)
{
#if 0
	int index;
	for(index = 0; index < pme->count; index++){
		g_object_unref_0(pme->submenu[index]);
	}
#endif
}

static gboolean	dsource_Measure(t_HDataSource h, t_GuiViewType viewtype, int index, int *width, int *height)
{
	t_MenuSource *gdata = (t_MenuSource *)h;
	const char *text = brd_get_string(BRD_HANDLE_SELF, gdata->items[gdata->real_array[index]].txtId, NULL);
	t_HMedia media;
	t_MediaInfoImage *info;
	int iw, ih, tw, th;


	
	iw = ih = tw = th = 0;
	if(text){
		font_measure_string(text, -1, CHARSET_UNDEF, -1, &tw, &th, NULL);
	}
	media = brd_get_media(BRD_HANDLE_SELF, gdata->items[gdata->real_array[index]].imgId);
	if(g_object_valid(media)){
		info = (t_MediaInfoImage *)g_media_info(media);
		iw = info->width;
		ih = info->height;
		g_object_unref(media);
	}
	
	if(viewtype == GUI_VIEWTYPE_NORMAL){
		if(width)*width = tw+iw+2;
		if(height)*height = MAX(th, ih);
	}else if(viewtype == GUI_VIEWTYPE_ICON_ONLY || viewtype == GUI_VIEWTYPE_ICON_TEXT_TITLE){
		if(width)*width = iw;
		if(height)*height = ih;
	}else if(viewtype == GUI_VIEWTYPE_ICON_TEXT_TOP || viewtype == GUI_VIEWTYPE_ICON_TEXT_BOTTOM){
		if(width)*width = MAX(tw, iw);
		if(height)*height = th+ih+2;
	}else{
		if(width)*width = 0;
		if(height)*height = 0;
	}
	return TRUE;
}

static void	dsource_Redraw(t_HDataSource h, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight)
{
	t_MenuSource* pme = (t_MenuSource *)h;
	t_MenuItem *item;
	int __w = width;
	int __h = height;
	if(index < 0 || index >= pme->real_count || !g_object_valid(gdi))
		return;
	index = pme->real_array[index];
	if(index < 0 || index >= pme->count)
		return;
	item = &pme->items[index];
	if(item->type == MITEM_TYPE_SPLIT){
		//cairo_set_line_width(cr, 4.0);
		gdi_move_to(gdi, MITEM_MARGIN/2, height/2);
		gdi_line_to(gdi, width-MITEM_MARGIN/2, height/2);
		//cairo_set_line_width(cr, 2.0);
	}else{
		t_HImage icon = brd_get_image(BRD_HANDLE_SELF, item->imgId);
		const char *text = brd_get_string(BRD_HANDLE_SELF, item->txtId, NULL);
		int x=MITEM_MARGIN;

		width -= x;
		if(g_object_valid(icon)){
			int icon_w, icon_h;
			
			icon_w = image_get_width(icon);
			icon_h = image_get_height(icon);
			if(icon_h < height && icon_w <= height)
				gdi_draw_image_at(gdi, x, (height-icon_h)/2, icon, 0, 0, icon_w, icon_h, 1.0, 1.0);
			else
				gdi_draw_image_in(gdi, x, 0, height, height, icon, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE | IDF_HALIGN_STRETCH | IDF_VALIGN_STRETCH);
			x += height+1;
			width -= height+1;
		}
		if(text)
			gdi_draw_text_in(gdi, x, 0, width-MITEM_MARGIN, height, text, -1, CHARSET_UTF8, IDF_VALIGN_MIDDLE);
		if(item->type == MITEM_TYPE_SUBMENU){
			x += width - MITEM_MARGIN-MITEM_MARGIN;
			width = MITEM_MARGIN;
			gdi_move_to(gdi, x+1, (height-MITEM_MARGIN-2)/2);
			gdi_line_to(gdi, x+MITEM_MARGIN-1, height/2);
			gdi_line_to(gdi, x+1, height-(height-MITEM_MARGIN-2)/2);
			gdi_line_to(gdi, x+1, (height-MITEM_MARGIN-2)/2);
		}else if(item->hint_redraw){
			item->hint_redraw(gdi, viewtype, index, __w, __h, hilight, h->list);
		}
		g_object_unref(icon);
	}
}

static int	dsource_GetCount(t_HDataSource h)
{
	t_MenuSource* pme = (t_MenuSource*)h;
	return pme->real_count;
}

static gboolean	dsource_CanFocused(t_HDataSource h, int index)
{
	t_MenuSource* pme = (t_MenuSource*)h;
	gboolean ret = FALSE;
	if(index >= 0 && index < pme->real_count){
		index = pme->real_array[index];
		if(index >= 0 && index < pme->count){
			if(pme->items[index].type != MITEM_TYPE_SPLIT)
				ret = BIT_TEST(pme->flags[index], MITEM_FLAG_ENABLED)? TRUE : FALSE;
		}
	}
	return ret;
}

static VTBL(datasource) dsourceVtbl = {
	(PFNONCREATE)dsource_OnCreate
	, (PFNONDESTROY)dsource_OnDestroy

	, dsource_Measure
	, dsource_Redraw
	, dsource_GetCount
	, dsource_CanFocused
	, NULL
	, NULL
};

static CLASS_DECLARE(msource, t_MenuSource, dsourceVtbl);

static int	msource_find(t_MenuSource *pme, int id)
{
	int i;
	for(i = 0; i < pme->count; i++){
		if(pme->items[i].txtId == id)
			return i;
	}
	return -1;
}

static error_t	msource_bind(t_MenuSource* pme, t_HMenu handle, t_MenuItem *items, void *data)
{
	error_t ret = EBADPARM;
	if(g_object_valid(pme) && g_object_valid(handle) && items){
		int i;
		pme->items = items;
		for(i = 0; items->type != MITEM_TYPE_UNDEF && i < MITEM_MAX; i++, items++){
			if(items->type == MITEM_TYPE_SPLIT){
				BIT_CLEAR(pme->flags[i], MITEM_FLAG_ENABLED);
				pme->submenu[i] = NULL;
			}else if(items->type == MITEM_TYPE_NORMAL){
				BIT_SET(pme->flags[i], MITEM_FLAG_ENABLED);
				pme->submenu[i] = NULL;
			}else if(items->type == MITEM_TYPE_SUBMENU){
				BIT_SET(pme->flags[i], MITEM_FLAG_ENABLED);
				pme->submenu[i] = wgt_create(wgt_get_parent(handle), "menu");
				if(g_object_valid(pme->submenu[i])){
					wgt_disable_attr(pme->submenu[i], CTRL_ATTR_VISIBLE);
					menu_bind(pme->submenu[i], items->u_sub, data);
				}
			}
			BIT_SET(pme->flags[i], MITEM_FLAG_VISIBLE);
		}
		pme->count = pme->real_count = i;
		for(i = 0; i < pme->count; i++)
			pme->real_array[i] = i;
	}
	return ret;
}

/*===========================================================================

Class Definitions

===========================================================================*/

static error_t		__menu_OnCreate(t_CommonListCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	t_HDataSource ds;
	
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP | CTRL_ATTR_VSCROLLABLE/* | MENU_ATTR_NUMBERED*/);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	ds = (t_HDataSource)g_object_new0(&class_msource);
	listctrl_attach_datasource((t_HList)pme, ds);
	g_object_unref((t_HObject)ds);

	listctrl_set_flags((t_HList)pme, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM|CLFLAG_AUTO_RESIZE);
	listctrl_set_rows((t_HList)pme, 0, prod_ui_list_item_height(), 0);
	listctrl_set_item_margin((t_HList)pme, 8, 8, 4, 4);
	return SUCCESS;
}

static void	__menu_OnDestroy(t_CommonListCtrl* pme)
{
	g_object_inherit_destroy(pme);
}

static error_t __menu_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
	t_MenuSource *gdata = (t_MenuSource *)listctrl_get_datasource(handle);
	switch(evt){
		case WIDGET_OnSelected:
			{
				int index = listctrl_get_cursor(handle);
				t_MenuItem *item;
				
				if(index < 0 || index >= gdata->real_count)
					break;
				index = gdata->real_array[index];
				if(index < 0 || index >= gdata->count)
					break;
				item = &gdata->items[index];
				if(item->type == MITEM_TYPE_NORMAL){
					if(item->u_command){
						item->u_command(handle, gdata->data);
					}else{
						wgt_hook(handle, MENU_OnSelected, 0, item->txtId);
					}
					//2 allow auto hide
					wgt_disable_attr(handle, POPUP_ATTR_DISABLE_SELECT_HIDE);
				}else if(item->type == MITEM_TYPE_SUBMENU){
					if(g_object_valid(gdata->submenu[index])){
						coord_t x, y;
						gui_rect_t client;
						wgt_get_client_rect((t_HWidget)pme, &client);
						x = wgt_bound_dx(pme);
						y = index*pme->row_height;
						x += handle->client_origin_x + client.x;
						y += handle->client_origin_y + client.y;
						wgt_popup(handle, gdata->submenu[index], x, y, guiRight);
					}
					//2 not allowed to auto hide when selected
					wgt_enable_attr(handle, POPUP_ATTR_DISABLE_SELECT_HIDE);
				}
				ret = SUCCESS;
			}
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_PRESS){
				if((pme->row_n == 1 && lparam == MVK_UP) || (pme->row_n != 1 && lparam == MVK_LEFT)){
					wgt_hide(handle);
					ret = SUCCESS;
				}else if((pme->row_n == 1 && lparam == MVK_DOWN) || (pme->row_n != 1 && lparam == MVK_RIGHT)){
					int index = listctrl_get_cursor(handle);
					if(index < 0 || index >= gdata->real_count)
						break;
					index = gdata->real_array[index];
					if(index < 0 || index >= gdata->count)
						break;
					if(gdata->items[index].type == MITEM_TYPE_SUBMENU){
						if(g_object_valid(gdata->submenu[index])){
							coord_t x, y;
							gui_rect_t client;
							wgt_get_client_rect((t_HWidget)pme, &client);
							x = wgt_bound_dx(pme);
							y = index*pme->row_height;
							x += handle->client_origin_x + client.x;
							y += handle->client_origin_y + client.y;
							if(lparam == MVK_RIGHT)
								wgt_popup(handle, gdata->submenu[index], x, y, guiRight);
							else
								wgt_popup(handle, gdata->submenu[index], x, y+pme->row_height, guiBottom);
						}
					}
					ret = SUCCESS;
				}
			}
			break;
	}
	if(ret == NOT_HANDLED){
		ret = g_widget_inherit_OnEvent(handle, evt, sparam, lparam);
	}
	return ret;
}


static VTBL(widget) menuVtbl = {
	(PFNONCREATE)__menu_OnCreate
	, (PFNONDESTROY)__menu_OnDestroy

	, __menu_OnEvent
	, NULL
};

CLASS_DECLARE(menu, t_CommonListCtrl, menuVtbl);


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_menu;
}

#endif

#undef PARENT_CLASS

// TODO: add functions for your own widget.  You MUST export them in dll.def


error_t	menu_bind(t_HMenu handle, t_MenuItem *items, void *data)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle) && items){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		msource_bind((t_MenuSource*)pme->u.s_custom.hds, handle, items, data);
		((t_MenuSource*)pme->u.s_custom.hds)->data = data;
		wgt_enable_attr(pme, CTRL_ATTR_MODIFIED);
		ret = SUCCESS;
	}
	return ret;
}

#define IF_DEFAULT_DSOURCE(h)	(g_object_valid(h) && g_object_valid(((t_CommonListCtrl *)(h))->u.s_custom.hds) \
								&& g_object_class(((t_CommonListCtrl *)(h))->u.s_custom.hds) == &class_msource)

error_t	menu_EnableItem(t_HMenu handle, int id, t_IdentType flags, gboolean enabled)
{
	error_t ret = EBADPARM;
	if(IF_DEFAULT_DSOURCE(handle)){
		t_MenuSource* ds = (t_MenuSource*)((t_CommonListCtrl *)(handle))->u.s_custom.hds;
		if(flags & IDENT_BY_ID){
			id = msource_find(ds, id);
			flags |= IDENT_BY_POSITION;
		}
		if((flags & IDENT_BY_POSITION) && id >= 0 && id < ds->count){
			if(enabled){
				BIT_SET(ds->flags[id], MITEM_FLAG_ENABLED);
				//2 如果控件为disabled，则任一条目enalbed，则控件须enabled
				if(!wgt_is_enabled(handle))
					wgt_enable_attr(handle, CTRL_ATTR_ENABLED);
			}else{
				BIT_CLEAR(ds->flags[id], MITEM_FLAG_ENABLED);
				//2 条目变为Disabled，如果恰好是焦点项，是否要做处理?
				//2 如果所有条目都变disabled，则整个控件必须disabled
				for(id = 0; id < ds->real_count; id++){
					if(BIT_TEST(ds->flags[ds->real_array[id]], MITEM_FLAG_ENABLED))
						break;
				}
				if(id == ds->real_count)
					wgt_disable_attr(handle, CTRL_ATTR_ENABLED);
			}
			ret = SUCCESS;
		}
	}
	return ret;
}

error_t	menu_SetItemVisible(t_HMenu handle, int id, t_IdentType flags, gboolean visible)
{
	error_t ret = EBADPARM;
	if(IF_DEFAULT_DSOURCE(handle)){
		t_MenuSource* ds = (t_MenuSource*)((t_CommonListCtrl *)(handle))->u.s_custom.hds;
		gu8 mask;
		gs8 i;
		if(flags & IDENT_BY_ID){
			id = msource_find(ds, id);
			flags |= IDENT_BY_POSITION;
		}
		if((flags & IDENT_BY_POSITION) && id >= 0 && id < ds->count){
			if(BIT_TEST(ds->flags[id], MITEM_FLAG_VISIBLE) != visible){
				if(visible){
					BIT_SET(ds->flags[id], MITEM_FLAG_VISIBLE);
					g_binsert_u8(ds->real_array, 0, ds->real_count-1, id);
					ds->real_count++;
				}else{
					BIT_CLEAR(ds->flags[id], MITEM_FLAG_VISIBLE);
					i = g_bsearch_u8(ds->real_array, 0, ds->real_count-1, id);
					if(i >= 0){
						for(; i < ds->real_count-1; i++)
							ds->real_array[i] = ds->real_array[i+1];
						ds->real_count--;
					}
				}
				wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
			}
			ret = SUCCESS;
		}
	}
	return ret;
}

gu32	menu_GetItemID(t_HMenu handle, int index)
{
	gu32 id = 0;
	if(IF_DEFAULT_DSOURCE(handle)){
		t_MenuSource* ds = (t_MenuSource*)((t_CommonListCtrl *)(handle))->u.s_custom.hds;
		if(index >= 0 && index < ds->real_count){
			index = ds->real_array[index];
			if(index >= 0 && index < ds->count)
				id = ds->items[index].txtId;
		}
	}
	return id;
}

t_HMenu	menu_GetSubMenu(t_HMenu handle, int id, t_IdentType flags)
{
	t_HMenu sub = NULL;
	if(IF_DEFAULT_DSOURCE(handle)){
		t_MenuSource* ds = (t_MenuSource*)((t_CommonListCtrl *)(handle))->u.s_custom.hds;
		if(flags & IDENT_BY_ID){
			int i;
			for(i = 0; i < ds->count; i++){
				if(ds->items[i].txtId == id && ds->items[i].type == MITEM_TYPE_SUBMENU){
					id = i;
					flags |= IDENT_BY_POSITION;
					break;
				}
			}
		}
		if((flags & IDENT_BY_POSITION) && id >= 0 && id < ds->count && ds->items[id].type == MITEM_TYPE_SUBMENU){
			sub = ds->submenu[id];
		}
	}
	return sub;
}

error_t	menu_SetItemPopup(t_HMenu handle, int id, t_IdentType flags, t_HWidget popup)
{
	error_t ret = EBADPARM;
	if(IF_DEFAULT_DSOURCE(handle)){
		t_HMenu sub = NULL;
		t_MenuSource* ds = (t_MenuSource*)((t_CommonListCtrl *)(handle))->u.s_custom.hds;
		if(flags & IDENT_BY_ID){
			int i;
			BIT_CLEAR(flags, IDENT_BY_POSITION);
			for(i = 0; i < ds->count; i++){
				if(ds->items[i].txtId == id && ds->items[i].type == MITEM_TYPE_SUBMENU){
					id = i;
					BIT_SET(flags, IDENT_BY_POSITION);;
					break;
				}
			}
		}
		if((flags & IDENT_BY_POSITION) && id >= 0 && id < ds->count && ds->items[id].type == MITEM_TYPE_SUBMENU){
			#if 0
			g_object_unref_0(ds->submenu[id]);
			if(g_object_valid(popup))
				ds->submenu[id] = g_object_ref(popup);
			#else
			ds->submenu[id] = (popup);
			#endif
			ret = SUCCESS;
		}
	}
	return ret;
}

#undef PARENT_CLASS


