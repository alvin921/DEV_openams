
//////////////////////////////////////////////////////////////////////////
// Mapper160  PC-Aladdin                                                //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	refresh_type;
}Mapper160;

void	Mapper160_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	((Mapper160 *)mapper)->irq_enable = 0;
	((Mapper160 *)mapper)->irq_counter = 0;
	((Mapper160 *)mapper)->irq_latch = 0;
	((Mapper160 *)mapper)->refresh_type = 0;
}

void	Mapper160_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x8001:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0x8002:
			SetPROM_8K_Bank( 6, data );
			break;

		case	0x9000:
			if( data == 0x2B ) {
				((Mapper160 *)mapper)->refresh_type = 1;
			} else if( data == 0xA8 ) {
				((Mapper160 *)mapper)->refresh_type = 2;
			} else if( data == 0x1F ) {
				((Mapper160 *)mapper)->refresh_type = 3;
			} else if( data == 0x7C ) {
				((Mapper160 *)mapper)->refresh_type = 4;
			} else if( data == 0x18 ) {
				((Mapper160 *)mapper)->refresh_type = 5;
			} else if( data == 0x60 ) {
				((Mapper160 *)mapper)->refresh_type = 6;
			} else {
				((Mapper160 *)mapper)->refresh_type = 0;
			}
			SetVROM_1K_Bank( 0, data );
			break;
		case	0x9001:
			SetVROM_1K_Bank( 1, data );
			break;

		case	0x9002:
			if( ((Mapper160 *)mapper)->refresh_type == 2 && data != 0xE8 ) {
				((Mapper160 *)mapper)->refresh_type = 0;
			}
			SetVROM_1K_Bank( 2, data );
			break;

		case	0x9003:
			SetVROM_1K_Bank( 3, data );
			break;
		case	0x9004:
			SetVROM_1K_Bank( 4, data );
			break;
		case	0x9005:
			SetVROM_1K_Bank( 5, data );
			break;
		case	0x9006:
			SetVROM_1K_Bank( 6, data );
			break;
		case	0x9007:
			SetVROM_1K_Bank( 7, data );
			break;

		case	0xC000:
			((Mapper160 *)mapper)->irq_counter = ((Mapper160 *)mapper)->irq_latch;
			((Mapper160 *)mapper)->irq_enable = ((Mapper160 *)mapper)->irq_latch;
			break;
		case	0xC001:
			((Mapper160 *)mapper)->irq_latch = data;
			break;
		case	0xC002:
			((Mapper160 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0xC003:
			((Mapper160 *)mapper)->irq_counter = data;
			break;
	}
}

void	Mapper160_HSync( Mapper *mapper, INT scanline )
{
	if( scanline == 0 || scanline == SCREEN_HEIGHT-1 ) {
		switch( ((Mapper160 *)mapper)->refresh_type ) {
			case	1:
				SetVROM_8K_Bank(0x58,0x59,0x5A,0x5B,0x58,0x59,0x5A,0x5B);
				break;
			case	2:
				SetVROM_8K_Bank(0x78,0x79,0x7A,0x7B,0x78,0x79,0x7A,0x7B);
				break;
			case	3:
				SetVROM_8K_Bank(0x7C,0x7D,0x7E,0x7F,0x7C,0x7D,0x7E,0x7F);
				break;
			case	5:
				SetVROM_8K_Bank(0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77);
				break;
			case	6:
				SetVROM_8K_Bank(0x5C,0x5D,0x5E,0x5F,0x7C,0x7D,0x7E,0x7F);
				break;
		}
	}
	if( scanline == 64 ) {
		if( ((Mapper160 *)mapper)->refresh_type == 4 ) {
			if( PPU_MEM_BANK[8][32*10+16] == 0x0A ) {
				SetVROM_1K_Bank( 0, 0x68 );
				SetVROM_1K_Bank( 1, 0x69 );
				SetVROM_1K_Bank( 2, 0x6A );
				SetVROM_1K_Bank( 3, 0x6B );
			} else {
				SetVROM_1K_Bank( 0, 0x6C );
				SetVROM_1K_Bank( 1, 0x6D );
				SetVROM_1K_Bank( 2, 0x6E );
				SetVROM_1K_Bank( 3, 0x6F );
			}
		}
	}
	if( scanline == 128 ) {
		if( ((Mapper160 *)mapper)->refresh_type == 4 ) {
			SetVROM_1K_Bank( 0, 0x68 );
			SetVROM_1K_Bank( 1, 0x69 );
			SetVROM_1K_Bank( 2, 0x6A );
			SetVROM_1K_Bank( 3, 0x6B );
		} else if( ((Mapper160 *)mapper)->refresh_type == 5 ) {
			SetVROM_8K_Bank(0x74,0x75,0x76,0x77,0x74,0x75,0x76,0x77);
		}
	}
	if( scanline == 160 ) {
		if( ((Mapper160 *)mapper)->refresh_type == 6 ) {
			SetVROM_8K_Bank(0x60,0x61,0x5E,0x5F,0x7C,0x7D,0x7E,0x7F);
		}
	}

	if( ((Mapper160 *)mapper)->irq_enable ) {
		if( ((Mapper160 *)mapper)->irq_counter == 0xFF ) {
//			mapper->nes->cpu->IRQ_NotPending();
			((Mapper160 *)mapper)->irq_enable  = 0;
			((Mapper160 *)mapper)->irq_counter = 0;
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
		} else {
			((Mapper160 *)mapper)->irq_counter++;
		}
	}
}

void	Mapper160_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper160 *)mapper)->irq_enable;
	p[1] = ((Mapper160 *)mapper)->irq_counter;
	p[2] = ((Mapper160 *)mapper)->irq_latch;
	p[3] = ((Mapper160 *)mapper)->refresh_type;
}

void	Mapper160_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper160 *)mapper)->irq_enable   = p[0];
	((Mapper160 *)mapper)->irq_counter  = p[1];
	((Mapper160 *)mapper)->irq_latch    = p[2];
	((Mapper160 *)mapper)->refresh_type = p[3];
}

BOOL	Mapper160_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper160_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper160));
	
	memset(mapper, 0, sizeof(Mapper160));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper160_Reset;
	mapper->vtbl.Write = Mapper160_Write;
	mapper->vtbl.HSync = Mapper160_HSync;
	mapper->vtbl.SaveState = Mapper160_SaveState;
	mapper->vtbl.LoadState = Mapper160_LoadState;
	mapper->vtbl.IsStateSave = Mapper160_IsStateSave;

	return (Mapper *)mapper;
}

