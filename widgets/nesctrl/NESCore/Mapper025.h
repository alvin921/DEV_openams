
//////////////////////////////////////////////////////////////////////////
// Mapper025  Konami VRC4(Normal)                                       //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[11];
	BYTE	irq_enable;
	BYTE	irq_latch;
	BYTE	irq_occur;
	BYTE	irq_counter;
	INT	irq_clock;
}Mapper025;
void	Mapper025_SetBank_CPU(Mapper *mapper);
void	Mapper025_SetBank_PPU(Mapper *mapper);

void	Mapper025_Reset(Mapper *mapper)
{
	INT i;
	DWORD crc;
	for( i = 0; i < 11; i++ ) {
		((Mapper025 *)mapper)->reg[i] = 0;
	}
	((Mapper025 *)mapper)->reg[9] = PROM_8K_SIZE-2;

	((Mapper025 *)mapper)->irq_enable = 0;
	((Mapper025 *)mapper)->irq_counter = 0;
	((Mapper025 *)mapper)->irq_latch = 0;
	((Mapper025 *)mapper)->irq_occur = 0;
	((Mapper025 *)mapper)->irq_clock = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	crc = ROM_GetPROM_CRC(mapper->nes->rom);
	if( crc == 0xc71d4ce7 ) {	// Gradius II(J)
//		mapper->nes->SetRenderMethod( POST_RENDER );
	}
	if( crc == 0xa2e68da8 ) {	// For Racer Mini Yonku - Japan Cup(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0xea74c587 ) {	// For Teenage Mutant Ninja Turtles(J)
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
	if( crc == 0x5f82cb7d ) {	// For Teenage Mutant Ninja Turtles 2(J)
	}
	if( crc == 0x0bbd85ff ) {	// For Bio Miracle Bokutte Upa(J)
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}
}

void	Mapper025_Write( Mapper *mapper, WORD addr, BYTE data )
{
//if( addr >= 0xF000 )
//DebugOut( "M25 WR $%04X=$%02X L=%3d\n", addr, data, mapper->nes->GetScanline() );

	switch( addr & 0xF000 ) {
		case	0x8000:
			if(((Mapper025 *)mapper)->reg[10] & 0x02) {
				((Mapper025 *)mapper)->reg[9] = data;
				SetPROM_8K_Bank( 6, data );
			} else {
				((Mapper025 *)mapper)->reg[8] = data;
				SetPROM_8K_Bank( 4, data );
			}
			break;
		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;
	}

	switch( addr & 0xF00F ) {
		case	0x9000:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			break;

		case 0x9001:
		case 0x9004:
			if((((Mapper025 *)mapper)->reg[10] & 0x02) != (data & 0x02)) {
				BYTE	swap = ((Mapper025 *)mapper)->reg[8];
				((Mapper025 *)mapper)->reg[8] = ((Mapper025 *)mapper)->reg[9];
				((Mapper025 *)mapper)->reg[9] = swap;

				SetPROM_8K_Bank( 4, ((Mapper025 *)mapper)->reg[8] );
				SetPROM_8K_Bank( 6, ((Mapper025 *)mapper)->reg[9] );
			}
			((Mapper025 *)mapper)->reg[10] = data;
			break;

		case 0xB000:
			((Mapper025 *)mapper)->reg[0] = (((Mapper025 *)mapper)->reg[0] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 0, ((Mapper025 *)mapper)->reg[0] );
			break;
		case 0xB002:
		case 0xB008:
			((Mapper025 *)mapper)->reg[0] = (((Mapper025 *)mapper)->reg[0] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 0, ((Mapper025 *)mapper)->reg[0] );
			break;

		case 0xB001:
		case 0xB004:
			((Mapper025 *)mapper)->reg[1] = (((Mapper025 *)mapper)->reg[1] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 1, ((Mapper025 *)mapper)->reg[1] );
			break;
		case 0xB003:
		case 0xB00C:
			((Mapper025 *)mapper)->reg[1] = (((Mapper025 *)mapper)->reg[1] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 1, ((Mapper025 *)mapper)->reg[1] );
			break;

		case 0xC000:
			((Mapper025 *)mapper)->reg[2] = (((Mapper025 *)mapper)->reg[2] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 2, ((Mapper025 *)mapper)->reg[2] );
			break;
		case 0xC002:
		case 0xC008:
			((Mapper025 *)mapper)->reg[2] = (((Mapper025 *)mapper)->reg[2] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 2, ((Mapper025 *)mapper)->reg[2] );
			break;

		case 0xC001:
		case 0xC004:
			((Mapper025 *)mapper)->reg[3] = (((Mapper025 *)mapper)->reg[3] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 3, ((Mapper025 *)mapper)->reg[3] );
			break;
		case 0xC003:
		case 0xC00C:
			((Mapper025 *)mapper)->reg[3] = (((Mapper025 *)mapper)->reg[3] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 3, ((Mapper025 *)mapper)->reg[3] );
			break;

		case 0xD000:
			((Mapper025 *)mapper)->reg[4] = (((Mapper025 *)mapper)->reg[4] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 4, ((Mapper025 *)mapper)->reg[4] );
			break;
		case 0xD002:
		case 0xD008:
			((Mapper025 *)mapper)->reg[4] = (((Mapper025 *)mapper)->reg[4] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 4, ((Mapper025 *)mapper)->reg[4] );
			break;

		case 0xD001:
		case 0xD004:
			((Mapper025 *)mapper)->reg[5] = (((Mapper025 *)mapper)->reg[5] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 5, ((Mapper025 *)mapper)->reg[5] );
			break;
		case 0xD003:
		case 0xD00C:
			((Mapper025 *)mapper)->reg[5] = (((Mapper025 *)mapper)->reg[5] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 5, ((Mapper025 *)mapper)->reg[5] );
			break;

		case 0xE000:
			((Mapper025 *)mapper)->reg[6] = (((Mapper025 *)mapper)->reg[6] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 6, ((Mapper025 *)mapper)->reg[6] );
			break;
		case 0xE002:
		case 0xE008:
			((Mapper025 *)mapper)->reg[6] = (((Mapper025 *)mapper)->reg[6] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 6, ((Mapper025 *)mapper)->reg[6] );
			break;

		case 0xE001:
		case 0xE004:
			((Mapper025 *)mapper)->reg[7] = (((Mapper025 *)mapper)->reg[7] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 7, ((Mapper025 *)mapper)->reg[7] );
			break;
		case 0xE003:
		case 0xE00C:
			((Mapper025 *)mapper)->reg[7] = (((Mapper025 *)mapper)->reg[7] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 7, ((Mapper025 *)mapper)->reg[7] );
			break;

		case 0xF000:
			((Mapper025 *)mapper)->irq_latch = (((Mapper025 *)mapper)->irq_latch & 0xF0) | (data & 0x0F);
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;

		case 0xF002:
		case 0xF008:
			((Mapper025 *)mapper)->irq_latch = (((Mapper025 *)mapper)->irq_latch & 0x0F) | ((data & 0x0F) << 4);
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;

		case 0xF001:
		case 0xF004:
			((Mapper025 *)mapper)->irq_enable = data & 0x03;
//			((Mapper025 *)mapper)->irq_counter = 0x100 - ((Mapper025 *)mapper)->irq_latch;
			((Mapper025 *)mapper)->irq_counter = ((Mapper025 *)mapper)->irq_latch;
			((Mapper025 *)mapper)->irq_clock = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;

		case 0xF003:
		case 0xF00C:
			((Mapper025 *)mapper)->irq_enable = (((Mapper025 *)mapper)->irq_enable & 0x01)*3;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper025_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper025 *)mapper)->irq_enable & 0x02 ) {
		((Mapper025 *)mapper)->irq_clock += cycles*3;
		while( ((Mapper025 *)mapper)->irq_clock >= 341 ) {
			((Mapper025 *)mapper)->irq_clock -= 341;
			((Mapper025 *)mapper)->irq_counter++;
			if( ((Mapper025 *)mapper)->irq_counter == 0 ) {
				((Mapper025 *)mapper)->irq_counter = ((Mapper025 *)mapper)->irq_latch;
				CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
			}
		}
	}
}

void	Mapper025_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 11; i++ ) {
		p[i] = ((Mapper025 *)mapper)->reg[i];
	}
	p[11] = ((Mapper025 *)mapper)->irq_enable;
	p[12] = ((Mapper025 *)mapper)->irq_occur;
	p[13] = ((Mapper025 *)mapper)->irq_latch;
	p[14] = ((Mapper025 *)mapper)->irq_counter;
	*((INT*)&p[15]) = ((Mapper025 *)mapper)->irq_clock;
}

void	Mapper025_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 11; i++ ) {
		((Mapper025 *)mapper)->reg[i] = p[i];
	}
	((Mapper025 *)mapper)->irq_enable  = p[11];
	((Mapper025 *)mapper)->irq_occur   = p[12];
	((Mapper025 *)mapper)->irq_latch   = p[13];
	((Mapper025 *)mapper)->irq_counter = p[14];
	((Mapper025 *)mapper)->irq_clock   = *((INT*)&p[15]);
}

BOOL	Mapper025_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper025_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper025));
	
	memset(mapper, 0, sizeof(Mapper025));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper025_Reset;
	mapper->vtbl.Write = Mapper025_Write;
	mapper->vtbl.Clock = Mapper025_Clock;
	mapper->vtbl.SaveState = Mapper025_SaveState;
	mapper->vtbl.LoadState = Mapper025_LoadState;
	mapper->vtbl.IsStateSave = Mapper025_IsStateSave;

	return (Mapper *)mapper;
}

