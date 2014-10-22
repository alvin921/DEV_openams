
//////////////////////////////////////////////////////////////////////////
// Mapper180  Nichibutsu                                                //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper180;
void	Mapper180_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper180_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_16K_Bank( 6, data&0x07 );
}

Mapper * Mapper180_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper180));
	
	memset(mapper, 0, sizeof(Mapper180));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper180_Reset;
	mapper->vtbl.Write = Mapper180_Write;

	return (Mapper *)mapper;
}

