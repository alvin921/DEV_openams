	
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
	
#define DLL_EXPORT_onoff

#include "wgt_onoff.h"
#include "sys_resid.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/

//1 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

//2 如果ICON_ONOFF存在，则type为IMAGE，如果ICON_CHECKED存在，则type为CHECK，否则自行绘制
enum {
	ONOFF_NONE,
	ONOFF_CHECK,
	ONOFF_IMAGE,
};
typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	t_HImage	icons[3];
	gu8		type;
	gu8		n_icons;
	gu8		value;
}t_OnoffCtrl;


/*===========================================================================

Class Definitions

===========================================================================*/


static error_t		__onoff_OnCreate(t_OnoffCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	//2NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	if(brd_get_media_num(BRD_HANDLE_SELF, RESID_ICON_ONOFF) == 3){
		pme->type = ONOFF_IMAGE;
		pme->n_icons = 3;
		pme->icons[0] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_ICON_ONOFF, 0));
		pme->icons[1] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_ICON_ONOFF, 1));
		pme->icons[2] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_ICON_ONOFF, 2));
	}else if(brd_get_media_num(BRD_HANDLE_SELF, RESID_ICON_CHECKED) == 2){
		pme->type = ONOFF_CHECK;
		pme->n_icons = 2;
		pme->icons[0] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_ICON_CHECKED, 0));
		pme->icons[1] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_ICON_CHECKED, 1));
	}else{
		pme->type = ONOFF_NONE;
	}
	pme->value = 0;
	return SUCCESS;
}

static void	__onoff_OnDestroy(t_OnoffCtrl* pme)
{
	int i;
	for(i = 0; i < pme->n_icons; i++)
		g_object_unref(pme->icons[i]);

	g_object_inherit_destroy(pme);
}

#define X_OFFSET	5

static error_t __onoff_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_OnoffCtrl *pme = (t_OnoffCtrl *)handle;

	switch(evt){
		case WIDGET_OnDraw:
			{
				gu32 dwFlags = IDF_HALIGN_CENTER | IDF_FILL_TRANSPARENT;
				t_HGDI gdi;
				coord_t x,y;
				t_ColorRasterOp rop;
				t_UITheme *theme = ui_theme_get();
				
				gdi = wgt_get_gdi(handle);
				if(!g_object_valid(gdi))
					break;
				
				// TODO: do your own works
#if defined(FTR_NO_INPUT_KEY)
				gdi_set_color(gdi, theme->fg_color);
#else
				if(wgt_is_focused(handle)){
					//2 show bg
					gdi_set_color(gdi, theme->hilight_bg_color);
					gdi_rectangle_fill(gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle), handle->border_corner);
					gdi_set_color(gdi, theme->fg_color);
				}
				else
					gdi_set_color(gdi, theme->hilight_fg_color);
#endif
				//2 show caption
				//rop = gdi_set_rop(gdi, IMG_RO_NOT);
				gdi_draw_text_in(gdi, X_OFFSET, 0, wgt_bound_dx(handle), wgt_bound_dy(handle), wgt_get_caption(handle), -1, CHARSET_UTF8, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
				//gdi_set_rop(gdi, rop);
				//2 show onoff bg
				x = wgt_bound_dx(handle)-image_get_width(pme->icons[0])-X_OFFSET;
				y = (wgt_bound_dy(handle)-image_get_height(pme->icons[0]))/2;
				gdi_draw_image_at(gdi, x, y, pme->icons[0], 0, 0, -1, -1, 1.0, 1.0);
				//2 show onoff state&slider
				if(pme->value){
					x += image_get_width(pme->icons[0])-image_get_width(pme->icons[2]);
					gdi_draw_image_at(gdi, x, y, pme->icons[2], 0, 0, -1, -1, 1.0, 1.0);
				}else{
					gdi_draw_image_at(gdi, x+image_get_width(pme->icons[2])/2, y, pme->icons[1], image_get_width(pme->icons[1])/2, 0, image_get_width(pme->icons[1])/2, -1, 1.0, 1.0);
					gdi_draw_image_at(gdi, x, y, pme->icons[2], 0, 0, -1, -1, 1.0, 1.0);
				}
				wgt_show_border(handle, gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
				
				g_object_unref(gdi);
				
				ret = SUCCESS;
			}
			break;

		case WIDGET_OnSetFocus:
			wgt_redraw(handle, FALSE);
			break;
		case WIDGET_OnKillFocus:
			wgt_redraw(handle, FALSE);
			break;
		case WIDGET_OnSelected:
			if(pme->value)
				pme->value = FALSE;
			else
				pme->value = TRUE;
			wgt_redraw(handle, FALSE);
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
			switch(sparam){
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

static void	__onoff_GetClientRect(t_HWidget handle, gui_rect_t *prc)
{
	prc->x = handle->border_width;
	prc->y = handle->border_width;
	prc->dx = wgt_bound_dx(handle) - (handle->border_width<<1);
	prc->dy = wgt_bound_dy(handle) - (handle->border_width<<1);
}


static VTBL(widget) onoffVtbl = {
	(PFNONCREATE)__onoff_OnCreate
	, (PFNONDESTROY)__onoff_OnDestroy

	, __onoff_OnEvent
	, __onoff_GetClientRect
};

CLASS_DECLARE(onoff, t_OnoffCtrl, onoffVtbl);


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_onoff;
}

#endif

// TODO: add functions for your own widget.  You MUST export them in dll.def

int		onoff_get_value(t_HWidget handle)
{
	t_OnoffCtrl *pme = (t_OnoffCtrl *)handle;
	return pme? pme->value : 0;
}

int		onoff_set_value(t_HWidget handle, int value)
{
	t_OnoffCtrl *pme = (t_OnoffCtrl *)handle;
	int old = -1;
	value = !!value;
	if(pme){
		old = pme->value;
		pme->value = value;
		if(old != value)
			wgt_redraw(handle, FALSE);
	}
	return old;
}

#undef PARENT_CLASS

