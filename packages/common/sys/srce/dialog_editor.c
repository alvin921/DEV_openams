#include "pack_sys.hi"

typedef struct {
	char *buffer;
	int bufsize;
	const char *title;
	const char *lsk;
	gu32	flags;
}t_EditorDlgParam;

typedef struct {
	char *buffer;
	int bufsize;
	const char *title;
	const char *lsk;
	gu32	flags;
	t_HWidget	widget;
}t_EditorDlgData;

static void SKCallback(t_HWidget sk, t_HWindow handle)
{
	if(g_object_valid(handle)){
		t_EditorDlgData *dlg = (t_EditorDlgData*)wgt_get_tag(handle);
		int len;
		if(editor_get_nchars(dlg->widget) == 0){
			msgbox_show(NULL, _CSK(OK), 0, 0, "Please input");
			return;
		}
		len = editor_get_text(dlg->widget, dlg->buffer, dlg->bufsize);
		wnd_send_close(handle, len);
	}
}

#define	DLG_EDITOR_WINDOW_H		30

static void	__win_OnOpen(t_HWindow handle, t_EditorDlgParam *param)
{
	t_EditorDlgData *data;
	t_HWidget widget;
	gu32 flag = 0;
	coord_t x, y;
	gu16 dx, dy;
	gui_rect_t client;
	
	dx = lcd_get_width();
	dy = lcd_get_height();
	x = y = 0;

	wgt_set_bound((t_HWidget)handle, x, y, dx, dy);
	wgt_enable_attr(handle, WND_ATTR_DEFAULT);
	wgt_set_border((t_HWidget)handle, BORDER_TYPE_NONE, 0, 0);
	wgt_set_caption((t_HWidget)handle, param->title);
	wgt_set_bg_fill_solid(handle, MAKE_RGB(192,192,255));

	wgt_get_client_rect((t_HWidget)handle, &client);
	x = 5;
	dx = client.dx-10;
	y = 5;
	if(BIT_TEST(param->flags, IDF_TEXT_SINGLE) && !BIT_TEST(param->flags, IDF_TEXT_AUTO_WRAP))
		dy = 40;
	else
		dy = client.dy - y - 138;
	
	widget = (t_HWidget)wgt_create((t_HWidget)handle, "editor");
	if(g_object_valid(widget)){
		wgt_freeze(widget); //2 disable drawing
		editor_set_type(widget, EDITOR_TYPE_TEXT, FALSE);
		wgt_set_bound((t_HWidget)widget, x, y, dx, dy);
		wgt_set_border((t_HWidget)widget, BORDER_TYPE_3DOUT, 0, guiFourCorner);
		editor_set_nbytes_max(widget, param->bufsize);
		editor_set_text(widget, param->buffer);
		editor_set_cursor(widget, strlen(param->buffer));
		editor_set_sel(widget, -strlen(param->buffer));
		editor_set_flags(widget, param->flags);
		wgt_unfreeze(widget); //2 unfreeze to enable drawing
	}

	data = MALLOC(sizeof(t_EditorDlgData));
	data->widget = widget;
	data->buffer = param->buffer;
	data->bufsize = param->bufsize;

	wgt_set_tag(handle, data);
#if defined(FTR_SOFT_KEY)
		{
			t_SKHandle sk;
			sk = wnd_get_skbar(handle);
			//2 LSK: OK/YES
			if(param->lsk && param->lsk[0])
				sk_set_text(sk, SK_LSK, param->lsk, 0);
			else
				sk_set_text(sk, SK_LSK, NULL, RESID_STR_OK);
			sk_set_callback(sk, SK_LSK, SKCallback, (void *)handle);
			
			//2 RSK: NO/BACK/CANCEL
			sk_set_text(sk, SK_RSK, "CANCEL", RESID_STR_CANCEL);
		}
#endif
}

static error_t 	EditorDlgMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t handled = NOT_HANDLED;
	t_EditorDlgData *dlg = (t_EditorDlgData*)wgt_get_tag(handle);
	
	switch(evt){
	case WINDOW_OnOpen:
		if(lparam){
			__win_OnOpen(handle, (t_EditorDlgParam*)lparam);
		}
		break;
	case WINDOW_OnClose:
		FREEIF(dlg);
		break;
	case WINDOW_OnDraw:
		break;
#if 0
	case WIDGET_OnKeyEvent:
		switch(lparam){
		case MVK_OK:
		//case MVK_LSK:
		//case MVK_RSK:
			wgt_send_msg(handle, EVT_WND_CLOSE, MBB_OK, 0, 0);
			handled = SUCCESS;
			break;
		case MVK_CLR:
			handled = SUCCESS;
			break;
		default:
			handled = SUCCESS;
			break;
		}
#endif
	}
	return handled;
}

gboolean	dlg_editor_show(char *buffer, int bufsize, const char *title, const char *lsk, gu32 flags)
{
	t_HWindow wnd;
	t_EditorDlgParam param;

	if(buffer == NULL || bufsize <= 0)
		return FALSE;

	param.buffer = buffer;
	param.bufsize = bufsize;
	param.title = title;
	param.lsk = lsk;
	param.flags = flags;

	wnd = wnd_create(EditorDlgMsgHandler, sizeof(param), (gu32)&param);
	return wnd_modal(wnd);
}

