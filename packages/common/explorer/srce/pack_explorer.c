
#include "pack_explorer.hi"

extern error_t	explorer_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam);



#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "explorer_brd_le.h"
#else
#include "explorer_brd_be.h"
#endif
#endif

error_t 	FileDlgMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam);

static t_AmsModule		pack_explorer_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	MOD_INFO_FUNC(MOD_ID_FUNC),
	MOD_INFO_END
};

static int	explorer_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				t_Global *glob = MALLOC(sizeof(t_Global));
				t_HWindow h = wnd_create_by_resid(explorer_MainWndMsgHandler, sparam, lparam, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN);

				memset(glob, 0, sizeof(t_Global));
				app_set_data(glob);

				if(wnd_show(h))
					ret = app_loop(NULL);
				
				FREEIF(glob);
			}
			break;
		case MOD_ID_FUNC:
			{
				t_FileDlgArg *arg = (t_FileDlgArg*)lparam;
				t_HWindow wnd;

				wnd = wnd_create(FileDlgMsgHandler, sparam, lparam);
				arg->result = wnd_modal(wnd);
				return SUCCESS;
			}
			break;
	}
	return ret;
}

extern t_GClass class_explorer;

static error_t		pack_explorer_OnLoad(void)
{
	wgt_register_class(&class_explorer);
	return SUCCESS;
}

static error_t		pack_explorer_OnUnload(void)
{
	wgt_unregister_class(&class_explorer);
	return SUCCESS;
}

t_AmsPackage pack_info_explorer = {
	PACK_GUID_explorer
	, pack_explorer_OnLoad
	, pack_explorer_OnUnload
	, explorer_AppEntry

	, pack_explorer_modules
	
#if defined(TARGET_WIN)
	, explorer_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_explorer;
}

#endif


