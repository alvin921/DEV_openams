#include "pack_AmpManager.hi"

static void	DS_refresh(t_HDataSource h)
{
	t_DSInstall *pme = (t_DSInstall *)h;
	int i;

	if(prod_tflash_ready())
		amp_scan();
	pme->num = amp_get_app_num();
}

static error_t		DS_OnCreate(t_DSInstall* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	pme->num = 0;
	DS_refresh((t_HDataSource)pme);
	return SUCCESS;
}

static void	DS_OnDestroy(t_DSInstall* pme)
{
}

static int	DS_GetCount(t_HDataSource h)
{
	t_DSInstall *pme = (t_DSInstall*)h;

	return pme->num;
}

#define	COL_SIZE_WIDTH	40
static void	DS_redraw(t_HDataSource h, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight)
{
	t_DSInstall* pme = (t_DSInstall*)h;
	const char *guid = amp_get_app_guid(index);
	char title[32];
	t_HImage icon;
	int icon_w, icon_h;

	pack_GetModuleTitle(guid, 0, title, sizeof(title));
	icon = pack_GetIcon(guid);
	if(g_object_valid(icon)){
		icon_w = image_get_width(icon);
		icon_h = image_get_height(icon);
	}
	
	if(VIEWTYPE_IS_NORMAL(viewtype)){
		int x=1;
		width -= x; 		
	
		gdi_draw_image_in(gdi, x, 1, height-2, height-2, icon, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
		x += height-2;
		width -= height-2;
		
		gdi_draw_text_in(gdi, x, 0, width, height, title, -1, CHARSET_UTF8, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE);
	}else if(VIEWTYPE_IS_ICON(viewtype)){
		gs16 text_height = font_get(lang_get_current())->height;
		gs16 icon_height = height;
		icon_height -= text_height+2;
		if(g_object_valid(icon)){
			gdi_draw_image_in(gdi, 0, 0, width, icon_height, icon, IDF_HALIGN_BESTFIT | IDF_VALIGN_BESTFIT);
		}
		gdi_draw_text_in(gdi, 0, icon_height, width, text_height, title, -1, CHARSET_UTF8, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE);
	}
}

static error_t DS_on_event(t_HDataSource h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	t_DSInstall* pme = (t_DSInstall *)h;
	error_t ret = NOT_HANDLED;
	switch(evt){
		case WIDGET_OnSelected:
			{
				const char *guid = amp_get_app_guid(sparam);
				if(guid){
					pack_call(guid, MOD_ID_MAIN, NULL, 0, FALSE, NULL);
				}
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
	, DS_on_event
	, DS_refresh
};

static CLASS_DECLARE(dsource, t_DSInstall, dsourceVtbl);

t_HDataSource	DSInstall_new(void)
{
	return (t_HDataSource)g_object_new0(&class_dsource);
}

void	DSInstall_run(t_HDataSource h, int index)
{
	t_DSInstall * ds = (t_DSInstall*)h;
	if(index >= 0 && g_object_valid(ds) && index < ds->num){
		const char *guid = amp_get_app_guid(index);
		if(guid){
			if(msgbox_show("AmpManager", MBB_YES_NO, 0, 0, "sure to run <%s>?", guid) == MBB_YES){
				pack_call(guid, MOD_ID_MAIN, NULL, 0, FALSE, NULL);
			}
		}
	}
}
void	DSInstall_uninstall(t_HDataSource h, int index)
{
	t_DSInstall *pme = (t_DSInstall *)h;
	t_HList list = h->list;

	if(index >= 0 && g_object_valid(pme) && index < pme->num){
		const char *guid = amp_get_app_guid(index);
		if(guid && msgbox_show(NULL, MBB_YES_NO, 0, 0, "sure to uninstall %s?", guid) == MBB_YES){
			int i;
			amp_uninstall(guid);
			pme->num--;
			if(g_object_valid(list)){
				wgt_enable_attr(list, CTRL_ATTR_MODIFIED);
				wgt_redraw(list, FALSE);
			}
		}
	}
}

