//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Konami VRC7                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "nes.h"
#include "APU_VRC7.h"



static void Destroy(APU_VRC7 *pme)
{
	if( pme && pme->VRC7_OPLL ) {
		OPLL_delete( pme->VRC7_OPLL );
		pme->VRC7_OPLL = NULL;
//		OPLL_close();	// 無くても良い(中身無し)
	}
}

static void	Setup( APU_VRC7 *pme, FLOAT fClock, INT nRate );

static void	Reset( APU_VRC7 *pme, FLOAT fClock, INT nRate )
{
	if( pme->VRC7_OPLL ) {
		OPLL_reset( pme->VRC7_OPLL );
		OPLL_reset_patch( pme->VRC7_OPLL, OPLL_VRC7_TONE );
		pme->VRC7_OPLL->masterVolume = 128;
	}

	pme->address = 0;

	Setup( pme, fClock, nRate );
}

static void	Setup( APU_VRC7 *pme, FLOAT fClock, INT nRate )
{
	OPLL_setClock( (uint32)(fClock*2.0f), (uint32)nRate );
}

static void	Write( APU_VRC7 *pme, WORD addr, BYTE data )
{
	if( pme->VRC7_OPLL ) {
		if( addr == 0x9010 ) {
			pme->address = data;
		} else if( addr == 0x9030 ) {
			OPLL_writeReg( pme->VRC7_OPLL, pme->address, data );
		}
	}
}

static INT	Process( APU_VRC7 *pme, INT channel )
{
	if( pme->VRC7_OPLL )
		return	OPLL_calc( pme->VRC7_OPLL );

	return	0;
}

static INT	GetFreq( APU_VRC7 *pme, INT channel )
{
	if( pme->VRC7_OPLL && channel < 8 ) {
		INT	fno = (((INT)pme->VRC7_OPLL->reg[0x20+channel]&0x01)<<8)
			    + (INT)pme->VRC7_OPLL->reg[0x10+channel];
		INT	blk = (pme->VRC7_OPLL->reg[0x20+channel]>>1) & 0x07;
		float	blkmul[] = { 0.5f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f };

		if( pme->VRC7_OPLL->reg[0x20+channel] & 0x10 ) {
			return	(INT)((256.0*(double)fno*blkmul[blk]) / ((double)(1<<18)/(3579545.0/72.0)));
		}
	}

	return	0;
}

static APU_VRC7_vtbl vtbl = {
	Destroy
	, Reset
	, Setup
	, Process
	, Write
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, GetFreq
	, NULL
	, NULL
	, NULL
};

APU_VRC7 * APU_VRC7_new(NES* parent)
{
	APU_VRC7 *pme = (APU_VRC7 *)MALLOC(sizeof(APU_VRC7));

	memset(pme, 0, sizeof(APU_VRC7));

	pme->vtbl = &vtbl;
	
	OPLL_init( 3579545, (uint32)22050 );	// 仮のサンプリングレート
	pme->VRC7_OPLL = OPLL_new();

	if( pme->VRC7_OPLL ) {
		OPLL_reset( pme->VRC7_OPLL );
		OPLL_reset_patch( pme->VRC7_OPLL, OPLL_VRC7_TONE );
		pme->VRC7_OPLL->masterVolume = 128;
	}

	// 仮設定
	Reset( pme, APU_CLOCK, 22050 );
	
	return pme;
}

