#include "pack_calllog.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "calllog_brd_le.h"
#else
#include "calllog_brd_be.h"
#endif
#endif

void	calllog_msi_Query(t_ContactMSInterface pme, char *number)
{
	if(g_object_valid(pme->hds)){

#if defined(FTR_COMM_SESSION_SUPPORT)
		g_ds_CondSet2(pme->hds, CALL_DBFIELD_NUMBER, DBREL_HAS, number, BOOL_AND, CSESSION_DBFIELD_TYPE, DBREL_EQ, COMM_TYPE_CALL_LOG);
#else
		g_ds_CondSet1(pme->hds, CALL_DBFIELD_NUMBER, DBREL_HAS, number);
#endif
		g_ds_Query(pme->hds, TRUE);

	}
}

const char * calllog_msi_GetItemNumber(t_ContactMSInterface pme, int index)
{
	if(g_ds_GotoByIndex(pme->hds, index) != SUCCESS)
		return NULL;
	return g_ds_GetString(pme->hds, CALL_DBFIELD_NUMBER);
}

static t_AmsModule		pack_calllog_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	{
		MOD_ID_CALLLOG_OPEN_CONTACT,			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024,				// stack size
		RESID_STR_TITLE,		// title resid
	},
	MOD_INFO_FUNC(MOD_ID_CALLLOG_MSI_GET),
	MOD_INFO_END
};

static int	calllog_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
		case MOD_ID_CALLLOG_OPEN_CONTACT:
			//2 lparam=<t_MSArgContact *>
			{
				t_HDataSet hdb;
				hdb = calllog_db_open();
				if(hdb == NULL){
					msgbox_show(NULL, _RSK(BACK), 0, 0, "CallLog initializing...");
				}else{
					g_object_unref(hdb);
#if defined(FTR_COMM_SESSION_SUPPORT)
					{
						t_MSArgContact *arg = (t_MSArgContact *)lparam;
						if(arg && (!IS_SAVED_CONTACT(arg->sid) || IS_SAVED_CONTACT(arg->sid) && (arg->sid&0xff))){
							//2 现有联系人的指定号码或未知联系人，则显示指定session
							if(wnd_show(wnd_create(calllog_ViewSessionWndMsgHandler, BY_SID, arg->sid)))
								ret = app_loop(NULL);
						}else if(arg == NULL || arg->sid == CSESSION_NULL && arg->number[0] == 0){
							if(wnd_show(wnd_create(calllog_MainWndMsgHandler, BY_NONE, 0)))
								ret = app_loop(NULL);
						}else if(arg->sid == CSESSION_NULL){
							if(wnd_show(wnd_create(calllog_MainWndMsgHandler, BY_NUMBER, (gu32)arg->number)))
								ret = app_loop(NULL);
						}else/* if((arg->sid&0xff) == 0 && (arg->sid>>16))*/{
							//2 查询指定contact的所有记录
							if(wnd_show(wnd_create(calllog_MainWndMsgHandler, BY_SID, arg->sid)))
								ret = app_loop(NULL);
						}
					}
#else
					if(wnd_show(wnd_create(calllog_MainWndMsgHandler, BY_NUMBER, lparam)))
						ret = app_loop(NULL);
#endif
				}
				ret = SUCCESS;
			}
			break;
		case MOD_ID_CALLLOG_MSI_GET:
			if(lparam){
				t_ContactMSInterface out = (t_ContactMSInterface)MALLOC(sizeof(struct stContactMSContext));
				if(out){
#if defined(FTR_COMM_SESSION_SUPPORT)
					out->hds = comm_session_db_open();
					g_ds_bind(out->hds, cs_redraw_record, cs_view_record);
#else
					out->hds = calllog_db_open();
					g_ds_bind(out->hds, db_calllog_redraw_record, db_calllog_view_record);
#endif

					if(g_object_valid(out->hds)){
						g_ds_OrderBy(out->hds, CSESSION_DBFIELD_TIME, FALSE, FALSE);
					}
					out->Query = calllog_msi_Query;
					out->GetItemNumber = calllog_msi_GetItemNumber;
					*((t_ContactMSInterface *)lparam) = (t_ContactMSInterface)out;
					ret = SUCCESS;
				}
			}
			break;
	}
	return ret;
}

static error_t	calllog_OnLoad(void)
{
	wgt_register_class(&class_calllog);
#if defined(FTR_COMM_SESSION_SUPPORT)
	wgt_register_class(&class_calllogview);
#endif
	return SUCCESS;
}

static error_t	calllog_OnUnload(void)
{
	wgt_unregister_class(&class_calllog);
#if defined(FTR_COMM_SESSION_SUPPORT)
	wgt_unregister_class(&class_calllogview);
#endif
	return SUCCESS;
}

t_AmsPackage pack_info_calllog = {
	PACK_GUID_calllog
	, calllog_OnLoad
	, calllog_OnUnload
	, calllog_AppEntry

	, pack_calllog_modules
	
#if defined(TARGET_WIN)
	, calllog_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_calllog;
}

#endif

