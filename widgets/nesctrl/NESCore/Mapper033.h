//////////////////////////////////////////////////////////////////////////
// Mapper033  Taito TC0190                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[7];

	BYTE	patch;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
}Mapper033;

void	Mapper033_SetBank(Mapper *mapper);

void	Mapper033_Reset(Mapper *mapper)
{
	DWORD crc;
	((Mapper033 *)mapper)->patch = 0;

	((Mapper033 *)mapper)->irq_enable = 0;
	((Mapper033 *)mapper)->irq_counter = 0;
	((Mapper033 *)mapper)->irq_latch  = 0;

	((Mapper033 *)mapper)->reg[0] = 0;
	((Mapper033 *)mapper)->reg[1] = 2;
	((Mapper033 *)mapper)->reg[2] = 4;
	((Mapper033 *)mapper)->reg[3] = 5;
	((Mapper033 *)mapper)->reg[4] = 6;
	((Mapper033 *)mapper)->reg[5] = 7;
	((Mapper033 *)mapper)->reg[6] = 1;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		Mapper033_SetBank(mapper);
	}

	crc = ROM_GetPROM_CRC(((Mapper033 *)mapper)->nes->rom);
	// Check For Old #33 games.... (CRC code by NesToy)
	if( crc == 0x5e9bc161		// Akira(J)
	 || crc == 0xecdbafa4		// Bakushou!! Jinsei Gekijou(J)
	 || crc == 0x59cd0c31		// Don Doko Don(J)
	 || crc == 0x837c1342		// Golf Ko Open(J)
	 || crc == 0x42d893e4		// Operation Wolf(J)
	 || crc == 0x1388aeb9		// Operation Wolf(U)
	 || crc == 0x07ee6d8f		// Power Blazer(J)
	 || crc == 0x5193fb54		// Takeshi no Sengoku Fuuunji(J)
	 || crc == 0xa71c3452 ) {	// Insector X(J)
		((Mapper033 *)mapper)->patch = 1;
	}

	NES_SetRenderMethod( mapper->nes, PRE_RENDER );

	if( crc == 0x202df297 ) {	// Captain Saver(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x63bb86b5 ) {	// The Jetsons(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
}

void	Mapper033_Write( Mapper *mapper, WORD addr, BYTE data )
{
//	LOG( "Mapper033 addr=%04X data=%02X", addr&0xFFFF, data&0xFF );

	switch( addr ) {
		case	0x8000:
			if( ((Mapper033 *)mapper)->patch ) {
				if( data & 0x40 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
				SetPROM_8K_Bank( 4, data & 0x1F );
			} else {
				SetPROM_8K_Bank( 4, data );
			}
			break;
		case	0x8001:
			if( ((Mapper033 *)mapper)->patch ) {
				SetPROM_8K_Bank( 5, data & 0x1F );
			} else {
				SetPROM_8K_Bank( 5, data );
			}
			break;

		case	0x8002:
			((Mapper033 *)mapper)->reg[0] = data;
			Mapper033_SetBank(mapper);
			break;
		case	0x8003:
			((Mapper033 *)mapper)->reg[1] = data;
			Mapper033_SetBank(mapper);
			break;
		case	0xA000:
			((Mapper033 *)mapper)->reg[2] = data;
			Mapper033_SetBank(mapper);
			break;
		case	0xA001:
			((Mapper033 *)mapper)->reg[3] = data;
			Mapper033_SetBank(mapper);
			break;
		case	0xA002:
			((Mapper033 *)mapper)->reg[4] = data;
			Mapper033_SetBank(mapper);
			break;
		case	0xA003:
			((Mapper033 *)mapper)->reg[5] = data;
			Mapper033_SetBank(mapper);
			break;

#if	0
		case	0xC003:
		case	0xE003:
			((Mapper033 *)mapper)->reg[6] = data;
			Mapper033_SetBank(mapper);
			break;

		case	0xC000:
			((Mapper033 *)mapper)->irq_counter = data;
//			((Mapper033 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case	0xC001:
		case	0xC002:
		case	0xE001:
		case	0xE002:
			((Mapper033 *)mapper)->irq_enable = data;
//			((Mapper033 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
#else
		case	0xC000:
			((Mapper033 *)mapper)->irq_latch = data;
			((Mapper033 *)mapper)->irq_counter = ((Mapper033 *)mapper)->irq_latch;
			break;
		case	0xC001:
			((Mapper033 *)mapper)->irq_counter = ((Mapper033 *)mapper)->irq_latch;
			break;
		case	0xC002:
			((Mapper033 *)mapper)->irq_enable = 1;
			break;
		case	0xC003:
			((Mapper033 *)mapper)->irq_enable = 0;
			break;

		case	0xE001:
		case	0xE002:
		case	0xE003:
			break;
#endif

		case	0xE000:
			if( data & 0x40 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;
	}
}

void	Mapper033_HSync( Mapper *mapper, INT scanline )
{
#if	0
//	((Mapper033 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
	if( scanline >= 0 && scanline <= 239 ) {
		if( PPU_IsDispON(((Mapper033 *)mapper)->nes->ppu) ) {
			if( ((Mapper033 *)mapper)->irq_enable ) {
				if( ((Mapper033 *)mapper)->irq_counter == 0xFF ) {
					((Mapper033 *)mapper)->irq_enable  = 0;
					((Mapper033 *)mapper)->irq_counter = 0;
//					CPU_SetIRQ( ((Mapper033 *)mapper)->nes->cpu, IRQ_MAPPER );
					CPU_SetIRQ( ((Mapper033 *)mapper)->nes->cpu, IRQ_TRIGGER );
				} else {
					((Mapper033 *)mapper)->irq_counter++;
				}
			}
		}
	}
#else
	if( scanline >= 0 && scanline < SCREEN_HEIGHT && PPU_IsDispON(((Mapper033 *)mapper)->nes->ppu) ) {
		if( ((Mapper033 *)mapper)->irq_enable ) {
			if( ++((Mapper033 *)mapper)->irq_counter == 0 ) {
				((Mapper033 *)mapper)->irq_enable  = 0;
				((Mapper033 *)mapper)->irq_counter = 0;
				CPU_SetIRQ( ((Mapper033 *)mapper)->nes->cpu, IRQ_TRIGGER );
			}
		}
	}
#endif
}

void	Mapper033_SetBank(Mapper *mapper)
{
	SetVROM_2K_Bank( 0, ((Mapper033 *)mapper)->reg[0] );
	SetVROM_2K_Bank( 2, ((Mapper033 *)mapper)->reg[1] );

//	if( ((Mapper033 *)mapper)->reg[6] & 0x01 ) {
		SetVROM_1K_Bank( 4, ((Mapper033 *)mapper)->reg[2] );
		SetVROM_1K_Bank( 5, ((Mapper033 *)mapper)->reg[3] );
		SetVROM_1K_Bank( 6, ((Mapper033 *)mapper)->reg[4] );
		SetVROM_1K_Bank( 7, ((Mapper033 *)mapper)->reg[5] );
//	} else {
//		SetVROM_2K_Bank( 4, ((Mapper033 *)mapper)->reg[0] );
//		SetVROM_2K_Bank( 6, ((Mapper033 *)mapper)->reg[1] );
//	}
}

void	Mapper033_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 7; i++ ) {
		p[i] = ((Mapper033 *)mapper)->reg[i];
	}

	p[7] = ((Mapper033 *)mapper)->irq_enable;
	p[8] = ((Mapper033 *)mapper)->irq_counter;
	p[9] = ((Mapper033 *)mapper)->irq_latch;
}

void	Mapper033_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 7; i++ ) {
		((Mapper033 *)mapper)->reg[i] = p[i];
	}

	((Mapper033 *)mapper)->irq_enable  = p[7];
	((Mapper033 *)mapper)->irq_counter = p[8];
	((Mapper033 *)mapper)->irq_latch   = p[9];
}

BOOL	Mapper033_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper033_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper033));
	
	memset(mapper, 0, sizeof(Mapper033));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper033_Reset;
	mapper->vtbl.Write = Mapper033_Write;
	mapper->vtbl.HSync = Mapper033_HSync;
	mapper->vtbl.SaveState = Mapper033_SaveState;
	mapper->vtbl.LoadState = Mapper033_LoadState;
	mapper->vtbl.IsStateSave = Mapper033_IsStateSave;

	return (Mapper *)mapper;
}

