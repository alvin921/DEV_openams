/*----------------------------------------------------------------------*/
/*                                                                      */
/*      NES Mapeers                                                     */
/*                                                           Norix      */
/*                                               written     2000/02/05 */
/*                                               last modify ----/--/-- */
/*----------------------------------------------------------------------*/
/*--------------[ INCLUDE               ]-------------------------------*/
#include "mapper.h"
#include "nes.h"
#include "mmu.h"





// $4100-$7FFF Lower Memory read
BYTE	Mapper_DefReadLow( Mapper* mapper, WORD addr )
{
	// $6000-$7FFF WRAM
	if( addr >= 0x6000 && addr <= 0x7FFF ) {
		return	CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	}

	return	(BYTE)(addr>>8);
}

// $4100-$7FFF Lower Memory write
void	Mapper_DefWriteLow( Mapper* mapper, WORD addr, BYTE data )
{
	// $6000-$7FFF WRAM
	if( addr >= 0x6000 && addr <= 0x7FFF ) {
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}
}

MapperVtbl MapperBaseVtbl = {
	NULL				// void	(*Reset)(Mapper*);
	, NULL				// void	(*Write)( Mapper*, WORD addr, BYTE data );
	, NULL				// void	(*Read)( Mapper*, WORD addr, BYTE data );
	, Mapper_DefReadLow			// BYTE	(*ReadLow )( Mapper*, WORD addr );
	, Mapper_DefWriteLow			// void	(*WriteLow)( Mapper*, WORD addr, BYTE data );
	, NULL				// BYTE	(*ExRead )( Mapper*, WORD addr )	;
	, NULL				// void	(*ExWrite)( Mapper*, WORD addr, BYTE data );
	, NULL				// BYTE	(*ExCmdRead )( Mapper*, EXCMDRD cmd );
	, NULL				// void	(*ExCmdWrite)( Mapper*, EXCMDWR cmd, BYTE data );
	, NULL				// void	(*HSync)( Mapper*, INT scanline );
	, NULL				// void	(*VSync)(Mapper*);
	, NULL				// void	(*Clock)( Mapper*, INT cycles );
	, NULL				// void	(*PPU_Latch)( Mapper*, WORD addr );
	, NULL				// void	(*PPU_ChrLatch)( Mapper*, WORD addr );
	, NULL				// void	(*PPU_ExtLatchX)( Mapper*, INT x );
	, NULL				// void	(*PPU_ExtLatch)( Mapper*, WORD addr, BYTE& chr_l, BYTE& chr_h, BYTE& attr );
	, NULL				// BOOL	(*IsStateSave)(Mapper*);
	, NULL				// void	(*SaveState)( Mapper*, LPBYTE p );
	, NULL				// void	(*LoadState)( Mapper*, LPBYTE p );
};

void Mapper_Destroy( Mapper *mapper )
{
	if(mapper)
		FREE(mapper);
}
