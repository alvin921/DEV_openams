#ifndef	__VSUNISYSTEM_INCLUDED__
#define	__VSUNISYSTEM_INCLUDED__

#include "macro.h"

typedef struct VSDIPSWITCH
{
	char*	name;
	WORD	value;
}VSDIPSWITCH;

extern	VSDIPSWITCH	vsdip_default[];

extern	VSDIPSWITCH*	FindVSDipSwitchTable( DWORD crc );
extern	BYTE	GetVSDefaultDipSwitchValue( DWORD crc );

#endif	// !__VSUNISYSTEM_INCLUDED__
