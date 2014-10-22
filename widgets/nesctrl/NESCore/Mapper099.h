
//////////////////////////////////////////////////////////////////////////
// Mapper099  VS-Unisystem                                              //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE	coin;
}Mapper099;

void	Mapper099_Reset(Mapper *mapper)
{
	// set CPU bank pointers
	if( PROM_8K_SIZE > 2 ) {
		SetPROM_32K_Bank( 0, 1, 2, 3 );
	} else if( PROM_8K_SIZE > 1 ) {
		SetPROM_32K_Bank( 0, 1, 0, 1 );
	} else {
		SetPROM_32K_Bank( 0, 0, 0, 0 );
	}

	// set VROM bank
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	}

	((Mapper099 *)mapper)->coin = 0;
}

BYTE	Mapper099_ExRead( Mapper *mapper, WORD addr )
{
	if( addr == 0x4020 ) {
		return	((Mapper099 *)mapper)->coin;
	}

	return	addr>>8;
}

void	Mapper099_ExWrite( Mapper *mapper, WORD addr, BYTE data )
{
	if( addr == 0x4016 ) {
		if( data & 0x04 ) {
			SetVROM_8K_Bank_cont( 1 );
		} else {
			SetVROM_8K_Bank_cont( 0 );
		}

		if( ROM_GetPROM_CRC(((Mapper099 *)mapper)->nes->rom) == 0xC99EC059 ) {	// VS Raid on Bungeling Bay(J)
			if( data & 0x02 ) {
				CPU_SetIRQ( ((Mapper099 *)mapper)->nes->cpu, IRQ_MAPPER );
			} else {
				CPU_ClrIRQ( ((Mapper099 *)mapper)->nes->cpu, IRQ_MAPPER );
			}
		}
	}

	if( addr == 0x4020 ) {
		((Mapper099 *)mapper)->coin = data;
	}
}

Mapper * Mapper099_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper099));
	
	memset(mapper, 0, sizeof(Mapper099));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper099_Reset;
	mapper->vtbl.ExRead = Mapper099_ExRead;
	mapper->vtbl.ExWrite = Mapper099_ExWrite;

	return (Mapper *)mapper;
}

