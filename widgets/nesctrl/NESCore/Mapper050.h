
//////////////////////////////////////////////////////////////////////////
// Mapper050  SMB2J                                                     //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
}Mapper050;

void	Mapper050_Reset(Mapper *mapper)
{
	((Mapper050 *)mapper)->irq_enable = 0;
	SetPROM_8K_Bank( 3, 15 );
	SetPROM_8K_Bank( 4, 8 );
	SetPROM_8K_Bank( 5, 9 );
	SetPROM_8K_Bank( 6, 0 );
	SetPROM_8K_Bank( 7, 11 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper050_ExWrite( Mapper *mapper, WORD addr, BYTE data )
{
	if( (addr & 0xE060) == 0x4020 ) {
		if( addr & 0x0100 ) {
			((Mapper050 *)mapper)->irq_enable = data & 0x01;
			CPU_ClrIRQ( ((Mapper050 *)mapper)->nes->cpu, IRQ_MAPPER );
		} else {
			SetPROM_8K_Bank( 6, (data&0x08)|((data&0x01)<<2)|((data&0x06)>>1) );
		}
	}
}

void	Mapper050_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( (addr & 0xE060) == 0x4020 ) {
		if( addr & 0x0100 ) {
			((Mapper050 *)mapper)->irq_enable = data & 0x01;
			CPU_ClrIRQ( ((Mapper050 *)mapper)->nes->cpu, IRQ_MAPPER );
		} else {
			SetPROM_8K_Bank( 6, (data&0x08)|((data&0x01)<<2)|((data&0x06)>>1) );
		}
	}
}

void	Mapper050_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper050 *)mapper)->irq_enable ) {
		if( scanline == 21 ) {
//			CPU_IRQ(((Mapper050 *)mapper)->nes->cpu);
			CPU_SetIRQ( ((Mapper050 *)mapper)->nes->cpu, IRQ_MAPPER );
		}
	}
}

void	Mapper050_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper050 *)mapper)->irq_enable;
}

void	Mapper050_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper050 *)mapper)->irq_enable = p[0];
}


BOOL	Mapper050_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper050_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper050));
	
	memset(mapper, 0, sizeof(Mapper050));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper050_Reset;
	mapper->vtbl.ExWrite = Mapper050_ExWrite;
	mapper->vtbl.WriteLow = Mapper050_WriteLow;
	mapper->vtbl.HSync = Mapper050_HSync;
	mapper->vtbl.SaveState = Mapper050_SaveState;
	mapper->vtbl.LoadState = Mapper050_LoadState;
	mapper->vtbl.IsStateSave = Mapper050_IsStateSave;

	return (Mapper *)mapper;
}

