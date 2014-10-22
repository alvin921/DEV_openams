//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Pad                                                         //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#include "macro.h"

#include "DirectDraw.h"
#include "DirectInput.h"

#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "pad.h"
#include "rom.h"





static	void	PAD_NsfSub(PAD*pme);






PAD* PAD_New( NES* parent ) 
{
	PAD*pme = (PAD *)MALLOC(sizeof(PAD));

	pme->nes = parent;
	pme->bStrobe = FALSE;
	pme->bSwapButton = FALSE;
	pme->bSwapPlayer = FALSE;
	pme->bZapperMode = FALSE;
	pme->nVSSwapType = VS_TYPE0;

	pme->padbit[0] = pme->padbit[1] = pme->padbit[2] = pme->padbit[3] = 0;
	pme->micbit = 0;

	pme->padbitsync[0] = pme->padbitsync[1] = pme->padbitsync[2] = pme->padbitsync[3] = 0;
	pme->micbitsync = 0;

	return pme;
}

void PAD_Destroy(PAD *pme)
{
	if(pme)
		FREE(pme);
}

void	PAD_Reset(PAD *pme)
{
	DWORD crc;
	pme->pad1bit = pme->pad2bit = 0;
	pme->bStrobe = FALSE;

//	if( !pme->bZapperMode ) {
//		pme->bZapperMode = FALSE;
//	}

	pme->bBarcodeWorld = FALSE;

	ZEROMEMORY( pme->padcnt, sizeof(pme->padcnt) );

	// Select Extension Devices
	crc = ROM_GetPROM_CRC(pme->nes->rom);

	if( crc == 0xfbfc6a6c		// Adventures of Bayou Billy, The(E)
	 || crc == 0xcb275051		// Adventures of Bayou Billy, The(U)
	 || crc == 0xfb69c131		// Baby Boomer(Unl)(U)
	 || crc == 0xf2641ad0		// Barker Bill's Trick Shooting(U)
	 || crc == 0xbc1dce96		// Chiller (Unl)(U)
	 || crc == 0x90ca616d		// Duck Hunt(JUE)
	 || crc == 0x59e3343f		// Freedom Force(U)
	 || crc == 0x242a270c		// Gotcha!(U)
	 || crc == 0x7b5bd2de		// Gumshoe(UE)
	 || crc == 0x255b129c		// Gun Sight(J)
	 || crc == 0x8963ae6e		// Hogan's Alley(JU)
	 || crc == 0x51d2112f		// Laser Invasion(U)
	 || crc == 0x0a866c94		// Lone Ranger, The(U)
//	 || crc == 0xe4c04eea		// Mad City(J)
	 || crc == 0x9eef47aa		// Mechanized Attack(U)
	 || crc == 0xc2db7551		// Shooting Range(U)
	 || crc == 0x163e86c0		// To The Earth(U)
	 || crc == 0x42d893e4		// Operation Wolf(J)
	 || crc == 0x1388aeb9		// Operation Wolf(U)
	 || crc == 0x0d3cf705		// Wild Gunman(J)
	 || crc == 0x389960db ) {	// Wild Gunman(JUE)
	}
	if( crc == 0x35893b67		// Arkanoid(J)
	 || crc == 0x6267fbd1 ) {	// Arkanoid 2(J)
	}
	if( crc == 0xff6621ce		// Hyper Olympic(J)
	 || crc == 0xdb9418e8		// Hyper Olympic(Tonosama Ban)(J)
	 || crc == 0xac98cd70 ) {	// Hyper Sports(J)
	}
	if( crc == 0xf9def527		// Family BASIC(Ver2.0)
	 || crc == 0xde34526e		// Family BASIC(Ver2.1a)
	 || crc == 0xf050b611		// Family BASIC(Ver3)
	 || crc == 0x3aaeed3f		// Family BASIC(Ver3)(Alt)
	 || crc == 0x868FCD89		// Family BASIC(Ver1.0)
	 || crc == 0x2D6B7E5A		// PLAYBOX BASIC(J) (Prototype_v0.0)
	 || crc == 0xDA03D908 ) {	// PLAYBOX BASIC (J)
	}
	if( crc == 0x589b6b0d		// Supor Computer V3.0
	 || crc == 0x8b265862		// Supor English
	 || crc == 0x41401c6d		// Supor Computer V4.0
	 || crc == 0x82F1Fb96		// Supor Computer(Russia) V1.0
	 || crc == 0xd5d6eac4 ) {	// EDU(C) Computer
		NES_SetVideoMode( pme->nes, TRUE );
	}
	if( crc == 0xc68363f6		// Crazy Climber(J)
	 || crc == 0x2989ead6		// Smash TV(U) [!]
	 || crc == 0x0b8f8128 ) {	// Smash TV(E) [!]
	}
	if( crc == 0x20d22251 ) {	// Top rider(J)
	}
	if( crc == 0x0cd00488 ) {	// Space Shadow(J)
	}

	if( crc == 0x8c8fa83b		// Family Trainer - Athletic World (J)
	 || crc == 0x7e704a14		// Family Trainer - Jogging Race (J)
	 || crc == 0x2330a5d3 ) {	// Family Trainer - Rairai Kyonshiizu (J)
	}
	if( crc == 0xf8da2506		// Family Trainer - Aerobics Studio (J)
	 || crc == 0xca26a0f1		// Family Trainer - Dai Undoukai (J)
	 || crc == 0x28068b8c		// Family Trainer - Fuuun Takeshi Jou 2 (J)
	 || crc == 0x10bb8f9a		// Family Trainer - Manhattan Police (J)
	 || crc == 0xad3df455		// Family Trainer - Meiro Dai Sakusen (J)
	 || crc == 0x8a5b72c0		// Family Trainer - Running Stadium (J)
	 || crc == 0x59794f2d ) {	// Family Trainer - Totsugeki Fuuun Takeshi Jou (J)
	}
	if( crc == 0x9fae4d46		// Ide Yousuke Meijin no Jissen Mahjong (J)
	 || crc == 0x7b44fb2a ) {	// Ide Yousuke Meijin no Jissen Mahjong 2 (J)
	}
	if( crc == 0x786148b6 ) {	// Exciting Boxing (J)
	}
	if( crc == 0xc3c0811d		// Oeka Kids - Anpanman no Hiragana Daisuki (J)
	 || crc == 0x9d048ea4 ) {	// Oeka Kids - Anpanman to Oekaki Shiyou!! (J)
	}
#if	0
	if( crc == 0xe792de94		// Best Play - Pro Yakyuu (New) (J)
	 || crc == 0xf79d684a		// Best Play - Pro Yakyuu (Old) (J)
	 || crc == 0xc2ef3422		// Best Play - Pro Yakyuu 2 (J)
	 || crc == 0x974e8840		// Best Play - Pro Yakyuu '90 (J)
	 || crc == 0xb8747abf		// Best Play - Pro Yakyuu Special (J)
	 || crc == 0x9fa1c11f		// Castle Excellent (J)
	 || crc == 0x0b0d4d1b		// Derby Stallion - Zenkoku Ban (J)
	 || crc == 0x728c3d98		// Downtown - Nekketsu Monogatari (J)
	 || crc == 0xd68a6f33		// Dungeon Kid (J)
	 || crc == 0x3a51eb04		// Fleet Commander (J)
	 || crc == 0x7c46998b		// Haja no Fuuin (J)
	 || crc == 0x7e5d2f1a		// Itadaki Street - Watashi no Mise ni Yottette (J)
	 || crc == 0xcee5857b		// Ninjara Hoi! (J)
	 || crc == 0x50ec5e8b		// Wizardry - Legacy of Llylgamyn (J)
	 || crc == 0x343e9146		// Wizardry - Proving Grounds of the Mad Overlord (J)
	 || crc == 0x33d07e45 ) {	// Wizardry - The Knight of Diamonds (J)
		SetExController( EXCONTROLLER_TURBOFILE );
	}
#endif

	if( crc == 0x67898319 ) {	// Barcode World (J)
		pme->bBarcodeWorld = TRUE;
	}

	// VS-Unisystem
	if( ROM_IsVSUNISYSTEM(pme->nes->rom) ) {
		if( crc == 0xff5135a3		// VS Hogan's Alley
		 || crc == 0xed588f00		// VS Duck Hunt
		 || crc == 0x17ae56be ) {	// VS Freedom Force
		} else {
		}
	}

	if( crc == 0x21b099f3 ) {	// Gyromite (JUE)
	}

//	if( crc == 0x4ee735c1 ) {	// Stack-Up (JU)
//		SetExController( EXCONTROLLER_STACKUP );
//	}
}

DWORD	PAD_GetSyncData(PAD *pme)
{
DWORD	ret;

	ret = (DWORD)pme->padbit[0]|((DWORD)pme->padbit[1]<<8)|((DWORD)pme->padbit[2]<<16)|((DWORD)pme->padbit[3]<<24);
	ret |= (DWORD)pme->micbit<<8;

	return	ret;
}

void	PAD_SetSyncData( PAD *pme, DWORD data )
{
	pme->micbit = (BYTE)((data&0x00000400)>>8);
	pme->padbit[0] = (BYTE) data;
	pme->padbit[1] = (BYTE)(data>> 8);
	pme->padbit[2] = (BYTE)(data>>16);
	pme->padbit[3] = (BYTE)(data>>24);
}

void	PAD_Sync(PAD *pme)
{
	BYTE b = pme->padbit[0];
	pme->padbit[0] = PAD_SyncSub( pme, 0 );
	pme->padbit[1] = PAD_SyncSub( pme, 1 );
	pme->padbit[2] = PAD_SyncSub( pme, 2 );
	pme->padbit[3] = PAD_SyncSub( pme, 3 );

	if(b != pme->padbit[0]){
		DebugOut("PAD->Sync: pme->padbit[0](0x%02x->0x%02x)\n", b, pme->padbit[0]);
	}
	// Mic
	pme->micbit = 0;

	// For NSF
	PAD_NsfSub(pme);
}

void	PAD_VSync(PAD *pme)
{
	if(pme->padbit[0] != pme->padbitsync[0])
		DebugOut("PAD->VSync: pme->padbit[0]=0x%02x, pme->padbitsync[0]=0x%02x\n", pme->padbit[0], pme->padbitsync[0]);

	pme->padbitsync[0] = pme->padbit[0];
	pme->padbitsync[1] = pme->padbit[1];
	pme->padbitsync[2] = pme->padbit[2];
	pme->padbitsync[3] = pme->padbit[3];
	pme->micbitsync = pme->micbit;
}

static	INT	ren30fps[] = {
	1, 0
};
static	INT	ren20fps[] = {
	1, 1, 0
};
static	INT	ren15fps[] = {
	1, 1, 0, 0
};
static	INT	ren10fps[] = {
	1, 1, 1, 0, 0, 0
};

static	INT	renmask[] = {
	6, 4, 3, 2,
};
static	INT*	rentbl[] = {
	ren10fps, ren15fps, ren20fps, ren30fps
};


	void	PAD_SetStrobe( PAD *pme, BOOL bStrb ) { pme->bStrobe = bStrb; }
	BOOL	PAD_GetStrobe(PAD *pme) { return pme->bStrobe; }

	BOOL	PAD_IsZapperMode(PAD *pme) { return pme->bZapperMode; }

	// For VS-Unisystem
	void	PAD_SetSwapPlayer( PAD *pme, BOOL bSwap ) { pme->bSwapPlayer = bSwap; }
	void	PAD_SetSwapButton( PAD *pme, BOOL bSwap ) { pme->bSwapButton = bSwap; }
	void	PAD_SetVSType( PAD *pme, INT nType ) { pme->nVSSwapType = nType; }


BYTE	PAD_SyncSub( PAD *pme, INT no )
{
WORD	bit = 0;

	if(no == 0){
		bit = DirectInput_GetBits();
		if(bit){
			DebugOut("PAD->SyncSub: 0x%02x\n", bit);
		}
		if( (bit&(NES_KEY_LEFT|NES_KEY_RIGHT)) == (NES_KEY_LEFT|NES_KEY_RIGHT) )
			bit &= ~(NES_KEY_LEFT|NES_KEY_RIGHT);
	}
#if 0 // alvin
	// A rapid setup
	if( bit&NES_KEY_A_RAPID ) {
		INT	spd = Config.controller.nRapid[no][0];
		if( spd >= 3 ) spd = 3;
		INT*	tbl = rentbl[spd];

		if( pme->padcnt[no][0] >= renmask[spd] )
			pme->padcnt[no][0] = 0;

		if( tbl[pme->padcnt[no][0]] )
			bit |= NES_KEY_A;
		else
			bit &= ~NES_KEY_A;

		pme->padcnt[no][0]++;
	} else {
		pme->padcnt[no][0] = 0;
	}
	// B rapid setup
	if( bit&NES_KEY_B_RAPID ) {
		INT	spd = Config.controller.nRapid[no][1];
		if( spd >= 3 ) spd = 3;
		INT*	tbl = rentbl[spd];

		if( pme->padcnt[no][1] >= renmask[spd] )
			pme->padcnt[no][1] = 0;

		if( tbl[pme->padcnt[no][1]] )
			bit |= NES_KEY_B;
		else
			bit &= ~NES_KEY_B;

		pme->padcnt[no][1]++;
	} else {
		pme->padcnt[no][1] = 0;
	}
#else
pme->padcnt[no][0] = 0;
pme->padcnt[no][1] = 0;
#endif

	return	(BYTE)(bit&0xFF);
}

void	PAD_Strobe(PAD *pme)
{
	// For VS-Unisystem
	if(pme->padbitsync[0] & 0x08){
		int x = 0;
		DebugOut("PAD->Strobe: 0x%02x\n", pme->padbitsync[0]);
	}
	if( ROM_IsVSUNISYSTEM(pme->nes->rom) ) {
		DWORD	pad1 = (DWORD)pme->padbitsync[0] & 0xF3;
		DWORD	pad2 = (DWORD)pme->padbitsync[1] & 0xF3;
		DWORD	st1  = ((DWORD)pme->padbitsync[0] & 0x08)>>3;
		DWORD	st2  = ((DWORD)pme->padbitsync[1] & 0x08)>>3;

		switch( pme->nVSSwapType ) {
			case	VS_TYPE0:
				pme->pad1bit = pad1 | (st1<<2);
				pme->pad2bit = pad2 | (st2<<2);
				break;
			case	VS_TYPE1:
				pme->pad1bit = pad2 | (st1<<2);
				pme->pad2bit = pad1 | (st2<<2);
				break;
			case	VS_TYPE2:
				pme->pad1bit = pad1 | (st1<<2) | (st2<<3);
				pme->pad2bit = pad2;
				break;
			case	VS_TYPE3:
				pme->pad1bit = pad2 | (st1<<2) | (st2<<3);
				pme->pad2bit = pad1;
				break;
			case	VS_TYPE4:
				pme->pad1bit = pad1 | (st1<<2) | 0x08;	// 0x08=Start Protect
				pme->pad2bit = pad2 | (st2<<2) | 0x08;	// 0x08=Start Protect
				break;
			case	VS_TYPE5:
				pme->pad1bit = pad2 | (st1<<2) | 0x08;	// 0x08=Start Protect
				pme->pad2bit = pad1 | (st2<<2) | 0x08;	// 0x08=Start Protect
				break;
			case	VS_TYPE6:
				pme->pad1bit = pad1 | (st1<<2) | (((DWORD)pme->padbitsync[0] & 0x04)<<1);
				pme->pad2bit = pad2 | (st2<<2) | (((DWORD)pme->padbitsync[1] & 0x04)<<1);
				break;
			case	VS_TYPEZ:
				pme->pad1bit = 0;
				pme->pad2bit = 0;
				break;
		}

		// Coin 2‚Æ”í‚éˆ×‚ÉÁ‚·
		pme->micbit  = 0;
	} else {
		if( Config.emulator.bFourPlayer ) {
		// NES type
			pme->pad1bit = (DWORD)pme->padbitsync[0] | ((DWORD)pme->padbitsync[2]<<8) | 0x00080000;
			pme->pad2bit = (DWORD)pme->padbitsync[1] | ((DWORD)pme->padbitsync[3]<<8) | 0x00040000;
		} else {
		// Famicom type
			pme->pad1bit = (DWORD)pme->padbitsync[0];
			pme->pad2bit = (DWORD)pme->padbitsync[1];
		}
	}
	pme->pad3bit = (DWORD)pme->padbitsync[2];
	pme->pad4bit = (DWORD)pme->padbitsync[3];
}

BYTE	PAD_Read( PAD *pme, WORD addr )
{
BYTE	data = 0x00;

	if( addr == 0x4016 ) {
		if(pme->pad1bit & 0xFF)
			data = 0;
		data = (BYTE)pme->pad1bit&1;
		pme->pad1bit>>=1;
		data |= ((BYTE)pme->pad3bit&1)<<1;
		pme->pad3bit>>=1;
		// Mic
		if( !ROM_IsVSUNISYSTEM(pme->nes->rom) ) {
			data |= pme->micbitsync;
		}
	}
	if( addr == 0x4017 ) {
		data = (BYTE)pme->pad2bit&1;
		pme->pad2bit>>=1;
		data |= ((BYTE)pme->pad4bit&1)<<1;
		pme->pad4bit>>=1;

		if( pme->bBarcodeWorld ) {
			data |= NES_Barcode2(pme->nes);
		}
	}
	if(data){
//DebugOut( "Read: %x:%02X\n", addr,data );
	}

	return	data;
}

void	PAD_Write( PAD *pme, WORD addr, BYTE data )
{
//DebugOut( "Write:%x:%02X\n", addr,data );
	if( addr == 0x4016 ) {
		if( data&0x01 ) {
			pme->bStrobe = TRUE;
		} else if( pme->bStrobe ) {
			pme->bStrobe = FALSE;

			PAD_Strobe(pme);
		}
	}
	if( addr == 0x4017 ) {// external controller
	}
}

void	PAD_NsfSub(PAD *pme)
{
	WORD	bit = 0;
	
	pme->nsfbit = 0;

	bit = DirectInput_GetBits();
	if(bit){
		DebugOut("PAD->NsfSub: 0x%04x\n", bit);
	}

	// Play
	if( bit & NES_KEY_UP ) pme->nsfbit |= 1<<0;
	// Stop
	if( bit & NES_KEY_DOWN ) pme->nsfbit |= 1<<1;

	// Number -1
	if( bit & NES_KEY_LEFT ) pme->nsfbit |= 1<<2;
	// Number +1
	if( bit & NES_KEY_RIGHT ) pme->nsfbit |= 1<<3;
	// Number -16
	if( bit & NES_KEY_PRIOR ) pme->nsfbit |= 1<<4;
	// Number +16
	if( bit & NES_KEY_NEXT ) pme->nsfbit |= 1<<5;

	// “¯“ü—Í‚ğ‹Ö~‚·‚é
	if( (pme->nsfbit&((1<<2)|(1<<3))) == ((1<<2)|(1<<3)) )
		pme->nsfbit &= ~((1<<2)|(1<<3));
	if( (pme->nsfbit&((1<<4)|(1<<5))) == ((1<<4)|(1<<5)) )
		pme->nsfbit &= ~((1<<4)|(1<<5));
}

BYTE	PAD_GetNsfController(PAD *pme)
{
	return pme->nsfbit;
}


