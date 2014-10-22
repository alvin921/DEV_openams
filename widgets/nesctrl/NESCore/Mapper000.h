//////////////////////////////////////////////////////////////////////////
// Mapper000                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper000_Reset(Mapper *mapper)
{
	DWORD	crc;
	switch( PROM_16K_SIZE ) {
		default:
		case	1:	// 16K only
			SetPROM_16K_Bank( 4, 0 );
			SetPROM_16K_Bank( 6, 0 );
			break;
		case	2:	// 32K
			SetPROM_32K_Bank_cont( 0 );
			break;
	}

	crc = ROM_GetPROM_CRC(mapper->nes->rom);
	if( crc == 0x4e7db5af ) {	// Circus Charlie(J)
		NES_SetRenderMethod( mapper->nes, POST_RENDER );
	}
	if( crc == 0x57970078 ) {	// F-1 Race(J)
		NES_SetRenderMethod( mapper->nes, POST_RENDER );
	}
	if( crc == 0xaf2bbcbc		// Mach Rider(JU)
	 || crc == 0x3acd4bf1 ) {	// Mach Rider(Alt)(JU)
		NES_SetRenderMethod( mapper->nes, POST_RENDER );
	}
}

Mapper * Mapper000_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper000_Reset;

	return mapper;
}

