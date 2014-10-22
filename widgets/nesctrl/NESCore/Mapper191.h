//////////////////////////////////////////////////////////////////////////
// Mapper191 SACHEN Super Cartridge Xin1 (Ver.1-9)                      //
//           SACHEN Q-BOY Support                                       //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr0, chr1, chr2, chr3;
	BYTE	highbank;
}Mapper191;
void	Mapper191_SetBank_CPU(Mapper *mapper);
void	Mapper191_SetBank_PPU(Mapper *mapper);


void	Mapper191_Reset(Mapper *mapper)
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper191 *)mapper)->reg[i] = 0x00;
	}

	((Mapper191 *)mapper)->prg0 = 0;
//	((Mapper191 *)mapper)->prg1 = 1;
	Mapper191_SetBank_CPU(mapper);

	((Mapper191 *)mapper)->chr0 = 0;
	((Mapper191 *)mapper)->chr1 = 0;
	((Mapper191 *)mapper)->chr2 = 0;
	((Mapper191 *)mapper)->chr3 = 0;
	((Mapper191 *)mapper)->highbank = 0;
	Mapper191_SetBank_PPU(mapper);
}

void	Mapper191_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x4100:
			((Mapper191 *)mapper)->reg[0]=data;
			break;
		case	0x4101:
			((Mapper191 *)mapper)->reg[1]=data;
			switch( ((Mapper191 *)mapper)->reg[0] ) {
				case	0:
					((Mapper191 *)mapper)->chr0=data&7;
					Mapper191_SetBank_PPU(mapper);
					break;
				case	1:
					((Mapper191 *)mapper)->chr1=data&7;
					Mapper191_SetBank_PPU(mapper);
					break;
				case	2:
					((Mapper191 *)mapper)->chr2=data&7;
					Mapper191_SetBank_PPU(mapper);
					break;
				case	3:
					((Mapper191 *)mapper)->chr3=data&7;
					Mapper191_SetBank_PPU(mapper);
					break;
				case	4:
					((Mapper191 *)mapper)->highbank=data&7;
					Mapper191_SetBank_PPU(mapper);
					break;
				case	5:
					((Mapper191 *)mapper)->prg0=data&7;
					Mapper191_SetBank_CPU(mapper);
					break;
				case	7:
					if( data & 0x02 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
					else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
					break;
			}
			break;
	}
}

void	Mapper191_SetBank_CPU(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( ((Mapper191 *)mapper)->prg0 );
}

void	Mapper191_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		SetVROM_1K_Bank( 0, (((((Mapper191 *)mapper)->highbank<<3)+((Mapper191 *)mapper)->chr0)<<2)+0 );
		SetVROM_1K_Bank( 1, (((((Mapper191 *)mapper)->highbank<<3)+((Mapper191 *)mapper)->chr0)<<2)+1 );
		SetVROM_1K_Bank( 2, (((((Mapper191 *)mapper)->highbank<<3)+((Mapper191 *)mapper)->chr1)<<2)+2 );
		SetVROM_1K_Bank( 3, (((((Mapper191 *)mapper)->highbank<<3)+((Mapper191 *)mapper)->chr1)<<2)+3 );
		SetVROM_1K_Bank( 4, (((((Mapper191 *)mapper)->highbank<<3)+((Mapper191 *)mapper)->chr2)<<2)+0 );
		SetVROM_1K_Bank( 5, (((((Mapper191 *)mapper)->highbank<<3)+((Mapper191 *)mapper)->chr2)<<2)+1 );
		SetVROM_1K_Bank( 6, (((((Mapper191 *)mapper)->highbank<<3)+((Mapper191 *)mapper)->chr3)<<2)+2 );
		SetVROM_1K_Bank( 7, (((((Mapper191 *)mapper)->highbank<<3)+((Mapper191 *)mapper)->chr3)<<2)+3 );
	}
}

void	Mapper191_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper191 *)mapper)->prg0;
	p[1] = ((Mapper191 *)mapper)->chr0;
	p[2] = ((Mapper191 *)mapper)->chr1;
 	p[3] = ((Mapper191 *)mapper)->chr2;
	p[4] = ((Mapper191 *)mapper)->chr3;
	p[5] = ((Mapper191 *)mapper)->highbank;
}

void	Mapper191_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper191 *)mapper)->prg0     = p[0];
	((Mapper191 *)mapper)->chr0     = p[1];
	((Mapper191 *)mapper)->chr1     = p[2];
	((Mapper191 *)mapper)->chr2     = p[3];
	((Mapper191 *)mapper)->chr3     = p[4];
	((Mapper191 *)mapper)->highbank = p[5];
}

BOOL	Mapper191_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper191_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper191));
	
	memset(mapper, 0, sizeof(Mapper191));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper191_Reset;
	mapper->vtbl.WriteLow = Mapper191_WriteLow;
	mapper->vtbl.SaveState = Mapper191_SaveState;
	mapper->vtbl.LoadState = Mapper191_LoadState;
	mapper->vtbl.IsStateSave = Mapper191_IsStateSave;

	return (Mapper *)mapper;
}


