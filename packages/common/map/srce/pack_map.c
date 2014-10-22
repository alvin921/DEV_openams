#include "pack_map.hi"



#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "map_brd_le.h"
#else
#include "map_brd_be.h"
#endif
#endif

static t_AmsModule		pack_map_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	{
		MOD_ID_ADDRESS_BROWSE, 			// id
		0,						// prior
		0,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	MOD_INFO_END
};

static int	map_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
		case MOD_ID_ADDRESS_BROWSE:
			{
				t_Global *glob = MALLOC(sizeof(t_Global));
				const char *addr = (const char *)lparam;

				memset(glob, 0, sizeof(t_Global));
				app_set_data(glob);

				if(addr == NULL || *addr == 0)
					ret = msgbox_show("Map", _RSK(BACK), 0, 0, "Service not implemented yet!");
				else
					ret = msgbox_show(addr, _RSK(BACK), 0, 0, "Service not implemented yet!");
				ret = SUCCESS;
				FREEIF(glob);
			}
			break;
	}
	return ret;
}

t_AmsPackage pack_info_map = {
	PACK_GUID_map
	, NULL
	, NULL
	, map_AppEntry

	, pack_map_modules
	
#if defined(TARGET_WIN)
			, map_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_map;
}

#endif


