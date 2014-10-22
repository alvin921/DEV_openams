//
// ê›íËï€ë∂ÉNÉâÉX
//

#include "macro.h"

// Global instance

CConfig Config = {
	{ // CCfgEmulator
		FALSE
		, TRUE
		, TRUE
		, 120
		, FALSE
		, 3
		, TRUE	// TRUE:NES FALSE:Famicom
		, TRUE
		, TRUE
		, FALSE
		, FALSE
		, FALSE
	}
	, { // CCfgGraphics
		/*bAspect*/FALSE
		/*bAllSprite	*/, TRUE
		/*bAllLine	*/, FALSE
		/*bFPSDisp	*/, FALSE
		/*bTVFrame	*/, FALSE
		/*bScanline */, FALSE
		/*nScanlineColor	*/, 75
		/*bSyncDraw */, FALSE
		/*bFitZoom	*/, FALSE
		/*bLeftClip */, TRUE
		/*bWindowVSync	*/, FALSE
		/*bSyncNoSleep	*/, FALSE
		/*bDiskAccessLamp */, FALSE
		/*bDoubleSize */, FALSE
		/*bSystemMemory */, FALSE
		/*bUseHEL	*/, FALSE
		/*bNoSquareList */, FALSE
		/*nGraphicsFilter */, 0
		/*dwDisplayWidth	*/, 640
		/*dwDisplayHeight */, 480
		/*dwDisplayDepth	*/, 16
		/*dwDisplayRate */, 0
		/*bPaletteFile */, FALSE
	}
	, { // CCfgSound
		TRUE
		, 22050
		, 8
		, 4
		, 0
		, FALSE
		, FALSE
		, TRUE
		, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100}
	}
	, { // CCfgController
		{MVK_UP, MVK_UP, MVK_UP, MVK_UP, MVK_UP, MVK_UP, MVK_UP, MVK_UP, MVK_UP, MVK_UP}
	}
};

