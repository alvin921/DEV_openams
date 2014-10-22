
//////////////////////////////////////////////////////////////////////////
// Mapper057                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg;
}Mapper057;

void	Mapper057_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank( 0, 1, 0, 1 );
	SetVROM_8K_Bank_cont( 0 );

	((Mapper057 *)mapper)->reg = 0;
}

void	Mapper057_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
		case	0x8001:
		case	0x8002:
		case	0x8003:
			if( data & 0x40 ) {
				SetVROM_8K_Bank_cont( (data&0x03)+((((Mapper057 *)mapper)->reg&0x10)>>1)+(((Mapper057 *)mapper)->reg&0x07) );
			}
			break;
		case	0x8800:
			((Mapper057 *)mapper)->reg = data;

			if( data & 0x80 ) {
				SetPROM_8K_Bank( 4, ((data & 0x40)>>6)*4+8+0 );
				SetPROM_8K_Bank( 5, ((data & 0x40)>>6)*4+8+1 );
				SetPROM_8K_Bank( 6, ((data & 0x40)>>6)*4+8+2 );
				SetPROM_8K_Bank( 7, ((data & 0x40)>>6)*4+8+3 );
			} else {
				SetPROM_8K_Bank( 4, ((data & 0x60)>>5)*2+0 );
				SetPROM_8K_Bank( 5, ((data & 0x60)>>5)*2+1 );
				SetPROM_8K_Bank( 6, ((data & 0x60)>>5)*2+0 );
				SetPROM_8K_Bank( 7, ((data & 0x60)>>5)*2+1 );
			}

			SetVROM_8K_Bank_cont( (data&0x07)+((data&0x10)>>1) );

			if( data & 0x08 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );

			break;
	}
}

void	Mapper057_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper057 *)mapper)->reg;
}

void	Mapper057_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper057 *)mapper)->reg = p[0];
}

BOOL	Mapper057_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper057_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper057));
	
	memset(mapper, 0, sizeof(Mapper057));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper057_Reset;
	mapper->vtbl.Write = Mapper057_Write;
	mapper->vtbl.SaveState = Mapper057_SaveState;
	mapper->vtbl.LoadState = Mapper057_LoadState;
	mapper->vtbl.IsStateSave = Mapper057_IsStateSave;

	return (Mapper *)mapper;
}

