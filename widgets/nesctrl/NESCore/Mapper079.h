
//////////////////////////////////////////////////////////////////////////
// Mapper079  Nina-3                                                    //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper079;
void	Mapper079_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper079_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr&0x0100 ) {
		SetPROM_32K_Bank_cont( (data>>3)&0x01 );
		SetVROM_8K_Bank_cont( data&0x07 );
	}
}

Mapper * Mapper079_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper079));
	
	memset(mapper, 0, sizeof(Mapper079));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper079_Reset;
	mapper->vtbl.WriteLow = Mapper079_WriteLow;

	return (Mapper *)mapper;
}

