#ifndef WGT_onoff_H
#define WGT_onoff_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_onoff	"onoff"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_onoff)
__declspec(dllimport) extern t_GClass	 class_onoff;
#else
extern t_GClass class_onoff;
#endif

BEGIN_DECLARATION
	
int		onoff_get_member(t_HWidget handle);
int		onoff_set_member(t_HWidget handle, int value);

END_DECLARATION



#endif /* WGT_onoff_H */


