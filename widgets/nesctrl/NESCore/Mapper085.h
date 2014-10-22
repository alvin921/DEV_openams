//////////////////////////////////////////////////////////////////////////
// Mapper085  Konami VRC7                                               //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	INT	irq_clock;
}Mapper085;


void	Mapper085_Reset(Mapper *mapper)
{
	((Mapper085 *)mapper)->irq_enable = 0;
	((Mapper085 *)mapper)->irq_counter = 0;
	((Mapper085 *)mapper)->irq_latch = 0;
	((Mapper085 *)mapper)->irq_clock = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	} else {
		SetCRAM_8K_Bank( 0 );
	}

#if	0
//	DWORD	crc = ((Mapper085 *)mapper)->nes->rom->GetPROM_CRC();
//	if( crc == 0x1aa0479c ) {	// For Tiny Toon Adventures 2 - Montana Land he Youkoso(J)
//		((Mapper085 *)mapper)->nes->SetRenderMethod( PRE_RENDER );
//	}
//	if( crc == 0x33ce3ff0 ) {	// For Lagrange Point(J)
//		((Mapper085 *)mapper)->nes->SetRenderMethod( TILE_RENDER );
//	}
#endif
	APU_SelectExSound( ((Mapper085 *)mapper)->nes->apu, 2 );
}

void	Mapper085_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF038 ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x8008:
		case	0x8010:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0x9000:
			SetPROM_8K_Bank( 6, data );
			break;

		case	0x9010:
		case	0x9030:
			APU_ExWrite( ((Mapper085 *)mapper)->nes->apu, addr, data );
			break;

		case	0xA000:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 0, data );
			} else {
				SetCRAM_1K_Bank( 0, data );
			}
			break;

		case	0xA008:
		case	0xA010:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 1, data );
			} else {
				SetCRAM_1K_Bank( 1, data );
			}
			break;

		case	0xB000:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 2, data );
			} else {
				SetCRAM_1K_Bank( 2, data );
			}
			break;

		case	0xB008:
		case	0xB010:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 3, data );
			} else {
				SetCRAM_1K_Bank( 3, data );
			}
			break;

		case	0xC000:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 4, data );
			} else {
				SetCRAM_1K_Bank( 4, data );
			}
			break;

		case	0xC008:
		case	0xC010:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 5, data );
			} else {
				SetCRAM_1K_Bank( 5, data );
			}
			break;

		case	0xD000:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 6, data );
			} else {
				SetCRAM_1K_Bank( 6, data );
			}
			break;

		case	0xD008:
		case	0xD010:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 7, data );
			} else {
				SetCRAM_1K_Bank( 7, data );
			}
			break;

		case	0xE000:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			break;

		case	0xE008:
		case	0xE010:
			((Mapper085 *)mapper)->irq_latch = data;
			break;

		case	0xF000:
			((Mapper085 *)mapper)->irq_enable = data & 0x03;
			((Mapper085 *)mapper)->irq_counter = ((Mapper085 *)mapper)->irq_latch;
			((Mapper085 *)mapper)->irq_clock = 0;
			CPU_ClrIRQ( ((Mapper085 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;

		case	0xF008:
		case	0xF010:
			((Mapper085 *)mapper)->irq_enable = (((Mapper085 *)mapper)->irq_enable & 0x01) * 3;
			CPU_ClrIRQ( ((Mapper085 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper085_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper085 *)mapper)->irq_enable & 0x02 ) {
		((Mapper085 *)mapper)->irq_clock += cycles*4;
		while( ((Mapper085 *)mapper)->irq_clock >= 455 ) {
			((Mapper085 *)mapper)->irq_clock -= 455;
			((Mapper085 *)mapper)->irq_counter++;
			if( ((Mapper085 *)mapper)->irq_counter == 0 ) {
				((Mapper085 *)mapper)->irq_counter = ((Mapper085 *)mapper)->irq_latch;
				CPU_SetIRQ( ((Mapper085 *)mapper)->nes->cpu, IRQ_MAPPER );
			}
		}
	}
}

void	Mapper085_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper085 *)mapper)->irq_enable;
	p[1] = ((Mapper085 *)mapper)->irq_counter;
	p[2] = ((Mapper085 *)mapper)->irq_latch;
	*((INT*)&p[4]) = ((Mapper085 *)mapper)->irq_clock;
}

void	Mapper085_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper085 *)mapper)->irq_enable  = p[0];
	((Mapper085 *)mapper)->irq_counter = p[1];
	((Mapper085 *)mapper)->irq_latch   = p[2];
	((Mapper085 *)mapper)->irq_clock   = *((INT*)&p[4]);
}

BOOL	Mapper085_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper085_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper085));
	
	memset(mapper, 0, sizeof(Mapper085));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper085_Reset;
	mapper->vtbl.Write = Mapper085_Write;
	mapper->vtbl.Clock = Mapper085_Clock;
	mapper->vtbl.SaveState = Mapper085_SaveState;
	mapper->vtbl.LoadState = Mapper085_LoadState;
	mapper->vtbl.IsStateSave = Mapper085_IsStateSave;

	return (Mapper *)mapper;
}


