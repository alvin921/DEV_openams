#include "pack_contact.hi"


typedef struct {
	t_HDataSet	hdb;
	char *number;
	contact_number_t parsed_phone;
	t_ContactID	cid;
	gboolean		modified;
	gu8		group;
	dos_date_t	birthday;
	MC_FILL_STRUCT2;
	resid_t		resid_ring;
	resid_t		resid_picture;
	//contact_info_t	info;
}contact_edit_t;

static void	__save_contact(t_HWindow handle, contact_edit_t *tag)
{
	char *string;
	t_DBRecordID rid;
	
	if(tag->cid == DB_RID_NULL){
		if(g_ds_AddNew(tag->hdb) != SUCCESS){
			msgbox_show(NULL, _CSK(OK), 0, 0, "contact db full");
			return;
		}
	}
	//2 Name
	string = editor_get_text_dup(wnd_get_widget(handle, RESID_EDITOR_NAME));
	g_ds_SetString(tag->hdb, CONTACT_DBFIELD_NAME, string);
	FREEIF(string);
	//2 Number
	memset(&tag->parsed_phone, 0, sizeof(tag->parsed_phone));
	editor_get_text(wnd_get_widget(handle, RESID_EDITOR_OFFICE), tag->parsed_phone.number[F_NUMBER_OFFICE], CONTACT_NUMBER_MAX+1);
	editor_get_text(wnd_get_widget(handle, RESID_EDITOR_HOME), tag->parsed_phone.number[F_NUMBER_HOME], CONTACT_NUMBER_MAX+1);
	editor_get_text(wnd_get_widget(handle, RESID_EDITOR_MOBILE), tag->parsed_phone.number[F_NUMBER_MOBILE], CONTACT_NUMBER_MAX+1);
	editor_get_text(wnd_get_widget(handle, RESID_EDITOR_FAX), tag->parsed_phone.number[F_NUMBER_FAX], CONTACT_NUMBER_MAX+1);
	editor_get_text(wnd_get_widget(handle, RESID_EDITOR_OTHER), tag->parsed_phone.number[F_NUMBER_OTHER], CONTACT_NUMBER_MAX+1);
	string = contact_number_format_dup(&tag->parsed_phone);
	g_ds_SetString(tag->hdb, CONTACT_DBFIELD_NUMBER, string);
	FREEIF(string);
	//2 Group
	g_ds_SetValue(tag->hdb, CONTACT_DBFIELD_GROUP, tag->group);
	//2 Birthday
	g_ds_SetValue(tag->hdb, CONTACT_DBFIELD_BIRTHDAY, tag->birthday);
	//2 Ring
	string = editor_get_text_dup(wnd_get_widget(handle, RESID_EDITOR_RING));
	g_ds_SetString(tag->hdb, CONTACT_DBFIELD_RING, string);
	FREEIF(string);
	//2 Picture
	string = editor_get_text_dup(wnd_get_widget(handle, RESID_EDITOR_PICTURE));
	g_ds_SetString(tag->hdb, CONTACT_DBFIELD_PICTURE, string);
	FREEIF(string);
	//2 Email
	string = editor_get_text_dup(wnd_get_widget(handle, RESID_EDITOR_EMAIL));
	g_ds_SetString(tag->hdb, CONTACT_DBFIELD_EMAIL, string);
	FREEIF(string);
	//2 Address
	string = editor_get_text_dup(wnd_get_widget(handle, RESID_EDITOR_ADDRESS));
	g_ds_SetString(tag->hdb, CONTACT_DBFIELD_ADDRESS, string);
	FREEIF(string);

	rid = g_ds_Update(tag->hdb);
	if(rid == DB_RID_NULL){
		msgbox_show(NULL, _CSK(OK), 0, 0, "failed: contact db full");
	}else if(tag->cid == DB_RID_NULL && app_current_module() == MOD_ID_MAIN){
		//2 新建联系人
	}
	tag->cid = rid;
}

static void __sk_callback(t_HWidget sk, void *data)
{
	t_HWindow handle = wgt_parent_window(sk);
	contact_edit_t *tag = (contact_edit_t *)wgt_get_tag(handle);
	int id = (int)data;
	if(g_object_valid(handle)){
		if(id == SK_RSK){
			if(tag->modified && (msgbox_show(NULL, MBB_YES_NO, 0, 0, brd_get_string(BRD_HANDLE_SELF, RESID_STR_PROMPT_SAVE_CONTACT, NULL)) == MBB_YES)){
				id = SK_LSK;
			}
		}
		if(id == SK_LSK){
			t_HWidget widget;
			
			//2 check name
			widget = wnd_get_widget(handle, RESID_EDITOR_NAME);
			if(editor_get_nchars(widget) <= 0){
				msgbox_show(NULL, _CSK(OK), 0, 0, "Name empty");
				return;
			}
			//2 check number
			if((editor_get_nchars(wnd_get_widget(handle, RESID_EDITOR_OFFICE)) <= 0)
				&& (editor_get_nchars(wnd_get_widget(handle, RESID_EDITOR_HOME)) <= 0)
				&& (editor_get_nchars(wnd_get_widget(handle, RESID_EDITOR_MOBILE)) <= 0)
				&& (editor_get_nchars(wnd_get_widget(handle, RESID_EDITOR_FAX)) <= 0)
				&& (editor_get_nchars(wnd_get_widget(handle, RESID_EDITOR_OTHER)) <= 0)
				){
				msgbox_show(NULL, _CSK(OK), 0, 0, "no number provided");
				return;
			}
			__save_contact(handle, tag);
		}
		wnd_send_close(handle, tag->cid);
	}
}

//2 进入编辑界面有如下几种场合: 
//3 1. 新建联系人: (hdb)
//3 2. 号码创建新联系人: (hdb, number)
//3 3. 号码保存到现有联系人: (hdb, rid, number)
//3 4. 编辑指定联系人: (hdb, rid)

error_t	contact_EditWndMsgHandler(t_HWindow handle, t_WidgetEvent evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	contact_edit_t *tag = (contact_edit_t *)wgt_get_tag(handle);
	
	switch(evt){
		case WINDOW_OnOpen:
			{
				t_ContactEditWndParam *param = (t_ContactEditWndParam *)lparam;
				t_HWidget widget;

				wgt_enable_attr(handle, WND_ATTR_DEFAULT|CTRL_ATTR_VSCROLLABLE);
				tag = MALLOC(sizeof(contact_edit_t));
				memset(tag, 0, sizeof(contact_edit_t));
				if(param->hdb)
					tag->hdb = g_object_ref(param->hdb);
				else
					tag->hdb = g_ds_open(CONTACT_DB_FNAME, CONTACT_DBTAB_LIST);
					
				tag->cid = param->cid;
				tag->number = (param->number&&param->number[0]) ? g_str_dup(param->number) : NULL;
				tag->modified = FALSE;
				wgt_set_tag(handle, tag);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_NAME);
				wgt_set_border(widget, BORDER_TYPE_LINE, 0, guiTop);
				editor_set_type(widget, EDITOR_TYPE_TEXT, FALSE);
				editor_set_nbytes_max(widget, CONTACT_NAME_MAX);

				widget = wnd_get_widget(handle, RESID_EDITOR_OFFICE);
				editor_set_type(widget, EDITOR_TYPE_PHONE, TRUE);
				editor_set_nbytes_max(widget, CONTACT_NUMBER_MAX);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_HOME);
				editor_set_type(widget, EDITOR_TYPE_PHONE, TRUE);
				editor_set_nbytes_max(widget, CONTACT_NUMBER_MAX);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_MOBILE);
				editor_set_type(widget, EDITOR_TYPE_PHONE, TRUE);
				editor_set_nbytes_max(widget, CONTACT_NUMBER_MAX);

				widget = wnd_get_widget(handle, RESID_EDITOR_FAX);
				editor_set_type(widget, EDITOR_TYPE_PHONE, TRUE);
				editor_set_nbytes_max(widget, CONTACT_NUMBER_MAX);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_OTHER);
				editor_set_type(widget, EDITOR_TYPE_PHONE, TRUE);
				editor_set_nbytes_max(widget, CONTACT_NUMBER_MAX);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_EMAIL);
				editor_set_type(widget, EDITOR_TYPE_TEXT, FALSE);
				editor_set_nbytes_max(widget, CONTACT_NAME_MAX);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_ADDRESS);
				editor_set_type(widget, EDITOR_TYPE_TEXT, FALSE);
				editor_set_nbytes_max(widget, CONTACT_NAME_MAX);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_RING);
				editor_set_type(widget, EDITOR_TYPE_TEXT, FALSE);
				editor_set_nbytes_max(widget, CONTACT_NAME_MAX);
				
				widget = wnd_get_widget(handle, RESID_EDITOR_GROUP);
				editor_set_type(widget, EDITOR_TYPE_TEXT, FALSE);
				editor_set_nbytes_max(widget, CONTACT_NAME_MAX);

				widget = wnd_get_widget(handle, RESID_EDITOR_PICTURE);
				editor_set_type(widget, EDITOR_TYPE_TEXT, FALSE);
				editor_set_nbytes_max(widget, CONTACT_NAME_MAX);

				if(tag->cid == DB_RID_NULL){
					//2 新建联系人
					wnd_set_caption_by_resid(handle, BRD_HANDLE_SELF, RESID_STR_ADD_CONTACT);
					//g_ds_AddNew(tag->hdb);
					
					if(tag->number != NULL){
						editor_set_text(wnd_get_widget(handle, RESID_EDITOR_OFFICE), (const char *)tag->number);
						tag->modified = TRUE;
					}
				}else{
					//2 编辑联系人或保存号码到现有联系人
					wnd_set_caption_by_resid(handle, BRD_HANDLE_SELF, RESID_STR_EDIT_CONTACT);
					g_ds_Goto(tag->hdb, tag->cid);
					editor_set_text(wnd_get_widget(handle, RESID_EDITOR_NAME), g_ds_GetString(tag->hdb, CONTACT_DBFIELD_NAME));
					
					contact_number_parse(g_ds_GetString(tag->hdb, CONTACT_DBFIELD_NUMBER), &tag->parsed_phone);
					widget = wnd_get_widget(handle, RESID_EDITOR_MOBILE);
					if(tag->parsed_phone.number[F_NUMBER_MOBILE][0])
						editor_set_text(widget, tag->parsed_phone.number[F_NUMBER_MOBILE]);
					else if(tag->number != NULL)
						editor_set_text(widget, tag->number);
					
					widget = wnd_get_widget(handle, RESID_EDITOR_OFFICE);
					if(tag->parsed_phone.number[F_NUMBER_OFFICE][0])
						editor_set_text(widget, tag->parsed_phone.number[F_NUMBER_OFFICE]);
					else if(tag->number != NULL)
						editor_set_text(widget, tag->number);
					
					widget = wnd_get_widget(handle, RESID_EDITOR_HOME);
					if(tag->parsed_phone.number[F_NUMBER_HOME][0])
						editor_set_text(widget, tag->parsed_phone.number[F_NUMBER_HOME]);
					else if(tag->number != NULL)
						editor_set_text(widget, tag->number);
					
					widget = wnd_get_widget(handle, RESID_EDITOR_FAX);
					if(tag->parsed_phone.number[F_NUMBER_FAX][0])
						editor_set_text(widget, tag->parsed_phone.number[F_NUMBER_FAX]);
					else if(tag->number != NULL)
						editor_set_text(widget, tag->number);
					
					widget = wnd_get_widget(handle, RESID_EDITOR_OTHER);
					if(tag->parsed_phone.number[F_NUMBER_OTHER][0])
						editor_set_text(widget, tag->parsed_phone.number[F_NUMBER_OTHER]);
					else if(tag->number != NULL)
						editor_set_text(widget, tag->number);
					
					editor_set_text(wnd_get_widget(handle, RESID_EDITOR_EMAIL), g_ds_GetString(tag->hdb, CONTACT_DBFIELD_EMAIL));
					editor_set_text(wnd_get_widget(handle, RESID_EDITOR_ADDRESS), g_ds_GetString(tag->hdb, CONTACT_DBFIELD_ADDRESS));
					editor_set_text(wnd_get_widget(handle, RESID_EDITOR_RING), g_ds_GetString(tag->hdb, CONTACT_DBFIELD_RING));
					//editor_set_text(wnd_get_widget(handle, RESID_EDITOR_GROUP), g_ds_GetString(glob->hdb, CONTACT_DBFIELD_NAME));
					editor_set_text(wnd_get_widget(handle, RESID_EDITOR_PICTURE), g_ds_GetString(tag->hdb, CONTACT_DBFIELD_PICTURE));
				}
#if defined(FTR_SOFT_KEY)
				{
					t_SKHandle sk = wnd_get_skbar(handle);
					if(g_object_valid(sk)){
						sk_set_text(sk, SK_LSK, NULL, RESID_STR_SAVE);
						sk_set_callback(sk, SK_LSK, (t_WidgetCallback)__sk_callback, (void *)SK_LSK);
						sk_set_text(sk, SK_RSK, NULL, RESID_STR_BACK);
						sk_set_callback(sk, SK_RSK, (t_WidgetCallback)__sk_callback, (void *)SK_RSK);
					}
				}
#endif
			}
			
			
			ret = SUCCESS;
			break;
		case WIDGET_OnKeyEvent:
			if(sparam == MMI_KEY_PRESS && MVK_IS_DIGIT(lparam) && !tag->modified)
				tag->modified = TRUE;
			break;
		case WINDOW_OnClose:
			if(tag){
				g_object_unref(tag->hdb);
				FREEIF(tag->number);
				FREEIF(tag);
			}
			ret = SUCCESS;
			break;
		case WINDOW_OnDraw:
			ret = SUCCESS;
			break;
	}
	return ret;
}

