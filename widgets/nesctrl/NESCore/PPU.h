//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES PPU core                                                    //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__PPU_INCLUDED__
#define	__PPU_INCLUDED__

#include "macro.h"


// PPU Control Register #1	PPU #0
#define	PPU_VBLANK_BIT		0x80
#define	PPU_SPHIT_BIT		0x40		// à·Ç§ÅH
#define	PPU_SP16_BIT		0x20
#define	PPU_BGTBL_BIT		0x10
#define	PPU_SPTBL_BIT		0x08
#define	PPU_INC32_BIT		0x04
#define	PPU_NAMETBL_BIT		0x03

// PPU Control Register #2	PPU #1
#define	PPU_BGCOLOR_BIT		0xE0
#define	PPU_SPDISP_BIT		0x10
#define	PPU_BGDISP_BIT		0x08
#define	PPU_SPCLIP_BIT		0x04
#define	PPU_BGCLIP_BIT		0x02
#define	PPU_COLORMODE_BIT	0x01

// PPU Status Register		PPU #2
#define	PPU_VBLANK_FLAG		0x80
#define	PPU_SPHIT_FLAG		0x40
#define	PPU_SPMAX_FLAG		0x20
#define	PPU_WENABLE_FLAG	0x10

// SPRITE Attribute
#define	SP_VMIRROR_BIT		0x80
#define	SP_HMIRROR_BIT		0x40
#define	SP_PRIORITY_BIT		0x20
#define	SP_COLOR_BIT		0x03


typedef struct PPU_struct PPU;

	PPU * PPU_New( NES* parent );
	void	PPU_Destroy(PPU *pme);

	void	PPU_Reset(PPU *pme);

	BYTE	PPU_Read ( PPU *pme, WORD addr );
	void	PPU_Write( PPU *pme, WORD addr, BYTE data );

	void	PPU_DMA( PPU *pme, BYTE data );

	void	PPU_VBlankStart(PPU *pme);
	void	PPU_VBlankEnd(PPU *pme);

	void	PPU_FrameStart(PPU *pme);
	void	PPU_FrameEnd(PPU *pme);

	void	PPU_SetRenderScanline( PPU *pme, INT scanline );

	void	PPU_ScanlineStart(PPU *pme);
	void	PPU_ScanlineNext(PPU *pme);

	WORD	PPU_GetPPUADDR(PPU *pme);
	WORD	PPU_GetTILEY(PPU *pme);

	// Scanline base render
	void	PPU_Scanline( PPU *pme, INT scanline, BOOL bMax, BOOL bLeftClip );
	void	PPU_DummyScanline( PPU *pme, INT scanline );

	// For mapper
	void	PPU_SetExtLatchMode( PPU *pme, BOOL bMode );
	void	PPU_SetChrLatchMode( PPU *pme, BOOL bMode );
	void	PPU_SetExtNameTableMode( PPU *pme, BOOL bMode );
	void	PPU_SetExtMonoMode( PPU *pme, BOOL bMode );
	BOOL	PPU_GetExtMonoMode(PPU *pme);

	BOOL	PPU_IsDispON(PPU *pme);
	BOOL	PPU_IsBGON(PPU *pme);
	BOOL	PPU_IsSPON(PPU *pme);

	BYTE	PPU_GetBGCOLOR(PPU *pme);

	BOOL	PPU_IsBGCLIP(PPU *pme);
	BOOL	PPU_IsSPCLIP(PPU *pme);

	BOOL	PPU_IsMONOCROME(PPU *pme);

	BOOL	PPU_IsVBLANK(PPU *pme);
	BOOL	PPU_IsSPHIT(PPU *pme);
	BOOL	PPU_IsSPMAX(PPU *pme);
	BOOL	PPU_IsPPUWE(PPU *pme);

	BOOL	PPU_IsSprite0( PPU *pme, INT scanline );

	void	PPU_SetScreenPtr( PPU *pme, LPBYTE lpScn, LPBYTE lpMode );
	LPBYTE	PPU_GetScreenPtr(PPU *pme);

	INT	PPU_GetScanlineNo(PPU *pme);

	// For VS-Unisystem
	void	PPU_SetVSMode( PPU *pme, BOOL bMode );
	void	PPU_SetVSSecurity( PPU *pme, BYTE data );
	void	PPU_SetVSColorMap( PPU *pme, INT nColorMap );

#endif	// !__PPU_INCLUDED__

