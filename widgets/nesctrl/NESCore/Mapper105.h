
//////////////////////////////////////////////////////////////////////////
// Mapper105  Nintendo World Championship                               //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	init_state;
	BYTE	write_count;
	BYTE	bits;
	BYTE	reg[4];

	BYTE	irq_enable;
	INT	irq_counter;
}Mapper105;


void	Mapper105_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );

	((Mapper105 *)mapper)->reg[0] = 0x0C;
	((Mapper105 *)mapper)->reg[1] = 0x00;
	((Mapper105 *)mapper)->reg[2] = 0x00;
	((Mapper105 *)mapper)->reg[3] = 0x10;

	((Mapper105 *)mapper)->bits = 0;
	((Mapper105 *)mapper)->write_count = 0;

	((Mapper105 *)mapper)->irq_counter = 0;
	((Mapper105 *)mapper)->irq_enable = 0;
	((Mapper105 *)mapper)->init_state = 0;
}

void	Mapper105_Write( Mapper *mapper, WORD addr, BYTE data )
{
	WORD reg_num = (addr & 0x7FFF) >> 13;

	if( data & 0x80 ) {
		((Mapper105 *)mapper)->bits = ((Mapper105 *)mapper)->write_count = 0;
		if( reg_num == 0 ) {
			((Mapper105 *)mapper)->reg[reg_num] |= 0x0C;
		}
	} else {
		((Mapper105 *)mapper)->bits |= (data & 1) << ((Mapper105 *)mapper)->write_count++;
		if( ((Mapper105 *)mapper)->write_count == 5) {
			((Mapper105 *)mapper)->reg[reg_num] = ((Mapper105 *)mapper)->bits & 0x1F;
			((Mapper105 *)mapper)->bits = ((Mapper105 *)mapper)->write_count = 0;
		}
	}

	if( ((Mapper105 *)mapper)->reg[0] & 0x02 ) {
		if( ((Mapper105 *)mapper)->reg[0] & 0x01 ) {
			SetVRAM_Mirror_cont( VRAM_HMIRROR );
		} else {
			SetVRAM_Mirror_cont( VRAM_VMIRROR );
		}
	} else {
		if( ((Mapper105 *)mapper)->reg[0] & 0x01 ) {
			SetVRAM_Mirror_cont( VRAM_MIRROR4H );
		} else {
			SetVRAM_Mirror_cont( VRAM_MIRROR4L );
		}
	}

	switch( ((Mapper105 *)mapper)->init_state ) {
		case 0:
		case 1:
			((Mapper105 *)mapper)->init_state++;
			break;
		case 2:
			if(((Mapper105 *)mapper)->reg[1] & 0x08) {
				if (((Mapper105 *)mapper)->reg[0] & 0x08) {
					if (((Mapper105 *)mapper)->reg[0] & 0x04) {
						SetPROM_8K_Bank(4,((((Mapper105 *)mapper)->reg[3] & 0x07) * 2 + 16));
						SetPROM_8K_Bank(5,((((Mapper105 *)mapper)->reg[3] & 0x07) * 2 + 17));
						SetPROM_8K_Bank(6,30);
						SetPROM_8K_Bank(7,31);
					} else {
						SetPROM_8K_Bank(4,16);
						SetPROM_8K_Bank(5,17);
						SetPROM_8K_Bank(6,((((Mapper105 *)mapper)->reg[3] & 0x07) * 2 + 16));
						SetPROM_8K_Bank(7,((((Mapper105 *)mapper)->reg[3] & 0x07) * 2 + 17));
					}
				} else {
					SetPROM_8K_Bank(4,((((Mapper105 *)mapper)->reg[3] & 0x06) * 2 + 16));
					SetPROM_8K_Bank(5,((((Mapper105 *)mapper)->reg[3] & 0x06) * 2 + 17));
					SetPROM_8K_Bank(6,((((Mapper105 *)mapper)->reg[3] & 0x06) * 2 + 18));
					SetPROM_8K_Bank(7,((((Mapper105 *)mapper)->reg[3] & 0x06) * 2 + 19));
				}
			} else {
				SetPROM_8K_Bank(4,((((Mapper105 *)mapper)->reg[1] & 0x06) * 2 + 0));
				SetPROM_8K_Bank(5,((((Mapper105 *)mapper)->reg[1] & 0x06) * 2 + 1));
				SetPROM_8K_Bank(6,((((Mapper105 *)mapper)->reg[1] & 0x06) * 2 + 2));
				SetPROM_8K_Bank(7,((((Mapper105 *)mapper)->reg[1] & 0x06) * 2 + 3));
			}

			if( ((Mapper105 *)mapper)->reg[1] & 0x10 ) {
				((Mapper105 *)mapper)->irq_counter = 0;
				((Mapper105 *)mapper)->irq_enable = 0;
			} else {
				((Mapper105 *)mapper)->irq_enable = 1;
			}
//			((Mapper105 *)mapper)->nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		default:
			break;
	}
}

void	Mapper105_HSync( Mapper *mapper, INT scanline )
{
	if( !scanline ) {
		if( ((Mapper105 *)mapper)->irq_enable ) {
			((Mapper105 *)mapper)->irq_counter += 29781;
		}
		if( ((((Mapper105 *)mapper)->irq_counter | 0x21FFFFFF) & 0x3E000000) == 0x3E000000 ) {
//			CPU_IRQ_NotPending(((Mapper105 *)mapper)->nes->cpu);
//			CPU_SetIRQ( ((Mapper105 *)mapper)->nes->cpu, IRQ_MAPPER );
			CPU_SetIRQ( ((Mapper105 *)mapper)->nes->cpu, IRQ_TRIGGER2 );
		}
	}
}

void	Mapper105_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 4; i++ ) {
		p[i] = ((Mapper105 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper105 *)mapper)->init_state;
	p[ 9] = ((Mapper105 *)mapper)->write_count;
	p[10] = ((Mapper105 *)mapper)->bits;
	p[11] = ((Mapper105 *)mapper)->irq_enable;
	*((INT*)&p[12]) = ((Mapper105 *)mapper)->irq_counter;
}

void	Mapper105_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 4; i++ ) {
		((Mapper105 *)mapper)->reg[i] = p[i];
	}
	((Mapper105 *)mapper)->init_state  = p[ 8];
	((Mapper105 *)mapper)->write_count = p[ 9];
	((Mapper105 *)mapper)->bits        = p[10];
	((Mapper105 *)mapper)->irq_enable  = p[11];
	((Mapper105 *)mapper)->irq_counter = *((INT*)&p[12]);
}

BOOL	Mapper105_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper105_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper105));
	
	memset(mapper, 0, sizeof(Mapper105));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper105_Reset;
	mapper->vtbl.Write = Mapper105_Write;
	mapper->vtbl.HSync = Mapper105_HSync;
	mapper->vtbl.SaveState = Mapper105_SaveState;
	mapper->vtbl.LoadState = Mapper105_LoadState;
	mapper->vtbl.IsStateSave = Mapper105_IsStateSave;

	return (Mapper *)mapper;
}


