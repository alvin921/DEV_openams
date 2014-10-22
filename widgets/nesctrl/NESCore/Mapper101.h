//////////////////////////////////////////////////////////////////////////
// Mapper101  Jaleco(Urusei Yatsura)                                    //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper101;

void	Mapper101_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper101_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr >= 0x6000 ) {
		SetVROM_8K_Bank_cont( data&0x03 );
	}
}

void	Mapper101_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetVROM_8K_Bank_cont( data&0x03 );
}

Mapper * Mapper101_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper101));
	
	memset(mapper, 0, sizeof(Mapper101));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper101_Reset;
	mapper->vtbl.WriteLow = Mapper101_WriteLow;
	mapper->vtbl.Write = Mapper101_Write;

	return (Mapper *)mapper;
}

