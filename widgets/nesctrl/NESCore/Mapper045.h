
//////////////////////////////////////////////////////////////////////////
// Mapper045  1000000-in-1                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	patch;
	BYTE	prg0, prg1, prg2, prg3;
	BYTE	chr0, chr1, chr2, chr3, chr4, chr5, chr6, chr7;
	BYTE	p[4];
	INT	c[8];
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_latched;
	BYTE	irq_reset;
}Mapper045;

void	Mapper045_SetBank_CPU_4( Mapper *mapper, INT data );
void	Mapper045_SetBank_CPU_5( Mapper *mapper, INT data );
void	Mapper045_SetBank_CPU_6( Mapper *mapper, INT data );
void	Mapper045_SetBank_CPU_7( Mapper *mapper, INT data );
void	Mapper045_SetBank_PPU(Mapper *mapper );

void	Mapper045_Reset(Mapper *mapper)
{
	INT i;
	DWORD	crc;

	((Mapper045 *)mapper)->patch = 0;
	for( i = 0; i < 8; i++ ) {
		((Mapper045 *)mapper)->reg[i] = 0;
	}

	((Mapper045 *)mapper)->prg0 = 0;
	((Mapper045 *)mapper)->prg1 = 1;
	((Mapper045 *)mapper)->prg2 = PROM_8K_SIZE-2;
	((Mapper045 *)mapper)->prg3 = PROM_8K_SIZE-1;

	crc = ROM_GetPROM_CRC(((Mapper045 *)mapper)->nes->rom);
	if( crc == 0x58bcacf6		// Kunio 8-in-1 (Pirate Cart)
	 || crc == 0x9103cfd6		// HIK 7-in-1 (Pirate Cart)
	 || crc == 0xc082e6d3 ) {	// Super 8-in-1 (Pirate Cart)
		((Mapper045 *)mapper)->patch = 1;
		((Mapper045 *)mapper)->prg2 = 62;
		((Mapper045 *)mapper)->prg3 = 63;
	}
	if( crc == 0xe0dd259d ) {	// Super 3-in-1 (Pirate Cart)
		((Mapper045 *)mapper)->patch = 2;
	}
	SetPROM_32K_Bank( ((Mapper045 *)mapper)->prg0, ((Mapper045 *)mapper)->prg1, ((Mapper045 *)mapper)->prg2, ((Mapper045 *)mapper)->prg3 );
	((Mapper045 *)mapper)->p[0] = ((Mapper045 *)mapper)->prg0;
	((Mapper045 *)mapper)->p[1] = ((Mapper045 *)mapper)->prg1;
	((Mapper045 *)mapper)->p[2] = ((Mapper045 *)mapper)->prg2;
	((Mapper045 *)mapper)->p[3] = ((Mapper045 *)mapper)->prg3;

	SetVROM_8K_Bank_cont( 0 );

//	((Mapper045 *)mapper)->chr0 = ((Mapper045 *)mapper)->c[0] = 0;
//	((Mapper045 *)mapper)->chr1 = ((Mapper045 *)mapper)->c[1] = 0
//	((Mapper045 *)mapper)->chr2 = ((Mapper045 *)mapper)->c[2] = 0;
//	((Mapper045 *)mapper)->chr3 = ((Mapper045 *)mapper)->c[3] = 0;
//	((Mapper045 *)mapper)->chr4 = ((Mapper045 *)mapper)->c[4] = 0;
//	((Mapper045 *)mapper)->chr5 = ((Mapper045 *)mapper)->c[5] = 0;
//	((Mapper045 *)mapper)->chr6 = ((Mapper045 *)mapper)->c[6] = 0;
//	((Mapper045 *)mapper)->chr7 = ((Mapper045 *)mapper)->c[7] = 0;

	((Mapper045 *)mapper)->chr0 = ((Mapper045 *)mapper)->c[0] = 0;
	((Mapper045 *)mapper)->chr1 = ((Mapper045 *)mapper)->c[1] = 1;
	((Mapper045 *)mapper)->chr2 = ((Mapper045 *)mapper)->c[2] = 2;
	((Mapper045 *)mapper)->chr3 = ((Mapper045 *)mapper)->c[3] = 3;
	((Mapper045 *)mapper)->chr4 = ((Mapper045 *)mapper)->c[4] = 4;
	((Mapper045 *)mapper)->chr5 = ((Mapper045 *)mapper)->c[5] = 5;
	((Mapper045 *)mapper)->chr6 = ((Mapper045 *)mapper)->c[6] = 6;
	((Mapper045 *)mapper)->chr7 = ((Mapper045 *)mapper)->c[7] = 7;

	((Mapper045 *)mapper)->irq_enable = 0;
	((Mapper045 *)mapper)->irq_counter = 0;
	((Mapper045 *)mapper)->irq_latch = 0;
	((Mapper045 *)mapper)->irq_latched = 0;
	((Mapper045 *)mapper)->irq_reset = 0;
}

void	Mapper045_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
//	if( addr == 0x6000 ) {
//	if( addr == 0x6000 && !(((Mapper045 *)mapper)->reg[3]&0x40) ) {
	if( !(((Mapper045 *)mapper)->reg[3]&0x40) ) {
		((Mapper045 *)mapper)->reg[((Mapper045 *)mapper)->reg[5]] = data;
		((Mapper045 *)mapper)->reg[5] = (((Mapper045 *)mapper)->reg[5]+1) & 0x03;

		Mapper045_SetBank_CPU_4( mapper, ((Mapper045 *)mapper)->prg0 );
		Mapper045_SetBank_CPU_5( mapper, ((Mapper045 *)mapper)->prg1 );
		Mapper045_SetBank_CPU_6( mapper, ((Mapper045 *)mapper)->prg2 );
		Mapper045_SetBank_CPU_7( mapper, ((Mapper045 *)mapper)->prg3 );
		Mapper045_SetBank_PPU(mapper);
	}
}

void	Mapper045_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			if( (data&0x40)!=(((Mapper045 *)mapper)->reg[6]&0x40) ) {
				BYTE	swp;
				swp = ((Mapper045 *)mapper)->prg0; ((Mapper045 *)mapper)->prg0 = ((Mapper045 *)mapper)->prg2; ((Mapper045 *)mapper)->prg2 = swp;
				swp = ((Mapper045 *)mapper)->p[0]; ((Mapper045 *)mapper)->p[0] = ((Mapper045 *)mapper)->p[2]; ((Mapper045 *)mapper)->p[2] = swp;
				Mapper045_SetBank_CPU_4( mapper, ((Mapper045 *)mapper)->p[0] );
				Mapper045_SetBank_CPU_5( mapper, ((Mapper045 *)mapper)->p[1] );
			}
			if( VROM_1K_SIZE ) {
				if( (data&0x80)!=(((Mapper045 *)mapper)->reg[6]&0x80) ) {
					INT	swp;
					swp = ((Mapper045 *)mapper)->chr4; ((Mapper045 *)mapper)->chr4 = ((Mapper045 *)mapper)->chr0; ((Mapper045 *)mapper)->chr0 = swp;
					swp = ((Mapper045 *)mapper)->chr5; ((Mapper045 *)mapper)->chr5 = ((Mapper045 *)mapper)->chr1; ((Mapper045 *)mapper)->chr1 = swp;
					swp = ((Mapper045 *)mapper)->chr6; ((Mapper045 *)mapper)->chr6 = ((Mapper045 *)mapper)->chr2; ((Mapper045 *)mapper)->chr2 = swp;
					swp = ((Mapper045 *)mapper)->chr7; ((Mapper045 *)mapper)->chr7 = ((Mapper045 *)mapper)->chr3; ((Mapper045 *)mapper)->chr3 = swp;
					swp = ((Mapper045 *)mapper)->c[4]; ((Mapper045 *)mapper)->c[4] = ((Mapper045 *)mapper)->c[0]; ((Mapper045 *)mapper)->c[0] = swp;
					swp = ((Mapper045 *)mapper)->c[5]; ((Mapper045 *)mapper)->c[5] = ((Mapper045 *)mapper)->c[1]; ((Mapper045 *)mapper)->c[1] = swp;
					swp = ((Mapper045 *)mapper)->c[6]; ((Mapper045 *)mapper)->c[6] = ((Mapper045 *)mapper)->c[2]; ((Mapper045 *)mapper)->c[2] = swp;
					swp = ((Mapper045 *)mapper)->c[7]; ((Mapper045 *)mapper)->c[7] = ((Mapper045 *)mapper)->c[3]; ((Mapper045 *)mapper)->c[3] = swp;
					SetVROM_8K_Bank( ((Mapper045 *)mapper)->c[0],((Mapper045 *)mapper)->c[1],((Mapper045 *)mapper)->c[2],((Mapper045 *)mapper)->c[3],((Mapper045 *)mapper)->c[4],((Mapper045 *)mapper)->c[5],((Mapper045 *)mapper)->c[6],((Mapper045 *)mapper)->c[7] );
				}
			}
			((Mapper045 *)mapper)->reg[6] = data;
			break;
		case	0x8001:
			switch( ((Mapper045 *)mapper)->reg[6] & 0x07 ) {
				case	0x00:
					((Mapper045 *)mapper)->chr0 = (data & 0xFE)+0;
					((Mapper045 *)mapper)->chr1 = (data & 0xFE)+1;
					Mapper045_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper045 *)mapper)->chr2 = (data & 0xFE)+0;
					((Mapper045 *)mapper)->chr3 = (data & 0xFE)+1;
					Mapper045_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper045 *)mapper)->chr4 = data;
					Mapper045_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper045 *)mapper)->chr5 = data;
					Mapper045_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper045 *)mapper)->chr6 = data;
					Mapper045_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper045 *)mapper)->chr7 = data;
					Mapper045_SetBank_PPU(mapper);
					break;
				case	0x06:
					if( ((Mapper045 *)mapper)->reg[6] & 0x40 ) {
						((Mapper045 *)mapper)->prg2 = data & 0x3F;
						Mapper045_SetBank_CPU_6( mapper, data );
					} else {
						((Mapper045 *)mapper)->prg0 = data & 0x3F;
						Mapper045_SetBank_CPU_4( mapper, data );
					}
					break;
				case	0x07:
					((Mapper045 *)mapper)->prg1 = data & 0x3F;
					Mapper045_SetBank_CPU_5( mapper, data );
					break;
			}
			break;
		case	0xA000:
			if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;
		case	0xC000:
			if( ((Mapper045 *)mapper)->patch == 2 ) {
			      if( data == 0x29 || data == 0x70 )
					data = 0x07;
			}
			((Mapper045 *)mapper)->irq_latch = data;
			((Mapper045 *)mapper)->irq_latched = 1;
			if( ((Mapper045 *)mapper)->irq_reset ) {
				((Mapper045 *)mapper)->irq_counter = data;
				((Mapper045 *)mapper)->irq_latched = 0;
			}
//			((Mapper045 *)mapper)->irq_counter = data;
			break;
		case	0xC001:
//			((Mapper045 *)mapper)->irq_latch = data;
			((Mapper045 *)mapper)->irq_counter = ((Mapper045 *)mapper)->irq_latch;
			break;
		case	0xE000:
			((Mapper045 *)mapper)->irq_enable = 0;
			((Mapper045 *)mapper)->irq_reset = 1;
			CPU_ClrIRQ( ((Mapper045 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper045 *)mapper)->irq_enable = 1;
			if( ((Mapper045 *)mapper)->irq_latched ) {
				((Mapper045 *)mapper)->irq_counter = ((Mapper045 *)mapper)->irq_latch;
			}
			break;
	}
}

void	Mapper045_HSync( Mapper *mapper, INT scanline )
{
	((Mapper045 *)mapper)->irq_reset = 0;
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) && PPU_IsDispON(((Mapper045 *)mapper)->nes->ppu) ) {
		if( ((Mapper045 *)mapper)->irq_counter ) {
			((Mapper045 *)mapper)->irq_counter--;
			if( ((Mapper045 *)mapper)->irq_counter == 0 ) {
				if( ((Mapper045 *)mapper)->irq_enable ) {
					CPU_SetIRQ( ((Mapper045 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper045_SetBank_CPU_4( Mapper *mapper, INT data )
{
	data &= (((Mapper045 *)mapper)->reg[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= ((Mapper045 *)mapper)->reg[1];
	SetPROM_8K_Bank( 4, data );
	((Mapper045 *)mapper)->p[0] = data;
}

void	Mapper045_SetBank_CPU_5( Mapper *mapper, INT data )
{
	data &= (((Mapper045 *)mapper)->reg[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= ((Mapper045 *)mapper)->reg[1];
	SetPROM_8K_Bank( 5, data );
	((Mapper045 *)mapper)->p[1] = data;
}

void	Mapper045_SetBank_CPU_6( Mapper *mapper, INT data )
{
	data &= (((Mapper045 *)mapper)->reg[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= ((Mapper045 *)mapper)->reg[1];
	SetPROM_8K_Bank( 6, data );
	((Mapper045 *)mapper)->p[2] = data;
}

void	Mapper045_SetBank_CPU_7( Mapper *mapper, INT data )
{
	data &= (((Mapper045 *)mapper)->reg[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= ((Mapper045 *)mapper)->reg[1];
	SetPROM_8K_Bank( 7, data );
	((Mapper045 *)mapper)->p[3] = data;
}

void	Mapper045_SetBank_PPU(Mapper *mapper)
{
	INT i;
	BYTE	table[16] = {
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF
	};

	((Mapper045 *)mapper)->c[0] = ((Mapper045 *)mapper)->chr0;
	((Mapper045 *)mapper)->c[1] = ((Mapper045 *)mapper)->chr1;
	((Mapper045 *)mapper)->c[2] = ((Mapper045 *)mapper)->chr2;
	((Mapper045 *)mapper)->c[3] = ((Mapper045 *)mapper)->chr3;
	((Mapper045 *)mapper)->c[4] = ((Mapper045 *)mapper)->chr4;
	((Mapper045 *)mapper)->c[5] = ((Mapper045 *)mapper)->chr5;
	((Mapper045 *)mapper)->c[6] = ((Mapper045 *)mapper)->chr6;
	((Mapper045 *)mapper)->c[7] = ((Mapper045 *)mapper)->chr7;

	for( i = 0; i < 8; i++ ) {
		((Mapper045 *)mapper)->c[i] &= table[((Mapper045 *)mapper)->reg[2] & 0x0F];
		((Mapper045 *)mapper)->c[i] |= ((Mapper045 *)mapper)->reg[0] & ((((Mapper045 *)mapper)->patch!=1)?0xFF:0xC0);
		((Mapper045 *)mapper)->c[i] += (((Mapper045 *)mapper)->reg[2] & ((((Mapper045 *)mapper)->patch!=1)?0x10:0x30))<<4;
	}

	if( ((Mapper045 *)mapper)->reg[6] & 0x80 ) {
		SetVROM_8K_Bank( ((Mapper045 *)mapper)->c[4], ((Mapper045 *)mapper)->c[5], ((Mapper045 *)mapper)->c[6], ((Mapper045 *)mapper)->c[7], ((Mapper045 *)mapper)->c[0], ((Mapper045 *)mapper)->c[1], ((Mapper045 *)mapper)->c[2], ((Mapper045 *)mapper)->c[3] );
	} else {
		SetVROM_8K_Bank( ((Mapper045 *)mapper)->c[0], ((Mapper045 *)mapper)->c[1], ((Mapper045 *)mapper)->c[2], ((Mapper045 *)mapper)->c[3], ((Mapper045 *)mapper)->c[4], ((Mapper045 *)mapper)->c[5], ((Mapper045 *)mapper)->c[6], ((Mapper045 *)mapper)->c[7] );
	}
}

void	Mapper045_SaveState( Mapper *mapper, LPBYTE ps )
{
	INT	i;
	for( i = 0; i < 8; i++ ) {
		ps[i] = ((Mapper045 *)mapper)->reg[i];
	}
	for( i = 0; i < 4; i++ ) {
		ps[i+8] = ((Mapper045 *)mapper)->p[i];
	}
	for( i = 0; i < 8; i++ ) {
		*(INT*)&ps[i*4+64] = ((Mapper045 *)mapper)->c[i];
	}
	ps[20] = ((Mapper045 *)mapper)->prg0;
	ps[21] = ((Mapper045 *)mapper)->prg1;
	ps[22] = ((Mapper045 *)mapper)->prg2;
	ps[23] = ((Mapper045 *)mapper)->prg3;
	ps[24] = ((Mapper045 *)mapper)->chr0;
	ps[25] = ((Mapper045 *)mapper)->chr1;
	ps[26] = ((Mapper045 *)mapper)->chr2;
	ps[27] = ((Mapper045 *)mapper)->chr3;
	ps[28] = ((Mapper045 *)mapper)->chr4;
	ps[29] = ((Mapper045 *)mapper)->chr5;
	ps[30] = ((Mapper045 *)mapper)->chr6;
	ps[31] = ((Mapper045 *)mapper)->chr7;
	ps[32] = ((Mapper045 *)mapper)->irq_enable;
	ps[33] = ((Mapper045 *)mapper)->irq_counter;
	ps[34] = ((Mapper045 *)mapper)->irq_latch;
}

void	Mapper045_LoadState( Mapper *mapper, LPBYTE ps )
{
	INT	i;
	for( i = 0; i < 8; i++ ) {
		((Mapper045 *)mapper)->reg[i] = ps[i];
	}
	for( i = 0; i < 4; i++ ) {
		((Mapper045 *)mapper)->p[i] = ps[i+8];
	}
	for( i = 0; i < 8; i++ ) {
		((Mapper045 *)mapper)->c[i] = *(INT*)&ps[i*4+64];
	}
	((Mapper045 *)mapper)->prg0 = ps[20];
	((Mapper045 *)mapper)->prg1 = ps[21];
	((Mapper045 *)mapper)->prg2 = ps[22];
	((Mapper045 *)mapper)->prg3 = ps[23];
	((Mapper045 *)mapper)->chr0 = ps[24];
	((Mapper045 *)mapper)->chr1 = ps[25];
	((Mapper045 *)mapper)->chr2 = ps[26];
	((Mapper045 *)mapper)->chr3 = ps[27];
	((Mapper045 *)mapper)->chr4 = ps[28];
	((Mapper045 *)mapper)->chr5 = ps[29];
	((Mapper045 *)mapper)->chr6 = ps[30];
	((Mapper045 *)mapper)->chr7 = ps[31];
	((Mapper045 *)mapper)->irq_enable  = ps[32];
	((Mapper045 *)mapper)->irq_counter = ps[33];
	((Mapper045 *)mapper)->irq_latch   = ps[34];
}

BOOL	Mapper045_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper045_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper045));
	
	memset(mapper, 0, sizeof(Mapper045));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper045_Reset;
	mapper->vtbl.Write = Mapper045_Write;
	mapper->vtbl.WriteLow = Mapper045_WriteLow;
	mapper->vtbl.HSync = Mapper045_HSync;
	mapper->vtbl.SaveState = Mapper045_SaveState;
	mapper->vtbl.LoadState = Mapper045_LoadState;
	mapper->vtbl.IsStateSave = Mapper045_IsStateSave;

	return (Mapper *)mapper;
}

