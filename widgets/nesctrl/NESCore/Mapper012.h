//////////////////////////////////////////////////////////////////////////
// Mapper012  DBZ5                                                      //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	DWORD	vb0, vb1;
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;
	BYTE	irq_preset;
	BYTE	irq_preset_vbl;
}Mapper012;

void	Mapper012_SetBank_CPU(Mapper *mapper);
void	Mapper012_SetBank_PPU(Mapper *mapper);


void	Mapper012_Reset(Mapper *mapper)
{
	INT i;
	for( i = 0; i < 8; i++ ) {
		((Mapper012 *)mapper)->reg[i] = 0x00;
	}

	((Mapper012 *)mapper)->prg0 = 0;
	((Mapper012 *)mapper)->prg1 = 1;
	Mapper012_SetBank_CPU(mapper);

	((Mapper012 *)mapper)->vb0 = 0;
	((Mapper012 *)mapper)->vb1 = 0;
	((Mapper012 *)mapper)->chr01 = 0;
	((Mapper012 *)mapper)->chr23 = 2;
	((Mapper012 *)mapper)->chr4  = 4;
	((Mapper012 *)mapper)->chr5  = 5;
	((Mapper012 *)mapper)->chr6  = 6;
	((Mapper012 *)mapper)->chr7  = 7;
	Mapper012_SetBank_PPU(mapper);

	((Mapper012 *)mapper)->we_sram  = 0;	// Disable
	((Mapper012 *)mapper)->irq_enable = 0;	// Disable
	((Mapper012 *)mapper)->irq_counter = 0;
	((Mapper012 *)mapper)->irq_latch = 0xFF;
	((Mapper012 *)mapper)->irq_request = 0;
	((Mapper012 *)mapper)->irq_preset = 0;
	((Mapper012 *)mapper)->irq_preset_vbl = 0;
}

void Mapper012_WriteLow(Mapper *mapper, WORD addr, BYTE data)
{
	if( addr > 0x4100 && addr < 0x6000 ){
		((Mapper012 *)mapper)->vb0 = (data&0x01)<<8;
		((Mapper012 *)mapper)->vb1 = (data&0x10)<<4;
		Mapper012_SetBank_PPU(mapper);
	}else{
		Mapper_DefWriteLow( mapper, addr, data);
	}
}

BYTE Mapper012_ReadLow(Mapper *mapper, WORD addr)
{
	return 0x01;
}

void	Mapper012_Write( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, mapper->nes->GetScanline(), mapper->nes->cpu->GetTotalCycles() );

	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper012 *)mapper)->reg[0] = data;
			Mapper012_SetBank_CPU(mapper);
			Mapper012_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper012 *)mapper)->reg[1] = data;

			switch( ((Mapper012 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper012 *)mapper)->chr01 = data & 0xFE;
					Mapper012_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper012 *)mapper)->chr23 = data & 0xFE;
					Mapper012_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper012 *)mapper)->chr4 = data;
					Mapper012_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper012 *)mapper)->chr5 = data;
					Mapper012_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper012 *)mapper)->chr6 = data;
					Mapper012_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper012 *)mapper)->chr7 = data;
					Mapper012_SetBank_PPU(mapper);
					break;
				case	0x06:
					((Mapper012 *)mapper)->prg0 = data;
					Mapper012_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper012 *)mapper)->prg1 = data;
					Mapper012_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper012 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(mapper->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper012 *)mapper)->reg[3] = data;
			break;
		case	0xC000:
			((Mapper012 *)mapper)->reg[4] = data;
			((Mapper012 *)mapper)->irq_latch = data;
			break;
		case	0xC001:
			((Mapper012 *)mapper)->reg[5] = data;
			if( NES_GetScanline(mapper->nes) < SCREEN_HEIGHT ) {
				((Mapper012 *)mapper)->irq_counter |= 0x80;
				((Mapper012 *)mapper)->irq_preset = 0xFF;
			} else {
				((Mapper012 *)mapper)->irq_counter |= 0x80;
				((Mapper012 *)mapper)->irq_preset_vbl = 0xFF;
				((Mapper012 *)mapper)->irq_preset = 0;
			}
			break;
		case	0xE000:
			((Mapper012 *)mapper)->reg[6] = data;
			((Mapper012 *)mapper)->irq_enable = 0;
			((Mapper012 *)mapper)->irq_request = 0;

			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper012 *)mapper)->reg[7] = data;
			((Mapper012 *)mapper)->irq_enable = 1;
			((Mapper012 *)mapper)->irq_request = 0;
			break;
	}
}

void	Mapper012_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) && PPU_IsDispON(mapper->nes->ppu) ) {
		if( ((Mapper012 *)mapper)->irq_preset_vbl ) {
			((Mapper012 *)mapper)->irq_counter = ((Mapper012 *)mapper)->irq_latch;
			((Mapper012 *)mapper)->irq_preset_vbl = 0;
		}
		if( ((Mapper012 *)mapper)->irq_preset ) {
			((Mapper012 *)mapper)->irq_counter = ((Mapper012 *)mapper)->irq_latch;
			((Mapper012 *)mapper)->irq_preset = 0;
		} else if( ((Mapper012 *)mapper)->irq_counter > 0 ) {
			((Mapper012 *)mapper)->irq_counter--;
		}

		if( ((Mapper012 *)mapper)->irq_counter == 0 ) {
			// Some game set ((Mapper012 *)mapper)->irq_latch to zero to disable irq. So check it here.
			if( ((Mapper012 *)mapper)->irq_enable && ((Mapper012 *)mapper)->irq_latch ) {
				((Mapper012 *)mapper)->irq_request = 0xFF;
				CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
			}
			((Mapper012 *)mapper)->irq_preset = 0xFF;
		}
	}
}

void	Mapper012_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper012 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, ((Mapper012 *)mapper)->prg1, ((Mapper012 *)mapper)->prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( ((Mapper012 *)mapper)->prg0, ((Mapper012 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper012_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		if( ((Mapper012 *)mapper)->reg[0] & 0x80 ) {
			SetVROM_8K_Bank( ((Mapper012 *)mapper)->vb0+((Mapper012 *)mapper)->chr4, ((Mapper012 *)mapper)->vb0+((Mapper012 *)mapper)->chr5, ((Mapper012 *)mapper)->vb0+((Mapper012 *)mapper)->chr6, ((Mapper012 *)mapper)->vb0+((Mapper012 *)mapper)->chr7,
					 ((Mapper012 *)mapper)->vb1+((Mapper012 *)mapper)->chr01, ((Mapper012 *)mapper)->vb1+((Mapper012 *)mapper)->chr01+1, ((Mapper012 *)mapper)->vb1+((Mapper012 *)mapper)->chr23, ((Mapper012 *)mapper)->vb1+((Mapper012 *)mapper)->chr23+1 );
		} else {
			SetVROM_8K_Bank( ((Mapper012 *)mapper)->vb0+((Mapper012 *)mapper)->chr01, ((Mapper012 *)mapper)->vb0+((Mapper012 *)mapper)->chr01+1, ((Mapper012 *)mapper)->vb0+((Mapper012 *)mapper)->chr23, ((Mapper012 *)mapper)->vb0+((Mapper012 *)mapper)->chr23+1,
					 ((Mapper012 *)mapper)->vb1+((Mapper012 *)mapper)->chr4, ((Mapper012 *)mapper)->vb1+((Mapper012 *)mapper)->chr5, ((Mapper012 *)mapper)->vb1+((Mapper012 *)mapper)->chr6, ((Mapper012 *)mapper)->vb1+((Mapper012 *)mapper)->chr7 );
		}
	} else {
		if( ((Mapper012 *)mapper)->reg[0] & 0x80 ) {
			SetCRAM_1K_Bank( 4, (((Mapper012 *)mapper)->chr01+0)&0x07 );
			SetCRAM_1K_Bank( 5, (((Mapper012 *)mapper)->chr01+1)&0x07 );
			SetCRAM_1K_Bank( 6, (((Mapper012 *)mapper)->chr23+0)&0x07 );
			SetCRAM_1K_Bank( 7, (((Mapper012 *)mapper)->chr23+1)&0x07 );
			SetCRAM_1K_Bank( 0, ((Mapper012 *)mapper)->chr4&0x07 );
			SetCRAM_1K_Bank( 1, ((Mapper012 *)mapper)->chr5&0x07 );
			SetCRAM_1K_Bank( 2, ((Mapper012 *)mapper)->chr6&0x07 );
			SetCRAM_1K_Bank( 3, ((Mapper012 *)mapper)->chr7&0x07 );
		} else {
			SetCRAM_1K_Bank( 0, (((Mapper012 *)mapper)->chr01+0)&0x07 );
			SetCRAM_1K_Bank( 1, (((Mapper012 *)mapper)->chr01+1)&0x07 );
			SetCRAM_1K_Bank( 2, (((Mapper012 *)mapper)->chr23+0)&0x07 );
			SetCRAM_1K_Bank( 3, (((Mapper012 *)mapper)->chr23+1)&0x07 );
			SetCRAM_1K_Bank( 4, ((Mapper012 *)mapper)->chr4&0x07 );
			SetCRAM_1K_Bank( 5, ((Mapper012 *)mapper)->chr5&0x07 );
			SetCRAM_1K_Bank( 6, ((Mapper012 *)mapper)->chr6&0x07 );
			SetCRAM_1K_Bank( 7, ((Mapper012 *)mapper)->chr7&0x07 );
		}
	}
}

void	Mapper012_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper012 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper012 *)mapper)->prg0;
	p[ 9] = ((Mapper012 *)mapper)->prg1;
	p[10] = ((Mapper012 *)mapper)->chr01;
	p[11] = ((Mapper012 *)mapper)->chr23;
	p[12] = ((Mapper012 *)mapper)->chr4;
	p[13] = ((Mapper012 *)mapper)->chr5;
	p[14] = ((Mapper012 *)mapper)->chr6;
	p[15] = ((Mapper012 *)mapper)->chr7;
	p[16] = ((Mapper012 *)mapper)->irq_enable;
	p[17] = (BYTE)((Mapper012 *)mapper)->irq_counter;
	p[18] = ((Mapper012 *)mapper)->irq_latch;
	p[19] = ((Mapper012 *)mapper)->irq_request;
	p[20] = ((Mapper012 *)mapper)->irq_preset;
	p[21] = ((Mapper012 *)mapper)->irq_preset_vbl;
	*((DWORD*)&p[22]) = ((Mapper012 *)mapper)->vb0;
	*((DWORD*)&p[26]) = ((Mapper012 *)mapper)->vb1;
}

void	Mapper012_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;
	for( i = 0; i < 8; i++ ) {
		((Mapper012 *)mapper)->reg[i] = p[i];
	}
	((Mapper012 *)mapper)->prg0  = p[ 8];
	((Mapper012 *)mapper)->prg1  = p[ 9];
	((Mapper012 *)mapper)->chr01 = p[10];
	((Mapper012 *)mapper)->chr23 = p[11];
	((Mapper012 *)mapper)->chr4  = p[12];
	((Mapper012 *)mapper)->chr5  = p[13];
	((Mapper012 *)mapper)->chr6  = p[14];
	((Mapper012 *)mapper)->chr7  = p[15];
	((Mapper012 *)mapper)->irq_enable  = p[16];
	((Mapper012 *)mapper)->irq_counter = (INT)p[17];
	((Mapper012 *)mapper)->irq_latch   = p[18];
	((Mapper012 *)mapper)->irq_request = p[19];
	((Mapper012 *)mapper)->irq_preset  = p[20];
	((Mapper012 *)mapper)->irq_preset_vbl = p[21];
	((Mapper012 *)mapper)->vb0 = *((DWORD*)&p[22]);
	((Mapper012 *)mapper)->vb1 = *((DWORD*)&p[26]);
}

BOOL	Mapper012_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper012_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper012));
	
	memset(mapper, 0, sizeof(Mapper012));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper012_Reset;
	mapper->vtbl.ReadLow = Mapper012_ReadLow;
	mapper->vtbl.WriteLow = Mapper012_WriteLow;
	mapper->vtbl.Write = Mapper012_Write;
	mapper->vtbl.HSync = Mapper012_HSync;
	mapper->vtbl.SaveState = Mapper012_SaveState;
	mapper->vtbl.LoadState = Mapper012_LoadState;
	mapper->vtbl.IsStateSave = Mapper012_IsStateSave;

	return (Mapper *)mapper;
}

