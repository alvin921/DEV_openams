//////////////////////////////////////////////////////////////////////////
// Mapper027  Konami VRC4 (World Hero)                                  //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	WORD	reg[9];

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	INT	irq_clock;
}Mapper027;

void	Mapper027_Reset(Mapper *mapper)
{
	INT i;
	DWORD crc;
	for( i = 0; i < 8; i++ ) {
		((Mapper027 *)mapper)->reg[i] = i;
	}
	((Mapper027 *)mapper)->reg[8] = 0;

	((Mapper027 *)mapper)->irq_enable = 0;
	((Mapper027 *)mapper)->irq_counter = 0;
	((Mapper027 *)mapper)->irq_latch = 0;
	((Mapper027 *)mapper)->irq_clock = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	crc = ROM_GetPROM_CRC(((Mapper027 *)mapper)->nes->rom);
	if( crc == 0x47DCBCC4 ) {	// Gradius II(sample)
		NES_SetRenderMethod( mapper->nes, POST_RENDER );
	}
	if( crc == 0x468F21FC ) {	// Racer Mini 4 ku(sample)
		NES_SetRenderMethod( mapper->nes, POST_RENDER );
	}
}

void	Mapper027_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF0CF ) {
		case	0x8000:
			if(((Mapper027 *)mapper)->reg[8] & 0x02) {
				SetPROM_8K_Bank( 6, data );
			} else {
				SetPROM_8K_Bank( 4, data );
			}
			break;
		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;

		case	0x9000:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			break;

		case 0x9002:
		case 0x9080:
			((Mapper027 *)mapper)->reg[8] = data;
			break;

		case 0xB000:
			((Mapper027 *)mapper)->reg[0] = (((Mapper027 *)mapper)->reg[0] & 0xFF0) | (data & 0x0F);
			SetVROM_1K_Bank( 0, ((Mapper027 *)mapper)->reg[0] );
			break;
		case 0xB001:
			((Mapper027 *)mapper)->reg[0] = (((Mapper027 *)mapper)->reg[0] & 0x0F) | (data<< 4);
			SetVROM_1K_Bank( 0, ((Mapper027 *)mapper)->reg[0] );
			break;

		case 0xB002:
			((Mapper027 *)mapper)->reg[1] = (((Mapper027 *)mapper)->reg[1] & 0xFF0) | (data & 0x0F);
			SetVROM_1K_Bank( 1, ((Mapper027 *)mapper)->reg[1] );
			break;
		case 0xB003:
			((Mapper027 *)mapper)->reg[1] = (((Mapper027 *)mapper)->reg[1] & 0x0F) | (data<< 4);
			SetVROM_1K_Bank( 1, ((Mapper027 *)mapper)->reg[1] );
			break;

		case 0xC000:
			((Mapper027 *)mapper)->reg[2] = (((Mapper027 *)mapper)->reg[2] & 0xFF0) | (data & 0x0F);
			SetVROM_1K_Bank( 2, ((Mapper027 *)mapper)->reg[2] );
			break;
		case 0xC001:
			((Mapper027 *)mapper)->reg[2] = (((Mapper027 *)mapper)->reg[2] & 0x0F) | (data<< 4);
			SetVROM_1K_Bank( 2, ((Mapper027 *)mapper)->reg[2] );
			break;

		case 0xC002:
			((Mapper027 *)mapper)->reg[3] = (((Mapper027 *)mapper)->reg[3] & 0xFF0) | (data & 0x0F);
			SetVROM_1K_Bank( 3, ((Mapper027 *)mapper)->reg[3] );
			break;
		case 0xC003:
			((Mapper027 *)mapper)->reg[3] = (((Mapper027 *)mapper)->reg[3] & 0x0F) | (data<< 4);
			SetVROM_1K_Bank( 3, ((Mapper027 *)mapper)->reg[3] );
			break;

		case 0xD000:
			((Mapper027 *)mapper)->reg[4] = (((Mapper027 *)mapper)->reg[4] & 0xFF0) | (data & 0x0F);
			SetVROM_1K_Bank( 4, ((Mapper027 *)mapper)->reg[4] );
			break;
		case 0xD001:
			((Mapper027 *)mapper)->reg[4] = (((Mapper027 *)mapper)->reg[4] & 0x0F) | (data<< 4);
			SetVROM_1K_Bank( 4, ((Mapper027 *)mapper)->reg[4] );
			break;

		case 0xD002:
			((Mapper027 *)mapper)->reg[5] = (((Mapper027 *)mapper)->reg[5] & 0xFF0) | (data & 0x0F);
			SetVROM_1K_Bank( 5, ((Mapper027 *)mapper)->reg[5] );
			break;
		case 0xD003:
			((Mapper027 *)mapper)->reg[5] = (((Mapper027 *)mapper)->reg[5] & 0x0F) | (data << 4);
			SetVROM_1K_Bank( 5, ((Mapper027 *)mapper)->reg[5] );
			break;

		case 0xE000:
			((Mapper027 *)mapper)->reg[6] = (((Mapper027 *)mapper)->reg[6] & 0xFF0) | (data & 0x0F);
			SetVROM_1K_Bank( 6, ((Mapper027 *)mapper)->reg[6] );
			break;
		case 0xE001:
			((Mapper027 *)mapper)->reg[6] = (((Mapper027 *)mapper)->reg[6] & 0x0F) | (data << 4);
			SetVROM_1K_Bank( 6, ((Mapper027 *)mapper)->reg[6] );
			break;

		case 0xE002:
			((Mapper027 *)mapper)->reg[7] = (((Mapper027 *)mapper)->reg[7] & 0xFF0) | (data & 0x0F);
			SetVROM_1K_Bank( 7, ((Mapper027 *)mapper)->reg[7] );
			break;
		case 0xE003:
			((Mapper027 *)mapper)->reg[7] = (((Mapper027 *)mapper)->reg[7] & 0x0F) | (data<< 4);
			SetVROM_1K_Bank( 7, ((Mapper027 *)mapper)->reg[7] );
			break;

		case 0xF000:
			((Mapper027 *)mapper)->irq_latch = (((Mapper027 *)mapper)->irq_latch & 0xF0) | (data & 0x0F);
			CPU_ClrIRQ( ((Mapper027 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case 0xF001:
			((Mapper027 *)mapper)->irq_latch = (((Mapper027 *)mapper)->irq_latch & 0x0F) | ((data & 0x0F) << 4);
			CPU_ClrIRQ( ((Mapper027 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;

		case 0xF003:
			((Mapper027 *)mapper)->irq_enable = (((Mapper027 *)mapper)->irq_enable & 0x01) * 3;
			((Mapper027 *)mapper)->irq_clock = 0;
			CPU_ClrIRQ( ((Mapper027 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;

		case 0xF002:
			((Mapper027 *)mapper)->irq_enable = data & 0x03;
			if( ((Mapper027 *)mapper)->irq_enable & 0x02 ) {
				((Mapper027 *)mapper)->irq_counter = ((Mapper027 *)mapper)->irq_latch;
				((Mapper027 *)mapper)->irq_clock = 0;
			}
			CPU_ClrIRQ( ((Mapper027 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper027_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper027 *)mapper)->irq_enable & 0x02 ) {
		if( ((Mapper027 *)mapper)->irq_counter == 0xFF ) {
			((Mapper027 *)mapper)->irq_counter = ((Mapper027 *)mapper)->irq_latch;
//			((Mapper027 *)mapper)->nes->cpu->IRQ_NotPending();
			CPU_SetIRQ( ((Mapper027 *)mapper)->nes->cpu, IRQ_MAPPER );
		} else {
			((Mapper027 *)mapper)->irq_counter++;
		}
	}
}

void	Mapper027_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 9; i++ ) {
		p[i] = ((Mapper027 *)mapper)->reg[i];
	}
	p[ 9] = ((Mapper027 *)mapper)->irq_enable;
	p[10] = ((Mapper027 *)mapper)->irq_counter;
	p[11] = ((Mapper027 *)mapper)->irq_latch;
	*(INT*)&p[12] = ((Mapper027 *)mapper)->irq_clock;
}

void	Mapper027_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 9; i++ ) {
		((Mapper027 *)mapper)->reg[i] = p[i];
	}
	((Mapper027 *)mapper)->irq_enable  = p[ 9];
	((Mapper027 *)mapper)->irq_counter = p[10];
	((Mapper027 *)mapper)->irq_latch   = p[11];
	((Mapper027 *)mapper)->irq_clock   = *(INT*)&p[12];
}

BOOL	Mapper027_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper027_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper027));
	
	memset(mapper, 0, sizeof(Mapper027));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper027_Reset;
	mapper->vtbl.Write = Mapper027_Write;
	mapper->vtbl.HSync = Mapper027_HSync;
	mapper->vtbl.SaveState = Mapper027_SaveState;
	mapper->vtbl.LoadState = Mapper027_LoadState;
	mapper->vtbl.IsStateSave = Mapper027_IsStateSave;

	return (Mapper *)mapper;
}

