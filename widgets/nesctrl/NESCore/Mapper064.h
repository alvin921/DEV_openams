//////////////////////////////////////////////////////////////////////////
// Mapper064  Tengen Rambo-1                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[3];
	BYTE	irq_enable;
	BYTE	irq_mode;
	INT	irq_counter;
	INT	irq_counter2;
	BYTE	irq_latch;
	BYTE	irq_reset;
}Mapper064;


void	Mapper064_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	((Mapper064 *)mapper)->reg[0] = ((Mapper064 *)mapper)->reg[1] = ((Mapper064 *)mapper)->reg[2] = 0;

	((Mapper064 *)mapper)->irq_enable   = 0;
	((Mapper064 *)mapper)->irq_mode     = 0;
	((Mapper064 *)mapper)->irq_counter  = 0;
	((Mapper064 *)mapper)->irq_counter2 = 0;
	((Mapper064 *)mapper)->irq_latch    = 0;
	((Mapper064 *)mapper)->irq_reset    = 0;
}

void	Mapper064_Write( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "$%04X:$%02X\n", addr, data );
	switch( addr&0xF003 ) {
		case	0x8000:
			((Mapper064 *)mapper)->reg[0] = data&0x0F;
			((Mapper064 *)mapper)->reg[1] = data&0x40;
			((Mapper064 *)mapper)->reg[2] = data&0x80;
			break;

		case	0x8001:
			switch( ((Mapper064 *)mapper)->reg[0] ) {
				case	0x00:
					if( ((Mapper064 *)mapper)->reg[2] ) {
						SetVROM_1K_Bank( 4, data+0 );
						SetVROM_1K_Bank( 5, data+1 );
					} else {
						SetVROM_1K_Bank( 0, data+0 );
						SetVROM_1K_Bank( 1, data+1 );
					}
					break;
				case	0x01:
					if( ((Mapper064 *)mapper)->reg[2] ) {
						SetVROM_1K_Bank( 6, data+0 );
						SetVROM_1K_Bank( 7, data+1 );
					} else {
						SetVROM_1K_Bank( 2, data+0 );
						SetVROM_1K_Bank( 3, data+1 );
					}
					break;
				case	0x02:
					if( ((Mapper064 *)mapper)->reg[2] ) {
						SetVROM_1K_Bank( 0, data );
					} else {
						SetVROM_1K_Bank( 4, data );
					}
					break;
				case	0x03:
					if( ((Mapper064 *)mapper)->reg[2] ) {
						SetVROM_1K_Bank( 1, data );
					} else {
						SetVROM_1K_Bank( 5, data );
					}
					break;
				case	0x04:
					if( ((Mapper064 *)mapper)->reg[2] ) {
						SetVROM_1K_Bank( 2, data );
					} else {
						SetVROM_1K_Bank( 6, data );
					}
					break;
				case	0x05:
					if( ((Mapper064 *)mapper)->reg[2] ) {
						SetVROM_1K_Bank( 3, data );
					} else {
						SetVROM_1K_Bank( 7, data );
					}
					break;
				case	0x06:
					if( ((Mapper064 *)mapper)->reg[1] ) {
						SetPROM_8K_Bank( 5, data );
					} else {
						SetPROM_8K_Bank( 4, data );
					}
					break;
				case	0x07:
					if( ((Mapper064 *)mapper)->reg[1] ) {
						SetPROM_8K_Bank( 6, data );
					} else {
						SetPROM_8K_Bank( 5, data );
					}
					break;
				case	0x08:
					SetVROM_1K_Bank( 1, data );
					break;
				case	0x09:
					SetVROM_1K_Bank( 3, data );
					break;
				case	0x0F:
					if( ((Mapper064 *)mapper)->reg[1] ) {
						SetPROM_8K_Bank( 4, data );
					} else {
						SetPROM_8K_Bank( 6, data );
					}
					break;
			}
			break;

		case	0xA000:
			if( data&0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;

		case	0xC000:
			((Mapper064 *)mapper)->irq_latch = data;
			if( ((Mapper064 *)mapper)->irq_reset ) {
				((Mapper064 *)mapper)->irq_counter = ((Mapper064 *)mapper)->irq_latch;
			}
			break;
		case	0xC001:
			((Mapper064 *)mapper)->irq_reset = 0xFF;
			((Mapper064 *)mapper)->irq_counter = ((Mapper064 *)mapper)->irq_latch;
			((Mapper064 *)mapper)->irq_mode = data & 0x01;
			break;
		case	0xE000:
			((Mapper064 *)mapper)->irq_enable = 0;
			if( ((Mapper064 *)mapper)->irq_reset ) {
				((Mapper064 *)mapper)->irq_counter = ((Mapper064 *)mapper)->irq_latch;
			}
			CPU_ClrIRQ( ((Mapper064 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper064 *)mapper)->irq_enable = 0xFF;
			if( ((Mapper064 *)mapper)->irq_reset ) {
				((Mapper064 *)mapper)->irq_counter = ((Mapper064 *)mapper)->irq_latch;
			}
			break;
	}
}

void	Mapper064_Clock( Mapper *mapper, INT cycles )
{
	if( !((Mapper064 *)mapper)->irq_mode )
		return;

	((Mapper064 *)mapper)->irq_counter2 += cycles;
	while( ((Mapper064 *)mapper)->irq_counter2 >= 4 ) {
		((Mapper064 *)mapper)->irq_counter2 -= 4;
		if( ((Mapper064 *)mapper)->irq_counter >= 0 ) {
			((Mapper064 *)mapper)->irq_counter--;
			if( ((Mapper064 *)mapper)->irq_counter < 0 ) {
				if( ((Mapper064 *)mapper)->irq_enable ) {
					CPU_SetIRQ( ((Mapper064 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper064_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper064 *)mapper)->irq_mode )
		return;

	((Mapper064 *)mapper)->irq_reset = 0;

	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(((Mapper064 *)mapper)->nes->ppu) ) {
			if( ((Mapper064 *)mapper)->irq_counter >= 0 ) {
				((Mapper064 *)mapper)->irq_counter--;
				if( ((Mapper064 *)mapper)->irq_counter < 0 ) {
					if( ((Mapper064 *)mapper)->irq_enable ) {
						((Mapper064 *)mapper)->irq_reset = 1;
						CPU_SetIRQ( ((Mapper064 *)mapper)->nes->cpu, IRQ_MAPPER );
					}
				}
			}
		}
	}
}

void	Mapper064_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper064 *)mapper)->reg[0];
	p[1] = ((Mapper064 *)mapper)->reg[1];
	p[2] = ((Mapper064 *)mapper)->reg[2];
	p[3] = ((Mapper064 *)mapper)->irq_enable;
	p[4] = ((Mapper064 *)mapper)->irq_mode;
	p[5] = ((Mapper064 *)mapper)->irq_latch;
	p[6] = ((Mapper064 *)mapper)->irq_reset;
	*((INT*)&p[ 8]) = ((Mapper064 *)mapper)->irq_counter;
	*((INT*)&p[12]) = ((Mapper064 *)mapper)->irq_counter2;
}

void	Mapper064_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper064 *)mapper)->reg[0] = p[0];
	((Mapper064 *)mapper)->reg[1] = p[1];
	((Mapper064 *)mapper)->reg[2] = p[2];
	((Mapper064 *)mapper)->irq_enable   = p[3];
	((Mapper064 *)mapper)->irq_mode     = p[4];
	((Mapper064 *)mapper)->irq_latch    = p[5];
	((Mapper064 *)mapper)->irq_reset    = p[6];
	((Mapper064 *)mapper)->irq_counter  = *((INT*)&p[ 8]);
	((Mapper064 *)mapper)->irq_counter2 = *((INT*)&p[12]);
}

BOOL	Mapper064_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper064_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper064));
	
	memset(mapper, 0, sizeof(Mapper064));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper064_Reset;
	mapper->vtbl.Write = Mapper064_Write;
	mapper->vtbl.Clock = Mapper064_Clock;
	mapper->vtbl.HSync = Mapper064_HSync;
	mapper->vtbl.SaveState = Mapper064_SaveState;
	mapper->vtbl.LoadState = Mapper064_LoadState;
	mapper->vtbl.IsStateSave = Mapper064_IsStateSave;

	return (Mapper *)mapper;
}

