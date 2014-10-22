
//////////////////////////////////////////////////////////////////////////
// Mapper061                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
}Mapper061;
void	Mapper061_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper061_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0x30 ) {
		case	0x00:
		case	0x30:
			SetPROM_32K_Bank_cont( addr & 0x0F );
			break;
		case	0x10:
		case	0x20:
			SetPROM_16K_Bank( 4, ((addr & 0x0F)<<1)|((addr&0x20)>>4) );
			SetPROM_16K_Bank( 6, ((addr & 0x0F)<<1)|((addr&0x20)>>4) );
			break;
	}

	if( addr & 0x80 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
	else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
}

Mapper * Mapper061_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper061));
	
	memset(mapper, 0, sizeof(Mapper061));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper061_Reset;
	mapper->vtbl.Write = Mapper061_Write;

	return (Mapper *)mapper;
}

