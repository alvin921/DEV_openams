//////////////////////////////////////////////////////////////////////////
// Mapper091  PC-HK-SF3                                                 //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	BYTE	irq_counter;
}Mapper091;

void	Mapper091_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( PROM_8K_SIZE-2, PROM_8K_SIZE-1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank( 0, 0, 0, 0, 0, 0, 0, 0 );
	}

	((Mapper091 *)mapper)->irq_enable = 0;
	((Mapper091 *)mapper)->irq_counter = 0;

	NES_SetRenderMethod( mapper->nes, POST_ALL_RENDER );
}

void	Mapper091_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "$%04X:$%02X(%3d) L=%3d\n", addr, data, data, ((Mapper091 *)mapper)->nes->GetScanline() );
	switch( addr & 0xF003 ) {
		case	0x6000:
		case	0x6001:
		case	0x6002:
		case	0x6003:
			SetVROM_2K_Bank( (addr&0x03)*2, data );
			break;

		case	0x7000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x7001:
			SetPROM_8K_Bank( 5, data );
			break;

		case	0x7002:
			((Mapper091 *)mapper)->irq_enable = 0;
			((Mapper091 *)mapper)->irq_counter = 0;
			CPU_ClrIRQ( ((Mapper091 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0x7003:
			((Mapper091 *)mapper)->irq_enable = 1;
			break;
	}
}

void	Mapper091_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) && PPU_IsDispON(((Mapper091 *)mapper)->nes->ppu) ) {
		if( ((Mapper091 *)mapper)->irq_enable ) {
			((Mapper091 *)mapper)->irq_counter++;
		}
		if( ((Mapper091 *)mapper)->irq_counter >= 8 ) {
//			CPU_IRQ_NotPending(((Mapper091 *)mapper)->nes->cpu);
			CPU_SetIRQ( ((Mapper091 *)mapper)->nes->cpu, IRQ_MAPPER );
		}
	}
}

void	Mapper091_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper091 *)mapper)->irq_enable;
	p[1] = ((Mapper091 *)mapper)->irq_counter;
}

void	Mapper091_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper091 *)mapper)->irq_enable  = p[0];
	((Mapper091 *)mapper)->irq_counter = p[1];
}

BOOL	Mapper091_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper091_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper091));
	
	memset(mapper, 0, sizeof(Mapper091));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper091_Reset;
	mapper->vtbl.WriteLow = Mapper091_WriteLow;
	mapper->vtbl.HSync = Mapper091_HSync;
	mapper->vtbl.SaveState = Mapper091_SaveState;
	mapper->vtbl.LoadState = Mapper091_LoadState;
	mapper->vtbl.IsStateSave = Mapper091_IsStateSave;

	return (Mapper *)mapper;
}



