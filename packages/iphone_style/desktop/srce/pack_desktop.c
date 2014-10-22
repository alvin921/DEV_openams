#include "pack_desktop.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "desktop_brd_le.h"
#else
#include "desktop_brd_be.h"
#endif
#endif

static t_AmsModule		pack_desktop_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 						// title resid
	},
	{
		MOD_ID_SCREENSAVER,			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		3*1024,				// stack size
		0,		// title resid
	},
	{
		MOD_ID_SHUTDOWN, 		// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		3*1024, 			// stack size
		0,		// title resid
	},
	MOD_INFO_END
};

error_t	screensaver_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam);
error_t	shutdown_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam);
static int	desktop_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			if(wnd_show(wnd_create_by_resid(desktop_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN)))
				ret = app_loop(NULL);
			break;
		case MOD_ID_SCREENSAVER:
			if(wnd_show(wnd_create(screensaver_MainWndMsgHandler, 0, 0)))
				ret = app_loop(NULL);
			break;
		case MOD_ID_SHUTDOWN:
			if(wnd_show(wnd_create(shutdown_MainWndMsgHandler, 0, 0)))
				ret = app_loop(NULL);
			break;
	}
	return ret;
}

t_AmsPackage pack_info_desktop = {
	PACK_GUID_desktop
	, NULL	// OnLoad
	, NULL	// OnUnload
	, desktop_AppEntry

	, pack_desktop_modules
	
#if defined(TARGET_WIN)
			, desktop_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_desktop;
}

#endif


