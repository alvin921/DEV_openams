
//////////////////////////////////////////////////////////////////////////
// Mapper071  Camerica                                                  //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper071;

void	Mapper071_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper071_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( (addr&0xE000)==0x6000 ) {
		SetPROM_16K_Bank( 4, data );
	}
}

void	Mapper071_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr&0xF000 ) {
		case	0x9000:
			if( data&0x10 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			else		SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			break;

		case	0xC000:
		case	0xD000:
		case	0xE000:
		case	0xF000:
			SetPROM_16K_Bank( 4, data );
			break;
	}
}
Mapper * Mapper071_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper071));
	
	memset(mapper, 0, sizeof(Mapper071));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper071_Reset;
	mapper->vtbl.Write = Mapper071_Write;
	mapper->vtbl.WriteLow = Mapper071_WriteLow;

	return (Mapper *)mapper;
}


