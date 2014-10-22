
//////////////////////////////////////////////////////////////////////////
// Mapper182  PC-SuperDonkeyKong                                        //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg;
	BYTE	irq_enable;
	BYTE	irq_counter;
}Mapper182;

void	Mapper182_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );

	((Mapper182 *)mapper)->reg = 0;
	((Mapper182 *)mapper)->irq_enable = 0;
	((Mapper182 *)mapper)->irq_counter = 0;
}

void	Mapper182_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF003 ) {
		case	0x8001:
			if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;
		case	0xA000:
			((Mapper182 *)mapper)->reg = data & 0x07;
			break;
		case	0xC000:
			switch( ((Mapper182 *)mapper)->reg ) {
				case	0:
					SetVROM_1K_Bank( 0, (data&0xFE)+0 );
					SetVROM_1K_Bank( 1, (data&0xFE)+1 );
					break;
				case	1:
					SetVROM_1K_Bank( 5, data );
					break;
				case	2:
					SetVROM_1K_Bank( 2, (data&0xFE)+0 );
					SetVROM_1K_Bank( 3, (data&0xFE)+1 );
					break;
				case	3:
					SetVROM_1K_Bank( 7, data );
					break;
				case	4:
					SetPROM_8K_Bank( 4, data );
					break;
				case	5:
					SetPROM_8K_Bank( 5, data );
					break;
				case	6:
					SetVROM_1K_Bank( 4, data );
					break;
				case	7:
					SetVROM_1K_Bank( 6, data );
					break;
			}
			break;
		case	0xE003:
			((Mapper182 *)mapper)->irq_enable  = data;
			((Mapper182 *)mapper)->irq_counter = data;
			CPU_ClrIRQ( ((Mapper182 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper182_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper182 *)mapper)->irq_enable ) {
		if( (scanline >= 0 && scanline <= SCREEN_HEIGHT-1) && PPU_IsDispON(((Mapper182 *)mapper)->nes->ppu) ) {
			if( !(--((Mapper182 *)mapper)->irq_counter) ) {
				((Mapper182 *)mapper)->irq_enable  = 0;
				((Mapper182 *)mapper)->irq_counter = 0;
//				CPU_IRQ_NotPending(((Mapper182 *)mapper)->nes->cpu);
				CPU_SetIRQ( ((Mapper182 *)mapper)->nes->cpu, IRQ_MAPPER );
			}
		}
	}
}

void	Mapper182_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper182 *)mapper)->reg;
	p[1] = ((Mapper182 *)mapper)->irq_enable;
	p[2] = ((Mapper182 *)mapper)->irq_counter;
}

void	Mapper182_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper182 *)mapper)->reg         = p[0];
	((Mapper182 *)mapper)->irq_enable  = p[1];
	((Mapper182 *)mapper)->irq_counter = p[2];
}

BOOL	Mapper182_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper182_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper182));
	
	memset(mapper, 0, sizeof(Mapper182));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper182_Reset;
	mapper->vtbl.Write = Mapper182_Write;
	mapper->vtbl.HSync = Mapper182_HSync;
	mapper->vtbl.SaveState = Mapper182_SaveState;
	mapper->vtbl.LoadState = Mapper182_LoadState;
	mapper->vtbl.IsStateSave = Mapper182_IsStateSave;

	return (Mapper *)mapper;
}

