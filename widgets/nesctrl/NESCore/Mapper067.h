
//////////////////////////////////////////////////////////////////////////
// Mapper067  SunSoft Mapper 3                                          //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	BYTE	irq_occur;
	BYTE	irq_toggle;
	INT	irq_counter;
}Mapper067;

void	Mapper067_Reset(Mapper *mapper)
{
	DWORD crc;
	((Mapper067 *)mapper)->irq_enable = 0;
	((Mapper067 *)mapper)->irq_toggle = 0;
	((Mapper067 *)mapper)->irq_counter = 0;
	((Mapper067 *)mapper)->irq_occur = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	SetVROM_4K_Bank( 0, 0 );
	SetVROM_4K_Bank( 4, VROM_4K_SIZE-1 );

	crc = ROM_GetPROM_CRC(((Mapper067 *)mapper)->nes->rom);

	if( crc == 0x7f2a04bf ) {	// For Fantasy Zone 2(J)
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}
}

void	Mapper067_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF800 ) {
		case	0x8800:
			SetVROM_2K_Bank( 0, data );
			break;
		case	0x9800:
			SetVROM_2K_Bank( 2, data );
			break;
		case	0xA800:
			SetVROM_2K_Bank( 4, data );
			break;
		case	0xB800:
			SetVROM_2K_Bank( 6, data );
			break;

		case	0xC800:
			if( !((Mapper067 *)mapper)->irq_toggle ) {
				((Mapper067 *)mapper)->irq_counter = (((Mapper067 *)mapper)->irq_counter&0x00FF)|((INT)data<<8);
			} else {
				((Mapper067 *)mapper)->irq_counter = (((Mapper067 *)mapper)->irq_counter&0xFF00)|((INT)data&0xFF);
			}
			((Mapper067 *)mapper)->irq_toggle ^= 1;
			((Mapper067 *)mapper)->irq_occur = 0;
			CPU_ClrIRQ( ((Mapper067 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xD800:
			((Mapper067 *)mapper)->irq_enable = data & 0x10;
			((Mapper067 *)mapper)->irq_toggle = 0;
			((Mapper067 *)mapper)->irq_occur = 0;
			CPU_ClrIRQ( ((Mapper067 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;

		case	0xE800:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			break;

		case	0xF800:
			SetPROM_16K_Bank( 4, data );
			break;
	}
}

void	Mapper067_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper067 *)mapper)->irq_enable ) {
		if( (((Mapper067 *)mapper)->irq_counter -= cycles) <= 0 ) {
			((Mapper067 *)mapper)->irq_enable = 0;
			((Mapper067 *)mapper)->irq_occur = 0xFF;
			((Mapper067 *)mapper)->irq_counter = 0xFFFF;
			CPU_SetIRQ( ((Mapper067 *)mapper)->nes->cpu, IRQ_MAPPER );
		}
	}

//	if( ((Mapper067 *)mapper)->irq_occur ) {
//		((Mapper067 *)mapper)->nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper067_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper067 *)mapper)->irq_enable;
	p[1] = ((Mapper067 *)mapper)->irq_occur;
	p[2] = ((Mapper067 *)mapper)->irq_toggle;
	*((INT*)&p[3]) = ((Mapper067 *)mapper)->irq_counter;
}

void	Mapper067_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper067 *)mapper)->irq_enable  = p[0];
	((Mapper067 *)mapper)->irq_occur   = p[1];
	((Mapper067 *)mapper)->irq_toggle  = p[2];
	((Mapper067 *)mapper)->irq_counter = *((INT*)&p[3]);
}

BOOL	Mapper067_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper067_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper067));
	
	memset(mapper, 0, sizeof(Mapper067));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper067_Reset;
	mapper->vtbl.Write = Mapper067_Write;
	mapper->vtbl.Clock = Mapper067_Clock;
	mapper->vtbl.SaveState = Mapper067_SaveState;
	mapper->vtbl.LoadState = Mapper067_LoadState;
	mapper->vtbl.IsStateSave = Mapper067_IsStateSave;

	return (Mapper *)mapper;
}

