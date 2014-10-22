#include "pack_contact.hi"


const resid_t	ga_phone_type_resid[CONTACT_F_NUMBER_N] = {
	RESID_STR_NUMBER_MOBILE,
	RESID_STR_NUMBER_OFFICE,
	RESID_STR_NUMBER_HOME,
	RESID_STR_NUMBER_FAX,
	RESID_STR_NUMBER_OTHER,
};

const char ga_phone_type[CONTACT_F_NUMBER_N] = {
	CONTACT_F_NUMBER_MOBILE,
	CONTACT_F_NUMBER_OFFICE,
	CONTACT_F_NUMBER_HOME,
	CONTACT_F_NUMBER_FAX,
	CONTACT_F_NUMBER_OTHER,
};

int	contact_get_fnum_by_index(contact_number_t *phone, int index)
{
	int i;
	for(i = 0; i < CONTACT_F_NUMBER_N; i++){
		if(phone->number[i][0] != 0){
			if(index == 0)
				return i;
			index--;
		}
	}
	return -1;
}


#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "contact_brd_le.h"
#else
#include "contact_brd_be.h"
#endif
#endif

static error_t 		GetNameByNumberHandler(t_sparam sparam, t_lparam lparam)
{
	t_MSArgContactName *arg = (t_MSArgContactName *)lparam;
	t_HDataSet hdb;

	hdb = g_ds_open(CONTACT_DB_FNAME, CONTACT_DBTAB_LIST);
	if(!g_object_valid(hdb)){
		return EFS_OPENHANDLE;
	}
	g_ds_CondSet1(hdb, CONTACT_DBFIELD_NUMBER, DBREL_HAS, arg->number);
	g_ds_Query(hdb, FALSE);
	if(g_ds_Empty(hdb)){
		if(arg->name)
			arg->name[0] = 0;
		arg->size = 0;
		arg->cid = UNKNOWN_CONTACT;
	}else{
		char *name;
		int len = 0;
		int i;
		contact_number_t phone;

		for(g_ds_First(hdb); !g_ds_Eof(hdb);g_ds_Next(hdb)){
			if(contact_number_parse(g_ds_GetString(hdb, CONTACT_DBFIELD_NUMBER), &phone)){
				for(i = 0; i < CONTACT_F_NUMBER_N; i++){
					if(strcmp(phone.number[i], arg->number) == 0){
						if(arg->name){
							name = g_ds_GetString(hdb, CONTACT_DBFIELD_NAME);
							if(name)
								len = strlen(name);
							if(len >= arg->size)
								len = arg->size-1;
							if(len > 0)
								strncpy(arg->name, name, len);
							else
								len = 0;
							arg->name[len] = 0;
						}
						arg->size = len;
						arg->cid = g_ds_CurrentID(hdb);
						arg->cid = CSESSION_ID(arg->cid, ga_phone_type[i]);
						goto out;
					}
				}
			}
		}
	}
out:
	g_object_unref(hdb);
	return SUCCESS;
}

static error_t 		GetInfoByIDHandler(t_sparam sparam, t_lparam lparam)
{
	error_t	ret;
	t_HDataSet hdb;
	t_MSArgContactInfo *arg = (t_MSArgContactInfo *)lparam;

	hdb = g_ds_open(CONTACT_DB_FNAME, CONTACT_DBTAB_LIST);
	if(!g_object_valid(hdb)){
		return EFS_OPENHANDLE;
	}

	if(g_ds_Goto(hdb, (arg->cid)>>16) == SUCCESS){
		char *str;
		int len = 0;
		arg->cinfo = MALLOC(sizeof(t_ContactInfo));
		memset(arg->cinfo, 0, sizeof(t_ContactInfo));
		arg->cinfo->cid = arg->cid;
		arg->cinfo->name = g_str_dup(g_ds_GetString(hdb, CONTACT_DBFIELD_NAME));
		arg->cinfo->number = g_str_dup(g_ds_GetString(hdb, CONTACT_DBFIELD_NUMBER));
		arg->cinfo->email = g_str_dup(g_ds_GetString(hdb, CONTACT_DBFIELD_EMAIL));
		arg->cinfo->address = g_str_dup(g_ds_GetString(hdb, CONTACT_DBFIELD_ADDRESS));
		str = g_ds_GetString(hdb, CONTACT_DBFIELD_PICTURE);
		arg->cinfo->pic_resid = g_str2int(str);
		if(arg->cinfo->pic_resid == 0)
			arg->cinfo->pic_fname = g_str_dup(str);
		str = g_ds_GetString(hdb, CONTACT_DBFIELD_RING);
		arg->cinfo->ring_resid = g_str2int(str);
		if(arg->cinfo->ring_resid == 0)
			arg->cinfo->ring_fname = g_str_dup(str);
		ret = SUCCESS;
	}else{
		ret = EDB_RECORD_NOT_EXIST;
	}
	
	g_object_unref(hdb);
	return ret;
}

void	contact_msi_Query(t_ContactMSInterface pme, char *number)
{
	if(g_object_valid(pme->hds)){
		g_ds_CondSet1(pme->hds, CONTACT_DBFIELD_NUMBER, DBREL_HAS, number);
		g_ds_Query(pme->hds, TRUE);
	}
}
const char * contact_msi_GetItemNumber(t_ContactMSInterface pme, int index)
{
	if(g_ds_GotoByIndex(pme->hds, index) != SUCCESS)
		return NULL;
	return g_ds_GetString(pme->hds, CONTACT_DBFIELD_NUMBER);
}

static error_t 		MSIFuncHandler(t_sparam sparam, t_lparam lparam)
{
	if(lparam){
		t_ContactMSInterface out = (t_ContactMSInterface)MALLOC(sizeof(struct stContactMSContext));
		if(out){
			out->hds = g_ds_open(CONTACT_DB_FNAME, CONTACT_DBTAB_LIST);
			g_ds_bind(out->hds, db_contact_redraw_record, db_contact_view_record);
			out->Query = contact_msi_Query;
			out->GetItemNumber = contact_msi_GetItemNumber;
			*((t_ContactMSInterface *)lparam) = (t_ContactMSInterface)out;
		}
	}
	return SUCCESS;
}

static t_AmsModule		pack_contact_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	{
		MOD_ID_CONTACT_OPEN,			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024,				// stack size
		RESID_STR_OPEN_CONTACT, 		// title resid
	},

	MOD_INFO_FUNC(MOD_ID_PHONENO_GETNAME),
	MOD_INFO_FUNC(MOD_ID_CONTACT_GETINFO),
	MOD_INFO_FUNC(MOD_ID_CONTACT_QUERY_INTERFACE),
	MOD_INFO_END
};

extern t_GClass class_contact;
static error_t	contact_OnLoad(void)
{
	wgt_register_class(&class_contact);
	db_contact_init();
	return SUCCESS;
}

static error_t	contact_OnUnload(void)
{
	wgt_unregister_class(&class_contact);
	return SUCCESS;
}

static int	contact_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
		case MOD_ID_CONTACT_OPEN:
			{
				t_MSArgContact *arg = (t_MSArgContact *)lparam;
				if(arg == NULL || arg->sid == CSESSION_NULL && arg->number[0] == 0){
					//2 未指定sid和number，则表示是默认入口，主界面显示所有联系人列表
					if(wnd_show(wnd_create(contact_MainWndMsgHandler, BY_NONE, 0)))
						ret = app_loop(NULL);
				}else if(arg->sid == CSESSION_NULL){
					//2 根据number查询联系人，如果只有一个，则直接打开详情，否则主界面显示所有满足条件联系人列表
					if(wnd_show(wnd_create(contact_MainWndMsgHandler, BY_NUMBER, (t_lparam)arg->number)))
						ret = app_loop(NULL);
				}else if(IS_SAVED_CONTACT(arg->sid)){
					//2 打开浏览指定联系人的详情
					if(contact_ShowViewWindow((arg->sid>>16), NULL))
						ret = app_loop(NULL);
				}else{
					//2 联系人不存在, 提示保存为新联系人还是保存到现有联系人
					//ret = msgbox_show(NULL, );
					contact_AddToWindow(NULL, 0, arg->number);
					ret = app_loop(NULL);
					//2 保存成功，则需要将CSESSIONID由cid更新为new_rid
					// TODO: 
					if(ret){
						t_HDataSet hdb;
						int i;
						contact_number_t phone;
						
						hdb = g_ds_open(CONTACT_DB_FNAME, CONTACT_DBTAB_LIST);
						g_ds_Goto(hdb, ret);

						//2 检查number，相同则更新sid，因为保存号码时用户可能会修改number
						if(contact_number_parse(g_ds_GetString(hdb, CONTACT_DBFIELD_NUMBER), &phone)){
							for(i = 0; i < CONTACT_F_NUMBER_N; i++){
								if(strcmp(phone.number[i], arg->number) == 0){
									ret = CSESSION_ID(ret, ga_phone_type[i]);
									comm_session_update(TRUE, arg->sid, ret);
									break;
								}
							}
						}
						g_object_unref(hdb);
					}
				}
			}
			break;
		case MOD_ID_PHONENO_GETNAME:
			ret = GetNameByNumberHandler(sparam, lparam);
			break;
		case MOD_ID_CONTACT_GETINFO:
			ret = GetInfoByIDHandler(sparam, lparam);
			break;
		case MOD_ID_CONTACT_QUERY_INTERFACE:
			ret = MSIFuncHandler(sparam, lparam);
			break;
	}
	return ret;
}

t_AmsPackage pack_info_contact = {
	PACK_GUID_contact
	, contact_OnLoad
	, contact_OnUnload
	, contact_AppEntry
	
	, pack_contact_modules
	
#if defined(TARGET_WIN)
	, contact_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_contact;
}

#endif

