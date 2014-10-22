	
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
#include "ds_appstore.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/


#define	CELL_WIDTH_DEFAULT		40
#define	CELL_HEIGHT_DEFAULT		40

typedef struct {
	t_AppStoreDir	*node;
	coord_t		origin_x;
	coord_t		origin_y;
	int			cursor;
}t_AppStoreContext;

#define APPSTORE_CONTEXT_MAX	10
typedef struct {
	t_DataSource	common;
	
	t_AppStoreDir	*root;
	t_AppStoreDir *entry;

	t_AppStoreContext	stack[APPSTORE_CONTEXT_MAX];
	t_MMCHandle	mmc_anim;
	gs16		num;
	
	MC_FILL_STRUCT2;
	
	gu16		icon_w;	// default is 0, auto resize
	gu16		icon_h;	//  default is 0, auto resize

	appstore_select_func_t	func;
	void *userdata;
	void	*arg;
	gu32	len;
}t_DSAppStore;


static void	__DSAppStore_push(t_HDataSource h, t_AppStoreDir *entry)
{
	t_DSAppStore* gdata = (t_DSAppStore*)h;
	//2 save current context
	if(g_object_valid(h->list)){
		gdata->stack[gdata->num].cursor = listctrl_get_cursor(h->list); 
		gdata->stack[gdata->num].origin_x = wgt_client_origin_x(h->list);
		gdata->stack[gdata->num].origin_y = wgt_client_origin_y(h->list);
		gdata->stack[gdata->num].node = gdata->entry;
		gdata->num++;
	}else{
		gdata->num = 0;
	}
	//2 reset widget
	gdata->entry = entry;
	if(g_object_valid(h->list)){
		wgt_client_origin_x(h->list) = 0;
		wgt_client_origin_y(h->list) = 0;
		listctrl_refresh_datasource(h->list, FALSE);
		listctrl_set_cursor(h->list, 0, FALSE);
	}
}

static gboolean	__DSAppStore_pop(t_HDataSource h)
{
	t_DSAppStore *gdata = (t_DSAppStore *)h;
	if(gdata->num > 0){
		gdata->num--;
		wgt_client_origin_x(h->list) = gdata->stack[gdata->num].origin_x;
		wgt_client_origin_y(h->list) = gdata->stack[gdata->num].origin_y;
		gdata->entry = gdata->stack[gdata->num].node;
		
		listctrl_refresh_datasource(h->list, FALSE);
		//2 SetCursor MUST be following REFRESH, thus list item COUNT can be refreshed
		listctrl_set_cursor(h->list, gdata->stack[gdata->num].cursor, FALSE);
		return TRUE;
	}
	return FALSE;
}


static void	__AppStoreDir_Free(t_AppStoreDir *dir)
{
	if(dir && dir->type == APPSTORE_ENTRY_DIR){
		g_array_destroy(dir->children, (GFreeFunc)__AppStoreDir_Free);
	}
}

static error_t		dsource_OnCreate(t_DSAppStore* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	pme->root = AppStore_AddDir(NULL, RESID_UNDEF, RESID_UNDEF);
	pme->entry = pme->root;
	pme->num = 0;
	pme->mmc_anim = mmc_open();
	return SUCCESS;
}

static void	dsource_OnDestroy(t_DSAppStore* pme)
{
	if(pme->root){
		__AppStoreDir_Free(pme->root);
		FREE(pme->root);
	}
	mmc_stop(pme->mmc_anim);
	mmc_close(pme->mmc_anim);
}

#define _GetEntry(dir, i)	g_array_element(((t_AppStoreDir*)(dir))->children, i)

#define	ICON_TEXT_SPLIT	2

static gboolean	dsource_Measure(t_HDataSource h, t_GuiViewType viewtype, int index, int *width, int *height)
{
	t_DSAppStore* pme = (t_DSAppStore *)h;
	t_AppStoreDir *entry = pme->entry;
	t_AppStoreEntry* child;
	t_AppStoreDir *dir;
	t_AppStoreApp *app;
	BrdHandle brd;
	char title[GUID_NAME_MAX+1];
	t_HImage image = NULL;
	int iw, ih, tw, th;
	
	child = _GetEntry(entry, index);
	if(!child)
		return FALSE;
	strcpy(title, "<N/A>");
	if(child->type == APPSTORE_ENTRY_APP){
		app = (t_AppStoreApp *)child;
		image = pack_GetIcon(app->guid);
		pack_GetModuleTitle(app->guid, app->module, title, sizeof(title));
	}else{
		dir = (t_AppStoreDir *)child;
		brd = brd_get("sys");
		image = brd_get_image(brd, dir->icon);
		brd_get_string2(brd, dir->text, title, sizeof(title));
	}

	iw = ih = tw = th = 0;
	font_measure_string(title, -1, CHARSET_UNDEF, -1, &tw, &th, NULL);
	if(g_object_valid(image)){
		iw = image_get_width(image);
		ih = image_get_height(image);
	}
	
	if(viewtype == GUI_VIEWTYPE_NORMAL){
		if(width)*width = tw+iw+ICON_TEXT_SPLIT;
		if(height)*height = MAX(th, ih);
	}else if(viewtype == GUI_VIEWTYPE_ICON_ONLY || viewtype == GUI_VIEWTYPE_ICON_TEXT_TITLE){
		if(width)*width = iw;
		if(height)*height = ih;
	}else if(viewtype == GUI_VIEWTYPE_ICON_TEXT_TOP || viewtype == GUI_VIEWTYPE_ICON_TEXT_BOTTOM){
		if(width){*width = MAX(tw, iw); if(*width > 80)*width = 80; }
		if(height){*height = th+ih+ICON_TEXT_SPLIT; if(*height > 80)*height = 80; }
	}else{
		if(width)*width = 0;
		if(height)*height = 0;
	}
	g_object_unref(image);
	return TRUE;
}

static void	dsource_Redraw(t_HDataSource h, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight)
{
	t_DSAppStore* pme = (t_DSAppStore *)h;
	t_AppStoreEntry* child;
	BrdHandle brd;
	resid_t resid;
	t_HImage image = NULL;
	char title[GUID_NAME_MAX+1];
	gu32 old;
	
	child = _GetEntry(pme->entry, index);
	if(!child)
		return;
	if(pme->icon_w >= width || pme->icon_h >= height){
		pme->icon_w = 0;
		pme->icon_h = 0;
	}
	
	old = gdi_set_color(gdi, RGB_WHITE);
	strcpy(title, "<N/A>");
	if(child->type == APPSTORE_ENTRY_APP){
		t_AppStoreApp *app = (t_AppStoreApp *)child;
		brd = brd_get(app->guid);
		resid = RESID_APP_ICON;
		if(brd == BRD_HANDLE_ERROR) //2 资源获取失败则表示动态加载应用
			image = pack_GetIcon(app->guid);
		pack_GetModuleTitle(app->guid, app->module, title, sizeof(title));
	}else{
		t_AppStoreDir *dir = (t_AppStoreDir *)child;
		brd = brd_get("sys");
		resid = dir->icon;
		//image = brd_get_image0(brd, dir->icon);
		brd_get_string2(brd, dir->text, title, sizeof(title));
	}
	
	if(VIEWTYPE_IS_NORMAL(viewtype)){
		if(image == NULL)
			image = brd_get_image(brd, resid);
		gdi_draw_icon_text(gdi, image, title, width, height, guiRight, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
	}else if(VIEWTYPE_IS_ICON(viewtype)){
		int text_height = font_get(lang_get_current())->height;
		int icon_height = height;
		if(viewtype == GUI_VIEWTYPE_ICON_TEXT_BOTTOM || viewtype == GUI_VIEWTYPE_ICON_TEXT_TOP)
			icon_height -= text_height+ICON_TEXT_SPLIT;
		{
			gui_rect_t tmp;
			if(pme->icon_w && pme->icon_h){
				tmp.x = (width-pme->icon_w)>>1;
				tmp.y = (icon_height-pme->icon_h)>>1;
				tmp.dx = pme->icon_w;
				tmp.dy = pme->icon_h;
			}else{
				tmp.x = 0;
				tmp.y = 0;
				tmp.dx = width;
				tmp.dy = icon_height;
			}
			
			if(g_object_valid(image)){
				//2 动态加载的应用的图标，直接显示
				gdi_draw_image_in(gdi, tmp.x, tmp.y, tmp.dx, tmp.dy, image, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
			}else if(!hilight){
				//2 非焦点项，也直接显示
				gdi_draw_image_in_by_resid(gdi, tmp.x, tmp.y, tmp.dx, tmp.dy, brd, resid, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
			}else{
				//2 焦点项，如果是多帧gif，则播放动画
				mmc_stop(pme->mmc_anim);
				mmc_set_image_output(pme->mmc_anim, gdi, &tmp);
				mmc_play_resid(pme->mmc_anim, -1, brd, resid);
			}
		}
		if(viewtype == GUI_VIEWTYPE_ICON_TEXT_TITLE){
			if(hilight){
				t_HWindow wnd = wgt_parent_window(h->list);
				wnd_set_caption(wnd, title);
				if(wnd_has_title(wnd)){
					wnd_show_title_bar(wnd, FALSE);
				}else if(wnd_has_status(wnd)){
					wnd_show_sb_element(wnd, TB_TITLE);
				}else if(wnd_has_softkey(wnd)&&prod_ui_sk_show_title()){
					wgt_redraw(wnd_get_skbar(wnd), FALSE);
				}
			}
		}else{
			int ww;
			gdi_measure_text(gdi, title, -1, CHARSET_UTF8, width, &ww, NULL, NULL);
			ww += 8;
			if(ww > width)ww = width;
			gdi_rectangle_fill_ex(gdi, (width-ww)/2, icon_height, ww, text_height+2, 0xa0808080, 0xa0808080, GDI_FILL_MODE_SOLID, guiFourCorner);
			gdi_draw_text_in(gdi, 0, icon_height+1, width, text_height, title, -1, CHARSET_UTF8, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE);
		}
	}
	g_object_unref(image);
	gdi_set_color(gdi, old);
}

static int	dsource_GetCount(t_HDataSource h)
{
	return g_array_size(((t_DSAppStore *)h)->entry->children);
}
// if provided, hooked WIDGET_OnSelected never works. other wise, WIDGET_OnSelected can work.
void DSAppStore_Call(t_HDataSource h, int index, void *arg, int len)
{
	if(g_object_valid(h)){
		t_DSAppStore *pme = (t_DSAppStore *)h;
		t_AppStoreEntry* child;
		child = _GetEntry(pme->entry, index);
		if(child){
			if(child->type == APPSTORE_ENTRY_APP){
				t_AppStoreApp *app = (t_AppStoreApp *)child;
					pack_call(app->guid, app->module, arg, len, FALSE, NULL);
			}else{
				t_AppStoreDir *dir = (t_AppStoreDir *)child;
				__DSAppStore_push(h, dir);
				wgt_enable_attr(h->list, CTRL_ATTR_MODIFIED);
				wgt_redraw(h->list, FALSE);
				if(listctrl_get_viewtype(h->list) != GUI_VIEWTYPE_ICON_TEXT_TITLE){
					t_HWindow wnd = wgt_parent_window(h->list);
					if(g_object_valid(wnd)){
						wgt_set_caption_by_resid((t_HWidget)wnd, BRD_HANDLE_SELF, dir->text);
						wgt_set_icon_by_resid((t_HWidget)wnd, BRD_HANDLE_SELF, dir->icon);
						if(wnd_has_title(wnd)){
							wnd_show_title_bar(wnd, FALSE);
						}else if(wnd_has_status(wnd)){
							wnd_show_sb_element(wnd, TB_TITLE);
							wnd_show_sb_element(wnd, TB_TITLE_ICON);
						}else if(wnd_has_softkey(wnd)&&prod_ui_sk_show_title()){
							wgt_redraw(wnd_get_skbar(wnd), FALSE);
						}
					}
				}
			}
		}
	}
}

static error_t dsource_on_event(t_HDataSource h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	t_DSAppStore* pme = (t_DSAppStore *)h;
	t_AppStoreEntry* child;
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnSelected:
			child = _GetEntry(pme->entry, sparam);
			if(child){
				if(child->type == APPSTORE_ENTRY_APP){
					t_AppStoreApp *app = (t_AppStoreApp *)child;
					if(pme->func){
						pme->func(h, app->guid, app->module, pme->userdata);
					}else{
						pack_call(app->guid, app->module, NULL, 0, FALSE, NULL);
					}
				}else{
					t_AppStoreDir *dir = (t_AppStoreDir *)child;
					__DSAppStore_push(h, dir);
					wgt_enable_attr(h->list, CTRL_ATTR_MODIFIED);
					wgt_redraw(h->list, FALSE);
					if(listctrl_get_viewtype(h->list) != GUI_VIEWTYPE_ICON_TEXT_TITLE){
						t_HWindow wnd = wgt_parent_window(h->list);
						if(g_object_valid(wnd)){
							wgt_set_caption_by_resid((t_HWidget)wnd, BRD_HANDLE_SELF, dir->text);
							wgt_set_icon_by_resid((t_HWidget)wnd, BRD_HANDLE_SELF, dir->icon);
							if(wnd_has_title(wnd)){
								wnd_show_title_bar(wnd, FALSE);
							}else if(wnd_has_status(wnd)){
								wnd_show_sb_element(wnd, TB_TITLE);
								wnd_show_sb_element(wnd, TB_TITLE_ICON);
							}else if(wnd_has_softkey(wnd)&&prod_ui_sk_show_title()){
								wgt_redraw(wnd_get_skbar(wnd), FALSE);
							}
						}
					}
				}
				ret = SUCCESS;
			}
			break;
			
		case WIDGET_OnKeyEvent:
#if !defined(FTR_NO_INPUT_KEY)
			if((lparam == MVK_BACK || lparam == MVK_RSK) && sparam == MMI_KEY_UP){
				if(__DSAppStore_pop(h)){
					wgt_enable_attr(h->list, CTRL_ATTR_MODIFIED);
					wgt_redraw(h->list, FALSE);
					if(listctrl_get_viewtype(h->list) != GUI_VIEWTYPE_ICON_TEXT_TITLE){
						t_HWindow wnd = wgt_parent_window(h->list);
						if(g_object_valid(wnd)){
							wnd_set_caption_by_resid(wnd, BRD_HANDLE_SELF, pme->entry->text);
							wnd_set_icon_by_resid(wnd, BRD_HANDLE_SELF, pme->entry->icon);
							if(wnd_has_title(wnd)){
								wnd_show_title_bar(wnd, FALSE);
							}else if(wnd_has_status(wnd)){
								wnd_show_sb_element(wnd, TB_TITLE);
								wnd_show_sb_element(wnd, TB_TITLE_ICON);
							}else if(wnd_has_softkey(wnd)&&prod_ui_sk_show_title()){
								wgt_redraw(wnd_get_skbar(wnd), FALSE);
							}
						}
					}
					return SUCCESS;
				}
			}
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
	(PFNONCREATE)dsource_OnCreate
	, (PFNONDESTROY)dsource_OnDestroy

	, dsource_Measure
	, dsource_Redraw
	, dsource_GetCount
	, NULL			// CanFocused
	, dsource_on_event
	, NULL			// Refresh
};

CLASS_DECLARE(DSAppStore, t_DSAppStore, dsourceVtbl);


t_HDataSource	DSAppStore_New(appstore_select_func_t func, void *userdata)
{
	t_HDataSource ds = NULL;
	ds = (t_HDataSource)g_object_new0(&class_DSAppStore);
	if(g_object_valid(ds)){
		((t_DSAppStore*)ds)->func = func;
		((t_DSAppStore*)ds)->userdata = userdata;
	}
	return ds;
}

t_AppStoreDir* DSAppStore_GetRootNode(t_HDataSource h)
{
	return g_object_valid(h)? ((t_DSAppStore*)h)->root : NULL;
}

t_AppStoreDir * DSAppStore_GetCWD(t_HDataSource h)
{
	return g_object_valid(h)? ((t_DSAppStore*)h)->entry : NULL;
}

t_AppStoreNode DSAppStore_GetNode(t_HDataSource h, int index)
{
	return (g_object_valid(h))? _GetEntry(((t_DSAppStore*)h)->entry, index) : NULL;
}

void	DSAppStore_SetIconSize(t_HDataSource h, int icon_width, int icon_height)
{
	if(g_object_valid(h)){
		t_DSAppStore *pme = (t_DSAppStore *)h;
		pme->icon_w = icon_width;
		pme->icon_h = icon_height;
	}
}

t_AppStoreDir * AppStore_AddDir(t_AppStoreDir *node, resid_t icon, resid_t text)
{
	t_AppStoreDir *ndir = NULL;
	t_AppStoreDir *dir = (t_AppStoreDir *)node;
	if(dir == NULL){
		ndir = (t_AppStoreDir*)MALLOC(sizeof(t_AppStoreEntry));
		ndir->level = 0;
	}else if(dir->type == APPSTORE_ENTRY_DIR){
		g_idx_t index;
		index = g_array_append(dir->children, NULL);
		ndir = (t_AppStoreDir *)g_array_element(dir->children, index);
		if(ndir){
			ndir->type = APPSTORE_ENTRY_DIR;
			ndir->level = dir->level+1;
		}
	}
	if(ndir){
		ndir->children = g_array_new(sizeof(t_AppStoreEntry), 4, ARRAY_TYPE_DYN_ALIGN, 4);
		ndir->type = APPSTORE_ENTRY_DIR;
		ndir->icon = icon;
		ndir->text = text;
	}
	return ndir;
}

t_AppStoreApp * AppStore_AddApp(t_AppStoreDir *dir, const char *guid, t_AmsModuleId module)
{
	t_AppStoreApp *app = NULL;
	if(dir && dir->type == APPSTORE_ENTRY_DIR){
		g_idx_t index;
		index = g_array_append(dir->children, NULL);
		app = (t_AppStoreApp *)g_array_element(dir->children, index);
		if(app){
			app->type = APPSTORE_ENTRY_APP;
			app->module = module;
			app->guid = guid;
		}
	}
	return app;
}

//2 return number of app added
int	AppStore_AddByMime(t_AppStoreDir *dir, const char *mime, const char *method)
{
	int cursor = 0;
	if(dir && dir->type == APPSTORE_ENTRY_DIR){
#define N_ARRAY	6
		t_MimeMethodInfo info[N_ARRAY];
		int num, i;

		cursor = 0;
		while(1){
			num = mime_service_query(mime, method, app_current_guid(), info, N_ARRAY, cursor, MS_QUERY_F_SKIP_SELF|MS_QUERY_F_APP);
			for(i = 0; i < num; i++){
				if(info[i].module < MOD_ID_FUNC){
					AppStore_AddApp(dir, info[i].guid, info[i].module);
					cursor ++;
				}
			}
			if(num < N_ARRAY)
				break;
		}
	}
	return cursor;
}

int	AppStore_AddByMenuTree(t_AppStoreDir *dir, t_MenuTreeNode node)
{
	int count = 0;

	if(dir && dir->type == APPSTORE_ENTRY_DIR){
		t_MenuTreeNodeAttr attr;
		int index;
		if(prod_get_menu_node_attr(node, &attr) == SUCCESS){
			dir->icon = attr.img;
			dir->text = attr.text;
		}
		count = prod_get_menu_count(node);
		for(index = 0; index < count; index++){
			t_MenuTreeNode child;
			t_AppStoreDir * entry;
			
			child = prod_get_menu_node(node, index);
			if(prod_get_menu_node_attr(child, &attr) == SUCCESS){
				if(attr.type == MTREE_NODE_DIR){
					entry = AppStore_AddDir(dir, attr.img, attr.text);
					AppStore_AddByMenuTree(entry, child);
				}else{
					AppStore_AddApp(dir, attr.name, attr.module);
				}
			}
		}
	}
	return count;
}



void	listctrl_attach_mime_method(t_HWidget handle, const char *mime, const char *method, appstore_select_func_t func, void *userdata)
{
	if(g_object_valid(handle)){
		int num;
		
		num = mime_service_query(mime, method, app_current_guid(), NULL, -1, 0, MS_QUERY_F_SKIP_SELF|MS_QUERY_F_APP);
		if(num > 0){
			t_HDataSource ds;
			ds = DSAppStore_New(func, userdata);
			
			AppStore_AddByMime(DSAppStore_GetRootNode(ds), mime, method);
			
			listctrl_attach_datasource(handle, ds);
			g_object_unref((t_HObject)ds);
		}
	}
}

