
//////////////////////////////////////////////////////////////////////////
// Mapper092  Jaleco/Type1 Higher bank switch                           //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper092;
void	Mapper092_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper092_Write( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "A:%04X D:%02X\n", addr, data );

	data = addr & 0xFF;

	if( addr >= 0x9000 ) {
		if( (data & 0xF0) == 0xD0 ) {
			SetPROM_16K_Bank( 6, data & 0x0F );
		} else if( (data & 0xF0) == 0xE0 ) {
			SetVROM_8K_Bank_cont( data & 0x0F );
		}
	} else {
		if( (data & 0xF0) == 0xB0 ) {
			SetPROM_16K_Bank( 6, data & 0x0F );
		} else if( (data & 0xF0) == 0x70 ) {
			SetVROM_8K_Bank_cont( data & 0x0F );
		} else if( (data & 0xF0) == 0xC0 ) {
			INT	tbl[] = { 3, 4, 5, 6, 0, 1, 2, 7,
					  9,10, 8,11,13,12,14,15 };

		}
	}
}

Mapper * Mapper092_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper092));
	
	memset(mapper, 0, sizeof(Mapper092));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper092_Reset;
	mapper->vtbl.Write = Mapper092_Write;

	return (Mapper *)mapper;
}

