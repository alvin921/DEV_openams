
//////////////////////////////////////////////////////////////////////////
// Mapper088  Namcot 118                                                //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg;
	BYTE	patch;
}Mapper088;

void	Mapper088_Reset(Mapper *mapper)
{
	DWORD crc;
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE >= 8 ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	((Mapper088 *)mapper)->patch = 0;

	crc = ROM_GetPROM_CRC(mapper->nes->rom);
	if( crc == 0xc1b6b2a6 ) { // Devil Man(J)
		((Mapper088 *)mapper)->patch = 1;
		NES_SetRenderMethod( mapper->nes, POST_RENDER );
	}
	if( crc == 0xd9803a35 ) {	// Quinty(J)
		NES_SetRenderMethod( mapper->nes, POST_RENDER );
	}
}

void	Mapper088_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			((Mapper088 *)mapper)->reg = data;
			if( ((Mapper088 *)mapper)->patch ) {
				if( data&0x40 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
				else	        SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			}
			break;
		case	0x8001:
			switch( ((Mapper088 *)mapper)->reg & 0x07 ) {
				case	0:
					SetVROM_2K_Bank( 0, data>>1 );
					break;
				case	1:
					SetVROM_2K_Bank( 2, data>>1 );
					break;
				case	2:
					SetVROM_1K_Bank( 4, data+0x40 );
					break;
				case	3:
					SetVROM_1K_Bank( 5, data+0x40 );
					break;
				case	4:
					SetVROM_1K_Bank( 6, data+0x40 );
					break;
				case	5:
					SetVROM_1K_Bank( 7, data+0x40 );
					break;
				case	6:
					SetPROM_8K_Bank( 4, data );
					break;
				case	7:
					SetPROM_8K_Bank( 5, data );
					break;
			}
			break;
		case	0xC000:
			if( data ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
			else	   SetVRAM_Mirror_cont( VRAM_MIRROR4L );
			break;
	}
}

void	Mapper088_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper088 *)mapper)->reg;
}

void	Mapper088_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper088 *)mapper)->reg = p[0];
}

BOOL	Mapper088_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper088_New(NES* parent)
{
	Mapper *mapper =(Mapper *) MALLOC(sizeof(Mapper088));
	
	memset(mapper, 0, sizeof(Mapper088));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper088_Reset;
	mapper->vtbl.Write = Mapper088_Write;
	mapper->vtbl.SaveState = Mapper088_SaveState;
	mapper->vtbl.LoadState = Mapper088_LoadState;
	mapper->vtbl.IsStateSave = Mapper088_IsStateSave;

	return (Mapper *)mapper;
}

