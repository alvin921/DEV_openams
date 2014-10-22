#include "pack_taskmng.hi"


#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "taskmng_brd_le.h"
#else
#include "taskmng_brd_be.h"
#endif
#endif

static t_AmsModule		pack_taskmng_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	}
	, MOD_INFO_END
};

static int	taskmng_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				t_TaskmngGlobData *glob;
				t_HWindow h;

				glob = MALLOC(sizeof(t_TaskmngGlobData));
				memset(glob, 0, sizeof(t_TaskmngGlobData));
				app_set_data(glob);

				h = wnd_create_by_resid(taskmng_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN);
				wnd_show(h);
				task_notify_register((gu32)h);

				ret = app_loop(NULL);
				
				FREEIF(glob);
				task_notify_register(0);
				ret = SUCCESS;
			}
			break;
	}
	return ret;
}

t_AmsPackage pack_info_taskmng = {
	PACK_GUID_taskmng
	, NULL	// OnLoad
	, NULL	// OnUnload
	, taskmng_AppEntry

	, pack_taskmng_modules
	
#if defined(TARGET_WIN)
	, taskmng_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_taskmng;
}

#endif

