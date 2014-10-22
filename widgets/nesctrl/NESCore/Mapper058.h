
//////////////////////////////////////////////////////////////////////////
// Mapper058                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
}Mapper058;
	
void	Mapper058_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 0, 1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper058_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr & 0x40 ) {
		SetPROM_16K_Bank( 4, addr&0x07 );
		SetPROM_16K_Bank( 6, addr&0x07 );
	} else {
		SetPROM_32K_Bank_cont( (addr&0x06)>>1 );
	}

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( (addr&0x38)>>3 );
	}

	if( data & 0x02 ) SetVRAM_Mirror_cont( VRAM_VMIRROR );
	else		  SetVRAM_Mirror_cont( VRAM_HMIRROR );
}

Mapper * Mapper058_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper058));
	
	memset(mapper, 0, sizeof(Mapper058));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper058_Reset;
	mapper->vtbl.Write = Mapper058_Write;

	return (Mapper *)mapper;
}

