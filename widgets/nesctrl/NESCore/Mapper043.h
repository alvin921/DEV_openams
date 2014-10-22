
//////////////////////////////////////////////////////////////////////////
// Mapper043  SMB2J                                                     //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	INT	irq_counter;
}Mapper043;

void	Mapper043_Reset(Mapper *mapper)
{
	((Mapper043*)mapper)->irq_enable = 0xFF;
	((Mapper043*)mapper)->irq_counter = 0;

	SetPROM_8K_Bank( 3, 2 );
	SetPROM_32K_Bank( 1, 0, 4, 9 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

BYTE	Mapper043_ReadLow( Mapper *mapper, WORD addr )
{
	if( 0x5000 <= addr && addr < 0x6000 ) {
		LPBYTE	pPtr = ROM_GetPROM(((Mapper043*)mapper)->nes->rom);
		return	pPtr[0x2000*8+0x1000+(addr-0x5000)];
	}

	return	(BYTE)(addr>>8);
}

void	Mapper043_ExWrite( Mapper *mapper, WORD addr, BYTE data )
{
	if( (addr&0xF0FF) == 0x4022 ) {
		switch( data&0x07 ) {
			case	0x00:
			case	0x02:
			case	0x03:
			case	0x04:
				SetPROM_8K_Bank( 6, 4 );
				break;
			case	0x01:
				SetPROM_8K_Bank( 6, 3 );
				break;
			case	0x05:
				SetPROM_8K_Bank( 6, 7 );
				break;
			case	0x06:
				SetPROM_8K_Bank( 6, 5 );
				break;
			case	0x07:
				SetPROM_8K_Bank( 6, 6 );
				break;
		}
	}
}

void	Mapper043_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( (addr&0xF0FF) == 0x4022 ) {
		Mapper043_ExWrite( mapper, addr, data );
	}
}

void	Mapper043_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x8122 ) {
		if( data&0x03 ) {
			((Mapper043*)mapper)->irq_enable = 1;
		} else {
			((Mapper043*)mapper)->irq_counter = 0;
			((Mapper043*)mapper)->irq_enable = 0;
		}
		CPU_ClrIRQ( ((Mapper043*)mapper)->nes->cpu, IRQ_MAPPER );
	}
}

void	Mapper043_HSync( Mapper *mapper, INT scanline )
{
	CPU_ClrIRQ( ((Mapper043*)mapper)->nes->cpu, IRQ_MAPPER );
	if( ((Mapper043*)mapper)->irq_enable ) {
		((Mapper043*)mapper)->irq_counter += 341;
		if( ((Mapper043*)mapper)->irq_counter >= 12288 ) {
			((Mapper043*)mapper)->irq_counter = 0;
//			CPU_IRQ(((Mapper043*)mapper)->nes->cpu, );
			CPU_SetIRQ( ((Mapper043*)mapper)->nes->cpu, IRQ_MAPPER );
		}
	}
}

void	Mapper043_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper043*)mapper)->irq_enable;
	*(INT*)&p[1] = ((Mapper043*)mapper)->irq_counter;
}

void	Mapper043_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper043*)mapper)->irq_enable  = p[0];
	((Mapper043*)mapper)->irq_counter = *(INT*)&p[1];
}

BOOL	Mapper043_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper043_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper043));
	
	memset(mapper, 0, sizeof(Mapper043));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper043_Reset;
	mapper->vtbl.ReadLow = Mapper043_ReadLow;
	mapper->vtbl.ExWrite = Mapper043_ExWrite;
	mapper->vtbl.WriteLow = Mapper043_WriteLow;
	mapper->vtbl.Write = Mapper043_Write;
	mapper->vtbl.HSync = Mapper043_HSync;
	mapper->vtbl.SaveState = Mapper043_SaveState;
	mapper->vtbl.LoadState = Mapper043_LoadState;
	mapper->vtbl.IsStateSave = Mapper043_IsStateSave;

	return (Mapper *)mapper;
}

