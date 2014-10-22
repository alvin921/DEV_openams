//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Nintendo MMC5                                                   //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_MMC5_INCLUDED__
#define	__APU_MMC5_INCLUDED__


#include "macro.h"

#include "APU_INTERFACE.h"


typedef struct APU_MMC5_struct APU_MMC5;

typedef const struct {
	INHERIT_IAPU_VTBL(APU_MMC5);

}APU_MMC5_vtbl;

typedef struct {
	BYTE	reg[4];
	BYTE	enable;

	INT vbl_length;

	INT phaseacc;
	INT freq;

	INT output_vol;
	BYTE	fixed_envelope;
	BYTE	holdnote;
	BYTE	volume;

	BYTE	env_vol;
	INT env_phase;
	INT env_decay;

	INT adder;
	INT duty_flip;
} APU_MMC5_RECTANGLE;

typedef struct {
	// For sync
	BYTE	reg[4];
	BYTE	enable;
	BYTE	holdnote;
	BYTE	dummy[2];
	INT vbl_length;
} SYNCRECTANGLE, *LPSYNCRECTANGLE;

struct	APU_MMC5_struct {
	APU_MMC5_vtbl *vtbl;
	
	APU_MMC5_RECTANGLE	ch0, ch1;
	SYNCRECTANGLE	sch0, sch1;

	BYTE	reg5010;
	BYTE	reg5011;
	BYTE	reg5015;
	INT	cycle_rate;

	INT	FrameCycle;
	BYTE	sync_reg5015;

	FLOAT	cpu_clock;

};

APU_MMC5 * APU_MMC5_new(NES* parent);


#endif	// !__APU_MMC5_INCLUDED__
