
#define DB_CONTACT_C

#include "pack_contact.hi"


void		db_contact_init(void)
{
	if(fsi_stat(CONTACT_DB_FNAME, NULL)){
		t_HDB h;
		g_db_create(CONTACT_DB_FNAME);
		h = g_db_open(CONTACT_DB_FNAME);
		if(g_object_valid(h)){
			g_db_create_table(h, CONTACT_DBTAB_LIST, prod_storage_contact_max(), contact_db_fields, ARRAY_SIZE(contact_db_fields));
			g_object_unref(h);
		}
	}
}

int contact_number_parse(const char *string, contact_number_t *phone)
{
	char *p;
	g_strlist_t *list;
	int count, i, j;

	if(!string)
		return 0;
	list = g_str_split(string, CONTACT_F_NUMBER_SPLIT, FALSE);
	count = g_strlist_size(list);
	if(count > CONTACT_F_NUMBER_N)
		count = CONTACT_F_NUMBER_N;
	if(phone){
		memset(phone, 0, sizeof(contact_number_t));

		//2 第一遍先扫描有指定类型的号码
		for(i = 0; i < count; i++){
			p = g_strlist_get(list, i);
			#define x_case(f)		case CONTACT_##f: strncpy(phone->number[f], p+1, CONTACT_NUMBER_MAX); break
			switch(*p){
				x_case(F_NUMBER_MOBILE);
				x_case(F_NUMBER_OFFICE);
				x_case(F_NUMBER_HOME);
				x_case(F_NUMBER_FAX);
				x_case(F_NUMBER_OTHER);
			}
		}
		//2 第二遍扫描未指定类型的号码
		for(i = 0; i < count; i++){
			p = g_strlist_get(list, i);
			if(isdigit(*p) || *p == '+'){
				for(j = 0; j < CONTACT_F_NUMBER_N; j++){
					if(phone->number[j][0] == 0){
						strncpy(phone->number[j], p, CONTACT_NUMBER_MAX);
						break;
					}
				}
			}
		}
	}
	g_strlist_destroy(list);
	return count;
}

int contact_number_format(char *string, contact_number_t *phone)
{
	int len = 0, i;
	if(!phone)
		return 0;
	if(string == NULL){
		for(i = 0; i < CONTACT_F_NUMBER_N; i++){
			if(phone->number[i][0]){
				len += strlen(phone->number[i])+2; //3 <f>+<number>+';'
			}
		}
		if(len > 0)
			len--;
	}else{
		for(i = 0; i < CONTACT_F_NUMBER_N; i++){
			if(phone->number[i][0]){
				if(len) string[len++] = ';';
				len += sprintf(string+len, "%c%s", ga_phone_type[i], phone->number[i]);
			}
		}
		string[len] = 0;
	}
	return len;
}

char * contact_number_format_dup(contact_number_t *number)
{
	char *str = NULL;
	int len;
	len = contact_number_format(NULL, number);
	if(len){
		str = MALLOC(len+1);
		contact_number_format(str, number);
	}
	return str;
}


#undef DB_CONTACT_C

