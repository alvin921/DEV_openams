//
// DirectInput class
//
#include "DirectInput.h"


static WORD	m_SyncBits = 0;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

void DirectInput_Initial(void)
{
	m_SyncBits = 0;
}

// データポーリング
void	DirectInput_Poll(void)
{
	m_SyncBits = 0;
	
	if(amos_get_key_status(MVK_UP) != MMI_KEY_UP)	m_SyncBits |= NES_KEY_UP;
	if(amos_get_key_status(MVK_DOWN) != MMI_KEY_UP)	m_SyncBits |= NES_KEY_DOWN; 
	if(amos_get_key_status(MVK_LEFT) != MMI_KEY_UP)	m_SyncBits |= NES_KEY_LEFT; 
	if(amos_get_key_status(MVK_RIGHT) != MMI_KEY_UP)	m_SyncBits |= NES_KEY_RIGHT; 
	if(amos_get_key_status(MVK_1) != MMI_KEY_UP)	m_SyncBits |= NES_KEY_A; 
	if(amos_get_key_status(MVK_3) != MMI_KEY_UP)	m_SyncBits |= NES_KEY_B; 
	if(amos_get_key_status(MVK_0) != MMI_KEY_UP)	m_SyncBits |= NES_KEY_SELECT; 
	if(amos_get_key_status(MVK_OK) != MMI_KEY_UP)	m_SyncBits |= NES_KEY_START; 
	if(amos_get_key_status(MVK_STAR) != MMI_KEY_UP)	m_SyncBits |= NES_KEY_PRIOR; 
	if(amos_get_key_status(MVK_HASH) != MMI_KEY_UP)	m_SyncBits |= NES_KEY_NEXT; 
}

WORD	DirectInput_GetBits(void)
{
	return m_SyncBits;
}

