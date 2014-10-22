//////////////////////////////////////////////////////////////////////////
// Mapper041  Caltron 6-in-1                                            //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[2];
}Mapper041;


void	Mapper041_Reset(Mapper *mapper)
{
	((Mapper041 *)mapper)->reg[0] = ((Mapper041 *)mapper)->reg[1] = 0;

	SetPROM_32K_Bank( 0, 1, 2, 3 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper041_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr >= 0x6000 && addr < 0x6800 ) {
		SetPROM_32K_Bank_cont( addr&0x07 );
		((Mapper041 *)mapper)->reg[0] = addr & 0x04;
		((Mapper041 *)mapper)->reg[1] &= 0x03;
		((Mapper041 *)mapper)->reg[1] |= (addr>>1)&0x0C;
		SetVROM_8K_Bank_cont( ((Mapper041 *)mapper)->reg[1] );
		if( addr&0x20 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
		else		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	}
}

void	Mapper041_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( ((Mapper041 *)mapper)->reg[0] ) {
		((Mapper041 *)mapper)->reg[1] &= 0x0C;
		((Mapper041 *)mapper)->reg[1] |= addr & 0x03;
		SetVROM_8K_Bank_cont( ((Mapper041 *)mapper)->reg[1] );
	}
}

void	Mapper041_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper041 *)mapper)->reg[0];
	p[1] = ((Mapper041 *)mapper)->reg[1];
}

void	Mapper041_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper041 *)mapper)->reg[0] = p[0];
	((Mapper041 *)mapper)->reg[1] = p[1];
}

BOOL	Mapper041_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper041_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper041));
	
	memset(mapper, 0, sizeof(Mapper041));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper041_Reset;
	mapper->vtbl.WriteLow = Mapper041_WriteLow;
	mapper->vtbl.Write = Mapper041_Write;
	mapper->vtbl.SaveState = Mapper041_SaveState;
	mapper->vtbl.LoadState = Mapper041_LoadState;
	mapper->vtbl.IsStateSave = Mapper041_IsStateSave;

	return (Mapper *)mapper;
}

