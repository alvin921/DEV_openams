//////////////////////////////////////////////////////////////////////////
// Mapper009  Nintendo MMC2                                             //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[4];
	BYTE	latch_a, latch_b;
}Mapper009;

void	Mapper009_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	((Mapper009 *)mapper)->reg[0] = 0; ((Mapper009 *)mapper)->reg[1] = 4;
	((Mapper009 *)mapper)->reg[2] = 0; ((Mapper009 *)mapper)->reg[3] = 0;

	((Mapper009 *)mapper)->latch_a = 0xFE;
	((Mapper009 *)mapper)->latch_b = 0xFE;
	SetVROM_4K_Bank( 0, 4 );
	SetVROM_4K_Bank( 4, 0 );

	PPU_SetChrLatchMode( ((Mapper009 *)mapper)->nes->ppu, TRUE );
}

void	Mapper009_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF000 ) {
		case	0xA000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0xB000:
			((Mapper009 *)mapper)->reg[0] = data;
			if( ((Mapper009 *)mapper)->latch_a == 0xFD ) {
				SetVROM_4K_Bank( 0, ((Mapper009 *)mapper)->reg[0] );
			}
			break;
		case	0xC000:
			((Mapper009 *)mapper)->reg[1] = data;
			if( ((Mapper009 *)mapper)->latch_a == 0xFE ) {
				SetVROM_4K_Bank( 0, ((Mapper009 *)mapper)->reg[1] );
			}
			break;
		case	0xD000:
			((Mapper009 *)mapper)->reg[2] = data;
			if( ((Mapper009 *)mapper)->latch_b == 0xFD ) {
				SetVROM_4K_Bank( 4, ((Mapper009 *)mapper)->reg[2] );
			}
			break;
		case	0xE000:
			((Mapper009 *)mapper)->reg[3] = data;
			if( ((Mapper009 *)mapper)->latch_b == 0xFE ) {
				SetVROM_4K_Bank( 4, ((Mapper009 *)mapper)->reg[3] );
			}
			break;
		case	0xF000:
			if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;
	}
}

void	Mapper009_PPU_ChrLatch( Mapper *mapper, WORD addr )
{
	if( (addr&0x1FF0) == 0x0FD0 && ((Mapper009 *)mapper)->latch_a != 0xFD ) {
		((Mapper009 *)mapper)->latch_a = 0xFD;
		SetVROM_4K_Bank( 0, ((Mapper009 *)mapper)->reg[0] );
	} else if( (addr&0x1FF0) == 0x0FE0 && ((Mapper009 *)mapper)->latch_a != 0xFE ) {
		((Mapper009 *)mapper)->latch_a = 0xFE;
		SetVROM_4K_Bank( 0, ((Mapper009 *)mapper)->reg[1] );
	} else if( (addr&0x1FF0) == 0x1FD0 && ((Mapper009 *)mapper)->latch_b != 0xFD ) {
		((Mapper009 *)mapper)->latch_b = 0xFD;
		SetVROM_4K_Bank( 4, ((Mapper009 *)mapper)->reg[2] );
	} else if( (addr&0x1FF0) == 0x1FE0 && ((Mapper009 *)mapper)->latch_b != 0xFE ) {
		((Mapper009 *)mapper)->latch_b = 0xFE;
		SetVROM_4K_Bank( 4, ((Mapper009 *)mapper)->reg[3] );
	}
}

void	Mapper009_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper009 *)mapper)->reg[0];
	p[1] = ((Mapper009 *)mapper)->reg[1];
	p[2] = ((Mapper009 *)mapper)->reg[2];
	p[3] = ((Mapper009 *)mapper)->reg[3];
	p[4] = ((Mapper009 *)mapper)->latch_a;
	p[5] = ((Mapper009 *)mapper)->latch_b;
}

void	Mapper009_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper009 *)mapper)->reg[0] = p[0];
	((Mapper009 *)mapper)->reg[1] = p[1];
	((Mapper009 *)mapper)->reg[2] = p[2];
	((Mapper009 *)mapper)->reg[3] = p[3];
	((Mapper009 *)mapper)->latch_a = p[4];
	((Mapper009 *)mapper)->latch_b = p[5];
}

BOOL	Mapper009_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper009_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper009));
	
	memset(mapper, 0, sizeof(Mapper009));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper009_Reset;
	mapper->vtbl.Write = Mapper009_Write;
	mapper->vtbl.PPU_ChrLatch = Mapper009_PPU_ChrLatch;
	mapper->vtbl.SaveState = Mapper009_SaveState;
	mapper->vtbl.LoadState = Mapper009_LoadState;
	mapper->vtbl.IsStateSave = Mapper009_IsStateSave;

	return (Mapper *)mapper;
}

