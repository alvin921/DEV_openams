
//////////////////////////////////////////////////////////////////////////
// Mapper095  Namcot 106M (Dragon Buster)                               //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg;
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
}Mapper095;

void	Mapper095_SetBank_CPU(Mapper *mapper);
void	Mapper095_SetBank_PPU(Mapper *mapper);

void	Mapper095_Reset(Mapper *mapper)
{
	((Mapper095 *)mapper)->reg = 0x00;
	((Mapper095 *)mapper)->prg0 = 0;
	((Mapper095 *)mapper)->prg1 = 1;
	Mapper095_SetBank_CPU(mapper);

	if( VROM_1K_SIZE ) {
		((Mapper095 *)mapper)->chr01 = 0;
		((Mapper095 *)mapper)->chr23 = 2;
		((Mapper095 *)mapper)->chr4  = 4;
		((Mapper095 *)mapper)->chr5  = 5;
		((Mapper095 *)mapper)->chr6  = 6;
		((Mapper095 *)mapper)->chr7  = 7;
	} else {
		((Mapper095 *)mapper)->chr01 = ((Mapper095 *)mapper)->chr23 = ((Mapper095 *)mapper)->chr4  = ((Mapper095 *)mapper)->chr5  = ((Mapper095 *)mapper)->chr6  = ((Mapper095 *)mapper)->chr7  = 0;
	}

	Mapper095_SetBank_PPU(mapper);

	NES_SetRenderMethod( mapper->nes, POST_RENDER );
}

void	Mapper095_Write( Mapper *mapper, WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper095 *)mapper)->reg = data;
			Mapper095_SetBank_CPU(mapper);
			Mapper095_SetBank_PPU(mapper);
			break;
		case	0x8001:
			if( ((Mapper095 *)mapper)->reg <= 0x05 ) {
				if( data & 0x20 ) SetVRAM_Mirror_cont( VRAM_MIRROR4H );
				else		  SetVRAM_Mirror_cont( VRAM_MIRROR4L );
				data &= 0x1F;
			}

			switch( ((Mapper095 *)mapper)->reg & 0x07 ) {
				case	0x00:
					if( VROM_1K_SIZE ) {
						((Mapper095 *)mapper)->chr01 = data & 0xFE;
						Mapper095_SetBank_PPU(mapper);
					}
					break;
				case	0x01:
					if( VROM_1K_SIZE ) {
						((Mapper095 *)mapper)->chr23 = data & 0xFE;
						Mapper095_SetBank_PPU(mapper);
					}
					break;
				case	0x02:
					if( VROM_1K_SIZE ) {
						((Mapper095 *)mapper)->chr4 = data;
						Mapper095_SetBank_PPU(mapper);
					}
					break;
				case	0x03:
					if( VROM_1K_SIZE ) {
						((Mapper095 *)mapper)->chr5 = data;
						Mapper095_SetBank_PPU(mapper);
					}
					break;
				case	0x04:
					if( VROM_1K_SIZE ) {
						((Mapper095 *)mapper)->chr6 = data;
						Mapper095_SetBank_PPU(mapper);
					}
					break;
				case	0x05:
					if( VROM_1K_SIZE ) {
						((Mapper095 *)mapper)->chr7 = data;
						Mapper095_SetBank_PPU(mapper);
					}
					break;
				case	0x06:
					((Mapper095 *)mapper)->prg0 = data;
					Mapper095_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper095 *)mapper)->prg1 = data;
					Mapper095_SetBank_CPU(mapper);
					break;
			}
			break;
	}
}

void	Mapper095_SetBank_CPU(Mapper *mapper)
{
	if( ((Mapper095 *)mapper)->reg & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, ((Mapper095 *)mapper)->prg1, ((Mapper095 *)mapper)->prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( ((Mapper095 *)mapper)->prg0, ((Mapper095 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper095_SetBank_PPU(Mapper *mapper)
{
	if( VROM_1K_SIZE ) {
		if( ((Mapper095 *)mapper)->reg & 0x80 ) {
			SetVROM_8K_Bank( ((Mapper095 *)mapper)->chr4, ((Mapper095 *)mapper)->chr5, ((Mapper095 *)mapper)->chr6, ((Mapper095 *)mapper)->chr7,
					 ((Mapper095 *)mapper)->chr01, ((Mapper095 *)mapper)->chr01+1, ((Mapper095 *)mapper)->chr23, ((Mapper095 *)mapper)->chr23+1 );
		} else {
			SetVROM_8K_Bank( ((Mapper095 *)mapper)->chr01, ((Mapper095 *)mapper)->chr01+1, ((Mapper095 *)mapper)->chr23, ((Mapper095 *)mapper)->chr23+1,
					 ((Mapper095 *)mapper)->chr4, ((Mapper095 *)mapper)->chr5, ((Mapper095 *)mapper)->chr6, ((Mapper095 *)mapper)->chr7 );
		}
	}
}

void	Mapper095_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper095 *)mapper)->reg;
	p[1] = ((Mapper095 *)mapper)->prg0;
	p[2] = ((Mapper095 *)mapper)->prg1;
	p[3] = ((Mapper095 *)mapper)->chr01;
	p[4] = ((Mapper095 *)mapper)->chr23;
	p[5] = ((Mapper095 *)mapper)->chr4;
	p[6] = ((Mapper095 *)mapper)->chr5;
	p[7] = ((Mapper095 *)mapper)->chr6;
	p[8] = ((Mapper095 *)mapper)->chr7;
}

void	Mapper095_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper095 *)mapper)->reg   = p[0];
	((Mapper095 *)mapper)->prg0  = p[1];
	((Mapper095 *)mapper)->prg1  = p[2];
	((Mapper095 *)mapper)->chr01 = p[3];
	((Mapper095 *)mapper)->chr23 = p[4];
	((Mapper095 *)mapper)->chr4  = p[5];
	((Mapper095 *)mapper)->chr5  = p[6];
	((Mapper095 *)mapper)->chr6  = p[7];
	((Mapper095 *)mapper)->chr7  = p[8];
}

BOOL	Mapper095_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper095_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper095));
	
	memset(mapper, 0, sizeof(Mapper095));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper095_Reset;
	mapper->vtbl.Write = Mapper095_Write;
	mapper->vtbl.SaveState = Mapper095_SaveState;
	mapper->vtbl.LoadState = Mapper095_LoadState;
	mapper->vtbl.IsStateSave = Mapper095_IsStateSave;

	return (Mapper *)mapper;
}

