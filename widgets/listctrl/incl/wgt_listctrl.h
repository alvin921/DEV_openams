#ifndef WGT_LISTCTRL_H
#define WGT_LISTCTRL_H

#include "ams.h"
#include "ds_icontext.h"

#define CLSID_LISTCTRL	"listctrl"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_LISTCTRL)
__declspec(dllimport) extern t_GClass	 class_listctrl;
#else
extern t_GClass class_listctrl;
#endif

#define LISTCTRL_ATTR_AUTOTAB		CTRL_ATTR_BITS_USER /*  */



enum {
	LISTCTRL_STYLE_NORMAL	/* MULTI_SEL automatically off */
	,LISTCTRL_STYLE_NUMBERED	/* number displayed in the front of each item and MULTI_SEL automatically off */
	,LISTCTRL_STYLE_CHECK		/* check box will be display in the front of item and MULTI_SEL automatically on */
	,LISTCTRL_STYLE_RADIO		/* radio box displayed in the front of each item and MULTI_SEL automatically off */
	
	,LISTCTRL_STYLE_N
};
typedef gu8	t_ListStyle;

#define	CLFLAG_AUTOTAB_UD			BIT_0	//2 UP/DOWN键切换焦点
#define	CLFLAG_AUTOTAB_LR				BIT_1	//2LEFT/RIGHT键切换焦点

#define	CLFLAG_SCROLLING_ITEM			0
#define	CLFLAG_SCROLLING_PAGE		BIT_2

#define	CLFLAG_SCROLLING_VERTICAL	0
#define	CLFLAG_SCROLLING_HORIZONTAL	BIT_3

#define	CLFLAG_CURSOR_AUTOCHG_ONSETFOCUS	BIT_4	//2 获得焦点时根据按键UP/DOWN方向动态设置hilight项, 达到多个控件间条目循环滚动的效果
#define	CLFLAG_CURSOR_NODRAW_ONKILLFOCUS	BIT_5	//2 控件失去焦点时 当前条目不hilight不高亮显示

#define	CLFLAG_HILIGHT_SHOW_MASK	BIT_6	//2 cell中图标高亮显示
#define	CLFLAG_HILIGHT_SHOW_BG		0		//2 默认显示cell高亮背景
#define	CLFLAG_HILIGHT_SHOW_CUSTOM	BIT_7

#define	CLFLAG_DISABLE_HLINE			BIT_8
#define	CLFLAG_DISABLE_VLINE			BIT_9

#define	CLFLAG_AUTO_RESIZE_W			BIT_10
#define	CLFLAG_AUTO_RESIZE_H			BIT_11

#define	CLFLAG_AUTO_RESIZE			(CLFLAG_AUTO_RESIZE_W|CLFLAG_AUTO_RESIZE_H)

//2 对于相册/短信/通信录等大数据量数据源，允许边加载边显示
#define	CLFLAG_ENABLE_ITEM_FSHOW	BIT_12

#define	CLFLAG_USER_MASK	(BIT_0|BIT_1|BIT_2|BIT_3|BIT_4|BIT_5|BIT_6|BIT_7|BIT_8|BIT_9|BIT_10|BIT_11|BIT_12)

typedef void	(*listctrl_redraw_func_t)(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, void *userdata);
enum {
	DSOURCE_TYPE_NORMAL,
	DSOURCE_TYPE_DBASE,
	DSOURCE_TYPE_CUSTOM,
};

typedef struct {
	coord_t	y;
	gu16	dy;
}gui_v_t;

typedef struct {
	t_HImage		icons[2];
	gs16		i_selected;	// for RADIO style
	//2 for CHECKED style
	gu16	n_selected;
	gs16		*selects;	// for CHECKED style, NULL for RADIO style
	gu8		*checked;	// TRUE: selected, FALSE: unselected, NULL for RADIO style
}t_ListStyleData;

typedef struct tagCommonListCtrl{
	t_Widget		common;

	union {
		struct {
			listctrl_redraw_func_t	func;
			void	*userdata;
		}s_normal;
		t_HDataSet 		hdb;
		struct {
			t_HDataSource	hds;
			gui_v_t	*pv;		//2 height[] of all items
		}s_custom;
	}u;

	t_ListStyleData *	sdata;
	
	gu32			flags;
	t_GuiViewType	viewtype;
	t_ListStyle		style;
	gu8 			move_dir;
	gu8			s_type;

	gu8		item_x0_margin;
	gu8		item_x1_margin;
	gu8		item_y0_margin;
	gu8		item_y1_margin;

	//4 如下两项是用户设置的值，如果为0，真正的row_perpage和col_perpage需要计算得出，而且xxx_set会对控件显示有影响，因此必须保留设置值
	gu16	row_perpage_set;
	gu16	col_perpage_set;

	gu16	row_perpage;	// vscroll: rows perpage
	gu16	row_height;	// client_rect_dy/cell_dy is rows_perpage
	gu16	row_height_hilight;
	MC_FILL_STRUCT2;
	gu16	col_perpage;
	gu16	col_width;	// for list view, is client_dx

	gs16		row_n;	// total rows
	gs16		col_n;	// total cols
	
	int		row_i;	// cursor row
	int		col_i;	// cursor col, always 0 for listview

	int		count;	// total item count, get from datasource
	int		cursor;

}t_CommonListCtrl;

#include "dbase.h"
typedef void	(*dbrecord_redraw_func_t)(t_HList listctrl, t_HDataSet hdb, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight);

BEGIN_DECLARATION

void		listctrl_set_count(t_HList handle, int count);
int		listctrl_get_count(t_HList handle);
void		listctrl_attach(t_HList handle, listctrl_redraw_func_t func, void *userdata);
int		listctrl_next(t_HList handle, int idx, gboolean redraw);
void		listctrl_attach_db(t_HList handle, t_HDataSet hds);
void		listctrl_attach_datasource(t_HList handle, t_HDataSource dsource);
t_HDataSource listctrl_get_datasource(t_HList handle);
void		listctrl_refresh_datasource(t_HList handle, gboolean redraw);
int		listctrl_index_by_point(t_HList handle, coord_t *px, coord_t *py, int *row, int *col);
gboolean	listctrl_rect_by_index(t_HList handle, int index, gui_rect_t *rect);
int		listctrl_get_cursor	(t_HList handle);
int		listctrl_set_cursor	(t_HList handle, int idx, gboolean redraw);

error_t	listctrl_set_select(t_HList handle, int idx, gboolean select);
int		listctrl_get_num_select(t_HList handle);
int		listctrl_enum_select(t_HList handle, int current);

void		listctrl_set_flags(t_HList handle, gu16 flags);
gu16	listctrl_get_flags(t_HList handle);

t_GuiViewType	listctrl_set_viewtype(t_HList handle, t_GuiViewType viewtype);
/*
设置每页行数或行高来计算行数，高亮项高度可不同，默认rows=0,rheight=rheight_hilight=20
对非GUI_VIEWTYPE_CUSTOM类型，rows和row_height不能同为0
对GUI_VIEWTYPE_CUSTOM类型，此函数无效
*/
error_t	listctrl_set_rows(t_HList handle, int rows_perpage, int row_height, int row_height_hilight);

/*
设置每页列数或列宽来计算列数，两者不可以同为0，默认cols=1, col_width=0(表示client_dx)
*/
	
error_t 	listctrl_set_cols(t_HList handle, int cols_perpage, int col_width);
//2 设置item边框宽度
void		listctrl_set_item_margin(t_HList handle, gu16 margin_x0, gu16 margin_x1, gu16 margin_y0, gu16 margin_y1);
t_GuiViewType	listctrl_get_viewtype(t_HList handle);
t_ListStyle	listctrl_get_style(t_HList handle);
void		listctrl_set_style(t_HList handle, t_ListStyle style);
void	listctrl_redraw_item(t_HList handle, int index);
void	listctrl_show_item_in(t_CommonListCtrl *pme, int index, t_HGDI gdi, coord_t x, coord_t y, gu16 dx, gu16 dy, gboolean defer, gboolean selected);

void	gdi_draw_mime_method(t_HGDI gdi, t_MimeMethodInfo * info, int width, int height);


END_DECLARATION

#endif	/* WGT_LISTCTRL_H */

