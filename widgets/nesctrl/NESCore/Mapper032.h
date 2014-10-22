//////////////////////////////////////////////////////////////////////////
// Mapper032  Irem G101                                                 //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;

	BYTE	reg;
}Mapper032;

void	Mapper032_Reset(Mapper *mapper)
{
	DWORD crc;
	((Mapper032 *)mapper)->patch = 0;
	((Mapper032 *)mapper)->reg = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	crc = ROM_GetPROM_CRC(((Mapper032 *)mapper)->nes->rom);

	// For Major League(J)
	if( crc == 0xc0fed437 ) {
		((Mapper032 *)mapper)->patch = 1;
	}
	// For Ai Sensei no Oshiete - Watashi no Hoshi(J)
	if( crc == 0xfd3fc292 ) {
		SetPROM_32K_Bank( 30, 31, 30, 31 );
	}
}

void	Mapper032_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF000 ) {
		case	0x8000:
			if( ((Mapper032 *)mapper)->reg & 0x02 ) {
				SetPROM_8K_Bank( 6, data );
			} else {
				SetPROM_8K_Bank( 4, data );
			}
			break;

		case	0x9000:
			((Mapper032 *)mapper)->reg = data;
			if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;

		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;
	}

	switch( addr & 0xF007 ) {
		case	0xB000:
		case	0xB001:
		case	0xB002:
		case	0xB003:
		case	0xB004:
		case	0xB005:
			SetVROM_1K_Bank( addr & 0x0007, data );
			break;
		case	0xB006:
			SetVROM_1K_Bank( 6, data );

			if( ((Mapper032 *)mapper)->patch && (data & 0x40) ) {
				SetVRAM_Mirror( 0, 0, 0, 1 );
			}
			break;
		case	0xB007:
			SetVROM_1K_Bank( 7, data );

			if( ((Mapper032 *)mapper)->patch && (data & 0x40) ) {
				SetVRAM_Mirror( 0, 0, 0, 0 );
			}
			break;
	}
}

void	Mapper032_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper032 *)mapper)->reg;
}

void	Mapper032_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper032 *)mapper)->reg = p[0];
}

BOOL	Mapper032_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper032_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper032));
	
	memset(mapper, 0, sizeof(Mapper032));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper032_Reset;
	mapper->vtbl.Write = Mapper032_Write;
	mapper->vtbl.SaveState = Mapper032_SaveState;
	mapper->vtbl.LoadState = Mapper032_LoadState;
	mapper->vtbl.IsStateSave = Mapper032_IsStateSave;

	return (Mapper *)mapper;
}

