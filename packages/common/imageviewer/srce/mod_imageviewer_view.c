#include "pack_imageviewer.hi"

enum {
	IDX_PREV,
	IDX_CURR,
	IDX_NEXT,
};

typedef struct {
	int 	cursor;
	int		timer;
	gu32	anim:1;
	gu32	started:1;
		
	t_HMedia		media;
	t_HImage	image;
#if defined(FTR_TOUCH_SCREEN)
	t_HWidget	list;
	t_HMedia		media_prev, media_next;
	t_HImage	image_prev, image_next;
	coord_t down_x, down_y;
	coord_t prev_x, prev_y;
	gu32 down_tick;
	coord_t delta_x;
	gboolean	same_dir;
	MC_FILL_STRUCT1;
#endif
}t_ViewWndData;

enum {
	ACT_NONE,
	ACT_FIRST,
	ACT_PREV,
	ACT_NEXT,
	ACT_LAST,
};

static void	__load_image(t_HWindow handle, gu8 action)
{
	t_Global *glob = app_get_data();
	t_ViewWndData *wdata = (t_ViewWndData *)wgt_get_tag(handle);
	int cursor = wdata->cursor;
	t_HMedia *pm = &wdata->media;
	t_HImage *pi = &wdata->image;

#if defined(FTR_TOUCH_SCREEN)
	if(action == IDX_PREV){
		cursor--;
		pm = &wdata->media_prev;
		pi = &wdata->image_prev;
	}else if(action == IDX_NEXT){
		cursor++;
		pm = &wdata->media_next;
		pi = &wdata->image_next;
	}
#endif
	if(!g_object_valid(*pm)){
		char *fname = g_strlist_get(glob->fnames, cursor);
		if(fname){
			*pm = g_media_new_for_file(fname);
			if(g_object_valid(*pm))
				g_media_decode_image(*pm, pi);
		}
	}
	if(action != IDX_CURR)
		return;
	timer_stop(wdata->timer);
	wdata->started = FALSE;
	wdata->anim = FALSE;
#if defined(FTR_TOUCH_SCREEN)
	wdata->delta_x = 0;
#endif
	if(g_media_type(wdata->media) == IMG_FORMAT_GIF){
		t_MediaInfoGif *info = (t_MediaInfoGif*)g_media_info(wdata->media);
		if(info && info->frames > 1){
			int duration;
			duration = image_get_duration(wdata->media);
			if(duration <= 0)
				duration = 100;
			timer_start(wdata->timer, duration, TIMER_MODE_ONCE);
			wdata->anim = TRUE;
			wdata->started = TRUE;
		}
	}
}

static void	__display_image_LR(t_HGDI gdi, gu16 lcd_dx, gu16 lcd_dy, t_HImage left, t_HImage right, coord_t delta)
{
	int w, h, x, y;

	gdi_clear(gdi, 0, 0, lcd_dx, lcd_dy, 0);
	if(g_object_valid(left)){
		w = image_get_width(left);
		h = image_get_height(left);
		if(w <= lcd_dx && h <= lcd_dy){
			//2 无缩放
			x = (lcd_dx-w)/2;
			y = (lcd_dy-h)/2;
			if(x < delta){
				gdi_draw_image_at(gdi, 0, y, left, w-(delta-x), 0, delta-x, h, 1.0, 1.0);
			}
		}else if(h * lcd_dx >= w * lcd_dy){
			//2  根据height计算缩放比例
		}else{
			//2  根据width计算缩放比例
		}
	}
	delta = lcd_dx-delta;
	if(g_object_valid(right)){
		w = image_get_width(right);
		h = image_get_height(right);
		if(w <= lcd_dx && h <= lcd_dy){
			//2 无缩放
			x = (lcd_dx-w)/2;
			y = (lcd_dy-h)/2;
			if(x < delta){
				gdi_draw_image_at(gdi, lcd_dx-delta+x, y, right, 0, 0, delta-x, h, 1.0, 1.0);
			}
		}else if(h * lcd_dx >= w * lcd_dy){
			//2  根据height计算缩放比例
		}else{
			//2  根据width计算缩放比例
		}
	}
}

static void __display_image(t_HWindow handle, gboolean defer)
{
	t_Global *glob = app_get_data();
	t_ViewWndData *wdata = (t_ViewWndData *)wgt_get_tag(handle);
	t_HGDI gdi = wgt_get_gdi((t_HWidget)handle);
#if defined(FTR_TOUCH_SCREEN)
	if(wdata->delta_x == 0){
		//2  无滑动
		gdi_clear(gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle), 0);
		gdi_draw_image_in(gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle), wdata->image, IDF_VALIGN_BESTFIT|IDF_HALIGN_BESTFIT);
	}else{
		t_HImage left, right;

		if(wdata->delta_x > 0){
			//2 右滑
			__load_image(handle, IDX_PREV);
			__display_image_LR(gdi, wgt_bound_dx(handle), wgt_bound_dy(handle), wdata->image_prev, wdata->image, wdata->delta_x);
		}else{
			//2 左滑
			__load_image(handle, IDX_NEXT);
			__display_image_LR(gdi, wgt_bound_dx(handle), wgt_bound_dy(handle), wdata->image, wdata->image_next, wgt_bound_dx(handle)+wdata->delta_x);
		}
	}
#else
	gdi_draw_image_in(gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle), wdata->image, IDF_VALIGN_BESTFIT|IDF_HALIGN_BESTFIT);
#endif
	if(!defer){
		gdi_blt(gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
	}
	g_object_unref(gdi);
}

static void __view_wnd_action(t_HWindow handle, gu8 action, t_ViewWndData *wdata)
{
	t_Global *glob = app_get_data();
	int cursor = wdata->cursor;
	
	if(action == ACT_PREV){
		cursor--;
		if(cursor < 0)
			cursor = g_strlist_size(glob->fnames)-1;
	}else if(action == ACT_FIRST){
		cursor = 0;
	}else if(action == ACT_NEXT){
		cursor++;
		if(cursor >= g_strlist_size(glob->fnames))
			cursor = 0;
	}else if(action == ACT_LAST){
		cursor = g_strlist_size(glob->fnames)-1;
	}
	if(cursor != wdata->cursor){
#if defined(FTR_TOUCH_SCREEN)
		if(wdata->cursor >= 0){
			if(cursor == wdata->cursor+1){
				g_object_unref(wdata->image_prev);
				g_object_unref(wdata->media_prev);
				wdata->image_prev = wdata->image;
				wdata->media_prev = wdata->media;
				wdata->image = wdata->image_next;
				wdata->media = wdata->media_next;
				wdata->image_next = NULL;
				wdata->media_next = NULL;
				wdata->cursor = cursor;
			}else if(cursor == wdata->cursor-1){
				g_object_unref(wdata->image_next);
				g_object_unref(wdata->media_next);
				wdata->image_next = wdata->image;
				wdata->media_next = wdata->media;
				wdata->image = wdata->image_prev;
				wdata->media = wdata->media_prev;
				wdata->image_prev = NULL;
				wdata->media_prev = NULL;
				wdata->cursor = cursor;
			}
		}
		if(wdata->cursor != cursor){
			g_object_unref(wdata->image);
			g_object_unref(wdata->media);
			g_object_unref(wdata->image_prev);
			g_object_unref(wdata->media_prev);
			g_object_unref(wdata->image_next);
			g_object_unref(wdata->media_next);
		}
#else
		g_object_unref(wdata->image);
		g_object_unref(wdata->media);
#endif
		wdata->cursor = cursor;
		__load_image(handle, IDX_CURR);
		#if defined(FTR_TOUCH_SCREEN)
		wdata->delta_x = 0;
		#endif
		__display_image(handle, FALSE);
	}
}

#if defined(FTR_TOUCH_SCREEN)
enum {
	LIST_INDEX_BACK,
	LIST_INDEX_FORWARD,
	LIST_INDEX_DELETE,

	LIST_COUNT
};
static void	__view_list_redraw(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, t_ViewWndData *wdata)
{
	resid_t icons[LIST_COUNT] = {RESID_ICON_BACK, RESID_ICON_FORWARD, RESID_ICON_TRASH};
	t_HImage image;

	image = brd_get_image(brd_get(PACK_GUID_SELF), icons[index]);
	
	gdi_draw_image_in(gdi, 0, 0, width, height, image, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
	g_object_unref(image);
}

static error_t	__view_list_hook(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_Global *glob = app_get_data();
	t_HWindow win = wgt_parent_window(handle);
	t_ViewWndData *wdata = (t_ViewWndData*)wgt_get_tag(win);
	switch(sparam){
		case LIST_INDEX_BACK:
			wnd_send_close(win, 0);
			ret = SUCCESS;
			break;
		case LIST_INDEX_FORWARD:
			{
				char *fname = g_strlist_get(glob->fnames, wdata->cursor);
				char fullname[AMOS_PATH_MAX+1];
				char *p = strrchr(fname, '.');
				char mime[16] = "file/*";
				if(p){
					p++;
					if(strlen(p) < 6)
						strcpy(&mime[5], p);
				}

				sprintf(fullname, "%s/%s", fsi_getcwd(), fname);
				mime_service_exec(mime, NULL, PACK_GUID_SELF, fullname, strlen(fullname)+1, FALSE);
				ret = SUCCESS;
			}
			break;
		case LIST_INDEX_DELETE:
			if(msgbox_show(NULL, _LSK(YES)|_RSK(CANCEL), 0, 0, brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_DEL, "")) == MBB_YES){
			}
			ret = SUCCESS;
			break;
	}
	return ret;
}
#endif

static void	__timer_callback(int timerid, t_HWindow handle)
{
	t_ViewWndData *wdata = (t_ViewWndData *)wgt_get_tag(handle);
#if defined(FTR_TOUCH_SCREEN)
	if(wgt_is_visible(wdata->list))return;
#endif
	if(wdata->anim && wdata->started){
		t_HImage image = NULL;
		gu32 delta;
		int ret;

		delta = tick_current();
		ret = g_media_decode_image(wdata->media, &image);
		if(ret != SUCCESS){
			g_media_seek(wdata->media, 0, SEEK_SET);
			ret = g_media_decode_image(wdata->media, &image);
		}
		delta = tick_current() - delta;
		if(ret == SUCCESS){
			int duration;
			duration = image_get_duration(image);
			if(duration <= 0)
				duration = 100;
			timer_start(wdata->timer, duration, TIMER_MODE_ONCE);
			g_object_unref(wdata->image);
			wdata->image = image;
			__display_image(handle, FALSE);
		}
	}
}

//2 图片浏览界面全屏显示，滑动或左右键切换照片
//2 触屏点击或ok键在屏幕底部弹出操作菜单: 
error_t	imageviewer_ViewWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_Global *glob = app_get_data();
	t_ViewWndData *wdata = (t_ViewWndData *)wgt_get_tag(handle);
	
	switch(evt){
		case WINDOW_OnOpen:
			//2 全屏显示
#if !defined(FTR_TOUCH_SCREEN)
			wgt_enable_attr(handle, WND_ATTR_AUTO_CLOSE);
#endif
			wdata = MALLOC(sizeof(t_ViewWndData));
			memset(wdata, 0, sizeof(t_ViewWndData));
			wdata->cursor = sparam;
			wdata->timer = timer_create((TimerCallback)__timer_callback, handle);
			wgt_set_tag(handle, wdata);

			//wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			wgt_set_bg_fill_solid(handle, RGB_BLACK);

			amos_pen_enable_slide(FALSE);

#if defined(FTR_TOUCH_SCREEN)
			wdata->list = wgt_create(handle, CLSID_LISTCTRL);
			wgt_disable_attr(wdata->list, CTRL_ATTR_VISIBLE);
			//wgt_enable_attr(wdata->list, POPUP_ATTR_DISABLE_SELECT_HIDE|POPUP_ATTR_BORDER_COLOR);
			wgt_set_bound(wdata->list, 0, 0, LCD_WIDTH, 40);
			wgt_set_border(wdata->list, BORDER_TYPE_NONE, 1, guiNoCorner);
			wgt_set_bg_fill(wdata->list, GDI_FILL_MODE_2COLOR_H, 0x80202020, 0x80101010);
			wgt_hook_register(wdata->list, WIDGET_OnSelected, __view_list_hook, NULL);
			listctrl_set_flags(wdata->list, listctrl_get_flags(wdata->list)|CLFLAG_HILIGHT_SHOW_MASK|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE);
			listctrl_set_viewtype(wdata->list, GUI_VIEWTYPE_NORMAL);
			listctrl_set_rows(wdata->list, 1, 0, 0);
			listctrl_set_cols(wdata->list, LIST_COUNT, 0);
			listctrl_attach(wdata->list, (listctrl_redraw_func_t)__view_list_redraw, wdata);
			listctrl_set_count(wdata->list, LIST_COUNT);
#endif

			__load_image(handle, IDX_CURR);
			break;
		case WINDOW_OnClose:
			if(wdata){
#if defined(FTR_TOUCH_SCREEN)
				g_object_unref(wdata->image_prev);
				g_object_unref(wdata->media_prev);
				g_object_unref(wdata->image_next);
				g_object_unref(wdata->media_next);
#endif
				g_object_unref_0(wdata->image);
				g_object_unref_0(wdata->media);
				timer_release(wdata->timer);
				FREE(wdata);
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			amos_pen_enable_slide(FALSE);
			if(wdata->anim && !wdata->started){
				timer_start(wdata->timer, 10, TIMER_MODE_ONCE);
				wdata->started = TRUE;
			}
			break;
		case WIDGET_OnKillFocus:
			if(wdata->anim && wdata->started){
				timer_stop(wdata->timer);
				wdata->started = FALSE;
			}
			break;
		case WIDGET_OnPreDraw: // WIDGET_OnDraw
			{	//2 因为有控件显示，如果在WIDGET_OnDraw中显示会覆盖控件
				__display_image(handle, TRUE);
			}
			ret = SUCCESS;
			break;
#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
			{
				coord_t x0, y0;
				PEN_XY(lparam, x0, y0);
				switch(PEN_TYPE(lparam)){
					case MMI_PEN_DOWN:
						wdata->down_x = wdata->prev_x = x0;
						wdata->down_y = wdata->prev_y = y0;
						wdata->down_tick = tick_current();
						wdata->same_dir = TRUE;
						wdata->delta_x = 0;
						break;
					case MMI_PEN_MOVE:
						wgt_set_visible(wdata->list, FALSE);
						if(x0 != wdata->prev_x){
							if(wdata->delta_x < 0){
								if((x0-wdata->prev_x)>0)
									wdata->same_dir = FALSE;
							}else if(wdata->delta_x > 0){
								if((x0-wdata->prev_x)<0)
									wdata->same_dir = FALSE;
							}
							wdata->delta_x += x0-wdata->prev_x;
							__display_image(handle, FALSE);
						}
						wdata->prev_x = x0;
						wdata->prev_y = y0;
						break;
					case MMI_PEN_UP:
						if(wdata->delta_x == 0)
							break;
						if(wdata->same_dir && tick_current()-wdata->down_tick<=600){
							__view_wnd_action(handle, (wdata->delta_x>0)?ACT_PREV:ACT_NEXT, wdata);
						}else if(wdata->delta_x >= (wgt_bound_dx(handle)/2))
							__view_wnd_action(handle, ACT_PREV, wdata);
						else if(wdata->delta_x <= -(wgt_bound_dx(handle)/2))
							__view_wnd_action(handle, ACT_NEXT, wdata);
						else{
							wdata->delta_x = 0;
							__display_image(handle, FALSE);
						}
						break;
					case MMI_PEN_PRESS:
						if(wgt_is_visible(wdata->list)){
							if(wdata->anim && !wdata->started){
								timer_start(wdata->timer, 10, TIMER_MODE_ONCE);
								wdata->started = TRUE;
							}
							wgt_set_visible(wdata->list, FALSE);
						}else{
							if(wdata->anim && wdata->started){
								timer_stop(wdata->timer);
								wdata->started = FALSE;
							}
							wgt_set_visible(wdata->list, TRUE);
						}
						break;
				}
				ret = SUCCESS;
			}
			break;
#endif
		case WIDGET_OnKeyEvent:
			if(lparam == MVK_LEFT){
				if(sparam == MMI_KEY_PRESS)
					__view_wnd_action(handle, ACT_PREV, wdata);
				else if(sparam == MMI_KEY_LONG_PRESS)
					__view_wnd_action(handle, ACT_FIRST, wdata);
				ret = SUCCESS;
			}else if(lparam == MVK_RIGHT){
				if(sparam == MMI_KEY_PRESS)
					__view_wnd_action(handle, ACT_NEXT, wdata);
				else if(sparam == MMI_KEY_LONG_PRESS)
					__view_wnd_action(handle, ACT_LAST, wdata);
				ret = SUCCESS;
#if defined(FTR_TOUCH_SCREEN)
			}else if(lparam == MVK_OK){
				if(sparam == MMI_KEY_PRESS){
					if(wgt_is_visible(wdata->list)){
						if(wdata->anim && !wdata->started){
							timer_start(wdata->timer, 10, TIMER_MODE_ONCE);
							wdata->started = TRUE;
						}
						wgt_set_visible(wdata->list, FALSE);
					}else{
						if(wdata->anim && wdata->started){
							timer_stop(wdata->timer);
							wdata->started = FALSE;
						}
						wgt_set_visible(wdata->list, TRUE);
					}
				}
				ret = SUCCESS;
#endif
			}
			break;
	}
	return ret;
}

