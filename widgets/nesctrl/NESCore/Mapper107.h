
//////////////////////////////////////////////////////////////////////////
// Mapper107  Magic Dragon Mapper                                       //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper107;
void	Mapper107_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper107_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_32K_Bank_cont( (data>>1)&0x03 );
	SetVROM_8K_Bank_cont( data&0x07 );
}

Mapper * Mapper107_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper107));
	
	memset(mapper, 0, sizeof(Mapper107));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper107_Reset;
	mapper->vtbl.Write = Mapper107_Write;

	return (Mapper *)mapper;
}

