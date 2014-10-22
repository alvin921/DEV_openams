#include "pack_sms.hi"
#include "wgt_editor.h"

typedef struct {
	gboolean	flag_failed;
}t_NewMsgWData;

static void	_SmsMoCallback(gu8 state, t_CSessionID cid, const char *number, void *userdata)
{
	t_HWindow handle = userdata;
	t_NewMsgWData *wdata = (t_NewMsgWData*)wgt_get_tag(handle);
	t_HWidget widget = wnd_get_widget(handle, RESID_LABEL_STATE);
	switch(state){
		case SMS_MO_STATE_BEGIN:
			label_set_text(widget, "TX begin......");
			wgt_redraw(widget, FALSE);
			break;
		case SMS_MO_STATE_SENDING:
			label_set_text(widget, "state tx: cid=0x%x, number=%s", cid, number);
			wgt_redraw(widget, FALSE);
			break;
		case SMS_MO_STATE_SENT_OK:
			label_set_text(widget, "TX OK: cid=0x%x, number=%s", cid, number);
			wgt_redraw(widget, FALSE);
			break;
		case SMS_MO_STATE_SENT_KO:
			label_set_text(widget, "TX ERR: cid=0x%x, number=%s", cid, number);
			wgt_redraw(widget, FALSE);
			break;
		case SMS_MO_STATE_END:
			label_set_text(widget, "TX end......");
			wgt_redraw(widget, FALSE);
			amos_sleep(200);
			wnd_send_close(handle, 0);	
			break;
		case SMS_MO_STATE_ERROR:
			wdata->flag_failed = TRUE;
			label_set_text(widget, "TX error end......");
			wgt_redraw(widget, FALSE);
			//amos_sleep(200);
			//wnd_send_close(handle, 0);	
			break;
	}
}

static void	_LskCallback(t_HWidget sk, void *data)
{
	t_HWindow handle = data;
	t_NewMsgWData* wdata = (t_NewMsgWData *)wgt_get_tag(handle);
	t_HWidget widget;
	char *to, *content;
	int n, i;
	gboolean f_ascii = TRUE;

	//2 如果此前发送失败，则resend
	if(wdata->flag_failed){
		sms_glob_t *glob = task_get_data();
#if !defined(FTR_COMM_SESSION_SUPPORT) || defined(SMALL_LCD)
		g_ds_First(glob->ds);
#else
		g_ds_Last(glob->ds);
#endif
		sms_resend(g_ds_CurrentID(glob->ds), TRUE, _SmsMoCallback, handle);
		return;
	}
	widget = wnd_get_widget(handle, RESID_EDITOR_TO);
	to = editor_get_text_dup(widget);
	widget = wnd_get_widget(handle, RESID_EDITOR_CONTENT);
	content = editor_get_text_dup(widget);
	n = strlen(content);
	for(i = 0; i < n; i++){
		if(!isascii(content[i])){
			f_ascii = FALSE;
			break;
		}
	}
	if(f_ascii)
		sms_send(content, strlen(content), DCS_7BIT, to, 0, FALSE, _SmsMoCallback, handle, TRUE);
	else{
		ucs2_t *ucs;
		ucs = string_to_ucs2_dup(content, n, CHARSET_UNDEF);
		sms_send(ucs, 2*wstrlen(ucs), DCS_UCS2, to, 0, FALSE, _SmsMoCallback, handle, TRUE);
		FREE(ucs);
	}
	FREEIF(to);
	FREEIF(content);

	widget = wnd_get_widget(handle, RESID_EDITOR_TO);
	wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
	widget = wnd_get_widget(handle, RESID_EDITOR_CONTENT);
	wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
	widget = wnd_get_widget(handle, RESID_BTN_ADD_TO);
	wgt_disable_attr(widget, CTRL_ATTR_ENABLED);
	widget = wnd_get_widget(handle, RESID_LABEL_STATE);
	wgt_set_visible(widget, TRUE);
	
}

//3 sparam=with, lparam=<to addr> or <content>: 
error_t	sms_NewMessageWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	sms_glob_t *glob = task_get_data();
	t_NewMsgWData *wdata = (t_NewMsgWData*)wgt_get_tag(handle);
	switch(evt){
		case WINDOW_OnOpen:
		{
			t_HWidget wgt_to, wgt_cont;
			char *string = (char *)lparam;

			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			wdata = MALLOC(sizeof(t_NewMsgWData));
			memset(wdata, 0, (sizeof(t_NewMsgWData)));
			wdata->flag_failed = FALSE;

			wgt_set_tag(handle, wdata);

			wgt_to = wnd_get_widget(handle, RESID_EDITOR_TO);
			editor_set_flags(wgt_to, IDF_TEXT_SINGLE);
			editor_set_type(wgt_to, EDITOR_TYPE_PHONE, TRUE);
			
			wgt_cont = wnd_get_widget(handle, RESID_EDITOR_CONTENT);
			editor_set_flags(wgt_cont, IDF_TEXT_MULTILINE);
			editor_set_type(wgt_cont, EDITOR_TYPE_TEXT, FALSE);
			editor_set_nchar_max(wgt_cont, 600);

			if(sparam == WITH_TO){
				editor_set_text(wgt_to, string);
				wgt_set_focus(wgt_cont, TAB_NONE);
			}else if(sparam == WITH_CONTENT){
				editor_set_text(wgt_cont, string);
				wgt_set_focus(wgt_to, TAB_NONE);
			}

			{
				t_HWidget sk = wnd_get_skbar(handle);
				if(g_object_valid(sk)){
					sk_set_text(sk, SK_LSK, NULL, RESID_STR_SEND_SMS);
					sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
					sk_set_callback(sk, SK_LSK, _LskCallback, handle);
					//widget = sms_ms_listctrl_new(glob, handle);
					//sk_set_popup(sk, SK_LSK, widget);
					//g_object_unref(widget);
				}
			}
			ret = SUCCESS;
		}
			break;
		case WIDGET_OnSelected:
			switch(wgt_get_resid(lparam)){
				case RESID_BTN_ADD_TO:
				{
					ret = SUCCESS;
					break;
				}
			}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			FREEIF(wdata);
			ret = SUCCESS;
			break;
		case WIDGET_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(lparam == MVK_SEND && sparam == MMI_KEY_PRESS)
				;//pack_call("call", MOD_ID_MAIN, (gu8*)glob->items[glob->cursor].CellNo, strlen(glob->items[glob->cursor].CellNo)+1, TRUE, NULL);
	}
	return ret;
}

