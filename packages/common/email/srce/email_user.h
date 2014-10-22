#ifndef EMAIL_USER_H
#define EMAIL_USER_H

#define	EMAIL_USER_FILE		"c:/email_user"

#define	DISPLAY_NAME_MAX		23
#define	EMAIL_LENGTH_MAX		31
#define	HOST_NAME_MAX		39
#define	HOST_USER_MAX		39
#define	HOST_PASS_MAX		19

typedef struct {
	char		display_name[DISPLAY_NAME_MAX+1];
	char		email[EMAIL_LENGTH_MAX+1];
	char		pop3_server[HOST_NAME_MAX+1];
	char		pop3_user[HOST_USER_MAX+1];
	char		pop3_pass[HOST_PASS_MAX+1];
	char		smtp_server[HOST_NAME_MAX+1];
	char		smtp_user[HOST_USER_MAX+1];
	char		smtp_pass[HOST_PASS_MAX+1];
}t_EmailUser;


BEGIN_DECLARATION
	
t_EmailUser * email_userinfo_load(void);
void	email_userinfo_store(void);

END_DECLARATION
	
#endif /* EMAIL_USER_H */