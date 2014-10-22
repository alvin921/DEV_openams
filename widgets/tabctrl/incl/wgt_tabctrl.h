#ifndef WGT_tabctrl_H
#define WGT_tabctrl_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_tabctrl	"tabctrl"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_tabctrl)
__declspec(dllimport) extern t_GClass	 class_tabctrl;
#else
extern t_GClass class_tabctrl;
#endif


BEGIN_DECLARATION
	
int		tabctrl_get_cursor(t_HWidget handle);
int		tabctrl_set_cursor(t_HWidget handle, int value);
void		tabctrl_set_title_style(t_HWidget handle, gu8 title_height, gu8 title_dir);

END_DECLARATION



#endif /* WGT_tabctrl_H */


