#ifndef WGT_softkey_H
#define WGT_softkey_H


#include "ams.h"

#if defined(FTR_SOFT_KEY)

#include "wgt_menu.h"

// TODO: CLSID_xxx MUST be unique!!!
#ifndef CLSID_SOFTKEY
#define	CLSID_SOFTKEY	"softkey"
#endif

#if defined(TARGET_WIN) && !defined(DLL_EXPORT_softkey)
__declspec(dllimport) extern t_GClass	 class_softkey;
#else
extern t_GClass class_softkey;
#endif



#define SKCTRL_ATTR_WIDGET		(CTRL_ATTR_BITS_USER << 1)
//#define SKCTRL_ATTR_RSK			(CTRL_ATTR_BITS_USER << 2)

enum {
	SK_LSK
#if defined(FTR_CSK_ENABLED)
	, SK_CSK
#endif
	, SK_RSK

	, SK_TOTAL
};

enum {
	SK_TEXT_ALIGN_LEFT
	, SK_TEXT_ALIGN_CENTER
	, SK_TEXT_ALIGN_RIGHT

	, SK_TEXT_ALIGN_N
};


//#define	sk_bind_lsk(h)	wgt_disable_attr(h, SKCTRL_ATTR_RSK)
//#define sk_bind_rsk(h)	wgt_enable_attr(h, SKCTRL_ATTR_RSK)

error_t	sk_set_text(t_SKHandle sk, gu8 id, const char *str, resid_t resid);
void	sk_set_text_align(t_SKHandle sk, gu8 id, gu8 align);
error_t	sk_set_callback(t_SKHandle sk, gu8 id, t_WidgetCallback cb, void *data);
error_t	sk_set_popup(t_SKHandle sk, gu8 id, t_HWidget h);
t_HWidget	sk_get_popup(t_SKHandle sk, gu8 id);
error_t	sk_set_app(t_HWidget sk, gu8 id, const char *guid, t_AmsModuleId modid);

#endif





#endif /* WGT_softkey_H */


