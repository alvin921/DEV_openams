//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES PPU core                                                    //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#include "macro.h"

#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "mapper.h"



	typedef	struct	tagSPRITE {
		BYTE	y;
		BYTE	tile;
		BYTE	attr;
		BYTE	x;
	} SPRITE, *LPSPRITE;

struct PPU_struct {
	NES*	nes;

	BOOL	bExtLatch;	// For MMC5
	BOOL	bChrLatch;	// For MMC2/MMC4
	BOOL	bExtNameTable;	// For Super Monkey no Dai Bouken
	BOOL	bExtMono;	// For Final Fantasy

	WORD	loopy_y;
	WORD	loopy_shift;

	LPBYTE	lpScreen;
	LPBYTE	lpScanline;
	INT	ScanlineNo;
	LPBYTE	lpColormode;


	// Reversed bits
	BYTE	Bit2Rev[256];

	// For VS-Unisystem
	BOOL	bVSMode;
	BYTE	VSSecurityData;
	INT	nVSColorMap;
};





static BYTE	VSColorMap[5][64] = {
	{	0x35, 0xFF, 0x16, 0x22, 0x1C, 0xFF, 0xFF, 0x15,
		0xFF, 0x00, 0x27, 0x05, 0x04, 0x27, 0x08, 0x30,
		0x21, 0xFF, 0xFF, 0x29, 0x3C, 0xFF, 0x36, 0x12,
		0xFF, 0x2B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,
		0xFF, 0x31, 0xFF, 0x2A, 0x2C, 0x0C, 0xFF, 0xFF,
		0xFF, 0x07, 0x34, 0x06, 0x13, 0xFF, 0x26, 0x0F,
		0xFF, 0x19, 0x10, 0x0A, 0xFF, 0xFF, 0xFF, 0x17,
		0xFF, 0x11, 0x09, 0xFF, 0xFF, 0x25, 0x18, 0xFF 
	},
	{	0xFF, 0x27, 0x18, 0xFF, 0x3A, 0x25, 0xFF, 0x31,
////		0x16, 0x13, 0x38, 0x34, 0x20, 0x23, 0xFF, 0x0B,
//		0x16, 0x13, 0x38, 0x34, 0x20, 0x23, 0xFF, 0x1A,
		0x16, 0x13, 0x38, 0x34, 0x20, 0x23, 0x31, 0x1A,
		0xFF, 0x21, 0x06, 0xFF, 0x1B, 0x29, 0xFF, 0x22,
		0xFF, 0x24, 0xFF, 0xFF, 0xFF, 0x08, 0xFF, 0x03,
		0xFF, 0x36, 0x26, 0x33, 0x11, 0xFF, 0x10, 0x02,
		0x14, 0xFF, 0x00, 0x09, 0x12, 0x0F, 0xFF, 0x30,
		0xFF, 0xFF, 0x2A, 0x17, 0x0C, 0x01, 0x15, 0x19,
		0xFF, 0x2C, 0x07, 0x37, 0xFF, 0x05, 0xFF, 0xFF 
	},
#if	1
	{	0xFF, 0xFF, 0xFF, 0x10, 0x1A, 0x30, 0x31, 0x09,
		0x01, 0x0F, 0x36, 0x08, 0x15, 0xFF, 0xFF, 0xF0,
		0x22, 0x1C, 0xFF, 0x12, 0x19, 0x18, 0x17, 0xFF,
		0x00, 0xFF, 0xFF, 0x02, 0x16, 0x06, 0xFF, 0x35,
		0x23, 0xFF, 0x8B, 0xF7, 0xFF, 0x27, 0x26, 0x20,
		0x29, 0xFF, 0x21, 0x24, 0x11, 0xFF, 0xEF, 0xFF,
		0x2C, 0xFF, 0xFF, 0xFF, 0x07, 0xF9, 0x28, 0xFF,
		0x0A, 0xFF, 0x32, 0x37, 0x13, 0xFF, 0xFF, 0x0C 
	},
#else
	{	0xFF, 0xFF, 0xFF, 0x10, 0x0B, 0x30, 0x31, 0x09,	// 00-07
		0x01, 0x0F, 0x36, 0x08, 0x15, 0xFF, 0xFF, 0x3C,	// 08-0F
		0x22, 0x1C, 0xFF, 0x12, 0x19, 0x18, 0x17, 0x1B,	// 10-17
		0x00, 0xFF, 0xFF, 0x02, 0x16, 0x06, 0xFF, 0x35,	// 18-1F
		0x23, 0xFF, 0x8B, 0x3C, 0xFF, 0x27, 0x26, 0x20,	// 20-27
		0x29, 0x04, 0x21, 0x24, 0x11, 0xFF, 0xEF, 0xFF,	// 28-2F
		0x2C, 0xFF, 0xFF, 0xFF, 0x07, 0x39, 0x28, 0xFF,	// 30-37
		0x0A, 0xFF, 0x32, 0x38, 0x13, 0x3B, 0xFF, 0x0C 	// 38-3F
	},
#endif
#if	0
	{	0x18, 0xFF, 0x1C, 0x89, 0x0F, 0xFF, 0x01, 0x17,
		0x10, 0x0F, 0x2A, 0xFF, 0x36, 0x37, 0x1A, 0xFF,
		0x25, 0xFF, 0x12, 0xFF, 0x0F, 0xFF, 0xFF, 0x26,
		0xFF, 0xFF, 0x22, 0xFF, 0xFF, 0x0F, 0x3A, 0x21,
		0x05, 0x0A, 0x07, 0xC2, 0x13, 0xFF, 0x00, 0x15,
		0x0C, 0xFF, 0x11, 0xFF, 0xFF, 0x38, 0xFF, 0xFF,
		0xFF, 0xFF, 0x08, 0x45, 0xFF, 0xFF, 0x30, 0x3C,
		0x0F, 0x27, 0xFF, 0x60, 0x29, 0xFF, 0x30, 0x09 
	},
#else
	{	0x18, 0xFF, 0x1C, 0x89, 0x0F, 0xFF, 0x01, 0x17,	// 00-07
		0x10, 0x0F, 0x2A, 0xFF, 0x36, 0x37, 0x1A, 0xFF,	// 08-0F
		0x25, 0xFF, 0x12, 0xFF, 0x0F, 0xFF, 0xFF, 0x26,	// 10-17
		0xFF, 0xFF, 0x22, 0xFF, 0xFF, 0x0F, 0x3A, 0x21,	// 18-1F
		0x05, 0x0A, 0x07, 0xC2, 0x13, 0xFF, 0x00, 0x15,	// 20-27
		0x0C, 0xFF, 0x11, 0xFF, 0xFF, 0x38, 0xFF, 0xFF,	// 28-2F
		0xFF, 0xFF, 0x08, 0x16, 0xFF, 0xFF, 0x30, 0x3C,	// 30-37
		0x0F, 0x27, 0xFF, 0x60, 0x29, 0xFF, 0x30, 0x09 	// 38-3F
	},
#endif
	{
	// Super Xevious/Gradius
		0x35, 0xFF, 0x16, 0x22, 0x1C, 0x09, 0xFF, 0x15,	// 00-07
		0x20, 0x00, 0x27, 0x05, 0x04, 0x28, 0x08, 0x30,	// 08-0F
		0x21, 0xFF, 0xFF, 0x29, 0x3C, 0xFF, 0x36, 0x12,	// 10-17
		0xFF, 0x2B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,	// 18-1F
		0xFF, 0x31, 0xFF, 0x2A, 0x2C, 0x0C, 0x1B, 0xFF,	// 20-27
		0xFF, 0x07, 0x34, 0x06, 0xFF, 0x25, 0x26, 0x0F,	// 28-2F
		0xFF, 0x19, 0x10, 0x0A, 0xFF, 0xFF, 0xFF, 0x17,	// 30-37
		0xFF, 0x11, 0x1A, 0xFF, 0x38, 0xFF, 0x18, 0x3A,	// 38-3F
	}
};

PPU * PPU_New( NES* parent )
{
	PPU *pme = (PPU *)MALLOC(sizeof(PPU));
	INT i;
 
	memset(pme, 0, sizeof(PPU));
	pme->nes = parent;
	
	pme->lpScreen = NULL;
	pme->lpColormode = NULL;

	pme->bVSMode = FALSE;
	pme->nVSColorMap = -1;
	pme->VSSecurityData = 0;

	// ���E���]�}�X�N�e�[�u��
	for(  i = 0; i < 256; i++ ) {
		BYTE	m = 0x80;
		BYTE	c = 0;
		INT j;
		for( j = 0; j < 8; j++ ) {
			if( i&(1<<j) ) {
				c |= m;
			}
			m >>= 1;
		}
		pme->Bit2Rev[i] = c;
	}
	return pme;
}

void	PPU_Destroy(PPU *pme)
{
	if(pme)
		FREE(pme);
}

void	PPU_Reset(PPU *pme)
{
	pme->bExtLatch = FALSE;
	pme->bChrLatch = FALSE;
	pme->bExtNameTable = FALSE;
	pme->bExtMono = FALSE;

	PPUREG[0] = PPUREG[1] = 0;

	PPU56Toggle = 0;

	PPU7_Temp = 0xFF;	// VS Excitebike�ł��������Ȃ�($2006��ǂ݂ɍs���o�O������)
//	PPU7_Temp = 0;

	loopy_v = loopy_t = 0;
	loopy_x = pme->loopy_y = 0;
	pme->loopy_shift = 0;

	if( pme->lpScreen )
		memset( pme->lpScreen, 0x3F, SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(BYTE) );

	if( pme->lpColormode )
		memset( pme->lpColormode, 0, SCREEN_HEIGHT*sizeof(BYTE) );
}

BYTE	PPU_Read( PPU *pme, WORD addr )
{
BYTE	data = 0x00;

	switch( addr ) {
		// Write only Register
		case	0x2000: // PPU Control Register #1(W)
		case	0x2001: // PPU Control Register #2(W)
		case	0x2003: // SPR-RAM Address Register(W)
		case	0x2005: // PPU Scroll Register(W2)
		case	0x2006: // VRAM Address Register(W2)
			data = PPU7_Temp;	// ����
			break;
		// Read/Write Register
		case	0x2002: // PPU Status Register(R)
//DebugOut( "2002 RD L:%3d C:%8d\n", pme->ScanlineNo, pme->nes->cpu->GetTotalCycles() );
			data = PPUREG[2] | pme->VSSecurityData;
			PPU56Toggle = 0;
			PPUREG[2] &= ~PPU_VBLANK_FLAG;
			break;
		case	0x2004: // SPR_RAM I/O Register(RW)
			data = SPRAM[ PPUREG[3]++ ];
			break;
		case	0x2007: // VRAM I/O Register(RW)
			addr = loopy_v & 0x3FFF;
			data = PPU7_Temp;
			if( PPUREG[0] & PPU_INC32_BIT ) loopy_v+=32;
			else				loopy_v++;
			if( addr >= 0x3000 ) {
				if( addr >= 0x3F00 ) {
//					data &= 0x3F;
					if( !(addr&0x0010) ) {
						return	BGPAL[addr&0x000F];
					} else {
						return	SPPAL[addr&0x000F];
					}
				}
				addr &= 0xEFFF;
			}
			PPU7_Temp = PPU_MEM_BANK[addr>>10][addr&0x03FF];
	}

	return	data;
}

void	PPU_Write( PPU *pme, WORD addr, BYTE data )
{
	WORD	vaddr;
	if( pme->bVSMode && pme->VSSecurityData ) {
		if( addr == 0x2000 ) {
			addr = 0x2001;
		} else if( addr == 0x2001 ){
			addr = 0x2000;
		}
	}

	switch( addr ) {
		// Read only Register
		case	0x2002: // PPU Status register(R)
			break;
		// Write Register
		case	0x2000: // PPU Control Register #1(W)
			// NameTable select
			// t:0000110000000000=d:00000011
			loopy_t = (loopy_t & 0xF3FF)|(((WORD)data & 0x03)<<10);

			if( (data & 0x80) && !(PPUREG[0] & 0x80) && (PPUREG[2] & 0x80) ) {
				CPU_NMI(pme->nes->cpu);	// hmm..
			}
//DebugOut( "W2000 %02X O:%02X S:%02X L:%3d C:%8d\n", data, PPUREG[0], PPUREG[2], pme->ScanlineNo, pme->nes->cpu->GetTotalCycles() );

			PPUREG[0] = data;
			break;
		case	0x2001: // PPU Control Register #2(W)
//DebugOut( "W2001 %02X L:%3d C:%8d\n", data, pme->ScanlineNo, pme->nes->cpu->GetTotalCycles() );
			PPUREG[1] = data;
			break;
		case	0x2003: // SPR-RAM Address Register(W)
			PPUREG[3] = data;
			break;
		case	0x2004: // SPR_RAM I/O Register(RW)
			SPRAM[ PPUREG[3]++ ] = data;
			break;

		case	0x2005: // PPU Scroll Register(W2)
//DebugOut( "SCR WRT L:%3d C:%8d\n", pme->ScanlineNo, pme->nes->cpu->GetTotalCycles() );
			if( !PPU56Toggle ) {
			// First write
				// tile X t:0000000000011111=d:11111000
				loopy_t = (loopy_t & 0xFFE0)|(((WORD)data)>>3);
				// scroll offset X x=d:00000111
				loopy_x = data & 0x07;
			} else {
			// Second write
				// tile Y t:0000001111100000=d:11111000
				loopy_t = (loopy_t & 0xFC1F)|((((WORD)data) & 0xF8)<<2);
				// scroll offset Y t:0111000000000000=d:00000111
				loopy_t = (loopy_t & 0x8FFF)|((((WORD)data) & 0x07)<<12);
			}
			PPU56Toggle = !PPU56Toggle;
			break;
		case	0x2006: // VRAM Address Register(W2)
			if( !PPU56Toggle ) {
			// First write
				// t:0011111100000000=d:00111111
				// t:1100000000000000=0
				loopy_t = (loopy_t & 0x00FF)|((((WORD)data) & 0x3F)<<8);
			} else {
			// Second write
				// t:0000000011111111=d:11111111
				loopy_t = (loopy_t & 0xFF00)|(WORD)data;
				// v=t
				loopy_v = loopy_t;

				Mapper_PPU_Latch( pme->nes->mapper, loopy_v );
			}
			PPU56Toggle = !PPU56Toggle;
			break;

		case	0x2007: // VRAM I/O Register(RW)
			vaddr = loopy_v & 0x3FFF;
			if( PPUREG[0] & PPU_INC32_BIT ) loopy_v+=32;
			else				loopy_v++;

			if( vaddr >= 0x3000 ) {
				if( vaddr >= 0x3F00 ) {
					data &= 0x3F;
					if( pme->bVSMode && pme->nVSColorMap != -1 ) {
						BYTE	temp = VSColorMap[pme->nVSColorMap][data];
						if( temp != 0xFF ) {
							data = temp & 0x3F;
						}
					}

					if( !(vaddr&0x000F) ) {
						BGPAL[0] = SPPAL[0] = data;
					} else if( !(vaddr&0x0010) ) {
						BGPAL[vaddr&0x000F] = data;
					} else {
						SPPAL[vaddr&0x000F] = data;
					}
					BGPAL[0x04] = BGPAL[0x08] = BGPAL[0x0C] = BGPAL[0x00];
					SPPAL[0x00] = SPPAL[0x04] = SPPAL[0x08] = SPPAL[0x0C] = BGPAL[0x00];
					return;
				}
				vaddr &= 0xEFFF;
			}
			if( PPU_MEM_TYPE[vaddr>>10] != BANKTYPE_VROM ) {
				PPU_MEM_BANK[vaddr>>10][vaddr&0x03FF] = data;
			}
			break;
	}
}

void	PPU_DMA( PPU *pme, BYTE data )
{
WORD	addr = data<<8;
	INT i;
 
	for(  i = 0; i < 256; i++ ) {
		SPRAM[i] = NES_Read( pme->nes, addr+i );
	}
}

void	PPU_VBlankStart(PPU *pme)
{
	PPUREG[2] |= PPU_VBLANK_FLAG;
//	PPUREG[2] |= PPU_SPHIT_FLAG;	// VBlank�˓����ɕK��ON�H
}

void	PPU_VBlankEnd(PPU *pme)
{
	PPUREG[2] &= ~PPU_VBLANK_FLAG;
	// VBlank�E�o���ɃN���A�����
	// �G�L�T�C�g�o�C�N�ŏd�v
	PPUREG[2] &= ~PPU_SPHIT_FLAG;
}

void	PPU_FrameStart(PPU *pme)
{
	if( PPUREG[1] & (PPU_SPDISP_BIT|PPU_BGDISP_BIT) ) {
		loopy_v = loopy_t;
		pme->loopy_shift = loopy_x;
		pme->loopy_y = (loopy_v&0x7000)>>12;
	}

	if( pme->lpScreen ) {
		memset( pme->lpScreen, 0x3F, RENDER_WIDTH*sizeof(BYTE) );
	}
	if( pme->lpColormode ) {
		pme->lpColormode[0] = 0;
	}
}

void	PPU_FrameEnd(PPU *pme)
{
}

void	PPU_SetRenderScanline( PPU *pme, INT scanline )
{
	pme->ScanlineNo = scanline;
	if( scanline < RENDER_HEIGHT ) {
		pme->lpScanline = pme->lpScreen+RENDER_WIDTH*scanline;
	}
}

void	PPU_ScanlineStart(PPU *pme)
{
	if( PPUREG[1] & (PPU_BGDISP_BIT|PPU_SPDISP_BIT) ) {
		loopy_v = (loopy_v & 0xFBE0)|(loopy_t & 0x041F);
		pme->loopy_shift = loopy_x;
		pme->loopy_y = (loopy_v&0x7000)>>12;
		Mapper_PPU_Latch( pme->nes->mapper, 0x2000 + (loopy_v & 0x0FFF) );
	}
}

void	PPU_ScanlineNext(PPU *pme)
{
	if( PPUREG[1] & (PPU_BGDISP_BIT|PPU_SPDISP_BIT) ) {
		if( (loopy_v & 0x7000) == 0x7000 ) {
			loopy_v &= 0x8FFF;
			if( (loopy_v & 0x03E0) == 0x03A0 ) {
				loopy_v ^= 0x0800;
				loopy_v &= 0xFC1F;
			} else {
				if( (loopy_v & 0x03E0) == 0x03E0 ) {
					loopy_v &= 0xFC1F;
				} else {
					loopy_v += 0x0020;
				}
			}
		} else {
			loopy_v += 0x1000;
		}
		pme->loopy_y = (loopy_v&0x7000)>>12;
	}
}
static scanline_1(PPU *pme, LPBYTE BGwrite)
{
	// Without Extension Latch
	LPBYTE	pScn = pme->lpScanline+(8-pme->loopy_shift);
	LPBYTE	pBGw = BGwrite;
	
	INT tileofs = (PPUREG[0]&PPU_BGTBL_BIT)<<8;
	INT ntbladr = 0x2000+(loopy_v&0x0FFF);
	INT attradr = 0x23C0+(loopy_v&0x0C00)+((loopy_v&0x0380)>>4);
	INT ntbl_x	= ntbladr&0x001F;
	INT attrsft = (ntbladr&0x0040)>>4;
	LPBYTE	pNTBL = PPU_MEM_BANK[ntbladr>>10];
	
	INT tileadr;
	INT cache_tile = 0xFFFF0000;
	BYTE	cache_attr = 0xFF;
	
	BYTE	chr_h, chr_l, attr;
	INT i;
	LPBYTE	pBGPAL;
	
	attradr &= 0x3FF;
	
	for(  i = 0; i < 33; i++ ) {
		tileadr = tileofs+pNTBL[ntbladr&0x03FF]*0x10+pme->loopy_y;
		attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;
	
		if( cache_tile == tileadr && cache_attr == attr ) {
			*(LPDWORD)(pScn+0) = *(LPDWORD)(pScn-8);
			*(LPDWORD)(pScn+4) = *(LPDWORD)(pScn-4);
			*(pBGw+0) = *(pBGw-1);
		} else {
			cache_tile = tileadr;
			cache_attr = attr;
			chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF	];
			chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			*pBGw = chr_h|chr_l;
	
			pBGPAL = &BGPAL[attr];
			{
			register INT	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
			register INT	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
			pScn[0] = pBGPAL[(c1>>6)];
			pScn[4] = pBGPAL[(c1>>2)&3];
			pScn[1] = pBGPAL[(c2>>6)];
			pScn[5] = pBGPAL[(c2>>2)&3];
			pScn[2] = pBGPAL[(c1>>4)&3];
			pScn[6] = pBGPAL[c1&3];
			pScn[3] = pBGPAL[(c2>>4)&3];
			pScn[7] = pBGPAL[c2&3];
			}
		}
		pScn+=8;
		pBGw++;
	
		// Character latch(For MMC2/MMC4)
		if( pme->bChrLatch ) {
			Mapper_PPU_ChrLatch( pme->nes->mapper, tileadr );
		}
	
		if( ++ntbl_x == 32 ) {
			ntbl_x = 0;
			ntbladr ^= 0x41F;
			attradr = 0x03C0+((ntbladr&0x0380)>>4);
			pNTBL = PPU_MEM_BANK[ntbladr>>10];
		} else {
			ntbladr++;
		}
	}
}

void scanline_2(PPU *pme, LPBYTE BGwrite)
{
	// With Extension Latch(For MMC5)
	LPBYTE	pScn = pme->lpScanline+(8-pme->loopy_shift);
	LPBYTE	pBGw = BGwrite;
	
	INT ntbladr = 0x2000+(loopy_v&0x0FFF);
	INT ntbl_x	= ntbladr & 0x1F;
	
	INT cache_tile = 0xFFFF0000;
	BYTE	cache_attr = 0xFF;
	
	BYTE	chr_h, chr_l, attr, exattr;
	INT i;
	
	for(  i = 0; i < 33; i++ ) {
		Mapper_PPU_ExtLatchX( pme->nes->mapper, i );
		Mapper_PPU_ExtLatch( pme->nes->mapper, ntbladr, &chr_l, &chr_h, &exattr );
		attr = exattr&0x0C;
	
		if( cache_tile != (((INT)chr_h<<8)+(INT)chr_l) || cache_attr != attr ) {
			LPBYTE	pBGPAL = &BGPAL[attr];
			cache_tile = (((INT)chr_h<<8)+(INT)chr_l);
			cache_attr = attr;
			*pBGw = chr_h|chr_l;
	
			{
			register INT	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
			register INT	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
			pScn[0] = pBGPAL[(c1>>6)];
			pScn[4] = pBGPAL[(c1>>2)&3];
			pScn[1] = pBGPAL[(c2>>6)];
			pScn[5] = pBGPAL[(c2>>2)&3];
			pScn[2] = pBGPAL[(c1>>4)&3];
			pScn[6] = pBGPAL[c1&3];
			pScn[3] = pBGPAL[(c2>>4)&3];
			pScn[7] = pBGPAL[c2&3];
			}
		} else {
			*(DWORD*)(pScn+0) = *(DWORD*)(pScn-8);
			*(DWORD*)(pScn+4) = *(DWORD*)(pScn-4);
			*(pBGw+0) = *(pBGw-1);
		}
		pScn+=8;
		pBGw++;
	
		if( ++ntbl_x == 32 ) {
			ntbl_x = 0;
			ntbladr ^= 0x41F;
		} else {
			ntbladr++;
		}
	}
}

void scanline_3(PPU *pme, LPBYTE BGwrite)
{
	LPBYTE	pScn = pme->lpScanline+(8-pme->loopy_shift);
	LPBYTE	pBGw = BGwrite;
	
	INT ntbladr = 0x2000+(loopy_v&0x0FFF);
	INT attradr = 0x03C0+((loopy_v&0x0380)>>4);
	INT ntbl_x	= ntbladr&0x001F;
	INT attrsft = (ntbladr&0x0040)>>4;
	LPBYTE	pNTBL = PPU_MEM_BANK[ntbladr>>10];
	
	INT tileadr;
	INT cache_tile = 0xFFFF0000;
	BYTE	cache_attr = 0xFF;
	BYTE	cache_mono = 0x00;
	
	BYTE	chr_h, chr_l, attr;
	INT i;
	
	for(  i = 0; i < 33; i++ ) {
		tileadr = ((PPUREG[0]&PPU_BGTBL_BIT)<<8)+pNTBL[ntbladr&0x03FF]*0x10+pme->loopy_y;
	
		if( i != 0 ) {
			NES_EmulationCPU( pme->nes, FETCH_CYCLES*4 );
		}
	
		attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;
	
		if( cache_tile != tileadr || cache_attr != attr ) {
			LPBYTE	pBGPAL = &BGPAL[attr];
			cache_tile = tileadr;
			cache_attr = attr;
	
			chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF	];
			chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			*pBGw = chr_l|chr_h;
	
			{
			register INT	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
			register INT	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
			pScn[0] = pBGPAL[(c1>>6)];
			pScn[4] = pBGPAL[(c1>>2)&3];
			pScn[1] = pBGPAL[(c2>>6)];
			pScn[5] = pBGPAL[(c2>>2)&3];
			pScn[2] = pBGPAL[(c1>>4)&3];
			pScn[6] = pBGPAL[c1&3];
			pScn[3] = pBGPAL[(c2>>4)&3];
			pScn[7] = pBGPAL[c2&3];
			}
		} else {
			*(DWORD*)(pScn+0) = *(DWORD*)(pScn-8);
			*(DWORD*)(pScn+4) = *(DWORD*)(pScn-4);
			*(pBGw+0) = *(pBGw-1);
		}
		pScn+=8;
		pBGw++;
	
		// Character latch(For MMC2/MMC4)
		if( pme->bChrLatch ) {
			Mapper_PPU_ChrLatch( pme->nes->mapper, tileadr );
		}
	
		if( ++ntbl_x == 32 ) {
			ntbl_x = 0;
			ntbladr ^= 0x41F;
			attradr = 0x03C0+((ntbladr&0x0380)>>4);
			pNTBL = PPU_MEM_BANK[ntbladr>>10];
		} else {
			ntbladr++;
		}
	}
}

void scanline_4(PPU *pme, LPBYTE BGwrite)
{
	LPBYTE	pScn = pme->lpScanline+(8-pme->loopy_shift);
	LPBYTE	pBGw = BGwrite;
	
	INT ntbladr;
	INT tileadr;
	INT cache_tile = 0xFFFF0000;
	BYTE	cache_attr = 0xFF;
	BYTE	cache_mono = 0x00;
	
	BYTE	chr_h, chr_l, attr;
	
	WORD	loopy_v_tmp = loopy_v;
	INT i;
	
	for(  i = 0; i < 33; i++ ) {
		if( i != 0 ) {
			NES_EmulationCPU( pme->nes, FETCH_CYCLES*4 );
		}
	
		ntbladr = 0x2000+(loopy_v&0x0FFF);
		tileadr = ((PPUREG[0]&PPU_BGTBL_BIT)<<8)+PPU_MEM_BANK[ntbladr>>10][ntbladr&0x03FF]*0x10+((loopy_v&0x7000)>>12);
		attr = ((PPU_MEM_BANK[ntbladr>>10][0x03C0+((ntbladr&0x0380)>>4)+((ntbladr&0x001C)>>2)]>>(((ntbladr&0x40)>>4)+(ntbladr&0x02)))&3)<<2;
	
		if( cache_tile != tileadr || cache_attr != attr ) {
			LPBYTE	pBGPAL = &BGPAL[attr];
			cache_tile = tileadr;
			cache_attr = attr;
	
			chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF	];
			chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			*pBGw = chr_l|chr_h;
	
			{
			register INT	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
			register INT	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
			pScn[0] = pBGPAL[(c1>>6)];
			pScn[4] = pBGPAL[(c1>>2)&3];
			pScn[1] = pBGPAL[(c2>>6)];
			pScn[5] = pBGPAL[(c2>>2)&3];
			pScn[2] = pBGPAL[(c1>>4)&3];
			pScn[6] = pBGPAL[c1&3];
			pScn[3] = pBGPAL[(c2>>4)&3];
			pScn[7] = pBGPAL[c2&3];
			}
		} else {
			*(DWORD*)(pScn+0) = *(DWORD*)(pScn-8);
			*(DWORD*)(pScn+4) = *(DWORD*)(pScn-4);
			*(pBGw+0) = *(pBGw-1);
		}
		pScn+=8;
		pBGw++;
	
		// Character latch(For MMC2/MMC4)
		if( pme->bChrLatch ) {
			Mapper_PPU_ChrLatch( pme->nes->mapper, tileadr );
		}
	
		if( (loopy_v & 0x1F) == 0x1F ) {
			loopy_v ^= 0x041F;
		} else {
			loopy_v++;
		}
	}
	loopy_v = loopy_v_tmp;
}

void scanline_5(PPU *pme, LPBYTE BGwrite)
{
	LPBYTE	pScn = pme->lpScanline+(8-pme->loopy_shift);
	LPBYTE	pBGw = BGwrite;
	
	INT ntbladr = 0x2000+(loopy_v&0x0FFF);
	INT ntbl_x	= ntbladr & 0x1F;
	
	INT cache_tile = 0xFFFF0000;
	BYTE	cache_attr = 0xFF;
	
	BYTE	chr_h, chr_l, attr, exattr;
	INT i;
	
	for(  i = 0; i < 33; i++ ) {
		if( i != 0 ) {
			NES_EmulationCPU( pme->nes, FETCH_CYCLES*4 );
		}
		Mapper_PPU_ExtLatchX( pme->nes->mapper, i );
		Mapper_PPU_ExtLatch( pme->nes->mapper, ntbladr, &chr_l, &chr_h, &exattr );
		attr = exattr&0x0C;
	
		if( cache_tile != (((INT)chr_h<<8)+(INT)chr_l) || cache_attr != attr ) {
			LPBYTE	pBGPAL	 = &BGPAL[attr];
			cache_tile = (((INT)chr_h<<8)+(INT)chr_l);
			cache_attr = attr;
			*pBGw = chr_l|chr_h;
	
			{
			register INT	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
			register INT	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
			pScn[0] = pBGPAL[(c1>>6)];
			pScn[4] = pBGPAL[(c1>>2)&3];
			pScn[1] = pBGPAL[(c2>>6)];
			pScn[5] = pBGPAL[(c2>>2)&3];
			pScn[2] = pBGPAL[(c1>>4)&3];
			pScn[6] = pBGPAL[c1&3];
			pScn[3] = pBGPAL[(c2>>4)&3];
			pScn[7] = pBGPAL[c2&3];
			}
		} else {
			*(DWORD*)(pScn+0) = *(DWORD*)(pScn-8);
			*(DWORD*)(pScn+4) = *(DWORD*)(pScn-4);
			*(pBGw+0) = *(pBGw-1);
		}
		pScn+=8;
		pBGw++;
	
		if( ++ntbl_x == 32 ) {
			ntbl_x = 0;
			ntbladr ^= 0x41F;
		} else {
			ntbladr++;
		}
	}
}

void scanline_6(PPU *pme, LPBYTE BGwrite)
{
}

void	PPU_Scanline( PPU *pme, INT scanline, BOOL bMax, BOOL bLeftClip )
{
BYTE	BGwrite[33+1];
BYTE	BGmono[33+1];

	ZEROMEMORY( BGwrite, sizeof(BGwrite) );
	ZEROMEMORY( BGmono, sizeof(BGmono) );

	// Linecolor mode
	pme->lpColormode[scanline] = ((PPUREG[1]&PPU_BGCOLOR_BIT)>>5)|((PPUREG[1]&PPU_COLORMODE_BIT)<<7);

	// Render BG
	if( !(PPUREG[1]&PPU_BGDISP_BIT) ) {
		memset( pme->lpScanline, BGPAL[0], RENDER_WIDTH );
		if( NES_GetRenderMethod(pme->nes) == TILE_RENDER ) {
			NES_EmulationCPU( pme->nes, FETCH_CYCLES*4*32 );
		}
	} else {
		if( NES_GetRenderMethod(pme->nes) != TILE_RENDER ) {
			if( !pme->bExtLatch ) {
				// Without Extension Latch
				scanline_1(pme, BGwrite);
			} else {
				// With Extension Latch(For MMC5)
				scanline_2(pme, BGwrite);
			}
		} else {
			if( !pme->bExtLatch ) {
				// Without Extension Latch
				if( !pme->bExtNameTable ) {
					scanline_3(pme, BGwrite);
				} else {
					scanline_4(pme, BGwrite);
				}
			} else {
				// With Extension Latch(For MMC5)
				scanline_5(pme, BGwrite);
			}
		}
		if( !(PPUREG[1]&PPU_BGCLIP_BIT) && bLeftClip ) {
			LPBYTE	pScn = pme->lpScanline+8;
			#if 0
			INT i;
 			for( i = 0; i < 8; i++ ) {
				pScn[i] = BGPAL[0];
			}
			#else
			memset(pScn, BGPAL[0], 8);
			#endif
		}
	}

	// Render sprites
	PPUREG[2] &= ~PPU_SPMAX_FLAG;

	// �\�����ԊO�ł���΃L�����Z��
	if( scanline > SCREEN_HEIGHT-1 )
		return;

	if( !(PPUREG[1]&PPU_SPDISP_BIT) ) {
		return;
	}
{
	BYTE	SPwrite[33+1];
	INT	spmax;
	INT	spraddr, sp_y, sp_h;
	BYTE	chr_h, chr_l;
	LPSPRITE sp;

	LPBYTE	pBGw = BGwrite;
	LPBYTE	pSPw = SPwrite;
	LPBYTE	pBit2Rev = pme->Bit2Rev;
	INT i;
	BYTE	SPpat;
	
	// Sprite mask
	INT SPpos; 
	INT SPsft; 
	BYTE	SPmsk; 
	WORD	SPwrt; 
	// Attribute
	LPBYTE	pSPPAL;
	// Ptr
	LPBYTE	pScn;
 
	ZEROMEMORY( SPwrite, sizeof(SPwrite) );

	spmax = 0;
	sp = (LPSPRITE)SPRAM;
	sp_h = (PPUREG[0]&PPU_SP16_BIT)?15:7;

	// Left clip
	if( !(PPUREG[1]&PPU_SPCLIP_BIT) && bLeftClip ) {
		SPwrite[0] = 0xFF;
	}

	for(  i = 0; i < 64; i++, sp++ ) {
		sp_y = scanline - (sp->y+1);
		// �X�L�������C������SPRITE�����݂��邩���`�F�b�N
		if( sp_y != (sp_y & sp_h) )
			continue;

		if( !(PPUREG[0]&PPU_SP16_BIT) ) {
		// 8x8 Sprite
			spraddr = (((INT)PPUREG[0]&PPU_SPTBL_BIT)<<9)+((INT)sp->tile<<4);
			if( !(sp->attr&SP_VMIRROR_BIT) )
				spraddr += sp_y;
			else
				spraddr += 7-sp_y;
		} else {
		// 8x16 Sprite
			spraddr = (((INT)sp->tile&1)<<12)+(((INT)sp->tile&0xFE)<<4);
			if( !(sp->attr&SP_VMIRROR_BIT) )
				spraddr += ((sp_y&8)<<1)+(sp_y&7);
			else
				spraddr += ((~sp_y&8)<<1)+(7-(sp_y&7));
		}
		// Character pattern
		chr_l = PPU_MEM_BANK[spraddr>>10][ spraddr&0x3FF   ];
		chr_h = PPU_MEM_BANK[spraddr>>10][(spraddr&0x3FF)+8];

		// Character latch(For MMC2/MMC4)
		if( pme->bChrLatch ) {
			Mapper_PPU_ChrLatch( pme->nes->mapper, spraddr );
		}

		// pattern mask
		if( sp->attr&SP_HMIRROR_BIT ) {
			chr_l = pBit2Rev[chr_l];
			chr_h = pBit2Rev[chr_h];
		}
		SPpat = chr_l|chr_h;

		// Sprite hitcheck
		if( i == 0 && !(PPUREG[2]&PPU_SPHIT_FLAG) ) {
			INT	BGpos = ((sp->x&0xF8)+((pme->loopy_shift+(sp->x&7))&8))>>3;
			INT	BGsft = 8-((pme->loopy_shift+sp->x)&7);
			BYTE	BGmsk = (((WORD)pBGw[BGpos+0]<<8)|(WORD)pBGw[BGpos+1])>>BGsft;

			if( SPpat & BGmsk ) {
				PPUREG[2] |= PPU_SPHIT_FLAG;
			}
		}
		
		SPpos = sp->x/8;
		SPsft = 8-(sp->x&7);
		SPmsk = (((WORD)pSPw[SPpos+0]<<8)|(WORD)pSPw[SPpos+1])>>SPsft;
		SPwrt = (WORD)SPpat<<SPsft;
		pSPw[SPpos+0] |= SPwrt >> 8;
		pSPw[SPpos+1] |= SPwrt & 0xFF;
		SPpat &= ~SPmsk;

		if( sp->attr&SP_PRIORITY_BIT ) {
		// BG > SP priority
			INT	BGpos = ((sp->x&0xF8)+((pme->loopy_shift+(sp->x&7))&8))>>3;
			INT	BGsft = 8-((pme->loopy_shift+sp->x)&7);
			BYTE	BGmsk = (((WORD)pBGw[BGpos+0]<<8)|(WORD)pBGw[BGpos+1])>>BGsft;

			SPpat &= ~BGmsk;
		}

		// Attribute
		pSPPAL = &SPPAL[(sp->attr&SP_COLOR_BIT)<<2];
		// Ptr
		pScn   = pme->lpScanline+sp->x+8;

		if( !pme->bExtMono ) {
			register INT	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
			register INT	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
			if( SPpat&0x80 ) pScn[0] = pSPPAL[(c1>>6)];
			if( SPpat&0x08 ) pScn[4] = pSPPAL[(c1>>2)&3];
			if( SPpat&0x40 ) pScn[1] = pSPPAL[(c2>>6)];
			if( SPpat&0x04 ) pScn[5] = pSPPAL[(c2>>2)&3];
			if( SPpat&0x20 ) pScn[2] = pSPPAL[(c1>>4)&3];
			if( SPpat&0x02 ) pScn[6] = pSPPAL[c1&3];
			if( SPpat&0x10 ) pScn[3] = pSPPAL[(c2>>4)&3];
			if( SPpat&0x01 ) pScn[7] = pSPPAL[c2&3];
		} else {
		// Monocrome effect (for Final Fantasy)
			BYTE	mono = BGmono[((sp->x&0xF8)+((pme->loopy_shift+(sp->x&7))&8))>>3];

			register INT	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
			register INT	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
			if( SPpat&0x80 ) pScn[0] = pSPPAL[c1>>6]	|mono;
			if( SPpat&0x08 ) pScn[4] = pSPPAL[(c1>>2)&3]	|mono;
			if( SPpat&0x40 ) pScn[1] = pSPPAL[c2>>6]	|mono;
			if( SPpat&0x04 ) pScn[5] = pSPPAL[(c2>>2)&3]	|mono;
			if( SPpat&0x20 ) pScn[2] = pSPPAL[(c1>>4)&3]	|mono;
			if( SPpat&0x02 ) pScn[6] = pSPPAL[c1&3]		|mono;
			if( SPpat&0x10 ) pScn[3] = pSPPAL[(c2>>4)&3]	|mono;
			if( SPpat&0x01 ) pScn[7] = pSPPAL[c2&3]		|mono;
		}

		if( ++spmax > 8-1 ) {
			if( !bMax )
				break;
		}
	}
	if( spmax > 8-1 ) {
		PPUREG[2] |= PPU_SPMAX_FLAG;
	}
}
}

// �X�v���C�g�O���q�b�g���邩���m��Ȃ����C���H
BOOL	PPU_IsSprite0( PPU *pme, INT scanline )
{
	// �X�v���C�gorBG��\���̓L�����Z��(�q�b�g���Ȃ�)
	if( (PPUREG[1]&(PPU_SPDISP_BIT|PPU_BGDISP_BIT)) != (PPU_SPDISP_BIT|PPU_BGDISP_BIT) )
		return	FALSE;

	// ���Ƀq�b�g���Ă�����L�����Z��
	if( PPUREG[2]&PPU_SPHIT_FLAG )
		return	FALSE;

	if( !(PPUREG[0]&PPU_SP16_BIT) ) {
	// 8x8
		if( (scanline < (INT)SPRAM[0]+1) || (scanline > ((INT)SPRAM[0]+7+1)) )
			return	FALSE;
	} else {
	// 8x16
		if( (scanline < (INT)SPRAM[0]+1) || (scanline > ((INT)SPRAM[0]+15+1)) )
			return	FALSE;
	}

	return	TRUE;
}

void	PPU_DummyScanline( PPU *pme, INT scanline )
{
INT	i;
INT	spmax;
INT	sp_h;
LPSPRITE sp;

	PPUREG[2] &= ~PPU_SPMAX_FLAG;

	// �X�v���C�g��\���̓L�����Z��
	if( !(PPUREG[1]&PPU_SPDISP_BIT) )
		return;

	// �\�����ԊO�ł���΃L�����Z��
	if( scanline < 0 || scanline > SCREEN_HEIGHT-1 )
		return;

	sp = (LPSPRITE)SPRAM;
	sp_h = (PPUREG[0]&PPU_SP16_BIT)?15:7;

	spmax = 0;
	// Sprite Max check
	for( i = 0; i < 64; i++, sp++ ) {
		// �X�L�������C������SPRITE�����݂��邩���`�F�b�N
		if( (scanline < (INT)sp->y+1) || (scanline > ((INT)sp->y+sp_h+1)) ) {
			continue;
		}

		if( ++spmax > 8-1 ) {
			PPUREG[2] |= PPU_SPMAX_FLAG;
			break;
		}
	}
}



	WORD	PPU_GetPPUADDR(PPU *pme)	{ return loopy_v; }
	WORD	PPU_GetTILEY(PPU *pme)	{ return pme->loopy_y; }

	// For mapper
	void	PPU_SetExtLatchMode( PPU *pme, BOOL bMode )	{ pme->bExtLatch = bMode; }
	void	PPU_SetChrLatchMode( PPU *pme, BOOL bMode )	{ pme->bChrLatch = bMode; }
	void	PPU_SetExtNameTableMode( PPU *pme, BOOL bMode ) { pme->bExtNameTable = bMode; }
	void	PPU_SetExtMonoMode( PPU *pme, BOOL bMode )	{ pme->bExtMono = bMode; }
	BOOL	PPU_GetExtMonoMode(PPU *pme)	{ return pme->bExtMono; }

	BOOL	PPU_IsDispON(PPU *pme)	{ return PPUREG[1]&(PPU_BGDISP_BIT|PPU_SPDISP_BIT); }
	BOOL	PPU_IsBGON(PPU *pme)	{ return PPUREG[1]&PPU_BGDISP_BIT; }
	BOOL	PPU_IsSPON(PPU *pme)	{ return PPUREG[1]&PPU_SPDISP_BIT; }

	BYTE	PPU_GetBGCOLOR(PPU *pme)	{ return (PPUREG[1]&PPU_BGCOLOR_BIT)>>5; }

	BOOL	PPU_IsBGCLIP(PPU *pme)	{ return PPUREG[1]&PPU_BGCLIP_BIT; }
	BOOL	PPU_IsSPCLIP(PPU *pme)	{ return PPUREG[1]&PPU_SPCLIP_BIT; }

	BOOL	PPU_IsMONOCROME(PPU *pme)	{ return PPUREG[1]&PPU_COLORMODE_BIT; }

	BOOL	PPU_IsVBLANK(PPU *pme)	{ return PPUREG[2]&PPU_VBLANK_FLAG; }
	BOOL	PPU_IsSPHIT(PPU *pme)	{ return PPUREG[2]&PPU_SPHIT_FLAG; }
	BOOL	PPU_IsSPMAX(PPU *pme)	{ return PPUREG[2]&PPU_SPMAX_FLAG; }
	BOOL	PPU_IsPPUWE(PPU *pme)	{ return PPUREG[2]&PPU_WENABLE_FLAG; }


	void	PPU_SetScreenPtr( PPU *pme, LPBYTE lpScn, LPBYTE lpMode ) { pme->lpScreen = lpScn; pme->lpColormode = lpMode; }
	LPBYTE	PPU_GetScreenPtr(PPU *pme)	{ return pme->lpScreen; }

	INT	PPU_GetScanlineNo(PPU *pme)	{ return pme->ScanlineNo; }

	// For VS-Unisystem
	void	PPU_SetVSMode( PPU *pme, BOOL bMode ) { pme->bVSMode = bMode; }
	void	PPU_SetVSSecurity( PPU *pme, BYTE data ) { pme->VSSecurityData = data; }
	void	PPU_SetVSColorMap( PPU *pme, INT nColorMap ) { pme->nVSColorMap = nColorMap; }


