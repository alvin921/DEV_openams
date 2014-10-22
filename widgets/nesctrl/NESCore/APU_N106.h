//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Namcot N106                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_N106_INCLUDED__
#define	__APU_N106_INCLUDED__


#include "macro.h"

#include "APU_INTERFACE.h"



typedef struct APU_N106_struct APU_N106;

typedef const struct {
	INHERIT_IAPU_VTBL(APU_N106);

}APU_N106_vtbl;

typedef struct {
	INT phaseacc;

	DWORD	freq;
	DWORD	phase;
	DWORD	tonelen;

	INT output;

	BYTE	toneadr;
	BYTE	volupdate;

	BYTE	vol;
	BYTE	databuf;
} CHANNEL;

struct	APU_N106_struct {
	APU_N106_vtbl *vtbl;

	CHANNEL	op[8];

	FLOAT	cpu_clock;
	DWORD	cycle_rate;

	BYTE	addrinc;
	BYTE	address;
	BYTE	channel_use;

	BYTE	tone[0x100];
};

APU_N106 * APU_N106_new(NES* parent);

#endif	// !__APU_N106_INCLUDED__
