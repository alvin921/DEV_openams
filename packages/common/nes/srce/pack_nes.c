#include "pack_nes.hi"


#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "nes_brd_le.h"
#else
#include "nes_brd_be.h"
#endif
#endif

static t_AmsModule		pack_nes_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	}
	, MOD_INFO_END
};

static int	nes_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				t_Global *glob;

				if(lparam){
					if(!nesctrl_checkfile((const char *)lparam)){
						msgbox_show(NULL, _CSK(OK), 0, 0, brd_get_string(BRD_HANDLE_SELF, RESID_STR_FILE_FORMAT, "format error"));
						app_end(TID_SELF, 0);
						break;
					}
				}

				glob = MALLOC(sizeof(t_Global));
				memset(glob, 0, sizeof(t_Global));
				app_set_data(glob);

				if(wnd_show(wnd_create(nes_MainWndMsgHandler, sparam, lparam)))
					ret = app_loop(NULL);
				FREEIF(glob);
			}
			break;
	}
	return ret;
}

t_AmsPackage pack_info_nes = {
	PACK_GUID_nes
	, NULL
	, NULL
	, nes_AppEntry

	, pack_nes_modules
	
#if defined(TARGET_WIN)
	, nes_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_nes;
}

#endif


