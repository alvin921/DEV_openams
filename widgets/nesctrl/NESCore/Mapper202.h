
//////////////////////////////////////////////////////////////////////////
// Mapper202  150-in-1                                                  //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper202;
void	Mapper202_WriteSub( Mapper *mapper, WORD addr, BYTE data );
void	Mapper202_Reset(Mapper *mapper)
{
	SetPROM_16K_Bank( 4, 6 );
	SetPROM_16K_Bank( 6, 7 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper202_ExWrite( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr >= 0x4020 ) {
		Mapper202_WriteSub( mapper, addr, data );
	}
}

void	Mapper202_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	Mapper202_WriteSub( mapper, addr, data );
}

void	Mapper202_Write( Mapper *mapper, WORD addr, BYTE data )
{
	Mapper202_WriteSub( mapper, addr, data );
}

void	Mapper202_WriteSub( Mapper *mapper, WORD addr, BYTE data )
{
	INT	bank = (addr>>1) & 0x07;

	SetPROM_16K_Bank( 4, bank );
	if( (addr & 0x0C) == 0x0C ) {
		SetPROM_16K_Bank( 6, bank+1 );
	} else {
		SetPROM_16K_Bank( 6, bank );
	}
	SetVROM_8K_Bank_cont( bank );

	if( addr & 0x01 ) {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	}
}

Mapper * Mapper202_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper202));
	
	memset(mapper, 0, sizeof(Mapper202));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper202_Reset;
	mapper->vtbl.Write = Mapper202_Write;
	mapper->vtbl.ExWrite = Mapper202_ExWrite;
	mapper->vtbl.WriteLow = Mapper202_WriteLow;

	return (Mapper *)mapper;
}

