//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Pad                                                         //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#ifndef	__PAD_INCLUDED__
#define	__PAD_INCLUDED__

#include "macro.h"


enum	{
	VS_TYPE0 = 0,	// SELECT1P=START1P/SELECT2P=START2P 1P/2P No reverse
	VS_TYPE1,	// SELECT1P=START1P/SELECT2P=START2P 1P/2P Reverse
	VS_TYPE2,	// SELECT1P=START1P/START1P =START2P 1P/2P No reverse
	VS_TYPE3,	// SELECT1P=START1P/START1P =START2P 1P/2P Reverse
	VS_TYPE4,	// SELECT1P=START1P/SELECT2P=START2P 1P/2P No reverse (Protection)
	VS_TYPE5,	// SELECT1P=START1P/SELECT2P=START2P 1P/2P Reverse    (Protection)
	VS_TYPE6,	// SELECT1P=START1P/SELECT2P=START2P 1P/2P Reverse	(For Golf)
	VS_TYPEZ,	// ZAPPER
};



typedef struct PAD_struct PAD;

PAD *	PAD_New( NES* parent );
void PAD_Destroy(PAD *pme);

// ÉÅÉìÉoä÷êî
	void	PAD_Reset(PAD *pme);

	// For movie
	DWORD	PAD_GetSyncData(PAD *pme);
	void	PAD_SetSyncData( PAD *pme, DWORD data );

	void	PAD_VSync(PAD *pme);

	void	PAD_Sync(PAD *pme);
	BYTE	PAD_SyncSub( PAD *pme, INT no );

	void	PAD_SetStrobe( PAD *pme, BOOL bStrb ) ;
	BOOL	PAD_GetStrobe(PAD *pme) ;

	void	PAD_Strobe(PAD *pme);
	WORD	PAD_StrobeSub(PAD *pme,  INT no );

	BYTE	PAD_Read( PAD *pme, WORD addr );
	void	PAD_Write( PAD *pme, WORD addr, BYTE data );

	BOOL	PAD_IsZapperMode(PAD *pme) ;

	// For VS-Unisystem
	void	PAD_SetSwapPlayer( PAD *pme, BOOL bSwap ) ;
	void	PAD_SetSwapButton( PAD *pme, BOOL bSwap ) ;
	void	PAD_SetVSType( PAD *pme, INT nType ) ;
	BYTE	PAD_GetNsfController(PAD *pme) ;





struct	PAD_struct{
//public:

	DWORD	pad1bit, pad2bit;
	DWORD	pad3bit, pad4bit;

	// Frame Synchronized
	LONG	zapperx, zappery;
	BYTE	zapperbit;
	BYTE	crazyclimberbit;

	// For NSF Player

//protected:
	NES*	nes;

	// Frame Synchronized
	BYTE	padbit[4];
	BYTE	micbit;

	BYTE	padbitsync[4];
	BYTE	micbitsync;

	INT	padcnt[4][2];

	BOOL	bStrobe;
	BOOL	bZapperMode;

	// For VS-Unisystem
	INT	nVSSwapType;
	BOOL	bSwapPlayer;
	BOOL	bSwapButton;

	// For BarcodeWorld
	BOOL	bBarcodeWorld;

	// For NSF Player

	BYTE	nsfbit;
};



#endif	// !__PAD_INCLUDED__

