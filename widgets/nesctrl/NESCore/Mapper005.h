//////////////////////////////////////////////////////////////////////////
// Mapper005  Nintendo MMC5                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	sram_size;

	BYTE	prg_size;		// $5100
	BYTE	chr_size;		// $5101
	BYTE	sram_we_a, sram_we_b;	// $5102-$5103
	BYTE	graphic_mode;		// $5104
	BYTE	nametable_mode;		// $5105
	BYTE	nametable_type[4];	// $5105 use

	BYTE	sram_page;		// $5113

	BYTE	fill_chr, fill_pal;	// $5106-$5107
	BYTE	split_control;		// $5200
	BYTE	split_scroll;		// $5201
	BYTE	split_page;		// $5202

	BYTE	split_x;
	WORD	split_addr;
	WORD	split_yofs;

	BYTE	chr_type;
	BYTE	chr_mode;		// $5120-$512B use
	BYTE	chr_page[2][8];		// $5120-$512B
	LPBYTE	BG_MEM_BANK[8];		// BGパターン用バンク
	BYTE	BG_MEM_PAGE[8];

	BYTE	irq_status;		// $5204(R)
	BYTE	irq_enable;		// $5204(W)
	BYTE	irq_line;		// $5203
	BYTE	irq_scanline;
	BYTE	irq_clear;		// HSyncで使用
	BYTE	irq_type;

	BYTE	mult_a, mult_b;		// $5205-$5206
}Mapper005;

void	Mapper005_SetBank_CPU( Mapper *mapper, WORD addr, BYTE data );
void	Mapper005_SetBank_SRAM( Mapper *mapper, BYTE page, BYTE data );
void	Mapper005_SetBank_PPU(Mapper *mapper);

#define	MMC5_IRQ_METAL		(1<<0)

void	Mapper005_Reset(Mapper *mapper)
{
INT	i;
	DWORD	crc;

	((Mapper005 *)mapper)->prg_size = 3;
	((Mapper005 *)mapper)->chr_size = 3;

	((Mapper005 *)mapper)->sram_we_a = 0x00;
	((Mapper005 *)mapper)->sram_we_b = 0x00;

	((Mapper005 *)mapper)->graphic_mode = 0;
	((Mapper005 *)mapper)->nametable_mode = 0;

	for( i = 0; i < 4; i++ ) {
		((Mapper005 *)mapper)->nametable_type[i] = 0;
	}

	((Mapper005 *)mapper)->fill_chr = ((Mapper005 *)mapper)->fill_pal = 0;
	((Mapper005 *)mapper)->split_control = ((Mapper005 *)mapper)->split_scroll = ((Mapper005 *)mapper)->split_page = 0;

	((Mapper005 *)mapper)->irq_enable = 0;
	((Mapper005 *)mapper)->irq_status = 0;
	((Mapper005 *)mapper)->irq_scanline = 0;
	((Mapper005 *)mapper)->irq_line = 0;
	((Mapper005 *)mapper)->irq_clear = 0;

	((Mapper005 *)mapper)->irq_type = 0;

	((Mapper005 *)mapper)->mult_a = ((Mapper005 *)mapper)->mult_b = 0;

	((Mapper005 *)mapper)->chr_type = 0;
	((Mapper005 *)mapper)->chr_mode = 0;
	for( i = 0; i < 8; i++ ) {
		((Mapper005 *)mapper)->chr_page[0][i] = i;
		((Mapper005 *)mapper)->chr_page[1][i] = 4+(i&0x03);
	}

	SetPROM_32K_Bank( PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );

	for( i = 0; i < 8; i++ ) {
		((Mapper005 *)mapper)->BG_MEM_BANK[i] = VROM+0x0400*i;
		((Mapper005 *)mapper)->BG_MEM_PAGE[i] = i;
	}

	// SRAM設定
	Mapper005_SetBank_SRAM( mapper, 3, 0 );

	((Mapper005 *)mapper)->sram_size = 0;
	NES_SetSAVERAM_SIZE( mapper->nes, 16*1024 );

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	if( crc == 0x2b548d75	// Bandit Kings of Ancient China(U)
	 || crc == 0xf4cd4998	// Dai Koukai Jidai(J)
	 || crc == 0x8fa95456	// Ishin no Arashi(J)
	 || crc == 0x98c8e090	// Nobunaga no Yabou - Sengoku Gunyuu Den(J)
	 || crc == 0x8e9a5e2f	// L'Empereur(Alt)(U)
	 || crc == 0x57e3218b	// L'Empereur(U)
	 || crc == 0x2f50bd38	// L'Empereur(J)
	 || crc == 0xb56958d1	// Nobunaga's Ambition 2(U)
	 || crc == 0xe6c28c5f	// Suikoden - Tenmei no Chikai(J)
	 || crc == 0xcd35e2e9 ) {	// Uncharted Waters(U)
		((Mapper005 *)mapper)->sram_size = 1;
		NES_SetSAVERAM_SIZE( mapper->nes, 32*1024 );
	} else
	if( crc == 0xf4120e58	// Aoki Ookami to Shiroki Mejika - Genchou Hishi(J)
	 || crc == 0x286613d8	// Nobunaga no Yabou - Bushou Fuuun Roku(J)
	 || crc == 0x11eaad26	// Romance of the Three Kingdoms 2(U)
	 || crc == 0x95ba5733 ) {	// Sangokushi 2(J)
		((Mapper005 *)mapper)->sram_size = 2;
		NES_SetSAVERAM_SIZE( mapper->nes, 64*1024 );
	}

	if( crc == 0x95ca9ec7 ) { // Castlevania 3 - Dracula's Curse(U)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}

	if( crc == 0xcd9acf43 ) { // Metal Slader Glory(J)
		((Mapper005 *)mapper)->irq_type = MMC5_IRQ_METAL;
	}

	if( crc == 0xe91548d8 ) { // Shin 4 Nin Uchi Mahjong - Yakuman Tengoku(J)
		((Mapper005 *)mapper)->chr_type = 1;
	}

	PPU_SetExtLatchMode( mapper->nes->ppu, TRUE );
	APU_SelectExSound( mapper->nes->apu, 8 );
}

BYTE	Mapper005_ReadLow( Mapper *mapper, WORD addr )
{
BYTE	data = (BYTE)(addr>>8);

	switch( addr ) {
		case	0x5015:
			data = APU_ExRead( mapper->nes->apu, addr );
			break;

		case	0x5204:
			data = ((Mapper005 *)mapper)->irq_status;
//			irq_status = 0;
			((Mapper005 *)mapper)->irq_status &= ~0x80;

			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0x5205:
			data = ((Mapper005 *)mapper)->mult_a*((Mapper005 *)mapper)->mult_b;
			break;
		case	0x5206:
			data = (BYTE)(((WORD)((Mapper005 *)mapper)->mult_a*(WORD)((Mapper005 *)mapper)->mult_b)>>8);
			break;
	}

	if( addr >= 0x5C00 && addr <= 0x5FFF ) {
		if( ((Mapper005 *)mapper)->graphic_mode >= 2 ) { // ExRAM mode
			data = VRAM[0x0800+(addr&0x3FF)];
		}
	} else if( addr >= 0x6000 && addr <= 0x7FFF ) {
		data = Mapper_DefReadLow( mapper, addr );
	}

	return	data;
}

void	Mapper005_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
INT	i;

#if	0
if( addr >= 0x5000 && addr <=0x50FF ) {
DebugOut( "$%04X=%02X C:%10d\n", addr, data, CPU_GetTotalCycles(nes->cpu) );
}
#endif

	switch( addr ) {
		case	0x5100:
			((Mapper005 *)mapper)->prg_size = data & 0x03;
			break;
		case	0x5101:
			((Mapper005 *)mapper)->chr_size = data & 0x03;
			break;

		case	0x5102:
			((Mapper005 *)mapper)->sram_we_a = data & 0x03;
			break;
		case	0x5103:
			((Mapper005 *)mapper)->sram_we_b = data & 0x03;
			break;

		case	0x5104:
			((Mapper005 *)mapper)->graphic_mode = data & 0x03;
			break;
		case	0x5105:
			((Mapper005 *)mapper)->nametable_mode = data;
			for( i = 0; i < 4; i++ ) {
				((Mapper005 *)mapper)->nametable_type[i] = data&0x03;
				SetVRAM_1K_Bank( 8+i, ((Mapper005 *)mapper)->nametable_type[i] );
				data >>= 2;
			}
			break;

		case	0x5106:
			((Mapper005 *)mapper)->fill_chr = data;
			break;
		case	0x5107:
			((Mapper005 *)mapper)->fill_pal = data & 0x03;
			break;

		case	0x5113:
			Mapper005_SetBank_SRAM( mapper, 3, data&0x07 );
			break;

		case	0x5114:
		case	0x5115:
		case	0x5116:
		case	0x5117:
			Mapper005_SetBank_CPU( mapper, addr, data );
			break;

		case	0x5120:
		case	0x5121:
		case	0x5122:
		case	0x5123:
		case	0x5124:
		case	0x5125:
		case	0x5126:
		case	0x5127:
			((Mapper005 *)mapper)->chr_mode = 0;
			((Mapper005 *)mapper)->chr_page[0][addr&0x07] = data;
			Mapper005_SetBank_PPU(mapper);
			break;

		case	0x5128:
		case	0x5129:
		case	0x512A:
		case	0x512B:
			((Mapper005 *)mapper)->chr_mode = 1;
			((Mapper005 *)mapper)->chr_page[1][(addr&0x03)+0] = data;
			((Mapper005 *)mapper)->chr_page[1][(addr&0x03)+4] = data;
			Mapper005_SetBank_PPU(mapper);
			break;

		case	0x5200:
			((Mapper005 *)mapper)->split_control = data;
			break;
		case	0x5201:
			((Mapper005 *)mapper)->split_scroll = data;
			break;
		case	0x5202:
			((Mapper005 *)mapper)->split_page = data&0x3F;
			break;

		case	0x5203:
			((Mapper005 *)mapper)->irq_line = data;

			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0x5204:
			((Mapper005 *)mapper)->irq_enable = data;

			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;

		case	0x5205:
			((Mapper005 *)mapper)->mult_a = data;
			break;
		case	0x5206:
			((Mapper005 *)mapper)->mult_b = data;
			break;

		default:
			if( addr >= 0x5000 && addr <= 0x5015 ) {
				APU_ExWrite( mapper->nes->apu, addr, data );
			} else if( addr >= 0x5C00 && addr <= 0x5FFF ) {
				if( ((Mapper005 *)mapper)->graphic_mode == 2 ) {		// ExRAM
					VRAM[0x0800+(addr&0x3FF)] = data;
				} else if( ((Mapper005 *)mapper)->graphic_mode != 3 ) {	// Split,ExGraphic
					if( ((Mapper005 *)mapper)->irq_status&0x40 ) {
						VRAM[0x0800+(addr&0x3FF)] = data;
					} else {
						VRAM[0x0800+(addr&0x3FF)] = 0;
					}
				}
			} else if( addr >= 0x6000 && addr <= 0x7FFF ) {
				if( (((Mapper005 *)mapper)->sram_we_a == 0x02) && (((Mapper005 *)mapper)->sram_we_b == 0x01) ) {
					if( CPU_MEM_TYPE[3] == BANKTYPE_RAM ) {
						CPU_MEM_BANK[3][addr&0x1FFF] = data;
					}
				}
			}
			break;
	}
}

void	Mapper005_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( ((Mapper005 *)mapper)->sram_we_a == 0x02 && ((Mapper005 *)mapper)->sram_we_b == 0x01 ) {
		if( addr >= 0x8000 && addr < 0xE000 ) {
			if( CPU_MEM_TYPE[addr>>13] == BANKTYPE_RAM ) {
				CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
			}
		}
	}
}

void	Mapper005_SetBank_CPU( Mapper *mapper, WORD addr, BYTE data )
{
	if( data & 0x80 ) {
	// PROM Bank
		switch( addr & 7 ) {
			case	4:
				if( ((Mapper005 *)mapper)->prg_size == 3 ) {
					SetPROM_8K_Bank( 4, data&0x7F );
				}
				break;
			case	5:
				if( ((Mapper005 *)mapper)->prg_size == 1 || ((Mapper005 *)mapper)->prg_size == 2 ) {
					SetPROM_16K_Bank( 4, (data&0x7F)>>1 );
				} else if( ((Mapper005 *)mapper)->prg_size == 3 ) {
					SetPROM_8K_Bank( 5, (data&0x7F) );
				}
				break;
			case	6:
				if( ((Mapper005 *)mapper)->prg_size == 2 || ((Mapper005 *)mapper)->prg_size == 3 ) {
					SetPROM_8K_Bank( 6, (data&0x7F) );
				}
				break;
			case	7:
				if( ((Mapper005 *)mapper)->prg_size == 0 ) {
					SetPROM_32K_Bank_cont( (data&0x7F)>>2 );
				} else if( ((Mapper005 *)mapper)->prg_size == 1 ) {
					SetPROM_16K_Bank( 6, (data&0x7F)>>1 );
				} else if( ((Mapper005 *)mapper)->prg_size == 2 || ((Mapper005 *)mapper)->prg_size == 3 ) {
					SetPROM_8K_Bank( 7, (data&0x7F) );
				}
				break;
		}
	} else {
	// WRAM Bank
		switch( addr & 7 ) {
			case	4:
				if( ((Mapper005 *)mapper)->prg_size == 3 ) {
					Mapper005_SetBank_SRAM( mapper, 4, data&0x07 );
				}
				break;
			case	5:
				if( ((Mapper005 *)mapper)->prg_size == 1 || ((Mapper005 *)mapper)->prg_size == 2 ) {
					Mapper005_SetBank_SRAM( mapper, 4, (data&0x06)+0 );
					Mapper005_SetBank_SRAM( mapper, 5, (data&0x06)+1 );
				} else if( ((Mapper005 *)mapper)->prg_size == 3 ) {
					Mapper005_SetBank_SRAM( mapper, 5, data&0x07 );
				}
				break;
			case	6:
				if( ((Mapper005 *)mapper)->prg_size == 2 || ((Mapper005 *)mapper)->prg_size == 3 ) {
					Mapper005_SetBank_SRAM( mapper, 6, data&0x07 );
				}
				break;
		}
	}
}

void	Mapper005_SetBank_SRAM( Mapper *mapper, BYTE page, BYTE data )
{
	if( ((Mapper005 *)mapper)->sram_size == 0 ) data = (data > 3) ? 8 : 0;
	if( ((Mapper005 *)mapper)->sram_size == 1 ) data = (data > 3) ? 1 : 0;
	if( ((Mapper005 *)mapper)->sram_size == 2 ) data = (data > 3) ? 8 : data;
	if( ((Mapper005 *)mapper)->sram_size == 3 ) data = (data > 3) ? 4 : data;

	if( data != 8 ) {
		SetPROM_Bank( page, &WRAM[0x2000*data], BANKTYPE_RAM );
		CPU_MEM_PAGE[page] = data;
	} else {
		CPU_MEM_TYPE[page] = BANKTYPE_ROM;
	}
}

void	Mapper005_SetBank_PPU(Mapper *mapper)
{
INT	i;

	if( ((Mapper005 *)mapper)->chr_mode == 0 ) {
	// PPU SP Bank
		switch( ((Mapper005 *)mapper)->chr_size ) {
			case	0:
				SetVROM_8K_Bank_cont( ((Mapper005 *)mapper)->chr_page[0][7] );
				break;
			case	1:
				SetVROM_4K_Bank( 0, ((Mapper005 *)mapper)->chr_page[0][3] );
				SetVROM_4K_Bank( 4, ((Mapper005 *)mapper)->chr_page[0][7] );
				break;
			case	2:
				SetVROM_2K_Bank( 0, ((Mapper005 *)mapper)->chr_page[0][1] );
				SetVROM_2K_Bank( 2, ((Mapper005 *)mapper)->chr_page[0][3] );
				SetVROM_2K_Bank( 4, ((Mapper005 *)mapper)->chr_page[0][5] );
				SetVROM_2K_Bank( 6, ((Mapper005 *)mapper)->chr_page[0][7] );
				break;
			case	3:
				SetVROM_8K_Bank( ((Mapper005 *)mapper)->chr_page[0][0],
						 ((Mapper005 *)mapper)->chr_page[0][1],
						 ((Mapper005 *)mapper)->chr_page[0][2],
						 ((Mapper005 *)mapper)->chr_page[0][3],
						 ((Mapper005 *)mapper)->chr_page[0][4],
						 ((Mapper005 *)mapper)->chr_page[0][5],
						 ((Mapper005 *)mapper)->chr_page[0][6],
						 ((Mapper005 *)mapper)->chr_page[0][7] );
				break;
		}
	} else if( ((Mapper005 *)mapper)->chr_mode == 1 ) {
		// PPU BG Bank
		switch( ((Mapper005 *)mapper)->chr_size ) {
			case	0:
				for( i = 0; i < 8; i++ ) {
					((Mapper005 *)mapper)->BG_MEM_BANK[i] = VROM+0x2000*(((Mapper005 *)mapper)->chr_page[1][7]%VROM_8K_SIZE)+0x0400*i;
					((Mapper005 *)mapper)->BG_MEM_PAGE[i] = (((Mapper005 *)mapper)->chr_page[1][7]%VROM_8K_SIZE)*8+i;
				}
				break;
			case	1:
				for( i = 0; i < 4; i++ ) {
					((Mapper005 *)mapper)->BG_MEM_BANK[i+0] = VROM+0x1000*(((Mapper005 *)mapper)->chr_page[1][3]%VROM_4K_SIZE)+0x0400*i;
					((Mapper005 *)mapper)->BG_MEM_BANK[i+4] = VROM+0x1000*(((Mapper005 *)mapper)->chr_page[1][7]%VROM_4K_SIZE)+0x0400*i;
					((Mapper005 *)mapper)->BG_MEM_PAGE[i+0] = (((Mapper005 *)mapper)->chr_page[1][3]%VROM_4K_SIZE)*4+i;
					((Mapper005 *)mapper)->BG_MEM_PAGE[i+4] = (((Mapper005 *)mapper)->chr_page[1][7]%VROM_4K_SIZE)*4+i;
				}
				break;
			case	2:
				for( i = 0; i < 2; i++ ) {
					((Mapper005 *)mapper)->BG_MEM_BANK[i+0] = VROM+0x0800*(((Mapper005 *)mapper)->chr_page[1][1]%VROM_2K_SIZE)+0x0400*i;
					((Mapper005 *)mapper)->BG_MEM_BANK[i+2] = VROM+0x0800*(((Mapper005 *)mapper)->chr_page[1][3]%VROM_2K_SIZE)+0x0400*i;
					((Mapper005 *)mapper)->BG_MEM_BANK[i+4] = VROM+0x0800*(((Mapper005 *)mapper)->chr_page[1][5]%VROM_2K_SIZE)+0x0400*i;
					((Mapper005 *)mapper)->BG_MEM_BANK[i+6] = VROM+0x0800*(((Mapper005 *)mapper)->chr_page[1][7]%VROM_2K_SIZE)+0x0400*i;
					((Mapper005 *)mapper)->BG_MEM_PAGE[i+0] = (((Mapper005 *)mapper)->chr_page[1][1]%VROM_2K_SIZE)*2+i;
					((Mapper005 *)mapper)->BG_MEM_PAGE[i+2] = (((Mapper005 *)mapper)->chr_page[1][3]%VROM_2K_SIZE)*2+i;
					((Mapper005 *)mapper)->BG_MEM_PAGE[i+4] = (((Mapper005 *)mapper)->chr_page[1][5]%VROM_2K_SIZE)*2+i;
					((Mapper005 *)mapper)->BG_MEM_PAGE[i+6] = (((Mapper005 *)mapper)->chr_page[1][7]%VROM_2K_SIZE)*2+i;
				}
				break;
			case	3:
				for( i = 0; i < 8; i++ ) {
					((Mapper005 *)mapper)->BG_MEM_BANK[i] = VROM+0x0400*(((Mapper005 *)mapper)->chr_page[1][i]%VROM_1K_SIZE);
					((Mapper005 *)mapper)->BG_MEM_PAGE[i] = (((Mapper005 *)mapper)->chr_page[1][i]%VROM_1K_SIZE)+i;
				}
				break;
		}
	}
}

void	Mapper005_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper005 *)mapper)->irq_type & MMC5_IRQ_METAL ) {
		if( ((Mapper005 *)mapper)->irq_scanline == ((Mapper005 *)mapper)->irq_line ) {
			((Mapper005 *)mapper)->irq_status |= 0x80;
		}
	}

//	if( nes->ppu->IsDispON() && scanline < 239 ) {
	if( PPU_IsDispON(mapper->nes->ppu) && scanline < SCREEN_HEIGHT ) {
		((Mapper005 *)mapper)->irq_scanline++;
		((Mapper005 *)mapper)->irq_status |= 0x40;
		((Mapper005 *)mapper)->irq_clear = 0;
	} else if( ((Mapper005 *)mapper)->irq_type & MMC5_IRQ_METAL ) {
		((Mapper005 *)mapper)->irq_scanline = 0;
		((Mapper005 *)mapper)->irq_status &= ~0x80;
		((Mapper005 *)mapper)->irq_status &= ~0x40;
	}

	if( !(((Mapper005 *)mapper)->irq_type & MMC5_IRQ_METAL) ) {
		if( ((Mapper005 *)mapper)->irq_scanline == ((Mapper005 *)mapper)->irq_line ) {
			((Mapper005 *)mapper)->irq_status |= 0x80;
		}

		if( ++((Mapper005 *)mapper)->irq_clear > 2 ) {
			((Mapper005 *)mapper)->irq_scanline = 0;
			((Mapper005 *)mapper)->irq_status &= ~0x80;
			((Mapper005 *)mapper)->irq_status &= ~0x40;

			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
		}
	}

	if( (((Mapper005 *)mapper)->irq_enable & 0x80) && (((Mapper005 *)mapper)->irq_status & 0x80) && (((Mapper005 *)mapper)->irq_status & 0x40) ) {
		CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
///		nes->cpu->IRQ_NotPending();
#if	0
{
LPBYTE	lpScn = PPU_GetScreenPtr(nes->ppu);
	INT i;

	lpScn = lpScn+(256+16)*scanline;

	for( i = 0; i < 256+16; i++ ) {
		lpScn[i] = 22;
	}
}
#endif
	}

	// For Split mode!
	if( scanline == 0 ) {
		((Mapper005 *)mapper)->split_yofs = ((Mapper005 *)mapper)->split_scroll&0x07;
		((Mapper005 *)mapper)->split_addr = ((((Mapper005 *)mapper)->split_scroll&0xF8)<<2);
	} else if( PPU_IsDispON(mapper->nes->ppu) ) {
		if( ((Mapper005 *)mapper)->split_yofs == 7 ) {
			((Mapper005 *)mapper)->split_yofs = 0;
			if( (((Mapper005 *)mapper)->split_addr & 0x03E0) == 0x03A0 ) {
				((Mapper005 *)mapper)->split_addr &= 0x001F;
			} else {
				if( (((Mapper005 *)mapper)->split_addr & 0x03E0) == 0x03E0 ) {
					((Mapper005 *)mapper)->split_addr &= 0x001F;
				} else {
					((Mapper005 *)mapper)->split_addr += 0x0020;
				}
			}
		} else {
			((Mapper005 *)mapper)->split_yofs++;
		}
	}
}

void	Mapper005_PPU_ExtLatchX( Mapper *mapper, INT x )
{
	((Mapper005 *)mapper)->split_x = x;
}

void	Mapper005_PPU_ExtLatch( Mapper *mapper, WORD addr, BYTE* chr_l, BYTE* chr_h, BYTE* attr )
{
WORD	ntbladr, attradr, tileadr, tileofs;
WORD	tile_yofs;
DWORD	tilebank;
BOOL	bSplit;

	tile_yofs = PPU_GetTILEY(mapper->nes->ppu);

	bSplit = FALSE;
	if( ((Mapper005 *)mapper)->split_control & 0x80 ) {
		if( !(((Mapper005 *)mapper)->split_control&0x40) ) {
		// Left side
			if( (((Mapper005 *)mapper)->split_control&0x1F) > ((Mapper005 *)mapper)->split_x ) {
				bSplit = TRUE;
			}
		} else {
		// Right side
			if( (((Mapper005 *)mapper)->split_control&0x1F) <= ((Mapper005 *)mapper)->split_x ) {
				bSplit = TRUE;
			}
		}
	}

	if( !bSplit ) {
		if( ((Mapper005 *)mapper)->nametable_type[(addr&0x0C00)>>10] == 3 ) {
		// Fill mode
			if( ((Mapper005 *)mapper)->graphic_mode == 1 ) {
			// ExGraphic mode
				ntbladr = 0x2000+(addr&0x0FFF);
				// Get Nametable
				tileadr = ((Mapper005 *)mapper)->fill_chr*0x10+tile_yofs;
				// Get TileBank
				tilebank = 0x1000*((VRAM[0x0800+(ntbladr&0x03FF)]&0x3F)%VROM_4K_SIZE);
				// Attribute
				*attr = (((Mapper005 *)mapper)->fill_pal<<2)&0x0C;
				// Get Pattern
				*chr_l = VROM[tilebank+tileadr  ];
				*chr_h = VROM[tilebank+tileadr+8];
			} else {
			// Normal
				tileofs = (PPUREG[0]&PPU_BGTBL_BIT)?0x1000:0x0000;
				tileadr = tileofs+((Mapper005 *)mapper)->fill_chr*0x10+tile_yofs;
				*attr = (((Mapper005 *)mapper)->fill_pal<<2)&0x0C;
				// Get Pattern
				if( ((Mapper005 *)mapper)->chr_type ) {
					*chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
					*chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
				} else {
					*chr_l = ((Mapper005 *)mapper)->BG_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
					*chr_h = ((Mapper005 *)mapper)->BG_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
				}
			}
		} else if( ((Mapper005 *)mapper)->graphic_mode == 1 ) {
		// ExGraphic mode
			ntbladr = 0x2000+(addr&0x0FFF);
			// Get Nametable
			tileadr = (WORD)PPU_MEM_BANK[ntbladr>>10][ntbladr&0x03FF]*0x10+tile_yofs;
			// Get TileBank
			tilebank = 0x1000*((VRAM[0x0800+(ntbladr&0x03FF)]&0x3F)%VROM_4K_SIZE);
			// Get Attribute
			*attr = (VRAM[0x0800+(ntbladr&0x03FF)]&0xC0)>>4;
			// Get Pattern
			*chr_l = VROM[tilebank+tileadr  ];
			*chr_h = VROM[tilebank+tileadr+8];
		} else {
		// Normal or ExVRAM
			tileofs = (PPUREG[0]&PPU_BGTBL_BIT)?0x1000:0x0000;
			ntbladr = 0x2000+(addr&0x0FFF);
			attradr = 0x23C0+(addr&0x0C00)+((addr&0x0380)>>4)+((addr&0x001C)>>2);
			// Get Nametable
			tileadr = tileofs+PPU_MEM_BANK[ntbladr>>10][ntbladr&0x03FF]*0x10+tile_yofs;
			// Get Attribute
			*attr = PPU_MEM_BANK[attradr>>10][attradr&0x03FF];
			if( ntbladr & 0x0002 ) *attr >>= 2;
			if( ntbladr & 0x0040 ) *attr >>= 4;
			*attr = (*attr&0x03)<<2;
			// Get Pattern
			if( ((Mapper005 *)mapper)->chr_type ) {
				*chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
				*chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			} else {
				*chr_l = ((Mapper005 *)mapper)->BG_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
				*chr_h = ((Mapper005 *)mapper)->BG_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			}
		}
	} else {
		ntbladr = ((((Mapper005 *)mapper)->split_addr&0x03E0)|(((Mapper005 *)mapper)->split_x&0x1F))&0x03FF;
		// Get Split TileBank
		tilebank = 0x1000*((INT)((Mapper005 *)mapper)->split_page%VROM_4K_SIZE);
		tileadr  = (WORD)VRAM[0x0800+ntbladr]*0x10+((Mapper005 *)mapper)->split_yofs;
		// Get Attribute
		attradr = 0x03C0+((ntbladr&0x0380)>>4)+((ntbladr&0x001C)>>2);
		*attr = VRAM[0x0800+attradr];
		if( ntbladr & 0x0002 ) *attr >>= 2;
		if( ntbladr & 0x0040 ) *attr >>= 4;
		*attr = (*attr&0x03)<<2;
		// Get Pattern
		*chr_l = VROM[tilebank+tileadr  ];
		*chr_h = VROM[tilebank+tileadr+8];
	}
}

void	Mapper005_SaveState( Mapper *mapper, LPBYTE p )
{
	INT	i, j;
	p[ 0] = ((Mapper005 *)mapper)->prg_size;
	p[ 1] = ((Mapper005 *)mapper)->chr_size;
	p[ 2] = ((Mapper005 *)mapper)->sram_we_a;
	p[ 3] = ((Mapper005 *)mapper)->sram_we_b;
	p[ 4] = ((Mapper005 *)mapper)->graphic_mode;
	p[ 5] = ((Mapper005 *)mapper)->nametable_mode;
	p[ 6] = ((Mapper005 *)mapper)->nametable_type[0];
	p[ 7] = ((Mapper005 *)mapper)->nametable_type[1];
	p[ 8] = ((Mapper005 *)mapper)->nametable_type[2];
	p[ 9] = ((Mapper005 *)mapper)->nametable_type[3];
	p[10] = ((Mapper005 *)mapper)->sram_page;
	p[11] = ((Mapper005 *)mapper)->fill_chr;
	p[12] = ((Mapper005 *)mapper)->fill_pal;
	p[13] = ((Mapper005 *)mapper)->split_control;
	p[14] = ((Mapper005 *)mapper)->split_scroll;
	p[15] = ((Mapper005 *)mapper)->split_page;
	p[16] = ((Mapper005 *)mapper)->chr_mode;
	p[17] = ((Mapper005 *)mapper)->irq_status;
	p[18] = ((Mapper005 *)mapper)->irq_enable;
	p[19] = ((Mapper005 *)mapper)->irq_line;
	p[20] = ((Mapper005 *)mapper)->irq_scanline;
	p[21] = ((Mapper005 *)mapper)->irq_clear;
	p[22] = ((Mapper005 *)mapper)->mult_a;
	p[23] = ((Mapper005 *)mapper)->mult_b;

	for( j = 0; j < 2; j++ ) {
		for( i = 0; i < 8; i++ ) {
			p[24+j*8+i] = ((Mapper005 *)mapper)->chr_page[j][i];
		}
	}
//	for( i = 0; i < 8; i++ ) {
//		p[40+i] = BG_MEM_PAGE[i];
//	}
}

void	Mapper005_LoadState( Mapper *mapper, LPBYTE p )
{
	INT	i, j;

	((Mapper005 *)mapper)->prg_size	  = p[ 0];
	((Mapper005 *)mapper)->chr_size	  = p[ 1];
	((Mapper005 *)mapper)->sram_we_a	  = p[ 2];
	((Mapper005 *)mapper)->sram_we_b	  = p[ 3];
	((Mapper005 *)mapper)->graphic_mode	  = p[ 4];
	((Mapper005 *)mapper)->nametable_mode	  = p[ 5];
	((Mapper005 *)mapper)->nametable_type[0] = p[ 6];
	((Mapper005 *)mapper)->nametable_type[1] = p[ 7];
	((Mapper005 *)mapper)->nametable_type[2] = p[ 8];
	((Mapper005 *)mapper)->nametable_type[3] = p[ 9];
	((Mapper005 *)mapper)->sram_page	  = p[10];
	((Mapper005 *)mapper)->fill_chr	  = p[11];
	((Mapper005 *)mapper)->fill_pal	  = p[12];
	((Mapper005 *)mapper)->split_control	  = p[13];
	((Mapper005 *)mapper)->split_scroll	  = p[14];
	((Mapper005 *)mapper)->split_page	  = p[15];
	((Mapper005 *)mapper)->chr_mode          = p[16];
	((Mapper005 *)mapper)->irq_status	  = p[17];
	((Mapper005 *)mapper)->irq_enable	  = p[18];
	((Mapper005 *)mapper)->irq_line	  = p[19];
	((Mapper005 *)mapper)->irq_scanline	  = p[20];
	((Mapper005 *)mapper)->irq_clear	  = p[21];
	((Mapper005 *)mapper)->mult_a		  = p[22];
	((Mapper005 *)mapper)->mult_b		  = p[23];

	for( j = 0; j < 2; j++ ) {
		for( i = 0; i < 8; i++ ) {
			((Mapper005 *)mapper)->chr_page[j][i] = p[24+j*8+i];
		}
	}
//	// BGバンクの再設定処理
//	for( i = 0; i < 8; i++ ) {
//		BG_MEM_PAGE[i] = p[40+i]%VROM_1K_SIZE;
//	}
//	for( i = 0; i < 8; i++ ) {
//		BG_MEM_BANK[i] = VROM+0x0400*BG_MEM_PAGE[i];
//	}

	Mapper005_SetBank_PPU(mapper);

}

BOOL	Mapper005_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper005_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper005));
	
	memset(mapper, 0, sizeof(Mapper005));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper005_Reset;
	mapper->vtbl.ReadLow = Mapper005_ReadLow;
	mapper->vtbl.WriteLow = Mapper005_WriteLow;
	mapper->vtbl.Write = Mapper005_Write;
	mapper->vtbl.HSync = Mapper005_HSync;
	mapper->vtbl.PPU_ExtLatchX = Mapper005_PPU_ExtLatchX;
	mapper->vtbl.PPU_ExtLatch = Mapper005_PPU_ExtLatch;
	mapper->vtbl.SaveState = Mapper005_SaveState;
	mapper->vtbl.LoadState = Mapper005_LoadState;
	mapper->vtbl.IsStateSave = Mapper005_IsStateSave;

	return (Mapper *)mapper;
}

