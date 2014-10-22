//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Namcot N106                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "nes.h"
#include "APU_N106.h"
#include "state.h"



static INT ChannelRender( APU_N106 *pme, CHANNEL* ch );



#define	CHANNEL_VOL_SHIFT	6

static void	Setup( APU_N106 *pme, FLOAT fClock, INT nRate );


static void	Reset( APU_N106 *pme, FLOAT fClock, INT nRate )
{
	INT i;
	for( i = 0; i < 8; i++ ) {
		ZEROMEMORY( &pme->op[i], sizeof(pme->op[i]) );
		pme->op[i].tonelen = 0x10<<18;
	}

	pme->address = 0;
	pme->addrinc = 1;
	pme->channel_use = 8;

	Setup( pme, fClock, nRate );

	// TONE‚Ì‰Šú‰»‚Í‚µ‚È‚¢...
}

static void	Setup( APU_N106 *pme, FLOAT fClock, INT nRate )
{
	pme->cpu_clock = fClock;
	pme->cycle_rate = (DWORD)(pme->cpu_clock*12.0f*(1<<20)/(45.0f*nRate));
}

static void	Write( APU_N106 *pme, WORD addr, BYTE data )
{
	if( addr == 0x4800 ) {
//		pme->tone[pme->address*2+0] = (INT)(data&0x0F);
//		pme->tone[pme->address*2+1] = (INT)(data  >>4);
		pme->tone[pme->address*2+0] = data&0x0F;
		pme->tone[pme->address*2+1] = data>>4;

		if( pme->address >= 0x40 ) {
			INT	no = (pme->address-0x40)>>3;
			DWORD	tonelen;
			CHANNEL* ch = &pme->op[no];

			switch( pme->address & 7 ) {
				case	0x00:
					ch->freq = (ch->freq&~0x000000FF)|(DWORD)data;
					break;
				case	0x02:
					ch->freq = (ch->freq&~0x0000FF00)|((DWORD)data<<8);
					break;
				case	0x04:
					ch->freq = (ch->freq&~0x00030000)|(((DWORD)data&0x03)<<16);
					tonelen = (0x20-(data&0x1c))<<18;
					ch->databuf = (data&0x1c)>>2;
					if( ch->tonelen != tonelen ) {
						ch->tonelen = tonelen;
						ch->phase = 0;
					}
					break;
				case	0x06:
					ch->toneadr = data;
					break;
				case	0x07:
					ch->vol = data&0x0f;
					ch->volupdate = 0xFF;
					if( no == 7 )
						pme->channel_use = ((data>>4)&0x07)+1;
					break;
			}
		}

		if( pme->addrinc ) {
			pme->address = (pme->address+1)&0x7f;
		}
	} else if( addr == 0xF800 ) {
		pme->address = data&0x7F;
		pme->addrinc = data&0x80;
	}
}

static BYTE	Read( APU_N106 *pme, WORD addr )
{
	// $4800 dummy read!!
	if( addr == 0x0000 ) {
		if( pme->addrinc ) {
			pme->address = (pme->address+1)&0x7F;
		}
	}

	return	(BYTE)(addr>>8);
}

static INT	Process( APU_N106 *pme, INT channel )
{
	if( channel >= (8-pme->channel_use) && channel < 8 ) {
		return	ChannelRender( pme, &pme->op[channel] );
	}

	return	0;
}

static INT	GetFreq( APU_N106 *pme, INT channel )
{
	CHANNEL* ch;
	INT	temp;
	if( channel < 8 ) {
		channel &= 7;
		if( channel < (8-pme->channel_use) )
			return	0;

		ch = &pme->op[channel&0x07];
		if( !ch->freq || !ch->vol )
			return	0;
		temp = pme->channel_use*(8-ch->databuf)*4*45;
		if( !temp )
			return	0;
		return	(INT)(256.0*(double)pme->cpu_clock*12.0*ch->freq/((double)0x40000*temp));
	}

	return	0;
}

static INT	ChannelRender( APU_N106 *pme, CHANNEL* ch )
{
DWORD	phasespd = pme->channel_use<<20;

	ch->phaseacc -= pme->cycle_rate;
	if( ch->phaseacc >= 0 ) {
		if( ch->volupdate ) {
			ch->output = ((INT)pme->tone[((ch->phase>>18)+ch->toneadr)&0xFF]*ch->vol)<<CHANNEL_VOL_SHIFT;
			ch->volupdate = 0;
		}
		return	ch->output;
	}

	while( ch->phaseacc < 0 ) {
		ch->phaseacc += phasespd;
		ch->phase += ch->freq;
	}
	while( ch->tonelen && (ch->phase >= ch->tonelen)) {
		ch->phase -= ch->tonelen;
	}

	ch->output = ((INT)pme->tone[((ch->phase>>18)+ch->toneadr)&0xFF]*ch->vol)<<CHANNEL_VOL_SHIFT;

	return	ch->output;
}

static INT	GetStateSize(APU_N106 *pme)
{
	return	3*sizeof(BYTE) + 8*sizeof(CHANNEL) + sizeof(pme->tone);
}

static void	SaveState( APU_N106 *pme, LPBYTE p )
{
	SETBYTE( p, pme->addrinc );
	SETBYTE( p, pme->address );
	SETBYTE( p, pme->channel_use );

	SETBLOCK( p, pme->op, sizeof(pme->op) );
	SETBLOCK( p, pme->tone, sizeof(pme->tone) );
}

static void	LoadState( APU_N106 *pme, LPBYTE p )
{
	GETBYTE( p, pme->addrinc );
	GETBYTE( p, pme->address );
	GETBYTE( p, pme->channel_use );

	GETBLOCK( p, pme->op, sizeof(pme->op) );
	GETBLOCK( p, pme->tone, sizeof(pme->tone) );
}

static APU_N106_vtbl vtbl = {
	NULL
	, Reset
	, Setup
	, Process
	, Write
	, Read
	, NULL
	, NULL
	, NULL
	, NULL
	, GetFreq
	, GetStateSize
	, SaveState
	, LoadState
};

APU_N106 * APU_N106_new(NES* parent)
{
	APU_N106 *pme = (APU_N106 *)MALLOC(sizeof(APU_N106));

	memset(pme, 0, sizeof(APU_N106));

	pme->vtbl = &vtbl;
	
	// ‰¼Ý’è
	pme->cpu_clock = APU_CLOCK;
	pme->cycle_rate = (DWORD)(pme->cpu_clock*12.0f*(1<<20)/(45.0f*22050.0f));
	
	return pme;
}

