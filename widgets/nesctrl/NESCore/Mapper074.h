
//////////////////////////////////////////////////////////////////////////
// Mapper074  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;

	BYTE	irq_type;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;

	BYTE	patch;
}Mapper074;

void	Mapper074_SetBank_CPU(Mapper *mapper);
void	Mapper074_SetBank_PPU(Mapper *mapper);
void	Mapper074_SetBank_PPUSUB(Mapper *mapper, int bank, int page );

void	Mapper074_Reset(Mapper *mapper)
{
	INT i;
	DWORD	crc;
	for( i = 0; i < 8; i++ ) {
		((Mapper074 *)mapper)->reg[i] = 0x00;
	}
	((Mapper074 *)mapper)->prg0 = 0;
	((Mapper074 *)mapper)->prg1 = 1;
	Mapper074_SetBank_CPU(mapper);

	((Mapper074 *)mapper)->chr01 = 0;
	((Mapper074 *)mapper)->chr23 = 2;
	((Mapper074 *)mapper)->chr4  = 4;
	((Mapper074 *)mapper)->chr5  = 5;
	((Mapper074 *)mapper)->chr6  = 6;
	((Mapper074 *)mapper)->chr7  = 7;
	Mapper074_SetBank_PPU(mapper);

	((Mapper074 *)mapper)->we_sram  = 0;	// Disable
	((Mapper074 *)mapper)->irq_enable = 0;	// Disable
	((Mapper074 *)mapper)->irq_counter = 0;
	((Mapper074 *)mapper)->irq_latch = 0;
	((Mapper074 *)mapper)->irq_request = 0;

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	((Mapper074 *)mapper)->patch = 0;
	if( crc == 0x37ae04a8 ) {
		((Mapper074 *)mapper)->patch = 1;
		NES_SetRenderMethod( mapper->nes, TILE_RENDER );
	}
}

void	Mapper074_Write( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, mapper->nes->GetScanline(), mapper->nes->cpu->GetTotalCycles() );

	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper074 *)mapper)->reg[0] = data;
			Mapper074_SetBank_CPU(mapper);
			Mapper074_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper074 *)mapper)->reg[1] = data;

			switch( ((Mapper074 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper074 *)mapper)->chr01 = data & 0xFE;
					Mapper074_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper074 *)mapper)->chr23 = data & 0xFE;
					Mapper074_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper074 *)mapper)->chr4 = data;
					Mapper074_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper074 *)mapper)->chr5 = data;
					Mapper074_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper074 *)mapper)->chr6 = data;
					Mapper074_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper074 *)mapper)->chr7 = data;
					Mapper074_SetBank_PPU(mapper);
					break;
				case	0x06:
					((Mapper074 *)mapper)->prg0 = data;
					Mapper074_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper074 *)mapper)->prg1 = data;
					Mapper074_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper074 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(mapper->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper074 *)mapper)->reg[3] = data;
			break;
		case	0xC000:
			((Mapper074 *)mapper)->reg[4] = data;
			((Mapper074 *)mapper)->irq_counter = data;
			((Mapper074 *)mapper)->irq_request = 0;
			break;
		case	0xC001:
			((Mapper074 *)mapper)->reg[5] = data;
			((Mapper074 *)mapper)->irq_latch = data;
			((Mapper074 *)mapper)->irq_request = 0;
			break;
		case	0xE000:
			((Mapper074 *)mapper)->reg[6] = data;
			((Mapper074 *)mapper)->irq_enable = 0;
			((Mapper074 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper074 *)mapper)->reg[7] = data;
			((Mapper074 *)mapper)->irq_enable = 1;
			((Mapper074 *)mapper)->irq_request = 0;
			break;
	}	
	
}

void	Mapper074_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(mapper->nes->ppu) ) {
			if( ((Mapper074 *)mapper)->irq_enable && !((Mapper074 *)mapper)->irq_request ) {
				if( scanline == 0 ) {
					if( ((Mapper074 *)mapper)->irq_counter ) {
						((Mapper074 *)mapper)->irq_counter--;
					}
				}
				if( !(((Mapper074 *)mapper)->irq_counter--) ) {
					((Mapper074 *)mapper)->irq_request = 0xFF;
					((Mapper074 *)mapper)->irq_counter = ((Mapper074 *)mapper)->irq_latch;
					CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper074_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper074 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, ((Mapper074 *)mapper)->prg1, ((Mapper074 *)mapper)->prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( ((Mapper074 *)mapper)->prg0, ((Mapper074 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper074_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		if( ((Mapper074 *)mapper)->reg[0] & 0x80 ) {
//			SetVROM_8K_Bank( ((Mapper074 *)mapper)->chr4, ((Mapper074 *)mapper)->chr5, ((Mapper074 *)mapper)->chr6, ((Mapper074 *)mapper)->chr7,
//					 ((Mapper074 *)mapper)->chr01, ((Mapper074 *)mapper)->chr01+1, ((Mapper074 *)mapper)->chr23, ((Mapper074 *)mapper)->chr23+1 );
			Mapper074_SetBank_PPUSUB(mapper,  4, ((Mapper074 *)mapper)->chr01+0 );
			Mapper074_SetBank_PPUSUB(mapper,  5, ((Mapper074 *)mapper)->chr01+1 );
			Mapper074_SetBank_PPUSUB(mapper,  6, ((Mapper074 *)mapper)->chr23+0 );
			Mapper074_SetBank_PPUSUB(mapper,  7, ((Mapper074 *)mapper)->chr23+1 );
			Mapper074_SetBank_PPUSUB(mapper,  0, ((Mapper074 *)mapper)->chr4 );
			Mapper074_SetBank_PPUSUB(mapper,  1, ((Mapper074 *)mapper)->chr5 );
			Mapper074_SetBank_PPUSUB(mapper,  2, ((Mapper074 *)mapper)->chr6 );
			Mapper074_SetBank_PPUSUB(mapper,  3, ((Mapper074 *)mapper)->chr7 );
		} else {
//			SetVROM_8K_Bank( ((Mapper074 *)mapper)->chr01, ((Mapper074 *)mapper)->chr01+1, ((Mapper074 *)mapper)->chr23, ((Mapper074 *)mapper)->chr23+1,
//					 ((Mapper074 *)mapper)->chr4, ((Mapper074 *)mapper)->chr5, ((Mapper074 *)mapper)->chr6, ((Mapper074 *)mapper)->chr7 );
			Mapper074_SetBank_PPUSUB(mapper,  0, ((Mapper074 *)mapper)->chr01+0 );
			Mapper074_SetBank_PPUSUB(mapper,  1, ((Mapper074 *)mapper)->chr01+1 );
			Mapper074_SetBank_PPUSUB(mapper,  2, ((Mapper074 *)mapper)->chr23+0 );
			Mapper074_SetBank_PPUSUB(mapper,  3, ((Mapper074 *)mapper)->chr23+1 );
			Mapper074_SetBank_PPUSUB(mapper,  4, ((Mapper074 *)mapper)->chr4 );
			Mapper074_SetBank_PPUSUB(mapper,  5, ((Mapper074 *)mapper)->chr5 );
			Mapper074_SetBank_PPUSUB(mapper,  6, ((Mapper074 *)mapper)->chr6 );
			Mapper074_SetBank_PPUSUB(mapper,  7, ((Mapper074 *)mapper)->chr7 );
		}
	} else {
		if( ((Mapper074 *)mapper)->reg[0] & 0x80 ) {
			SetCRAM_1K_Bank( 4, (((Mapper074 *)mapper)->chr01+0)&0x07 );
			SetCRAM_1K_Bank( 5, (((Mapper074 *)mapper)->chr01+1)&0x07 );
			SetCRAM_1K_Bank( 6, (((Mapper074 *)mapper)->chr23+0)&0x07 );
			SetCRAM_1K_Bank( 7, (((Mapper074 *)mapper)->chr23+1)&0x07 );
			SetCRAM_1K_Bank( 0, ((Mapper074 *)mapper)->chr4&0x07 );
			SetCRAM_1K_Bank( 1, ((Mapper074 *)mapper)->chr5&0x07 );
			SetCRAM_1K_Bank( 2, ((Mapper074 *)mapper)->chr6&0x07 );
			SetCRAM_1K_Bank( 3, ((Mapper074 *)mapper)->chr7&0x07 );
		} else {
			SetCRAM_1K_Bank( 0, (((Mapper074 *)mapper)->chr01+0)&0x07 );
			SetCRAM_1K_Bank( 1, (((Mapper074 *)mapper)->chr01+1)&0x07 );
			SetCRAM_1K_Bank( 2, (((Mapper074 *)mapper)->chr23+0)&0x07 );
			SetCRAM_1K_Bank( 3, (((Mapper074 *)mapper)->chr23+1)&0x07 );
			SetCRAM_1K_Bank( 4, ((Mapper074 *)mapper)->chr4&0x07 );
			SetCRAM_1K_Bank( 5, ((Mapper074 *)mapper)->chr5&0x07 );
			SetCRAM_1K_Bank( 6, ((Mapper074 *)mapper)->chr6&0x07 );
			SetCRAM_1K_Bank( 7, ((Mapper074 *)mapper)->chr7&0x07 );
		}
	}
}

void	Mapper074_SetBank_PPUSUB( Mapper *mapper, int bank, int page )
{
	if( !((Mapper074 *)mapper)->patch && (page == 8 || page == 9) ) {
		SetCRAM_1K_Bank( bank, page & 7 );
	} else if( ((Mapper074 *)mapper)->patch == 1 && page >= 128 ) {
		SetCRAM_1K_Bank( bank, page & 7 );
	} else {
		SetVROM_1K_Bank( bank, page );
	}
}

void	Mapper074_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper074 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper074 *)mapper)->prg0;
	p[ 9] = ((Mapper074 *)mapper)->prg1;
	p[10] = ((Mapper074 *)mapper)->chr01;
	p[11] = ((Mapper074 *)mapper)->chr23;
	p[12] = ((Mapper074 *)mapper)->chr4;
	p[13] = ((Mapper074 *)mapper)->chr5;
	p[14] = ((Mapper074 *)mapper)->chr6;
	p[15] = ((Mapper074 *)mapper)->chr7;
	p[16] = ((Mapper074 *)mapper)->irq_enable;
	p[17] = ((Mapper074 *)mapper)->irq_counter;
	p[18] = ((Mapper074 *)mapper)->irq_latch;
	p[19] = ((Mapper074 *)mapper)->irq_request;
}

void	Mapper074_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper074 *)mapper)->reg[i] = p[i];
	}
	((Mapper074 *)mapper)->prg0  = p[ 8];
	((Mapper074 *)mapper)->prg1  = p[ 9];
	((Mapper074 *)mapper)->chr01 = p[10];
	((Mapper074 *)mapper)->chr23 = p[11];
	((Mapper074 *)mapper)->chr4  = p[12];
	((Mapper074 *)mapper)->chr5  = p[13];
	((Mapper074 *)mapper)->chr6  = p[14];
	((Mapper074 *)mapper)->chr7  = p[15];
	((Mapper074 *)mapper)->irq_enable  = p[16];
	((Mapper074 *)mapper)->irq_counter = p[17];
	((Mapper074 *)mapper)->irq_latch   = p[18];
	((Mapper074 *)mapper)->irq_request = p[19];
}

BOOL	Mapper074_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper074_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper074));
	
	memset(mapper, 0, sizeof(Mapper074));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper074_Reset;
	mapper->vtbl.Write = Mapper074_Write;
	mapper->vtbl.HSync = Mapper074_HSync;
	mapper->vtbl.SaveState = Mapper074_SaveState;
	mapper->vtbl.LoadState = Mapper074_LoadState;
	mapper->vtbl.IsStateSave = Mapper074_IsStateSave;

	return (Mapper *)mapper;
}

