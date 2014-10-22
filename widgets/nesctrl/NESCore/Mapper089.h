
//////////////////////////////////////////////////////////////////////////
// Mapper089  SunSoft (…ŒË‰©–å)                                        //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper089;
void	Mapper089_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper089_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( (addr&0xFF00) == 0xC000 ) {
		SetPROM_16K_Bank( 4, (data&0x70)>>4 );

		SetVROM_8K_Bank_cont( ((data&0x80)>>4)|(data&0x07) );

		if( data & 0x08 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
		else		  SetVRAM_Mirror_cont( VRAM_MIRROR4L );
	}
}

Mapper * Mapper089_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper089));
	
	memset(mapper, 0, sizeof(Mapper089));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper089_Reset;
	mapper->vtbl.Write = Mapper089_Write;

	return (Mapper *)mapper;
}

