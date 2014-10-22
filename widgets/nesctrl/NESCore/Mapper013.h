//////////////////////////////////////////////////////////////////////////
// Mapper013  CPROM                                                     //
//////////////////////////////////////////////////////////////////////////
	
typedef struct {
	INHERIT_MAPPER;
}Mapper013;

void	Mapper013_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );
	SetCRAM_4K_Bank( 0, 0 );
	SetCRAM_4K_Bank( 4, 0 );
}

void	Mapper013_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_32K_Bank_cont( (data&0x30)>>4 );
	SetCRAM_4K_Bank( 4, data&0x03 );
}

Mapper * Mapper013_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper013));
	
	memset(mapper, 0, sizeof(Mapper013));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper013_Reset;
	mapper->vtbl.Write = Mapper013_Write;

	return (Mapper *)mapper;
}

