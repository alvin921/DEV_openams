void	Mapper044_SetBank_CPU(Mapper *mapper);
void	Mapper044_SetBank_PPU(Mapper *mapper);

//////////////////////////////////////////////////////////////////////////
// Mapper044  Super HiK 7-in-1                                          //
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
}Mapper044;

void	Mapper044_Reset(Mapper *mapper)
{
	INT i;

	((Mapper044 *)mapper)->patch = 0;

	if( ROM_GetPROM_CRC(((Mapper044 *)mapper)->nes->rom) == 0x7eef434c ) {
		((Mapper044 *)mapper)->patch = 1;
	}

	for( i = 0; i < 8; i++ ) {
		((Mapper044 *)mapper)->reg[i] = 0;
	}

	((Mapper044 *)mapper)->bank = 0;
	((Mapper044 *)mapper)->prg0 = 0;
	((Mapper044 *)mapper)->prg1 = 1;

	// set VROM banks
	if( VROM_1K_SIZE ) {
		((Mapper044 *)mapper)->chr01 = 0;
		((Mapper044 *)mapper)->chr23 = 2;
		((Mapper044 *)mapper)->chr4  = 4;
		((Mapper044 *)mapper)->chr5  = 5;
		((Mapper044 *)mapper)->chr6  = 6;
		((Mapper044 *)mapper)->chr7  = 7;
	} else {
		((Mapper044 *)mapper)->chr01 = ((Mapper044 *)mapper)->chr23 = ((Mapper044 *)mapper)->chr4 = ((Mapper044 *)mapper)->chr5 = ((Mapper044 *)mapper)->chr6 = ((Mapper044 *)mapper)->chr7 = 0;
	}

	Mapper044_SetBank_CPU(mapper);
	Mapper044_SetBank_PPU(mapper);

	((Mapper044 *)mapper)->irq_enable = 0;
	((Mapper044 *)mapper)->irq_counter = 0;
	((Mapper044 *)mapper)->irq_latch = 0;
}

void	Mapper044_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x6000 ) {
		if( ((Mapper044 *)mapper)->patch ) {
			((Mapper044 *)mapper)->bank = (data & 0x06) >> 1;
		} else {
			((Mapper044 *)mapper)->bank = (data & 0x01) << 1;
		}
		Mapper044_SetBank_CPU(mapper);
		Mapper044_SetBank_PPU(mapper);
	}
}

void	Mapper044_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper044 *)mapper)->reg[0] = data;
			Mapper044_SetBank_CPU(mapper);
			Mapper044_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper044 *)mapper)->reg[1] = data;
			switch( ((Mapper044 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper044 *)mapper)->chr01 = data & 0xFE;
					Mapper044_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper044 *)mapper)->chr23 = data & 0xFE;
					Mapper044_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper044 *)mapper)->chr4 = data;
					Mapper044_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper044 *)mapper)->chr5 = data;
					Mapper044_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper044 *)mapper)->chr6 = data;
					Mapper044_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper044 *)mapper)->chr7 = data;
					Mapper044_SetBank_PPU(mapper);
					break;
				case	0x06:
					((Mapper044 *)mapper)->prg0 = data;
					Mapper044_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper044 *)mapper)->prg1 = data;
					Mapper044_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper044 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(((Mapper044 *)mapper)->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper044 *)mapper)->reg[3] = data;
			((Mapper044 *)mapper)->bank = data & 0x07;
			if( ((Mapper044 *)mapper)->bank == 7 ) {
				((Mapper044 *)mapper)->bank = 6;
			}
			Mapper044_SetBank_CPU(mapper);
			Mapper044_SetBank_PPU(mapper);
			break;
		case	0xC000:
			((Mapper044 *)mapper)->reg[4] = data;
			((Mapper044 *)mapper)->irq_counter = data;
			break;
		case	0xC001:
			((Mapper044 *)mapper)->reg[5] = data;
			((Mapper044 *)mapper)->irq_latch = data;
			break;
		case	0xE000:
			((Mapper044 *)mapper)->reg[6] = data;
			((Mapper044 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( ((Mapper044 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper044 *)mapper)->reg[7] = data;
			((Mapper044 *)mapper)->irq_enable = 1;
//			((Mapper044 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper044_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(((Mapper044 *)mapper)->nes->ppu) ) {
			if( ((Mapper044 *)mapper)->irq_enable ) {
				if( !(--((Mapper044 *)mapper)->irq_counter) ) {
					((Mapper044 *)mapper)->irq_counter = ((Mapper044 *)mapper)->irq_latch;
//					CPU_IRQ(((Mapper044 *)mapper)->nes->cpu);
					CPU_SetIRQ( ((Mapper044 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper044_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper044 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_8K_Bank( 4, ((((Mapper044 *)mapper)->bank == 6)?0x1e:0x0e)|(((Mapper044 *)mapper)->bank<<4) );
		SetPROM_8K_Bank( 5, ((((Mapper044 *)mapper)->bank == 6)?0x1f&((Mapper044 *)mapper)->prg1:0x0f&((Mapper044 *)mapper)->prg1)|(((Mapper044 *)mapper)->bank<<4) );
		SetPROM_8K_Bank( 6, ((((Mapper044 *)mapper)->bank == 6)?0x1f&((Mapper044 *)mapper)->prg0:0x0f&((Mapper044 *)mapper)->prg0)|(((Mapper044 *)mapper)->bank<<4) );
		SetPROM_8K_Bank( 7, ((((Mapper044 *)mapper)->bank == 6)?0x1f:0x0f)|(((Mapper044 *)mapper)->bank<<4) );
	} else {
		SetPROM_8K_Bank( 4, ((((Mapper044 *)mapper)->bank == 6)?0x1f&((Mapper044 *)mapper)->prg0:0x0f&((Mapper044 *)mapper)->prg0)|(((Mapper044 *)mapper)->bank<<4) );
		SetPROM_8K_Bank( 5, ((((Mapper044 *)mapper)->bank == 6)?0x1f&((Mapper044 *)mapper)->prg1:0x0f&((Mapper044 *)mapper)->prg1)|(((Mapper044 *)mapper)->bank<<4) );
		SetPROM_8K_Bank( 6, ((((Mapper044 *)mapper)->bank == 6)?0x1e:0x0e)|(((Mapper044 *)mapper)->bank<<4) );
		SetPROM_8K_Bank( 7, ((((Mapper044 *)mapper)->bank == 6)?0x1f:0x0f)|(((Mapper044 *)mapper)->bank<<4) );
	}
}

void	Mapper044_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		if( ((Mapper044 *)mapper)->reg[0] & 0x80 ) {
			SetVROM_1K_Bank( 0, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr4:0x7f&((Mapper044 *)mapper)->chr4)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 1, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr5:0x7f&((Mapper044 *)mapper)->chr5)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 2, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr6:0x7f&((Mapper044 *)mapper)->chr6)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 3, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr7:0x7f&((Mapper044 *)mapper)->chr7)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 4, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr01:0x7f&((Mapper044 *)mapper)->chr01)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 5, ((((Mapper044 *)mapper)->bank == 6)?0xff&(((Mapper044 *)mapper)->chr01+1):0x7f&(((Mapper044 *)mapper)->chr01+1))|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 6, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr23:0x7f&((Mapper044 *)mapper)->chr23)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 7, ((((Mapper044 *)mapper)->bank == 6)?0xff&(((Mapper044 *)mapper)->chr23+1):0x7f&(((Mapper044 *)mapper)->chr23+1))|(((Mapper044 *)mapper)->bank<<7) );
		} else {
			SetVROM_1K_Bank( 0, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr01:0x7f&((Mapper044 *)mapper)->chr01)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 1, ((((Mapper044 *)mapper)->bank == 6)?0xff&(((Mapper044 *)mapper)->chr01+1):0x7f&(((Mapper044 *)mapper)->chr01+1))|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 2, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr23:0x7f&((Mapper044 *)mapper)->chr23)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 3, ((((Mapper044 *)mapper)->bank == 6)?0xff&(((Mapper044 *)mapper)->chr23+1):0x7f&(((Mapper044 *)mapper)->chr23+1))|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 4, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr4:0x7f&((Mapper044 *)mapper)->chr4)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 5, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr5:0x7f&((Mapper044 *)mapper)->chr5)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 6, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr6:0x7f&((Mapper044 *)mapper)->chr6)|(((Mapper044 *)mapper)->bank<<7) );
			SetVROM_1K_Bank( 7, ((((Mapper044 *)mapper)->bank == 6)?0xff&((Mapper044 *)mapper)->chr7:0x7f&((Mapper044 *)mapper)->chr7)|(((Mapper044 *)mapper)->bank<<7) );
		}
	}
}

void	Mapper044_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper044 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper044 *)mapper)->prg0;
	p[ 9] = ((Mapper044 *)mapper)->prg1;
	p[10] = ((Mapper044 *)mapper)->chr01;
	p[11] = ((Mapper044 *)mapper)->chr23;
	p[12] = ((Mapper044 *)mapper)->chr4;
	p[13] = ((Mapper044 *)mapper)->chr5;
	p[14] = ((Mapper044 *)mapper)->chr6;
	p[15] = ((Mapper044 *)mapper)->chr7;
	p[16] = ((Mapper044 *)mapper)->irq_enable;
	p[17] = ((Mapper044 *)mapper)->irq_counter;
	p[18] = ((Mapper044 *)mapper)->irq_latch;
	p[19] = ((Mapper044 *)mapper)->bank;
}

void	Mapper044_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper044 *)mapper)->reg[i] = p[i];
	}
	((Mapper044 *)mapper)->prg0  = p[ 8];
	((Mapper044 *)mapper)->prg1  = p[ 9];
	((Mapper044 *)mapper)->chr01 = p[10];
	((Mapper044 *)mapper)->chr23 = p[11];
	((Mapper044 *)mapper)->chr4  = p[12];
	((Mapper044 *)mapper)->chr5  = p[13];
	((Mapper044 *)mapper)->chr6  = p[14];
	((Mapper044 *)mapper)->chr7  = p[15];
	((Mapper044 *)mapper)->irq_enable  = p[16];
	((Mapper044 *)mapper)->irq_counter = p[17];
	((Mapper044 *)mapper)->irq_latch   = p[18];
	((Mapper044 *)mapper)->bank        = p[19];
}

BOOL	Mapper044_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper044_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper044));
	
	memset(mapper, 0, sizeof(Mapper044));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper044_Reset;
	mapper->vtbl.Write = Mapper044_Write;
	mapper->vtbl.WriteLow = Mapper044_WriteLow;
	mapper->vtbl.HSync = Mapper044_HSync;
	mapper->vtbl.SaveState = Mapper044_SaveState;
	mapper->vtbl.LoadState = Mapper044_LoadState;
	mapper->vtbl.IsStateSave = Mapper044_IsStateSave;

	return (Mapper *)mapper;
}

