#include "pack_pushbox.h"
#include "pushbox_resid.h"

typedef struct {
	gu32		v;
}t_pushboxGlobal;


extern error_t	pushbox_MainWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam);



#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "pushbox_brd_le.h"
#else
#include "pushbox_brd_be.h"
#endif
#endif

static t_AmsModule		pack_pushbox_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	}
	, MOD_INFO_END
};

static int	pushbox_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				t_pushboxGlobal *glob = MALLOC(sizeof(t_pushboxGlobal));
				t_HWindow h;

				memset(glob, 0, sizeof(t_pushboxGlobal));
				app_set_data(glob);
				
				h = wnd_create_by_resid(pushbox_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_pushbox), RESID_WIN_MAIN);
				if(wnd_show(h))
					ret = app_loop(NULL);
				FREEIF(glob);
			}
			break;
	}
	return ret;
}

t_AmsPackage pack_info_pushbox = {
	PACK_GUID_pushbox
	, NULL
	, NULL
	, pushbox_AppEntry

	, pack_pushbox_modules
	
#if defined(TARGET_WIN)
			, pushbox_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_pushbox;
}

#endif


