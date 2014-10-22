
//////////////////////////////////////////////////////////////////////////
// Mapper119  TQ-ROM                                                    //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	patch;

	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
}Mapper119;

void	Mapper119_SetBank_CPU(Mapper *mapper);
void	Mapper119_SetBank_PPU(Mapper *mapper);

void	Mapper119_Reset(Mapper *mapper)
{
	INT i;

	((Mapper119 *)mapper)->patch = 0;

	for( i = 0; i < 8; i++ ) {
		((Mapper119 *)mapper)->reg[i] = 0x00;
	}

	((Mapper119 *)mapper)->prg0 = 0;
	((Mapper119 *)mapper)->prg1 = 1;
	Mapper119_SetBank_CPU(mapper);

	((Mapper119 *)mapper)->chr01 = 0;
	((Mapper119 *)mapper)->chr23 = 2;
	((Mapper119 *)mapper)->chr4  = 4;
	((Mapper119 *)mapper)->chr5  = 5;
	((Mapper119 *)mapper)->chr6  = 6;
	((Mapper119 *)mapper)->chr7  = 7;
	Mapper119_SetBank_PPU(mapper);

	((Mapper119 *)mapper)->we_sram  = 0;	// Disable
	((Mapper119 *)mapper)->irq_enable = 0;	// Disable
	((Mapper119 *)mapper)->irq_counter = 0;
	((Mapper119 *)mapper)->irq_latch = 0;
}

void	Mapper119_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper119 *)mapper)->reg[0] = data;
			Mapper119_SetBank_CPU(mapper);
			Mapper119_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper119 *)mapper)->reg[1] = data;

			switch( ((Mapper119 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					if( VROM_1K_SIZE ) {
						((Mapper119 *)mapper)->chr01 = data & 0xFE;
						Mapper119_SetBank_PPU(mapper);
					}
					break;
				case	0x01:
					if( VROM_1K_SIZE ) {
						((Mapper119 *)mapper)->chr23 = data & 0xFE;
						Mapper119_SetBank_PPU(mapper);
					}
					break;
				case	0x02:
					if( VROM_1K_SIZE ) {
						((Mapper119 *)mapper)->chr4 = data;
						Mapper119_SetBank_PPU(mapper);
					}
					break;
				case	0x03:
					if( VROM_1K_SIZE ) {
						((Mapper119 *)mapper)->chr5 = data;
						Mapper119_SetBank_PPU(mapper);
					}
					break;
				case	0x04:
					if( VROM_1K_SIZE ) {
						((Mapper119 *)mapper)->chr6 = data;
						Mapper119_SetBank_PPU(mapper);
					}
					break;
				case	0x05:
					if( VROM_1K_SIZE ) {
						((Mapper119 *)mapper)->chr7 = data;
						Mapper119_SetBank_PPU(mapper);
					}
					break;
				case	0x06:
					((Mapper119 *)mapper)->prg0 = data;
					Mapper119_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper119 *)mapper)->prg1 = data;
					Mapper119_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper119 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(((Mapper119 *)mapper)->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper119 *)mapper)->reg[3] = data;
			break;
		case	0xC000:
			((Mapper119 *)mapper)->reg[4] = data;
			((Mapper119 *)mapper)->irq_counter = data;
			break;
		case	0xC001:
			((Mapper119 *)mapper)->reg[5] = data;
			((Mapper119 *)mapper)->irq_latch = data;
			break;
		case	0xE000:
			((Mapper119 *)mapper)->reg[6] = data;
			((Mapper119 *)mapper)->irq_enable = 0;
			((Mapper119 *)mapper)->irq_counter = ((Mapper119 *)mapper)->irq_latch;
			CPU_ClrIRQ( ((Mapper119 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper119 *)mapper)->reg[7] = data;
			((Mapper119 *)mapper)->irq_enable = 1;
			break;
	}
}

void	Mapper119_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(((Mapper119 *)mapper)->nes->ppu) ) {
			if( ((Mapper119 *)mapper)->irq_enable ) {
				if( !(((Mapper119 *)mapper)->irq_counter--) ) {
					((Mapper119 *)mapper)->irq_counter = ((Mapper119 *)mapper)->irq_latch;
//					CPU_IRQ(((Mapper119 *)mapper)->nes->cpu);
					CPU_SetIRQ( ((Mapper119 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper119_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper119 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, ((Mapper119 *)mapper)->prg1, ((Mapper119 *)mapper)->prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( ((Mapper119 *)mapper)->prg0, ((Mapper119 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper119_SetBank_PPU(Mapper *mapper)
{
	if( ((Mapper119 *)mapper)->reg[0]&0x80 ) {
		if( ((Mapper119 *)mapper)->chr4&0x40 )	SetCRAM_1K_Bank( 0, ((Mapper119 *)mapper)->chr4&0x07 );
		else		SetVROM_1K_Bank( 0, ((Mapper119 *)mapper)->chr4 );
		if( ((Mapper119 *)mapper)->chr5&0x40 )	SetCRAM_1K_Bank( 1, ((Mapper119 *)mapper)->chr5&0x07 );
		else		SetVROM_1K_Bank( 1, ((Mapper119 *)mapper)->chr5 );
		if( ((Mapper119 *)mapper)->chr6&0x40 )	SetCRAM_1K_Bank( 2, ((Mapper119 *)mapper)->chr6&0x07 );
		else		SetVROM_1K_Bank( 2, ((Mapper119 *)mapper)->chr6 );
		if( ((Mapper119 *)mapper)->chr7&0x40 )	SetCRAM_1K_Bank( 3, ((Mapper119 *)mapper)->chr7&0x07 );
		else		SetVROM_1K_Bank( 3, ((Mapper119 *)mapper)->chr7 );

		if( (((Mapper119 *)mapper)->chr01+0)&0x40 ) SetCRAM_1K_Bank( 4, (((Mapper119 *)mapper)->chr01+0)&0x07 );
		else		     SetVROM_1K_Bank( 4, (((Mapper119 *)mapper)->chr01+0) );
		if( (((Mapper119 *)mapper)->chr01+1)&0x40 ) SetCRAM_1K_Bank( 5, (((Mapper119 *)mapper)->chr01+1)&0x07 );
		else		     SetVROM_1K_Bank( 5, (((Mapper119 *)mapper)->chr01+1) );
		if( (((Mapper119 *)mapper)->chr23+0)&0x40 ) SetCRAM_1K_Bank( 6, (((Mapper119 *)mapper)->chr23+0)&0x07 );
		else		     SetVROM_1K_Bank( 6, (((Mapper119 *)mapper)->chr23+0) );
		if( (((Mapper119 *)mapper)->chr23+1)&0x40 ) SetCRAM_1K_Bank( 7, (((Mapper119 *)mapper)->chr23+1)&0x07 );
		else		     SetVROM_1K_Bank( 7, (((Mapper119 *)mapper)->chr23+1) );
	} else {
		if( (((Mapper119 *)mapper)->chr01+0)&0x40 ) SetCRAM_1K_Bank( 0, (((Mapper119 *)mapper)->chr01+0)&0x07 );
		else		     SetVROM_1K_Bank( 0, (((Mapper119 *)mapper)->chr01+0) );
		if( (((Mapper119 *)mapper)->chr01+1)&0x40 ) SetCRAM_1K_Bank( 1, (((Mapper119 *)mapper)->chr01+1)&0x07 );
		else		     SetVROM_1K_Bank( 1, (((Mapper119 *)mapper)->chr01+1) );
		if( (((Mapper119 *)mapper)->chr23+0)&0x40 ) SetCRAM_1K_Bank( 2, (((Mapper119 *)mapper)->chr23+0)&0x07 );
		else		     SetVROM_1K_Bank( 2, (((Mapper119 *)mapper)->chr23+0) );
		if( (((Mapper119 *)mapper)->chr23+1)&0x40 ) SetCRAM_1K_Bank( 3, (((Mapper119 *)mapper)->chr23+1)&0x07 );
		else		     SetVROM_1K_Bank( 3, (((Mapper119 *)mapper)->chr23+1) );

		if( ((Mapper119 *)mapper)->chr4&0x40 )	SetCRAM_1K_Bank( 4, ((Mapper119 *)mapper)->chr4&0x07 );
		else		SetVROM_1K_Bank( 4, ((Mapper119 *)mapper)->chr4 );
		if( ((Mapper119 *)mapper)->chr5&0x40 )	SetCRAM_1K_Bank( 5, ((Mapper119 *)mapper)->chr5&0x07 );
		else		SetVROM_1K_Bank( 5, ((Mapper119 *)mapper)->chr5 );
		if( ((Mapper119 *)mapper)->chr6&0x40 )	SetCRAM_1K_Bank( 6, ((Mapper119 *)mapper)->chr6&0x07 );
		else		SetVROM_1K_Bank( 6, ((Mapper119 *)mapper)->chr6 );
		if( ((Mapper119 *)mapper)->chr7&0x40 )	SetCRAM_1K_Bank( 7, ((Mapper119 *)mapper)->chr7&0x07 );
		else		SetVROM_1K_Bank( 7, ((Mapper119 *)mapper)->chr7 );
	}
}

void	Mapper119_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper119 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper119 *)mapper)->prg0;
	p[ 9] = ((Mapper119 *)mapper)->prg1;
	p[10] = ((Mapper119 *)mapper)->chr01;
	p[11] = ((Mapper119 *)mapper)->chr23;
	p[12] = ((Mapper119 *)mapper)->chr4;
	p[13] = ((Mapper119 *)mapper)->chr5;
	p[14] = ((Mapper119 *)mapper)->chr6;
	p[15] = ((Mapper119 *)mapper)->chr7;
	p[16] = ((Mapper119 *)mapper)->irq_enable;
	p[17] = ((Mapper119 *)mapper)->irq_counter;
	p[18] = ((Mapper119 *)mapper)->irq_latch;
}

void	Mapper119_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper119 *)mapper)->reg[i] = p[i];
	}
	((Mapper119 *)mapper)->prg0  = p[ 8];
	((Mapper119 *)mapper)->prg1  = p[ 9];
	((Mapper119 *)mapper)->chr01 = p[10];
	((Mapper119 *)mapper)->chr23 = p[11];
	((Mapper119 *)mapper)->chr4  = p[12];
	((Mapper119 *)mapper)->chr5  = p[13];
	((Mapper119 *)mapper)->chr6  = p[14];
	((Mapper119 *)mapper)->chr7  = p[15];
	((Mapper119 *)mapper)->irq_enable  = p[16];
	((Mapper119 *)mapper)->irq_counter = p[17];
	((Mapper119 *)mapper)->irq_latch   = p[18];
}

BOOL	Mapper119_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper119_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper119));
	
	memset(mapper, 0, sizeof(Mapper119));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper119_Reset;
	mapper->vtbl.Write = Mapper119_Write;
	mapper->vtbl.HSync = Mapper119_HSync;
	mapper->vtbl.SaveState = Mapper119_SaveState;
	mapper->vtbl.LoadState = Mapper119_LoadState;
	mapper->vtbl.IsStateSave = Mapper119_IsStateSave;

	return (Mapper *)mapper;
}

