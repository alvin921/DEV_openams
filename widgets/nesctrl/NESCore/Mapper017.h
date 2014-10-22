//////////////////////////////////////////////////////////////////////////
// Mapper017  FFE F8xxx                                                 //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	INT	irq_counter;
	INT	irq_latch;
	BYTE	irq_enable;
}Mapper017;

void	Mapper017_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	((Mapper017 *)mapper)->irq_enable = 0;
	((Mapper017 *)mapper)->irq_counter = 0;
	((Mapper017 *)mapper)->irq_latch = 0;
}

void	Mapper017_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x42FE:
			if( data&0x10 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			else		SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			break;
		case	0x42FF:
			if( data&0x10 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;

		case	0x4501:
			((Mapper017 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0x4502:
			((Mapper017 *)mapper)->irq_latch = (((Mapper017 *)mapper)->irq_latch&0xFF00)|data;
			break;
		case	0x4503:
			((Mapper017 *)mapper)->irq_latch = (((Mapper017 *)mapper)->irq_latch&0x00FF)|((INT)data<<8);
			((Mapper017 *)mapper)->irq_counter = ((Mapper017 *)mapper)->irq_latch;
			((Mapper017 *)mapper)->irq_enable = 0xFF;
			break;

		case	0x4504:
		case	0x4505:
		case	0x4506:
		case	0x4507:
			SetPROM_8K_Bank( addr&0x07, data );
			break;

		case	0x4510:
		case	0x4511:
		case	0x4512:
		case	0x4513:
		case	0x4514:
		case	0x4515:
		case	0x4516:
		case	0x4517:
			SetVROM_1K_Bank( addr&0x07, data );
			break;

		default:
			Mapper_DefWriteLow( mapper, addr, data );
			break;
	}
}

void	Mapper017_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper017 *)mapper)->irq_enable ) {
		if( ((Mapper017 *)mapper)->irq_counter >= 0xFFFF-113 ) {
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
//			((Mapper017 *)mapper)->nes->cpu->IRQ();
//			((Mapper017 *)mapper)->irq_counter = 0;
//			((Mapper017 *)mapper)->irq_enable = 0;
			((Mapper017 *)mapper)->irq_counter &= 0xFFFF;
		} else {
			((Mapper017 *)mapper)->irq_counter += 113;
		}
	}
}

void	Mapper017_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper017 *)mapper)->irq_enable;
	*(INT*)&p[1] = ((Mapper017 *)mapper)->irq_counter;
	*(INT*)&p[5] = ((Mapper017 *)mapper)->irq_latch;
}

void	Mapper017_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper017 *)mapper)->irq_enable = p[0];
	((Mapper017 *)mapper)->irq_counter = *(INT*)&p[1];
	((Mapper017 *)mapper)->irq_latch   = *(INT*)&p[5];
}

BOOL	Mapper017_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper017_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper017));
	
	memset(mapper, 0, sizeof(Mapper017));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper017_Reset;
	mapper->vtbl.WriteLow = Mapper017_WriteLow;
	mapper->vtbl.HSync = Mapper017_HSync;
	mapper->vtbl.SaveState = Mapper017_SaveState;
	mapper->vtbl.LoadState = Mapper017_LoadState;
	mapper->vtbl.IsStateSave = Mapper017_IsStateSave;

	return (Mapper *)mapper;
}

