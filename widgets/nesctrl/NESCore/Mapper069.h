
//////////////////////////////////////////////////////////////////////////
// Mapper069  SunSoft FME-7                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;

	BYTE	reg;
	BYTE	irq_enable;
	INT	irq_counter;
}Mapper069;

void	Mapper069_Reset(Mapper *mapper)
{
	DWORD crc;
	((Mapper069 *)mapper)->reg = 0;
	((Mapper069 *)mapper)->irq_enable = 0;
	((Mapper069 *)mapper)->irq_counter = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	APU_SelectExSound(mapper->nes->apu, 32);
	NES_SetIrqType( mapper->nes, IRQ_CLOCK );

	((Mapper069 *)mapper)->patch = 0;

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	if( crc == 0xfeac6916 ) {	// Honoo no Toukyuuji - Dodge Danpei 2(J)
//		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}

	if( crc == 0xad28aef6 ) {	// Dynamite Batman(J) / Dynamite Batman - Return of the Joker(U)
		((Mapper069 *)mapper)->patch = 1;
	}
}

void	Mapper069_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE000 ) {
		case	0x8000:
			((Mapper069 *)mapper)->reg = data;
			break;

		case	0xA000:
			switch( ((Mapper069 *)mapper)->reg & 0x0F ) {
				case	0x00:	case	0x01:
				case	0x02:	case	0x03:
				case	0x04:	case	0x05:
				case	0x06:	case	0x07:
					SetVROM_1K_Bank( ((Mapper069 *)mapper)->reg&0x07, data );
					break;
				case	0x08:
					if( !((Mapper069 *)mapper)->patch && !(data & 0x40) ) {
						SetPROM_8K_Bank( 3, data );
					}
					break;
				case	0x09:
					SetPROM_8K_Bank( 4, data );
					break;
				case	0x0A:
					SetPROM_8K_Bank( 5, data );
					break;
				case	0x0B:
					SetPROM_8K_Bank( 6, data );
					break;

				case	0x0C:
					data &= 0x03;
					if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_VMIRROR );
					else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
					else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
					else		     SetVRAM_Mirror_cont( VRAM_MIRROR4H );
					break;

				case	0x0D:
					((Mapper069 *)mapper)->irq_enable = data;
					CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
					break;

				case	0x0E:
					((Mapper069 *)mapper)->irq_counter = (((Mapper069 *)mapper)->irq_counter & 0xFF00) | data;
					CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
					break;

				case	0x0F:
					((Mapper069 *)mapper)->irq_counter = (((Mapper069 *)mapper)->irq_counter & 0x00FF) | (data << 8);
					CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
					break;
			}
			break;

		case	0xC000:
		case	0xE000:
			APU_ExWrite( mapper->nes->apu, addr, data );
			break;
	}
}

void	Mapper069_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper069 *)mapper)->irq_enable && (NES_GetIrqType(mapper->nes) == IRQ_CLOCK) ) {
		((Mapper069 *)mapper)->irq_counter -= cycles;
		if( ((Mapper069 *)mapper)->irq_counter <= 0 ) {
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
			((Mapper069 *)mapper)->irq_enable = 0;
			((Mapper069 *)mapper)->irq_counter = 0xFFFF;
		}
	}
}

void	Mapper069_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper069 *)mapper)->irq_enable && (NES_GetIrqType(mapper->nes) == IRQ_HSYNC) ) {
		((Mapper069 *)mapper)->irq_counter -= 114;
		if( ((Mapper069 *)mapper)->irq_counter <= 0 ) {
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
			((Mapper069 *)mapper)->irq_enable = 0;
			((Mapper069 *)mapper)->irq_counter = 0xFFFF;
		}
	}
}

void	Mapper069_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper069 *)mapper)->reg;
	p[1] = ((Mapper069 *)mapper)->irq_enable;
	*(INT*)&p[2] = ((Mapper069 *)mapper)->irq_counter;
}

void	Mapper069_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper069 *)mapper)->reg = p[0];
	((Mapper069 *)mapper)->irq_enable  = p[1];
	((Mapper069 *)mapper)->irq_counter = *(INT*)&p[2];
}

BOOL	Mapper069_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper069_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper069));
	
	memset(mapper, 0, sizeof(Mapper069));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper069_Reset;
	mapper->vtbl.Write = Mapper069_Write;
	mapper->vtbl.Clock = Mapper069_Clock;
	mapper->vtbl.HSync = Mapper069_HSync;
	mapper->vtbl.SaveState = Mapper069_SaveState;
	mapper->vtbl.LoadState = Mapper069_LoadState;
	mapper->vtbl.IsStateSave = Mapper069_IsStateSave;

	return (Mapper *)mapper;
}

