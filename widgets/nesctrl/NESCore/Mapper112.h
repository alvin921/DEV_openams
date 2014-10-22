
//////////////////////////////////////////////////////////////////////////
// Mapper112  ASDER Mapper                                              //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[4];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
}Mapper112;
void	Mapper112_SetBank_CPU(Mapper *mapper);
void	Mapper112_SetBank_PPU(Mapper *mapper);


void	Mapper112_Reset(Mapper *mapper)
{
	INT i;

	for( i = 0; i < 4; i++ ) {
		((Mapper112 *)mapper)->reg[i] = 0x00;
	}

	((Mapper112 *)mapper)->prg0 = 0;
	((Mapper112 *)mapper)->prg1 = 1;
	Mapper112_SetBank_CPU(mapper);

	((Mapper112 *)mapper)->chr01 = 0;
	((Mapper112 *)mapper)->chr23 = 2;
	((Mapper112 *)mapper)->chr4  = 4;
	((Mapper112 *)mapper)->chr5  = 5;
	((Mapper112 *)mapper)->chr6  = 6;
	((Mapper112 *)mapper)->chr7  = 7;
	Mapper112_SetBank_PPU(mapper);
}

void	Mapper112_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			((Mapper112 *)mapper)->reg[0] = data;
			Mapper112_SetBank_CPU(mapper);
			Mapper112_SetBank_PPU(mapper);
			break;
		case	0xA000:
			((Mapper112 *)mapper)->reg[1] = data;
			switch( ((Mapper112 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper112 *)mapper)->prg0 = (data&(PROM_8K_SIZE-1));
					Mapper112_SetBank_CPU(mapper);
					break;
				case	0x01:
					((Mapper112 *)mapper)->prg1 = (data&(PROM_8K_SIZE-1));
					Mapper112_SetBank_CPU(mapper);
					break;
				case	0x02:
					((Mapper112 *)mapper)->chr01 = data & 0xFE;
					Mapper112_SetBank_PPU(mapper);
					break;
				case	0x03:
					((Mapper112 *)mapper)->chr23 = data & 0xFE;
					Mapper112_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper112 *)mapper)->chr4 = data;
					Mapper112_SetBank_PPU(mapper);
					break;
				case	0x05:
					((Mapper112 *)mapper)->chr5 = data;
					Mapper112_SetBank_PPU(mapper);
					break;
				case	0x06:
					((Mapper112 *)mapper)->chr6 = data;
					Mapper112_SetBank_PPU(mapper);
					break;
				case	0x07:
					((Mapper112 *)mapper)->chr7 = data;
					Mapper112_SetBank_PPU(mapper);
					break;
			}
			break;

		case	0xC000:
			((Mapper112 *)mapper)->reg[3] = data;
			Mapper112_SetBank_PPU(mapper);

		case	0xE000:
			((Mapper112 *)mapper)->reg[2] = data;
			if( !ROM_Is4SCREEN(mapper->nes->rom) ) {
				if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
				else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			Mapper112_SetBank_PPU(mapper);
			break;
	}
}

void	Mapper112_SetBank_CPU(Mapper *mapper)
{
	SetPROM_32K_Bank( ((Mapper112 *)mapper)->prg0, ((Mapper112 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper112_SetBank_PPU(Mapper *mapper)
{
	if( ((Mapper112 *)mapper)->reg[2] & 0x02 ) {
		SetVROM_8K_Bank( ((Mapper112 *)mapper)->chr01, ((Mapper112 *)mapper)->chr01+1, ((Mapper112 *)mapper)->chr23, ((Mapper112 *)mapper)->chr23+1, ((Mapper112 *)mapper)->chr4, ((Mapper112 *)mapper)->chr5, ((Mapper112 *)mapper)->chr6, ((Mapper112 *)mapper)->chr7 );
	} else {
		SetVROM_8K_Bank(((((Mapper112 *)mapper)->reg[3]<<6)&0x100)+((Mapper112 *)mapper)->chr01, 
				((((Mapper112 *)mapper)->reg[3]<<6)&0x100)+((Mapper112 *)mapper)->chr01+1, 
				((((Mapper112 *)mapper)->reg[3]<<5)&0x100)+((Mapper112 *)mapper)->chr23, 
				((((Mapper112 *)mapper)->reg[3]<<5)&0x100)+((Mapper112 *)mapper)->chr23+1, 
				((((Mapper112 *)mapper)->reg[3]<<4)&0x100)+((Mapper112 *)mapper)->chr4, 
				((((Mapper112 *)mapper)->reg[3]<<3)&0x100)+((Mapper112 *)mapper)->chr5, 
				((((Mapper112 *)mapper)->reg[3]<<2)&0x100)+((Mapper112 *)mapper)->chr6, 
				((((Mapper112 *)mapper)->reg[3]<<1)&0x100)+((Mapper112 *)mapper)->chr7 );
	}
}

void	Mapper112_SaveState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 4; i++ ) {
		p[i] = ((Mapper112 *)mapper)->reg[i];
	}
	p[ 4] = ((Mapper112 *)mapper)->chr01;
	p[ 5] = ((Mapper112 *)mapper)->chr23;
	p[ 6] = ((Mapper112 *)mapper)->chr4;
	p[ 7] = ((Mapper112 *)mapper)->chr5;
	p[ 8] = ((Mapper112 *)mapper)->chr6;
	p[ 9] = ((Mapper112 *)mapper)->chr7;
}

void	Mapper112_LoadState( Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 4; i++ ) {
		((Mapper112 *)mapper)->reg[i] = p[i];
	}

	((Mapper112 *)mapper)->chr01 = p[ 4];
	((Mapper112 *)mapper)->chr23 = p[ 5];
	((Mapper112 *)mapper)->chr4  = p[ 6];
	((Mapper112 *)mapper)->chr5  = p[ 7];
	((Mapper112 *)mapper)->chr6  = p[ 8];
	((Mapper112 *)mapper)->chr7  = p[ 9];
}

BOOL	Mapper112_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper112_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper112));
	
	memset(mapper, 0, sizeof(Mapper112));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper112_Reset;
	mapper->vtbl.Write = Mapper112_Write;
	mapper->vtbl.SaveState = Mapper112_SaveState;
	mapper->vtbl.LoadState = Mapper112_LoadState;
	mapper->vtbl.IsStateSave = Mapper112_IsStateSave;

	return (Mapper *)mapper;
}

