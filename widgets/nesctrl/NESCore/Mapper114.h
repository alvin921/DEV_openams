//////////////////////////////////////////////////////////////////////////
// Mapper114                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg_m, reg_a, reg_b[8];
	BYTE	reg_c;
	BYTE	irq_counter;
	BYTE	irq_occur;
}Mapper114;

void	Mapper114_SetBank_CPU(Mapper *mapper);
void	Mapper114_SetBank_PPU(Mapper *mapper);

void	Mapper114_Reset(Mapper *mapper)
{
	INT i;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	((Mapper114 *)mapper)->reg_a = ((Mapper114 *)mapper)->reg_c = ((Mapper114 *)mapper)->reg_m = 0;
	for( i = 0; i < 8; i++ ) {
		((Mapper114 *)mapper)->reg_b[i] = 0;
	}
	((Mapper114 *)mapper)->irq_counter = 0;
	((Mapper114 *)mapper)->irq_occur = 0;

	NES_SetRenderMethod( mapper->nes, POST_RENDER );
}

void	Mapper114_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	((Mapper114 *)mapper)->reg_m = data;
	Mapper114_SetBank_CPU(mapper);
}

void	Mapper114_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0xE003 ) {
		((Mapper114 *)mapper)->irq_counter = data;
	} else
	if( addr == 0xE002 ) {
		((Mapper114 *)mapper)->irq_occur = 0;
		CPU_ClrIRQ( ((Mapper114 *)mapper)->nes->cpu, IRQ_MAPPER );
	} else {
		switch( addr & 0xE000 ) {
			case	0x8000:
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
				break;
			case	0xA000:
				((Mapper114 *)mapper)->reg_c = 1;
				((Mapper114 *)mapper)->reg_a = data;
				break;
			case	0xC000:
				if( !((Mapper114 *)mapper)->reg_c ) {
					break;
				}
				((Mapper114 *)mapper)->reg_b[((Mapper114 *)mapper)->reg_a&0x07] = data;
				switch( ((Mapper114 *)mapper)->reg_a & 0x07 ) {
					case	0:
					case	1:
					case	2:
					case	3:
					case	6:
					case	7:
						Mapper114_SetBank_PPU(mapper);
						break;
					case	4:
					case	5:
						Mapper114_SetBank_CPU(mapper);
						break;
				}
				((Mapper114 *)mapper)->reg_c = 0;
				break;
		}
	}
}

void	Mapper114_Clock( Mapper *mapper, INT scanline )
{
//	if( ((Mapper114 *)mapper)->irq_occur ) {
//		((Mapper114 *)mapper)->nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper114_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) && PPU_IsDispON(((Mapper114 *)mapper)->nes->ppu) ) {
		if( ((Mapper114 *)mapper)->irq_counter ) {
			((Mapper114 *)mapper)->irq_counter--;
			if( !((Mapper114 *)mapper)->irq_counter ) {
				((Mapper114 *)mapper)->irq_occur = 0xFF;
				CPU_SetIRQ( ((Mapper114 *)mapper)->nes->cpu, IRQ_MAPPER );
			}
		}
	}
}

void	Mapper114_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper114 *)mapper)->reg_m & 0x80 ) {
		SetPROM_16K_Bank( 4, ((Mapper114 *)mapper)->reg_m & 0x1F );
	} else {
		SetPROM_8K_Bank( 4, ((Mapper114 *)mapper)->reg_b[4] );
		SetPROM_8K_Bank( 5, ((Mapper114 *)mapper)->reg_b[5] );
	}
}

void	Mapper114_SetBank_PPU(Mapper *mapper)
{
	SetVROM_2K_Bank( 0, ((Mapper114 *)mapper)->reg_b[0]>>1 );
	SetVROM_2K_Bank( 2, ((Mapper114 *)mapper)->reg_b[2]>>1 );
	SetVROM_1K_Bank( 4, ((Mapper114 *)mapper)->reg_b[6] );
	SetVROM_1K_Bank( 5, ((Mapper114 *)mapper)->reg_b[1] );
	SetVROM_1K_Bank( 6, ((Mapper114 *)mapper)->reg_b[7] );
	SetVROM_1K_Bank( 7, ((Mapper114 *)mapper)->reg_b[3] );
}

void	Mapper114_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper114 *)mapper)->reg_b[i];
	}

	p[ 8] = ((Mapper114 *)mapper)->reg_m;
	p[ 9] = ((Mapper114 *)mapper)->reg_a;
	p[10] = ((Mapper114 *)mapper)->reg_c;
	p[11] = ((Mapper114 *)mapper)->irq_counter;
	p[12] = ((Mapper114 *)mapper)->irq_occur;
}

void	Mapper114_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper114 *)mapper)->reg_b[i] = p[i];
	}
	((Mapper114 *)mapper)->reg_m = p[ 8];
	((Mapper114 *)mapper)->reg_a = p[ 9];
	((Mapper114 *)mapper)->reg_c = p[10];
	((Mapper114 *)mapper)->irq_counter = p[11];
	((Mapper114 *)mapper)->irq_occur   = p[12];
}

BOOL	Mapper114_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper114_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper114));
	
	memset(mapper, 0, sizeof(Mapper114));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper114_Reset;
	mapper->vtbl.WriteLow = Mapper114_WriteLow;
	mapper->vtbl.Write = Mapper114_Write;
	mapper->vtbl.Clock = Mapper114_Clock;
	mapper->vtbl.HSync = Mapper114_HSync;
	mapper->vtbl.SaveState = Mapper114_SaveState;
	mapper->vtbl.LoadState = Mapper114_LoadState;
	mapper->vtbl.IsStateSave = Mapper114_IsStateSave;

	return (Mapper *)mapper;
}


