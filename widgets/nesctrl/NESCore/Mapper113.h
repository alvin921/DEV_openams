//////////////////////////////////////////////////////////////////////////
// Mapper113  PC-Sachen/Hacker                                          //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper113;
void	Mapper113_Reset(Mapper *mapper)
{
//	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetPROM_32K_Bank_cont( 0 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper113_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "$%04X:$%02X L=%3d\n", addr, data, mapper->nes->GetScanline() );
	switch( addr ) {
		case	0x4100:
		case	0x4111:
		case	0x4120:
		case	0x4194:
		case	0x4195:
		case	0x4900:
			if( ROM_GetPROM_CRC(mapper->nes->rom) == 0xA75AEDE5 ) { // HES 6-in-1
				if( data & 0x80 ) {
					SetVRAM_Mirror_cont( VRAM_VMIRROR );
				} else {
					SetVRAM_Mirror_cont( VRAM_HMIRROR );
				}
			}
			SetPROM_32K_Bank_cont( data >> 3 );
			SetVROM_8K_Bank_cont( ((data>>3)&0x08)+(data&0x07) );
			break;
	}
}

void	Mapper113_Write( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "$%04X:$%02X L=%3d\n", addr, data, mapper->nes->GetScanline() );
	switch( addr ) {
		case	0x8008:
		case	0x8009:
			SetPROM_32K_Bank_cont( data >> 3 );
			SetVROM_8K_Bank_cont( ((data>>3)&0x08)+(data&0x07) );
			break;
		case	0x8E66:
		case	0x8E67:
			SetVROM_8K_Bank_cont( (data&0x07)?0:1 );
			break;
		case	0xE00A:
			SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			break;
	}
}

Mapper * Mapper113_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper113));
	
	memset(mapper, 0, sizeof(Mapper113));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper113_Reset;
	mapper->vtbl.WriteLow = Mapper113_WriteLow;
	mapper->vtbl.Write = Mapper113_Write;

	return (Mapper *)mapper;
}


