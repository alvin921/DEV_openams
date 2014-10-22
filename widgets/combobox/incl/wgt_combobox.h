#ifndef WGT_combobox_H
#define WGT_combobox_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_combobox	"combobox"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_combobox)
__declspec(dllimport) extern t_GClass	 class_combobox;
#else
extern t_GClass class_combobox;
#endif

BEGIN_DECLARATION
	
int		combobox_get_cursor(t_HWidget handle);
int		combobox_set_cursor(t_HWidget handle, int value);
t_HWidget	combobox_get_listctrl(t_HWidget handle);

END_DECLARATION



#endif /* WGT_combobox_H */


