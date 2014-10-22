
//////////////////////////////////////////////////////////////////////////
// Mapper233  42-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper233;
void	Mapper233_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );
}

void	Mapper233_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( data & 0x20 ) {
		SetPROM_8K_Bank( 4, (data&0x1F)*2+0 );
		SetPROM_8K_Bank( 5, (data&0x1F)*2+1 );
		SetPROM_8K_Bank( 6, (data&0x1F)*2+0 );
		SetPROM_8K_Bank( 7, (data&0x1F)*2+1 );
	} else {
		BYTE	bank = (data&0x1E)>>1;

		SetPROM_8K_Bank( 4, bank*4+0 );
		SetPROM_8K_Bank( 5, bank*4+1 );
		SetPROM_8K_Bank( 6, bank*4+2 );
		SetPROM_8K_Bank( 7, bank*4+3 );
	}

	if( (data&0xC0) == 0x00 ) {
		SetVRAM_Mirror( 0, 0, 0, 1 );
	} else if( (data&0xC0) == 0x40 ) {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	} else if( (data&0xC0) == 0x80 ) {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_MIRROR4H );
	}
}

Mapper * Mapper233_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper233));
	
	memset(mapper, 0, sizeof(Mapper233));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper233_Reset;
	mapper->vtbl.Write = Mapper233_Write;

	return (Mapper *)mapper;
}

