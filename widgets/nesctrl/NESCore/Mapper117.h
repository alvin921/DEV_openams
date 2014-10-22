//////////////////////////////////////////////////////////////////////////
// Mapper117 San Ko Gu (TW) :                                           //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	BYTE	irq_counter;
}Mapper117;

void    Mapper117_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper117_Write( Mapper *mapper,WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x8001:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0x8002:
			SetPROM_8K_Bank( 6, data );
			break;
		case	0xA000:
			SetVROM_1K_Bank( 0, data );
			break;
		case	0xA001:
			SetVROM_1K_Bank( 1, data );
			break;
		case	0xA002:
			SetVROM_1K_Bank( 2, data );
			break;
		case	0xA003:
			SetVROM_1K_Bank( 3, data );
			break;
		case	0xA004:
			SetVROM_1K_Bank( 4, data );
			break;
		case	0xA005:
			SetVROM_1K_Bank( 5, data );
			break;
		case	0xA006:
			SetVROM_1K_Bank( 6, data );
			break;
		case	0xA007:
			SetVROM_1K_Bank( 7, data );
			break;
		case	0xC001:
		case	0xC002:
		case	0xC003:
			((Mapper117 *)mapper)->irq_counter = data;
			break;
		case	0xE000:
			((Mapper117 *)mapper)->irq_enable = data & 1;
//			((Mapper117 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}
void	Mapper117_HSync( Mapper *mapper,INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(((Mapper117 *)mapper)->nes->ppu) ) {
			if( ((Mapper117 *)mapper)->irq_enable ) {
				if(((Mapper117 *)mapper)->irq_counter==scanline) {
					((Mapper117 *)mapper)->irq_counter = 0;
//					CPU_IRQ(((Mapper117 *)mapper)->nes->cpu);
//					CPU_SetIRQ( ((Mapper117 *)mapper)->nes->cpu, IRQ_MAPPER );
					CPU_SetIRQ( ((Mapper117 *)mapper)->nes->cpu, IRQ_TRIGGER );
				}
			}
		}
	}
}

void	Mapper117_SaveState( Mapper *mapper,LPBYTE p )
{
	p[0] = ((Mapper117 *)mapper)->irq_counter;
	p[1] = ((Mapper117 *)mapper)->irq_enable;
}

void	Mapper117_LoadState( Mapper *mapper,LPBYTE p )
{
	((Mapper117 *)mapper)->irq_counter=p[0];
	((Mapper117 *)mapper)->irq_enable=p[1];
}

BOOL	Mapper117_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper117_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper117));
	
	memset(mapper, 0, sizeof(Mapper117));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper117_Reset;
	mapper->vtbl.Write = Mapper117_Write;
	mapper->vtbl.HSync = Mapper117_HSync;
	mapper->vtbl.SaveState = Mapper117_SaveState;
	mapper->vtbl.LoadState = Mapper117_LoadState;
	mapper->vtbl.IsStateSave = Mapper117_IsStateSave;

	return (Mapper *)mapper;
}


