//////////////////////////////////////////////////////////////////////////
// Mapper133  SACHEN CHEN                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE    cmd, prg, chr;
}Mapper134;

void	Mapper134_Reset(Mapper *mapper)
{
	SetPROM_32K_Bank_cont( 0 );
//	SetPROM_16K_Bank( 6, 0 );
//	SetPROM_16K_Bank( 6, 1 );
	SetVROM_8K_Bank_cont( 0 );
}

void	Mapper134_WriteLow( Mapper *mapper,WORD addr, BYTE data )
{
	switch( addr & 0x4101 ) {
		case	0x4100:
			((Mapper134 *)mapper)->cmd = data & 0x07;
			break;
		case	0x4101:
			switch( ((Mapper134 *)mapper)->cmd ) {
				case 0:	
					((Mapper134 *)mapper)->prg = 0;
					((Mapper134 *)mapper)->chr = 3;
					break;
				case 4:
					((Mapper134 *)mapper)->chr &= 0x3;
					((Mapper134 *)mapper)->chr |= (data & 0x07) << 2;
					break;
				case 5:
					((Mapper134 *)mapper)->prg = data & 0x07;
					break;
				case 6:
					((Mapper134 *)mapper)->chr &= 0x1C;
					((Mapper134 *)mapper)->chr |= data & 0x3;
					break;
				case 7:
					if( data & 0x01 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
					else		  SetVRAM_Mirror_cont( VRAM_VMIRROR );
					break;
			}
			break;
	}
	SetPROM_32K_Bank_cont( ((Mapper134 *)mapper)->prg );
//	SetPROM_16K_Bank( 4, (((Mapper134 *)mapper)->prg<<1)|0 );
//	SetPROM_16K_Bank( 6, (((Mapper134 *)mapper)->prg<<1)|1 ); 
	SetVROM_8K_Bank_cont( ((Mapper134 *)mapper)->chr );
	CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
}

void	Mapper134_SaveState( Mapper *mapper,LPBYTE p )
{
	p[ 0] = ((Mapper134 *)mapper)->cmd;
	p[ 1] = ((Mapper134 *)mapper)->prg;
	p[ 2] = ((Mapper134 *)mapper)->chr;
}

void	Mapper134_LoadState( Mapper *mapper,LPBYTE p )
{
	((Mapper134 *)mapper)->cmd = p[ 0];
	((Mapper134 *)mapper)->prg = p[ 1];
	((Mapper134 *)mapper)->chr = p[ 2];
}

BOOL	Mapper134_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper134_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper134));
	
	memset(mapper, 0, sizeof(Mapper134));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper134_Reset;
	mapper->vtbl.WriteLow = Mapper134_WriteLow;
	mapper->vtbl.SaveState = Mapper134_SaveState;
	mapper->vtbl.LoadState = Mapper134_LoadState;
	mapper->vtbl.IsStateSave = Mapper134_IsStateSave;

	return (Mapper *)mapper;
}



