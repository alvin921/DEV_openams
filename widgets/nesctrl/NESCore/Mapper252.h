//////////////////////////////////////////////////////////////////////////
// Mapper252                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[9];
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_occur;
	INT	irq_clock;
}Mapper252;

void	Mapper252_Reset(Mapper *mapper)
{
	INT i;

 	for(  i = 0; i < 8; i++ ) {
		((Mapper252 *)mapper)->reg[i] = i;
	}
	((Mapper252 *)mapper)->reg[8] = 0;

	((Mapper252 *)mapper)->irq_enable = 0;
	((Mapper252 *)mapper)->irq_counter = 0;
	((Mapper252 *)mapper)->irq_latch = 0;
	((Mapper252 *)mapper)->irq_clock = 0;
	((Mapper252 *)mapper)->irq_occur = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );

	NES_SetRenderMethod( mapper->nes, POST_RENDER );
}

void	Mapper252_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( (addr & 0xF000) == 0x8000 ) {
		SetPROM_8K_Bank( 4, data );
		return;
	}
	if( (addr & 0xF000) == 0xA000 ) {
		SetPROM_8K_Bank( 5, data );
		return;
	}
	switch( addr & 0xF00C ) {
		case 0xB000:
			((Mapper252 *)mapper)->reg[0] = (((Mapper252 *)mapper)->reg[0] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 0, ((Mapper252 *)mapper)->reg[0] );
			break;	
		case 0xB004:
			((Mapper252 *)mapper)->reg[0] = (((Mapper252 *)mapper)->reg[0] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 0, ((Mapper252 *)mapper)->reg[0] );
			break;
		case 0xB008:
			((Mapper252 *)mapper)->reg[1] = (((Mapper252 *)mapper)->reg[1] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 1, ((Mapper252 *)mapper)->reg[1] );
			break;
		case 0xB00C:
			((Mapper252 *)mapper)->reg[1] = (((Mapper252 *)mapper)->reg[1] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 1, ((Mapper252 *)mapper)->reg[1] );
			break;

		case 0xC000:
			((Mapper252 *)mapper)->reg[2] = (((Mapper252 *)mapper)->reg[2] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 2, ((Mapper252 *)mapper)->reg[2] );
			break;
		case 0xC004:
			((Mapper252 *)mapper)->reg[2] = (((Mapper252 *)mapper)->reg[2] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 2, ((Mapper252 *)mapper)->reg[2] );
			break;
		case 0xC008:
			((Mapper252 *)mapper)->reg[3] = (((Mapper252 *)mapper)->reg[3] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 3, ((Mapper252 *)mapper)->reg[3] );
			break;
		case 0xC00C:
			((Mapper252 *)mapper)->reg[3] = (((Mapper252 *)mapper)->reg[3] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 3, ((Mapper252 *)mapper)->reg[3] );
			break;

		case 0xD000:
			((Mapper252 *)mapper)->reg[4] = (((Mapper252 *)mapper)->reg[4] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 4, ((Mapper252 *)mapper)->reg[4] );
			break;
		case 0xD004:
			((Mapper252 *)mapper)->reg[4] = (((Mapper252 *)mapper)->reg[4] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 4, ((Mapper252 *)mapper)->reg[4] );
			break;
		case 0xD008:
			((Mapper252 *)mapper)->reg[5] = (((Mapper252 *)mapper)->reg[5] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 5, ((Mapper252 *)mapper)->reg[5] );
			break;
		case 0xD00C:
			((Mapper252 *)mapper)->reg[5] = (((Mapper252 *)mapper)->reg[5] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 5, ((Mapper252 *)mapper)->reg[5] );
			break;

		case 0xE000:
			((Mapper252 *)mapper)->reg[6] = (((Mapper252 *)mapper)->reg[6] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 6, ((Mapper252 *)mapper)->reg[6] );
			break;
		case 0xE004:
			((Mapper252 *)mapper)->reg[6] = (((Mapper252 *)mapper)->reg[6] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 6, ((Mapper252 *)mapper)->reg[6] );
			break;
		case 0xE008:
			((Mapper252 *)mapper)->reg[7] = (((Mapper252 *)mapper)->reg[7] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 7, ((Mapper252 *)mapper)->reg[7] );
			break;
		case 0xE00C:
			((Mapper252 *)mapper)->reg[7] = (((Mapper252 *)mapper)->reg[7] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 7, ((Mapper252 *)mapper)->reg[7] );
			break;

		case 0xF000:
			((Mapper252 *)mapper)->irq_latch = (((Mapper252 *)mapper)->irq_latch & 0xF0) | (data & 0x0F);
			((Mapper252 *)mapper)->irq_occur = 0;
			break;
		case 0xF004:
			((Mapper252 *)mapper)->irq_latch = (((Mapper252 *)mapper)->irq_latch & 0x0F) | ((data & 0x0F) << 4);
			((Mapper252 *)mapper)->irq_occur = 0;
			break;

		case 0xF008:
			((Mapper252 *)mapper)->irq_enable = data & 0x03;
			if( ((Mapper252 *)mapper)->irq_enable & 0x02 ) {
				((Mapper252 *)mapper)->irq_counter = ((Mapper252 *)mapper)->irq_latch;
				((Mapper252 *)mapper)->irq_clock = 0;
			}
			((Mapper252 *)mapper)->irq_occur = 0;
			CPU_ClrIRQ( ((Mapper252 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;

		case 0xF00C:
			((Mapper252 *)mapper)->irq_enable = (((Mapper252 *)mapper)->irq_enable & 0x01) * 3;
			((Mapper252 *)mapper)->irq_occur = 0;
			CPU_ClrIRQ( ((Mapper252 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper252_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper252 *)mapper)->irq_enable & 0x02 ) {
		if( (((Mapper252 *)mapper)->irq_clock+=cycles) >= 0x72 ) {
			((Mapper252 *)mapper)->irq_clock -= 0x72;
			if( ((Mapper252 *)mapper)->irq_counter == 0xFF ) {
				((Mapper252 *)mapper)->irq_occur = 0xFF;
				((Mapper252 *)mapper)->irq_counter = ((Mapper252 *)mapper)->irq_latch;
				((Mapper252 *)mapper)->irq_enable = (((Mapper252 *)mapper)->irq_enable & 0x01) * 3;

				CPU_SetIRQ( ((Mapper252 *)mapper)->nes->cpu, IRQ_MAPPER );
			} else {
				((Mapper252 *)mapper)->irq_counter++;
			}
		}
//		if( ((Mapper252 *)mapper)->irq_occur ) {
//			((Mapper252 *)mapper)->nes->cpu->IRQ_NotPending();
//		}
	}
}

void	Mapper252_SaveState(Mapper *mapper, LPBYTE p )
{
	INT i;

 	for(  i = 0; i < 9; i++ ) {
		p[i] = ((Mapper252 *)mapper)->reg[i];
	}
	p[ 9] = ((Mapper252 *)mapper)->irq_enable;
	p[10] = ((Mapper252 *)mapper)->irq_counter;
	p[11] = ((Mapper252 *)mapper)->irq_latch;
	*(INT*)&p[12] = ((Mapper252 *)mapper)->irq_clock;
}

void	Mapper252_LoadState(Mapper *mapper, LPBYTE p )
{
	INT i;

 	for(  i = 0; i < 9; i++ ) {
		((Mapper252 *)mapper)->reg[i] = p[i];
	}
	((Mapper252 *)mapper)->irq_enable  = p[ 9];
	((Mapper252 *)mapper)->irq_counter = p[10];
	((Mapper252 *)mapper)->irq_latch   = p[11];
	((Mapper252 *)mapper)->irq_clock   = *(INT*)&p[12];
}

BOOL	Mapper252_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper252_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper252));
	
	memset(mapper, 0, sizeof(Mapper252));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper252_Reset;
	mapper->vtbl.Write = Mapper252_Write;
	mapper->vtbl.Clock = Mapper252_Clock;
	mapper->vtbl.SaveState = Mapper252_SaveState;
	mapper->vtbl.LoadState = Mapper252_LoadState;
	mapper->vtbl.IsStateSave = Mapper252_IsStateSave;

	return (Mapper *)mapper;
}


