//////////////////////////////////////////////////////////////////////////
// Mapper004  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;
	
	BYTE	irq_type;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;
	BYTE	irq_preset;
	BYTE	irq_preset_vbl;
	
	BYTE	vs_patch;
	BYTE	vs_index;
}Mapper004;

void	Mapper004_SetBank_CPU(Mapper *mapper);
void	Mapper004_SetBank_PPU(Mapper *mapper);

#define	MMC3_IRQ_KLAX		1
#define	MMC3_IRQ_SHOUGIMEIKAN	2
#define	MMC3_IRQ_DAI2JISUPER	3
#define	MMC3_IRQ_DBZ2		4
#define	MMC3_IRQ_ROCKMAN3	5

void	Mapper004_Reset(Mapper *mapper)
{
	INT i;
	DWORD	crc;
	for( i = 0; i < 8; i++ ) {
		((Mapper004 *)mapper)->reg[i] = 0x00;
	}

	((Mapper004 *)mapper)->prg0 = 0;
	((Mapper004 *)mapper)->prg1 = 1;
	Mapper004_SetBank_CPU(mapper);

	((Mapper004 *)mapper)->chr01 = 0;
	((Mapper004 *)mapper)->chr23 = 2;
	((Mapper004 *)mapper)->chr4  = 4;
	((Mapper004 *)mapper)->chr5  = 5;
	((Mapper004 *)mapper)->chr6  = 6;
	((Mapper004 *)mapper)->chr7  = 7;
	Mapper004_SetBank_PPU(mapper);

	((Mapper004 *)mapper)->we_sram  = 0;	// Disable
	((Mapper004 *)mapper)->irq_enable = 0;	// Disable
	((Mapper004 *)mapper)->irq_counter = 0;
	((Mapper004 *)mapper)->irq_latch = 0xFF;
	((Mapper004 *)mapper)->irq_request = 0;
	((Mapper004 *)mapper)->irq_preset = 0;
	((Mapper004 *)mapper)->irq_preset_vbl = 0;

	// IRQƒ^ƒCƒvÝ’è
	NES_SetIrqType( mapper->nes, IRQ_CLOCK );
	((Mapper004 *)mapper)->irq_type = 0;

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	if( crc == 0x5c707ac4 ) {	// Mother(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0xcb106f49 ) {	// F-1 Sensation(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x1170392a ) {	// Karakuri Kengou Den - Musashi Road - Karakuri Nin Hashiru!(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0x14a01c70 ) {	// Gun-Dec(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0xeffeea40 ) {	// For Klax(J)
		((Mapper004 *)mapper)->irq_type = MMC3_IRQ_KLAX;
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0xc17ae2dc ) {	// God Slayer - Haruka Tenkuu no Sonata(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x126ea4a0 ) {	// Summer Carnival '92 - Recca(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0x1f2f4861 ) {	// J League Fighting Soccer - The King of Ace Strikers(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0x5a6860f1 ) {	// Shougi Meikan '92(J)
		((Mapper004 *)mapper)->irq_type = MMC3_IRQ_SHOUGIMEIKAN;
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0xae280e20 ) {	// Shougi Meikan '93(J)
		((Mapper004 *)mapper)->irq_type = MMC3_IRQ_SHOUGIMEIKAN;
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0xe19a2473 ) {	// Sugoro Quest - Dice no Senshi Tachi(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x702d9b33 ) {	// Star Wars - The Empire Strikes Back(Victor)(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0xa9a0d729 ) {	// Dai Kaijuu - Deburas(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0xc5fea9f2 ) {	// Dai 2 Ji - Super Robot Taisen(J)
		((Mapper004 *)mapper)->irq_type = MMC3_IRQ_DAI2JISUPER;
	}
	if( crc == 0xd852c2f7 ) {	// Time Zone(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0xecfd3c69 ) {	// Taito Chase H.Q.(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x7a748058 ) {	// Tom & Jerry (and Tuffy)(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0xaafe699c ) {	// Ninja Ryukenden 3 - Yomi no Hakobune(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x6cc62c06 ) {	// Hoshi no Kirby - Yume no Izumi no Monogatari(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x877dba77 ) {	// My Life My Love - Boku no Yume - Watashi no Negai(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0x6f96ed15 ) {	// Max Warrior - Wakusei Kaigenrei(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0x8685f366 ) {	// Matendouji(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x8635fed1 ) {	// Mickey Mouse 3 - Yume Fuusen(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x26ff3ea2 ) {	// Yume Penguin Monogatari(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0x7671bc51 ) {	// Red Ariimaa 2(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0xade11141 ) {	// Wanpaku Kokkun no Gourmet World(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0x7c7ab58e ) {	// Walkuere no Bouken - Toki no Kagi Densetsu(J)
		NES_SetRenderMethod( mapper->nes, POST_RENDER );
	}
	if( crc == 0x26ff3ea2 ) {	// Yume Penguin Monogatari(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x126ea4a0 ) {	// Summer Carnival '92 Recca(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}

	if( crc == 0x1d2e5018		// Rockman 3(J)
	 || crc == 0x6b999aaf ) {	// Megaman 3(U)
		((Mapper004 *)mapper)->irq_type = MMC3_IRQ_ROCKMAN3;
	}

	if( crc == 0xd88d48d7 ) {	// Kick Master(U)
		((Mapper004 *)mapper)->irq_type = MMC3_IRQ_ROCKMAN3;
	}

	if( crc == 0xA67EA466 ) {	// Alien 3(U)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}

	if( crc == 0xe763891b ) {	// DBZ2
		((Mapper004 *)mapper)->irq_type = MMC3_IRQ_DBZ2;
	}

	// VS-Unisystem
	((Mapper004 *)mapper)->vs_patch = 0;
	((Mapper004 *)mapper)->vs_index = 0;

	if( crc == 0xeb2dba63		// VS TKO Boxing
	 || crc == 0x98cfe016 ) {	// VS TKO Boxing (Alt)
		((Mapper004 *)mapper)->vs_patch = 1;
	}
	if( crc == 0x135adf7c ) {	// VS Atari RBI Baseball
		((Mapper004 *)mapper)->vs_patch = 2;
	}
	if( crc == 0xf9d3b0a3		// VS Super Xevious
	 || crc == 0x9924980a		// VS Super Xevious (b1)
	 || crc == 0x66bb838f ) {	// VS Super Xevious (b2)
		((Mapper004 *)mapper)->vs_patch = 3;
	}
}

BYTE	Mapper004_ReadLow( Mapper *mapper, WORD addr )
{
	if( !((Mapper004 *)mapper)->vs_patch ) {
		if( addr >= 0x5000 && addr <= 0x5FFF ) {
			return	XRAM[addr-0x4000];
		}
	} else if( ((Mapper004 *)mapper)->vs_patch == 1 ) {
		// VS TKO Boxing Security
		if( addr == 0x5E00 ) {
			((Mapper004 *)mapper)->vs_index = 0;
			return	0x00;
		} else if( addr == 0x5E01 ) {
			BYTE	VS_TKO_Security[32] = {
				0xff, 0xbf, 0xb7, 0x97, 0x97, 0x17, 0x57, 0x4f,
				0x6f, 0x6b, 0xeb, 0xa9, 0xb1, 0x90, 0x94, 0x14,
				0x56, 0x4e, 0x6f, 0x6b, 0xeb, 0xa9, 0xb1, 0x90,
				0xd4, 0x5c, 0x3e, 0x26, 0x87, 0x83, 0x13, 0x00 };
			return	VS_TKO_Security[(((Mapper004 *)mapper)->vs_index++) & 0x1F];
		}
	} else if( ((Mapper004 *)mapper)->vs_patch == 2 ) {
		// VS Atari RBI Baseball Security
		if( addr == 0x5E00 ) {
			((Mapper004 *)mapper)->vs_index = 0;
			return	0x00;
		} else if( addr == 0x5E01 ) {
			if( ((Mapper004 *)mapper)->vs_index++ == 9 )
				return	0x6F;
			else
				return	0xB4;
		}
	} else if( ((Mapper004 *)mapper)->vs_patch == 3 ) {
		// VS Super Xevious
		switch( addr ) {
			case	0x54FF:
				return	0x05;
			case	0x5678:
				if( ((Mapper004 *)mapper)->vs_index )
					return	0x00;
				else
					return	0x01;
				break;
			case	0x578f:
				if( ((Mapper004 *)mapper)->vs_index )
					return	0xD1;
				else
					return	0x89;
				break;
			case	0x5567:
				if( ((Mapper004 *)mapper)->vs_index ) {
					((Mapper004 *)mapper)->vs_index = 0;
					return	0x3E;
				} else {
					((Mapper004 *)mapper)->vs_index = 1;
					return	0x37;
				}
				break;
			default:
				break;
		}
	}

	return	Mapper_DefReadLow( mapper, addr );
}

void	Mapper004_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr >= 0x5000 && addr <= 0x5FFF ) {
		XRAM[addr-0x4000] = data;
	} else {
		Mapper_DefWriteLow( mapper, addr, data );
	}
}

void	Mapper004_Write( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );

	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper004 *)mapper)->reg[0] = data;
			Mapper004_SetBank_CPU(mapper);
			Mapper004_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper004 *)mapper)->reg[1] = data;

			switch( ((Mapper004 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper004 *)mapper)->chr01 = data & 0xFE;
					Mapper004_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper004 *)mapper)->chr23 = data & 0xFE;
					Mapper004_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper004 *)mapper)->chr4 = data;
					Mapper004_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper004 *)mapper)->chr5 = data;
					Mapper004_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper004 *)mapper)->chr6 = data;
					Mapper004_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper004 *)mapper)->chr7 = data;
					Mapper004_SetBank_PPU(mapper);
					break;
				case	0x06:
					((Mapper004 *)mapper)->prg0 = data;
					Mapper004_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper004 *)mapper)->prg1 = data;
					Mapper004_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper004 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(mapper->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper004 *)mapper)->reg[3] = data;
//DebugOut( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
			break;
		case	0xC000:
//DebugOut( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
			((Mapper004 *)mapper)->reg[4] = data;
			if( ((Mapper004 *)mapper)->irq_type == MMC3_IRQ_KLAX || ((Mapper004 *)mapper)->irq_type == MMC3_IRQ_ROCKMAN3 ) {
				((Mapper004 *)mapper)->irq_counter = data;
			} else {
				((Mapper004 *)mapper)->irq_latch = data;
			}
			if( ((Mapper004 *)mapper)->irq_type == MMC3_IRQ_DBZ2 ) {
				((Mapper004 *)mapper)->irq_latch = 0x07;
			}
			break;
		case	0xC001:
			((Mapper004 *)mapper)->reg[5] = data;
			if( ((Mapper004 *)mapper)->irq_type == MMC3_IRQ_KLAX || ((Mapper004 *)mapper)->irq_type == MMC3_IRQ_ROCKMAN3 ) {
				((Mapper004 *)mapper)->irq_latch = data;
			} else {
				if( (NES_GetScanline(mapper->nes) < SCREEN_HEIGHT) || (((Mapper004 *)mapper)->irq_type == MMC3_IRQ_SHOUGIMEIKAN) ) {
					((Mapper004 *)mapper)->irq_counter |= 0x80;
					((Mapper004 *)mapper)->irq_preset = 0xFF;
				} else {
					((Mapper004 *)mapper)->irq_counter |= 0x80;
					((Mapper004 *)mapper)->irq_preset_vbl = 0xFF;
					((Mapper004 *)mapper)->irq_preset = 0;
				}
			}
			break;
		case	0xE000:
			((Mapper004 *)mapper)->reg[6] = data;
			((Mapper004 *)mapper)->irq_enable = 0;
			((Mapper004 *)mapper)->irq_request = 0;

			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper004 *)mapper)->reg[7] = data;
			((Mapper004 *)mapper)->irq_enable = 1;
			((Mapper004 *)mapper)->irq_request = 0;

//			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper004_Clock( Mapper *mapper, INT cycles )
{
//	if( irq_request && (nes->GetIrqType() == IRQ_CLOCK) ) {
//		nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper004_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper004 *)mapper)->irq_type == MMC3_IRQ_KLAX ) {
		if( (scanline >= 0 && scanline < SCREEN_HEIGHT) && PPU_IsDispON(mapper->nes->ppu) ) {
			if( ((Mapper004 *)mapper)->irq_enable ) {
				if( ((Mapper004 *)mapper)->irq_counter == 0 ) {
					((Mapper004 *)mapper)->irq_counter = ((Mapper004 *)mapper)->irq_latch;
					((Mapper004 *)mapper)->irq_request = 0xFF;
				}
				if( ((Mapper004 *)mapper)->irq_counter > 0 ) {
					((Mapper004 *)mapper)->irq_counter--;
				}
			}
		}
		if( ((Mapper004 *)mapper)->irq_request ) {
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
		}
	} else if( ((Mapper004 *)mapper)->irq_type == MMC3_IRQ_ROCKMAN3 ) {
		if( (scanline >= 0 && scanline < SCREEN_HEIGHT) && PPU_IsDispON(mapper->nes->ppu) ) {
			if( ((Mapper004 *)mapper)->irq_enable ) {
				if( !(--((Mapper004 *)mapper)->irq_counter) ) {
					((Mapper004 *)mapper)->irq_request = 0xFF;
					((Mapper004 *)mapper)->irq_counter = ((Mapper004 *)mapper)->irq_latch;
				}
			}
		}
		if( ((Mapper004 *)mapper)->irq_request ) {
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
		}
	} else {
		if( (scanline >= 0 && scanline < SCREEN_HEIGHT) && PPU_IsDispON(mapper->nes->ppu) ) {
			if( ((Mapper004 *)mapper)->irq_preset_vbl ) {
				((Mapper004 *)mapper)->irq_counter = ((Mapper004 *)mapper)->irq_latch;
				((Mapper004 *)mapper)->irq_preset_vbl = 0;
			}
			if( ((Mapper004 *)mapper)->irq_preset ) {
				((Mapper004 *)mapper)->irq_counter = ((Mapper004 *)mapper)->irq_latch;
				((Mapper004 *)mapper)->irq_preset = 0;
				if( ((Mapper004 *)mapper)->irq_type == MMC3_IRQ_DAI2JISUPER && scanline == 0 ) {
					((Mapper004 *)mapper)->irq_counter--;
				}
			} else if( ((Mapper004 *)mapper)->irq_counter > 0 ) {
				((Mapper004 *)mapper)->irq_counter--;
			}

			if( ((Mapper004 *)mapper)->irq_counter == 0 ) {
				if( ((Mapper004 *)mapper)->irq_enable ) {
					((Mapper004 *)mapper)->irq_request = 0xFF;

					CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );

#if	0
{
LPBYTE	lpScn = PPU_GetScreenPtr(nes->ppu);
	INT i;

	lpScn = lpScn+(256+16)*nes->GetScanline();

	for( i = 0; i < 16; i++ ) {
		lpScn[i] = 22;
	}
}
#endif
				}
				((Mapper004 *)mapper)->irq_preset = 0xFF;
			}
		}
	}
//	if( irq_request && (nes->GetIrqType() == IRQ_HSYNC) ) {
//		nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper004_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper004 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, ((Mapper004 *)mapper)->prg1, ((Mapper004 *)mapper)->prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( ((Mapper004 *)mapper)->prg0, ((Mapper004 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper004_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		if( ((Mapper004 *)mapper)->reg[0] & 0x80 ) {
			SetVROM_8K_Bank( ((Mapper004 *)mapper)->chr4, ((Mapper004 *)mapper)->chr5, ((Mapper004 *)mapper)->chr6, ((Mapper004 *)mapper)->chr7,
					 ((Mapper004 *)mapper)->chr01, ((Mapper004 *)mapper)->chr01+1, ((Mapper004 *)mapper)->chr23, ((Mapper004 *)mapper)->chr23+1 );
		} else {
			SetVROM_8K_Bank( ((Mapper004 *)mapper)->chr01, ((Mapper004 *)mapper)->chr01+1, ((Mapper004 *)mapper)->chr23, ((Mapper004 *)mapper)->chr23+1,
					 ((Mapper004 *)mapper)->chr4, ((Mapper004 *)mapper)->chr5, ((Mapper004 *)mapper)->chr6, ((Mapper004 *)mapper)->chr7 );
		}
	} else {
		if( ((Mapper004 *)mapper)->reg[0] & 0x80 ) {
			SetCRAM_1K_Bank( 4, (((Mapper004 *)mapper)->chr01+0)&0x07 );
			SetCRAM_1K_Bank( 5, (((Mapper004 *)mapper)->chr01+1)&0x07 );
			SetCRAM_1K_Bank( 6, (((Mapper004 *)mapper)->chr23+0)&0x07 );
			SetCRAM_1K_Bank( 7, (((Mapper004 *)mapper)->chr23+1)&0x07 );
			SetCRAM_1K_Bank( 0, ((Mapper004 *)mapper)->chr4&0x07 );
			SetCRAM_1K_Bank( 1, ((Mapper004 *)mapper)->chr5&0x07 );
			SetCRAM_1K_Bank( 2, ((Mapper004 *)mapper)->chr6&0x07 );
			SetCRAM_1K_Bank( 3, ((Mapper004 *)mapper)->chr7&0x07 );
		} else {
			SetCRAM_1K_Bank( 0, (((Mapper004 *)mapper)->chr01+0)&0x07 );
			SetCRAM_1K_Bank( 1, (((Mapper004 *)mapper)->chr01+1)&0x07 );
			SetCRAM_1K_Bank( 2, (((Mapper004 *)mapper)->chr23+0)&0x07 );
			SetCRAM_1K_Bank( 3, (((Mapper004 *)mapper)->chr23+1)&0x07 );
			SetCRAM_1K_Bank( 4, ((Mapper004 *)mapper)->chr4&0x07 );
			SetCRAM_1K_Bank( 5, ((Mapper004 *)mapper)->chr5&0x07 );
			SetCRAM_1K_Bank( 6, ((Mapper004 *)mapper)->chr6&0x07 );
			SetCRAM_1K_Bank( 7, ((Mapper004 *)mapper)->chr7&0x07 );
		}
	}
}

BOOL	Mapper004_IsStateSave(Mapper *mapper)
{
	return TRUE;
}

void	Mapper004_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper004 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper004 *)mapper)->prg0;
	p[ 9] = ((Mapper004 *)mapper)->prg1;
	p[10] = ((Mapper004 *)mapper)->chr01;
	p[11] = ((Mapper004 *)mapper)->chr23;
	p[12] = ((Mapper004 *)mapper)->chr4;
	p[13] = ((Mapper004 *)mapper)->chr5;
	p[14] = ((Mapper004 *)mapper)->chr6;
	p[15] = ((Mapper004 *)mapper)->chr7;
	p[16] = ((Mapper004 *)mapper)->irq_enable;
	p[17] = (BYTE)((Mapper004 *)mapper)->irq_counter;
	p[18] = ((Mapper004 *)mapper)->irq_latch;
	p[19] = ((Mapper004 *)mapper)->irq_request;
	p[20] = ((Mapper004 *)mapper)->irq_preset;
	p[21] = ((Mapper004 *)mapper)->irq_preset_vbl;
}

void	Mapper004_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 8; i++ ) {
		((Mapper004 *)mapper)->reg[i] = p[i];
	}
	((Mapper004 *)mapper)->prg0  = p[ 8];
	((Mapper004 *)mapper)->prg1  = p[ 9];
	((Mapper004 *)mapper)->chr01 = p[10];
	((Mapper004 *)mapper)->chr23 = p[11];
	((Mapper004 *)mapper)->chr4  = p[12];
	((Mapper004 *)mapper)->chr5  = p[13];
	((Mapper004 *)mapper)->chr6  = p[14];
	((Mapper004 *)mapper)->chr7  = p[15];
	((Mapper004 *)mapper)->irq_enable  = p[16];
	((Mapper004 *)mapper)->irq_counter = (INT)p[17];
	((Mapper004 *)mapper)->irq_latch   = p[18];
	((Mapper004 *)mapper)->irq_request = p[19];
	((Mapper004 *)mapper)->irq_preset  = p[20];
	((Mapper004 *)mapper)->irq_preset_vbl = p[21];
}

Mapper * Mapper004_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper004));
	
	memset(mapper, 0, sizeof(Mapper004));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper004_Reset;
	mapper->vtbl.ReadLow = Mapper004_ReadLow;
	mapper->vtbl.WriteLow = Mapper004_WriteLow;
	mapper->vtbl.Write = Mapper004_Write;
	mapper->vtbl.Clock = Mapper004_Clock;
	mapper->vtbl.HSync = Mapper004_HSync;
	mapper->vtbl.SaveState = Mapper004_SaveState;
	mapper->vtbl.LoadState = Mapper004_LoadState;
	mapper->vtbl.IsStateSave = Mapper004_IsStateSave;

	return (Mapper *)mapper;
}

