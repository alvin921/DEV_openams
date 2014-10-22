//////////////////////////////////////////////////////////////////////////
// Mapper062                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
}Mapper062;

void	Mapper062_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper062_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xFF00 ) {
		case	0x8100:
			SetPROM_8K_Bank( 4, data );
			SetPROM_8K_Bank( 5, data+1 );
			break;
		case	0x8500:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x8700:
			SetPROM_8K_Bank( 5, data );
			break;
	SetVROM_1K_Bank( 0, data+0 );
	SetVROM_1K_Bank( 1, data+1 );
	SetVROM_1K_Bank( 2, data+2 );
	SetVROM_1K_Bank( 3, data+3 );
	SetVROM_1K_Bank( 4, data+4 );
	SetVROM_1K_Bank( 5, data+5 );
	SetVROM_1K_Bank( 6, data+6 );
	SetVROM_1K_Bank( 7, data+7 );
	}
}

Mapper * Mapper062_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper062));
	
	memset(mapper, 0, sizeof(Mapper062));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper062_Reset;
	mapper->vtbl.Write = Mapper062_Write;

	return (Mapper *)mapper;
}

