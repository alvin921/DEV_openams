#include "pack_ime.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "ime_brd_le.h"
#else
#include "ime_brd_be.h"
#endif
#endif

#if defined(FTR_NO_INPUT_KEY)
extern t_IMEInfo		gvIMEVk;
#else
extern t_IMEInfo		gvIMEMultitap;
#endif

static error_t		pack_ime_OnLoad(void)
{
#if defined(FTR_NO_INPUT_KEY)
	ime_agent_register(&gvIMEVk);
#else
	ime_agent_register(&gvIMEMultitap);
#endif
	return SUCCESS;
}

static error_t		pack_ime_OnUnload(void)
{
#if defined(FTR_NO_INPUT_KEY)
	ime_agent_unregister(&gvIMEVk);
#else
	ime_agent_unregister(&gvIMEMultitap);
#endif
	return SUCCESS;
}

t_AmsPackage pack_info_ime = {
	PACK_GUID_SELF
	, pack_ime_OnLoad	// OnLoad
	, pack_ime_OnUnload	// OnUnload
	, NULL
	
	, NULL
	
#if defined(TARGET_WIN)
	, ime_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_ime;
}

#endif

