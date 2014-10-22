
//////////////////////////////////////////////////////////////////////////
// Mapper076  Namcot 109 (—_“]¶)                                     //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg;
}Mapper076;

void	Mapper076_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE >= 8 ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper076_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			((Mapper076 *)mapper)->reg = data;
			break;
		case	0x8001:
			switch( ((Mapper076 *)mapper)->reg & 0x07 ) {
				case	2:
					SetVROM_2K_Bank( 0, data );
					break;
				case	3:
					SetVROM_2K_Bank( 2, data );
					break;
				case	4:
					SetVROM_2K_Bank( 4, data );
					break;
				case	5:
					SetVROM_2K_Bank( 6, data );
					break;
				case	6:
					SetPROM_8K_Bank( 4, data );
					break;
				case	7:
					SetPROM_8K_Bank( 5, data );
					break;
			}
			break;
	}
}

void	Mapper076_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper076 *)mapper)->reg;
}

void	Mapper076_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper076 *)mapper)->reg = p[0];
}

BOOL	Mapper076_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper076_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper076));
	
	memset(mapper, 0, sizeof(Mapper076));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper076_Reset;
	mapper->vtbl.Write = Mapper076_Write;
	mapper->vtbl.SaveState = Mapper076_SaveState;
	mapper->vtbl.LoadState = Mapper076_LoadState;
	mapper->vtbl.IsStateSave = Mapper076_IsStateSave;

	return (Mapper *)mapper;
}

