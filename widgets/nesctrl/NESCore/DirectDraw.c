//
// DirectDraw class
//
#include "DirectDraw.h"
#include "widget.h"




	LPBYTE			m_lpRender;
	BYTE			m_LineColormode[RENDER_HEIGHT];

	// 256色モード用
	t_RGBQUAD	m_cpPalette[8][64*2];	// Color
	t_RGBQUAD	m_mpPalette[8][64*2];	// Monochrome
	t_RGBQUAD	m_RGB[256];	// Monochrome



//////////////////////////////////////////////////////////////////////
// デフォルトパレット
//////////////////////////////////////////////////////////////////////
static const PALBUF	m_PalDefault[64] = {
	{0x7F, 0x7F, 0x7F}
	, {0x20, 0x00, 0xB0}
	, {0x28, 0x00, 0xB8}
	, {0x60, 0x10, 0xA0}
	, {0x98, 0x20, 0x78}
	, {0xB0, 0x10, 0x30}
	, {0xA0, 0x30, 0x00}
	, {0x78, 0x40, 0x00}
	, {0x48, 0x58, 0x00}
	, {0x38, 0x68, 0x00}
	, {0x38, 0x6C, 0x00}
	, {0x30, 0x60, 0x40}
	, {0x30, 0x50, 0x80}
	, {0x00, 0x00, 0x00}
	, {0x00, 0x00, 0x00}
	, {0x00, 0x00, 0x00}
	, {0xBC, 0xBC, 0xBC}
	, {0x40, 0x60, 0xF8}
	, {0x40, 0x40, 0xFF}
	, {0x90, 0x40, 0xF0}
	, {0xD8, 0x40, 0xC0}
	, {0xD8, 0x40, 0x60}
	, {0xE0, 0x50, 0x00}
	, {0xC0, 0x70, 0x00}
	, {0x88, 0x88, 0x00}
	, {0x50, 0xA0, 0x00}
	, {0x48, 0xA8, 0x10}
	, {0x48, 0xA0, 0x68}
	, {0x40, 0x90, 0xC0}
	, {0x00, 0x00, 0x00}
	, {0x00, 0x00, 0x00}
	, {0x00, 0x00, 0x00}
	, {0xFF, 0xFF, 0xFF}
	, {0x60, 0xA0, 0xFF}
	, {0x50, 0x80, 0xFF}
	, {0xA0, 0x70, 0xFF}
	, {0xF0, 0x60, 0xFF}
	, {0xFF, 0x60, 0xB0}
	, {0xFF, 0x78, 0x30}
	, {0xFF, 0xA0, 0x00}
	, {0xE8, 0xD0, 0x20}
	, {0x98, 0xE8, 0x00}
	, {0x70, 0xF0, 0x40}
	, {0x70, 0xE0, 0x90}
	, {0x60, 0xD0, 0xE0}
	, {0x60, 0x60, 0x60}
	, {0x00, 0x00, 0x00}
	, {0x00, 0x00, 0x00}
	, {0xFF, 0xFF, 0xFF}
	, {0x90, 0xD0, 0xFF}
	, {0xA0, 0xB8, 0xFF}
	, {0xC0, 0xB0, 0xFF}
	, {0xE0, 0xB0, 0xFF}
	, {0xFF, 0xB8, 0xE8}
	, {0xFF, 0xC8, 0xB8}
	, {0xFF, 0xD8, 0xA0}
	, {0xFF, 0xF0, 0x90}
	, {0xC8, 0xF0, 0x80}
	, {0xA0, 0xF0, 0xA0}
	, {0xA0, 0xFF, 0xC8}
	, {0xA0, 0xFF, 0xF0}
	, {0xA0, 0xA0, 0xA0}
	, {0x00, 0x00, 0x00}
	, {0x00, 0x00, 0x00}
};

static const float	PalConvTbl[8][3] = {
	1.00f, 1.00f, 1.00f,
	1.00f, 0.80f, 0.73f,
	0.73f, 1.00f, 0.70f,
	0.76f, 0.78f, 0.58f,
	0.86f, 0.80f, 1.00f,
	0.83f, 0.68f, 0.85f,
	0.67f, 0.77f, 0.83f,
	0.68f, 0.68f, 0.68f,
//	1.00f, 1.00f, 1.00f,
};


#define SCAN_LINE_COLOR	75
BOOL	CalcPaletteTable(void)
{
	INT	i, j;

	for( j = 0; j < 8; j++ ) {
		for( i = 0; i < 64; i++ ) {
			DWORD	Rn, Gn, Bn;
			DWORD	Rs, Gs, Bs;

			// Normal
			Rn = (DWORD)(PalConvTbl[j][0]*m_PalDefault[i].r);
			Gn = (DWORD)(PalConvTbl[j][1]*m_PalDefault[i].g);
			Bn = (DWORD)(PalConvTbl[j][2]*m_PalDefault[i].b);
			// Scanline
			Rs = (DWORD)(PalConvTbl[j][0]*m_PalDefault[i].r*SCAN_LINE_COLOR/100.0f);
			Gs = (DWORD)(PalConvTbl[j][1]*m_PalDefault[i].g*SCAN_LINE_COLOR/100.0f);
			Bs = (DWORD)(PalConvTbl[j][2]*m_PalDefault[i].b*SCAN_LINE_COLOR/100.0f);

			m_cpPalette[j][i+0x00].rgbRed   = (BYTE)Rn;
			m_cpPalette[j][i+0x00].rgbGreen = (BYTE)Gn;
			m_cpPalette[j][i+0x00].rgbBlue  = (BYTE)Bn;
			m_cpPalette[j][i+0x40].rgbRed   = (BYTE)Rs;
			m_cpPalette[j][i+0x40].rgbGreen = (BYTE)Gs;
			m_cpPalette[j][i+0x40].rgbBlue  = (BYTE)Bs;

			// Monochrome
			Rn = (DWORD)(m_PalDefault[i&0x30].r);
			Gn = (DWORD)(m_PalDefault[i&0x30].g);
			Bn = (DWORD)(m_PalDefault[i&0x30].b);
			Rn = 
			Gn = 
			Bn = (DWORD)(0.299f * Rn + 0.587f * Gn + 0.114f * Bn);
			Rn = (DWORD)(PalConvTbl[j][0]*Rn);
			Gn = (DWORD)(PalConvTbl[j][1]*Gn);
			Bn = (DWORD)(PalConvTbl[j][2]*Bn);
			if( Rn > 0xFF ) Rs = 0xFF;
			if( Gn > 0xFF ) Gs = 0xFF;
			if( Bn > 0xFF ) Bs = 0xFF;
			// Scanline
			Rs = (DWORD)(m_PalDefault[i&0x30].r*SCAN_LINE_COLOR/100.0f);
			Gs = (DWORD)(m_PalDefault[i&0x30].g*SCAN_LINE_COLOR/100.0f);
			Bs = (DWORD)(m_PalDefault[i&0x30].b*SCAN_LINE_COLOR/100.0f);
			Rs = 
			Gs = 
			Bs = (DWORD)(0.299f * Rs + 0.587f * Gs + 0.114f * Bs);
			Rs = (DWORD)(PalConvTbl[j][0]*Rs);
			Gs = (DWORD)(PalConvTbl[j][1]*Gs);
			Bs = (DWORD)(PalConvTbl[j][2]*Bs);
			if( Rs > 0xFF ) Rs = 0xFF;
			if( Gs > 0xFF ) Gs = 0xFF;
			if( Bs > 0xFF ) Bs = 0xFF;

			m_mpPalette[j][i+0x00].rgbRed   = (BYTE)Rn;
			m_mpPalette[j][i+0x00].rgbGreen = (BYTE)Gn;
			m_mpPalette[j][i+0x00].rgbBlue  = (BYTE)Bn;
			m_mpPalette[j][i+0x40].rgbRed   = (BYTE)Rs;
			m_mpPalette[j][i+0x40].rgbGreen = (BYTE)Gs;
			m_mpPalette[j][i+0x40].rgbBlue  = (BYTE)Bs;

		}
	}
	for( i = 0; i < 64; i++ ) {
		m_RGB[i	  ] = m_cpPalette[0][i];
		m_RGB[i+0x40] = m_mpPalette[0][i];
	}

	return	TRUE;
}


static t_HWidget ghWidget = NULL;

void DirectDraw_Initial(t_HWidget widget)
{
	// LineColormode
	memset( m_LineColormode, 0, sizeof(m_LineColormode) );

	ZEROMEMORY( m_cpPalette, sizeof(m_cpPalette) );
	ZEROMEMORY( m_mpPalette, sizeof(m_mpPalette) );
	ZEROMEMORY( m_RGB, sizeof(m_RGB) );
	
	m_lpRender = (LPBYTE)MALLOC( RENDER_WIDTH*RENDER_HEIGHT );
	memset( m_lpRender, 0x3F, RENDER_WIDTH*RENDER_HEIGHT );

	CalcPaletteTable();

	ghWidget = widget;
}

void DirectDraw_Release(void)
{
	FREEIF( m_lpRender );
}

// 表示中のパレットテーブルの取得
t_RGBQUAD*	DirectDraw_GetPaletteData(  void )
{
	return m_RGB;
}

LPBYTE	DirectDraw_GetRenderScreen(void)	{ return m_lpRender; }
LPBYTE	DirectDraw_GetLineColormode(void)	{ return m_LineColormode; }

#if 0
BYTE bmp_scrn[54+4*256+256*240];
#endif

void	DirectDraw_Paint(void)
{
	LPBYTE	lpScn = DirectDraw_GetRenderScreen();
	t_RGBQUAD *rgb = DirectDraw_GetPaletteData();
	t_HGDI gdi = wgt_get_gdi(ghWidget);
	t_HImage image;
	gu32 tick;
	t_ColorRasterOp rop = gdi_set_rop(gdi, IMG_RO_COPY);

	tick = tick_current();
	image = image_from_8bpp((gu32*)rgb, (gu32*)(lpScn+8), RENDER_WIDTH, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	g_printf("image_from_8bpp=%d", tick_current()-tick);
	tick = tick_current();
	gdi_draw_image_in(gdi, 0, 0, wgt_bound_dx(ghWidget), wgt_bound_dy(ghWidget), image, IDF_HALIGN_CENTER|IDF_VALIGN_MIDDLE);
	g_object_unref(image);
	gdi_set_rop(gdi, rop);
	g_printf("gdi_draw_image_in=%d", tick_current()-tick);
	tick = tick_current();
	gdi_blt(gdi, 0, 0, wgt_bound_dx(ghWidget), wgt_bound_dy(ghWidget));
	g_object_unref(gdi);
	g_printf("gdi_blt=%d", tick_current()-tick);
}

// Infomation string
void	DirectDraw_SetInfoString( char * str )
{
#if 0
	if( str ) {
		if( strlen(str) > INFOSTR_SIZE ) {
			memcpy( m_szInfo, str, INFOSTR_SIZE );
			m_szInfo[INFOSTR_SIZE] = '\0';
		} else {
			strcpy( m_szInfo, str );
		}
	} else {
		m_szInfo[0] = '\0';
	}
#else
	if(str){
		DebugOut(str);
	}
#endif
}

// Message string
void	DirectDraw_SetMessageString( char * str )
{
#if 0
	if( str ) {
		if( strlen(str) > INFOSTR_SIZE ) {
			memcpy( m_szMess, str, INFOSTR_SIZE );
			m_szMess[INFOSTR_SIZE] = '\0';
		} else {
			strcpy( m_szMess, str );
		}
		m_bMessage = TRUE;
		m_dwMessageTime = tick_current();
	} else {
		m_bMessage = FALSE;
		m_szMess[0] = '\0';
	}
#else
	if(str){
		DebugOut(str);
	}
#endif
}

#define GetPixel(x,y) \
	(( x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT )? m_lpRender[8+(x)+RENDER_WIDTH*y] : 0x00)
	
