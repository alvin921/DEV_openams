
//////////////////////////////////////////////////////////////////////////
// Mapper093  SunSoft (Fantasy Zone)                                    //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper093;
void	Mapper093_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper093_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x6000 ) {
		SetPROM_16K_Bank( 4, data );
	}
}

Mapper * Mapper093_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper093));
	
	memset(mapper, 0, sizeof(Mapper093));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper093_Reset;
	mapper->vtbl.WriteLow = Mapper093_WriteLow;

	return (Mapper *)mapper;
}

