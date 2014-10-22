//////////////////////////////////////////////////////////////////////////
// Mapper118  IQS MMC3                                                  //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
}Mapper118;
void	Mapper118_SetBank_CPU(Mapper *mapper);
void	Mapper118_SetBank_PPU(Mapper *mapper);

void	Mapper118_Reset(Mapper *mapper)
{
INT	i;

	for( i = 0; i < 8; i++ ) {
		((Mapper118 *)mapper)->reg[i] = 0x00;
	}

	((Mapper118 *)mapper)->prg0 = 0;
	((Mapper118 *)mapper)->prg1 = 1;
	Mapper118_SetBank_CPU(mapper);

	if( VROM_1K_SIZE ) {
		((Mapper118 *)mapper)->chr01 = 0;
		((Mapper118 *)mapper)->chr23 = 2;
		((Mapper118 *)mapper)->chr4  = 4;
		((Mapper118 *)mapper)->chr5  = 5;
		((Mapper118 *)mapper)->chr6  = 6;
		((Mapper118 *)mapper)->chr7  = 7;

		Mapper118_SetBank_PPU(mapper);
	} else {
		((Mapper118 *)mapper)->chr01 = 0;
		((Mapper118 *)mapper)->chr23 = 0;
		((Mapper118 *)mapper)->chr4  = 0;
		((Mapper118 *)mapper)->chr5  = 0;
		((Mapper118 *)mapper)->chr6  = 0;
		((Mapper118 *)mapper)->chr7  = 0;
	}

	((Mapper118 *)mapper)->we_sram  = 0;	// Disable
	((Mapper118 *)mapper)->irq_enable = 0;	// Disable
	((Mapper118 *)mapper)->irq_counter = 0;
	((Mapper118 *)mapper)->irq_latch = 0;
}

void	Mapper118_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper118 *)mapper)->reg[0] = data;
			Mapper118_SetBank_CPU(mapper);
			Mapper118_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper118 *)mapper)->reg[1] = data;

			if( (((Mapper118 *)mapper)->reg[0] & 0x80) ) {
				if( (((Mapper118 *)mapper)->reg[0] & 0x07) == 2 ) {
					if( data & 0x80 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
					else		  SetVRAM_Mirror_cont( VRAM_MIRROR4H );
				}
			} else {
				if( (((Mapper118 *)mapper)->reg[0] & 0x07) == 0 ) {
					if( data & 0x80 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
					else		  SetVRAM_Mirror_cont( VRAM_MIRROR4H );
				}
			}

			switch( ((Mapper118 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					if( VROM_1K_SIZE ) {
						((Mapper118 *)mapper)->chr01 = data & 0xFE;
						Mapper118_SetBank_PPU(mapper);
					}
					break;
				case	0x01:
					if( VROM_1K_SIZE ) {
						((Mapper118 *)mapper)->chr23 = data & 0xFE;
						Mapper118_SetBank_PPU(mapper);
					}
					break;
				case	0x02:
					if( VROM_1K_SIZE ) {
						((Mapper118 *)mapper)->chr4 = data;
						Mapper118_SetBank_PPU(mapper);
					}
					break;
				case	0x03:
					if( VROM_1K_SIZE ) {
						((Mapper118 *)mapper)->chr5 = data;
						Mapper118_SetBank_PPU(mapper);
					}
					break;
				case	0x04:
					if( VROM_1K_SIZE ) {
						((Mapper118 *)mapper)->chr6 = data;
						Mapper118_SetBank_PPU(mapper);
					}
					break;
				case	0x05:
					if( VROM_1K_SIZE ) {
						((Mapper118 *)mapper)->chr7 = data;
						Mapper118_SetBank_PPU(mapper);
					}
					break;
				case	0x06:
					((Mapper118 *)mapper)->prg0 = data;
					Mapper118_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper118 *)mapper)->prg1 = data;
					Mapper118_SetBank_CPU(mapper);
					break;
			}
			break;

		case	0xC000:
			((Mapper118 *)mapper)->reg[4] = data;
			((Mapper118 *)mapper)->irq_counter = data;
			break;
		case	0xC001:
			((Mapper118 *)mapper)->reg[5] = data;
			((Mapper118 *)mapper)->irq_latch = data;
			break;
		case	0xE000:
			((Mapper118 *)mapper)->reg[6] = data;
			((Mapper118 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( ((Mapper118 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper118 *)mapper)->reg[7] = data;
			((Mapper118 *)mapper)->irq_enable = 1;
			break;
	}
}

void	Mapper118_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(((Mapper118 *)mapper)->nes->ppu) ) {
			if( ((Mapper118 *)mapper)->irq_enable ) {
				if( !(((Mapper118 *)mapper)->irq_counter--) ) {
					((Mapper118 *)mapper)->irq_counter = ((Mapper118 *)mapper)->irq_latch;
//					CPU_IRQ(((Mapper118 *)mapper)->nes->cpu);
					CPU_SetIRQ( ((Mapper118 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper118_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper118 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, ((Mapper118 *)mapper)->prg1, ((Mapper118 *)mapper)->prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( ((Mapper118 *)mapper)->prg0, ((Mapper118 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper118_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		if( ((Mapper118 *)mapper)->reg[0] & 0x80 ) {
			SetVROM_8K_Bank( ((Mapper118 *)mapper)->chr4, ((Mapper118 *)mapper)->chr5, ((Mapper118 *)mapper)->chr6, ((Mapper118 *)mapper)->chr7,
					 ((Mapper118 *)mapper)->chr01, ((Mapper118 *)mapper)->chr01+1, ((Mapper118 *)mapper)->chr23, ((Mapper118 *)mapper)->chr23+1 );
		} else {
			SetVROM_8K_Bank( ((Mapper118 *)mapper)->chr01, ((Mapper118 *)mapper)->chr01+1, ((Mapper118 *)mapper)->chr23, ((Mapper118 *)mapper)->chr23+1,
					 ((Mapper118 *)mapper)->chr4, ((Mapper118 *)mapper)->chr5, ((Mapper118 *)mapper)->chr6, ((Mapper118 *)mapper)->chr7 );
		}
	}
}

void	Mapper118_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper118 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper118 *)mapper)->prg0;
	p[ 9] = ((Mapper118 *)mapper)->prg1;
	p[10] = ((Mapper118 *)mapper)->chr01;
	p[11] = ((Mapper118 *)mapper)->chr23;
	p[12] = ((Mapper118 *)mapper)->chr4;
	p[13] = ((Mapper118 *)mapper)->chr5;
	p[14] = ((Mapper118 *)mapper)->chr6;
	p[15] = ((Mapper118 *)mapper)->chr7;
	p[16] = ((Mapper118 *)mapper)->irq_enable;
	p[17] = ((Mapper118 *)mapper)->irq_counter;
	p[18] = ((Mapper118 *)mapper)->irq_latch;
}

void	Mapper118_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper118 *)mapper)->reg[i] = p[i];
	}
	((Mapper118 *)mapper)->prg0  = p[ 8];
	((Mapper118 *)mapper)->prg1  = p[ 9];
	((Mapper118 *)mapper)->chr01 = p[10];
	((Mapper118 *)mapper)->chr23 = p[11];
	((Mapper118 *)mapper)->chr4  = p[12];
	((Mapper118 *)mapper)->chr5  = p[13];
	((Mapper118 *)mapper)->chr6  = p[14];
	((Mapper118 *)mapper)->chr7  = p[15];
	((Mapper118 *)mapper)->irq_enable  = p[16];
	((Mapper118 *)mapper)->irq_counter = p[17];
	((Mapper118 *)mapper)->irq_latch   = p[18];
}

BOOL	Mapper118_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper118_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper118));
	
	memset(mapper, 0, sizeof(Mapper118));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper118_Reset;
	mapper->vtbl.Write = Mapper118_Write;
	mapper->vtbl.HSync = Mapper118_HSync;
	mapper->vtbl.SaveState = Mapper118_SaveState;
	mapper->vtbl.LoadState = Mapper118_LoadState;
	mapper->vtbl.IsStateSave = Mapper118_IsStateSave;

	return (Mapper *)mapper;
}


