
//////////////////////////////////////////////////////////////////////////
// Mapper070  Bandai 74161                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;
}Mapper070;


void	Mapper070_Reset(Mapper *mapper)
{
	DWORD crc;
	((Mapper070 *)mapper)->patch = 0;

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	if( crc == 0xa59ca2ef ) {	// Kamen Rider Club(J)
		((Mapper070 *)mapper)->patch = 1;
		NES_SetRenderMethod( mapper->nes, POST_ALL_RENDER );
	}
	if( crc == 0x10bb8f9a ) {	// Family Trainer - Manhattan Police(J)
		((Mapper070 *)mapper)->patch = 1;
	}
	if( crc == 0x0cd00488 ) {	// Space Shadow(J)
		((Mapper070 *)mapper)->patch = 1;
	}

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper070_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_16K_Bank( 4, (data & 0x70)>>4 );
	SetVROM_8K_Bank_cont( data & 0x0F );

	if( ((Mapper070 *)mapper)->patch ) {
		if( data & 0x80 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
		else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
	} else {
		if( data & 0x80 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
		else		  SetVRAM_Mirror_cont( VRAM_MIRROR4L );
	}
}
Mapper * Mapper070_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper070));	 
	memset(mapper, 0, sizeof(Mapper070));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper070_Reset;
	mapper->vtbl.Write = Mapper070_Write;

	return (Mapper *)mapper;
}

