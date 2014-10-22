
//////////////////////////////////////////////////////////////////////////
// Mapper189  Street Fighter 2 Yoko/Master Fighter 2                    //
//            ‰õ‘Å˜úà Street Fighter IV (GOUDER)                       //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;

	BYTE	reg[2];
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;

	// SF4
	BYTE	protect_dat[4];
	BYTE	lwd;
}Mapper189;
void	Mapper189_SetBank_PPU(Mapper *mapper);

void	Mapper189_Reset(Mapper *mapper)
{
	INT i;
	DWORD crc;

	SetPROM_32K_Bank( PROM_8K_SIZE-4, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	((Mapper189 *)mapper)->reg[0] = ((Mapper189 *)mapper)->reg[1] = 0;

	((Mapper189 *)mapper)->chr01 = 0;
	((Mapper189 *)mapper)->chr23 = 2;
	((Mapper189 *)mapper)->chr4  = 4;
	((Mapper189 *)mapper)->chr5  = 5;
	((Mapper189 *)mapper)->chr6  = 6;
	((Mapper189 *)mapper)->chr7  = 7;
	Mapper189_SetBank_PPU(mapper);

	((Mapper189 *)mapper)->irq_enable  = 0;
	((Mapper189 *)mapper)->irq_counter = 0;
	((Mapper189 *)mapper)->irq_latch   = 0;

	for( i = 0; i < 4; i++ ) {
		((Mapper189 *)mapper)->protect_dat[i] = 0;
	}
	((Mapper189 *)mapper)->lwd = 0xFF;

	((Mapper189 *)mapper)->patch = 0;
	crc = ROM_GetPROM_CRC(((Mapper189 *)mapper)->nes->rom);
	if( crc == 0x20ca2ad3 ) {	// Street Fighter IV (GOUDER)
		((Mapper189 *)mapper)->patch = 1;
		SetPROM_32K_Bank_cont( 0 );

		// $4000-$5FFF
		SetPROM_Bank( 2, XRAM, BANKTYPE_ROM );
	}
}

void	Mapper189_WriteLow( Mapper *mapper,WORD addr, BYTE data )
{
	if( (addr & 0xFF00) == 0x4100 ) {
	// Street Fighter 2 YOKO
		SetPROM_32K_Bank_cont( (data&0x30)>>4 );
	} else if( (addr & 0xFF00) == 0x6100 ) {
	// Master Fighter 2
		SetPROM_32K_Bank_cont( data&0x03 );
	}

	if( ((Mapper189 *)mapper)->patch ) {
	// Street Fighter IV (GOUDER)
		BYTE	a5000xordat[256] = {
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x49, 0x19, 0x09, 0x59, 0x49, 0x19, 0x09,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x51, 0x41, 0x11, 0x01, 0x51, 0x41, 0x11, 0x01,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x49, 0x19, 0x09, 0x59, 0x49, 0x19, 0x09,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x51, 0x41, 0x11, 0x01, 0x51, 0x41, 0x11, 0x01,
			0x00, 0x10, 0x40, 0x50, 0x00, 0x10, 0x40, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x08, 0x18, 0x48, 0x58, 0x08, 0x18, 0x48, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x10, 0x40, 0x50, 0x00, 0x10, 0x40, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x08, 0x18, 0x48, 0x58, 0x08, 0x18, 0x48, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x58, 0x48, 0x18, 0x08, 0x58, 0x48, 0x18, 0x08,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x50, 0x40, 0x10, 0x00, 0x50, 0x40, 0x10, 0x00,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x58, 0x48, 0x18, 0x08, 0x58, 0x48, 0x18, 0x08,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x50, 0x40, 0x10, 0x00, 0x50, 0x40, 0x10, 0x00,
			0x01, 0x11, 0x41, 0x51, 0x01, 0x11, 0x41, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x09, 0x19, 0x49, 0x59, 0x09, 0x19, 0x49, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x01, 0x11, 0x41, 0x51, 0x01, 0x11, 0x41, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x09, 0x19, 0x49, 0x59, 0x09, 0x19, 0x49, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};

		if( (addr >= 0x4800) && (addr <= 0x4FFF) ) {
			SetPROM_32K_Bank_cont( ((data&0x10)>>3)+(data&0x1) );

			if( !ROM_Is4SCREEN(((Mapper189 *)mapper)->nes->rom) ) {
				if( data & 0x20 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
		}
		if( (addr>=0x5000) && (addr<=0x57FF) ) {
			((Mapper189 *)mapper)->lwd = data;
		}
		if( (addr>=0x5800) && (addr<=0x5FFF) ) {
//			XRAM[0x1000+(addr & 3)] = 
			// $5800 "JMP $xxxx" write
			XRAM[0x1800+(addr & 3)] = 
			((Mapper189 *)mapper)->protect_dat[ addr & 3 ] = data ^ a5000xordat[ ((Mapper189 *)mapper)->lwd ];
		}
	}
}

void	Mapper189_Write( Mapper *mapper,WORD addr, BYTE data )
{
	switch( addr&0xE001 ) {
		case	0x8000:
			((Mapper189 *)mapper)->reg[0] = data;
			Mapper189_SetBank_PPU(mapper);
			break;

		case	0x8001:
			((Mapper189 *)mapper)->reg[1] = data;
			Mapper189_SetBank_PPU(mapper);
			switch( ((Mapper189 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper189 *)mapper)->chr01 = data & 0xFE;
					Mapper189_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper189 *)mapper)->chr23 = data & 0xFE;
					Mapper189_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper189 *)mapper)->chr4 = data;
					Mapper189_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper189 *)mapper)->chr5 = data;
					Mapper189_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper189 *)mapper)->chr6 = data;
					Mapper189_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper189 *)mapper)->chr7 = data;
					Mapper189_SetBank_PPU(mapper);
					break;
			}
			break;

		case	0xA000:
			if( data&0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;

		case	0xC000:
			((Mapper189 *)mapper)->irq_counter = data;
			break;
		case	0xC001:
			((Mapper189 *)mapper)->irq_latch = data;
			break;
		case	0xE000:
			((Mapper189 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( ((Mapper189 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper189 *)mapper)->irq_enable = 0xFF;
			break;
	}
}

void	Mapper189_HSync( Mapper *mapper,INT scanline )
{
	if( (scanline >= 0 && scanline <= SCREEN_HEIGHT-1) ) {
		if( PPU_IsDispON(((Mapper189 *)mapper)->nes->ppu) ) {
			if( ((Mapper189 *)mapper)->irq_enable ) {
				if( !(--((Mapper189 *)mapper)->irq_counter) ) {
//				if( !(((Mapper189 *)mapper)->irq_counter--) ) {
					((Mapper189 *)mapper)->irq_counter = ((Mapper189 *)mapper)->irq_latch;
					CPU_SetIRQ( ((Mapper189 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper189_SetBank_PPU(Mapper *mapper)
{
	if( ((Mapper189 *)mapper)->patch ) {
		SetVROM_8K_Bank( ((Mapper189 *)mapper)->chr01, ((Mapper189 *)mapper)->chr01+1, ((Mapper189 *)mapper)->chr23, ((Mapper189 *)mapper)->chr23+1,
				 ((Mapper189 *)mapper)->chr4, ((Mapper189 *)mapper)->chr5, ((Mapper189 *)mapper)->chr6, ((Mapper189 *)mapper)->chr7 );
	} else {
		if( VROM_1K_SIZE ) {
			if( ((Mapper189 *)mapper)->reg[0] & 0x80 ) {
				SetVROM_8K_Bank( ((Mapper189 *)mapper)->chr4, ((Mapper189 *)mapper)->chr5, ((Mapper189 *)mapper)->chr6, ((Mapper189 *)mapper)->chr7,
						 ((Mapper189 *)mapper)->chr01, ((Mapper189 *)mapper)->chr01+1, ((Mapper189 *)mapper)->chr23, ((Mapper189 *)mapper)->chr23+1 );
			} else {
				SetVROM_8K_Bank( ((Mapper189 *)mapper)->chr01, ((Mapper189 *)mapper)->chr01+1, ((Mapper189 *)mapper)->chr23, ((Mapper189 *)mapper)->chr23+1,
						 ((Mapper189 *)mapper)->chr4, ((Mapper189 *)mapper)->chr5, ((Mapper189 *)mapper)->chr6, ((Mapper189 *)mapper)->chr7 );
			}
		} else {
			if( ((Mapper189 *)mapper)->reg[0] & 0x80 ) {
				SetCRAM_1K_Bank( 4, (((Mapper189 *)mapper)->chr01+0)&0x07 );
				SetCRAM_1K_Bank( 5, (((Mapper189 *)mapper)->chr01+1)&0x07 );
				SetCRAM_1K_Bank( 6, (((Mapper189 *)mapper)->chr23+0)&0x07 );
				SetCRAM_1K_Bank( 7, (((Mapper189 *)mapper)->chr23+1)&0x07 );
				SetCRAM_1K_Bank( 0, ((Mapper189 *)mapper)->chr4&0x07 );
				SetCRAM_1K_Bank( 1, ((Mapper189 *)mapper)->chr5&0x07 );
				SetCRAM_1K_Bank( 2, ((Mapper189 *)mapper)->chr6&0x07 );
				SetCRAM_1K_Bank( 3, ((Mapper189 *)mapper)->chr7&0x07 );
			} else {
				SetCRAM_1K_Bank( 0, (((Mapper189 *)mapper)->chr01+0)&0x07 );
				SetCRAM_1K_Bank( 1, (((Mapper189 *)mapper)->chr01+1)&0x07 );
				SetCRAM_1K_Bank( 2, (((Mapper189 *)mapper)->chr23+0)&0x07 );
				SetCRAM_1K_Bank( 3, (((Mapper189 *)mapper)->chr23+1)&0x07 );
				SetCRAM_1K_Bank( 4, ((Mapper189 *)mapper)->chr4&0x07 );
				SetCRAM_1K_Bank( 5, ((Mapper189 *)mapper)->chr5&0x07 );
				SetCRAM_1K_Bank( 6, ((Mapper189 *)mapper)->chr6&0x07 );
				SetCRAM_1K_Bank( 7, ((Mapper189 *)mapper)->chr7&0x07 );
			}
		}
	}
}

void	Mapper189_SaveState( Mapper *mapper,LPBYTE p )
{
	p[0] = ((Mapper189 *)mapper)->reg[0];
	p[1] = ((Mapper189 *)mapper)->reg[1];
	p[2] = ((Mapper189 *)mapper)->chr01;
	p[3] = ((Mapper189 *)mapper)->chr23;
	p[4] = ((Mapper189 *)mapper)->chr4;
	p[5] = ((Mapper189 *)mapper)->chr5;
	p[6] = ((Mapper189 *)mapper)->chr6;
	p[7] = ((Mapper189 *)mapper)->chr7;
	p[ 8] = ((Mapper189 *)mapper)->irq_enable;
	p[ 9] = ((Mapper189 *)mapper)->irq_counter;
	p[10] = ((Mapper189 *)mapper)->irq_latch;

	p[16] = ((Mapper189 *)mapper)->protect_dat[0];
	p[17] = ((Mapper189 *)mapper)->protect_dat[1];
	p[18] = ((Mapper189 *)mapper)->protect_dat[2];
	p[19] = ((Mapper189 *)mapper)->protect_dat[3];
	p[20] = ((Mapper189 *)mapper)->lwd;
}

void	Mapper189_LoadState( Mapper *mapper,LPBYTE p )
{
	((Mapper189 *)mapper)->reg[0] = p[0];
	((Mapper189 *)mapper)->reg[1] = p[1];
	((Mapper189 *)mapper)->chr01  = p[2];
	((Mapper189 *)mapper)->chr23  = p[3];
	((Mapper189 *)mapper)->chr4   = p[4];
	((Mapper189 *)mapper)->chr5   = p[5];
	((Mapper189 *)mapper)->chr6   = p[6];
	((Mapper189 *)mapper)->chr7   = p[7];

	((Mapper189 *)mapper)->irq_enable  = p[ 8];
	((Mapper189 *)mapper)->irq_counter = p[ 9];
	((Mapper189 *)mapper)->irq_latch   = p[10];

	((Mapper189 *)mapper)->protect_dat[0] = p[16];
	((Mapper189 *)mapper)->protect_dat[1] = p[17];
	((Mapper189 *)mapper)->protect_dat[2] = p[18];
	((Mapper189 *)mapper)->protect_dat[3] = p[19];
	((Mapper189 *)mapper)->lwd = p[20];
}

BOOL	Mapper189_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper189_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper189));
	
	memset(mapper, 0, sizeof(Mapper189));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper189_Reset;
	mapper->vtbl.Write = Mapper189_Write;
	mapper->vtbl.WriteLow = Mapper189_WriteLow;
	mapper->vtbl.HSync = Mapper189_HSync;
	mapper->vtbl.SaveState = Mapper189_SaveState;
	mapper->vtbl.LoadState = Mapper189_LoadState;
	mapper->vtbl.IsStateSave = Mapper189_IsStateSave;

	return (Mapper *)mapper;
}

