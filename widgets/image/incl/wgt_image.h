#ifndef WGT_image_H
#define WGT_image_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_image	"image"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_image)
__declspec(dllimport) extern t_GClass	 class_imagectrl;
#else
extern t_GClass class_imagectrl;
#endif

BEGIN_DECLARATION
	
void	imagectrl_set(t_HWidget handle, t_HImage image);
void	imagectrl_set_by_resid(t_HWidget handle, BrdHandle brd, resid_t resid);
void	imagectrl_set_by_file(t_HWidget handle, const char *fname);

END_DECLARATION



#endif /* WGT_image_H */


