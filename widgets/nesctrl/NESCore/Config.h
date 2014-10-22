//
// ê›íËï€ë∂ÉNÉâÉX
//
#ifndef	__CCONFIG_INCLUDED__
#define	__CCONFIG_INCLUDED__


typedef struct 	
{
	BOOL	bIllegalOp;
	BOOL	bAutoFrameSkip;
	BOOL	bThrottle;
	INT	nThrottleFPS;
	BOOL	bBackground;
	INT	nPriority;
	BOOL	bFourPlayer;
	BOOL	bCrcCheck;
	BOOL	bDiskThrottle;
	BOOL	bLoadFullscreen;
	BOOL	bPNGsnapshot;
	BOOL	bAutoIPS;
}CCfgEmulator;

typedef struct
{
	BOOL	bAspect;
	BOOL	bAllSprite;
	BOOL	bAllLine;
	BOOL	bFPSDisp;
	BOOL	bTVFrame;
	BOOL	bScanline;
	INT	nScanlineColor;
	BOOL	bSyncDraw;
	BOOL	bFitZoom;

	BOOL	bLeftClip;

	BOOL	bWindowVSync;

	BOOL	bSyncNoSleep;

	BOOL	bDiskAccessLamp;

	BOOL	bDoubleSize;
	BOOL	bSystemMemory;
	BOOL	bUseHEL;

	BOOL	bNoSquareList;

	INT	nGraphicsFilter;

	DWORD	dwDisplayWidth;
	DWORD	dwDisplayHeight;
	DWORD	dwDisplayDepth;
	DWORD	dwDisplayRate;

	BOOL	bPaletteFile;
}CCfgGraphics;

typedef struct
{
	BOOL	bEnable;
	INT	nRate;
	INT	nBits;
	INT	nBufferSize;

	INT	nFilterType;

	BOOL	bChangeTone;

	BOOL	bDisableVolumeEffect;
	BOOL	bExtraSoundEnable;

	//  0:Master
	//  1:Rectangle 1
	//  2:Rectangle 2
	//  3:Triangle
	//  4:Noise
	//  5:DPCM
	//  6:VRC6
	//  7:VRC7
	//  8:FDS
	//  9:MMC5
	// 10:N106
	// 11:FME7
	SHORT	nVolume[16];
}CCfgSound;
typedef struct 
{
	WORD	nButton[10];
}CCfgController;
typedef struct
{
	CCfgEmulator	emulator;
	CCfgGraphics	graphics;
	CCfgSound	sound;
	CCfgController	controller;
}	CConfig;

extern	CConfig		Config;

#endif // !__CCONFIG_INCLUDED__
