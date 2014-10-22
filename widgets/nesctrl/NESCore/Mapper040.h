
//////////////////////////////////////////////////////////////////////////
// Mapper040  SMB2J                                                     //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	INT	irq_line;
}Mapper040;

void	Mapper040_Reset(Mapper *mapper)
{
	((Mapper040 *)mapper)->irq_enable = 0;
	((Mapper040 *)mapper)->irq_line = 0;

	SetPROM_8K_Bank( 3, 6 );
	SetPROM_32K_Bank( 4, 5, 0, 7 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper040_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE000 ) {
		case	0x8000:
			((Mapper040 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( ((Mapper040 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xA000:
			((Mapper040 *)mapper)->irq_enable = 0xFF;
			((Mapper040 *)mapper)->irq_line = 37;
			CPU_ClrIRQ( ((Mapper040 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xC000:
			break;
		case	0xE000:
			SetPROM_8K_Bank( 6, data&0x07 );
			break;
	}
}

void	Mapper040_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper040 *)mapper)->irq_enable ) {
		if( --((Mapper040 *)mapper)->irq_line <= 0 ) {
//			((Mapper040 *)mapper)->nes->cpu->IRQ();
			CPU_SetIRQ( ((Mapper040 *)mapper)->nes->cpu, IRQ_MAPPER );
		}
	}
}

void	Mapper040_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper040 *)mapper)->irq_enable;
	*(INT*)&p[1] = ((Mapper040 *)mapper)->irq_line;
}

void	Mapper040_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper040 *)mapper)->irq_enable  = p[0];
	((Mapper040 *)mapper)->irq_line = *(INT*)&p[1];
}

BOOL	Mapper040_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper040_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper040));
	
	memset(mapper, 0, sizeof(Mapper040));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper040_Reset;
	mapper->vtbl.Write = Mapper040_Write;
	mapper->vtbl.HSync = Mapper040_HSync;
	mapper->vtbl.SaveState = Mapper040_SaveState;
	mapper->vtbl.LoadState = Mapper040_LoadState;
	mapper->vtbl.IsStateSave = Mapper040_IsStateSave;

	return (Mapper *)mapper;
}

