//////////////////////////////////////////////////////////////////////////
// Mapper115 CartSaint : Yuu Yuu Hakusho Final                          //
//           JusticePao(?)                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	prg0, prg1, prg2, prg3;
	BYTE	prg0L, prg1L;
	BYTE	chr0, chr1, chr2, chr3, chr4, chr5, chr6, chr7;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;

	BYTE	ExPrgSwitch;
	BYTE	ExChrSwitch;
}Mapper115;
void	Mapper115_SetBank_CPU(Mapper *mapper);
void	Mapper115_SetBank_PPU(Mapper *mapper );


void	Mapper115_Reset(Mapper *mapper)
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper115 *)mapper)->reg[i] = 0x00;
	}

	((Mapper115 *)mapper)->prg0 = ((Mapper115 *)mapper)->prg0L = 0;
	((Mapper115 *)mapper)->prg1 = ((Mapper115 *)mapper)->prg1L = 1;
	((Mapper115 *)mapper)->prg2 = PROM_8K_SIZE-2;
	((Mapper115 *)mapper)->prg3 = PROM_8K_SIZE-1;

	((Mapper115 *)mapper)->ExPrgSwitch = 0;
	((Mapper115 *)mapper)->ExChrSwitch = 0;

	Mapper115_SetBank_CPU(mapper);

	if( VROM_1K_SIZE ) {
		((Mapper115 *)mapper)->chr0 = 0;
		((Mapper115 *)mapper)->chr1 = 1;
		((Mapper115 *)mapper)->chr2 = 2;
		((Mapper115 *)mapper)->chr3 = 3;
		((Mapper115 *)mapper)->chr4 = 4;
		((Mapper115 *)mapper)->chr5 = 5;
		((Mapper115 *)mapper)->chr6 = 6;
		((Mapper115 *)mapper)->chr7 = 7;
		Mapper115_SetBank_PPU(mapper);
	} else {
		((Mapper115 *)mapper)->chr0 = ((Mapper115 *)mapper)->chr2 = ((Mapper115 *)mapper)->chr4 = ((Mapper115 *)mapper)->chr5 = ((Mapper115 *)mapper)->chr6 = ((Mapper115 *)mapper)->chr7 = 0;
		((Mapper115 *)mapper)->chr1 = ((Mapper115 *)mapper)->chr3 = 1;
	}

	((Mapper115 *)mapper)->irq_enable = 0;	// Disable
	((Mapper115 *)mapper)->irq_counter = 0;
	((Mapper115 *)mapper)->irq_latch = 0;
}


void	Mapper115_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch ( addr ){
		case	0x6000:
			((Mapper115 *)mapper)->ExPrgSwitch = data; //data
			Mapper115_SetBank_CPU(mapper);
			break;
		case	0x6001:
			((Mapper115 *)mapper)->ExChrSwitch = data&0x1;
			Mapper115_SetBank_PPU(mapper);
			break;
	}
	Mapper_DefWriteLow( mapper, addr, data );
}


void	Mapper115_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper115 *)mapper)->reg[0] = data;
			Mapper115_SetBank_CPU(mapper);
			Mapper115_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper115 *)mapper)->reg[1] = data;
			switch( ((Mapper115 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper115 *)mapper)->chr0 = data & 0xFE;
					((Mapper115 *)mapper)->chr1 = ((Mapper115 *)mapper)->chr0+1;
					Mapper115_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper115 *)mapper)->chr2 = data & 0xFE;
					((Mapper115 *)mapper)->chr3 = ((Mapper115 *)mapper)->chr2+1;
					Mapper115_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper115 *)mapper)->chr4 = data;
					Mapper115_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper115 *)mapper)->chr5 = data;
					Mapper115_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper115 *)mapper)->chr6 = data;
					Mapper115_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper115 *)mapper)->chr7 = data;
					Mapper115_SetBank_PPU(mapper);
					break;
				case	0x06:
					((Mapper115 *)mapper)->prg0 = ((Mapper115 *)mapper)->prg0L = data;
					Mapper115_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper115 *)mapper)->prg1 = ((Mapper115 *)mapper)->prg1L = data;
					Mapper115_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper115 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(((Mapper115 *)mapper)->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper115 *)mapper)->reg[3] = data;
			break;
		case	0xC000:
			((Mapper115 *)mapper)->reg[4] = data;
			((Mapper115 *)mapper)->irq_counter = data;
			((Mapper115 *)mapper)->irq_enable = 0xFF;
			break;
		case	0xC001:
			((Mapper115 *)mapper)->reg[5] = data;
			((Mapper115 *)mapper)->irq_latch = data;
			break;
		case	0xE000:
			((Mapper115 *)mapper)->reg[6] = data;
			((Mapper115 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( ((Mapper115 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper115 *)mapper)->reg[7] = data;
			((Mapper115 *)mapper)->irq_enable = 0xFF;
			break;
	}
}

void	Mapper115_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(((Mapper115 *)mapper)->nes->ppu) ) {
			if( ((Mapper115 *)mapper)->irq_enable ) {
				if( !(((Mapper115 *)mapper)->irq_counter--) ) {
					((Mapper115 *)mapper)->irq_counter = ((Mapper115 *)mapper)->irq_latch;
//					CPU_IRQ_NotPending(((Mapper115 *)mapper)->nes->cpu);
					CPU_SetIRQ( ((Mapper115 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}


void	Mapper115_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper115 *)mapper)->ExPrgSwitch & 0x80 ) {
		((Mapper115 *)mapper)->prg0 = ((((Mapper115 *)mapper)->ExPrgSwitch<<1)&0x1e);
		((Mapper115 *)mapper)->prg1 = ((Mapper115 *)mapper)->prg0+1;

		SetPROM_32K_Bank( ((Mapper115 *)mapper)->prg0, ((Mapper115 *)mapper)->prg1, ((Mapper115 *)mapper)->prg0+2, ((Mapper115 *)mapper)->prg1+2);
	} else {
		((Mapper115 *)mapper)->prg0 = ((Mapper115 *)mapper)->prg0L;
		((Mapper115 *)mapper)->prg1 = ((Mapper115 *)mapper)->prg1L;
		if( ((Mapper115 *)mapper)->reg[0] & 0x40 ) {
			SetPROM_32K_Bank( PROM_8K_SIZE-2, ((Mapper115 *)mapper)->prg1, ((Mapper115 *)mapper)->prg0, PROM_8K_SIZE-1 );
		} else {
			SetPROM_32K_Bank( ((Mapper115 *)mapper)->prg0, ((Mapper115 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
		}
	}
}

void	Mapper115_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		if( ((Mapper115 *)mapper)->reg[0] & 0x80 ) {
			SetVROM_8K_Bank( (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr4, (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr5,
					 (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr6, (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr7,
					 (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr0, (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr1,
					 (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr2, (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr3 );
		} else {
			SetVROM_8K_Bank( (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr0, (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr1,
					 (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr2, (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr3,
			 		 (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr4, (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr5,
			 		 (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr6, (((Mapper115 *)mapper)->ExChrSwitch<<8)+((Mapper115 *)mapper)->chr7 );
		}
	}
}

void	Mapper115_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper115 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper115 *)mapper)->prg0;
	p[ 9] = ((Mapper115 *)mapper)->prg1;
	p[10] = ((Mapper115 *)mapper)->prg2;
	p[11] = ((Mapper115 *)mapper)->prg3;
	p[12] = ((Mapper115 *)mapper)->chr0;
	p[13] = ((Mapper115 *)mapper)->chr1;
	p[14] = ((Mapper115 *)mapper)->chr2;
	p[15] = ((Mapper115 *)mapper)->chr3;
	p[16] = ((Mapper115 *)mapper)->chr4;
	p[17] = ((Mapper115 *)mapper)->chr5;
	p[18] = ((Mapper115 *)mapper)->chr6;
	p[19] = ((Mapper115 *)mapper)->chr7;
	p[20] = ((Mapper115 *)mapper)->irq_enable;
	p[21] = ((Mapper115 *)mapper)->irq_counter;
	p[22] = ((Mapper115 *)mapper)->irq_latch;
	p[23] = ((Mapper115 *)mapper)->ExPrgSwitch;
	p[24] = ((Mapper115 *)mapper)->prg0L;
	p[25] = ((Mapper115 *)mapper)->prg1L;
	p[26] = ((Mapper115 *)mapper)->ExChrSwitch;

}

void	Mapper115_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper115 *)mapper)->reg[i] = p[i];
	}
	((Mapper115 *)mapper)->prg0  = p[ 8];
	((Mapper115 *)mapper)->prg1  = p[ 9];
	((Mapper115 *)mapper)->prg2  = p[10];
	((Mapper115 *)mapper)->prg3  = p[11];
	((Mapper115 *)mapper)->chr0  = p[12];
	((Mapper115 *)mapper)->chr1  = p[13];
	((Mapper115 *)mapper)->chr2  = p[14];
	((Mapper115 *)mapper)->chr3  = p[15];
	((Mapper115 *)mapper)->chr4  = p[16];
	((Mapper115 *)mapper)->chr5  = p[17];
	((Mapper115 *)mapper)->chr6  = p[18];
	((Mapper115 *)mapper)->chr7  = p[19];
	((Mapper115 *)mapper)->irq_enable  = p[20];
	((Mapper115 *)mapper)->irq_counter = p[21];
	((Mapper115 *)mapper)->irq_latch   = p[22];
	((Mapper115 *)mapper)->ExPrgSwitch = p[23];
	((Mapper115 *)mapper)->prg0L = p[24];
	((Mapper115 *)mapper)->prg1L = p[25];
	((Mapper115 *)mapper)->ExChrSwitch = p[26];
}

BOOL	Mapper115_IsStateSave(Mapper *mapper ) { return TRUE; }
Mapper * Mapper115_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper115));
	
	memset(mapper, 0, sizeof(Mapper115));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper115_Reset;
	mapper->vtbl.Write = Mapper115_Write;
	mapper->vtbl.WriteLow = Mapper115_WriteLow;
	mapper->vtbl.HSync = Mapper115_HSync;
	mapper->vtbl.SaveState = Mapper115_SaveState;
	mapper->vtbl.LoadState = Mapper115_LoadState;
	mapper->vtbl.IsStateSave = Mapper115_IsStateSave;

	return (Mapper *)mapper;
}


