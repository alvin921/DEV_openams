
//////////////////////////////////////////////////////////////////////////
// Mapper226  76-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[2];
}Mapper226;

void	Mapper226_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );

	((Mapper226 *)mapper)->reg[0] = 0;
	((Mapper226 *)mapper)->reg[1] = 0;
}

void	Mapper226_Write( Mapper *mapper,WORD addr, BYTE data )
{
	BYTE	bank;
	if( addr & 0x001 ) {
		((Mapper226 *)mapper)->reg[1] = data;
	} else {
		((Mapper226 *)mapper)->reg[0] = data;
	}

	if( ((Mapper226 *)mapper)->reg[0] & 0x40 ) {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	}

	bank = ((((Mapper226 *)mapper)->reg[0]&0x1E)>>1)|((((Mapper226 *)mapper)->reg[0]&0x80)>>3)|((((Mapper226 *)mapper)->reg[1]&0x01)<<5);

	if( ((Mapper226 *)mapper)->reg[0] & 0x20 ) {
		if( ((Mapper226 *)mapper)->reg[0] & 0x01 ) {
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
	} else {
		SetPROM_8K_Bank( 4, bank*4+0 );
		SetPROM_8K_Bank( 5, bank*4+1 );
		SetPROM_8K_Bank( 6, bank*4+2 );
		SetPROM_8K_Bank( 7, bank*4+3 );
	}
}

void	Mapper226_SaveState( Mapper *mapper,LPBYTE p )
{
	p[0] = ((Mapper226 *)mapper)->reg[0];
	p[1] = ((Mapper226 *)mapper)->reg[1];
}

void	Mapper226_LoadState( Mapper *mapper,LPBYTE p )
{
	((Mapper226 *)mapper)->reg[0] = p[0];
	((Mapper226 *)mapper)->reg[1] = p[1];
}

BOOL	Mapper226_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper226_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper226));
	
	memset(mapper, 0, sizeof(Mapper226));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper226_Reset;
	mapper->vtbl.Write = Mapper226_Write;
	mapper->vtbl.SaveState = Mapper226_SaveState;
	mapper->vtbl.LoadState = Mapper226_LoadState;
	mapper->vtbl.IsStateSave = Mapper226_IsStateSave;

	return (Mapper *)mapper;
}


