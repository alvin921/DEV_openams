
//////////////////////////////////////////////////////////////////////////
// Mapper072  Jaleco/Type1 lower bank switch                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper072;

void	Mapper072_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper072_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( data & 0x80 ) {
		SetPROM_16K_Bank( 4, data&0x0F );
	} else if( data & 0x40 ) {
		SetVROM_8K_Bank_cont( data&0x0F );
	} else {
		if( addr >= 0xC100 && addr <= 0xC11F && data == 0x20 ) {
//DebugOut( "ADDR:%04X DATA:%02X\n", addr, data );
DebugOut( "SOUND CODE:%02X\n", addr & 0x1F );
		}
	}
}

Mapper * Mapper072_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper072)); 
	memset(mapper, 0, sizeof(Mapper072));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper072_Reset;
	mapper->vtbl.Write = Mapper072_Write;

	return (Mapper *)mapper;
}

