//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Emulation core                                              //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__NES_INCLUDED__
#define	__NES_INCLUDED__

#include "macro.h"

#include "state.h"
#include "VsUnisystem.h"

#include "mapper.h"
#include "apu.h"
#include "cpu.h"
#include "pad.h"
#include "ppu.h"
#include "rom.h"

#define	NES_PROFILER	0

// コンフィグ
#define	FETCH_CYCLES	8

typedef struct tagNESCONFIG {
	FLOAT	BaseClock;		// NTSC:21477270.0  PAL:21281364.0
	FLOAT	CpuClock;		// NTSC: 1789772.5  PAL: 1773447.0

	INT	TotalScanlines;		// NTSC: 262  PAL: 312

	INT	ScanlineCycles;		// NTSC:1364  PAL:1362

	INT	HDrawCycles;		// NTSC:1024  PAL:1024
	INT	HBlankCycles;		// NTSC: 340  PAL: 338
	INT	ScanlineEndCycles;	// NTSC:   4  PAL:   2

	INT	FrameCycles;		// NTSC:29829.52  PAL:35468.94
	INT	FrameIrqCycles;		// NTSC:29829.52  PAL:35468.94

	INT	FrameRate;		// NTSC:60(59.94) PAL:50
	FLOAT	FramePeriod;		// NTSC:16.683    PAL:20.0
} NESCONFIG, *LPNESCONFIG;

extern	NESCONFIG NESCONFIG_NTSC;
extern	NESCONFIG NESCONFIG_PAL;










// 描画方式
typedef enum  { 
	POST_ALL_RENDER = 0, // スキャンライン分の命令実行後，レンダリング
	PRE_ALL_RENDER	= 1, // レンダリングの実行後，スキャンライン分の命令実行
	POST_RENDER 	= 2, // 表示期間分の命令実行後，レンダリング
	PRE_RENDER		= 3, // レンダリング実行後，表示期間分の命令実行
	TILE_RENDER 	= 4  // タイルベースレンダリング
}RENDERMETHOD;

// コマンド
typedef enum  {
	NESCMD_NONE = 0,
	NESCMD_HWRESET,
	NESCMD_SWRESET,
	NESCMD_EXCONTROLLER,	// Commandparam
	NESCMD_DISK_THROTTLE_ON,
	NESCMD_DISK_THROTTLE_OFF,
	NESCMD_DISK_EJECT,
	NESCMD_DISK_0A,
	NESCMD_DISK_0B,
	NESCMD_DISK_1A,
	NESCMD_DISK_1B,
	NESCMD_DISK_2A,
	NESCMD_DISK_2B,
	NESCMD_DISK_3A,
	NESCMD_DISK_3B,

	NESCMD_SOUND_MUTE,	// CommandParam

	NESCMD_START,
	NESCMD_PAUSE,
	NESCMD_RESUME,
	NESCMD_STOP,
	// 以下はEventで使用する
	NESCMD_MESSAGE_OUT, 	// メッセージだけ出力したい時に使用

	NESCMD_EMUPAUSE,
	NESCMD_ONEFRAME,
	NESCMD_THROTTLE,
	NESCMD_FRAMESKIP_AUTO,
	NESCMD_FRAMESKIP_UP,
	NESCMD_FRAMESKIP_DOWN,

	NESCMD_STATE_LOAD,		// NES_CommandReq
	NESCMD_STATE_SAVE,		// NES_CommandReq

	// For Snapshot
	NESCMD_SNAPSHOT,
}NESCOMMAND;


typedef struct {
	NESCOMMAND		cmd;
	const char	*fname; // NESCMD_START
	int		result;	// return value; also [IN] for NESCMD_SOUND_MUTE
}t_NESCmdParam;





struct NES_struct{
//public:
	CPU*	cpu;
	PPU*	ppu;
	APU*	apu;
	ROM*	rom;
	PAD*	pad;
	Mapper*	mapper;

	NESCONFIG* nescfg;

//protected:
	INT	nIRQtype;
	BOOL	bVideoMode;
	BOOL	bFrameIRQ;

	BOOL	bZapper;
	LONG	ZapperX, ZapperY;

	BOOL	m_bPadStrobe;

	RENDERMETHOD	RenderMethod;

	BOOL	m_bDiskThrottle;

	SQWORD	base_cycles;
	SQWORD	emul_cycles;

	INT	NES_scanline;

	INT	SAVERAM_SIZE;

	// For VS-Unisystem
	BYTE		m_VSDipValue;
	VSDIPSWITCH*	m_VSDipTable;

	// Snapshot number
	INT	m_nSnapNo;

	// For NSF
	BOOL	m_bNsfPlaying;
	BOOL	m_bNsfInit;
	INT	m_nNsfSongNo;
	INT	m_nNsfSongMode;

	// For Barcode
	BOOL	m_bBarcode;
	BYTE	m_BarcodeOut;
	BYTE	m_BarcodePtr;
	INT	m_BarcodeCycles;
	BYTE	m_BarcodeData[256];

	// For Barcode
	BOOL	m_bBarcode2;
	INT	m_Barcode2seq;
	INT	m_Barcode2ptr;
	INT	m_Barcode2cnt;
	BYTE	m_Barcode2bit;
	BYTE	m_Barcode2data[32];

	// gameoption backup
	INT	m_saveRenderMethod;
	INT	m_saveIrqType;
	BOOL	m_saveFrameIRQ;
	BOOL	m_saveVideoMode;

#if	NES_PROFILER
	// TEST
	BYTE	m_ProfileEnable;
	DWORD	m_dwTotalCycle;
	DWORD	m_dwTotalTempCycle;
	DWORD	m_dwProfileTotalCycle;
	DWORD	m_dwProfileTotalCount;
	DWORD	m_dwProfileCycle;
	DWORD	m_dwProfileTempCycle;
	DWORD	m_dwProfileAveCycle;
	DWORD	m_dwProfileMaxCycle;

//	static	BYTE	Font6x8[];

//	void	DrawFont( INT x, INT y, BYTE chr, BYTE col );
//	void	DrawString( INT x, INT y, LPSTR str, BYTE col );
#endif

#if 1	// added by alvin
	BYTE	bPause;		// Thread pause
	BYTE	bEmuPause;	// Emulation pause
	BYTE	bThrottle;	// Emulation throttle
	BYTE	bOneStep;	// Emulation one step

	INT	nFrameSkip;		// Emulation frameskip
	INT	frameskipno;
	double	frame_time;
	DWORD	start_time;
	DWORD	current_time;
	DWORD	now_time;

	// FPS
	INT	nFrameCount;
	DWORD	dwFrameTime[32];
	DWORD	FPS;
#endif	
};



BEGIN_DECLARATION


INT NES_IsStateFile( const char* fname, ROM* rom );


NES *NES_New( const char* fname );
void NES_Destroy(NES *pme);

void	NES_Reset(NES *pme);
void	NES_SoftReset(NES *pme);

void	NES_Clock( NES *pme, INT cycles );

BYTE	NES_Read ( NES *pme, WORD addr );
void	NES_Write( NES *pme, WORD addr, BYTE data );

void	NES_EmulationCPU( NES *pme, INT basecycles );
void	NES_EmulationCPU_BeforeNMI( NES *pme, INT cycles );

void	NES_EmulateFrame( NES *pme, BOOL bDraw );

// For NSF
void	NES_EmulateNSF(NES *pme);
void	NES_SetNsfPlay( NES *pme, INT songno, INT songmode );
void	NES_SetNsfStop(NES *pme);
BOOL	NES_IsNsfPlaying(NES *pme) ;

enum IRQMETHOD { 
	IRQ_HSYNC = 0,
	IRQ_CLOCK = 1
};

	// IRQ type contorol
	void	NES_SetIrqType( NES *pme, INT nType ) ;
	INT	NES_GetIrqType(NES *pme) ;

	// Frame-IRQ control (for Paris-Dakar Rally Special)
	void	NES_SetFrameIRQmode( NES *pme, BOOL bMode ) ;
	BOOL	NES_GetFrameIRQmode(NES *pme) ;

	// NTSC/PAL
	void	NES_SetVideoMode( NES *pme, BOOL bMode );
	BOOL	NES_GetVideoMode(NES *pme) ;

	//
	INT	NES_GetDiskNo(NES *pme);
	void	NES_SoundSetup(NES *pme);

	INT	NES_GetScanline(NES *pme);
	BOOL	NES_GetZapperHit(NES *pme);
	void	NES_GetZapperPos( NES *pme, LONG* x, LONG* y );
	void	NES_SetZapperPos( NES *pme, LONG x, LONG y )	;

	// ステートファイル
	// 0:ERROR 1:CRC OK -1:CRC ERR
	BOOL	NES_LoadState( NES *pme, const char* fname );
	BOOL	NES_SaveState( NES *pme, const char* fname );

	INT	NES_GetSAVERAM_SIZE(NES *pme);
	void	NES_SetSAVERAM_SIZE( NES *pme, INT size );

	// VS-Unisystem
	BYTE	NES_GetVSDipSwitch(NES *pme);
	void	NES_SetVSDipSwitch( NES *pme, BYTE v );
	VSDIPSWITCH*	NES_GetVSDipSwitchTable(NES *pme);

	// スナップショット
	BOOL	NES_Snapshot(NES *pme);

	// その他コントロール
	BOOL	NES_IsDiskThrottle(NES *pme) ;
//	BOOL	IsBraking() { return m_bBrake; }	// Debugger
	void		NES_SetRenderMethod( NES *pme, RENDERMETHOD type );
	RENDERMETHOD	NES_GetRenderMethod(NES *pme);

	BOOL	NES_Command( NES *pme, NESCOMMAND cmd, t_NESCmdParam *param);



	// Barcode battler(Bandai)
	void	NES_SetBarcodeData( NES *pme, LPBYTE code, INT len );
	void	NES_Barcode( NES *pme, INT cycles );
	BOOL	NES_IsBarcodeEnable(NES *pme) ;
	BYTE	NES_GetBarcodeStatus(NES *pme) ;

	// Barcode world(Sunsoft/EPOCH)
	void	NES_SetBarcode2Data( NES *pme, LPBYTE code, INT len );
	BYTE	NES_Barcode2(  NES *pme);
	BOOL	NES_IsBarcode2Enable(NES *pme) ;

#if	NES_PROFILER
	// TEST
	DWORD	NES_GetFrameTotalCycles(NES *pme) ;
	DWORD	NES_GetProfileTotalCycles(NES *pme) ;
	DWORD	NES_GetProfileTotalCount(NES *pme) ;
	DWORD	NES_GetProfileCycles(NES *pme) ;
#endif

DWORD	NES_Process(NES *pme);


END_DECLARATION


#endif	// !__NES_INCLUDED__

