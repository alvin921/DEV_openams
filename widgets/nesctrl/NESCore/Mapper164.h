//////////////////////////////////////////////////////////////////////////
// Mapper164  Pocket Monster Gold                                       //
//////////////////////////////////////////////////////////////////////////
typedef struct {
	INHERIT_MAPPER;
	
	BYTE	reg5000;
	BYTE	reg5100;
	BYTE	a3, p_mode;
}Mapper164;

void	Mapper164_SetBank_CPU(Mapper *mapper);
void	Mapper164_SetBank_PPU(Mapper *mapper);


void	Mapper164_Reset(Mapper *mapper)
{
	((Mapper164 *)mapper)->reg5000 = 0;
	((Mapper164 *)mapper)->reg5100 = 0;
	Mapper164_SetBank_CPU(mapper);
	Mapper164_SetBank_PPU(mapper);
	PPU_SetExtLatchMode( ((Mapper164 *)mapper)->nes->ppu, TRUE );
}

void Mapper164_WriteLow(Mapper *mapper, WORD addr, BYTE data)
{
	if(addr==0x5000){
		((Mapper164 *)mapper)->p_mode = data>>7;
		((Mapper164 *)mapper)->reg5000 = data;
		Mapper164_SetBank_CPU(mapper);
		Mapper164_SetBank_PPU(mapper);
	}else if(addr==0x5100){
		((Mapper164 *)mapper)->reg5100 = data;
		Mapper164_SetBank_CPU(mapper);
		Mapper164_SetBank_PPU(mapper);
	}else if(addr>=0x6000){
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}else{
		DebugOut("write to %04x:%02x\n", addr, data);
	}

}


void	Mapper164_SetBank_CPU(Mapper *mapper)
{
	int mode, base, bank;

	base = (((Mapper164 *)mapper)->reg5100&1)<<5;
	mode = (((Mapper164 *)mapper)->reg5000>>4)&0x07;

	switch(mode){
		case 0:
		case 2:
		case 4:
		case 6:				/* NORMAL MODE */
			bank = (((Mapper164 *)mapper)->reg5000&0x0f);
			bank += (((Mapper164 *)mapper)->reg5000&0x20)>>1;
			SetPROM_16K_Bank(4, bank+base);
			SetPROM_16K_Bank(6, base+0x1f);
			DebugOut("-- normal mode: mode=%d, bank=%d --\n", mode, bank);
			break;
		case 1:
		case 3:				/* REG MODE */
			DebugOut("-- reg mode --\n");
			break;
		case 5:				/* 32K MODE */
			bank = (((Mapper164 *)mapper)->reg5000&0x0f);
			SetPROM_32K_Bank_cont(bank+(base>>1));
//			DebugOut("-- 32K MODE: bank=%02x --\n", bank);
			break;
		case 7:				/* HALF MODE */
			bank = (((Mapper164 *)mapper)->reg5000&0x0f);
			bank += (bank&0x08)<<1;
			SetPROM_16K_Bank(4, bank+base);
			bank = (bank&0x10)+0x0f;
			SetPROM_16K_Bank(6, base+0x1f);
			DebugOut("-- half mode --\n");
			break;
		default:
			break;
	};

}

void	Mapper164_SetBank_PPU(Mapper *mapper)
{
	SetCRAM_8K_Bank(0);
}


void	Mapper164_PPU_ExtLatchX( Mapper *mapper, INT x )
{
	((Mapper164 *)mapper)->a3 = (x&1)<<3;
}

void	Mapper164_PPU_ExtLatch( Mapper *mapper, WORD ntbladr, BYTE* chr_l, BYTE* chr_h, BYTE* attr )
{
	INT loopy_v = PPU_GetPPUADDR(((Mapper164 *)mapper)->nes->ppu);
	INT loopy_y = PPU_GetTILEY(((Mapper164 *)mapper)->nes->ppu);
	INT	tileofs = (PPUREG[0]&PPU_BGTBL_BIT)<<8;
	INT	attradr = 0x23C0+(loopy_v&0x0C00)+((loopy_v&0x0380)>>4);
	INT	attrsft = (ntbladr&0x0040)>>4;
	LPBYTE	pNTBL = PPU_MEM_BANK[ntbladr>>10];
	INT	ntbl_x  = ntbladr&0x001F;
	INT	tileadr;

	attradr &= 0x3FF;
	*attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;
	tileadr = tileofs+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;

	if(((Mapper164 *)mapper)->p_mode){
		tileadr = (tileadr&0xfff7)|((Mapper164 *)mapper)->a3;
		*chr_l = *chr_h = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
	}else{
		*chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
		*chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
	}

}

void	Mapper164_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper164 *)mapper)->reg5000;
	p[1] = ((Mapper164 *)mapper)->reg5100;
	p[2] = ((Mapper164 *)mapper)->a3;
	p[3] = ((Mapper164 *)mapper)->p_mode;
}

void	Mapper164_LoadState( Mapper *mapper, LPBYTE p )
{

	((Mapper164 *)mapper)->reg5000 = p[0];
	((Mapper164 *)mapper)->reg5100 = p[1];
	((Mapper164 *)mapper)->a3 = p[2];
	((Mapper164 *)mapper)->p_mode = p[3];
}

BOOL	Mapper164_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper164_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper164));
	
	memset(mapper, 0, sizeof(Mapper164));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper164_Reset;
	mapper->vtbl.WriteLow = Mapper164_WriteLow;
	mapper->vtbl.PPU_ExtLatchX = Mapper164_PPU_ExtLatchX;
	mapper->vtbl.PPU_ExtLatch = Mapper164_PPU_ExtLatch;
	mapper->vtbl.SaveState = Mapper164_SaveState;
	mapper->vtbl.LoadState = Mapper164_LoadState;
	mapper->vtbl.IsStateSave = Mapper164_IsStateSave;

	return (Mapper *)mapper;
}


