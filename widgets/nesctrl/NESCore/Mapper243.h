
//////////////////////////////////////////////////////////////////////////
// Mapper243  PC-Sachen/Hacker                                          //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[4];
}Mapper243;

void	Mapper243_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
	if( VROM_8K_SIZE > 4 ) {
		SetVROM_8K_Bank_cont( 4 );
	} else if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	SetVRAM_Mirror_cont( VRAM_HMIRROR );

	((Mapper243 *)mapper)->reg[0] = 0;
	((Mapper243 *)mapper)->reg[1] = 0;
	((Mapper243 *)mapper)->reg[2] = 3;
	((Mapper243 *)mapper)->reg[3] = 0;
}

void	Mapper243_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( (addr&0x4101) == 0x4100 ) {
		((Mapper243 *)mapper)->reg[0] = data;
	} else if( (addr&0x4101) == 0x4101 ) {
		switch( ((Mapper243 *)mapper)->reg[0] & 0x07 ) {
			case	0:
				((Mapper243 *)mapper)->reg[1] = 0;
				((Mapper243 *)mapper)->reg[2] = 3;
				break;
			case	4:
				((Mapper243 *)mapper)->reg[2] = (((Mapper243 *)mapper)->reg[2]&0x06)|(data&0x01);
				break;
			case	5:
				((Mapper243 *)mapper)->reg[1] = data&0x01;
				break;
			case	6:
				((Mapper243 *)mapper)->reg[2] = (((Mapper243 *)mapper)->reg[2]&0x01)|((data&0x03)<<1);
				break;
			case	7:
				((Mapper243 *)mapper)->reg[3] = data&0x01;
				break;
			default:
				break;
		}

		SetPROM_32K_Bank_cont( ((Mapper243 *)mapper)->reg[1] );
		SetVROM_8K_Bank( ((Mapper243 *)mapper)->reg[2]*8+0, ((Mapper243 *)mapper)->reg[2]*8+1, ((Mapper243 *)mapper)->reg[2]*8+2, ((Mapper243 *)mapper)->reg[2]*8+3,
				 ((Mapper243 *)mapper)->reg[2]*8+4, ((Mapper243 *)mapper)->reg[2]*8+5, ((Mapper243 *)mapper)->reg[2]*8+6, ((Mapper243 *)mapper)->reg[2]*8+7 );

		if( ((Mapper243 *)mapper)->reg[3] ) {
			SetVRAM_Mirror_cont( VRAM_VMIRROR );
		} else {
			SetVRAM_Mirror_cont( VRAM_HMIRROR );
		}
	}
}

void	Mapper243_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper243 *)mapper)->reg[0];
	p[1] = ((Mapper243 *)mapper)->reg[1];
	p[2] = ((Mapper243 *)mapper)->reg[2];
	p[3] = ((Mapper243 *)mapper)->reg[3];
}

void	Mapper243_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper243 *)mapper)->reg[0] = p[0];
	((Mapper243 *)mapper)->reg[1] = p[1];
	((Mapper243 *)mapper)->reg[2] = p[2];
	((Mapper243 *)mapper)->reg[3] = p[3];
}

BOOL	Mapper243_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper243_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper243));
	
	memset(mapper, 0, sizeof(Mapper243));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper243_Reset;
	mapper->vtbl.WriteLow = Mapper243_WriteLow;
	mapper->vtbl.SaveState = Mapper243_SaveState;
	mapper->vtbl.LoadState = Mapper243_LoadState;
	mapper->vtbl.IsStateSave = Mapper243_IsStateSave;

	return (Mapper *)mapper;
}


