//////////////////////////////////////////////////////////////////////////
// Mapper198  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;

	BYTE	adr5000buf[1024*4];
}Mapper198;
void	Mapper198_SetBank_CPU(Mapper *mapper);
void	Mapper198_SetBank_PPU(Mapper *mapper);

void	Mapper198_Reset(Mapper *mapper)
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper198 *)mapper)->reg[i] = 0x00;
	}

	((Mapper198 *)mapper)->prg0 = 0;
	((Mapper198 *)mapper)->prg1 = 1;
	Mapper198_SetBank_CPU(mapper);

	((Mapper198 *)mapper)->chr01 = 0;
	((Mapper198 *)mapper)->chr23 = 2;
	((Mapper198 *)mapper)->chr4  = 4;
	((Mapper198 *)mapper)->chr5  = 5;
	((Mapper198 *)mapper)->chr6  = 6;
	((Mapper198 *)mapper)->chr7  = 7;
	Mapper198_SetBank_PPU(mapper);
}

void	Mapper198_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr>0x4018 && addr<0x6000 )
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	else
		((Mapper198 *)mapper)->adr5000buf[addr&0xFFF] = data;
}
BYTE	Mapper198_ReadLow( Mapper *mapper, WORD addr )
{
	if( addr>0x4018 && addr<0x6000 )
		return	CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	else
		return	((Mapper198 *)mapper)->adr5000buf[addr&0xFFF];
}

void	Mapper198_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper198 *)mapper)->reg[0] = data;
			Mapper198_SetBank_CPU(mapper);
			Mapper198_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper198 *)mapper)->reg[1] = data;

			switch( ((Mapper198 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper198 *)mapper)->chr01 = data & 0xFE;
					Mapper198_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper198 *)mapper)->chr23 = data & 0xFE;
					Mapper198_SetBank_PPU(mapper);
					break;
				case	0x02:
					((Mapper198 *)mapper)->chr4 = data;
					Mapper198_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper198 *)mapper)->chr5 = data;
					Mapper198_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper198 *)mapper)->chr6 = data;
					Mapper198_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper198 *)mapper)->chr7 = data;
					Mapper198_SetBank_PPU(mapper);
					break;
				case	0x06:
					if(data>=0x50) data&=0x4F;
					((Mapper198 *)mapper)->prg0 = data;
					Mapper198_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper198 *)mapper)->prg1 = data;
					Mapper198_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper198 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(((Mapper198 *)mapper)->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper198 *)mapper)->reg[3] = data;
			break;
		case	0xC000:
			((Mapper198 *)mapper)->reg[4] = data;
			break;
		case	0xC001:
			((Mapper198 *)mapper)->reg[5] = data;
			break;
		case	0xE000:
			((Mapper198 *)mapper)->reg[6] = data;
			break;
		case	0xE001:
			((Mapper198 *)mapper)->reg[7] = data;
			break;
	}
}

void	Mapper198_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper198 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, ((Mapper198 *)mapper)->prg1, ((Mapper198 *)mapper)->prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( ((Mapper198 *)mapper)->prg0, ((Mapper198 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper198_SetBank_PPU(Mapper *mapper)
{

	if( VROM_1K_SIZE ) {
		if( ((Mapper198 *)mapper)->reg[0] & 0x80 ) {
			SetVROM_8K_Bank( ((Mapper198 *)mapper)->chr4, ((Mapper198 *)mapper)->chr5, ((Mapper198 *)mapper)->chr6, ((Mapper198 *)mapper)->chr7,
					 ((Mapper198 *)mapper)->chr01, ((Mapper198 *)mapper)->chr01+1, ((Mapper198 *)mapper)->chr23, ((Mapper198 *)mapper)->chr23+1 );
		} else {
			SetVROM_8K_Bank( ((Mapper198 *)mapper)->chr01, ((Mapper198 *)mapper)->chr01+1, ((Mapper198 *)mapper)->chr23, ((Mapper198 *)mapper)->chr23+1,
					 ((Mapper198 *)mapper)->chr4, ((Mapper198 *)mapper)->chr5, ((Mapper198 *)mapper)->chr6, ((Mapper198 *)mapper)->chr7 );
		}
	}
}

void	Mapper198_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper198 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper198 *)mapper)->prg0;
	p[ 9] = ((Mapper198 *)mapper)->prg1;
	p[10] = ((Mapper198 *)mapper)->chr01;
	p[11] = ((Mapper198 *)mapper)->chr23;
	p[12] = ((Mapper198 *)mapper)->chr4;
	p[13] = ((Mapper198 *)mapper)->chr5;
	p[14] = ((Mapper198 *)mapper)->chr6;
	p[15] = ((Mapper198 *)mapper)->chr7;
}

void	Mapper198_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper198 *)mapper)->reg[i] = p[i];
	}
	((Mapper198 *)mapper)->prg0  = p[ 8];
	((Mapper198 *)mapper)->prg1  = p[ 9];
	((Mapper198 *)mapper)->chr01 = p[10];
	((Mapper198 *)mapper)->chr23 = p[11];
	((Mapper198 *)mapper)->chr4  = p[12];
	((Mapper198 *)mapper)->chr5  = p[13];
	((Mapper198 *)mapper)->chr6  = p[14];
	((Mapper198 *)mapper)->chr7  = p[15];
}

BOOL	Mapper198_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper198_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper198));
	
	memset(mapper, 0, sizeof(Mapper198));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper198_Reset;
	mapper->vtbl.Write = Mapper198_Write;
	mapper->vtbl.ReadLow = Mapper198_ReadLow;
	mapper->vtbl.WriteLow = Mapper198_WriteLow;
	mapper->vtbl.SaveState = Mapper198_SaveState;
	mapper->vtbl.LoadState = Mapper198_LoadState;
	mapper->vtbl.IsStateSave = Mapper198_IsStateSave;

	return (Mapper *)mapper;
}



