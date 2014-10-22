#include "pack_sys.hi"


#if defined(FTR_COMM_SESSION_SUPPORT)

typedef struct {
	void *req;
	gs16	 len;
	gboolean	sync;
	gs8		num;
	t_MimeMethodInfo *info;
#if !defined(FTR_NO_INPUT_KEY)
	t_HWidget list;
#endif
}t_MethodDlgData;


#if !defined(FTR_NO_INPUT_KEY)

static void LSKCallback(t_HWidget sk, void *data)
{
	t_HWindow handle = data;
	if(g_object_valid(handle)){
		t_MethodDlgData *wdata = (t_MethodDlgData*)wgt_get_tag(handle);
		int index = listctrl_get_cursor(wdata->list);
		if(index >= 0 && index < wdata->num){
			pack_call(wdata->info[index].guid, wdata->info[index].module, (void*)wdata->req, wdata->len, wdata->sync, NULL);
		}
		wnd_send_close(handle, TRUE);
	}
}

static void	__ms_listctrl_on_select(t_HDataSource h, const char *guid, t_AmsModuleId module, void *userdata)
{
	t_HWindow handle = userdata;
	t_MethodDlgData *wdata = (t_MethodDlgData*)wgt_get_tag(handle);
	
	pack_call(guid, module, (void*)wdata->req, wdata->len, wdata->sync, NULL);
	wnd_send_close(handle, 0);
}
#endif

#endif




static error_t 	MethodDlgMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t handled = NOT_HANDLED;
	t_MethodDlgData *wdata = (t_MethodDlgData*)wgt_get_tag(handle);
	t_HWidget widget;
	int i;
	
	switch(evt){
	case WINDOW_OnOpen:
		if(lparam){
			t_MethodDlgArg *param = (t_MethodDlgArg *)lparam;
			
			wdata = MALLOC(sizeof(t_MethodDlgData));
			memset(wdata, 0, sizeof(t_MethodDlgData));
			wgt_set_tag(handle, wdata);
			wdata->req = param->req;
			wdata->len = param->len;
			wdata->sync = param->sync;
			wdata->num = mime_service_query(param->mime, param->method, param->guid, NULL, -1, 0, MS_QUERY_F_SKIP_SELF|MS_QUERY_F_APP);
			if(wdata->num > 0){
				wdata->info = MALLOC(wdata->num*sizeof(t_MimeMethodInfo));
				memset(wdata->info, 0, (wdata->num*sizeof(t_MimeMethodInfo)));
				mime_service_query(param->mime, param->method, param->guid, wdata->info, wdata->num, 0, MS_QUERY_F_SKIP_SELF|MS_QUERY_F_APP);
			}
#if defined(FTR_NO_INPUT_KEY)
			#define	BTN_COLOR_1	MAKE_RGB(0x23,0x41,0x56)
			#define	BTN_COLOR_2	MAKE_RGB(0x67,0x9a,0xc0)
			#define	PANEL_BTN_X	20
			#define	PANEL_BTN_Y	10
			#define	PANEL_BTN_DX	(lcd_get_width()-PANEL_BTN_X*2)
			#define	PANEL_BTN_DY	30
			{
				gu16 dy;
				coord_t y;
				char title[40];
				
				
				dy = (wdata->num+1)*(PANEL_BTN_Y+PANEL_BTN_DY)+PANEL_BTN_Y;
				wgt_set_bound(handle, 0, lcd_get_height()-dy, lcd_get_width(), dy);
				wgt_set_border(handle, BORDER_TYPE_3DIN, 2, guiLeftTop|guiRightTop);
				wgt_set_bg_fill(handle, GDI_FILL_MODE_GRADIENT, 0xa0679ac0, 0xd0679ac0);
				
				y = PANEL_BTN_Y;
				for(i = 0; i < wdata->num; i++){
					widget = wgt_create(handle, "button");
					wgt_set_tag(widget, &wdata->info[i]);
					wgt_set_bound(widget, PANEL_BTN_X, y, PANEL_BTN_DX, PANEL_BTN_DY);
					y += PANEL_BTN_DY+PANEL_BTN_Y;
					wgt_set_border(widget, BORDER_TYPE_3DIN, 2, guiFourCorner);
					wgt_set_bg_fill(widget, GDI_FILL_MODE_SOLID, 0xff679ac0, 0xff679ac0);
					wgt_set_text_align(widget, guiTop);
					pack_GetModuleTitle(wdata->info[i].guid, wdata->info[i].module, title, sizeof(title));
					wgt_set_caption(widget, title);
				}
				widget = wgt_create(handle, "button");
				wgt_set_tag(widget, 0);
				wgt_set_bound(widget, PANEL_BTN_X, y, PANEL_BTN_DX, PANEL_BTN_DY);
				wgt_set_border(widget, BORDER_TYPE_3DIN, 2, guiFourCorner);
				wgt_set_bg_fill(widget, GDI_FILL_MODE_SOLID, 0xff679ac0, 0xff679ac0);
				wgt_set_text_align(widget, guiTop);
				wgt_set_caption_by_resid(widget, BRD_HANDLE_SELF, RESID_STR_CANCEL);
			}
#else
		{
			gui_rect_t client;
			
			wgt_enable_attr(handle, WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_SK);
			wgt_set_bound(handle, 0, 40, lcd_get_width(), lcd_get_height()-40);
			wgt_set_border(handle, BORDER_TYPE_LINE, 0, guiTop);
			wgt_set_bg_fill(handle, GDI_FILL_MODE_GRADIENT, 0xa0679ac0, 0xd0679ac0);

			wgt_get_client_rect((t_HWidget)handle, &client);
			widget = wgt_create(handle, CLSID_LISTCTRL);
			wgt_set_bound(widget, 0, 0, client.dx, client.dy);
			listctrl_set_viewtype(widget, GUI_VIEWTYPE_ICON_TEXT_BOTTOM);
#if (LCD_WIDTH == 240) && (LCD_HEIGHT == 320)
			listctrl_set_rows(widget, 0, 60, 0);
			listctrl_set_cols(widget, 0, 80);
#elif (LCD_WIDTH == 320) && (LCD_HEIGHT == 240)
			listctrl_set_rows(widget, 3, 0, 0);
			listctrl_set_cols(widget, 4, 0);
#elif (LCD_WIDTH == 176) && (LCD_HEIGHT == 220)
			listctrl_set_rows(widget, 3, 0, 0);
			listctrl_set_cols(widget, 3, 0);
#elif (LCD_WIDTH == 220) && (LCD_HEIGHT == 176)
			listctrl_set_rows(widget, 3, 0, 0);
			listctrl_set_cols(widget, 4, 0);
#elif (LCD_WIDTH == 128) && (LCD_HEIGHT == 160)
			wgt_enable_attr(handle, WND_ATTR_HAS_TITLE);
			listctrl_set_viewtype(widget, GUI_VIEWTYPE_ICON_TEXT_TITLE);
			listctrl_set_rows(widget, 2, 0, 0);
			listctrl_set_cols(widget, 3, 0);
#endif
			listctrl_set_flags(widget, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE);

			if(g_object_valid(widget)){
				t_HDataSource ds = DSAppStore_New(__ms_listctrl_on_select, handle);
				t_AppStoreDir *dir = DSAppStore_GetRootNode(ds);
				for(i = 0; i < wdata->num; i++){
					AppStore_AddApp(dir, wdata->info[i].guid, wdata->info[i].module);
				}
				listctrl_attach_datasource(widget, ds);
				g_object_unref(ds);
			}
			wdata->list = widget;

			widget = wnd_get_skbar(handle);
			//2 LSK: OK/YES
			sk_set_text(widget, SK_LSK, "OK", RESID_STR_OK);
			sk_set_callback(widget, SK_LSK, LSKCallback, handle);
			//2 RSK: NO/BACK/CANCEL
			sk_set_text(widget, SK_RSK, "CANCEL", RESID_STR_CANCEL);
		}
#endif
		}
		break;
	case WINDOW_OnClose:
		if(wdata){
			FREEIF(wdata->info);
			FREEIF(wdata);
		}
		break;
	case WIDGET_OnSelected:
		widget = (t_HWidget)lparam;
		if(g_object_valid(widget)){
#if defined(FTR_NO_INPUT_KEY)
			t_MimeMethodInfo *info;
			info = (t_MimeMethodInfo*)wgt_get_tag(widget);
			if(info){
				pack_call(info->guid, info->module, (void*)wdata->req, wdata->len, wdata->sync, NULL);
			}
			wnd_send_close(handle, 0);
			handled = SUCCESS;
#else
#endif
		}
		
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

#if 0
static error_t 	MethodDlgMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t handled = NOT_HANDLED;
	t_MethodDlgArg *arg = (t_MethodDlgArg *)wgt_get_tag(handle);
	gu32 mbb;
	coord_t x, y;
	gu16 width, height;
	
	switch(evt){
	case WINDOW_OnOpen:
		{
			int lcd_width, lcd_height, line, width_total, height_total;
			gu16 width_max, height_max;

#if !defined(FTR_NO_INPUT_KEY)
			if(arg->mask != 0)
				wgt_enable_attr(handle, WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_SK);
#endif
			arg = (t_MethodDlgArg*)lparam;
			wgt_set_tag(handle, arg);
			wgt_set_caption((t_HWidget)handle, arg->title);

			lcd_width = lcd_get_width();
			lcd_height = lcd_get_height();
			width_max = lcd_width*3/4;
			height_max = lcd_height*3/4;
			
			font_measure_string(arg->msg, -1, CHARSET_UNDEF, -1, &width_total, &height_total, NULL);
			line = ALIGN_DIV(width_total, width_max);
			if(line == 1){
				width = width_total + DLG_BORDER_X*2 + DLG_MARGIN_X*2;
			}else{
				width = width_max + DLG_BORDER_X*2 + DLG_MARGIN_X*2;
			}
			height = height_total*line + DLG_BORDER_Y*2 + DLG_MARGIN_Y*2 + DLG_TITLE_H + DLG_SK_H;

#if defined(FTR_IPHONE_STYLE)
			width = lcd_width;
			x = (lcd_width-width)/2;
			y = (lcd_height-height);
			wgt_set_border((t_HWidget)handle, BORDER_TYPE_3DIN, DLG_BORDER_X, guiLeftTop|guiRightTop);
			wgt_set_bg_fill(handle, GDI_FILL_MODE_GRADIENT, 0xa0679ac0, 0xd0679ac0);
#elif defined(FTR_ANDROID_STYLE)
			width = lcd_width;
			x = (lcd_width-width)/2;
			y = (lcd_height-height)/2;
			wgt_set_border((t_HWidget)handle, BORDER_TYPE_FRAME, DLG_BORDER_X, guiFourCorner);
#else
			x = (lcd_width-width)/2;
			y = (lcd_height-height)/2;
			wgt_set_border((t_HWidget)handle, BORDER_TYPE_FRAME, DLG_BORDER_X, guiFourCorner);
#endif
			wgt_set_bound((t_HWidget)handle, x, y, width, height);

#if defined(FTR_NO_INPUT_KEY)
			x = 0;
			y += height-DLG_SK_H;
			width /= 3;
			height = DLG_SK_H;
			#define	BTN_X	5
			#define	BTN_Y	5
			{
				t_HWidget btn;
				
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
				{
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
		if(sparam != MMI_KEY_UP && sparam != MMI_KEY_PRESS && sparam != MMI_KEY_LONG_PRESS)
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
#endif

error_t 	MethodDlg_Handler(t_sparam sparam, t_lparam lparam)
{
	t_HWindow wnd;
	wnd = wnd_create(MethodDlgMsgHandler, sparam, lparam);
	return wnd_modal(wnd);
}


