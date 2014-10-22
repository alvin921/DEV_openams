
//////////////////////////////////////////////////////////////////////////
// Mapper181  Hacker International Type2                                //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper181;
void	Mapper181_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper181_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "$%04X:$%02X\n", addr, data );
	if( addr == 0x4120 ) {
		SetPROM_32K_Bank_cont( (data & 0x08) >> 3 );
		SetVROM_8K_Bank_cont( data & 0x07 );
	}
}

Mapper * Mapper181_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper181));
	
	memset(mapper, 0, sizeof(Mapper181));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper181_Reset;
	mapper->vtbl.WriteLow = Mapper181_WriteLow;

	return (Mapper *)mapper;
}

