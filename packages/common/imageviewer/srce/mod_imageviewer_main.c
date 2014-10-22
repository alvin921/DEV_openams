#include "pack_imageviewer.hi"

static void	__listctrl_redraw(t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight, t_Global *wdata)
{
	char *fname = g_strlist_get(wdata->fnames, index);
	t_HImage image;
	t_ColorRasterOp rop = gdi_set_rop(gdi, IMG_RO_COPY);

	image = image_load_from_file(fname);

	if(!g_object_valid(image))
		image = brd_get_image(brd_get(PACK_GUID_SELF), RESID_IMG_UNKNOWN);

	if(VIEWTYPE_IS_NORMAL(viewtype)){
		dentry_stat_t stat;
		datetime_t dt;
		char de_size[20]="";
		int x=1;
		
		fsi_stat(fname, &stat);
		g_printf("fname=%s, mode=%x, size=%d", fname, stat.mode, stat.size);
		if(stat.size >= BIT_20)
			sprintf(de_size, "%dM", stat.size/BIT_20);
		else if(stat.size >= BIT_10)
			sprintf(de_size, "%dK", stat.size/BIT_10);
		else
			sprintf(de_size, "%d", stat.size);
				
		gdi_draw_image_in(gdi, 1, 1, height-2, height-2, image, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
		gdi_set_rop(gdi, rop);

		x = height+1;
		width -= x;
#define	COL_SIZE_WIDTH	40
#if defined(FTR_IMAGE_VIEW_2LINE)
		//2 Line1: <fname>        <size>
		gdi_draw_text_in(gdi, x, 0, width-COL_SIZE_WIDTH, height/2, fname, -1, CHARSET_UNDEF, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
		gdi_draw_text_in(gdi, x+width-COL_SIZE_WIDTH, 0, COL_SIZE_WIDTH, height/2, de_size, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);

		//2 Line2: yyyy-mm-dd hh:mm:ss
		filetime_to_datetime(stat.mtime, &dt);
		sprintf(de_size, "%04d-%02d-%02d %02d:%02d:%02d", 
			dt.dt_year, dt.dt_month, dt.dt_day, dt.dt_hour, dt.dt_minute, dt.dt_second);
		gdi_draw_text_in(gdi, x, height/2, width, height/2, de_size, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
#else
		gdi_draw_text_in(gdi, x, 0, width-COL_SIZE_WIDTH, height, fname, -1, CHARSET_UNDEF, IDF_HALIGN_LEFT| IDF_VALIGN_MIDDLE);
		gdi_draw_text_in(gdi, x+width-COL_SIZE_WIDTH, 0, COL_SIZE_WIDTH, height, de_size, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
#endif
	}else if(VIEWTYPE_IS_ICON(viewtype)){
		gdi_draw_image_in(gdi, 0, 0, width, height, image, IDF_HALIGN_BESTFIT|IDF_VALIGN_BESTFIT);
		gdi_set_rop(gdi, rop);
	}
	g_object_unref(image);
}

static void	__image_list_refresh(t_Global *wdata, t_HWidget listctrl, gboolean redraw)
{
	void *odh;
	const char *name;

	g_strlist_reset(wdata->fnames);
	g_printf("find all images in <%s>", wdata->cwd);
	odh = fsi_opendir(wdata->cwd);
			
	g_printf("fsi_opendir(%s):%s\r\n", wdata->cwd, odh?"OK":"ERR");
	if(odh){
		dentry_stat_t stat;
		
		fsi_chdir(wdata->cwd);
		while(name = fsi_readdir(odh)){
			if(strcmp(name, ".") == 0) continue;
			if(strcmp(name, "..") == 0) continue;
			//if(name[0] == '.') continue;
			
			fsi_stat(name, &stat);
			g_printf("mode=%x", stat.mode);
		
			if(!(stat.mode & F_MODE_DIR)){
				t_HMedia media = g_media_new_for_file(name);
				if(g_object_valid(media) && MEDIA_IS_IMAGE(media->u.c_info.type)){
					g_printf("dentry=<%s>[%02x, %02x]", name, name[0], name[1]);
					g_strlist_add(wdata->fnames, name, FALSE);
				}
				g_object_unref(media);
			}
		}
		fsi_closedir(odh);
	}

	listctrl_set_count(listctrl, g_strlist_size(wdata->fnames));
	if(redraw)
		wgt_redraw(listctrl, FALSE);
}

static error_t	__image_list_on_hook(t_HWidget handle, t_WidgetEvent ev, t_sparam sparam, t_lparam lparam, void *userdata)
{
	error_t ret = NOT_HANDLED;
	t_Global *glob = app_get_data();
	switch(ev){
		case WIDGET_OnSelected:
			wnd_show(wnd_create(imageviewer_ViewWndMsgHandler, sparam, lparam));
			ret = SUCCESS;
			break;
	}
	return ret;
}


static void	CmdChooseAlbum(t_HWidget handle, void *data)
{
	t_HWindow wnd = data;
	char path[AMOS_PATH_MAX+1];
	
	if(FileDlg_Show(path, sizeof(path), "x:", NULL, OFD_DIR_OPEN)){
		t_Global *glob = app_get_data();
		if(g_stricmp(path, glob->cwd)){
			strcpy(glob->cwd, path);
			__image_list_refresh(glob, wnd_get_widget(wnd, RESID_LIST_MAIN), TRUE);
		}
	}
}
#if !defined(FTR_NO_INPUT_KEY)
enum {
	RESID_MENU_LISTVIEW = RESID_CTRL_DYNAMIC,
	RESID_MENU_CELLVIEW,
};
#endif

static void	CmdCellView(t_HWidget handle, void *data);
static void	CmdListView(t_HWidget handle, void *data)
{
	t_HWindow wnd = data;
	t_HList list = wnd_get_widget(wnd, RESID_LIST_MAIN);
	if(g_object_valid(list)){
		listctrl_set_viewtype(list, GUI_VIEWTYPE_NORMAL);
		listctrl_set_rows(list, 0, prod_ui_list_item_height(), 0);
		listctrl_set_cols(list, 1, 0);
		listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_ITEM|CLFLAG_ENABLE_ITEM_FSHOW|CLFLAG_HILIGHT_SHOW_MASK);
		wgt_redraw(list, FALSE);
		
		handle = wnd_get_skbar(wnd);
#if defined(FTR_NO_INPUT_KEY)
		sk_set_text(handle, SK_LSK, NULL, RESID_STR_CELLVIEW);
		sk_set_callback(handle, SK_LSK, CmdCellView, data);
		wgt_redraw(handle, FALSE);
#else
		sk_set_popup(handle, SK_LSK, wnd_get_widget(wnd, RESID_MENU_LISTVIEW));
#endif
	}
}

static void	CmdCellView(t_HWidget handle, void *data)
{
	t_HWindow wnd = data;
	t_HList list = wnd_get_widget(wnd, RESID_LIST_MAIN);
	if(g_object_valid(list)){
		listctrl_set_viewtype(list, GUI_VIEWTYPE_ICON);
		listctrl_set_item_margin(list, 2, 0, 2, 0);
#if defined(FTR_IPHONE_STYLE)
		listctrl_set_rows(list, 0, 68, 0);
		listctrl_set_cols(list, 0, 60);
#elif (LCD_WIDTH == 240) && (LCD_HEIGHT == 320)
		listctrl_set_rows(list, 0, 36, 0);
		listctrl_set_cols(list, 0, 36);
#elif (LCD_WIDTH == 320) && (LCD_HEIGHT == 240)
		listctrl_set_rows(list, 0, 36, 0);
		listctrl_set_cols(list, 0, 36);
#elif (LCD_WIDTH == 220) && (LCD_HEIGHT == 176)
		listctrl_set_rows(list, 0, 30, 0);
		listctrl_set_cols(list, 0, 30);
#elif (LCD_WIDTH == 176) && (LCD_HEIGHT == 220)
		listctrl_set_rows(list, 0, 36, 0);
		listctrl_set_cols(list, 0, 36);
#elif (LCD_WIDTH == 128) && (LCD_HEIGHT == 160)
		listctrl_set_rows(list, 0, 40, 0);
		listctrl_set_cols(list, 0, 40);
#endif
		
#if defined(FTR_IPHONE_STYLE)
		listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE|CLFLAG_HILIGHT_SHOW_MASK|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE|CLFLAG_ENABLE_ITEM_FSHOW);
#else
		listctrl_set_flags(list, CLFLAG_SCROLLING_VERTICAL|CLFLAG_SCROLLING_PAGE|CLFLAG_HILIGHT_SHOW_MASK|CLFLAG_DISABLE_HLINE|CLFLAG_DISABLE_VLINE|CLFLAG_ENABLE_ITEM_FSHOW);
#endif

		wgt_redraw(list, FALSE);

		handle = wnd_get_skbar(wnd);
#if defined(FTR_NO_INPUT_KEY)
		sk_set_text(handle, SK_LSK, NULL, RESID_STR_LISTVIEW);
		sk_set_callback(handle, SK_LSK, CmdListView, data);
		wgt_redraw(handle, FALSE);
#else
		sk_set_popup(handle, SK_LSK, wnd_get_widget(wnd, RESID_MENU_CELLVIEW));
#endif
	}
}

#if !defined(FTR_NO_INPUT_KEY)

static t_MenuItem miCellViewOptions[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_LISTVIEW, CmdListView),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_SELECT_ALBUM, CmdChooseAlbum),
	MITEM_END
};

static t_MenuItem miListViewOptions[] = {
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_CELLVIEW, CmdCellView),
	MITEM_NORMAL(RESID_UNDEF, RESID_STR_SELECT_ALBUM, CmdChooseAlbum),
	MITEM_END
};
#endif

error_t	imageviewer_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_Global *glob = app_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
			wgt_enable_attr(handle, WND_ATTR_DEFAULT);
			{
				t_HWidget widget;
				gui_rect_t rect;
				
#if defined(FTR_SOFT_KEY)
				widget = wnd_get_skbar(handle);
				if(g_object_valid(widget)){
#if defined(FTR_NO_INPUT_KEY)
					sk_set_text(widget, SK_RSK, "", RESID_STR_SELECT_ALBUM);
					sk_set_callback(widget, SK_RSK, CmdChooseAlbum, handle);
#else
					t_HWidget menu = wgt_create(handle, "menu");
					if(g_object_valid(menu)){
						wgt_disable_attr(menu, CTRL_ATTR_VISIBLE);
						wgt_set_resid(menu, RESID_MENU_LISTVIEW);
						menu_bind(menu, miListViewOptions, handle);
					}
					menu = wgt_create(handle, "menu");
					if(g_object_valid(menu)){
						wgt_disable_attr(menu, CTRL_ATTR_VISIBLE);
						wgt_set_resid(menu, RESID_MENU_CELLVIEW);
						menu_bind(menu, miCellViewOptions, handle);
					}
					sk_set_text(widget, SK_LSK, "Options", RESID_STR_OPTION);
					sk_set_text(widget, SK_RSK, "Back", RESID_STR_BACK);
#endif
				}
#endif

				wgt_get_client_rect((t_HWidget)handle, &rect);
				widget = wnd_get_widget(handle, RESID_LIST_MAIN);
				wgt_set_bound(widget, 0, 0, rect.dx, rect.dy);
				listctrl_set_flags(widget, listctrl_get_flags(widget)|CLFLAG_ENABLE_ITEM_FSHOW);
				listctrl_attach(widget, (listctrl_redraw_func_t)__listctrl_redraw, glob);
				wgt_hook_register(widget, WIDGET_OnSelected, __image_list_on_hook, NULL);
				
				__image_list_refresh(glob, widget, FALSE);
			
				wgt_freeze(widget);
				CmdCellView(widget, handle);
				wgt_unfreeze(widget);
			

			}
			break;
		case WINDOW_OnClose:
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			ret = SUCCESS;
			break;
	}
	return ret;
}

