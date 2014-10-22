
//////////////////////////////////////////////////////////////////////////
// Mapper200  1200-in-1                                                 //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper200;
void	Mapper200_Reset(Mapper *mapper)
{
//	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetPROM_16K_Bank( 4, 0 );
	SetPROM_16K_Bank( 6, 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper200_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_16K_Bank( 4, addr & 0x07 );
	SetPROM_16K_Bank( 6, addr & 0x07 );
	SetVROM_8K_Bank_cont( addr & 0x07 );

	if( addr & 0x01 ) {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	}
}

Mapper * Mapper200_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper200));
	
	memset(mapper, 0, sizeof(Mapper200));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper200_Reset;
	mapper->vtbl.Write = Mapper200_Write;

	return (Mapper *)mapper;
}

