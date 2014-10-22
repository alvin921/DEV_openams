
//////////////////////////////////////////////////////////////////////////
// Mapper242  Wai Xing Zhan Shi                                         //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper242;
void	Mapper242_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
}

void	Mapper242_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr & 0x01 ) {
		SetPROM_32K_Bank_cont( (addr&0xF8)>>3 );
	}
}

Mapper * Mapper242_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper242));
	
	memset(mapper, 0, sizeof(Mapper242));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper242_Reset;
	mapper->vtbl.Write = Mapper242_Write;

	return (Mapper *)mapper;
}

