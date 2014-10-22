
//////////////////////////////////////////////////////////////////////////
// Mapper183  Gimmick (Bootleg)                                         //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	irq_enable;
	INT	irq_counter;
}Mapper183;

void	Mapper183_Reset(Mapper *mapper)
{
	INT i;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank_cont( 0 );

	for( i = 0; i < 8; i++ ) {
		((Mapper183 *)mapper)->reg[i] = i;
	}
	((Mapper183 *)mapper)->irq_enable = 0;
	((Mapper183 *)mapper)->irq_counter = 0;
}

void	Mapper183_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8800:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0xA800:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0xA000:
			SetPROM_8K_Bank( 6, data );
			break;

		case	0xB000:
			((Mapper183 *)mapper)->reg[0] = (((Mapper183 *)mapper)->reg[0]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 0, ((Mapper183 *)mapper)->reg[0] );
			break;
		case	0xB004:
			((Mapper183 *)mapper)->reg[0] = (((Mapper183 *)mapper)->reg[0]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 0, ((Mapper183 *)mapper)->reg[0] );
			break;
		case	0xB008:
			((Mapper183 *)mapper)->reg[1] = (((Mapper183 *)mapper)->reg[1]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 1, ((Mapper183 *)mapper)->reg[1] );
			break;
		case	0xB00C:
			((Mapper183 *)mapper)->reg[1] = (((Mapper183 *)mapper)->reg[1]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 1, ((Mapper183 *)mapper)->reg[1] );
			break;

		case	0xC000:
			((Mapper183 *)mapper)->reg[2] = (((Mapper183 *)mapper)->reg[2]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 2, ((Mapper183 *)mapper)->reg[2] );
			break;
		case	0xC004:
			((Mapper183 *)mapper)->reg[2] = (((Mapper183 *)mapper)->reg[2]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 2, ((Mapper183 *)mapper)->reg[2] );
			break;
		case	0xC008:
			((Mapper183 *)mapper)->reg[3] = (((Mapper183 *)mapper)->reg[3]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 3, ((Mapper183 *)mapper)->reg[3] );
			break;
		case	0xC00C:
			((Mapper183 *)mapper)->reg[3] = (((Mapper183 *)mapper)->reg[3]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 3, ((Mapper183 *)mapper)->reg[3] );
			break;

		case	0xD000:
			((Mapper183 *)mapper)->reg[4] = (((Mapper183 *)mapper)->reg[4]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 4, ((Mapper183 *)mapper)->reg[4] );
			break;
		case	0xD004:
			((Mapper183 *)mapper)->reg[4] = (((Mapper183 *)mapper)->reg[4]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 4, ((Mapper183 *)mapper)->reg[4] );
			break;
		case	0xD008:
			((Mapper183 *)mapper)->reg[5] = (((Mapper183 *)mapper)->reg[5]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 5, ((Mapper183 *)mapper)->reg[5] );
			break;
		case	0xD00C:
			((Mapper183 *)mapper)->reg[5] = (((Mapper183 *)mapper)->reg[5]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 5, ((Mapper183 *)mapper)->reg[5] );
			break;

		case	0xE000:
			((Mapper183 *)mapper)->reg[6] = (((Mapper183 *)mapper)->reg[6]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 6, ((Mapper183 *)mapper)->reg[6] );
			break;
		case	0xE004:
			((Mapper183 *)mapper)->reg[6] = (((Mapper183 *)mapper)->reg[6]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 6, ((Mapper183 *)mapper)->reg[6] );
			break;
		case	0xE008:
			((Mapper183 *)mapper)->reg[7] = (((Mapper183 *)mapper)->reg[3]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 7, ((Mapper183 *)mapper)->reg[7] );
			break;
		case	0xE00C:
			((Mapper183 *)mapper)->reg[7] = (((Mapper183 *)mapper)->reg[3]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 7, ((Mapper183 *)mapper)->reg[7] );
			break;

		case	0x9008:
			if( data == 1 ) {
				INT i;

				for( i = 0; i < 8; i++ ) {
					((Mapper183 *)mapper)->reg[i] = i;
				}
				SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
				SetVROM_8K_Bank_cont( 0 );
			}
			break;

		case	0x9800:
			if( data == 0 )      SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			else if( data == 3 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			break;

		case	0xF000:
			((Mapper183 *)mapper)->irq_counter = (((Mapper183 *)mapper)->irq_counter&0xFF00)|data;
			break;
		case	0xF004:
			((Mapper183 *)mapper)->irq_counter = (((Mapper183 *)mapper)->irq_counter&0x00FF)|(data<<8);
			break;
		case	0xF008:
			((Mapper183 *)mapper)->irq_enable = data;
			CPU_ClrIRQ( ((Mapper183 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper183_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper183 *)mapper)->irq_enable & 0x02 ) {
		if( ((Mapper183 *)mapper)->irq_counter <= 113 ) {
			((Mapper183 *)mapper)->irq_counter = 0;
//			CPU_IRQ_NotPending(((Mapper183 *)mapper)->nes->cpu);
			CPU_SetIRQ( ((Mapper183 *)mapper)->nes->cpu, IRQ_MAPPER );
		} else {
			((Mapper183 *)mapper)->irq_counter -= 113;
		}
	}
}

void	Mapper183_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper183 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper183 *)mapper)->irq_enable;
	*((INT*)&p[ 9]) = ((Mapper183 *)mapper)->irq_counter;
}

void	Mapper183_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper183 *)mapper)->reg[i] = p[i];
	}
	((Mapper183 *)mapper)->irq_enable  = p[8];
	((Mapper183 *)mapper)->irq_counter = *((INT*)&p[ 9]);
}

BOOL	Mapper183_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper183_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper183));
	
	memset(mapper, 0, sizeof(Mapper183));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper183_Reset;
	mapper->vtbl.Write = Mapper183_Write;
	mapper->vtbl.HSync = Mapper183_HSync;
	mapper->vtbl.SaveState = Mapper183_SaveState;
	mapper->vtbl.LoadState = Mapper183_LoadState;
	mapper->vtbl.IsStateSave = Mapper183_IsStateSave;

	return (Mapper *)mapper;
}

