//////////////////////////////////////////////////////////////////////////
// Mapper046  Rumble Station                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	int     reg[4];
}Mapper046;

void	Mapper046_SetBank(Mapper *mapper);

void	Mapper046_Reset(Mapper *mapper)
{
	((Mapper046 *)mapper)->reg[0] = 0;
	((Mapper046 *)mapper)->reg[1] = 0;
	((Mapper046 *)mapper)->reg[2] = 0;
	((Mapper046 *)mapper)->reg[3] = 0;

	Mapper046_SetBank(mapper);
	SetVRAM_Mirror_cont( VRAM_VMIRROR );
}

void	Mapper046_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	((Mapper046 *)mapper)->reg[0] = data & 0x0F;
	((Mapper046 *)mapper)->reg[1] = (data & 0xF0)>>4;
	Mapper046_SetBank(mapper);
}

void	Mapper046_Write( Mapper *mapper, WORD addr, BYTE data )
{
	((Mapper046 *)mapper)->reg[2] = data & 0x01;
	((Mapper046 *)mapper)->reg[3] = (data & 0x70)>>4;
	Mapper046_SetBank(mapper);
}

void	Mapper046_SetBank(Mapper *mapper)
{
	SetPROM_8K_Bank( 4, ((Mapper046 *)mapper)->reg[0]*8+((Mapper046 *)mapper)->reg[2]*4+0 );
	SetPROM_8K_Bank( 5, ((Mapper046 *)mapper)->reg[0]*8+((Mapper046 *)mapper)->reg[2]*4+1 );
	SetPROM_8K_Bank( 6, ((Mapper046 *)mapper)->reg[0]*8+((Mapper046 *)mapper)->reg[2]*4+2 );
	SetPROM_8K_Bank( 7, ((Mapper046 *)mapper)->reg[0]*8+((Mapper046 *)mapper)->reg[2]*4+3 );

	SetVROM_1K_Bank( 0, ((Mapper046 *)mapper)->reg[1]*64+((Mapper046 *)mapper)->reg[3]*8+0 );
	SetVROM_1K_Bank( 1, ((Mapper046 *)mapper)->reg[1]*64+((Mapper046 *)mapper)->reg[3]*8+1 );
	SetVROM_1K_Bank( 2, ((Mapper046 *)mapper)->reg[1]*64+((Mapper046 *)mapper)->reg[3]*8+2 );
	SetVROM_1K_Bank( 3, ((Mapper046 *)mapper)->reg[1]*64+((Mapper046 *)mapper)->reg[3]*8+3 );
	SetVROM_1K_Bank( 4, ((Mapper046 *)mapper)->reg[1]*64+((Mapper046 *)mapper)->reg[3]*8+4 );
	SetVROM_1K_Bank( 5, ((Mapper046 *)mapper)->reg[1]*64+((Mapper046 *)mapper)->reg[3]*8+5 );
	SetVROM_1K_Bank( 6, ((Mapper046 *)mapper)->reg[1]*64+((Mapper046 *)mapper)->reg[3]*8+6 );
	SetVROM_1K_Bank( 7, ((Mapper046 *)mapper)->reg[1]*64+((Mapper046 *)mapper)->reg[3]*8+7 );
}

void	Mapper046_SaveState( Mapper *mapper, LPBYTE p )
{
	p[ 0] = ((Mapper046 *)mapper)->reg[0];
	p[ 1] = ((Mapper046 *)mapper)->reg[1];
	p[ 2] = ((Mapper046 *)mapper)->reg[2];
	p[ 3] = ((Mapper046 *)mapper)->reg[3];
}

void	Mapper046_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper046 *)mapper)->reg[0] = p[ 0];
	((Mapper046 *)mapper)->reg[1] = p[ 1];
	((Mapper046 *)mapper)->reg[2] = p[ 2];
	((Mapper046 *)mapper)->reg[3] = p[ 3];
}
BOOL	Mapper046_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper046_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper046));
	
	memset(mapper, 0, sizeof(Mapper046));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper046_Reset;
	mapper->vtbl.WriteLow = Mapper046_WriteLow;
	mapper->vtbl.Write = Mapper046_Write;
	mapper->vtbl.SaveState = Mapper046_SaveState;
	mapper->vtbl.LoadState = Mapper046_LoadState;
	mapper->vtbl.IsStateSave = Mapper046_IsStateSave;

	return (Mapper *)mapper;
}


