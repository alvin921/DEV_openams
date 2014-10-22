	
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
	
#define DLL_EXPORT_tabctrl

#include "wgt_tabctrl.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/

//1 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)


#define TAB_MAXPAGE	16

typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	gs16		cursor;
	gu8		title_height;
	gu8		title_dir;
}t_TabCtrl;


#define	__SET_CURSOR(pme, ii)	do{ \
		if(((t_TabCtrl *)(pme))->cursor >= 0) \
		wgt_disable_attr(g_vector_element_at(((t_HWidget)(pme))->vect_child, ((t_TabCtrl *)(pme))->cursor), CTRL_ATTR_VISIBLE); \
		((t_TabCtrl *)(pme))->cursor = ii; \
		wgt_enable_attr(g_vector_element_at(((t_HWidget)(pme))->vect_child, ii), CTRL_ATTR_VISIBLE); \
		wgt_set_focus(g_vector_element_at(((t_HWidget)(pme))->vect_child, ii), TAB_NONE); \
		wgt_redraw((t_HWidget)(pme), FALSE); \
	}while(0)

/*===========================================================================

Class Definitions

===========================================================================*/


static error_t		__tabctrl_OnCreate(t_TabCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	//2 NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP|CTRL_ATTR_CONTAINER);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	pme->cursor = -1;
	pme->title_height = 20;
	pme->title_dir = guiBottom;
	return SUCCESS;
}

static void	__tabctrl_OnDestroy(t_TabCtrl* pme)
{
	g_object_inherit_destroy(pme);
}

static void	__tabctrl_GetTitleRect(t_HWidget handle, gui_rect_t *prc)
{
	t_TabCtrl *pme = (t_TabCtrl *)handle;
	prc->x = handle->border_width;
	prc->y = handle->border_width;
	prc->dx = wgt_bound_dx(handle) - (handle->border_width<<1);
	prc->dy = wgt_bound_dy(handle) - (handle->border_width<<1);
	if(pme->title_dir == guiTop || pme->title_dir == guiBottom){
		if(pme->title_dir ==  guiBottom)
			prc->y += prc->dy - pme->title_height;
		prc->dy = pme->title_height;
	}else if(pme->title_dir == guiLeft || pme->title_dir == guiRight){
		if(pme->title_dir ==  guiRight)
			prc->x += prc->dx - pme->title_height;
		prc->dx = pme->title_height;
	}
}

static void	__tabctrl_GetClientRect(t_HWidget handle, gui_rect_t *prc)
{
	t_TabCtrl *pme = (t_TabCtrl *)handle;
	prc->x = handle->border_width;
	prc->y = handle->border_width;
	prc->dx = wgt_bound_dx(handle) - (handle->border_width<<1);
	prc->dy = wgt_bound_dy(handle) - (handle->border_width<<1);
	if(pme->title_dir == guiTop || pme->title_dir == guiBottom){
		prc->dy -= pme->title_height;
		if(pme->title_dir ==  guiTop)
			prc->y += pme->title_height;
	}else if(pme->title_dir == guiLeft || pme->title_dir == guiRight){
		prc->dx -= pme->title_height;
		if(pme->title_dir ==  guiLeft)
			prc->x += pme->title_height;
	}
}


static error_t __tabctrl_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_TabCtrl *pme = (t_TabCtrl *)handle;

	switch(evt){
		case WIDGET_OnDraw:
			{
				gu32 dwFlags = IDF_HALIGN_CENTER | IDF_FILL_TRANSPARENT;
				t_HGDI gdi;
				gui_rect_t rt;
				int i;
				t_HImage icon;

				if(wgt_test_attr(handle, CTRL_ATTR_MODIFIED)){
					__tabctrl_GetClientRect(handle, &rt);
					for(i = 0; i < g_vector_size(handle->vect_child); i++){
						wgt_set_bound(g_vector_element_at(handle->vect_child, i), 0, 0, rt.dx, rt.dy);
						if(pme->cursor != i)
						wgt_disable_attr(g_vector_element_at(handle->vect_child, i), CTRL_ATTR_VISIBLE);
					}
					if(pme->cursor < 0 && g_vector_size(handle->vect_child))
						__SET_CURSOR(handle, 0);
				}
				gdi = wgt_get_gdi(handle);
				if(!g_object_valid(gdi))
					break;
				// TODO: do your own works
				//2 计算title区域，绘制背景
				__tabctrl_GetTitleRect(handle, &rt);
				if(pme->title_dir == guiTop || pme->title_dir == guiBottom){
					gdi_rectangle_fill_ex(gdi, rt.x, rt.y, rt.dx, rt.dy, MAKE_RGB(16,16,16), MAKE_RGB(0,0,0), GDI_FILL_MODE_2COLOR_H, 0);
					if(g_vector_size(handle->vect_child))
						rt.dx /= g_vector_size(handle->vect_child);
				}else{
					gdi_rectangle_fill_ex(gdi, rt.x, rt.y, rt.dx, rt.dy, MAKE_RGB(16,16,16), MAKE_RGB(0,0,0), GDI_FILL_MODE_2COLOR_V, 0);
					if(g_vector_size(handle->vect_child))
						rt.dy /= g_vector_size(handle->vect_child);
				}
				//2 绘制各个title
				for(i = 0; i < g_vector_size(handle->vect_child); i++){
					//2 绘制焦点背景
					if(pme->cursor == i){
						gdi_rectangle_fill_ex(gdi, rt.x+1, rt.y+1, rt.dx-2, rt.dy-2, MAKE_RGB(64,64,64), MAKE_RGB(32,32,32), GDI_FILL_MODE_2COLOR_H, guiFourCorner);
					}
					//2 绘制子控件caption
					gdi_save_context(gdi);
					gdi_translate(gdi, rt.x+1, rt.y+1);
					gdi_set_clip(gdi, 0, 0, rt.dx-2, rt.dy-2);
					gdi_move_to(gdi, 0, 0);

					icon = wgt_get_icon(g_vector_element_at(handle->vect_child, i));
					gdi_draw_icon_text(gdi, icon, wgt_get_caption(g_vector_element_at(handle->vect_child, i)), rt.dx-2, rt.dy-2, guiBottom, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
					g_object_unref(icon);
					gdi_restore_context(gdi);
					
					if(pme->title_dir == guiTop || pme->title_dir == guiBottom){
						rt.x += rt.dx;
					}else{
						rt.y += rt.dy;
					}
				}
				g_object_unref(gdi);

				if(pme->cursor >= 0) 
					wgt_redraw(g_vector_element_at(handle->vect_child, pme->cursor), TRUE);
				ret = SUCCESS;
			}
			break;

		case WIDGET_OnSetFocus:
			break;
		case WIDGET_OnKillFocus:
			break;
		case WIDGET_OnSelected:
			break;
			
		case WIDGET_OnKeyEvent:
			ret = wgt_hook(handle, evt, sparam, lparam);
			if(METH_HANDLED(ret))
				break;
			if(sparam != AMOS_KEY_PRESS && sparam != AMOS_KEY_REPEAT)
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
			coord_t x0, y0;
			PEN_XY(lparam, x0, y0);
			switch(PEN_TYPE(lparam)){
				case MMI_PEN_PRESS:
					//2 PenPress就表示没有PenMove
					break;
				case MMI_PEN_LONG_PRESS:
					//2 PenLongPress就表示没有PenMove
					break;
				case MMI_PEN_DOWN:
					{
						gui_rect_t rt;
						__tabctrl_GetTitleRect(handle, &rt);
						if(point_in_rect(x0, y0, &rt)){
							if(g_vector_size(handle->vect_child)){
								int cursor;
								if(pme->title_dir == guiTop || pme->title_dir == guiBottom){
									cursor = (x0-rt.x)*g_vector_size(handle->vect_child)/rt.dx;
								}else{
									cursor = (y0-rt.y)*g_vector_size(handle->vect_child)/rt.dy;
								}
								if(cursor != pme->cursor){
									__SET_CURSOR(handle, cursor);
								}
							}
							ret = SUCCESS;
						}
					}
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

static VTBL(widget) tabctrlVtbl = {
	(PFNONCREATE)__tabctrl_OnCreate
	, (PFNONDESTROY)__tabctrl_OnDestroy

	, __tabctrl_OnEvent
	, __tabctrl_GetClientRect
};

CLASS_DECLARE(tabctrl, t_TabCtrl, tabctrlVtbl);


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_tabctrl;
}

#endif

// TODO: add functions for your own widget.  You MUST export them in dll.def

int		tabctrl_get_cursor(t_HWidget handle)
{
	if(g_object_valid(handle) && g_object_class(handle) == &class_tabctrl){
		t_TabCtrl *pme = (t_TabCtrl *)handle;
		t_HWidget child = g_vector_element_at(handle->vect_child, pme->cursor);
		if(g_object_valid(child) && wgt_is_focused(child))
			return pme->cursor;
		{
			int i;
			for(i = 0; i < g_vector_size(handle->vect_child); i++){
				child = g_vector_element_at(handle->vect_child, i);
				if(wgt_is_focused(child)){
					pme->cursor = i;
					wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
					return i;
				}
			}
		}
	}
	return -1;
}
	
int		tabctrl_set_cursor(t_HWidget handle, int value)
{
	t_TabCtrl *pme = (t_TabCtrl *)handle;
	int old = -1;
	if(g_object_valid(pme) && g_object_class(handle) == &class_tabctrl && pme->cursor != value && value >= 0 && value < g_vector_size(handle->vect_child)){
		old = pme->cursor;
		__SET_CURSOR(pme, value);
	}
	return old;
}

void		tabctrl_set_title_style(t_HWidget handle, gu8 title_height, gu8 title_dir)
{
	t_TabCtrl *pme = (t_TabCtrl *)handle;
	if(g_object_valid(pme) && g_object_class(handle) == &class_tabctrl){
		pme->title_height = title_height;
		pme->title_dir = title_dir;
	}
}

#undef PARENT_CLASS

