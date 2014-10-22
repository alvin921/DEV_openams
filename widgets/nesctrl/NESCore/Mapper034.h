
//////////////////////////////////////////////////////////////////////////
// Mapper034  Nina-1                                                    //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
}Mapper034;
	
void	Mapper034_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper034_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x7FFD:
			SetPROM_32K_Bank_cont( data );
			break;
		case	0x7FFE:
			SetVROM_4K_Bank( 0, data );
			break;
		case	0x7FFF:
			SetVROM_4K_Bank( 4, data );
			break;
	}
}

void	Mapper034_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_32K_Bank_cont( data );
}

Mapper * Mapper034_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper034));
	
	memset(mapper, 0, sizeof(Mapper034));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper034_Reset;
	mapper->vtbl.WriteLow = Mapper034_WriteLow;
	mapper->vtbl.Write = Mapper034_Write;

	return (Mapper *)mapper;
}

