	
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
	
#define DLL_EXPORT_pushbutton

#include "wgt_pushbutton.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/

//1 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	t_HWidget	popup;
}t_CPushButton;

#if !defined(FTR_NO_INPUT_KEY)
#define	BTN_ATTR_DOWN		BIT_30
#endif

/*===========================================================================

Class Definitions

===========================================================================*/


static error_t		__pushbutton_OnCreate(t_CPushButton* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	//2NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	pme->popup = NULL;
	return SUCCESS;
}

static void	__pushbutton_OnDestroy(t_CPushButton* pme)
{
	g_object_inherit_destroy(pme);
}

#define MARGIN	2
#define DOWN_M	1
static void __button_redraw(t_HWidget h, gboolean defer)
{
	t_CPushButton *pme = (t_CPushButton *)h;
	t_HGDI gdi = NULL;
	const char *text = brd_get_string(h->brd_text, h->resid_text, (const char*)h->title);
	t_HImage icon;
	gui_rect_t client;
	gboolean down = FALSE;

#if !defined(FTR_NO_INPUT_KEY)
	if(wgt_test_attr(h, BTN_ATTR_DOWN))
		down = TRUE;
#endif
	gdi = wgt_get_gdi(h);
	if(!gdi)
		return;

	icon = h->icon;

	if(!g_object_valid(icon))
		icon = brd_get_image(h->brd_icon, h->resid_icon);

	wgt_show_border(h, gdi, 0, 0, wgt_bound_dx(h), wgt_bound_dy(h));
	
	wgt_get_client_rect(h, &client);
	gdi_translate(gdi, client.x, client.y);
	gdi_set_color(gdi, ui_theme_get()->fg_color);
	if(text == NULL || *text == 0){
		if(g_object_valid(icon))
			gdi_draw_image_in(gdi, down?DOWN_M:0, (down?DOWN_M:0), client.dx-(down?DOWN_M:0), client.dy-(down?DOWN_M:0), icon, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
	}else if(!g_object_valid(icon)){
		//gdi_set_fontsize(gdi, 1.0);
		gdi_draw_text_in(gdi, down?DOWN_M:0, down?DOWN_M:0, client.dx-(down?DOWN_M:0), client.dy-(down?DOWN_M:0), text, -1, CHARSET_UTF8, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE);
		//gdi_set_fontsize(gdi, 1.0);
	}else{
		gu8 dir = h->text_dir;
		int tw, th;
		int iw, ih;
		coord_t ix, iy, tx, ty;
		gu16 idx, idy, tdx, tdy;
		
		iw = image_get_width(icon);
		ih = image_get_height(icon);
		font_measure_string(text, -1, CHARSET_UTF8, -1, &tw, &th, NULL);

		if(dir == guiTop || dir == guiBottom){
			if(th > client.dy){
				//2 不显示文字, 只显示图片
				tx = ty = tdx = tdy = 0;
				ix = down?DOWN_M:0;
				iy = down?DOWN_M:0;
				idx = client.dx-(down?DOWN_M:0);
				idy = client.dy-(down?DOWN_M:0);
			}else if(th + ih > client.dy){
				//2 显示文字, 图片缩小显示
				tx = ix = down?DOWN_M:0;
				tdx = idx = client.dx-(down?DOWN_M:0);
				tdy = th;
				idy = client.dy-th-(down?DOWN_M:0);
				
				if(dir == guiTop){
					ty = down?DOWN_M:0;
					iy = th+(down?DOWN_M:0);
				}else{
					iy = (down?DOWN_M:0);
					ty = client.dy-th+(down?DOWN_M:0);
				}
			}else{
				//2 文字和图片都居中显示
				tx = ix = down?DOWN_M:0;
				tdx = idx = client.dx-(down?DOWN_M:0);
				tdy = th;
				idy = ih;
				
				if(dir == guiTop){
					ty = (client.dy-th-ih)/2+(down?DOWN_M:0);
					iy = ty+tdy;
				}else{
					iy = (client.dy-th-ih)/2+(down?DOWN_M:0);
					ty = iy+idy;
				}
			}
		}else{
			if(tw > client.dx){
				//2 不显示文字, 只显示图片
				tx = ty = tdx = tdy = 0;
				ix = down?DOWN_M:0;
				iy = down?DOWN_M:0;
				idx = client.dx-(down?DOWN_M:0);
				idy = client.dy-(down?DOWN_M:0);
			}else if(tw + iw > client.dx){
				//2 显示文字, 图片缩小显示
				if(dir == guiLeft){
					tx = down?DOWN_M:0;
					ty = down?DOWN_M:0;
					tdx = tw;
					tdy = client.dy-(down?DOWN_M:0);

					ix = tx + tw;
					iy = down?DOWN_M:0;
					idx = client.dx-tw-(down?DOWN_M:0);
					idy = client.dy-(down?DOWN_M:0);
				}else{
					tx = client.dx-tw+(down?DOWN_M:0);
					ty = down?DOWN_M:0;
					tdx = tw;
					tdy = client.dy-(down?DOWN_M:0);
					
					ix = down?DOWN_M:0;
					iy = down?DOWN_M:0;
					idx = client.dx-(down?DOWN_M:0)-tw;
					idy = client.dy-(down?DOWN_M:0);
				}
			}else{
				//2 文字和图片都居中显示
				if(dir == guiLeft){
					tx = (client.dx-tw-iw)/2+(down?DOWN_M:0);
					ty = down?DOWN_M:0;
					tdx = tw;
					tdy = client.dy-(down?DOWN_M:0);

					ix = tx + tw;
					iy = down?DOWN_M:0;
					idx = iw;
					idy = client.dy-(down?DOWN_M:0);
				}else{
					tx = (client.dx-tw-iw)/2+iw+(down?DOWN_M:0);
					ty = down?DOWN_M:0;
					tdx = tw;
					tdy = client.dy-(down?DOWN_M:0);

					ix = (client.dx-tw-iw)/2+(down?DOWN_M:0);
					iy = down?DOWN_M:0;
					idx = iw;
					idy = client.dy-(down?DOWN_M:0);
				}
			}
		}

		if(tdx && tdy)
			gdi_draw_text_in(gdi, tx, ty, tdx, tdy, text, -1, CHARSET_UTF8, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE);
		gdi_draw_image_in(gdi, ix, iy, idx, idy, icon, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
	}
	if(icon != h->icon)
		g_object_unref(icon);

	gdi_translate(gdi, -client.x, -client.y);

	if(!defer)
		gdi_blt(gdi, 0, 0, wgt_bound_dx(h), wgt_bound_dy(h));
	g_object_unref(gdi);
}
#define HILIGHT_COLOR	MAKE_ARGB(0x40, 0x00, 0x00, 0xff)
static void	__button_redraw_state(t_HWidget h, gboolean defer)
{
	t_CPushButton *pme = (t_CPushButton *)h;

#if !defined(FTR_NO_INPUT_KEY)
	if(wgt_is_focused(h)){
		//2 防止状态没复位
		wgt_disable_attr(h, BTN_ATTR_DOWN);
	}
#endif
	if(!wgt_is_enabled(h) || wgt_test_attr(h, BTN_ATTR_PUSHED)){
		t_HGDI gdi = wgt_get_client_gdi(h);
		gu32 old;
		gui_rect_t client;

		if(!wgt_is_enabled(h)){
			//2 灰度显示
			old = gdi_set_color(gdi, MAKE_ARGB(0xd0, 0x32, 0x32, 0x32));
		}else{
			//2 高亮显示
			old = gdi_set_color(gdi, HILIGHT_COLOR);
		}
		gdi_rectangle_fill(gdi, 0, 0, wgt_bound_dx(h), wgt_bound_dy(h), h->border_corner);
		gdi_set_color(gdi, old);
		if(!defer)
			gdi_blt(gdi, 0, 0, wgt_bound_dx(h), wgt_bound_dy(h));
		g_object_unref(gdi);
	}
}

static error_t __button_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_CPushButton *pme = (t_CPushButton *)handle;

	switch(evt){
		case WIDGET_OnDraw:
			{
				__button_redraw(pme, TRUE);
				__button_redraw_state(pme, TRUE);
				ret = SUCCESS;
			}
			break;

		case WIDGET_OnSetFocus:
#if !defined(FTR_NO_INPUT_KEY)
			//2 防止按键在窗口重绘期间先输出到屏幕上, 或直接用wgt_redraw(), 因为该函数会作此判断
			if(wgt_is_focused(wgt_parent_window(handle)) && !wgt_test_attr(wgt_get_parent(handle), CTRL_ATTR_DRAWING))
			__button_redraw(pme, FALSE);
#endif
			break;
		case WIDGET_OnKillFocus:
#if !defined(FTR_NO_INPUT_KEY)
			if(wgt_is_focused(wgt_parent_window(handle)) && !wgt_test_attr(wgt_get_parent(handle), CTRL_ATTR_DRAWING))
			__button_redraw(pme, FALSE);
#endif
			break;
		case WIDGET_OnSelected:
			break;
		case WIDGET_OnKeyEvent:
			if(lparam == MVK_OK){
				if(sparam == MMI_KEY_DOWN){
#if !defined(FTR_NO_INPUT_KEY)
					wgt_enable_attr(pme, BTN_ATTR_DOWN);
#endif
					if(wgt_test_attr(pme, BTN_ATTR_PUSHED)){
						wgt_disable_attr(pme, BTN_ATTR_PUSHED);
						wgt_redraw(handle, FALSE);
					}else{
						wgt_enable_attr(pme, BTN_ATTR_PUSHED);
#if defined(FTR_NO_INPUT_KEY)
						//2 仅仅只需要在原图上重绘
						__button_redraw_state(pme, FALSE);
#else
						//2 除需要在原图上重绘外，还须更新icontext，所以完全重绘
						wgt_redraw(handle, FALSE);
#endif
					}
					wgt_send_msg(handle, EVT_SELECT, 0, 0, 0);
				}
#if !defined(FTR_NO_INPUT_KEY)
				else if(sparam == MMI_KEY_UP){
					wgt_disable_attr(pme, BTN_ATTR_DOWN);
					wgt_redraw(pme, FALSE);
				}
#endif
			}
			ret = SUCCESS;
			break;

#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
		{
			coord_t x0, y0;
			PEN_XY(lparam, x0, y0);
			switch(PEN_TYPE(lparam)){
				case MMI_PEN_PRESS:
					//2 PenPress就表示没有PenMove, 截获消息，方泽默认转换为OK(PRESS)消息
					ret = SUCCESS;
					break;
				case MMI_PEN_LONG_PRESS:
					//2 PenLongPress就表示没有PenMove
					break;
				case MMI_PEN_DOWN:
					__button_OnEvent(handle, WIDGET_OnKeyEvent, MMI_KEY_DOWN, MVK_OK);
					ret = SUCCESS;
					break;
				case MMI_PEN_UP:
					__button_OnEvent(handle, WIDGET_OnKeyEvent, MMI_KEY_UP, MVK_OK);
					ret = SUCCESS;
					break;
				case MMI_PEN_MOVE_IN:
						ret = SUCCESS;
					break;
				case MMI_PEN_MOVE_OUT:
						ret = SUCCESS;
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


static VTBL(widget) pushbuttonVtbl = {
	(PFNONCREATE)__pushbutton_OnCreate
	, (PFNONDESTROY)__pushbutton_OnDestroy

	, __button_OnEvent
};

CLASS_DECLARE(pushbutton, t_CPushButton, pushbuttonVtbl);


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_pushbutton;
}

#endif

// TODO: add functions for your own widget.  You MUST export them in dll.def


#undef PARENT_CLASS

