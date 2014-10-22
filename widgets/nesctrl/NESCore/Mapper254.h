//////////////////////////////////////////////////////////////////////////
// Mapper254  Pokemon Pirate Cart                                       //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;
	BYTE	protectflag;
}Mapper254;
void	Mapper254_SetBank_CPU(Mapper *mapper);
void	Mapper254_SetBank_PPU(Mapper *mapper);

void	Mapper254_Reset(Mapper *mapper)
{
	INT i;

 	for(  i = 0; i < 8; i++ ) {
		((Mapper254 *)mapper)->reg[i] = 0x00;
	}

	((Mapper254 *)mapper)->protectflag = 0;

	((Mapper254 *)mapper)->prg0 = 0;
	((Mapper254 *)mapper)->prg1 = 1;
	Mapper254_SetBank_CPU(mapper);

	((Mapper254 *)mapper)->chr01 = 0;
	((Mapper254 *)mapper)->chr23 = 2;
	((Mapper254 *)mapper)->chr4  = 4;
	((Mapper254 *)mapper)->chr5  = 5;
	((Mapper254 *)mapper)->chr6  = 6;
	((Mapper254 *)mapper)->chr7  = 7;
	Mapper254_SetBank_PPU(mapper);

	((Mapper254 *)mapper)->irq_enable = 0;	// Disable
	((Mapper254 *)mapper)->irq_counter = 0;
	((Mapper254 *)mapper)->irq_latch = 0;
	((Mapper254 *)mapper)->irq_request = 0;
}

void	Mapper254_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF000 ) {
		case	0x6000:
		case	0x7000:
			CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
			break;
	}

}
BYTE	Mapper254_ReadLow( Mapper *mapper, WORD addr )
{
	if( addr>=0x6000 ) {
		if( ((Mapper254 *)mapper)->protectflag ) {
			return	(CPU_MEM_BANK[addr>>13][addr&0x1FFF]);
		} else {
			return	((CPU_MEM_BANK[addr>>13][addr&0x1FFF])^0x1);
		}
	}
	return	Mapper_DefReadLow(mapper,  addr );
}

void	Mapper254_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper254 *)mapper)->protectflag=0xFF;
			((Mapper254 *)mapper)->reg[0] = data;
			Mapper254_SetBank_CPU(mapper);
			Mapper254_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper254 *)mapper)->reg[1] = data;

			switch( ((Mapper254 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper254 *)mapper)->chr01 = data & 0xFE;
					Mapper254_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper254 *)mapper)->chr23 = data & 0xFE;
					Mapper254_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper254 *)mapper)->chr4 = data;
					Mapper254_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper254 *)mapper)->chr5 = data;
					Mapper254_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper254 *)mapper)->chr6 = data;
					Mapper254_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper254 *)mapper)->chr7 = data;
					Mapper254_SetBank_PPU(mapper);
					break;
				case	0x06:
					((Mapper254 *)mapper)->prg0 = data;
					Mapper254_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper254 *)mapper)->prg1 = data;
					Mapper254_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper254 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(((Mapper254 *)mapper)->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper254 *)mapper)->reg[3] = data;
			break;
		case	0xC000:
			((Mapper254 *)mapper)->reg[4] = data;
			((Mapper254 *)mapper)->irq_counter = data;
			((Mapper254 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( ((Mapper254 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xC001:
			((Mapper254 *)mapper)->reg[5] = data;
			((Mapper254 *)mapper)->irq_latch = data;
			((Mapper254 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( ((Mapper254 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE000:
			((Mapper254 *)mapper)->reg[6] = data;
			((Mapper254 *)mapper)->irq_enable = 0;
			((Mapper254 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( ((Mapper254 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper254 *)mapper)->reg[7] = data;
			((Mapper254 *)mapper)->irq_enable = 1;
			((Mapper254 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( ((Mapper254 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper254_Clock( Mapper *mapper, INT cycles )
{
//	if( ((Mapper254 *)mapper)->irq_request && (((Mapper254 *)mapper)->nes->GetIrqType() == IRQ_CLOCK) ) {
//		((Mapper254 *)mapper)->nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper254_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline <= SCREEN_HEIGHT-1) ) {
		if(PPU_IsDispON( ((Mapper254 *)mapper)->nes->ppu) ) {
			if( ((Mapper254 *)mapper)->irq_enable && !((Mapper254 *)mapper)->irq_request ) {
				if( scanline == 0 ) {
					if( ((Mapper254 *)mapper)->irq_counter ) {
						((Mapper254 *)mapper)->irq_counter--;
					}
				}
				if( !(((Mapper254 *)mapper)->irq_counter--) ) {
					((Mapper254 *)mapper)->irq_request = 0xFF;
					((Mapper254 *)mapper)->irq_counter = ((Mapper254 *)mapper)->irq_latch;
					CPU_SetIRQ( ((Mapper254 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
//	if( ((Mapper254 *)mapper)->irq_request && (((Mapper254 *)mapper)->nes->GetIrqType() == IRQ_HSYNC) ) {
//		((Mapper254 *)mapper)->nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper254_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper254 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, ((Mapper254 *)mapper)->prg1, ((Mapper254 *)mapper)->prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( ((Mapper254 *)mapper)->prg0, ((Mapper254 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper254_SetBank_PPU(Mapper *mapper)
{

	if( VROM_1K_SIZE ) {
		if( ((Mapper254 *)mapper)->reg[0] & 0x80 ) {
			SetVROM_8K_Bank( ((Mapper254 *)mapper)->chr4, ((Mapper254 *)mapper)->chr5, ((Mapper254 *)mapper)->chr6, ((Mapper254 *)mapper)->chr7,
					 ((Mapper254 *)mapper)->chr01, ((Mapper254 *)mapper)->chr01+1, ((Mapper254 *)mapper)->chr23, ((Mapper254 *)mapper)->chr23+1 );
		} else {
			SetVROM_8K_Bank( ((Mapper254 *)mapper)->chr01, ((Mapper254 *)mapper)->chr01+1, ((Mapper254 *)mapper)->chr23, ((Mapper254 *)mapper)->chr23+1,
					 ((Mapper254 *)mapper)->chr4, ((Mapper254 *)mapper)->chr5, ((Mapper254 *)mapper)->chr6, ((Mapper254 *)mapper)->chr7 );
		}
	} else {
		if( ((Mapper254 *)mapper)->reg[0] & 0x80 ) {
			SetCRAM_1K_Bank( 4, (((Mapper254 *)mapper)->chr01+0)&0x07 );
			SetCRAM_1K_Bank( 5, (((Mapper254 *)mapper)->chr01+1)&0x07 );
			SetCRAM_1K_Bank( 6, (((Mapper254 *)mapper)->chr23+0)&0x07 );
			SetCRAM_1K_Bank( 7, (((Mapper254 *)mapper)->chr23+1)&0x07 );
			SetCRAM_1K_Bank( 0, ((Mapper254 *)mapper)->chr4&0x07 );
			SetCRAM_1K_Bank( 1, ((Mapper254 *)mapper)->chr5&0x07 );
			SetCRAM_1K_Bank( 2, ((Mapper254 *)mapper)->chr6&0x07 );
			SetCRAM_1K_Bank( 3, ((Mapper254 *)mapper)->chr7&0x07 );
		} else {
			SetCRAM_1K_Bank( 0, (((Mapper254 *)mapper)->chr01+0)&0x07 );
			SetCRAM_1K_Bank( 1, (((Mapper254 *)mapper)->chr01+1)&0x07 );
			SetCRAM_1K_Bank( 2, (((Mapper254 *)mapper)->chr23+0)&0x07 );
			SetCRAM_1K_Bank( 3, (((Mapper254 *)mapper)->chr23+1)&0x07 );
			SetCRAM_1K_Bank( 4, ((Mapper254 *)mapper)->chr4&0x07 );
			SetCRAM_1K_Bank( 5, ((Mapper254 *)mapper)->chr5&0x07 );
			SetCRAM_1K_Bank( 6, ((Mapper254 *)mapper)->chr6&0x07 );
			SetCRAM_1K_Bank( 7, ((Mapper254 *)mapper)->chr7&0x07 );
		}
	}
}

void	Mapper254_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

 	for(  i = 0; i < 8; i++ ) {
		p[i] = ((Mapper254 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper254 *)mapper)->prg0;
	p[ 9] = ((Mapper254 *)mapper)->prg1;
	p[10] = ((Mapper254 *)mapper)->chr01;
	p[11] = ((Mapper254 *)mapper)->chr23;
	p[12] = ((Mapper254 *)mapper)->chr4;
	p[13] = ((Mapper254 *)mapper)->chr5;
	p[14] = ((Mapper254 *)mapper)->chr6;
	p[15] = ((Mapper254 *)mapper)->chr7;
	p[16] = ((Mapper254 *)mapper)->irq_enable;
	p[17] = ((Mapper254 *)mapper)->irq_counter;
	p[18] = ((Mapper254 *)mapper)->irq_latch;
	p[19] = ((Mapper254 *)mapper)->irq_request;
	p[20] = ((Mapper254 *)mapper)->protectflag;
}

void	Mapper254_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

 	for(  i = 0; i < 8; i++ ) {
		((Mapper254 *)mapper)->reg[i] = p[i];
	}
	((Mapper254 *)mapper)->prg0  = p[ 8];
	((Mapper254 *)mapper)->prg1  = p[ 9];
	((Mapper254 *)mapper)->chr01 = p[10];
	((Mapper254 *)mapper)->chr23 = p[11];
	((Mapper254 *)mapper)->chr4  = p[12];
	((Mapper254 *)mapper)->chr5  = p[13];
	((Mapper254 *)mapper)->chr6  = p[14];
	((Mapper254 *)mapper)->chr7  = p[15];
	((Mapper254 *)mapper)->irq_enable  = p[16];
	((Mapper254 *)mapper)->irq_counter = p[17];
	((Mapper254 *)mapper)->irq_latch   = p[18];
	((Mapper254 *)mapper)->irq_request = p[19];
	((Mapper254 *)mapper)->protectflag = p[20];
}

BOOL	Mapper254_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper254_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper254));
	
	memset(mapper, 0, sizeof(Mapper254));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper254_Reset;
	mapper->vtbl.Write = Mapper254_Write;
	mapper->vtbl.ReadLow = Mapper254_ReadLow;
	mapper->vtbl.WriteLow = Mapper254_WriteLow;
	mapper->vtbl.Clock = Mapper254_Clock;
	mapper->vtbl.HSync = Mapper254_HSync;
	mapper->vtbl.SaveState = Mapper254_SaveState;
	mapper->vtbl.LoadState = Mapper254_LoadState;
	mapper->vtbl.IsStateSave = Mapper254_IsStateSave;

	return (Mapper *)mapper;
}


