
//////////////////////////////////////////////////////////////////////////
// Mapper094  Capcom 74161/32                                           //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper094;
void	Mapper094_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper094_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( (addr&0xFFF0) == 0xFF00 ) {
		SetPROM_16K_Bank( 4, (data>>2)&0x7 );
	}
}

Mapper * Mapper094_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper094));
	
	memset(mapper, 0, sizeof(Mapper094));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper094_Reset;
	mapper->vtbl.Write = Mapper094_Write;

	return (Mapper *)mapper;
}

