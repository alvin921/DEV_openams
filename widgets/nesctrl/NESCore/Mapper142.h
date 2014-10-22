
//////////////////////////////////////////////////////////////////////////
// Mapper142  SMB2j Pirate (KS 202)                                     //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	prg_sel;
	BYTE	irq_enable;
	INT	irq_counter;
}Mapper142;

void	Mapper142_Reset(Mapper *mapper)
{
	((Mapper142 *)mapper)->prg_sel = 0;
	((Mapper142 *)mapper)->irq_enable = 0;
	((Mapper142 *)mapper)->irq_counter = 0;

	SetPROM_8K_Bank( 3, 0 );
	SetPROM_8K_Bank( 7, 0x0F );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper142_Write( Mapper *mapper,WORD addr, BYTE data )
{
	switch( addr & 0xF000 ) {
		case	0x8000:
			((Mapper142 *)mapper)->irq_counter = (((Mapper142 *)mapper)->irq_counter & 0xFFF0) | ((data & 0x0F)<< 0);
			break;
		case	0x9000:
			((Mapper142 *)mapper)->irq_counter = (((Mapper142 *)mapper)->irq_counter & 0xFF0F) | ((data & 0x0F)<< 4);
			break;
		case	0xA000:
			((Mapper142 *)mapper)->irq_counter = (((Mapper142 *)mapper)->irq_counter & 0xF0FF) | ((data & 0x0F)<< 8);
			break;
		case	0xB000:
			((Mapper142 *)mapper)->irq_counter = (((Mapper142 *)mapper)->irq_counter & 0x0FFF) | ((data & 0x0F)<<12);
			break;
		case	0xC000:
			((Mapper142 *)mapper)->irq_enable = data & 0x0F;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE000:
			((Mapper142 *)mapper)->prg_sel = data & 0x0F;
			break;
		case	0xF000:
			switch( ((Mapper142 *)mapper)->prg_sel ) {
				case	1: SetPROM_8K_Bank( 4, data & 0x0F ); break;
				case	2: SetPROM_8K_Bank( 5, data & 0x0F ); break;
				case	3: SetPROM_8K_Bank( 6, data & 0x0F ); break;
				case	4: SetPROM_8K_Bank( 3, data & 0x0F ); break;
			}
			break;
	}
}

void	Mapper142_HSync( Mapper *mapper,INT scanline )
{
	if( ((Mapper142 *)mapper)->irq_enable ) {
		if( ((Mapper142 *)mapper)->irq_counter > (0xFFFF - 113) ) {
			((Mapper142 *)mapper)->irq_counter = 0;
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
		} else {
			((Mapper142 *)mapper)->irq_counter += 113;
		}
	}
}

void	Mapper142_SaveState( Mapper *mapper,LPBYTE p )
{
	p[0] = ((Mapper142 *)mapper)->prg_sel;
	p[0] = ((Mapper142 *)mapper)->irq_enable;
	*(INT*)&p[2] = ((Mapper142 *)mapper)->irq_counter;
}

void	Mapper142_LoadState( Mapper *mapper,LPBYTE p )
{
	((Mapper142 *)mapper)->prg_sel = p[0];
	((Mapper142 *)mapper)->irq_enable  = p[1];
	((Mapper142 *)mapper)->irq_counter = *(INT*)&p[2];
}

BOOL	Mapper142_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper142_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper142));
	
	memset(mapper, 0, sizeof(Mapper142));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper142_Reset;
	mapper->vtbl.Write = Mapper142_Write;
	mapper->vtbl.HSync = Mapper142_HSync;
	mapper->vtbl.SaveState = Mapper142_SaveState;
	mapper->vtbl.LoadState = Mapper142_LoadState;
	mapper->vtbl.IsStateSave = Mapper142_IsStateSave;

	return (Mapper *)mapper;
}

