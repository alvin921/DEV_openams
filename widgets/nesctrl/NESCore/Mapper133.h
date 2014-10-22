
//////////////////////////////////////////////////////////////////////////
// Mapper133  SACHEN CHEN                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper133;
void	Mapper133_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper133_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x4120 ) {
		SetPROM_32K_Bank_cont( (data&0x04)>>2 );
		SetVROM_8K_Bank_cont( data & 0x03 );
	}
	CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
}

Mapper * Mapper133_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper133));
	
	memset(mapper, 0, sizeof(Mapper133));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper133_Reset;
	mapper->vtbl.WriteLow = Mapper133_WriteLow;

	return (Mapper *)mapper;
}

