//
// 便利かも知れないマクロたち
//
#ifndef	__MACRO_INCLUDED__
#define	__MACRO_INCLUDED__

#include "genlib.h"
#include "ams.h"

#if defined(HAVE_MATH_LIB)  // alvin
#include <math.h>
#endif

#include "Config.h"

typedef struct NES_struct NES;

// 多重呼び出し対応マクロちゃん
#define	CLOSEHANDLE(x)	if(x) { CloseHandle(x); x = NULL; }

// その他

// RECT構造体用
#define	RCWIDTH(rc)	((rc).right-(rc).left)
#define	RCHEIGHT(rc)	((rc).bottom-(rc).top)

#if 1 // alvin: removed from DirectDraw.h & PPU.h
	// Screen size
	enum { SCREEN_WIDTH = 256, SCREEN_HEIGHT = 240 };
	//enum { SCREEN_WIDTH = 240, SCREEN_HEIGHT = 240 };
	//enum { SCREEN_WIDTH = 320, SCREEN_HEIGHT = 240 };

	// Render screen size
	enum { RENDER_WIDTH = SCREEN_WIDTH+16, RENDER_HEIGHT = SCREEN_HEIGHT };

	// Delta screen size
	enum { DELTA_WIDTH = SCREEN_WIDTH, DELTA_HEIGHT = SCREEN_HEIGHT+6 };
#endif


#endif	// !__MACRO_INCLUDED__
