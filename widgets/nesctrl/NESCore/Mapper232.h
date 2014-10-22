
BOOL	Mapper232_IsStateSave(Mapper *mapper) { return TRUE; }

//////////////////////////////////////////////////////////////////////////
// Mapper232  Quattro Games                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE    reg[2];
}Mapper232;

void	Mapper232_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	((Mapper232 *)mapper)->reg[0] = 0x0C;
	((Mapper232 *)mapper)->reg[1] = 0x00;
}

void	Mapper232_Write( Mapper *mapper, WORD addr, BYTE data )
{
//	if( addr == 0x9000 ) {
//		((Mapper232 *)mapper)->reg[0] = (data & 0x18)>>1;
//	} else if( addr >= 0xA000 && addr <= 0xFFFF ) {
//		((Mapper232 *)mapper)->reg[1] = data & 0x03;
//	}
	if( addr <= 0x9FFF ) {
		((Mapper232 *)mapper)->reg[0] = (data & 0x18)>>1;
	} else {
		((Mapper232 *)mapper)->reg[1] = data & 0x03;
	}

	SetPROM_8K_Bank( 4, (((Mapper232 *)mapper)->reg[0]|((Mapper232 *)mapper)->reg[1])*2+0 );
	SetPROM_8K_Bank( 5, (((Mapper232 *)mapper)->reg[0]|((Mapper232 *)mapper)->reg[1])*2+1 );
	SetPROM_8K_Bank( 6, (((Mapper232 *)mapper)->reg[0]|0x03)*2+0 );
	SetPROM_8K_Bank( 7, (((Mapper232 *)mapper)->reg[0]|0x03)*2+1 );
}

void	Mapper232_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr >= 0x6000 ) {
		Mapper232_Write( mapper, addr, data );
	}
}

void	Mapper232_SaveState( Mapper *mapper, LPBYTE p )
{
	p[ 0] = ((Mapper232 *)mapper)->reg[0];
	p[ 1] = ((Mapper232 *)mapper)->reg[1];
}

void	Mapper232_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper232 *)mapper)->reg[0] = p[ 0];
	((Mapper232 *)mapper)->reg[1] = p[ 1];
}
Mapper * Mapper232_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper232));
	
	memset(mapper, 0, sizeof(Mapper232));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper232_Reset;
	mapper->vtbl.WriteLow = Mapper232_WriteLow;
	mapper->vtbl.Write = Mapper232_Write;
	mapper->vtbl.SaveState = Mapper232_SaveState;
	mapper->vtbl.LoadState = Mapper232_LoadState;
	mapper->vtbl.IsStateSave = Mapper232_IsStateSave;

	return (Mapper *)mapper;
}

