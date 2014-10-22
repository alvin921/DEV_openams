
//////////////////////////////////////////////////////////////////////////
// Mapper066  Bandai 74161                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
}Mapper066;
void	Mapper066_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );

//	if( nes->rom->GetPROM_CRC() == 0xe30552db ) {	// Paris-Dakar Rally Special
//		nes->SetFrameIRQmode( FALSE );
//	}
}

void	Mapper066_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr >= 0x6000 ) {
		SetPROM_32K_Bank_cont( (data & 0xF0) >> 4 );
		SetVROM_8K_Bank_cont( data & 0x0F );
	}
}

void	Mapper066_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_32K_Bank_cont( (data & 0xF0) >> 4 );
	SetVROM_8K_Bank_cont( data & 0x0F );
}

Mapper * Mapper066_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper066));
	
	memset(mapper, 0, sizeof(Mapper066));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper066_Reset;
	mapper->vtbl.Write = Mapper066_Write;
	mapper->vtbl.WriteLow = Mapper066_WriteLow;

	return (Mapper *)mapper;
}

