
//////////////////////////////////////////////////////////////////////////
// Mapper075  Konami VRC1/Jaleco D65005                                 //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[2];
}Mapper075;

void	Mapper075_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	((Mapper075 *)mapper)->reg[0] = 0;
	((Mapper075 *)mapper)->reg[1] = 1;
}

void	Mapper075_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF000 ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;

		case	0x9000:
			if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );

			((Mapper075 *)mapper)->reg[0] = (((Mapper075 *)mapper)->reg[0] & 0x0F) | ((data & 0x02) << 3);
			((Mapper075 *)mapper)->reg[1] = (((Mapper075 *)mapper)->reg[1] & 0x0F) | ((data & 0x04) << 2);
			SetVROM_4K_Bank( 0, ((Mapper075 *)mapper)->reg[0] );
			SetVROM_4K_Bank( 4, ((Mapper075 *)mapper)->reg[1] );
			break;

		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0xC000:
			SetPROM_8K_Bank( 6, data );
			break;

		case	0xE000:
			((Mapper075 *)mapper)->reg[0] = (((Mapper075 *)mapper)->reg[0] & 0x10) | (data & 0x0F);
			SetVROM_4K_Bank( 0, ((Mapper075 *)mapper)->reg[0] );
			break;

		case	0xF000:
			((Mapper075 *)mapper)->reg[1] = (((Mapper075 *)mapper)->reg[1] & 0x10) | (data & 0x0F);
			SetVROM_4K_Bank( 4, ((Mapper075 *)mapper)->reg[1] );
			break;
	}
}

void	Mapper075_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper075 *)mapper)->reg[0];
	p[1] = ((Mapper075 *)mapper)->reg[1];
}

void	Mapper075_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper075 *)mapper)->reg[0] = p[0];
	((Mapper075 *)mapper)->reg[1] = p[1];
}

BOOL	Mapper075_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper075_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper075));
	
	memset(mapper, 0, sizeof(Mapper075));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper075_Reset;
	mapper->vtbl.Write = Mapper075_Write;
	mapper->vtbl.SaveState = Mapper075_SaveState;
	mapper->vtbl.LoadState = Mapper075_LoadState;
	mapper->vtbl.IsStateSave = Mapper075_IsStateSave;

	return (Mapper *)mapper;
}

