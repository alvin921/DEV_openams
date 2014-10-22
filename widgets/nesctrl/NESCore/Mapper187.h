//////////////////////////////////////////////////////////////////////////
// Mapper187  Street Fighter Zero 2 97                                  //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	prg[4];
	INT	chr[8];
	BYTE	bank[8];

	BYTE	ext_mode;
	BYTE	chr_mode;
	BYTE	ext_enable;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_occur;
	BYTE	last_write;
}Mapper187;
void	Mapper187_SetBank_CPU(Mapper *mapper);
void	Mapper187_SetBank_PPU(Mapper *mapper);

void	Mapper187_Reset(Mapper *mapper)
{
INT	i;

	for( i = 0; i < 8; i++ ) {
		((Mapper187 *)mapper)->chr[i] = 0x00;
		((Mapper187 *)mapper)->bank[i] = 0x00;
	}

	((Mapper187 *)mapper)->prg[0] = PROM_8K_SIZE-4;
	((Mapper187 *)mapper)->prg[1] = PROM_8K_SIZE-3;
	((Mapper187 *)mapper)->prg[2] = PROM_8K_SIZE-2;
	((Mapper187 *)mapper)->prg[3] = PROM_8K_SIZE-1;
	Mapper187_SetBank_CPU(mapper);

	((Mapper187 *)mapper)->ext_mode = 0;
	((Mapper187 *)mapper)->chr_mode = 0;
	((Mapper187 *)mapper)->ext_enable = 0;

	((Mapper187 *)mapper)->irq_enable = 0;
	((Mapper187 *)mapper)->irq_counter = 0;
	((Mapper187 *)mapper)->irq_latch = 0;

	((Mapper187 *)mapper)->last_write = 0;

	NES_SetRenderMethod( mapper->nes, POST_ALL_RENDER );
}

BYTE	Mapper187_ReadLow( Mapper *mapper, WORD addr )
{
	switch( ((Mapper187 *)mapper)->last_write&0x03 ) {
		case 0:
			return	0x83;
		case 1:
			return	0x83;
		case 2:
			return	0x42;
		case 3:
			return	0x00;
	}
	return 0;
}

void	Mapper187_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	((Mapper187 *)mapper)->last_write = data;
	if( addr == 0x5000 ) {
		((Mapper187 *)mapper)->ext_mode = data;
		if( data & 0x80 ) {
			if( data & 0x20 ) {
				((Mapper187 *)mapper)->prg[0] = ((data&0x1E)<<1)+0;
				((Mapper187 *)mapper)->prg[1] = ((data&0x1E)<<1)+1;
				((Mapper187 *)mapper)->prg[2] = ((data&0x1E)<<1)+2;
				((Mapper187 *)mapper)->prg[3] = ((data&0x1E)<<1)+3;
			} else {
				((Mapper187 *)mapper)->prg[2] = ((data&0x1F)<<1)+0;
				((Mapper187 *)mapper)->prg[3] = ((data&0x1F)<<1)+1;
			}
		} else {
			((Mapper187 *)mapper)->prg[0] = ((Mapper187 *)mapper)->bank[6];
			((Mapper187 *)mapper)->prg[1] = ((Mapper187 *)mapper)->bank[7];
			((Mapper187 *)mapper)->prg[2] = PROM_8K_SIZE-2;
			((Mapper187 *)mapper)->prg[3] = PROM_8K_SIZE-1;
		}
		Mapper187_SetBank_CPU(mapper);
	}
}

void	Mapper187_Write( Mapper *mapper, WORD addr, BYTE data )
{
	((Mapper187 *)mapper)->last_write = data;
	switch( addr ) {
		case	0x8003:
			((Mapper187 *)mapper)->ext_enable = 0xFF;
//			if( (data&0x80) != (((Mapper187 *)mapper)->chr_mode&0x80) ) {
//				for( INT i = 0; i < 4; i++ ) {
//					INT temp = ((Mapper187 *)mapper)->chr[i];
//					((Mapper187 *)mapper)->chr[i] = ((Mapper187 *)mapper)->chr[i+4];
//					((Mapper187 *)mapper)->chr[i+4] = temp;
//				}
//				Mapper187_SetBank_PPU(mapper);
//			}
			((Mapper187 *)mapper)->chr_mode = data;
			if( (data&0xF0) == 0 ) {
				((Mapper187 *)mapper)->prg[2] = PROM_8K_SIZE-2;
				Mapper187_SetBank_CPU(mapper);
			}
			break;

		case	0x8000:
			((Mapper187 *)mapper)->ext_enable = 0;
//			if( (data&0x80) != (((Mapper187 *)mapper)->chr_mode&0x80) ) {
//				for( INT i = 0; i < 4; i++ ) {
//					INT temp = ((Mapper187 *)mapper)->chr[i];
//					((Mapper187 *)mapper)->chr[i] = ((Mapper187 *)mapper)->chr[i+4];
//					((Mapper187 *)mapper)->chr[i+4] = temp;
//				}
//				Mapper187_SetBank_PPU(mapper);
//			}
			((Mapper187 *)mapper)->chr_mode = data;
			break;

		case	0x8001:
			if( !((Mapper187 *)mapper)->ext_enable ) {
				switch( ((Mapper187 *)mapper)->chr_mode & 7 ) {
					case	0:
						data &= 0xFE;
						((Mapper187 *)mapper)->chr[4] = (INT)data+0x100;
						((Mapper187 *)mapper)->chr[5] = (INT)data+0x100+1;
//						((Mapper187 *)mapper)->chr[0+((((Mapper187 *)mapper)->chr_mode&0x80)?4:0)] = data;
//						((Mapper187 *)mapper)->chr[1+((((Mapper187 *)mapper)->chr_mode&0x80)?4:0)] = data+1;
						Mapper187_SetBank_PPU(mapper);
						break;
					case	1:
						data &= 0xFE;
						((Mapper187 *)mapper)->chr[6] = (INT)data+0x100;
						((Mapper187 *)mapper)->chr[7] = (INT)data+0x100+1;
//						((Mapper187 *)mapper)->chr[2+((((Mapper187 *)mapper)->chr_mode&0x80)?4:0)] = data;
//						((Mapper187 *)mapper)->chr[3+((((Mapper187 *)mapper)->chr_mode&0x80)?4:0)] = data+1;
						Mapper187_SetBank_PPU(mapper);
						break;
					case	2:
						((Mapper187 *)mapper)->chr[0] = data;
//						((Mapper187 *)mapper)->chr[0+((((Mapper187 *)mapper)->chr_mode&0x80)?0:4)] = data;
						Mapper187_SetBank_PPU(mapper);
						break;
					case	3:
						((Mapper187 *)mapper)->chr[1] = data;
//						((Mapper187 *)mapper)->chr[1+((((Mapper187 *)mapper)->chr_mode&0x80)?0:4)] = data;
						Mapper187_SetBank_PPU(mapper);
						break;
					case	4:
						((Mapper187 *)mapper)->chr[2] = data;
//						((Mapper187 *)mapper)->chr[2+((((Mapper187 *)mapper)->chr_mode&0x80)?0:4)] = data;
						Mapper187_SetBank_PPU(mapper);
						break;
					case	5:
						((Mapper187 *)mapper)->chr[3] = data;
//						((Mapper187 *)mapper)->chr[3+((((Mapper187 *)mapper)->chr_mode&0x80)?0:4)] = data;
						Mapper187_SetBank_PPU(mapper);
						break;
					case	6:
						if( (((Mapper187 *)mapper)->ext_mode&0xA0)!=0xA0 ) {
							((Mapper187 *)mapper)->prg[0] = data;
							Mapper187_SetBank_CPU(mapper);
						}
						break;
					case	7:
						if( (((Mapper187 *)mapper)->ext_mode&0xA0)!=0xA0 ) {
							((Mapper187 *)mapper)->prg[1] = data;
							Mapper187_SetBank_CPU(mapper);
						}
						break;
					default:
						break;
				}
			} else {
				switch( ((Mapper187 *)mapper)->chr_mode ) {
					case	0x2A:
						((Mapper187 *)mapper)->prg[1] = 0x0F;
						break;
					case	0x28:
						((Mapper187 *)mapper)->prg[2] = 0x17;
						break;
					case	0x26:
						break;
					default:
						break;
				}
				Mapper187_SetBank_CPU(mapper);
			}
			((Mapper187 *)mapper)->bank[((Mapper187 *)mapper)->chr_mode&7] = data;
			break;

		case	0xA000:
			if( data & 0x01 ) {
				SetVRAM_Mirror_cont( VRAM_HMIRROR );
			} else {
				SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			break;

		case	0xC000:
			((Mapper187 *)mapper)->irq_counter = data;
			((Mapper187 *)mapper)->irq_occur = 0;
			CPU_ClrIRQ( ((Mapper187 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xC001:
			((Mapper187 *)mapper)->irq_latch = data;
			((Mapper187 *)mapper)->irq_occur = 0;
			CPU_ClrIRQ( ((Mapper187 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE000:
		case	0xE002:
			((Mapper187 *)mapper)->irq_enable = 0;
			((Mapper187 *)mapper)->irq_occur = 0;
			CPU_ClrIRQ( ((Mapper187 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
		case	0xE003:
			((Mapper187 *)mapper)->irq_enable = 1;
			((Mapper187 *)mapper)->irq_occur = 0;
			CPU_ClrIRQ( ((Mapper187 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
	}
}

void	Mapper187_Clock( Mapper *mapper, INT cycles )
{
//	if( ((Mapper187 *)mapper)->irq_occur ) {
//		((Mapper187 *)mapper)->nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper187_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline <= SCREEN_HEIGHT-1) ) {
		if( PPU_IsDispON(((Mapper187 *)mapper)->nes->ppu) ) {
			if( ((Mapper187 *)mapper)->irq_enable ) {
				if( !((Mapper187 *)mapper)->irq_counter ) {
					((Mapper187 *)mapper)->irq_counter--;
					((Mapper187 *)mapper)->irq_enable = 0;
					((Mapper187 *)mapper)->irq_occur = 0xFF;
					CPU_SetIRQ( ((Mapper187 *)mapper)->nes->cpu, IRQ_MAPPER );
				} else {
					((Mapper187 *)mapper)->irq_counter--;
				}
			}
		}
	}
}

void	Mapper187_SetBank_CPU(Mapper *mapper)
{
	SetPROM_32K_Bank( ((Mapper187 *)mapper)->prg[0], ((Mapper187 *)mapper)->prg[1], ((Mapper187 *)mapper)->prg[2], ((Mapper187 *)mapper)->prg[3] );
}

void	Mapper187_SetBank_PPU(Mapper *mapper)
{
	SetVROM_8K_Bank( ((Mapper187 *)mapper)->chr[0], ((Mapper187 *)mapper)->chr[1], ((Mapper187 *)mapper)->chr[2], ((Mapper187 *)mapper)->chr[3],
			 ((Mapper187 *)mapper)->chr[4], ((Mapper187 *)mapper)->chr[5], ((Mapper187 *)mapper)->chr[6], ((Mapper187 *)mapper)->chr[7] );
}

void	Mapper187_SaveState(Mapper *mapper, LPBYTE p )
{
INT	i;

	for( i = 0; i < 4; i++ ) {
		p[i] = ((Mapper187 *)mapper)->prg[i];
	}
	for( i = 0; i < 8; i++ ) {
		p[4+i] = ((Mapper187 *)mapper)->bank[i];
	}
	for( i = 0; i < 8; i++ ) {
		*((INT*)&p[12+i*sizeof(INT)]) = ((Mapper187 *)mapper)->chr[i];
	}

	p[44] = ((Mapper187 *)mapper)->ext_mode;
	p[45] = ((Mapper187 *)mapper)->chr_mode;
	p[46] = ((Mapper187 *)mapper)->ext_enable;
	p[47] = ((Mapper187 *)mapper)->irq_enable;
	p[48] = ((Mapper187 *)mapper)->irq_counter;
	p[49] = ((Mapper187 *)mapper)->irq_latch;
	p[50] = ((Mapper187 *)mapper)->irq_occur;
	p[51] = ((Mapper187 *)mapper)->last_write;
}

void	Mapper187_LoadState( Mapper *mapper, LPBYTE p )
{
INT	i;

	for( i = 0; i < 4; i++ ) {
		((Mapper187 *)mapper)->prg[i] = p[i];
	}
	for( i = 0; i < 8; i++ ) {
		((Mapper187 *)mapper)->bank[i] = p[4+i];
	}
	for( i = 0; i < 8; i++ ) {
		((Mapper187 *)mapper)->chr[i] = *((INT*)&p[12+i*sizeof(INT)]);
	}
	((Mapper187 *)mapper)->ext_mode    = p[44];
	((Mapper187 *)mapper)->chr_mode    = p[45];
	((Mapper187 *)mapper)->ext_enable  = p[46];
	((Mapper187 *)mapper)->irq_enable  = p[47];
	((Mapper187 *)mapper)->irq_counter = p[48];
	((Mapper187 *)mapper)->irq_latch   = p[49];
	((Mapper187 *)mapper)->irq_occur   = p[50];
	((Mapper187 *)mapper)->last_write  = p[51];
}

BOOL	Mapper187_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper187_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper187));
	
	memset(mapper, 0, sizeof(Mapper187));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper187_Reset;
	mapper->vtbl.Write = Mapper187_Write;
	mapper->vtbl.ReadLow = Mapper187_ReadLow;
	mapper->vtbl.WriteLow = Mapper187_WriteLow;
	mapper->vtbl.Clock = Mapper187_Clock;
	mapper->vtbl.HSync = Mapper187_HSync;
	mapper->vtbl.SaveState = Mapper187_SaveState;
	mapper->vtbl.LoadState = Mapper187_LoadState;
	mapper->vtbl.IsStateSave = Mapper187_IsStateSave;

	return (Mapper *)mapper;
}


