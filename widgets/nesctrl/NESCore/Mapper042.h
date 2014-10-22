
//////////////////////////////////////////////////////////////////////////
// Mapper042  Mario Baby                                                //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	BYTE	irq_counter;
}Mapper042;


void	Mapper042_Reset(Mapper *mapper)
{
	((Mapper042 *)mapper)->irq_enable = 0;
	((Mapper042 *)mapper)->irq_counter = 0;

	SetPROM_8K_Bank( 3, 0 );
	SetPROM_32K_Bank( PROM_8K_SIZE-4, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper042_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE003 ) {
		case	0xE000:
			SetPROM_8K_Bank( 3, data&0x0F );
			break;

		case	0xE001:
			if( data&0x08 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;

		case	0xE002:
			if( data&0x02 ) {
				((Mapper042 *)mapper)->irq_enable = 0xFF;
			} else {
				((Mapper042 *)mapper)->irq_enable = 0;
				((Mapper042 *)mapper)->irq_counter = 0;
			}
			CPU_ClrIRQ( ((Mapper042 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper042_HSync( Mapper *mapper, INT scanline )
{
	CPU_ClrIRQ( ((Mapper042 *)mapper)->nes->cpu, IRQ_MAPPER );
	if( ((Mapper042 *)mapper)->irq_enable ) {
		if( ((Mapper042 *)mapper)->irq_counter < 215 ) {
			((Mapper042 *)mapper)->irq_counter++;
		}
		if( ((Mapper042 *)mapper)->irq_counter == 215 ) {
			((Mapper042 *)mapper)->irq_enable = 0;
//			((Mapper042 *)mapper)->nes->cpu->IRQ();
			CPU_SetIRQ( ((Mapper042 *)mapper)->nes->cpu, IRQ_MAPPER );
		}
	}
}

void	Mapper042_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper042 *)mapper)->irq_enable;
	p[1] = ((Mapper042 *)mapper)->irq_counter;
}

void	Mapper042_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper042 *)mapper)->irq_enable  = p[0];
	((Mapper042 *)mapper)->irq_counter = p[1];
}
BOOL	Mapper042_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper042_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper042));
	
	memset(mapper, 0, sizeof(Mapper042));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper042_Reset;
	mapper->vtbl.Write = Mapper042_Write;
	mapper->vtbl.HSync = Mapper042_HSync;
	mapper->vtbl.SaveState = Mapper042_SaveState;
	mapper->vtbl.LoadState = Mapper042_LoadState;
	mapper->vtbl.IsStateSave = Mapper042_IsStateSave;

	return (Mapper *)mapper;
}

