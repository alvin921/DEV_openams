
//////////////////////////////////////////////////////////////////////////
// Mapper193 MEGA SOFT (NTDEC) : Fighting Hero                          //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper193;

void    Mapper193_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( PROM_32K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper193_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x6000:
			SetVROM_2K_Bank( 0, ((data>>1)&0x7e)+0 );
			SetVROM_2K_Bank( 2, ((data>>1)&0x7e)+1 );
			break;
		case	0x6001:
			SetVROM_2K_Bank( 4, data>>1 );
			break;
		case	0x6002:
			SetVROM_2K_Bank( 6, data>>1 );
			break;
		case	0x6003:
			SetPROM_32K_Bank_cont( data );
			break;
	}
}

Mapper * Mapper193_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper193));
	
	memset(mapper, 0, sizeof(Mapper193));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper193_Reset;
	mapper->vtbl.WriteLow = Mapper193_WriteLow;

	return (Mapper *)mapper;
}

