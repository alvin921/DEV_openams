#ifndef WGT_button_H
#define WGT_button_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_button	"button"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_button)
__declspec(dllimport) extern t_GClass	 class_button;
#else
extern t_GClass class_button;
#endif

#define  BUTTON_OnClicked	WIDGET_OnSelected

BEGIN_DECLARATION


END_DECLARATION



#endif /* WGT_button_H */


