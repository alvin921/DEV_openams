
//////////////////////////////////////////////////////////////////////////
// Mapper096  Bandai 74161                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[2];
}Mapper096;
void	Mapper096_SetBank(Mapper *mapper);

void	Mapper096_Reset(Mapper *mapper)
{
	((Mapper096 *)mapper)->reg[0] = ((Mapper096 *)mapper)->reg[1] = 0;

	SetPROM_32K_Bank( 0, 1, 2, 3 );
	Mapper096_SetBank(mapper);

	SetVRAM_Mirror_cont( VRAM_MIRROR4L );
}

void	Mapper096_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_32K_Bank_cont( data & 0x03 );

	((Mapper096 *)mapper)->reg[0] = (data & 0x04) >> 2;
	Mapper096_SetBank(mapper);
}

void	Mapper096_PPU_Latch( Mapper *mapper, WORD addr )
{
	if( (addr & 0xF000) == 0x2000 ) {
		((Mapper096 *)mapper)->reg[1] = (addr>>8)&0x03;
		Mapper096_SetBank(mapper);
	}
}

void	Mapper096_SetBank(Mapper *mapper)
{
	SetCRAM_4K_Bank( 0, ((Mapper096 *)mapper)->reg[0]*4+((Mapper096 *)mapper)->reg[1] );
	SetCRAM_4K_Bank( 4, ((Mapper096 *)mapper)->reg[0]*4+0x03 );
}

void	Mapper096_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper096 *)mapper)->reg[0];
	p[1] = ((Mapper096 *)mapper)->reg[1];
}

void	Mapper096_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper096 *)mapper)->reg[0] = p[0];
	((Mapper096 *)mapper)->reg[1] = p[1];
}

BOOL	Mapper096_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper096_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper096));
	
	memset(mapper, 0, sizeof(Mapper096));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper096_Reset;
	mapper->vtbl.Write = Mapper096_Write;
	mapper->vtbl.PPU_Latch = Mapper096_PPU_Latch;
	mapper->vtbl.SaveState = Mapper096_SaveState;
	mapper->vtbl.LoadState = Mapper096_LoadState;
	mapper->vtbl.IsStateSave = Mapper096_IsStateSave;

	return (Mapper *)mapper;
}

