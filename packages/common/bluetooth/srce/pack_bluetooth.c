#include "pack_bluetooth.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "bluetooth_brd_le.h"
#else
#include "bluetooth_brd_be.h"
#endif
#endif

static void	bluetooth_glob_destroy(bluetooth_glob_t *glob)
{
	if(glob){
		FREEIF(glob);
	}
}

static bluetooth_glob_t *	bluetooth_glob_new(t_sparam sparam, t_lparam lparam)
{
	bluetooth_glob_t *glob;
	
	glob = MALLOC(sizeof(bluetooth_glob_t));
	memset(glob, 0, sizeof(bluetooth_glob_t));

	return glob;
}


static int	bluetooth_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_MAIN:
			{
				bluetooth_glob_t *glob;
				glob = bluetooth_glob_new(sparam, lparam);
				app_set_data(glob);
				if(wnd_show(wnd_create_by_resid(bluetooth_MainWndMsgHandler, 0, 0, brd_get(PACK_GUID_SELF), RESID_WIN_MAIN)))
					ret = app_loop(NULL);
				bluetooth_glob_destroy(glob);
			}
			break;
	}
	return ret;
}


static t_AmsModule		pack_bluetooth_modules[] = {
	{
		MOD_ID_MAIN, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		16*1024, 				// stack size
		RESID_STR_TITLE, 		// title resid
	},
	MOD_INFO_END
};

static error_t		pack_bluetooth_OnLoad(void)
{
	return SUCCESS;
}

static error_t		pack_bluetooth_OnUnload(void)
{
	return SUCCESS;
}

t_AmsPackage pack_info_bluetooth = {
	PACK_GUID_SELF
	, pack_bluetooth_OnLoad	// OnLoad
	, pack_bluetooth_OnUnload	// OnUnload
	, bluetooth_AppEntry

	, pack_bluetooth_modules
	
#if defined(TARGET_WIN)
	, bluetooth_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_bluetooth;
}

#endif

typedef const struct {
	t_BtProfileID	profile;
	t_BtProfileID	peer_profile;
	resid_t		resid;
}t_BtProfRes;

static t_BtProfRes ga_prof_resid[] = {
	{BT_UUID_A2DP		, BT_UUID_A2DP_SINK		, RESID_STR_BT_PROF_A2DP},
	{BT_UUID_AVRCP		, BT_UUID_AVRCP_CT 		, RESID_STR_BT_PROF_AVRCP},
	{BT_UUID_HFP		, BT_UUID_HFP_AG		, RESID_STR_BT_PROF_HFP},
	{BT_UUID_OPP		, BT_UUID_OPP_CLIENT		, RESID_STR_BT_PROF_OPP},
	{BT_UUID_FTP		, BT_UUID_FTP_CLIENT		, RESID_STR_BT_PROF_FTP},
	{BT_UUID_HSP		, BT_UUID_HSP_AG		, RESID_STR_BT_PROF_HSP},
	{BT_UUID_BIP		, BT_UUID_BIP_RESPONDER	, RESID_STR_BT_PROF_BIP},
	{BT_UUID_SPP		, BT_UUID_SPP			, RESID_STR_BT_PROF_SPP},
	{BT_UUID_LAP		, BT_UUID_LAP			, RESID_STR_BT_PROF_LAP},
	{BT_UUID_MAP		, BT_UUID_MAP			, RESID_STR_BT_PROF_MAP},
	{BT_UUID_DUN		, BT_UUID_DUN			, RESID_STR_BT_PROF_DUN},
	{BT_UUID_CTP		, BT_UUID_CTP			, RESID_STR_BT_PROF_CTP},
	{BT_UUID_ICP		, BT_UUID_ICP			, RESID_STR_BT_PROF_ICP},
	{BT_UUID_FAX		, BT_UUID_FAX			, RESID_STR_BT_PROF_FAX},
	{BT_UUID_HDP		, BT_UUID_HDP			, RESID_STR_BT_PROF_HDP},
	{BT_UUID_PANP		, BT_UUID_PANP			, RESID_STR_BT_PROF_PANP},
	{BT_UUID_BPP		, BT_UUID_BPP			, RESID_STR_BT_PROF_BPP},
	{BT_UUID_HID		, BT_UUID_HID			, RESID_STR_BT_PROF_HID},
	{BT_UUID_SAP		, BT_UUID_SAP			, RESID_STR_BT_PROF_SAP},
	{BT_UUID_OBEX_PBAP	, BT_UUID_OBEX_PBAP		, RESID_STR_BT_PROF_PBAP},
};

const char*	bt_profile_string(t_BtProfileID profile)
{
	resid_t resid = RESID_STR_BT_NO_PROF;
	if(profile){
		int i;
		int n = ARRAY_SIZE(ga_prof_resid);
		resid = RESID_STR_BT_PROF_UNKNOWN;
		for(i = 0; i < n; i++){
			if(profile == ga_prof_resid[i].profile || profile == ga_prof_resid[i].peer_profile){
				resid = ga_prof_resid[i].resid;
				break;
			}
		}
	}
	return brd_get_string(brd_get(PACK_GUID_SELF), resid, "");
}

t_HImage	bt_get_cod_icon(gu32 cod)
{
	t_HImage icon;
	if(BT_COD_MAJOR_DEVICE(cod) == BT_COD_MAJOR_AV)
		icon = brd_get_image(brd_get(PACK_GUID_SELF), RESID_ICON_BT_AV);
	else if(BT_COD_MAJOR_DEVICE(cod) == BT_COD_MAJOR_COMPUTER)
		icon = brd_get_image(brd_get(PACK_GUID_SELF), RESID_ICON_BT_COMPUTER);
	else if(BT_COD_MAJOR_DEVICE(cod) == BT_COD_MAJOR_PHONE)
		icon = brd_get_image(brd_get(PACK_GUID_SELF), RESID_ICON_BT_PHONE);
	else if(BT_COD_MAJOR_DEVICE(cod) == BT_COD_MAJOR_IMAGING)
		icon = brd_get_image(brd_get(PACK_GUID_SELF), RESID_ICON_BT_IMAGING);
	else
		icon = brd_get_image(brd_get(PACK_GUID_SELF), RESID_ICON_BT_OTHER);
	return icon;
}
const char *bt_get_state_string(gu8 state)
{
	BrdHandle brd = brd_get(PACK_GUID_SELF);
	resid_t resid = RESID_UNDEF;
	if(BIT_TEST(state, BTDEV_STATE_CONNECTED))
		resid = RESID_STR_BT_CONNECTED;
	else if(BIT_TEST(state, BTDEV_STATE_CONNECTING))
		resid = RESID_STR_BT_CONNECTING;
	else if(BIT_TEST(state, BTDEV_STATE_PAIRED))
		resid = RESID_STR_BT_PAIRED;
	else if(BIT_TEST(state, BTDEV_STATE_PAIRING))
		resid = RESID_STR_BT_PAIRING;
	else if(BIT_TEST(state, BTDEV_STATE_BONDING))
		resid = RESID_STR_BT_PAIRING;
	else if(BIT_TEST(state, BTDEV_STATE_INQUIRING))
		resid = RESID_STR_BT_INQUIRING;
	else if(BIT_TEST(state, BTDEV_STATE_NORMAL))
		resid = RESID_STR_BT_UNPAIRED;
	return brd_get_string(brd, resid, "");
}
