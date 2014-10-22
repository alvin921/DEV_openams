//////////////////////////////////////////////////////////////////////////
// Mapper024  Konami VRC6(Normal)                                       //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	INT	irq_clock;
}Mapper024;

void	Mapper024_Reset(Mapper *mapper)
{
	((Mapper024 *)mapper)->irq_enable = 0;
	((Mapper024 *)mapper)->irq_counter = 0;
	((Mapper024 *)mapper)->irq_latch = 0;
	((Mapper024 *)mapper)->irq_clock = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	NES_SetRenderMethod( mapper->nes, POST_RENDER );
//	NES_SetRenderMethod( mapper->nes, PRE_RENDER );

	APU_SelectExSound( mapper->nes->apu, 1 );
}

void	Mapper024_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF003 ) {
		case 0x8000:
			SetPROM_16K_Bank( 4, data );
			break;

		case 0x9000: case 0x9001: case 0x9002:
		case 0xA000: case 0xA001: case 0xA002:
		case 0xB000: case 0xB001: case 0xB002:
			APU_ExWrite( mapper->nes->apu, addr, data );
			break;

		case 0xB003:
			data = data & 0x0C;
			if( data == 0x00 )	SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 0x04 )	SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 0x08 )	SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			else if( data == 0x0C )	SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			break;

		case 0xC000:
			SetPROM_8K_Bank( 6, data );
			break;

		case 0xD000:
			SetVROM_1K_Bank( 0, data );
			break;

		case 0xD001:
			SetVROM_1K_Bank( 1, data );
			break;

		case 0xD002:
			SetVROM_1K_Bank( 2, data );
			break;

		case 0xD003:
			SetVROM_1K_Bank( 3, data );
			break;

		case 0xE000:
			SetVROM_1K_Bank( 4, data );
			break;

		case 0xE001:
			SetVROM_1K_Bank( 5, data );
			break;

		case 0xE002:
			SetVROM_1K_Bank( 6, data );
			break;

		case 0xE003:
			SetVROM_1K_Bank( 7, data );
			break;

		case 0xF000:
			((Mapper024 *)mapper)->irq_latch = data;
			break;
		case 0xF001:
			((Mapper024 *)mapper)->irq_enable = data & 0x03;
			if( ((Mapper024 *)mapper)->irq_enable & 0x02 ) {
				((Mapper024 *)mapper)->irq_counter = ((Mapper024 *)mapper)->irq_latch;
				((Mapper024 *)mapper)->irq_clock = 0;
			}
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case 0xF002:
			((Mapper024 *)mapper)->irq_enable = (((Mapper024 *)mapper)->irq_enable & 0x01) * 3;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper024_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper024 *)mapper)->irq_enable & 0x02 ) {
		if( (((Mapper024 *)mapper)->irq_clock+=cycles) >= 0x72 ) {
			((Mapper024 *)mapper)->irq_clock -= 0x72;
			if( ((Mapper024 *)mapper)->irq_counter == 0xFF ) {
				((Mapper024 *)mapper)->irq_counter = ((Mapper024 *)mapper)->irq_latch;
//				mapper->nes->cpu->IRQ_NotPending();
				CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
			} else {
				((Mapper024 *)mapper)->irq_counter++;
			}
		}
	}
}

void	Mapper024_SaveState( Mapper *mapper,  LPBYTE p )
{
	p[0] = ((Mapper024 *)mapper)->irq_enable;
	p[1] = ((Mapper024 *)mapper)->irq_counter;
	p[2] = ((Mapper024 *)mapper)->irq_latch;
	*(INT*)&p[3] = ((Mapper024 *)mapper)->irq_clock;
}

void	Mapper024_LoadState( Mapper *mapper,  LPBYTE p )
{
	((Mapper024 *)mapper)->irq_enable  = p[0];
	((Mapper024 *)mapper)->irq_counter = p[1];
	((Mapper024 *)mapper)->irq_latch   = p[2];
	((Mapper024 *)mapper)->irq_clock   = *(INT*)&p[3];
}

BOOL	Mapper024_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper024_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper024));
	
	memset(mapper, 0, sizeof(Mapper024));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper024_Reset;
	mapper->vtbl.Write = Mapper024_Write;
	mapper->vtbl.Clock = Mapper024_Clock;
	mapper->vtbl.SaveState = Mapper024_SaveState;
	mapper->vtbl.LoadState = Mapper024_LoadState;
	mapper->vtbl.IsStateSave = Mapper024_IsStateSave;

	return (Mapper *)mapper;
}


