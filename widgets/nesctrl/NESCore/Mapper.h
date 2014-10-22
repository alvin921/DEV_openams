/*----------------------------------------------------------------------*/
/*                                                                      */
/*      NES Mapper                                                      */
/*                                                           Norix      */
/*                                               written     2001/02/05 */
/*                                               last modify ----/--/-- */
/*----------------------------------------------------------------------*/
#ifndef	__MAPPER_INCLUDED__
#define	__MAPPER_INCLUDED__

#include "macro.h"
#include "DirectSound.h"



// For ExCmdRead command
typedef enum	 {
	EXCMDRD_NONE = 0,
	EXCMDRD_DISKACCESS,
}EXCMDRD;
// For ExCmdWrite command
typedef enum	 {
	EXCMDWR_NONE = 0,
	EXCMDWR_DISKINSERT,
	EXCMDWR_DISKEJECT,
}EXCMDWR;

typedef struct _Mapper Mapper;

typedef struct {
	void	(*Reset)(Mapper*);

	// $8000-$FFFF Memory write
	void	(*Write)( Mapper*, WORD addr, BYTE data );

	// $8000-$FFFF Memory read(Dummy)
	void	(*Read)( Mapper*, WORD addr, BYTE data );

	// $4100-$7FFF Lower Memory read/write
	BYTE	(*ReadLow )( Mapper*, WORD addr );
	void	(*WriteLow)( Mapper*, WORD addr, BYTE data );

	// $4018-$40FF Extention register read/write
	BYTE	(*ExRead )( Mapper*, WORD addr )	;
	void	(*ExWrite)( Mapper*, WORD addr, BYTE data );

	// Extension commands

	BYTE	(*ExCmdRead )( Mapper*, EXCMDRD cmd );
	void	(*ExCmdWrite)( Mapper*, EXCMDWR cmd, BYTE data );

	// H sync/V sync/Clock sync
	void	(*HSync)( Mapper*, INT scanline );
	void	(*VSync)(Mapper*);
	void	(*Clock)( Mapper*, INT cycles );

	// PPU address bus latch
	void	(*PPU_Latch)( Mapper*, WORD addr );

	// PPU Character latch
	void	(*PPU_ChrLatch)( Mapper*, WORD addr );

	// PPU Extension character/palette
	void	(*PPU_ExtLatchX)( Mapper*, INT x );
	void	(*PPU_ExtLatch)( Mapper*, WORD addr, BYTE* chr_l, BYTE* chr_h, BYTE* attr );

	// For State save
	BOOL	(*IsStateSave)(Mapper*);
	void	(*SaveState)( Mapper*, LPBYTE p );
	void	(*LoadState)( Mapper*, LPBYTE p );
}MapperVtbl;

#define INHERIT_MAPPER		MapperVtbl	vtbl; NES * nes
	
struct _Mapper{
	MapperVtbl	vtbl;
	NES * nes;
};

#define VOID_MAPPER_FUNC(mapper, func) \
			do{if((mapper)->vtbl.func)(mapper)->vtbl.func(mapper);}while(0)
#define VOID_MAPPER_FUNC_1(mapper, func, v1) \
			do{if((mapper)->vtbl.func)(mapper)->vtbl.func(mapper, v1);}while(0)
#define VOID_MAPPER_FUNC_2(mapper, func, v1, v2) \
			do{if((mapper)->vtbl.func)(mapper)->vtbl.func(mapper, v1, v2);}while(0)
#define VOID_MAPPER_FUNC_4(mapper, func, v1, v2, v3, v4) \
							do{if((mapper)->vtbl.func)(mapper)->vtbl.func(mapper, v1, v2, v3, v4);}while(0)

#define MAPPER_FUNC(mapper, func, default_value) \
			((mapper)->vtbl.func?(mapper)->vtbl.func(mapper):(default_value))
#define MAPPER_FUNC_1(mapper, func, v1, default_value) \
			((mapper)->vtbl.func?(mapper)->vtbl.func(mapper, v1):(default_value))
#define MAPPER_FUNC_2(mapper, func, v1, v2, default_value) \
			((mapper)->vtbl.func?(mapper)->vtbl.func(mapper, v1, v2):(default_value))


#define Mapper_Reset(mapper)	VOID_MAPPER_FUNC(mapper, Reset)
// $8000-$FFFF Memory write
#define Mapper_Write(mapper, addr, data)	VOID_MAPPER_FUNC_2(mapper, Write, addr, data)
// $8000-$FFFF Memory read(Dummy)
#define Mapper_Read(mapper, addr, data)		VOID_MAPPER_FUNC_2(mapper, Read, addr, data)

// $4100-$7FFF Lower Memory read/write
#define Mapper_ReadLow(mapper, addr )		MAPPER_FUNC_1(mapper, ReadLow, addr, 0)
#define Mapper_WriteLow(mapper, addr, data)	VOID_MAPPER_FUNC_2(mapper, WriteLow, addr, data)

// $4018-$40FF Extention register read/write
#define Mapper_ExRead( mapper, addr )	MAPPER_FUNC_1(mapper, ExRead, addr, 0x00)
#define Mapper_ExWrite( mapper, addr, data ) VOID_MAPPER_FUNC_2(mapper, ExWrite, addr, data)

// Extension commands
#define Mapper_ExCmdRead( mapper, cmd )	MAPPER_FUNC_1(mapper, ExCmdRead, cmd, 0x00)
#define Mapper_ExCmdWrite( mapper, cmd, data ) VOID_MAPPER_FUNC_2(mapper, ExCmdWrite, cmd, data)

// H sync/V sync/Clock sync
#define Mapper_HSync( mapper, scanline )	VOID_MAPPER_FUNC_1(mapper, HSync, scanline)
#define Mapper_VSync(mapper)				VOID_MAPPER_FUNC(mapper, VSync)
#define Mapper_Clock( mapper, cycles )		VOID_MAPPER_FUNC_1(mapper, Clock, cycles)

// PPU address bus latch
#define Mapper_PPU_Latch( mapper, addr )	VOID_MAPPER_FUNC_1(mapper, PPU_Latch, addr)

// PPU Character latch
#define Mapper_PPU_ChrLatch(mapper, addr)	VOID_MAPPER_FUNC_1(mapper, PPU_ChrLatch, addr)

// PPU Extension character/palette
#define Mapper_PPU_ExtLatchX( mapper, x )		VOID_MAPPER_FUNC_1(mapper, PPU_ExtLatchX, x)
#define Mapper_PPU_ExtLatch( mapper, addr, chr_l, chr_h, attr )	VOID_MAPPER_FUNC_4(mapper, PPU_ExtLatch, addr, chr_l, chr_h, attr)

// For State save
#define Mapper_IsStateSave(mapper)		MAPPER_FUNC(mapper, IsStateSave, FALSE)
#define Mapper_SaveState( mapper, p )	VOID_MAPPER_FUNC_1(mapper, SaveState, p)
#define Mapper_LoadState( mapper, p )	VOID_MAPPER_FUNC_1(mapper, LoadState, p)











extern MapperVtbl MapperBaseVtbl;

extern BYTE	Mapper_DefReadLow( Mapper* mapper, WORD addr );
extern void	Mapper_DefWriteLow( Mapper* mapper, WORD addr, BYTE data );

// Create class instance
extern	Mapper*	CreateMapper( NES* parent, INT no );
void Mapper_Destroy( Mapper *mapper );

#endif
