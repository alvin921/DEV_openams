
//////////////////////////////////////////////////////////////////////////
// Mapper201  21-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper201;
void	Mapper201_Reset(Mapper *mapper)
{
//	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetPROM_16K_Bank( 4, 0 );
	SetPROM_16K_Bank( 6, 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper201_Write( Mapper *mapper, WORD addr, BYTE data )
{
	BYTE	bank = (BYTE)addr & 0x03;
	if( !(addr&0x08) )
		bank = 0;
	SetPROM_32K_Bank_cont( bank );
	SetVROM_8K_Bank_cont( bank );
}

Mapper * Mapper201_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper201));
	
	memset(mapper, 0, sizeof(Mapper201));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper201_Reset;
	mapper->vtbl.Write = Mapper201_Write;

	return (Mapper *)mapper;
}

