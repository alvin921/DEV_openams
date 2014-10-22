#include "pack_explorer.hi"
#include "ds_explorer.h"

typedef struct {
	t_FileDlgMode	mode;
	MC_FILL_STRUCT1;
	gs16		fnsize;
	char		*fname;
	t_HList	dentry;
}t_FileDlgData;

//2 选择目录或文件的操作方式
/*
1. 选择目录
		ok键(pen press)进入目录，lsk键确定选择
2. 选择文件
		ok键(pen press): 目录则进入，文件则选择，无lsk
*/

static void	CmdSelect(t_HWidget dentry, const char *path, const char *fname)
{
	t_HWindow wnd = wgt_parent_window(dentry);
	t_FileDlgData *dlg = (t_FileDlgData*)wgt_get_tag(wnd);
	dentry_stat_t st;

	if(dlg->mode == OFD_DIR_OPEN){
		strcpy(dlg->fname, path);
		wnd_send_close(wnd, TRUE);
	}else if(dlg->mode == OFD_FILE_OPEN){
		sprintf(dlg->fname, "%s/%s", path, fname);
		fsi_stat(dlg->fname, &st);
		if(st.mode&F_MODE_DIR){
		}else{
			wnd_send_close(wnd, TRUE);
		}
	}
}

static void SKCallback(t_HWidget sk, void *data)
{
	t_HWindow handle = wgt_parent_window(sk);
	int id = (int)data;
	if(g_object_valid(handle)){
		if(id == SK_LSK){
			//2 要么是dir open，要么是file save
			t_FileDlgData *dlg = (t_FileDlgData*)wgt_get_tag(handle);
			t_HDataSource ds = listctrl_get_datasource(wgt_get_child(dlg->dentry, RESID_DENTRY_LIST, 0));
			const char *path, *fname;
			
			path = DSExplorer_GetPath(ds);
			fname = DSExplorer_GetFname(ds);
			if(dlg->mode == OFD_DIR_OPEN){
				CmdSelect(dlg->dentry, path, NULL);
			}else if(dlg->mode == OFD_FILE_SAVE){
			}
		}else if(id == SK_RSK){
			wnd_send_close(handle, FALSE);
		}
	}
}

static void	__win_OnOpen(t_HWindow handle, t_FileDlgArg *param)
{
	t_FileDlgData *data;
	t_HList dentry;
	gu32 flag = 0;

	wgt_enable_attr(handle, WND_ATTR_DEFAULT);
	wgt_set_bg_fill_solid(handle, RGB_WHITE);
	if(param->mode == OFD_FILE_OPEN){
		wgt_set_caption_by_resid((t_HWidget)handle, BRD_HANDLE_SELF, RESID_STR_FILE_OPEN);
		flag = DENTRY_FLAG_FILE|DENTRY_FLAG_FOLDER;
	}else if(param->mode == OFD_FILE_SAVE){
		wgt_set_caption_by_resid((t_HWidget)handle, BRD_HANDLE_SELF, RESID_STR_FILE_SAVE);
		flag = DENTRY_FLAG_FILE|DENTRY_FLAG_FOLDER;
	}else if(param->mode == OFD_DIR_OPEN){
		wgt_set_caption_by_resid((t_HWidget)handle, BRD_HANDLE_SELF, RESID_STR_DIR_OPEN);
		flag = DENTRY_FLAG_FOLDER;
	}

	dentry = (t_HList)wgt_create((t_HWidget)handle, "explorer");
	if(g_object_valid(dentry)){
		gui_rect_t client;

		wgt_get_client_rect((t_HWidget)handle, &client);
		wgt_set_bound((t_HWidget)dentry, 0, 0, client.dx, client.dy);
		wgt_set_border(dentry, BORDER_TYPE_FRAME, 1, 0);

		wgt_freeze(dentry);
		wgt_explorer_set(dentry, param->path, param->filter, flag, CmdSelect);
		wgt_unfreeze(dentry);
	}

	data = MALLOC(sizeof(t_FileDlgData));
	data->dentry = dentry;
	data->fname = param->fname;
	data->fnsize = param->fnsize;
	data->mode = param->mode;
	wgt_set_tag(handle, data);
#if defined(FTR_SOFT_KEY)
		{
			t_SKHandle sk;
			sk = wnd_get_skbar(handle);
#if defined(FTR_NO_INPUT_KEY)
			//2 LSK: OK/YES
			if(param->mode != OFD_FILE_OPEN)
#endif
			{
				sk_set_text(sk, SK_LSK, "OK", RESID_STR_OK);
				sk_set_callback(sk, SK_LSK, SKCallback, (void*)SK_LSK);
			}
			//2 RSK: NO/BACK/CANCEL
			sk_set_text(sk, SK_RSK, "CANCEL", RESID_STR_CANCEL);
			sk_set_callback(sk, SK_RSK, SKCallback, (void*)SK_RSK);
		}
#else
	
#endif
}

error_t 	FileDlgMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t handled = NOT_HANDLED;
	t_FileDlgData *dlg = (t_FileDlgData*)wgt_get_tag(handle);
	
	switch(evt){
	case WINDOW_OnOpen:
		if(lparam){
			__win_OnOpen(handle, (t_FileDlgArg*)lparam);
		}
		break;
	case WINDOW_OnClose:
		FREEIF(dlg);
		break;
	case WINDOW_OnDraw:
		break;
#if 0
	case WIDGET_OnKeyEvent:
		switch(sparam){
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

