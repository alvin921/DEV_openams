#ifndef WGT_label_H
#define WGT_label_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_label	"label"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_label)
__declspec(dllimport) extern t_GClass	 class_label;
#else
extern t_GClass class_label;
#endif

BEGIN_DECLARATION

void		label_set_text(t_HWidget handle, const char *fmt, ...);
void		label_set_text_by_resid(t_HWidget handle, BrdHandle brd, resid_t resid);
const char *	label_get_text(t_HWidget handle);
void		label_insert_text(t_HWidget handle, int cursor, int replace, const char *str, int len);


#define	label_append_char(handle, ch)		do{char cc=(ch); label_insert_text(handle, -1, 0, &(cc), 1);}while(0)
#define	label_append_text(handle, text, n)	label_insert_text(handle, -1, 0, text, n)
#define	label_prepend_char(handle, ch)		do{char cc=(ch); label_insert_text(handle, 0, 0, &(cc), 1);}while(0)
#define	label_prepend_text(handle, text, n)	label_insert_text(handle, 0, 0, text, n)


END_DECLARATION



#endif /* WGT_label_H */


