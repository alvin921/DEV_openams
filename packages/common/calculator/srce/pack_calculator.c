#include "pack_calculator.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "calculator_brd_le.h"
#else
#include "calculator_brd_be.h"
#endif
#endif


static int	calculator_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				t_Global *glob = MALLOC(sizeof(t_Global));
				t_HWindow h;

				memset(glob, 0, sizeof(t_Global));
				app_set_data(glob);

				h = wnd_create_by_resid(calculator_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN);
				if(wnd_show(h))
					ret = app_loop(NULL);
				FREEIF(glob);
			}
			break;
	}
	return ret;
}


static t_AmsModule		pack_calculator_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	}
	, MOD_INFO_END
};

t_AmsPackage pack_info_calculator = {
	PACK_GUID_calculator
	, NULL	// OnLoad
	, NULL	// OnUnload
	, calculator_AppEntry

	, pack_calculator_modules
	
#if defined(TARGET_WIN)
	, calculator_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_calculator;
}

#endif

