//////////////////////////////////////////////////////////////////////////
// Mapper001  Nintendo MMC1                                             //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	WORD	last_addr;

	BYTE	patch;
	BYTE	wram_patch;
	BYTE	wram_bank;
	BYTE	wram_count;

	BYTE	reg[4];
	BYTE	shift, regbuf;
}Mapper001;

void	Mapper001_Reset(Mapper *mapper)
{
	DWORD	crc;
	((Mapper001 *)mapper)->reg[0] = 0x0C;		// D3=1,D2=1
	((Mapper001 *)mapper)->reg[1] = ((Mapper001 *)mapper)->reg[2] = ((Mapper001 *)mapper)->reg[3] = 0;
	((Mapper001 *)mapper)->shift = ((Mapper001 *)mapper)->regbuf = 0;

	((Mapper001 *)mapper)->patch = 0;
	((Mapper001 *)mapper)->wram_patch = 0;

	if( PROM_16K_SIZE < 32 ) {
		SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	} else {
		// For 512K/1M byte Cartridge
		SetPROM_16K_Bank( 4, 0 );
		SetPROM_16K_Bank( 6, 16-1 );

		((Mapper001 *)mapper)->patch = 1;
	}

	if( VROM_8K_SIZE ) {
//		SetVROM_8K_Bank_cont( 0 );
	}

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	if( crc == 0xb8e16bd0 ) {	// Snow Bros.(J)
		((Mapper001 *)mapper)->patch = 2;
	}
//	if( crc == 0x9b565541 ) {	// Triathron, The(J)
//		nes->SetFrameIRQmode( FALSE );
//	}
	if( crc == 0xc96c6f04 ) {	// Venus Senki(J)
		NES_SetRenderMethod( mapper->nes, POST_ALL_RENDER );
	}
//	if( crc == 0x5e3f7004 ) {	// Softball Tengoku(J)
//	}

	if( crc == 0x4d2edf70 ) {	// Night Rider(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0xcd2a73f0 ) {	// Pirates!(U)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
		((Mapper001 *)mapper)->patch = 2;
	}

//	if( crc == 0x09efe54b ) {	// Majaventure - Mahjong Senki(J)
//		nes->SetFrameIRQmode( FALSE );
//	}

	if( crc == 0x11469ce3 ) {	// Viva! Las Vegas(J)
	}
	if( crc == 0xd878ebf5 ) {	// Ninja Ryukenden(J)
		NES_SetRenderMethod( mapper->nes, POST_ALL_RENDER );
	}

//	if( crc == 0x7bd7b849 ) {	// Nekketsu Koukou - Dodgeball Bu(J)
//	}

	if( crc == 0x466efdc2 ) {	// Final Fantasy(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
		PPU_SetExtMonoMode( mapper->nes->ppu, TRUE );
	}
	if( crc == 0xc9556b36 ) {	// Final Fantasy I&II(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
		PPU_SetExtMonoMode( mapper->nes->ppu, TRUE );
		NES_SetSAVERAM_SIZE( mapper->nes, 16*1024 );
		((Mapper001 *)mapper)->wram_patch = 2;
	}

	if( crc == 0x717e1169 ) {	// Cosmic Wars(J)
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}

	if( crc == 0xC05D2034 ) {	// Snake's Revenge(U)
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}

	if( crc == 0xb8747abf		// Best Play - Pro Yakyuu Special(J)
	 || crc == 0x29449ba9		// Nobunaga no Yabou - Zenkoku Ban(J)
	 || crc == 0x2b11e0b0		// Nobunaga no Yabou - Zenkoku Ban(J)(alt)
	 || crc == 0x4642dda6		// Nobunaga's Ambition(U)
	 || crc == 0xfb69743a		// Aoki Ookami to Shiroki Mejika - Genghis Khan(J)
	 || crc == 0x2225c20f		// Genghis Khan(U)
	 || crc == 0xabbf7217		// Sangokushi(J)
	) {

		NES_SetSAVERAM_SIZE( mapper->nes, 16*1024 );
		((Mapper001 *)mapper)->wram_patch = 1;
		((Mapper001 *)mapper)->wram_bank  = 0;
		((Mapper001 *)mapper)->wram_count = 0;
	}
}

void	Mapper001_Write( Mapper *mapper, WORD addr, BYTE data )
{
//	DebugOut( "MMC1 %04X=%02X\n", addr&0xFFFF,data&0xFF );

	if( ((Mapper001 *)mapper)->wram_patch == 1 && addr == 0xBFFF ) {
		((Mapper001 *)mapper)->wram_count++;
		((Mapper001 *)mapper)->wram_bank += data&0x01;
		if( ((Mapper001 *)mapper)->wram_count == 5 ) {
			if( ((Mapper001 *)mapper)->wram_bank ) {
				SetPROM_Bank( 3, &WRAM[0x2000], BANKTYPE_RAM );
			} else {
				SetPROM_Bank( 3, &WRAM[0x0000], BANKTYPE_RAM );
			}
			((Mapper001 *)mapper)->wram_bank = ((Mapper001 *)mapper)->wram_count = 0;
		}
	}

	if( ((Mapper001 *)mapper)->patch != 1 ) {
		if((addr & 0x6000) != (((Mapper001 *)mapper)->last_addr & 0x6000)) {
			((Mapper001 *)mapper)->shift = ((Mapper001 *)mapper)->regbuf = 0;
		}
		((Mapper001 *)mapper)->last_addr = addr;
	}

	if( data & 0x80 ) {
		((Mapper001 *)mapper)->shift = ((Mapper001 *)mapper)->regbuf = 0;
//		reg[0] = 0x0C;		// D3=1,D2=1
		((Mapper001 *)mapper)->reg[0] |= 0x0C;		// D3=1,D2=1 残りはリセットされない
		return;
	}

	if( data&0x01 ) ((Mapper001 *)mapper)->regbuf |= 1<<((Mapper001 *)mapper)->shift;
	if( ++((Mapper001 *)mapper)->shift < 5 )
		return;
	addr = (addr&0x7FFF)>>13;
	((Mapper001 *)mapper)->reg[addr] = ((Mapper001 *)mapper)->regbuf;

//	DebugOut( "MMC1 %d=%02X\n", addr&0xFFFF,regbuf&0xFF );

	((Mapper001 *)mapper)->regbuf = 0;
	((Mapper001 *)mapper)->shift = 0;

	if( ((Mapper001 *)mapper)->patch != 1 ) {
	// For Normal Cartridge
		switch( addr ) {
			case	0:
				if( ((Mapper001 *)mapper)->reg[0] & 0x02 ) {
					if( ((Mapper001 *)mapper)->reg[0] & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
					else		    SetVRAM_Mirror_cont( VRAM_VMIRROR );
				} else {
					if( ((Mapper001 *)mapper)->reg[0] & 0x01 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
					else		    SetVRAM_Mirror_cont( VRAM_MIRROR4L );
				}
				break;
			case	1:
				// Register #1
				if( VROM_1K_SIZE ) {
					if( ((Mapper001 *)mapper)->reg[0] & 0x10 ) {
						// CHR 4K bank lower($0000-$0FFF)
						SetVROM_4K_Bank( 0, ((Mapper001 *)mapper)->reg[1] );
						// CHR 4K bank higher($1000-$1FFF)
						SetVROM_4K_Bank( 4, ((Mapper001 *)mapper)->reg[2] );
					} else {
						// CHR 8K bank($0000-$1FFF)
						SetVROM_8K_Bank_cont( ((Mapper001 *)mapper)->reg[1]>>1 );
					}
				} else {
					// For Romancia
					if( ((Mapper001 *)mapper)->reg[0] & 0x10 ) {
						SetCRAM_4K_Bank( 0, ((Mapper001 *)mapper)->reg[1] );
					}
				}
				break;
			case	2:
				// Register #2
				if( VROM_1K_SIZE ) {
					if( ((Mapper001 *)mapper)->reg[0] & 0x10 ) {
						// CHR 4K bank lower($0000-$0FFF)
						SetVROM_4K_Bank( 0, ((Mapper001 *)mapper)->reg[1] );
						// CHR 4K bank higher($1000-$1FFF)
						SetVROM_4K_Bank( 4, ((Mapper001 *)mapper)->reg[2] );
					} else {
						// CHR 8K bank($0000-$1FFF)
						SetVROM_8K_Bank_cont( ((Mapper001 *)mapper)->reg[1]>>1 );
					}
				} else {
					// For Romancia
					if( ((Mapper001 *)mapper)->reg[0] & 0x10 ) {
						SetCRAM_4K_Bank( 4, ((Mapper001 *)mapper)->reg[2] );
					}
				}
				break;
			case	3:
				if( !(((Mapper001 *)mapper)->reg[0] & 0x08) ) {
				// PRG 32K bank ($8000-$FFFF)
					SetPROM_32K_Bank_cont( ((Mapper001 *)mapper)->reg[3]>>1 );
				} else {
					if( ((Mapper001 *)mapper)->reg[0] & 0x04 ) {
					// PRG 16K bank ($8000-$BFFF)
						SetPROM_16K_Bank( 4, ((Mapper001 *)mapper)->reg[3] );
						SetPROM_16K_Bank( 6, PROM_16K_SIZE-1 );
					} else {
					// PRG 16K bank ($C000-$FFFF)
						SetPROM_16K_Bank( 6, ((Mapper001 *)mapper)->reg[3] );
						SetPROM_16K_Bank( 4, 0 );
					}
				}
				break;
		}
	} else {
		// For 512K/1M byte Cartridge
		INT	PROM_BASE = 0;
		if( PROM_16K_SIZE >= 32 ) {
			PROM_BASE = ((Mapper001 *)mapper)->reg[1] & 0x10;
		}

		// For FinalFantasy I&II
		if( ((Mapper001 *)mapper)->wram_patch == 2 ) {
			if( !(((Mapper001 *)mapper)->reg[1] & 0x18) ) {
				SetPROM_Bank( 3, &WRAM[0x0000], BANKTYPE_RAM );
			} else {
				SetPROM_Bank( 3, &WRAM[0x2000], BANKTYPE_RAM );
			}
		}

		// Register #0
		if( addr == 0 ) {
			if( ((Mapper001 *)mapper)->reg[0] & 0x02 ) {
				if( ((Mapper001 *)mapper)->reg[0] & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		    SetVRAM_Mirror_cont( VRAM_VMIRROR );
			} else {
				if( ((Mapper001 *)mapper)->reg[0] & 0x01 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
				else		    SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			}
		}
		// Register #1
		if( VROM_1K_SIZE ) {
			if( ((Mapper001 *)mapper)->reg[0] & 0x10 ) {
			// CHR 4K bank lower($0000-$0FFF)
				SetVROM_4K_Bank( 0, ((Mapper001 *)mapper)->reg[1] );
			} else {
			// CHR 8K bank($0000-$1FFF)
				SetVROM_8K_Bank_cont( ((Mapper001 *)mapper)->reg[1]>>1 );
			}
		} else {
			// For Romancia
			if( ((Mapper001 *)mapper)->reg[0] & 0x10 ) {
				SetCRAM_4K_Bank( 0, ((Mapper001 *)mapper)->reg[1] );
			}
		}
		// Register #2
		if( VROM_1K_SIZE ) {
			if( ((Mapper001 *)mapper)->reg[0] & 0x10 ) {
			// CHR 4K bank higher($1000-$1FFF)
				SetVROM_4K_Bank( 4, ((Mapper001 *)mapper)->reg[2] );
			}
		} else {
			// For Romancia
			if( ((Mapper001 *)mapper)->reg[0] & 0x10 ) {
				SetCRAM_4K_Bank( 4, ((Mapper001 *)mapper)->reg[2] );
			}
		}
		// Register #3
		if( !(((Mapper001 *)mapper)->reg[0] & 0x08) ) {
		// PRG 32K bank ($8000-$FFFF)
			SetPROM_32K_Bank_cont( (((Mapper001 *)mapper)->reg[3]&(0xF+PROM_BASE))>>1 );
		} else {
			if( ((Mapper001 *)mapper)->reg[0] & 0x04 ) {
			// PRG 16K bank ($8000-$BFFF)
				SetPROM_16K_Bank( 4, PROM_BASE+(((Mapper001 *)mapper)->reg[3]&0x0F) );
				if( PROM_16K_SIZE >= 32 ) SetPROM_16K_Bank( 6, PROM_BASE+16-1 );
			} else {
			// PRG 16K bank ($C000-$FFFF)
				SetPROM_16K_Bank( 6, PROM_BASE+(((Mapper001 *)mapper)->reg[3]&0x0F) );
				if( PROM_16K_SIZE >= 32 ) SetPROM_16K_Bank( 4, PROM_BASE );
			}
		}
	}
}

void	Mapper001_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper001 *)mapper)->reg[0];
	p[1] = ((Mapper001 *)mapper)->reg[1];
	p[2] = ((Mapper001 *)mapper)->reg[2];
	p[3] = ((Mapper001 *)mapper)->reg[3];
	p[4] = ((Mapper001 *)mapper)->shift;
	p[5] = ((Mapper001 *)mapper)->regbuf;

	p[6] = ((Mapper001 *)mapper)->wram_bank;
	p[7] = ((Mapper001 *)mapper)->wram_count;
}

void	Mapper001_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper001 *)mapper)->reg[0] = p[0];
	((Mapper001 *)mapper)->reg[1] = p[1];
	((Mapper001 *)mapper)->reg[2] = p[2];
	((Mapper001 *)mapper)->reg[3] = p[3];
	((Mapper001 *)mapper)->shift  = p[4];
	((Mapper001 *)mapper)->regbuf = p[5];

	((Mapper001 *)mapper)->wram_bank  = p[6];
	((Mapper001 *)mapper)->wram_count = p[7];
}

BOOL	Mapper001_IsStateSave(Mapper *mapper)
{
	return TRUE;
}


Mapper * Mapper001_New(NES* parent)
{
	Mapper * mapper = (Mapper *)MALLOC(sizeof(Mapper001));
	
	memset(mapper, 0, sizeof(Mapper001));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper001_Reset;
	mapper->vtbl.Write = Mapper001_Write;
	mapper->vtbl.SaveState = Mapper001_SaveState;
	mapper->vtbl.LoadState = Mapper001_LoadState;
	mapper->vtbl.IsStateSave = Mapper001_IsStateSave;

	return (Mapper *)mapper;
}

