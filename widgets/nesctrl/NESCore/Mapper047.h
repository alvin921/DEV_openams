
//////////////////////////////////////////////////////////////////////////
// Mapper047  NES-QJ                                                    //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	patch;
	BYTE	bank;
	BYTE	prg0, prg1;
	BYTE	chr01,chr23,chr4,chr5,chr6,chr7;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
}Mapper047;
	
void	Mapper047_SetBank_CPU(Mapper *mapper);
void	Mapper047_SetBank_PPU(Mapper *mapper);

void	Mapper047_Reset(Mapper *mapper)
{
	INT i;

	((Mapper047 *)mapper)->patch = 0;

	if( ROM_GetPROM_CRC(((Mapper047 *)mapper)->nes->rom) == 0x7eef434c ) {
		((Mapper047 *)mapper)->patch = 1;
	}

	for( i = 0; i < 8; i++ ) {
		((Mapper047 *)mapper)->reg[i] = 0;
	}

	((Mapper047 *)mapper)->bank = 0;
	((Mapper047 *)mapper)->prg0 = 0;
	((Mapper047 *)mapper)->prg1 = 1;

	// set VROM banks
	if( VROM_1K_SIZE ) {
		((Mapper047 *)mapper)->chr01 = 0;
		((Mapper047 *)mapper)->chr23 = 2;
		((Mapper047 *)mapper)->chr4  = 4;
		((Mapper047 *)mapper)->chr5  = 5;
		((Mapper047 *)mapper)->chr6  = 6;
		((Mapper047 *)mapper)->chr7  = 7;
	} else {
		((Mapper047 *)mapper)->chr01 = ((Mapper047 *)mapper)->chr23 = ((Mapper047 *)mapper)->chr4 = ((Mapper047 *)mapper)->chr5 = ((Mapper047 *)mapper)->chr6 = ((Mapper047 *)mapper)->chr7 = 0;
	}

	Mapper047_SetBank_CPU(mapper);
	Mapper047_SetBank_PPU(mapper);

	((Mapper047 *)mapper)->irq_enable = 0;
	((Mapper047 *)mapper)->irq_enable = 0;
	((Mapper047 *)mapper)->irq_latch = 0;
}

void	Mapper047_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x6000 ) {
		if( ((Mapper047 *)mapper)->patch ) {
			((Mapper047 *)mapper)->bank = (data & 0x06) >> 1;
		} else {
			((Mapper047 *)mapper)->bank = (data & 0x01) << 1;
		}
		Mapper047_SetBank_CPU(mapper);
		Mapper047_SetBank_PPU(mapper);
	}
}

void	Mapper047_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper047 *)mapper)->reg[0] = data;
			Mapper047_SetBank_CPU(mapper);
			Mapper047_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper047 *)mapper)->reg[1] = data;
			switch( ((Mapper047 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper047 *)mapper)->chr01 = data & 0xFE;
					Mapper047_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper047 *)mapper)->chr23 = data & 0xFE;
					Mapper047_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper047 *)mapper)->chr4 = data;
					Mapper047_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper047 *)mapper)->chr5 = data;
					Mapper047_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper047 *)mapper)->chr6 = data;
					Mapper047_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper047 *)mapper)->chr7 = data;
					Mapper047_SetBank_PPU(mapper);
					break;
				case	0x06:
					((Mapper047 *)mapper)->prg0 = data;
					Mapper047_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper047 *)mapper)->prg1 = data;
					Mapper047_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper047 *)mapper)->reg[2] = data;
			if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;
		case	0xA001:
			((Mapper047 *)mapper)->reg[3] = data;
			break;
		case	0xC000:
			((Mapper047 *)mapper)->reg[4] = data;
			((Mapper047 *)mapper)->irq_enable = data;
			break;
		case	0xC001:
			((Mapper047 *)mapper)->reg[5] = data;
			((Mapper047 *)mapper)->irq_latch = data;
			break;
		case	0xE000:
			((Mapper047 *)mapper)->reg[6] = data;
			((Mapper047 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( ((Mapper047 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper047 *)mapper)->reg[7] = data;
			((Mapper047 *)mapper)->irq_enable = 1;
			break;
	}
}

void	Mapper047_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(((Mapper047 *)mapper)->nes->ppu) ) {
			if( ((Mapper047 *)mapper)->irq_enable ) {
				if( !(--((Mapper047 *)mapper)->irq_enable) ) {
					((Mapper047 *)mapper)->irq_enable = ((Mapper047 *)mapper)->irq_latch;
//					CPU_IRQ(((Mapper047 *)mapper)->nes->cpu);
					CPU_SetIRQ( ((Mapper047 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper047_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper047 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_8K_Bank( 4, ((Mapper047 *)mapper)->bank * 8 + ((((Mapper047 *)mapper)->patch && ((Mapper047 *)mapper)->bank != 2)?6:14) );
		SetPROM_8K_Bank( 5, ((Mapper047 *)mapper)->bank * 8 + ((Mapper047 *)mapper)->prg1 );
		SetPROM_8K_Bank( 6, ((Mapper047 *)mapper)->bank * 8 + ((Mapper047 *)mapper)->prg0 );
		SetPROM_8K_Bank( 7, ((Mapper047 *)mapper)->bank * 8 + ((((Mapper047 *)mapper)->patch && ((Mapper047 *)mapper)->bank != 2)?7:15) );
	} else {
		SetPROM_8K_Bank( 4, ((Mapper047 *)mapper)->bank * 8 + ((Mapper047 *)mapper)->prg0 );
		SetPROM_8K_Bank( 5, ((Mapper047 *)mapper)->bank * 8 + ((Mapper047 *)mapper)->prg1 );
		SetPROM_8K_Bank( 6, ((Mapper047 *)mapper)->bank * 8 + ((((Mapper047 *)mapper)->patch && ((Mapper047 *)mapper)->bank != 2)?6:14) );
		SetPROM_8K_Bank( 7, ((Mapper047 *)mapper)->bank * 8 + ((((Mapper047 *)mapper)->patch && ((Mapper047 *)mapper)->bank != 2)?7:15) );
	}
}

void	Mapper047_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		if( ((Mapper047 *)mapper)->reg[0] & 0x80 ) {
			SetVROM_1K_Bank( 0, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr4 );
			SetVROM_1K_Bank( 1, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr5 );
			SetVROM_1K_Bank( 2, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr6 );
			SetVROM_1K_Bank( 3, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr7 );
			SetVROM_1K_Bank( 4, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr01 + 0 );
			SetVROM_1K_Bank( 5, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr01 + 1 );
			SetVROM_1K_Bank( 6, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr23 + 0 );
			SetVROM_1K_Bank( 7, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr23 + 1 );
		} else {
			SetVROM_1K_Bank( 0, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr01 + 0 );
			SetVROM_1K_Bank( 1, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr01 + 1 );
			SetVROM_1K_Bank( 2, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr23 + 0 );
			SetVROM_1K_Bank( 3, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr23 + 1 );
			SetVROM_1K_Bank( 4, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr4 );
			SetVROM_1K_Bank( 5, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr5 );
			SetVROM_1K_Bank( 6, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr6 );
			SetVROM_1K_Bank( 7, (((Mapper047 *)mapper)->bank & 0x02) * 64 + ((Mapper047 *)mapper)->chr7 );
		}
	}
}

void	Mapper047_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper047 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper047 *)mapper)->prg0;
	p[ 9] = ((Mapper047 *)mapper)->prg1;
	p[10] = ((Mapper047 *)mapper)->chr01;
	p[11] = ((Mapper047 *)mapper)->chr23;
	p[12] = ((Mapper047 *)mapper)->chr4;
	p[13] = ((Mapper047 *)mapper)->chr5;
	p[14] = ((Mapper047 *)mapper)->chr6;
	p[15] = ((Mapper047 *)mapper)->chr7;
	p[16] = ((Mapper047 *)mapper)->irq_enable;
	p[17] = ((Mapper047 *)mapper)->irq_enable;
	p[18] = ((Mapper047 *)mapper)->irq_latch;
	p[19] = ((Mapper047 *)mapper)->bank;
}

void	Mapper047_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper047 *)mapper)->reg[i] = p[i];
	}
	((Mapper047 *)mapper)->prg0  = p[ 8];
	((Mapper047 *)mapper)->prg1  = p[ 9];
	((Mapper047 *)mapper)->chr01 = p[10];
	((Mapper047 *)mapper)->chr23 = p[11];
	((Mapper047 *)mapper)->chr4  = p[12];
	((Mapper047 *)mapper)->chr5  = p[13];
	((Mapper047 *)mapper)->chr6  = p[14];
	((Mapper047 *)mapper)->chr7  = p[15];
	((Mapper047 *)mapper)->irq_enable  = p[16];
	((Mapper047 *)mapper)->irq_enable = p[17];
	((Mapper047 *)mapper)->irq_latch   = p[18];
	((Mapper047 *)mapper)->bank        = p[19];
}

BOOL	Mapper047_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper047_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper047));
	
	memset(mapper, 0, sizeof(Mapper047));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper047_Reset;
	mapper->vtbl.Write = Mapper047_Write;
	mapper->vtbl.WriteLow = Mapper047_WriteLow;
	mapper->vtbl.HSync = Mapper047_HSync;
	mapper->vtbl.SaveState = Mapper047_SaveState;
	mapper->vtbl.LoadState = Mapper047_LoadState;
	mapper->vtbl.IsStateSave = Mapper047_IsStateSave;

	return (Mapper *)mapper;
}


