	
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
	
#define DLL_EXPORT_image

#include "wgt_image.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/


#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	t_HImage		image;
}t_ImageCtrl;


/*===========================================================================

Class Definitions

===========================================================================*/


static error_t		__image_OnCreate(t_ImageCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP | CTRL_ATTR_HSCROLLABLE | CTRL_ATTR_VSCROLLABLE);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	return SUCCESS;
}

static void	__image_OnDestroy(t_ImageCtrl* pme)
{
	g_object_inherit_destroy(pme);
}


static error_t __image_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_ImageCtrl *pme = (t_ImageCtrl *)handle;
	switch(evt){
		case WIDGET_OnDraw:
			{
#if 0
				gu32 dwFlags = IDF_HALIGN_CENTER | IDF_FILL_TRANSPARENT;
				t_HGDI gdi = wgt_get_gdi(handle);
				if(gdi){
					gui_rect_t rt;
					wgt_get_client_rect(handle, &rt);
					gdi_draw_image_in(gdi, 0, 0, rt.dx, rt.dy, handle->bg_image, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
					g_object_unref(gdi);
				}
#endif
				wgt_show_border(handle, NULL, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
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
			ret = wgt_hook((t_HWidget)pme, evt, sparam, lparam);
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
			coord_t x0, y0;
			PEN_XY(lparam, x0, y0);
			switch(PEN_TYPE(lparam)){
				case MMI_PEN_PRESS:
					//2 PenPress就表示没有PenMove和PenUp
					break;
				case MMI_PEN_LONG_PRESS:
					//2 PenLongPress就表示没有PenMove和PenUp
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


static VTBL(widget) imageVtbl = {
	(PFNONCREATE)__image_OnCreate
	, (PFNONDESTROY)__image_OnDestroy

	, __image_OnEvent
	, NULL
};

CLASS_DECLARE(imagectrl, t_ImageCtrl, imageVtbl);


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_imagectrl;
}

#endif

#undef PARENT_CLASS

// TODO: add functions for your own widget.  You MUST export them in dll.def

void	imagectrl_set(t_HWidget handle, t_HImage image)
{
	wgt_set_bg_image(handle, image);
}

void	imagectrl_set_by_resid(t_HWidget handle, BrdHandle brd, resid_t resid)
{
	t_HImage image = brd_get_image(brd, resid);
	if(g_object_valid(image)){
		wgt_set_bg_image(handle, image);
		g_object_unref(image);
	}
}

void	imagectrl_set_by_file(t_HWidget handle, const char *fname)
{
	t_HImage image = image_load_from_file(fname);
	if(g_object_valid(image)){
		wgt_set_bg_image(handle, image);
		g_object_unref(image);
	}
}


