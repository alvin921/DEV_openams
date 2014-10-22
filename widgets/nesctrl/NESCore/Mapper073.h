
//////////////////////////////////////////////////////////////////////////
// Mapper073  Konami VRC3                                               //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	INT	irq_counter;
}Mapper073;

void	Mapper073_Reset(Mapper *mapper)
{
	((Mapper073 *)mapper)->irq_enable = 0;
	((Mapper073 *)mapper)->irq_counter = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper073_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0xF000:
			SetPROM_16K_Bank( 4, data );
			break;

		case	0x8000:
			((Mapper073 *)mapper)->irq_counter = (((Mapper073 *)mapper)->irq_counter & 0xFFF0) | (data & 0x0F);
			break;
		case	0x9000:
			((Mapper073 *)mapper)->irq_counter = (((Mapper073 *)mapper)->irq_counter & 0xFF0F) | ((data & 0x0F) << 4);
			break;
		case	0xA000:
			((Mapper073 *)mapper)->irq_counter = (((Mapper073 *)mapper)->irq_counter & 0xF0FF) | ((data & 0x0F) << 8);
			break;
		case	0xB000:
			((Mapper073 *)mapper)->irq_counter = (((Mapper073 *)mapper)->irq_counter & 0x0FFF) | ((data & 0x0F) << 12);
			break;
		case	0xC000:
			((Mapper073 *)mapper)->irq_enable = data & 0x02;
			CPU_ClrIRQ( ((Mapper073 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xD000:
			CPU_ClrIRQ( ((Mapper073 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper073_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper073 *)mapper)->irq_enable ) {
		if( (((Mapper073 *)mapper)->irq_counter+=cycles) >= 0xFFFF ) {
			((Mapper073 *)mapper)->irq_enable = 0;
			((Mapper073 *)mapper)->irq_counter &= 0xFFFF;
			CPU_SetIRQ( ((Mapper073 *)mapper)->nes->cpu, IRQ_MAPPER );
		}
	}
}

void	Mapper073_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper073 *)mapper)->irq_enable;
	*(INT*)&p[1] = ((Mapper073 *)mapper)->irq_counter;
}

void	Mapper073_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper073 *)mapper)->irq_enable  = p[0];
	((Mapper073 *)mapper)->irq_counter = *(INT*)&p[1];
}

BOOL	Mapper073_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper073_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper073));
	
	memset(mapper, 0, sizeof(Mapper073));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper073_Reset;
	mapper->vtbl.Write = Mapper073_Write;
	mapper->vtbl.Clock = Mapper073_Clock;
	mapper->vtbl.SaveState = Mapper073_SaveState;
	mapper->vtbl.LoadState = Mapper073_LoadState;
	mapper->vtbl.IsStateSave = Mapper073_IsStateSave;

	return (Mapper *)mapper;
}

