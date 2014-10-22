#include "pack_desktop.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "desktop_brd_le.h"
#else
#include "desktop_brd_be.h"
#endif
#endif

static error_t 		AppMsgHandler(gpointer globPtr, t_AmosEvent   evt, t_sparam sparam, t_lparam lparam)
{
	error_t ret = NOT_HANDLED;
	switch(evt){
		case EVT_SIRQ_NOTIFY:
			switch(sparam){
				case SIRQ_PAP:
					switch(lparam){
						case PAP_CHARGER:
							break;
						case PAP_TFLASH:
							break;
						case PAP_HEADSET:
							if(lparam == PAP_PLUGIN)
								msgbox_show("Prompt", 0, 1500, 0, "heaset plugin");
							else
								msgbox_show("Prompt", 0, 1500, 0, "headset plugout");
							break;
						case PAP_USB:
							if(lparam == PAP_PLUGIN)
								msgbox_show("Prompt", 0, 1500, 0, "usb plugin");
							else
								msgbox_show("Prompt", 0, 1500, 0, "usb plugout");
							break;
					}
					break;
			}
			break;
		default:
			break;
	}
	return ret;
}

static t_AmsModule		pack_desktop_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 						// title resid
	}
	, MOD_INFO_END
};

static int	desktop_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				t_DesktopGlobData *glob; 
				t_HWindow h;

				glob = MALLOC(sizeof(t_DesktopGlobData));
				memset(glob, 0, sizeof(t_DesktopGlobData));
				app_set_data(glob);

				h = wnd_create_by_resid(desktop_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN);
				wnd_show(h);
				ret = app_loop(AppMsgHandler);
				FREEIF(glob);
			}
			break;
	}
	return ret;
}

t_AmsPackage pack_info_desktop = {
	PACK_GUID_desktop
	, NULL	// OnLoad
	, NULL	// OnUnload
	, desktop_AppEntry

	, pack_desktop_modules
	
#if defined(TARGET_WIN)
			, desktop_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_desktop;
}

#endif


