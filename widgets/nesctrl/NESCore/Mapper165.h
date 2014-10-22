//////////////////////////////////////////////////////////////////////////
// Mapper165  Fire Emblem Chinese version                               //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr0, chr1, chr2, chr3;
	BYTE	we_sram;
	BYTE	latch;
}Mapper165;
void	Mapper165_SetBank_CPU(Mapper *mapper);
void	Mapper165_SetBank_PPU(Mapper *mapper);
void	Mapper165_SetBank_PPUSUB(Mapper *mapper, int bank, int page );

void	Mapper165_Reset(Mapper *mapper)
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper165 *)mapper)->reg[i] = 0x00;
	}
	((Mapper165 *)mapper)->prg0 = 0;
	((Mapper165 *)mapper)->prg1 = 1;
	Mapper165_SetBank_CPU(mapper);

	((Mapper165 *)mapper)->chr0 = 0;
	((Mapper165 *)mapper)->chr1 = 0;
	((Mapper165 *)mapper)->chr2 = 4;
	((Mapper165 *)mapper)->chr3 = 4;
	((Mapper165 *)mapper)->latch = 0xFD;
	Mapper165_SetBank_PPU(mapper);

	((Mapper165 *)mapper)->we_sram  = 0;	// Disable

	PPU_SetChrLatchMode( ((Mapper165 *)mapper)->nes->ppu, TRUE );
}

void	Mapper165_Write( Mapper *mapper, WORD addr, BYTE data )
{

	switch( addr & 0xE001 ) {
		case	0x8000:
			((Mapper165 *)mapper)->reg[0] = data;
			Mapper165_SetBank_CPU(mapper);
			Mapper165_SetBank_PPU(mapper);
			break;
		case	0x8001:
			((Mapper165 *)mapper)->reg[1] = data;

			switch( ((Mapper165 *)mapper)->reg[0] & 0x07 ) {
				case	0x00:
					((Mapper165 *)mapper)->chr0 = data & 0xFC;
					if( ((Mapper165 *)mapper)->latch == 0xFD )
						Mapper165_SetBank_PPU(mapper);
					break;
				case	0x01:
					((Mapper165 *)mapper)->chr1 = data & 0xFC;
					if( ((Mapper165 *)mapper)->latch == 0xFE )
						Mapper165_SetBank_PPU(mapper);
					break;

				case	0x02:
					((Mapper165 *)mapper)->chr2 = data & 0xFC;
					if( ((Mapper165 *)mapper)->latch == 0xFD )
						Mapper165_SetBank_PPU(mapper);
					break;
				case	0x04:
					((Mapper165 *)mapper)->chr3 = data & 0xFC;
					if( ((Mapper165 *)mapper)->latch == 0xFE )
						Mapper165_SetBank_PPU(mapper);
					break;

				case	0x06:
					((Mapper165 *)mapper)->prg0 = data;
					Mapper165_SetBank_CPU(mapper);
					break;
				case	0x07:
					((Mapper165 *)mapper)->prg1 = data;
					Mapper165_SetBank_CPU(mapper);
					break;
			}
			break;
		case	0xA000:
			((Mapper165 *)mapper)->reg[2] = data;
			if( data & 0x01 ) 
			{
				SetVRAM_Mirror_cont( VRAM_HMIRROR );
			}else{
				SetVRAM_Mirror_cont( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			((Mapper165 *)mapper)->reg[3] = data;
			break;
		default:
			break;
	}	

}

void	Mapper165_SetBank_CPU(Mapper *mapper)
{
	SetPROM_32K_Bank( ((Mapper165 *)mapper)->prg0, ((Mapper165 *)mapper)->prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper165_SetBank_PPU(Mapper *mapper)
{
	if(((Mapper165 *)mapper)->latch==0xFD){
		Mapper165_SetBank_PPUSUB(mapper, 0, ((Mapper165 *)mapper)->chr0 );
		Mapper165_SetBank_PPUSUB(mapper, 4, ((Mapper165 *)mapper)->chr2 );
	}else{
		Mapper165_SetBank_PPUSUB(mapper, 0, ((Mapper165 *)mapper)->chr1 );
		Mapper165_SetBank_PPUSUB(mapper, 4, ((Mapper165 *)mapper)->chr3 );
	}
}

void	Mapper165_SetBank_PPUSUB( Mapper *mapper, int bank, int page )
{
	if( page == 0 ) {
		SetCRAM_4K_Bank( bank, page>>2 );
	} else {
		SetVROM_4K_Bank( bank, page>>2 );
	}
}

void	Mapper165_PPU_ChrLatch( Mapper *mapper, WORD addr )
{
	WORD mask = addr&0x1FF0;

	if( mask == 0x1FD0 ) {
		((Mapper165 *)mapper)->latch = 0xFD;
		Mapper165_SetBank_PPU(mapper);
	} else if( mask == 0x1FE0 ) {
		((Mapper165 *)mapper)->latch = 0xFE;
		Mapper165_SetBank_PPU(mapper);
	}

}

void	Mapper165_SaveState(Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		p[i] = ((Mapper165 *)mapper)->reg[i];
	}
	p[ 8] = ((Mapper165 *)mapper)->prg0;
	p[ 9] = ((Mapper165 *)mapper)->prg1;
	p[10] = ((Mapper165 *)mapper)->chr0;
	p[11] = ((Mapper165 *)mapper)->chr1;
	p[12] = ((Mapper165 *)mapper)->chr2;
	p[13] = ((Mapper165 *)mapper)->chr3;
	p[14] = ((Mapper165 *)mapper)->latch;
}

void	Mapper165_LoadState(Mapper *mapper, LPBYTE p )
{
	INT i;

	for( i = 0; i < 8; i++ ) {
		((Mapper165 *)mapper)->reg[i] = p[i];
	}
	((Mapper165 *)mapper)->prg0  = p[ 8];
	((Mapper165 *)mapper)->prg1  = p[ 9];
	((Mapper165 *)mapper)->chr0 = p[10];
	((Mapper165 *)mapper)->chr1 = p[11];
	((Mapper165 *)mapper)->chr2  = p[12];
	((Mapper165 *)mapper)->chr3  = p[13];
	((Mapper165 *)mapper)->latch  = p[14];
}

BOOL	Mapper165_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper165_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper165));
	
	memset(mapper, 0, sizeof(Mapper165));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper165_Reset;
	mapper->vtbl.Write = Mapper165_Write;
	mapper->vtbl.PPU_ChrLatch = Mapper165_PPU_ChrLatch;
	mapper->vtbl.SaveState = Mapper165_SaveState;
	mapper->vtbl.LoadState = Mapper165_LoadState;
	mapper->vtbl.IsStateSave = Mapper165_IsStateSave;

	return (Mapper *)mapper;
}


