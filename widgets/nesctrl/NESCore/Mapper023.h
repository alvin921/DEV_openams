//////////////////////////////////////////////////////////////////////////
// Mapper023  Konami VRC2 type B                                        //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	WORD	addrmask;

	BYTE	reg[9];
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	INT	irq_clock;
}Mapper023;


void	Mapper023_Reset(Mapper *mapper)
{
	DWORD	crc;
	INT i;

	((Mapper023 *)mapper)->addrmask = 0xFFFF;

	for( i = 0; i < 8; i++ ) {
		((Mapper023 *)mapper)->reg[i] = i;
	}
	((Mapper023 *)mapper)->reg[8] = 0;

	((Mapper023 *)mapper)->irq_enable = 0;
	((Mapper023 *)mapper)->irq_counter = 0;
	((Mapper023 *)mapper)->irq_latch = 0;
	((Mapper023 *)mapper)->irq_clock = 0;

	((Mapper023 *)mapper)->reg[9] = 1;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );

//	mapper->nes->SetRenderMethod( POST_RENDER );

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	if( crc == 0x93794634		// Akumajou Special Boku Dracula Kun(J)
	 || crc == 0xc7829dae		// Akumajou Special Boku Dracula Kun(T-Eng)
	 || crc == 0xf82dc02f ) {	// Akumajou Special Boku Dracula Kun(T-Eng v1.02)
		((Mapper023 *)mapper)->addrmask = 0xF00C;
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}
	if( crc == 0xdd53c4ae ) {	// Tiny Toon Adventures(J)
		NES_SetRenderMethod( mapper->nes, POST_ALL_RENDER );
	}
}

void	Mapper023_Write( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, mapper->nes->GetScanline(), mapper->nes->cpu->GetTotalCycles() );
	switch( addr & ((Mapper023 *)mapper)->addrmask ) {
		case 0x8000:
		case 0x8004:
		case 0x8008:
		case 0x800C:
			if(((Mapper023 *)mapper)->reg[8]) {
				SetPROM_8K_Bank( 6, data );
			} else {
				SetPROM_8K_Bank( 4, data );
			}
			break;

		case 0x9000:
			if( data != 0xFF ) {
				data &= 0x03;
				if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_VMIRROR );
				else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
				else		     SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			}
			break;

		case 0x9008:
			((Mapper023 *)mapper)->reg[8] = data & 0x02;
			break;

		case 0xA000:
		case 0xA004:
		case 0xA008:
		case 0xA00C:
			SetPROM_8K_Bank( 5, data );
			break;

		case 0xB000:
			((Mapper023 *)mapper)->reg[0] = (((Mapper023 *)mapper)->reg[0] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 0, ((Mapper023 *)mapper)->reg[0] );
			break;
		case 0xB001:
		case 0xB004:
			((Mapper023 *)mapper)->reg[0] = (((Mapper023 *)mapper)->reg[0] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 0, ((Mapper023 *)mapper)->reg[0] );
			break;

		case 0xB002:
		case 0xB008:
			((Mapper023 *)mapper)->reg[1] = (((Mapper023 *)mapper)->reg[1] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 1, ((Mapper023 *)mapper)->reg[1] );
			break;

		case 0xB003:
		case 0xB00C:
			((Mapper023 *)mapper)->reg[1] = (((Mapper023 *)mapper)->reg[1] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 1, ((Mapper023 *)mapper)->reg[1] );
			break;

		case 0xC000:
			((Mapper023 *)mapper)->reg[2] = (((Mapper023 *)mapper)->reg[2] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 2, ((Mapper023 *)mapper)->reg[2] );
			break;

		case 0xC001:
		case 0xC004:
			((Mapper023 *)mapper)->reg[2] = (((Mapper023 *)mapper)->reg[2] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 2, ((Mapper023 *)mapper)->reg[2] );
			break;

		case 0xC002:
		case 0xC008:
			((Mapper023 *)mapper)->reg[3] = (((Mapper023 *)mapper)->reg[3] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 3, ((Mapper023 *)mapper)->reg[3] );
			break;

		case 0xC003:
		case 0xC00C:
			((Mapper023 *)mapper)->reg[3] = (((Mapper023 *)mapper)->reg[3] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 3, ((Mapper023 *)mapper)->reg[3] );
			break;

		case 0xD000:
			((Mapper023 *)mapper)->reg[4] = (((Mapper023 *)mapper)->reg[4] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 4, ((Mapper023 *)mapper)->reg[4] );
			break;

		case 0xD001:
		case 0xD004:
			((Mapper023 *)mapper)->reg[4] = (((Mapper023 *)mapper)->reg[4] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 4, ((Mapper023 *)mapper)->reg[4] );
			break;

		case 0xD002:
		case 0xD008:
			((Mapper023 *)mapper)->reg[5] = (((Mapper023 *)mapper)->reg[5] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 5, ((Mapper023 *)mapper)->reg[5] );
			break;

		case 0xD003:
		case 0xD00C:
			((Mapper023 *)mapper)->reg[5] = (((Mapper023 *)mapper)->reg[5] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 5, ((Mapper023 *)mapper)->reg[5] );
			break;

		case 0xE000:
			((Mapper023 *)mapper)->reg[6] = (((Mapper023 *)mapper)->reg[6] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 6, ((Mapper023 *)mapper)->reg[6] );
			break;

		case 0xE001:
		case 0xE004:
			((Mapper023 *)mapper)->reg[6] = (((Mapper023 *)mapper)->reg[6] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 6, ((Mapper023 *)mapper)->reg[6] );
			break;

		case 0xE002:
		case 0xE008:
			((Mapper023 *)mapper)->reg[7] = (((Mapper023 *)mapper)->reg[7] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 7, ((Mapper023 *)mapper)->reg[7] );
			break;

		case 0xE003:
		case 0xE00C:
			((Mapper023 *)mapper)->reg[7] = (((Mapper023 *)mapper)->reg[7] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 7, ((Mapper023 *)mapper)->reg[7] );
			break;

		case 0xF000:
			((Mapper023 *)mapper)->irq_latch = (((Mapper023 *)mapper)->irq_latch & 0xF0) | (data & 0x0F);
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case 0xF004:
			((Mapper023 *)mapper)->irq_latch = (((Mapper023 *)mapper)->irq_latch & 0x0F) | ((data & 0x0F) << 4);
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;

		case 0xF008:
			((Mapper023 *)mapper)->irq_enable = data & 0x03;
			((Mapper023 *)mapper)->irq_counter = ((Mapper023 *)mapper)->irq_latch;
			((Mapper023 *)mapper)->irq_clock = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;

		case 0xF00C:
			((Mapper023 *)mapper)->irq_enable = (((Mapper023 *)mapper)->irq_enable & 0x01) * 3;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper023_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper023 *)mapper)->irq_enable & 0x02 ) {
		((Mapper023 *)mapper)->irq_clock += cycles*3;
		while( ((Mapper023 *)mapper)->irq_clock >= 341 ) {
			((Mapper023 *)mapper)->irq_clock -= 341;
			((Mapper023 *)mapper)->irq_counter++;
			if( ((Mapper023 *)mapper)->irq_counter == 0 ) {
				((Mapper023 *)mapper)->irq_counter = ((Mapper023 *)mapper)->irq_latch;
				CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
			}
		}
	}
}

void	Mapper023_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 9; i++ ) {
		p[i] = ((Mapper023 *)mapper)->reg[i];
	}
	p[ 9] = ((Mapper023 *)mapper)->irq_enable;
	p[10] = ((Mapper023 *)mapper)->irq_counter;
	p[11] = ((Mapper023 *)mapper)->irq_latch;
	*(INT*)&p[12] = ((Mapper023 *)mapper)->irq_clock;
}

void	Mapper023_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 9; i++ ) {
		((Mapper023 *)mapper)->reg[i] = p[i];
	}
	((Mapper023 *)mapper)->irq_enable  = p[ 9];
	((Mapper023 *)mapper)->irq_counter = p[10];
	((Mapper023 *)mapper)->irq_latch   = p[11];
	((Mapper023 *)mapper)->irq_clock   = *(INT*)&p[12];
}

BOOL	Mapper023_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper023_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper023));
	
	memset(mapper, 0, sizeof(Mapper023));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper023_Reset;
	mapper->vtbl.Write = Mapper023_Write;
	mapper->vtbl.Clock = Mapper023_Clock;
	mapper->vtbl.SaveState = Mapper023_SaveState;
	mapper->vtbl.LoadState = Mapper023_LoadState;
	mapper->vtbl.IsStateSave = Mapper023_IsStateSave;

	return (Mapper *)mapper;
}

