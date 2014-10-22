
//////////////////////////////////////////////////////////////////////////
// Mapper083  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[3];
	INT	chr_bank;
	BYTE	irq_enable;
	INT	irq_counter;

	BYTE	patch;
}Mapper083;

void	Mapper083_Reset(Mapper *mapper)
{
	INT i;

	for( i = 0; i < 3; i++ ) {
		((Mapper083 *)mapper)->reg[i] = 0x00;
	}

	if( PROM_8K_SIZE >= 32 ) {
		SetPROM_32K_Bank( 0, 1, 30, 31 );
		((Mapper083 *)mapper)->reg[1] = 0x30;
	} else {
		SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	((Mapper083 *)mapper)->chr_bank = 0;

	((Mapper083 *)mapper)->irq_enable = 0;	// Disable
	((Mapper083 *)mapper)->irq_counter = 0;

	((Mapper083 *)mapper)->patch = 0;
	if( ROM_GetPROM_CRC(((Mapper083 *)mapper)->nes->rom) == 0x1461D1F8 ) {
		((Mapper083 *)mapper)->patch = 1;
	}
}

BYTE	Mapper083_ReadLow( Mapper *mapper, WORD addr )
{
	if( (addr&0x5100) == 0x5100 ) {
		return	((Mapper083 *)mapper)->reg[2];
	} else if( addr >= 0x6000 ) {
		return	Mapper_DefReadLow(mapper,  addr );
	}

	return	(BYTE)(addr>>8);
}

void	Mapper083_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, ((Mapper083 *)mapper)->nes->GetScanline(), ((Mapper083 *)mapper)->nes->cpu->GetTotalCycles() );
	switch( addr ) {
		case	0x5101:
		case	0x5102:
		case	0x5103:
			((Mapper083 *)mapper)->reg[2] = data;
			break;
	}

	if( addr >= 0x6000 ) {
		Mapper_DefWriteLow( mapper, addr, data );
	}
}

void	Mapper083_Write( Mapper *mapper, WORD addr, BYTE data )
{
//DebugOut( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, ((Mapper083 *)mapper)->nes->GetScanline(), ((Mapper083 *)mapper)->nes->cpu->GetTotalCycles() );
	switch( addr ) {
		case	0x8000:
		case	0xB000:
		case	0xB0FF:
		case	0xB1FF:
			((Mapper083 *)mapper)->reg[0] = data;
			((Mapper083 *)mapper)->chr_bank = (data&0x30)<<4;
			SetPROM_16K_Bank( 4, data );
			SetPROM_16K_Bank( 6, (data&0x30)|0x0F );
			break;

		case	0x8100:
			((Mapper083 *)mapper)->reg[1] = data & 0x80;
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			break;

		case	0x8200:
			((Mapper083 *)mapper)->irq_counter = (((Mapper083 *)mapper)->irq_counter&0xFF00)|(INT)data;
//			((Mapper083 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0x8201:
			((Mapper083 *)mapper)->irq_counter = (((Mapper083 *)mapper)->irq_counter&0x00FF)|((INT)data<<8);
			((Mapper083 *)mapper)->irq_enable = ((Mapper083 *)mapper)->reg[1];
//			((Mapper083 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case	0x8300:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x8301:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0x8302:
			SetPROM_8K_Bank( 6, data );
			break;

		case	0x8310:
			if( ((Mapper083 *)mapper)->patch ) {
				SetVROM_2K_Bank( 0, ((Mapper083 *)mapper)->chr_bank|data );
			} else {
				SetVROM_1K_Bank( 0, ((Mapper083 *)mapper)->chr_bank|data );
			}
			break;
		case	0x8311:
			if( ((Mapper083 *)mapper)->patch ) {
				SetVROM_2K_Bank( 2, ((Mapper083 *)mapper)->chr_bank|data );
			} else {
				SetVROM_1K_Bank( 1, ((Mapper083 *)mapper)->chr_bank|data );
			}
			break;
		case	0x8312:
			SetVROM_1K_Bank( 2, ((Mapper083 *)mapper)->chr_bank|data );
			break;
		case	0x8313:
			SetVROM_1K_Bank( 3, ((Mapper083 *)mapper)->chr_bank|data );
			break;
		case	0x8314:
			SetVROM_1K_Bank( 4, ((Mapper083 *)mapper)->chr_bank|data );
			break;
		case	0x8315:
			SetVROM_1K_Bank( 5, ((Mapper083 *)mapper)->chr_bank|data );
			break;
		case	0x8316:
			if( ((Mapper083 *)mapper)->patch ) {
				SetVROM_2K_Bank( 4, ((Mapper083 *)mapper)->chr_bank|data );
			} else {
				SetVROM_1K_Bank( 6, ((Mapper083 *)mapper)->chr_bank|data );
			}
			break;
		case	0x8317:
			if( ((Mapper083 *)mapper)->patch ) {
				SetVROM_2K_Bank( 6, ((Mapper083 *)mapper)->chr_bank|data );
			} else {
				SetVROM_1K_Bank( 7, ((Mapper083 *)mapper)->chr_bank|data );
			}
			break;

		case	0x8318:
			SetPROM_16K_Bank( 4, (((Mapper083 *)mapper)->reg[0]&0x30)|data );
			break;
	}
}

void	Mapper083_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper083 *)mapper)->irq_enable ) {
		if( ((Mapper083 *)mapper)->irq_counter <= 113 ) {
//			CPU_IRQ(((Mapper083 *)mapper)->nes->cpu);
			((Mapper083 *)mapper)->irq_enable = 0;
//			CPU_SetIRQ( ((Mapper083 *)mapper)->nes->cpu, IRQ_MAPPER );
			CPU_SetIRQ( ((Mapper083 *)mapper)->nes->cpu, IRQ_TRIGGER );
		} else {
			((Mapper083 *)mapper)->irq_counter -= 113;
		}
	}
}

void	Mapper083_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper083 *)mapper)->reg[0];
	p[1] = ((Mapper083 *)mapper)->reg[1];
	p[2] = ((Mapper083 *)mapper)->reg[2];
	*(INT*)&p[3] = ((Mapper083 *)mapper)->chr_bank;
	p[7] = ((Mapper083 *)mapper)->irq_enable;
	*(INT*)&p[8] = ((Mapper083 *)mapper)->irq_counter;
}

void	Mapper083_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper083 *)mapper)->reg[0] = p[0];
	((Mapper083 *)mapper)->reg[1] = p[1];
	((Mapper083 *)mapper)->reg[2] = p[2];
	((Mapper083 *)mapper)->chr_bank = *(INT*)&p[3];
	((Mapper083 *)mapper)->irq_enable = p[7];
	((Mapper083 *)mapper)->irq_counter = *(INT*)&p[8];
}

BOOL	Mapper083_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper083_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper083));
	
	memset(mapper, 0, sizeof(Mapper083));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper083_Reset;
	mapper->vtbl.ReadLow = Mapper083_ReadLow;
	mapper->vtbl.WriteLow = Mapper083_WriteLow;
	mapper->vtbl.Write = Mapper083_Write;
	mapper->vtbl.HSync = Mapper083_HSync;
	mapper->vtbl.SaveState = Mapper083_SaveState;
	mapper->vtbl.LoadState = Mapper083_LoadState;
	mapper->vtbl.IsStateSave = Mapper083_IsStateSave;

	return (Mapper *)mapper;
}

