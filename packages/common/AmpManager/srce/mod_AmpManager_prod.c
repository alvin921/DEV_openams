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
}t_ProdWinData;

static void __ProdBurn(t_HWidget sk, void *arg)
{
	t_HWindow handle = arg;
	t_ProdWinData *data = (t_ProdWinData*)wgt_get_tag(handle);
	if(data){
		if(msgbox_show("AmpManager", MBB_YES_NO, 0, 0, "sure to burn it to MS?") == MBB_YES){
			if(amp_download(data->fname)){
				app_end(TID_SELF, 0);
			}
		}
	}
}

static void	__win_OnOpen(t_HWindow handle, const char *fname)
{
	t_ProdWinData *data;
	gboolean opened = FALSE;

	if(!fname || fsi_stat(fname, NULL)){
		msgbox_show("AmpManager", _RSK(BACK), 0, 0, "<%s> not exists", fname);
		wnd_send_close(handle, 0);
		return;
	}

	wgt_enable_attr(handle, WND_ATTR_DEFAULT);
	data = MALLOC(sizeof(t_ProdWinData));
	wgt_set_tag(handle, data);
	strcpy(data->fname, fname);
	data->info = amp_get_info(fname);
	g_printf("prod info ok");
#if defined(FTR_SOFT_KEY)
	{
		t_SKHandle sk;
		sk = wnd_get_skbar(handle);
		if(g_object_valid(sk)){
			sk_set_text(sk, SK_LSK, "BurnIt", RESID_STR_BURN);
			sk_set_callback(sk, SK_LSK, __ProdBurn, handle);
			sk_set_text(sk, SK_RSK, "Back", RESID_STR_BACK);
		}
	}
#else
#endif
}
static void __win_OnClose(t_HWindow handle)
{
	t_ProdWinData *data = (t_ProdWinData*)wgt_get_tag(handle);
	if(data){
		amp_free_info(data->info);
		FREE(data);
	}
}

error_t	AmpManager_ProdWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
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
				t_ProdWinData *data = (t_ProdWinData*)wgt_get_tag(handle);
				AmpManager_DisplayInfo(handle, data->info);
			}
			ret = SUCCESS;
			break;
	}
	return ret;
}

