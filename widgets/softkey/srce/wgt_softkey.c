	
/*=============================================================================
	
$DateTime: 2004/11/06 16:41:00 $
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

#include "widget.h"
#include "applet.h"

#if defined(FTR_SOFT_KEY)

#define DLL_EXPORT_softkey


#include "wgt_softkey.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/
	
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)
	

/*===========================================================================

Class Definitions

===========================================================================*/


enum {
	SK_ACT_NONE
	, SK_ACT_CALLBACK
	, SK_ACT_WIDGET
	, SK_ACT_APP
};

#define SK_TEXT_MAX  	15  // Max buffer size for softkey string buffers


typedef struct{
	gu16	act_type:2;
	gu16	align:2;
	gu16	down:1;
	gu16	reserved:11;
	MC_FILL_STRUCT2;
	resid_t		resid;
	char		text[SK_TEXT_MAX+1];        // soft1 string
	
	t_WidgetCallback	callback;
	void *		pointer;
	gui_rect_t	rect;
}t_SKItem;

typedef struct {
	t_Widget	common;

	t_SKItem		items[SK_TOTAL];
#if defined(FTR_TOUCH_SCREEN)
	gs8		sk_down;
	gu8		down_draw;
#endif
}t_SKBar;

static int	__sk_xy_id(t_SKBar *pme, coord_t x, coord_t y)
{
	int k;
	for(k = 0; k < SK_TOTAL; k++)
		if(point_in_rect(x, y, &pme->items[k].rect))
			return k;
	return -1;
}

#define	SK_ITEM(pme, i)	(&((t_SKBar*)(pme))->items[i])

#define	SK_ACT_RESET(pme, i)	do { \
			if(SK_ITEM(pme, i)->act_type == SK_ACT_CALLBACK) \
				SK_ITEM(pme, i)->callback = NULL; \
			else if(SK_ITEM(pme, i)->act_type == SK_ACT_WIDGET) \
				g_object_unref_0(SK_ITEM(pme, i)->pointer); \
			else if(SK_ITEM(pme, i)->act_type == SK_ACT_APP){ \
				SK_ITEM(pme, i)->callback = NULL; \
				SK_ITEM(pme, i)->pointer = NULL; \
			}\
			SK_ITEM(pme, i)->act_type = SK_ACT_NONE; \
		}while(0)

static error_t		__sk_on_create(t_SKBar* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;

	wgt_enable_attr(pme, CTRL_ATTR_IS_SOFTKEY);
	if(prod_ui_sk_bottom())
		wgt_enable_attr(pme, SK_ATTR_BOTTOM);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	wgt_set_bg_fill_solid((t_HWidget)pme, ui_theme_get()->sk_bg_color);
	if(ret == SUCCESS){
		memset(pme->items, 0, sizeof(t_SKItem)*SK_TOTAL);
		SK_ITEM(pme, SK_LSK)->align = SK_TEXT_ALIGN_LEFT;
		#if defined(FTR_CSK_ENABLED)
		SK_ITEM(pme, SK_CSK)->align = SK_TEXT_ALIGN_CENTER;
		#endif
		SK_ITEM(pme, SK_RSK)->align = SK_TEXT_ALIGN_RIGHT;
	}
	wgt_bound_dy(pme) = prod_ui_sk_height();
	
#if defined(FTR_TOUCH_SCREEN)
		pme->sk_down = -1;
		pme->down_draw = FALSE;
#endif
	return ret;
}

static void	__sk_on_destroy(t_SKBar *pme)
{
	SK_ACT_RESET(pme, SK_LSK);
	SK_ACT_RESET(pme, SK_RSK);
#if defined(FTR_CSK_ENABLED)
	SK_ACT_RESET(pme, SK_CSK);
#endif
	g_object_inherit_destroy(pme);
}

static void	__sk_draw_item(t_SKBar *pme, gu8 id, gboolean defer)
{
	gu32 dwFlags = IDF_VALIGN_MIDDLE;
	gui_rect_t client;
	double dx, dy, x, y;
	const char *str = NULL;
	t_HGDI gdi;
	t_SKItem *item = SK_ITEM(pme, id);
	gu8 align = item->align;

	if(item->act_type == SK_ACT_APP){
		pack_GetModuleTitle(item->pointer, (t_AmsModuleId)((gu32)item->callback), item->text, sizeof(item->text));
		str = item->text;
	}else{
		str = brd_get_string(BRD_HANDLE_SELF, item->resid, item->text);
	}
	if(!str || *str == 0)
		return;
	gdi = wgt_get_client_gdi((t_HWidget)pme);
	if(!g_object_valid(gdi))
		return;

	wgt_get_client_rect((t_Widget*)pme, &client);
	if(item->rect.dx == 0){
		//2 重新计算sk区域
#if defined(FTR_IPHONE_STYLE)
		int ww, hh;
		gdi_measure_text(gdi, str, -1, CHARSET_UTF8, client.dx/SK_TOTAL, &ww, &hh, NULL);
		item->rect.dx = 4+ww+4;
		if(item->rect.dx > client.dx/SK_TOTAL)
			item->rect.dx = client.dx/SK_TOTAL;
		item->rect.dy = 4+hh+4;
		if(item->rect.dy > client.dy)
			item->rect.dy = client.dy;
		item->rect.y = (client.dy-item->rect.dy)/2;
		if(id == SK_LSK)
			item->rect.x = client.dx-2-item->rect.dx;
#if defined(FTR_CSK_ENABLED)
		else if(id == SK_CSK)
			item->rect.x = (client.dx-item->rect.dx)/2;
#endif
		else if(id == SK_RSK)
			item->rect.x = 2;
#else
		if(wgt_test_attr(pme, SK_ATTR_BOTTOM)){
			item->rect.dx = client.dx/SK_TOTAL;
			item->rect.dy = client.dy;
			item->rect.y = 0;
			item->rect.x = item->rect.dx * id;
		}else{
		}
#endif
	}
	
#if defined(FTR_IPHONE_STYLE)
	dwFlags |= IDF_HALIGN_CENTER;
#else
	if(align == SK_TEXT_ALIGN_CENTER)
		dwFlags |= IDF_HALIGN_CENTER;
	else if(align == SK_TEXT_ALIGN_LEFT)
		dwFlags |= IDF_HALIGN_LEFT;
	else if(align == SK_TEXT_ALIGN_RIGHT)
		dwFlags |= IDF_HALIGN_RIGHT;
#endif
	dx = item->rect.dx;
	dy = item->rect.dy;
	x = item->rect.x;
	y = item->rect.y;
	x+= 1;
	y+= 1;
	dx-=2;
	dy-=2;
	
	if(!defer){
		gdi_clear(gdi, x, y, dx, dy, guiNoCorner);
	}

	gdi_set_color(gdi, ui_theme_get()->sk_fg_color);
	if(item->down){
		gdi_draw_text_in(gdi, x+1, y+1, dx-2, dy-2, str, -1, CHARSET_UTF8, dwFlags);
	}else{
		gdi_draw_text_in(gdi, x, y, dx, dy, str, -1, CHARSET_UTF8, dwFlags);
	}

#if defined(FTR_IPHONE_STYLE)
	gdi_set_color(gdi, ui_theme_get()->border_color);
	gdi_rectangle(gdi, item->rect.x, item->rect.y, item->rect.dx, item->rect.dy, guiFourCorner);
	if(item->down){
		//2 高亮显示
		gu32 old = gdi_set_color(gdi, 0x400000ff);
		gdi_rectangle_fill(gdi, item->rect.x, item->rect.y, item->rect.dx, item->rect.dy, guiFourCorner);
		gdi_set_color(gdi, old);
	}
#endif

	if(!defer)
		gdi_blt(gdi, item->rect.x, item->rect.y, item->rect.dx, item->rect.dy);
	g_object_unref(gdi);
}

#define __sk_set_status(pme, id, bdown)  do {\
	if(id < SK_TOTAL && SK_ITEM(pme, id)->down != bdown){ \
		SK_ITEM(pme, id)->down = bdown; \
		__sk_draw_item(pme, id, FALSE); \
	}}while(0)

#define SK_X_ID(pme, x)		((x)*SK_TOTAL/wgt_bound_dx(pme))

static const amos_key_t	gaSKKeys[SK_TOTAL] = {
	MVK_LSK
#if defined(FTR_CSK_ENABLED)
	, MVK_OK
#endif
	, MVK_RSK
};
static	int	SK_KEY_ID(t_SKBar *pme, amos_key_t key)
{
	int i;
	for(i = 0; i < SK_TOTAL; i++)
		if(gaSKKeys[i] == key)
			return i;
	return SK_TOTAL;
}

static error_t __sk_on_event(t_SKBar *pme, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnDraw:
			{
				gu8 i;
#if defined(FTR_IPHONE_STYLE)
				const char *title = wgt_get_caption(wgt_parent_window((t_HWidget)pme));
				if(title && title[0]){
					t_HGDI gdi = wgt_get_client_gdi((t_HWidget)pme);
					gdi_set_color(gdi, RGB_WHITE);
					gdi_draw_text_in(gdi, 0, 0, wgt_bound_dx(pme), wgt_bound_dy(pme), title, -1, CHARSET_UTF8, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
					g_object_unref(gdi);
				}
#endif
				for(i = 0; i < SK_TOTAL; i++)
					__sk_draw_item(pme, i, TRUE);
				ret = SUCCESS;
			}
			break;
		case WIDGET_OnKeyEvent: /* */
			{
				int id = SK_KEY_ID(pme, lparam);
				t_SKItem *item;
				if(id < 0 || id >= SK_TOTAL)
					break;
				
				item = SK_ITEM(pme, id);
				if(sparam == MMI_KEY_DOWN){
					__sk_set_status(pme, id, TRUE);
					ret = SUCCESS;
				}else if(sparam == MMI_KEY_UP){
					__sk_set_status(pme, id, FALSE);
					if(item->act_type == SK_ACT_WIDGET){
						if(g_object_valid(item->pointer)){
							if(wgt_is_visible(item->pointer))
								wgt_hide(item->pointer);
							else
							if(wgt_test_attr(pme, SK_ATTR_BOTTOM)){
								wgt_popup((t_HWidget)pme, item->pointer, item->rect.x, item->rect.y, guiTop|guiRight);
							}else{
								wgt_popup((t_HWidget)pme, item->pointer, item->rect.x, item->rect.y+item->rect.dy, guiBottom|guiRight);
							}
							ret = SUCCESS;
						}
					}else if(item->act_type == SK_ACT_CALLBACK){
						if(item->callback){
							item->callback((t_HWidget)pme, item->pointer);
							ret = SUCCESS;
						}
					}else if(item->act_type == SK_ACT_APP){
						pack_call(item->pointer, (t_AmsModuleId)((gu32)item->callback), NULL, 0, FALSE, NULL);
						ret = SUCCESS;
					}
				}else if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_LONG_PRESS){
					//2 之所以将sk事件处理放在press中而不是up中, 
					//2 是因为如果sk没有处理press事件的话, 在widget默认处理中会转换为OK键press消息
					//2 从而导致触发不该触发的按键处理.
					if(item->act_type == SK_ACT_WIDGET){
						if(g_object_valid(item->pointer)){
							ret = SUCCESS;
						}
					}else if(item->act_type == SK_ACT_CALLBACK){
						if(item->callback){
							ret = SUCCESS;
						}
					}else if(item->act_type == SK_ACT_APP){
						ret = SUCCESS;
					}
				}
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
					if(pme->sk_down >= 0)
					wgt_send_msg((t_HWidget)wgt_parent_window((t_HWidget)pme), EVT_KEY, MMI_KEY_PRESS, gaSKKeys[pme->sk_down], 0);
					ret = SUCCESS;
					break;
				case MMI_PEN_LONG_PRESS:
					//2 PenLongPress就表示没有PenMove
					if(pme->sk_down >= 0)
					wgt_send_msg((t_HWidget)wgt_parent_window((t_HWidget)pme), EVT_KEY, MMI_KEY_LONG_PRESS, gaSKKeys[pme->sk_down], 0);
					ret = SUCCESS;
					break;
				case MMI_PEN_DOWN:
					pme->sk_down = __sk_xy_id(pme, x0, y0);
					if(pme->sk_down >= 0){
						wgt_send_msg((t_HWidget)wgt_parent_window((t_HWidget)pme), EVT_KEY, MMI_KEY_DOWN, gaSKKeys[pme->sk_down], 0);
					}
					ret = SUCCESS;
					break;
				case MMI_PEN_UP:
					if(pme->sk_down >= 0){
						if(__sk_xy_id(pme, x0, y0) == pme->sk_down){
							wgt_send_msg((t_HWidget)wgt_parent_window((t_HWidget)pme), EVT_KEY, MMI_KEY_UP, gaSKKeys[pme->sk_down], 0);
						}
						ret = SUCCESS;
					}
					break;
				case MMI_PEN_MOVE_IN:
					if(pme->sk_down >= 0){
						int id = __sk_xy_id(pme, x0, y0);
						if(id == pme->sk_down){
							__sk_set_status(pme, pme->sk_down, TRUE);
						}
					}
					ret = SUCCESS;
					break;
				case MMI_PEN_MOVE_OUT:
					if(pme->sk_down >= 0){
						__sk_set_status(pme, pme->sk_down, FALSE);
					}
					ret = SUCCESS;
					break;
				case MMI_PEN_MOVE:
					if(pme->sk_down >= 0){
						int id = __sk_xy_id(pme, x0, y0);
						if(id == pme->sk_down){
							__sk_set_status(pme, pme->sk_down, TRUE);
						}else{
							__sk_set_status(pme, pme->sk_down, FALSE);
						}
					}
					ret = SUCCESS;
					break;
			}
			break;
		}
#endif
	}
	return ret;
}

static VTBL(widget) skVtbl = {
	(PFNONCREATE)__sk_on_create
	, (PFNONDESTROY)__sk_on_destroy

	, (WidgetOnEventHandler)__sk_on_event
	, NULL
};

CLASS_DECLARE(softkey, t_SKBar, skVtbl);


	
error_t	sk_set_text(t_SKHandle sk, gu8 id, const char *str, resid_t resid)
{
	error_t ret = EBADPARM;
	if(g_object_valid(sk) && id < SK_TOTAL){
		SK_ITEM(sk, id)->resid = resid;
		SK_ITEM(sk, id)->text[0] = '\0';
		SK_ITEM(sk, id)->rect.dx = 0;	//2 force to recalc the bound of SK
		SK_ITEM(sk, id)->rect.dy = 0;
		if(str){
			strncpy(SK_ITEM(sk, id)->text, str, SK_TEXT_MAX);
			SK_ITEM(sk, id)->text[SK_TEXT_MAX] = '\0';
		}
		ret = SUCCESS;
	}
	return ret;
}

void	sk_set_text_align(t_SKHandle sk, gu8 id, gu8 align)
{
	if(g_object_valid(sk) && id < SK_TOTAL && align < SK_TEXT_ALIGN_N){
		SK_ITEM(sk, id)->align = align;
	}
}

error_t	sk_set_callback(t_SKHandle sk, gu8 id, t_WidgetCallback cb, void *data)
{
	error_t ret = EBADPARM;
	if(g_object_valid(sk) && id < SK_TOTAL){
		SK_ACT_RESET(sk, id);
		
		SK_ITEM(sk, id)->act_type = SK_ACT_CALLBACK;
		SK_ITEM(sk, id)->callback = cb;
		SK_ITEM(sk, id)->pointer = data;

		ret = SUCCESS;
	}
	return ret;
}

error_t	sk_set_popup(t_SKHandle sk, gu8 id, t_HWidget h)
{
	error_t ret = EBADPARM;
	if(g_object_valid(sk) && id < SK_TOTAL){
		SK_ACT_RESET(sk, id);

		SK_ITEM(sk, id)->act_type = SK_ACT_WIDGET;
		SK_ITEM(sk, id)->pointer = h;
		if(g_object_valid(h)){
			SK_ITEM(sk, id)->act_type = SK_ACT_WIDGET;
			g_object_ref(h);
		}

		ret = SUCCESS;
	}
	return ret;
}

t_HWidget	sk_get_popup(t_HWidget sk, gu8 id)
{
	return (g_object_valid(sk) && id < SK_TOTAL)?SK_ITEM(sk, id)->pointer : NULL;
}

error_t	sk_set_app(t_HWidget sk, gu8 id, const char *guid, t_AmsModuleId modid)
{
	error_t ret = EBADPARM;
	if(g_object_valid(sk) && id < SK_TOTAL){
		SK_ACT_RESET(sk, id);

		SK_ITEM(sk, id)->act_type = SK_ACT_APP;
		SK_ITEM(sk, id)->pointer = (void*)guid;
		SK_ITEM(sk, id)->callback = (t_WidgetCallback)modid;

		ret = SUCCESS;
	}
	return ret;
}

#if defined(TARGET_WIN)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_softkey;
}

#endif


#undef PARENT_CLASS

#endif


