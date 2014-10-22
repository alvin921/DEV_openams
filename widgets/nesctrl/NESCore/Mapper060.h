
//////////////////////////////////////////////////////////////////////////
// Mapper060                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;
	BYTE	game_sel;
}Mapper060;

void	Mapper060_Reset(Mapper *mapper)
{
	DWORD crc;
	((Mapper060 *)mapper)->patch = 0; 
	crc = ROM_GetPROM_CRC(((Mapper060 *)mapper)->nes->rom);
	if( crc == 0xf9c484a0 ) {	// Reset Based 4-in-1(Unl)
		SetPROM_16K_Bank( 4, ((Mapper060 *)mapper)->game_sel );
		SetPROM_16K_Bank( 6, ((Mapper060 *)mapper)->game_sel );
		SetVROM_8K_Bank_cont( ((Mapper060 *)mapper)->game_sel );
		((Mapper060 *)mapper)->game_sel++;
		((Mapper060 *)mapper)->game_sel &= 3;
	} else {
		((Mapper060 *)mapper)->patch = 1;
		SetPROM_32K_Bank_cont( 0 );
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper060_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( ((Mapper060 *)mapper)->patch ) {
		if( addr & 0x80 ) {
			SetPROM_16K_Bank( 4, (addr&0x70)>>4 );
			SetPROM_16K_Bank( 6, (addr&0x70)>>4 );
		} else {
			SetPROM_32K_Bank_cont( (addr&0x70)>>5 );
		}

		SetVROM_8K_Bank_cont( addr&0x07 );

		if( data & 0x08 ) SetVRAM_Mirror_cont( VRAM_VMIRROR );
		else		  SetVRAM_Mirror_cont( VRAM_HMIRROR );
	}
}

Mapper * Mapper060_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper060));
	
	memset(mapper, 0, sizeof(Mapper060));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper060_Reset;
	mapper->vtbl.Write = Mapper060_Write;

	return (Mapper *)mapper;
}

