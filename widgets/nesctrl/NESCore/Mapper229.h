
//////////////////////////////////////////////////////////////////////////
// Mapper229  31-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper229;
void	Mapper229_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper229_Write( Mapper *mapper,WORD addr, BYTE data )
{
	if( addr & 0x001E ) {
		BYTE	prg = addr&0x001F;

		SetPROM_8K_Bank( 4, prg*2+0 );
		SetPROM_8K_Bank( 5, prg*2+1 );
		SetPROM_8K_Bank( 6, prg*2+0 );
		SetPROM_8K_Bank( 7, prg*2+1 );

		SetVROM_8K_Bank_cont( addr & 0x0FFF );
	} else {
		SetPROM_32K_Bank_cont( 0 );
		SetVROM_8K_Bank_cont( 0 );
	}

	if( addr & 0x0020 ) {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	}
}

Mapper * Mapper229_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper229));
	
	memset(mapper, 0, sizeof(Mapper229));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper229_Reset;
	mapper->vtbl.Write = Mapper229_Write;

	return (Mapper *)mapper;
}

