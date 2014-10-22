/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  OEMToolbar.c

SERVICES:  OEM Toolbar Control Interface

GENERAL DESCRIPTION:  Provides support for IToolbar services
in OEM.

PUBLIC CLASSES AND STATIC FUNCTIONS: IToolbar

        Copyright ?2005 MAS Incorporated.
               All Rights Reserved.

=====================================================*/
	
/*=============================================================================
	
$DateTime: 2004/11/06 16:41:00 $
$Author: Wang MinGang $

					  EDIT HISTORY FOR FILE
	
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when		 who	 what, where, why
----------	 ---	 ----------------------------------------------------------
=============================================================================*/



/*=========================================================================
List控件特性描述:
1) list view: 主体是text
	1.1) 每行一个item(icon+text或者单text), 每行等高
	1.2) 可以添加属性如check/radio, 分别对应多选/单选
	1.3) hilight状态, item icon如果是动画, 则运行动画, 否则显示静态图片
	1.4) 暂时不支持: 允许设置hilight item占用两行, 这样text就有两个, detailTxt只在hilight显示

	高级特性:适用于数据比较多, 一次性加载浪费内存切加载慢, 比如数据库
	1.4) 允许用户设置数据源和显示方式, 提供最大限度灵活度, 将其实现为DBList
	
2) icon view: 主体是icon
	2.1) 每个item包括icon+text
	2.2) 每个item显示区域为正方形, cell方式显示
	2.3) 如果item icon为动画, 则hilight时显示动画

===========================================================================*/



/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "pack_explorer.hi"

#include "ds_explorer.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/


/*===========================================================================

Class Definitions

===========================================================================*/



typedef const struct {
	resid_t	resid;
	char 	ext[4];
}t_FileIconDef;

static t_FileIconDef  ga_file_icon_def[] = {
	{RESID_ICON_EXT_IMAGE		, "jpg"},
	{RESID_ICON_EXT_IMAGE		, "bmp"},
	{RESID_ICON_EXT_IMAGE		, "gif"},
	{RESID_ICON_EXT_IMAGE		, "png"},
	{RESID_ICON_EXT_VIDEO		, "avi"},
	{RESID_ICON_EXT_VIDEO		, "mp4"},
	{RESID_ICON_EXT_AUDIO		, "mp3"},
	{RESID_ICON_EXT_AUDIO		, "amr"},
	{RESID_ICON_EXT_AUDIO		, "acc"},
	{RESID_ICON_EXT_AUDIO		, "wav"},
	{RESID_ICON_EXT_TEXT		, "txt"},
	{RESID_UNDEF		, NULL},
};

t_HImage	sys_get_file_icon(gu8 mode, const char *fname)
{
	const char *ext = NULL;
	resid_t resid = RESID_UNDEF;
	if(mode & F_MODE_DIR)
		resid = RESID_ICON_FOLDER;
	else{
		t_FileIconDef *p = ga_file_icon_def;
		
		ext = strrchr(fname, '.');
		if(ext){
			ext++;
			if(p && ext && *ext){
				while(p->resid != RESID_UNDEF){
					if(g_stricmp(p->ext, ext) == 0){
						resid = p->resid;
						break;
					}
					p++;
				}
			}
		}
		if(resid == RESID_UNDEF)
			resid = RESID_ICON_FILE;
	}
	return brd_get_image(brd_get(PACK_GUID_SELF), resid);
}









#if !defined(FTR_NO_INPUT_KEY)
#define	FTR_EXPLORER_RSK_AS_UPLEVEL
#endif


#define	FILTER_NAME_MAX	(32-1-1)
typedef struct {
	t_DataSource		common;
	
	f_DentrySelect 	cb_select;
	f_DentryChange	cb_change;

	gu8				flag;
	char			filter[FILTER_NAME_MAX+1];

	char			cwd[AMOS_PATH_MAX+1];
	g_strlist_t		*fnames;
}t_DSExplorer;

static error_t		DE_OnCreate(t_DSExplorer* pme, gu32 d1, gu32 d2, gu32 d3, gu32 d4)
{
	strcpy(pme->filter, "*");
	strcpy(pme->cwd, fsi_getcwd());
	pme->fnames = g_strlist_new(4);
	pme->flag = DENTRY_FLAG_DEFAULT;

	return SUCCESS;
}

static void	DE_OnDestroy(t_DSExplorer* pme)
{
	g_strlist_destroy(pme->fnames);
}

#define	ICON_WIDTH		20

#define	COL_SIZE_WIDTH	40

static void	DE_Redraw(t_HDataSource h, t_HGDI gdi, t_GuiViewType viewtype, int index, int width, int height, gboolean hilight)
{
	t_DSExplorer* pme = (t_DSExplorer *)h;
	char *name = g_strlist_get(pme->fnames, index);
	if(name){
		dentry_stat_t stat;
		t_HImage icon = NULL;
		int icon_w, icon_h;
		char de_size[10]="";
		char *fullname;
		t_UITheme *theme = ui_theme_get();

		if(hilight)
			gdi_set_color(gdi, theme->hilight_fg_color);
		else
			gdi_set_color(gdi, theme->fg_color);
		fullname = MALLOC(strlen(pme->cwd)+1+strlen(name)+1);
		sprintf(fullname, "%s/%s", pme->cwd, name);

		fsi_stat(fullname, &stat);
		FREE(fullname);
		g_printf("fname=%s, mode=%x, size=%d", name, stat.mode, stat.size);

		
		icon = sys_get_file_icon(stat.mode, name);
		if(g_object_valid(icon)){
			icon_w = image_get_width(icon);
			icon_h = image_get_height(icon);
		}
		if(!(stat.mode & F_MODE_DIR)){
			if(stat.size >= BIT_20)
				sprintf(de_size, "%dM", stat.size/BIT_20);
			else if(stat.size >= BIT_10)
				sprintf(de_size, "%dK", stat.size/BIT_10);
			else
				sprintf(de_size, "%d", stat.size);
		}
		
		if(VIEWTYPE_IS_NORMAL(viewtype)){
			int x=1;
			width -= x;			
			if(g_object_valid(icon)){
				if(icon_h < height && icon_w <= ICON_WIDTH)
					gdi_draw_image_at(gdi, x, (height-icon_h)/2, icon, 0, 0, icon_w, icon_h, 1.0, 1.0);
				else
					gdi_draw_image_in(gdi, x, 1, ICON_WIDTH, height-2, icon, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE | IDF_HALIGN_STRETCH | IDF_VALIGN_STRETCH);
				x += ICON_WIDTH+2;
				width -= ICON_WIDTH+2;
			}
#if defined(FTR_EXPLORER_VIEW_2LINE)
			gdi_draw_text_in(gdi, x, 0, width, height/2, name, -1, CHARSET_UNDEF, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE);
			gdi_draw_text_in(gdi, x, height/2, width, height/2, de_size, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
#else
			gdi_draw_text_in(gdi, x, 0, width-COL_SIZE_WIDTH, height, name, -1, CHARSET_UNDEF, IDF_HALIGN_LEFT| IDF_VALIGN_MIDDLE);
			x += width-COL_SIZE_WIDTH;
			width = COL_SIZE_WIDTH;
			gdi_draw_text_in(gdi, x, 0, width, height, de_size, -1, CHARSET_UTF8, IDF_HALIGN_RIGHT | IDF_VALIGN_MIDDLE);
#endif
		}else{
			int text_height = font_get(lang_get_current())->height;
			if(g_object_valid(icon)){
				gdi_draw_image_in(gdi, 0, 0, width, height-text_height, icon, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE);
			}
			gdi_draw_text_in(gdi, 0, height-text_height, width, text_height, name, -1, CHARSET_UNDEF, IDF_HALIGN_CENTER | IDF_VALIGN_MIDDLE);
		}
		g_object_unref(icon);
	}
}

static void	DE_refresh(t_HDataSource handle);

static int	DE_GetCount(t_HDataSource h)
{
	t_DSExplorer* pme = (t_DSExplorer*)h;
	int count = g_strlist_size(pme->fnames);
	if(count <= 0){
		DE_refresh(h);
		count = g_strlist_size(pme->fnames);
	}
	return count;
}

static error_t DE_on_event(t_HDataSource h, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	t_DSExplorer* pme = (t_DSExplorer *)h;
	error_t ret = NOT_HANDLED;
	char *name, *fullname;
	int len;
	dentry_stat_t stat;
	switch(evt){
		case WIDGET_OnSelected:
			{
				name = g_strlist_get(pme->fnames, sparam);
			
				if(name){
					len = strlen(name);
					g_printf("<%s>(%d) selected!", name, len);
					fullname = MALLOC(strlen(pme->cwd)+1+strlen(name)+1);
					sprintf(fullname, "%s/%s", pme->cwd, name);
#if !defined(FTR_EXPLORER_RSK_AS_UPLEVEL)
					if(strcmp(name, "..") == 0){
						DSExplorer_SetPath(h, fullname);
						listctrl_refresh_datasource(h->list, TRUE);
					}else
#endif
					{
						if(fsi_stat(fullname, &stat) == SUCCESS){
							g_printf("mode=%x", stat.mode);
							if(stat.mode & F_MODE_DIR){
								DSExplorer_SetPath(h, fullname);
								listctrl_refresh_datasource(h->list, TRUE);
							}else if(pme->cb_select){
								pme->cb_select(h->list, pme->cwd, name);
							}else{
								char mime[MIME_NAME_MAX+1];
								char *p;
								
								strcpy(mime, "file/");
								p = strrchr(name, '.');
								if(p)
									strcat(mime, p+1);
								mime_service_exec(mime, METHOD_NAME_OPEN, PACK_GUID_SELF, fullname, strlen(fullname)+1, FALSE);
							}
						}else{
							msgbox_show("Error", _LSK(OK), 0, 0, "<%s> not found", name);
						}
					}
					FREE(fullname);
				}
				ret = SUCCESS;
			}
			break;
			
		case WIDGET_OnKeyEvent:
#if defined(FTR_EXPLORER_RSK_AS_UPLEVEL)
				if((lparam == MVK_BACK || lparam == MVK_RSK) && sparam == MMI_KEY_UP){
					if(pme->cwd[3] != '\0' && pme->cwd[2] != '\0' && (pme->flag & DENTRY_FLAG_FOLDER)){
						DSExplorer_SetPath(h, "..");
						listctrl_refresh_datasource(h->list, TRUE);
						return SUCCESS;
					}
				}else
#endif
				if(lparam == MVK_LSK && sparam == MMI_KEY_PRESS){
					name = g_strlist_get(pme->fnames, sparam);
					if(!(pme->flag & DENTRY_FLAG_FILE)){
						//2 目录选择
						if(pme->cb_select)
							pme->cb_select(h->list, pme->cwd, NULL);
						ret = SUCCESS;
					}
#if !defined(FTR_EXPLORER_RSK_AS_UPLEVEL)
					else if(strcmp(name, "..") == 0){
						DSExplorer_SetPath(h, "..");
						listctrl_refresh_datasource(h->list, TRUE);
					}
#endif
					else if(name){
						//2 文件选择
						len = strlen(name);
						fullname = MALLOC(strlen(pme->cwd)+1+strlen(name)+1);
						sprintf(fullname, "%s/%s", pme->cwd, name);
						if(fsi_stat(fullname, &stat) == SUCCESS){
							g_printf("mode=%x", stat.mode);
							if(stat.mode & F_MODE_DIR){
								DSExplorer_SetPath(h, fullname);
								listctrl_refresh_datasource(h->list, TRUE);
							}else if(pme->cb_select){
								pme->cb_select(h->list, pme->cwd, name);
							}else{
								char mime[MIME_NAME_MAX+1];
								char *p;
								
								strcpy(mime, "file/");
								p = strrchr(name, '.');
								if(p)
									strcat(mime, p+1);
								mime_service_exec(mime, METHOD_NAME_OPEN, PACK_GUID_SELF, fullname, strlen(fullname)+1, FALSE);
							}
						}else{
							msgbox_show("Error", _LSK(OK), 0, 0, "<%s> not found", name);
						}
						FREE(fullname);
					}
					ret = SUCCESS;

				}
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

static void	DE_refresh(t_HDataSource handle)
{
	int count = 0;
	if(g_object_valid(handle)){
		t_DSExplorer * pme = (t_DSExplorer *)handle;
		void *odh;
		char *cwd = pme->cwd;
		const char *name;
		
		g_printf("cwd=<%s>", cwd);
		g_strlist_reset(pme->fnames);
#if !defined(FTR_EXPLORER_RSK_AS_UPLEVEL)
		if(cwd[3] != '\0' && cwd[2] != '\0' && (pme->flag & DENTRY_FLAG_FOLDER))
			g_strlist_add(pme->fnames, "..", FALSE);
#endif
		odh = fsi_opendir(pme->cwd);
		
		g_printf("fsi_opendir(%s):%s\r\n", pme->cwd, odh?"OK":"ERR");
		if(odh){
			dentry_stat_t stat;
			char *fullname;
			while(name = fsi_readdir(odh)){
				if(strcmp(name, ".") == 0) continue;
				if(strcmp(name, "..") == 0) continue;
				//if(name[0] == '.') continue;
				
				fullname = MALLOC(strlen(pme->cwd)+1+strlen(name)+1);
				sprintf(fullname, "%s/%s", pme->cwd, name);
				fsi_stat(fullname, &stat);
				g_printf("mode=%x", stat.mode);

				if( (stat.mode & F_MODE_DIR) && (pme->flag & DENTRY_FLAG_FOLDER)){
					g_strlist_add(pme->fnames, name, FALSE);
				}else if(!(stat.mode & F_MODE_DIR) && (pme->flag & DENTRY_FLAG_FILE)){
					if(g_str_pattern(pme->filter, name)){
						//g_printf("dentry=<%s>[%02x, %02x]\r\n", name, name[0], name[1]);
						g_strlist_add(pme->fnames, name, FALSE);
					}
				}
				FREE(fullname);
			}
			fsi_closedir(odh);
		}
		count = g_strlist_size(pme->fnames);
	}
}


static VTBL(datasource) deVtbl = {
	(PFNONCREATE)DE_OnCreate
	, (PFNONDESTROY)DE_OnDestroy

	, NULL
	, DE_Redraw
	, DE_GetCount
	, NULL	// CanFocused
	, DE_on_event
	, DE_refresh
};

CLASS_DECLARE(DSExplorer, t_DSExplorer,  deVtbl);


t_HDataSource	DSExplorer_Create(void)
{
	return (t_HDataSource)g_object_new0(&class_DSExplorer);
}


void		DSExplorer_Bind(t_HDataSource ds, f_DentrySelect f_select, f_DentryChange f_change)
{
	if(g_object_valid(ds)){
		t_DSExplorer *pme = (t_DSExplorer*)(ds);
		pme->cb_select = f_select;
		pme->cb_change = f_change;
	}
}


int DSExplorer_SetPath(t_HDataSource handle, const char *path)
{
	t_DSExplorer *pme = (t_DSExplorer*)(handle);
	if(g_object_valid(handle) && path && strcmp(path, ".") && g_stricmp(pme->cwd, path)){
		char			cwd[AMOS_PATH_MAX+1];
		strcpy(cwd, pme->cwd);
		if(fsi_fullpath(path, cwd) && g_stricmp(cwd, pme->cwd)){
			dentry_stat_t st;
			if(fsi_stat(cwd, &st) == SUCCESS && (st.mode & F_MODE_DIR)){
				strcpy(pme->cwd, cwd);
				#if 0 //2 fsi_chdir会有副作用
				fsi_chdir(cwd);
				#endif
				g_strlist_reset(pme->fnames); // means no entries fentched
				if(g_object_valid(handle->list))
					wgt_enable_attr(handle->list, CTRL_ATTR_MODIFIED);
				if(pme->cb_change)
					pme->cb_change(handle->list, cwd);
				return SUCCESS;
			}
		}
	}
	return EBADPARM;
}

int	DSExplorer_SetFilter(t_HDataSource handle, const char *filter, gu32 flag)
{
	t_DSExplorer *pme = (t_DSExplorer*)(handle);
	if(flag != DENTRY_FLAG_UNCHANGED){
		if(flag == 0)
			flag = DENTRY_FLAG_DEFAULT;
		if(flag != pme->flag){
			g_strlist_reset(pme->fnames); // means no entries fentched
			if(g_object_valid(handle->list))
				wgt_enable_attr(handle->list, CTRL_ATTR_MODIFIED);
			pme->flag = flag;
		}
	}
	if(filter == NULL)
		filter = "*";
	if(strcmp(filter, pme->filter)){
		strcpy(pme->filter, filter);
		g_strlist_reset(pme->fnames); // means no entries fentched
		if(g_object_valid(handle->list))
			wgt_enable_attr(handle->list, CTRL_ATTR_MODIFIED);
	}
	return SUCCESS;
}
const char *DSExplorer_GetPath(t_HDataSource handle)
{
	return g_object_valid(handle)? ((t_DSExplorer*)handle)->cwd : NULL;
}
const char *DSExplorer_GetFname(t_HDataSource handle)
{
	t_DSExplorer *pme = (t_DSExplorer*)(handle);
	return pme? g_strlist_get(pme->fnames, listctrl_get_cursor(handle->list)) : NULL;
}

void	listctrl_attach_explorer(t_HWidget handle, const char *path, const char *filter, gu32 flag, f_DentrySelect select, f_DentryChange change)
{
	if(g_object_valid(handle)){
		t_HDataSource ds;
		ds = DSExplorer_Create();

		DSExplorer_Bind(ds, select, change);
		DSExplorer_SetPath(ds, path);
		DSExplorer_SetFilter(ds, filter, flag);

		listctrl_attach_datasource(handle, ds);
		g_object_unref((t_HObject)ds);
	}
}



