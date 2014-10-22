//////////////////////////////////////////////////////////////////////////
// Mapper190  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	cbase;		/* PowerOn OR RESET : cbase=0 */
	BYTE	mp190_lcchk;	/* PowerOn OR RESET */
	BYTE	mp190_lcmd;
	BYTE	mp190_cmd;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE    lowoutdata;
}Mapper190;

void	Mapper190_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

//	DWORD	crc = ((Mapper190 *)mapper)->nes->rom->GetPROM_CRC();
//	if( crc == 0x6F3D187A ) {
//		Temp_Buf=0;	//Kof96
//	} else {
//		Temp_Buf=1;	//ST97
//	}

	((Mapper190 *)mapper)->irq_enable = 0;
	((Mapper190 *)mapper)->irq_counter = 0;
	((Mapper190 *)mapper)->cbase = 0;		/* PowerOn OR RESET : ((Mapper190 *)mapper)->cbase=0 */
	((Mapper190 *)mapper)->mp190_lcchk = 0;	/* PowerOn OR RESET */
	((Mapper190 *)mapper)->mp190_lcmd = 1;
}

void	Mapper190_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	/* For Initial Copy Protect check (KOF'96) */
	if( addr == 0x5000 ) {
		((Mapper190 *)mapper)->mp190_lcmd = data;
		switch( data ) {
			case	0xE0:
				SetPROM_32K_Bank_cont( 0 );
				break;
			case	0xEE:
				SetPROM_32K_Bank_cont( 3 );
				break;
		}
	}
	if( (addr==0x5001) && (((Mapper190 *)mapper)->mp190_lcmd==0x00) ) {
		 SetPROM_32K_Bank_cont( 7 );
	}
	if( addr == 0x5080 ) {
		switch( data ) {
			case 0x1:((Mapper190 *)mapper)->lowoutdata=0x83;break;
			case 0x2:((Mapper190 *)mapper)->lowoutdata=0x42;break;
			case 0x3:((Mapper190 *)mapper)->lowoutdata=0x00;break;
		}
	}
	CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
}

BYTE	Mapper190_ReadLow( Mapper *mapper, WORD addr )
{
	switch( addr ) {
		case	0x5000:
			return	((Mapper190 *)mapper)->lowoutdata;
		default:
			return	CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	}
}

void	Mapper190_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE003 ) {
		case	0x8000:
			((Mapper190 *)mapper)->mp190_cmd = data;
			if( ((Mapper190 *)mapper)->mp190_cmd & 0x80 )
				((Mapper190 *)mapper)->cbase = 1;
			else
				((Mapper190 *)mapper)->cbase = 0;
			break;
		case	0x8003:	/* for Street Fighter Zero 2 '97 */
			((Mapper190 *)mapper)->mp190_lcchk = data;
			switch( data ) {
				case	0x28:
					SetPROM_8K_Bank( 4, 0x1F );
					SetPROM_8K_Bank( 5, 0x1F );
					SetPROM_8K_Bank( 6, 0x17 );
					SetPROM_8K_Bank( 7, 0x1F );
					break;
				case	0x2A:
					SetPROM_8K_Bank( 4, 0x1F );
					SetPROM_8K_Bank( 5, 0x0F );
					SetPROM_8K_Bank( 6, 0x17 );
					SetPROM_8K_Bank( 7, 0x1F );
					break;
				case 0x06:
					SetPROM_8K_Bank( 4, 0x1E );
					SetPROM_8K_Bank( 5, 0x1F );
					SetPROM_8K_Bank( 6, 0x1F );
					SetPROM_8K_Bank( 7, 0x1F );
					break;	
			}
			break;
		case	0x8001:
			if( (((Mapper190 *)mapper)->mp190_lcchk==0x6) || (((Mapper190 *)mapper)->mp190_lcmd==0x0) ) {
				switch( ((Mapper190 *)mapper)->mp190_cmd & 0x07 ) {
					case	0:
						if( ((Mapper190 *)mapper)->cbase == 0 ) {
							SetVROM_1K_Bank(0,data+0x100);
							SetVROM_1K_Bank(1,data+0x101);
						} else {
							SetVROM_1K_Bank(4,data+0x100);
							SetVROM_1K_Bank(5,data+0x101);
						}
						break;
					case	1:
						if( ((Mapper190 *)mapper)->cbase == 0 ) {
							SetVROM_1K_Bank(2,data+0x100);
							SetVROM_1K_Bank(3,data+0x101);
						} else {
							SetVROM_1K_Bank(6,data+0x100);
							SetVROM_1K_Bank(7,data+0x101);
						}
						break;
					case	2:
						if( ((Mapper190 *)mapper)->cbase == 0 ) {
							SetVROM_1K_Bank(4,data);
						} else {
							SetVROM_1K_Bank(0,data);
						}
						break;
					case	3:
						if( ((Mapper190 *)mapper)->cbase == 0 ) {
							SetVROM_1K_Bank(5,data);
						} else {
							SetVROM_1K_Bank(1,data);
						}
						break;
					case	4:
						if( ((Mapper190 *)mapper)->cbase == 0 ) {
							SetVROM_1K_Bank(6,data);
						} else {
							SetVROM_1K_Bank(2,data);
						}
						break;
					case	5:
						if( ((Mapper190 *)mapper)->cbase == 0 ) {
							SetVROM_1K_Bank(7,data);
						} else {
							SetVROM_1K_Bank(3,data);
						}
						break;
					case	6:
						data=data&((PROM_8K_SIZE*2)-1);
						if( ((Mapper190 *)mapper)->mp190_lcmd & 0x40 ) {
							SetPROM_8K_Bank(6,data);
							SetPROM_8K_Bank(4,(PROM_8K_SIZE-1)*2);
						} else {
							SetPROM_8K_Bank(4,data);
							SetPROM_8K_Bank(6,(PROM_8K_SIZE-1)*2);
						}
						break;
					case	7:
						data=data&((PROM_8K_SIZE*2)-1);
						if( ((Mapper190 *)mapper)->mp190_lcmd & 0x40 ) {
							SetPROM_8K_Bank(5,data);
							SetPROM_8K_Bank(4,(PROM_8K_SIZE-1)*2);
						} else {
							SetPROM_8K_Bank(5,data);
							SetPROM_8K_Bank(6,(PROM_8K_SIZE-1)*2);
						}
						break;
				}
			}
			break;
		case	0xA000:
		        if( (data&0x1) == 0x1 )
				SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else
				SetVRAM_Mirror_cont( VRAM_VMIRROR );
		        break;
		case	0xA001:
			break;
		case	0xC000:
			((Mapper190 *)mapper)->irq_counter = data-1;
			break;
		case	0xC001:
			((Mapper190 *)mapper)->irq_latch = data-1;
			break;
		case	0xC002:
			((Mapper190 *)mapper)->irq_counter = data;
			break;
		case	0xC003:
			((Mapper190 *)mapper)->irq_latch = data;
			break;
		case	0xE000:
			((Mapper190 *)mapper)->irq_counter = ((Mapper190 *)mapper)->irq_latch;
			((Mapper190 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( ((Mapper190 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE001:
			((Mapper190 *)mapper)->irq_enable = 1;
			break;
		case	0xE002:
			((Mapper190 *)mapper)->irq_counter = ((Mapper190 *)mapper)->irq_latch;
			((Mapper190 *)mapper)->irq_enable = 0;
			CPU_ClrIRQ( ((Mapper190 *)mapper)->nes->cpu, IRQ_MAPPER );
			break;
		case	0xE003:
			((Mapper190 *)mapper)->irq_enable = 1;
			((Mapper190 *)mapper)->irq_counter = ((Mapper190 *)mapper)->irq_counter;
			break;
	}
}

void	Mapper190_HSync( Mapper *mapper, INT scanline )
{
	if( (scanline >= 0 && scanline <= SCREEN_HEIGHT-1) ) {
		if( PPU_IsDispON(((Mapper190 *)mapper)->nes->ppu) ) {
			if( ((Mapper190 *)mapper)->irq_enable ) {
				if( !(((Mapper190 *)mapper)->irq_counter--) ) {
//					CPU_IRQ_NotPending(((Mapper190 *)mapper)->nes->cpu);
					CPU_SetIRQ( ((Mapper190 *)mapper)->nes->cpu, IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper190_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper190 *)mapper)->irq_enable;
	p[1] = ((Mapper190 *)mapper)->irq_counter;
	p[2] = ((Mapper190 *)mapper)->irq_latch;

	p[3] = ((Mapper190 *)mapper)->cbase;
	p[4] = ((Mapper190 *)mapper)->mp190_lcchk;
	p[5] = ((Mapper190 *)mapper)->mp190_lcmd;
	p[6] = ((Mapper190 *)mapper)->mp190_cmd;
	p[7] = ((Mapper190 *)mapper)->lowoutdata;
}

void	Mapper190_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper190 *)mapper)->irq_enable  = p[0];
	((Mapper190 *)mapper)->irq_counter = p[1];
	((Mapper190 *)mapper)->irq_latch   = p[2];

	((Mapper190 *)mapper)->cbase       = p[3];
	((Mapper190 *)mapper)->mp190_lcchk = p[4];
	((Mapper190 *)mapper)->mp190_lcmd  = p[5];
	((Mapper190 *)mapper)->mp190_cmd   = p[6];
	((Mapper190 *)mapper)->lowoutdata  = p[7];
}

BOOL	Mapper190_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper190_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper190));
	
	memset(mapper, 0, sizeof(Mapper190));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper190_Reset;
	mapper->vtbl.Write = Mapper190_Write;
	mapper->vtbl.ReadLow = Mapper190_ReadLow;
	mapper->vtbl.WriteLow = Mapper190_WriteLow;
	mapper->vtbl.HSync = Mapper190_HSync;
	mapper->vtbl.SaveState = Mapper190_SaveState;
	mapper->vtbl.LoadState = Mapper190_LoadState;
	mapper->vtbl.IsStateSave = Mapper190_IsStateSave;

	return (Mapper *)mapper;
}


