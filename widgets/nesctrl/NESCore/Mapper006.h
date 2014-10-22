//////////////////////////////////////////////////////////////////////////
// Mapper006  FFE F4xxx                                                 //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;

	INT	irq_counter;
	BYTE	irq_enable;
}Mapper006;

void	Mapper006_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 14, 15 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	} else {
		SetCRAM_8K_Bank( 0 );
	}

	((Mapper006 *)mapper)->irq_enable = 0;
	((Mapper006 *)mapper)->irq_counter = 0;
}

void	Mapper006_WriteLow( Mapper *mapper, WORD addr, BYTE data )
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
			((Mapper006 *)mapper)->irq_enable = 0;

			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0x4502:
			((Mapper006 *)mapper)->irq_counter = (((Mapper006 *)mapper)->irq_counter&0xFF00)|data;
			break;
		case	0x4503:
			((Mapper006 *)mapper)->irq_counter = (((Mapper006 *)mapper)->irq_counter&0x00FF)|((INT)data<<8);
			((Mapper006 *)mapper)->irq_enable = 0xFF;

			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		default:
			Mapper_DefWriteLow( mapper, addr, data );
			break;
	}
}

void	Mapper006_Write( Mapper *mapper, WORD addr, BYTE data )
{
	SetPROM_16K_Bank( 4, (data&0x3C)>>2 );
	SetCRAM_8K_Bank( data&0x03 );
}

void	Mapper006_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper006 *)mapper)->irq_enable ) {
		((Mapper006 *)mapper)->irq_counter+=133;
		if( ((Mapper006 *)mapper)->irq_counter >= 0xFFFF ) {
//			((Mapper006 *)mapper)->nes->cpu->IRQ();
			((Mapper006 *)mapper)->irq_counter = 0;

			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
		}
	}
}

void	Mapper006_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper006 *)mapper)->irq_enable;
	*(INT*)&p[1] = ((Mapper006 *)mapper)->irq_counter;
}

void	Mapper006_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper006 *)mapper)->irq_enable = p[0];
	((Mapper006 *)mapper)->irq_counter = *(INT*)&p[1];
}

BOOL	Mapper006_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper006_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper006));
	
	memset(mapper, 0, sizeof(Mapper006));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper006_Reset;
	mapper->vtbl.WriteLow = Mapper006_WriteLow;
	mapper->vtbl.Write = Mapper006_Write;
	mapper->vtbl.HSync = Mapper006_HSync;
	mapper->vtbl.SaveState = Mapper006_SaveState;
	mapper->vtbl.LoadState = Mapper006_LoadState;
	mapper->vtbl.IsStateSave = Mapper006_IsStateSave;

	return (Mapper *)mapper;
}

