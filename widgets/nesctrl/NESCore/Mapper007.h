//////////////////////////////////////////////////////////////////////////
// Mapper007 AOROM/AMROM                                                //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;
}Mapper007;


void	Mapper007_Reset(Mapper *mapper)
{
	DWORD	crc;
	((Mapper007 *)mapper)->patch = 0;

	SetPROM_32K_Bank_cont( 0 );
	SetVRAM_Mirror_cont( VRAM_MIRROR4L );

	crc = ROM_GetPROM_CRC(mapper->nes->rom);
	if( crc == 0x3c9fe649 ) {	// WWF Wrestlemania Challenge(U)
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
		((Mapper007 *)mapper)->patch = 1;
	}
	if( crc == 0x09874777 ) {	// Marble Madness(U)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}

	if( crc == 0x279710DC		// Battletoads (U)
	 || crc == 0xCEB65B06 ) {	// Battletoads Double Dragon (U)
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
		memset( WRAM, 0, sizeof(WRAM) );
	}
}

void	Mapper007_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_32K_Bank_cont( data & 0x07 );

	if( !((Mapper007 *)mapper)->patch ) {
		if( data & 0x10 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
		else		  SetVRAM_Mirror_cont( VRAM_MIRROR4L );
	}
}

Mapper * Mapper007_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper007));
	
	memset(mapper, 0, sizeof(Mapper007));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper007_Reset;
	mapper->vtbl.Write = Mapper007_Write;

	return (Mapper *)mapper;
}

