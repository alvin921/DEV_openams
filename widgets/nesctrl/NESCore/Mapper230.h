
//////////////////////////////////////////////////////////////////////////
// Mapper230  22-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	rom_sw;
}Mapper230;

void	Mapper230_Reset(Mapper *mapper)
{
	if( ((Mapper230 *)mapper)->rom_sw ) {
		((Mapper230 *)mapper)->rom_sw = 0;
	} else {
		((Mapper230 *)mapper)->rom_sw = 1;
	}
	if( ((Mapper230 *)mapper)->rom_sw ) {
		SetPROM_32K_Bank( 0, 1, 14, 15 );
	} else {
		SetPROM_32K_Bank( 16, 17, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper230_Write( Mapper *mapper,WORD addr, BYTE data )
{
	if( ((Mapper230 *)mapper)->rom_sw ) {
		SetPROM_8K_Bank( 4, (data&0x07)*2+0 );
		SetPROM_8K_Bank( 5, (data&0x07)*2+1 );
	} else {
		if( data & 0x20 ) {
			SetPROM_8K_Bank( 4, (data&0x1F)*2+16 );
			SetPROM_8K_Bank( 5, (data&0x1F)*2+17 );
			SetPROM_8K_Bank( 6, (data&0x1F)*2+16 );
			SetPROM_8K_Bank( 7, (data&0x1F)*2+17 );
		} else {
			SetPROM_8K_Bank( 4, (data&0x1E)*2+16 );
			SetPROM_8K_Bank( 5, (data&0x1E)*2+17 );
			SetPROM_8K_Bank( 6, (data&0x1E)*2+18 );
			SetPROM_8K_Bank( 7, (data&0x1E)*2+19 );
		}
		if( data & 0x40 ) {
			SetVRAM_Mirror_cont( VRAM_VMIRROR );
		} else {
			SetVRAM_Mirror_cont( VRAM_HMIRROR );
		}
	}
}

Mapper * Mapper230_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper230));
	
	memset(mapper, 0, sizeof(Mapper230));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper230_Reset;
	mapper->vtbl.Write = Mapper230_Write;

	return (Mapper *)mapper;
}

