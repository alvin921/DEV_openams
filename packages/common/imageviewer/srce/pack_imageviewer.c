#include "pack_imageviewer.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "imageviewer_brd_le.h"
#else
#include "imageviewer_brd_be.h"
#endif
#endif


static t_AmsModule		pack_imageviewer_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	}
	, MOD_INFO_END
};

static int	imageviewer_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				t_Global *glob = MALLOC(sizeof(t_Global));

				memset(glob, 0, sizeof(t_Global));
				app_set_data(glob);
				strcpy(glob->cwd, "x:/album");
				glob->fnames = g_strlist_new(4);
				if(wnd_show(wnd_create_by_resid(imageviewer_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN)))
					ret = app_loop(NULL);
				g_strlist_destroy(glob->fnames);
				FREE(glob);
			}
			break;
	}
	return ret;
}

t_AmsPackage pack_info_imageviewer = {
	PACK_GUID_imageviewer
	, NULL	// OnLoad
	, NULL	// OnUnload
	, imageviewer_AppEntry

	, pack_imageviewer_modules
	
#if defined(TARGET_WIN)
	, imageviewer_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_imageviewer;
}

#endif

