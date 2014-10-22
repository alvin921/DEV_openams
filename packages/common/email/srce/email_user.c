#include "ams.h"
#include "email_user.h"

static t_EmailUser gs_email_user = {{0xff, }, };

t_EmailUser * email_userinfo_load(void)
{
	if((gu8)gs_email_user.display_name[0] == 0xff){
		int fd;
		memset(&gs_email_user, 0, sizeof(gs_email_user));
		fd = fsi_open(EMAIL_USER_FILE, _O_RDONLY, _S_IREAD);
		if(fd >= 0){
			fsi_read(fd, &gs_email_user, sizeof(gs_email_user));
			fsi_close(fd);
		}
	}
	return &gs_email_user;
}

void	email_userinfo_store(void)
{
	int fd;
	fd = fsi_open(EMAIL_USER_FILE, _O_RDWR|_O_CREAT, _S_IREAD|_S_IWRITE);
	if(fd >= 0){
		fsi_write(fd, &gs_email_user, sizeof(gs_email_user));
		fsi_close(fd);
	}
}

