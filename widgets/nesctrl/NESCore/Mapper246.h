
//////////////////////////////////////////////////////////////////////////
// Mapper246  Phone Serm Berm                                           //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
}Mapper246;

void    Mapper246_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper246_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr>=0x6000 && addr<0x8000 ) {
		switch( addr ) {
			case 0x6000:
				SetPROM_8K_Bank( 4, data );
				break;
			case 0x6001:
				SetPROM_8K_Bank( 5, data );
				break;
			case 0x6002:
				SetPROM_8K_Bank( 6, data );
				break;
			case 0x6003: 
				SetPROM_8K_Bank( 7, data );
				break;
			case 0x6004:
				SetVROM_2K_Bank(0,data);
				break;
			case 0x6005:
				SetVROM_2K_Bank(2,data);
				break;
			case 0x6006:
				SetVROM_2K_Bank(4,data);
				break;
			case 0x6007:
				SetVROM_2K_Bank(6,data);
				break;
			default:
				CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
				break;
		}
	}
}

Mapper * Mapper246_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper246));
	
	memset(mapper, 0, sizeof(Mapper246));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper246_Reset;
	mapper->vtbl.WriteLow = Mapper246_WriteLow;

	return (Mapper *)mapper;
}

