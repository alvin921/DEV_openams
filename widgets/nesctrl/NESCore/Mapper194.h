
//////////////////////////////////////////////////////////////////////////
// Mapper194 –À‹{›‰@ƒ_ƒoƒo                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper194;

void	Mapper194_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( PROM_32K_SIZE-1 );
}

void	Mapper194_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_8K_Bank( 3, data );
}

Mapper * Mapper194_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper194));
	
	memset(mapper, 0, sizeof(Mapper194));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper194_Reset;
	mapper->vtbl.Write = Mapper194_Write;

	return (Mapper *)mapper;
}

