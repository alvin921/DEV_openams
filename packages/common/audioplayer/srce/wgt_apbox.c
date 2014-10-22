	
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

#if 0	
#define DLL_EXPORT_apbox
#endif

#include "wgt_apbox.h"
#include "pack_audioplayer.hi"

/*===========================================================================

DEFINITIONS

===========================================================================*/

//1 !!!PARENT_CLASS MUST be defined here and undefined at EOF
#undef PARENT_CLASS
#define PARENT_CLASS	(&class_widget)


typedef struct {
	t_Widget		common;
	// TODO: add your own members here
	t_MMCHandle	mmc;
	t_ApboxConfig	*attr;
	apbox_get_song_name_callback_t cb_name;
	apbox_state_change_callback_t cb_change;
	mplayer_callback_t cb_finish;
	void *userdata;

	char *fname;
	int	timer;
	audio_handle_t	dev;
	gu32	tim_tick;
	gu32	duration;
	gs32		time;
	t_MediaFormat	format;
	gu8		ap_state;
	gs8		region_down;
	gu16	repeat:2;	// none, all, one, shuffle
	gu16	if_bg:1;
	gu16	d_prev:1;	// button(prev) down
	gu16	d_play:1;
	gu16	d_next:1;
	gu16	volume:4;
	gu16	client:1;		//2 client区域是否可见，client区域用于显示歌词或视频
	gu16	fullscreen:1;	//2  全屏模式
	gu16	playing_set_focus:1;
	gu16	reserved:3;

	t_HImage	img_bg;

	t_HImage	img_prg[3];	// 0(bg), 1(fill), 2(indicator)
	//t_HImage	img_prg_bg;
	//t_HImage	img_prg_fill;
	//t_HImage	img_prg_indicator;
	
	t_HImage	img_vol[3];	// 0(bg), 1(fill), 2(indicator)
	//t_HImage	img_vol_bg;
	//t_HImage	img_vol_fill;
	//t_HImage	img_vol_indicator;

	t_HImage	img_prev;
	t_HImage	img_next;
	t_HImage	img_play;
	t_HImage	img_pause[2];
	t_HImage	img_repeat[AP_REPEAT_N];// REPEAT_NONE, _ONE, _ALL
	t_HImage	img_detail[2];
	
	gu16	prg_fill_dx;
	gu16	prg_fill_dy;
	gu16	prg_ind_dx;
	gu16	prg_ind_dy;
	
	gu16	vol_fill_dx;
	gu16	vol_fill_dy;
	gu16	vol_ind_dx;
	gu16	vol_ind_dy;

	gu16	orig_dx, orig_dy;

	t_HWidget	wgt_output;
}apbox_t;



#define	APBOX_STOP(pme)	do { \
	if(((apbox_t*)(pme))->ap_state != AP_STATE_IDLE){ \
		mmc_stop(((apbox_t*)(pme))->mmc); \
		((apbox_t*)(pme))->time = 0; \
		((apbox_t*)(pme))->ap_state = AP_STATE_IDLE; \
		timer_stop(((apbox_t*)(pme))->timer); \
		if(((apbox_t*)(pme))->cb_change) \
			((apbox_t*)(pme))->cb_change(((apbox_t*)(pme))->userdata, AP_STATE_IDLE, 0); \
	} \
}while(0)

#define	APBOX_START_TIMER(pme)	timer_start(((apbox_t*)(pme))->timer, 1000, TIMER_MODE_PERIOD)
#define	APBOX_STOP_TIMER(pme)	timer_stop(((apbox_t*)(pme))->timer)

/*===========================================================================

Class Definitions

===========================================================================*/


#define PLAY_FF_TIME	4000
static gu8 __apbox_get_region(apbox_t *pme, coord_t x, coord_t y)
{
	int i;
	for(i = 0; i < APBOX_REGION_N; i++)
		if(point_in_rect(x, y, &pme->attr->rect[i]))
			return i;
	return -1;
}


static char * path_last(char *path)
{
	char *p = NULL;
	if(path){
		p = path+strlen(path)-1;
		while(p > path && (*p == '/' || *p == '\\'))
			*p-- = 0;
		while(p > path && (*p != '/' && *p != '\\')){
			p--;
		}
		if(*p == '/' || *p == '\\')p++;
	}
	return p;
}

#define HILIGHT_COLOR	MAKE_ARGB(0x40, 0x00, 0x00, 0xff)
#define DRAW_HILIGHT(gdi, _x, _y, _dx, _dy) do{ \
						gu32 __old = gdi_set_color(gdi, HILIGHT_COLOR); \
						gdi_rectangle_fill(gdi, _x, _y, _dx, _dy, guiNoCorner); \
						gdi_set_color(gdi, __old); \
					}while(0)
					
static void	__apbox_show_region(apbox_t *pme, t_HGDI gdi, gu8 region, gboolean defer)
{
	gui_rect_t *prc;
	t_HImage image;

	if(g_object_valid(gdi))
		g_object_ref(gdi);
	else
		gdi = wgt_get_gdi((t_HWidget)pme);

	if(!g_object_valid(gdi))return;
	prc = &pme->attr->rect[region];
	if(!defer)
		gdi_clear(gdi, prc->x, prc->y, prc->dx, prc->dy, guiFourCorner);
	switch(region){
		case APBOX_REGION_PROGRESS:
			{
				gu32 width;
				
				if(pme->ap_state != AP_STATE_IDLE)
					width = pme->attr->prog_width*pme->time/pme->duration;
				else
					width = 0;
				gdi_draw_image_at(gdi, prc->x, prc->y, pme->img_prg[0], 0, 0, -1, -1, 1.0, 1.0);
				gdi_draw_image_in(gdi, prc->x+pme->attr->prog_start, prc->y+(prc->dy-pme->prg_fill_dy)/2, width, pme->prg_fill_dy, pme->img_prg[1], IDF_HALIGN_STRETCH);
				gdi_draw_image_at(gdi, prc->x+pme->attr->prog_start+width-pme->prg_ind_dx/2, prc->y+(prc->dy-pme->prg_ind_dy)/2, pme->img_prg[2], 0, 0, -1, -1, 1.0, 1.0);
			}
			break;
		case APBOX_REGION_VOLUME:
			{
				gu32 width;
				
				width = pme->attr->vol_width*pme->volume/VOLUME_LEVEL_MAX;
				gdi_draw_image_at(gdi, prc->x, prc->y, pme->img_vol[0], 0, 0, -1, -1, 1.0, 1.0);
				gdi_draw_image_in(gdi, prc->x+pme->attr->vol_start, prc->y+(prc->dy-pme->vol_fill_dy)/2, width, pme->vol_fill_dy, pme->img_vol[1], IDF_HALIGN_STRETCH);
				gdi_draw_image_at(gdi, prc->x+pme->attr->vol_start+width-pme->vol_ind_dx/2, prc->y+(prc->dy-pme->vol_ind_dy)/2, pme->img_vol[2], 0, 0, -1, -1, 1.0, 1.0);
			}
			break;
#if !defined(FTR_APBOX_TIME_PAST_REMAIN)
		case APBOX_REGION_TIME:
			if(pme->time >= 0){
				char str[20];
				
				if(pme->duration >= 60*60){
					sprintf(str, "%02d:%02d:%02d/%02d:%02d:%02d", 
						pme->time/60/60, (pme->time/60)%60, (pme->time)%60,
						pme->duration/60/60, (pme->duration/60)%60, (pme->duration)%60);
				}else{
					sprintf(str, "%02d:%02d/%02d:%02d", 
						pme->time/60, (pme->time)%60,
						pme->duration/60, (pme->duration)%60);
				}
				gdi_set_color(gdi, RGB_BLACK);
				gdi_draw_text_in(gdi, prc->x, prc->y, prc->dx, prc->dy, str, -1, CHARSET_US_ASCII, IDF_HALIGN_RIGHT|IDF_VALIGN_MIDDLE);
			}
			break;
#else		
		case APBOX_REGION_TIME_PAST:
			if(pme->time >= 0){
				char str[12];
				gu32 tmp = pme->time/1000;
				if(tmp >= 60*60){
					sprintf(str, "%02d:%02d:%02d", tmp/60/60, (tmp/60)%60, tmp%60);
				}else{
					sprintf(str, "%02d:%02d", tmp/60, tmp%60);
				}
				gdi_set_color(gdi, RGB_BLACK);
				//gdi_set_color(gdi, ui_theme_get()->fg_color);
				gdi_draw_text_in(gdi, prc->x, prc->y, prc->dx, prc->dy, str, -1, CHARSET_US_ASCII, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
			}
			break;
		case APBOX_REGION_TIME_REMAIN:
			if(pme->duration >= 0){
				char str[12];
				gs32 tmp = pme->duration;
				if(pme->time > 0)
					tmp -= pme->time;
				if(tmp < 0)tmp = 0;
				tmp /= 1000;
				if(tmp >= 60*60){
					sprintf(str, "%02d:%02d:%02d", tmp/60/60, (tmp/60)%60, tmp%60);
				}else{
					sprintf(str, "%02d:%02d", tmp/60, tmp%60);
				}
				gdi_set_color(gdi, RGB_BLACK);
				//gdi_set_color(gdi, ui_theme_get()->fg_color);
				gdi_draw_text_in(gdi, prc->x, prc->y, prc->dx, prc->dy, str, -1, CHARSET_US_ASCII, IDF_HALIGN_RIGHT|IDF_VALIGN_MIDDLE);
			}
			break;
#endif
		case APBOX_REGION_BUTTON_PLAY_PAUSE:
			if(pme->ap_state == AP_STATE_PLAYING){
				//2 playing状态显示pause按键
				if(pme->d_play && pme->if_bg)
					image = pme->img_pause[1];
				else
					image = pme->img_pause[0];
			}else{
				//2 非playing状态则显示play按键
				if(pme->d_play || !pme->if_bg)
					image = pme->img_play;
				else
					image = NULL;	//2 表示要清除，恢复背景显示
			}
			if(image){
				gdi_draw_image_in(gdi, prc->x, prc->y, prc->dx, prc->dy, image, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
				if(pme->d_play && !pme->if_bg){
					gu32 old = gdi_set_color(gdi, HILIGHT_COLOR);
					gdi_rectangle_fill(gdi, prc->x, prc->y, prc->dx, prc->dy, guiNoCorner);
					gdi_set_color(gdi, old);
				}
			}
				
			break;
		case APBOX_REGION_BUTTON_PREV:
			if(pme->d_prev || !pme->if_bg){
				image = pme->img_prev;
				if(image){
					gdi_draw_image_in(gdi, prc->x, prc->y, prc->dx, prc->dy, image, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
					if(pme->d_prev && !pme->if_bg)
						DRAW_HILIGHT(gdi, prc->x, prc->y, prc->dx, prc->dy);
				}
			}
			break;
		case APBOX_REGION_BUTTON_NEXT:
			if(pme->d_next || !pme->if_bg){
				image = pme->img_next;
				if(image){
					gdi_draw_image_in(gdi, prc->x, prc->y, prc->dx, prc->dy, image, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
					if(pme->d_next && !pme->if_bg)
						DRAW_HILIGHT(gdi, prc->x, prc->y, prc->dx, prc->dy);
				}
			}
			break;
		case APBOX_REGION_SONG:
			{
				char *ptr = path_last(pme->fname);
				gdi_set_color(gdi, RGB_BLACK);
				gdi_draw_text_in(gdi, prc->x, prc->y, prc->dx, prc->dy, ptr, -1, CHARSET_UNDEF, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
			}
			break;
		case APBOX_REGION_REPEAT:
			if(pme->repeat){
				image = pme->img_repeat[pme->repeat];
				if(image){
					gdi_draw_image_in(gdi, prc->x, prc->y, prc->dx, prc->dy, image, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
				}
			}
			break;
		case APBOX_REGION_DETAIL:
			if(1){
				#if defined(FTR_APBOX_BOTTOM)
				image = pme->img_detail[!pme->client];
				#else
				image = pme->img_detail[pme->client];
				#endif
				if(image){
					gdi_draw_image_in(gdi, prc->x, prc->y, prc->dx, prc->dy, image, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
				}
			}
			break;
	}
	if(!defer)
		gdi_blt(gdi, prc->x, prc->y, prc->dx, prc->dy);
	g_object_unref(gdi);
}

static void	__timer_callback(int timerid, apbox_t *pme)
{
	if(pme->ap_state != AP_STATE_IDLE){
		gu32 tick = tick_current();
#if 0
		pme->time = mmc_tell(pme->mmc);
		if(pme->time >= 0)
			pme->time /= 1000;
#else
		pme->time += tick-pme->tim_tick;
		pme->tim_tick = tick;
#endif
#if defined(FTR_APBOX_TIME_PAST_REMAIN)
		__apbox_show_region(pme, NULL, APBOX_REGION_TIME_PAST, FALSE);
		__apbox_show_region(pme, NULL, APBOX_REGION_TIME_REMAIN, FALSE);
#else
		__apbox_show_region(pme, NULL, APBOX_REGION_TIME, FALSE);
#endif
		//2 之所以放在后面，是因为progress的显示做依赖的time在显示时才更新
		__apbox_show_region(pme, NULL, APBOX_REGION_PROGRESS, FALSE);
	}
}

static error_t		__apbox_output_hook(t_HWidget handle, t_WidgetEvent ev, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	apbox_t* pme = (apbox_t*)wgt_get_tag(handle);
	switch(ev){
		case WIDGET_OnDraw:
			if(!MEDIA_IS_VIDEO(pme->format)){
				t_HGDI gdi = wgt_get_gdi(handle);
				gdi_draw_text_in(gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle), pme->fname, -1, CHARSET_UNDEF, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
				wgt_show_border(handle, gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
				g_object_unref(gdi);
				ret = SUCCESS;
			}
			break;
#if 1
		case WIDGET_OnSetFocus:
			if(pme->playing_set_focus)
				mmc_enable_image_output(pme->mmc, TRUE);
			break;
		case WIDGET_OnKillFocus:
			//2 视频区域失去焦点，且apbox无焦点，则须暂停视频输出
			if(pme->ap_state == AP_STATE_PLAYING && !wgt_is_focused(pme) && MEDIA_IS_VIDEO(pme->format)){
				pme->playing_set_focus = TRUE;
				//apbox_pause_resume((t_HWidget)pme);
				mmc_enable_image_output(pme->mmc, FALSE);
			}else
				pme->playing_set_focus = FALSE;
			break;
#endif
	}
	return ret;
}

void __apbox_make_client_visible(apbox_t *pme, gboolean visible)
{
	//2 默认client区域是向下扩展
	gui_rect_t client;
	if(visible != pme->client){
		if(visible){
			if(!g_object_valid(pme->wgt_output)){
				pme->wgt_output = wgt_create(wgt_get_parent(pme), NULL);
				wgt_set_tag(pme->wgt_output, pme);
				wgt_set_border(pme->wgt_output, BORDER_TYPE_FRAME, 1, guiNoCorner);
				wgt_set_bg_fill_solid(pme->wgt_output, RGB_BLACK);
				wgt_disable_attr(pme->wgt_output, CTRL_ATTR_VISIBLE);
				wgt_enable_attr(pme->wgt_output, CTRL_ATTR_TABSTOP);
				//wgt_enable_attr(pme->wgt_output, POPUP_ATTR_DISABLE_AUTO_HIDE|POPUP_ATTR_DISABLE_SELECT_HIDE|POPUP_ATTR_BORDER_COLOR);
				wgt_hook_register(pme->wgt_output, WIDGET_OnDraw|WIDGET_OnSetFocus|WIDGET_OnKillFocus, __apbox_output_hook, NULL);
			}
			wgt_get_client_rect(wgt_get_parent(pme), &client);
#if 0
			wgt_set_bound(pme->wgt_output, 0, wgt_position_y(pme)+wgt_bound_dy(pme), pme->orig_dx, client.dy - wgt_position_y(pme)-wgt_bound_dy(pme));
			wgt_set_visible(pme->wgt_output, TRUE);
#else
#if defined(FTR_APBOX_BOTTOM)
			wgt_set_bound(pme->wgt_output, 0, 0, client.dx, client.dy - wgt_bound_dy(pme));
			wgt_show(pme->wgt_output);
			wgt_set_focus(pme->wgt_output, TAB_NONE);
			//wgt_popup((t_HWidget)pme, pme->wgt_output, 0, 0, guiTop|guiRight);
#else
			wgt_set_bound(pme->wgt_output, 0, wgt_bound_dy(pme), client.dx, client.dy - wgt_bound_dy(pme));
			wgt_show(pme->wgt_output);
			wgt_set_focus(pme->wgt_output, TAB_NONE);
			//wgt_popup((t_HWidget)pme, pme->wgt_output, 0, wgt_bound_dy(pme), guiBottom|guiRight);
#endif
#endif
		}else{
			wgt_hide(pme->wgt_output);
		}
		pme->client = visible;
	}
	if(visible && MEDIA_IS_VIDEO(pme->format)){
		t_HGDI gdi;
		gdi = wgt_get_gdi(pme->wgt_output);
		rect_init(&client, 0, 0, wgt_bound_dx(pme->wgt_output), wgt_bound_dy(pme->wgt_output));
		mmc_set_image_output(pme->mmc, gdi, &client);
		g_object_unref(gdi);
	}
	mmc_enable_image_output(pme->mmc, visible);
}

static void	__apbox_finish_callback(gs16 errcode, void *userdata)
{
	t_HWidget apbox = userdata;
	apbox_stop(apbox, TRUE);

	__apbox_make_client_visible(apbox, FALSE);
	if(errcode == EEOF){
		apbox_play(apbox, AP_PLAY_CURRENT);
	}else{
		msgbox_show(NULL, _CSK(OK), 0, 0, "Error:(%d)", errcode);
	}
}

static error_t		__apbox_OnCreate(apbox_t* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	error_t ret;
	//2 NOTICE: attr MUST be set first since specific initialization should be done
	wgt_enable_attr(pme, CTRL_ATTR_TABSTOP);
	ret = g_object_inherit_create(pme, d1, d2, d3, d4);

	// TODO: add your own initializing codes here
	pme->attr = NULL;
	pme->timer = timer_create((TimerCallback)__timer_callback, pme);
	pme->ap_state = AP_STATE_IDLE;
	pme->duration = 0;
	pme->time = 0;
	pme->tim_tick = 0;
	pme->volume = VOLUME_LEVEL_4;

	pme->dev = dev_audio_open(AUD_PRIORITY_MEDIUM);
	dev_audio_set_path(pme->dev, AUD_DEV_SPEAKER);
	dev_audio_set_volume(pme->dev, pme->volume);

	pme->mmc = mmc_open();
	mmc_set_audio_output(pme->mmc, pme->dev);
	mmc_set_callback(pme->mmc, __apbox_finish_callback, pme);
	pme->region_down = 0xff;
	return SUCCESS;
}

static void	__apbox_OnDestroy(apbox_t* pme)
{
	int i;

	g_object_unref(pme->wgt_output);
	apbox_stop((t_HWidget)pme, FALSE);
	
	if(pme->timer != -1){
		timer_release(pme->timer);
	}
	dev_audio_close(pme->dev);
	mmc_close(pme->mmc);
	FREEIF(pme->fname);

	g_object_unref(pme->img_bg);
	g_object_unref(pme->img_prg[0]);
	g_object_unref(pme->img_prg[1]);
	g_object_unref(pme->img_prg[2]);
	g_object_unref(pme->img_vol[0]);
	g_object_unref(pme->img_vol[1]);
	g_object_unref(pme->img_vol[2]);
	g_object_unref(pme->img_prev);
	g_object_unref(pme->img_next);
	g_object_unref(pme->img_play);
	g_object_unref(pme->img_pause[0]);
	g_object_unref(pme->img_pause[1]);

	for(i = 0; i < AP_REPEAT_N; i++)
		g_object_unref_0(pme->img_repeat[i]);
	g_object_unref(pme->img_detail[0]);
	g_object_unref(pme->img_detail[1]);

	g_object_inherit_destroy(pme);
}


#if defined(FTR_TOUCH_SCREEN)
static error_t	__apbox_pen_event(t_HWidget handle, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	apbox_t *pme = (apbox_t *)handle;
	coord_t x0, y0;
	gu8 r0;
	gui_rect_t *prc;
	
	PEN_XY(lparam, x0, y0);
	r0 = __apbox_get_region(pme, x0, y0);
	if(r0 != -1){
		prc = &pme->attr->rect[r0];
	}
		switch(PEN_TYPE(lparam)){
			case MMI_PEN_DOWN:
				if(r0 == APBOX_REGION_BUTTON_NEXT){
					pme->d_next = TRUE;
					__apbox_show_region(pme, NULL, r0, FALSE);
				}else if(r0 == APBOX_REGION_BUTTON_PREV){
					pme->d_prev = TRUE;
					__apbox_show_region(pme, NULL, r0, FALSE);
				}else if(r0 == APBOX_REGION_BUTTON_PLAY_PAUSE){
					pme->d_play = TRUE;
					__apbox_show_region(pme, NULL, r0, FALSE);
				}else if(r0 == APBOX_REGION_PROGRESS){
					if(pme->ap_state != AP_STATE_IDLE){
						pme->time = pme->duration*(x0-prc->x-pme->attr->prog_start)/pme->attr->prog_width;
						mmc_seek(pme->mmc, pme->time, SEEK_SET);
						__timer_callback(pme->timer, pme);
					}
				}else if(r0 == APBOX_REGION_VOLUME){
					pme->volume = VOLUME_LEVEL_MAX*(x0-prc->x-pme->attr->vol_start)/pme->attr->vol_width;
					dev_audio_set_volume(pme->dev, pme->volume);
					__apbox_show_region(pme, NULL, r0, FALSE);
				}else if(r0 == APBOX_REGION_REPEAT){
					pme->repeat++;
					if(pme->repeat >= AP_REPEAT_N)
						pme->repeat = AP_REPEAT_NONE;
					__apbox_show_region(pme, NULL, r0, FALSE);
				}else if(r0 == APBOX_REGION_DETAIL){
					__apbox_make_client_visible(pme, !pme->client);
					__apbox_show_region(pme, NULL, r0, FALSE);
				}
				pme->region_down = r0;
				ret = SUCCESS;
				break;
			case MMI_PEN_MOVE_OUT:
				r0 = -1;
			case MMI_PEN_UP:
				pme->d_prev = FALSE;
				pme->d_play = FALSE;
				pme->d_next = FALSE;
				if(pme->region_down == APBOX_REGION_BUTTON_PREV){
					if(r0 == pme->region_down){
						apbox_play(handle, AP_PLAY_PREV);
					}
					__apbox_show_region(pme, NULL, pme->region_down, FALSE);
				}else if(pme->region_down == APBOX_REGION_BUTTON_NEXT){
					if(r0 == pme->region_down){
						apbox_play(handle, AP_PLAY_NEXT);
					}
					__apbox_show_region(pme, NULL, pme->region_down, FALSE);
				}else if(pme->region_down == APBOX_REGION_BUTTON_PLAY_PAUSE){
					if(r0 == pme->region_down){
						if(pme->ap_state == AP_STATE_IDLE){
							apbox_play(pme, AP_PLAY_NONE);
						}else{
							apbox_pause_resume(pme);
						}
					}
					__apbox_show_region(pme, NULL, pme->region_down, FALSE);
				}
				pme->region_down = -1;
				ret = SUCCESS;
				break;
			case MMI_PEN_LONG_PRESS:
				if(pme->ap_state != AP_STATE_IDLE){
					if(pme->region_down == APBOX_REGION_BUTTON_NEXT){
						mmc_seek(pme->mmc, PLAY_FF_TIME, SEEK_CUR);
						pme->d_next = FALSE;
						__apbox_show_region(pme, NULL, pme->region_down, FALSE);
						amos_pen_reset();	//2 up消息将不会发送
					}else if(pme->region_down == APBOX_REGION_BUTTON_PREV){
						mmc_seek(pme->mmc, -PLAY_FF_TIME, SEEK_CUR);
						pme->d_prev = FALSE;
						__apbox_show_region(pme, NULL, pme->region_down, FALSE);
						amos_pen_reset();
					}
				}
				ret = SUCCESS;
				break;
			case MMI_PEN_PRESS:
				ret = SUCCESS;
				break;
			case MMI_PEN_MOVE_IN:
				break;
			case MMI_PEN_MOVE:
				break;
		}
	return ret;
}

#endif

#if !defined(FTR_NO_INPUT_KEY)
static error_t __apbox_OnPreKeyEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	apbox_t *pme = (apbox_t *)userdata;
	error_t ret = NOT_HANDLED;
	if(g_object_valid(pme)){
		if(lparam == pme->attr->key_prev){
			if(sparam == MMI_KEY_DOWN){
				pme->d_prev = TRUE;
				__apbox_show_region(pme, NULL, APBOX_REGION_BUTTON_PREV, FALSE);
			}else if(sparam == MMI_KEY_PRESS){
				apbox_play((t_HWidget)pme, AP_PLAY_PREV);
			}else if(sparam == MMI_KEY_LONG_PRESS){
				if(pme->ap_state != AP_STATE_IDLE)
					mmc_seek(pme->mmc, -PLAY_FF_TIME, SEEK_CUR);
			}else if(sparam == MMI_KEY_UP){
				pme->d_prev = FALSE;
				__apbox_show_region(pme, NULL, APBOX_REGION_BUTTON_PREV, FALSE);
			}
			ret = SUCCESS;
		}else if(lparam == pme->attr->key_next){
			if(sparam == MMI_KEY_DOWN){
				pme->d_next = TRUE;
				__apbox_show_region(pme, NULL, APBOX_REGION_BUTTON_NEXT, FALSE);
			}else if(sparam == MMI_KEY_PRESS){
				apbox_play((t_HWidget)pme, AP_PLAY_NEXT);
			}else if(sparam == MMI_KEY_LONG_PRESS){
				if(pme->ap_state != AP_STATE_IDLE)
					mmc_seek(pme->mmc, PLAY_FF_TIME, SEEK_CUR);
			}else if(sparam == MMI_KEY_UP){
				pme->d_next = FALSE;
				__apbox_show_region(pme, NULL, APBOX_REGION_BUTTON_NEXT, FALSE);
			}
			ret = SUCCESS;
		}else if(lparam == pme->attr->key_play){
			if(sparam == MMI_KEY_DOWN){
				pme->d_play = TRUE;
				__apbox_show_region(pme, NULL, APBOX_REGION_BUTTON_PLAY_PAUSE, FALSE);
			}else if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_LONG_PRESS){
				//2 之所以将OK事件处理放到PRESS中，是因为LSK的UP事件会因为在FILELIST界面未处理而转换为OK键的UP事件
				if(pme->ap_state == AP_STATE_IDLE){
					apbox_play(pme, AP_PLAY_NONE);
				}else{
					apbox_pause_resume(pme);
				}
				pme->d_play = FALSE;
				__apbox_show_region(pme, NULL, APBOX_REGION_BUTTON_PLAY_PAUSE, FALSE);
			}else if(sparam == MMI_KEY_UP){
			}
			ret = SUCCESS;
		}else if(lparam == pme->attr->key_vol_dec){
			if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
				if(pme->volume > VOLUME_LEVEL_0){
					pme->volume--;
					dev_audio_set_volume(pme->dev, pme->volume);
					__apbox_show_region(pme, NULL, APBOX_REGION_VOLUME, FALSE);
				}
			}
			ret = SUCCESS;
		}else if(lparam == pme->attr->key_vol_inc){
			if(sparam == MMI_KEY_PRESS || sparam == MMI_KEY_REPEAT){
				if(pme->volume < VOLUME_LEVEL_MAX){
					pme->volume++;
					dev_audio_set_volume(pme->dev, pme->volume);
					__apbox_show_region(pme, NULL, APBOX_REGION_VOLUME, FALSE);
				}
			}
			ret = SUCCESS;
		}else if(lparam == pme->attr->key_mode){
			if(sparam == MMI_KEY_DOWN){
				if(pme->repeat == AP_REPEAT_NONE)
					pme->repeat = AP_REPEAT_ALL;
				else if(pme->repeat == AP_REPEAT_ALL)
					pme->repeat = AP_REPEAT_ONE;
				else if(pme->repeat == AP_REPEAT_ONE)
					pme->repeat = AP_REPEAT_SHUFFLE;
				else
					pme->repeat = AP_REPEAT_NONE;
				__apbox_show_region(pme, NULL, APBOX_REGION_REPEAT, FALSE);
			}
			ret = SUCCESS;
		}else if(lparam == pme->attr->key_detail){
			if(sparam == MMI_KEY_DOWN){
				__apbox_make_client_visible(pme, !pme->client);
				__apbox_show_region(pme, NULL, APBOX_REGION_CLIENT, FALSE);
			}
			ret = SUCCESS;
		}
	}
	return ret;
}
#endif

static error_t __apbox_OnEvent(t_Widget *handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void*userdata)
{
	error_t ret = NOT_HANDLED;
	apbox_t *pme = (apbox_t *)handle;
	switch(evt){
		case WIDGET_OnDraw:
#if !defined(FTR_NO_INPUT_KEY)
			//2 apbox强制预处理key事件，因为控件初始化时未加入到parent中，只能放到这里注册，
			//2 以后如果有增加AddChildHook，可考虑移走
			wgt_hook_register((t_HWidget)wgt_parent_window(pme), WIDGET_OnPreKeyEvent, __apbox_OnPreKeyEvent, pme);
#endif
			if(1){
				t_HGDI gdi = wgt_get_gdi(handle);
				int i;
				for(i = 0; i < APBOX_REGION_N; i++)
					__apbox_show_region(pme, gdi, i, TRUE);
				g_object_unref(gdi);
				ret = SUCCESS;
			}
			break;

		case WIDGET_OnSetFocus:
#if 0
			if(pme->ap_state == AP_STATE_PLAYING){
				APBOX_START_TIMER(pme);
				mmc_enable_image_output(pme->mmc, TRUE);
			}
#endif
			break;
		case WIDGET_OnKillFocus:
#if 0
			if(pme->ap_state == AP_STATE_PLAYING){
				APBOX_STOP_TIMER(pme);
				mmc_enable_image_output(pme->mmc, FALSE);
			}
#endif
			break;
		case WIDGET_OnSelected:
			break;
			
#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
			ret = __apbox_pen_event(handle, sparam, lparam);
			break;
#endif
	}
	if(ret == NOT_HANDLED){
		ret = g_widget_inherit_OnEvent(handle, evt, sparam, lparam);
	}
	return ret;
}


static VTBL(widget) apboxVtbl = {
	(PFNONCREATE)__apbox_OnCreate
	, (PFNONDESTROY)__apbox_OnDestroy

	, __apbox_OnEvent
	, NULL
};

CLASS_DECLARE(apbox, apbox_t, apboxVtbl);


#if defined(TARGET_WIN) && defined(DLL_EXPORT_apbox)

__declspec(dllexport) t_GClass *DllGetWidgetClass(void)
{
	return &class_apbox;
}

#endif

// TODO: add functions for your own widget.  You MUST export them in dll.def


void	apbox_bind(t_HWidget handle, t_ApboxConfig *attr, apbox_get_song_name_callback_t cb_name, apbox_state_change_callback_t cb_change, void *userdata)
{
	if(g_object_valid(handle) && attr){
		apbox_t *pme = (apbox_t *)handle;
		BrdHandle brd = brd_get(PACK_GUID_SELF);
		int i;
		
		pme->attr = attr;
		pme->cb_name = cb_name;
		pme->cb_change = cb_change;
		pme->userdata = userdata;

		pme->img_bg = brd_get_image(brd, attr->resid[APBOX_IMG_BG]);
		if(pme->img_bg){
			pme->if_bg = TRUE;
		}else{
			pme->if_bg = FALSE;
		}
		pme->img_prg[0] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_PROGRESS], 0));
		pme->img_prg[1] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_PROGRESS], 1));
		pme->img_prg[2] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_PROGRESS], 2));
		pme->img_vol[0] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_VOLUME], 0));
		pme->img_vol[1] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_VOLUME], 1));
		pme->img_vol[2] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_VOLUME], 2));
		pme->img_prev = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_PREV], 0));
		pme->img_next = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_NEXT], 0));
		pme->img_play = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_PLAY], 0));
		pme->img_pause[0] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_PAUSE], 0));
		pme->img_pause[1] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_PAUSE], 1));
		
		for(i = 0; i < AP_REPEAT_N; i++)
			pme->img_repeat[i] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_REPEAT], i));
		pme->img_detail[0] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_DETAIL], 0));
		pme->img_detail[1] = brd_get_image(brd, MAKE_RESID2(attr->resid[APBOX_IMG_DETAIL], 1));

		pme->prg_fill_dx = image_get_width(pme->img_prg[1]);
		pme->prg_fill_dy = image_get_height(pme->img_prg[1]);
		pme->prg_ind_dx = image_get_width(pme->img_prg[2]);
		pme->prg_ind_dy = image_get_height(pme->img_prg[2]);
		pme->vol_fill_dx = image_get_width(pme->img_vol[1]);
		pme->vol_fill_dy = image_get_height(pme->img_vol[1]);
		pme->vol_ind_dx = image_get_width(pme->img_vol[2]);
		pme->vol_ind_dy = image_get_height(pme->img_vol[2]);
		
		wgt_set_bg_image(handle, pme->img_bg);
		if(g_object_valid(pme->img_bg))
			wgt_set_bg_fill_solid(handle, 0x00000000);	//2 全透
		else
			wgt_set_bg_fill_solid(handle, MAKE_RGB(0x67,0x9a,0xc0));
		if(pme->if_bg){
			pme->orig_dx = image_get_width(pme->img_bg);
			pme->orig_dy = image_get_height(pme->img_bg);
		}else{
			pme->orig_dx = attr->bg_width;
			pme->orig_dy = attr->bg_height;
		}
		wgt_set_extent(handle, pme->orig_dx, pme->orig_dy);
		pme->client = FALSE;
	}
}
error_t	apbox_play(t_HWidget handle, gu8 mode)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle) && mode <= AP_PLAY_NEXT){
		apbox_t *pme = (apbox_t *)handle;

		APBOX_STOP(handle);

		if(pme->repeat != AP_REPEAT_ONE){
			FREEIF(pme->fname);
			if(pme->cb_name)
				pme->fname = pme->cb_name(pme->userdata, mode, pme->repeat);
			if(pme->fname == NULL){
				ret = EBADPARM;
				goto out;
			}
		}
		if(mmc_play_file(pme->mmc, 1, pme->fname) != 0){
			msgbox_show(NULL, _CSK(OK), 0, 0, "ERROR:<%s>", pme->fname);
			ret = EBADPARM;
		}else{
			t_HMedia media = g_media_new_for_file(pme->fname);
			pme->time = 0;
			pme->duration = g_media_duration(media);
			pme->format = g_media_type(media);
			pme->ap_state = AP_STATE_PLAYING;
			if(wgt_is_visible(pme)){
				APBOX_START_TIMER(pme);
				if(MEDIA_IS_VIDEO(pme->format)){
					__apbox_make_client_visible(pme, TRUE);
				}else{
					__apbox_make_client_visible(pme, FALSE);
				}
				mmc_enable_image_output(pme->mmc, pme->client);
				wgt_redraw(handle, FALSE);
			}else{
				if(MEDIA_IS_VIDEO(pme->format)){
					mmc_enable_image_output(pme->mmc, FALSE);
				}
			}
			if(pme->cb_change)
				pme->cb_change(pme->userdata, pme->ap_state, 0);
			g_object_unref(media);
			pme->tim_tick = tick_current();
			ret = SUCCESS;
		}
//out:
		//if(ret != SUCCESS && pme->cb_finish)
		//	pme->cb_finish(ret, pme->userdata);
	}
out:
	return ret;
}

error_t	apbox_pause_resume(t_HWidget handle)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle)){
		apbox_t *pme = (apbox_t *)handle;
		if(pme->ap_state == AP_STATE_PLAYING){
			APBOX_STOP_TIMER(pme);
			mmc_pause(pme->mmc);
			pme->ap_state = AP_STATE_PAUSED;
		}else{
			APBOX_START_TIMER(pme);
			mmc_resume(pme->mmc);
			pme->tim_tick = tick_current();
			pme->ap_state = AP_STATE_PLAYING;
		}
		__apbox_show_region(pme, NULL, APBOX_REGION_BUTTON_PLAY_PAUSE, FALSE);
		if(pme->cb_change)
			pme->cb_change(pme->userdata, pme->ap_state, 0);
	}
	return ret;
}

error_t	apbox_stop(t_HWidget handle, gboolean redraw)
{
	error_t ret = EBADPARM;
	if(g_object_valid(handle)){
		APBOX_STOP(handle);
		if(redraw && wgt_is_visible(handle)){
			__apbox_show_region((apbox_t*)handle, NULL, APBOX_REGION_BUTTON_PLAY_PAUSE, FALSE);
			__apbox_show_region((apbox_t*)handle, NULL, APBOX_REGION_PROGRESS, FALSE);
#if defined(FTR_APBOX_TIME_PAST_REMAIN)
			__apbox_show_region((apbox_t*)handle, NULL, APBOX_REGION_TIME_PAST, FALSE);
			__apbox_show_region((apbox_t*)handle, NULL, APBOX_REGION_TIME_REMAIN, FALSE);
#else
			__apbox_show_region((apbox_t*)handle, NULL, APBOX_REGION_TIME, FALSE);
#endif
		}
		ret = SUCCESS;
	}
	return ret;
}

gu8	apbox_get_state(t_HWidget handle)
{
	return g_object_valid(handle)? ((apbox_t*)handle)->ap_state : AP_STATE_IDLE;
}

gu8	apbox_get_repeat(t_HWidget handle)
{
	return g_object_valid(handle)? ((apbox_t*)handle)->repeat : AP_REPEAT_NONE;
}

gu8	apbox_set_repeat(t_HWidget handle, gu8 repeat)
{
	gu8 old = AP_REPEAT_NONE;
	if(g_object_valid(handle) ){
		apbox_t *pme = (apbox_t *)handle;
		old = pme->repeat;
		pme->repeat = repeat;
		if(old != repeat){
			__apbox_show_region(pme, NULL, APBOX_REGION_REPEAT, FALSE);
		}
	}
	return old;
}

gu8	apbox_toggle_repeat(t_HWidget handle)
{
	if(g_object_valid(handle)){
		apbox_t *pme = (apbox_t *)handle;
		if(pme->repeat == AP_REPEAT_NONE)
			pme->repeat = AP_REPEAT_ALL;
		else if(pme->repeat == AP_REPEAT_ALL)
			pme->repeat = AP_REPEAT_ONE;
		else if(pme->repeat == AP_REPEAT_ONE)
			pme->repeat = AP_REPEAT_SHUFFLE;
		else
			pme->repeat = AP_REPEAT_NONE;
		__apbox_show_region(pme, NULL, APBOX_REGION_REPEAT, FALSE);
		return pme->repeat;
	}
	return AP_REPEAT_NONE;
}

void	apbox_toggle_detail(t_HWidget handle)
{
	if(g_object_valid(handle)){
		apbox_t *pme = (apbox_t *)handle;
		__apbox_make_client_visible(pme, !pme->client);
		__apbox_show_region(pme, NULL, APBOX_REGION_CLIENT, FALSE);
	}
}

#undef PARENT_CLASS

