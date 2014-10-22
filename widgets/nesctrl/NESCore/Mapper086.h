
//////////////////////////////////////////////////////////////////////////
// Mapper086  Jaleco Early Mapper #2                                    //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg, cnt;
}Mapper086;

void	Mapper086_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );
	SetVROM_8K_Bank_cont( 0 );

	((Mapper086 *)mapper)->reg = 0xFF;
	((Mapper086 *)mapper)->cnt = 0;
}

void	Mapper086_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x6000 ) {
		SetPROM_32K_Bank_cont( (data&0x30)>>4 );

		SetVROM_8K_Bank_cont( (data&0x03)|((data & 0x40)>>4) );
	}
	if( addr == 0x7000 ) {
		if( !(((Mapper086 *)mapper)->reg&0x10) && (data&0x10) && !((Mapper086 *)mapper)->cnt ) {
//DebugOut( "WR:$%02X\n", data );
			if( (data&0x0F) == 0		// Strike
			 || (data&0x0F) == 5 ) {	// Foul
				((Mapper086 *)mapper)->cnt = 60;		// Ÿ‚Ì”­º‚ğ1•b’ö‹Ö~‚·‚é
			}

		}
		((Mapper086 *)mapper)->reg = data;
	}
}

void	Mapper086_VSync(Mapper *mapper)
{
	if( ((Mapper086 *)mapper)->cnt ) {
		((Mapper086 *)mapper)->cnt--;
	}
}

Mapper * Mapper086_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper086));
	
	memset(mapper, 0, sizeof(Mapper086));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper086_Reset;
	mapper->vtbl.WriteLow = Mapper086_WriteLow;
	mapper->vtbl.VSync = Mapper086_VSync;

	return (Mapper *)mapper;
}

