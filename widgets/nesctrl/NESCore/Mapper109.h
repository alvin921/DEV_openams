
//////////////////////////////////////////////////////////////////////////
// Mapper109 SACHEN The Great Wall SA-019                               //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg;
	BYTE	chr0, chr1, chr2, chr3;
	BYTE	chrmode0, chrmode1;
}Mapper109;
void	Mapper109_SetBank_PPU(Mapper *mapper);


void	Mapper109_Reset(Mapper *mapper)
{
	((Mapper109 *)mapper)->reg = 0;
	SetPROM_32K_Bank_cont( 0 );

	((Mapper109 *)mapper)->chr0 = 0;
	((Mapper109 *)mapper)->chr1 = 0;
	((Mapper109 *)mapper)->chr2 = 0;
	((Mapper109 *)mapper)->chr3 = 0;
	Mapper109_SetBank_PPU(mapper);
	((Mapper109 *)mapper)->chrmode0 = 0;
	((Mapper109 *)mapper)->chrmode1 = 0;
}

void	Mapper109_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x4100:
			((Mapper109 *)mapper)->reg = data;
			break;
		case	0x4101:
			switch( ((Mapper109 *)mapper)->reg ) {
				case	0:
					((Mapper109 *)mapper)->chr0 = data;
					Mapper109_SetBank_PPU(mapper);
					break;
				case	1:
					((Mapper109 *)mapper)->chr1 = data;
					Mapper109_SetBank_PPU(mapper);
					break;
				case	2:
					((Mapper109 *)mapper)->chr2 = data;
					Mapper109_SetBank_PPU(mapper);
					break;
				case	3:
					((Mapper109 *)mapper)->chr3 = data;
					Mapper109_SetBank_PPU(mapper);
					break;
				case	4:
					((Mapper109 *)mapper)->chrmode0 = data & 0x01;
					Mapper109_SetBank_PPU(mapper);
					break;
				case	5:
					SetPROM_32K_Bank_cont( data & 0x07 );
					break;
				case	6:
					((Mapper109 *)mapper)->chrmode1 = data & 0x07;
					Mapper109_SetBank_PPU(mapper);
					break;
				case	7:
					if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
					else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
					break;
			}
		break;
	}

}

void	Mapper109_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		SetVROM_1K_Bank( 0, ((Mapper109 *)mapper)->chr0 );
		SetVROM_1K_Bank( 1, ((Mapper109 *)mapper)->chr1|((((Mapper109 *)mapper)->chrmode1<<3)&0x8) );
		SetVROM_1K_Bank( 2, ((Mapper109 *)mapper)->chr2|((((Mapper109 *)mapper)->chrmode1<<2)&0x8) );
		SetVROM_1K_Bank( 3, ((Mapper109 *)mapper)->chr3|((((Mapper109 *)mapper)->chrmode1<<1)&0x8) |(((Mapper109 *)mapper)->chrmode0*0x10) );
		SetVROM_1K_Bank( 4, VROM_1K_SIZE-4 );
		SetVROM_1K_Bank( 5, VROM_1K_SIZE-3 );
		SetVROM_1K_Bank( 6, VROM_1K_SIZE-2 );
		SetVROM_1K_Bank( 7, VROM_1K_SIZE-1 );
	}
}

void	Mapper109_SaveState( Mapper *mapper, LPBYTE p )
{
	p[ 0] = ((Mapper109 *)mapper)->reg;
	p[ 1] = ((Mapper109 *)mapper)->chr0;
	p[ 2] = ((Mapper109 *)mapper)->chr1;
	p[ 3] = ((Mapper109 *)mapper)->chr2;
	p[ 4] = ((Mapper109 *)mapper)->chr3;
	p[ 5] = ((Mapper109 *)mapper)->chrmode0;
	p[ 6] = ((Mapper109 *)mapper)->chrmode1;
}

void	Mapper109_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper109 *)mapper)->reg      = p[ 0];
	((Mapper109 *)mapper)->chr0     = p[ 1];
	((Mapper109 *)mapper)->chr1     = p[ 2];
	((Mapper109 *)mapper)->chr2     = p[ 3];
	((Mapper109 *)mapper)->chr3     = p[ 4];
	((Mapper109 *)mapper)->chrmode0 = p[ 5];
	((Mapper109 *)mapper)->chrmode1 = p[ 6];
}

BOOL	Mapper109_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper109_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper109));
	
	memset(mapper, 0, sizeof(Mapper109));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper109_Reset;
	mapper->vtbl.WriteLow = Mapper109_WriteLow;
	mapper->vtbl.SaveState = Mapper109_SaveState;
	mapper->vtbl.LoadState = Mapper109_LoadState;
	mapper->vtbl.IsStateSave = Mapper109_IsStateSave;

	return (Mapper *)mapper;
}

