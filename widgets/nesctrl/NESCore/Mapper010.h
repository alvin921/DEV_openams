//////////////////////////////////////////////////////////////////////////
// Mapper010 Nintendo MMC4                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[4];
	BYTE	latch_a, latch_b;
}Mapper010;

void	Mapper010_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	((Mapper010 *)mapper)->reg[0] = 0; ((Mapper010 *)mapper)->reg[1] = 4;
	((Mapper010 *)mapper)->reg[2] = 0; ((Mapper010 *)mapper)->reg[3] = 0;

	((Mapper010 *)mapper)->latch_a = 0xFE;
	((Mapper010 *)mapper)->latch_b = 0xFE;
	SetVROM_4K_Bank( 0, 4 );
	SetVROM_4K_Bank( 4, 0 );

	PPU_SetChrLatchMode( ((Mapper010 *)mapper)->nes->ppu, TRUE );
}

void	Mapper010_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF000 ) {
		case	0xA000:
			SetPROM_16K_Bank( 4, data );
			break;
		case	0xB000:
			((Mapper010 *)mapper)->reg[0] = data;
			if( ((Mapper010 *)mapper)->latch_a == 0xFD ) {
				SetVROM_4K_Bank( 0, ((Mapper010 *)mapper)->reg[0] );
			}
			break;
		case	0xC000:
			((Mapper010 *)mapper)->reg[1] = data;
			if( ((Mapper010 *)mapper)->latch_a == 0xFE ) {
				SetVROM_4K_Bank( 0, ((Mapper010 *)mapper)->reg[1] );
			}
			break;
		case	0xD000:
			((Mapper010 *)mapper)->reg[2] = data;
			if( ((Mapper010 *)mapper)->latch_b == 0xFD ) {
				SetVROM_4K_Bank( 4, ((Mapper010 *)mapper)->reg[2] );
			}
			break;
		case	0xE000:
			((Mapper010 *)mapper)->reg[3] = data;
			if( ((Mapper010 *)mapper)->latch_b == 0xFE ) {
				SetVROM_4K_Bank( 4, ((Mapper010 *)mapper)->reg[3] );
			}
			break;
		case	0xF000:
			if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;
	}
}

void	Mapper010_PPU_ChrLatch( Mapper *mapper, WORD addr )
{
	if( (addr&0x1FF0) == 0x0FD0 && ((Mapper010 *)mapper)->latch_a != 0xFD ) {
		((Mapper010 *)mapper)->latch_a = 0xFD;
		SetVROM_4K_Bank( 0, ((Mapper010 *)mapper)->reg[0] );
	} else if( (addr&0x1FF0) == 0x0FE0 && ((Mapper010 *)mapper)->latch_a != 0xFE ) {
		((Mapper010 *)mapper)->latch_a = 0xFE;
		SetVROM_4K_Bank( 0, ((Mapper010 *)mapper)->reg[1] );
	} else if( (addr&0x1FF0) == 0x1FD0 && ((Mapper010 *)mapper)->latch_b != 0xFD ) {
		((Mapper010 *)mapper)->latch_b = 0xFD;
		SetVROM_4K_Bank( 4, ((Mapper010 *)mapper)->reg[2] );
	} else if( (addr&0x1FF0) == 0x1FE0 && ((Mapper010 *)mapper)->latch_b != 0xFE ) {
		((Mapper010 *)mapper)->latch_b = 0xFE;
		SetVROM_4K_Bank( 4, ((Mapper010 *)mapper)->reg[3] );
	}
}

void	Mapper010_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper010 *)mapper)->reg[0];
	p[1] = ((Mapper010 *)mapper)->reg[1];
	p[2] = ((Mapper010 *)mapper)->reg[2];
	p[3] = ((Mapper010 *)mapper)->reg[3];
	p[4] = ((Mapper010 *)mapper)->latch_a;
	p[5] = ((Mapper010 *)mapper)->latch_b;
}

void	Mapper010_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper010 *)mapper)->reg[0] = p[0];
	((Mapper010 *)mapper)->reg[1] = p[1];
	((Mapper010 *)mapper)->reg[2] = p[2];
	((Mapper010 *)mapper)->reg[3] = p[3];
	((Mapper010 *)mapper)->latch_a = p[4];
	((Mapper010 *)mapper)->latch_b = p[5];
}

BOOL	Mapper010_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper010_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper010));
	
	memset(mapper, 0, sizeof(Mapper010));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper010_Reset;
	mapper->vtbl.Write = Mapper010_Write;
	mapper->vtbl.PPU_ChrLatch = Mapper010_PPU_ChrLatch;
	mapper->vtbl.SaveState = Mapper010_SaveState;
	mapper->vtbl.LoadState = Mapper010_LoadState;
	mapper->vtbl.IsStateSave = Mapper010_IsStateSave;

	return (Mapper *)mapper;
}


