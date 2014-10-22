//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Emulation core                                              //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#include "macro.h"

#include "Crclib.h"

#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "pad.h"
#include "rom.h"
#include "mapper.h"

#include "DirectDraw.h"
#include "DirectSound.h"
#include "DirectInput.h"

#if 0 // alvin
#include "pngwrite.c"
#endif

NESCONFIG NESCONFIG_NTSC = {
	21477270.0f,		// Base clock
	1789772.5f,		// Cpu clock

	262,			// Total scanlines

	1364,			// Scanline total cycles(15.75KHz)

	1024,			// H-Draw cycles
	340,			// H-Blank cycles
	4,			// End cycles

	1364*262,		// Frame cycles
	29830,			// FrameIRQ cycles

	60,			// Frame rate(Be originally 59.94Hz)
	1000.0f/60.0f		// Frame period(ms)
};

NESCONFIG NESCONFIG_PAL = {
//	21281364.0f,		// Base clock
	26601714.0f,		// Base clock
//	1773447.0f,		// Cpu clock
	1662607.125f,		// Cpu clock

	312,			// Total scanlines

//	1362,			// Scanline total cycles(15.625KHz)
	1278,			// Scanline total cycles(15.625KHz)

//	1024,			// H-Draw cycles
	960,			// H-Draw cycles
//	338,			// H-Blank cycles
	318,			// H-Blank cycles
	2,			// End cycles

//	1362*312,		// Frame cycles
	1278*312,		// Frame cycles
//	35469,			// FrameIRQ cycles
	33252,			// FrameIRQ cycles

	50,			// Frame rate(Hz)
	1000.0f/50.0f		// Frame period(ms)
};




static BYTE	NES_ReadReg ( NES * pme, WORD addr );
static void	NES_WriteReg( NES * pme, WORD addr, BYTE data );

// ステートサブ
static BOOL	NES_ReadState( NES * pme, t_HIO gio );
static BOOL	NES_WriteState( NES * pme, t_HIO gio );

static void	NES_LoadSRAM(NES * pme);
static void	NES_SaveSRAM(NES * pme);

// FALSE: should exit
static BOOL	NES_LoadDISK(NES * pme);
static void	NES_SaveDISK(NES * pme);


// For Movie pad display
static void	NES_DrawBitmap( NES * pme, INT x, INT y, LPBYTE lpBitmap );

static void	NES_DrawFont( NES * pme, INT x, INT y, BYTE chr, BYTE col );
static void	NES_DrawString( NES * pme, INT x, INT y, char* str, BYTE col );


NES *NES_New( const char* fname )
{

	NES *pme = (NES*)MALLOC(sizeof(NES));
	
	DebugOut( "VirtuaNES - NES Emulator for Win32 by Norix (C)2001\n" );

	memset(pme, 0, sizeof(NES));

	pme->m_bDiskThrottle = FALSE;

	pme->m_nSnapNo = 0;

	pme->m_bNsfPlaying = FALSE;

	pme->m_bBarcode = FALSE;
	pme->m_BarcodeOut = 0;
	pme->m_BarcodePtr = 0;
	pme->m_BarcodeCycles = 0;

	pme->m_bBarcode2 = FALSE;

	pme->cpu = NULL;
	pme->ppu = NULL;
	pme->apu = NULL;
	pme->rom = NULL;
	pme->pad = NULL;
	pme->mapper = NULL;

	pme->SAVERAM_SIZE = 8*1024;	// 8K byte

	// IRQ type
	pme->nIRQtype = 0;

	// FrameIRQ mode
	pme->bFrameIRQ = TRUE;

	// NTSC/PAL VideoMode
	pme->bVideoMode = FALSE;

	// Default config
	pme->nescfg = &NESCONFIG_NTSC;

	// Cheat
	//CheatInitial();

	// TEST
#if	NES_PROFILER
	pme->m_dwTotalCycle = 0;
	pme->m_dwTotalTempCycle = 0;
	pme->m_dwProfileTotalCycle = 0;
	pme->m_dwProfileTotalCount = 0;
	pme->m_dwProfileCycle = 0;
	pme->m_dwProfileTempCycle = 0;
	pme->m_dwProfileAveCycle = 0;
	pme->m_dwProfileMaxCycle = 0;
#endif
		
#if 1 // alvin
	pme->bPause = FALSE; 	// Thread pause
	pme->bEmuPause = FALSE;	// Emulation pause
	pme->bThrottle = FALSE;	// Emulation throttle
	pme->bOneStep = FALSE;	// Emulation one step
	
	pme->nFrameSkip = 0; 	// Emulation frameskip
	pme->frame_time = 0.0f;
	
	// FPS
	pme->nFrameCount = 0;
	pme->FPS = 0;
	
#endif

	//-------

		DebugOut( "Allocating CPU..." );
		if( !(pme->cpu = CPU_New(pme)) )
			goto FAIL;
		DebugOut( "Ok.\n" );

		DebugOut( "Allocating PPU..." );
		if( !(pme->ppu = PPU_New(pme)) )
			goto FAIL;
		DebugOut( "Ok.\n" );

		DebugOut( "Allocating APU..." );
		if( !(pme->apu = APU_New(pme)) )
			goto FAIL;
		DebugOut( "Ok.\n" );

		DebugOut( "Allocating PAD..." );
		if( !(pme->pad = PAD_New(pme)) )
			goto FAIL;
		DebugOut( "Ok.\n" );

		DebugOut( "Loading ROM Image...\n" );

		if( !(pme->rom = ROM_New(fname)) )
			goto FAIL;

		DebugOut( "Loading Mapper...\n" );
		if( !(pme->mapper = CreateMapper(pme, ROM_GetMapperNo(pme->rom))) )
			goto FAIL;

		DebugOut( "Ok.\n" );

		DebugOut( "ROM status\n" );
		DebugOut( " %s\n", ROM_GetRomName(pme->rom) );
		DebugOut( " Mapper       : %03d\n", ROM_GetMapperNo(pme->rom) );
		DebugOut( " PRG SIZE     : %4dK\n", 16*(INT)ROM_GetPROM_SIZE(pme->rom) );
		DebugOut( " CHR SIZE     : %4dK\n",  8*(INT)ROM_GetVROM_SIZE(pme->rom) );

		DebugOut( " V MIRROR     : " );
		if( ROM_IsVMIRROR(pme->rom) ) DebugOut( "Yes\n" );
		else		       DebugOut( "No\n" );
		DebugOut( " 4 SCREEN     : " );
		if( ROM_Is4SCREEN(pme->rom) ) DebugOut( "Yes\n" );
		else		       DebugOut( "No\n" );
		DebugOut( " SAVE RAM     : " );
		if( ROM_IsSAVERAM(pme->rom) ) DebugOut( "Yes\n" );
		else		       DebugOut( "No\n" );
		DebugOut( " TRAINER      : " );
		if( ROM_IsTRAINER(pme->rom) ) DebugOut( "Yes\n" );
		else		       DebugOut( "No\n" );
		DebugOut( " VS-Unisystem : " );
		if( ROM_IsVSUNISYSTEM(pme->rom) ) DebugOut( "Yes\n" );
		else			   DebugOut( "No\n" );

		NesSub_MemoryInitial();
		NES_LoadSRAM(pme);
		if(!NES_LoadDISK(pme))
			goto FAIL;
			

		{
		// Padクラス内だと初期化タイミングが遅いのでここで
		DWORD	crc = ROM_GetPROM_CRC(pme->rom);
		if( crc == 0xe792de94		// Best Play - Pro Yakyuu (New) (J)
		 || crc == 0xf79d684a		// Best Play - Pro Yakyuu (Old) (J)
		 || crc == 0xc2ef3422		// Best Play - Pro Yakyuu 2 (J)
		 || crc == 0x974e8840		// Best Play - Pro Yakyuu '90 (J)
		 || crc == 0xb8747abf		// Best Play - Pro Yakyuu Special (J)
		 || crc == 0x9fa1c11f		// Castle Excellent (J)
		 || crc == 0x0b0d4d1b		// Derby Stallion - Zenkoku Ban (J)
		 || crc == 0x728c3d98		// Downtown - Nekketsu Monogatari (J)
		 || crc == 0xd68a6f33		// Dungeon Kid (J)
		 || crc == 0x3a51eb04		// Fleet Commander (J)
		 || crc == 0x7c46998b		// Haja no Fuuin (J)
		 || crc == 0x7e5d2f1a		// Itadaki Street - Watashi no Mise ni Yottette (J)
		 || crc == 0xcee5857b		// Ninjara Hoi! (J)
		 || crc == 0x50ec5e8b		// Wizardry - Legacy of Llylgamyn (J)
		 || crc == 0x343e9146		// Wizardry - Proving Grounds of the Mad Overlord (J)
		 || crc == 0x33d07e45 ) {	// Wizardry - The Knight of Diamonds (J)
		}
		}

		// VS-Unisystemのデフォルト設定
		if( ROM_IsVSUNISYSTEM(pme->rom) ) {
			DWORD	crc = ROM_GetPROM_CRC(pme->rom);

			pme->m_VSDipValue = GetVSDefaultDipSwitchValue( crc );
			pme->m_VSDipTable = FindVSDipSwitchTable( crc );

			switch( ROM_GetPROM_CRC(pme->rom) ) {
				case	0xeb2dba63:	// VS TKO Boxing
				case	0x9818f656:
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0xed588f00:	// VS Duck Hunt
					PAD_SetVSType( pme->pad, VS_TYPEZ );
					goto	vsexit;
				case	0x8c0c2df5:	// VS Top Gun
					PPU_SetVSSecurity( pme->ppu, 0x1B );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0x16d3f469:	// VS Ninja Jajamaru Kun (J)
					PPU_SetVSSecurity( pme->ppu, 0x1B );
					PAD_SetVSType( pme->pad, VS_TYPE3 );
					goto	vsexit;
				case	0x8850924b:	// VS Tetris
					PAD_SetVSType( pme->pad, VS_TYPE1 );
					goto	vsexit;
				case	0xcf36261e:	// VS Sky Kid
					PAD_SetVSType( pme->pad, VS_TYPE3 );
					goto	vsexit;
				case	0xe1aa8214:	// VS Star Luster
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0xec461db9:	// VS Pinball
					PPU_SetVSColorMap( pme->ppu, 0 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0xe528f651:	// VS Pinball (alt)
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0x17ae56be:	// VS Freedom Force
		//			PPU_SetVSColorMap( pme->ppu, 0 );
					PPU_SetVSColorMap( pme->ppu, 4 );
					PAD_SetVSType( pme->pad, VS_TYPEZ );
					goto	vsexit;
				case	0xe2c0a2be:	// VS Platoon
					PPU_SetVSColorMap( pme->ppu, 0 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0xff5135a3:	// VS Hogan's Alley
					PPU_SetVSColorMap( pme->ppu, 0 );
					PAD_SetVSType( pme->pad, VS_TYPEZ );
					goto	vsexit;
				case	0x70901b25:	// VS Slalom
					PPU_SetVSColorMap( pme->ppu, 1 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0x0b65a917:	// VS Mach Rider(Endurance Course)
				case	0x8a6a9848:
					PPU_SetVSColorMap( pme->ppu, 1 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0xae8063ef:	// VS Mach Rider(Japan, Fighting Course)
					PPU_SetVSColorMap( pme->ppu, 0 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0xcc2c4b5d:	// VS Golf
					PPU_SetVSColorMap( pme->ppu, 1 );
		//			PAD_SetVSType( pme->pad, VS_TYPE0 );
					PAD_SetVSType( pme->pad, VS_TYPE6 );
					goto	vsexit;
				case	0xa93a5aee:	// VS Stroke and Match Golf
					PAD_SetVSType( pme->pad, VS_TYPE1 );
					goto	vsexit;
				case	0x86167220:	// VS Lady Golf
					PPU_SetVSColorMap( pme->ppu, 1 );
					PAD_SetVSType( pme->pad, VS_TYPE1 );
					goto	vsexit;
				case	0xffbef374:	// VS Castlevania
					PPU_SetVSColorMap( pme->ppu, 1 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0x135adf7c:	// VS Atari RBI Baseball
					PPU_SetVSColorMap( pme->ppu, 2 );
					PAD_SetVSType( pme->pad, VS_TYPE1 );
					goto	vsexit;
				case	0xd5d7eac4:	// VS Dr. Mario
					PPU_SetVSColorMap( pme->ppu, 2 );
					PAD_SetVSType( pme->pad, VS_TYPE1 );
					goto	vsexit;
				case	0x46914e3e:	// VS Soccer
					PPU_SetVSColorMap( pme->ppu, 2 );
					PAD_SetVSType( pme->pad, VS_TYPE1 );
					goto	vsexit;
				case	0x70433f2c:	// VS Battle City
				case	0x8d15a6e6:	// VS bad .nes
					PPU_SetVSColorMap( pme->ppu, 2 );
					PAD_SetVSType( pme->pad, VS_TYPE1 );
					goto	vsexit;
				case	0x1e438d52:	// VS Goonies
					PPU_SetVSColorMap( pme->ppu, 2 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0xcbe85490:	// VS Excitebike
					PPU_SetVSColorMap( pme->ppu, 2 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0x29155e0c:	// VS Excitebike (alt)
					PPU_SetVSColorMap( pme->ppu, 3 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0x07138c06:	// VS Clu Clu Land
					PPU_SetVSColorMap( pme->ppu, 3 );
					PAD_SetVSType( pme->pad, VS_TYPE1 );
					goto	vsexit;
				case	0x43a357ef:	// VS Ice Climber
					PPU_SetVSColorMap( pme->ppu, 3 );
					PAD_SetVSType( pme->pad, VS_TYPE1 );
					goto	vsexit;
				case	0x737dd1bf:	// VS Super Mario Bros
				case	0x4bf3972d:	// VS Super Mario Bros
				case	0x8b60cc58:	// VS Super Mario Bros
				case	0x8192c804:	// VS Super Mario Bros
					PPU_SetVSColorMap( pme->ppu, 3 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;
				case	0xd99a2087:	// VS Gradius
					PPU_SetVSColorMap( pme->ppu, 4 );
					PAD_SetVSType( pme->pad, VS_TYPE1 );
					goto	vsexit;
				case	0xf9d3b0a3:	// VS Super Xevious
				case	0x9924980a:	// VS Super Xevious
				case	0x66bb838f:	// VS Super Xevious
					PPU_SetVSColorMap( pme->ppu, 4 );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;

				case	0xc99ec059:	// VS Raid on Bungeling Bay(J)
					PPU_SetVSColorMap( pme->ppu, 1 );
					PAD_SetVSType( pme->pad, VS_TYPE5 );
					goto	vsexit;
				case	0xca85e56d:	// VS Mighty Bomb Jack(J)
					PPU_SetVSSecurity( pme->ppu, 0x3D );
					PAD_SetVSType( pme->pad, VS_TYPE0 );
					goto	vsexit;

			vsexit:
					PPU_SetVSMode( pme->ppu, TRUE );
					break;
			}
		} else {
			pme->m_VSDipValue = 0;
			pme->m_VSDipTable = vsdip_default;
		}

		NES_Reset(pme);
#if 0 // alvin
		// ゲーム固有のデフォルトオプションを設定(設定戻す時に使う為)
		GameOption.defRenderMethod = (INT)NES_GetRenderMethod(pme);
		GameOption.defIRQtype      = (INT)NES_GetIrqType(pme);
		GameOption.defFrameIRQ     = NES_GetFrameIRQmode(pme);
		GameOption.defVideoMode    = NES_GetVideoMode(pme);

		// 設定をロードして設定する(エントリが無ければデフォルトが入る)
		if( ROM_GetMapperNo(pme->rom) != 20 ) {
			GameOption.Load( ROM_GetPROM_CRC(pme->rom) );
		} else {
			GameOption.Load( ROM_GetGameID(pme->rom), ROM_GetMakerID(pme->rom) );
		}
		NES_SetRenderMethod( pme, (RENDERMETHOD)GameOption.nRenderMethod );
		NES_SetIrqType ( pme, GameOption.nIRQtype );
		NES_SetFrameIRQmode( pme, GameOption.bFrameIRQ );
		NES_SetVideoMode ( pme, GameOption.bVideoMode );
#endif
	DebugOut( "Starting emulation!\n" );

	return pme;
FAIL:
	DebugOut("failed.\n");
	CPU_Destroy( pme->cpu ); pme->cpu = NULL;
	PPU_Destroy( pme->ppu ); pme->ppu = NULL;
	APU_Destroy(pme->apu); pme->apu = NULL;
	PAD_Destroy( pme->pad ); pme->pad = NULL;
	ROM_Destroy( pme->rom ); pme->rom = NULL;
	Mapper_Destroy( pme->mapper ); pme->mapper = NULL;
	FREE(pme);
	return NULL;
}

void NES_Destroy(NES *pme)
{
	if(pme){
		NES_SaveSRAM(pme);
		NES_SaveDISK(pme);
		
		DebugOut( "Free NES..." );
		
		CPU_Destroy( pme->cpu ); pme->cpu = NULL;
		PPU_Destroy( pme->ppu ); pme->ppu = NULL;
		APU_Destroy(pme->apu); pme->apu = NULL;
		PAD_Destroy( pme->pad ); pme->pad = NULL;
		ROM_Destroy( pme->rom ); pme->rom = NULL;
		Mapper_Destroy( pme->mapper ); pme->mapper = NULL;

		FREE(pme);
		DebugOut( "Ok.\n" );
	}
}

void	NES_Reset(NES *pme)
{
	NES_SaveSRAM(pme);
	NES_SaveDISK(pme);

	// RAM Clear
	ZEROMEMORY( RAM, sizeof(RAM) );
	if( ROM_GetPROM_CRC(pme->rom) == 0x29401686 ) {	// Minna no Taabou no Nakayoshi Dai Sakusen(J)
		memset( RAM, 0xFF, sizeof(RAM) );
	}

	// RAM set
	if( !ROM_IsSAVERAM(pme->rom) && ROM_GetMapperNo(pme->rom) != 20 ) {
		memset( WRAM, 0xFF, sizeof(WRAM) );
	}

	ZEROMEMORY( CRAM, sizeof(CRAM) );
	ZEROMEMORY( VRAM, sizeof(VRAM) );

	ZEROMEMORY( SPRAM, sizeof(SPRAM) );
	ZEROMEMORY( BGPAL, sizeof(BGPAL) );
	ZEROMEMORY( SPPAL, sizeof(SPPAL) );

	ZEROMEMORY( CPUREG, sizeof(CPUREG) );
	ZEROMEMORY( PPUREG, sizeof(PPUREG) );

	pme->m_bDiskThrottle = FALSE;

	NES_SetRenderMethod( pme, PRE_RENDER );

	if( ROM_IsPAL(pme->rom) ) {
		NES_SetVideoMode( pme, TRUE );
	}

	PROM = ROM_GetPROM(pme->rom);
	VROM = ROM_GetVROM(pme->rom);

	PROM_8K_SIZE  = ROM_GetPROM_SIZE(pme->rom)*2;
	PROM_16K_SIZE = ROM_GetPROM_SIZE(pme->rom);
	PROM_32K_SIZE = ROM_GetPROM_SIZE(pme->rom)/2;

	VROM_1K_SIZE = ROM_GetVROM_SIZE(pme->rom)*8;
	VROM_2K_SIZE = ROM_GetVROM_SIZE(pme->rom)*4;
	VROM_4K_SIZE = ROM_GetVROM_SIZE(pme->rom)*2;
	VROM_8K_SIZE = ROM_GetVROM_SIZE(pme->rom);

	// デフォルトバンク
	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank_cont( 0 );
	} else {
		SetCRAM_8K_Bank( 0 );
	}

	// ミラー
	if( ROM_Is4SCREEN(pme->rom) ) {
		SetVRAM_Mirror_cont( VRAM_MIRROR4 );
	} else if( ROM_IsVMIRROR(pme->rom) ) {
		SetVRAM_Mirror_cont( VRAM_VMIRROR );
	} else {
		SetVRAM_Mirror_cont( VRAM_HMIRROR );
	}

	APU_SelectExSound( pme->apu, 0 );

	PPU_Reset(pme->ppu);
	Mapper_Reset(pme->mapper);

	// Trainer
	if( ROM_IsTRAINER(pme->rom) ) {
		memcpy( WRAM+0x1000, ROM_GetTRAINER(pme->rom), 512 );
	}

	PAD_Reset(pme->pad);
	CPU_Reset(pme->cpu);
	APU_Reset(pme->apu);

	if( ROM_IsNSF(pme->rom) ) {
		Mapper_Reset(pme->mapper);
	}

	pme->base_cycles = pme->emul_cycles = 0;
}

void	NES_SoftReset(NES *pme)
{
	PAD_Reset(pme->pad);
	CPU_Reset(pme->cpu);
	APU_Reset(pme->apu);

	if( ROM_IsNSF(pme->rom) ) {
		Mapper_Reset(pme->mapper);
	}

	pme->m_bDiskThrottle = FALSE;

	pme->base_cycles = pme->emul_cycles = 0;
}

void	NES_EmulationCPU( NES *pme, INT basecycles )
{
INT	cycles;

	pme->base_cycles += basecycles;
	cycles = (INT)((pme->base_cycles/12)-pme->emul_cycles);

	if( cycles > 0 ) {
		pme->emul_cycles += CPU_EXEC( pme->cpu, cycles );
	}
}

void	NES_EmulationCPU_BeforeNMI( NES *pme, INT cycles )
{
	pme->base_cycles += cycles;
	pme->emul_cycles += CPU_EXEC( pme->cpu, cycles/12 );
}

/*
	描画シーケンス
	0		ダミースキャンライン(描画しない)
	1 - 239		描画
	240		ダミースキャンライン,VBLANKフラグON
	241		VINT期間,NMI発生
	242-261		VINT期間
	261		VINT期間,VBLANKフラグOFF
*/
void	NES_EmulateFrame( NES *pme, BOOL bDraw )
{
INT	scanline = 0;

	// NSFプレイヤの時
	if( ROM_IsNSF(pme->rom) ) {
		NES_EmulateNSF(pme);
		return;
	}

	// Cheat
	//CheatCodeProcess();
	
	//
	pme->NES_scanline = scanline;

	if( pme->RenderMethod != TILE_RENDER ) {
		pme->bZapper = FALSE;
		while( TRUE ) {
			PPU_SetRenderScanline( pme->ppu, scanline );

			if( scanline == 0 ) {
			// ダミースキャンライン
				if( pme->RenderMethod < POST_RENDER ) {
					NES_EmulationCPU( pme, pme->nescfg->ScanlineCycles );
					PPU_FrameStart(pme->ppu);
					PPU_ScanlineNext(pme->ppu);
					Mapper_HSync( pme->mapper, scanline );
					PPU_ScanlineStart(pme->ppu);
				} else {
					NES_EmulationCPU( pme, pme->nescfg->HDrawCycles );
					PPU_FrameStart(pme->ppu);
					PPU_ScanlineNext(pme->ppu);
					Mapper_HSync(pme->mapper, scanline);
					NES_EmulationCPU( pme, FETCH_CYCLES*32 );
					PPU_ScanlineStart(pme->ppu);
					NES_EmulationCPU( pme, FETCH_CYCLES*10+pme->nescfg->ScanlineEndCycles );
				}
			} else if( scanline < SCREEN_HEIGHT ) {
				if( pme->RenderMethod < POST_RENDER ) {
					if( pme->RenderMethod == POST_ALL_RENDER )
						NES_EmulationCPU( pme, pme->nescfg->ScanlineCycles );
					if( bDraw ) {
						PPU_Scanline( pme->ppu, scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
					} else {
						if( PAD_IsZapperMode(pme->pad) && scanline == pme->ZapperY ) {
							PPU_Scanline( pme->ppu, scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
						} else {
							if( !PPU_IsSprite0( pme->ppu, scanline ) ) {
								PPU_DummyScanline( pme->ppu, scanline );
							} else {
								PPU_Scanline( pme->ppu, scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
							}
						}
					}
					PPU_ScanlineNext(pme->ppu);				// これの位置でラスター系は画面が違う
					if( pme->RenderMethod == PRE_ALL_RENDER )
						NES_EmulationCPU( pme, pme->nescfg->ScanlineCycles );
//					pme->ppu->ScanlineNext();				// これの位置でラスター系は画面が違う
					Mapper_HSync(pme->mapper, scanline);
					PPU_ScanlineStart(pme->ppu);
				} else {
					if( pme->RenderMethod == POST_RENDER )
						NES_EmulationCPU( pme, pme->nescfg->HDrawCycles );
					if( bDraw ) {
						PPU_Scanline( pme->ppu, scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
					} else {
						if( PAD_IsZapperMode(pme->pad) && scanline == pme->ZapperY ) {
							PPU_Scanline( pme->ppu, scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
						} else {
							if( !PPU_IsSprite0( pme->ppu, scanline ) ) {
								PPU_DummyScanline( pme->ppu, scanline );
							} else {
								PPU_Scanline( pme->ppu, scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
							}
						}
					}
					if( pme->RenderMethod == PRE_RENDER )
						NES_EmulationCPU( pme, pme->nescfg->HDrawCycles );
					PPU_ScanlineNext(pme->ppu);
					Mapper_HSync(pme->mapper, scanline);
					NES_EmulationCPU( pme, FETCH_CYCLES*32 );
					PPU_ScanlineStart(pme->ppu);
					NES_EmulationCPU( pme, FETCH_CYCLES*10+pme->nescfg->ScanlineEndCycles );
				}
			} else if( scanline == SCREEN_HEIGHT ) {
				Mapper_VSync(pme->mapper);
				if( pme->RenderMethod < POST_RENDER ) {
					NES_EmulationCPU( pme, pme->nescfg->ScanlineCycles );
					Mapper_HSync(pme->mapper, scanline);
				} else {
					NES_EmulationCPU( pme, pme->nescfg->HDrawCycles );
					Mapper_HSync(pme->mapper, scanline);
					NES_EmulationCPU( pme, pme->nescfg->HBlankCycles );
				}
			} else if( scanline <= pme->nescfg->TotalScanlines-1 ) {
				PAD_VSync(pme->pad);

				// VBLANK期間
				if( scanline == pme->nescfg->TotalScanlines-1 ) {
					PPU_VBlankEnd(pme->ppu);
				}
				if( pme->RenderMethod < POST_RENDER ) {
					if( scanline == 241 ) {
						PPU_VBlankStart(pme->ppu);
						if( PPUREG[0] & PPU_VBLANK_BIT ) {
							CPU_NMI(pme->cpu);
						}
					}
					NES_EmulationCPU( pme, pme->nescfg->ScanlineCycles );
					Mapper_HSync(pme->mapper, scanline);
				} else {
					if( scanline == 241 ) {
						PPU_VBlankStart(pme->ppu);
						if( PPUREG[0] & PPU_VBLANK_BIT ) {
							CPU_NMI(pme->cpu);
						}
					}
					NES_EmulationCPU( pme, pme->nescfg->HDrawCycles );
					Mapper_HSync(pme->mapper, scanline);
					NES_EmulationCPU( pme, pme->nescfg->HBlankCycles );
				}

				if( scanline == pme->nescfg->TotalScanlines-1 ) {
					break;
				}
			}
			if( PAD_IsZapperMode(pme->pad) ) {
				if( scanline == pme->ZapperY )
					pme->bZapper = TRUE;
				else
					pme->bZapper = FALSE;
			}

			scanline++;
			pme->NES_scanline = scanline;
		}
	} else {
		pme->bZapper = FALSE;
		while( TRUE ) {
			PPU_SetRenderScanline( pme->ppu, scanline );

			if( scanline == 0 ) {
			// ダミースキャンライン
				// H-Draw (4fetches*32)
				NES_EmulationCPU( pme, FETCH_CYCLES*128 );
				PPU_FrameStart(pme->ppu);
				PPU_ScanlineNext(pme->ppu);
				NES_EmulationCPU( pme, FETCH_CYCLES*10 );
				Mapper_HSync(pme->mapper, scanline);
				NES_EmulationCPU( pme, FETCH_CYCLES*22 );
				PPU_ScanlineStart(pme->ppu);
				NES_EmulationCPU( pme, FETCH_CYCLES*10+pme->nescfg->ScanlineEndCycles );
			} else if( scanline < SCREEN_HEIGHT ) {
			// スクリーン描画(Scanline 1～239)
				if( bDraw ) {
					PPU_Scanline( pme->ppu, scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
					PPU_ScanlineNext(pme->ppu);
					NES_EmulationCPU( pme, FETCH_CYCLES*10 );
					Mapper_HSync(pme->mapper, scanline);
					NES_EmulationCPU( pme, FETCH_CYCLES*22 );
					PPU_ScanlineStart(pme->ppu);
					NES_EmulationCPU( pme, FETCH_CYCLES*10+pme->nescfg->ScanlineEndCycles );
				} else {
					if( PAD_IsZapperMode(pme->pad) && scanline == pme->ZapperY ) {
						PPU_Scanline( pme->ppu, scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
						PPU_ScanlineNext(pme->ppu);
						NES_EmulationCPU( pme, FETCH_CYCLES*10 );
						Mapper_HSync(pme->mapper, scanline);
						NES_EmulationCPU( pme, FETCH_CYCLES*22 );
						PPU_ScanlineStart(pme->ppu);
						NES_EmulationCPU( pme, FETCH_CYCLES*10+pme->nescfg->ScanlineEndCycles );
					} else {
						if( !PPU_IsSprite0( pme->ppu, scanline ) ) {
							// H-Draw (4fetches*32)
							NES_EmulationCPU( pme, FETCH_CYCLES*128 );
							PPU_DummyScanline( pme->ppu, scanline );
							PPU_ScanlineNext(pme->ppu);
							NES_EmulationCPU( pme, FETCH_CYCLES*10 );
							Mapper_HSync(pme->mapper, scanline);
							NES_EmulationCPU( pme, FETCH_CYCLES*22 );
							PPU_ScanlineStart(pme->ppu);
							NES_EmulationCPU( pme, FETCH_CYCLES*10+pme->nescfg->ScanlineEndCycles );
						} else {
							PPU_Scanline( pme->ppu, scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
							PPU_ScanlineNext(pme->ppu);
							NES_EmulationCPU( pme, FETCH_CYCLES*10 );
							Mapper_HSync(pme->mapper, scanline);
							NES_EmulationCPU( pme, FETCH_CYCLES*22 );
							PPU_ScanlineStart(pme->ppu);
							NES_EmulationCPU( pme, FETCH_CYCLES*10+pme->nescfg->ScanlineEndCycles );
						}
					}
				}
			} else if( scanline == SCREEN_HEIGHT ) {
			// ダミースキャンライン (Scanline 240)
				Mapper_VSync(pme->mapper);

				NES_EmulationCPU( pme, pme->nescfg->HDrawCycles );
				// H-Sync
				Mapper_HSync( pme->mapper, scanline );

				NES_EmulationCPU( pme, pme->nescfg->HBlankCycles );
			} else if( scanline <= pme->nescfg->TotalScanlines-1 ) {
				PAD_VSync(pme->pad);

			// VBLANK期間
				if( scanline == pme->nescfg->TotalScanlines-1 ) {
					PPU_VBlankEnd(pme->ppu);
				}
				if( scanline == 241 ) {
					PPU_VBlankStart(pme->ppu);
					if( PPUREG[0]&PPU_VBLANK_BIT ) {
						CPU_NMI(pme->cpu);
					}
				}
				NES_EmulationCPU( pme, pme->nescfg->HDrawCycles );

				// H-Sync
				Mapper_HSync( pme->mapper, scanline );

				NES_EmulationCPU( pme, pme->nescfg->HBlankCycles );

				if( scanline == pme->nescfg->TotalScanlines-1 ) {
					break;
				}
			}
			if( PAD_IsZapperMode(pme->pad) ) {
				if( scanline == pme->ZapperY )
					pme->bZapper = TRUE;
				else
					pme->bZapper = FALSE;
			}

			scanline++;
			pme->NES_scanline = scanline;
		}
	}

#if	NES_PROFILER
	{
	char	str[256];
	::wsprintf( str, "Cyc:%10d", pme->m_dwProfileCycle );
	NES_DrawString( pme, 9, 240-32, str, 0x1F );
	NES_DrawString( pme, 8, 240-33, str, 0x30 );
	::wsprintf( str, "Ave:%10d", pme->m_dwProfileAveCycle );
	NES_DrawString( pme, 9, 240-23, str, 0x1F );
	NES_DrawString( pme, 8, 240-24, str, 0x30 );
	::wsprintf( str, "Max:%10d", pme->m_dwProfileMaxCycle );
	NES_DrawString( pme, 9, 240-14, str, 0x1F );
	NES_DrawString( pme, 8, 240-15, str, 0x30 );
	}
#endif
}

void	NES_EmulateNSF(NES *pme)
{
R6502	reg;

	PPU_Reset(pme->ppu);
	Mapper_VSync(pme->mapper);

//DebugOut( "Frame\n" );

	if( pme->m_bNsfPlaying ) {
		INT i;

 		if( pme->m_bNsfInit ) {
			ZEROMEMORY( RAM, sizeof(RAM) );
			if( !(ROM_GetNsfHeader(pme->rom)->ExtraChipSelect&0x04) ) {
				ZEROMEMORY( WRAM, 0x2000 );
			}

			APU_Reset(pme->apu);
			APU_Write( pme->apu, 0x4015, 0x0F );
			APU_Write( pme->apu, 0x4017, 0xC0 );
			APU_ExWrite( pme->apu, 0x4080, 0x80 );	// FDS Volume 0
			APU_ExWrite( pme->apu, 0x408A, 0xE8 );	// FDS Envelope Speed

			CPU_GetContext( pme->cpu, &reg );
			reg.PC = 0x4710;	// Init Address
			reg.A  = (BYTE)pme->m_nNsfSongNo;
			reg.X  = (BYTE)pme->m_nNsfSongMode;
			reg.Y  = 0;
			reg.S  = 0xFF;
			reg.P  = Z_FLAG|R_FLAG|I_FLAG;
			CPU_SetContext( pme->cpu, &reg );

			// 安全対策を兼ねてあえてループに(1秒分)
			for(  i = 0; i < pme->nescfg->TotalScanlines*60; i++ ) {
				NES_EmulationCPU( pme, pme->nescfg->ScanlineCycles );
				CPU_GetContext( pme->cpu, &reg );

				// 無限ループに入ったことを確認したら抜ける
				if( reg.PC == 0x4700 ) {
					break;
				}
			}

			pme->m_bNsfInit = FALSE;
		}

		CPU_GetContext( pme->cpu, &reg );
		// 無限ループに入っていたら再設定する
		if( reg.PC == 0x4700 ) {
			reg.PC = 0x4720;	// Play Address
			reg.A  = 0;
			reg.S  = 0xFF;
			CPU_SetContext( pme->cpu, &reg );
		}

		for(  i = 0; i < pme->nescfg->TotalScanlines; i++ ) {
			NES_EmulationCPU( pme, pme->nescfg->ScanlineCycles );
		}
	} else {
		CPU_GetContext( pme->cpu, &reg );
		reg.PC = 0x4700;	// 無限ループ
		reg.S  = 0xFF;
		CPU_SetContext( pme->cpu, &reg );

		NES_EmulationCPU( pme, pme->nescfg->ScanlineCycles*pme->nescfg->TotalScanlines );
	}
}

void	NES_SetNsfPlay( NES *pme, INT songno, INT songmode )
{
	pme->m_bNsfPlaying  = TRUE;
	pme->m_bNsfInit     = TRUE;
	pme->m_nNsfSongNo   = songno;
	pme->m_nNsfSongMode = songmode;
}

void	NES_SetNsfStop(NES *pme)
{
	pme->m_bNsfPlaying = FALSE;
	APU_Reset(pme->apu);
}
BOOL	NES_IsNsfPlaying(NES *pme) { return pme->m_bNsfPlaying; }

void	NES_Clock( NES *pme, INT cycles )
{
	NES_Barcode( pme, cycles );
}
void	NES_SetIrqType( NES *pme, INT nType ) { pme->nIRQtype = nType; }
INT NES_GetIrqType(NES *pme) { return (INT)pme->nIRQtype; }

// Frame-IRQ control (for Paris-Dakar Rally Special)
void	NES_SetFrameIRQmode( NES *pme, BOOL bMode ) { pme->bFrameIRQ = bMode; }
BOOL	NES_GetFrameIRQmode(NES *pme) { return pme->bFrameIRQ; }

// NTSC/PAL

void	NES_SetVideoMode( NES *pme, BOOL bMode )
{	
	pme->bVideoMode = bMode;
	if( !pme->bVideoMode ) {
		pme->nescfg = &NESCONFIG_NTSC;
	} else {
		pme->nescfg = &NESCONFIG_PAL;
	}
	APU_SoundSetup(pme->apu);
}

BOOL	NES_GetVideoMode(NES *pme) { return pme->bVideoMode; }

BYTE	NES_Read( NES *pme, WORD addr )
{
	switch( addr>>13 ) {
		case	0x00:	// $0000-$1FFF
			return	RAM[addr&0x07FF];
		case	0x01:	// $2000-$3FFF
			return	PPU_Read( pme->ppu, addr&0xE007 );
		case	0x02:	// $4000-$5FFF
			if( addr < 0x4100 ) {
				return	NES_ReadReg( pme, addr );
			} else {
				return	Mapper_ReadLow( pme->mapper, addr );
			}
			break;
		case	0x03:	// $6000-$7FFF
			return	Mapper_ReadLow( pme->mapper, addr );
		case	0x04:	// $8000-$9FFF
		case	0x05:	// $A000-$BFFF
		case	0x06:	// $C000-$DFFF
		case	0x07:	// $E000-$FFFF
			return	CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	}

	return	0x00;	// Warning予防
}

void	NES_Write( NES *pme, WORD addr, BYTE data )
{
	switch( addr>>13 ) {
		case	0x00:	// $0000-$1FFF
			RAM[addr&0x07FF] = data;
			break;
		case	0x01:	// $2000-$3FFF
			if( !ROM_IsNSF(pme->rom) ) {
				PPU_Write( pme->ppu, addr&0xE007, data );
			}
			break;
		case	0x02:	// $4000-$5FFF
			if( addr < 0x4100 ) {
				NES_WriteReg( pme, addr, data );
			} else {
				Mapper_WriteLow( pme->mapper, addr, data );
			}
			break;
		case	0x03:	// $6000-$7FFF
			Mapper_WriteLow( pme->mapper, addr, data );
			break;
		case	0x04:	// $8000-$9FFF
		case	0x05:	// $A000-$BFFF
		case	0x06:	// $C000-$DFFF
		case	0x07:	// $E000-$FFFF
			Mapper_Write( pme->mapper, addr, data );

			#if 0 // alvin
			GenieCodeProcess();
			#endif
			break;
	}
}

BYTE	NES_ReadReg( NES *pme, WORD addr )
{
	switch( addr & 0xFF ) {
		case 0x00: case 0x01: case 0x02: case 0x03:
		case 0x04: case 0x05: case 0x06: case 0x07:
		case 0x08: case 0x09: case 0x0A: case 0x0B:
		case 0x0C: case 0x0D: case 0x0E: case 0x0F:
		case 0x10: case 0x11: case 0x12: case 0x13:
			return	APU_Read( pme->apu, addr );
			break;
		case	0x15:
			return	APU_Read( pme->apu, addr );
			break;

		case	0x14:
			return	addr&0xFF;
			break;

		case	0x16:
			if( ROM_IsVSUNISYSTEM(pme->rom) ) {
				return	PAD_Read( pme->pad, addr );
			} else {
				return	PAD_Read( pme->pad, addr ) | 0x40;
			}
			break;
		case	0x17:
			if( ROM_IsVSUNISYSTEM(pme->rom) ) {
				return	PAD_Read( pme->pad, addr );
			} else {
				return	PAD_Read( pme->pad, addr ) | APU_Read( pme->apu, addr );
			}
			break;
		default:
			return	Mapper_ExRead( pme->mapper, addr );
			break;
	}
}

void	NES_WriteReg( NES *pme, WORD addr, BYTE data )
{
	switch( addr & 0xFF ) {
		case 0x00: case 0x01: case 0x02: case 0x03:
		case 0x04: case 0x05: case 0x06: case 0x07:
		case 0x08: case 0x09: case 0x0A: case 0x0B:
		case 0x0C: case 0x0D: case 0x0E: case 0x0F:
		case 0x10: case 0x11: case 0x12: case 0x13:
		case 0x15:
			APU_Write( pme->apu, addr, data );
			CPUREG[addr & 0xFF] = data;
			break;

		case	0x14:
			PPU_DMA( pme->ppu, data );
			CPU_DMA( pme->cpu, 514 ); // DMA Pending cycle
			CPUREG[addr & 0xFF] = data;
			break;

		case	0x16:
			Mapper_ExWrite( pme->mapper, addr, data );	// For VS-Unisystem
			PAD_Write( pme->pad, addr, data );
			CPUREG[addr & 0xFF] = data;
			break;
		case	0x17:
			CPUREG[addr & 0xFF] = data;
			PAD_Write( pme->pad, addr, data );
			APU_Write( pme->apu, addr, data );
			break;
		// VirtuaNES固有ポート
		case	0x18:
			APU_Write( pme->apu, addr, data );
			break;

#if	NES_PROFILER
		case	0x1D:
			pme->m_dwProfileAveCycle = 0;
			pme->m_dwProfileMaxCycle = 0;
			break;
		case	0x1E:
			pme->m_dwProfileTempCycle = CPU_GetTotalCycles(pme->cpu);
			break;
		case	0x1F:
			pme->m_dwProfileCycle = CPU_GetTotalCycles(pme->cpu)-pme->m_dwProfileTempCycle-4;
			if( !pme->m_dwProfileAveCycle ) {
				pme->m_dwProfileAveCycle += pme->m_dwProfileCycle;
			} else {
				pme->m_dwProfileAveCycle += pme->m_dwProfileCycle;
				pme->m_dwProfileAveCycle /= 2;
			}
			if( pme->m_dwProfileMaxCycle < pme->m_dwProfileCycle ) {
				pme->m_dwProfileMaxCycle = pme->m_dwProfileCycle;
			}
			break;
#endif
#if	0
		case	0x1C:
			pme->m_dwProfileTempCycle = pme->cpu->GetTotalCycles();
			break;
		case	0x1D:
			pme->m_dwProfileCycle = pme->cpu->GetTotalCycles()-pme->m_dwProfileTempCycle-4;
			pme->m_dwProfileTotalCycle += pme->m_dwProfileCycle;
			pme->m_dwProfileTotalCount++;
			break;
		case	0x1E:
			pme->m_dwProfileTotalCount = 0;
			pme->m_dwProfileTotalCycle = 0;
			pme->m_dwTotalTempCycle = pme->cpu->GetTotalCycles();
			break;
		case	0x1F:
			pme->m_dwTotalCycle = pme->cpu->GetTotalCycles()-pme->m_dwTotalTempCycle-4;
			break;
#endif
		default:
			Mapper_ExWrite( pme->mapper, addr, data );
			break;
	}
}

static void	NES_LoadSRAM(NES *pme)
{
	char fname[256];
	t_HIO gio;
	int size = sizeof(WRAM);
	
	if( ROM_IsNSF(pme->rom) )
		return;

	ZEROMEMORY( WRAM, sizeof(WRAM) );

	if( !ROM_IsSAVERAM(pme->rom) )
		return;

	sprintf(fname, "%s/%s.sav", ROM_GetRomPath(pme->rom), ROM_GetRomName(pme->rom));
	DebugOut( "NES_LoadSRAM: %s\n", fname );

	gio = gio_new_file(fname, _O_RDONLY);
	if(!g_object_valid(gio)){
		DebugOut( "Loading SAVERAM Error: open file failed.\n" );
		return;
	}
	DebugOut( "Loading SAVERAM..." );
	
	size = gio_size(gio);
	if( size > 128*1024 || gio_read( gio, WRAM, size ) != size) {
		DebugOut( "Loading SAVERAM Error.\n" );
	}else{
		DebugOut( "Ok.\n" );
	}
	
	g_object_unref(gio);
}

void	NES_SaveSRAM(NES *pme)
{
INT	i;

	if( ROM_IsNSF(pme->rom) )
		return;

	if( !ROM_IsSAVERAM(pme->rom) )
		return;

	for( i = 0; i < pme->SAVERAM_SIZE; i++ ) {
		if( WRAM[i] != 0x00 )
			break;
	}

	if( i < pme->SAVERAM_SIZE ) {
		char fname[256];
		t_HIO gio;
		
		DebugOut( "Saving SAVERAM...\n" );

		sprintf(fname, "%s/%s.sav", ROM_GetRomPath(pme->rom), ROM_GetRomName(pme->rom));
		DebugOut( "NES_SaveSRAM: %s\n", fname );

		gio = gio_new_file(fname, _O_RDONLY);
		if(!g_object_valid(gio)){
			DebugOut( "Writing SAVERAM Error: open file failed.\n" );
			return;
		}
		if(gio_write(gio, WRAM, pme->SAVERAM_SIZE) != pme->SAVERAM_SIZE){
			DebugOut( "Writing SAVERAM Error: write file failed.\n" );
		}else{
			DebugOut( "Ok.\n" );
		}
		g_object_unref(gio);
	}
}

BOOL	NES_LoadDISK(NES *pme)
{

	BOOL	bExit = FALSE;

	INT	i, j, diskno;
	DISKIMGFILEHDR	ifh;
	DISKIMGHDR	hdr;
	LPBYTE		disk;

	WORD	Version;
	char fname[200];
	t_HIO gio = NULL;
	
	if( ROM_GetMapperNo(pme->rom) != 20 )
		return TRUE;

	sprintf(fname, "%s/%s.dsv", ROM_GetRomPath(pme->rom), ROM_GetRomName(pme->rom));
	DebugOut( "Path: %s\n", fname );

	gio = gio_new_file(fname, _O_RDONLY);
	if(!g_object_valid(gio)){
		DebugOut("NES_LoadDISK error: open file");
		return TRUE;
	}
	if(gio_read(gio, &ifh, sizeof(DISKIMGFILEHDR)) != sizeof(DISKIMGFILEHDR)){
		DebugOut("NES_LoadDISK error: read DISKIMGFILEHDR");
		goto out;
	}
	if( memcmp( ifh.ID, "VirtuaNES DI", sizeof(ifh.ID) ) 
		||ifh.BlockVersion < 0x0100
		||ifh.BlockVersion > 0x200) {
		DebugOut("NES_LoadDISK error: unsupported format");
		goto out;
	}
	Version = ifh.BlockVersion;
	if( Version == 0x0100 ) {
		// Ver0.24以前
		if( ifh.DiskNumber > 4 ) {
			DebugOut("NES_LoadDISK error: unsupported format");
			goto out;
		}
	
		for( i = 0; i < (INT)ifh.DiskNumber; i++ ) {
			if( gio_read( gio, &hdr, sizeof(DISKIMGHDR)) != sizeof(DISKIMGHDR) ) {
				if( i == 0 ) {
					DebugOut("NES_LoadDISK error: read DISKIMGHDR");
					goto out;
				} else {
					break;
				}
			}
	
			if( memcmp( hdr.ID, "SIDE0A", sizeof(hdr.ID) ) == 0 ) {
				diskno = 0;
			} else if( memcmp( hdr.ID, "SIDE0B", sizeof(hdr.ID) ) == 0 ) {
				diskno = 1;
			} else if( memcmp( hdr.ID, "SIDE1A", sizeof(hdr.ID) ) == 0 ) {
				diskno = 2;
			} else if( memcmp( hdr.ID, "SIDE1B", sizeof(hdr.ID) ) == 0 ) {
				diskno = 3;
			} else {
				DebugOut("NES_LoadDISK error: unsupported format");
				goto out;
			}
	
			for( j = 0; j < 16; j++ ) {
				if( hdr.DiskTouch[j] ) {
					disk = ROM_GetPROM(pme->rom)+16+65500*diskno+(4*1024)*j;
					if( j < 15 ) {
						if( gio_read( gio, disk, 4*1024 ) != 4*1024 ) {
							bExit = TRUE;
							DebugOut("NES_LoadDISK error: read");
							goto out;
						}
					} else {
						if( gio_read( gio, disk, 4*1024-36 ) != 4*1024-36 ) {
							bExit = TRUE;
							DebugOut("NES_LoadDISK error: read");
							goto out;
						}
					}
				}
			}
		}
	} else 
	if( Version == 0x0200 || Version == 0x0210 ) {
		// Ver0.30以降
		DISKFILEHDR dfh;
		LPBYTE	lpDisk = ROM_GetPROM(pme->rom);
		LPBYTE	lpWrite = ROM_GetDISK(pme->rom);
		LONG	DiskSize = 16+65500*ROM_GetDiskNo(pme->rom);
		DWORD	pos;
		BYTE	data;
		
		// 書き換えFLAG消去
		ZEROMEMORY( lpWrite, 16+65500*ROM_GetDiskNo(pme->rom) );
		
		// ヘッダ読み直し
		if( gio_seek( gio, 0, SEEK_SET ) < 0) {
			// ファイルの読み込みに失敗しました
			DebugOut("NES_LoadDISK error: read");
			goto out;
		}
		if( gio_read( gio, &dfh, sizeof(DISKFILEHDR) ) != sizeof(DISKFILEHDR) ) {
			DebugOut("NES_LoadDISK error: read DISKFILEHDR");
			goto out;
		}
		
		if( 1/*alvin: Config.emulator.bCrcCheck*/ ) {
			// 現在ロード中のタイトルと違うかをチェック
			if( dfh.ProgID	!=		 ROM_GetGameID(pme->rom)
			 || dfh.MakerID != (WORD)ROM_GetMakerID(pme->rom)
			 || dfh.DiskNo	!= (WORD)ROM_GetDiskNo(pme->rom) ) {
				DebugOut("NES_LoadDISK error: read");
				goto out;
			}
		}

		for( i = 0; i < dfh.DifferentSize; i++ ) {
			if( gio_read( gio, &pos, sizeof(DWORD) ) != sizeof(DWORD) ) {
				bExit = TRUE;
				DebugOut("NES_LoadDISK error: read");
				goto out;
			}
			data = (BYTE)(pos>>24);
			pos &= 0x00FFFFFF;
			if( pos >= 16 && pos < DiskSize ) {
				lpDisk[pos] = data;
				lpWrite[pos] = 0xFF;
			}
		}
	}	
out:
	g_object_unref(gio);
	return (bExit)?FALSE : TRUE;
	
}

void	NES_SaveDISK(NES *pme)
{
	INT	i;
	DISKFILEHDR ifh;
	LPBYTE	lpDisk  = ROM_GetPROM(pme->rom);
	LPBYTE	lpWrite = ROM_GetDISK(pme->rom);
	LONG	DiskSize = 16+65500*ROM_GetDiskNo(pme->rom);
	DWORD	data;


	char fname[200];
	t_HIO gio = NULL;

	if( ROM_GetMapperNo(pme->rom) != 20 )
		return;

	sprintf(fname, "%s/%s.dsv", ROM_GetRomPath(pme->rom), ROM_GetRomName(pme->rom));
	DebugOut( "Path: %s\n", fname );

	gio = gio_new_file(fname, _O_WRONLY);
	if(!g_object_valid(gio)){
		DebugOut("NES_SaveDISK error: open file");
		return;
	}
	
	ZEROMEMORY( &ifh, sizeof(ifh) );
	
	memcpy( ifh.ID, "VirtuaNES DI", sizeof(ifh.ID) );
	ifh.BlockVersion = 0x0210;
	ifh.ProgID	= ROM_GetGameID(pme->rom);
	ifh.MakerID = (WORD)ROM_GetMakerID(pme->rom);
	ifh.DiskNo	= (WORD)ROM_GetDiskNo(pme->rom);
	
	// 相違数をカウント
	for( i = 16; i < DiskSize; i++ ) {
		if( lpWrite[i] )
			ifh.DifferentSize++;
	}
	
	if( !ifh.DifferentSize )
		goto out;

	if( gio_write( gio, &ifh, sizeof(DISKFILEHDR) ) != sizeof(DISKFILEHDR) ) {
		DebugOut("NES_SaveDISK error: write DISKFILEHDR");
		goto out;
	}
	
	for( i = 16; i < DiskSize; i++ ) {
		if( lpWrite[i] ) {
			data = i & 0x00FFFFFF;
			data |= ((DWORD)lpDisk[i]&0xFF)<<24;
	
			// Write File
			if( gio_write( gio, &data, sizeof(DWORD) ) != sizeof(DWORD) ) {
				DebugOut("NES_SaveDISK error: write");
				goto out;
			}
		}
	}
out:
	g_object_unref(gio);
}

INT	NES_IsStateFile( const char* fname, ROM* rom )
{
t_HIO gio;
FILEHDR2 header;

	gio = gio_new_file(fname, _O_RDONLY);
	if(!g_object_valid(gio))
		return	-1;

	if( gio_read( gio, &header, sizeof(header) ) != sizeof(header) ) {
		g_object_unref(gio);
		return	-1;
	}
	g_object_unref(gio);

	if( memcmp( header.ID, "VirtuaNES ST", sizeof(header.ID) ) == 0 ) {
		if( header.BlockVersion < 0x0100 )
			return	0;

		if( 1/*alvin: Config.emulator.bCrcCheck*/ ) {
			if( header.BlockVersion >= 0x200 ) {
				if( ROM_GetMapperNo(rom) != 20 ) {
					// FDS以外
					if( header.Ext0 != ROM_GetPROM_CRC(rom) ) {
						return	-2;	// 違うじゃん
					}
				} else {
					// FDS
					if( header.Ext0 != ROM_GetGameID(rom) ||
					    header.Ext1 != (WORD)ROM_GetMakerID(rom) ||
					    header.Ext2 != (WORD)ROM_GetDiskNo(rom) )
						return	-2;	// 違うじゃん
				}
			}
		}
		return	0;
	}
	return	-1;
}

BOOL	NES_LoadState( NES *pme, const char* fname )
{
t_HIO gio = NULL;
BOOL	bRet = FALSE;

	if( ROM_IsNSF(pme->rom) )
		return	TRUE;

	gio_new_file(fname, _O_RDONLY);
	if(!g_object_valid(gio)){
		DebugOut( "State load error.\n" );
		goto out;
	}
	bRet = NES_ReadState( pme, gio );
out:
	g_object_unref(gio);
	return	bRet;
}

BOOL	NES_SaveState( NES *pme, const char* fname )
{
	t_HIO gio = NULL;
	BOOL	bRet = FALSE;

	if( ROM_IsNSF(pme->rom) )
		return	TRUE;

	gio_new_file(fname, _O_WRONLY);
	if(!g_object_valid(gio)){
		DebugOut( "State load error.\n" );
		goto out;
	}
	bRet = NES_WriteState( pme, gio );
	
out:
	g_object_unref(gio);
	return	bRet;
}

INT NES_GetSAVERAM_SIZE(NES *pme)		{ return pme->SAVERAM_SIZE; }
void	NES_SetSAVERAM_SIZE( NES *pme, INT size )	{ pme->SAVERAM_SIZE = size; }

// VS-Unisystem
BYTE	NES_GetVSDipSwitch(NES *pme)		{ return pme->m_VSDipValue; }
void	NES_SetVSDipSwitch( NES *pme, BYTE v )	{ pme->m_VSDipValue = v; }
VSDIPSWITCH*	NES_GetVSDipSwitchTable(NES *pme)	{ return pme->m_VSDipTable; }

BOOL	NES_ReadState( NES * pme, t_HIO gio )
{
	INT	i;
	BOOL	bHeader = FALSE;
	WORD	Version = 0;

	BLOCKHDR hdr;
	INT	type;

	while( TRUE ) {
		// Read File
		if( gio_read( gio, &hdr, sizeof(BLOCKHDR) ) != sizeof(BLOCKHDR) )
			break;

		// File Header check
		if( !bHeader ) {
			LPFILEHDR	fh = (LPFILEHDR)&hdr;
			if( memcmp( fh->ID, "VirtuaNES ST", sizeof(fh->ID) ) == 0 ) {
				Version = fh->BlockVersion;
				if( Version == 0x0100 ) {
				// Ver0.24まで
					bHeader = TRUE;
					// 古い奴のFDSはロード出来ません
					if( ROM_GetMapperNo(pme->rom) == 20 ) {
						DebugOut("NES_ReadState: unsupported format");
						return FALSE;
					}
				} else 
				if( Version == 0x0200 || Version == 0x0210 ) {
				// Ver0.30以降 Ver0.60以降
					FILEHDR2 hdr2;
					int s = sizeof(BLOCKHDR);
					// ヘッダ部読み直し
					if( gio_seek( gio, -s, SEEK_CUR ) < 0) {
						DebugOut("NES_ReadState: error seek");
						return FALSE;
					}
					// Read File
					if( gio_read( gio, &hdr2, sizeof(FILEHDR2) ) != sizeof(FILEHDR2) ) {
						DebugOut("NES_ReadState: error read");
						return FALSE;
					}

#if	0
					if( Config.emulator.bCrcCheck ) {
						// 現在ロード中のタイトルと違うかをチェック
						if( ROM_GetMapperNo(pme->rom) != 20 ) {
						// FDS以外
							if( hdr2.Ext0 != ROM_GetPROM_CRC(pme->rom) ) {
								return	FALSE;	// 違うじゃん
							}
						} else {
						// FDS
							if( hdr2.Ext0 != ROM_GetGameID(pme->rom) ||
							    hdr2.Ext1 != (WORD)ROM_GetMakerID(pme->rom) ||
							    hdr2.Ext2 != (WORD)ROM_GetDiskNo(pme->rom) )
								return	FALSE;	// 違うじゃん
						}
					}
#endif
				}
				bHeader = TRUE;
				continue;
			}
		}

		if( !bHeader ) {
			DebugOut("NES_ReadState: unsupported format");
			return FALSE;
		}

//DebugOut( "HEADER ID=%8s\n", hdr.ID );

		type = -1;
		if( memcmp( hdr.ID, "REG DATA", sizeof(hdr.ID) ) == 0 ) 
			type = 0;
		if( memcmp( hdr.ID, "RAM DATA", sizeof(hdr.ID) ) == 0 )
			type = 1;
		if( memcmp( hdr.ID, "MMU DATA", sizeof(hdr.ID) ) == 0 )
			type = 2;
		if( memcmp( hdr.ID, "MMC DATA", sizeof(hdr.ID) ) == 0 )
			type = 3;
		if( memcmp( hdr.ID, "CTR DATA", sizeof(hdr.ID) ) == 0 )
			type = 4;
		if( memcmp( hdr.ID, "SND DATA", sizeof(hdr.ID) ) == 0 )
			type = 5;

		if( ROM_GetMapperNo(pme->rom) == 20 ) {
			if( memcmp( hdr.ID, "DISKDATA", sizeof(hdr.ID) ) == 0 )
				type = 6;
		}

		if( memcmp( hdr.ID, "EXCTRDAT", sizeof(hdr.ID) ) == 0 )
			type = 7;

		if( type == -1 ) {
//DebugOut( "UNKNOWN HEADER ID=%8s\n", hdr.ID );
			break;
		}

		switch( type ) {
			case	0:
				// REGISTER STATE
				{
				if( hdr.BlockVersion < 0x0200 ) {
					REGSTAT_O	reg;
					R6502	R;
					if( gio_read( gio, &reg, sizeof(REGSTAT_O) ) != sizeof(REGSTAT_O) ) {
						DebugOut("NES_ReadState: error read");
						return FALSE;
					}

					// LOAD CPU STATE
					R.PC = reg.cpureg.cpu.PC;
					R.A  = reg.cpureg.cpu.A;
					R.X  = reg.cpureg.cpu.X;
					R.Y  = reg.cpureg.cpu.Y;
					R.S  = reg.cpureg.cpu.S;
					R.P  = reg.cpureg.cpu.P;
					R.INT_pending = reg.cpureg.cpu.I;
					CPU_SetContext( pme->cpu, &R );
//					FrameIRQ = reg.cpureg.pme->cpu.FrameIRQ;

					if( hdr.BlockVersion < 0x0110 ) {
						pme->emul_cycles = 0;
						pme->base_cycles = reg.cpureg.cpu.mod_cycles;
					} else if( hdr.BlockVersion == 0x0110 ) {
//						FrameIRQ_cycles = reg.cpureg.pme->cpu.mod_cycles;
						pme->emul_cycles = reg.cpureg.cpu.emul_cycles;
						pme->base_cycles = reg.cpureg.cpu.base_cycles;
					}

					// LOAD PPU STATE
					PPUREG[0] = reg.ppureg.ppu.reg0;
					PPUREG[1] = reg.ppureg.ppu.reg1;
					PPUREG[2] = reg.ppureg.ppu.reg2;
					PPUREG[3] = reg.ppureg.ppu.reg3;
					PPU7_Temp = reg.ppureg.ppu.reg7;
					loopy_t = reg.ppureg.ppu.loopy_t;
					loopy_v = reg.ppureg.ppu.loopy_v;
					loopy_x = reg.ppureg.ppu.loopy_x;
					PPU56Toggle = reg.ppureg.ppu.toggle56;
				} else {
					REGSTAT	reg;
					R6502	R;
					if( gio_read( gio, &reg, sizeof(REGSTAT) ) != sizeof(REGSTAT) ) {
						DebugOut("NES_ReadState: error read");
						return FALSE;
					}

					// LOAD CPU STATE
					R.PC = reg.cpureg.cpu.PC;
					R.A  = reg.cpureg.cpu.A;
					R.X  = reg.cpureg.cpu.X;
					R.Y  = reg.cpureg.cpu.Y;
					R.S  = reg.cpureg.cpu.S;
					R.P  = reg.cpureg.cpu.P;
					R.INT_pending = reg.cpureg.cpu.I;
					CPU_SetContext( pme->cpu, &R );

					if( hdr.BlockVersion == 0x0200 ) {
//						FrameIRQ = reg.cpureg.cpu.FrameIRQ;
//						bFrameIRQ_occur = (reg.cpureg.cpu.FrameIRQ_occur!=0)?TRUE:FALSE;
//						FrameIRQ_cycles = reg.cpureg.cpu.FrameIRQ_cycles;
					} else {
						APU_SetFrameIRQ( pme->apu, reg.cpureg.cpu.FrameIRQ_cycles,
								  reg.cpureg.cpu.FrameIRQ_count,
								  reg.cpureg.cpu.FrameIRQ_type,
								  reg.cpureg.cpu.FrameIRQ,
								  reg.cpureg.cpu.FrameIRQ_occur );
					}

					pme->emul_cycles = reg.cpureg.cpu.emul_cycles;
					pme->base_cycles = reg.cpureg.cpu.base_cycles;

					CPU_SetDmaCycles( pme->cpu, (INT)reg.cpureg.cpu.DMA_cycles );

					// LOAD PPU STATE
					PPUREG[0] = reg.ppureg.ppu.reg0;
					PPUREG[1] = reg.ppureg.ppu.reg1;
					PPUREG[2] = reg.ppureg.ppu.reg2;
					PPUREG[3] = reg.ppureg.ppu.reg3;
					PPU7_Temp = reg.ppureg.ppu.reg7;
					loopy_t = reg.ppureg.ppu.loopy_t;
					loopy_v = reg.ppureg.ppu.loopy_v;
					loopy_x = reg.ppureg.ppu.loopy_x;
					PPU56Toggle = reg.ppureg.ppu.toggle56;
				}

				// APU STATE
				// キューを消す
				APU_QueueClear(pme->apu);

// APUステートを保存するようにしたのでヤメ
//				// DMCは止めないとまずい事が起こる
//				for( i = 0x4010; i <= 0x4013; i++ ) {
//					pme->apu->Write( i, 0 );
//				}
				}
				break;
			case	1:
				// RAM STATE
				{
				#if 0 // alvin
				RAMSTAT	ram;
				if( gio_read( gio, &ram, sizeof(RAMSTAT) ) != sizeof(RAMSTAT) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}
				memcpy( RAM, ram.RAM, sizeof(ram.RAM) );
				memcpy( BGPAL, ram.BGPAL, sizeof(ram.BGPAL) );
				memcpy( SPPAL, ram.SPPAL, sizeof(ram.SPPAL) );
				memcpy( SPRAM, ram.SPRAM, sizeof(ram.SPRAM) );
				#else
				if( gio_read( gio, RAM, 2048 ) != 2048 ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}
				if( gio_read( gio, BGPAL, sizeof(BGPAL) ) != sizeof(BGPAL) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}
				if( gio_read( gio, SPPAL, sizeof(SPPAL) ) != sizeof(SPPAL) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}
				if( gio_read( gio, SPRAM, sizeof(SPRAM) ) != sizeof(SPRAM) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}
				#endif
				if( ROM_IsSAVERAM(pme->rom) ) {
					if( gio_read( gio, WRAM, pme->SAVERAM_SIZE ) != pme->SAVERAM_SIZE ) {
						DebugOut("NES_ReadState: error read");
						return FALSE;
					}
				}
				}
				break;
			case	2:
				// BANK STATE
				{
				MMUSTAT mmu;
				if( gio_read( gio, &mmu, sizeof(MMUSTAT) ) != sizeof(MMUSTAT) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}
				if( hdr.BlockVersion == 0x100 ) {
				// ちょっと前のバージョン
					if( mmu.CPU_MEM_TYPE[3] == BANKTYPE_RAM
					 || mmu.CPU_MEM_TYPE[3] == BANKTYPE_DRAM ) {
						if( gio_read( gio, CPU_MEM_BANK[3], 8*1024 ) != 8*1024 ) {
							DebugOut("NES_ReadState: error read");
							return FALSE;
						}
					} else if( !ROM_IsSAVERAM(pme->rom) ) {
						SetPROM_8K_Bank( 3, mmu.CPU_MEM_PAGE[3] );
					}
					// バンク0～3以外ロード
					for( i = 4; i < 8; i++ ) {
						CPU_MEM_TYPE[i] = mmu.CPU_MEM_TYPE[i];
						CPU_MEM_PAGE[i] = mmu.CPU_MEM_PAGE[i];
						if( CPU_MEM_TYPE[i] == BANKTYPE_ROM ) {
							SetPROM_8K_Bank( i, CPU_MEM_PAGE[i] );
						} else {
							if( gio_read( gio, CPU_MEM_BANK[i], 8*1024 ) != 8*1024 ) {
								DebugOut("NES_ReadState: error read");
								return FALSE;
							}
						}
					}
				} else if( hdr.BlockVersion == 0x200 ) {
				// 最新バージョン
					// SRAMがあっても全部ロードしなおし
					for( i = 3; i < 8; i++ ) {
						CPU_MEM_TYPE[i] = mmu.CPU_MEM_TYPE[i];
						CPU_MEM_PAGE[i] = mmu.CPU_MEM_PAGE[i];
						if( CPU_MEM_TYPE[i] == BANKTYPE_ROM ) {
							SetPROM_8K_Bank( i, CPU_MEM_PAGE[i] );
						} else {
							if( gio_read( gio, CPU_MEM_BANK[i], 8*1024 ) != 8*1024 ) {
								DebugOut("NES_ReadState: error read");
								return FALSE;
							}
						}
					}
				}
				// VRAM
				if( gio_read( gio, VRAM, sizeof(VRAM) ) != sizeof(VRAM) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}

				// CRAM
				for( i = 0; i < 8; i++ ) {
					if( mmu.CRAM_USED[i] != 0 ) {
						if( gio_read( gio,  &CRAM[0x1000*i], 4*1024) != 4*1024 ) {
							DebugOut("NES_ReadState: error read");
							return FALSE;
						}
					}
				}
				// BANK
				for( i = 0; i < 12; i++ ) {
					if( mmu.PPU_MEM_TYPE[i] == BANKTYPE_VROM ) {
						SetVROM_1K_Bank( i, mmu.PPU_MEM_PAGE[i] );
					} else if( mmu.PPU_MEM_TYPE[i] == BANKTYPE_CRAM ) {
						SetCRAM_1K_Bank( i, mmu.PPU_MEM_PAGE[i] );
					} else if( mmu.PPU_MEM_TYPE[i] == BANKTYPE_VRAM ) {
						SetVRAM_1K_Bank( i, mmu.PPU_MEM_PAGE[i] );
					} else {
						DebugOut("NES_ReadState: Unknown bank types");
						return FALSE;
					}
				}
				}
				break;
			case	3:
				// MMC STATE
				{
				MMCSTAT	mmc;
				if( gio_read( gio, &mmc, sizeof(MMCSTAT)) != sizeof(MMCSTAT) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}
				Mapper_LoadState( pme->mapper, mmc.mmcdata );
				}
				break;
			case	4:
				// CTR STATE
				{
				CTRSTAT	ctr;
				if( gio_read( gio, &ctr, sizeof(CTRSTAT)) != sizeof(CTRSTAT) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}

				pme->pad->pad1bit = ctr.ctrreg.ctr.pad1bit;
				pme->pad->pad2bit = ctr.ctrreg.ctr.pad2bit;
				pme->pad->pad3bit = ctr.ctrreg.ctr.pad3bit;
				pme->pad->pad4bit = ctr.ctrreg.ctr.pad4bit;
				PAD_SetStrobe( pme->pad, (ctr.ctrreg.ctr.strobe!=0)?TRUE:FALSE );
				}
				break;

			case	5:
				// SND STATE
				{
				SNDSTAT	snd;
				if( gio_read( gio, &snd, sizeof(SNDSTAT)) != sizeof(SNDSTAT) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}
				APU_LoadState( pme->apu, snd.snddata );
				}
				break;

			// Disk Images
			// Ver0.30以降
			case	6:
				{
				DISKDATA ddata;
				DWORD	pos;
				BYTE	data;
				LONG	DiskSize = 16+65500*ROM_GetDiskNo(pme->rom);
				LPBYTE	lpDisk  = ROM_GetPROM(pme->rom);
				LPBYTE	lpWrite = ROM_GetDISK(pme->rom);

				// 書き換えFLAG消去
				ZEROMEMORY( lpWrite, 16+65500*ROM_GetDiskNo(pme->rom) );

				if( gio_read( gio, &ddata, sizeof(DISKDATA) ) != sizeof(DISKDATA) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}

				for( i = 0; i < ddata.DifferentSize; i++ ) {
					if( gio_read( gio, &pos, sizeof(DWORD)) != sizeof(DWORD) ) {
						DebugOut("NES_ReadState: error read");
						return FALSE;
					}
					data = (BYTE)(pos>>24);
					pos &= 0x00FFFFFF;
					if( pos >= 16 && pos < DiskSize ) {
						lpDisk[pos] = data;
						lpWrite[pos] = 0xFF;
					}
				}

				}
				break;

			// EXCTR STATE
			case  7:
				{
				EXCTRSTAT exctr;
				if( gio_read( gio, &exctr, sizeof(exctr)) != sizeof(exctr) ) {
					DebugOut("NES_ReadState: error read");
					return FALSE;
				}

				}
				break;
		}
	}

	return	TRUE;
}

BOOL	NES_WriteState( NES *pme, t_HIO gio )
{
	INT	i;
	BLOCKHDR hdr;

	// HEADER
	{
		FILEHDR2 fhdr;

		ZEROMEMORY( &fhdr, sizeof(FILEHDR2) );
		memcpy( fhdr.ID, "VirtuaNES ST", sizeof(fhdr.ID) );
		fhdr.BlockVersion = 0x0200;

		if( ROM_GetMapperNo(pme->rom) != 20 ) {
			fhdr.Ext0 = ROM_GetPROM_CRC(pme->rom);
		} else {
			fhdr.Ext0 = ROM_GetGameID(pme->rom);
			fhdr.Ext1 = (WORD)ROM_GetMakerID(pme->rom);
			fhdr.Ext2 = (WORD)ROM_GetDiskNo(pme->rom);
		}

		// Write File
		if( gio_write( gio, &fhdr, sizeof(FILEHDR2)) != sizeof(FILEHDR2) ) {
			DebugOut("NES_WriteState: error write");
			return FALSE;
		}
	}

	// REGISTER STATE
	{
	REGSTAT	reg;
	R6502	R;
	INT	cycles;

	ZEROMEMORY( &hdr, sizeof(BLOCKHDR) );
	ZEROMEMORY( &reg, sizeof(REGSTAT) );

	// Create Header
	memcpy( hdr.ID, "REG DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0210;
	hdr.BlockSize    = sizeof(REGSTAT);

	// SAVE CPU STATE
	CPU_GetContext( pme->cpu, &R );

	reg.cpureg.cpu.PC = R.PC;
	reg.cpureg.cpu.A  = R.A;
	reg.cpureg.cpu.X  = R.X;
	reg.cpureg.cpu.Y  = R.Y;
	reg.cpureg.cpu.S  = R.S;
	reg.cpureg.cpu.P  = R.P;
	reg.cpureg.cpu.I  = R.INT_pending;

	APU_GetFrameIRQ( pme->apu, &cycles,
			  &reg.cpureg.cpu.FrameIRQ_count,
			  &reg.cpureg.cpu.FrameIRQ_type,
			  &reg.cpureg.cpu.FrameIRQ,
			  &reg.cpureg.cpu.FrameIRQ_occur );
	reg.cpureg.cpu.FrameIRQ_cycles = (LONG)cycles;	// 参照がINTな為（ぉ

	reg.cpureg.cpu.DMA_cycles = (LONG)CPU_GetDmaCycles(pme->cpu);
	reg.cpureg.cpu.emul_cycles = pme->emul_cycles;
	reg.cpureg.cpu.base_cycles = pme->base_cycles;

	// SAVE PPU STATE
	reg.ppureg.ppu.reg0 = PPUREG[0];
	reg.ppureg.ppu.reg1 = PPUREG[1];
	reg.ppureg.ppu.reg2 = PPUREG[2];
	reg.ppureg.ppu.reg3 = PPUREG[3];
	reg.ppureg.ppu.reg7 = PPU7_Temp;
	reg.ppureg.ppu.loopy_t  = loopy_t;
	reg.ppureg.ppu.loopy_v  = loopy_v;
	reg.ppureg.ppu.loopy_x  = loopy_x;
	reg.ppureg.ppu.toggle56 = PPU56Toggle;

	// Write File
	if( gio_write( gio, &hdr, sizeof(BLOCKHDR)) != sizeof(BLOCKHDR) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}
	if( gio_write( gio, &reg, sizeof(REGSTAT)) != sizeof(REGSTAT) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}
	}

	// RAM STATE
	{
	RAMSTAT	ram;
	DWORD	size = 0;

	ZEROMEMORY( &hdr, sizeof(BLOCKHDR) );
	ZEROMEMORY( &ram, sizeof(RAMSTAT) );

	// SAVE RAM STATE
	memcpy( ram.RAM, RAM, sizeof(ram.RAM) );
	memcpy( ram.BGPAL, BGPAL, sizeof(ram.BGPAL) );
	memcpy( ram.SPPAL, SPPAL, sizeof(ram.SPPAL) );
	memcpy( ram.SPRAM, SPRAM, sizeof(ram.SPRAM) );

	// S-RAM STATE(使用/未使用に関わらず存在すればセーブする)
	if( ROM_IsSAVERAM(pme->rom) ) {
		size = pme->SAVERAM_SIZE;
	}

	// Create Header
	memcpy( hdr.ID, "RAM DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0100;
	hdr.BlockSize    = size+sizeof(RAMSTAT);

	// Write File
	if( gio_write( gio, &hdr, sizeof(BLOCKHDR)) != sizeof(BLOCKHDR) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}
	if( gio_write( gio, &ram, sizeof(RAMSTAT)) != sizeof(RAMSTAT) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}

	if( ROM_IsSAVERAM(pme->rom) ) {
		if( gio_write( gio, WRAM, pme->SAVERAM_SIZE) != pme->SAVERAM_SIZE ) {
			DebugOut("NES_WriteState: error write");
			return FALSE;
		}
	}
	}

	// BANK STATE
	{
	MMUSTAT mmu;
	DWORD	size;

	ZEROMEMORY( &hdr, sizeof(BLOCKHDR) );
	ZEROMEMORY( &mmu, sizeof(MMUSTAT) );

	size = 0;
	// SAVE CPU MEMORY BANK DATA
	// BANK0,1,2はバンクセーブに関係なし
	// VirtuaNES0.30から
	// バンク３はSRAM使用に関わらずセーブ
	for( i = 3; i < 8; i++ ) {
		mmu.CPU_MEM_TYPE[i] = CPU_MEM_TYPE[i];
		mmu.CPU_MEM_PAGE[i] = CPU_MEM_PAGE[i];

		if( CPU_MEM_TYPE[i] == BANKTYPE_RAM
		 || CPU_MEM_TYPE[i] == BANKTYPE_DRAM ) {
			size += 8*1024;	// 8K BANK
		}
	}

	// SAVE VRAM MEMORY DATA
	for( i = 0; i < 12; i++ ) {
		mmu.PPU_MEM_TYPE[i] = PPU_MEM_TYPE[i];
		mmu.PPU_MEM_PAGE[i] = PPU_MEM_PAGE[i];
	}
	size += 4*1024;	// 1K BANK x 4 (VRAM)

	for( i = 0; i < 8; i++ ) {
		mmu.CRAM_USED[i] = CRAM_USED[i];
		if( CRAM_USED[i] != 0 ) {
			size += 4*1024;	// 4K BANK
		}
	}

	// Create Header
	memcpy( hdr.ID, "MMU DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0200;
	hdr.BlockSize    = size+sizeof(MMUSTAT);

	// Write File
	if( gio_write( gio, &hdr, sizeof(BLOCKHDR)) != sizeof(BLOCKHDR) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}
	if( gio_write( gio, &mmu, sizeof(MMUSTAT)) != sizeof(MMUSTAT) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}

	// WRITE CPU RAM MEMORY BANK
	for( i = 3; i < 8; i++ ) {
		if( mmu.CPU_MEM_TYPE[i] != BANKTYPE_ROM ) {
			if( gio_write( gio, CPU_MEM_BANK[i], 8*1024) != 8*1024) {
				DebugOut("NES_WriteState: error write");
				return FALSE;
			}
		}
	}
	// WRITE VRAM MEMORY(常に4K分すべて書き込む)
	if( gio_write( gio, VRAM, 4*1024) != 4*1024) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}

	// WRITE CRAM MEMORY
	for( i = 0; i < 8; i++ ) {
		if( CRAM_USED[i] != 0 ) {
			if( gio_write( gio, &CRAM[0x1000*i], 4*1024) != 4*1024) {
				DebugOut("NES_WriteState: error write");
				return FALSE;
			}
		}
	}
	}

	// MMC STATE
	{
	MMCSTAT	mmc;

	ZEROMEMORY( &hdr, sizeof(BLOCKHDR) );
	ZEROMEMORY( &mmc, sizeof(MMCSTAT) );

	// Create Header
	memcpy( hdr.ID, "MMC DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0100;
	hdr.BlockSize    = sizeof(MMCSTAT);

	if( Mapper_IsStateSave(pme->mapper) ) {
		Mapper_SaveState( pme->mapper, mmc.mmcdata );
		// Write File
		if( gio_write( gio, &hdr, sizeof(BLOCKHDR)) != sizeof(BLOCKHDR) ) {
			DebugOut("NES_WriteState: error write");
			return FALSE;
		}
		if( gio_write( gio, &mmc, sizeof(MMCSTAT)) != sizeof(MMCSTAT) ) {
			DebugOut("NES_WriteState: error write");
			return FALSE;
		}
	}
	}

	// CONTROLLER STATE
	{
	CTRSTAT	ctr;

	ZEROMEMORY( &hdr, sizeof(BLOCKHDR) );
	ZEROMEMORY( &ctr, sizeof(CTRSTAT) );

	// Create Header
	memcpy( hdr.ID, "CTR DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0100;
	hdr.BlockSize    = sizeof(CTRSTAT);

	ctr.ctrreg.ctr.pad1bit = pme->pad->pad1bit;
	ctr.ctrreg.ctr.pad2bit = pme->pad->pad2bit;
	ctr.ctrreg.ctr.pad3bit = pme->pad->pad3bit;
	ctr.ctrreg.ctr.pad4bit = pme->pad->pad4bit;
	ctr.ctrreg.ctr.strobe  = PAD_GetStrobe(pme->pad)?0xFF:0;
//DebugOut( "SV pad1bit=%08X Strobe=%d\n", pme->pad->pad1bit, pme->pad->GetStrobe()?1:0 );

	if( gio_write( gio, &hdr, sizeof(BLOCKHDR)) != sizeof(BLOCKHDR) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}
	if( gio_write( gio, &ctr, sizeof(CTRSTAT)) != sizeof(CTRSTAT) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}

	}

	// SND STATE
	{
	SNDSTAT	snd;

	ZEROMEMORY( &hdr, sizeof(BLOCKHDR) );
	ZEROMEMORY( &snd, sizeof(SNDSTAT) );

	// Create Header
	memcpy( hdr.ID, "SND DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0100;
	hdr.BlockSize    = sizeof(SNDSTAT);

	APU_SaveState( pme->apu, snd.snddata );

	if( gio_write( gio, &hdr, sizeof(BLOCKHDR)) != sizeof(BLOCKHDR) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}
	if( gio_write( gio, &snd, sizeof(SNDSTAT)) != sizeof(SNDSTAT) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}
	}

	// DISKIMAGE STATE
	if( ROM_GetMapperNo(pme->rom) == 20 )
	{
	DISKDATA dsk;
	LPBYTE	lpDisk  = ROM_GetPROM(pme->rom);
	LPBYTE	lpWrite = ROM_GetDISK(pme->rom);
	LONG	DiskSize = 16+65500*ROM_GetDiskNo(pme->rom);
	DWORD	data;

	ZEROMEMORY( &hdr, sizeof(BLOCKHDR) );
	ZEROMEMORY( &dsk, sizeof(DISKDATA) );

	// 相違数をカウント
	for( i = 16; i < DiskSize; i++ ) {
		if( lpWrite[i] )
			dsk.DifferentSize++;
	}

	memcpy( hdr.ID, "DISKDATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0210;
	hdr.BlockSize    = 0;

	// Write File
	if( gio_write( gio, &hdr, sizeof(BLOCKHDR)) != sizeof(BLOCKHDR) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}
	// Write File
	if( gio_write( gio, &dsk, sizeof(DISKDATA)) != sizeof(DISKDATA) ) {
		DebugOut("NES_WriteState: error write");
		return FALSE;
	}

	for( i = 16; i < DiskSize; i++ ) {
		if( lpWrite[i] ) {
			data = i & 0x00FFFFFF;
			data |= ((DWORD)lpDisk[i]&0xFF)<<24;

			// Write File
			if( gio_write( gio, &data, sizeof(DWORD)) != sizeof(DWORD) ) {
				DebugOut("NES_WriteState: error write");
				return FALSE;
			}
		}
	}
	}
	return TRUE;
}

INT	NES_GetDiskNo(NES *pme)
{
	return	ROM_GetDiskNo(pme->rom);
}

void	NES_SoundSetup(NES *pme)
{
	APU_SoundSetup(pme->apu);
}
INT NES_GetScanline(NES *pme)	{ return pme->NES_scanline; }
BOOL	NES_GetZapperHit(NES *pme)	{ return pme->bZapper; }
void	NES_GetZapperPos( NES *pme, LONG *x, LONG* y )	{ *x = pme->ZapperX; *y = pme->ZapperY; }
void	NES_SetZapperPos( NES *pme, LONG x, LONG y )	{ pme->ZapperX = x; pme->ZapperY = y; }


BOOL	NES_Snapshot(NES *pme)
{
#if 0 
FILE*	fp = NULL;

	try {
		SYSTEMTIME	now;
		::GetLocalTime( &now );

		CHAR	name[_MAX_PATH];

		if( !Config.emulator.bPNGsnapshot ) {
			sprintf( name, "%s %04d%02d%02d%02d%02d%02d%01d.bmp", ROM_GetRomName(pme->rom),
				now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds/100 );
		} else {
			sprintf( name, "%s %04d%02d%02d%02d%02d%02d%01d.png", ROM_GetRomName(pme->rom),
				now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds/100 );
		}

		string	pathstr, tempstr;
		if( Config.path.bSnapshotPath ) {
			pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szSnapshotPath );
			::CreateDirectory( pathstr.c_str(), NULL );
		} else {
			pathstr = ROM_GetRomPath(pme->rom);
		}
		tempstr = CPathlib::MakePath( pathstr.c_str(), name );
		DebugOut( "Snapshot: %s\n", tempstr.c_str() );

		if( !Config.emulator.bPNGsnapshot ) {
			INT i;
 			if( !(fp = ::fopen( tempstr.c_str(), "wb" )) ) {
				// xxx ファイルを開けません
				LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
				sprintf( szErrorString, szErrStr, tempstr.c_str() );
				throw	szErrorString;
			}

			LPBYTE	lpScn = PPU_GetScreenPtr(pme->ppu);

			BITMAPFILEHEADER bfh;
			BITMAPINFOHEADER bih;
			t_RGBQUAD		 *rgb;

			ZEROMEMORY( &bfh, sizeof(bfh) );
			ZEROMEMORY( &bih, sizeof(bih) );

			bfh.bfType = 0x4D42;	// 'BM'
			bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizeof(t_RGBQUAD)*256;
			bfh.bfSize = bfh.bfOffBits+SCREEN_WIDTH*SCREEN_HEIGHT;

			bih.biSize          = sizeof(bih);
			bih.biWidth         = SCREEN_WIDTH;
			bih.biHeight        = SCREEN_HEIGHT;
			bih.biPlanes        = 1;
			bih.biBitCount      = 8;
			bih.biCompression   = BI_RGB;
			bih.biSizeImage     = 0;
			bih.biXPelsPerMeter = 0;
			bih.biYPelsPerMeter = 0;
			bih.biClrUsed       = 256;
			bih.biClrImportant  = 0;

			rgb = DirectDraw_GetPaletteData();

			if( ::fwrite( &bfh, sizeof(bfh), 1, fp ) != 1 ) {
				// ファイルの書き込みに失敗しました
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
			if( ::fwrite( &bih, sizeof(bih), 1, fp ) != 1 ) {
				// ファイルの書き込みに失敗しました
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
			if( ::fwrite( rgb, sizeof(t_RGBQUAD)*256, 1, fp ) != 1 ) {
				// ファイルの書き込みに失敗しました
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}

			lpScn += 8;
			for(  i = SCREEN_HEIGHT-1; i >= 0; i-- ) {
				if( ::fwrite( &lpScn[RENDER_WIDTH*i], SCREEN_WIDTH, 1, fp ) != 1 ) {
					// ファイルの書き込みに失敗しました
					throw	CApp::GetErrorString( IDS_ERROR_WRITE );
				}
			}

			FCLOSE( fp );
		}
	}

#endif
	return	TRUE;
}

BOOL	NES_IsDiskThrottle(NES *pme) { return pme->m_bDiskThrottle; }
void		NES_SetRenderMethod( NES *pme, RENDERMETHOD type )	{ pme->RenderMethod = type; }
RENDERMETHOD	NES_GetRenderMethod(NES *pme)			{ return pme->RenderMethod; }

void	NES_DrawBitmap( NES *pme, INT x, INT y, LPBYTE lpBitmap )
{
INT	i, j;
INT	h, v;
LPBYTE	pScn = PPU_GetScreenPtr(pme->ppu)+8+RENDER_WIDTH*y+x;
LPBYTE	pPtr;

	h = (INT)*lpBitmap++;
	v = (INT)*lpBitmap++;

	for( j = 0; j < v; j++ ) {
		pPtr = pScn;
		for( i = 0; i < h; i++ ) {
			if( *lpBitmap != 0xFF ) {
				*pPtr = *lpBitmap;
			}
			lpBitmap++;
			pPtr++;
		}
		pScn += RENDER_WIDTH;
	}
}

void	NES_Barcode( NES *pme, INT cycles )
{
	if( pme->m_bBarcode ) {
		if( (pme->m_BarcodeCycles+=cycles) > 1000 ) {
			pme->m_BarcodeCycles = 0;
			// 停止？
			if( pme->m_BarcodeData[pme->m_BarcodePtr] != 0xFF ) {
				pme->m_BarcodeOut = pme->m_BarcodeData[pme->m_BarcodePtr++];
			} else {
				pme->m_bBarcode   = FALSE;
				pme->m_BarcodeOut = 0;
DebugOut( "Barcode data trasnfer complete!!\n" );

				CPU_SetClockProcess( pme->cpu, FALSE );
			}
		}
	}
}
BOOL	NES_IsBarcodeEnable(NES *pme) { return pme->m_bBarcode; }
BYTE	NES_GetBarcodeStatus(NES *pme) { return pme->m_BarcodeOut; }

void	NES_SetBarcodeData( NES *pme, LPBYTE code, INT len )
{
	BYTE	prefix_parity_type[10][6] = {
		{0,0,0,0,0,0}, {0,0,1,0,1,1}, {0,0,1,1,0,1}, {0,0,1,1,1,0},
		{0,1,0,0,1,1}, {0,1,1,0,0,1}, {0,1,1,1,0,0}, {0,1,0,1,0,1},
		{0,1,0,1,1,0}, {0,1,1,0,1,0}
	};
	BYTE	data_left_odd[10][7] = {
		{0,0,0,1,1,0,1}, {0,0,1,1,0,0,1}, {0,0,1,0,0,1,1}, {0,1,1,1,1,0,1},
		{0,1,0,0,0,1,1}, {0,1,1,0,0,0,1}, {0,1,0,1,1,1,1}, {0,1,1,1,0,1,1},
		{0,1,1,0,1,1,1}, {0,0,0,1,0,1,1}
	};
	BYTE	data_left_even[10][7] = {
		{0,1,0,0,1,1,1}, {0,1,1,0,0,1,1}, {0,0,1,1,0,1,1}, {0,1,0,0,0,0,1},
		{0,0,1,1,1,0,1}, {0,1,1,1,0,0,1}, {0,0,0,0,1,0,1}, {0,0,1,0,0,0,1},
		{0,0,0,1,0,0,1}, {0,0,1,0,1,1,1}
	};
	BYTE	data_right[10][7] = {
		{1,1,1,0,0,1,0}, {1,1,0,0,1,1,0}, {1,1,0,1,1,0,0}, {1,0,0,0,0,1,0},
		{1,0,1,1,1,0,0}, {1,0,0,1,1,1,0}, {1,0,1,0,0,0,0}, {1,0,0,0,1,0,0},
		{1,0,0,1,0,0,0}, {1,1,1,0,1,0,0}
	};

	INT	i, j, count = 0;;

	if( ROM_GetPROM_CRC(pme->rom) == 0x67898319 ) {	// Barcode World (J)
		NES_SetBarcode2Data( pme, code, len );
		return;
	}

	DebugOut( "SetBarcodeData code=%s len=%d\n", code, len );

	// 数値に変換
	for( i = 0; i < len; i++ ) {
		code[i] = code[i]-'0';
	}

	// レフトマージン
	for( i = 0; i < 32; i++ ) {
		pme->m_BarcodeData[count++] = 0x08;
	}
	// レフトガードバー
	pme->m_BarcodeData[count++] = 0x00;
	pme->m_BarcodeData[count++] = 0x08;
	pme->m_BarcodeData[count++] = 0x00;

	if( len == 13 ) {
#if	0
		// チェックディジットの再計算
		INT	sum = 0;
		for( i = 0; i < 12; i++ ) {
			sum += (i&1)?(code[i]*3):code[i];
		}
		code[12] = (10-(sum%10))%10;
// test start
//		INT	cs = (10-(sum%10))%10;
//		if( cs == 0 ) {
//			cs = 9;
//		} else {
//			cs--;
//		}
//		code[12] = cs;
// test end
#endif
		// 左側6キャラクタ
		for( i = 0; i < 6; i++ ) {
			if( prefix_parity_type[code[0]][i] ) {
				// 偶数パリティ
				for( j = 0; j < 7; j++ ) {
					pme->m_BarcodeData[count++] = data_left_even[code[i+1]][j]?0x00:0x08;
				}
			} else {
				// 奇数パリティ
				for( j = 0; j < 7; j++ ) {
					pme->m_BarcodeData[count++] = data_left_odd[code[i+1]][j]?0x00:0x08;
				}
			}
		}

		// センターバー
		pme->m_BarcodeData[count++] = 0x08;
		pme->m_BarcodeData[count++] = 0x00;
		pme->m_BarcodeData[count++] = 0x08;
		pme->m_BarcodeData[count++] = 0x00;
		pme->m_BarcodeData[count++] = 0x08;

		// 右側5キャラクタとチェックディジット
		for( i = 7; i < 13; i++ ) {
			// 偶数パリティ
			for( j = 0; j < 7; j++ ) {
				pme->m_BarcodeData[count++] = data_right[code[i]][j]?0x00:0x08;
			}
		}
	} else
	if( len == 8 ) {
		// チェックディジットの再計算
		INT	sum = 0;
		for( i = 0; i < 7; i++ ) {
			sum += (i&1)?code[i]:(code[i]*3);
		}
		code[7] = (10-(sum%10))%10;

		// 左側4キャラクタ
		for( i = 0; i < 4; i++ ) {
			// 奇数パリティ
			for( j = 0; j < 7; j++ ) {
				pme->m_BarcodeData[count++] = data_left_odd[code[i]][j]?0x00:0x08;
			}
		}

		// センターバー
		pme->m_BarcodeData[count++] = 0x08;
		pme->m_BarcodeData[count++] = 0x00;
		pme->m_BarcodeData[count++] = 0x08;
		pme->m_BarcodeData[count++] = 0x00;
		pme->m_BarcodeData[count++] = 0x08;

		// 右側3キャラクタとチェックディジット
		for( i = 4; i < 8; i++ ) {
			// 偶数パリティ
			for( j = 0; j < 7; j++ ) {
				pme->m_BarcodeData[count++] = data_right[code[i]][j]?0x00:0x08;
			}
		}
	}

	// ライトガードバー
	pme->m_BarcodeData[count++] = 0x00;
	pme->m_BarcodeData[count++] = 0x08;
	pme->m_BarcodeData[count++] = 0x00;
	// ライトマージン
	for( i = 0; i < 32; i++ ) {
		pme->m_BarcodeData[count++] = 0x08;
	}
	// 終了マーク
	pme->m_BarcodeData[count++] = 0xFF;

	// 転送開始
	pme->m_bBarcode      = TRUE;
	pme->m_BarcodeCycles = 0;
	pme->m_BarcodePtr    = 0;
	pme->m_BarcodeOut    = 0x08;

	CPU_SetClockProcess( pme->cpu, TRUE );

DebugOut( "BARCODE DATA MAX:%d\n", count );
}

BYTE	NES_Barcode2( NES *pme )
{
BYTE	ret = 0x00;

	if( !pme->m_bBarcode2 || pme->m_Barcode2seq < 0 )
		return	ret;

	switch( pme->m_Barcode2seq ) {
		case	0:
			pme->m_Barcode2seq++;
			pme->m_Barcode2ptr = 0;
			ret = 0x04;		// d3
			break;

		case	1:
			pme->m_Barcode2seq++;
			pme->m_Barcode2bit = pme->m_Barcode2data[pme->m_Barcode2ptr];
			pme->m_Barcode2cnt = 0;
			ret = 0x04;		// d3
			break;

		case	2:
			ret = (pme->m_Barcode2bit&0x01)?0x00:0x04;	// Bit rev.
			pme->m_Barcode2bit >>= 1;
			if( ++pme->m_Barcode2cnt > 7 ) {
				pme->m_Barcode2seq++;
			}
			break;
		case	3:
			if( ++pme->m_Barcode2ptr > 19 ) {
				pme->m_bBarcode2 = FALSE;
				pme->m_Barcode2seq = -1;
			} else {
				pme->m_Barcode2seq = 1;
			}
			break;
		default:
			break;
	}

	return	ret;
}
BOOL	NES_IsBarcode2Enable(NES *pme) { return pme->m_bBarcode2; }

void	NES_SetBarcode2Data( NES *pme, LPBYTE code, INT len )
{
	DebugOut( "SetBarcodeData2 code=%s len=%d\n", code, len );

	if( len < 13 )
		return;

	pme->m_bBarcode2   = TRUE;
	pme->m_Barcode2seq = 0;
	pme->m_Barcode2ptr = 0;

	strcpy( (char*)pme->m_Barcode2data, (char*)code );

	pme->m_Barcode2data[13] = 'S';
	pme->m_Barcode2data[14] = 'U';
	pme->m_Barcode2data[15] = 'N';
	pme->m_Barcode2data[16] = 'S';
	pme->m_Barcode2data[17] = 'O';
	pme->m_Barcode2data[18] = 'F';
	pme->m_Barcode2data[19] = 'T';
}

//#if	NES_PROFILER
#if	1
extern const BYTE	Font6x8[];

void	NES_DrawFont( NES *pme, INT x, INT y, BYTE chr, BYTE col )
{
	if( pme && chr >= 0x20 && chr < 0x80 ){
		INT i;
		const BYTE *	pFnt;
		LPBYTE	pPtr;
		LPBYTE	pScn = PPU_GetScreenPtr(pme->ppu)+8;
		chr -= 0x20;
		pFnt = &Font6x8[chr*8];
		pPtr = pScn+RENDER_WIDTH*y+x;
		for( i = 0; i < 8; i++ ) {
			if( pFnt[i] & 0x80 ) pPtr[0] = col;
			if( pFnt[i] & 0x40 ) pPtr[1] = col;
			if( pFnt[i] & 0x20 ) pPtr[2] = col;
			if( pFnt[i] & 0x10 ) pPtr[3] = col;
			if( pFnt[i] & 0x08 ) pPtr[4] = col;
			if( pFnt[i] & 0x04 ) pPtr[5] = col;
			pPtr += RENDER_WIDTH;
		}
	}
}

void	NES_DrawString( NES *pme, INT x, INT y, char* str, BYTE col )
{
	while( *str ) {
		NES_DrawFont( pme, x, y, (BYTE)*str, col );
		str++;
		x += 6;
	}
}

#endif

#if	NES_PROFILER
	// TEST
	DWORD	NES_GetFrameTotalCycles(NES *pme) {
		return	pme->m_dwTotalCycle;
	}
	DWORD	NES_GetProfileTotalCycles(NES *pme) {
		return	pme->m_dwProfileTotalCycle;
	}
	DWORD	NES_GetProfileTotalCount(NES *pme) {
		return	pme->m_dwProfileTotalCount;
	}
	DWORD	NES_GetProfileCycles(NES *pme) {
		return	pme->m_dwProfileCycle;
	}
#endif



static const char *g_lpSoundMuteStringTable[] = {
	"Master ",
	"Rectangle 1",
	"Rectangle 2",
	"Triangle ",
	"Noise ",
	"DPCM ",
	"Ex CH1 ",
	"Ex CH2 ",
	"Ex CH3 ",
	"Ex CH4 ",
	"Ex CH5 ",
	"Ex CH6 ",
	"Ex CH7 ",
	"Ex CH8 ",
	NULL,
	NULL,
};

static NES*	g_nes = NULL;

BOOL	NES_Command( NES *pme, NESCOMMAND cmd, t_NESCmdParam* param )
{
	char	szStr[256];
	switch(cmd){
		case	NESCMD_START:
			NES_Destroy(pme);
			DirectDraw_SetMessageString( "Emulation start." );
			pme = NES_New((const char *)param->fname);
			param->result = EBADPARM;
			if(pme){
				DirectSound_StreamPlay();
				pme->frame_time = 0.0f;
				pme->start_time = tick_current();
				pme->bPause = FALSE;
				PPU_SetScreenPtr( pme->ppu, DirectDraw_GetRenderScreen(), DirectDraw_GetLineColormode() );

				app_set_data(pme);
				param->result = 0;
			}
			g_nes = pme;
			break;
		case	NESCMD_STOP:
			DirectSound_StreamStop();
			NES_Destroy(pme);
			app_set_data(NULL);
			g_nes = NULL;
			break;
		case	NESCMD_PAUSE:
			if(pme){
				DirectSound_StreamPause();
				pme->bPause = TRUE;
			}
			break;
		case	NESCMD_RESUME:
			if(pme){
				if( !pme->bEmuPause )
					DirectSound_StreamResume();
				pme->frame_time = 0.0f;
				pme->start_time = tick_current();
				pme->bPause = FALSE;
			}
			break;
		
		case	NESCMD_EMUPAUSE:
			if(pme){
				pme->bEmuPause = !pme->bEmuPause;
				if( pme->bEmuPause ) {
					DirectSound_StreamPause();
				} else if( !pme->bPause ) {
					DirectSound_StreamResume();
				}
				DirectDraw_SetMessageString( "Pause." );
			}
			break;
		
		case	NESCMD_ONEFRAME:
			if(pme){
				pme->bEmuPause = TRUE;
				DirectSound_StreamPause();
				pme->bOneStep = TRUE;
				DirectDraw_SetMessageString( "One Frame." );
			}
			break;
		
		case	NESCMD_THROTTLE:
			if(pme){
				pme->bThrottle = !pme->bThrottle;
				if( pme->bThrottle ) {
					DirectDraw_SetMessageString( "Throttle ON." );
				} else {
					DirectDraw_SetMessageString( "Throttle OFF." );
				}
			}
			break;
		
		case	NESCMD_FRAMESKIP_AUTO:
			if(pme){
				DirectDraw_SetMessageString( "FrameSkip Auto." );
				pme->nFrameSkip = 0;
			}
			break;
		case	NESCMD_FRAMESKIP_UP:
			if(pme){
				if( pme->nFrameSkip < 20 )
					pme->nFrameSkip++;
				sprintf( szStr, "FrameSkip %d", pme->nFrameSkip );
				DirectDraw_SetMessageString( szStr );
			}
			break;
		case	NESCMD_FRAMESKIP_DOWN:
			if(pme){
				if( pme->nFrameSkip )
					pme->nFrameSkip--;
				if( pme->nFrameSkip ) {
					sprintf( szStr, "FrameSkip %d", pme->nFrameSkip );
					DirectDraw_SetMessageString( szStr );
				} else {
					DirectDraw_SetMessageString( "FrameSkip Auto." );
				}
			}
			break;

		case	NESCMD_NONE:
			break;
		case	NESCMD_HWRESET:
			if(pme){
				NES_Reset(pme);
				DirectDraw_SetMessageString( "Hardware reset." );
			}
			break;
		case	NESCMD_SWRESET:
			if(pme){
				NES_SoftReset(pme);
				DirectDraw_SetMessageString( "Software reset." );
			}
			break;
	
		case	NESCMD_STATE_LOAD:
			if(pme){
				if( NES_LoadState( pme, (const char*)param->fname) ) {
					sprintf( szStr, "State Loaded<%s>." , (char*)param->fname);
					DirectDraw_SetMessageString( szStr );
				}
			}
			break;
		case	NESCMD_STATE_SAVE:
			if(pme){
				if( NES_SaveState( pme, (const char*)param->fname ) ) {
					sprintf( szStr, "State Saved<%s>.", param->fname );
					DirectDraw_SetMessageString( szStr );
				}
			}
			break;
	
		case	NESCMD_SOUND_MUTE:
			if(pme){
				if( APU_SetChannelMute( pme->apu, (BOOL)param->result )) {
					sprintf( szStr, "%s Enable.", g_lpSoundMuteStringTable[param->result] );
				} else {
					sprintf( szStr, "%s Mute.", g_lpSoundMuteStringTable[param->result] );
				}
				DirectDraw_SetMessageString( szStr );
			}
			break;
	
		case	NESCMD_SNAPSHOT:
			if(pme){
				if( NES_Snapshot(pme) ) {
					DirectDraw_SetMessageString( "Snap shot." );
				}
			}
			break;
	
		case	NESCMD_MESSAGE_OUT:
			DirectDraw_SetMessageString( (char *)param->fname );
			break;

		case	NESCMD_DISK_THROTTLE_ON:
			if(pme){
				if( Config.emulator.bDiskThrottle ) {
					pme->m_bDiskThrottle = TRUE;
				}
			}
			break;
		case	NESCMD_DISK_THROTTLE_OFF:
			if(pme){
				pme->m_bDiskThrottle = FALSE;
			}
			break;

		case	NESCMD_DISK_EJECT:
			if(pme){
				if( ROM_GetDiskNo(pme->rom) > 0 ) {
					Mapper_ExCmdWrite( pme->mapper, EXCMDWR_DISKEJECT, 0 );
					DirectDraw_SetMessageString( "Disk Eject." );
				}
			}
			break;
		case	NESCMD_DISK_0A:
			if(pme){
				if( ROM_GetDiskNo(pme->rom) > 0 ) {
					Mapper_ExCmdWrite( pme->mapper, EXCMDWR_DISKINSERT, 0 );
					DirectDraw_SetMessageString( "Change Disk1 SideA." );
				}
			}
			break;
		case	NESCMD_DISK_0B:
			if(pme){
				if( ROM_GetDiskNo(pme->rom) > 1 ) {
					Mapper_ExCmdWrite( pme->mapper, EXCMDWR_DISKINSERT, 1 );
					DirectDraw_SetMessageString( "Change Disk1 SideB." );
				}
			}
			break;
		case	NESCMD_DISK_1A:
			if(pme){
				if( ROM_GetDiskNo(pme->rom) > 2 ) {
					Mapper_ExCmdWrite( pme->mapper, EXCMDWR_DISKINSERT, 2 );
					DirectDraw_SetMessageString( "Change Disk2 SideA." );
				}
			}
			break;
		case	NESCMD_DISK_1B:
			if(pme){
				if( ROM_GetDiskNo(pme->rom) > 3 ) {
					Mapper_ExCmdWrite( pme->mapper, EXCMDWR_DISKINSERT, 3 );
					DirectDraw_SetMessageString( "Change Disk2 SideB." );
				}
			}
			break;
		case	NESCMD_DISK_2A:
			if(pme){
				if( ROM_GetDiskNo(pme->rom) > 4 ) {
					Mapper_ExCmdWrite( pme->mapper, EXCMDWR_DISKINSERT, 4 );
					DirectDraw_SetMessageString( "Change Disk3 SideA." );
				}
			}
			break;
		case	NESCMD_DISK_2B:
			if(pme){
				if( ROM_GetDiskNo(pme->rom) > 5 ) {
					Mapper_ExCmdWrite( pme->mapper, EXCMDWR_DISKINSERT, 5 );
					DirectDraw_SetMessageString( "Change Disk3 SideB." );
				}
			}
			break;
		case	NESCMD_DISK_3A:
			if(pme){
				if( ROM_GetDiskNo(pme->rom) > 6 ) {
					Mapper_ExCmdWrite( pme->mapper, EXCMDWR_DISKINSERT, 6 );
					DirectDraw_SetMessageString( "Change Disk4 SideA." );
				}
			}
			break;
		case	NESCMD_DISK_3B:
			if(pme){
				if( ROM_GetDiskNo(pme->rom) > 7 ) {
					Mapper_ExCmdWrite( pme->mapper, EXCMDWR_DISKINSERT, 7 );
					DirectDraw_SetMessageString( "Change Disk4 SideB." );
				}
			}
			break;

		default:
			DebugOut( "ThreadProc:Unknown event.\n" );
			break;
	}

	return	TRUE;
}

BOOL	FrameInput(NES *pme)
{
	DirectInput_Poll();
	PAD_Sync(pme->pad);
	return	TRUE;
}

DWORD	NES_Process(NES *pme)
{
	int i;
	double	frame_period;
	LONG	sleep_time;
	BOOL	bSleep = TRUE;	// Sleep use

	if(pme == NULL || pme->bPause)
		return INFINITE;
	if( Config.graphics.bWindowVSync && Config.graphics.bSyncNoSleep ) {
		bSleep = FALSE;
	} else if( Config.emulator.bAutoFrameSkip ) {
		bSleep = TRUE;
	} else {
		bSleep = FALSE;
	}
	
	frame_period = 1000.0/pme->nescfg->FrameRate;
	
	pme->current_time = tick_current();
	pme->now_time	 = pme->current_time - pme->start_time;
	
	if( Config.emulator.bAutoFrameSkip && bSleep ) {
		if( NES_IsDiskThrottle(pme) ) {
			frame_period = pme->nescfg->FramePeriod/10.0f;
		} else if( !pme->nFrameSkip ) {
		// Auto
			double	fps = 0.0;
			if( (pme->bThrottle) ) {
				fps = (double)Config.emulator.nThrottleFPS;
			} else {
				fps = pme->nescfg->FrameRate;
			}
			if( fps < 0.0 )   fps = 60.0;
			if( fps > 600.0 ) fps = 600.0;
			frame_period = 1000.0/fps;
		} else {
			frame_period = (1000.0/pme->nescfg->FrameRate)/(pme->nFrameSkip+1);
		}
	
		if( !pme->nFrameSkip && !NES_IsDiskThrottle(pme) ) {
			pme->frameskipno = (INT)(((double)pme->now_time-pme->frame_time) / frame_period);
			if( pme->frameskipno < 0 || pme->frameskipno > 20 ) {
				pme->frameskipno = 1;
				pme->frame_time = 0.0;
				pme->start_time = tick_current();
			}
		} else if( NES_IsDiskThrottle(pme) ) {
			pme->frameskipno = 10;
			pme->frame_time = 0.0;
			pme->start_time = tick_current();
		} else {
			if( pme->nFrameSkip < 0 )
				pme->frameskipno = 1;
			else
				pme->frameskipno = pme->nFrameSkip+1;
		}
	} else {
		if( NES_IsDiskThrottle(pme) ) {
			pme->frameskipno = 10;
		}
		if( pme->bThrottle ) {
			pme->frameskipno = (INT)(((double)Config.emulator.nThrottleFPS+30)/60.0);
		} else {
			if( pme->nFrameSkip < 0 ) {
				pme->frameskipno = 1;
			} else {
				pme->frameskipno = pme->nFrameSkip+1;
			}
		}
	}
	
	// Emulation
	if( (!pme->bEmuPause || pme->bOneStep) && pme ) {
		//gu32 tm = amos_get_tick();
		PPU_SetScreenPtr( pme->ppu, DirectDraw_GetRenderScreen(), DirectDraw_GetLineColormode() );
		
		if( !pme->bOneStep ) {
			for( i = 0; i < pme->frameskipno-1; i++ ) {
				// Skip frames
				FrameInput(pme);
				NES_EmulateFrame( pme, FALSE );
				pme->frame_time +=frame_period;
			}
		}
		pme->bOneStep = FALSE;
		FrameInput(pme);
		NES_EmulateFrame( pme, TRUE );
		pme->frame_time += frame_period;
		
		//g_printf("NES_EmulateFrame: %d/%d/%f", amos_get_tick()-tm, (amos_get_tick() - start_time), (frame_time+frame_period));
		// blit
		DirectDraw_Paint();
		//g_printf("DirectDraw_Paint: %d/%d/%f", amos_get_tick()-tm, (amos_get_tick() - start_time), (frame_time+frame_period));
		
	} else {
		for( i = 0; i < pme->frameskipno-1; i++ ) {
			pme->frame_time += frame_period;
		}
		pme->frame_time += frame_period;
	}
	
	sleep_time = (pme->frame_time+frame_period) - (LONG)(tick_current() - pme->start_time);
	if( bSleep && (sleep_time > 1) ) {
		//amos_sleep( sleep_time );
	}else{
	}
	if(sleep_time < 10)
		sleep_time = 10;
	#if 0
	amos_sleep(sleep_time);
	#endif
	
	// FPS表示
	pme->dwFrameTime[pme->nFrameCount] = tick_current();
	if( ++pme->nFrameCount > 32-1 ) {
		pme->nFrameCount = 0;
		if( Config.graphics.bFPSDisp ) {
			if( pme->dwFrameTime[32-1]-pme->dwFrameTime[0] > 0 ) {
				pme->FPS = 1000*10*(32-1)/(pme->dwFrameTime[32-1]-pme->dwFrameTime[0]);
			} else {
				pme->FPS = 0;
			}
		} else {
			pme->FPS = 0;
		}
	}
	if( Config.graphics.bFPSDisp ) {
		char	szStr[30];
		sprintf( szStr, "FPS:%d.%01d", pme->FPS/10, pme->FPS%10 );
		DirectDraw_SetInfoString( szStr );
	} else {
		DirectDraw_SetInfoString( NULL );
	}
	if(sleep_time > 0){
		int xxx = 0;
	}
	return sleep_time;
}

int AudioCallback(void *user_data, void *buf, int size)
{
	if(g_nes == NULL || g_nes->bPause)
		return 0;
	APU_Process( g_nes->apu, (LPBYTE)buf, size );
	return size;
}



