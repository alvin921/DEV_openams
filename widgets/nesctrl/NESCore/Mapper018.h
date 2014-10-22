//////////////////////////////////////////////////////////////////////////
// Mapper018  Jaleco SS8806                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[11];

	BYTE	irq_enable;
	BYTE	irq_mode;
	INT	irq_latch;
	INT	irq_counter;
}Mapper018;

void	Mapper018_Reset(Mapper *mapper)
{
	INT i;
	DWORD	crc;
	for( i = 0; i < 11; i++ ) {
		((Mapper018 *)mapper)->reg[i] = 0;
	}
	((Mapper018 *)mapper)->reg[2] = PROM_8K_SIZE-2;
	((Mapper018 *)mapper)->reg[3] = PROM_8K_SIZE-1;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	((Mapper018 *)mapper)->irq_enable  = 0;
	((Mapper018 *)mapper)->irq_mode    = 0;
	((Mapper018 *)mapper)->irq_counter = 0xFFFF;
	((Mapper018 *)mapper)->irq_latch   = 0xFFFF;

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	if( crc == 0xefb1df9e ) {	// The Lord of King(J)
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}
	if( crc == 0x3746f951 ) {	// Pizza Pop!(J)
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}

//	nes->SetRenderMethod( PRE_ALL_RENDER );
//	nes->SetRenderMethod( POST_ALL_RENDER );
}

void	Mapper018_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			((Mapper018 *)mapper)->reg[0] = (((Mapper018 *)mapper)->reg[0] & 0xF0) | (data & 0x0F);
			SetPROM_8K_Bank( 4, ((Mapper018 *)mapper)->reg[0] );
			break;
		case	0x8001:
			((Mapper018 *)mapper)->reg[0] = (((Mapper018 *)mapper)->reg[0] & 0x0F) | ((data & 0x0F) << 4);
			SetPROM_8K_Bank( 4, ((Mapper018 *)mapper)->reg[0] );
			break;
		case	0x8002:
			((Mapper018 *)mapper)->reg[1] = (((Mapper018 *)mapper)->reg[1] & 0xF0) | (data & 0x0F);
			SetPROM_8K_Bank( 5, ((Mapper018 *)mapper)->reg[1] );
			break;
		case	0x8003:
			((Mapper018 *)mapper)->reg[1] = (((Mapper018 *)mapper)->reg[1] & 0x0F) | ((data & 0x0F) << 4);
			SetPROM_8K_Bank( 5, ((Mapper018 *)mapper)->reg[1] );
			break;
		case	0x9000:
			((Mapper018 *)mapper)->reg[2] = (((Mapper018 *)mapper)->reg[2] & 0xF0) | (data & 0x0F);
			SetPROM_8K_Bank( 6, ((Mapper018 *)mapper)->reg[2] );
			break;
		case	0x9001:
			((Mapper018 *)mapper)->reg[2] = (((Mapper018 *)mapper)->reg[2] & 0x0F) | ((data & 0x0F) << 4);
			SetPROM_8K_Bank( 6, ((Mapper018 *)mapper)->reg[2] );
			break;

		case	0xA000:
			((Mapper018 *)mapper)->reg[3] = (((Mapper018 *)mapper)->reg[3] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 0, ((Mapper018 *)mapper)->reg[3] );
			break;
		case	0xA001:
			((Mapper018 *)mapper)->reg[3] = (((Mapper018 *)mapper)->reg[3] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 0, ((Mapper018 *)mapper)->reg[3] );
			break;
		case	0xA002:
			((Mapper018 *)mapper)->reg[4] = (((Mapper018 *)mapper)->reg[4] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 1, ((Mapper018 *)mapper)->reg[4] );
			break;
		case	0xA003:
			((Mapper018 *)mapper)->reg[4] = (((Mapper018 *)mapper)->reg[4] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 1, ((Mapper018 *)mapper)->reg[4] );
			break;

		case	0xB000:
			((Mapper018 *)mapper)->reg[5] = (((Mapper018 *)mapper)->reg[5] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 2, ((Mapper018 *)mapper)->reg[5] );
			break;
		case	0xB001:
			((Mapper018 *)mapper)->reg[5] = (((Mapper018 *)mapper)->reg[5] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 2, ((Mapper018 *)mapper)->reg[5] );
			break;
		case	0xB002:
			((Mapper018 *)mapper)->reg[6] = (((Mapper018 *)mapper)->reg[6] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 3, ((Mapper018 *)mapper)->reg[6] );
			break;
		case	0xB003:
			((Mapper018 *)mapper)->reg[6] = (((Mapper018 *)mapper)->reg[6] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 3, ((Mapper018 *)mapper)->reg[6] );
			break;

		case	0xC000:
			((Mapper018 *)mapper)->reg[7] = (((Mapper018 *)mapper)->reg[7] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 4, ((Mapper018 *)mapper)->reg[7] );
			break;
		case	0xC001:
			((Mapper018 *)mapper)->reg[7] = (((Mapper018 *)mapper)->reg[7] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 4, ((Mapper018 *)mapper)->reg[7] );
			break;
		case	0xC002:
			((Mapper018 *)mapper)->reg[8] = (((Mapper018 *)mapper)->reg[8] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 5, ((Mapper018 *)mapper)->reg[8] );
			break;
		case	0xC003:
			((Mapper018 *)mapper)->reg[8] = (((Mapper018 *)mapper)->reg[8] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 5, ((Mapper018 *)mapper)->reg[8] );
			break;

		case	0xD000:
			((Mapper018 *)mapper)->reg[9] = (((Mapper018 *)mapper)->reg[9] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 6, ((Mapper018 *)mapper)->reg[9] );
			break;
		case	0xD001:
			((Mapper018 *)mapper)->reg[9] = (((Mapper018 *)mapper)->reg[9] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 6, ((Mapper018 *)mapper)->reg[9] );
			break;
		case	0xD002:
			((Mapper018 *)mapper)->reg[10] = (((Mapper018 *)mapper)->reg[10] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 7, ((Mapper018 *)mapper)->reg[10] );
			break;
		case	0xD003:
			((Mapper018 *)mapper)->reg[10] = (((Mapper018 *)mapper)->reg[10] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 7, ((Mapper018 *)mapper)->reg[10] );
			break;

		case	0xE000:
			((Mapper018 *)mapper)->irq_latch = (((Mapper018 *)mapper)->irq_latch & 0xFFF0) | (data & 0x0F);
			break;
		case	0xE001:
			((Mapper018 *)mapper)->irq_latch = (((Mapper018 *)mapper)->irq_latch & 0xFF0F) | ((data & 0x0F) << 4);
			break;
		case	0xE002:
			((Mapper018 *)mapper)->irq_latch = (((Mapper018 *)mapper)->irq_latch & 0xF0FF) | ((data & 0x0F) << 8);
			break;
		case	0xE003:
			((Mapper018 *)mapper)->irq_latch = (((Mapper018 *)mapper)->irq_latch & 0x0FFF) | ((data & 0x0F) << 12);
			break;

		case	0xF000:
//			if( data & 0x01 ) {
				((Mapper018 *)mapper)->irq_counter = ((Mapper018 *)mapper)->irq_latch;
//			} else {
//				((Mapper018 *)mapper)->irq_counter = 0;
//			}
			break;
		case	0xF001:
			((Mapper018 *)mapper)->irq_mode = (data>>1) & 0x07;
			((Mapper018 *)mapper)->irq_enable = (data & 0x01);
//			if( !((Mapper018 *)mapper)->irq_enable ) {
				CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
//			}
			break;

		case	0xF002:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else		     SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			break;
	}
}

void	Mapper018_Clock( Mapper *mapper, INT cycles )
{
BOOL	bIRQ = FALSE;
INT	irq_counter_old = ((Mapper018 *)mapper)->irq_counter;

	if( ((Mapper018 *)mapper)->irq_enable && ((Mapper018 *)mapper)->irq_counter ) {
		((Mapper018 *)mapper)->irq_counter -= cycles;

		switch( ((Mapper018 *)mapper)->irq_mode ) {
			case	0:
				if( ((Mapper018 *)mapper)->irq_counter <= 0 ) {
					bIRQ = TRUE;
				}
				break;
			case	1:
				if( (((Mapper018 *)mapper)->irq_counter & 0xF000) != (irq_counter_old & 0xF000) ) {
					bIRQ = TRUE;
				}
				break;
			case	2:
			case	3:
				if( (((Mapper018 *)mapper)->irq_counter & 0xFF00) != (irq_counter_old & 0xFF00) ) {
					bIRQ = TRUE;
				}
				break;
			case	4:
			case	5:
			case	6:
			case	7:
				if( (((Mapper018 *)mapper)->irq_counter & 0xFFF0) != (irq_counter_old & 0xFFF0) ) {
					bIRQ = TRUE;
				}
				break;
		}

		if( bIRQ ) {
////			((Mapper018 *)mapper)->irq_enable = 0;
//			((Mapper018 *)mapper)->irq_counter = ((Mapper018 *)mapper)->irq_latch;
			((Mapper018 *)mapper)->irq_counter = 0;
			((Mapper018 *)mapper)->irq_enable = 0;
//			nes->cpu->IRQ_NotPending();
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
		}
	}
}

void	Mapper018_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 11; i++ ) {
		p[i] = ((Mapper018 *)mapper)->reg[i];
	}
	p[11] = ((Mapper018 *)mapper)->irq_enable;
	p[12] = ((Mapper018 *)mapper)->irq_mode;
	*(INT*)&p[13] = ((Mapper018 *)mapper)->irq_counter;
	*(INT*)&p[17] = ((Mapper018 *)mapper)->irq_latch;
}

void	Mapper018_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 11; i++ ) {
		p[i] = ((Mapper018 *)mapper)->reg[i];
	}
	((Mapper018 *)mapper)->irq_enable  = p[11];
	((Mapper018 *)mapper)->irq_mode    = p[12];
	((Mapper018 *)mapper)->irq_counter = *(INT*)&p[13];
	((Mapper018 *)mapper)->irq_latch   = *(INT*)&p[17];
}

BOOL	Mapper018_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper018_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper018));
	
	memset(mapper, 0, sizeof(Mapper018));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper018_Reset;
	mapper->vtbl.Write = Mapper018_Write;
	mapper->vtbl.Clock = Mapper018_Clock;
	mapper->vtbl.SaveState = Mapper018_SaveState;
	mapper->vtbl.LoadState = Mapper018_LoadState;
	mapper->vtbl.IsStateSave = Mapper018_IsStateSave;

	return (Mapper *)mapper;
}

