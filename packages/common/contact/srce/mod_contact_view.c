#include "pack_contact.hi"

#if defined(FTR_IPHONE_STYLE)
enum {
	F_PHONE,
	F_EMAIL,
	F_ADDRESS,

	F_DELETE,
	
	F_N
};
#endif

#if defined(FTR_SOFT_KEY) && !defined(FTR_IPHONE_STYLE)
const char *ga_mime_contact[] = {
	MIME_TYPE_CONTACT,
	MIME_TYPE_EMAIL,
	MIME_TYPE_ADDRESS,
};

#define CONTACT_MIME_TYPE_N	3
#endif

typedef struct {
	t_HDataSet		hdb;
	t_DBRecordID		rid;
	gs16 	n_phone;
	contact_number_t parsed_phone;
#if defined(FTR_IPHONE_STYLE)
	gui_rect_t	f_rect[F_N];
	gs8			f_down;
	gs8			p_down;		// phone number index for <f_down>=F_PHONE
	MC_FILL_STRUCT2;
#else
	#if defined(FTR_SOFT_KEY)
	int		n_ms_start[CONTACT_MIME_TYPE_N];
	#endif
#endif
}t_ViewContactWData;


#if defined(FTR_SOFT_KEY) && !defined(FTR_IPHONE_STYLE)

enum {
	RESID_LIST_OPERATION = RESID_CTRL_DYNAMIC
};
static void	CmdEditContact(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	t_ViewContactWData *wdata = (t_ViewContactWData *)wgt_get_tag(wnd);
	
	contact_EditWindow(wdata->hdb, wdata->rid);
}

static void	CmdDelContact(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	t_ViewContactWData *wdata = (t_ViewContactWData *)wgt_get_tag(wnd);

	if(msgbox_show(brd_get_string(BRD_HANDLE_SELF, RESID_STR_DEL_CONTACT, NULL), MBB_YES_NO, 0, 0, brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_DEL_CONTACT, NULL)) == MBB_YES){
		g_ds_GotoByIndex(wdata->hdb, wdata->rid);
		g_ds_Delete(wdata->hdb);
		wnd_send_close(wnd, 0);
	}
}

static void	CmdSendContact(t_HMenu handle, void *data)
{
	t_HWindow wnd = data;
	t_ViewContactWData *wdata = (t_ViewContactWData *)wgt_get_tag(wnd);
	t_HWidget listctrl;

	listctrl = wnd_get_widget(wnd, RESID_LIST_OPERATION);
	if(g_object_valid(listctrl)){
		wgt_popup(wnd, listctrl, 0, lcd_get_height()-prod_ui_sk_height(), guiBottom);
	}
}

static t_MenuItem miContact[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_DEL_CONTACT, CmdDelContact),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_EDIT_CONTACT, CmdEditContact),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_OPERATION, CmdSendContact),
	MITEM_END
};

static void	__ms_listctrl_on_select(t_HDataSource h, const char *guid, t_AmsModuleId module, void *userdata)
{
	t_HWindow win = userdata;
	t_ViewContactWData *wdata = (t_ViewContactWData *)wgt_get_tag(win);
	int cursor = (int)listctrl_get_cursor(h->list);
	char *string = NULL;

	g_ds_Goto(wdata->hdb, wdata->rid);

	if(cursor >= wdata->n_ms_start[2]){
		//2 address
		string = g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_ADDRESS);
		if(string && *string)
			pack_call(guid, module, (void*)string, strlen(string)+1, FALSE, NULL);
	}else if(cursor >= wdata->n_ms_start[1]){
		//2 email
		string = g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_EMAIL);
		if(string && *string)
			pack_call(guid, module, (void*)string, strlen(string)+1, FALSE, NULL);
	}else if(cursor >= 0){
		//3 Contact
		string = g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_NUMBER);
		if(string && *string){
			int n;
			
			n = contact_number_parse(string, &wdata->parsed_phone);
			if(n <= 0)return;
			if(n == 1){
				for(cursor = 0; cursor < CONTACT_F_NUMBER_N; cursor++){
					if(wdata->parsed_phone.number[cursor][0]){
						break;
					}
				}
			}else{
				cursor = wnd_modal(wnd_create_by_resid(contact_SelectPhoneWndMsgHandler, n, (t_lparam)&wdata->parsed_phone, brd_get(PACK_GUID_SELF), RESID_WIN_SELECT_PHONE));
			}
			if(cursor >= 0 && cursor < CONTACT_F_NUMBER_N){
				t_MSArgContact arg;
				arg.sid = CSESSION_ID(wdata->rid, ga_phone_type[cursor]);
				strcpy(arg.number, wdata->parsed_phone.number[cursor]);
				pack_call(guid, module, &arg, sizeof(arg), FALSE, NULL);
			}
		}
	}
}

#endif

#if defined(FTR_IPHONE_STYLE)

#define	MARGIN_X	5
#define	BLOCK_SPLIT_Y	10
#define	BLOCK_DY	30

#define	BLOCK_TITLE_DX	60
#define	BLOCK_TITLE_COLOR	RGB_LTBLUE

#define	PROFILE_DY		40

#define	PROFILE_PIC_X		MARGIN_X
#define	PROFILE_PIC_Y		BLOCK_SPLIT_Y
#define	PROFILE_PIC_DX		PROFILE_DY
#define	PROFILE_PIC_DY		PROFILE_DY

#define	PROFILE_NAME_X	(MARGIN_X+PROFILE_DY+MARGIN_X)
#define	PROFILE_NAME_Y	PROFILE_PIC_Y

static gs8		__get_area(t_HWindow handle, coord_t x, coord_t y, gs8 *p_num_idx)
{
	if(wgt_point_in_client((t_HWidget)handle, &x, &y)){
		t_ViewContactWData *wdata = (t_ViewContactWData *)wgt_get_tag(handle);
		gs8 f;
		
		for(f =  0; f < F_N; f++){
			if(point_in_rect(x, y, &wdata->f_rect[f])){
				if(f == F_PHONE){
					if(p_num_idx)*p_num_idx = (y-wdata->f_rect[f].y)/BLOCK_DY;
				}
				return f;
			}
		}
	}
	return -1;
}

static void	__display_area(t_HGDI gdi, gs8 f, gs8 p, gboolean focused, gboolean defer, t_HWindow handle)
{
	t_ViewContactWData *wdata = (t_ViewContactWData *)wgt_get_tag(handle);
	char *title, *str;
	gui_direction_e dir = guiFourCorner;
	coord_t x, y;
	gu16 dx, dy;

	if(g_object_valid(gdi))
		g_object_ref(gdi);
	else
		gdi = wgt_get_client_gdi((t_HWidget)handle);
	
	x = wdata->f_rect[f].x;
	y = wdata->f_rect[f].y;
	dx = wdata->f_rect[f].dx;
	dy = BLOCK_DY;
	switch(f){
	case F_PHONE:
		{
			int i;

			y = wdata->f_rect[f].y + (BLOCK_DY-1)*p;

			dir = 0;
			if(p == 0)	//2 第一项
				dir |= guiLeftTop | guiRightTop;
			if(p == wdata->n_phone-1)	//2  最后一项
				dir |= guiLeftBottom | guiRightBottom;

			i = contact_get_fnum_by_index(&wdata->parsed_phone, p);
			if(i < 0)
				goto out;
			title = brd_get_string(brd_get(PACK_GUID_SELF), ga_phone_type_resid[i], NULL);
			str = wdata->parsed_phone.number[i];
		}
		break;
	case F_EMAIL:
		str = g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_EMAIL);
		if(!str || *str == 0)
			goto out;
		title = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_EMAIL, "Email");
		break;
	case F_ADDRESS:
		str = g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_ADDRESS);
		if(!str || *str == 0)
			goto out;
		title = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_ADDRESS, "Address");
		break;
	case F_DELETE:
		title = brd_get_string(brd_get(PACK_GUID_SELF), RESID_STR_DEL_CONTACT, "Delete Contact");
		gdi_rectangle_fill_ex(gdi, x, y, dx, dy, MAKE_RGB(0xd0, 0,0), MAKE_RGB(0xb0,0,0), GDI_FILL_MODE_2COLOR_H, dir);
		gdi_set_color(gdi, RGB_WHITE);
		gdi_draw_text_in(gdi, x, y, dx, dy, title, -1, CHARSET_UTF8, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
		if(focused)
		gdi_rectangle_fill_ex(gdi, x, y, dx, dy, MAKE_ARGB(0x40, 0x00, 0x00, 0xff), MAKE_ARGB(0x40, 0x00, 0x00, 0xff), GDI_FILL_MODE_SOLID, dir);
		if(!defer)
			gdi_blt(gdi, x, y, dx, dy);
		goto out;
		break;
	default: 
		goto out;
	}
	//2 bkground
	if(focused)
		gdi_set_color(gdi, RGB_BLUE);
	else
		gdi_set_color(gdi, RGB_WHITE);
	gdi_rectangle_fill(gdi, x, y, dx, dy, dir);
	gdi_set_color(gdi, RGB_DKGRAY);
	gdi_rectangle(gdi, x, y, dx, dy, dir);
	//2 title
	if(focused)
		gdi_set_color(gdi, RGB_WHITE);
	else
		gdi_set_color(gdi, BLOCK_TITLE_COLOR);
	gdi_draw_text_in(gdi, x, y, BLOCK_TITLE_DX, dy, title, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT|IDF_VALIGN_MIDDLE);
	//2 phone number
	if(focused)
		gdi_set_color(gdi, RGB_WHITE);
	else
		gdi_set_color(gdi, RGB_BLACK);
	gdi_draw_text_in(gdi, x+BLOCK_TITLE_DX+MARGIN_X, y, dx-BLOCK_TITLE_DX-MARGIN_X, dy, 
		str, -1, CHARSET_UTF8, IDF_HALIGN_LEFT|IDF_VALIGN_MIDDLE);
	if(!defer)
		gdi_blt(gdi, x, y, dx, dy);
out:
	g_object_unref(gdi);
}

static void	__iphone_view_display(t_HWindow handle)
{
	t_ViewContactWData *wdata = (t_ViewContactWData *)wgt_get_tag(handle);
	t_HGDI gdi = wgt_get_client_gdi(handle);
	const char *string;
	coord_t y;
	wdata->n_phone = contact_number_parse(g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_NUMBER), &wdata->parsed_phone);

	//2 显示照片、姓名、公司等信息
	gdi_set_color(gdi, RGB_BLACK);
	gdi_rectangle_3d(gdi, PROFILE_PIC_X, PROFILE_PIC_Y, PROFILE_PIC_DX, PROFILE_PIC_DY, 1, guiNoCorner, TRUE);

	gdi_set_color(gdi, RGB_BLACK);
	string = g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_NAME);
	gdi_draw_text_at(gdi, PROFILE_NAME_X, PROFILE_NAME_Y, string, -1, CHARSET_UTF8);
	y = PROFILE_PIC_Y+PROFILE_PIC_DY+BLOCK_SPLIT_Y;
	//2 电话号码
	wdata->f_rect[F_PHONE].x = MARGIN_X;
	wdata->f_rect[F_PHONE].y = y;
	wdata->f_rect[F_PHONE].dx = lcd_get_width()-MARGIN_X-MARGIN_X;
	wdata->f_rect[F_PHONE].dy = wdata->n_phone*BLOCK_DY;
	
	if(wdata->n_phone){
		int i;
		for(i = 0; i < wdata->n_phone; i++){
			__display_area(gdi, F_PHONE, i, FALSE, TRUE, handle);
			y += BLOCK_DY-1;
		}
		y += BLOCK_SPLIT_Y;
	}
	
	//2 Email
	string = g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_EMAIL);
	wdata->f_rect[F_EMAIL].x = MARGIN_X;
	wdata->f_rect[F_EMAIL].y = y;
	wdata->f_rect[F_EMAIL].dx = lcd_get_width()-MARGIN_X-MARGIN_X;
	wdata->f_rect[F_EMAIL].dy = (string&&*string)?BLOCK_DY:0;
	__display_area(gdi, F_EMAIL, 0, FALSE, TRUE, handle);
	if(string && *string){
		y += BLOCK_DY+BLOCK_SPLIT_Y;
	}
	//2 Address
	string = g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_ADDRESS);
	wdata->f_rect[F_ADDRESS].x = MARGIN_X;
	wdata->f_rect[F_ADDRESS].y = y;
	wdata->f_rect[F_ADDRESS].dx = lcd_get_width()-MARGIN_X-MARGIN_X;
	wdata->f_rect[F_ADDRESS].dy = (string&&*string)?BLOCK_DY:0;
	__display_area(gdi, F_ADDRESS, 0, FALSE, TRUE, handle);
	if(string && *string){
		y += BLOCK_DY+BLOCK_SPLIT_Y;
	}
	//2 Delete Contact
	wdata->f_rect[F_DELETE].x = MARGIN_X;
	wdata->f_rect[F_DELETE].y = y;
	wdata->f_rect[F_DELETE].dx = lcd_get_width()-MARGIN_X-MARGIN_X;
	wdata->f_rect[F_DELETE].dy = BLOCK_DY;
	__display_area(gdi, F_DELETE, 0, FALSE, TRUE, handle);
		y += BLOCK_DY+BLOCK_SPLIT_Y;

	//wgt_client_drawing_dy(handle) = y;
	g_object_unref(gdi);
}

static void	__LskCallback(t_HWidget sk, t_HWindow handle)
{
	t_ViewContactWData *wdata = (t_ViewContactWData *)wgt_get_tag(handle);
	
	contact_EditWindow(wdata->hdb, wdata->rid);
}

#endif

//2 sparam=<ID>, lparam=<hdb>
//2 hdb非空表示内部加载，id为索引号；
//2 hdb为空则表示外部加载，id为t_ContactID，即记录号
error_t	contact_ViewWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_ViewContactWData *wdata = (t_ViewContactWData *)wgt_get_tag(handle);
	
	switch(evt){
		case WINDOW_OnOpen:
			{
				contact_number_t *phone;
				t_HWidget widget;

				wdata = MALLOC(sizeof(t_ViewContactWData));
				memset(wdata, 0, sizeof(t_ViewContactWData));
				
				wgt_enable_attr(handle, WND_ATTR_DEFAULT|CTRL_ATTR_VSCROLLABLE);
				wdata->hdb = (t_HDataSet)lparam;
				//2 hdb为空则表示外部加载，id为t_ContactID，即记录号
				//2 hdb非空表示内部加载，id为索引号；
				if(wdata->hdb == NULL){
					wdata->hdb = g_ds_open(CONTACT_DB_FNAME, CONTACT_DBTAB_LIST);
					g_ds_Goto(wdata->hdb, sparam);
					wdata->rid = sparam;
				}else{
					g_ds_GotoByIndex(wdata->hdb, sparam);
					wdata->rid = g_ds_CurrentID(wdata->hdb);
				}
				wgt_set_tag(handle, wdata);
#if defined(FTR_SOFT_KEY)
				widget = wnd_get_skbar(handle);
				if(g_object_valid(widget)){
					#if !defined(FTR_IPHONE_STYLE)
					t_HWidget menu;
					menu = wnd_get_widget(handle, RESID_MENU_OPTION);
					menu_bind(menu, miContact, handle);
					sk_set_popup(widget, SK_LSK, menu);
					sk_set_text(widget, SK_LSK, NULL, RESID_STR_OPTION);
					#else
					sk_set_text(widget, SK_LSK, NULL, RESID_STR_EDIT);
					sk_set_callback(widget, SK_LSK, (t_WidgetCallback)__LskCallback, handle);
					#endif
					sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
				}
#endif

				wdata->n_phone = contact_number_parse(g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_NUMBER), &wdata->parsed_phone);
#if defined(FTR_IPHONE_STYLE)
				wgt_set_caption_by_resid((t_HWidget)handle, brd_get(PACK_GUID_SELF), RESID_STR_PROFILE);
				wgt_set_bg_fill_solid(handle, RGB_LTGRAY);
#else
				widget = wnd_get_widget(handle, RESID_LABEL_NAME);
				label_set_text(widget, g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_NAME));
				phone = &wdata->parsed_phone;
				label_set_text(wnd_get_widget(handle, RESID_LABEL_MOBILE), phone->number[F_NUMBER_MOBILE]);
				label_set_text(wnd_get_widget(handle, RESID_LABEL_OFFICE), phone->number[F_NUMBER_OFFICE]);
				label_set_text(wnd_get_widget(handle, RESID_LABEL_HOME), phone->number[F_NUMBER_HOME]);
				label_set_text(wnd_get_widget(handle, RESID_LABEL_FAX), phone->number[F_NUMBER_FAX]);
				label_set_text(wnd_get_widget(handle, RESID_LABEL_OTHER), phone->number[F_NUMBER_OTHER]);
				
				label_set_text(wnd_get_widget(handle, RESID_LABEL_EMAIL), g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_EMAIL));
				label_set_text(wnd_get_widget(handle, RESID_LABEL_ADDRESS), g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_ADDRESS));
				label_set_text(wnd_get_widget(handle, RESID_LABEL_RING), g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_RING));
				//label_set_text(wnd_get_widget(handle, RESID_LABEL_GROUP), g_ds_GetString(hdb, CONTACT_DBFIELD_NAME));
				label_set_text(wnd_get_widget(handle, RESID_LABEL_PICTURE), g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_PICTURE));
				
#if defined(FTR_SOFT_KEY)
				{
					t_HWidget menu;
					menu = wnd_get_widget(handle, RESID_MENU_OPTION);
					menu_bind(menu, miContact, handle);
					widget = wnd_get_skbar(handle);
					if(g_object_valid(widget)){
						sk_set_text(widget, SK_LSK, NULL, RESID_STR_OPTION);
						sk_set_text(widget, SK_RSK, NULL, RESID_STR_BACK);
						sk_set_popup(widget, SK_LSK, menu);
					}

					//3 POPUP list for menu item <RESID_STR_OPERATION>
					widget = wgt_create((t_HWidget)handle, "listctrl");
					if(g_object_valid(widget)){
						t_HDataSource ds;
						int i, n;
						
						wgt_set_resid(widget, RESID_LIST_OPERATION);
						//wgt_enable_attr(widget, CTRL_ATTR_POPUP);
						wgt_disable_attr(widget, CTRL_ATTR_VISIBLE);
						wgt_set_bound(widget, 0, lcd_get_height()-130, lcd_get_width(), 130);
						wgt_set_border(widget, BORDER_TYPE_LINE, 5, guiFourDir);
						ds = DSAppStore_New(__ms_listctrl_on_select, handle);
						wdata->n_ms_start[0] = 0;
						for(i = 0; i < CONTACT_MIME_TYPE_N; i++){
							n = AppStore_AddByMime(DSAppStore_GetRootNode(ds), ga_mime_contact[i], NULL);
							if(i+1 < CONTACT_MIME_TYPE_N)
								wdata->n_ms_start[i+1] = wdata->n_ms_start[i] + n;
						}
						listctrl_attach_datasource(widget, ds);
						g_object_unref(ds);

						listctrl_set_viewtype(widget, GUI_VIEWTYPE_ICON_TEXT_BOTTOM);
						listctrl_set_flags(widget, CLFLAG_SCROLLING_HORIZONTAL|CLFLAG_SCROLLING_PAGE|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE);
						listctrl_set_rows(widget, 2, 0, 0);
#if (LCD_WIDTH == 240) && (LCD_HEIGHT == 320)
						listctrl_set_cols(widget, 4, 0);
#elif (LCD_WIDTH == 320) && (LCD_HEIGHT == 240)
						listctrl_set_cols(widget, 4, 0);
#elif (LCD_WIDTH == 176) && (LCD_HEIGHT == 220)
						listctrl_set_cols(widget, 3, 0);
#elif (LCD_WIDTH == 220) && (LCD_HEIGHT == 176)
						listctrl_set_cols(widget, 4, 0);
#elif (LCD_WIDTH == 128) && (LCD_HEIGHT == 160)
						listctrl_set_cols(widget, 3, 0);
#endif
					}
				}
#endif	/* FTR_SOFT_KEY */
#endif	/* FTR_IPHONE_STYLE */
			}
			
			
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			break;
		case WINDOW_OnClose:
			if(wdata){
				if(app_current_module() == MOD_ID_CONTACT_OPEN)
					g_object_unref(wdata->hdb);
				FREE(wdata);
			}
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			#if defined(FTR_IPHONE_STYLE)
			__iphone_view_display(handle);
			#endif
			ret = SUCCESS;
			break;
#if defined(FTR_TOUCH_SCREEN) && defined(FTR_IPHONE_STYLE)
		case WIDGET_OnPenEvent:
			{
				coord_t x0, y0;
				int index;
				PEN_XY(lparam, x0, y0);
				switch(PEN_TYPE(lparam)){
					case MMI_PEN_DOWN:
						wdata->f_down = __get_area(handle, x0, y0, &wdata->p_down);
						if(wdata->f_down >= 0){
							__display_area(NULL, wdata->f_down, wdata->p_down, TRUE, FALSE, handle);
						}
						ret = SUCCESS;
						break;
					case MMI_PEN_UP:
						{
							gs8 f, p;
							f = __get_area(handle, x0, y0, &p);
							if(wdata->f_down >= 0){
								__display_area(NULL, wdata->f_down, wdata->p_down, FALSE, FALSE, handle);
								if(f == wdata->f_down){
									char *string;
									if(f == F_PHONE && (p == wdata->p_down)){
										//3 Contact
										p = contact_get_fnum_by_index(&wdata->parsed_phone, p);
										if(p >= 0){
											dlg_contact_op_show(PACK_GUID_SELF, CSESSION_ID(wdata->rid, ga_phone_type[p]), wdata->parsed_phone.number[p]);
										}
									}else if(f == F_EMAIL){
										//2 email
										string = g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_EMAIL);
										dlg_email_op_show(PACK_GUID_SELF, string);
									}else if(f == F_ADDRESS){
										//2 address
										string = g_ds_GetString(wdata->hdb, CONTACT_DBFIELD_ADDRESS);
										dlg_address_op_show(PACK_GUID_SELF, string);
									}else if(f == F_DELETE){
										//2 Delete Contact and then back to List
										if(msgbox_show(brd_get_string(BRD_HANDLE_SELF, RESID_STR_DEL_CONTACT, NULL), MBB_YES_NO, 0, 0, brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_DEL_CONTACT, NULL)) == MBB_YES){
											g_ds_Goto(wdata->hdb, wdata->rid);
											g_ds_Delete(wdata->hdb);
											wnd_send_close(handle, 0);
										}
									}
								}
							}
						}
						break;
					case MMI_PEN_MOVE:
						{
							gs8 f, p;
							f = __get_area(handle, x0, y0, &p);
						}
						break;
					case MMI_PEN_PRESS:
						break;
				}
			}	
			break;
#endif
	}
	return ret;
}


