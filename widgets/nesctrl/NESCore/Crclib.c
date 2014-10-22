//
// CRCチェック用ライブラリクラス
//
#include "Crclib.h"

static BOOL	m_Init = FALSE;
static BOOL	m_InitRev = FALSE;
static DWORD	m_CrcTable[ G_U8_MAX+1 ];
static DWORD	m_CrcTableRev[ G_U8_MAX+1 ];

#define CRCPOLY1 0x04C11DB7UL
#define CRCPOLY2 0xEDB88320UL

#ifndef CHAR_BIT
#define CHAR_BIT	8
#endif

void	MakeTable()
{
INT	i, j;
DWORD	r;

	for( i = 0; i <= G_U8_MAX; i++ ) {
		r = (DWORD)i << (32 - CHAR_BIT);
		for( j = 0; j < CHAR_BIT; j++ ) {
			if( r & 0x80000000UL ) r = (r << 1) ^ CRCPOLY1;
			else                   r <<= 1;
		}
		m_CrcTable[i] = r & 0xFFFFFFFFUL;
	}
}

DWORD	Crc( INT size, LPBYTE c )
{
	DWORD	r = 0xFFFFFFFFUL;
	if( !m_Init ) {
		MakeTable();
		m_Init = TRUE;
	}

	while( --size >= 0 ) {
		r = (r << CHAR_BIT) ^ m_CrcTable[(BYTE)(r >> (32 - CHAR_BIT)) ^ *c++];
	}
	return ~r & 0xFFFFFFFFUL;
}

void	MakeTableRev()
{
INT	i, j;
DWORD	r;

	for( i = 0; i <= G_U8_MAX; i++ ) {
		r = i;
		for( j = 0; j < CHAR_BIT; j++ ) {
			if( r & 1 ) r = (r >> 1) ^ CRCPOLY2;
			else        r >>= 1;
		}
		m_CrcTableRev[i] = r;
	}
}

DWORD	CrcRev( INT size, LPBYTE c )
{
	DWORD	r = 0xFFFFFFFFUL;
	if( !m_InitRev ) {
		MakeTableRev();
		m_InitRev = TRUE;
	}

	while( --size >= 0 ) {
		r = (r >> CHAR_BIT) ^ m_CrcTableRev[(BYTE)r ^ *c++];
	}
	return r ^ 0xFFFFFFFFUL;
}

