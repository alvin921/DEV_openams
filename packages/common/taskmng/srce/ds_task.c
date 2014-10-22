#include "pack_taskmng.hi"

typedef struct {
	t_DataSource	common;
	t_AppInfo	tasks[TASK_NB_MAX];
	int	num;
}t_DSTask;

static void	DS_refresh(t_HDataSource h)
{
	t_DSTask *pme = (t_DSTask *)h;
	t_TaskID cid = tid_current();
	t_TaskID tid_first = app_query(prod_get_first_app(), 0);
	int i;

	pme->num = 0;	
	for(i = 0; i < tid_first; i++){
		if(app_get_info(i, &pme->tasks[pme->num]))
			pme->num++;
	}
	for(i = tid_first+1; i < cid; i++){
		if(app_get_info(i, &pme->tasks[pme->num]))
			pme->num++;
	}
	for(i = cid+1; i < TASK_NB_MAX; i++){
		if(app_get_info(i, &pme->tasks[pme->num]))
			pme->num++;
	}
}

static error_t		DS_OnCreate(t_DSTask* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	pme->num = 0;
	DS_refresh((t_HDataSource)pme);
	return SUCCESS;
}

static void	DS_OnDestroy(t_DSTask* pme)
{
}

static int	DS_GetCount(t_HDataSource h)
{
	t_DSTask *pme = (t_DSTask*)h;

	return pme->num;
}

static void	DS_redraw(t_HDataSource h, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight)
{
	t_DSTask* pme = (t_DSTask*)h;
	t_AppInfo *info = &pme->tasks[index];
	const char *title;
	t_HImage icon;
	int icon_w, icon_h;
	coord_t x;
	char msg[16];
	gu32 size;
	
	icon = brd_get_image(info->brd, info->image);
	if(g_object_valid(icon)){
		icon_w = image_get_width(icon);
		icon_h = image_get_height(icon);
	}
	title = brd_get_string(info->brd, info->name, NULL);
	if(!title || *title == 0)
		title = info->guid;
	if(VIEWTYPE_IS_NORMAL(viewtype)){
		//2 ICON: application
		x = LINE_X_MARGIN;
		icon_w = icon_h = height-LINE_Y_MARGIN*2;
		gdi_draw_image_in(gdi, x, LINE_Y_MARGIN, icon_w, icon_h, icon, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
		//2 TITLE: application
		x += icon_w + LINE_X_MARGIN;
		gdi_draw_text_in(gdi, x, 0, width-x, height, title, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
		//2 RAM
#if (LCD_WIDTH >= 160) && defined(TARGET_WIN)
		size = task_ram_usage(info->tid);
		if(size >= (1<<20)){
			size >>= 10;
			sprintf(msg, "RAM:%dM", (size+1)>>10);
		}else if(size >= 1024){
			sprintf(msg, "RAM:%dK", (size+1)>>10);
		}else{
			sprintf(msg, "RAM:%dB", size);
		}
		gdi_draw_text_in(gdi, width-COL_SIZE_WIDTH-height, 0, COL_SIZE_WIDTH, height, msg, -1, CHARSET_UTF8, IDF_HALIGN_LEFT | IDF_VALIGN_MIDDLE);
#endif
#if defined(FTR_TOUCH_SCREEN)
		if(prod_ui_ts_support()){
			//2 ICON: delete
			g_object_unref(icon);
			icon = brd_get_image(BRD_HANDLE_SELF, RESID_ICON_DELETE);
			gdi_draw_image_in(gdi, width-height, 0, height, height, icon, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
		}
#endif		
	}else if(VIEWTYPE_IS_ICON(viewtype)){
		gs16 text_height = font_get(lang_get_current())->height;
		gs16 icon_height = height;
		icon_height -= text_height;
		if(g_object_valid(icon)){
			gdi_draw_image_in(gdi, 0, 0, width, icon_height, icon, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
		}
		gdi_draw_text_in(gdi, 0, icon_height, width, text_height, title, -1, CHARSET_UTF8, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE);
	}
	g_object_unref(icon);
}

static error_t DS_on_event(t_HDataSource h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	t_DSTask* pme = (t_DSTask *)h;
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnSelected:
			{
				DSTask_activate(h, sparam);
				ret = SUCCESS;
			}
			break;
			
		case WIDGET_OnKeyEvent:
#if !defined(FTR_NO_INPUT_KEY)
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
						int row, col;
						int index = listctrl_index_by_point(h->list, &x0, &y0, &row, &col);
					}
					break;
				case MMI_PEN_DOWN:
					if(0){
						int row, col;
						int index = listctrl_index_by_point(h->list, &x0, &y0, &row, &col);
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
	(PFNONCREATE)DS_OnCreate
	, (PFNONDESTROY)DS_OnDestroy

	, NULL
	, DS_redraw
	, DS_GetCount
	, NULL	// CanFocused
	, DS_on_event	// HookEvent
	, DS_refresh
};

static CLASS_DECLARE(dsource, t_DSTask, dsourceVtbl);

t_HDataSource	DSTask_new(void)
{
	return (t_HDataSource)g_object_new0(&class_dsource);
}

void	DSTask_activate(t_HDataSource h, int index)
{
	t_DSTask * pme = (t_DSTask*)h;
	if(index >= 0 && g_object_valid(pme) && index < pme->num){
		t_AppInfo *info = &pme->tasks[index];
		//if(msgbox_show("AmpManager", MBB_YES_NO, "sure to run <%s>?", brd_get_string(info->brd, info->name, "Unknown")) == MBB_YES){
			app_activate(info->tid);
			task_end(TID_SELF, 0);
		//}
	}
}

void	DSTask_kill(t_HDataSource h, int index)
{
	t_DSTask *pme = (t_DSTask *)h;
	t_HList list = h->list;

	if(index >= 0 && g_object_valid(pme) && index < pme->num){
		t_AppInfo *info = &pme->tasks[index];
		t_TaskID tid = app_query(prod_get_first_app(), 0);
		if(tid == info->tid){
			msgbox_show(NULL, _CSK(OK), 0, 0, "<%s> NEVER be killed", brd_get_string(info->brd, info->name, prod_get_first_app()));
			return;
		}
		if(app_get_attr(info->tid) & MOD_ATTR_APP_DAEMON){
			msgbox_show(NULL, _CSK(OK), 0, 0, "System daemon app CANNOT be killed!");
			return;
		}
		if(msgbox_show(NULL, MBB_YES_NO, 0, 0, "sure to kill <%s>?", brd_get_string(info->brd, info->name, "Unknown")) == MBB_YES){
			int i;
			task_kill(info->tid);
			
			for(i = index; i < pme->num-1; i++)
				pme->tasks[i] = pme->tasks[i+1];
			pme->num--;
			if(g_object_valid(list)){
				wgt_enable_attr(list, CTRL_ATTR_MODIFIED);
				wgt_redraw(list, FALSE);
			}
		}
	}
}

void	DSTask_add(t_HDataSource h, t_TaskID tid)
{
	t_DSTask *pme = (t_DSTask *)h;
	t_HList list = h->list;
	
	if(app_get_info(tid, &pme->tasks[pme->num])){
		pme->num++;
		if(g_object_valid(list)){
			wgt_enable_attr(list, CTRL_ATTR_MODIFIED);
			wgt_redraw(list, FALSE);
		}
	}
}

void	DSTask_remove(t_HDataSource h, t_TaskID tid)
{
	t_DSTask *pme = (t_DSTask *)h;
	t_HList list = h->list;
	int i;
	for(i = 0; i < pme->num; i++){
		if(pme->tasks[i].tid == tid){
			int j;
			for(j = i; j < pme->num-1; j++)
				pme->tasks[j] = pme->tasks[j+1];
			pme->num--;
			if(g_object_valid(list)){
				wgt_enable_attr(list, CTRL_ATTR_MODIFIED);
				wgt_redraw(list, FALSE);
			}
			break;
		}
	}
}

