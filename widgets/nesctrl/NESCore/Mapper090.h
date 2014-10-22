
//////////////////////////////////////////////////////////////////////////
// Mapper090  PC-JY-??                                                  //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;

	BYTE	prg_reg[4];
	BYTE	nth_reg[4], ntl_reg[4];
	BYTE	chh_reg[8], chl_reg[8];

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_occur;
	BYTE	irq_preset;
	BYTE	irq_offset;

	BYTE	prg_6000, prg_E000;
	BYTE	prg_size, chr_size;
	BYTE	mir_mode, mir_type;

	BYTE	key_val;
	BYTE	mul_val1, mul_val2;
	BYTE	sw_val;
}Mapper090;

void	Mapper090_SetBank_CPU(Mapper *mapper);
void	Mapper090_SetBank_PPU(Mapper *mapper);
void	Mapper090_SetBank_VRAM(Mapper *mapper);

void	Mapper090_Reset(Mapper *mapper)
{
	INT i;
	DWORD	crc;

	SetPROM_32K_Bank( PROM_8K_SIZE-4, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );

	((Mapper090 *)mapper)->patch = 0;

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	if( crc == 0x2a268152 ) {
		((Mapper090 *)mapper)->patch = 1;
	}
	if( crc == 0x2224b882 ) {
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}

	((Mapper090 *)mapper)->irq_enable = 0;	// Disable
	((Mapper090 *)mapper)->irq_counter = 0;
	((Mapper090 *)mapper)->irq_latch = 0;
	((Mapper090 *)mapper)->irq_occur = 0;
	((Mapper090 *)mapper)->irq_preset = 0;
	((Mapper090 *)mapper)->irq_offset = 0;

	((Mapper090 *)mapper)->prg_6000 = 0;
	((Mapper090 *)mapper)->prg_E000 = 0;
	((Mapper090 *)mapper)->prg_size = 0;
	((Mapper090 *)mapper)->chr_size = 0;
	((Mapper090 *)mapper)->mir_mode = 0;
	((Mapper090 *)mapper)->mir_type = 0;

	((Mapper090 *)mapper)->key_val = 0;
	((Mapper090 *)mapper)->mul_val1 = ((Mapper090 *)mapper)->mul_val2 = 0;

	for( i = 0; i < 4; i++ ) {
		((Mapper090 *)mapper)->prg_reg[i] = PROM_8K_SIZE-4+i;
		((Mapper090 *)mapper)->ntl_reg[i] = 0;
		((Mapper090 *)mapper)->nth_reg[i] = 0;
		((Mapper090 *)mapper)->chl_reg[i] = i;
		((Mapper090 *)mapper)->chh_reg[i] = 0;
		((Mapper090 *)mapper)->chl_reg[i+4] = i+4;
		((Mapper090 *)mapper)->chh_reg[i+4] = 0;
	}

	if( ((Mapper090 *)mapper)->sw_val )
		((Mapper090 *)mapper)->sw_val = 0x00;
	else
		((Mapper090 *)mapper)->sw_val = 0xFF;

//	NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
}

BYTE	Mapper090_ReadLow( Mapper *mapper, WORD addr )
{
DebugOut( "RD:%04X\n", addr );

	switch( addr ) {
		case	0x5000:
			return	((Mapper090 *)mapper)->sw_val?0x00:0xFF;
		case	0x5800:
			return	(BYTE)(((Mapper090 *)mapper)->mul_val1*((Mapper090 *)mapper)->mul_val2);
		case	0x5801:
			return	(BYTE)((((Mapper090 *)mapper)->mul_val1*((Mapper090 *)mapper)->mul_val2)>>8);
		case	0x5803:
			return	((Mapper090 *)mapper)->key_val;
	}

	if( addr >= 0x6000 ) {
		return	Mapper_DefReadLow(mapper,  addr );
	}

//	return	((Mapper090 *)mapper)->sw_val?0x00:0xFF;
	return	(BYTE)(addr>>8);
}

void	Mapper090_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
DebugOut( "WR:%04X %02X\n", addr, data );

	if( addr == 0x5800 ) {
		((Mapper090 *)mapper)->mul_val1 = data;
	} else
	if( addr == 0x5801 ) {
		((Mapper090 *)mapper)->mul_val2 = data;
	} else
	if( addr == 0x5803 ) {
		((Mapper090 *)mapper)->key_val = data;
	}
}

void	Mapper090_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF007 ) {
		case	0x8000:
		case	0x8001:
		case	0x8002:
		case	0x8003:
			((Mapper090 *)mapper)->prg_reg[addr&3] = data;
			Mapper090_SetBank_CPU( mapper);
			break;

		case	0x9000:
		case	0x9001:
		case	0x9002:
		case	0x9003:
		case	0x9004:
		case	0x9005:
		case	0x9006:
		case	0x9007:
			((Mapper090 *)mapper)->chl_reg[addr&7] = data;
			Mapper090_SetBank_PPU( mapper);
			break;

		case	0xA000:
		case	0xA001:
		case	0xA002:
		case	0xA003:
		case	0xA004:
		case	0xA005:
		case	0xA006:
		case	0xA007:
			((Mapper090 *)mapper)->chh_reg[addr&7] = data;
			Mapper090_SetBank_PPU( mapper);
			break;

		case	0xB000:
		case	0xB001:
		case	0xB002:
		case	0xB003:
			((Mapper090 *)mapper)->ntl_reg[addr&3] = data;
			Mapper090_SetBank_VRAM( mapper);
			break;

		case	0xB004:
		case	0xB005:
		case	0xB006:
		case	0xB007:
			((Mapper090 *)mapper)->nth_reg[addr&3] = data;
			Mapper090_SetBank_VRAM( mapper);
			break;

		case	0xC002:
			((Mapper090 *)mapper)->irq_enable = 0;
			((Mapper090 *)mapper)->irq_occur = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0xC003:
			((Mapper090 *)mapper)->irq_enable = 0xFF;
			((Mapper090 *)mapper)->irq_preset = 0xFF;
			break;
		case	0xC004:
			break;
		case	0xC005:
			if( ((Mapper090 *)mapper)->irq_offset & 0x80 ) {
				((Mapper090 *)mapper)->irq_latch = data ^ (((Mapper090 *)mapper)->irq_offset | 1);
			} else {
				((Mapper090 *)mapper)->irq_latch = data | (((Mapper090 *)mapper)->irq_offset&0x27);
			}
			((Mapper090 *)mapper)->irq_preset = 0xFF;
			break;
		case	0xC006:
			if( ((Mapper090 *)mapper)->patch ) {
				((Mapper090 *)mapper)->irq_offset = data;
			}
			break;

		case	0xD000:
			((Mapper090 *)mapper)->prg_6000 = data & 0x80;
			((Mapper090 *)mapper)->prg_E000 = data & 0x04;
			((Mapper090 *)mapper)->prg_size = data & 0x03;
			((Mapper090 *)mapper)->chr_size = (data & 0x18)>>3;
			((Mapper090 *)mapper)->mir_mode = data & 0x20;
			Mapper090_SetBank_CPU( mapper);
			Mapper090_SetBank_PPU( mapper);
			Mapper090_SetBank_VRAM( mapper);
			break;

		case	0xD001:
			((Mapper090 *)mapper)->mir_type = data & 0x03;
			Mapper090_SetBank_VRAM( mapper);
			break;

		case	0xD003:
			break;
	}
}

void	Mapper090_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(mapper->nes->ppu) ) {
			if( ((Mapper090 *)mapper)->irq_preset ) {
				((Mapper090 *)mapper)->irq_counter = ((Mapper090 *)mapper)->irq_latch;
				((Mapper090 *)mapper)->irq_preset = 0;
			}
			if( ((Mapper090 *)mapper)->irq_counter ) {
				((Mapper090 *)mapper)->irq_counter--;
			}
			if( !((Mapper090 *)mapper)->irq_counter ) {
				if( ((Mapper090 *)mapper)->irq_enable ) {
//					((Mapper090 *)mapper)->irq_occur = 0xFF;
					CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper090_Clock(Mapper *mapper, INT cycles)
{
//	if( ((Mapper090 *)mapper)->irq_occur ) {
//		mapper->nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper090_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper090 *)mapper)->prg_size == 0 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-4, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	} else
	if( ((Mapper090 *)mapper)->prg_size == 1 ) {
		SetPROM_32K_Bank( ((Mapper090 *)mapper)->prg_reg[1]*2, ((Mapper090 *)mapper)->prg_reg[1]*2+1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	} else
	if( ((Mapper090 *)mapper)->prg_size == 2 ) {
		if( ((Mapper090 *)mapper)->prg_E000 ) {
			SetPROM_32K_Bank( ((Mapper090 *)mapper)->prg_reg[0], ((Mapper090 *)mapper)->prg_reg[1], ((Mapper090 *)mapper)->prg_reg[2], ((Mapper090 *)mapper)->prg_reg[3] );
		} else {
			if( ((Mapper090 *)mapper)->prg_6000 ) {
				SetPROM_8K_Bank( 3, ((Mapper090 *)mapper)->prg_reg[3] );
			}
			SetPROM_32K_Bank( ((Mapper090 *)mapper)->prg_reg[0], ((Mapper090 *)mapper)->prg_reg[1], ((Mapper090 *)mapper)->prg_reg[2], PROM_8K_SIZE-1 );
		}
	} else {
		SetPROM_32K_Bank( ((Mapper090 *)mapper)->prg_reg[3], ((Mapper090 *)mapper)->prg_reg[2], ((Mapper090 *)mapper)->prg_reg[1], ((Mapper090 *)mapper)->prg_reg[0] );
	}
}

void	Mapper090_SetBank_PPU(Mapper *mapper)
{
INT	bank[8];
	INT i;

	for( i = 0; i < 8; i++ ) {
		bank[i] = ((INT)((Mapper090 *)mapper)->chh_reg[i]<<8)|((INT)((Mapper090 *)mapper)->chl_reg[i]);
	}

	if( ((Mapper090 *)mapper)->chr_size == 0 ) {
		SetVROM_8K_Bank_cont( bank[0] );
	} else
	if( ((Mapper090 *)mapper)->chr_size == 1 ) {
		SetVROM_4K_Bank( 0, bank[0] );
		SetVROM_4K_Bank( 4, bank[4] );
	} else
	if( ((Mapper090 *)mapper)->chr_size == 2 ) {
		SetVROM_2K_Bank( 0, bank[0] );
		SetVROM_2K_Bank( 2, bank[2] );
		SetVROM_2K_Bank( 4, bank[4] );
		SetVROM_2K_Bank( 6, bank[6] );
	} else {
		SetVROM_8K_Bank( bank[0], bank[1], bank[2], bank[3], bank[4], bank[5], bank[6], bank[7] );
	}
}

void	Mapper090_SetBank_VRAM(Mapper *mapper)
{
INT	bank[4];
	INT i;

	for( i = 0; i < 4; i++ ) {
		bank[i] = ((INT)((Mapper090 *)mapper)->nth_reg[i]<<8)|((INT)((Mapper090 *)mapper)->ntl_reg[i]);
	}

	if( !((Mapper090 *)mapper)->patch && ((Mapper090 *)mapper)->mir_mode ) {
		for( i = 0; i < 4; i++ ) {
			if( !((Mapper090 *)mapper)->nth_reg[i] && (((Mapper090 *)mapper)->ntl_reg[i] == (BYTE)i) ) {
				((Mapper090 *)mapper)->mir_mode = 0;
			}
		}

		if( ((Mapper090 *)mapper)->mir_mode ) {
			SetVROM_1K_Bank(  8, bank[0] );
			SetVROM_1K_Bank(  9, bank[1] );
			SetVROM_1K_Bank( 10, bank[2] );
			SetVROM_1K_Bank( 11, bank[3] );
		}
	} else {
		if( ((Mapper090 *)mapper)->mir_type == 0 ) {
			SetVRAM_Mirror_cont( VRAM_VMIRROR );
		} else
		if( ((Mapper090 *)mapper)->mir_type == 1 ) {
			SetVRAM_Mirror_cont( VRAM_HMIRROR );
		} else {
			SetVRAM_Mirror_cont( VRAM_MIRROR4L );
		}
	}
}

void	Mapper090_SaveState( Mapper *mapper, LPBYTE p )
{
INT	i;

	for( i = 0; i < 4; i++ ) {
		p[i] = ((Mapper090 *)mapper)->prg_reg[i];
	}
	for( i = 0; i < 8; i++ ) {
		p[i+ 4] = ((Mapper090 *)mapper)->chh_reg[i];
	}
	for( i = 0; i < 8; i++ ) {
		p[i+12] = ((Mapper090 *)mapper)->chl_reg[i];
	}
	for( i = 0; i < 4; i++ ) {
		p[i+20] = ((Mapper090 *)mapper)->nth_reg[i];
	}
	for( i = 0; i < 4; i++ ) {
		p[i+24] = ((Mapper090 *)mapper)->ntl_reg[i];
	}
	p[28] = ((Mapper090 *)mapper)->irq_enable;
	p[29] = ((Mapper090 *)mapper)->irq_counter;
	p[30] = ((Mapper090 *)mapper)->irq_latch;
	p[31] = ((Mapper090 *)mapper)->prg_6000;
	p[32] = ((Mapper090 *)mapper)->prg_E000;
	p[33] = ((Mapper090 *)mapper)->prg_size;
	p[34] = ((Mapper090 *)mapper)->chr_size;
	p[35] = ((Mapper090 *)mapper)->mir_mode;
	p[36] = ((Mapper090 *)mapper)->mir_type;
	p[37] = ((Mapper090 *)mapper)->mul_val1;
	p[38] = ((Mapper090 *)mapper)->mul_val2;
	p[39] = ((Mapper090 *)mapper)->key_val;
	p[40] = ((Mapper090 *)mapper)->irq_occur;
	p[41] = ((Mapper090 *)mapper)->irq_preset;
	p[42] = ((Mapper090 *)mapper)->irq_offset;
}

void	Mapper090_LoadState( Mapper *mapper, LPBYTE p )
{
INT	i;

	for( i = 0; i < 4; i++ ) {
		((Mapper090 *)mapper)->prg_reg[i] = p[i];
	}
	for( i = 0; i < 8; i++ ) {
		((Mapper090 *)mapper)->chh_reg[i] = p[i+ 4];
	}
	for( i = 0; i < 8; i++ ) {
		((Mapper090 *)mapper)->chl_reg[i] = p[i+12];
	}
	for( i = 0; i < 4; i++ ) {
		((Mapper090 *)mapper)->nth_reg[i] = p[i+20];
	}
	for( i = 0; i < 4; i++ ) {
		((Mapper090 *)mapper)->ntl_reg[i] = p[i+24];
	}
	((Mapper090 *)mapper)->irq_enable  = p[28];
	((Mapper090 *)mapper)->irq_counter = p[29];
	((Mapper090 *)mapper)->irq_latch   = p[30];
	((Mapper090 *)mapper)->prg_6000    = p[31];
	((Mapper090 *)mapper)->prg_E000    = p[32];
	((Mapper090 *)mapper)->prg_size    = p[33];
	((Mapper090 *)mapper)->chr_size    = p[34];
	((Mapper090 *)mapper)->mir_mode    = p[35];
	((Mapper090 *)mapper)->mir_type    = p[36];
	((Mapper090 *)mapper)->mul_val1    = p[37];
	((Mapper090 *)mapper)->mul_val2    = p[38];
	((Mapper090 *)mapper)->key_val     = p[39];
	((Mapper090 *)mapper)->irq_occur   = p[40];
	((Mapper090 *)mapper)->irq_preset  = p[41];
	((Mapper090 *)mapper)->irq_offset  = p[42];
}

BOOL	Mapper090_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper090_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper090));
	
	memset(mapper, 0, sizeof(Mapper090));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper090_Reset;
	mapper->vtbl.Write = Mapper090_Write;
	mapper->vtbl.ReadLow = Mapper090_ReadLow;
	mapper->vtbl.WriteLow = Mapper090_WriteLow;
	mapper->vtbl.HSync = Mapper090_HSync;
	mapper->vtbl.Clock = Mapper090_Clock;
	mapper->vtbl.SaveState = Mapper090_SaveState;
	mapper->vtbl.LoadState = Mapper090_LoadState;
	mapper->vtbl.IsStateSave = Mapper090_IsStateSave;

	return (Mapper *)mapper;
}

