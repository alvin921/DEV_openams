#include "pack_call.hi"
#include "wgt_editor.h"
#include "wgt_button.h"

const resid_t gaCallEvtResid[CALL_EVT_MAX] = {
	RESID_UNDEF,		// CALL_EVT_NONE // 信道数据被修改
	RESID_STR_INCOMING,	// CALL_EVT_INCOMING		// 来电
	RESID_STR_CONNECT,	//CALL_EVT_CONNECT
	RESID_STR_DISCONNECT,	//CALL_EVT_DISCONNECT	// 对方挂断电话
	RESID_STR_WAITING,	//CALL_EVT_WAITING // 对方呼叫等待
	RESID_STR_ONHELD,	//CALL_EVT_ONHELD // 对方保持呼叫
	RESID_STR_RESUME,	//CALL_EVT_RESUME // 对方恢复呼叫
	RESID_STR_IN_MPTY,	//CALL_EVT_IN_MPTY // 多方通话
	RESID_STR_AUDIO_ON,	//CALL_EVT_AUDIO_ON // 语音通道开启
	RESID_STR_AUDIO_OFF,	//CALL_EVT_AUDIO_OFF // 语音通道关闭
	RESID_STR_MUTE,	//CALL_EVT_AUDIO_MUTE // 禁音
	RESID_STR_USER_SUSPENDED,	//CALL_EVT_USER_SUSPENDED // 网络挂起呼叫
	RESID_STR_USER_RESUMED,	//CALL_EVT_USER_RESUMED // 网络恢复呼叫
	RESID_STR_BARRING_ALL,	//CALL_EVT_BARRING_ALL // 禁止所有电话
	RESID_STR_I_FORWARDED_CALL,	//CALL_EVT_I_FORWARDED_CALL // 来电是被转移的呼叫
	RESID_STR_I_FORWARDED,	//CALL_EVT_I_FORWARDED // 来电被转移
	RESID_STR_I_BARRING,	//CALL_EVT_I_BARRING // 禁接当前来电
	RESID_STR_I_BARRING_ALL,	//CALL_EVT_I_BARRING_ALL // 禁接所有来电
	RESID_STR_I_BARRING_ROAMING,	//CALL_EVT_I_BARRING_ROAMING // 漫游时禁接来电
	RESID_STR_O_PROCEED,	//CALL_EVT_O_PROCEED
	RESID_STR_O_ALERT,	//CALL_EVT_O_ALERT
	RESID_STR_O_FORWARED,	//CALL_EVT_O_FORWARDED  // 呼叫被转移
	RESID_STR_O_BARRING,	//CALL_EVT_O_BARRING // 禁拨当前电话
	RESID_STR_O_BARRING_ALL,	//CALL_EVT_O_BARRING_ALL // 禁拨所有电话
	RESID_STR_O_BARRING_INTERNATION,	//CALL_EVT_O_BARRING_INTERNATION // 禁拨国际电话
	RESID_STR_O_BARRING_INT_EXCEPT_HONE,	//CALL_EVT_O_BARRING_INT_EXCEPT_HOME // 禁拨本国外电话
	RESID_STR_FORWARDING_ALL,	//CALL_EVT_FORWARDING_ALL // 所有通话被转移
	RESID_STR_ALL_COND_FORWARDING,	//CALL_EVT_ALL_COND_FORWARDING // 条件转移已开启
	RESID_STR_FORWARDING_ON_BUSY,	//CALL_EVT_FORWARDING_ON_BUSY // 遇忙转移
	RESID_STR_FORWARDING_ON_NOREPLY,	//CALL_EVT_FORWARDING_ON_NOREPLY // 无应答转移
	RESID_STR_FORWARDING_ON_UNREACHABLE,	//CALL_EVT_FORWARDING_ON_UNREACHABLE // 联络不到转移
};

#define	DTMF_INCALL_MAX	20
typedef struct {
	int	cid;
	t_CallInfo *info;
	char *number;
	t_CSessionID	contact;
	t_ContactInfo *cinfo;
	char		*name;
	datetime_t dt;
	gu32	tick;
	gu32	duration;
	gs16		timer;
	gu8		connected:1;
	gu8		outgoing:1;
	gu8		handfree:1;
	gu8		mute:1;
	gu8		held:1;
	gu8		keypad:1;
	gu8		reserved:2;

	gs8	btn_down;

	char		dtmf[DTMF_INCALL_MAX];
	gs8		n_dtmf;
}t_CallWindowData;

enum {
	CALL_BTN_MUTE,
	CALL_BTN_DIALER,
	CALL_BTN_SPEAKER,
	CALL_BTN_ADD,
	CALL_BTN_HOLD,
	CALL_BTN_CONTACT,
	CALL_BTN_N,
};
typedef const struct {
	gui_rect_t	rc_top;		//2 top: 分两行分别显示电话号码和时间
	gu8		phone_height;	//2  top中电话号码显示高度
	MC_FILL_STRUCT3;
	gui_rect_t	rc_pad;		//2 middle: 面板
	gui_rect_t	rc_bottom;	//2   bottom: 显示call按键
	gu32	grad_c1;
	gu32	grad_c2;
	gu32	hilight_color;
	resid_t	resid_text[CALL_BTN_N];
	resid_t	resid_icon[CALL_BTN_N];
}t_CallLayoutInfo;

#if (LCD_HEIGHT == 320)
#define	TITLE_HEIGHT	16
#define	PAD_TOP_HEIGHT	60
#define	PAD_BOT_HEIGHT	60
#define	PAD_HEIGHT		160
#else
#define	TITLE_HEIGHT	20
#define	PAD_TOP_HEIGHT	60
#define	PAD_BOT_HEIGHT	60
#define	PAD_HEIGHT		200
#endif

#define	PAD_OFFSET_Y	(PAD_TOP_HEIGHT+(LCD_HEIGHT-PAD_HEIGHT-PAD_BOT_HEIGHT-PAD_TOP_HEIGHT-TITLE_HEIGHT)/2)
t_CallLayoutInfo ga_btn_info = {
	{0, 	0, 	LCD_WIDTH, 	PAD_TOP_HEIGHT},
	40,
	{15,	PAD_OFFSET_Y, 	LCD_WIDTH-15*2, 	PAD_HEIGHT},
	{0, 	LCD_HEIGHT-PAD_BOT_HEIGHT-TITLE_HEIGHT,	LCD_WIDTH,	PAD_BOT_HEIGHT},
	MAKE_ARGB(0xa0,0x23,0x41,0x56),//(0xc0,103,154,192),//(0xc0,103,154,192),
	MAKE_ARGB(0xd0,0x23,0x41,0x56),//(0xc0,0x23,0x41,0x56),
	MAKE_ARGB(0xff, 0x00,0x00,0xff),
	{
		RESID_STR_MUTE, 
		RESID_STR_PANEL_DIALER, 
		RESID_STR_SPEAKER_ON, 
		RESID_STR_CALL_ADD, 
		RESID_STR_CALL_HOLD,
		RESID_STR_PANEL_CONTACT
	},
		{
			RESID_IMG_MUTE, 
			RESID_IMG_DIALER, 
			RESID_IMG_SPEAKER_ON, 
			RESID_IMG_CALL_ADD, 
			RESID_IMG_CALL_HOLD,
			RESID_IMG_CONTACT
		},
};

static int	__get_pad_cell(t_CallWindowData *wdata, coord_t x, coord_t y, gui_rect_t *prc)
{
	if(point_in_rect(x, y, &ga_btn_info.rc_pad)){
		int r, c;
		int nr;
		nr = (wdata->keypad)?4:2;
		x -= ga_btn_info.rc_pad.x;
		y -= ga_btn_info.rc_pad.y;
		r = y*nr/ga_btn_info.rc_pad.dy;
		c = x*3/ga_btn_info.rc_pad.dx;
		if(prc){
			prc->x = ga_btn_info.rc_pad.x + c*3/ga_btn_info.rc_pad.dx;
			prc->y = ga_btn_info.rc_pad.y + r*3/ga_btn_info.rc_pad.dy;
			prc->dx = ga_btn_info.rc_pad.dx/3;
			prc->dy = ga_btn_info.rc_pad.dy/nr;
		}
		return (r*3+c);
	}
	return -1;
}

#define	__show_top_bg(gdi, clear) do{ \
		gui_rect_t *prc = &ga_btn_info.rc_top; \
		if(clear)gdi_clear(gdi, prc->x, prc->y, prc->dx, prc->dy, guiNoCorner); \
		gdi_rectangle_fill_ex(gdi, prc->x, prc->y, prc->dx, prc->dy, ga_btn_info.grad_c1, ga_btn_info.grad_c2, GDI_FILL_MODE_GRADIENT_H, guiNoCorner); \
		gdi_set_color(gdi, RGB_DKGRAY); \
		gdi_hline(gdi, prc->y, prc->x, prc->x+prc->dx-1); \
		gdi_set_color(gdi, RGB_DKGRAY); \
		gdi_hline(gdi, prc->y+prc->dy-1, prc->x, prc->x+prc->dx-1); \
	}while(0)
#define	__show_bottom_bg(gdi, clear)	do{ \
		gui_rect_t *prc = &ga_btn_info.rc_bottom; \
		if(clear)gdi_clear(gdi, prc->x, prc->y, prc->dx, prc->dy, guiNoCorner); \
		gdi_rectangle_fill_ex(gdi, prc->x, prc->y, prc->dx, prc->dy, ga_btn_info.grad_c1, ga_btn_info.grad_c2, GDI_FILL_MODE_GRADIENT_H, guiNoCorner); \
		gdi_set_color(gdi, RGB_DKGRAY); \
		gdi_hline(gdi, prc->y, prc->x, prc->x+prc->dx-1); \
	}while(0)
	
	
static void	__show_btnpad_bg(t_CallWindowData *wdata, t_HGDI gdi)
{
	gui_rect_t *prc = &ga_btn_info.rc_pad;
	coord_t x,y, nr, i;

	gdi_rectangle_fill_ex(gdi, prc->x, prc->y, prc->dx, prc->dy, ga_btn_info.grad_c1, ga_btn_info.grad_c2, GDI_FILL_MODE_GRADIENT_H, guiFourCorner);

	x = prc->x;
	y = prc->y;
	//2 绘制横线
	nr = (wdata->keypad)?4:2;
	for(i = 0; i < nr-1; i++){
		y += prc->dy/nr;
		gdi_set_color(gdi, RGB_DKGRAY);
		gdi_hline(gdi, y, prc->x, prc->x+prc->dx-1);
		gdi_set_color(gdi, RGB_LTGRAY);
		gdi_hline(gdi, y+1, prc->x, prc->x+prc->dx-1);
	}

	//2 绘制竖线
	nr = 3;
	for(i = 0; i < nr-1; i++){
		x += prc->dx/nr;
		gdi_set_color(gdi, RGB_DKGRAY);
		gdi_vline(gdi, x, prc->y, prc->y+prc->dy-1);
		gdi_set_color(gdi, RGB_LTGRAY);
		gdi_vline(gdi, x+1, prc->y, prc->y+prc->dy-1);
	}
	//2 绘制外框
	gdi_set_color(gdi, RGB_LTGRAY);
	gdi_rectangle(gdi, prc->x, prc->y, prc->dx, prc->dy, guiFourCorner);
}

static void	__show_incall_button(t_HWindow handle, t_HGDI gdi, int btn, gboolean hilight, gboolean defer)
{
	t_CallWindowData *wdata = (t_CallWindowData*)wgt_get_tag(handle);
	t_HImage icon;
	const char *text;
	coord_t x, y, nr;
	gu16 dx, dy;

	nr = (wdata->keypad)?4:2;
	if(g_object_valid(gdi)){
		g_object_ref(gdi);
	}else{
		gdi = wgt_get_client_gdi((t_HWidget)handle);
	}
	dx = ga_btn_info.rc_pad.dx/3;
	dy = ga_btn_info.rc_pad.dy/nr;
	x = ga_btn_info.rc_pad.x + dx*(btn%3);
	y = ga_btn_info.rc_pad.y + dy*(btn/3);
	
	gdi_save_context(gdi);
	gdi_set_clip(gdi, x, y, dx, dy);

	if(hilight){
		gu32 old = gdi_set_color(gdi, RGB_DKBLUE);
		gdi_rectangle_fill(gdi, x+2, y+2, dx-4, dy-4, guiNoCorner);
		gdi_set_color(gdi, old);
	}else if(!defer){
		gdi_clear(gdi, x+2, y+2, dx-4, dy-4, guiNoCorner);
		__show_btnpad_bg(wdata, gdi);
	}
	if(wdata->keypad){
		if(btn >= 0 && btn < 9) // 1-9
			btn++;
		else if(btn == 9)	// '*'
			btn = 11;
		else if(btn == 10)	// 0
			btn = 0;
		else if(btn == 11)	// #
			btn = 12;
	
		//gdi_translate(gdi, x, y);
		//gdi_move_to(gdi, 5, 5);
		icon = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_IMG_DIGIT, btn));
		gdi_draw_image_in(gdi, x, y, dx, dy, icon, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
	}else{
		gdi_translate(gdi, x, y);
		gdi_move_to(gdi, 5, 5);

		icon = brd_get_image(BRD_HANDLE_SELF, ga_btn_info.resid_icon[btn]);
		text = brd_get_string(BRD_HANDLE_SELF, ga_btn_info.resid_text[btn], "");
		gdi_draw_icon_text(gdi, icon, text, dx-10, dy-10, guiBottom, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
	}
	g_object_unref(icon);
	
	gdi_restore_context(gdi);
	if(!defer)
		gdi_blt(gdi, x, y, dx, dy);
	g_object_unref(gdi);
}

static void	__show_incall_panel(t_HWindow handle, t_HGDI gdi, gboolean defer)
{
	t_CallWindowData *wdata = (t_CallWindowData *)wgt_get_tag(handle);

	if(g_object_valid(gdi)){
		g_object_ref(gdi);
	}else{
		gdi = wgt_get_client_gdi((t_HWidget)handle);
	}
	
	if(!defer)
		gdi_clear(gdi, ga_btn_info.rc_pad.x, ga_btn_info.rc_pad.y, ga_btn_info.rc_pad.dx, ga_btn_info.rc_pad.dy, guiFourCorner);

	__show_btnpad_bg(wdata, gdi);
	if(wdata->keypad){
		int i;
		for(i = 0; i < 12; i++)
			__show_incall_button(handle, gdi, i, FALSE, FALSE);
	}else{
		gdi_set_color(gdi, RGB_WHITE);
		__show_incall_button(handle, gdi, CALL_BTN_MUTE, wdata->mute?TRUE:FALSE, FALSE);
		__show_incall_button(handle, gdi, CALL_BTN_DIALER, FALSE, FALSE);
		__show_incall_button(handle, gdi, CALL_BTN_SPEAKER, wdata->handfree?TRUE:FALSE, FALSE);
		__show_incall_button(handle, gdi, CALL_BTN_ADD, FALSE, FALSE);
		__show_incall_button(handle, gdi, CALL_BTN_HOLD, wdata->held?TRUE:FALSE, FALSE);
		__show_incall_button(handle, gdi, CALL_BTN_CONTACT, FALSE, FALSE);
	}
	if(!defer)
		gdi_blt(gdi, ga_btn_info.rc_pad.x, ga_btn_info.rc_pad.y, ga_btn_info.rc_pad.dx, ga_btn_info.rc_pad.dy);
	
	g_object_unref(gdi);
}

static void	__show_incall_phone(t_HWindow handle, t_HGDI gdi, gboolean defer)
{
	t_CallWindowData *wdata = (t_CallWindowData*)wgt_get_tag(handle);
	coord_t x, y, dx, dy;
	int len;
	char *phone;

	if(wdata->keypad){
		len = wdata->n_dtmf;
		phone = wdata->dtmf;
	}else{
		phone = wdata->number;
		len = strlen(wdata->number);
	}
	x = ga_btn_info.rc_top.x;
	y = ga_btn_info.rc_top.y;
	dx = ga_btn_info.rc_top.dx;
	if(wgt_get_resid(handle) == RESID_WIN_CALL_MT)
		dy = ga_btn_info.rc_top.dy;
	else
		dy = ga_btn_info.phone_height;


	if(g_object_valid(gdi)){
		g_object_ref(gdi);
	}else{
		gdi = wgt_get_client_gdi((t_HWidget)handle);
	}
	if(!defer){
		gdi_save_context(gdi);
		gdi_set_clip(gdi, x, y, dx, dy);
		gdi_clear(gdi, x, y, dx, dy, guiNoCorner);
		__show_top_bg(gdi, FALSE);
	}
	
	__dialer_show_phone(gdi, phone, len, x, y, dx, dy);

	if(!defer){
		gdi_restore_context(gdi);
		gdi_blt(gdi, x, y, dx, dy);
	}
	g_object_unref(gdi);
}

static void	__show_incall_events(t_HWindow handle, t_HGDI gdi, gboolean defer)
{
	t_CallWindowData *wdata = (t_CallWindowData*)wgt_get_tag(handle);
	coord_t x, y, dx, dy;
	char buf[8];
	char *text = NULL;
	if(wgt_get_resid(handle) == RESID_WIN_CALL_MT)
		return;
	x = ga_btn_info.rc_top.x;
	dx = ga_btn_info.rc_top.dx;
	dy = ga_btn_info.rc_top.dy-ga_btn_info.phone_height;
	y = ga_btn_info.rc_top.y + ga_btn_info.phone_height;

	if(g_object_valid(gdi)){
		g_object_ref(gdi);
	}else{
		gdi = wgt_get_client_gdi((t_HWidget)handle);
	}
	
	if(!defer){
		gdi_save_context(gdi);
		gdi_set_clip(gdi, x, y, dx, dy);
		gdi_clear(gdi, x, y, dx, dy, guiNoCorner);
		__show_top_bg(gdi, FALSE);
	}

	if(wdata->connected){
		//2 显示时间
		gu32 second;
		second = (tick_current()-wdata->tick)/1000;
		sprintf(buf, "%02d:%02d", second/60, second%60);
		text = buf;
	}else if(wdata->info->outgoing){
		text = brd_get_string(brd_get(PACK_GUID_call), RESID_STR_O_ALERT, NULL);
	}
	gdi_set_color(gdi, RGB_WHITE);
	gdi_draw_text_in(gdi, x, y, dx, dy, text, -1, CHARSET_UTF8, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
	
	if(!defer){
		gdi_restore_context(gdi);
		gdi_blt(gdi, x, y, dx, dy);
	}
	g_object_unref(gdi);
}

static void	__window_predraw(t_HWindow handle)
{
	t_HGDI gdi = wgt_get_client_gdi((t_HWidget)handle);
	
	__show_top_bg(gdi, FALSE);
	__show_incall_phone(handle, gdi, TRUE);
	__show_incall_events(handle, gdi, TRUE);
	__show_incall_panel(handle, gdi, TRUE);
	__show_bottom_bg(gdi, FALSE);

	g_object_unref(gdi);
}

static void	call_CloseTimerCB(int timerID, void *userdata)
{
	t_HWindow handle = userdata;
	wnd_send_close(handle, 0);
}

static void	call_StartTimerClose(t_HWindow handle)
{
	t_CallWindowData *data = (t_CallWindowData*)wgt_get_tag(handle);
	if(data->timer >= 0)
		timer_release(data->timer);
	data->timer = timer_create(call_CloseTimerCB, handle);
	if(data->timer >= 0)
		timer_start(data->timer, 500, TIMER_MODE_ONCE);
	
}

static void	call_TimerCB(int timerID, void *userdata)
{
	t_HWindow handle = userdata;
	if(g_object_valid(handle) && wgt_is_focused(handle)){
		__show_incall_events(handle, NULL, FALSE);
	}
}

static void	call_StartTimer(t_HWindow handle)
{
	t_CallWindowData *data = (t_CallWindowData*)wgt_get_tag(handle);
	if(data->timer >= 0)
		timer_release(data->timer);
	data->timer = timer_create(call_TimerCB, handle);
	if(data->timer >= 0)
		timer_start(data->timer, 1000, TIMER_MODE_PERIOD);
	
}

static void	dtmf_callback(int timer, void *userdata)
{
	t_CallWindowData *wdata = userdata;
	gu8 flag = TRUE;
	char *p = &wdata->info->dtmf[1];
	if(*(p-1) == 'w'){
		//3 notify user to determine whether to start dtmf or not.
		if(msgbox_show(NULL, MBB_YES_NO, 0, 0, "%s %s?", brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_DTMF, ""), p) == MBB_NO)
			flag = FALSE;
	}
	if(flag){
		while(*p){
			call_send_dtmf(wdata->info->cid, *p++);
			call_stop_dtmf(wdata->info->cid);
		}
	}

	timer_release(timer);
}

static void	CallNotificationCB(int callid, t_CallEvent evt, void *data)
{
	t_HWindow handle = data;
	t_CallWindowData *wdata = (t_CallWindowData*)wgt_get_tag(handle);
	t_HWidget widget;// = wnd_get_widget(handle, RESID_LABEL_EVENT);
	const char *text;
	
#if defined(TARGET_CSD)
	mon_Event(0xcdcdcd01);
	mon_Event(callid<<24|evt);
#endif
#if 0
	text = brd_get_string(brd_get(PACK_GUID_call), gaCallEvtResid[evt], NULL);
	g_printf("id=%d,evt=%d(%s)", callid, evt, text?text:"");
	if(g_object_valid(widget) && text){
		wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_call), gaCallEvtResid[evt]);
		wgt_redraw(widget, FALSE);
	}
#endif

	if(evt == CALL_EVT_DISCONNECT){
		call_StartTimerClose(handle);
		if(wdata->connected){
			calllog_save(wdata->contact, wdata->number, &wdata->dt, tick_current()-wdata->tick, wdata->info->outgoing?CALL_LOG_OUTGOING:CALL_LOG_INCOMING, 0);
		}else{
			calllog_save(wdata->contact, wdata->number, &wdata->dt, 0, wdata->info->outgoing?CALL_LOG_OUTGOING:CALL_LOG_MISSED, 0);
		}
	}else if(evt == CALL_EVT_CONNECT){
		if(!wdata->info->outgoing){
			wnd_send_close(handle, 0);
			handle = wnd_create_by_resid(call_WndMsgHandler, wdata->cid, wdata->contact, brd_get(PACK_GUID_SELF), RESID_WIN_CALL);
			//call_notifier(cid, CallNotificationCB, handle);
			wnd_show(handle);
			return;
		}
		
		wdata->tick = tick_current();
		datetime_current(&wdata->dt);
		wdata->connected = TRUE;
		call_StartTimer(handle);
		
		wnd_redraw(handle, NULL);
		if(wdata->info->outgoing && (wdata->info->dtmf[0] == 'p' || wdata->info->dtmf[0] == 'w')){
			int timer = timer_create(dtmf_callback, wdata);
			timer_start(timer, 3000, TIMER_MODE_ONCE);
		}
	}
}

void		call_handle_end(t_CallWindowData *wdata, t_HWindow handle)
{
	call_end(wdata->cid);
	wdata->cid = -1;
	{
		#if 0
		t_HWidget widget = wnd_get_widget(handle, RESID_LABEL_EVENT);
		wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_call), gaCallEvtResid[CALL_EVT_DISCONNECT]);
		wgt_redraw(widget, FALSE);
		#endif
		if(wdata->connected){
			calllog_save(wdata->contact, wdata->number, &wdata->dt, (tick_current()-wdata->tick)/1000, wdata->info->outgoing?CALL_LOG_OUTGOING:CALL_LOG_INCOMING, 0);
		}else{
			calllog_save(wdata->contact, wdata->number, &wdata->dt, 0, wdata->info->outgoing?CALL_LOG_OUTGOING:CALL_LOG_REJECTED, 0);
		}
		call_StartTimerClose(handle);
	}
}

static void	__incall_keypad_toggle(t_HWindow handle, t_CallWindowData *wdata)
{
	t_HWidget widget;
	if(wdata->keypad){
		widget = wnd_get_widget(handle, RESID_BTN_HIDE_KEYPAD);
		wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
		
		widget = wnd_get_widget(handle, RESID_BTN_END);
		wgt_set_extent(widget, 210, wgt_bound_dy(widget));
		
		wdata->keypad = FALSE;
	}else{
		widget = wnd_get_widget(handle, RESID_BTN_HIDE_KEYPAD);
		wgt_enable_attr(widget, CTRL_ATTR_VISIBLE);
		
		widget = wnd_get_widget(handle, RESID_BTN_END);
		wgt_set_extent(widget, 100, wgt_bound_dy(widget));

		wdata->keypad = TRUE;
	}
	wnd_redraw(handle, NULL);
}

static const char ga_keypad[12] = {'1','2','3','4','5','6','7','8','9','*','0','#'};
static void	__incall_OnPenEvent(t_HWindow handle, t_CallWindowData *wdata, gs8 btn, gu8 type)
{
	if(type == MMI_PEN_DOWN){
		t_HGDI gdi;
		wdata->btn_down = btn;
		if(btn < 0)return;
		gdi = wgt_get_client_gdi((t_HWidget)handle);
		if(wdata->keypad){

			call_stop_dtmf(wdata->cid);
			call_send_dtmf(wdata->cid, ga_keypad[btn]);

			if(wdata->n_dtmf == DTMF_INCALL_MAX){
				memmove(wdata->dtmf, &wdata->dtmf[1], DTMF_INCALL_MAX-1);
				wdata->n_dtmf--;
			}
			wdata->dtmf[wdata->n_dtmf++] = ga_keypad[btn];

			__show_incall_phone(handle, gdi, FALSE);
		}
		__show_incall_button(handle, gdi, btn, TRUE, FALSE);
		g_object_unref(gdi);
	}else if(type == MMI_PEN_UP){
		if(wdata->btn_down != btn){
			if(wdata->btn_down >= 0)
				__show_incall_button(handle, NULL, wdata->btn_down, FALSE, FALSE);
		}else if(wdata->keypad){
			__show_incall_button(handle, NULL, btn, FALSE, FALSE);
		}else{
			switch(btn){
				case CALL_BTN_MUTE:
					wdata->mute = !wdata->mute;
					dev_audio_mute(wdata->info->aud, wdata->mute);
					if(!wdata->mute)
						__show_incall_button(handle, NULL, btn, FALSE, FALSE);
					break;
				case CALL_BTN_DIALER:
					//2 拨号键盘
					__incall_keypad_toggle(handle, wdata);
					break;
				case CALL_BTN_SPEAKER:
					wdata->handfree = !wdata->handfree;
					dev_audio_set_path(wdata->info->aud, wdata->handfree?AUD_DEV_SPEAKER:AUD_DEV_RECEIVER);
					if(!wdata->handfree)
						__show_incall_button(handle, NULL, btn, FALSE, FALSE);
					break;
				case CALL_BTN_ADD:
					//2 添加通话
					__show_incall_button(handle, NULL, btn, FALSE, FALSE);
					break;
				case CALL_BTN_HOLD:
					wdata->held = !wdata->held;
					//call_hold(wdata->info->cid, wdata->held);
					__show_incall_button(handle, NULL, btn, FALSE, FALSE);
					break;
				case CALL_BTN_CONTACT:
					//2 通讯录
					__show_incall_button(handle, NULL, btn, FALSE, FALSE);
					break;
			}
		}
		wdata->btn_down = -1;
	}
}

//2 <sparam>=<call_id>, <lparam>=<t_CSessionID>
error_t	call_WndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_CallWindowData *wdata = (t_CallWindowData*)wgt_get_tag(handle);
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			{
				t_HWidget widget;

				wgt_enable_attr(handle, WND_ATTR_HAS_STATUS|WND_ATTR_HAS_TITLE);
				wdata = MALLOC(sizeof(t_CallWindowData));
				memset(wdata, 0, sizeof(t_CallWindowData));
				wdata->cid = sparam;
				wdata->contact = lparam;
				wdata->info = call_get_info(wdata->cid);
				wdata->number = wdata->info->phone;
				if(wdata->contact == CSESSION_NULL){
					wdata->contact = contact_query_by_number(wdata->number, wdata->name, sizeof(wdata->name));
				}
				wdata->cinfo = contact_get_info(wdata->contact);
				if(wdata->cinfo)
					wdata->name = wdata->cinfo->name;
				else
					wdata->name = wdata->number;

				wdata->timer = -1;
				datetime_current(&wdata->dt);
				wdata->tick = tick_current();
				wdata->connected = FALSE;
				wdata->handfree = FALSE;
				wdata->mute = FALSE;
				wdata->held = FALSE;
				wdata->keypad = FALSE;
				wdata->btn_down = -1;
				dev_audio_set_path(wdata->info->aud, AUD_DEV_RECEIVER);
				wgt_set_tag(handle, wdata);
				call_StartTimer(handle);
				//wnd_SetTitle(handle, "Outgoing Call", BRD_HANDLE_SELF, RESID_STR_OUTGOING);
#if 0
				widget = wnd_get_widget(handle, RESID_LABEL_NUMBER);
				if(g_object_valid(widget)){
					label_set_text(widget, wdata->name);
				}
				widget = wnd_get_widget(handle, RESID_LABEL_EVENT);
				if(g_object_valid(widget)){
					if(wdata->info->outgoing)
						wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), gaCallEvtResid[CALL_EVT_O_ALERT]);
					else
						wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_SELF), gaCallEvtResid[CALL_EVT_INCOMING]);
						
					label_set_text(widget, "");
				}
				widget = wnd_get_widget(handle, RESID_CTRL_IMAGE);
				if(g_object_valid(widget)){
					t_ContactInfo info;
					if(!IS_UNSAVED_CONTACT(wdata->contact) && wdata->contact != UNKNOWN_CONTACT && contact_get_info(wdata->contact, &info)){
						if(info.pic_resid)
							imagectrl_set_by_resid(widget, brd_get("contact"), info.pic_resid);
						else if(info.pic_fname)
							imagectrl_set_by_file(widget, info.pic_fname);
						else
							imagectrl_set_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_IMG_PICTURE);
						contact_free_info(&info);
					}else{
						imagectrl_set_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_IMG_PICTURE);
					}
					wgt_disable_attr(widget, CTRL_ATTR_TABSTOP);
				}
				#endif
				if(wgt_get_resid(handle) == RESID_WIN_CALL){
					widget = wnd_get_widget(handle, RESID_BTN_END);
					wgt_set_text_align(widget, guiRight);
					wgt_set_border(widget, BORDER_TYPE_3DIN, 1, guiFourCorner);
					wgt_set_bg_fill(widget, GDI_FILL_MODE_2COLOR_H, MAKE_RGB(0xe0,0,0), MAKE_RGB(0xb0,0,0));
					
					widget = wnd_get_widget(handle, RESID_BTN_HIDE_KEYPAD);
					wgt_set_text_align(widget, guiRight);
					wgt_set_border(widget, BORDER_TYPE_3DIN, 2, guiFourCorner);
					wgt_set_bg_fill(widget, GDI_FILL_MODE_2COLOR_H, MAKE_RGB(0xe0,0,0), MAKE_RGB(0xb0,0,0));
				}else if(wgt_get_resid(handle) == RESID_WIN_CALL_MT){
					wgt_set_text_align(wnd_get_widget(handle, RESID_BTN_ANSWER), guiRight);
					wgt_set_text_align(wnd_get_widget(handle, RESID_BTN_END_MO), guiRight);
				}

				sk_set_text(wnd_get_skbar(handle), SK_LSK, NULL, RESID_STR_CALL_KEYPAD);
				sk_set_text(wnd_get_skbar(handle), SK_RSK, NULL, RESID_STR_BACK);
				
				ret = SUCCESS;
			}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			if(wdata){
				if(wdata->cid >= 0)
					call_end(wdata->cid);
				if(wdata->timer >= 0)
					timer_release(wdata->timer);
				contact_free_info(wdata->cinfo);
				FREE(wdata);
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnPreDraw:
			__window_predraw(handle);
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			//__show_incall_panel(handle, NULL, TRUE);
			ret = SUCCESS;
			break;
		case WIDGET_OnSelected:
			switch(wgt_get_resid(lparam)){
#if 0 // alvin
				case RESID_BTN_MUTE:
					if(wdata->mute){
						dev_audio_mute(wdata->info->aud, FALSE);
						wdata->mute = FALSE;
						wgt_set_caption_by_resid((t_HWidget)lparam, BRD_HANDLE_SELF, RESID_STR_MUTE);
						wgt_set_icon_by_resid((t_HWidget)lparam, BRD_HANDLE_SELF, RESID_IMG_MUTE);
					}else{
						dev_audio_mute(wdata->info->aud, TRUE);
						wdata->mute = TRUE;
						wgt_set_caption_by_resid((t_HWidget)lparam, BRD_HANDLE_SELF, RESID_STR_UNMUTE);
						wgt_set_icon_by_resid((t_HWidget)lparam, BRD_HANDLE_SELF, RESID_IMG_UNMUTE);
					}
					wgt_redraw((t_HWidget)lparam, FALSE);
					ret = SUCCESS;
					break;
				case RESID_BTN_SPEAKER:
					if(wdata->handfree){
						dev_audio_set_path(wdata->info->aud, AUD_DEV_RECEIVER);
						dev_audio_set_volume(wdata->info->aud, VOLUME_LEVEL_6);
						wdata->handfree = FALSE;
						wgt_set_caption_by_resid((t_HWidget)lparam, BRD_HANDLE_SELF, RESID_STR_SPEAKER_ON);
						wgt_set_icon_by_resid((t_HWidget)lparam, BRD_HANDLE_SELF, RESID_IMG_SPEAKER_ON);
					}else{
						dev_audio_set_path(wdata->info->aud, AUD_DEV_SPEAKER);
						dev_audio_set_volume(wdata->info->aud, VOLUME_LEVEL_6);
						wdata->handfree = TRUE;
						wgt_set_caption_by_resid((t_HWidget)lparam, BRD_HANDLE_SELF, RESID_STR_SPEAKER_OFF);
						wgt_set_icon_by_resid((t_HWidget)lparam, BRD_HANDLE_SELF, RESID_IMG_SPEAKER_OFF);
					}
					wgt_redraw((t_HWidget)lparam, FALSE);
					ret = SUCCESS;
					break;
				#endif
				case RESID_BTN_END:
					call_handle_end(wdata, handle);
					ret = SUCCESS;
					break;
				case RESID_BTN_END_MO:
					call_handle_end(wdata, handle);
					ret = SUCCESS;
					break;
				case RESID_BTN_ANSWER:
					call_answer(wdata->cid);
					ret = SUCCESS;
					break;
				case RESID_BTN_HIDE_KEYPAD:
					__incall_keypad_toggle(handle, wdata);
					break;
			}
			break;
#if !defined(FTR_NO_INPUT_KEY)
		case WIDGET_OnKeyEvent:
			if(sparam != MMI_KEY_PRESS && sparam != MMI_KEY_REPEAT)
				break;
			switch(lparam){
				case MVK_SEND:
					break;
				case MVK_END:
					call_handle_end(wdata, handle);
					ret = SUCCESS;
					break;
				case MVK_RSK:
					GOTO_DESKTOP();
					ret = SUCCESS;
					break;
				case MVK_0:
				case MVK_1:
				case MVK_2:
				case MVK_3:
				case MVK_4:
				case MVK_5:
				case MVK_6:
				case MVK_7:
				case MVK_8:
				case MVK_9:
					if(wdata->info->outgoing && wdata->info->state == CALL_STATE_INCALL){
						call_stop_dtmf(wdata->cid);
						call_send_dtmf(wdata->cid, '0'+lparam-MVK_0);
						label_append_char(wnd_get_widget(handle, RESID_LABEL_NUMBER), '0'+lparam-MVK_0);
						wgt_redraw(wnd_get_widget(handle, RESID_LABEL_NUMBER), FALSE);
					}
					break;
				case MVK_STAR:
					if(wdata->info->outgoing && wdata->info->state == CALL_STATE_INCALL){
						call_stop_dtmf(wdata->cid);
						call_send_dtmf(wdata->cid, '*');
						label_append_char(wnd_get_widget(handle, RESID_LABEL_NUMBER), '*');
						wgt_redraw(wnd_get_widget(handle, RESID_LABEL_NUMBER), FALSE);
					}
					break;
				case MVK_HASH:
					if(wdata->info->outgoing && wdata->info->state == CALL_STATE_INCALL){
						call_stop_dtmf(wdata->cid);
						call_send_dtmf(wdata->cid, '#');
						label_append_char(wnd_get_widget(handle, RESID_LABEL_NUMBER), '#');
						wgt_redraw(wnd_get_widget(handle, RESID_LABEL_NUMBER), FALSE);
					}
					break;
			}
			break;
#endif
		case WIDGET_OnPenEvent:
			{
				coord_t x0,y0;
				PEN_XY(lparam, x0,y0);
				switch(PEN_TYPE(lparam)){
					case MMI_PEN_DOWN:
						if(!wgt_point_in_client(handle, &x0, &y0))
							break;
						__incall_OnPenEvent(handle, wdata, __get_pad_cell(wdata, x0, y0, NULL), MMI_PEN_DOWN);
						break;
					case MMI_PEN_UP:
						if(wdata->btn_down >= 0 && wgt_point_in_client(handle, &x0, &y0)){
							__incall_OnPenEvent(handle, wdata, __get_pad_cell(wdata, x0, y0, NULL), MMI_PEN_UP);
						}
						break;
					case MMI_PEN_MOVE:
						if(wdata->btn_down >= 0){
						}
						break;
				}
				ret = SUCCESS;
			}
			break;
	}
	return ret;
}

t_HWindow	call_LoadOutgoing(gu8 sim, const char *phone, t_CSessionID sid)
{
	t_HWindow handle = NULL;

	if(prod_get_sim_count() == 0){
		msgbox_show(NULL, _CSK(OK), 0, 0, "No SIM inserted!");
		return NULL;
	}
	if(sim == 0xff || sim >= prod_get_sim_slots()){
		int i;
		sim = 0xff;
		for(i = 0; i < prod_get_sim_slots(); i++){
			if(prod_sim_status(i) == SIM_CARD_NORMAL){
				sim = i;
				break;
			}
		}
		if(sim == 0xff){
			msgbox_show(NULL, _CSK(OK), 0, 0, "No valid SIM cards!");
			return NULL;
		}
	}
	if(phone && phone[0]){
		int cid;
		cid = call_setup(sim, phone);
		g_printf("call(%s) id=%d", phone, cid);
		if(cid >= 0){
			handle = wnd_create_by_resid(call_WndMsgHandler, cid, sid, brd_get(PACK_GUID_SELF), RESID_WIN_CALL);
			call_notifier(cid, CallNotificationCB, handle);
			wnd_show(handle);
		}else{
		}
	}
	return handle;
}

t_HWindow	call_LoadIncoming(int cid)
{
	t_HWindow handle = NULL;

	if(cid >= 0){
		handle = wnd_create_by_resid(call_WndMsgHandler, cid, CSESSION_NULL, brd_get(PACK_GUID_SELF), RESID_WIN_CALL);
		call_notifier(cid, CallNotificationCB, handle);
		wnd_show(handle);
	}
	return handle;
}

