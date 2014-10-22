
//////////////////////////////////////////////////////////////////////////
// Mapper108                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper108;
void	Mapper108_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0xC,0xD,0xE,0xF );
	SetPROM_8K_Bank( 3, 0 );
}

void	Mapper108_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_8K_Bank( 3, data );
}

Mapper * Mapper108_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper108));
	
	memset(mapper, 0, sizeof(Mapper108));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper108_Reset;
	mapper->vtbl.Write = Mapper108_Write;

	return (Mapper *)mapper;
}

