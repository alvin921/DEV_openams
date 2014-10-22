
//////////////////////////////////////////////////////////////////////////
// Mapper065  Irem H3001                                                //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;

	BYTE	irq_enable;
	INT	irq_counter;
	INT	irq_latch;
}Mapper065;

void	Mapper065_Reset(Mapper *mapper)
{
	((Mapper065 *)mapper)->patch = 0;

	// Kaiketsu Yanchamaru 3(J)
	if( ROM_GetPROM_CRC(((Mapper065 *)mapper)->nes->rom) == 0xe30b7f64 ) {
		((Mapper065 *)mapper)->patch = 1;
	}

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	((Mapper065 *)mapper)->irq_enable = 0;
	((Mapper065 *)mapper)->irq_counter = 0;
}

void	Mapper065_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;

		case	0x9000:
			if( !((Mapper065 *)mapper)->patch ) {
				if( data & 0x40 ) SetVRAM_Mirror_cont( VRAM_VMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_HMIRROR );
			}
			break;

		case	0x9001:
			if( ((Mapper065 *)mapper)->patch ) {
				if( data & 0x80 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;

		case	0x9003:
			if( !((Mapper065 *)mapper)->patch ) {
				((Mapper065 *)mapper)->irq_enable = data & 0x80;
				CPU_ClrIRQ( ((Mapper065 *)mapper)->nes->cpu, IRQ_MAPPER );
			}
			break;
		case	0x9004:
			if( !((Mapper065 *)mapper)->patch ) {
				((Mapper065 *)mapper)->irq_counter = ((Mapper065 *)mapper)->irq_latch;
			}
			break;
		case	0x9005:
			if( ((Mapper065 *)mapper)->patch ) {
				((Mapper065 *)mapper)->irq_counter = (BYTE)(data<<1);
				((Mapper065 *)mapper)->irq_enable = data;
				CPU_ClrIRQ( ((Mapper065 *)mapper)->nes->cpu, IRQ_MAPPER );
			} else {
				((Mapper065 *)mapper)->irq_latch = (((Mapper065 *)mapper)->irq_latch & 0x00FF)|((INT)data<<8);
			}
			break;

		case	0x9006:
			if( ((Mapper065 *)mapper)->patch ) {
				((Mapper065 *)mapper)->irq_enable = 1;
			} else {
				((Mapper065 *)mapper)->irq_latch = (((Mapper065 *)mapper)->irq_latch & 0xFF00)|data;
			}
			break;

		case	0xB000:
		case	0xB001:
		case	0xB002:
		case	0xB003:
		case	0xB004:
		case	0xB005:
		case	0xB006:
		case	0xB007:
			SetVROM_1K_Bank( addr & 0x0007, data );
			break;

		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0xC000:
			SetPROM_8K_Bank( 6, data );
			break;
	}
}

void	Mapper065_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper065 *)mapper)->patch ) {
		if( ((Mapper065 *)mapper)->irq_enable ) {
			if( ((Mapper065 *)mapper)->irq_counter == 0 ) {
//				CPU_IRQ_NotPending(((Mapper065 *)mapper)->nes->cpu);
				CPU_SetIRQ( ((Mapper065 *)mapper)->nes->cpu, IRQ_MAPPER );
			} else {
				((Mapper065 *)mapper)->irq_counter--;
			}
		}
	}
}

void	Mapper065_Clock( Mapper *mapper, INT cycles )
{
	if( !((Mapper065 *)mapper)->patch ) {
		if( ((Mapper065 *)mapper)->irq_enable ) {
			if( ((Mapper065 *)mapper)->irq_counter <= 0 ) {
//				CPU_IRQ_NotPending(((Mapper065 *)mapper)->nes->cpu);
				CPU_SetIRQ( ((Mapper065 *)mapper)->nes->cpu, IRQ_MAPPER );
			} else {
				((Mapper065 *)mapper)->irq_counter -= cycles;
			}
		}
	}
}

void	Mapper065_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper065 *)mapper)->irq_enable;
	*(INT*)&p[1] = ((Mapper065 *)mapper)->irq_counter;
	*(INT*)&p[5] = ((Mapper065 *)mapper)->irq_latch;
}

void	Mapper065_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper065 *)mapper)->irq_enable  = p[0];
	((Mapper065 *)mapper)->irq_counter = *(INT*)&p[1];
	((Mapper065 *)mapper)->irq_latch   = *(INT*)&p[5];
}

BOOL	Mapper065_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper065_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper065));
	
	memset(mapper, 0, sizeof(Mapper065));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper065_Reset;
	mapper->vtbl.Write = Mapper065_Write;
	mapper->vtbl.HSync = Mapper065_HSync;
	mapper->vtbl.Clock = Mapper065_Clock;
	mapper->vtbl.SaveState = Mapper065_SaveState;
	mapper->vtbl.LoadState = Mapper065_LoadState;
	mapper->vtbl.IsStateSave = Mapper065_IsStateSave;

	return (Mapper *)mapper;
}

