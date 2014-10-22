
//////////////////////////////////////////////////////////////////////////
// Mapper087  Konami 74161/32                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper087;
void	Mapper087_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper087_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x6000 ) {
		SetVROM_8K_Bank_cont( (data & 0x02)>>1 );
	}
}

Mapper * Mapper087_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper087));
	
	memset(mapper, 0, sizeof(Mapper087));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper087_Reset;
	mapper->vtbl.WriteLow = Mapper087_WriteLow;

	return (Mapper *)mapper;
}

