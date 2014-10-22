//////////////////////////////////////////////////////////////////////////
// Mapper002 UNROM                                                      //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;
}Mapper002;

void	Mapper002_Reset(Mapper *mapper)
{
	DWORD	crc;
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	((Mapper002 *)mapper)->patch = 0;

	crc = ROM_GetPROM_CRC(((Mapper002 *)mapper)->nes->rom);
//	if( crc == 0x322c9b09 ) {	// Metal Gear (Alt)(J)
////		nes->SetFrameIRQmode( FALSE );
//	}
//	if( crc == 0xe7a3867b ) {	// Dragon Quest 2(Alt)(J)
//		nes->SetFrameIRQmode( FALSE );
//	}
////	if( crc == 0x9622fbd9 ) {	// Ballblazer(J)
////		patch = 0;
////	}
	if( crc == 0x8c3d54e8		// Ikari(J)
	 || crc == 0x655efeed		// Ikari Warriors(U)
	 || crc == 0x538218b2 ) {	// Ikari Warriors(E)
		((Mapper002 *)mapper)->patch = 1;
	}

	if( crc == 0xb20c1030 ) {	// Shanghai(J)(original)
		((Mapper002 *)mapper)->patch = 2;
	}
}


void	Mapper002_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( !ROM_IsSAVERAM(((Mapper002 *)mapper)->nes->rom) ) {
		if( addr >= 0x5000 && ((Mapper002 *)mapper)->patch == 1 )
			SetPROM_16K_Bank( 4, data );
	} else {
		Mapper_DefWriteLow( mapper, addr, data );
	}
}

void	Mapper002_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( ((Mapper002 *)mapper)->patch != 2 )
		SetPROM_16K_Bank( 4, data );
	else
		SetPROM_16K_Bank( 4, data>>4 );
}

Mapper * Mapper002_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper002));
	
	memset(mapper, 0, sizeof(Mapper002));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper002_Reset;
	mapper->vtbl.Write = Mapper002_Write;
	mapper->vtbl.WriteLow = Mapper002_WriteLow;

	return (Mapper *)mapper;
}

