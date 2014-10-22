//////////////////////////////////////////////////////////////////////////
// Mapper016  Bandai Standard                                           //
//////////////////////////////////////////////////////////////////////////
#include "EEPROM.h"
typedef struct {
	INHERIT_MAPPER;

	BYTE	patch;	// For Famicom Jump 2
	BYTE	eeprom_type;	// EEPROM type

	BYTE	reg[3];

	BYTE	irq_enable;
	INT	irq_counter;
	INT	irq_latch;
	BYTE	irq_type;

	X24C01	x24c01;
	X24C02	x24c02;
}Mapper016;


void	Mapper016_WriteSubA(Mapper *mapper, WORD addr, BYTE data);
void	Mapper016_WriteSubB(Mapper *mapper, WORD addr, BYTE data);

void	Mapper016_Reset(Mapper *mapper)
{
	DWORD	crc;
	((Mapper016 *)mapper)->patch = 0;

	((Mapper016 *)mapper)->reg[0] = ((Mapper016 *)mapper)->reg[1] = ((Mapper016 *)mapper)->reg[2] = 0;
	((Mapper016 *)mapper)->irq_enable = 0;
	((Mapper016 *)mapper)->irq_counter = 0;
	((Mapper016 *)mapper)->irq_latch = 0;

	((Mapper016 *)mapper)->irq_type = 0;
	NES_SetIrqType( mapper->nes, IRQ_CLOCK );

	((Mapper016 *)mapper)->eeprom_type = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	crc = ROM_GetPROM_CRC(mapper->nes->rom);

	if( crc == 0x3f15d20d		// Famicom Jump 2(J)
	 || crc == 0xf76aa523 ) {	// Famicom Jump 2(J)(alt)
		((Mapper016 *)mapper)->patch = 1;
		((Mapper016 *)mapper)->eeprom_type = 0xFF;

		WRAM[0x0BBC] = 0xFF;	// SRAM‘Îô
	}

	if( crc == 0x1d6f27f7 ) {	// Dragon Ball Z 2(Korean Hack)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
		((Mapper016 *)mapper)->eeprom_type = 1;
	}
	if( crc == 0x6f7247c8 ) {	// Dragon Ball Z 3(Korean Hack)
		NES_SetIrqType( mapper->nes, IRQ_CLOCK );
		((Mapper016 *)mapper)->eeprom_type = 1;
	}

	if( crc == 0x7fb799fd ) {	// Dragon Ball 2 - Dai Maou Fukkatsu(J)
	}
	if( crc == 0x6c6c2feb		// Dragon Ball 3 - Gokuu Den(J)
	 || crc == 0x8edeb257 ) {	// Dragon Ball 3 - Gokuu Den(J)(Alt)
	}
	if( crc == 0x31cd9903 ) {	// Dragon Ball Z - Kyoushuu! Saiya Jin(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
	}
	if( crc == 0xe49fc53e		// Dragon Ball Z 2 - Gekishin Freeza!!(J)
	 || crc == 0x1582fee0 ) {	// Dragon Ball Z 2 - Gekishin Freeza!!(J) [alt]
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
		((Mapper016 *)mapper)->eeprom_type = 1;
	}
	if( crc == 0x09499f4d ) {	// Dragon Ball Z 3 - Ressen Jinzou Ningen(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
		((Mapper016 *)mapper)->eeprom_type = 1;
	}
	if( crc == 0x2e991109 ) {	// Dragon Ball Z Gaiden - Saiya Jin Zetsumetsu Keikaku (J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
		((Mapper016 *)mapper)->eeprom_type = 1;
	}
	if( crc == 0x146fb9c3 ) {	// SD Gundam Gaiden - Knight Gundam Monogatari(J)
	}

	if( crc == 0x73ac76db		// SD Gundam Gaiden - Knight Gundam Monogatari 2 - Hikari no Kishi(J)
	 || crc == 0x81a15eb8 ) {	// SD Gundam Gaiden - Knight Gundam Monogatari 3 - Densetsu no Kishi Dan(J)
		((Mapper016 *)mapper)->eeprom_type = 1;
	}
	if( crc == 0x170250de ) {	// Rokudenashi Blues(J)
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
		((Mapper016 *)mapper)->eeprom_type = 1;
	}

	// DATACHŒn
	if( crc == 0x0be0a328 		// Datach - SD Gundam - Gundam Wars(J)
	 || crc == 0x19e81461		// Datach - Dragon Ball Z - Gekitou Tenkaichi Budou Kai(J)
	 || crc == 0x5b457641		// Datach - Ultraman Club - Supokon Fight!(J)
	 || crc == 0x894efdbc		// Datach - Crayon Shin Chan - Ora to Poi Poi(J)
	 || crc == 0x983d8175		// Datach - Battle Rush - Build Up Robot Tournament(J)
	 || crc == 0xbe06853f ) {	// Datach - J League Super Top Players(J)
		((Mapper016 *)mapper)->eeprom_type = 2;
	}
	if( crc == 0xf51a7f46 ) {	// Datach - Yuu Yuu Hakusho - Bakutou Ankoku Bujutsu Kai(J)
		NES_SetIrqType( mapper->nes, IRQ_HSYNC );
		((Mapper016 *)mapper)->eeprom_type = 2;
	}

	if( ((Mapper016 *)mapper)->eeprom_type == 0 ) {
		NES_SetSAVERAM_SIZE( mapper->nes, 128 );
		X24C01_Reset(&((Mapper016 *)mapper)->x24c01, WRAM );
	} else 
	if( ((Mapper016 *)mapper)->eeprom_type == 1 ) {
		NES_SetSAVERAM_SIZE( mapper->nes, 256 );
		X24C02_Reset(&((Mapper016 *)mapper)->x24c02, WRAM );
	} else 
	if( ((Mapper016 *)mapper)->eeprom_type == 2 ) {
		NES_SetSAVERAM_SIZE( mapper->nes, 384 );
		X24C02_Reset(&((Mapper016 *)mapper)->x24c02, WRAM );
		X24C01_Reset(&((Mapper016 *)mapper)->x24c01, WRAM+256 );
	}
}

BYTE	Mapper016_ReadLow( Mapper *mapper, WORD addr )
{
	if( ((Mapper016 *)mapper)->patch ) {
		return	Mapper_ReadLow( mapper, addr );
	} else {
		if( (addr & 0x00FF) == 0x0000 ) {
			BYTE	ret = 0;
			if( ((Mapper016 *)mapper)->eeprom_type == 0 ) {
				ret = X24C01_Read(&((Mapper016 *)mapper)->x24c01);
			} else
			if( ((Mapper016 *)mapper)->eeprom_type == 1 ) {
				ret = X24C02_Read(&((Mapper016 *)mapper)->x24c02);
			} else
			if( ((Mapper016 *)mapper)->eeprom_type == 2 ) {
				ret = X24C02_Read(&((Mapper016 *)mapper)->x24c02) & X24C01_Read(&((Mapper016 *)mapper)->x24c01);
			}
			return	(ret?0x10:0)|(NES_GetBarcodeStatus(mapper->nes));
		}
	}
	return	0x00;
}

void	Mapper016_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( !((Mapper016 *)mapper)->patch ) {
		Mapper016_WriteSubA( mapper, addr, data );
	} else {
		Mapper_DefWriteLow( mapper, addr, data );
	}
}

void	Mapper016_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( !((Mapper016 *)mapper)->patch ) {
		Mapper016_WriteSubA( mapper, addr, data );
	} else {
		Mapper016_WriteSubB( mapper, addr, data );
	}
}

static	BYTE	eeprom_addinc;

// Normal mapper #16
void	Mapper016_WriteSubA( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0x000F ) {
		case	0x0000:
		case	0x0001:
		case	0x0002:
		case	0x0003:
		case	0x0004:
		case	0x0005:
		case	0x0006:
		case	0x0007:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( addr&0x0007, data );
			}
			if( ((Mapper016 *)mapper)->eeprom_type == 2 ) {
				((Mapper016 *)mapper)->reg[0] = data;
				X24C01_Write(&((Mapper016 *)mapper)->x24c01, (data&0x08)?0xFF:0, (((Mapper016 *)mapper)->reg[1]&0x40)?0xFF:0 );
			}
			break;

		case	0x0008:
			SetPROM_16K_Bank( 4, data );
			break;

		case	0x0009:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			break;

		case	0x000A:
			((Mapper016 *)mapper)->irq_enable = data & 0x01;
			((Mapper016 *)mapper)->irq_counter = ((Mapper016 *)mapper)->irq_latch;
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0x000B:
			((Mapper016 *)mapper)->irq_latch = (((Mapper016 *)mapper)->irq_latch & 0xFF00) | data;
			((Mapper016 *)mapper)->irq_counter = (((Mapper016 *)mapper)->irq_counter & 0xFF00) | data;
			break;
		case	0x000C:
			((Mapper016 *)mapper)->irq_latch = ((INT)data << 8) | (((Mapper016 *)mapper)->irq_latch & 0x00FF);
			((Mapper016 *)mapper)->irq_counter = ((INT)data << 8) | (((Mapper016 *)mapper)->irq_counter & 0x00FF);
			break;

		case	0x000D:
			// EEPTYPE0(DragonBallZ)
			if( ((Mapper016 *)mapper)->eeprom_type == 0 ) {
				X24C01_Write(&((Mapper016 *)mapper)->x24c01, (data&0x20)?0xFF:0, (data&0x40)?0xFF:0 );
			}
			// EEPTYPE1(DragonBallZ2,Z3,Z Gaiden)
			if( ((Mapper016 *)mapper)->eeprom_type == 1 ) {
				X24C02_Write(&((Mapper016 *)mapper)->x24c02, (data&0x20)?0xFF:0, (data&0x40)?0xFF:0 );
			}
			// EEPTYPE2(DATACH)
			if( ((Mapper016 *)mapper)->eeprom_type == 2 ) {
				((Mapper016 *)mapper)->reg[1] = data;
				X24C02_Write(&((Mapper016 *)mapper)->x24c02, (data&0x20)?0xFF:0, (data&0x40)?0xFF:0 );
				X24C01_Write(&((Mapper016 *)mapper)->x24c01, (((Mapper016 *)mapper)->reg[0]&0x08)?0xFF:0, (data&0x40)?0xFF:0 );
			}
			break;
	}
}

// Famicom Jump 2
void	Mapper016_WriteSubB( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
		case	0x8001:
		case	0x8002:
		case	0x8003:
			((Mapper016 *)mapper)->reg[0] = data & 0x01;
			SetPROM_8K_Bank( 4, ((Mapper016 *)mapper)->reg[0]*0x20+((Mapper016 *)mapper)->reg[2]*2+0 );
			SetPROM_8K_Bank( 5, ((Mapper016 *)mapper)->reg[0]*0x20+((Mapper016 *)mapper)->reg[2]*2+1 );
			break;
		case	0x8004:
		case	0x8005:
		case	0x8006:
		case	0x8007:
			((Mapper016 *)mapper)->reg[1] = data & 0x01;
			SetPROM_8K_Bank( 6, ((Mapper016 *)mapper)->reg[1]*0x20+0x1E );
			SetPROM_8K_Bank( 7, ((Mapper016 *)mapper)->reg[1]*0x20+0x1F );
			break;
		case	0x8008:
			((Mapper016 *)mapper)->reg[2] = data;
			SetPROM_8K_Bank( 4, ((Mapper016 *)mapper)->reg[0]*0x20+((Mapper016 *)mapper)->reg[2]*2+0 );
			SetPROM_8K_Bank( 5, ((Mapper016 *)mapper)->reg[0]*0x20+((Mapper016 *)mapper)->reg[2]*2+1 );
			SetPROM_8K_Bank( 6, ((Mapper016 *)mapper)->reg[1]*0x20+0x1E );
			SetPROM_8K_Bank( 7, ((Mapper016 *)mapper)->reg[1]*0x20+0x1F );
			break;

		case	0x8009:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror_cont( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			break;

		case	0x800A:
			((Mapper016 *)mapper)->irq_enable = data & 0x01;
			((Mapper016 *)mapper)->irq_counter = ((Mapper016 *)mapper)->irq_latch;

//			if( !((Mapper016 *)mapper)->irq_enable ) {
//				NES_cpu->mapper->nes, ClrIRQ( IRQ_MAPPER );
//			}
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			break;
		case	0x800B:
			((Mapper016 *)mapper)->irq_latch = (((Mapper016 *)mapper)->irq_latch & 0xFF00) | data;
			break;
		case	0x800C:
			((Mapper016 *)mapper)->irq_latch = ((INT)data << 8) | (((Mapper016 *)mapper)->irq_latch & 0x00FF);
			break;

		case	0x800D:
			break;
	}
}

void	Mapper016_HSync( Mapper *mapper, INT scanline )
{
	if( ((Mapper016 *)mapper)->irq_enable && (NES_GetIrqType(mapper->nes) == IRQ_HSYNC) ) {
		if( ((Mapper016 *)mapper)->irq_counter <= 113 ) {
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
//			mapper->nes->cpu->IRQ();
////			mapper->nes->cpu->IRQ_NotPending();
//			((Mapper016 *)mapper)->irq_enable = 0;
//			((Mapper016 *)mapper)->irq_counter = 0;
			((Mapper016 *)mapper)->irq_counter &= 0xFFFF;
		} else {
			((Mapper016 *)mapper)->irq_counter -= 113;
		}
	}
}

void	Mapper016_Clock( Mapper *mapper, INT cycles )
{
	if( ((Mapper016 *)mapper)->irq_enable && (NES_GetIrqType(mapper->nes) == IRQ_CLOCK) ) {
		if( (((Mapper016 *)mapper)->irq_counter-=cycles) <= 0 ) {
			CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
//			mapper->nes->cpu->IRQ();
////			mapper->nes->cpu->IRQ_NotPending();
//			((Mapper016 *)mapper)->irq_enable = 0;
//			((Mapper016 *)mapper)->irq_counter = 0;
			((Mapper016 *)mapper)->irq_counter &= 0xFFFF;
		}
	}
}

void	Mapper016_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper016 *)mapper)->reg[0];
	p[1] = ((Mapper016 *)mapper)->reg[1];
	p[2] = ((Mapper016 *)mapper)->reg[2];
	p[3] = ((Mapper016 *)mapper)->irq_enable;
	*(INT*)&p[4] = ((Mapper016 *)mapper)->irq_counter;
	*(INT*)&p[8] = ((Mapper016 *)mapper)->irq_latch;

	if( ((Mapper016 *)mapper)->eeprom_type == 0 ) {
		X24C01_Save(&((Mapper016 *)mapper)->x24c01, &p[16] );
	} else
	if( ((Mapper016 *)mapper)->eeprom_type == 1 ) {
		X24C02_Save(&((Mapper016 *)mapper)->x24c02, &p[16] );
	} else
	if( ((Mapper016 *)mapper)->eeprom_type == 2 ) {
		X24C02_Save(&((Mapper016 *)mapper)->x24c02, &p[16] );
		X24C01_Save(&((Mapper016 *)mapper)->x24c01, &p[48] );
	}
}

void	Mapper016_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper016 *)mapper)->reg[0] = p[0];
	((Mapper016 *)mapper)->reg[1] = p[1];
	((Mapper016 *)mapper)->reg[2] = p[2];
	((Mapper016 *)mapper)->irq_enable  = p[3];
	((Mapper016 *)mapper)->irq_counter = *(INT*)&p[4];
	((Mapper016 *)mapper)->irq_latch   = *(INT*)&p[8];
	if( ((Mapper016 *)mapper)->eeprom_type == 0 ) {
		X24C01_Load(&((Mapper016 *)mapper)->x24c01, &p[16] );
	} else
	if( ((Mapper016 *)mapper)->eeprom_type == 1 ) {
		X24C02_Load(&((Mapper016 *)mapper)->x24c02, &p[16] );
	} else
	if( ((Mapper016 *)mapper)->eeprom_type == 2 ) {
		X24C02_Load(&((Mapper016 *)mapper)->x24c02, &p[16] );
		X24C01_Load(&((Mapper016 *)mapper)->x24c01, &p[48] );
	}
}

BOOL	Mapper016_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper016_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper016));
	
	memset(mapper, 0, sizeof(Mapper016));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper016_Reset;
	mapper->vtbl.ReadLow = Mapper016_ReadLow;
	mapper->vtbl.WriteLow = Mapper016_WriteLow;
	mapper->vtbl.Write = Mapper016_Write;
	mapper->vtbl.HSync = Mapper016_HSync;
	mapper->vtbl.Clock = Mapper016_Clock;
	mapper->vtbl.SaveState = Mapper016_SaveState;
	mapper->vtbl.LoadState = Mapper016_LoadState;
	mapper->vtbl.IsStateSave = Mapper016_IsStateSave;

	X24C01_Reset(&((Mapper016*)mapper)->x24c01, NULL);
	X24C02_Reset(&((Mapper016*)mapper)->x24c02, NULL);
	return (Mapper *)mapper;
}

