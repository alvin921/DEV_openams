
//////////////////////////////////////////////////////////////////////////
// Mapper097  Irem 74161                                                //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper097;
void	Mapper097_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( PROM_8K_SIZE-2, PROM_8K_SIZE-1, 0, 1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper097_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr < 0xC000 ) {
		SetPROM_16K_Bank( 6, data & 0x0F );

		if( data & 0x80 ) SetVRAM_Mirror_cont( VRAM_VMIRROR );
		else		  SetVRAM_Mirror_cont( VRAM_HMIRROR );
	}
}

Mapper * Mapper097_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper097));
	
	memset(mapper, 0, sizeof(Mapper097));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper097_Reset;
	mapper->vtbl.Write = Mapper097_Write;

	return (Mapper *)mapper;
}

