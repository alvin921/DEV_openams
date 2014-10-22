#ifndef WGT_pushbutton_H
#define WGT_pushbutton_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_pushbutton	"pushbutton"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_pushbutton)
__declspec(dllimport) extern t_GClass	 class_pushbutton;
#else
extern t_GClass class_pushbutton;
#endif


#define	BTN_ATTR_PUSHED		CTRL_ATTR_BITS_USER

BEGIN_DECLARATION
	

END_DECLARATION



#endif /* WGT_pushbutton_H */


