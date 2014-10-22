
//////////////////////////////////////////////////////////////////////////
// Mapper227  1200-in-1                                                 //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper227;
void	Mapper227_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 0, 1 );
}

void	Mapper227_Write( Mapper *mapper, WORD addr, BYTE data )
{
	BYTE	bank = ((addr&0x0100)>>4)|((addr&0x0078)>>3);

	if( addr & 0x0001 ) {
		SetPROM_32K_Bank_cont( bank );
	} else {
		if( addr & 0x0004 ) {
			SetPROM_8K_Bank( 4, bank*4+2 );
			SetPROM_8K_Bank( 5, bank*4+3 );
			SetPROM_8K_Bank( 6, bank*4+2 );
			SetPROM_8K_Bank( 7, bank*4+3 );
		} else {
			SetPROM_8K_Bank( 4, bank*4+0 );
			SetPROM_8K_Bank( 5, bank*4+1 );
			SetPROM_8K_Bank( 6, bank*4+0 );
			SetPROM_8K_Bank( 7, bank*4+1 );
		}
	}

	if( !(addr & 0x0080) ) {
		if( addr & 0x0200 ) {
			SetPROM_8K_Bank( 6, (bank&0x1C)*4+14 );
			SetPROM_8K_Bank( 7, (bank&0x1C)*4+15 );
		} else {
			SetPROM_8K_Bank( 6, (bank&0x1C)*4+0 );
			SetPROM_8K_Bank( 7, (bank&0x1C)*4+1 );
		}
	}
	if( addr & 0x0002 ) {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	}
}

Mapper * Mapper227_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper227));
	
	memset(mapper, 0, sizeof(Mapper227));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper227_Reset;
	mapper->vtbl.Write = Mapper227_Write;

	return (Mapper *)mapper;
}

