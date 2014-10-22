//////////////////////////////////////////////////////////////////////////
// Mapper003 CNROM                                                      //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
}Mapper003;

void	Mapper003_Reset(Mapper *mapper)
{
	DWORD	crc;
	switch( PROM_16K_SIZE ) {
		case	1:	// 16K only
			SetPROM_16K_Bank( 4, 0 );
			SetPROM_16K_Bank( 6, 0 );
			break;
		case	2:	// 32K
			SetPROM_32K_Bank_cont( 0 );
			break;
	}
//	nes->SetRenderMethod( TILE_RENDER );
	crc = ROM_GetPROM_CRC(((Mapper003 *)mapper)->nes->rom);

	if( crc == 0x2b72fe7e ) {	// Ganso Saiyuuki - Super Monkey Dai Bouken(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
		PPU_SetExtNameTableMode( ((Mapper003 *)mapper)->nes->ppu, TRUE );
	}

//	if( crc == 0xE44D95B5 ) {	// ‚Ğ‚İ‚Â‚—
//	}
}

#if	0
void	Mapper003_WriteLow( WORD addr, BYTE data )
{
	if( patch ) {
		Mapper_DefWriteLow( mapper, addr, data );
	} else {
		if( ROM_IsSAVERAM(nes->rom) ) {
			Mapper_DefWriteLow( mapper, addr, data );
		} else {
			if( addr >= 0x4800 ) {
				SetVROM_8K_Bank_cont( data & 0x03 );
			}
		}
	}
}
#endif

void	Mapper003_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetVROM_8K_Bank_cont( data );
}

Mapper * Mapper003_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper003));
	
	memset(mapper, 0, sizeof(Mapper003));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper003_Reset;
	mapper->vtbl.Write = Mapper003_Write;

	return (Mapper *)mapper;
}

