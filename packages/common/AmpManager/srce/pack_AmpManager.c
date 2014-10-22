#include "pack_AmpManager.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "AmpManager_brd_le.h"
#else
#include "AmpManager_brd_be.h"
#endif
#endif

gboolean	AmpManager_ShowInfoWindow(const char *fname)
{
	t_HWindow h=NULL;
	t_AmpFormat type;

	type = amp_check(fname);
	if(type == AMP_FORMAT_PROD){
		h = wnd_create_by_resid(AmpManager_ProdWndMsgHandler, 0, (gu32)fname, brd_get(PACK_GUID_SELF), RESID_WIN_PROD);
	}else if(type == AMP_FORMAT_PACK_DLL || type == AMP_FORMAT_PACK_PTL){
		h = wnd_create_by_resid(AmpManager_PackWndMsgHandler, 0, (gu32)fname, brd_get(PACK_GUID_SELF), RESID_WIN_PACK);
	}else{
		msgbox_show("AmpManager", _RSK(BACK), 0, 0, "amp file error: %s", fname);
		return FALSE;
	}
	wnd_show(h);
	return g_object_valid(h)?TRUE:FALSE;
}

void AmpManager_DisplayInfo(t_HWindow handle, t_AmpInfo *info)
{
	t_HGDI gdi = wgt_get_client_gdi((t_HWidget)handle);
	if(gdi && info){
		coord_t x, y;
		coord_t icon_w, icon_h;
		gui_rect_t client;

		wgt_get_client_rect((t_HWidget)handle, &client);
		
		x = 10;
		y = 0;
		icon_w = icon_h = 0;
		if(g_object_valid(info->icon)){
			icon_w = image_get_width(info->icon);
			icon_h = image_get_height(info->icon);
			gdi_draw_image_in(gdi, 0, y, client.dx, icon_h, info->icon, IDF_HALIGN_BESTFIT);
		}
		y += icon_h+10;
		gdi_set_color(gdi, RGB_RED);
		gdi_move_to(gdi, x, y);
		gdi_draw_text(gdi, "Title: ", -1, CHARSET_UNDEF);
		gdi_draw_text(gdi, info->title, -1, CHARSET_UNDEF);
		y += 20;
		gdi_move_to(gdi, x, y);
		gdi_draw_text(gdi, "Author: ", -1, CHARSET_UNDEF);
		gdi_draw_text(gdi, info->author, -1, CHARSET_UNDEF);
		y += 20;
		gdi_move_to(gdi, x, y);
		gdi_draw_text(gdi, "Company: ", -1, CHARSET_UNDEF);
		gdi_draw_text(gdi, info->company, -1, CHARSET_UNDEF);
		y += 20;
		gdi_move_to(gdi, x, y);
		gdi_draw_text(gdi, "Version: ", -1, CHARSET_UNDEF);
		gdi_draw_text(gdi, info->version, -1, CHARSET_UNDEF);
		y += 20;
		gdi_move_to(gdi, x, y);
		gdi_draw_text(gdi, "Description: ", -1, CHARSET_UNDEF);
		y += 20;
		gdi_move_to(gdi, x, y);
		gdi_draw_text(gdi, info->description, -1, CHARSET_UNDEF);

	}
	g_object_unref(gdi);
}


static t_AmsModule		pack_AmpManager_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	{
		MOD_ID_AMP, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	MOD_INFO_END
};

static int	AmpManager_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				t_AmpManagerData *glob;

				glob = MALLOC(sizeof(t_AmpManagerData));
				memset(glob, 0, sizeof(t_AmpManagerData));
				app_set_data(glob);
				
				if(wnd_show(wnd_create_by_resid(AmpManager_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN)))
					ret = app_loop(NULL);
				FREEIF(glob);
			}
			break;
		case MOD_ID_AMP:
			{
				t_AmpManagerData *glob;
				const char *fname = (const char *)lparam;
				int fnsize = sparam;

				glob = MALLOC(sizeof(t_AmpManagerData));
				memset(glob, 0, sizeof(t_AmpManagerData));
				app_set_data(glob);

				if(AmpManager_ShowInfoWindow(fname)){
					ret = app_loop(NULL);
				}
				FREEIF(glob);
			}
			break;
	}
	return ret;
}

t_AmsPackage pack_info_AmpManager = {
	PACK_GUID_AmpManager
	, NULL	// OnLoad
	, NULL	// OnUnload
	, AmpManager_AppEntry
	
	, pack_AmpManager_modules
	
#if defined(TARGET_WIN) 
	, AmpManager_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_AmpManager;
}

#endif

