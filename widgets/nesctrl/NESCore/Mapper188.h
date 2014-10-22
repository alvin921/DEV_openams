
//////////////////////////////////////////////////////////////////////////
// Mapper188  Bandai Karaoke Studio                                     //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper188;
void	Mapper188_Reset(Mapper *mapper)
{
	if( PROM_8K_SIZE > 16 ) {
		SetPROM_32K_Bank( 0, 1, 14, 15 );
	} else {
		SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper188_Write( Mapper *mapper, WORD addr, BYTE data )
{
	if( data ) {
		if( data & 0x10 ) {
			data &= 0x07;
			SetPROM_16K_Bank( 4, data );
		} else {
			SetPROM_16K_Bank( 4, data+8 );
		}
	} else {
		if( PROM_8K_SIZE == 0x10 ) {
			SetPROM_16K_Bank( 4, 7 );
		} else {
			SetPROM_16K_Bank( 4, 8 );
		}
	}
}

Mapper * Mapper188_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper188));
	
	memset(mapper, 0, sizeof(Mapper188));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper188_Reset;
	mapper->vtbl.Write = Mapper188_Write;

	return (Mapper *)mapper;
}

