#include "pack_call.hi"

typedef struct {
	char		strPhone[CONTACT_NUMBER_MAX+1];
	gs16		len;
	gs8		r_down;
	gboolean	moved;
}t_DialerWData;

typedef struct {
	t_DialerWData		dialer;
}t_CallWndData;

enum {
	DIALER_PAD_BG,
	DIALER_PAD_1,
	DIALER_PAD_2,
	DIALER_PAD_3,
	DIALER_PAD_4,
	DIALER_PAD_5,
	DIALER_PAD_6,
	DIALER_PAD_7,
	DIALER_PAD_8,
	DIALER_PAD_9,
	DIALER_PAD_STAR,
	DIALER_PAD_0,
	DIALER_PAD_HASH,
	DIALER_PAD_ADD,
	DIALER_PAD_CALL,
	DIALER_PAD_BKSPACE,

	DIALER_PAD_N		/* 16 */
};
const gui_rect_t	ga_dialer_pad[DIALER_PAD_N] = {
#if (LCD_WIDTH==240) && (LCD_HEIGHT==320)
	{0	, 0	, 240	, 39},
	//2 1-2-3
	{1	, 40	, 78		, 43},
	{81	, 40	, 78		, 43},
	{161, 40	, 78		, 43},
	//2 4-5-6
	{1	, 85	, 78		, 43},
	{81	, 85	, 78		, 43},
	{161, 85	, 78		, 43},
	//2 7-8-9
	{1	, 130	, 78		, 43},
	{81	, 130	, 78		, 43},
	{161, 130	, 78		, 43},
	//2 *-0-#
	{1	, 175	, 78		, 43},
	{81	, 175	, 78		, 43},
	{161, 175	, 78		, 43},
	//2 Add-Call-Del
	{1	, 220	, 78		, 43},
	{80	, 221	, 80		, 40},
	{161, 220	, 78		, 43},
#elif (LCD_WIDTH==240) && (LCD_HEIGHT==400)
	{0		, 0 , 240	, 59},
	//2 1-2-3
	{2		, 64	, 76		, 53},
	{82		, 64	, 76		, 53},
	{162	, 64	, 76		, 53},
	//2 4-5-6
	{2		, 120	, 76		, 53},
	{82		, 120	, 76		, 53},
	{162	, 120	, 76		, 53},
	//2 7-8-9
	{2		, 175	, 76		, 53},
	{82		, 175	, 76		, 53},
	{162	, 175	, 76		, 53},
	//2 *-0-#
	{2		, 231	, 76		, 53},
	{82		, 231	, 76		, 53},
	{162	, 231	, 76		, 53},
	//2 Add-Call-Del
	{2		, 286	, 76		, 53},
	{80		, 288	, 80		, 49},
	{162	, 286	, 76		, 53},
#elif (LCD_WIDTH==272) && (LCD_HEIGHT==480)
	{0	, 0 , 272	, 75},
	//2 1-2-3
	{2		, 77	, 87	, 63},
	{93		, 77	, 87	, 63},
	{184	, 77	, 87	, 63},
	//2 4-5-6
	{2		, 144	, 87	, 63},
	{93		, 144	, 87	, 63},
	{184	, 144	, 87	, 63},
	//2 7-8-9
	{2		, 211	, 87	, 63},
	{93		, 211	, 87	, 63},
	{184	, 211	, 87	, 63},
	//2 *-0-#
	{2		, 278	, 87	, 63},
	{93		, 278	, 87	, 63},
	{184	, 278	, 87	, 63},
	//2 Add-Call-Del
	{2		, 345	, 87	, 63},
	{91		, 347	, 90	, 59},
	{184	, 345	, 87	, 63},
#elif (LCD_WIDTH==320) && (LCD_HEIGHT==480)
	{0	, 0 , 320	, 75},
	//2 1-2-3
	{3		, 77	, 102	, 63},
	{109	, 77	, 102	, 63},
	{215	, 77	, 102	, 63},
	//2 4-5-6
	{3		, 144	, 102	, 63},
	{109	, 144	, 102	, 63},
	{215	, 144	, 102	, 63},
	//2 7-8-9
	{3		, 211	, 102	, 63},
	{109	, 211	, 102	, 63},
	{215	, 211	, 102	, 63},
	//2 *-0-#
	{3		, 278	, 102	, 63},
	{109	, 278	, 102	, 63},
	{215	, 278	, 102	, 63},
	//2 Add-Call-Del
	{3		, 345	, 102	, 63},
	{107	, 347	, 106	, 59},
	{215	, 345	, 102	, 63},
#endif
};
static gs8		__dialer_pad_get(coord_t x, coord_t y)
{
	int idx;
	for(idx = 0; idx < DIALER_PAD_N; idx++){
		if(point_in_rect(x, y, &ga_dialer_pad[idx]))
			return idx;
	}
	return -1;
}

static void	__dialer_pad_show(t_HWidget handle, t_HGDI gdi, int pad, gboolean hilight, gboolean defer)
{
	gui_rect_t *prc = &ga_dialer_pad[pad];
	if(g_object_valid(gdi)){
		g_object_ref(gdi);
	}else{
		gdi = wgt_get_gdi(handle);
	}
	if(hilight){
		if(pad == DIALER_PAD_CALL){
			gu32 old = gdi_set_color(gdi, MAKE_ARGB(0xa0, 64,64,64));
			gdi_rectangle_fill(gdi, prc->x+1, prc->y+1, prc->dx-2, prc->dy-2, guiNoCorner);
			gdi_set_color(gdi, old);
		}else{
			gu32 old = gdi_set_color(gdi, MAKE_ARGB(0xa0, 192,192,192));
			gdi_rectangle_fill(gdi, prc->x+1, prc->y+1, prc->dx-2, prc->dy-2, guiNoCorner);
			gdi_set_color(gdi, old);
		}
	}else{
		gdi_clear(gdi, prc->x, prc->y, prc->dx, prc->dy, guiFourCorner);
	}
	gdi_blt(gdi, prc->x, prc->y, prc->dx, prc->dy);
	g_object_unref(gdi);
}

gui_extend_t ga_digit_extend[DIGIT_N] = {{0,0}, };

void	__dialer_show_phone(t_HGDI gdi, char *strPhone, int len, coord_t x, coord_t y, gu16 dx, gu16 dy)
{
	gui_extend_t extend;
	coord_t ix, iy, iw, ih;
	int i, k, j;
	t_HImage images[DIGIT_N];

	memset(images, 0, sizeof(images));
	//2 计算各图片资源大小
	if(ga_digit_extend[0].width == 0 && ga_digit_extend[0].height == 0){
		for(i = 0; i < DIGIT_N; i++){
			images[i] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_IMG_DIGIT, i));
			ga_digit_extend[i].width = image_get_width(images[i]);
			ga_digit_extend[i].height = image_get_height(images[i]);
		}
	}
	//2 计算总extend和图片显示位置
	j = 0;
	extend.width = extend.height = 0;
	for(i = 0; i < len; i++){
		switch(strPhone[i]){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				k = strPhone[i]-'0';
				break;
			case '*':
				k = DIGIT_STAR;
				break;
			case '#':
				k = DIGIT_HASH;
				break;
			default:
				k = -1;
		}
		if(k >= 0){
			if(extend.height < ga_digit_extend[k].height)
				extend.height = ga_digit_extend[k].height;
			extend.width += ga_digit_extend[k].width;
			if(ga_digit_extend[k].width && extend.width > dx)
				j++;
		}
	}
	iy = y+((dy-extend.height)>>1);
	ix = x+((dx-extend.width)>>1);
	if(ix < x)ix = x;
	for(i = j; i < len; i++){
		switch(strPhone[i]){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				k = strPhone[i]-'0';
				break;
			case '*':
				k = DIGIT_STAR;
				break;
			case '#':
				k = DIGIT_HASH;
				break;
			default:
				k = -1;
		}
		if(k >= 0){
			if(!g_object_valid(images[k]))
				images[k] = brd_get_image(BRD_HANDLE_SELF, MAKE_RESID2(RESID_IMG_DIGIT, k));
			gdi_draw_image_at(gdi, ix, iy, images[k], 0, 0, -1, -1, 1.0, 1.0);
			ix += ga_digit_extend[k].width;
		}
	}
	for(i = 0; i < DIGIT_N; i++){
		g_object_unref(images[i]);
	}
}

static void	__dialer_window_show_phone(t_HWidget handle, t_DialerWData *wdata)
{
	t_HImage image;
	t_HGDI gdi;
	int i;
	coord_t x, y, iw, ih;
	gui_rect_t *prc = &ga_dialer_pad[DIALER_PAD_BG];

	gdi = wgt_get_gdi((t_HWidget)handle);
	gdi_clear(gdi, prc->x, prc->y, prc->dx, prc->dy, guiNoCorner);

	__dialer_show_phone(gdi, wdata->strPhone, wdata->len, prc->x, prc->y, prc->dx, prc->dy);
	
	//if(!defer)
	//	gdi_blt(gdi, 0, 0, wgt_bound_dx(handle), wgt_bound_dy(handle));
	gdi_blt(gdi, prc->x, prc->y, prc->dx, prc->dy);
	g_object_unref(gdi);
}

static void	__dialer_pad_trigger(t_HWidget handle, gs8 pad, gu8 type, t_DialerWData *wdata)
{
	if(pad >= DIALER_PAD_1 && pad <= DIALER_PAD_HASH){
		if(type == MMI_PEN_DOWN){
			if(wdata->len < CONTACT_NUMBER_MAX){
				ucs2_t ucs;
				if(pad >= DIALER_PAD_1 && pad <= DIALER_PAD_9)
					ucs = '1' + pad -DIALER_PAD_1;
				else if(pad == DIALER_PAD_STAR)
					ucs = '*';
				else if(pad == DIALER_PAD_0)
					ucs = '0';
				else if(pad == DIALER_PAD_HASH)
					ucs = '#';
				wdata->strPhone[wdata->len++] = ucs;
				wdata->strPhone[wdata->len] = 0;
				__dialer_window_show_phone(handle, wdata);
			}
		}else if(type == MMI_PEN_LONG_PRESS){
			ucs2_t ucs;
			if(pad == DIALER_PAD_STAR)
				ucs = ',';
			else if(pad == DIALER_PAD_0)
				ucs = '+';
			else if(pad == DIALER_PAD_HASH)
				ucs = ';';
			else return;
			wdata->strPhone[wdata->len-1] = ucs;
			__dialer_window_show_phone(handle, wdata);
		}
	}else if(pad == DIALER_PAD_ADD){
		if(type == MMI_PEN_UP){
		}
	}else if(pad == DIALER_PAD_CALL){
		if(type == MMI_PEN_UP && wdata->len){
			call_LoadOutgoing(0xff, wdata->strPhone, CSESSION_NULL);
		}
	}else if(pad == DIALER_PAD_BKSPACE){
		if((type == MMI_PEN_PRESS || type == MMI_PEN_REPEAT) && wdata->len > 0){
			wdata->len--;
			wdata->strPhone[wdata->len] = 0;
			__dialer_window_show_phone(handle, wdata);
		}
	}
}

//2 拨号键盘各按键事件有所不同
//3  数字键: down时绘制焦点，输入，up时重新绘制，其它事件都不须响应
//3 add/call: down时绘制焦点，move时检测是否需要重绘，up响应事件
//3 del: down绘制焦点，move时检测是否需要重绘，响应press/repeat
static error_t	call_DialerPadOnEvent(t_HWidget handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam, void *userdata)
{
	t_CallWndData *wdata = (t_CallWndData *)wgt_get_tag(wgt_parent_window(handle));
	t_DialerWData *dialer = &wdata->dialer;
	error_t ret = NOT_HANDLED;
	coord_t x0, y0;
	gs8 r0;
	switch(evt){
	case WIDGET_OnDraw:
		__dialer_window_show_phone(handle, dialer);
		break;
	case WIDGET_OnPenEvent:
		PEN_XY(lparam, x0, y0);
		r0 = __dialer_pad_get(x0, y0);
		switch(PEN_TYPE(lparam)){
		case MMI_PEN_DOWN:
			dialer->r_down = r0;
			dialer->moved = FALSE;
			if(r0 > DIALER_PAD_BG){
				__dialer_pad_show(handle, NULL, r0, TRUE, FALSE);
				__dialer_pad_trigger(handle, r0, PEN_TYPE(lparam), dialer);
			}
			break;
		case MMI_PEN_PRESS:
		case MMI_PEN_REPEAT:
		case MMI_PEN_LONG_PRESS:
			if(dialer->r_down == DIALER_PAD_BKSPACE){
				__dialer_pad_trigger(handle, dialer->r_down, PEN_TYPE(lparam), dialer);
			}
			break;
		case MMI_PEN_UP:
			if(r0 > DIALER_PAD_BG){
				__dialer_pad_show(handle, NULL, r0, FALSE, FALSE);
				__dialer_pad_trigger(handle, r0, PEN_TYPE(lparam), dialer);
			}
			break;
		case MMI_PEN_MOVE:
			if(dialer->r_down >= DIALER_PAD_ADD){
				if(r0 != dialer->r_down){
					if(!dialer->moved){
						dialer->moved = TRUE;
						__dialer_pad_show(handle, NULL, dialer->r_down, FALSE, FALSE);
					}
				}else{
					if(dialer->moved){
						dialer->moved = FALSE;
						__dialer_pad_show(handle, NULL, dialer->r_down, TRUE, FALSE);
					}
				}
			}
			break;
		case MMI_PEN_MOVE_IN:
			break;
		case MMI_PEN_MOVE_OUT:
			break;
		}
		break;
	}
	ret = SUCCESS;
	return ret;
}

error_t	call_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	call_glob_t *glob = app_get_data();
	t_CallWndData *wdata = (t_CallWndData *)wgt_get_tag(handle);
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			{
				t_HWidget widget;

				wgt_enable_attr(handle, WND_ATTR_CLOSE_APP|WND_ATTR_AUTO_CLOSE|WND_ATTR_HAS_STATUS|WND_ATTR_HAS_TITLE);
				wdata = MALLOC(sizeof(t_CallWndData));
				wdata->dialer.strPhone[0] = 0;
				wdata->dialer.len = 0;
				wdata->dialer.r_down = -1;
				wgt_set_tag(handle, wdata);
				
				widget = wnd_get_widget(handle, RESID_TABCTRL_MAIN);
				wgt_freeze(widget);
				#if (LCD_HEIGHT == 480)
				tabctrl_set_title_style(widget, 50, guiBottom);
				#else
				tabctrl_set_title_style(widget, 40, guiBottom);
				#endif
				tabctrl_set_cursor(widget, 3);
				wgt_unfreeze(widget);

				widget = wnd_get_widget(handle, RESID_PANEL_DIALER);
				wgt_hook_register(widget, WIDGET_OnPenEvent|WIDGET_OnDraw, call_DialerPadOnEvent, NULL);
			}
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			FREEIF(wdata);
			ret = SUCCESS;
			break;
		case WIDGET_OnSetFocus:
			break;
		case WIDGET_OnKillFocus:
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam != MMI_KEY_PRESS && sparam != MMI_KEY_REPEAT)
				break;
			switch(lparam){
				case MVK_RSK:
					break;
				case MVK_SEND1:
				case MVK_SEND2:
				case MVK_SEND3:
				case MVK_SEND4:
				case MVK_SEND:
					break;
				
				case MVK_END:
					break;
			}
			break;
	}
	return ret;
}

