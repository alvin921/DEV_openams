#include "pack_sys.hi"


#define MBB_LSK(b)	(((b)>>MBB_LSK_BITS) & 0xFF)
#define MBB_CSK(b)	(((b)>>MBB_CSK_BITS) & 0xFF)
#define MBB_RSK(b)	(((b)>>MBB_RSK_BITS) & 0xFF)

typedef const struct {
	const char *	str;
	resid_t		resid;
}t_MbbText;
static t_MbbText ga_MbbText[MBB_MAX] = {
	{NULL		, RESID_UNDEF}
	, {"OK"		, RESID_STR_OK}		//MBB_OK			// 1
	, {"YES"		, RESID_STR_YES}		//MBB_YES		// 2
	, {"BACK"	, RESID_STR_BACK}	//MBB_BACK		// 3
	, {"NO"		, RESID_STR_NO}		//MBB_NO			// 4
	, {"CANCEL"	, RESID_STR_CANCEL}	//MBB_CANCEL		// 5
	, {"YALL"		, RESID_STR_YESALL}	//MBB_YESALL		// 6
	, {"NALL"		, RESID_STR_NOALL}	//MBB_NOALL		// 7
};

#define	DLG_BORDER_X	2
#define	DLG_BORDER_Y	2
#define	DLG_MARGIN_X	3
#define	DLG_MARGIN_Y	3
#define	DLG_TITLE_H	32
#if defined(FTR_NO_INPUT_KEY)
#define	DLG_SK_H		40
#elif defined(FTR_SOFT_KEY)
#define	DLG_SK_H		prod_ui_sk_height()
#endif


//2 实现全局唯一无按键的信息提示窗口，按任意键关闭，或超时2s后自动关闭
static t_HWindow gh_prompt_win = NULL;

static void	__timer_to_close_wnd(int timerid, void *userdata)
{
	if(g_object_valid(gh_prompt_win)){
		t_HWindow handle = userdata;
		t_MBArg *arg = (t_MBArg *)wgt_get_tag(handle);
		wnd_send_close(handle, arg->default_value);
	}
}

t_MBArg *	__msgbox_update(t_HWindow handle, t_MBArg *lparam)
{
	t_MBArg *arg = (t_MBArg *)wgt_get_tag(handle);
	int lcd_width, lcd_height, line, width_total, height_total;
	gu16 width_max, height_max;
	coord_t x, y;
	gu16 width, height;
	int timerid = -1;

	if(arg){
		timerid = arg->result;
		FREEIF(arg->title);
		FREEIF(arg->msg);
		FREE(arg);
	}
	arg = MALLOC(sizeof(t_MBArg));
	arg->title = g_str_dup(lparam->title);
	arg->msg = g_str_dup(lparam->msg);
	arg->mask = lparam->mask;
	arg->timeout = lparam->timeout;
	arg->default_value = lparam->default_value;
	arg->result = -1;
	if(arg->mask == 0){
		if(timerid != -1)
			timer_stop(timerid);
		if(arg->timeout == 0 || arg->timeout == INFINITE){
			//2 无须启动timer
		}else{
			if(timerid == -1){
				timerid = timer_create(__timer_to_close_wnd, handle);
				arg->result = timerid;
			}
			timer_start(timerid, arg->timeout, TIMER_MODE_ONCE);
		}
	}
	wgt_set_tag(handle, arg);
	
	wgt_set_caption((t_HWidget)handle, arg->title);

	lcd_width = lcd_get_width();
	lcd_height = lcd_get_height();
	width_max = lcd_width*3/4;
	height_max = lcd_height*3/4;

	font_measure_string(arg->msg, -1, CHARSET_UNDEF, -1, &width_total, &height_total, NULL);
	line = ALIGN_DIV(width_total, width_max);

	height = height_total*line + DLG_BORDER_Y*2 + DLG_MARGIN_Y*2 + DLG_TITLE_H + DLG_SK_H;

	width = lcd_width;

	if(width < wgt_bound_dx(handle))
		width = wgt_bound_dx(handle);
	if(height < wgt_bound_dy(handle))
		height = wgt_bound_dy(handle);
	
#if defined(FTR_IPHONE_STYLE)
	x = (lcd_width-width)/2;
	y = (lcd_height-height);
#elif defined(FTR_ANDROID_STYLE)
	x = (lcd_width-width)/2;
	y = (lcd_height-height)/2;
#else
	x = (lcd_width-width)/2;
	y = (lcd_height-height)/2;
#endif
	wgt_set_bound((t_HWidget)handle, x, y, width, height);


	return arg;
}

static error_t 	PromptDlgMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t handled = NOT_HANDLED;
	t_MBArg *arg = (t_MBArg *)wgt_get_tag(handle);
	gu32 mbb;
	coord_t x, y;
	gu16 width, height;
	
	switch(evt){
	case WINDOW_OnOpen:
		{
			wgt_set_bound(handle, 0, 0, 1, 1);
			arg = __msgbox_update(handle, (t_MBArg*)lparam);
			
#if !defined(FTR_NO_INPUT_KEY)
			if(arg->mask != 0)
				wgt_enable_attr(handle, WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_SK);
#endif

#if defined(FTR_IPHONE_STYLE)
			wgt_set_border((t_HWidget)handle, BORDER_TYPE_3DIN, DLG_BORDER_X, guiLeftTop|guiRightTop);
			wgt_set_bg_fill(handle, GDI_FILL_MODE_GRADIENT, 0xa0679ac0, 0xd0679ac0);
#elif defined(FTR_ANDROID_STYLE)
			wgt_set_border((t_HWidget)handle, BORDER_TYPE_FRAME, DLG_BORDER_X, guiFourCorner);
#else
			//wgt_set_border((t_HWidget)handle, BORDER_TYPE_3DIN, DLG_BORDER_X, guiLeftTop|guiRightTop);
			wgt_set_border((t_HWidget)handle, BORDER_TYPE_LINE, DLG_BORDER_X, guiFourDir);
			wgt_set_bg_fill(handle, GDI_FILL_MODE_GRADIENT, 0xa0679ac0, 0xd0679ac0);
#endif
			
#if defined(FTR_NO_INPUT_KEY)
			if(arg->mask){
				t_HWidget btn;
				
				x = 0;
				y = wgt_bound_dy(handle)-DLG_SK_H;
				width = wgt_bound_dx(handle)/3;
				height = DLG_SK_H;
				#define	BTN_X	5
				#define	BTN_Y	5
				//2 LSK: OK/YES
				mbb = MBB_LSK(arg->mask);
				if(mbb > 0 && mbb < MBB_MAX){
					btn = wgt_create(handle, "button");
					wgt_set_caption_by_resid(btn, BRD_HANDLE_SELF, ga_MbbText[mbb].resid);
					wgt_set_bound(btn, x+BTN_X, y+BTN_Y, width-BTN_X*2, height-BTN_Y*2);
					wgt_set_border(btn, BORDER_TYPE_3DIN, 2, guiFourCorner);
					wgt_set_bg_fill(btn, GDI_FILL_MODE_SOLID, 0xff679ac0, 0xff679ac0);
					wgt_set_text_align(btn, guiTop);
					wgt_set_tag(btn, mbb);
				}
				x+= width;
				//2 CSK: OK/YES
				mbb = MBB_CSK(arg->mask);
				if(mbb > 0 && mbb < MBB_MAX){
					btn = wgt_create(handle, "button");
					wgt_set_caption_by_resid(btn, BRD_HANDLE_SELF, ga_MbbText[mbb].resid);
					wgt_set_bound(btn, x+BTN_X, y+BTN_Y, width-BTN_X*2, height-BTN_Y*2);
					wgt_set_border(btn, BORDER_TYPE_3DIN, 2, guiFourCorner);
					wgt_set_bg_fill(btn, GDI_FILL_MODE_SOLID, 0xff679ac0, 0xff679ac0);
					wgt_set_text_align(btn, guiTop);
					wgt_set_tag(btn, mbb);
				}
				x+= width;
				//2 RSK: CANCEL/NO
				mbb = MBB_RSK(arg->mask);
				if(mbb > 0 && mbb < MBB_MAX){
					btn = wgt_create(handle, "button");
					wgt_set_caption_by_resid(btn, BRD_HANDLE_SELF, ga_MbbText[mbb].resid);
					wgt_set_bound(btn, x+BTN_X, y+BTN_Y, width-BTN_X*2, height-BTN_Y*2);
					wgt_set_border(btn, BORDER_TYPE_3DIN, 2, guiFourCorner);
					wgt_set_bg_fill(btn, GDI_FILL_MODE_SOLID, 0xff679ac0, 0xff679ac0);
					wgt_set_text_align(btn, guiTop);
					wgt_set_tag(btn, mbb);
				}
			}
#elif defined(FTR_SOFT_KEY)
			if(arg->mask){
					t_HWidget sk;
					
					sk = wnd_get_skbar(handle);
					//2 LSK: OK/YES
					mbb = MBB_LSK(arg->mask);
					if(mbb > 0 && mbb < MBB_MAX)
						sk_set_text(sk, SK_LSK, ga_MbbText[mbb].str, ga_MbbText[mbb].resid);
					//2 RSK: CANCEL/NO
					mbb = MBB_RSK(arg->mask);
					if(mbb > 0 && mbb < MBB_MAX)
						sk_set_text(sk, SK_RSK, ga_MbbText[mbb].str, ga_MbbText[mbb].resid);
#if defined(FTR_CSK_ENABLED)
					//2 CSK: OK/YES
					mbb = MBB_CSK(arg->mask);
					if(mbb > 0 && mbb < MBB_MAX)
						sk_set_text(sk, SK_CSK, ga_MbbText[mbb].str, ga_MbbText[mbb].resid);
#endif
				}
#endif
		}
		break;
	case WINDOW_OnClose:
		if(arg){
			int timerid = arg->result;
			if(timerid >= 0)
				timer_release(timerid);
			if(arg->mask == 0)
				gh_prompt_win = NULL;
			FREEIF(arg->title);
			FREEIF(arg->msg);
			FREE(arg);
		}
		wgt_set_tag(handle, 0);
		break;
	case WINDOW_OnDraw:
	{
		t_HGDI gdi = wgt_get_gdi((t_HWidget)handle);
		
		x = DLG_BORDER_X;
		y = DLG_BORDER_Y;
		width = wgt_bound_dx(handle)-DLG_BORDER_X*2;
		//2 Title
		gdi_set_color(gdi, ui_theme_get()->fg_color);
		gdi_draw_text_in(gdi, x, y, width, DLG_TITLE_H, arg->title, -1, CHARSET_UTF8, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE);
		//2 Horizontal Line
		y = DLG_TITLE_H;
		gdi_set_color(gdi, RGB_DKGRAY);
		gdi_hline(gdi, y-2, x, x+width-1);
		gdi_set_color(gdi, RGB_LTGRAY);
		gdi_hline(gdi, y-1, x, x+width-1);

		//2 Message
		height = wgt_bound_dy(handle)-DLG_TITLE_H-DLG_SK_H;
		gdi_set_color(gdi, ui_theme_get()->fg_color);
		gdi_draw_text_in(gdi, x+DLG_MARGIN_X, y+DLG_MARGIN_Y, width-DLG_MARGIN_X*2, height-DLG_MARGIN_Y*2, arg->msg, -1, CHARSET_UTF8, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE | IDF_TEXT_MULTILINE);
		//2 Border
		//wgt_show_border(((t_HWidget)handle), gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
		g_object_unref(gdi);

		break;
	}
#if defined(FTR_NO_INPUT_KEY)
	case WIDGET_OnSelected:
		{
			t_HWidget btn = (t_HWidget)lparam;
			if(g_object_valid(btn)){
				mbb = wgt_get_tag(btn);
				wnd_send_close(handle, mbb);
				handled = SUCCESS;
			}
		}
		break;
#endif
	case WIDGET_OnKeyEvent:
		if(sparam != MMI_KEY_UP)
			return SUCCESS;
		if(arg->mask == 0){
			wnd_send_close(handle, MBB_OK);
			handled = SUCCESS;
			break;
		}
		switch(lparam){
#if defined(FTR_SOFT_KEY)
		case MVK_LSK:
			mbb = MBB_LSK(arg->mask);
			if(mbb > 0 && mbb < MBB_MAX){
				wnd_send_close(handle, mbb);
			}
			handled = SUCCESS;
			break;
		case MVK_RSK:
			mbb = MBB_RSK(arg->mask);
			if(mbb > 0 && mbb < MBB_MAX){
				wnd_send_close(handle, mbb);
			}
			handled = SUCCESS;
			break;
		case MVK_OK:
#if defined(FTR_CSK_ENABLED)
			mbb = MBB_CSK(arg->mask);
			if(mbb == MBB_NONE){
				mbb = MBB_LSK(arg->mask);
			}
			if(mbb > 0 && mbb < MBB_MAX){
				wnd_send_close(handle, mbb);
			}
#else
			mbb = MBB_LSK(arg->mask);
			if(mbb == MBB_NONE){
				mbb = MBB_RSK(arg->mask);
			}
			if(mbb > 0 && mbb < MBB_MAX){
				wnd_send_close(handle, mbb);
			}
#endif
			handled = SUCCESS;
			break;

#endif
		case MVK_CLR:
			handled = SUCCESS;
			break;
		default:
			handled = SUCCESS;
			break;
		}
	}
	return handled;
}

error_t 	MessageBox_Handler(t_sparam sparam, t_lparam lparam)
{
	t_MBArg *arg = (t_MBArg*)lparam;
	t_HWindow wnd;

	if(arg->mask){
		wnd = wnd_create(PromptDlgMsgHandler, sparam, lparam);
		arg->result = wnd_modal(wnd);
	}else{
		if(gh_prompt_win == NULL){
			gh_prompt_win = wnd_create(PromptDlgMsgHandler, sparam, lparam);
		}else{
			__msgbox_update(gh_prompt_win, arg);
		}
		arg->result = 0;
		wnd_show(gh_prompt_win);
	}
	return SUCCESS;
}

