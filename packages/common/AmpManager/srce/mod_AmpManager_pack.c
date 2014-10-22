#include "pack_AmpManager.hi"

static void	__callback_file(const char *fname)
{
	if(fname && strstr(fname, "prod_pack.bin")){
		amp_download(fname);
	}
}
typedef struct {
	char fname[AMOS_PATH_MAX+1];
	t_AmpInfo *info;
}t_PackWinData;

void	__notify_refresh(void)
{
	t_TaskID tid;
	t_HWindow win;
	tid = app_query(PACK_GUID_AmpManager, MOD_ID_MAIN);
	if(tid != TID_UNDEF){
		win = wnd_get(tid, RESID_WIN_MAIN);
		if(g_object_valid(win))
			msg_send_s(tid, win, MSG_AMP_REFRESH, 0, 0);
	}
}

static void	__PackInstall(t_HWidget sk, t_HWindow handle)
{
	t_PackWinData *data = (t_PackWinData*)wgt_get_tag(handle);
	int err;

	if(!prod_tflash_ready()){
		msgbox_show("AmpManager", _RSK(BACK), 0, 0, "T-flash not ready");
		return;
	}
	err = amp_install(data->fname);
	if(err != AMP_SUCCESS){
		msgbox_show("AmpManager", _RSK(BACK), 0, 0, "install failed, errno=%d", err);
	}else{
		gu32 ret;
		__notify_refresh();
		ret = msgbox_show("AmpManager", MBB_YES_NO, 0, 0, "Install OK, launch to run?");
		if(ret == MBB_YES){
			pack_call(data->info->guid, 0, NULL, 0, FALSE, NULL);
		}
		wnd_send_close(handle, 0);
	}
}

static void	__win_OnOpen(t_HWindow handle, const char *fname)
{
	t_PackWinData *data;
	int len;
	
	if(!fname || fsi_stat(fname, NULL)){
		msgbox_show("AmpManager", _RSK(BACK), 0, 0, "<%s> not exists", fname);
		wnd_send_close(handle, 0);
		return;
	}
	wgt_enable_attr(handle, WND_ATTR_DEFAULT);
	data = MALLOC(sizeof(t_PackWinData));
	wgt_set_tag(handle, data);
	len = strlen(fname);
	strcpy(data->fname, fname);
	data->info = amp_get_info(fname);
	
#if defined(FTR_SOFT_KEY)
	{
		t_SKHandle sk;
		sk = wnd_get_skbar(handle);
		if(g_object_valid(sk)){
			sk_set_text(sk, SK_LSK, "Install", RESID_STR_INSTALL);
			sk_set_callback(sk, SK_LSK, (t_WidgetCallback)__PackInstall, handle);
			sk_set_text(sk, SK_RSK, "Back", RESID_STR_BACK);
		}
	}
#else
#endif
}
static void __win_OnClose(t_HWindow handle)
{
	t_PackWinData *data = (t_PackWinData*)wgt_get_tag(handle);
	if(data){
		amp_free_info(data->info);
		FREE(data);
	}
}

error_t	AmpManager_PackWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	t_AmpManagerData *glob = app_get_data();
	
	switch(evt){
		case WINDOW_OnOpen:
			g_printf("WINDOW_OnOpen\r\n");
			__win_OnOpen(handle, (const char *)lparam);
			break;
		case WINDOW_OnClose:
			g_printf("WINDOW_OnClose\r\n");
			__win_OnClose(handle);
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			g_printf("WINDOW_OnDraw\r\n");
			{
				t_PackWinData *data = (t_PackWinData*)wgt_get_tag(handle);
				AmpManager_DisplayInfo(handle, data->info);
			}
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			break;
	}
	return ret;
}

