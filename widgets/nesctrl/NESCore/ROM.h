//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES ROM Cartridge class                                         //
//                                                           Norix      //
//                                               written     2001/02/20 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__ROM_INCLUDED__
#define	__ROM_INCLUDED__

#include "macro.h"

#pragma pack( push, 1 )

typedef	struct	tagNESHEADER {
	BYTE	ID[4];
	BYTE	PRG_PAGE_SIZE;
	BYTE	CHR_PAGE_SIZE;
	BYTE	control1;
	BYTE	control2;
	BYTE	reserved[8];
} NESHEADER;

typedef	struct	tagNSFHEADER {
	BYTE	ID[5];
	BYTE	Version;
	BYTE	TotalSong;
	BYTE	StartSong;
	WORD	LoadAddress;
	WORD	InitAddress;
	WORD	PlayAddress;
	BYTE	SongName[32];
	BYTE	ArtistName[32];
	BYTE	CopyrightName[32];
	WORD	SpeedNTSC;
	BYTE	BankSwitch[8];
	WORD	SpeedPAL;
	BYTE	NTSC_PALbits;
	BYTE	ExtraChipSelect;
	BYTE	Expansion[4];		// must be 0
} NSFHEADER;

#pragma pack( pop )


// ROM control byte #1
enum {
	ROM_VMIRROR = 0x01, 
	ROM_SAVERAM = 0x02,
	ROM_TRAINER = 0x04, 
	ROM_4SCREEN = 0x08 
};

// ROM control byte #2
enum {	ROM_VSUNISYSTEM = 0x01 };

typedef struct ROM_struct ROM;

ROM *ROM_New( const char* fname );
void ROM_Destroy(ROM *pme);

// Get NESHEADER
NESHEADER* ROM_GetNesHeader(ROM *pme);
// Get NSFHEADER
NSFHEADER* ROM_GetNsfHeader(ROM *pme) ;

// Get ROM buffer pointer
LPBYTE	ROM_GetPROM(ROM *pme)	;
LPBYTE	ROM_GetVROM(ROM *pme)	;
LPBYTE	ROM_GetTRAINER(ROM *pme)	;
LPBYTE	ROM_GetDISKBIOS(ROM *pme);
LPBYTE	ROM_GetDISK(ROM *pme)	;

// Get ROM size
BYTE	ROM_GetPROM_SIZE(ROM *pme);
BYTE	ROM_GetVROM_SIZE(ROM *pme);

// Get PROM
DWORD	ROM_GetPROM_CRC(ROM *pme);
DWORD	ROM_GetROM_CRC(ROM *pme);
DWORD	ROM_GetVROM_CRC(ROM *pme);

// Get FDS ID
DWORD	ROM_GetMakerID(ROM *pme);
DWORD	ROM_GetGameID(ROM *pme);

// ROM control
BOOL	ROM_IsVMIRROR(ROM *pme) ;
BOOL	ROM_Is4SCREEN(ROM *pme) ;
BOOL	ROM_IsSAVERAM(ROM *pme) ;
BOOL	ROM_IsTRAINER(ROM *pme) ;
BOOL	ROM_IsVSUNISYSTEM(ROM *pme) ;
BOOL	ROM_IsPAL(ROM *pme) 	;

// Mapper
INT ROM_GetMapperNo(ROM *pme) ;

// Disks
INT ROM_GetDiskNo(ROM *pme) ;

// NSF
BOOL	ROM_IsNSF(ROM *pme) ;
BYTE	ROM_GetNSF_SIZE(ROM *pme)	;

// ROM Paths
const char* ROM_GetRomPath(ROM *pme) ;
const char* ROM_GetRomName(ROM *pme) ;
const char* ROM_GetFullPathName(ROM *pme) ;

// File check
enum {
	ROM_OK
	, ROM_ERR_OPEN
	, ROM_ERR_READ
	, ROM_ERR_ILLEGALHEADER
	, ROM_ERR_UNSUPPORTFORMAT
};
INT ROM_IsRomFile( const char* fname );


/*
  A. iNES Format (.NES)
  ---------------------
    +--------+------+------------------------------------------+
    | Offset | Size | Content(s)                               |
    +--------+------+------------------------------------------+
    |   0    |  3   | 'NES'                                    |
    |   3    |  1   | $1A                                      |
    |   4    |  1   | 16K PRG-ROM page count                   |
    |   5    |  1   |  8K CHR-ROM page count                   |
    |   6    |  1   | ROM Control Byte #1                      |
    |        |      |   %####vTsM                              |
    |        |      |    |  ||||+- 0=Horizontal mirroring      |
    |        |      |    |  ||||   1=Vertical mirroring        |
    |        |      |    |  |||+-- 1=SRAM enabled              |
    |        |      |    |  ||+--- 1=512-byte trainer present  |
    |        |      |    |  |+---- 1=Four-screen mirroring     |
    |        |      |    |  |                                  |
    |        |      |    +--+----- Mapper # (lower 4-bits)     |
    |   7    |  1   | ROM Control Byte #2                      |
    |        |      |   %####00PV                              |
    |        |      |    |  |  |+- 1=VS-Unisystem Images       |
    |        |      |    |  |  +-- 1=PlayChoice10 Images       |
    |        |      |    |  |                                  |
    |        |      |    +--+----- Mapper # (upper 4-bits)     |
    |  8-15  |  8   | $00                                      |
    | 16-..  |      | Actual 16K PRG-ROM pages (in linear      |
    |  ...   |      | order). If a trainer exists, it precedes |
    |  ...   |      | the first PRG-ROM page.                  |
    | ..-EOF |      | CHR-ROM pages (in ascending order).      |
    +--------+------+------------------------------------------+
*/

#endif	// !__ROM_INCLUDED__
