//////////////////////////////////////////////////////////////////////////
// Mapper255  110-in-1                                                  //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE    reg[4];
}Mapper255;

void	Mapper255_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
	SetVROM_8K_Bank_cont( 0 );
	SetVRAM_Mirror_cont( VRAM_VMIRROR );

	((Mapper255 *)mapper)->reg[0] = 0;
	((Mapper255 *)mapper)->reg[1] = 0;
	((Mapper255 *)mapper)->reg[2] = 0;
	((Mapper255 *)mapper)->reg[3] = 0;
}

BYTE	Mapper255_ReadLow( Mapper *mapper, WORD addr )
{
	if( addr >= 0x5800 ) {
		return	((Mapper255 *)mapper)->reg[addr&0x0003] & 0x0F;
	} else {
		return	addr>>8;
	}
}

void	Mapper255_WriteLow( Mapper *mapper,WORD addr, BYTE data )
{
	if( addr >= 0x5800 ) {
		((Mapper255 *)mapper)->reg[addr&0x0003] = data & 0x0F;
	}
}

void	Mapper255_Write( Mapper *mapper,WORD addr, BYTE data )
{
	BYTE	prg = (addr & 0x0F80)>>7;
	INT	chr = (addr & 0x003F);
	INT	bank = (addr & 0x4000)>>14;

	if( addr & 0x2000 ) {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	}

	if( addr & 0x1000 ) {
		if( addr & 0x0040 ) {
			SetPROM_8K_Bank( 4, 0x80*bank+prg*4+2 );
			SetPROM_8K_Bank( 5, 0x80*bank+prg*4+3 );
			SetPROM_8K_Bank( 6, 0x80*bank+prg*4+2 );
			SetPROM_8K_Bank( 7, 0x80*bank+prg*4+3 );
		} else {
			SetPROM_8K_Bank( 4, 0x80*bank+prg*4+0 );
			SetPROM_8K_Bank( 5, 0x80*bank+prg*4+1 );
			SetPROM_8K_Bank( 6, 0x80*bank+prg*4+0 );
			SetPROM_8K_Bank( 7, 0x80*bank+prg*4+1 );
		}
	} else {
		SetPROM_8K_Bank( 4, 0x80*bank+prg*4+0 );
		SetPROM_8K_Bank( 5, 0x80*bank+prg*4+1 );
		SetPROM_8K_Bank( 6, 0x80*bank+prg*4+2 );
		SetPROM_8K_Bank( 7, 0x80*bank+prg*4+3 );
	}

	SetVROM_1K_Bank( 0, 0x200*bank+chr*8+0 );
	SetVROM_1K_Bank( 1, 0x200*bank+chr*8+1 );
	SetVROM_1K_Bank( 2, 0x200*bank+chr*8+2 );
	SetVROM_1K_Bank( 3, 0x200*bank+chr*8+3 );
	SetVROM_1K_Bank( 4, 0x200*bank+chr*8+4 );
	SetVROM_1K_Bank( 5, 0x200*bank+chr*8+5 );
	SetVROM_1K_Bank( 6, 0x200*bank+chr*8+6 );
	SetVROM_1K_Bank( 7, 0x200*bank+chr*8+7 );
}

void	Mapper255_SaveState( Mapper *mapper,LPBYTE p )
{
	p[ 0] = ((Mapper255 *)mapper)->reg[0];
	p[ 1] = ((Mapper255 *)mapper)->reg[1];
	p[ 2] = ((Mapper255 *)mapper)->reg[2];
	p[ 3] = ((Mapper255 *)mapper)->reg[3];
}

void	Mapper255_LoadState( Mapper *mapper,LPBYTE p )
{
	((Mapper255 *)mapper)->reg[0] = p[ 0];
	((Mapper255 *)mapper)->reg[1] = p[ 1];
	((Mapper255 *)mapper)->reg[2] = p[ 2];
	((Mapper255 *)mapper)->reg[3] = p[ 3];
}

BOOL	Mapper255_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper255_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper255));
	
	memset(mapper, 0, sizeof(Mapper255));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper255_Reset;
	mapper->vtbl.Write = Mapper255_Write;
	mapper->vtbl.ReadLow = Mapper255_ReadLow;
	mapper->vtbl.WriteLow = Mapper255_WriteLow;
	mapper->vtbl.SaveState = Mapper255_SaveState;
	mapper->vtbl.LoadState = Mapper255_LoadState;
	mapper->vtbl.IsStateSave = Mapper255_IsStateSave;

	return (Mapper *)mapper;
}


