//////////////////////////////////////////////////////////////////////////
// Mapper019  Namcot 106                                                //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;
	BYTE	exsound_enable;

	BYTE	reg[3];
	BYTE	exram[128];

	BYTE	irq_enable;
	WORD	irq_counter;
}Mapper019;


void	Mapper019_Reset(Mapper *mapper)
{
	DWORD	crc;
	((Mapper019 *)mapper)->patch = 0;

	((Mapper019 *)mapper)->reg[0] = ((Mapper019 *)mapper)->reg[1] = ((Mapper019 *)mapper)->reg[2] = 0;

	memset( ((Mapper019 *)mapper)->exram, 0, sizeof(((Mapper019 *)mapper)->exram) );

	((Mapper019 *)mapper)->irq_enable = 0;
	((Mapper019 *)mapper)->irq_counter = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE >= 8 ) {
		SetVROM_8K_Bank_cont( VROM_8K_SIZE-1 );
	}

	((Mapper019 *)mapper)->exsound_enable = 0xFF;

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	if( crc == 0xb62a7b71 ) {	// Family Circuit '91(J)
		((Mapper019 *)mapper)->patch = 1;
	}

	if( crc == 0x02738c68 ) {	// Wagan Land 2(J)
		((Mapper019 *)mapper)->patch = 3;
	}
	if( crc == 0x14942c06 ) {	// Wagan Land 3(J)
		((Mapper019 *)mapper)->patch = 2;
	}

	if( crc == 0x968dcf09 ) {	// Final Lap(J)
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}
	if( crc == 0x3deac303 ) {	// Rolling Thunder(J)
		NES_SetRenderMethod( mapper->nes, POST_ALL_RENDER );
	}

	if( crc == 0xb1b9e187 ) {	// For Kaijuu Monogatari(J)
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}

	if( crc == 0x6901346e ) {	// For Sangokushi 2 - Haou no Tairiku(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}

//	if( crc == 0xdd454208 ) {	// Hydlide 3(J)
//		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
//	}

	if( crc == 0xaf15338f		// For Mindseeker(J)
	 || crc == 0xb1b9e187		// For Kaijuu Monogatari(J)
	 || crc == 0x96533999		// Dokuganryuu Masamune(J)
//	 || crc == 0x2b825ce1		// Namco Classic(J)
//	 || crc == 0x9a2b0641		// Namco Classic 2(J)
	 || crc == 0x3296ff7a		// Battle Fleet(J)
	 || crc == 0xdd454208 ) {	// Hydlide 3(J)
		((Mapper019 *)mapper)->exsound_enable = 0;
	}

	if( crc == 0x429fd177 ) {	// Famista '90(J)
		((Mapper019 *)mapper)->exsound_enable = 0;
	}

	if( ((Mapper019 *)mapper)->exsound_enable ) {
		APU_SelectExSound( mapper->nes->apu, 0x10 );
	}
}

BYTE	Mapper019_ReadLow( Mapper *mapper, WORD addr )
{
BYTE	data = 0;

	switch( addr & 0xF800 ) {
		case	0x4800:
			if( addr == 0x4800 ) {
				if( ((Mapper019 *)mapper)->exsound_enable ) {
					APU_ExRead(mapper->nes->apu, addr);
					data = ((Mapper019 *)mapper)->exram[((Mapper019 *)mapper)->reg[2]&0x7F];
				} else {
					data = WRAM[((Mapper019 *)mapper)->reg[2]&0x7F];
				}
				if( ((Mapper019 *)mapper)->reg[2]&0x80 )
					((Mapper019 *)mapper)->reg[2] = (((Mapper019 *)mapper)->reg[2]+1)|0x80;
				return	data;
			}
			break;
		case	0x5000:
			return	(BYTE)((Mapper019 *)mapper)->irq_counter & 0x00FF;
		case	0x5800:
			return	(BYTE)((((Mapper019 *)mapper)->irq_counter>>8) & 0x7F);
		case	0x6000:
		case	0x6800:
		case	0x7000:
		case	0x7800:
			return	Mapper_DefReadLow( mapper, addr );
	}

	return	(BYTE)(addr>>8);
}

void	Mapper019_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF800 ) {
		case	0x4800:
			if( addr == 0x4800 ) {
				if( ((Mapper019 *)mapper)->exsound_enable ) {
					APU_ExWrite( mapper->nes->apu, addr, data );
					((Mapper019 *)mapper)->exram[((Mapper019 *)mapper)->reg[2]&0x7F] = data;
				} else {
					WRAM[((Mapper019 *)mapper)->reg[2]&0x7F] = data;
				}
				if( ((Mapper019 *)mapper)->reg[2]&0x80 )
					((Mapper019 *)mapper)->reg[2] = (((Mapper019 *)mapper)->reg[2]+1)|0x80;
			}
			break;
		case	0x5000:
			((Mapper019 *)mapper)->irq_counter = (((Mapper019 *)mapper)->irq_counter & 0xFF00) | (WORD)data;
//			if( ((Mapper019 *)mapper)->irq_enable ) {
//				((Mapper019 *)mapper)->irq_counter++;
//			}
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0x5800:
			((Mapper019 *)mapper)->irq_counter = (((Mapper019 *)mapper)->irq_counter & 0x00FF) | ((WORD)(data & 0x7F) << 8);
			((Mapper019 *)mapper)->irq_enable  = data & 0x80;
//			if( ((Mapper019 *)mapper)->irq_enable ) {
//				((Mapper019 *)mapper)->irq_counter++;
//			}
//			if( !((Mapper019 *)mapper)->irq_enable ) {
//				mapper->nes->cpu->ClrIRQ( IRQ_MAPPER );
//			}
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0x6000:
		case	0x6800:
		case	0x7000:
		case	0x7800:
			Mapper_DefWriteLow( mapper, addr, data );
			break;
	}
}

void	Mapper019_Write( Mapper *mapper, WORD addr, BYTE data )
{
//if( addr >= 0xC000 ) {
//DebugOut( "W %04X %02X L:%3d\n", addr, data, mapper->nes->GetScanline() );
//}
	switch( addr & 0xF800 ) {
		case	0x8000:
			if( (data < 0xE0) || (((Mapper019 *)mapper)->reg[0] != 0) ) {
				SetVROM_1K_Bank( 0, data );
			} else {
				SetCRAM_1K_Bank( 0, data&0x1F );
			}
			break;
		case	0x8800:
			if( (data < 0xE0) || (((Mapper019 *)mapper)->reg[0] != 0) ) {
				SetVROM_1K_Bank( 1, data );
			} else {
				SetCRAM_1K_Bank( 1, data&0x1F );
			}
			break;
		case	0x9000:
			if( (data < 0xE0) || (((Mapper019 *)mapper)->reg[0] != 0) ) {
				SetVROM_1K_Bank( 2, data );
			} else {
				SetCRAM_1K_Bank( 2, data&0x1F );
			}
			break;
		case	0x9800:
			if( (data < 0xE0) || (((Mapper019 *)mapper)->reg[0] != 0) ) {
				SetVROM_1K_Bank( 3, data );
			} else {
				SetCRAM_1K_Bank( 3, data&0x1F );
			}
			break;
		case	0xA000:
			if( (data < 0xE0) || (((Mapper019 *)mapper)->reg[1] != 0) ) {
				SetVROM_1K_Bank( 4, data );
			} else {
				SetCRAM_1K_Bank( 4, data&0x1F );
			}
			break;
		case	0xA800:
			if( (data < 0xE0) || (((Mapper019 *)mapper)->reg[1] != 0) ) {
				SetVROM_1K_Bank( 5, data );
			} else {
				SetCRAM_1K_Bank( 5, data&0x1F );
			}
			break;
		case	0xB000:
			if( (data < 0xE0) || (((Mapper019 *)mapper)->reg[1] != 0) ) {
				SetVROM_1K_Bank( 6, data );
			} else {
				SetCRAM_1K_Bank( 6, data&0x1F );
			}
			break;
		case	0xB800:
			if( (data < 0xE0) || (((Mapper019 *)mapper)->reg[1] != 0) ) {
				SetVROM_1K_Bank( 7, data );
			} else {
				SetCRAM_1K_Bank( 7, data&0x1F );
			}
			break;
		case	0xC000:
			if( !((Mapper019 *)mapper)->patch ) {
				if( data <= 0xDF ) {
					SetVROM_1K_Bank( 8, data );
				} else {
					SetVRAM_1K_Bank( 8, data & 0x01 );
				}
			}
			break;
		case	0xC800:
			if( !((Mapper019 *)mapper)->patch ) {
				if( data <= 0xDF ) {
					SetVROM_1K_Bank( 9, data );
				} else {
					SetVRAM_1K_Bank( 9, data & 0x01 );
				}
			}
			break;
		case	0xD000:
			if( !((Mapper019 *)mapper)->patch ) {
				if( data <= 0xDF ) {
					SetVROM_1K_Bank( 10, data );
				} else {
					SetVRAM_1K_Bank( 10, data & 0x01 );
				}
			}
			break;
		case	0xD800:
			if( !((Mapper019 *)mapper)->patch ) {
				if( data <= 0xDF ) {
					SetVROM_1K_Bank( 11, data );
				} else {
					SetVRAM_1K_Bank( 11, data & 0x01 );
				}
			}
			break;
		case	0xE000:
			SetPROM_8K_Bank( 4, data & 0x3F );
			if( ((Mapper019 *)mapper)->patch == 2 ) {
				if( data & 0x40 ) SetVRAM_Mirror_cont( VRAM_VMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			}
			if( ((Mapper019 *)mapper)->patch == 3 ) {
				if( data & 0x80 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xE800:
			((Mapper019 *)mapper)->reg[0] = data & 0x40;
			((Mapper019 *)mapper)->reg[1] = data & 0x80;
			SetPROM_8K_Bank( 5, data & 0x3F );
			break;
		case	0xF000:
			SetPROM_8K_Bank( 6, data & 0x3F );
			break;
		case	0xF800:
			if( addr == 0xF800 ) {
				if( ((Mapper019 *)mapper)->exsound_enable ) {
					APU_ExWrite( mapper->nes->apu, addr, data );
				}
				((Mapper019 *)mapper)->reg[2] = data;
			}
			break;
	}
}

void	Mapper019_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper019 *)mapper)->irq_enable ) {
		if( (((Mapper019 *)mapper)->irq_counter+=cycles) >= 0x7FFF ) {
//			((Mapper019 *)mapper)->irq_counter = 0x7FFF;
//			mapper->nes->cpu->IRQ_NotPending();

			((Mapper019 *)mapper)->irq_enable  = 0;
//			((Mapper019 *)mapper)->irq_counter &= 0x7FFF;
			((Mapper019 *)mapper)->irq_counter = 0x7FFF;
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
		}
	}
}

void	Mapper019_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper019 *)mapper)->reg[0];
	p[1] = ((Mapper019 *)mapper)->reg[1];
	p[2] = ((Mapper019 *)mapper)->reg[2];
	p[3] = ((Mapper019 *)mapper)->irq_enable;
	*(WORD*)&p[4] = ((Mapper019 *)mapper)->irq_counter;

	memcpy( &p[8], ((Mapper019 *)mapper)->exram, sizeof(((Mapper019 *)mapper)->exram) );
}

void	Mapper019_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper019 *)mapper)->reg[0] = p[0];
	((Mapper019 *)mapper)->reg[1] = p[1];
	((Mapper019 *)mapper)->reg[2] = p[2];
	((Mapper019 *)mapper)->irq_enable  = p[3];
	((Mapper019 *)mapper)->irq_counter = *(WORD*)&p[4];

	memcpy( ((Mapper019 *)mapper)->exram, &p[8], sizeof(((Mapper019 *)mapper)->exram) );
}

BOOL	Mapper019_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper019_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper019));
	
	memset(mapper, 0, sizeof(Mapper019));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper019_Reset;
	mapper->vtbl.ReadLow = Mapper019_ReadLow;
	mapper->vtbl.WriteLow = Mapper019_WriteLow;
	mapper->vtbl.Write = Mapper019_Write;
	mapper->vtbl.Clock = Mapper019_Clock;
	mapper->vtbl.SaveState = Mapper019_SaveState;
	mapper->vtbl.LoadState = Mapper019_LoadState;
	mapper->vtbl.IsStateSave = Mapper019_IsStateSave;

	return (Mapper *)mapper;
}

