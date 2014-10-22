
//////////////////////////////////////////////////////////////////////////
// Mapper048  Taito TC190V                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg;
	BYTE	irq_enable;
	BYTE	irq_counter;

}Mapper048;

void	Mapper048_Reset(Mapper *mapper)
{
	DWORD crc;
	((Mapper048 *)mapper)->reg = 0;
	((Mapper048 *)mapper)->irq_enable = 0;
	((Mapper048 *)mapper)->irq_counter = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );

	crc = ROM_GetPROM_CRC(((Mapper048 *)mapper)->nes->rom);
//	if( crc == 0x547e6cc1 ) { // Flintstones - The Rescue of Dino & Hoppy(J)
//		((Mapper048 *)mapper)->nes->SetRenderMethod( POST_RENDER );
//	}
}

void	Mapper048_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			if( !((Mapper048 *)mapper)->reg ) {
				if( data & 0x40 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x8001:
			SetPROM_8K_Bank( 5, data );
			break;

		case	0x8002:
			SetVROM_2K_Bank( 0, data );
			break;
		case	0x8003:
			SetVROM_2K_Bank( 2, data );
			break;
		case	0xA000:
			SetVROM_1K_Bank( 4, data );
			break;
		case	0xA001:
			SetVROM_1K_Bank( 5, data );
			break;
		case	0xA002:
			SetVROM_1K_Bank( 6, data );
			break;
		case	0xA003:
			SetVROM_1K_Bank( 7, data );
			break;

		case	0xC000:
			((Mapper048 *)mapper)->irq_counter = data;
			((Mapper048 *)mapper)->irq_enable = 0;
//			((Mapper048 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case	0xC001:
			((Mapper048 *)mapper)->irq_counter = data;
			((Mapper048 *)mapper)->irq_enable = 1;
//			((Mapper048 *)mapper)->irq_enable = data & 0x01;
//			((Mapper048 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case	0xC002:
			break;
		case	0xC003:
			break;

		case	0xE000:
			if( data & 0x40 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			((Mapper048 *)mapper)->reg = 1;
			break;
	}
}

void	Mapper048_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline < SCREEN_HEIGHT) ) {
		if( PPU_IsDispON(((Mapper048 *)mapper)->nes->ppu) ) {
			if( ((Mapper048 *)mapper)->irq_enable ) {
				if( ((Mapper048 *)mapper)->irq_counter == 0xFF ) {
//					CPU_IRQ_NotPending(((Mapper048 *)mapper)->nes->cpu);
//					CPU_SetIRQ( ((Mapper048 *)mapper)->nes->cpu, IRQ_MAPPER );
					CPU_SetIRQ( ((Mapper048 *)mapper)->nes->cpu, IRQ_TRIGGER2 );
				}
				((Mapper048 *)mapper)->irq_counter++;
			}
		}
	}
}

void	Mapper048_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper048 *)mapper)->reg;
	p[1] = ((Mapper048 *)mapper)->irq_enable;
	p[2] = ((Mapper048 *)mapper)->irq_counter;
}

void	Mapper048_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper048 *)mapper)->reg = p[0];
	((Mapper048 *)mapper)->irq_enable  = p[1];
	((Mapper048 *)mapper)->irq_counter = p[2];
}

BOOL	Mapper048_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper048_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper048));
	
	memset(mapper, 0, sizeof(Mapper048));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper048_Reset;
	mapper->vtbl.Write = Mapper048_Write;
	mapper->vtbl.HSync = Mapper048_HSync;
	mapper->vtbl.SaveState = Mapper048_SaveState;
	mapper->vtbl.LoadState = Mapper048_LoadState;
	mapper->vtbl.IsStateSave = Mapper048_IsStateSave;

	return (Mapper *)mapper;
}

