
//////////////////////////////////////////////////////////////////////////
// Mapper082  Taito C075                                                //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg;
}Mapper082;

void	Mapper082_Reset(Mapper *mapper)
{
	((Mapper082 *)mapper)->reg = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	SetVRAM_Mirror_cont( VRAM_VMIRROR );
}

void	Mapper082_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x7EF0:
			if( ((Mapper082 *)mapper)->reg ) {
				SetVROM_2K_Bank( 4, data>>1 );
			} else {
				SetVROM_2K_Bank( 0, data>>1 );
			}
			break;

		case	0x7EF1:
			if( ((Mapper082 *)mapper)->reg ) {
				SetVROM_2K_Bank( 6, data>>1 );
			} else {
				SetVROM_2K_Bank( 2, data>>1 );
			}
			break;

		case	0x7EF2:
			if( ((Mapper082 *)mapper)->reg ) SetVROM_1K_Bank( 0, data );
			else	  SetVROM_1K_Bank( 4, data );
			break;
		case	0x7EF3:
			if( ((Mapper082 *)mapper)->reg ) SetVROM_1K_Bank( 1, data );
			else	  SetVROM_1K_Bank( 5, data );
			break;
		case	0x7EF4:
			if( ((Mapper082 *)mapper)->reg ) SetVROM_1K_Bank( 2, data );
			else	  SetVROM_1K_Bank( 6, data );
			break;
		case	0x7EF5:
			if( ((Mapper082 *)mapper)->reg ) SetVROM_1K_Bank( 3, data );
			else	  SetVROM_1K_Bank( 7, data );
			break;

		case	0x7EF6:
			((Mapper082 *)mapper)->reg = data & 0x02;
			if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_HMIRROR );
			break;

		case	0x7EFA:
			SetPROM_8K_Bank( 4, data>>2 );
			break;
		case	0x7EFB:
			SetPROM_8K_Bank( 5, data>>2 );
			break;
		case	0x7EFC:
			SetPROM_8K_Bank( 6, data>>2 );
			break;
		default:
			Mapper_DefWriteLow( mapper, addr, data );
			break;
	}
}

void	Mapper082_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper082 *)mapper)->reg;
}

void	Mapper082_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper082 *)mapper)->reg = p[0];
}

BOOL	Mapper082_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper082_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper082));
	
	memset(mapper, 0, sizeof(Mapper082));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper082_Reset;
	mapper->vtbl.WriteLow = Mapper082_WriteLow;
	mapper->vtbl.SaveState = Mapper082_SaveState;
	mapper->vtbl.LoadState = Mapper082_LoadState;
	mapper->vtbl.IsStateSave = Mapper082_IsStateSave;

	return (Mapper *)mapper;
}

