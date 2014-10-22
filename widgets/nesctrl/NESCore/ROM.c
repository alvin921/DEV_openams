//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES ROM Cartridge class                                         //
//                                                           Norix      //
//                                               written     2001/02/20 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#include "macro.h"

#include "Crclib.h"

#include "rom.h"
#include "mmu.h"


void	ROM_FilenameCheck( ROM *pme, const char* fname );

struct	ROM_struct{
	NESHEADER	header;
	NSFHEADER	nsfheader;
	LPBYTE		lpPRG;
	LPBYTE		lpCHR;
	LPBYTE		lpTrainer;
	LPBYTE		lpDiskBios;
	LPBYTE		lpDisk;

	// PROM CRC
	DWORD		crc;
	DWORD		crcall;
	DWORD		crcvrom;

	DWORD		fdsmakerID, fdsgameID;

	INT		mapper;
	INT		diskno;

	// For PAL(Database)
	BOOL		bPAL;

	// For NSF
	BOOL		bNSF;
	INT		NSF_PAGE_SIZE;

	char		path[AMOS_PATH_MAX+1];
	char		name[AMOS_FNAME_MAX+1];
	char		fullpath[AMOS_PATH_MAX+1];
};

ROM *ROM_New( const char* fname )
{
	t_HIO gio;
	ROM *pme = (ROM*)MALLOC(sizeof(ROM));
	NESHEADER *header;
	DWORD	PRGoffset, CHRoffset;
	LONG	PRGsize, CHRsize;
	
LPBYTE	temp = NULL;
LONG	FileSize;

	ZEROMEMORY(pme, sizeof(ROM));

	pme->bPAL = FALSE;
	pme->bNSF = FALSE;
	pme->NSF_PAGE_SIZE = 0;

	pme->lpPRG = pme->lpCHR = pme->lpTrainer = pme->lpDiskBios = pme->lpDisk = NULL;

	pme->crc = pme->crcall = 0;
	pme->mapper = 0;
	pme->diskno = 0;
	
	gio = gio_new_file(fname, _O_RDONLY);
	if(!g_object_valid(gio)){
		DebugOut("Error open <%s>\n", fname);
		goto FAILED;
	}
	FileSize = gio_size(gio);
	if( FileSize < 17 ) {
		DebugOut("Error: Small file\n");
		goto FAILED;
	}
	
	if( !(temp = (LPBYTE)MALLOC( FileSize )) ) {
		DebugOut("Error: no memory\n");
		goto FAILED;
	}
	
	if( gio_read(gio, temp, FileSize) != FileSize ) {
		DebugOut("Error: read\n");
		goto FAILED;
	}
	g_object_unref_0(gio);
	header = (NESHEADER *)temp;
	
	if( strncmp((char*)header->ID, "NES", 3) == 0 && header->ID[3] == 0x1A ) {
	} else if( strncmp((char*)header->ID, "FDS", 3) == 0 && header->ID[3] == 0x1A ) {
		DebugOut("Error: unsupported of zlib compressed\n");
		goto FAILED;
	} else if( strncmp((char*)header->ID, "NESM", 4) == 0){
	} else {
		DebugOut("Error: unsupported of zlib compressed\n");
		goto FAILED;
	}
	
	
	memcpy( &pme->header, temp, sizeof(NESHEADER) );
	if( strncmp((char*)header->ID, "NES", 3) == 0 && header->ID[3] == 0x1A ) {
		PRGsize = (LONG)pme->header.PRG_PAGE_SIZE*0x4000;
		CHRsize = (LONG)pme->header.CHR_PAGE_SIZE*0x2000;
		PRGoffset = sizeof(NESHEADER);
		CHRoffset = PRGoffset + PRGsize;
		
		// Trainer
		if( ROM_IsTRAINER(pme) ) {
			if( !(pme->lpTrainer = (LPBYTE)MALLOC( 512 )) ) {
				DebugOut("Error: out of memory\n");
				goto FAILED;
			}
			memcpy( pme->lpTrainer, temp+sizeof(NESHEADER), 512 );
			PRGoffset += 512;
			CHRoffset += 512;
		} else {
			pme->lpTrainer = NULL;
		}
		if( PRGsize <= 0 || (PRGsize+CHRsize) > FileSize ) {
			DebugOut("Error: invalid nes header\n");
			goto FAILED;
		}
		
		// PRG BANK
		if( !(pme->lpPRG = (LPBYTE)MALLOC( PRGsize )) ) {
			DebugOut("Error: out of memory\n");
			goto FAILED;
		}
		
		memcpy( pme->lpPRG, temp+PRGoffset, PRGsize );
		
		// CHR BANK
		if( CHRsize > 0 ) {
			if( !(pme->lpCHR = (LPBYTE)MALLOC( CHRsize )) ) {
				DebugOut("Error: out of memory\n");
				goto FAILED;
			}
			if( FileSize < CHRoffset+CHRsize ) {
				CHRsize = FileSize - CHRoffset;
			}
			memcpy( pme->lpCHR, temp+CHRoffset, CHRsize );
		} else {
			pme->lpCHR = NULL;
		}
	}else if( strncmp((char*)header->ID, "NESM", 4) == 0) {
		// NSF
		pme->bNSF = TRUE;
		ZEROMEMORY( &pme->header, sizeof(NESHEADER) );
		
		// ヘッダコピー
		memcpy( &pme->nsfheader, temp, sizeof(NSFHEADER) );
		
		PRGsize = FileSize-sizeof(NSFHEADER);
		DebugOut( "PRGSIZE:%d\n", PRGsize );
		PRGsize = (PRGsize+0x0FFF)&~0x0FFF;
		DebugOut( "PRGSIZE:%d\n", PRGsize );
		if( !(pme->lpPRG = (LPBYTE)MALLOC( PRGsize )) ) {
			DebugOut("Error: out of memory\n");
			goto FAILED;
		}
		ZEROMEMORY( pme->lpPRG, PRGsize );
		memcpy( pme->lpPRG, temp+sizeof(NSFHEADER), FileSize-sizeof(NSFHEADER) );
		
		pme->NSF_PAGE_SIZE = PRGsize>>12;
		DebugOut( "PAGESIZE:%d\n", pme->NSF_PAGE_SIZE );
	} else {
		DebugOut("Error: unsupported\n");
		goto FAILED;
	}

		// パス/ファイル名取得
		{
			char drive[AMOS_DRIVE_MAX+1];
			char dir[AMOS_PATH_MAX+1];
			char name[AMOS_FNAME_MAX+1];
			char ext[AMOS_EXT_MAX+1];
			fsi_splitpath(fname, drive, dir, name, ext);
			fsi_makepath(pme->path, drive, dir, NULL, NULL);
			fsi_makepath(pme->name, NULL, NULL, name, ext);
			strcpy( pme->fullpath, fname );
		}

		// マッパ設定
		if( !pme->bNSF ) {
			pme->mapper = (pme->header.control1>>4)|(pme->header.control2&0xF0);
			pme->crc = pme->crcall = pme->crcvrom = 0;

			if( pme->mapper != 20 ) {
				// PRG crcの計算(NesToyのPRG CRCと同じ)
				if( ROM_IsTRAINER(pme) ) {
					pme->crcall  = CrcRev( 512+PRGsize+CHRsize, temp+sizeof(NESHEADER) );
					pme->crc     = CrcRev( 512+PRGsize, temp+sizeof(NESHEADER) );
					if( CHRsize )
						pme->crcvrom = CrcRev( CHRsize, temp+PRGsize+512+sizeof(NESHEADER) );
				} else {
					pme->crcall  = CrcRev( PRGsize+CHRsize, temp+sizeof(NESHEADER) );
					pme->crc     = CrcRev( PRGsize, temp+sizeof(NESHEADER) );
					if( CHRsize )
						pme->crcvrom = CrcRev( CHRsize, temp+PRGsize+sizeof(NESHEADER) );
				}

				ROM_FilenameCheck( pme, pme->name );

#include "ROM_Patch.h"
				pme->fdsmakerID = pme->fdsgameID = 0;
			} else {
				pme->crc = pme->crcall = pme->crcvrom = 0;

				pme->fdsmakerID = pme->lpPRG[0x1F];
				pme->fdsgameID  = (pme->lpPRG[0x20]<<24)|(pme->lpPRG[0x21]<<16)|(pme->lpPRG[0x22]<<8)|(pme->lpPRG[0x23]<<0);
			}
		} else {
		// NSF
			pme->mapper = 0x0100;	// Private pme->mapper
			pme->crc = pme->crcall = pme->crcvrom = 0;
			pme->fdsmakerID = pme->fdsgameID = 0;
		}

		FREE( temp );

	return pme;
FAILED:
	
	g_object_unref_0(gio);
	FREEIF( temp );
	
	FREEIF( pme->lpPRG );
	FREEIF( pme->lpCHR );
	FREEIF( pme->lpTrainer );
	FREEIF( pme->lpDiskBios );
	FREEIF( pme->lpDisk );
	return NULL;
}

//
// デストラクタ
//
void ROM_Destroy(ROM *pme)
{
	if(pme){
		FREE( pme->lpPRG );
		FREE( pme->lpCHR );
		FREE( pme->lpTrainer );
		FREE( pme->lpDiskBios );
		FREE( pme->lpDisk );

		FREE(pme);
	}
}

//
// ROMファイルチェック
//
INT	ROM_IsRomFile( const char* fname )
{
	t_HIO gio;
	NESHEADER	header;
	
	gio = gio_new_file(fname, _O_RDONLY);
	if(!g_object_valid(gio)){
		DebugOut("Error open <%s>\n", fname);
		return ROM_ERR_OPEN;
	}

	if( gio_read( gio, &header, sizeof(header)) != sizeof(header) ) {
		g_object_unref(gio);
		return	ROM_ERR_READ;
	}
	g_object_unref(gio);

	if( header.ID[0] == 'N' && header.ID[1] == 'E'
	 && header.ID[2] == 'S' && header.ID[3] == 0x1A ) {
		INT i;
 		for(  i = 0; i < 8; i++ ) {
			if( header.reserved[i] )
				return	ROM_ERR_ILLEGALHEADER;
		}
		return	0;
	} else if( header.ID[0] == 'F' && header.ID[1] == 'D'
		&& header.ID[2] == 'S' && header.ID[3] == 0x1A ) {
		return	ROM_ERR_UNSUPPORTFORMAT;
	} else if( header.ID[0] == 'N' && header.ID[1] == 'E'
		&& header.ID[2] == 'S' && header.ID[3] == 'M') {
		return	0;
	} else {
		return	ROM_ERR_UNSUPPORTFORMAT;
	}

	return	ROM_ERR_UNSUPPORTFORMAT;
}

//
// ROMファイル名のチェック(PALを自動判別)
//
void	ROM_FilenameCheck( ROM *pme, const char* fname )
{
	if(strstr(fname, "(E)")){
		pme->bPAL = TRUE;
	}
}

// Get NESHEADER
NESHEADER* ROM_GetNesHeader(ROM *pme) { return &pme->header; }
// Get NSFHEADER
NSFHEADER* ROM_GetNsfHeader(ROM *pme) { return &pme->nsfheader; }

// Get ROM buffer pointer
LPBYTE	ROM_GetPROM(ROM *pme)	{ return pme->lpPRG; }
LPBYTE	ROM_GetVROM(ROM *pme)	{ return pme->lpCHR; }
LPBYTE	ROM_GetTRAINER(ROM *pme)	{ return pme->lpTrainer; }
LPBYTE	ROM_GetDISKBIOS(ROM *pme)	{ return pme->lpDiskBios; }
LPBYTE	ROM_GetDISK(ROM *pme)	{ return pme->lpDisk; }

// Get ROM size
BYTE	ROM_GetPROM_SIZE(ROM *pme)	{ return pme->header.PRG_PAGE_SIZE; }
BYTE	ROM_GetVROM_SIZE(ROM *pme)	{ return pme->header.CHR_PAGE_SIZE; }

// Get PROM
DWORD	ROM_GetPROM_CRC(ROM *pme)	{ return pme->crc; }
DWORD	ROM_GetROM_CRC(ROM *pme)	{ return pme->crcall; }
DWORD	ROM_GetVROM_CRC(ROM *pme)	{ return pme->crcvrom; }

// Get FDS ID
DWORD	ROM_GetMakerID(ROM *pme)	{ return pme->fdsmakerID; }
DWORD	ROM_GetGameID(ROM *pme) { return pme->fdsgameID; }

// ROM control
BOOL	ROM_IsVMIRROR(ROM *pme) { return pme->header.control1 & ROM_VMIRROR; }
BOOL	ROM_Is4SCREEN(ROM *pme) { return pme->header.control1 & ROM_4SCREEN; }
BOOL	ROM_IsSAVERAM(ROM *pme) { return pme->header.control1 & ROM_SAVERAM; }
BOOL	ROM_IsTRAINER(ROM *pme) { return pme->header.control1 & ROM_TRAINER; }
BOOL	ROM_IsVSUNISYSTEM(ROM *pme) { return pme->header.control2 & ROM_VSUNISYSTEM; }
BOOL	ROM_IsPAL(ROM *pme) 	{ return pme->bPAL; }

// Mapper
INT ROM_GetMapperNo(ROM *pme) { return pme->mapper; }

// Disks
INT ROM_GetDiskNo(ROM *pme) { return pme->diskno; }

// NSF
BOOL	ROM_IsNSF(ROM *pme) { return pme->bNSF; }
BYTE	ROM_GetNSF_SIZE(ROM *pme)	{ return pme->NSF_PAGE_SIZE; }

// ROM Paths
const char* ROM_GetRomPath(ROM *pme) { return pme->path; }
const char* ROM_GetRomName(ROM *pme) { return pme->name; }
const char* ROM_GetFullPathName(ROM *pme) { return pme->fullpath; }

