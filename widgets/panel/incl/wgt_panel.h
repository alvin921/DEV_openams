#ifndef WGT_panel_H
#define WGT_panel_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_panel	"panel"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_panel)
__declspec(dllimport) extern t_GClass	 class_panel;
#else
extern t_GClass class_panel;
#endif

BEGIN_DECLARATION
	
int		panel_get_member(t_HWidget handle);
int		panel_set_member(t_HWidget handle, int value);

END_DECLARATION



#endif /* WGT_panel_H */


