
//////////////////////////////////////////////////////////////////////////
// Mapper241  Fon Serm Bon                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper241;
void    Mapper241_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper241_Write(Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x8000 ) {
		SetPROM_32K_Bank_cont( data );
	}
}


Mapper * Mapper241_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper241));
	
	memset(mapper, 0, sizeof(Mapper241));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper241_Reset;
	mapper->vtbl.Write = Mapper241_Write;

	return (Mapper *)mapper;
}

