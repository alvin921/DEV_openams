//
// アーカイブファイル操作
//
// Original:NesterJ arc.cpp arc.h by Mikami Kana
// Original:NNNesterJ ulunzip.cpp
//
// Zlib use!
// Reprogrammed by Norix
//
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include <time.h>

#include "macro.h"

#include "App.h"
#include "Pathlib.h"

#include "VirtuaNESres.h"

#define UNZ_BUFSIZE (65536)

#include "unzip.h"

#pragma	pack(1)
#define	FNAME_MAX32	512

typedef struct {
	DWORD 	dwOriginalSize;
 	DWORD 	dwCompressedSize;
	DWORD	dwCRC;
	UINT	uFlag;
	UINT	uOSType;
	WORD	wRatio;
	WORD	wDate;
	WORD 	wTime;
	char	szFileName[FNAME_MAX32 + 1];
	char	dummy1[3];
	char	szAttribute[8];
	char	szMode[8];
} INDIVIDUALINFO, *LPINDIVIDUALINFO;
#pragma pack()

// Un??? use function
typedef	int(WINAPI *EXECUTECOMMAND)(HWND,LPCSTR,LPSTR,const DWORD);
typedef	BOOL(WINAPI *CHECKARCHIVE)(LPCSTR,const int);
typedef	int(WINAPI *EXTRACTMEM)(HWND,LPCSTR,LPBYTE,const DWORD,time_t,LPWORD,LPDWORD);
typedef	HGLOBAL(WINAPI *OPENARCHIVE)(HWND,LPCSTR,const DWORD);
typedef	int(WINAPI *CLOSEARCHIVE)(HGLOBAL);
typedef	int(WINAPI *FINDFIRST)(HGLOBAL,LPCSTR,INDIVIDUALINFO*);

static	LPCSTR	pszArchiver[] = {
	"UNLHA32",
	"UNZIP32",
	"UNRAR32",
	"CAB32",
	NULL
};

static	LPCSTR	pszFuncPrefix[] = {
	"Unlha",
	"UnZip",
	"Unrar",
	"Cab",
};

static	LPCSTR	pszCommand[] = {
	NULL,
	NULL,
	"-e -u \"%s\" \"%s\" \"%s\"",
	"-x -j \"%s\" \"%s\" \"%s\"",
};

static	LPCSTR	pszExtension[] = {
	"*.nes",
	"*.fds",
	"*.nsf",
	NULL
};

static	BOOL	bFileMatching[] = {
	FALSE,
	TRUE,
	FALSE,
	FALSE,
};

#define	FREEDLL(h)	if( h ) { FreeLibrary(h);h=NULL; }

#define M_ERROR_MESSAGE_OFF		0x00800000L

// zlibを使用したZIP解凍ルーチン
BOOL	ZlibUnZip( LPCSTR fname, LPBYTE* ppBuf, LPDWORD lpdwSize )
{
	unzFile		unzipFile = NULL;
	unz_global_info	unzipGlobalInfo;
	unz_file_info	unzipFileInfo;
	char		fname_buf[256];

	*ppBuf = NULL;
	*lpdwSize = 0;

	if( !(unzipFile = unzOpen( (const char*)fname )) )
		return	FALSE;

	if( unzGetGlobalInfo( unzipFile, &unzipGlobalInfo ) != UNZ_OK ) {
		unzClose( unzipFile );
		return	FALSE;
	}

	for( uLong i = 0; i < unzipGlobalInfo.number_entry; i++ ) {
		if( unzGetCurrentFileInfo( unzipFile, &unzipFileInfo, fname_buf, sizeof(fname_buf), NULL, 0, NULL, 0 ) != UNZ_OK )
			break;

		char*	pExt = ::PathFindExtension( fname_buf );
		if( _stricmp( pExt, ".nes" ) == 0 || _stricmp( pExt, ".fds" ) == 0 || _stricmp( pExt, ".nsf" ) == 0 ) {
			if( unzipFileInfo.uncompressed_size ) {
				if( unzOpenCurrentFile( unzipFile ) != UNZ_OK )
					break;

				if( unzipFileInfo.uncompressed_size > 0 ) {
					if( !(*ppBuf = (LPBYTE)::malloc( unzipFileInfo.uncompressed_size )) )
						break;

					uInt	size = unzReadCurrentFile( unzipFile, *ppBuf, unzipFileInfo.uncompressed_size );
					if( size != unzipFileInfo.uncompressed_size )
						break;
				}
				*lpdwSize = unzipFileInfo.uncompressed_size;

				if( unzCloseCurrentFile( unzipFile ) != UNZ_OK )
					break;
				unzClose( unzipFile );
				return	TRUE;
			}
		}

		// Next file
		if( (i+1) < unzipGlobalInfo.number_entry ) {
			if( unzGoToNextFile( unzipFile ) != UNZ_OK ) {
				break;
			}
		}
	}

	FREE( *ppBuf );

	if( unzipFile ) {
		unzCloseCurrentFile( unzipFile );
		unzClose( unzipFile );
	}
	return	FALSE;
}

BOOL	UnCompress( LPCSTR fname, LPBYTE* ppBuf, LPDWORD lpdwSize )
{
//	char*	pExt = ::PathFindExtension( fname );
//	if( _stricmp( pExt, ".zip" ) == 0 ) {
		// ZIPならまずzlibライブラリの解凍を使ってみる
		if( ZlibUnZip( fname, ppBuf, lpdwSize ) ) {
//DebugOut( "zlib unzip ok! [%s]\n", fname );
			return	TRUE;
		}
//	}

	return	FALSE;
}
// Archive
