	
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
	
#define DLL_EXPORT_label

#include "wgt_label.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/

//1 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	BrdHandle	brd;
	MC_FILL_STRUCT2;
	resid_t		resid_text;
	char			*text;
}t_LabelCtrl;


#define SPLIT_WIDTH		1

/*===========================================================================

Class Definitions

===========================================================================*/


static error_t		__label_OnCreate(t_LabelCtrl* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	//2NOTICE: attr MUST be set first since specific initialization should be done
	//wgt_enable_attr(pme, CTRL_ATTR_TABSTOP);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	return SUCCESS;
}

static void	__label_OnDestroy(t_LabelCtrl* pme)
{
	FREEIF(pme->text);
	g_object_inherit_destroy(pme);
}


static error_t __label_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_LabelCtrl *pme = (t_LabelCtrl *)handle;
	switch(evt){
		case WIDGET_OnDraw:
			{
				t_HGDI gdi;
				coord_t caption_x, text_x;
				t_HImage icon;
				char *title, *text;
				coord_t x, y, width, height;
				int fh;

				gdi = wgt_get_gdi(handle);

				if(wgt_is_focused(handle))
					gdi_style_show_hilight_bg(gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle), handle->border_corner, TRUE);
				else
					wgt_show_border(handle, gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
				
				x = y = handle->border_width;
				width = wgt_bound_dx(handle)-(handle->border_width<<1);
				height = wgt_bound_dy(handle)-(handle->border_width<<1);

				//2 icon
				if(g_object_valid(handle->icon))
					icon = handle->icon;
				else
					icon = brd_get_image(handle->brd_icon, handle->resid_icon);
				if(g_object_valid(icon)){
					gdi_draw_image_in(gdi, x, y, height, height, icon, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
					if(icon != handle->icon)
						g_object_unref(icon);
					x += height+SPLIT_WIDTH;
					width -= height+SPLIT_WIDTH;
				}
				//2 caption
				fh = gdi_get_fontsize(gdi)*font_get(lang_get_current())->height;
				y = (height-fh)>>1;
				gdi_move_to(gdi, x, y);
				
				if(handle->title)
					title = handle->title;
				else
					title = brd_get_string(handle->brd_text, handle->resid_text, NULL);
				if(title && *title){
					gdi_set_color(gdi, ui_theme_get()->fg_color);
					gdi_draw_text(gdi, title, -1, CHARSET_UTF8);
					gdi_move(gdi, SPLIT_WIDTH, 0);
				}
				//2 text
				if(pme->text){
					text = pme->text;
				}else{
					text = brd_get_string(pme->brd, pme->resid_text, NULL);
				}
				if(text){
					gdi_set_color(gdi, ui_theme_get()->fg_color);
					gdi_draw_text(gdi, text, -1, CHARSET_UTF8);
				}
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
			break;
			
		case WIDGET_OnKeyEvent:
			ret = wgt_hook(handle, evt, sparam, lparam);
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


static VTBL(widget) labelVtbl = {
	(PFNONCREATE)__label_OnCreate
	, (PFNONDESTROY)__label_OnDestroy

	, __label_OnEvent
	, NULL
};

CLASS_DECLARE(label, t_LabelCtrl, labelVtbl);


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_label;
}

#endif

// TODO: add functions for your own widget.  You MUST export them in dll.def

void		label_set_text(t_HWidget handle, const char *fmt, ...)
{
	if(g_object_valid(handle)){
		t_LabelCtrl *pme = (t_LabelCtrl *)handle;

		FREEIF(pme->text);
		if(fmt && *fmt){
			va_list ap;
			va_start(ap, fmt);
			pme->text = g_vsprintf_dup(fmt, ap);		
			va_end( ap );
		}
		pme->resid_text = RESID_UNDEF;
		wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
	}
}

void		label_set_text_by_resid(t_HWidget handle, BrdHandle brd, resid_t resid)
{
	if(g_object_valid(handle)){
		t_LabelCtrl *pme = (t_LabelCtrl *)handle;

		FREEIF(pme->text);
		pme->resid_text = resid;
		//if(resid != RESID_UNDEF && brd == BRD_HANDLE_SELF){
		//	brd = current_brd_handle();
		//}
		pme->brd = brd;
		wgt_enable_attr(handle, CTRL_ATTR_MODIFIED);
	}
}

const char *	label_get_text(t_HWidget handle)
{
	const char *text = NULL;
	if(g_object_valid(handle)){
		t_LabelCtrl *pme = (t_LabelCtrl *)handle;
		if(pme->text)
			text = pme->text;
		else if(pme->resid_text != RESID_UNDEF)
			text = brd_get_string(pme->brd, pme->resid_text, NULL);
	}
	return text;
}

void		label_insert_text(t_HWidget handle, int cursor, int replace, const char *str, int len)
{
	if(g_object_valid(handle) && str && *str){
		t_LabelCtrl *pme = (t_LabelCtrl *)handle;
		char *text = pme->text;
		char *newt, *p;
		int n = 0, newn, i;
		if(!pme->text)
			text = brd_get_string(pme->brd, pme->resid_text, NULL);
		if(text)
			n = strlen(text);
		if(cursor < 0 || cursor > n)
			cursor = n;
		
		if(replace < 0)
			replace = 0;
		else if(replace > n - cursor)
			replace = n-cursor;

		if(len <= 0)
			len = strlen(str);
		newn = n - replace + len;
		newt = MALLOC(newn+1);
		p = newt;
		for(i = 0; i < cursor; i++)
			*p++ = *text++;
		text += replace;
		for(i = 0; i < len; i++)
			*p++ = *str++;
		for(i = cursor+replace; i < n; i++)
			*p++ = *text++;
		*p = 0;

		FREEIF(pme->text);
		pme->text = newt;
	}
}


#undef PARENT_CLASS

