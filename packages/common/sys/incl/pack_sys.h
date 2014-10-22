#ifndef PACK_sys_H
#define PACK_sys_H

#include "ams.h"
#include "sys_resid.h"
#include "ds_appstore.h"
#include "ds_today.h"

#define PACK_GUID_sys	"sys"

#define TB_STYLE_NORMAL	0
#define TB_STYLE_IDLE		1


#define	GOTO_DESKTOP()		app_activate(app_query(prod_get_first_app(), 0));


BEGIN_DECLARATION

gboolean	dlg_editor_show(char *buffer, int bufsize, const char *title, const char *lsk, gu32 flags);


END_DECLARATION

#endif /* PACK_sys_H */

