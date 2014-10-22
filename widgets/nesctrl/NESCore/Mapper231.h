
//////////////////////////////////////////////////////////////////////////
// Mapper231  20-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper231;
void	Mapper231_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper231_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr & 0x0020 ) {
		SetPROM_32K_Bank_cont( (BYTE)(addr>>1) );
	} else {
		BYTE	bank = addr & 0x1E;
		SetPROM_8K_Bank( 4, bank*2+0 );
		SetPROM_8K_Bank( 5, bank*2+1 );
		SetPROM_8K_Bank( 6, bank*2+0 );
		SetPROM_8K_Bank( 7, bank*2+1 );
	}

	if( addr & 0x0080 ) {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	}
}

Mapper * Mapper231_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper231));
	
	memset(mapper, 0, sizeof(Mapper231));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper231_Reset;
	mapper->vtbl.Write = Mapper231_Write;

	return (Mapper *)mapper;
}

