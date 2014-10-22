
//////////////////////////////////////////////////////////////////////////
// Mapper078  Jaleco(Cosmo Carrier)                                     //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper078;
void	Mapper078_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper078_Write( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "MAP78 WR $%04X=$%02X L=%d\n", addr, data, nes->GetScanline() );
	SetPROM_16K_Bank( 4, data&0x0F );
	SetVROM_8K_Bank_cont( (data&0xF0)>>4 );

	if( (addr & 0xFE00) != 0xFE00 ) {
		if( data & 0x08 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
		else		  SetVRAM_Mirror_cont( VRAM_MIRROR4L );
	}
}

Mapper * Mapper078_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper078));
	
	memset(mapper, 0, sizeof(Mapper078));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper078_Reset;
	mapper->vtbl.Write = Mapper078_Write;

	return (Mapper *)mapper;
}

