//////////////////////////////////////////////////////////////////////////
// Mapper008  FFE F3xxx                                                 //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
}Mapper008;

void	Mapper008_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper008_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_16K_Bank( 4, (data&0xF8)>>3 );
	SetVROM_8K_Bank_cont( data&0x07 );
}

Mapper * Mapper008_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper008));
	
	memset(mapper, 0, sizeof(Mapper008));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper008_Reset;
	mapper->vtbl.Write = Mapper008_Write;

	return (Mapper *)mapper;
}

