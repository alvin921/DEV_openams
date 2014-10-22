
//////////////////////////////////////////////////////////////////////////
// Mapper116 CartSaint : —H—VAV‹­—ñ“`                                   //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	prg0, prg1, prg2, prg3;
	BYTE	prg0L, prg1L;
	BYTE	chr0, chr1, chr2, chr3, chr4, chr5, chr6, chr7;

	BYTE	irq_enable;
	INT	irq_counter;
	BYTE	irq_latch;

	BYTE	ExPrgSwitch;
	BYTE	ExChrSwitch;
}Mapper116;
void	Mapper116_SetBank_CPU(Mapper *mapper);
void	Mapper116_SetBank_PPU(Mapper *mapper);


void	Mapper116_Reset(Mapper *mapper)
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper116 *)mapper)->reg[i] = 0x00;
	}

	((Mapper116 *)mapper)->prg0 = ((Mapper116 *)mapper)->prg0L = 0;
	((Mapper116 *)mapper)->prg1 = ((Mapper116 *)mapper)->prg1L = 1;
	((Mapper116 *)mapper)->prg2 = PROM_8K_SIZE-2;
	((Mapper116 *)mapper)->prg3 = PROM_8K_SIZE-1;

	((Mapper116 *)mapper)->ExPrgSwitch = 0;
	((Mapper116 *)mapper)->ExChrSwitch = 0;

	Mapper116_SetBank_CPU(mapper);

	if( VROM_1K_SIZE ) {
		((Mapper116 *)mapper)->chr0 = 0;
		((Mapper116 *)mapper)->chr1 = 1;
		((Mapper116 *)mapper)->chr2 = 2;
		((Mapper116 *)mapper)->chr3 = 3;
		((Mapper116 *)mapper)->chr4 = 4;
		((Mapper116 *)mapper)->chr5 = 5;
		((Mapper116 *)mapper)->chr6 = 6;
		((Mapper116 *)mapper)->chr7 = 7;
		Mapper116_SetBank_PPU(mapper);
	} else {
		((Mapper116 *)mapper)->chr0 = ((Mapper116 *)mapper)->chr2 = ((Mapper116 *)mapper)->chr4 = ((Mapper116 *)mapper)->chr5 = ((Mapper116 *)mapper)->chr6 = ((Mapper116 *)mapper)->chr7 = 0;
		((Mapper116 *)mapper)->chr1 = ((Mapper116 *)mapper)->chr3 = 1;
	}

	((Mapper116 *)mapper)->irq_enable = 0;	// Disable
	((Mapper116 *)mapper)->irq_counter = 0;
	((Mapper116 *)mapper)->irq_latch = 0;

//	mapper->nes->SetFrameIRQmode( FALSE );
}

void	Mapper116_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
DebugOut( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, NES_GetScanline(mapper->nes), CPU_GetTotalCycles(mapper->nes->cpu) );
	if( (addr & 0x4100) == 0x4100 ) {
		((Mapper116 *)mapper)->ExChrSwitch = data;
		Mapper116_SetBank_PPU(mapper);
	}
}

void	Mapper116_Write( Mapper *mapper, WORD addr, BYTE data )
{
DebugOut( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, NES_GetScanline(mapper->nes), CPU_GetTotalCycles(mapper->nes->cpu) );

	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper116 *)mapper)->reg[0] = data;
			Mapper116_SetBank_CPU(mapper);
			Mapper116_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper116 *)mapper)->reg[1] = data;
			switch( ((Mapper116 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper116 *)mapper)->chr0 = data & 0xFE;
					((Mapper116 *)mapper)->chr1 = ((Mapper116 *)mapper)->chr0+1;
					Mapper116_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper116 *)mapper)->chr2 = data & 0xFE;
					((Mapper116 *)mapper)->chr3 = ((Mapper116 *)mapper)->chr2+1;
					Mapper116_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper116 *)mapper)->chr4 = data;
					Mapper116_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper116 *)mapper)->chr5 = data;
					Mapper116_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper116 *)mapper)->chr6 = data;
					Mapper116_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper116 *)mapper)->chr7 = data;
					Mapper116_SetBank_PPU(mapper);
					break;
				case	0x06:
					((Mapper116 *)mapper)->prg0 =  data;
					Mapper116_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper116 *)mapper)->prg1 =  data;
					Mapper116_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper116 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(mapper->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper116 *)mapper)->reg[3] = data;
			break;
		case	0xC000:
			((Mapper116 *)mapper)->reg[4] = data;
			((Mapper116 *)mapper)->irq_counter = data;
//			((Mapper116 *)mapper)->irq_enable = 0xFF;
			break;
		case	0xC001:
			((Mapper116 *)mapper)->reg[5] = data;
			((Mapper116 *)mapper)->irq_latch = data;
			break;
		case	0xE000:
			((Mapper116 *)mapper)->reg[6] = data;
			((Mapper116 *)mapper)->irq_counter = ((Mapper116 *)mapper)->irq_latch;
			((Mapper116 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper116 *)mapper)->reg[7] = data;
			((Mapper116 *)mapper)->irq_enable = 0xFF;
			break;
	}
}

void	Mapper116_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( ((Mapper116 *)mapper)->irq_counter <= 0 ) {
			if( ((Mapper116 *)mapper)->irq_enable ) {
//				CPU_IRQ_NotPending(mapper->nes->cpu);
				CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );

#if	0
DebugOut( "IRQ L=%3d\n", scanline );
{
LPBYTE	lpScn = PPU_GetScreenPtr(mapper->nes->ppu);
	INT i;

	lpScn = lpScn+(256+16)*scanline;

	for( i = 0; i < 64; i++ ) {
		lpScn[i] = 22;
	}
}
#endif
				return;
			}
		}

		if( PPU_IsDispON(mapper->nes->ppu) ) {
			((Mapper116 *)mapper)->irq_counter--;
		}
	}

#if	0
	if( (scanline >= 0 && scanline <= 239) ) {
		if(PPU_IsDispON( mapper->nes->ppu) ) {
			if( ((Mapper116 *)mapper)->irq_enable ) {
				if( !(((Mapper116 *)mapper)->irq_counter--) ) {
//					((Mapper116 *)mapper)->irq_counter = ((Mapper116 *)mapper)->irq_latch;
					CPU_IRQ_NotPending(mapper->nes->cpu);
				}
			}
		}
	}
#endif
}


void	Mapper116_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper116 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, ((Mapper116 *)mapper)->prg1, ((Mapper116 *)mapper)->prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( ((Mapper116 *)mapper)->prg0, ((Mapper116 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper116_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		if( ((Mapper116 *)mapper)->ExChrSwitch & 0x04 ) {
			INT	chrbank = 256;
			SetVROM_8K_Bank( chrbank+((Mapper116 *)mapper)->chr4, chrbank+((Mapper116 *)mapper)->chr5,
					 chrbank+((Mapper116 *)mapper)->chr6, chrbank+((Mapper116 *)mapper)->chr7,
					 ((Mapper116 *)mapper)->chr0, ((Mapper116 *)mapper)->chr1,
					 ((Mapper116 *)mapper)->chr2, ((Mapper116 *)mapper)->chr3 );
		} else {
			INT	chrbank = 0;
			SetVROM_8K_Bank( chrbank+((Mapper116 *)mapper)->chr4, chrbank+((Mapper116 *)mapper)->chr5,
					 chrbank+((Mapper116 *)mapper)->chr6, chrbank+((Mapper116 *)mapper)->chr7,
					 ((Mapper116 *)mapper)->chr0, ((Mapper116 *)mapper)->chr1,
					 ((Mapper116 *)mapper)->chr2, ((Mapper116 *)mapper)->chr3 );
		}

#if	0
		if( ((Mapper116 *)mapper)->reg[0] & 0x80 ) {
//			SetVROM_8K_Bank( chrbank+((Mapper116 *)mapper)->chr4, chrbank+((Mapper116 *)mapper)->chr5,
//					 chrbank+((Mapper116 *)mapper)->chr6, chrbank+((Mapper116 *)mapper)->chr7,
//					 chrbank+((Mapper116 *)mapper)->chr0, chrbank+((Mapper116 *)mapper)->chr1,
//					 chrbank+((Mapper116 *)mapper)->chr2, chrbank+((Mapper116 *)mapper)->chr3 );
			SetVROM_8K_Bank( chrbank+((Mapper116 *)mapper)->chr4, chrbank+((Mapper116 *)mapper)->chr5,
					 chrbank+((Mapper116 *)mapper)->chr6, chrbank+((Mapper116 *)mapper)->chr7,
					 ((Mapper116 *)mapper)->chr0, ((Mapper116 *)mapper)->chr1,
					 ((Mapper116 *)mapper)->chr2, ((Mapper116 *)mapper)->chr3 );
		} else {
			SetVROM_8K_Bank( ((Mapper116 *)mapper)->chr0, ((Mapper116 *)mapper)->chr1,
					 ((Mapper116 *)mapper)->chr2, ((Mapper116 *)mapper)->chr3,
					 chrbank+((Mapper116 *)mapper)->chr4, chrbank+((Mapper116 *)mapper)->chr5,
					 chrbank+((Mapper116 *)mapper)->chr6, chrbank+((Mapper116 *)mapper)->chr7 );
		}
#endif
	}
}

void	Mapper116_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper116 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper116 *)mapper)->prg0;
	p[ 9] = ((Mapper116 *)mapper)->prg1;
	p[10] = ((Mapper116 *)mapper)->prg2;
	p[11] = ((Mapper116 *)mapper)->prg3;
	p[12] = ((Mapper116 *)mapper)->chr0;
	p[13] = ((Mapper116 *)mapper)->chr1;
	p[14] = ((Mapper116 *)mapper)->chr2;
	p[15] = ((Mapper116 *)mapper)->chr3;
	p[16] = ((Mapper116 *)mapper)->chr4;
	p[17] = ((Mapper116 *)mapper)->chr5;
	p[18] = ((Mapper116 *)mapper)->chr6;
	p[19] = ((Mapper116 *)mapper)->chr7;
	p[20] = ((Mapper116 *)mapper)->irq_enable;
	p[21] = ((Mapper116 *)mapper)->irq_counter;
	p[22] = ((Mapper116 *)mapper)->irq_latch;
	p[23] = ((Mapper116 *)mapper)->ExPrgSwitch;
	p[24] = ((Mapper116 *)mapper)->prg0L;
	p[25] = ((Mapper116 *)mapper)->prg1L;
	p[26] = ((Mapper116 *)mapper)->ExChrSwitch;
}

void	Mapper116_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper116 *)mapper)->reg[i] = p[i];
	}
	((Mapper116 *)mapper)->prg0  = p[ 8];
	((Mapper116 *)mapper)->prg1  = p[ 9];
	((Mapper116 *)mapper)->prg2  = p[10];
	((Mapper116 *)mapper)->prg3  = p[11];
	((Mapper116 *)mapper)->chr0  = p[12];
	((Mapper116 *)mapper)->chr1  = p[13];
	((Mapper116 *)mapper)->chr2  = p[14];
	((Mapper116 *)mapper)->chr3  = p[15];
	((Mapper116 *)mapper)->chr4  = p[16];
	((Mapper116 *)mapper)->chr5  = p[17];
	((Mapper116 *)mapper)->chr6  = p[18];
	((Mapper116 *)mapper)->chr7  = p[19];
	((Mapper116 *)mapper)->irq_enable  = p[20];
	((Mapper116 *)mapper)->irq_counter = p[21];
	((Mapper116 *)mapper)->irq_latch   = p[22];
	((Mapper116 *)mapper)->ExPrgSwitch = p[23];
	((Mapper116 *)mapper)->prg0L = p[24];
	((Mapper116 *)mapper)->prg1L = p[25];
	((Mapper116 *)mapper)->ExChrSwitch = p[26];
}

BOOL	Mapper116_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper116_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper116));
	
	memset(mapper, 0, sizeof(Mapper116));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper116_Reset;
	mapper->vtbl.WriteLow = Mapper116_WriteLow;
	mapper->vtbl.Write = Mapper116_Write;
	mapper->vtbl.HSync = Mapper116_HSync;
	mapper->vtbl.SaveState = Mapper116_SaveState;
	mapper->vtbl.LoadState = Mapper116_LoadState;
	mapper->vtbl.IsStateSave = Mapper116_IsStateSave;

	return (Mapper *)mapper;
}

