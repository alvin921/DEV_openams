
//////////////////////////////////////////////////////////////////////////
// Mapper051  11-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	int     mode, bank;
}Mapper051;

void	Mapper051_SetBank_CPU(Mapper *mapper);
void	Mapper051_Reset(Mapper *mapper)
{
	((Mapper051 *)mapper)->bank = 0;
	((Mapper051 *)mapper)->mode = 1;

	Mapper051_SetBank_CPU(mapper);

	SetCRAM_8K_Bank( 0 );
}

void	Mapper051_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr>=0x6000 ) {
		((Mapper051 *)mapper)->mode = ((data & 0x10) >> 3) | ((data & 0x02) >> 1);
		Mapper051_SetBank_CPU(mapper);
	}
}

void	Mapper051_Write( Mapper *mapper, WORD addr, BYTE data )
{
	((Mapper051 *)mapper)->bank = (data & 0x0f) << 2;
	if( 0xC000 <= addr && addr <= 0xDFFF ) {
		((Mapper051 *)mapper)->mode = (((Mapper051 *)mapper)->mode & 0x01) | ((data & 0x10) >> 3);
	}
	Mapper051_SetBank_CPU(mapper);
}

void	Mapper051_SetBank_CPU(Mapper *mapper)
{
	switch(((Mapper051 *)mapper)->mode) {
		case 0:
			SetVRAM_Mirror_cont( VRAM_VMIRROR );
			SetPROM_8K_Bank( 3, (((Mapper051 *)mapper)->bank|0x2c|3) );
			SetPROM_8K_Bank( 4, (((Mapper051 *)mapper)->bank|0x00|0) );
			SetPROM_8K_Bank( 5, (((Mapper051 *)mapper)->bank|0x00|1) );
			SetPROM_8K_Bank( 6, (((Mapper051 *)mapper)->bank|0x0c|2) );
			SetPROM_8K_Bank( 7, (((Mapper051 *)mapper)->bank|0x0c|3) );
			break;
		case 1:
			SetVRAM_Mirror_cont( VRAM_VMIRROR );
			SetPROM_8K_Bank( 3, (((Mapper051 *)mapper)->bank|0x20|3) );
			SetPROM_8K_Bank( 4, (((Mapper051 *)mapper)->bank|0x00|0) );
			SetPROM_8K_Bank( 5, (((Mapper051 *)mapper)->bank|0x00|1) );
			SetPROM_8K_Bank( 6, (((Mapper051 *)mapper)->bank|0x00|2) );
			SetPROM_8K_Bank( 7, (((Mapper051 *)mapper)->bank|0x00|3) );
			break;
		case 2:
			SetVRAM_Mirror_cont( VRAM_VMIRROR );
			SetPROM_8K_Bank( 3, (((Mapper051 *)mapper)->bank|0x2e|3) );
			SetPROM_8K_Bank( 4, (((Mapper051 *)mapper)->bank|0x02|0) );
			SetPROM_8K_Bank( 5, (((Mapper051 *)mapper)->bank|0x02|1) );
			SetPROM_8K_Bank( 6, (((Mapper051 *)mapper)->bank|0x0e|2) );
			SetPROM_8K_Bank( 7, (((Mapper051 *)mapper)->bank|0x0e|3) );
			break;
		case 3:
			SetVRAM_Mirror_cont( VRAM_HMIRROR );
			SetPROM_8K_Bank( 3, (((Mapper051 *)mapper)->bank|0x20|3) );
			SetPROM_8K_Bank( 4, (((Mapper051 *)mapper)->bank|0x00|0) );
			SetPROM_8K_Bank( 5, (((Mapper051 *)mapper)->bank|0x00|1) );
			SetPROM_8K_Bank( 6, (((Mapper051 *)mapper)->bank|0x00|2) );
			SetPROM_8K_Bank( 7, (((Mapper051 *)mapper)->bank|0x00|3) );
			break;
	}
}

void	Mapper051_SaveState( Mapper *mapper, LPBYTE p )
{
	p[ 0] = ((Mapper051 *)mapper)->mode;
	p[ 1] = ((Mapper051 *)mapper)->bank;
}

void	Mapper051_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper051 *)mapper)->mode = p[ 0];
	((Mapper051 *)mapper)->bank = p[ 1];
}

BOOL	Mapper051_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper051_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper051));
	
	memset(mapper, 0, sizeof(Mapper051));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper051_Reset;
	mapper->vtbl.Write = Mapper051_Write;
	mapper->vtbl.WriteLow = Mapper051_WriteLow;
	mapper->vtbl.SaveState = Mapper051_SaveState;
	mapper->vtbl.LoadState = Mapper051_LoadState;
	mapper->vtbl.IsStateSave = Mapper051_IsStateSave;

	return (Mapper *)mapper;
}

