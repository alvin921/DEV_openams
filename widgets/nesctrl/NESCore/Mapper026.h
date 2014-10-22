
//////////////////////////////////////////////////////////////////////////
// Mapper026  Konami VRC6 (PA0,PA1 reverse)                             //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	INT irq_clock;
}Mapper026;

void	Mapper026_Reset(Mapper *mapper)
{
	DWORD crc;
	((Mapper026 *)mapper)->irq_enable = 0;
	((Mapper026 *)mapper)->irq_counter = 0;
	((Mapper026 *)mapper)->irq_latch = 0;
	((Mapper026 *)mapper)->irq_clock = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	crc = ROM_GetPROM_CRC(((Mapper026 *)mapper)->nes->rom);
	if( crc == 0x30e64d03 ) {	// Esper Dream 2 - Aratanaru Tatakai(J)
		NES_SetRenderMethod( mapper->nes, POST_ALL_RENDER );
	}
	if( crc == 0x836cc1ab ) {	// Mouryou Senki Madara(J)
		NES_SetRenderMethod( mapper->nes, POST_RENDER );
	}

	APU_SelectExSound( ((Mapper026 *)mapper)->nes->apu, 1 );
}

void	Mapper026_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF003 ) {
		case 0x8000:
			SetPROM_16K_Bank( 4, data );
			break;

		case 0x9000: case 0x9001: case 0x9002: case 0x9003:
		case 0xA000: case 0xA001: case 0xA002: case 0xA003:
		case 0xB000: case 0xB001: case 0xB002:
			addr = (addr&0xfffc)|((addr&1)<<1)|((addr&2)>>1);
			APU_ExWrite( ((Mapper026 *)mapper)->nes->apu, addr, data );
			break;

		case 0xB003:
			data = data & 0x7F;
			if( data == 0x08 || data == 0x2C ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			else if( data == 0x20 )		   SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 0x24 )		   SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 0x28 )		   SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			break;

		case 0xC000:
			SetPROM_8K_Bank( 6, data );
			break;

		case 0xD000:
			SetVROM_1K_Bank( 0, data );
			break;

		case 0xD001:
			SetVROM_1K_Bank( 2, data );
			break;

		case 0xD002:
			SetVROM_1K_Bank( 1, data );
			break;

		case 0xD003:
			SetVROM_1K_Bank( 3, data );
			break;

		case 0xE000:
			SetVROM_1K_Bank( 4, data );
			break;

		case 0xE001:
			SetVROM_1K_Bank( 6, data );
			break;

		case 0xE002:
			SetVROM_1K_Bank( 5, data );
			break;

		case 0xE003:
			SetVROM_1K_Bank( 7, data );
			break;

		case 0xF000:
			((Mapper026 *)mapper)->irq_latch = data;
			break;
		case 0xF001:
			((Mapper026 *)mapper)->irq_enable = (((Mapper026 *)mapper)->irq_enable & 0x01) * 3;
			CPU_ClrIRQ( ((Mapper026 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case 0xF002:
			((Mapper026 *)mapper)->irq_enable = data & 0x03;
			if( ((Mapper026 *)mapper)->irq_enable & 0x02 ) {
				((Mapper026 *)mapper)->irq_counter = ((Mapper026 *)mapper)->irq_latch;
				((Mapper026 *)mapper)->irq_clock = 0;
			}
			CPU_ClrIRQ( ((Mapper026 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper026_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper026 *)mapper)->irq_enable & 0x02 ) {
		if( (((Mapper026 *)mapper)->irq_clock+=cycles) >= 0x72 ) {
			((Mapper026 *)mapper)->irq_clock -= 0x72;
			if( ((Mapper026 *)mapper)->irq_counter >= 0xFF ) {
				((Mapper026 *)mapper)->irq_counter = ((Mapper026 *)mapper)->irq_latch;
//				((Mapper026 *)mapper)->nes->cpu->IRQ_NotPending();
////				((Mapper026 *)mapper)->nes->cpu->IRQ();
				CPU_SetIRQ( ((Mapper026 *)mapper)->nes->cpu, IRQ_MAPPER );
			} else {
				((Mapper026 *)mapper)->irq_counter++;
			}
		}
	}
}

void	Mapper026_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper026 *)mapper)->irq_enable;
	p[1] = ((Mapper026 *)mapper)->irq_counter;
	p[2] = ((Mapper026 *)mapper)->irq_latch;
	*(INT*)&p[3] = ((Mapper026 *)mapper)->irq_clock;
}

void	Mapper026_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper026 *)mapper)->irq_enable  = p[0];
	((Mapper026 *)mapper)->irq_counter = p[1];
	((Mapper026 *)mapper)->irq_latch   = p[2];
	((Mapper026 *)mapper)->irq_clock   = *(INT*)&p[3];
}

BOOL	Mapper026_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper026_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper026));
	
	memset(mapper, 0, sizeof(Mapper026));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper026_Reset;
	mapper->vtbl.Write = Mapper026_Write;
	mapper->vtbl.Clock = Mapper026_Clock;
	mapper->vtbl.SaveState = Mapper026_SaveState;
	mapper->vtbl.LoadState = Mapper026_LoadState;
	mapper->vtbl.IsStateSave = Mapper026_IsStateSave;

	return (Mapper *)mapper;
}

