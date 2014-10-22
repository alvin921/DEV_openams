#include "pack_sms.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "sms_brd_le.h"
#else
#include "sms_brd_be.h"
#endif
#endif


static int 		SmsViewMsgHandler(t_sparam sparam, t_lparam lparam)
{
	sms_glob_t *glob;
	int ret = NOT_HANDLED;
	
	t_MSArgContact *arg = (t_MSArgContact *)lparam;
	t_HDataSet ds;
	
	ds = sms_db_open();
	if(!g_object_valid(ds)){
		ret = msgbox_show(NULL, _RSK(BACK), 0, 0, "SMS initializing...");
	}else{
		glob = MALLOC(sizeof(sms_glob_t));
		memset(glob, 0, sizeof(sms_glob_t));
		glob->ds = ds;
		g_ds_OrderBy(ds, SMS_DBFIELD_TIME, FALSE, TRUE);
		app_set_data(glob);
		
#if defined(FTR_COMM_SESSION_SUPPORT)
		glob->hds_cs = comm_session_db_open();
		if(g_object_valid(glob->hds_cs)){
			gboolean main = FALSE;
			int count;
			g_ds_OrderBy(glob->hds_cs, CSESSION_DBFIELD_TIME, FALSE, FALSE);
			if(arg == NULL || arg->sid == CSESSION_NULL && arg->number[0] == 0){
				g_ds_CondSet1(glob->hds_cs, CSESSION_DBFIELD_TYPE, DBREL_EQ, COMM_TYPE_SMS);
				main = TRUE;
			}else if(arg->sid == CSESSION_NULL){
				g_ds_CondSet2(glob->hds_cs, CSESSION_DBFIELD_NUMBER, DBREL_IN, arg->number, BOOL_AND, CSESSION_DBFIELD_TYPE, DBREL_EQ, COMM_TYPE_SMS);
			}else if((arg->sid&0xff) == 0 && (arg->sid>>16)){
				//2 查询指定contact的所有记录
				t_DBCondition *cond1, *cond2, *cond;
				cond1 = dbcond_new(CSESSION_DBFIELD_SID, DBFIELD_TYPE_DWORD, DBREL_BT, arg->sid);
				cond2 = dbcond_new(CSESSION_DBFIELD_SID, DBFIELD_TYPE_DWORD, DBREL_LT, (arg->sid&0xffff0000)|0xff);
				cond = dbcond_compose(cond1, BOOL_AND, cond2);
				cond1 = dbcond_new(CSESSION_DBFIELD_TYPE, DBFIELD_TYPE_BYTE, DBREL_EQ, COMM_TYPE_SMS);
				cond = dbcond_compose(cond1, BOOL_AND, cond);
				g_ds_CondSet(glob->hds_cs, cond);
			}else{
				//2 查询指定session的所有记录
				g_ds_CondSet2(glob->hds_cs, CSESSION_DBFIELD_SID, DBREL_EQ, arg->sid, BOOL_AND, CSESSION_DBFIELD_TYPE, DBREL_EQ, COMM_TYPE_SMS);
			}
			g_ds_Query(glob->hds_cs, FALSE);
			
			count = g_ds_GetRecordNum(glob->hds_cs);
			if(main || count > 1){
				if(wnd_show(wnd_create_by_resid(sms_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN)))
					ret = app_loop(NULL);
			}else if(count == 1){
				g_ds_First(glob->hds_cs);
				if(wnd_show(wnd_create( sms_ViewSessionWndMsgHandler, g_ds_CurrentID(glob->hds_cs), glob->hds_cs)))
					ret = app_loop(NULL);
			}else{
				ret = msgbox_show(NULL, _RSK(BACK), 0, 0, "SMS not found!");
			}
		}
#else
		if(wnd_show(wnd_create(sms_ViewSessionWndMsgHandler, sparam, lparam)))
			ret = app_loop(NULL);
#endif
	}

	g_object_unref(glob->ds);
#if defined(FTR_COMM_SESSION_SUPPORT)
	g_object_unref(glob->hds_cs);
#endif
	FREEIF(glob);
	return ret;
}

static int 		SmsSendMsgHandler(t_sparam sparam, t_lparam lparam)
{
	sms_glob_t *glob;
	int ret = NOT_HANDLED;
	t_HDataSet ds;
	const char *number = (const char *)lparam;
	
	ret = msgbox_show(NULL, _RSK(BACK), 0, 0, "NOT implemented yet");
	return ret;
	ds = sms_db_open();
	if(!g_object_valid(ds)){
		ret = msgbox_show(NULL, _RSK(BACK), 0, 0, "SMS initializing...");
	}else{
		glob = MALLOC(sizeof(sms_glob_t));
		memset(glob, 0, sizeof(sms_glob_t));
		glob->ds = ds;
		g_ds_OrderBy(ds, SMS_DBFIELD_TIME, FALSE, TRUE);
		app_set_data(glob);
		//wnd_show(wnd_create(sms_ComposeWndMsgHandler, 1, *((t_ContactID*)lparam)));
	}
	g_object_unref(glob->ds);
	FREEIF(glob);
	return ret;
}

static t_AmsModule		pack_sms_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	{
		MOD_ID_SMS_OPEN_CONTACT, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_VIEW_SMS, 		// title resid
	},
	{
		MOD_ID_SMS_SEND,			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024,				// stack size
		RESID_STR_SEND_SMS,		// title resid
	},
	MOD_INFO_END
};

static int	sms_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
		case MOD_ID_SMS_OPEN_CONTACT:
			ret = SmsViewMsgHandler(sparam, lparam);
			break;
		case MOD_ID_SMS_SEND:
			ret = SmsSendMsgHandler(sparam, lparam);
			break;
	}
	return ret;
}

t_AmsPackage pack_info_sms = {
	PACK_GUID_sms
	, NULL	// OnLoad
	, NULL	// OnUnload
	, sms_AppEntry

	, pack_sms_modules
	
#if defined(TARGET_WIN)
	, sms_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_sms;
}

#endif

