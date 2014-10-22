#ifndef DB_CONTACT_H
#define DB_CONTACT_H

#include "dbase.h"


#define	CONTACT_DB_FNAME		"c:/contact"
#define	CONTACT_DBTAB_LIST	"list"

#define	CONTACT_DBFIELD_NAME			"name"
#define	CONTACT_DBFIELD_NUMBER		"number"
#define	CONTACT_DBFIELD_GROUP		"group"
#define	CONTACT_DBFIELD_BIRTHDAY		"birthday"
#define	CONTACT_DBFIELD_RING			"ring"
#define	CONTACT_DBFIELD_PICTURE		"picture"
#define	CONTACT_DBFIELD_EMAIL		"email"
#define	CONTACT_DBFIELD_ADDRESS		"address"

#if defined(DB_CONTACT_C)

static const t_DBFieldInfo		contact_db_fields[] = {
	{CONTACT_DBFIELD_NAME		, DBFIELD_TYPE_STRING	, FALSE	, 0},
	{CONTACT_DBFIELD_NUMBER 	, DBFIELD_TYPE_STRING	, FALSE	, 0},	//3 电话号码以,分隔
	{CONTACT_DBFIELD_GROUP	, DBFIELD_TYPE_BYTE		, FALSE	, 0},	//3 群组
	{CONTACT_DBFIELD_BIRTHDAY 	, DBFIELD_TYPE_DATE 		, FALSE	, 0},
	{CONTACT_DBFIELD_RING 		, DBFIELD_TYPE_STRING	, FALSE	, 0},	//3 如果是数字串, 则表示铃音资源, 否则是铃音文件完整路径(a:表示prod, c:表示fs)
	{CONTACT_DBFIELD_PICTURE 	, DBFIELD_TYPE_STRING	, FALSE	, 0},	//3 如果是数字串, 则表示图片资源, 否则是图片文件完整路径(a:表示prod, c:表示fs)
	{CONTACT_DBFIELD_EMAIL 		, DBFIELD_TYPE_STRING 	, FALSE	, 0},
	{CONTACT_DBFIELD_ADDRESS	, DBFIELD_TYPE_STRING	, FALSE	, 0},
};
#endif


#define	CONTACT_NAME_MAX		39
#define	CONTACT_EMAIL_MAX	23
#define	CONTACT_ADDRESS_MAX	63

#if 0
#define	CONTACT_F_NUMBER_OFFICE		'O'
#define	CONTACT_F_NUMBER_HOME		'H'
#define	CONTACT_F_NUMBER_MOBILE		'M'
#define	CONTACT_F_NUMBER_FAX		'F'
#define	CONTACT_F_NUMBER_OTHER		'X'

#define	CONTACT_F_NUMBER_N			5
#endif

#define	CONTACT_F_NUMBER_SPLIT		';'

enum {
	F_NUMBER_MOBILE,
	F_NUMBER_OFFICE,
	F_NUMBER_HOME,
	F_NUMBER_FAX,
	F_NUMBER_OTHER,
};
typedef struct {
	char		number[CONTACT_F_NUMBER_N][CONTACT_NUMBER_MAX+1]; // 5 * 20 = 100
}contact_number_t;

enum {
	CONTACT_STORAGE_MS,
	CONTACT_STORAGE_SIM0,
	CONTACT_STORAGE_SIM1,
	CONTACT_STORAGE_SIM2,
	CONTACT_STORAGE_SIM3,
};

BEGIN_DECLARATION

void		db_contact_init(void);
//2 return # of phone numbers
int 		contact_number_parse(const char *string, contact_number_t *phone);
int 		contact_number_format(char *string, contact_number_t *phone);
char * 	contact_number_format_dup(contact_number_t *number);


END_DECLARATION

#endif
