//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Konami VRC7                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_VRC7_INCLUDED__
#define	__APU_VRC7_INCLUDED__


#include "macro.h"

#include "APU_INTERFACE.h"



typedef struct APU_VRC7_struct APU_VRC7;

typedef const struct {
	INHERIT_IAPU_VTBL(APU_VRC7);

}APU_VRC7_vtbl;


#include "emu2413.h"

struct	APU_VRC7_struct{
	APU_VRC7_vtbl *vtbl;
	
	OPLL*	VRC7_OPLL;

	BYTE	address;
};


APU_VRC7 * APU_VRC7_new(NES* parent);


#endif	// !__APU_VRC7_INCLUDED__
