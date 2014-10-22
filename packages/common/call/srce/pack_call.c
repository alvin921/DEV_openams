

#include "pack_call.hi"


#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "call_brd_le.h"
#else
#include "call_brd_be.h"
#endif
#endif


call_glob_t *call_glob_new(t_AmsModuleId mod)
{
	call_glob_t *glob = MALLOC(sizeof(call_glob_t));
	t_HList list = NULL;
	int n;

	memset(glob, 0, sizeof(call_glob_t));
	//glob->mod = mod;

	glob->n_ms_phone = mime_service_query(MIME_TYPE_PHONE, NULL, NULL, glob->ms_phone, ARRAY_SIZE(glob->ms_phone), 0, MS_QUERY_F_APP);

	return glob;
}


void	call_glob_destroy(call_glob_t *glob)
{
	if(glob){
		FREE(glob);
	}
}

static t_AmsModule		pack_call_modules[] = {
	{
		MOD_CALL_DIALER, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	{
		MOD_CALL_INCOMING, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	{
		MOD_CALL_SEND,			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024,				// stack size
		RESID_STR_TITLE,		// title resid
	},
	MOD_INFO_END
};

static int	_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	call_glob_t *glob;
	switch(id){
		case MOD_CALL_DIALER:
			{
				t_HWindow h;

				glob = call_glob_new(MOD_CALL_DIALER);
				app_set_data(glob);

				h = wnd_create_by_resid(call_PanelWndMsgHandler, sparam, lparam, brd_get(PACK_GUID_SELF), RESID_WIN_PANEL);
				if(wnd_show(h))
					ret = app_loop(NULL);
				call_glob_destroy(glob);
			}
			break;
		case MOD_CALL_INCOMING:
			{
				int cid = *((int *)lparam);

				glob = call_glob_new(MOD_CALL_INCOMING);
				app_set_data(glob);

				if(call_LoadIncoming(cid))
					ret = app_loop(NULL);
				call_glob_destroy(glob);
			}
			break;
		case MOD_CALL_SEND:
			{
				t_MSArgContact *param = (t_MSArgContact *)lparam;
				t_ContactInfo *cinfo = contact_get_info(param->sid);
				
				glob = call_glob_new(MOD_CALL_SEND);
				app_set_data(glob);

				if(cinfo == NULL && param->number[0] == 0){
					ret = msgbox_show("Call", _RSK(BACK), 0, 0, "No cell number provided!");
				}else{
					if(cinfo)
						strcpy(glob->strPhone, cinfo->number);
					else
						strcpy(glob->strPhone, param->number);
					contact_free_info(cinfo);
					if(call_LoadOutgoing(0xFF, glob->strPhone, param->sid))
						ret = app_loop(NULL);
				}
				call_glob_destroy(glob);
				ret = SUCCESS;
			}
			break;
	}
	return ret;
}

t_AmsPackage pack_info_call = {
	PACK_GUID_call
	, NULL
	, NULL
	, _AppEntry

	, pack_call_modules
	
#if defined(TARGET_WIN)
	, call_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_call;
}

#endif


