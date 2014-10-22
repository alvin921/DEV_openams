
//////////////////////////////////////////////////////////////////////////
// Mapper100  Nesticle MMC3                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	prg0, prg1, prg2, prg3;
	BYTE	chr0, chr1, chr2, chr3, chr4, chr5, chr6, chr7;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
}Mapper100;
void	Mapper100_SetBank_CPU(Mapper *mapper);
void	Mapper100_SetBank_PPU(Mapper *mapper);

void	Mapper100_Reset(Mapper *mapper)
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper100 *)mapper)->reg[i] = 0x00;
	}

	((Mapper100 *)mapper)->prg0 = 0;
	((Mapper100 *)mapper)->prg1 = 1;
	((Mapper100 *)mapper)->prg2 = PROM_8K_SIZE-2;
	((Mapper100 *)mapper)->prg3 = PROM_8K_SIZE-1;
	Mapper100_SetBank_CPU(mapper);

	if( VROM_1K_SIZE ) {
		((Mapper100 *)mapper)->chr0 = 0;
		((Mapper100 *)mapper)->chr1 = 1;
		((Mapper100 *)mapper)->chr2 = 2;
		((Mapper100 *)mapper)->chr3 = 3;
		((Mapper100 *)mapper)->chr4 = 4;
		((Mapper100 *)mapper)->chr5 = 5;
		((Mapper100 *)mapper)->chr6 = 6;
		((Mapper100 *)mapper)->chr7 = 7;
		Mapper100_SetBank_PPU(mapper);
	} else {
		((Mapper100 *)mapper)->chr0 = ((Mapper100 *)mapper)->chr2 = ((Mapper100 *)mapper)->chr4 = ((Mapper100 *)mapper)->chr5 = ((Mapper100 *)mapper)->chr6 = ((Mapper100 *)mapper)->chr7 = 0;
		((Mapper100 *)mapper)->chr1 = ((Mapper100 *)mapper)->chr3 = 1;
	}

	((Mapper100 *)mapper)->irq_enable = 0;	// Disable
	((Mapper100 *)mapper)->irq_counter = 0;
	((Mapper100 *)mapper)->irq_latch = 0;
}

void	Mapper100_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper100 *)mapper)->reg[0] = data;
			break;
		case	0x8001:
			((Mapper100 *)mapper)->reg[1] = data;

			switch( ((Mapper100 *)mapper)->reg[0] & 0xC7 ) {
				case	0x00:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr0 = data&0xFE;
						((Mapper100 *)mapper)->chr1 = ((Mapper100 *)mapper)->chr0+1;
						Mapper100_SetBank_PPU(mapper);
					}
					break;
				case	0x01:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr2 = data&0xFE;
						((Mapper100 *)mapper)->chr3 = ((Mapper100 *)mapper)->chr2+1;
						Mapper100_SetBank_PPU(mapper);
					}
					break;
				case	0x02:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr4 = data;
						Mapper100_SetBank_PPU(mapper);
					}
					break;
				case	0x03:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr5 = data;
						Mapper100_SetBank_PPU(mapper);
					}
					break;
				case	0x04:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr6 = data;
						Mapper100_SetBank_PPU(mapper);
					}
					break;
				case	0x05:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr7 = data;
						Mapper100_SetBank_PPU(mapper);
					}
					break;

				case	0x06:
					((Mapper100 *)mapper)->prg0 = data;
					Mapper100_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper100 *)mapper)->prg1 = data;
					Mapper100_SetBank_CPU(mapper);
					break;
				case	0x46:
					((Mapper100 *)mapper)->prg2 = data;
					Mapper100_SetBank_CPU(mapper);
					break;
				case	0x47:
					((Mapper100 *)mapper)->prg3 = data;
					Mapper100_SetBank_CPU(mapper);
					break;

				case	0x80:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr4 = data&0xFE;
						((Mapper100 *)mapper)->chr5 = ((Mapper100 *)mapper)->chr4+1;
						Mapper100_SetBank_PPU(mapper);
					}
					break;
				case	0x81:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr6 = data&0xFE;
						((Mapper100 *)mapper)->chr7 = ((Mapper100 *)mapper)->chr6+1;
						Mapper100_SetBank_PPU(mapper);
					}
					break;
				case	0x82:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr0 = data;
						Mapper100_SetBank_PPU(mapper);
					}
					break;
				case	0x83:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr1 = data;
						Mapper100_SetBank_PPU(mapper);
					}
					break;
				case	0x84:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr2 = data;
						Mapper100_SetBank_PPU(mapper);
					}
					break;
				case	0x85:
					if( VROM_1K_SIZE ) {
						((Mapper100 *)mapper)->chr3 = data;
						Mapper100_SetBank_PPU(mapper);
					}
					break;

			}
			break;
		case	0xA000:
			((Mapper100 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(((Mapper100 *)mapper)->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper100 *)mapper)->reg[3] = data;
			break;
		case	0xC000:
			((Mapper100 *)mapper)->reg[4] = data;
			((Mapper100 *)mapper)->irq_counter = data;
			break;
		case	0xC001:
			((Mapper100 *)mapper)->reg[5] = data;
			((Mapper100 *)mapper)->irq_latch = data;
			break;
		case	0xE000:
			((Mapper100 *)mapper)->reg[6] = data;
			((Mapper100 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( ((Mapper100 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper100 *)mapper)->reg[7] = data;
			((Mapper100 *)mapper)->irq_enable = 0xFF;
			break;
	}
}

void	Mapper100_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(((Mapper100 *)mapper)->nes->ppu) ) {
			if( ((Mapper100 *)mapper)->irq_enable ) {
				if( !(((Mapper100 *)mapper)->irq_counter--) ) {
					((Mapper100 *)mapper)->irq_counter = ((Mapper100 *)mapper)->irq_latch;
//					CPU_IRQ(((Mapper100 *)mapper)->nes->cpu);
					CPU_SetIRQ( ((Mapper100 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper100_SetBank_CPU(Mapper *mapper)
{
	SetPROM_32K_Bank( ((Mapper100 *)mapper)->prg0, ((Mapper100 *)mapper)->prg1, ((Mapper100 *)mapper)->prg2, ((Mapper100 *)mapper)->prg3 );
}

void	Mapper100_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( ((Mapper100 *)mapper)->chr0, ((Mapper100 *)mapper)->chr1, ((Mapper100 *)mapper)->chr2, ((Mapper100 *)mapper)->chr3, ((Mapper100 *)mapper)->chr4, ((Mapper100 *)mapper)->chr5, ((Mapper100 *)mapper)->chr6, ((Mapper100 *)mapper)->chr7 );
	}
}

void	Mapper100_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper100 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper100 *)mapper)->prg0;
	p[ 9] = ((Mapper100 *)mapper)->prg1;
	p[10] = ((Mapper100 *)mapper)->prg2;
	p[11] = ((Mapper100 *)mapper)->prg3;
	p[12] = ((Mapper100 *)mapper)->chr0;
	p[13] = ((Mapper100 *)mapper)->chr1;
	p[14] = ((Mapper100 *)mapper)->chr2;
	p[15] = ((Mapper100 *)mapper)->chr3;
	p[16] = ((Mapper100 *)mapper)->chr4;
	p[17] = ((Mapper100 *)mapper)->chr5;
	p[18] = ((Mapper100 *)mapper)->chr6;
	p[19] = ((Mapper100 *)mapper)->chr7;
	p[20] = ((Mapper100 *)mapper)->irq_enable;
	p[21] = ((Mapper100 *)mapper)->irq_counter;
	p[22] = ((Mapper100 *)mapper)->irq_latch;
}

void	Mapper100_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper100 *)mapper)->reg[i] = p[i];
	}
	((Mapper100 *)mapper)->prg0  = p[ 8];
	((Mapper100 *)mapper)->prg1  = p[ 9];
	((Mapper100 *)mapper)->prg2  = p[10];
	((Mapper100 *)mapper)->prg3  = p[11];
	((Mapper100 *)mapper)->chr0  = p[12];
	((Mapper100 *)mapper)->chr1  = p[13];
	((Mapper100 *)mapper)->chr2  = p[14];
	((Mapper100 *)mapper)->chr3  = p[15];
	((Mapper100 *)mapper)->chr4  = p[16];
	((Mapper100 *)mapper)->chr5  = p[17];
	((Mapper100 *)mapper)->chr6  = p[18];
	((Mapper100 *)mapper)->chr7  = p[19];
	((Mapper100 *)mapper)->irq_enable  = p[20];
	((Mapper100 *)mapper)->irq_counter = p[21];
	((Mapper100 *)mapper)->irq_latch   = p[22];
}

BOOL	Mapper100_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper100_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper100));
	
	memset(mapper, 0, sizeof(Mapper100));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper100_Reset;
	mapper->vtbl.Write = Mapper100_Write;
	mapper->vtbl.HSync = Mapper100_HSync;
	mapper->vtbl.SaveState = Mapper100_SaveState;
	mapper->vtbl.LoadState = Mapper100_LoadState;
	mapper->vtbl.IsStateSave = Mapper100_IsStateSave;

	return (Mapper *)mapper;
}


