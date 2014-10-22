
//////////////////////////////////////////////////////////////////////////
// Mapper151 VS-Unisystem                                               //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper151;
void	Mapper151_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

#if	0
	DWORD	crc = ROM_GetPROM_CRC(nes->rom);
	if( crc == 0x1E438D52 ) {
		DirectDraw.SetVsPalette( 7 );	//VS_Goonies
	}
	if( crc == 0xD99A2087 ) {
		DirectDraw.SetVsPalette( 6 );	//VS_Gradius
	}
#endif
}

void	Mapper151_Write( Mapper *mapper,WORD addr, BYTE data )
{
	switch( addr ) {
		case 0x8000:
			SetPROM_8K_Bank( 4,data );
			break;
		case 0xA000:
			SetPROM_8K_Bank( 5,data );
			break;
		case 0xC000:
			SetPROM_8K_Bank( 6,data );
			break;
		case 0xE000:
			SetVROM_4K_Bank( 0, data );
			break;
		case	0xF000:
			SetVROM_4K_Bank( 4, data );
			break;
	}
}

Mapper * Mapper151_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper151));
	
	memset(mapper, 0, sizeof(Mapper151));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper151_Reset;
	mapper->vtbl.Write = Mapper151_Write;

	return (Mapper *)mapper;
}

