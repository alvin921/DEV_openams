//////////////////////////////////////////////////////////////////////////
// Mapper251                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[11];
	BYTE	breg[4];
}Mapper251;
void	Mapper251_SetBank(Mapper *mapper);

void	Mapper251_Reset(Mapper *mapper)
{
	INT	i;
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	SetVRAM_Mirror_cont( VRAM_VMIRROR );

	for( i = 0; i < 11; i++ )
		((Mapper251 *)mapper)->reg[i] = 0;
	for( i = 0; i < 4; i++ )
		((Mapper251 *)mapper)->breg[i] = 0;
}

void	Mapper251_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	if( (addr & 0xE001) == 0x6000 ) {
		if( ((Mapper251 *)mapper)->reg[9] ) {
			((Mapper251 *)mapper)->breg[((Mapper251 *)mapper)->reg[10]++] = data;
			if( ((Mapper251 *)mapper)->reg[10] == 4 ) {
				((Mapper251 *)mapper)->reg[10] = 0;
				Mapper251_SetBank(mapper);
			}
		}
	}
}

void	Mapper251_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper251 *)mapper)->reg[8] = data;
			Mapper251_SetBank(mapper);
			break;
		case	0x8001:
			((Mapper251 *)mapper)->reg[((Mapper251 *)mapper)->reg[8]&0x07] = data;
			Mapper251_SetBank(mapper);
			break;
		case	0xA001:
			if( data & 0x80 ) {
				((Mapper251 *)mapper)->reg[ 9] = 1;
				((Mapper251 *)mapper)->reg[10] = 0;
			} else {
				((Mapper251 *)mapper)->reg[ 9] = 0;
			}
			break;
	}
}

void	Mapper251_SetBank(Mapper *mapper)
{
	INT	chr[6];
	INT	prg[4];
	INT i;
 
	for(  i = 0; i < 6; i++ ) {
		chr[i] = (((Mapper251 *)mapper)->reg[i]|(((Mapper251 *)mapper)->breg[1]<<4)) & ((((Mapper251 *)mapper)->breg[2]<<4)|0x0F);
	}

	if( ((Mapper251 *)mapper)->reg[8] & 0x80 ) {
		SetVROM_8K_Bank(chr[2],chr[3],chr[4],chr[5],chr[0],chr[0]+1,chr[1],chr[1]+1);
	} else {
		SetVROM_8K_Bank(chr[0],chr[0]+1,chr[1],chr[1]+1,chr[2],chr[3],chr[4],chr[5]);
	}

	prg[0] = (((Mapper251 *)mapper)->reg[6]&((((Mapper251 *)mapper)->breg[3]&0x3F)^0x3F))|(((Mapper251 *)mapper)->breg[1]);
	prg[1] = (((Mapper251 *)mapper)->reg[7]&((((Mapper251 *)mapper)->breg[3]&0x3F)^0x3F))|(((Mapper251 *)mapper)->breg[1]);
	prg[2] = prg[3] =((((Mapper251 *)mapper)->breg[3]&0x3F)^0x3F)|(((Mapper251 *)mapper)->breg[1]);
	prg[2] &= PROM_8K_SIZE-1;

	if( ((Mapper251 *)mapper)->reg[8] & 0x40 ) {
		SetPROM_32K_Bank( prg[2],prg[1],prg[0],prg[3] );
	} else {
		SetPROM_32K_Bank( prg[0],prg[1],prg[2],prg[3] );
	}
}

void	Mapper251_SaveState( Mapper *mapper, LPBYTE p )
{
	INT	i;

	for( i = 0; i < 11; i++ ) {
		p[i] = ((Mapper251 *)mapper)->reg[i];
	}
	for( i = 0; i < 4; i++ ) {
		p[i+11] = ((Mapper251 *)mapper)->breg[i];
	}
}

void	Mapper251_LoadState( Mapper *mapper, LPBYTE p )
{
	INT	i;
	for( i = 0; i < 11; i++ ) {
		((Mapper251 *)mapper)->reg[i] = p[i];
	}
	for( i = 0; i < 4; i++ ) {
		((Mapper251 *)mapper)->reg[i] = p[i+11];
	}
}

BOOL	Mapper251_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper251_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper251));
	
	memset(mapper, 0, sizeof(Mapper251));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper251_Reset;
	mapper->vtbl.WriteLow = Mapper251_WriteLow;
	mapper->vtbl.Write = Mapper251_Write;
	mapper->vtbl.SaveState = Mapper251_SaveState;
	mapper->vtbl.LoadState = Mapper251_LoadState;
	mapper->vtbl.IsStateSave = Mapper251_IsStateSave;

	return (Mapper *)mapper;
}


