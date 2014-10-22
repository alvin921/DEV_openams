	
/*=============================================================================
	
$DateTime: 2010/06/06 16:41:00 $
$Author: Wang MinGang $

					  EDIT HISTORY FOR FILE
	
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when		 who	 what, where, why
----------	 ---	 ----------------------------------------------------------
=============================================================================*/

/*
【ComBox设计分析】
1. ComboBox会自动创建一个listctrl（通过listctrl用户可加载数据），但两者不是parent-child的关系
2. listctrl跟combobox同宽，允许设置listctrl高度
3. combobox在normal状态显示listctrl hilight项内容
4. combobox在dropdown状态popup listctrl, 方向是向下，如果超出屏幕边界，则方向朝上
*/


/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
	
#define DLL_EXPORT_combobox

#include "wgt_combobox.h"
#include "wgt_listctrl.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/

//1 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)


#define	COMBO_ATTR_DROPDOWN		BIT_31
typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	t_HList	list;
	gs16		list_height;
	gs16		list_cursor;
}t_ComboBox;


/*===========================================================================

Class Definitions

===========================================================================*/



static error_t __listctrl_on_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_ComboBox *combo = (t_ComboBox*)wgt_get_tag(handle);
	switch(evt){
		case WIDGET_OnSelected:
			wgt_hide(handle);
			if(sparam != combo->list_cursor){
				combo->list_cursor = sparam;
				wgt_redraw(combo, FALSE);
				ret = wgt_hook((t_HWidget)combo, WIDGET_OnSelChanged, combo->list_cursor, sparam);
			}
			ret = SUCCESS;
			break;
		default:
			break;
	}
	return ret;
}

static error_t		__combobox_OnCreate(t_ComboBox* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	//2NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	pme->list = wgt_create_instance("listctrl", NULL);
	wgt_disable_attr(pme->list, CTRL_ATTR_VISIBLE);
	listctrl_set_viewtype((t_HList)pme->list, GUI_VIEWTYPE_NORMAL);
	listctrl_set_rows((t_HList)pme->list, 0, prod_ui_list_item_height(), 0);
	listctrl_set_cols((t_HList)pme->list, 1, 0);
	listctrl_set_flags((t_HList)pme->list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM|CLFLAG_AUTO_RESIZE_H);
	wgt_set_tag(pme->list, pme);
	wgt_hook_register(pme->list, WIDGET_OnSelected, __listctrl_on_hook, NULL);
	//listctrl_set_item_margin((t_HList)pme->list, 8, 8, 4, 4);

	pme->list_height = 60;
	pme->list_cursor = -1;
	combobox_set_cursor((t_HWidget)pme, 0);
	return SUCCESS;
}

static void	__combobox_OnDestroy(t_ComboBox* pme)
{
	g_object_unref(pme->list);
	g_object_inherit_destroy(pme);
}

static error_t __combobox_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_ComboBox *pme = (t_ComboBox *)handle;

	switch(evt){
		case WIDGET_OnDraw:
			{
				gu32 dwFlags = IDF_HALIGN_CENTER | IDF_FILL_TRANSPARENT;
				t_HGDI gdi;
				gui_rect_t rect;
				gu16 a_w, a_h;
				
#define	ARROW_WIDTH	10				
				gdi = wgt_get_client_gdi(handle);
				if(!g_object_valid(gdi))
					break;
				// TODO: do your own works
				//2 计算listctrl控件大小
				
				wgt_get_client_rect(handle, &rect);

				if(wgt_is_focused(handle))
					gdi_style_show_hilight_bg(gdi, 0, 0, rect.dx, rect.dy, guiNoCorner, TRUE);
				else
				//2 绘制边框
				wgt_show_border(handle, NULL, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
				
				gdi_set_color(gdi, ui_theme_get()->fg_color);
				
				//2 绘制右边的dropdown按钮
				a_w = ARROW_WIDTH-2;
				a_h = rect.dy-2;
				if(a_h > a_w)
					a_h = a_w;
				gdi_move_to(gdi, rect.dx-ARROW_WIDTH+1, (rect.dy-a_h)/2);
				gdi_line_to(gdi, rect.dx-ARROW_WIDTH/2, (rect.dy+a_h)/2);
				gdi_line_to(gdi, rect.dx-1, (rect.dy-a_h)/2);
				gdi_line_to(gdi, rect.dx-ARROW_WIDTH+1, (rect.dy-a_h)/2);
				
				//2 绘制内容
				listctrl_show_item_in(pme->list, pme->list_cursor, gdi, 0, 0, rect.dx-ARROW_WIDTH, rect.dy, TRUE, wgt_is_focused(pme));

				g_object_unref(gdi);
				
				ret = SUCCESS;
			}
			break;

		case WIDGET_OnSetFocus:
		case WIDGET_OnKillFocus:
			wgt_redraw(handle, FALSE);
			break;
		case WIDGET_OnSelected:
			if(wgt_is_visible(pme->list)){
				wgt_hide(pme->list);
				wgt_disable_attr(handle, COMBO_ATTR_DROPDOWN);
			}else{
				wgt_set_bound(pme->list, 0, 0, wgt_bound_dx(handle), 40);
				wgt_popup(handle, pme->list, 0, wgt_bound_dy(handle), guiBottom|guiRight);
				wgt_enable_attr(handle, COMBO_ATTR_DROPDOWN);
				listctrl_set_cursor(pme->list, pme->list_cursor, TRUE);
			}
			break;
			
		case WIDGET_OnKeyEvent:
			ret = wgt_hook(handle, evt, sparam, lparam);
			if(METH_HANDLED(ret))
				break;
			if(sparam != MMI_KEY_PRESS && sparam != MMI_KEY_REPEAT)
				break;
			switch(lparam){
				case MVK_UP:
					break;
				case MVK_DOWN:
					break;
				case MVK_LEFT:
					break;
				case MVK_RIGHT:
					break;
				default:
					break;
			}
			break;

#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
		{
			coord_t x, y;
			PEN_XY(lparam, x, y);
			switch(PEN_TYPE(lparam)){
				case MMI_PEN_PRESS:
					//2 PenPress就表示没有PenMove
					break;
				case MMI_PEN_REPEAT:
					break;
				case MMI_PEN_LONG_PRESS:
					//2 PenLongPress就表示没有PenMove
					break;
				case MMI_PEN_DOWN:
					break;
				case MMI_PEN_UP:
					break;
				case MMI_PEN_MOVE_IN:
					break;
				case MMI_PEN_MOVE_OUT:
					break;
				case MMI_PEN_MOVE:
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

static void	__combobox_GetClientRect(t_HWidget handle, gui_rect_t *prc)
{
	prc->x = handle->border_width;
	prc->y = handle->border_width;
	prc->dx = wgt_bound_dx(handle) - (handle->border_width<<1);
	prc->dy = wgt_bound_dy(handle) - (handle->border_width<<1);
}


static VTBL(widget) comboboxVtbl = {
	(PFNONCREATE)__combobox_OnCreate
	, (PFNONDESTROY)__combobox_OnDestroy

	, __combobox_OnEvent
	, __combobox_GetClientRect
};

CLASS_DECLARE(combobox, t_ComboBox, comboboxVtbl);


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_combobox;
}

#endif

// TODO: add functions for your own widget.  You MUST export them in dll.def

int		combobox_get_cursor(t_HWidget handle)
{
	t_ComboBox *pme = (t_ComboBox *)handle;
	return pme? pme->list_cursor : -1;
}

int		combobox_set_cursor(t_HWidget handle, int value)
{
	t_ComboBox *pme = (t_ComboBox *)handle;
	int old = -1;
	if(pme){
		old = pme->list_cursor;
		pme->list_cursor = value;
		listctrl_set_cursor(pme->list, value, TRUE);
	}
	return old;
}

t_HList	combobox_get_listctrl(t_HWidget handle)
{
	t_ComboBox *pme = (t_ComboBox *)handle;
	return pme? pme->list : NULL;
}


#undef PARENT_CLASS

