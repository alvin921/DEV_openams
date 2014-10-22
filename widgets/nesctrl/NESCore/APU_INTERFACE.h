//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      APU Interface class                                             //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_INTERFACE_INCLUDED__
#define	__APU_INTERFACE_INCLUDED__


#include "macro.h"

#define	APU_CLOCK		1789772.5f

// Fixed point decimal macro
#define	INT2FIX(x)	((x)<<16)
#define	FIX2INT(x)	((x)>>16)






typedef struct _IAPU IAPU;

#define INHERIT_IAPU_VTBL(type)	\
	void	(*Destroy)( type *pme ); \
	void	(*Reset)( type *pme, FLOAT fClock, INT nRate ); \
	void	(*Setup)( type *pme, FLOAT fClock, INT nRate ); \
	INT 	(*Process)( type *pme, INT channel ) ; \
	void	(*Write)( type *pme, WORD addr, BYTE data ); \
	BYTE	(*Read)( type *pme, WORD addr ) ; \
	void	(*SyncWrite)( type *pme, WORD addr, BYTE data ) ; \
	BYTE	(*SyncRead)( type *pme, WORD addr ) ; \
	void	(*VSync)(type *pme ) ; \
	BOOL	(*Sync)( type *pme, INT cycles ) ; \
	INT 	(*GetFreq)( type *pme, INT channel ) ; \
	INT 	(*GetStateSize)(type *pme) ; \
	void	(*SaveState)( type *pme, LPBYTE p ) ; \
	void	(*LoadState)( type *pme, LPBYTE p ) 

#if 0
typedef struct {
	INHERIT_IAPU_VTBL(;
}IAPUVtbl;

#define INHERIT_IAPU		IAPUVtbl	vtbl
	
struct _IAPU{
	IAPUVtbl	vtbl;
};
#endif

#define VOID_IAPU_FUNC(iapu, func) \
			do{if((iapu)->vtbl->func)(iapu)->vtbl->func(iapu);}while(0)
#define VOID_IAPU_FUNC_1(iapu, func, v1) \
			do{if((iapu)->vtbl->func)(iapu)->vtbl->func(iapu, v1);}while(0)
#define VOID_IAPU_FUNC_2(iapu, func, v1, v2) \
			do{if((iapu)->vtbl->func)(iapu)->vtbl->func(iapu, v1, v2);}while(0)

#define IAPU_FUNC(iapu, func, default_value) \
			((iapu)->vtbl->func?(iapu)->vtbl->func(iapu):(default_value))
#define IAPU_FUNC_1(iapu, func, v1, default_value) \
			((iapu)->vtbl->func?(iapu)->vtbl->func(iapu, v1):(default_value))
#define IAPU_FUNC_2(iapu, func, v1, v2, default_value) \
			((iapu)->vtbl->func?(iapu)->vtbl->func(iapu, v1, v2):(default_value))


#define IAPU_Destroy( pme )	\
	do{if((pme)->vtbl->Destroy)(pme)->vtbl->Destroy(pme); FREE(pme);}while(0)

#define IAPU_Reset( pme, fClock, nRate )	VOID_IAPU_FUNC_2(pme, Reset, fClock, nRate)
#define IAPU_Setup( pme, fClock, nRate )	VOID_IAPU_FUNC_2(pme, Setup, fClock, nRate)
#define IAPU_Write( pme, addr, data )		VOID_IAPU_FUNC_2(pme, Write, addr, data)
#define IAPU_Process( pme, channel )		IAPU_FUNC_1(pme, Process, channel, 0)

// ƒIƒvƒVƒ‡ƒ“‚ÅŽÀ‘•‚·‚é
#define IAPU_Read( pme, addr )			IAPU_FUNC_1(pme, Read, addr, (BYTE)(addr>>8))

#define IAPU_SyncWrite( pme, addr, data )	VOID_IAPU_FUNC_2(pme, SyncWrite, addr, data)
#define IAPU_SyncRead( pme, addr )		IAPU_FUNC_1(pme, SyncRead, addr, 0)
#define IAPU_VSync(pme)					VOID_IAPU_FUNC(pme, VSync)
#define IAPU_Sync( pme, cycles )			IAPU_FUNC_1(pme, Sync, cycles, FALSE)

#define IAPU_GetFreq( pme, channel )		IAPU_FUNC_1(pme, GetFreq, channel, 0)

// For State save
#define IAPU_GetStateSize(pme) 			IAPU_FUNC(pme, GetStateSize, 0)
#define IAPU_SaveState( pme, p )			VOID_IAPU_FUNC_1(pme, SaveState, p)
#define IAPU_LoadState( pme, p )			VOID_IAPU_FUNC_1(pme, LoadState, p)



#endif	// !__APU_INTERFACE_INCLUDED__

