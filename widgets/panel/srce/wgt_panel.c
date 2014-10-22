	
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
	
#define DLL_EXPORT_panel

#include "wgt_panel.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/

//1 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	int		member1;
}t_PanelCtrl;


/*===========================================================================

Class Definitions

===========================================================================*/


static error_t		__panel_OnCreate(t_PanelCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	//2 NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP|CTRL_ATTR_CONTAINER);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	pme->member1 = 10;
	return SUCCESS;
}

static void	__panel_OnDestroy(t_PanelCtrl* pme)
{
	g_object_inherit_destroy(pme);
}


static error_t __panel_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_PanelCtrl *pme = (t_PanelCtrl *)handle;

	switch(evt){
		case WIDGET_OnDraw:
			{
				t_HGDI gdi = wgt_get_gdi(handle);

				//2 draw children widgets
				container_redraw(handle, TRUE);

				/* Do the user specified redraw */
				ret = wgt_hook(handle, evt, sparam, lparam);

				wgt_show_border(handle, gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
				g_object_unref(gdi);
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


static VTBL(widget) panelVtbl = {
	(PFNONCREATE)__panel_OnCreate
	, (PFNONDESTROY)__panel_OnDestroy

	, __panel_OnEvent
};

CLASS_DECLARE(panel, t_PanelCtrl, panelVtbl);


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_panel;
}

#endif

// TODO: add functions for your own widget.  You MUST export them in dll.def

int		panel_get_member(t_HWidget handle)
{
	t_PanelCtrl *pme = (t_PanelCtrl *)handle;
	return pme? pme->member1 : -1;
}

int		panel_set_member(t_HWidget handle, int value)
{
	t_PanelCtrl *pme = (t_PanelCtrl *)handle;
	int old = -1;
	if(pme){
		old = pme->member1;
		pme->member1 = value;
	}
	return old;
}

#undef PARENT_CLASS

