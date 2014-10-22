//////////////////////////////////////////////////////////////////////////
// Mapper122/184  SunSoft-1                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper122;

void	Mapper122_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );
}

void	Mapper122_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x6000 ) {
		SetVROM_4K_Bank( 0,  data & 0x07 );
		SetVROM_4K_Bank( 4, (data & 0x70)>>4 );
	}
}

Mapper * Mapper122_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper122));
	
	memset(mapper, 0, sizeof(Mapper122));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper122_Reset;
	mapper->vtbl.WriteLow = Mapper122_WriteLow;

	return (Mapper *)mapper;
}

