#include "pack_email.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "email_brd_le.h"
#else
#include "email_brd_be.h"
#endif
#endif

static void	email_glob_destroy(email_glob_t *glob)
{
	if(glob){
		g_object_unref(glob->hds_head);
		g_object_unref(glob->hds_cont);
		g_object_unref(glob->hds_uidl);
		FREEIF(glob);
	}
}

static email_glob_t *	email_glob_new(t_sparam sparam, t_lparam lparam)
{
	email_glob_t *glob;
	
	glob = MALLOC(sizeof(email_glob_t));
	memset(glob, 0, sizeof(email_glob_t));

	db_email_init();
	
	glob->user = email_userinfo_load();
	glob->hds_head = g_ds_open(EMAIL_DB_FNAME, EMAIL_TAB_HEADER);
	g_ds_OrderBy(glob->hds_head, HEAD_DBF_TIME, FALSE, FALSE);
	glob->hds_cont = g_ds_open(EMAIL_DB_FNAME, EMAIL_TAB_CONTENT);
	glob->hds_uidl = g_ds_open(EMAIL_DB_FNAME, EMAIL_TAB_UIDL);
	return glob;
}


static t_AmsModule		pack_email_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	MOD_INFO_END
};

static int	email_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				email_glob_t *glob;
				t_HWindow h;

				glob = email_glob_new(sparam, lparam);
				app_set_data(glob);

				if(glob->user->email[0] == 0)
					h = wnd_create_by_resid(email_UserProfileWndMsgHandler, 0, 1, brd_get(PACK_GUID_SELF), RESID_WIN_USER_PROFILE);
				else
					h = wnd_create_by_resid(email_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN);
				if(wnd_show(h))
					ret = app_loop(NULL);
				email_glob_destroy(glob);
			}
			break;
	}
	return ret;
}

static error_t		pack_email_OnLoad(void)
{
	return SUCCESS;
}

static error_t		pack_email_OnUnload(void)
{
	return SUCCESS;
}

t_AmsPackage pack_info_email = {
	PACK_GUID_SELF
	, pack_email_OnLoad	// OnLoad
	, pack_email_OnUnload	// OnUnload
	, email_AppEntry

	, pack_email_modules
	
#if defined(TARGET_WIN)
	, email_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_email;
}

#endif

