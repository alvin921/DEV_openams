#include "pack_sms.hi"





#if defined(FTR_COMM_SESSION_SUPPORT) && !defined(SMALL_LCD)


typedef struct {
	t_DataSource	common;
	
	t_HDataSet	hds;
	gu16	rec_num;
}t_DSSMS;

static error_t		dsource_OnCreate(t_DSSMS* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	pme->hds = (t_HDataSet)d1;
	g_object_ref(pme->hds);
	pme->rec_num = g_ds_GetRecordNum(pme->hds);

	return SUCCESS;
}

static void	dsource_OnDestroy(t_DSSMS* pme)
{
	g_object_unref(pme->hds);
}

//2 =====================================================
//3 显示风格:
//3 <LINE_Y_SPLIT>
//3 <Time>						居中显示
//3 <LINE_Y_SPLIT>
//3 <Content>				tx: 靠右显示；rx: 靠左显示
//3 <LINE_Y_SPLIT>

//2 其中

//3 <Content>如果是收到的信息则靠左显示:	<x_margin><bound_for_content><blank>
//3 <Content>如果是发送的信息则靠右显示:	<blank><bound_for_content><x_margin>
//2 =====================================================

#define	BOUND_X_MARGIN	8
#define	BOUND_Y_MARGIN	5
#define	BOUND_DX_MAX		(lcd_get_width()*5/8)


#define	BOUND_BORDER_COLOR	RGB_GRAY
#define	BOUND_FILL_COLOR1		RGB_LTGRAY
#define	BOUND_FILL_COLOR2		MAKE_RGB(0,192,0)
#define	BOUND_FILL_COLOR_HL	MAKE_RGB(0,0,192)

static gboolean	dsource_Measure(t_HDataSource h, t_GuiViewType viewtype, int index, int *width, int *height)
{
	if(index&1){
		t_DSSMS* pme = (t_DSSMS *)h;
		gu8 dcs, *data;
		gu16 size;
		int ww, hh;
		//2 内容
		g_ds_GotoByIndex(pme->hds, index/2);
		dcs = g_ds_GetValue(pme->hds, SMS_DBFIELD_DCS);
		data = g_ds_GetBinary(pme->hds, SMS_DBFIELD_DATA, &size);
		
		if(dcs == DCS_7BIT){
			hh = font_measure_string_multiline((const char *)data, size, CHARSET_US_ASCII, BOUND_DX_MAX, &ww, NULL);
		}else if(dcs == DCS_UCS2){
			hh = font_measure_ustring_multiline((const ucs2_t *)data, size>>1, BOUND_DX_MAX, &ww, NULL);
		}else{
			hh = font_measure_string_multiline("<unknown>", -1, CHARSET_US_ASCII, BOUND_DX_MAX, &ww, NULL);
		}
		if(width)*width = BOUND_X_MARGIN+ww+BOUND_X_MARGIN;
		if(height)*height = BOUND_Y_MARGIN+hh+BOUND_Y_MARGIN;
	}else{
		//2 时间
		if(width)*width = -1;
		if(height)*height = font_get(lang_get_current())->height;
	}
	return TRUE;
}

static void	dsource_Redraw(t_HDataSource h, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight)
{
	t_DSSMS* pme = (t_DSSMS *)h;
	gu8 dcs, *data, type;
	gu16 size;
	coord_t x;
	int fh, th, ww, hh;
	datetime_t dt;
	gboolean left;
	char msg[24];

	g_ds_GotoByIndex(pme->hds, index/2);
	if(index&1){
		//2 display bound & content
		type = g_ds_GetValue(pme->hds, SMS_DBFIELD_TYPE);
		dcs = g_ds_GetValue(pme->hds, SMS_DBFIELD_DCS);
		data = g_ds_GetBinary(pme->hds, SMS_DBFIELD_DATA, &size);
		left = (type == SMS_MO_SENT || type == SMS_MO_SENDING)?FALSE:TRUE;
		if(dcs == DCS_7BIT){
			hh = font_measure_string_multiline((const char *)data, size, CHARSET_US_ASCII, BOUND_DX_MAX, &ww, NULL);
		}else if(dcs == DCS_UCS2){
			hh = font_measure_ustring_multiline((const ucs2_t *)data, size>>1, BOUND_DX_MAX, &ww, NULL);
		}else{
			hh = font_measure_string_multiline("<unknown>", -1, CHARSET_US_ASCII, BOUND_DX_MAX, &ww, NULL);
		}
		if(left){
			x = LINE_X_MARGIN;
		}else{
			x = width-LINE_X_MARGIN-(BOUND_X_MARGIN+ww+BOUND_X_MARGIN);
		}
		//3 <bound> border
		if(hilight)
			gdi_set_color(gdi, BOUND_FILL_COLOR_HL);
		else if(left)
			gdi_set_color(gdi, BOUND_FILL_COLOR1);
		else
			gdi_set_color(gdi, BOUND_FILL_COLOR2);
		gdi_rectangle_fill(gdi, x, 0, BOUND_X_MARGIN+ww+BOUND_X_MARGIN, BOUND_Y_MARGIN+hh+BOUND_Y_MARGIN, guiFourCorner);
		gdi_set_color(gdi, BOUND_BORDER_COLOR);
		gdi_rectangle(gdi, x, 0, BOUND_X_MARGIN+ww+BOUND_X_MARGIN, BOUND_Y_MARGIN+hh+BOUND_Y_MARGIN, guiFourCorner);
		//3 <content>
		gdi_set_color(gdi, RGB_BLACK);
		if(dcs == DCS_7BIT){
			gdi_draw_text_in(gdi, x+BOUND_X_MARGIN, BOUND_Y_MARGIN, ww, hh, (const char *)data, size, CHARSET_US_ASCII, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE|IDF_TEXT_MULTILINE);
		}else if(dcs == DCS_UCS2){
			gdi_draw_utext_in(gdi, x+BOUND_X_MARGIN, BOUND_Y_MARGIN, ww, hh, (const ucs2_t *)data, size>>1, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE|IDF_TEXT_MULTILINE);
		}else{
			gdi_draw_text_in(gdi, x+BOUND_X_MARGIN, BOUND_Y_MARGIN, ww, hh, "<unknown>", -1, CHARSET_US_ASCII, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE|IDF_TEXT_MULTILINE);
		}
	}else{
		//gdi_set_color(gdi, MAKE_RGB(192,192,255));
		//gdi_rectangle_fill(gdi, 0, 0, width, height, 0);
		//2 display <time>
		gdi_set_color(gdi, RGB_DKGRAY);
		dostime_to_datetime(g_ds_GetValue(pme->hds, SMS_DBFIELD_TIME), &dt);
#if defined(FTR_IPHONE_STYLE)
		sprintf(msg, "%04d-%02d-%02d %02d:%02d", dt.dt_year, dt.dt_month, dt.dt_day, dt.dt_hour, dt.dt_minute);
#else
		sprintf(msg, "%04d-%02d-%02d %02d:%02d, SIM%d", dt.dt_year, dt.dt_month, dt.dt_day, dt.dt_hour, dt.dt_minute, 
				g_ds_GetValue(pme->hds, SMS_DBFIELD_SIM)+1);
#endif
		gdi_draw_text_in(gdi, 0, 0, width, height, msg, -1, CHARSET_US_ASCII, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
	}
}

static int	dsource_GetCount(t_HDataSource h)
{
	t_DSSMS* pme = (t_DSSMS *)h;
	return g_ds_GetRecordNum(pme->hds)*2;
}
static void	__dsource_popup(t_DSSMS *pme, int index)
{
	if((index&1) && index >= 0 && index/2 < g_ds_GetRecordNum(pme->hds)){
		gu8 type;
		gui_rect_t rt;
		t_HWindow win = wgt_parent_window(pme->common.list);
		t_HWidget menu = wnd_get_widget(win, RESID_MENU_POPUP);
		
		g_ds_GotoByIndex(pme->hds, index/2);
		type = g_ds_GetValue(pme->hds, SMS_DBFIELD_TYPE);
		if(type == SMS_MO_SENT || type == SMS_MO_SENDING){
			menu_SetItemVisible(menu, RESID_STR_RESEND, IDENT_BY_ID, TRUE);
		}else{
			menu_SetItemVisible(menu, RESID_STR_RESEND, IDENT_BY_ID, FALSE);
		}

		listctrl_rect_by_index(pme->common.list, index, &rt);
		wgt_popup(pme->common.list, menu, rt.x, rt.y-2, guiTop|guiLeft|guiRight);
	}
}
static gboolean	dsource_CanFocused(t_HDataSource h, int index)
{
	return (index&1)?TRUE:FALSE;
}

static error_t dsource_on_event(t_HDataSource h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	t_DSSMS* pme = (t_DSSMS *)h;
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnSelected:
			break;
			
		case WIDGET_OnKeyEvent:
#if !defined(FTR_NO_INPUT_KEY)
			if(sparam == MMI_KEY_LONG_PRESS && lparam == MVK_OK && g_ds_GetRecordNum(pme->hds) != 0){
				__dsource_popup(pme, listctrl_get_cursor(h->list));
				ret = SUCCESS;
			}
#endif
			break;

#if defined(FTR_TOUCH_SCREEN)
		case WIDGET_OnPenEvent:
		{
			coord_t x0, y0;
			PEN_XY(lparam, x0, y0);
			switch(PEN_TYPE(lparam)){
				case MMI_PEN_LONG_PRESS:
					//2 PenLongPress就表示没有PenMove
					{
						int index = listctrl_index_by_point(h->list, &x0, &y0, NULL, NULL);
						__dsource_popup(pme, index);
						ret = SUCCESS;
					}
					break;
				case MMI_PEN_DOWN:
					if(0){
						int row, col;
						int index = listctrl_index_by_point(h->list, &x0, &y0, &row, &col);
							//2 在此加hook是考虑到某些场合用户需要截获消息
							#if 0
							if(LIST_VIEWTYPE_CUSTOM(pme) && pme->s_type == DSOURCE_TYPE_CUSTOM && (pme->flags&CLFLAG_HILIGHT_SHOW_CUSTOM)){
								ret = LISTSOURCE_HookEvent(pme->u.s_custom.hds, evt, sparam, lparam);
								if(METH_HANDLED(ret))
									break;
							}
							#endif
							listctrl_set_cursor(h->list, index, TRUE);
					}
					break;
			}
			break;
		}
#endif
	}
	return ret;
}


static VTBL(datasource) dsourceVtbl = {
	(PFNONCREATE)dsource_OnCreate
	, (PFNONDESTROY)dsource_OnDestroy

	, dsource_Measure
	, dsource_Redraw
	, dsource_GetCount
	, dsource_CanFocused			// CanFocused
	, dsource_on_event
	, NULL			// Refresh
};

CLASS_DECLARE(DSSMS, t_DSSMS, dsourceVtbl);

t_HDataSource	ds_sms_new(t_HDataSet hds)
{
	return g_object_new1(&class_DSSMS, hds);
}

int	ds_sms_delete(t_HDataSource h)
{
	int num = 0;
	if(g_object_valid(h) && g_object_class(h) == &class_DSSMS){
		t_DSSMS* pme = (t_DSSMS *)h;
		num = listctrl_get_num_select(h->list);
		if(num > 0){
			t_DBRecordID *recid = MALLOC(sizeof(t_DBRecordID)*num);
			int i = 0, s = -1;
			while(1){
				s = listctrl_enum_select(h->list, s);
				if(s < 0)
					break;
				g_ds_GotoByIndex(pme->hds, s/2);
				recid[i++] = g_ds_CurrentID(pme->hds);
			}
			for(i = 0; i < num; i++){
				g_ds_Goto(pme->hds, recid[i]);
				g_ds_Delete(pme->hds);
			}
			FREE(recid);
		}
	}
	return num;
}

int	ds_sms_forward(t_HDataSource h)
{
	int num = 0;
	if(g_object_valid(h) && g_object_class(h) == &class_DSSMS){
		t_DSSMS* pme = (t_DSSMS *)h;
		num = listctrl_get_num_select(h->list);
		if(num > 0){
			char *str = NULL, *new_str;
			int len = 0, tmp, s;
			
			gu8 dcs, *data;
			gu16 size;

			s = -1;
			while(1){
				s = listctrl_enum_select(h->list, s);
				if(s < 0)
					break;
				g_ds_GotoByIndex(pme->hds, s/2);
				
				dcs = g_ds_GetValue(pme->hds, SMS_DBFIELD_DCS);
				data = g_ds_GetBinary(pme->hds, SMS_DBFIELD_DATA, &size);
				if(dcs == DCS_UCS2){
					tmp = ustring_to_utf8((const ucs2_t *)data, size>>1, NULL, -1);
					new_str = MALLOC(len + tmp + 1+1);
					if(str && len > 0)
						memcpy(new_str, str, len);
					ustring_to_utf8((const ucs2_t *)data, size>>1, new_str+len, -1);
					size = tmp;
				}else{
					new_str = MALLOC(len + size + 1+1);
					if(str && len > 0)
						memcpy(new_str, str, len);
					memcpy(new_str+len, data, size);
				}
				new_str[len+size] = '\n';
				new_str[len+size+1] = 0;
				FREEIF(str);
				str = new_str;
				len += size+1;
			}
			wnd_new_message(WITH_CONTENT, new_str);
			FREE(new_str);
		}
	}
	return num;
}

#endif


