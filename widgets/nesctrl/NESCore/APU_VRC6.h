//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Konami VRC6                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_VRC6_INCLUDED__
#define	__APU_VRC6_INCLUDED__


#include "macro.h"

#include "APU_INTERFACE.h"


typedef struct APU_VRC6_struct APU_VRC6;

typedef const struct {
	INHERIT_IAPU_VTBL(APU_VRC6);

}APU_VRC6_vtbl;

typedef struct {
	BYTE	reg[3];

	BYTE	enable;
	BYTE	gate;
	BYTE	volume;

	INT phaseacc;
	INT freq;
	INT output_vol;

	BYTE	adder;
	BYTE	duty_pos;
} APU_VRC6_RECTANGLE;

typedef struct {
	BYTE	reg[3];

	BYTE	enable;
	BYTE	volume;

	INT phaseacc;
	INT freq;
	INT output_vol;

	BYTE	adder;
	BYTE	accum;
	BYTE	phaseaccum;
} SAWTOOTH, *LPSAWTOOTH;

struct	APU_VRC6_struct {
	APU_VRC6_vtbl *vtbl;

	APU_VRC6_RECTANGLE	ch0, ch1;
	SAWTOOTH	ch2;

	INT	cycle_rate;

	FLOAT	cpu_clock;
};

APU_VRC6 * APU_VRC6_new(NES* parent);


#endif	// !__APU_VRC6_INCLUDED__
