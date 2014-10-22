#include "pack_mainmenu.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "mainmenu_brd_le.h"
#else
#include "mainmenu_brd_be.h"
#endif
#endif

static t_AmsModule		pack_mainmenu_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 						// title resid
	}
	, MOD_INFO_END
};

static int	mainmenu_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				t_HWindow h;

				h = wnd_create_by_resid(mainmenu_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN);

				if(wnd_show(h))
					ret = app_loop(NULL);
			}
			break;
	}
	return ret;
}

t_AmsPackage pack_info_mainmenu = {
	PACK_GUID_mainmenu
	, NULL	// OnLoad
	, NULL	// OnUnload
	, mainmenu_AppEntry

	, pack_mainmenu_modules
	
#if defined(TARGET_WIN)
			, mainmenu_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_mainmenu;
}

#endif


