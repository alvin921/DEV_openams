//
// DirectDraw class
//
#ifndef	__DIRECTDRAW_INCLUDED__
#define	__DIRECTDRAW_INCLUDED__

#if 0 // alvin
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>
#include <mmsystem.h>

#include <vector>
using namespace std;

#define DIRECTDRAW_VERSION	0x0700
#include <ddraw.h>
#pragma message("linking with Microsoft's DirectDraw library ...") 
#pragma comment(lib, "ddraw.lib") 
#pragma comment(lib, "dxguid.lib") 
#endif


#include "macro.h"

typedef struct	tagPALBUF {
	BYTE		r;
	BYTE		g;
	BYTE		b;
} PALBUF, *LPPALBUF;

typedef struct /*tagRGBQUAD*/ {
  BYTE    rgbBlue; 
  BYTE    rgbGreen; 
  BYTE    rgbRed; 
  BYTE    rgbReserved; 
} t_RGBQUAD; 

void DirectDraw_Initial(t_HWidget widget);
void DirectDraw_Release(void);

LPBYTE	DirectDraw_GetRenderScreen(void);
LPBYTE	DirectDraw_GetLineColormode(void);

void	DirectDraw_SetInfoString( char * str );
void	DirectDraw_SetMessageString( char * str );

t_RGBQUAD*	DirectDraw_GetPaletteData( void );

void	DirectDraw_Paint(void);

#endif	// !__DIRECTDRAW_INCLUDED__
