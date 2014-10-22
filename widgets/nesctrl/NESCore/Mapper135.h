//////////////////////////////////////////////////////////////////////////
// Mapper135  SACHEN CHEN                                             //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE    cmd;
	BYTE	chr0l, chr1l, chr0h, chr1h, chrch;
}Mapper135;
void	Mapper135_SetBank_PPU(Mapper *mapper);

void	Mapper135_Reset(Mapper *mapper)
{
	((Mapper135 *)mapper)->cmd = 0;
	((Mapper135 *)mapper)->chr0l = ((Mapper135 *)mapper)->chr1l = ((Mapper135 *)mapper)->chr0h = ((Mapper135 *)mapper)->chr1h = ((Mapper135 *)mapper)->chrch = 0;

	SetPROM_32K_Bank_cont( 0 );
	Mapper135_SetBank_PPU(mapper);
}

void	Mapper135_WriteLow( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0x4101 ) {
		case	0x4100:
			((Mapper135 *)mapper)->cmd = data & 0x07;
			break;
		case	0x4101:
			switch( ((Mapper135 *)mapper)->cmd ) {
				case	0:
					((Mapper135 *)mapper)->chr0l = data & 0x07;
					Mapper135_SetBank_PPU(mapper);
					break;
				case	1:
					((Mapper135 *)mapper)->chr0h = data & 0x07;
					Mapper135_SetBank_PPU(mapper);
					break;
				case	2:
					((Mapper135 *)mapper)->chr1l = data & 0x07;
					Mapper135_SetBank_PPU(mapper);
					break;
				case	3:
					((Mapper135 *)mapper)->chr1h = data & 0x07;
					Mapper135_SetBank_PPU(mapper);
					break;
				case	4:
					((Mapper135 *)mapper)->chrch = data & 0x07;
					Mapper135_SetBank_PPU(mapper);
					break;
				case	5:
					SetPROM_32K_Bank_cont( data&0x07 );
					break;
				case	6:
					break;
				case	7:
					switch( (data>>1)&0x03 ) {
						case	0: SetVRAM_Mirror_cont( VRAM_MIRROR4L ); break;
						case	1: SetVRAM_Mirror_cont( VRAM_HMIRROR  ); break;
						case	2: SetVRAM_Mirror_cont( VRAM_VMIRROR  ); break;
						case	3: SetVRAM_Mirror_cont( VRAM_MIRROR4L ); break;
					}
					break;
			}
			break;
	}

	CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
}

void	Mapper135_SetBank_PPU(Mapper *mapper)
{
	SetVROM_2K_Bank( 0, 0|(((Mapper135 *)mapper)->chr0l<<1)|(((Mapper135 *)mapper)->chrch<<4) );
	SetVROM_2K_Bank( 2, 1|(((Mapper135 *)mapper)->chr0h<<1)|(((Mapper135 *)mapper)->chrch<<4) );
	SetVROM_2K_Bank( 4, 0|(((Mapper135 *)mapper)->chr1l<<1)|(((Mapper135 *)mapper)->chrch<<4) );
	SetVROM_2K_Bank( 6, 1|(((Mapper135 *)mapper)->chr1h<<1)|(((Mapper135 *)mapper)->chrch<<4) );
}

void	Mapper135_SaveState( Mapper *mapper, LPBYTE p )
{
	p[ 0] = ((Mapper135 *)mapper)->cmd;
	p[ 1] = ((Mapper135 *)mapper)->chr0l;
	p[ 2] = ((Mapper135 *)mapper)->chr0h;
	p[ 3] = ((Mapper135 *)mapper)->chr1l;
	p[ 4] = ((Mapper135 *)mapper)->chr1h;
	p[ 5] = ((Mapper135 *)mapper)->chrch;
}

void	Mapper135_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper135 *)mapper)->cmd   = p[ 0];
	((Mapper135 *)mapper)->chr0l = p[ 1];
	((Mapper135 *)mapper)->chr0h = p[ 2];
	((Mapper135 *)mapper)->chr0l = p[ 3];
	((Mapper135 *)mapper)->chr0h = p[ 4];
	((Mapper135 *)mapper)->chrch = p[ 5];
}

BOOL	Mapper135_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper135_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper135));
	
	memset(mapper, 0, sizeof(Mapper135));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper135_Reset;
	mapper->vtbl.WriteLow = Mapper135_WriteLow;
	mapper->vtbl.SaveState = Mapper135_SaveState;
	mapper->vtbl.LoadState = Mapper135_LoadState;
	mapper->vtbl.IsStateSave = Mapper135_IsStateSave;

	return (Mapper *)mapper;
}


