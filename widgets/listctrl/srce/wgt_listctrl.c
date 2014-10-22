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



//1 listctrl再分析
/*
=========================================================================
listctrl有两种不同数据模式:
1) 普通模式----DSOURCE_TYPE_NORMAL
	一般的listctrl只需要如下两个接口
	SetItemCount(): 设置条目数目
	DrawItemCallback(): 条目绘制
	比如: 
2) dbase模式----DSOURCE_TYPE_DBASE
	因为dbase本身绑定有redraw和select接口，而且dbase本身有接口获取item count
3) 高级模式----DSOURCE_TYPE_CUSTOM
	提供VTBL(datasource)接口, 用户可以绑定不同的datasource
=========================================================================

=========================================================================
List控件显示特性描述:
1) list view: 主体是text
	1.1) 每行一个item(icon+text或者单text), 每行等高
	1.2) 可以添加属性如check/radio, 分别对应多选/单选
	1.3) hilight状态, item icon如果是动画, 则运行动画, 否则显示静态图片
	1.4) 允许hilight项与normal项高度有差异，呈现不同绘制风格

	高级特性:适用于数据比较多, 一次性加载浪费内存切加载慢, 比如数据库
	1.4) 允许用户设置数据源和显示方式, 提供最大限度灵活度, 将其实现为DBList
	
2) icon view: 主体是icon
	2.1) 每个item包括icon+text
	2.2) 每个item显示区域为正方形, cell方式显示
	2.3) 如果item icon为动画, 则hilight时显示动画

[2013-03-18]----补充: list view CUSTOM
目前list项是固定宽度，各item位置及每页能显示item数目都可以算出来
而且在数据未加载前就能确定。考虑到另一种情况:
比如短信对话框界面，每个item显示高度不同，需要从dsource获取各项item高度
对于非固定高度的list view要求如下:
1. 必须是list view模式
2. 必须绑定VTBL(datasource)，而且必须提供Measure接口以获取高度
3. 各item高度值不能在初始化时一次性获取，而是按需获取并记录下来
4. 数据刷新时高度值也必须刷新

[2013-03-26]----补充:
考虑到listctrl控件list方式显示的文本，其实也可以多行多列显示，
以前的list(单列文本为主)/cell(行列icon为主)两种方式无法满足显示需求
需要重新考虑设计:
1. listctrl可以任意行列显示: row和col不可能同时为0，
	1.1) row=0表示固定列, 如果设置成hscrolling则像iphone那样分页，如果vscrolling则上下滚动
	1.2) col=0表示固定行, 如果hscrolling则左右滚动，如果vscrolling则
	1.3) custom list限定为只适用于col=1或row=1的情况下
2. 允许给item设置属性，包括:
	2.1) icon view: 以icon为主，text: 不显示/顶部/底部/title
	2.2) normal view: 以text为主，icon: 不显示/text左边/text右边
	而item显示属性不是属于listctrl的范畴，它影响的是绘制，根据上述case，抽象出如下viewtype: 
	GUI_VIEWYTPE_TEXT,
	GUI_VIEWTYPE_ICON,
	GUI_VIEWTYPE_ICON_TEXT_LEFT,
	GUI_VIEWTYPE_ICON_TEXT_RIGHT,
	GUI_VIEWTYPE_ICON_TEXT_TOP,
	GUI_VIEWTYPE_ICON_TEXT_BOTTOM,
	GUI_VIEWTYPE_ICON_TEXT_TITLE,
	GUI_VIEWTYPE_CUSTOM,
	GUI_VIEWTYPE_USER,

listctrl_set_viewtype(h, viewtype);
	默认为GUI_VIEWYTPE_TEXT, 如果指定Custom类型，每个item高宽须通过VTBL(datasource)->Measure接口获取

listctrl_set_rows_perpage(h, rows, row_height, row_height_hilight);
	设置每页行数或行高来计算行数，高亮项高度可不同，默认rows=0,rheight=rheight_hilight=20
	对非GUI_VIEWTYPE_CUSTOM类型，rows和row_height不能同为0
	对GUI_VIEWTYPE_CUSTOM类型，此函数无效
listctrl_set_cols_perpage(h, cols, col_width);
	设置每页列数或列宽来计算列数，两者不可以同为0，默认cols=1, col_width=0(表示client_dx)
	
	row非0，rol非0----每页显示的行列数，这样cell(w,h)都可计算出来
	row非0，col为0----固定行数，须指定列宽，列数根据item count计算，显示顺序是从上至下从左至右
	row为0，col非0----固定列数，须指定行高，行数根据item count计算，显示顺序是从左至右从上至下
	row为0，col为0----非法值，不支持

如果同时指定cell(w,h)，根据控件client区域大小计算每页显示行列数，规定显示顺序是从左至右从上至下

默认如果两者都没调用，就是单列行高为20的列表
*/
	


/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
	
#include "wgt_listctrl.h"
#include "sys_resid.h"

//#define	FTR_LISTCTRL_SCROLL_WITH_PEN_MOVE


/*===========================================================================

DEFINITIONS

===========================================================================*/

#define LIST_DEFAULT_HEIGHT		100
#define LIST_DEFAULT_WIDTH		100
#define LIST_DEFAULT_CELL_DY	16

#define	POPUP_ITEM_MARGIN	8

#define LIST_DEFAULT_ITEM_NB		5

#define LISTCTRL_ATTR_MULTI_SEL		CTRL_ATTR_BITS_USER /*  */


// internal
#define	CLFLAG_ITEM_NAV_VERTICAL	BIT_28		//4 置位则表示vertial，从上至下从左至右，否则从左至右从上至下
#define	CLFLAG_ITEM_NAV_HORIZONTAL	0	

#define	CLFLAG_SCROLLABLE			BIT_29
//2 有两个含义:
//3 1. 支持滚动，表示pen有移动
//3 2. 支持滚动但条目只有一个page，从而无需滚动，或根本不支持滚动，则用来表示条目间焦点移动
#define	CLFLAG_PEN_MOVED			BIT_30
#define	CLFLAG_PEN_DOWN			BIT_31

#define	LISTCTRL_IS_NORMAL(h)		(((t_CommonListCtrl*)(h))->s_type == DSOURCE_TYPE_NORMAL)

#define	LIST_VIEWTYPE_CUSTOM(h)	(((t_CommonListCtrl*)(h))->viewtype == GUI_VIEWTYPE_CUSTOM)

#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

/*===========================================================================

Class Definitions

===========================================================================*/


static int	__listctrl_cell_index(t_CommonListCtrl *pme, int row, int col)
{
	int index = -1;
	if(row < 0 || col < 0){
		index = -1;
	}else if(BIT_TEST(pme->flags, CLFLAG_ITEM_NAV_HORIZONTAL)){
		if(BIT_TEST(pme->flags, CLFLAG_SCROLLING_HORIZONTAL)){
			//2 horizontal scrolling
			int col_i = col%pme->col_perpage;
			index = pme->row_n*(col-col_i)+row*pme->col_perpage+col_i;
		}else{
			//2 vertical scrolling
			index = row*pme->col_n+col;
		}
	}else{
		if(BIT_TEST(pme->flags, CLFLAG_SCROLLING_HORIZONTAL)){
			//2 horizontal scrolling
			index = col*pme->row_n+row;
		}else{
			//1 NEVER comes here!!!
			//2 vertical scrolling
			int row_i = row%pme->row_perpage;
			index = pme->col_n*(row-row_i)+col*pme->row_perpage+row_i;
		}
	}
	return index;
}

static void	__listctrl_index_cell(t_CommonListCtrl *pme, int index, int *row, int *col)
{
	int r, c;
	if(BIT_TEST(pme->flags, CLFLAG_ITEM_NAV_HORIZONTAL)){
		if(BIT_TEST(pme->flags, CLFLAG_SCROLLING_HORIZONTAL)){
			r = index/pme->col_perpage;
			c = index%pme->col_perpage;
			//2 horizontal scrolling
			while(r >= pme->row_n){
				r -= pme->row_n;
				c += pme->col_perpage;
			}
		}else{
			//2 vertical scrolling
			r = index/pme->col_n;
			c = index%pme->col_n;
		}
	}else{
		if(BIT_TEST(pme->flags, CLFLAG_SCROLLING_HORIZONTAL)){
			//2 horizontal scrolling
			c = index/pme->row_n;
			r = index%pme->row_n;
		}else{
			//2 vertical scrolling
			c = index/pme->row_perpage;
			r = index%pme->row_perpage;
			while(c >= pme->col_n){
				c -= pme->col_n;
				r += pme->row_perpage;
			}
		}
	}
	
	if(row)*row = r;
	if(col)*col = c;
}

#define	CELL_X(pme, row, col)	((col) * (pme)->col_width)
#define	CELL_Y(pme, row, col)	((row) * (pme)->row_height)


// row<0,col<0, index<0. unhilight
// row<0,col<0, index>=0, hilight, row&col should be calculated
// row>=0,col>=0, index<0. hilight, index should be calaculated
// row>=0,col>=0, index>=0, hilight, index&row&col all valid

#define	LISTCTRL_SYNC_INDEX(pme, index, row, col)	do { \
		if(index < 0 && row >= 0 && col >= 0){ \
			index = __listctrl_cell_index(pme, row, col); \
		}else if(index >= 0 && index < pme->count){ \
			__listctrl_index_cell(pme, index, &(row), &(col)); \
		}\
	}while(0)

static void	__listctrl_index_rect(t_CommonListCtrl *pme, int index, int row, int col, gui_rect_t *rect)
{
	if(rect){
		if(LIST_VIEWTYPE_CUSTOM(pme)){
			//2 CUSTOM List
			if(pme->u.s_custom.pv[index].dy == 0){
				//2 CUSTOM list: calc items' position
				int j, ww, hh;
				LISTSOURCE_Measure(pme->u.s_custom.hds, pme->viewtype, index, &ww, &hh);
				pme->u.s_custom.pv[index].dy = hh+pme->item_y0_margin+pme->item_y1_margin;
				for(j = index+1; j < pme->count; j++)
					pme->u.s_custom.pv[j].y += pme->u.s_custom.pv[index].dy;
				wgt_client_drawing_dy(pme) = pme->u.s_custom.pv[pme->count-1].y+pme->u.s_custom.pv[pme->count-1].dy;
			}
			rect->dx = pme->col_width;
			rect->dy = pme->u.s_custom.pv[index].dy;
			rect->x = 0;
			rect->y = pme->u.s_custom.pv[index].y;
		}else{
			rect->dx = pme->col_width;
			rect->dy = pme->row_height;
			rect->x = rect->dx * col;
			rect->y = rect->dy * row;
			if(pme->col_n == 1 && pme->row_height != pme->row_height_hilight){
				if(index == pme->cursor)
					rect->dy = pme->row_height_hilight;
				else if(index > pme->cursor)
					rect->y += pme->row_height_hilight - pme->row_height;
			}
		}
		
	}
}

static gboolean	__listctrl_index_visible(t_CommonListCtrl *pme, int index)
{
	int row, col;
	coord_t x, y, dx, dy;

	LISTCTRL_SYNC_INDEX(pme, index, row, col);
	
	if(index >= 0 && index < pme->count && wgt_is_visible(pme)){
		gui_rect_t rt;
		
		__listctrl_index_rect(pme, index, row, col, &rt);
		x = rt.x;
		y = rt.y;
		dx = rt.dx;
		dy = rt.dy;

		if(wgt_client_point_visible((t_HWidget)pme, x, y) || wgt_client_point_visible((t_HWidget)pme, x+dx-1, y+dy-1))
			return TRUE;
	}
	return FALSE;
}



void	listctrl_show_item_in(t_CommonListCtrl *pme, int index, t_HGDI gdi, coord_t x, coord_t y, gu16 dx, gu16 dy, gboolean defer, gboolean selected)
{
	if(index >= 0 && index < pme->count && g_object_valid(gdi) && dx > 0 && dy > 0){
		const t_UITheme *theme = ui_theme_get();
		gboolean masked = FALSE;
		gu32 mask=0;

	
		//2 备份gdi上下文
		gdi_save_context(gdi);
		gdi_translate(gdi, x, y);
		gdi_set_clip(gdi, 0, 0, dx, dy);
		gdi_move_to(gdi, 0, 0);
		
		if(pme->s_type == DSOURCE_TYPE_NORMAL){
			if(pme->u.s_normal.func)
				pme->u.s_normal.func(gdi, pme->viewtype, index, dx, dy, selected, pme->u.s_normal.userdata);
		}else if(pme->s_type == DSOURCE_TYPE_DBASE){
			g_ds_RedrawRecord(pme->u.hdb, index, gdi, pme->viewtype, dx, dy, selected);
		}else{
			LISTSOURCE_Redraw(pme->u.s_custom.hds, gdi, pme->viewtype, index, dx, dy, selected);
		}
		//2 恢复gdi上下文
		gdi_restore_context(gdi);

		if(!defer)
			gdi_blt(gdi, x, y, dx, dy);
	}
}

static gboolean	__listctrl_redraw_index(t_CommonListCtrl *pme, t_HGDI gdi, int index, int row, int col, gboolean defer, gboolean selected)
{
	coord_t x, y, dx, dy;

	//2 如果当前listctrl还没初始化，则不能进行后续绘制，否则可能异常
	if(wgt_test_attr(pme, CTRL_ATTR_MODIFIED))
		return FALSE;
	
	LISTCTRL_SYNC_INDEX(pme, index, row, col);
	
	//g_func_trace_entry();
	//g_printf("row=%d, col=%d, defer=%d, selected=%d,[%d,%d]", row, col, defer, selected, wgt_client_origin_x(pme), wgt_client_origin_y(pme));

	if(index >= 0 && index < pme->count && wgt_is_visible(pme)){
		gui_rect_t rt;
		const t_UITheme *theme = ui_theme_get();
		gboolean masked = FALSE;
		gu32 mask=0;
		
		__listctrl_index_rect(pme, index, row, col, &rt);
		x = rt.x;
		y = rt.y;
		dx = rt.dx;
		dy = rt.dy;

		if(!wgt_client_point_visible((t_HWidget)pme, x, y) &&
			!wgt_client_point_visible((t_HWidget)pme, x+dx-1, y+dy-1))
			return FALSE;

		if(gdi == NULL)
			gdi = wgt_get_client_gdi((t_HWidget)pme);
		else
			g_object_ref(gdi);

#if defined(FTR_NO_INPUT_KEY)
		//2 CTRL_ATTR_DRAWING被置位表示是整个listctrl在重画，否则仅仅是某个条目重画
		if(selected && wgt_test_attr(pme, CTRL_ATTR_DRAWING))
			selected = FALSE;
#endif
		if(selected && !wgt_test_attr(pme, CTRL_ATTR_ENABLED))
			selected = FALSE;

		if(selected && (wgt_is_focused(pme) || wgt_is_popup(pme) || (!(pme->flags & CLFLAG_CURSOR_NODRAW_ONKILLFOCUS)))){
			if((pme->flags & CLFLAG_HILIGHT_SHOW_MASK)){
				mask = gdi_set_mask_color(gdi, MAKE_ARGB(0x80, 0, 0, 0xff));
				masked = TRUE;
			}else if(wgt_is_focused(pme)){
				gdi_style_show_hilight_bg(gdi, x, y, dx, dy, guiFourCorner, TRUE);
			}else{
				gdi_set_color(gdi, RGB_GRAY);
				gdi_rectangle_fill(gdi, x, y, dx, dy, guiFourCorner);
			}
		}else{
			if(!defer)
				gdi_clear(gdi, x, y, dx, dy, guiFourCorner);
		}
		
		x += pme->item_x0_margin;
		y += pme->item_y0_margin;
		dx -= pme->item_x0_margin+pme->item_x1_margin;
		dy -= pme->item_y0_margin+pme->item_y1_margin;
		
		if(dx > 0 && dy > 0){
			if(pme->style == LISTCTRL_STYLE_CHECK){
				if(pme->s_type != DSOURCE_TYPE_CUSTOM || LISTSOURCE_CanFocused(pme->u.s_custom.hds, index)){
					gdi_draw_image_in(gdi, x, y, 20, dy, pme->sdata->icons[pme->sdata->checked[index]?1:0], IDF_VALIGN_BESTFIT|IDF_HALIGN_BESTFIT);
					x += 20;
					dx -= 20;
				}
			}
			if(pme->style == LISTCTRL_STYLE_RADIO){
				if(pme->s_type != DSOURCE_TYPE_CUSTOM || LISTSOURCE_CanFocused(pme->u.s_custom.hds, index)){
					gdi_draw_image_in(gdi, x, y, 20, dy, pme->sdata->icons[(index==pme->sdata->i_selected)?1:0], IDF_VALIGN_BESTFIT|IDF_HALIGN_BESTFIT);
					x += 20;
					dx -= 20;
				}
			}
			
			//2 备份gdi上下文
			gdi_save_context(gdi);
			gdi_translate(gdi, x, y);
			gdi_set_clip(gdi, 0, 0, dx, dy);
			gdi_move_to(gdi, 0, 0);
			
			if(selected)
				gdi_set_color(gdi, theme->hilight_fg_color);
			else
				gdi_set_color(gdi, theme->fg_color);
			
			if(pme->s_type == DSOURCE_TYPE_NORMAL){
				if(pme->u.s_normal.func)
					pme->u.s_normal.func(gdi, pme->viewtype, index, dx, dy, selected, pme->u.s_normal.userdata);
			}else if(pme->s_type == DSOURCE_TYPE_DBASE){
				g_ds_RedrawRecord(pme->u.hdb, index, gdi, pme->viewtype, dx, dy, selected);
			}else{
				LISTSOURCE_Redraw(pme->u.s_custom.hds, gdi, pme->viewtype, index, dx, dy, selected);
			}
			//2 恢复gdi上下文
			gdi_restore_context(gdi);
		}
		//2  绘制分割线
		if(!BIT_TEST(pme->flags, CLFLAG_DISABLE_HLINE)){
			gdi_set_color(gdi, RGB_LTGRAY);
			gdi_hline(gdi, rt.y+rt.dy-1, rt.x, rt.x+rt.dx-1);
		}
		if(col != pme->col_n-1 && !BIT_TEST(pme->flags, CLFLAG_DISABLE_VLINE)){
			gdi_set_color(gdi, RGB_LTGRAY);
			gdi_vline(gdi, rt.x+rt.dx-1, rt.y, rt.y+rt.dy-1);
		}
		if(!defer)
			gdi_blt(gdi, rt.x, rt.y, rt.dx, rt.dy);
		if(masked)
			gdi_set_mask_color(gdi, mask);
		g_object_unref(gdi);
		return TRUE;
	}

	//g_func_trace_exit();
	return FALSE;
}

static void	__listctrl_calc_perpage(t_CommonListCtrl *pme);
int		__listctrl_set_cursor(t_CommonListCtrl *pme, int index, int row, int col, gboolean redraw)
{
	int index_old;
	int row_old, col_old;

	index_old = pme->cursor;
	if(index_old < 0){
		pme->row_i = -1;
		pme->col_i = -1;
	}
	row_old = pme->row_i;
	col_old = pme->col_i;

	if(wgt_test_attr(pme, CTRL_ATTR_MODIFIED)){
		if(redraw){
			__listctrl_calc_perpage(pme);
		}else{
			if(index < 0){
				pme->row_i = -1;
				pme->col_i = -1;
				pme->cursor = -1;
			}else if(index < pme->count){
				pme->row_i = row;
				pme->col_i = col;
				pme->cursor = index;
				wgt_hook((t_HWidget)pme, WIDGET_OnSelChanged, index_old, index);
			}
			return index_old;
		}
	}
	LISTCTRL_SYNC_INDEX(pme, index, row, col);
	
	if(index < 0){
		pme->row_i = -1;
		pme->col_i = -1;
		pme->cursor = -1;
		if(redraw && index_old >= 0 && index_old < pme->count)
			__listctrl_redraw_index(pme, NULL, index_old, row_old, col_old, FALSE, FALSE);
	}else if(index < pme->count){
		if(index == index_old){
#if defined(FTR_NO_INPUT_KEY)
				__listctrl_redraw_index(pme, NULL, index, row, col, FALSE, TRUE);
#endif
		}else{
			gui_rect_t rect;
			
			pme->row_i = row;
			pme->col_i = col;
			pme->cursor = index;
			__listctrl_index_rect(pme, index, row, col, &rect);
			if(wgt_make_visible((t_HWidget)pme, (const gui_rect_t *)&rect)){
				//2 animation for page flipping can be here
				if(redraw)
					wgt_redraw((t_HWidget)pme, FALSE);
			}else if(redraw/* && !wgt_test_attr(pme, CTRL_ATTR_MODIFIED)*/){
				//2 如果以MODIFIED标志来判断是否绘制, 则在如下case会导致bug
				//4 desktop的list(today)控件须在OnDraw时先refresh然后redraw, 以保证today数据实时性
				//4 因为所属window处于DRAWING状态, 为防止重绘而直接返回, 
				//4 此处如果因为MODIFIED标志未复位而不绘制, 则表现为条目未滚动刷新
				t_HGDI gdi = wgt_get_client_gdi((t_HWidget)pme);
				if(gdi){
					if(/*VIEWTYPE_IS_CELL(pme->viewtype) || */pme->row_height == pme->row_height_hilight){
						//2 常态下不绘制焦点，因此也就没必要清除焦点
#if !defined(FTR_NO_INPUT_KEY)
						__listctrl_redraw_index(pme, gdi, index_old, row_old, col_old, FALSE, FALSE);
#endif
						__listctrl_redraw_index(pme, gdi, index, row, col, FALSE, TRUE);
					}else{
						int min, max, i;
						min = MIN(index, index_old);
						max = MAX(index, index_old);
						for(i = min; i < index; i++){
							__listctrl_redraw_index(pme, gdi, i, -1, -1, FALSE, FALSE);
						}
						__listctrl_redraw_index(pme, gdi, index, row, col, FALSE, TRUE);
						for(i = index+1; i < max; i++){
							__listctrl_redraw_index(pme, gdi, i, -1, -1, FALSE, FALSE);
						}
					}
					g_object_unref(gdi);
				}
			}
			wgt_hook((t_HWidget)pme, WIDGET_OnSelChanged, index_old, index);
		}
	}

	return index_old;
}

static error_t	__listctrl_next(t_CommonListCtrl *pme, gui_direction_e direction)
{
	int row, col, index;

	if(pme->count <= 0)
		return NOT_HANDLED;
	
	row = pme->row_i;
	col = pme->col_i;
	index = pme->cursor;

	while(1){
		switch(direction){
			case guiTop:
				row--;
				if(row < 0){
					if(BIT_TEST(pme->flags, CLFLAG_AUTOTAB_UD))
						return NOT_HANDLED;
					if(pme->row_n == 1)
						return SUCCESS;
					row = pme->row_n - 1;
				}
				break;
			case guiBottom:
				row++;
				if(row >= pme->row_n){
					if(BIT_TEST(pme->flags, CLFLAG_AUTOTAB_UD))
						return NOT_HANDLED;
					if(pme->row_n == 1)
						return SUCCESS;
					row = 0;
				}
				break;
			case guiLeft:
				if(index == 0 && BIT_TEST(pme->flags, CLFLAG_AUTOTAB_LR))
					return NOT_HANDLED;
				if(pme->col_n == 1)
					return SUCCESS;
				col--;
				if(col < 0){
					col = pme->col_n - 1;
					if(pme->row_n > 1){
						row--;
						if(row < 0)
							row = pme->row_n - 1;
					}
				}
				break;
			case guiRight:
				if(index == pme->count-1 && BIT_TEST(pme->flags, CLFLAG_AUTOTAB_LR))
					return NOT_HANDLED;
				if(pme->col_n == 1)
					return SUCCESS;
				col++;
				if(col >= pme->col_n){
					col = 0;
					if(pme->row_n > 1){
						row++;
						if(row >= pme->row_n)
							row = 0;
					}
				}
				break;
		}
		index = __listctrl_cell_index(pme, row, col);
		if(index >= 0 && index < pme->count && (pme->s_type != DSOURCE_TYPE_CUSTOM || LISTSOURCE_CanFocused(pme->u.s_custom.hds, index)))
			break;
	}
	if(index != pme->cursor)
		__listctrl_set_cursor(pme, index, row, col, TRUE);
	return SUCCESS;
}

#include "ds_icontext.h"

static error_t		__listctrl_on_create(t_CommonListCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	t_HDataSource ds;
	ds = g_object_new0(&class_DSIconText);
	
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);
	listctrl_set_rows((t_HList)pme, 0, prod_ui_list_item_height(), 0);
	listctrl_set_cols((t_HList)pme, 1, 0);
	listctrl_set_flags((t_HList)pme, CLFLAG_SCROLLING_ITEM);

	listctrl_attach_datasource((t_HList)pme, ds);
	g_object_unref(ds);
	
	pme->viewtype = GUI_VIEWTYPE_NORMAL;
	pme->cursor = -1;
	pme->row_i = -1;
	pme->col_i = -1;

	return SUCCESS;
}

#define	listctrl_detach(pme)	do { \
	if(((t_CommonListCtrl*)(pme))->s_type == DSOURCE_TYPE_DBASE){ \
		g_object_unref(((t_CommonListCtrl*)(pme))->u.hdb); \
	}else if(((t_CommonListCtrl*)(pme))->s_type == DSOURCE_TYPE_CUSTOM){ \
		g_object_unref(((t_CommonListCtrl*)(pme))->u.s_custom.hds); \
		FREEIF(((t_CommonListCtrl*)(pme))->u.s_custom.pv); \
	} \
}while(0)

static void	__listctrl_on_destroy(t_CommonListCtrl* pme)
{
	listctrl_detach(pme);
	if(pme->sdata){
		g_object_unref_0(pme->sdata->icons[0]);
		g_object_unref_0(pme->sdata->icons[1]);
		FREEIF(pme->sdata->checked);
		FREEIF(pme->sdata->selects);
		FREE(pme->sdata);
	}

	g_object_inherit_destroy(pme);
}

/*
listctrl_set_viewtype(h, viewtype);
	默认为GUI_VIEWYTPE_TEXT, 如果指定Custom类型，每个item高宽须通过VTBL(datasource)->Measure接口获取

listctrl_set_rows_perpage(h, rows, row_height, row_height_hilight);
	设置每页行数或行高来计算行数，高亮项高度可不同，默认rows=0,rheight=rheight_hilight=20
	对非GUI_VIEWTYPE_CUSTOM类型，rows和row_height不能同为0
	对GUI_VIEWTYPE_CUSTOM类型，此函数无效
listctrl_set_cols_perpage(h, cols, col_width);
	设置每页列数或列宽来计算列数，两者不可以同为0，默认cols=1, col_width=0(表示client_dx)
	
	row非0，rol非0----每页显示的行列数，这样cell(w,h)都可计算出来
	row非0，col为0----固定行数，须指定列宽，列数根据item count计算，显示顺序是从上至下从左至右
	row为0，col非0----固定列数，须指定行高，行数根据item count计算，显示顺序是从左至右从上至下
	row为0，col为0----非法值，不支持
*/
static void	__listctrl_calc_perpage(t_CommonListCtrl *pme)
{
	if(pme->s_type == DSOURCE_TYPE_DBASE && !wgt_test_attr(pme, CTRL_ATTR_MODIFIED)){
		if(g_object_valid(pme->u.hdb) && g_ds_GetRecordNum(pme->u.hdb) != pme->count)
			wgt_enable_attr(pme, CTRL_ATTR_MODIFIED);
	}
	if(wgt_test_attr(pme, CTRL_ATTR_MODIFIED)){
		gui_rect_t client;
		
		wgt_get_client_rect((t_HWidget)pme, &client);

		if(pme->s_type == DSOURCE_TYPE_DBASE)
			pme->count = g_ds_GetRecordNum(pme->u.hdb);
		else if(pme->s_type == DSOURCE_TYPE_CUSTOM)
			pme->count = LISTSOURCE_GetCount(pme->u.s_custom.hds);

		BIT_CLEAR(pme->flags, CLFLAG_ITEM_NAV_VERTICAL);
		BIT_CLEAR(pme->flags, CLFLAG_ITEM_NAV_HORIZONTAL);
		//2 检查style
		if(pme->style == LISTCTRL_STYLE_CHECK){
			if(pme->sdata->checked == NULL){
				pme->sdata->checked = MALLOC(pme->count);
				memset(pme->sdata->checked, FALSE, pme->count);
			}
			if(pme->sdata->selects == NULL){
				pme->sdata->selects = MALLOC(pme->count * sizeof(gu16));
				memset(pme->sdata->selects, 0xff, pme->count * sizeof(gu16));
			}
		}
		//2 检查list viewtype
		if(pme->viewtype == GUI_VIEWTYPE_CUSTOM){
			//2 CUSTOM
			if(pme->s_type != DSOURCE_TYPE_CUSTOM || !g_object_valid(pme->u.s_custom.hds) || !OBJECT_INTERFACE(pme->u.s_custom.hds, VTBL(datasource), Measure))
				g_raise("list view is CUSTOM, MUST bind data source");
			if(BIT_TEST(pme->flags, CLFLAG_AUTO_RESIZE_W) || BIT_TEST(pme->flags, CLFLAG_AUTO_RESIZE_H))
				g_raise("<CLFLAG_AUTO_RESIZE> not allowed for <GUI_VIEWTYPE_CUSTOM>");
			pme->col_perpage = 1;
			pme->col_width = client.dx;
			pme->col_n = 1;
			pme->col_i = 0;
			pme->row_n = pme->count;
			if(pme->u.s_custom.pv == NULL && pme->count){
				pme->u.s_custom.pv = MALLOC(sizeof(gui_v_t)*pme->count);
				memset(pme->u.s_custom.pv, 0, (sizeof(gui_v_t)*pme->count));
			}
			pme->row_perpage = 0;
			wgt_enable_attr(pme, CTRL_ATTR_VSCROLLABLE);
			BIT_CLEAR(pme->flags, CLFLAG_SCROLLING_HORIZONTAL);
			BIT_SET(pme->flags, CLFLAG_SCROLLING_VERTICAL);
			BIT_SET(pme->flags, CLFLAG_ITEM_NAV_HORIZONTAL);
			BIT_SET(pme->flags, CLFLAG_SCROLLABLE);
		}else{
			int row_perpage, col_perpage;
			int rows, cols;

			if(pme->row_perpage_set == 0 && pme->row_height == 0)
				g_raise("row_perpage and row_height not allowed to be both 0");
			if(pme->col_perpage_set == 0 && pme->col_width == 0)
				g_raise("col_perpage and col_width not allowed to be both 0");

			row_perpage = pme->row_perpage_set;
			col_perpage = pme->col_perpage_set;
			if(row_perpage){
				pme->row_height = client.dy/row_perpage;
				pme->row_height_hilight = pme->row_height;
			}else{
				if(client.dy <= pme->row_height_hilight)
					row_perpage = 1;
				else
					row_perpage = (client.dy-pme->row_height_hilight)/pme->row_height+1;
			}
			if(col_perpage){
				pme->col_width = client.dx/col_perpage;
				BIT_SET(pme->flags, CLFLAG_ITEM_NAV_HORIZONTAL);
			}else{
				col_perpage = client.dx/pme->col_width;
				if(col_perpage == 0)col_perpage = 1;
				BIT_SET(pme->flags, CLFLAG_ITEM_NAV_VERTICAL);
			}
			pme->row_perpage = row_perpage;
			pme->col_perpage = col_perpage;


			if(BIT_TEST(pme->flags, CLFLAG_ITEM_NAV_HORIZONTAL)){
				rows = (pme->count+col_perpage-1)/col_perpage;
				if(BIT_TEST(pme->flags, CLFLAG_SCROLLING_HORIZONTAL)){
					//2 horizontal scrolling
					if(rows >= row_perpage)
						pme->row_n = row_perpage;
					else
						pme->row_n = rows;
				
					pme->col_n = 0;
					while(rows > row_perpage){
						rows -= row_perpage;
						pme->col_n += col_perpage;
					}
					if(rows > 1){
						pme->col_n += col_perpage;
					}else if(rows == 1){
						int col_x = pme->count%col_perpage;
						if(pme->count > 0 &&  col_x == 0)
							pme->col_n += col_perpage;
						else
							pme->col_n += col_x;
					}
					if(pme->col_n > pme->col_perpage){
						wgt_enable_attr(pme, CTRL_ATTR_HSCROLLABLE);
						BIT_SET(pme->flags, CLFLAG_SCROLLABLE);
					}
				}else{
					//2 vertical scrolling
					pme->row_n = rows;
					if(rows == 1)
						pme->col_n = pme->count;
					else
						pme->col_n = col_perpage;
					
					if(pme->row_n > pme->row_perpage){
						wgt_enable_attr(pme, CTRL_ATTR_VSCROLLABLE);
						BIT_SET(pme->flags, CLFLAG_SCROLLABLE);
					}
				}
			}else{
				BIT_CLEAR(pme->flags, CLFLAG_SCROLLING_VERTICAL);
				BIT_CLEAR(pme->flags, CLFLAG_SCROLLING_HORIZONTAL);
				cols = (pme->count+row_perpage-1)/row_perpage;
				
				//2 horizontal scrolling
				pme->col_n = cols;
				if(cols == 1)
					pme->row_n = pme->count;
				else
					pme->row_n = row_perpage;
				
				if(pme->col_n > pme->col_perpage){
					wgt_enable_attr(pme, CTRL_ATTR_HSCROLLABLE);
					BIT_SET(pme->flags, CLFLAG_SCROLLABLE);
					BIT_SET(pme->flags, CLFLAG_SCROLLING_HORIZONTAL);
				}
			}
		}
		wgt_client_drawing_dx(pme) = pme->col_n*pme->col_width;
		wgt_client_drawing_dy(pme) = pme->row_n*pme->row_height+pme->row_height_hilight-pme->row_height;
		
		LISTCTRL_SYNC_INDEX(pme, pme->cursor, pme->row_i, pme->col_i);
	}
}

void	__listctrl_auto_resize(t_CommonListCtrl *pme)
{
	t_HDataSource ds = pme->u.s_custom.hds;
	gs16 index;
	gu16 dx, dy;
	int width, height;
	
	gs16 max_width, max_height, row_max, col_max, row, col;
	
	if(pme->s_type != DSOURCE_TYPE_CUSTOM)
		g_raise("<CLFLAG_AUTO_RESIZE> MUST be set with customized datasource attached");
	if(!OBJECT_INTERFACE(ds, VTBL(datasource), Measure))
		g_raise("<CLFLAG_AUTO_RESIZE> MUST be set with VTBL(datasource)->Measure provided");
	if(pme->row_perpage_set == 0 && pme->col_perpage_set == 0)
		pme->col_perpage_set = 1;
	//4 只允许单行或者单列能auto resize
	//else if(pme->row_perpage_set != 1 && pme->col_perpage_set != 1)
	//	g_raise("<CLFLAG_AUTO_RESIZE> MUST be set with 1 row or 1 column");

	if(pme->s_type == DSOURCE_TYPE_DBASE)
		pme->count = g_ds_GetRecordNum(pme->u.hdb);
	else if(pme->s_type == DSOURCE_TYPE_CUSTOM)
		pme->count = LISTSOURCE_GetCount(pme->u.s_custom.hds);
	
	dx = dy = 0;
	//2 如果未设置行高或列宽则须计算出来
	if(pme->row_height == 0 || pme->col_width == 0){
		for(index = 0; index < pme->count; index++){
			LISTSOURCE_Measure(ds, pme->viewtype, index, &width, &height);
			if(dx < width)
				dx = width;
			if(dy < height)
				dy = height;
		}
		if(dx == 0 || dy == 0){
			if(VIEWTYPE_IS_NORMAL(pme->viewtype)){
				dx = 100;
				dy = 20;
			}else{
				dx = 80;
				dy = 80;
			}
		}
		dx += pme->item_x0_margin+pme->item_x1_margin;
		dy += pme->item_y0_margin+pme->item_y1_margin;
		width = dx;
		height = dy;
		if(pme->row_height == 0){
			pme->row_height = pme->row_height_hilight = height;
		}
		if(pme->col_width == 0)
			pme->col_width = width;
	}
	//2 至此row_height和col_width都给出来了，可得出每页可容纳最大行列数
	max_width = lcd_get_width()-pme->common.border_width*2;
	max_height = lcd_get_height()-pme->common.border_width*2;
	if(max_height <= pme->row_height_hilight)
		row_max = 1;
	else
		row_max = 1+(max_height-pme->row_height_hilight)/pme->row_height;
	col_max = max_width/pme->col_width;
	
	if(pme->row_perpage_set && pme->col_perpage_set){
		//2 用户给定行列数，则根据每个item长宽，计算出能全覆盖的控件最小bound
		//2 以列为准，item显示从左至右从上至下
		row = ALIGN_DIV(pme->count, pme->col_perpage_set);
		if(row == 0){
			row = 1;
			col = 1;
		}else if(row == 1){
			col = pme->count;
		}else{
			col = pme->col_perpage_set;
			if(row > pme->row_perpage_set)
				row = pme->row_perpage_set;
		}
	}else if(pme->row_perpage_set){
		//2 指定每页行数，计算能全覆盖的控件高度，item显示从上至下从左至右
		col = ALIGN_DIV(pme->count, pme->row_perpage_set);
		if(col == 0){
			row = col = 1;
		}else if(col == 1){
			row = pme->count;
		}else{
			row = pme->row_perpage_set;
			if(col > col_max)
				col = col_max;
		}
	}else if(pme->col_perpage_set){
		//2 指定每页列数，计算能全覆盖的控件宽度，item显示从左至右从上至下
		row = ALIGN_DIV(pme->count, pme->col_perpage_set);
		if(row == 0){
			row = 1;
			col = 1;
		}else if(row == 1){
			col = pme->count;
		}else{
			col = pme->col_perpage_set;
			if(row > row_max)
				row = row_max;
		}
	}else{	//4 NEVER comes here
	}
	if(BIT_TEST(pme->flags, CLFLAG_AUTO_RESIZE_W)){
		pme->common.bound.dx = col * pme->col_width+pme->common.border_width*2;
	}
	if(BIT_TEST(pme->flags, CLFLAG_AUTO_RESIZE_H)){
		pme->common.bound.dy = row*pme->row_height+(pme->row_height_hilight-pme->row_height)+pme->common.border_width*2;
	}
	
}


gboolean	listctrl_rect_by_index(t_HList handle, int index, gui_rect_t *rect)
{
	gboolean ret = FALSE;
	t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
	if(g_object_valid(handle) && index >= 0 && index < pme->count && rect){
		int row, col;
		gui_rect_t client;

		ret = TRUE;
		LISTCTRL_SYNC_INDEX(pme, index, row, col);
		__listctrl_index_rect(pme, index, row, col, rect);
		
		wgt_get_client_rect(handle, &client);
		rect->x += handle->client_origin_x + client.x;
		rect->y += handle->client_origin_y + client.y;
	}
	return ret;
}

int		listctrl_index_by_point(t_HList handle, coord_t *px, coord_t *py, int *row, int *col)
{
	t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
	int index = -1;
	int r = -1;
	int c = -1;
	coord_t x0 = *px;
	coord_t y0 = *py;
	
	if(g_object_valid(handle) && pme->count > 0 && wgt_point_in_client(handle, &x0, &y0) && x0 >= 0 && y0 >= 0){
		if(LIST_VIEWTYPE_CUSTOM(handle)){
			//2 CUSTOM list
			c = 0;
			for(r = 0; r < pme->count; r++){
				if(pme->u.s_custom.pv[r].dy && y0 >= pme->u.s_custom.pv[r].y && y0 < pme->u.s_custom.pv[r].y+pme->u.s_custom.pv[r].dy)
					break;
			}
			index = r;
		}else{
			r = (y0)/pme->row_height;
			c = (x0)/pme->col_width;
			index = __listctrl_cell_index(pme, r, c);
			if(pme->col_n == 1 && pme->row_height != pme->row_height_hilight){
				if(pme->cursor >= 0 && index > pme->cursor){
					y0 -= pme->cursor*pme->row_height+pme->row_height_hilight; 
					if(y0 >= 0){
						r = index = pme->cursor + y0/pme->row_height;
						c = 0;
					}
				}
			}
		}
	}

	if(row) *row = r;
	if(col) *col = c;
	*px = x0;
	*py = y0;
	return index;
}

//2 关于list控件的auto size
/*
对于popup方式的list控件, 因为item数目的不确定, 控件width/height允许动态改变, x/y也可随意指定:
但是控件大小都有一个最大上限(dx_max, dy_max), 计算所得(dx, dy)如果小于(dx_max, dy_max), 则不须改变
否则在上限范围内计算合适的(dx, dy)

LINE view type: 
	row height用户指定, 获取每个item的width和height, 计算最合适的控件大小
CELL view type: 要求datasource所计算出来的width/height有上限, 比如限定最大80x80
	指定cell width/height: 获取每个item的width/height, 限定只有一列, 取最大width, 如果超过限制, 取上限
	指定cell row/col: dx=width*col, dy=height*row
*/

#define FTR_LISTCTRL_SCROLL_WITH_PEN_MOVE

#if defined(FTR_TOUCH_SCREEN) && defined(FTR_LISTCTRL_SCROLL_WITH_PEN_MOVE)
static void	__listctrl_slide_end(t_Widget *handle)
{
	t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
	int x = handle->client_origin_x;
	int y = handle->client_origin_y;
	coord_t delta;
	gui_rect_t client;
		
	wgt_get_client_rect(handle, &client);

	if(!BIT_TEST(pme->flags, CLFLAG_SCROLLING_HORIZONTAL)){
		//2 vertical scrolling
		if(LIST_VIEWTYPE_CUSTOM(pme)){
			//2 CUSTOM List, 默认处理
			t_PenParam lparam;
			lparam.type = MMI_PEN_SLIDE_END;
			g_widget_inherit_OnEvent(handle, WIDGET_OnPenEvent, sizeof(lparam), &lparam);
			return;
		}else{
			int page_dy;
			int dy; 
		
			page_dy = pme->row_perpage * pme->row_height;
			dy = pme->row_n * pme->row_height;
			if(pme->col_n == 1 && pme->row_height != pme->row_height_hilight){
				page_dy += pme->row_height_hilight-pme->row_height;
				dy += pme->row_height_hilight-pme->row_height;
			}
			
			if(y > 0) { y = 0; }
			else if(BIT_TEST(pme->flags, CLFLAG_SCROLLING_PAGE)){
				delta = (-y)%page_dy;
				y += delta;
				if(delta >= page_dy/2){
					y -= page_dy;
					if(-y >= dy)
						y += page_dy;
				}
				
			}else{
				delta = (-y)%pme->row_height;
				y += delta;
				if(delta >= pme->row_height/2)
					y -= pme->row_height;
				if(y+dy<client.dy)
					y = client.dy-dy;
			}
		}
	}else{
		//2 horizontal scrolling
		int page_dx = pme->col_perpage * pme->col_width;
		int dx = pme->col_n * pme->col_width;
	
		if(x > 0) { x = 0; }
		else if(BIT_TEST(pme->flags, CLFLAG_SCROLLING_PAGE)){
			delta = (-x)%page_dx;
			x += delta;
			if(delta >= page_dx/2){
				x -= page_dx;
				if(-x >= dx)
					x += page_dx;
			}
		}else{
			delta = (-x)%pme->col_width;
			x += delta;
			if(delta >= pme->col_width/2)
				x -= pme->col_width;
		}
	}
	if(x != handle->client_origin_x || y != handle->client_origin_y){
		handle->client_origin_x = x;
		handle->client_origin_y = y;
		wgt_redraw(handle, FALSE);
	}
}
#endif

static void	__listctrl_redraw(t_sparam defer, t_HWidget handle/*t_sparam sparam, t_lparam lparam*/)
{
	t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
	t_HGDI gdi;
	int index;
	
	if(pme->cursor < 0 && pme->count > 0){
		__listctrl_set_cursor(pme, 0, 0, 0, FALSE);
	}else if(pme->cursor >= pme->count && pme->count > 0){
		__listctrl_set_cursor(pme, pme->count-1, -1, -1, FALSE);
	}

	gdi = wgt_get_client_gdi(handle);
	if(!g_object_valid(gdi))return;

	if(pme->col_n == 1 || !(BIT_TEST(pme->flags, CLFLAG_SCROLLING_HORIZONTAL))){
		//2 如果是custom方式，则须先计算并保证可见item(y,dy)都计算出来，
		//2 否则逆序绘制会导致先绘制的位置不对
		if(LIST_VIEWTYPE_CUSTOM(pme)){
			gui_rect_t rect;
			__listctrl_index_visible(pme, pme->cursor);
			for(index = pme->cursor-1; index >= 0; index--){
				if(!__listctrl_index_visible(pme, index))
					break;
			}
			for(index = pme->cursor+1; index < pme->count; index++){
				if(!__listctrl_index_visible(pme, index))
					break;
			}
			//3 因为坐标可能有变，须保证hilight项在可视区域内
			__listctrl_index_rect(pme, pme->cursor, -1, -1, &rect);
			wgt_make_visible((t_HWidget)pme, (const gui_rect_t *)&rect);
		}
	}
	// TODO: 计算可见项，不可见不显示
	{
		gboolean flag = FALSE, tmp;
		for(index = 0; index < pme->count; index++){
#if defined(FTR_NO_INPUT_KEY)
			//2 全触屏项目不需要显示焦点项
			tmp = __listctrl_redraw_index(pme, gdi, index, -1, -1, defer, FALSE);
#else
			tmp = __listctrl_redraw_index(pme, gdi, index, -1, -1, defer, (index==pme->cursor)?TRUE:FALSE);
#endif
			//2 如果不加此判定，则会导致水平滚动时绘制不完全
			if(pme->col_n == 1 || !(BIT_TEST(pme->flags, CLFLAG_SCROLLING_HORIZONTAL))){
				if(tmp)flag = TRUE;
				else if(flag)break;
			}
		}
	}
	g_object_unref(gdi);

}

static error_t __listctrl_on_event(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
	t_HDataSource ds = pme->u.s_custom.hds;
	switch(evt){
		case WIDGET_AutoResize:
			if(BIT_TEST(pme->flags, CLFLAG_AUTO_RESIZE_W) || BIT_TEST(pme->flags, CLFLAG_AUTO_RESIZE_H))
				__listctrl_auto_resize(pme);
			ret = SUCCESS;
			break;
		case WIDGET_OnDraw:
			{
					
				t_HGDI gdi;
				
				__listctrl_calc_perpage(pme);
				wgt_disable_attr(handle, CTRL_ATTR_MODIFIED);
				gdi = wgt_get_gdi(handle);
				wgt_show_border(handle, gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
				g_object_unref(gdi);

				if(pme->flags&CLFLAG_ENABLE_ITEM_FSHOW)
					apc_call(TID_SELF, __listctrl_redraw, FALSE, pme);
				else
					__listctrl_redraw(TRUE, pme);
				
				ret = SUCCESS;
			}
			break;

		case WIDGET_OnSetFocus:
			if(pme->flags & CLFLAG_CURSOR_AUTOCHG_ONSETFOCUS){
				//2 必须考虑到切换焦点时条目是否enabled
				if(sparam == TAB_NEXT){
					int index;
					index = __listctrl_cell_index(pme, 0, pme->col_i);
					if(index >= 0 && index < pme->count && (pme->s_type != DSOURCE_TYPE_CUSTOM || LISTSOURCE_CanFocused(pme->u.s_custom.hds, index)))
						__listctrl_set_cursor(pme, -1, 0, pme->col_i, FALSE);
					else{
						pme->row_i = 0;
						pme->cursor = -1;
						LISTCTRL_SYNC_INDEX(pme, pme->cursor, pme->row_i, pme->col_i);
						__listctrl_next(pme, guiBottom);
					}
				}else if(sparam == TAB_PREV){
					int index, row_i;
					row_i = pme->row_n-1;
					while(1){
						index = __listctrl_cell_index(pme, row_i, pme->col_i);
						if(index >= 0 && index < pme->count){
							if(pme->s_type != DSOURCE_TYPE_CUSTOM || LISTSOURCE_CanFocused(pme->u.s_custom.hds, index))
								__listctrl_set_cursor(pme, index, row_i, pme->col_i, FALSE);
							else
								__listctrl_next(pme, guiTop);
							break;
						}
						row_i--;
						if(row_i < 0)
							break;
					}
				}
			}
			/*if(pme->flags & CLFLAG_CURSOR_NODRAW_ONKILLFOCUS)*/{
				t_HWindow win = wgt_parent_window(handle);
				if(wgt_is_focused(win) && !wgt_is_frozen(win))
				__listctrl_redraw_index(pme, NULL, pme->cursor, pme->row_i, pme->col_i, FALSE, TRUE);
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnKillFocus:
			if((pme->style != LISTCTRL_STYLE_RADIO) && (pme->style != LISTCTRL_STYLE_CHECK)){
				t_HWindow win = wgt_parent_window(handle);
				if(wgt_is_focused(win) && !wgt_is_frozen(win)){
#if defined(FTR_NO_INPUT_KEY)
					__listctrl_redraw_index(pme, NULL, pme->cursor, pme->row_i, pme->col_i, FALSE, FALSE);
#else
					if(pme->flags & CLFLAG_CURSOR_NODRAW_ONKILLFOCUS)
						__listctrl_redraw_index(pme, NULL, pme->cursor, pme->row_i, pme->col_i, FALSE, FALSE);
					else
						__listctrl_redraw_index(pme, NULL, pme->cursor, pme->row_i, pme->col_i, FALSE, TRUE);
#endif
				}
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnSelected:
			if(pme->count > pme->cursor && pme->cursor >= 0){
				if(pme->style == LISTCTRL_STYLE_RADIO){
					if(pme->sdata->i_selected != pme->cursor){
						int old = pme->sdata->i_selected;
						pme->sdata->i_selected = pme->cursor;
						if(old != -1)
							__listctrl_redraw_index(pme, NULL, old, -1, -1, FALSE, FALSE);
						__listctrl_redraw_index(pme, NULL, pme->cursor, -1, -1, FALSE, TRUE);
					}
				}else if(pme->style == LISTCTRL_STYLE_CHECK){
					if(pme->sdata->checked[pme->cursor]){
						pme->sdata->checked[pme->cursor] = FALSE;
						pme->sdata->n_selected--;
					}else{
						pme->sdata->checked[pme->cursor] = TRUE;
						pme->sdata->n_selected++;
					}
					__listctrl_redraw_index(pme, NULL, pme->cursor, -1, -1, FALSE, pme->sdata->checked[pme->cursor]);
				}
				if(pme->s_type == DSOURCE_TYPE_CUSTOM){
					ret = LISTSOURCE_HookEvent(pme->u.s_custom.hds, evt, pme->cursor, lparam);
					if(METH_HANDLED(ret))
						break;
				}else if(pme->s_type == DSOURCE_TYPE_DBASE){
					g_ds_ViewRecord(pme->u.hdb, pme->cursor);
					ret = SUCCESS;
					break;
				}
			}
			wgt_hook((t_Widget*)pme, evt, pme->cursor, 0);
			ret = SUCCESS;
			break;
			
		case WIDGET_OnKeyEvent:
#if !defined(FTR_NO_INPUT_KEY)
			switch(lparam){
				case MVK_UP:
					if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT)
					ret = __listctrl_next(pme, guiTop);
					break;
				case MVK_DOWN:
					if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT)
					ret = __listctrl_next(pme, guiBottom);
					break;
				case MVK_LEFT:
					if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
						if(pme->col_n > 1){
							ret = __listctrl_next(pme, guiLeft);
						}else if(wgt_is_popup(pme)){
							wgt_hide(handle);
							ret = SUCCESS;
						}else if(!BIT_TEST(pme->flags, CLFLAG_AUTOTAB_LR)){
							wgt_hook(handle, evt, sparam, lparam);
							ret = SUCCESS;
						}
					}
					break;
				case MVK_RIGHT:
					if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
						if(pme->col_n > 1){
							ret = __listctrl_next(pme, guiRight);
						}else if(!BIT_TEST(pme->flags, CLFLAG_AUTOTAB_LR)){
							wgt_hook(handle, evt, sparam, lparam);
							ret = SUCCESS;
						}
					}
					break;
				default:
					break;
			}
			if(ret == NOT_HANDLED && pme->s_type == DSOURCE_TYPE_CUSTOM){
				ret = LISTSOURCE_HookEvent(pme->u.s_custom.hds, evt, sparam, lparam);
			}
#endif
			break;

#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
		{
			coord_t x0, y0;
			PEN_XY(lparam, x0, y0);
			switch(PEN_TYPE(lparam)){
				case MMI_PEN_SLIDE_END:
					__listctrl_slide_end(handle);
					ret = SUCCESS;
					break;
				case MMI_PEN_PRESS:
					//2 PenPress就表示没有PenMove
					#if 0
					{
						int row, col;
						int index = listctrl_index_by_point(pme, &x0, &y0, &row, &col);
						if(index >= 0 && index < pme->count && index == pme->cursor)
							wgt_send_msg(handle, EVT_SELECT, 0, 0, 0);
							//g_widget_OnEvent(handle, WIDGET_OnSelected, 0, 0);
					}
					#endif
					//2 因为在pen_up事件中有处理，此处禁止转化为MVK_OK(PRESS)消息，
					//2 否则会导致在空白处点击也会进入焦点项
					wgt_hook(handle, evt, sparam, lparam);
					ret = SUCCESS;
					break;
				case MMI_PEN_LONG_PRESS:
					//2 PenLongPress就表示没有PenMove
					if(LIST_VIEWTYPE_CUSTOM(pme) && pme->s_type == DSOURCE_TYPE_CUSTOM){
						ret = LISTSOURCE_HookEvent(pme->u.s_custom.hds, evt, sparam, lparam);
						if(METH_HANDLED(ret))
							break;
					}
					break;
				case MMI_PEN_DOWN:
					wgt_set_focus(handle, TAB_NONE);
					{
						int row, col;
						int index = listctrl_index_by_point(pme, &x0, &y0, &row, &col);
						if(index >= 0 && index < pme->count && (pme->s_type != DSOURCE_TYPE_CUSTOM || LISTSOURCE_CanFocused(pme->u.s_custom.hds, index))){
							//2 在此加hook是考虑到某些场合用户需要截获消息
							#if 0
							if(LIST_VIEWTYPE_CUSTOM(pme) && pme->s_type == DSOURCE_TYPE_CUSTOM && (pme->flags&CLFLAG_HILIGHT_SHOW_CUSTOM)){
								ret = LISTSOURCE_HookEvent(pme->u.s_custom.hds, evt, sparam, lparam);
								if(METH_HANDLED(ret))
									break;
							}
							#endif
							ret = wgt_hook(handle, evt, sparam, lparam);
							if(METH_HANDLED(ret))
								break;
							__listctrl_set_cursor(pme, index, row, col, TRUE);
						}
					}
					ret = SUCCESS;
					break;
				case MMI_PEN_UP:
					ret = wgt_hook(handle, evt, sparam, lparam);
					if(!METH_HANDLED(ret)){
						//2  焦点未变，而且up就在焦点项上
						int index = listctrl_index_by_point(pme, &x0, &y0, NULL, NULL);
						if(index >= 0 && index < pme->count && index == pme->cursor){
#if defined(FTR_NO_INPUT_KEY)
							__listctrl_redraw_index(pme, NULL, index, -1, -1, FALSE, FALSE);
#endif
							wgt_send_msg(handle, EVT_SELECT, index, 0, 0);
							//g_widget_OnEvent(handle, WIDGET_OnSelected, index, 0);
							//2 listctrl可能是menu
							//__listctrl_on_event(handle, WIDGET_OnSelected, index, 0);
						}
					}
					ret = SUCCESS;
					break;
				case MMI_PEN_MOVE_IN:
					if(!BIT_TEST(pme->flags, CLFLAG_SCROLLABLE)){
						int index = listctrl_index_by_point(pme, &x0, &y0, NULL, NULL);
						if(index >= 0 && index < pme->count && index == pme->cursor){
							if(BIT_TEST(pme->flags, CLFLAG_PEN_MOVED)){
								//2 已经移出了焦点cell且重绘过，再次重绘焦点
								BIT_CLEAR(pme->flags, CLFLAG_PEN_MOVED);
								__listctrl_redraw_index(pme, NULL, pme->cursor, pme->row_i, pme->col_i, FALSE, TRUE);
							}
						}
					}
					ret = SUCCESS;
					break;
				case MMI_PEN_MOVE_OUT:
					if(!BIT_TEST(pme->flags, CLFLAG_SCROLLABLE)){
						if(pme->cursor >= 0 && pme->cursor < pme->count){
							if(BIT_TEST(pme->flags, CLFLAG_PEN_MOVED)){
								//2 已经移出了焦点cell且重绘过，什么都不做
							}else{
								//2 移出当前焦点cell，重绘
								BIT_SET(pme->flags, CLFLAG_PEN_MOVED);
								__listctrl_redraw_index(pme, NULL, pme->cursor, pme->row_i, pme->col_i, FALSE, FALSE);
							}
						}
					}
					ret = SUCCESS;
					break;
				case MMI_PEN_MOVE:
					ret = wgt_hook(handle, evt, sparam, lparam);
					if(!METH_HANDLED(ret)){
						int index = listctrl_index_by_point(pme, &x0, &y0, NULL, NULL);
						if(index >= 0 && index < pme->count){
							if(index != pme->cursor){
								if(BIT_TEST(pme->flags, CLFLAG_PEN_MOVED)){
									//2 已经移出了焦点cell且重绘过，什么都不做
								}else{
									//2 移出当前焦点cell，重绘
									BIT_SET(pme->flags, CLFLAG_PEN_MOVED);
									__listctrl_redraw_index(pme, NULL, pme->cursor, pme->row_i, pme->col_i, FALSE, FALSE);
								}
							}else{
								if(BIT_TEST(pme->flags, CLFLAG_PEN_MOVED)){
									//2 已经移出了焦点cell且重绘过，再次重绘焦点
									BIT_CLEAR(pme->flags, CLFLAG_PEN_MOVED);
									__listctrl_redraw_index(pme, NULL, pme->cursor, pme->row_i, pme->col_i, FALSE, TRUE);
								}
							}
						}else{
							//2 Move to <blank> items, treat as <move out>
							if(pme->cursor >= 0 && pme->cursor < pme->count && !BIT_TEST(pme->flags, CLFLAG_PEN_MOVED)){
								//2 移出当前焦点cell，重绘
								BIT_SET(pme->flags, CLFLAG_PEN_MOVED);
								__listctrl_redraw_index(pme, NULL, pme->cursor, pme->row_i, pme->col_i, FALSE, FALSE);
							}
						}
					}
					ret = SUCCESS;
					break;
			}
			break;
		}
#endif


	}
	if(ret == NOT_HANDLED){
		ret = g_widget_inherit_OnEvent(handle, evt, sparam, lparam);
	}
	return ret;
}


static VTBL(widget) listctrlVtbl = {
	(PFNONCREATE)__listctrl_on_create
	, (PFNONDESTROY)__listctrl_on_destroy

	, __listctrl_on_event
	, NULL
};

CLASS_DECLARE(listctrl, t_CommonListCtrl, listctrlVtbl);


void		listctrl_set_count(t_HList handle, int count)
{
	if(g_object_valid(handle) && count >= 0){
		t_CommonListCtrl *pme = (t_CommonListCtrl*)handle;
		pme->count = count;
		wgt_enable_attr(pme, CTRL_ATTR_MODIFIED);
		if(pme->col_n == 1){
			pme->col_perpage = 1;
			pme->row_n = pme->count;
			pme->col_n = 1;
			pme->col_i = 0;
		}
	}
}

int		listctrl_get_count(t_HList handle)
{
	return (g_object_valid(handle))?((t_CommonListCtrl*)handle)->count : 0;
}

void		listctrl_attach(t_HList handle, listctrl_redraw_func_t func, void *userdata)
{
	if(g_object_valid(handle) && func){
		t_CommonListCtrl *pme = (t_CommonListCtrl*)handle;

		listctrl_detach(pme);

		pme->s_type = DSOURCE_TYPE_NORMAL;
		pme->u.s_normal.func = func;
		pme->u.s_normal.userdata = userdata;
		wgt_enable_attr(pme, CTRL_ATTR_MODIFIED);
	}
}

void		listctrl_attach_db(t_HList handle, t_HDataSet hds)
{
	if(g_object_valid(handle) && g_object_valid(hds)){
		t_CommonListCtrl *pme = (t_CommonListCtrl*)handle;

		listctrl_detach(pme);

		pme->s_type = DSOURCE_TYPE_DBASE;
		pme->u.hdb = g_object_ref(hds);
		pme->count = g_ds_GetRecordNum(pme->u.hdb);

		wgt_enable_attr(pme, CTRL_ATTR_MODIFIED);
	}
}

void		listctrl_attach_datasource(t_HList handle, t_HDataSource dsource)
{
	if(g_object_valid(handle) && g_object_valid(dsource)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		listctrl_detach(pme);
		pme->s_type = DSOURCE_TYPE_CUSTOM;
		pme->u.s_custom.hds = g_object_ref(dsource);
		dsource->list = handle;
		pme->count = LISTSOURCE_GetCount(pme->u.s_custom.hds);
		wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
	}
}

t_HDataSource listctrl_get_datasource(t_HList handle)
{
	return (g_object_valid(handle) && !LISTCTRL_IS_NORMAL(handle)) ? ((t_CommonListCtrl*)handle)->u.s_custom.hds: NULL;
}

void		listctrl_refresh_datasource(t_HList handle, gboolean redraw)
{
	if(g_object_valid(handle) && !LISTCTRL_IS_NORMAL(handle)){
		t_CommonListCtrl * pme = (t_CommonListCtrl *)handle;
		if(pme->s_type != DSOURCE_TYPE_NORMAL){
			if(pme->s_type == DSOURCE_TYPE_CUSTOM){
				if(OBJECT_INTERFACE(pme->u.s_custom.hds, VTBL(datasource), Refresh))
					OBJECT_INTERFACE(pme->u.s_custom.hds, VTBL(datasource), Refresh)(pme->u.s_custom.hds);
				FREEIF(pme->u.s_custom.pv);
			}else if(pme->s_type == DSOURCE_TYPE_DBASE){
				//g_ds_Query(pme->dsource, TRUE);
			}
			wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
			__listctrl_calc_perpage(pme);
			wgt_disable_attr(handle, CTRL_ATTR_MODIFIED);
			if(redraw){
				if(pme->cursor < 0 || pme->cursor >= pme->count)
					listctrl_set_cursor(handle, 0, FALSE);
				wgt_redraw(handle, FALSE);
			}
		}
	}
}

int		listctrl_get_cursor	(t_HList handle)
{
	t_CommonListCtrl *pme = (t_CommonListCtrl*)handle;
	return (g_object_valid(handle) ? pme->cursor : -1);
}

int		listctrl_set_cursor	(t_HList handle, int idx, gboolean redraw)
{
	int old = -1;
	if(g_object_valid(handle) && /*idx >= 0 && */(wgt_test_attr(handle, CTRL_ATTR_MODIFIED) ||idx < ((t_CommonListCtrl *)handle)->count)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		old = pme->cursor;
		if(old != idx){
			__listctrl_set_cursor(pme, idx, -1, -1, redraw);
		}
	}
	return old;
}

void	listctrl_redraw_item(t_HList handle, int index)
{
	if(g_object_valid(handle)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		if(wgt_test_attr(pme, CTRL_ATTR_MODIFIED)){
			__listctrl_calc_perpage(pme);
			return;
		}
		if(index >= 0 && index < pme->count){
#if defined(FTR_NO_INPUT_KEY)
			__listctrl_redraw_index(pme, NULL, index, -1, -1, FALSE, FALSE);
#else
			if(index != pme->cursor || !wgt_is_focused(handle) && (pme->flags & CLFLAG_CURSOR_NODRAW_ONKILLFOCUS))
				__listctrl_redraw_index(pme, NULL, index, -1, -1, FALSE, FALSE);
			else
				__listctrl_redraw_index(pme, NULL, index, -1, -1, FALSE, TRUE);

#endif
		}
	}
}


int		listctrl_get_num_select(t_HList handle)
{
	int num = 0;
	if(g_object_valid(handle)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		if(pme->sdata){
			if(pme->style == LISTCTRL_STYLE_CHECK)
				num = pme->sdata->n_selected;
			else if(pme->style == LISTCTRL_STYLE_RADIO && pme->sdata->i_selected >= 0){
				num = 1;
			}
		}
	}
	return num;
}

int		listctrl_enum_select(t_HList handle, int current)
{
	int next = -1;
	t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
	if(g_object_valid(handle) && pme->sdata){
		if(pme->style == LISTCTRL_STYLE_CHECK){
			if(pme->sdata->n_selected > 0){
				if(current < 0)
					current = 0;
				else
					current++;
				while(current < pme->count){
					if(pme->sdata->checked[current]){
						next = current;
						break;
					}
					current++;
				}
			}
		}else if(pme->style == LISTCTRL_STYLE_RADIO && pme->sdata->i_selected >= 0){
			if(current < pme->sdata->i_selected)
				next = pme->sdata->i_selected;
		}
	}
	return next;
}

error_t	listctrl_set_select(t_HList handle, int idx, gboolean select)
{
	error_t ret = EBADPARM;
	t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
	if(g_object_valid(handle) && pme->sdata && idx >= 0 && idx < pme->count){
		if(pme->style == LISTCTRL_STYLE_CHECK){
			if(pme->sdata->checked[idx] != select){
				pme->sdata->checked[idx] = select;
				if(select)
					pme->sdata->n_selected++;
				else
					pme->sdata->n_selected--;
			}
		}else if(pme->style == LISTCTRL_STYLE_RADIO){
			pme->sdata->i_selected = idx;
		}
		ret = SUCCESS;
	}
	return ret;
}

int		listctrl_next(t_HList handle, int idx, gboolean redraw)
{
	int old = -1;
	if(g_object_valid(handle)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		old = pme->cursor;
		idx += old;
		if(old != idx && idx >= 0 && idx < pme->count){
			__listctrl_set_cursor(pme, idx, -1, -1, redraw);
		}
	}
	return old;
}

//2 设置item边框宽度
void		listctrl_set_item_margin(t_HList handle, gu16 margin_x0, gu16 margin_x1, gu16 margin_y0, gu16 margin_y1)
{
	if(g_object_valid(handle)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		pme->item_x0_margin = margin_x0;
		pme->item_x1_margin = margin_x1;
		pme->item_y0_margin = margin_y0;
		pme->item_y1_margin = margin_y1;
		wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
	}
}

void		listctrl_set_flags(t_HList handle, gu16 flags)
{
	if(g_object_valid(handle)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		pme->flags = (flags&CLFLAG_USER_MASK);
		wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
	}
}

gu16	listctrl_get_flags(t_HList handle)
{
	return (g_object_valid(handle))?((t_CommonListCtrl *)handle)->flags : 0;
}

t_GuiViewType	listctrl_get_viewtype(t_HList handle)
{
	return (g_object_valid(handle))?((t_CommonListCtrl *)handle)->viewtype : GUI_VIEWTYPE_NORMAL;
}

//2 默认为GUI_VIEWYTPE_TEXT, 如果指定Custom类型，每个item高宽须通过VTBL(datasource)->Measure接口获取
t_GuiViewType	listctrl_set_viewtype(t_HList handle, t_GuiViewType viewtype)
{
	t_GuiViewType old = GUI_VIEWTYPE_NORMAL;
	if(g_object_valid(handle)){
		old = ((t_CommonListCtrl *)handle)->viewtype;
		if(old != viewtype){
			((t_CommonListCtrl *)handle)->viewtype = viewtype;
			//2 防止切换viewtype时绘制有偏差
			wgt_client_origin_x(handle) = 0;
			wgt_client_origin_y(handle) = 0;
		}
	}
	return old;
}

/*
设置每页行数或行高来计算行数，高亮项高度可不同，默认rows=0,rheight=rheight_hilight=20
对非GUI_VIEWTYPE_CUSTOM类型，rows和row_height不能同为0
对GUI_VIEWTYPE_CUSTOM类型，此函数无效
*/
error_t	listctrl_set_rows(t_HList handle, int rows_perpage, int row_height, int row_height_hilight)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle) && !(rows_perpage == 0 && row_height == 0) ){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		if(row_height_hilight < row_height)
			row_height_hilight = row_height;

		pme->row_height = row_height;
		pme->row_height_hilight = row_height_hilight;
		pme->row_perpage_set = rows_perpage;

		wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
		ret = SUCCESS;
	}
	return ret;
}


/*
设置每页列数或列宽来计算列数，两者不可以同为0，默认cols=1, col_width=0(表示client_dx)
*/
error_t	listctrl_set_cols(t_HList handle, int cols_perpage, int col_width)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		
		pme->col_width = col_width;
		pme->col_perpage_set = cols_perpage;

		wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
		ret = SUCCESS;
	}
	return ret;
}

t_ListStyle	listctrl_get_style(t_HList handle)
{
	if(g_object_valid(handle)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		return pme->style;
	}
	return LISTCTRL_STYLE_NORMAL;
}
void	listctrl_set_style(t_HList handle, t_ListStyle style)
{
	if(g_object_valid(handle) && style < LISTCTRL_STYLE_N){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		if(pme->style != style){
			pme->style = style;
			if(pme->sdata){
				g_object_unref_0(pme->sdata->icons[0]);
				g_object_unref_0(pme->sdata->icons[1]);
				FREEIF(pme->sdata->checked);
				FREEIF(pme->sdata->selects);
			}
			if(style == LISTCTRL_STYLE_RADIO){
				if(pme->sdata == NULL){
					pme->sdata = MALLOC(sizeof(t_ListStyleData));
					memset(pme->sdata, 0, sizeof(t_ListStyleData));
				}
				pme->sdata->icons[0] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_ICON_RADIO, 0));
				pme->sdata->icons[1] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_ICON_RADIO, 1));
				pme->sdata->i_selected = -1;
			}else if(style == LISTCTRL_STYLE_CHECK){
				if(pme->sdata == NULL){
					pme->sdata = MALLOC(sizeof(t_ListStyleData));
					memset(pme->sdata, 0, sizeof(t_ListStyleData));
				}
				pme->sdata->icons[0] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_ICON_CHECKED, 0));
				pme->sdata->icons[1] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_ICON_CHECKED, 1));
				pme->sdata->n_selected = 0;
			}else{
				FREEIF(pme->sdata);
			}
			
			wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
		}
	}
}


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_listctrl;
}

#endif


#if 0
error_t 	listctrl_SetColWidth(t_HList handle, int col, int width)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle) && col >= 0 && col < LISTITEM_COLNUM_MAX){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		pme->list_colwidth[col] = width;
		ret = SUCCESS;
	}
	return ret;
}

error_t	listctrl_SetColTitle(t_HList handle, int col, BrdHandle brd, resid_t resid)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle) && col >= 0 && col < LISTITEM_COLNUM_MAX){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
		pme->list_title[col] = (char*)brd_get_string(brd, resid);
		if(pme->list_title[col])
			ret = SUCCESS;
	}
	return ret;
}

t_IconTextItem *	listctrl_NewItem(t_HList handle)
{
	t_IconTextItem *item = NULL;

	if(g_object_valid(handle)){
		g_idx_t idx = IDX_UNDEF;
		
		item = listitem_New();
		ITEM_ADD(handle, item);
		if(((t_CommonListCtrl *)handle)->cursor == IDX_UNDEF)
			((t_CommonListCtrl *)handle)->cursor = 0;
	}
	return item;
}

t_IconTextItem * listctrl_add_item(t_HList handle, const char *text, t_HImage image, g_tag_t id)
{
	t_IconTextItem * item = NULL;

	if(g_object_valid(handle)){
		item = listctrl_NewItem(handle);
		if(item){
			listitem_SetText(item, 0, text);
			listitem_SetImage(item, 0, image);
			listitem_SetTag(item, id);
			wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
		}
	}
	return item;
}


t_IconTextItem * listctrl_add_item2(t_HList handle, BrdHandle brdTxt, resid_t residTxt, BrdHandle brdImg, resid_t residImg, g_tag_t id)
{
	t_IconTextItem * item = NULL;
	if(g_object_valid(handle)){
		t_IconTextItem *item;
		
		item = listctrl_NewItem(handle);
		listitem_SetText2(item, 0, brdTxt, residTxt);
		listitem_SetImage2(item, 0, brdImg, residImg);
		listitem_SetTag(item, id);
		wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
	}
	return item;
}

error_t 	listctrl_delete_item	(t_HList handle, int idx)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle)){
		t_IconTextItem *item = g_vector_remove_idx(ITEM_ARRAY(handle), idx);
		listitem_Free(item);
		ret = SUCCESS;
	}
	return ret;
}

error_t 	listctrl_delete_all (t_HList handle)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle)){
		g_vector_reset(ITEM_ARRAY(handle), (GFreeFunc)listitem_Free);
		((t_CommonListCtrl *)handle)->cursor = IDX_UNDEF;
		wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
		ret = SUCCESS;
	}
	return ret;
}

int 	listctrl_get_item_count	(t_HList handle)
{
	return (g_object_valid(handle))?  ITEM_COUNT(handle) : 0;
}

t_IconTextItem *	listctrl_get_item(t_HList handle, g_idx_t idx)
{
	return (g_object_valid(handle))?ITEM_OF_IDX(handle, idx) : NULL;
}

int		listctrl_get_selnum(t_HList handle)
{
	int num = 0;
	if(g_object_valid(handle)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
	}
	return num;
}

int		listctrl_enum_sel(t_HList handle)
{
	int idx = -1;
	if(g_object_valid(handle)){
		t_CommonListCtrl *pme = (t_CommonListCtrl *)handle;
	}
	return idx;
}
#endif



#undef PARENT_CLASS


//2 listctrl for MimeService



void	gdi_draw_mime_method(t_HGDI gdi, t_MimeMethodInfo * info, int width, int height)
{
	if(info){
		t_HImage icon;
		char title[20];

		pack_GetModuleTitle(info->guid, info->module, title, sizeof(title));
		icon = pack_GetIcon(info->guid);
		
		gdi_draw_icon_text(gdi, icon, (const char *)title, width, height, guiRight, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
		
		g_object_unref_0(icon);
	}
}

