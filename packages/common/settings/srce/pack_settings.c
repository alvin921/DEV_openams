#include "pack_settings.hi"



#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "settings_brd_le.h"
#else
#include "settings_brd_be.h"
#endif
#endif

static t_AmsModule		pack_settings_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	}
	, MOD_INFO_END
};

static int	settings_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				setting_glob_t *glob = MALLOC(sizeof(setting_glob_t));

				memset(glob, 0, sizeof(setting_glob_t));
				app_set_data(glob);
				
				if(wnd_show(wnd_create(settings_MainWndMsgHandler, 0, 0)))
					ret = app_loop(NULL);
				FREEIF(glob);
			}
			break;
	}
	return ret;
}

t_AmsPackage pack_info_settings = {
	PACK_GUID_settings
	, NULL
	, NULL
	, settings_AppEntry

	, pack_settings_modules
	
#if defined(TARGET_WIN)
	, settings_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_settings;
}

#endif


