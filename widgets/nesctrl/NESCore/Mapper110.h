
//////////////////////////////////////////////////////////////////////////
// Mapper110                                                            //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg0, reg1;
}Mapper110;

void	Mapper110_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
	SetVROM_8K_Bank_cont( 0 );

	((Mapper110 *)mapper)->reg0 = 0;
	((Mapper110 *)mapper)->reg1 = 0;
}
void	Mapper110_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x4100:
			((Mapper110 *)mapper)->reg1 = data & 0x07;
			break;
		case	0x4101:
			switch( ((Mapper110 *)mapper)->reg1 ) {
				case	5:
					SetPROM_32K_Bank_cont( data );
					break;
				case	0:
					((Mapper110 *)mapper)->reg0 = data & 0x01;
					SetVROM_8K_Bank_cont( ((Mapper110 *)mapper)->reg0 );
					break;
				case	2:
					((Mapper110 *)mapper)->reg0 = data;
					SetVROM_8K_Bank_cont( ((Mapper110 *)mapper)->reg0 );
					break;
				case	4:
					((Mapper110 *)mapper)->reg0 = ((Mapper110 *)mapper)->reg0 | (data<<1);
					SetVROM_8K_Bank_cont( ((Mapper110 *)mapper)->reg0 );
					break;
				case	6:
					((Mapper110 *)mapper)->reg0 = ((Mapper110 *)mapper)->reg0 | (data<<2);
					SetVROM_8K_Bank_cont( ((Mapper110 *)mapper)->reg0 );
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void	Mapper110_SaveState( Mapper *mapper, LPBYTE p )
{
	p[ 0] = ((Mapper110 *)mapper)->reg0;
	p[ 1] = ((Mapper110 *)mapper)->reg1;
}

void	Mapper110_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper110 *)mapper)->reg0 = p[ 0];
	((Mapper110 *)mapper)->reg1 = p[ 1];
}

BOOL	Mapper110_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper110_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper110));
	
	memset(mapper, 0, sizeof(Mapper110));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper110_Reset;
	mapper->vtbl.WriteLow = Mapper110_WriteLow;
	mapper->vtbl.SaveState = Mapper110_SaveState;
	mapper->vtbl.LoadState = Mapper110_LoadState;
	mapper->vtbl.IsStateSave = Mapper110_IsStateSave;

	return (Mapper *)mapper;
}

