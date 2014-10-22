//////////////////////////////////////////////////////////////////////////
// Mapper021  Konami VRC4 (Address mask $F006 or $F0C0)                 //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[9];

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	INT		irq_clock;
}Mapper021;

void	Mapper021_Reset(Mapper *mapper)
{
	INT i;
	for( i = 0; i < 8; i++ ) {
		((Mapper021 *)mapper)->reg[i] = i;
	}
	((Mapper021 *)mapper)->reg[8] = 0;

	((Mapper021 *)mapper)->irq_enable = 0;
	((Mapper021 *)mapper)->irq_counter = 0;
	((Mapper021 *)mapper)->irq_latch = 0;
	((Mapper021 *)mapper)->irq_clock = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper021_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF0CF ) {
		case	0x8000:
			if(((Mapper021 *)mapper)->reg[8] & 0x02) {
				SetPROM_8K_Bank( 6, data );
			} else {
				SetPROM_8K_Bank( 4, data );
			}
			break;
		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;

		case	0x9000:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			break;

		case 0x9002:
		case 0x9080:
			((Mapper021 *)mapper)->reg[8] = data;
			break;

		case 0xB000:
			((Mapper021 *)mapper)->reg[0] = (((Mapper021 *)mapper)->reg[0] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 0, ((Mapper021 *)mapper)->reg[0] );
			break;
		case 0xB002:
		case 0xB040:
			((Mapper021 *)mapper)->reg[0] = (((Mapper021 *)mapper)->reg[0] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 0, ((Mapper021 *)mapper)->reg[0] );
			break;

		case 0xB001:
		case 0xB004:
		case 0xB080:
			((Mapper021 *)mapper)->reg[1] = (((Mapper021 *)mapper)->reg[1] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 1, ((Mapper021 *)mapper)->reg[1] );
			break;
		case 0xB003:
		case 0xB006:
		case 0xB0C0:
			((Mapper021 *)mapper)->reg[1] = (((Mapper021 *)mapper)->reg[1] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 1, ((Mapper021 *)mapper)->reg[1] );
			break;

		case 0xC000:
			((Mapper021 *)mapper)->reg[2] = (((Mapper021 *)mapper)->reg[2] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 2, ((Mapper021 *)mapper)->reg[2] );
			break;
		case 0xC002:
		case 0xC040:
			((Mapper021 *)mapper)->reg[2] = (((Mapper021 *)mapper)->reg[2] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 2, ((Mapper021 *)mapper)->reg[2] );
			break;

		case 0xC001:
		case 0xC004:
		case 0xC080:
			((Mapper021 *)mapper)->reg[3] = (((Mapper021 *)mapper)->reg[3] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 3, ((Mapper021 *)mapper)->reg[3] );
			break;
		case 0xC003:
		case 0xC006:
		case 0xC0C0:
			((Mapper021 *)mapper)->reg[3] = (((Mapper021 *)mapper)->reg[3] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 3, ((Mapper021 *)mapper)->reg[3] );
			break;

		case 0xD000:
			((Mapper021 *)mapper)->reg[4] = (((Mapper021 *)mapper)->reg[4] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 4, ((Mapper021 *)mapper)->reg[4] );
			break;
		case 0xD002:
		case 0xD040:
			((Mapper021 *)mapper)->reg[4] = (((Mapper021 *)mapper)->reg[4] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 4, ((Mapper021 *)mapper)->reg[4] );
			break;

		case 0xD001:
		case 0xD004:
		case 0xD080:
			((Mapper021 *)mapper)->reg[5] = (((Mapper021 *)mapper)->reg[5] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 5, ((Mapper021 *)mapper)->reg[5] );
			break;
		case 0xD003:
		case 0xD006:
		case 0xD0C0:
			((Mapper021 *)mapper)->reg[5] = (((Mapper021 *)mapper)->reg[5] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 5, ((Mapper021 *)mapper)->reg[5] );
			break;

		case 0xE000:
			((Mapper021 *)mapper)->reg[6] = (((Mapper021 *)mapper)->reg[6] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 6, ((Mapper021 *)mapper)->reg[6] );
			break;
		case 0xE002:
		case 0xE040:
			((Mapper021 *)mapper)->reg[6] = (((Mapper021 *)mapper)->reg[6] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 6, ((Mapper021 *)mapper)->reg[6] );
			break;

		case 0xE001:
		case 0xE004:
		case 0xE080:
			((Mapper021 *)mapper)->reg[7] = (((Mapper021 *)mapper)->reg[7] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 7, ((Mapper021 *)mapper)->reg[7] );
			break;
		case 0xE003:
		case 0xE006:
		case 0xE0C0:
			((Mapper021 *)mapper)->reg[7] = (((Mapper021 *)mapper)->reg[7] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 7, ((Mapper021 *)mapper)->reg[7] );
			break;

		case 0xF000:
			((Mapper021 *)mapper)->irq_latch = (((Mapper021 *)mapper)->irq_latch & 0xF0) | (data & 0x0F);
			break;
		case 0xF002:
		case 0xF040:
			((Mapper021 *)mapper)->irq_latch = (((Mapper021 *)mapper)->irq_latch & 0x0F) | ((data & 0x0F) << 4);
			break;

		case 0xF003:
		case 0xF0C0:
		case 0xF006:
			((Mapper021 *)mapper)->irq_enable = (((Mapper021 *)mapper)->irq_enable & 0x01) * 3;
			((Mapper021 *)mapper)->irq_clock = 0;

			CPU_ClrIRQ( ((Mapper021 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;

		case 0xF004:
		case 0xF080:
			((Mapper021 *)mapper)->irq_enable = data & 0x03;
			if( ((Mapper021 *)mapper)->irq_enable & 0x02 ) {
				((Mapper021 *)mapper)->irq_counter = ((Mapper021 *)mapper)->irq_latch;
				((Mapper021 *)mapper)->irq_clock = 0;
			}

			CPU_ClrIRQ( ((Mapper021 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;

//		case 0xF006:
//			((Mapper021 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
//			break;
	}
}

void	Mapper021_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper021 *)mapper)->irq_enable & 0x02 ) {
		if( (((Mapper021 *)mapper)->irq_clock-=cycles) < 0 ) {
			((Mapper021 *)mapper)->irq_clock += 0x72;
			if( ((Mapper021 *)mapper)->irq_counter == 0xFF ) {
				((Mapper021 *)mapper)->irq_counter = ((Mapper021 *)mapper)->irq_latch;
//				((Mapper021 *)mapper)->irq_enable = (((Mapper021 *)mapper)->irq_enable & 0x01) * 3;
//				((Mapper021 *)mapper)->nes->cpu->IRQ_NotPending();
				CPU_SetIRQ( ((Mapper021 *)mapper)->nes->cpu, IRQ_MAPPER );
			} else {
				((Mapper021 *)mapper)->irq_counter++;
			}
		}
	}
}

void	Mapper021_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 9; i++ ) {
		p[i] = ((Mapper021 *)mapper)->reg[i];
	}
	p[ 9] = ((Mapper021 *)mapper)->irq_enable;
	p[10] = ((Mapper021 *)mapper)->irq_counter;
	p[11] = ((Mapper021 *)mapper)->irq_latch;
	*(INT*)&p[12] = ((Mapper021 *)mapper)->irq_clock;
}

void	Mapper021_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 9; i++ ) {
		((Mapper021 *)mapper)->reg[i] = p[i];
	}
	((Mapper021 *)mapper)->irq_enable  = p[ 9];
	((Mapper021 *)mapper)->irq_counter = p[10];
	((Mapper021 *)mapper)->irq_latch   = p[11];
	((Mapper021 *)mapper)->irq_clock   = *(INT*)&p[12];
}

BOOL	Mapper021_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper021_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper021));
	
	memset(mapper, 0, sizeof(Mapper021));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper021_Reset;
	mapper->vtbl.Write = Mapper021_Write;
	mapper->vtbl.Clock = Mapper021_Clock;
	mapper->vtbl.SaveState = Mapper021_SaveState;
	mapper->vtbl.LoadState = Mapper021_LoadState;
	mapper->vtbl.IsStateSave = Mapper021_IsStateSave;

	return (Mapper *)mapper;
}

