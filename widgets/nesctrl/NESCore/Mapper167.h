
//////////////////////////////////////////////////////////////////////////
// Mapper012  Subor Computer V4.0                                       //
//////////////////////////////////////////////////////////////////////////

typedef struct {
	INHERIT_MAPPER;
	
	BYTE	regs[4];
	BYTE	rom_type;
}Mapper167;

void	Mapper167_SetBank_CPU(Mapper *mapper);
void	Mapper167_SetBank_PPU(Mapper *mapper);


void	Mapper167_Reset(Mapper *mapper)
{
	DWORD crc;

	((Mapper167 *)mapper)->regs[0] = 0;
	((Mapper167 *)mapper)->regs[1] = 0;
	((Mapper167 *)mapper)->regs[2] = 0;
	((Mapper167 *)mapper)->regs[3] = 0;

	crc = ROM_GetPROM_CRC(((Mapper167 *)mapper)->nes->rom);
	if(crc==0x82F1Fb96){
		// Subor Computer(Russia)
		((Mapper167 *)mapper)->rom_type = 1;
	}else{
		// Subor Computer(Chinese)
		((Mapper167 *)mapper)->rom_type = 0;
	}

	Mapper167_SetBank_CPU(mapper);
	Mapper167_SetBank_PPU(mapper);
}

void Mapper167_Write(Mapper *mapper, WORD addr, BYTE data)
{
	int idx;

	idx = (addr>>13)&0x03;
	((Mapper167 *)mapper)->regs[idx]=data;
	Mapper167_SetBank_CPU(mapper);
	Mapper167_SetBank_PPU(mapper);
//	DebugOut("write to %04x:%02x\n", addr, data);
}


void	Mapper167_SetBank_CPU(Mapper *mapper)
{
	int base, bank;

	base = ((((Mapper167 *)mapper)->regs[0]^((Mapper167 *)mapper)->regs[1])&0x10)<<1;
	bank = (((Mapper167 *)mapper)->regs[2]^((Mapper167 *)mapper)->regs[3])&0x1f;
	
	if(((Mapper167 *)mapper)->regs[1]&0x08){
		bank &= 0xfe;
		if(((Mapper167 *)mapper)->rom_type==0){
			SetPROM_16K_Bank(4, base+bank+1);
			SetPROM_16K_Bank(6, base+bank+0);
		}else{
			SetPROM_16K_Bank(6, base+bank+1);
			SetPROM_16K_Bank(4, base+bank+0);
		}
//		DebugOut("32K MODE!\n");
	}else{
		if(((Mapper167 *)mapper)->regs[1]&0x04){
			SetPROM_16K_Bank(4, 0x1f);
			SetPROM_16K_Bank(6, base+bank);
//			DebugOut("HIGH 16K MODE!\n");
		}else{
			SetPROM_16K_Bank(4, base+bank);
			if(((Mapper167 *)mapper)->rom_type==0){
				SetPROM_16K_Bank(6, 0x20);
			}else{
				SetPROM_16K_Bank(6, 0x07);
			}
//			DebugOut("LOW  16K MODE!\n");
		}
	}
	

}

void	Mapper167_SetBank_PPU(Mapper *mapper)
{
	SetCRAM_8K_Bank(0);
}

void	Mapper167_SaveState( Mapper *mapper, LPBYTE p )
{
	p[0] = ((Mapper167 *)mapper)->regs[0];
	p[1] = ((Mapper167 *)mapper)->regs[1];
	p[2] = ((Mapper167 *)mapper)->regs[2];
	p[3] = ((Mapper167 *)mapper)->regs[3];
	p[4] = ((Mapper167 *)mapper)->rom_type;
}

void	Mapper167_LoadState( Mapper *mapper, LPBYTE p )
{
	((Mapper167 *)mapper)->regs[0] = p[0];
	((Mapper167 *)mapper)->regs[1] = p[1];
	((Mapper167 *)mapper)->regs[2] = p[2];
	((Mapper167 *)mapper)->regs[3] = p[3];
	((Mapper167 *)mapper)->rom_type = p[4];
}

BOOL	Mapper167_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper167_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper167));
	
	memset(mapper, 0, sizeof(Mapper167));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper167_Reset;
	mapper->vtbl.Write = Mapper167_Write;
	mapper->vtbl.SaveState = Mapper167_SaveState;
	mapper->vtbl.LoadState = Mapper167_LoadState;
	mapper->vtbl.IsStateSave = Mapper167_IsStateSave;

	return (Mapper *)mapper;
}

