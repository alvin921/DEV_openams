//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Konami VRC6                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "nes.h"
#include "APU_VRC6.h"
#include "state.h"


static INT RectangleRender( APU_VRC6 * pme, APU_VRC6_RECTANGLE* ch );
static INT SawtoothRender( APU_VRC6 * pme, SAWTOOTH* ch );



#define	RECTANGLE_VOL_SHIFT	8
#define	SAWTOOTH_VOL_SHIFT	6

static void	Setup( APU_VRC6 * pme, FLOAT fClock, INT nRate );

static void	Reset( APU_VRC6 * pme, FLOAT fClock, INT nRate )
{
	ZeroMemory( &pme->ch0, sizeof(pme->ch0) );
	ZeroMemory( &pme->ch1, sizeof(pme->ch1) );
	ZeroMemory( &pme->ch2, sizeof(pme->ch2) );

	Setup( pme, fClock, nRate );
}

static void	Setup( APU_VRC6 * pme, FLOAT fClock, INT nRate )
{
	pme->cpu_clock = fClock;
	pme->cycle_rate = (INT)(fClock*65536.0f/(float)nRate);
}

static void	Write( APU_VRC6 * pme, WORD addr, BYTE data )
{
	switch( addr ) {
		// VRC6 CH0 rectangle
		case	0x9000:
			pme->ch0.reg[0] = data;
			pme->ch0.gate     = data&0x80;
			pme->ch0.volume   = data&0x0F;
			pme->ch0.duty_pos = (data>>4)&0x07;
			break;
		case	0x9001:
			pme->ch0.reg[1] = data;
			pme->ch0.freq = INT2FIX( (((pme->ch0.reg[2]&0x0F)<<8)|data)+1 );
			break;
		case	0x9002:
			pme->ch0.reg[2] = data;
			pme->ch0.enable = data&0x80;
			pme->ch0.freq   = INT2FIX( (((data&0x0F)<<8)|pme->ch0.reg[1])+1 );
			break;
		// VRC6 CH1 rectangle
		case	0xA000:
			pme->ch1.reg[0] = data;
			pme->ch1.gate     = data&0x80;
			pme->ch1.volume   = data&0x0F;
			pme->ch1.duty_pos = (data>>4)&0x07;
			break;
		case	0xA001:
			pme->ch1.reg[1] = data;
			pme->ch1.freq = INT2FIX( (((pme->ch1.reg[2]&0x0F)<<8)|data)+1 );
			break;
		case	0xA002:
			pme->ch1.reg[2] = data;
			pme->ch1.enable = data&0x80;
			pme->ch1.freq   = INT2FIX( (((data&0x0F)<<8)|pme->ch1.reg[1])+1 );
			break;
		// VRC6 CH2 sawtooth
		case	0xB000:
			pme->ch2.reg[1] = data;
			pme->ch2.phaseaccum = data&0x3F;
			break;
		case	0xB001:
			pme->ch2.reg[1] = data;
			pme->ch2.freq = INT2FIX( (((pme->ch2.reg[2]&0x0F)<<8)|data)+1 );
			break;
		case	0xB002:
			pme->ch2.reg[2] = data;
			pme->ch2.enable = data&0x80;
			pme->ch2.freq   = INT2FIX( (((data&0x0F)<<8)|pme->ch2.reg[1])+1 );
//			pme->ch2.adder = 0;	// ƒNƒŠƒA‚·‚é‚ÆƒmƒCƒY‚ÌŒ´ˆö‚É‚È‚é
//			pme->ch2.accum = 0;	// ƒNƒŠƒA‚·‚é‚ÆƒmƒCƒY‚ÌŒ´ˆö‚É‚È‚é
			break;
	}
}

static INT	Process( APU_VRC6 * pme, INT channel )
{
	switch( channel ) {
		case	0:
			return	RectangleRender( pme, &pme->ch0 );
			break;
		case	1:
			return	RectangleRender( pme, &pme->ch1 );
			break;
		case	2:
			return	SawtoothRender( pme, &pme->ch2 );
			break;
	}

	return	0;
}

static INT	GetFreq( APU_VRC6 * pme, INT channel )
{
	if( channel == 0 || channel == 1 ) {
		APU_VRC6_RECTANGLE*	ch;
		if( channel == 0 ) ch = &pme->ch0;
		else		   ch = &pme->ch1;
		if( !ch->enable || ch->gate || !ch->volume )
			return	0;
		if( ch->freq < INT2FIX( 8 ) )
			return	0;
		return	(INT)(256.0f*pme->cpu_clock/((FLOAT)FIX2INT(ch->freq)*16.0f));
	}
	if( channel == 2 ) {
		SAWTOOTH*	ch = &pme->ch2;
		if( !ch->enable || !ch->phaseaccum )
			return	0;
		if( ch->freq < INT2FIX( 8 ) )
			return	0;
		return	(INT)(256.0f*pme->cpu_clock/((FLOAT)FIX2INT(ch->freq)*14.0f));
	}

	return	0;
}

static INT	RectangleRender( APU_VRC6 * pme, APU_VRC6_RECTANGLE* ch )
{
	INT	output;
	// Enable?
	if( !ch->enable ) {
		ch->output_vol = 0;
		ch->adder = 0;
		return	ch->output_vol;
	}

	// Digitized output
	if( ch->gate ) {
		ch->output_vol = ch->volume<<RECTANGLE_VOL_SHIFT;
		return	ch->output_vol;
	}

	// ˆê’èˆÈã‚ÌŽü”g”‚Íˆ—‚µ‚È‚¢(–³‘Ê)
	if( ch->freq < INT2FIX( 8 ) ) {
		ch->output_vol = 0;
		return	ch->output_vol;
	}

	ch->phaseacc -= pme->cycle_rate;
	if( ch->phaseacc >= 0 )
		return	ch->output_vol;

	output = ch->volume<<RECTANGLE_VOL_SHIFT;

	if( ch->freq > pme->cycle_rate ) {
	// add 1 step
		ch->phaseacc += ch->freq;
		ch->adder = (ch->adder+1)&0x0F;
		if( ch->adder <= ch->duty_pos )
			ch->output_vol = output;
		else
			ch->output_vol = -output;
	} else {
	// average calculate
		INT	num_times, total;
		num_times = total = 0;
		while( ch->phaseacc < 0 ) {
			ch->phaseacc += ch->freq;
			ch->adder = (ch->adder+1)&0x0F;
			if( ch->adder <= ch->duty_pos )
				total += output;
			else
				total += -output;
			num_times++;
		}
		ch->output_vol = total/num_times;
	}

	return	ch->output_vol;
}

static INT	SawtoothRender( APU_VRC6 * pme, SAWTOOTH* ch )
{
	// Digitized output
	if( !ch->enable ) {
		ch->output_vol = 0;
		return	ch->output_vol;
	}

	// ˆê’èˆÈã‚ÌŽü”g”‚Íˆ—‚µ‚È‚¢(–³‘Ê)
	if( ch->freq < INT2FIX( 9 ) ) {
		return	ch->output_vol;
	}

	ch->phaseacc -= pme->cycle_rate/2;
	if( ch->phaseacc >= 0 )
		return	ch->output_vol;

	if( ch->freq > pme->cycle_rate/2 ) {
	// add 1 step
		ch->phaseacc += ch->freq;
		if( ++ch->adder >= 7 ) {
			ch->adder = 0;
			ch->accum = 0;
		}
		ch->accum += ch->phaseaccum;
		ch->output_vol = ch->accum<<SAWTOOTH_VOL_SHIFT;
	} else {
	// average calculate
		INT	num_times, total;
		num_times = total = 0;
		while( ch->phaseacc < 0 ) {
			ch->phaseacc += ch->freq;
			if( ++ch->adder >= 7 ) {
				ch->adder = 0;
				ch->accum = 0;
			}
			ch->accum += ch->phaseaccum;
			total += ch->accum<<SAWTOOTH_VOL_SHIFT;
			num_times++;
		}
		ch->output_vol = (total/num_times);
	}

	return	ch->output_vol;
}

static INT	GetStateSize(APU_VRC6 * pme)
{
	return	sizeof(pme->ch0) + sizeof(pme->ch1) + sizeof(pme->ch2);
}

static void	SaveState( APU_VRC6 * pme, LPBYTE p )
{
	SETBLOCK( p, &pme->ch0, sizeof(pme->ch0) );
	SETBLOCK( p, &pme->ch1, sizeof(pme->ch1) );
	SETBLOCK( p, &pme->ch2, sizeof(pme->ch2) );
}

static void	LoadState( APU_VRC6 * pme, LPBYTE p )
{
	GETBLOCK( p, &pme->ch0, sizeof(pme->ch0) );
	GETBLOCK( p, &pme->ch1, sizeof(pme->ch1) );
	GETBLOCK( p, &pme->ch2, sizeof(pme->ch2) );
}


static APU_VRC6_vtbl vtbl = {
	NULL
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
	, GetStateSize
	, SaveState
	, LoadState
};

APU_VRC6 * APU_VRC6_new(NES* parent)
{
	APU_VRC6 *pme = (APU_VRC6 *)MALLOC(sizeof(APU_VRC6));

	memset(pme, 0, sizeof(APU_VRC6));

	pme->vtbl = &vtbl;
	
	Reset( pme, APU_CLOCK, 22050 );
	
	return pme;
}


