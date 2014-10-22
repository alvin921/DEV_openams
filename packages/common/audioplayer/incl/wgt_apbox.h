#ifndef WGT_apbox_H
#define WGT_apbox_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_apbox	"apbox"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_apbox)
extern t_GClass class_apbox;
#endif

#define	FTR_APBOX_TIME_PAST_REMAIN

enum {
	APBOX_REGION_SONG,
	APBOX_REGION_PROGRESS,
	APBOX_REGION_VOLUME,
#if defined(FTR_APBOX_TIME_PAST_REMAIN)
	APBOX_REGION_TIME_PAST,
	APBOX_REGION_TIME_REMAIN,
#else
	APBOX_REGION_TIME,
#endif
	APBOX_REGION_BUTTON_PREV,
	APBOX_REGION_BUTTON_PLAY_PAUSE,
	APBOX_REGION_BUTTON_NEXT,
	APBOX_REGION_REPEAT,
	APBOX_REGION_DETAIL,
	APBOX_REGION_CLIENT,
	APBOX_REGION_N
};

enum {
	APBOX_IMG_BG,			//2 ����ͼ
	APBOX_IMG_PROGRESS,	//2 ��������2/3��ͼƬ: 0(fill), 1(indicator), 2(bg)
	APBOX_IMG_VOLUME,		//2 ������2/3��ͼƬ: 0(fill), 1(indicator), 2(bg)
	APBOX_IMG_PREV,		//2 1��ͼ������ṩIMG_BG�����Ǹ���ͼ������Ϊ��һ��͸��ɫͼ������ͨ������õ�
	APBOX_IMG_NEXT, 		//2 1��ͼ������ṩIMG_BG�����Ǹ���ͼ������Ϊ��һ��͸��ɫͼ������ͨ������õ�
	APBOX_IMG_PLAY,		//2 1��ͼ������ṩIMG_BG�����Ǹ���ͼ������Ϊ��һ��͸��ɫͼ������ͨ������õ�
	APBOX_IMG_PAUSE,		//2 1/2��ͼ������ṩIMG_BG����0(pause), 1(pause hilight), ����Ϊ��һ��͸��ɫͼ������ͨ������õ�
	APBOX_IMG_REPEAT,		//2 4��ͼ: 0(repeat none), 1(repeat all), 2(repeat one), 3 (shuffle), ����ṩIMG_BG����0Ϊ��
	APBOX_IMG_DETAIL,	//2 2��ͼ����Ϊ��һ��͸��ɫͼ

	APBOX_IMG_N
};


typedef const struct {
	resid_t	resid[APBOX_IMG_N];
	gui_rect_t rect[APBOX_REGION_N];

	gu16	bg_width, bg_height;
	gu16	prog_start, prog_width;
	gu16	vol_start, vol_width;
#if !defined(FTR_NO_INPUT_KEY)
	amos_key_t	key_prev;
	amos_key_t	key_play;
	amos_key_t	key_next;
	amos_key_t	key_vol_dec;
	amos_key_t	key_vol_inc;
	amos_key_t	key_mode;
	amos_key_t	key_detail;
#endif
}t_ApboxConfig;

enum {
	AP_PLAY_NONE,		//2  �Ѿ�ָ����
	AP_PLAY_CURRENT,	//2  ������ϣ�����repeat mode�Զ�ѡ����һ��
	AP_PLAY_PREV,		//2 ��һ��
	AP_PLAY_NEXT,		//2  ��һ��
};

enum {
	AP_STATE_IDLE,
	AP_STATE_PLAYING,
	AP_STATE_PAUSED,

	AP_STATE_N
};


enum {
	AP_BUTTON_PREV,
	AP_BUTTON_PLAY,
	AP_BUTTON_NEXT,

	AP_BUTTON_REPEAT,
	AP_BUTTON_SHUFFLE,
	
	AP_BUTTON_N
};


typedef const char *(*apbox_get_song_name_callback_t)(void *userdata, gu8 mode, gu8 repeat);
typedef void		(*apbox_state_change_callback_t)(void *userdata, gu8 state, int errcode);

BEGIN_DECLARATION

void		apbox_bind(t_HWidget handle, t_ApboxConfig *attr, apbox_get_song_name_callback_t cb_name, apbox_state_change_callback_t cb_change, void *userdata);
error_t	apbox_play(t_HWidget handle, gu8 mode);
error_t	apbox_pause_resume(t_HWidget handle);
error_t	apbox_stop(t_HWidget handle, gboolean redraw);
gu8	apbox_get_state(t_HWidget handle);
gu8	apbox_get_repeat(t_HWidget handle);
gu8	apbox_set_repeat(t_HWidget handle, gu8 repeat);
gu8	apbox_toggle_repeat(t_HWidget handle);
void	apbox_toggle_detail(t_HWidget handle);
gu8	apbox_vol_inc(t_HWidget handle);
gu8 apbox_vol_inc(t_HWidget handle);
	

END_DECLARATION



#endif /* WGT_apbox_H */


