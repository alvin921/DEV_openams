
//////////////////////////////////////////////////////////////////////////
// Mapper240  Gen Ke Le Zhuan                                           //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper240;

void    Mapper240_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}
}

void	Mapper240_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr>=0x4020 && addr<0x6000 ) {
		SetPROM_32K_Bank_cont( (data&0xF0)>>4 );
		SetVROM_8K_Bank_cont(data&0xF);
	}
}

Mapper * Mapper240_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper240));
	
	memset(mapper, 0, sizeof(Mapper240));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper240_Reset;
	mapper->vtbl.WriteLow = Mapper240_WriteLow;

	return (Mapper *)mapper;
}

