#include "pack_simlock.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "simlock_brd_le.h"
#else
#include "simlock_brd_be.h"
#endif
#endif


static t_AmsModule		pack_simlock_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	}
	, MOD_INFO_END
};

static int	simlock_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				t_HWindow h;
				gu8 sim = *((gu8*)lparam);
				t_simlockData *glob;
				
				glob = MALLOC(sizeof(t_simlockData));
				memset(glob, 0, sizeof(t_simlockData));
				app_set_data(glob);

				ret = msgbox_show(NULL, _LSK(OK), INFINITE, 0, "Not implemented");
				FREEIF(glob);
			}
			break;
	}
	return ret;
}

static error_t		pack_simlock_OnLoad(void)
{
	return SUCCESS;
}

static error_t		pack_simlock_OnUnload(void)
{
	return SUCCESS;
}

t_AmsPackage pack_info_simlock = {
	PACK_GUID_SELF
	, pack_simlock_OnLoad	// OnLoad
	, pack_simlock_OnUnload	// OnUnload
	, simlock_AppEntry

	, pack_simlock_modules
	
#if defined(TARGET_WIN)
	, simlock_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_simlock;
}

#endif

