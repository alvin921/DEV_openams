
//////////////////////////////////////////////////////////////////////////
// Mapper185  Character disable protect                                 //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;
}Mapper185;

void	Mapper185_Reset(Mapper *mapper)
{
	INT i;
	DWORD crc;

	switch( PROM_16K_SIZE ) {
		case	1:	// 16K only
			SetPROM_16K_Bank( 4, 0 );
			SetPROM_16K_Bank( 6, 0 );
			break;
		case	2:	// 32K
			SetPROM_32K_Bank_cont( 0 );
			break;
	}

	for( i = 0; i < 0x400; i++ ) {
		VRAM[0x800+i] = 0xFF;
	}

	((Mapper185 *)mapper)->patch = 0;

	crc = ROM_GetPROM_CRC(mapper->nes->rom);
	if( crc == 0xb36457c7 ) {	// Spy vs Spy(J)
		((Mapper185 *)mapper)->patch = 1;
	}
}

void	Mapper185_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( (!((Mapper185 *)mapper)->patch && (data&0x03)) || (((Mapper185 *)mapper)->patch && data == 0x21) ) {
		SetVROM_8K_Bank_cont( 0 );
	} else {
		SetVRAM_1K_Bank( 0, 2 );	// use vram bank 2
		SetVRAM_1K_Bank( 1, 2 );
		SetVRAM_1K_Bank( 2, 2 );
		SetVRAM_1K_Bank( 3, 2 );
		SetVRAM_1K_Bank( 4, 2 );
		SetVRAM_1K_Bank( 5, 2 );
		SetVRAM_1K_Bank( 6, 2 );
		SetVRAM_1K_Bank( 7, 2 );
	}
}

Mapper * Mapper185_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper185));
	
	memset(mapper, 0, sizeof(Mapper185));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper185_Reset;
	mapper->vtbl.Write = Mapper185_Write;

	return (Mapper *)mapper;
}

