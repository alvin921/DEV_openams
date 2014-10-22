#ifndef WGT_editor_H
#define WGT_editor_H


#include "ams.h"

// TODO: CLSID_xxx MUST be unique!!!
#define CLSID_editor	"editor"

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_editor)
__declspec(dllimport) extern t_GClass	 class_editor;
#else
extern t_GClass class_editor;
#endif


enum {
	EDITOR_TYPE_DIGIT	// only 0~9 valid
	, EDITOR_TYPE_PHONE	// "0~9*#+"valid, '+' is '*' long pressed
	, EDITOR_TYPE_INTEGER	// only 0~9 valid
	, EDITOR_TYPE_FLOAT // '*' is '.'
	, EDITOR_TYPE_TEXT

	, EDITOR_TYPE_N
};


#define	editor_unselect(h)	editor_set_sel((t_HWidget)(h), 0)

BEGIN_DECLARATION

//2 flags like: IDF_TEXT_xxxx, IDF_HALIGN_yyyy, IDF_VALIGN_zzzz
void		editor_set_flags(t_HWidget handle, gu32 flags);
void		editor_set_line_height(t_HWidget handle, gu8 line_height);
gu8		editor_get_line_height(t_HWidget handle);

error_t	editor_set_type(t_HWidget handle, int type, gboolean ime_frozen);
error_t	editor_set_nchar_max(t_HWidget handle, int nchars);
ucs2_t *	editor_get_utext(t_HWidget h);
void		editor_set_utext(t_HWidget h, const ucs2_t *string);
int		editor_get_nchars(t_HWidget handle);
error_t	editor_set_nbytes_max(t_HWidget handle, int nbytes);
int		editor_get_text(t_HWidget handle, char *buffer, int size);
char *	editor_get_text_dup(t_HWidget handle);
void		editor_set_text(t_HWidget h, const char *string);
void		editor_set_cursor(t_HWidget h, int cursor);
void		editor_set_sel(t_HWidget h, int nchars);
void		editor_add_char(t_HWidget h, gu8 mode, ucs2_t ch);

END_DECLARATION



#endif /* WGT_editor_H */


