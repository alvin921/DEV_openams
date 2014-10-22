
//////////////////////////////////////////////////////////////////////////
// Mapper236  800-in-1                                                  //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE    bank, mode;
}Mapper236;
void	Mapper236_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	((Mapper236 *)mapper)->bank = ((Mapper236 *)mapper)->mode = 0;
}

void	Mapper236_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr >= 0x8000 && addr <= 0xBFFF ) {
		((Mapper236 *)mapper)->bank = ((addr&0x03)<<4)|(((Mapper236 *)mapper)->bank&0x07);
	} else {
		((Mapper236 *)mapper)->bank = (addr&0x07)|(((Mapper236 *)mapper)->bank&0x30);
		((Mapper236 *)mapper)->mode = addr&0x30;
	}

	if( addr & 0x20 ) {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	}

	switch( ((Mapper236 *)mapper)->mode ) {
		case	0x00:
			((Mapper236 *)mapper)->bank |= 0x08;
			SetPROM_8K_Bank( 4, ((Mapper236 *)mapper)->bank*2+0 );
			SetPROM_8K_Bank( 5, ((Mapper236 *)mapper)->bank*2+1 );
			SetPROM_8K_Bank( 6, (((Mapper236 *)mapper)->bank|0x07)*2+0 );
			SetPROM_8K_Bank( 7, (((Mapper236 *)mapper)->bank|0x07)*2+1 );
			break;
		case	0x10:
			((Mapper236 *)mapper)->bank |= 0x37;
			SetPROM_8K_Bank( 4, ((Mapper236 *)mapper)->bank*2+0 );
			SetPROM_8K_Bank( 5, ((Mapper236 *)mapper)->bank*2+1 );
			SetPROM_8K_Bank( 6, (((Mapper236 *)mapper)->bank|0x07)*2+0 );
			SetPROM_8K_Bank( 7, (((Mapper236 *)mapper)->bank|0x07)*2+1 );
			break;
		case	0x20:
			((Mapper236 *)mapper)->bank |= 0x08;
			SetPROM_8K_Bank( 4, (((Mapper236 *)mapper)->bank&0xFE)*2+0 );
			SetPROM_8K_Bank( 5, (((Mapper236 *)mapper)->bank&0xFE)*2+1 );
			SetPROM_8K_Bank( 6, (((Mapper236 *)mapper)->bank&0xFE)*2+2 );
			SetPROM_8K_Bank( 7, (((Mapper236 *)mapper)->bank&0xFE)*2+3 );
			break;
		case	0x30:
			((Mapper236 *)mapper)->bank |= 0x08;
			SetPROM_8K_Bank( 4, ((Mapper236 *)mapper)->bank*2+0 );
			SetPROM_8K_Bank( 5, ((Mapper236 *)mapper)->bank*2+1 );
			SetPROM_8K_Bank( 6, ((Mapper236 *)mapper)->bank*2+0 );
			SetPROM_8K_Bank( 7, ((Mapper236 *)mapper)->bank*2+1 );
			break;
	}
}

void	Mapper236_SaveState( Mapper *mapper, LPBYTE p )
{
	p[ 0] = ((Mapper236 *)mapper)->bank;
	p[ 1] = ((Mapper236 *)mapper)->mode;
}

void	Mapper236_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper236 *)mapper)->bank = p[ 0];
	((Mapper236 *)mapper)->mode = p[ 1];
}

BOOL	Mapper236_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper236_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper236));
	
	memset(mapper, 0, sizeof(Mapper236));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper236_Reset;
	mapper->vtbl.Write = Mapper236_Write;
	mapper->vtbl.SaveState = Mapper236_SaveState;
	mapper->vtbl.LoadState = Mapper236_LoadState;
	mapper->vtbl.IsStateSave = Mapper236_IsStateSave;

	return (Mapper *)mapper;
}

