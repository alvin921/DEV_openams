
//////////////////////////////////////////////////////////////////////////
// Mapper077  Irem Early Mapper #0                                      //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper077;
void	Mapper077_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );

	SetVROM_2K_Bank( 0, 0 );
	SetCRAM_2K_Bank( 2, 1 );
	SetCRAM_2K_Bank( 4, 2 );
	SetCRAM_2K_Bank( 6, 3 );
}
void	Mapper077_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_32K_Bank_cont( data & 0x07 );

	SetVROM_2K_Bank( 0, (data & 0xF0)>>4 );
}


Mapper * Mapper077_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper077));
	
	memset(mapper, 0, sizeof(Mapper077));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper077_Reset;
	mapper->vtbl.Write = Mapper077_Write;

	return (Mapper *)mapper;
}

