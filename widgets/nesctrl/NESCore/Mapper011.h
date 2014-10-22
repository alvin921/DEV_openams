//////////////////////////////////////////////////////////////////////////
// Mapper011  Color Dreams                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
}Mapper011;

void	Mapper011_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
//		SetVROM_8K_Bank_cont( 1 );
	}
	SetVRAM_Mirror_cont( VRAM_VMIRROR );
}


void	Mapper011_Write( Mapper *mapper, WORD addr, BYTE data )
{
DebugOut( "WR A:%04X D:%02X\n", addr, data );
	SetPROM_32K_Bank_cont( data );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( data>>4 );
	}
}

Mapper * Mapper011_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper011));
	
	memset(mapper, 0, sizeof(Mapper011));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper011_Reset;
	mapper->vtbl.Write = Mapper011_Write;

	return (Mapper *)mapper;
}

