
//////////////////////////////////////////////////////////////////////////
// Mapper245  Yong Zhe Dou E Long                                       //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	int    MMC4prg,MMC4chr;
	
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;

	BYTE	irq_type;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;
}Mapper245;

void	Mapper245_Reset(Mapper *mapper)
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper245 *)mapper)->reg[i] = 0x00;
	}

	((Mapper245 *)mapper)->prg0 = 0;
	((Mapper245 *)mapper)->prg1 = 1;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	((Mapper245 *)mapper)->we_sram  = 0;	// Disable
	((Mapper245 *)mapper)->irq_enable = 0;	// Disable
	((Mapper245 *)mapper)->irq_counter = 0;
	((Mapper245 *)mapper)->irq_latch = 0;
	((Mapper245 *)mapper)->irq_request = 0;

	NES_SetIrqType( mapper->nes, IRQ_CLOCK );
}

void	Mapper245_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr&0xF7FF ) {
		case	0x8000:
			((Mapper245 *)mapper)->reg[0] = data;
			break;
		case	0x8001:
			((Mapper245 *)mapper)->reg[1] = data;
			switch( ((Mapper245 *)mapper)->reg[0] ) {
				case	0x00:
					((Mapper245 *)mapper)->reg[3]=(data & 2 )<<5;
					SetPROM_8K_Bank(6,0x3E | ((Mapper245 *)mapper)->reg[3]);
					SetPROM_8K_Bank(7,0x3F | ((Mapper245 *)mapper)->reg[3]);
					break;
				case	0x06:
					((Mapper245 *)mapper)->prg0=data;
					break;
				case	0x07:
					((Mapper245 *)mapper)->prg1=data;
					break;
			}
			SetPROM_8K_Bank( 4, ((Mapper245 *)mapper)->prg0|((Mapper245 *)mapper)->reg[3] );
			SetPROM_8K_Bank( 5, ((Mapper245 *)mapper)->prg1|((Mapper245 *)mapper)->reg[3] );
			break;
		case	0xA000:
			((Mapper245 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(mapper->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
		    
			break;
		case	0xC000:
			((Mapper245 *)mapper)->reg[4] = data;
			((Mapper245 *)mapper)->irq_counter = data;
			((Mapper245 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0xC001:
			((Mapper245 *)mapper)->reg[5] = data;
			((Mapper245 *)mapper)->irq_latch = data;
			((Mapper245 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE000:
			((Mapper245 *)mapper)->reg[6] = data;
			((Mapper245 *)mapper)->irq_enable = 0;
			((Mapper245 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper245 *)mapper)->reg[7] = data;
			((Mapper245 *)mapper)->irq_enable = 1;
			((Mapper245 *)mapper)->irq_request = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper245_Clock( Mapper *mapper, INT cycles )
{
//	if( ((Mapper245 *)mapper)->irq_request && (mapper->nes->GetIrqType() == IRQ_CLOCK) ) {
//		mapper->nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper245_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline <= SCREEN_HEIGHT-1) ) {
		if( PPU_IsDispON(mapper->nes->ppu) ) {
			if( ((Mapper245 *)mapper)->irq_enable && !((Mapper245 *)mapper)->irq_request ) {
				if( scanline == 0 ) {
					if( ((Mapper245 *)mapper)->irq_counter ) {
						((Mapper245 *)mapper)->irq_counter--;
					}
				}
				if( !(((Mapper245 *)mapper)->irq_counter--) ) {
					((Mapper245 *)mapper)->irq_request = 0xFF;
					((Mapper245 *)mapper)->irq_counter = ((Mapper245 *)mapper)->irq_latch;
					CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
//	if( ((Mapper245 *)mapper)->irq_request && (mapper->nes->GetIrqType() == IRQ_HSYNC) ) {
//		mapper->nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper245_SetBank_CPU(Mapper *mapper)
{
	SetPROM_32K_Bank( ((Mapper245 *)mapper)->prg0, ((Mapper245 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper245_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		if( ((Mapper245 *)mapper)->reg[0] & 0x80 ) {
			SetVROM_8K_Bank( ((Mapper245 *)mapper)->chr4, ((Mapper245 *)mapper)->chr5, ((Mapper245 *)mapper)->chr6, ((Mapper245 *)mapper)->chr7,
					 ((Mapper245 *)mapper)->chr23+1, ((Mapper245 *)mapper)->chr23, ((Mapper245 *)mapper)->chr01+1, ((Mapper245 *)mapper)->chr01 );
		} else {
			SetVROM_8K_Bank( ((Mapper245 *)mapper)->chr01, ((Mapper245 *)mapper)->chr01+1, ((Mapper245 *)mapper)->chr23, ((Mapper245 *)mapper)->chr23+1,
					 ((Mapper245 *)mapper)->chr4, ((Mapper245 *)mapper)->chr5, ((Mapper245 *)mapper)->chr6, ((Mapper245 *)mapper)->chr7 );
		}
	} else {
		if( ((Mapper245 *)mapper)->reg[0] & 0x80 ) {
			SetCRAM_1K_Bank( 4, (((Mapper245 *)mapper)->chr01+0)&0x07 );
			SetCRAM_1K_Bank( 5, (((Mapper245 *)mapper)->chr01+1)&0x07 );
			SetCRAM_1K_Bank( 6, (((Mapper245 *)mapper)->chr23+0)&0x07 );
			SetCRAM_1K_Bank( 7, (((Mapper245 *)mapper)->chr23+1)&0x07 );
			SetCRAM_1K_Bank( 0, ((Mapper245 *)mapper)->chr4&0x07 );
			SetCRAM_1K_Bank( 1, ((Mapper245 *)mapper)->chr5&0x07 );
			SetCRAM_1K_Bank( 2, ((Mapper245 *)mapper)->chr6&0x07 );
			SetCRAM_1K_Bank( 3, ((Mapper245 *)mapper)->chr7&0x07 );
		} else {
			SetCRAM_1K_Bank( 0, (((Mapper245 *)mapper)->chr01+0)&0x07 );
			SetCRAM_1K_Bank( 1, (((Mapper245 *)mapper)->chr01+1)&0x07 );
			SetCRAM_1K_Bank( 2, (((Mapper245 *)mapper)->chr23+0)&0x07 );
			SetCRAM_1K_Bank( 3, (((Mapper245 *)mapper)->chr23+1)&0x07 );
			SetCRAM_1K_Bank( 4, ((Mapper245 *)mapper)->chr4&0x07 );
			SetCRAM_1K_Bank( 5, ((Mapper245 *)mapper)->chr5&0x07 );
			SetCRAM_1K_Bank( 6, ((Mapper245 *)mapper)->chr6&0x07 );
			SetCRAM_1K_Bank( 7, ((Mapper245 *)mapper)->chr7&0x07 );
		}
	}
}

void	Mapper245_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

 	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper245 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper245 *)mapper)->prg0;
	p[ 9] = ((Mapper245 *)mapper)->prg1;
	p[10] = ((Mapper245 *)mapper)->chr01;
	p[11] = ((Mapper245 *)mapper)->chr23;
	p[12] = ((Mapper245 *)mapper)->chr4;
	p[13] = ((Mapper245 *)mapper)->chr5;
	p[14] = ((Mapper245 *)mapper)->chr6;
	p[15] = ((Mapper245 *)mapper)->chr7;
	p[16] = ((Mapper245 *)mapper)->irq_enable;
	p[17] = (BYTE)((Mapper245 *)mapper)->irq_counter;
	p[18] = ((Mapper245 *)mapper)->irq_latch;
	p[19] = ((Mapper245 *)mapper)->irq_request;
}

void	Mapper245_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

 	for(  i = 0; i < 8; i++ ) {
		((Mapper245 *)mapper)->reg[i] = p[i];
	}
	((Mapper245 *)mapper)->prg0  = p[ 8];
	((Mapper245 *)mapper)->prg1  = p[ 9];
	((Mapper245 *)mapper)->chr01 = p[10];
	((Mapper245 *)mapper)->chr23 = p[11];
	((Mapper245 *)mapper)->chr4  = p[12];
	((Mapper245 *)mapper)->chr5  = p[13];
	((Mapper245 *)mapper)->chr6  = p[14];
	((Mapper245 *)mapper)->chr7  = p[15];
	((Mapper245 *)mapper)->irq_enable  = p[16];
	((Mapper245 *)mapper)->irq_counter = (INT)p[17];
	((Mapper245 *)mapper)->irq_latch   = p[18];
	((Mapper245 *)mapper)->irq_request = p[19];
}

BOOL	Mapper245_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper245_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper245));
	
	memset(mapper, 0, sizeof(Mapper245));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper245_Reset;
	mapper->vtbl.Write = Mapper245_Write;
	mapper->vtbl.Clock = Mapper245_Clock;
	mapper->vtbl.HSync = Mapper245_HSync;
	mapper->vtbl.SaveState = Mapper245_SaveState;
	mapper->vtbl.LoadState = Mapper245_LoadState;
	mapper->vtbl.IsStateSave = Mapper245_IsStateSave;

	return (Mapper *)mapper;
}

