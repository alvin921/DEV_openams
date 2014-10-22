
//////////////////////////////////////////////////////////////////////////
// Mapper222                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper222;
void	Mapper222_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
	SetVRAM_Mirror_cont( VRAM_VMIRROR );
}

void	Mapper222_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF003 ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0xB000:
			SetVROM_1K_Bank( 0, data );
			break;
		case	0xB002:
			SetVROM_1K_Bank( 1, data );
			break;
		case	0xC000:
			SetVROM_1K_Bank( 2, data );
			break;
		case	0xC002:
			SetVROM_1K_Bank( 3, data );
			break;
		case	0xD000:
			SetVROM_1K_Bank( 4, data );
			break;
		case	0xD002:
			SetVROM_1K_Bank( 5, data );
			break;
		case	0xE000:
			SetVROM_1K_Bank( 6, data );
			break;
		case	0xE002:
			SetVROM_1K_Bank( 7, data );
			break;
	}
}

Mapper * Mapper222_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper222));
	
	memset(mapper, 0, sizeof(Mapper222));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper222_Reset;
	mapper->vtbl.Write = Mapper222_Write;

	return (Mapper *)mapper;
}


