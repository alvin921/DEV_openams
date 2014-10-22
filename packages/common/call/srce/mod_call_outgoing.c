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
}t_CallWindowData;

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
		t_CallWindowData *wdata = (t_CallWindowData*)wgt_get_tag(handle);
		t_HWidget label = wnd_get_widget(handle, RESID_LABEL_EVENT);
		gu32 second;

		second = (tick_current()-wdata->tick)/1000;
		
		label_set_text(label, "  %02d:%02d", second/60, second%60);
		wgt_redraw(label, FALSE);
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
	t_HWidget widget = wnd_get_widget(handle, RESID_LABEL_EVENT);
	const char *text;
	
#if defined(TARGET_CSD)
	mon_Event(0xcdcdcd01);
	mon_Event(callid<<24|evt);
#endif
	text = brd_get_string(brd_get(PACK_GUID_call), gaCallEvtResid[evt], NULL);
	g_printf("id=%d,evt=%d(%s)", callid, evt, text?text:"");
	if(g_object_valid(widget) && text){
		wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_call), gaCallEvtResid[evt]);
		wgt_redraw(widget, FALSE);
	}

	if(evt == CALL_EVT_DISCONNECT){
		call_StartTimerClose(handle);
		if(wdata->connected){
			calllog_save(wdata->contact, wdata->number, &wdata->dt, tick_current()-wdata->tick, wdata->info->outgoing?CALL_LOG_OUTGOING:CALL_LOG_INCOMING, 0);
		}else{
			calllog_save(wdata->contact, wdata->number, &wdata->dt, 0, wdata->info->outgoing?CALL_LOG_OUTGOING:CALL_LOG_MISSED, 0);
		}
	}else if(evt == CALL_EVT_CONNECT){
		widget = wnd_get_widget(handle, RESID_BTN_ADD);
		if(g_object_valid(widget)){
			wgt_enable_attr(widget, CTRL_ATTR_VISIBLE);
		}
		widget = wnd_get_widget(handle, RESID_BTN_HOLD);
		if(g_object_valid(widget)){
			wgt_enable_attr(widget, CTRL_ATTR_VISIBLE);
		}

		if(!wdata->info->outgoing){
			widget = wnd_get_widget(handle, RESID_BTN_SPEAKER);
			if(g_object_valid(widget)){
				wgt_enable_attr(widget, CTRL_ATTR_VISIBLE);
			}
			widget = wnd_get_widget(handle, RESID_BTN_ANSWER);
			if(g_object_valid(widget)){
				wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
			}
			widget = wnd_get_widget(handle, RESID_BTN_END);
			if(g_object_valid(widget)){
				wgt_set_bound(widget, 10, wgt_position_y(widget), lcd_get_width()-20, wgt_bound_dy(widget));
				wgt_set_focus(widget, TAB_NONE);
			}
		}
		
		wdata->tick = tick_current();
		datetime_current(&wdata->dt);
		wdata->connected = TRUE;
		call_StartTimer(handle);
		widget = wnd_get_widget(handle, RESID_LABEL_EVENT);
		label_set_text(widget, "  00:00");

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
		t_HWidget widget = wnd_get_widget(handle, RESID_LABEL_EVENT);
		wgt_set_caption_by_resid(widget, brd_get(PACK_GUID_call), gaCallEvtResid[CALL_EVT_DISCONNECT]);
		wgt_redraw(widget, FALSE);
	
		if(wdata->connected){
			calllog_save(wdata->contact, wdata->number, &wdata->dt, (tick_current()-wdata->tick)/1000, wdata->info->outgoing?CALL_LOG_OUTGOING:CALL_LOG_INCOMING, 0);
		}else{
			calllog_save(wdata->contact, wdata->number, &wdata->dt, 0, wdata->info->outgoing?CALL_LOG_OUTGOING:CALL_LOG_REJECTED, 0);
		}
		call_StartTimerClose(handle);
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

				wgt_enable_attr(handle, WND_ATTR_HAS_STATUS|WND_ATTR_HAS_SK);
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
				dev_audio_set_path(wdata->info->aud, AUD_DEV_RECEIVER);
				wgt_set_tag(handle, wdata);
				call_StartTimer(handle);
				//wnd_SetTitle(handle, "Outgoing Call", BRD_HANDLE_SELF, RESID_STR_OUTGOING);
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
					t_ContactInfo *cinfo;
					if(!IS_UNSAVED_CONTACT(wdata->contact) && wdata->contact != UNKNOWN_CONTACT && (cinfo=contact_get_info(wdata->contact))){
						if(cinfo->pic_resid)
							imagectrl_set_by_resid(widget, brd_get("contact"), cinfo->pic_resid);
						else if(cinfo->pic_fname)
							imagectrl_set_by_file(widget, cinfo->pic_fname);
						else
							imagectrl_set_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_IMG_PICTURE);
						contact_free_info(cinfo);
					}else{
						imagectrl_set_by_resid(widget, brd_get(PACK_GUID_SELF), RESID_IMG_PICTURE);
					}
					wgt_disable_attr(widget, CTRL_ATTR_TABSTOP);
				}
				wgt_set_text_align(wnd_get_widget(handle, RESID_BTN_MUTE), guiBottom);
				wgt_set_text_align(wnd_get_widget(handle, RESID_BTN_SPEAKER), guiBottom);
				wgt_set_text_align(wnd_get_widget(handle, RESID_BTN_ADD), guiBottom);
				wgt_set_text_align(wnd_get_widget(handle, RESID_BTN_HOLD), guiBottom);
				wgt_set_text_align(wnd_get_widget(handle, RESID_BTN_ANSWER), guiRight);
				wgt_set_text_align(wnd_get_widget(handle, RESID_BTN_END), guiRight);
#if 0
				wgt_disable_attr(wnd_get_widget(handle, RESID_BTN_MUTE), BUTTON_ATTR_BG_FULL);
				wgt_disable_attr(wnd_get_widget(handle, RESID_BTN_SPEAKER), BUTTON_ATTR_BG_FULL);
				wgt_disable_attr(wnd_get_widget(handle, RESID_BTN_ADD), BUTTON_ATTR_BG_FULL);
				wgt_disable_attr(wnd_get_widget(handle, RESID_BTN_HOLD), BUTTON_ATTR_BG_FULL);
				wgt_enable_attr(wnd_get_widget(handle, RESID_BTN_ANSWER), BUTTON_ATTR_BG_FULL);
				wgt_enable_attr(wnd_get_widget(handle, RESID_BTN_END), BUTTON_ATTR_BG_FULL);
#endif
				if(wdata->info->outgoing){
					widget = wnd_get_widget(handle, RESID_BTN_END);
					if(g_object_valid(widget)){
						wgt_set_bound(widget, 10, wgt_position_y(widget), lcd_get_width()-20, wgt_bound_dy(widget));
						wgt_set_focus(widget, TAB_NONE);
					}
				}else{
					widget = wnd_get_widget(handle, RESID_BTN_SPEAKER);
					if(g_object_valid(widget)){
						wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
					}
					widget = wnd_get_widget(handle, RESID_BTN_ANSWER);
					if(g_object_valid(widget)){
						wgt_enable_attr(widget, CTRL_ATTR_VISIBLE);
						wgt_set_focus(widget, TAB_NONE);
					}
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
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnSelected:
			switch(wgt_get_resid(lparam)){
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
				case RESID_BTN_END:
					call_handle_end(wdata, handle);
					ret = SUCCESS;
					break;
				case RESID_BTN_ANSWER:
					call_answer(wdata->cid);
					ret = SUCCESS;
					break;
				case RESID_BTN_HOLD:
					wdata->held = !wdata->held;
					ret = SUCCESS;
					break;
				case RESID_BTN_ADD:
					ret = SUCCESS;
					break;
			}
			break;
		case WIDGET_OnKeyEvent:
			if(sparam != MMI_KEY_UP)
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
						label_append_char(wnd_get_widget(handle, RESID_LABEL_DTMF), '0'+lparam-MVK_0);
						wgt_redraw(wnd_get_widget(handle, RESID_LABEL_DTMF), FALSE);
					}
					ret = SUCCESS;
					break;
				case MVK_STAR:
					if(wdata->info->outgoing && wdata->info->state == CALL_STATE_INCALL){
						call_stop_dtmf(wdata->cid);
						call_send_dtmf(wdata->cid, '*');
						label_append_char(wnd_get_widget(handle, RESID_LABEL_DTMF), '*');
						wgt_redraw(wnd_get_widget(handle, RESID_LABEL_DTMF), FALSE);
					}
					ret = SUCCESS;
					break;
				case MVK_HASH:
					if(wdata->info->outgoing && wdata->info->state == CALL_STATE_INCALL){
						call_stop_dtmf(wdata->cid);
						call_send_dtmf(wdata->cid, '#');
						label_append_char(wnd_get_widget(handle, RESID_LABEL_DTMF), '#');
						wgt_redraw(wnd_get_widget(handle, RESID_LABEL_DTMF), FALSE);
					}
					ret = SUCCESS;
					break;
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

