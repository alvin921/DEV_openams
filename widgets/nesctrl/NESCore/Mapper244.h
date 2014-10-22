
//////////////////////////////////////////////////////////////////////////
// Mapper244                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper244;
void	Mapper244_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
}

void	Mapper244_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr>=0x8065 && addr<=0x80A4 ) {
		SetPROM_32K_Bank_cont( (addr-0x8065)&0x3 );
	}

	if( addr>=0x80A5 && addr<=0x80E4 ) {
		SetVROM_8K_Bank_cont( (addr-0x80A5)&0x7 );
	}
}

Mapper * Mapper244_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper244));
	
	memset(mapper, 0, sizeof(Mapper244));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper244_Reset;
	mapper->vtbl.Write = Mapper244_Write;

	return (Mapper *)mapper;
}

