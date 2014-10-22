#ifndef WGT_nesctrl_H
#define WGT_nesctrl_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_nesctrl	"nesctrl"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_nesctrl)
__declspec(dllimport) extern t_GClass	 class_nesctrl;
#else
extern t_GClass class_nesctrl;
#endif


enum {
	NES_STATE_IDLE
	, NES_STATE_RUNNING
	, NES_STATE_PAUSE
};


BEGIN_DECLARATION
	
gboolean	nesctrl_checkfile(const char *fname);
gboolean	nesctrl_run( t_HWidget handle, const char *fname, gboolean check);
void		nesctrl_stop(t_HWidget handle);
void		nesctrl_pause(t_HWidget handle);
void		nesctrl_resume(t_HWidget handle);
int		nesctrl_state(t_HWidget handle);

END_DECLARATION



#endif /* WGT_nesctrl_H */


