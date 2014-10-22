//////////////////////////////////////////////////////////////////////////
// Mapper068  SunSoft (After Burner II)                                 //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[4];
	BYTE	coin;
}Mapper068;
void	Mapper068_SetBank(Mapper *mapper);

void	Mapper068_Reset(Mapper *mapper)
{
	((Mapper068 *)mapper)->reg[0] = ((Mapper068 *)mapper)->reg[1] = ((Mapper068 *)mapper)->reg[2] = ((Mapper068 *)mapper)->reg[3] = 0;
	((Mapper068 *)mapper)->coin = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

#if	0
BYTE	Mapper068_ExRead( WORD addr )
{
	if( addr == 0x4020 ) {
DebugOut( "RD $4020:%02X\n", ((Mapper068 *)mapper)->coin );
		return	((Mapper068 *)mapper)->coin;
	}

	return	addr>>8;
}

void	Mapper068_ExWrite( WORD addr, BYTE data )
{
	if( addr == 0x4020 ) {
DebugOut( "WR $4020:%02X\n", data );
		((Mapper068 *)mapper)->coin = data;
	}
}
#endif

void	Mapper068_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xF000 ) {
		case	0x8000:
			SetVROM_2K_Bank( 0, data );
			break;
		case	0x9000:
			SetVROM_2K_Bank( 2, data );
			break;
		case	0xA000:
			SetVROM_2K_Bank( 4, data );
			break;
		case	0xB000:
			SetVROM_2K_Bank( 6, data );
			break;

		case	0xC000:
			((Mapper068 *)mapper)->reg[2] = data;
			Mapper068_SetBank(mapper);
			break;
		case	0xD000:
			((Mapper068 *)mapper)->reg[3] = data;
			Mapper068_SetBank(mapper);
			break;
		case	0xE000:
			((Mapper068 *)mapper)->reg[0] = (data & 0x10)>>4;
			((Mapper068 *)mapper)->reg[1] = data & 0x03;
			Mapper068_SetBank(mapper);
			break;

		case	0xF000:
			SetPROM_16K_Bank( 4, data );
			break;
	}
}

void	Mapper068_SetBank(Mapper *mapper)
{
	if( ((Mapper068 *)mapper)->reg[0] ) {
		switch( ((Mapper068 *)mapper)->reg[1] ) {
			case	0:
				SetVROM_1K_Bank(  8, (INT)((Mapper068 *)mapper)->reg[2]+0x80 );
				SetVROM_1K_Bank(  9, (INT)((Mapper068 *)mapper)->reg[3]+0x80 );
				SetVROM_1K_Bank( 10, (INT)((Mapper068 *)mapper)->reg[2]+0x80 );
				SetVROM_1K_Bank( 11, (INT)((Mapper068 *)mapper)->reg[3]+0x80 );
				break;
			case	1:
				SetVROM_1K_Bank(  8, (INT)((Mapper068 *)mapper)->reg[2]+0x80 );
				SetVROM_1K_Bank(  9, (INT)((Mapper068 *)mapper)->reg[2]+0x80 );
				SetVROM_1K_Bank( 10, (INT)((Mapper068 *)mapper)->reg[3]+0x80 );
				SetVROM_1K_Bank( 11, (INT)((Mapper068 *)mapper)->reg[3]+0x80 );
				break;
			case	2:
				SetVROM_1K_Bank(  8, (INT)((Mapper068 *)mapper)->reg[2]+0x80 );
				SetVROM_1K_Bank(  9, (INT)((Mapper068 *)mapper)->reg[2]+0x80 );
				SetVROM_1K_Bank( 10, (INT)((Mapper068 *)mapper)->reg[2]+0x80 );
				SetVROM_1K_Bank( 11, (INT)((Mapper068 *)mapper)->reg[2]+0x80 );
				break;
			case	3:
				SetVROM_1K_Bank(  8, (INT)((Mapper068 *)mapper)->reg[3]+0x80 );
				SetVROM_1K_Bank(  9, (INT)((Mapper068 *)mapper)->reg[3]+0x80 );
				SetVROM_1K_Bank( 10, (INT)((Mapper068 *)mapper)->reg[3]+0x80 );
				SetVROM_1K_Bank( 11, (INT)((Mapper068 *)mapper)->reg[3]+0x80 );
				break;
		}
	} else {
		switch( ((Mapper068 *)mapper)->reg[1] ) {
			case	0:
				SetVRAM_Mirror_cont( VRAM_VMIRROR );
				break;
			case	1:
				SetVRAM_Mirror_cont( VRAM_HMIRROR );
				break;
			case	2:
				SetVRAM_Mirror_cont( VRAM_MIRROR4L );
				break;
			case	3:
				SetVRAM_Mirror_cont( VRAM_MIRROR4H );
				break;
		}
	}
}

void	Mapper068_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper068 *)mapper)->reg[0];
	p[1] = ((Mapper068 *)mapper)->reg[1];
	p[2] = ((Mapper068 *)mapper)->reg[2];
	p[3] = ((Mapper068 *)mapper)->reg[3];
}

void	Mapper068_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper068 *)mapper)->reg[0] = p[0];
	((Mapper068 *)mapper)->reg[1] = p[1];
	((Mapper068 *)mapper)->reg[2] = p[2];
	((Mapper068 *)mapper)->reg[3] = p[3];
}

BOOL	Mapper068_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper068_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper068));
	
	memset(mapper, 0, sizeof(Mapper068));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper068_Reset;
	mapper->vtbl.Write = Mapper068_Write;
	mapper->vtbl.SaveState = Mapper068_SaveState;
	mapper->vtbl.LoadState = Mapper068_LoadState;
	mapper->vtbl.IsStateSave = Mapper068_IsStateSave;

	return (Mapper *)mapper;
}


