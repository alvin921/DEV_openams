//////////////////////////////////////////////////////////////////////////
// Mapper140                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper140;
void	Mapper140_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper140_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_32K_Bank_cont( (data&0xF0)>>4 );
	SetVROM_8K_Bank_cont( data&0x0F );
}

Mapper * Mapper140_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper140));
	
	memset(mapper, 0, sizeof(Mapper140));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper140_Reset;
	mapper->vtbl.WriteLow = Mapper140_WriteLow;

	return (Mapper *)mapper;
}



