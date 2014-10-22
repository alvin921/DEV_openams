//1 此控件用于iphone滑动解锁、关机、接听电话

#include "pack_desktop.hi"



#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)

//2 滑动区域
#define	SLIDER_START	8
#define	SLIDER_END		231

typedef void	(*t_SliderNotifyFunc)(void *userdata);
typedef struct {
	gu8		type;
	
	MC_FILL_STRUCT3;
	t_SliderNotifyFunc	func;
	void		*userdata;
	
	t_HImage	bg;
	coord_t	bg_x, bg_y;
	int		bg_width;
	int		bg_height;

	t_HImage	slider;
	int		slider_width;
	int		slider_height;

	coord_t	slider_x;
	coord_t	down_x;		//2 -1表示没按下
}t_CSlider;

static void __slider_redraw(t_HWidget handle, t_CSlider *pme, gboolean defer)
{
	t_HGDI gdi = NULL;
	const char *text;

	if(pme->type == SLIDER_NONE)
		return;
	#if 0
	if(pme->type == SLIDER_TO_UNLOCK)
		text = brd_get_string();
	else if(pme->type == SLIDER_TO_POWEROFF)
		text = brd_get_string;
	else
		;
	#endif
	gdi = wgt_get_gdi((t_HWidget)handle);
	if(!gdi)
		return;

	if(g_object_valid(pme->bg)){
		gdi_draw_image_at(gdi, pme->bg_x, pme->bg_y, pme->bg, 0, 0, pme->bg_width, pme->bg_height, 1.0, 1.0);
		gdi_draw_image_at(gdi, pme->bg_x+SLIDER_START+pme->slider_x, pme->bg_y+(pme->bg_height-pme->slider_height)/2, pme->slider, 0, 0, pme->slider_width, pme->slider_height, 1.0, 1.0);
		if(!defer)
			gdi_blt(gdi, pme->bg_x, pme->bg_y, pme->bg_width, pme->bg_height);
		//if(!defer)
		//	gdi_blt(gdi, 0, 0, wgt_bound_dx(pme), wgt_bound_dy(pme));
	}
	g_object_unref(gdi);
}

static error_t __slider_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_CSlider *pme = (t_CSlider *)wgt_get_tag(handle);
	switch(evt){
		case WIDGET_OnClose:
			g_object_unref(pme->bg);
			g_object_unref(pme->slider);
			FREE(pme);
			ret = SUCCESS;
			break;
		case WIDGET_OnDraw:
			{
				if(wgt_test_attr(handle, CTRL_ATTR_MODIFIED)){
					pme->bg_x = (wgt_bound_dx(handle)-pme->bg_width)/2;
					pme->bg_y = (wgt_bound_dy(handle)-pme->bg_height)/2;
					if(pme->bg_x < 0)
						pme->bg_x = 0;
					if(pme->bg_y < 0)
						pme->bg_y = 0;
				}
				__slider_redraw(handle, pme, TRUE);
				ret = SUCCESS;
			}
			break;

#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
			{
				coord_t x0, y0;
				PEN_XY(lparam, x0, y0);
				switch(PEN_TYPE(lparam)){
					case MMI_PEN_DOWN:
						if(pme->type == SLIDER_NONE)
							break;
						if(x0 > pme->bg_x+SLIDER_START && x0 < pme->bg_x+SLIDER_END &&
							y0 > pme->bg_y && x0 < pme->bg_y+pme->slider_height){
							pme->down_x = x0;
						}
						ret = SUCCESS;
						break;
					case MMI_PEN_MOVE:
						if(pme->down_x >= 0 && x0 != pme->down_x){
							coord_t curr = x0 - pme->down_x;
							if(curr < 0)
								curr = 0;
							else if(curr > SLIDER_END-SLIDER_START-pme->slider_width)
								curr = SLIDER_END-SLIDER_START-pme->slider_width;
							if(pme->slider_x != curr){
								pme->slider_x = curr;
								__slider_redraw(handle, pme, FALSE);
							}
						}
						ret = SUCCESS;
						break;
					case MMI_PEN_UP:
						if(pme->down_x >= 0){
							if(pme->slider_x == SLIDER_END-SLIDER_START-pme->slider_width){
								if(pme->type == SLIDER_TO_UNLOCK)
									app_exit(0);
								else if(pme->type == SLIDER_TO_POWEROFF)
									ams_shutdown();
								else if(pme->func){
									wnd_send_close(wgt_parent_window(handle), 0);
									pme->func(pme->userdata);
								}
							}else{
								pme->slider_x = 0;
								__slider_redraw(handle, pme, FALSE);
							}
							pme->down_x = -1;
						}
						ret = SUCCESS;
						break;
					default:
						ret = SUCCESS;
						break;
					}
			}
			break;
#endif
	}
	return ret;
}


t_HWidget		wgt_slider_create(t_HWidget parent, gu8 type)
{
	if(type > SLIDER_NONE && type <= SLIDER_CUSTOM){
		t_HWidget widget = wgt_create(parent, NULL);
		t_CSlider *pme = (t_CSlider*)MALLOC(sizeof(t_CSlider));
		memset(pme, 0, sizeof(t_CSlider));
		pme->type = type;
		pme->bg = brd_get_image(BRD_HANDLE_SELF, RESID_IMG_UNLOCK_BG);
		pme->bg_width = image_get_width(pme->bg);
		pme->bg_height = image_get_height(pme->bg);

		if(type == SLIDER_TO_POWEROFF)
			pme->slider = brd_get_image(BRD_HANDLE_SELF, RESID_IMG_POWEROFF_SLIDER);
		else
			pme->slider = brd_get_image(BRD_HANDLE_SELF, RESID_IMG_UNLOCK_SLIDER);
		pme->slider_width = image_get_width(pme->slider);
		pme->slider_height = image_get_height(pme->slider);
		pme->slider_x = 0;
		pme->down_x = -1;
		wgt_set_tag(widget, pme);

		wgt_hook_register(widget, WIDGET_OnDraw|WIDGET_OnPenEvent|WIDGET_OnClose, __slider_OnEvent, NULL);
		return widget;
	}
	return NULL;
}

