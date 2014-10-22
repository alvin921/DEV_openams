//////////////////////////////////////////////////////////////////////////
// Mapper249  MMC3                                                      //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	spdata;
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;

}Mapper249;

void	Mapper249_Reset(Mapper *mapper)
{
	INT i;

 	for(  i = 0; i < 8; i++ ) {
		((Mapper249 *)mapper)->reg[i] = 0x00;
	}
	((Mapper249 *)mapper)->prg0 = 0;
	((Mapper249 *)mapper)->prg1 = 1;

	SetPROM_32K_Bank( 0,1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	((Mapper249 *)mapper)->chr01 = 0;
	((Mapper249 *)mapper)->chr23 = 2;
	((Mapper249 *)mapper)->chr4  = 4;
	((Mapper249 *)mapper)->chr5  = 5;
	((Mapper249 *)mapper)->chr6  = 6;
	((Mapper249 *)mapper)->chr7  = 7;

	SetVROM_8K_Bank_cont( 0 );

	((Mapper249 *)mapper)->we_sram  = 0;	// Disable
	((Mapper249 *)mapper)->irq_enable = 0;	// Disable
	((Mapper249 *)mapper)->irq_counter = 0;
	((Mapper249 *)mapper)->irq_latch = 0;
	((Mapper249 *)mapper)->irq_request = 0;

	// IRQƒ^ƒCƒvÝ’è
	NES_SetIrqType( mapper->nes, IRQ_CLOCK );

	((Mapper249 *)mapper)->spdata = 0;
}

void	Mapper249_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x5000 ) {
		switch( data ) {
			case	0x00:
				((Mapper249 *)mapper)->spdata = 0;
				break;
			case	0x02:
				((Mapper249 *)mapper)->spdata = 1;
				break;
		}
	}

	if( addr>=0x6000 && addr<0x8000 ) {
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}
}

void	Mapper249_Write( Mapper *mapper, WORD addr, BYTE data )
{
	BYTE	m0,m1,m2,m3,m4,m5,m6,m7;

	switch( addr&0xFF01 ) {
		case	0x8000:
		case    0x8800:
			((Mapper249 *)mapper)->reg[0] = data;
			break;
		case	0x8001:
		case    0x8801:
			switch( ((Mapper249 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					if( ((Mapper249 *)mapper)->spdata == 1 ) {
						m0=data&0x1;
						m1=(data&0x02)>>1;
						m2=(data&0x04)>>2;
						m3=(data&0x08)>>3;
						m4=(data&0x10)>>4;
						m5=(data&0x20)>>5;
						m6=(data&0x40)>>6;
						m7=(data&0x80)>>7;
						data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 0, data&0xFE );
					SetVROM_1K_Bank( 1, data|0x01 );
					break;
				case	0x01:
					if( ((Mapper249 *)mapper)->spdata == 1 ) {
						m0=data&0x1;
						m1=(data&0x02)>>1;
						m2=(data&0x04)>>2;
						m3=(data&0x08)>>3;
						m4=(data&0x10)>>4;
						m5=(data&0x20)>>5;
						m6=(data&0x40)>>6;
						m7=(data&0x80)>>7;
						data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 2, data&0xFE );
					SetVROM_1K_Bank( 3, data|0x01 );
					break;
				case	0x02:
					if( ((Mapper249 *)mapper)->spdata == 1 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 4, data );
					break;
				case	0x03:
					if( ((Mapper249 *)mapper)->spdata == 1 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 5, data );
					break;
				case	0x04:
					if( ((Mapper249 *)mapper)->spdata == 1 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 6, data );
					break;
				case	0x05:
					if( ((Mapper249 *)mapper)->spdata == 1 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 7, data );
					break;
				case	0x06:
					if( ((Mapper249 *)mapper)->spdata == 1 ) {
						if( data < 0x20 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=0;
							m6=0;
							m7=0;
							data=(m7<<7)|(m6<<6)|(m5<<5)|(m2<<4)|(m1<<3)|(m3<<2)|(m4<<1)|m0;
						} else {
							data=data-0x20;
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
						}
					}
					SetPROM_8K_Bank( 4, data );
					break;
				case	0x07:
					if( ((Mapper249 *)mapper)->spdata == 1 ) {
						if( data < 0x20 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=0;
							m6=0;
							m7=0;
							data=(m7<<7)|(m6<<6)|(m5<<5)|(m2<<4)|(m1<<3)|(m3<<2)|(m4<<1)|m0;
						} else {
							data=data-0x20;
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
						}
					}
					SetPROM_8K_Bank( 5, data );
					break;
			}
			break;
		case	0xA000:
		case    0xA800:
			((Mapper249 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(((Mapper249 *)mapper)->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
		case    0xA801:
			((Mapper249 *)mapper)->reg[3] = data;
			break;
		case	0xC000:
		case    0xC800:
			((Mapper249 *)mapper)->reg[4] = data;
			((Mapper249 *)mapper)->irq_counter = data;
			((Mapper249 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( ((Mapper249 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xC001:
		case    0xC801:
			((Mapper249 *)mapper)->reg[5] = data;
			((Mapper249 *)mapper)->irq_latch = data;
			((Mapper249 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( ((Mapper249 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE000:
		case    0xE800:
			((Mapper249 *)mapper)->reg[6] = data;
			((Mapper249 *)mapper)->irq_enable = 0;
			((Mapper249 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( ((Mapper249 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
		case    0xE801:
			((Mapper249 *)mapper)->reg[7] = data;
			((Mapper249 *)mapper)->irq_enable = 1;
			((Mapper249 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( ((Mapper249 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper249_Clock( Mapper *mapper, INT cycles )
{
//	if( ((Mapper249 *)mapper)->irq_request && (((Mapper249 *)mapper)->nes->GetIrqType() == IRQ_CLOCK) ) {
//		((Mapper249 *)mapper)->nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper249_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline <= SCREEN_HEIGHT-1) ) {
		if( PPU_IsDispON(((Mapper249 *)mapper)->nes->ppu) ) {
			if( ((Mapper249 *)mapper)->irq_enable && !((Mapper249 *)mapper)->irq_request ) {
				if( scanline == 0 ) {
					if( ((Mapper249 *)mapper)->irq_counter ) {
						((Mapper249 *)mapper)->irq_counter--;
					}
				}
				if( !(((Mapper249 *)mapper)->irq_counter--) ) {
					((Mapper249 *)mapper)->irq_request = 0xFF;
					((Mapper249 *)mapper)->irq_counter = ((Mapper249 *)mapper)->irq_latch;
					CPU_SetIRQ( ((Mapper249 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
//	if( ((Mapper249 *)mapper)->irq_request && (((Mapper249 *)mapper)->nes->GetIrqType() == IRQ_HSYNC) ) {
//		((Mapper249 *)mapper)->nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper249_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

 	for(  i = 0; i < 8; i++ ) {
		p[i] = ((Mapper249 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper249 *)mapper)->prg0;
	p[ 9] = ((Mapper249 *)mapper)->prg1;
	p[10] = ((Mapper249 *)mapper)->chr01;
	p[11] = ((Mapper249 *)mapper)->chr23;
	p[12] = ((Mapper249 *)mapper)->chr4;
	p[13] = ((Mapper249 *)mapper)->chr5;
	p[14] = ((Mapper249 *)mapper)->chr6;
	p[15] = ((Mapper249 *)mapper)->chr7;
	p[16] = ((Mapper249 *)mapper)->irq_enable;
	p[17] = ((Mapper249 *)mapper)->irq_counter;
	p[18] = ((Mapper249 *)mapper)->irq_latch;
	p[19] = ((Mapper249 *)mapper)->irq_request;
	p[20] = ((Mapper249 *)mapper)->spdata;
}

void	Mapper249_LoadState(Mapper *mapper, LPBYTE p )
{
	INT i;

 	for(  i = 0; i < 8; i++ ) {
		((Mapper249 *)mapper)->reg[i] = p[i];
	}
	((Mapper249 *)mapper)->prg0  = p[ 8];
	((Mapper249 *)mapper)->prg1  = p[ 9];
	((Mapper249 *)mapper)->chr01 = p[10];
	((Mapper249 *)mapper)->chr23 = p[11];
	((Mapper249 *)mapper)->chr4  = p[12];
	((Mapper249 *)mapper)->chr5  = p[13];
	((Mapper249 *)mapper)->chr6  = p[14];
	((Mapper249 *)mapper)->chr7  = p[15];
	((Mapper249 *)mapper)->irq_enable  = p[16];
	((Mapper249 *)mapper)->irq_counter = p[17];
	((Mapper249 *)mapper)->irq_latch   = p[18];
	((Mapper249 *)mapper)->irq_request = p[19];
	((Mapper249 *)mapper)->spdata = p[20];
}

BOOL	Mapper249_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper249_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper249));
	
	memset(mapper, 0, sizeof(Mapper249));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper249_Reset;
	mapper->vtbl.Write = Mapper249_Write;
	mapper->vtbl.WriteLow = Mapper249_WriteLow;
	mapper->vtbl.Clock = Mapper249_Clock;
	mapper->vtbl.HSync = Mapper249_HSync;
	mapper->vtbl.SaveState = Mapper249_SaveState;
	mapper->vtbl.LoadState = Mapper249_LoadState;
	mapper->vtbl.IsStateSave = Mapper249_IsStateSave;

	return (Mapper *)mapper;
}


