
//////////////////////////////////////////////////////////////////////////
// Mapper234  Maxi-15                                                   //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[2];
}Mapper234;
void	Mapper234_SetBank(Mapper *mapper);

void	Mapper234_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );

	((Mapper234 *)mapper)->reg[0] = 0;
	((Mapper234 *)mapper)->reg[1] = 0;
}

void	Mapper234_Read( Mapper *mapper, WORD addr, BYTE data)
{
	if( addr >= 0xFF80 && addr <= 0xFF9F ) {
		if( !((Mapper234 *)mapper)->reg[0] ) {
			((Mapper234 *)mapper)->reg[0] = data;
			Mapper234_SetBank(mapper);
		}
	}

	if( addr >= 0xFFE8 && addr <= 0xFFF7 ) {
		((Mapper234 *)mapper)->reg[1] = data;
		Mapper234_SetBank(mapper);
	}
}

void	Mapper234_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr >= 0xFF80 && addr <= 0xFF9F ) {
		if( !((Mapper234 *)mapper)->reg[0] ) {
			((Mapper234 *)mapper)->reg[0] = data;
			Mapper234_SetBank(mapper);
		}
	}

	if( addr >= 0xFFE8 && addr <= 0xFFF7 ) {
		((Mapper234 *)mapper)->reg[1] = data;
		Mapper234_SetBank(mapper);
	}
}

void	Mapper234_SetBank(Mapper *mapper)
{
	if( ((Mapper234 *)mapper)->reg[0] & 0x80 ) {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	}
	if( ((Mapper234 *)mapper)->reg[0] & 0x40 ) {
		SetPROM_32K_Bank_cont( (((Mapper234 *)mapper)->reg[0]&0x0E)|(((Mapper234 *)mapper)->reg[1]&0x01) );
		SetVROM_8K_Bank_cont( ((((Mapper234 *)mapper)->reg[0]&0x0E)<<2)|((((Mapper234 *)mapper)->reg[1]>>4)&0x07) );
	} else {
		SetPROM_32K_Bank_cont( ((Mapper234 *)mapper)->reg[0]&0x0F );
		SetVROM_8K_Bank_cont( ((((Mapper234 *)mapper)->reg[0]&0x0F)<<2)|((((Mapper234 *)mapper)->reg[1]>>4)&0x03) );
	}
}

void	Mapper234_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper234 *)mapper)->reg[0];
	p[1] = ((Mapper234 *)mapper)->reg[1];
}

void	Mapper234_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper234 *)mapper)->reg[0] = p[0];
	((Mapper234 *)mapper)->reg[1] = p[1];
}

BOOL	Mapper234_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper234_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper234));
	
	memset(mapper, 0, sizeof(Mapper234));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper234_Reset;
	mapper->vtbl.Write = Mapper234_Write;
	mapper->vtbl.Read = Mapper234_Read;
	mapper->vtbl.SaveState = Mapper234_SaveState;
	mapper->vtbl.LoadState = Mapper234_LoadState;
	mapper->vtbl.IsStateSave = Mapper234_IsStateSave;

	return (Mapper *)mapper;
}

