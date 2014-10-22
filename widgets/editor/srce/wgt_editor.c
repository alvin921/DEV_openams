	
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
描述:
菜单特性:
1) 最大字节数限制
2) 暂时只支持数字模式, 即[0~9*#], 方向键切换光标
3) 单行: 不能换行
4) 多行: 自动换行, 光标可行间切换
最简单最快的实现:
1) 单行, 光标左右切换

按键: 0~9, *, #
单行editor有:
1) 电话号码: 直接使用原始按键, #不可用于输入法切换
2) 数字: *代表小数点, #可用于输入法切换, 取决于用户设定
3) 
===========================================================================*/


/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
	
#define DLL_EXPORT_editor

#include "wgt_editor.h"
#include "ime.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/


#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)



#define	NCHARS_MAX_DEFAULT		40

#define SPLIT_WIDTH		1

typedef struct {
	gu16	start;
	gu16	num;
}t_CTextLine;

typedef struct {
	t_Widget	common;
	
	gu32	type : 3;
	gu32	ime_nav: 1;
	gu32	has_icon:1;
	gu32	has_caption:1;
	gu32	flags:16;

	gs16		bytes_selected;
	gs16		utf8_bytes;
	gs16		utf8_bytes_max;
	IMEID	imeid;

	gui_rect_t	rt_title;
	gui_rect_t	rt_content;

	//2 =====================================
	ucs2_t 	*buffer;
	gu16	nchars;
	gu16	nchars_max;

	gu16	cursor;		//2 光标坐在字符的位置
	gs16		nselects;	//2 选择状态字符数，>0表示(cursor,tail), <0表示(head,cursor)

	gs16		tw_max;
	gs16		th_max;

	//2 =====================================
	//2 following for <Single View>
	gu32	tw_total;
	//2 =====================================
	//2 following for <Multiline View>
	gu32	line_height:8;
	gu32	cursor_c:12;		//2 光标所在字符的行列号
	gu32	cursor_r:12;
	coord_t	cursor_x;	//2 光标所在坐标
	coord_t	cursor_y;
	g_array_t*	lines;
}t_CEditor;


//---------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/


/*===========================================================================

Class Definitions

===========================================================================*/

static error_t		__editor_OnCreate(t_CEditor* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;

	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	
	pme->buffer = NULL;
	pme->nchars_max = 0;
	pme->nchars = 0;
	pme->cursor = 0;
	pme->nselects = 0;

	pme->type = EDITOR_TYPE_DIGIT;
	pme->ime_nav = TRUE;
	pme->flags = IDF_TEXT_SINGLE|IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE;
	wgt_enable_attr(pme, CTRL_ATTR_HSCROLLABLE);
	
	pme->bytes_selected = 0;
	editor_set_nbytes_max((t_HWidget)pme, -1);

	pme->line_height = 16;
	pme->lines = NULL;
	return SUCCESS;
}

static void	__editor_OnDestroy(t_CEditor* pme)
{
	FREEIF(pme->buffer);
	g_array_destroy(pme->lines, NULL);
	g_object_inherit_destroy(pme);
}


void		__recalc_multiline(t_CEditor *pme, gu16 width)
{
	if(BIT_TEST(pme->flags, IDF_TEXT_AUTO_WRAP)){
		ucs2_t *ptr = pme->buffer;
		t_CTextLine line;
		int start;
		int len;
		
		len = pme->nchars;
		start = 0;
		
		g_array_reset(pme->lines, NULL);
		pme->cursor_r = 0;
		pme->cursor_c = pme->cursor;
		while(len > 0 && *ptr){
			line.num = font_measure_ustring(ptr, len, width, NULL, NULL);
			if(line.num == 0)
				break;
			line.start = start;
			start += line.num;
			if(pme->cursor >=  line.start && pme->cursor <= start){
				pme->cursor_r = g_array_size(pme->lines);
				pme->cursor_c = pme->cursor-line.start;
			}
			g_array_append(pme->lines, &line);
			len -= line.num;
			ptr += line.num;
		}
	}else{
		//2 单行显示，需要使光标可见
		
	}
}

void		__calc_single_total_width(t_CEditor *pme, ucs2_t *ptr, int nchars)
{
	int k, tw;
	pme->tw_total = 0;
	while(*ptr && nchars > 0){
		k = font_measure_ustring(ptr, nchars, -1, &tw, NULL);
		ptr += k;
		nchars -= k;
		pme->tw_total += tw;
	}
}


#define	ED_MARGIN_X	4
#define	ED_MARGIN_Y	1
#define	ED_TITLE_HEIGHT	16

//2 约定:
//4 如果Multiline或AutoWrap置位则title在第一行显示，否则title和编辑区域同行显示
static void	__calc_coordinate(t_HWidget handle)
{
	if(wgt_test_attr(handle, CTRL_ATTR_MODIFIED)){
		t_CEditor *pme = (t_CEditor *)handle;
		char *title;

		//3 icon
		pme->has_icon = (g_object_valid(handle->icon) || handle->resid_icon != RESID_UNDEF)?TRUE:FALSE;
		//3 caption
		pme->has_caption = (handle->title || handle->resid_text != RESID_UNDEF)?TRUE:FALSE;
		pme->rt_title.x = 0;
		pme->rt_title.y = 0;
		pme->rt_title.dx = 0;
		pme->rt_title.dy = 0;
		if(pme->flags & (IDF_TEXT_MULTILINE|IDF_TEXT_AUTO_WRAP)){
			if(pme->has_icon || pme->has_caption){
				pme->rt_title.dx = wgt_bound_dx(handle);
				pme->rt_title.dy = ED_TITLE_HEIGHT;
			}
			pme->rt_content.x = 0;
			pme->rt_content.y = pme->rt_title.y+pme->rt_title.dy;
			pme->rt_content.dx = wgt_bound_dx(handle);
			pme->rt_content.dy = wgt_bound_dy(handle) - pme->rt_title.dy;
		}else{
			pme->rt_title.dy = wgt_bound_dy(handle);
			if(pme->has_caption){
				title = wgt_get_caption(handle);
				if(title && *title){
					int tw;
					font_measure_string(title, -1, CHARSET_UTF8, wgt_bound_dx(handle), &tw, NULL, NULL);
					pme->rt_title.dx += 2+tw+2;
				}
			}
			if(pme->has_icon){
				pme->rt_title.dx += ED_TITLE_HEIGHT;
			}
			pme->rt_content.x = pme->rt_title.x+pme->rt_title.dx;
			pme->rt_content.y = 0;
			pme->rt_content.dx = wgt_bound_dx(handle)-pme->rt_title.dx;
			pme->rt_content.dy = wgt_bound_dy(handle);
		}

		__recalc_multiline(pme, pme->rt_content.dx-(handle->border_width+ED_MARGIN_X)*2);
		
		wgt_disable_attr(handle, CTRL_ATTR_MODIFIED);
	}
}

// 在指定区域单行显示text，如果有指定高亮和光标，也须显示
// 默认靠左显示，V方向上允许上对齐、下对齐或居中
static void	__show_single_line(t_HGDI gdi, ucs2_t *ptr, int start, int nchars, int s_start, int ns, coord_t x, coord_t y, gu16 dx, gu16 dy, gu32 flags, gboolean clear, gboolean defer, gboolean focused)
{
	int i;
	coord_t x0, y0;
	ucs2_t ch;
	int th, tw, defer_x, defer_dx, th_max;
	gu32 old_mask;

	defer_x = defer_dx = th_max = 0;
	if(start){
		gdi_measure_utext(gdi, ptr, start, -1, &defer_x, NULL);
		ptr += start;
	}
	if(clear || !defer){
		gdi_measure_utext(gdi, ptr, nchars, -1, &defer_dx, NULL);
		if(clear){
#if !defined(FTR_NO_INPUT_KEY)
			if(focused)
				gdi_style_show_hilight_bg(gdi, defer_x, 0, defer_dx, dy, guiNoCorner, FALSE);
			else
#endif
			gdi_clear(gdi, defer_x, 0, defer_dx, dy, guiNoCorner);
		}
	}


	x0 = x+defer_x;
	y0 = y;
	if(ns <= 0){
		s_start = 0;
		ns = 0;
	}else{
		if(s_start < 0)
			s_start = 0;
		if(s_start+ns <= start || s_start >= start+nchars){
			s_start = ns = 0;
		}else{
			if(s_start < start){
				ns -= start-s_start;
				s_start = start;
			}
			if(s_start+ns > nchars)
				ns = nchars -s_start;
		}
	}
	for(i = 0; i < nchars; i++){
		if(ns > 0){
			if(i == s_start)
				old_mask = gdi_set_mask_color(gdi, 0x000000a0);
			else if(i == s_start+ns)
				gdi_set_mask_color(gdi, old_mask);
		}
	
		ch = *ptr++;
		gdi_measure_utext(gdi, &ch, 1, -1, &tw, &th);
		if(th > th_max)th_max = th;
		if(flags & IDF_VALIGN_TOP){
			y0 = 0;
		}else if(flags & IDF_VALIGN_BOTTOM){
			y0 = y+dy-th;
		}else if(flags & IDF_VALIGN_MIDDLE){
			y0 = y+(dy-th)/2;
		}
		gdi_move_to(gdi, x0, y0);
		x0 += tw;
	
		gdi_draw_uchar(gdi, ch);
	}
	//2 如果不加ns>0条件，old_mask可能会未初始化而使用
	if(i == s_start+ns && ns > 0)
		gdi_set_mask_color(gdi, old_mask);
	if(!defer)
		gdi_blt(gdi, defer_x, 0, defer_dx, dy);
}

static void	__show_cursor(t_CEditor *pme, gboolean defer)
{
	t_HGDI gdi;
	int th, x, y, dy;
	gui_rect_t client;
	
	gdi = wgt_get_client_gdi((t_HWidget)pme);
	if(!g_object_valid(gdi))return;

	th = x = 0;
	if(pme->flags & (IDF_TEXT_MULTILINE|IDF_TEXT_AUTO_WRAP)){
		t_CTextLine *line = g_array_element(pme->lines, pme->cursor_r);
		y = pme->line_height*pme->cursor_r;
		dy = pme->line_height;
		if(pme->cursor_c > 0)
			gdi_measure_utext(gdi, pme->buffer+(line?line->start:0), pme->cursor_c, -1, &x, &th);
	}else{
		wgt_get_client_rect((t_HWidget)pme, &client);
		dy = client.dy;
		y = 0;
		if(pme->cursor > 0)
			gdi_measure_utext(gdi, pme->buffer, pme->cursor, -1, &x, &th);
	}
	if(th < 14)
		th = 14;
	y += (dy-th)/2;
	gdi_set_color(gdi, RGB_BLUE);
	gdi_vline(gdi, x, y, y+th-1);
	if(!defer)
		gdi_blt(gdi, x, y, 1, th);
	g_object_unref(gdi);
}

static void	__show_content(t_CEditor *pme, gboolean defer, int start, int nchars)
{
	const ucs2_t *ptr = (const ucs2_t *)pme->buffer;
	int len;
	t_HGDI gdi;
	gui_rect_t client;
	t_UITheme *theme = ui_theme_get();
	gu32 old_mask;
	gs16 s_start, s_end, ns;
	gs16 i;
	coord_t x,y;

	if(pme->nchars == 0 || nchars == 0) return;
	gdi = wgt_get_client_gdi((t_HWidget)pme);
	if(!g_object_valid(gdi))return;

	wgt_get_client_rect((t_HWidget)pme, &client);
	old_mask = gdi_get_mask_color(gdi);

	
	ns = pme->nselects;
	s_start = pme->cursor;
	if(ns < 0){
		s_start += ns;
		ns = -ns;
	}
#if !defined(FTR_NO_INPUT_KEY)
	if(wgt_is_focused(pme))
		gdi_set_color(gdi, theme->hilight_fg_color);
	else
#endif
	gdi_set_color(gdi, theme->fg_color);


	if(pme->flags & IDF_TEXT_AUTO_WRAP){
		//2 多行显示，上下滚动
		t_CTextLine *line;
		int s, n;
		x = 0;
		y = 0;
		
		gdi_set_color(gdi, theme->fg_color);
		for(i = 0; i < g_array_size(pme->lines); i++){
			line = g_array_element(pme->lines, i);
			if(line->start >= start+nchars)
				break;
			
			if(line->start+line->num >= start){
				if(ns == 0 || line->start+line->num < s_start || line->start > s_start+ns){
					s = n = 0;
				}else{
					if(line->start <= s_start){
						s = s_start-line->start;
					}else{
						s = 0;
					}
					if(s_start+ns >= line->start+line->num)
						n = line->num-s;
					else
						n = s_start+ns-line->start-s;
				}
				__show_single_line(gdi, ptr, 0, line->num, s, n, x, y, client.dx, pme->line_height, pme->flags, !defer, TRUE, wgt_is_focused(pme));
			}
			y += pme->line_height;
			ptr += line->num;
		}
	}else{
		//2 单行显示，左右滚动
		__show_single_line(gdi, ptr, start, nchars, s_start, ns, 0, 0, client.dx, client.dy, pme->flags, !defer, defer, wgt_is_focused(pme));
	}

	if(!defer)
		gdi_blt(gdi, 0, 0, client.dx, client.dy);
	g_object_unref(gdi);
}

static error_t __editor_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_CEditor *pme = (t_CEditor *)handle;
	switch(evt){
		case WIDGET_OnMessage:
			if(sparam == EVT_IME_ADD_CHAR){
				t_AmosMsg *msg = (t_AmosMsg *)lparam;
				editor_add_char(handle, msg->sparam, msg->lparam);
				ret = SUCCESS;
			}
			break;
		case WIDGET_OnDraw:
			{
				t_HGDI gdi = wgt_get_gdi(handle);
				t_HImage icon;
				char *title;

				__calc_coordinate(handle);
				//2 show TITLE(icon & caption)
				icon = wgt_get_icon(handle);
				title = wgt_get_caption(handle);
				gdi_set_color(gdi, ui_theme_get()->fg_color);
				gdi_draw_icon_text(gdi, icon, title, pme->rt_title.dx, pme->rt_title.dy, guiRight, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
				g_object_unref(icon);
				
				//2 show content border & client bg
#if !defined(FTR_NO_INPUT_KEY)
				if(wgt_is_focused(handle))
					gdi_style_show_hilight_bg(gdi, pme->rt_content.x, pme->rt_content.y, pme->rt_content.dx, pme->rt_content.dy, guiFourCorner, TRUE);
				else
#endif
					wgt_show_border(handle, gdi, pme->rt_content.x, pme->rt_content.y, pme->rt_content.dx, pme->rt_content.dy);
				g_object_unref(gdi);

				__show_content(pme, TRUE, 0, pme->nchars);
				if(wgt_is_focused(pme))
				__show_cursor(pme, TRUE);
				
				ret = SUCCESS;
			}
			break;

		case WIDGET_OnSetFocus:
			ime_agent_activate(handle);
			if(pme->type == EDITOR_TYPE_TEXT)
				ime_set(pme->imeid, -1);
			else if(pme->type == EDITOR_TYPE_PHONE)
				ime_set(pme->imeid, DIGIT_TYPE_PHONE);
			else
				ime_set(pme->imeid, DIGIT_TYPE_NUMBER);
			ime_enable_navigation(pme->ime_nav);
			wgt_redraw(handle, FALSE);
			ret = SUCCESS;
			break;
		case WIDGET_OnKillFocus:
			ime_agent_activate(NULL);
			pme->imeid = ime_current();
			wgt_redraw(handle, FALSE);
			ret = SUCCESS;
			break;
		case WIDGET_OnSelected:
			break;
		case WIDGET_OnKeyEvent:
			ret = wgt_hook((t_HWidget)pme, evt, sparam, lparam);
			if(METH_HANDLED(ret))
				break;
			if(sparam == MMI_KEY_DOWN)
				break;
			switch(lparam){
				case MVK_UP:
					break;
				case MVK_DOWN:
					break;
				case MVK_LEFT:
					if(pme->cursor == 0 && pme->nselects){
						editor_unselect(handle);
						ret = SUCCESS;
						break;
					}
					if(pme->cursor > 0){
						if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
							editor_set_cursor(handle, pme->cursor-1);
							wgt_redraw(handle, FALSE);
							ret = SUCCESS;
						}
					}
					break;
				case MVK_RIGHT:
					if(pme->cursor == pme->nchars && pme->nselects){
						editor_unselect(handle);
						ret = SUCCESS;
						break;
					}
					if(pme->cursor < pme->nchars){
						if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
							editor_set_cursor(handle, pme->cursor+1);
							wgt_redraw(handle, FALSE);
							ret = SUCCESS;
						}
					}
					break;
				case MVK_CLR:
#if defined(FTR_SOFT_KEY)
				case MVK_RSK:
#endif
					if(pme->cursor == 0 && pme->nselects == 0)
						break;
					if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
						if(pme->nselects == 0)
							editor_add_char(handle, IME_ADD_MODE_BKSPACE, 0);
						else
							editor_add_char(handle, IME_ADD_MODE_NORMAL, 0);
					}
					ret = SUCCESS;
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
					ret = SUCCESS;
					break;
				case MMI_PEN_LONG_PRESS:
					//2 PenLongPress就表示没有PenMove和PenUp
					break;
				case MMI_PEN_DOWN:
					ret = SUCCESS;
					break;
				case MMI_PEN_UP:
					ret = SUCCESS;
					break;
				case MMI_PEN_MOVE_IN:
					break;
				case MMI_PEN_MOVE_OUT:
					break;
				case MMI_PEN_MOVE:
					ret = SUCCESS;
					break;
			}
		}
			break;
#endif
	}
	if(ret == NOT_HANDLED){
		ret = g_widget_inherit_OnEvent(handle, evt, sparam, lparam);
	}
	return ret;
}

static void	__editor_get_client_rect(t_HWidget handle, gui_rect_t *prc)
{
	t_CEditor *pme = (t_CEditor *)handle;

	__calc_coordinate(handle);
	memcpy(prc, &pme->rt_content, sizeof(gui_rect_t));
	prc->x += handle->border_width+ED_MARGIN_X;
	prc->dx -= (handle->border_width+ED_MARGIN_X)*2;

	prc->y += handle->border_width+ED_MARGIN_Y;
	prc->dy -= (handle->border_width+ED_MARGIN_Y)*2;
}

static VTBL(widget) editorVtbl = {
	(PFNONCREATE)__editor_OnCreate
	, (PFNONDESTROY)__editor_OnDestroy

	, __editor_OnEvent
	, __editor_get_client_rect
};

CLASS_DECLARE(editor, t_CEditor, editorVtbl);


#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_editor;
}

#endif

#undef PARENT_CLASS

void		editor_set_flags(t_HWidget handle, gu32 flags)
{
	if(g_object_valid(handle)){
		t_CEditor *pme = (t_CEditor *)handle;

		if(pme->lines){
			g_array_destroy(pme->lines, NULL);
			pme->lines = NULL;
		}
		if(IDF_TEXT_MULTILINE & flags){
			BIT_SET(flags, IDF_TEXT_AUTO_WRAP);
		}
		if(IDF_TEXT_AUTO_WRAP & flags){
			//2 整个默认是从左至右，从上至下显示，每行则是以bottom对齐
			BIT_CLEAR(flags, IDF_VALIGN_MASK);
			BIT_SET(flags, IDF_VALIGN_BOTTOM);
			pme->lines = g_array_new(sizeof(t_CTextLine), 4, ARRAY_TYPE_DYN_ALIGN, 4);
		}else{
			BIT_CLEAR(flags, IDF_VALIGN_MASK);
			BIT_SET(flags, IDF_VALIGN_MIDDLE);
		}
		
		((t_CEditor *)handle)->flags = flags;
	}
}

void		editor_set_line_height(t_HWidget handle, gu8 line_height)
{
	if(g_object_valid(handle)){
		t_CEditor *pme = (t_CEditor *)handle;
		pme->line_height = line_height;
	}
}

gu8		editor_get_line_height(t_HWidget handle)
{
	return (g_object_valid(handle))? ((t_CEditor *)handle)->line_height : 16;
}

error_t	editor_set_type(t_HWidget handle, int type, gboolean ime_frozen)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle) && type >= 0 && type < EDITOR_TYPE_N){
		t_CEditor *pme = (t_CEditor *)handle;
		pme->type = type;
		if(type == EDITOR_TYPE_TEXT){
			pme->imeid = ime_query(NULL, lang_get_current(), NULL);
			if(pme->imeid == IME_UNDEF)
				pme->imeid = ime_query(NULL, aLANG_ENGLISH, NULL);
			//ime_set(ime_query("multitap", lang_get_current(), "abc"));
		}else{
			pme->imeid = ime_query(NULL, LANG_123, "123");
		}
		
		pme->ime_nav = ime_frozen?0:1;
		ret = SUCCESS;
	}
	return ret;
}

error_t	editor_set_nchar_max(t_HWidget handle, int nchars)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle)){
		t_CEditor *pme = (t_CEditor *)handle;
		ucs2_t *newbuf;
		if(nchars <= 0)
			nchars = NCHARS_MAX_DEFAULT;
		newbuf = MALLOC((nchars+1) * sizeof(ucs2_t));
		if(pme->nchars > 0 && pme->buffer){
			if(pme->nchars > nchars){
				pme->nchars = nchars;
				pme->utf8_bytes = ustring_to_utf8(pme->buffer, pme->nchars, NULL, -1);
			}
			memcpy(newbuf, pme->buffer, pme->nchars*sizeof(ucs2_t));
		}else{
			pme->nchars = 0;
		}
		FREEIF(pme->buffer);
		pme->nchars_max = nchars;
		pme->buffer = newbuf;
		pme->cursor = pme->nchars;
		pme->buffer[pme->nchars] = 0;
		ret = SUCCESS;
	}
	return ret;
}

ucs2_t *	editor_get_utext(t_HWidget h)
{
	return (g_object_valid(h))? ((t_CEditor*)h)->buffer : NULL;
}

void	editor_set_utext(t_HWidget h, const ucs2_t *string)
{
	if(g_object_valid(h) && string){
		t_CEditor* pme = (t_CEditor*)h;
		ucs2_t *ptr = NULL;
		int i;
		ptr = pme->buffer;
		
		for(i = 0; *string && i < pme->nchars_max; i++){
			*ptr++ = *string++;
		}
		pme->nchars = i;
		*ptr = 0;

		pme->utf8_bytes = ucs2_to_string(pme->buffer, pme->nchars, NULL, -1);

		pme->cursor = pme->nchars;

		__calc_single_total_width(pme, pme->buffer, pme->nchars);
	}
}

int		editor_get_nchars(t_HWidget handle)
{
	return (g_object_valid(handle))? ((t_CEditor *)handle)->nchars : 0;
}

error_t	editor_set_nbytes_max(t_HWidget handle, int nbytes)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle)){
		t_CEditor *pme = (t_CEditor *)handle;
		//3 n bytes can hold n unicode chars
		editor_set_nchar_max(handle, nbytes);
		pme->utf8_bytes_max = nbytes;
		ret = SUCCESS;
	}
	return ret;
}

int		editor_get_text(t_HWidget handle, char *buffer, int size)
{
	int nlen = 0;
	if(g_object_valid(handle)){
		t_CEditor *pme = (t_CEditor *)handle;
		if(buffer)
			nlen = ucs2_to_string(pme->buffer, pme->nchars, buffer, size);
		else
			nlen = pme->utf8_bytes;
	}
	return nlen;
}

char *		editor_get_text_dup(t_HWidget handle)
{
	char *str = NULL;

	if(g_object_valid(handle)){
		t_CEditor *pme = (t_CEditor *)handle;
		int nlen = pme->utf8_bytes;

		str = MALLOC(nlen + 1);
		editor_get_text(handle, str, nlen+1);
	}
	return str;
}

void	editor_set_text(t_HWidget h, const char *string)
{
	if(g_object_valid(h) && string){
		t_CEditor* pme = (t_CEditor*)h;
		pme->nchars = string_to_ucs2(string, -1, LANG_UNDEF, pme->buffer, pme->nchars_max+1);
		pme->utf8_bytes = ucs2_to_string(pme->buffer, pme->nchars, NULL, -1);

		__calc_single_total_width(pme, pme->buffer, pme->nchars);
	}
}

void		editor_set_cursor(t_HWidget h, int cursor)
{
	if(g_object_valid(h)){
		t_CEditor* pme = (t_CEditor*)h;
		gboolean ret = FALSE;

		//2 unselect first
		editor_set_sel(h, 0);
		if(cursor >= 0 && cursor <= pme->nchars){
			if(cursor != pme->cursor){
				//2 设置光标新位置，并重绘光标
				pme->cursor = cursor;
				if(g_array_size(pme->lines) && (pme->flags&(IDF_TEXT_AUTO_WRAP|IDF_TEXT_MULTILINE))){
					//2 表示有多行，必须计算光标所在行列
					g_idx_t idx;
					t_CTextLine *line;
					for(idx = 0; idx < g_array_size(pme->lines); idx++){
						line = g_array_element(pme->lines, idx);
						if(line->start <= cursor && cursor <= line->start+line->num){
							pme->cursor_r = idx;
							pme->cursor_c = cursor-line->start;
							break;
						}
					}
				}
			}
		}
	}
}

//2 以cursor为起点(ns>0)或终点(ns<0)设置选择区域
void	editor_set_sel(t_HWidget h, int nselected)
{
	if(g_object_valid(h)){
		t_CEditor* pme = (t_CEditor*)h;
		gboolean ret = FALSE;
		if(nselected < 0){
			if(nselected + pme->cursor < 0)
				nselected = -pme->cursor;
		}else if(nselected > 0){
			if(nselected + pme->cursor > pme->nchars)
				nselected = pme->nchars - pme->cursor;
		}
		if(pme->nselects != nselected){
			int old = pme->nselects;
			pme->nselects = nselected;
			if(wgt_drawable(h)){
				if(nselected == 0){
					//2 重新绘制这nselected字符
					if(old > 0)
						__show_content(pme, FALSE, pme->cursor, old);
					else
						__show_content(pme, FALSE, pme->cursor+old, -old);
				}else if(nselected > 0){
					if(old > nselected)
						__show_content(pme, FALSE, pme->cursor+nselected, old-nselected);
					else
						__show_content(pme, FALSE, pme->cursor+old, nselected-old);
				}else{
					if(old < nselected)
						__show_content(pme, FALSE, pme->cursor+old, nselected-old);
					else
						__show_content(pme, FALSE, pme->cursor+nselected, old-nselected);
				}
			}
		}
	}
}

void	editor_add_char(t_HWidget h, gu8 mode, ucs2_t ch)
{
	int i, n;
	char str[3];

	t_CEditor* pme = (t_CEditor*)h;
	if(!g_object_valid(h))
		return;
	if(pme->nselects > 0){
		pme->cursor += pme->nselects;
		pme->nselects = -pme->nselects;
	}
	if(mode == IME_ADD_MODE_UNSELECT){
		//2 unselect
		editor_unselect(h);
		return;
	}
	if(pme->type == EDITOR_TYPE_DIGIT || pme->type == EDITOR_TYPE_INTEGER){
		if(ch < '0' || ch > '9')
			return;
	}else if(pme->type == EDITOR_TYPE_PHONE){
	}else if(pme->type == EDITOR_TYPE_FLOAT){
		if(ch == '*')
			ch = '.';
		else if(ch == '#')
			return;
	}
	if(mode == IME_ADD_MODE_BKSPACE){
		//2 删除模式: 删除当前光标前一个字符
		if(pme->cursor <= 0)
			return;
		ch = pme->buffer[pme->cursor-1];
		for(i = pme->cursor; i < pme->nchars; i++){
			pme->buffer[i-1] = pme->buffer[i];
		}
		pme->cursor--;
		pme->nchars--;
		if(pme->nselects < 0){
			pme->nselects++;
			n = ucs2_to_utf8(ch, str);
			pme->bytes_selected -= n;
		}
	}else if(mode == IME_ADD_MODE_SELECT_SEQ){
		if(pme->nchars + 1 > pme->nchars_max)
			return;
		n = ucs2_to_utf8(ch, str);
		if(pme->utf8_bytes_max > 0 && pme->utf8_bytes + n > pme->utf8_bytes_max)
			return;
		for(i = pme->nchars-1; i >= pme->cursor; i--){
			pme->buffer[i+1] = pme->buffer[i];
		}
		pme->cursor = ch;
		pme->cursor++;
		pme->nselects--;
		pme->bytes_selected += n;
	}else if(mode == IME_ADD_MODE_NORMAL || mode == IME_ADD_MODE_SELECT){
		if(pme->nchars + (ch?1:0) - ABS(pme->nselects) > pme->nchars_max)
			return;
		if(ch){
			n = ucs2_to_utf8(ch, str);
			if(pme->utf8_bytes_max > 0 && pme->utf8_bytes + n - pme->bytes_selected > pme->utf8_bytes_max)
				return;
		}else
			n = 0;
		if(pme->nselects == 0){
			if(ch){
				for(i = pme->nchars-1; i >= pme->cursor; i--){
					pme->buffer[i+1] = pme->buffer[i];
				}
				pme->buffer[pme->cursor++] = ch;
				pme->nchars++;
				pme->utf8_bytes += n;
			}
		}else{
			if(pme->nselects > 0){// select: cursor->tail
				if(ch){
					pme->buffer[pme->cursor++] = ch;
					pme->nselects--;
				}
				if(pme->nselects > 0){
					for(i = pme->cursor+pme->nselects; i < pme->nchars; i++)
						pme->buffer[i-pme->nselects] = pme->buffer[i];
					pme->nchars -= pme->nselects;
					pme->nselects = 0;
				}
			}else{// select: cursor->head
				if(ch){
					pme->buffer[pme->cursor+pme->nselects] = ch;
					pme->nselects++;
				}
				if(pme->nselects < 0){
					for(i = pme->cursor; i < pme->nchars; i++)
						pme->buffer[i+pme->nselects] = pme->buffer[i];
					pme->cursor += pme->nselects;
					pme->nchars += pme->nselects;
					pme->nselects = 0;
				}
			}
			pme->utf8_bytes += n - pme->bytes_selected;
			pme->bytes_selected = 0;
		}
		if(mode == IME_ADD_MODE_SELECT){
			pme->nselects = -1;
			pme->bytes_selected = n;
		}
		pme->buffer[pme->nchars] = 0;
	}
	//2 暂时全文绘制
	wgt_enable_attr(h, CTRL_ATTR_MODIFIED);
	wgt_redraw((t_Widget*)pme, FALSE);
}

