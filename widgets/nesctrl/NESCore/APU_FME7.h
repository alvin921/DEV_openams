//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      SunSoft FME7                                                    //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_FME7_INCLUDED__
#define	__APU_FME7_INCLUDED__


#include "macro.h"

#include "APU_INTERFACE.h"

typedef struct APU_FME7_struct APU_FME7;

typedef const struct {
	INHERIT_IAPU_VTBL(APU_FME7);

}APU_FME7_vtbl;


typedef struct {
	BYTE	reg[3];
	BYTE	volume;

	INT freq;
	INT phaseacc;
	INT envadr;

	LPBYTE	envtbl;
	LPSBYTE envstep;
} ENVELOPE, *LPENVELOPE;

typedef struct {
	INT freq;
	INT phaseacc;
	INT noiserange;
	BYTE	noiseout;
} APU_FME7_NOISE;

typedef struct {
	BYTE	reg[3];
	BYTE	enable;
	BYTE	env_on;
	BYTE	noise_on;
	BYTE	adder;
	BYTE	volume;

	INT freq;
	INT phaseacc;

	INT output_vol;
} APU_FME7_CHANNEL;

struct	APU_FME7_struct{
	APU_FME7_vtbl *vtbl;
	
	ENVELOPE envelope;
	APU_FME7_NOISE	noise;
	APU_FME7_CHANNEL	op[3];

	BYTE	address;

	INT	vol_table[0x20];
	INT	cycle_rate;

	FLOAT	cpu_clock;
};

APU_FME7 * APU_FME7_new(NES* parent);



#endif	// !__APU_FME7_INCLUDED__
