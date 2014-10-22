//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Nintendo MMC5                                                   //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#include "nes.h"
#include "APU_MMC5.h"
#include "state.h"


static INT RectangleRender( APU_MMC5 *pme, APU_MMC5_RECTANGLE * ch );




#define	RECTANGLE_VOL_SHIFT	8
#define	DAOUT_VOL_SHIFT		6

static const INT	vbl_length[32] = {
    5, 127,   10,   1,   19,   2,   40,   3,
   80,   4,   30,   5,    7,   6,   13,   7,
    6,   8,   12,   9,   24,  10,   48,  11,
   96,  12,   36,  13,    8,  14,   16,  15
};

static const INT	duty_lut[4] = {
	 2,  4,  8, 12
};

static INT	decay_lut[16];
static INT	vbl_lut[32];

static void	Setup( APU_MMC5 *pme, FLOAT fClock, INT nRate );
static void	Write( APU_MMC5 *pme, WORD addr, BYTE data );

static void	Reset( APU_MMC5 *pme, FLOAT fClock, INT nRate )
{
	WORD addr;
	ZeroMemory( &pme->ch0, sizeof(pme->ch0) );
	ZeroMemory( &pme->ch1, sizeof(pme->ch1) );

	pme->reg5010 = pme->reg5011 = pme->reg5015 = 0;

	pme->sync_reg5015 = 0;
	pme->FrameCycle = 0;

	Setup( pme, fClock, nRate );

	for( addr = 0x5000; addr <= 0x5015; addr++ ) {
		Write( pme, addr, 0 );
	}
}

static void	Setup( APU_MMC5 *pme, FLOAT fClock, INT nRate )
{
	INT	i;
	INT	samples = (INT)((float)nRate/60.0f);

	pme->cpu_clock = fClock;
	pme->cycle_rate = (INT)(fClock*65536.0f/(float)nRate);

	// Create Tables
	for( i = 0; i < 16; i++ )
		decay_lut[i] = (i+1)*samples*5;
	for( i = 0; i < 32; i++ )
		vbl_lut[i] = vbl_length[i]*samples*5;
}

static void	Write( APU_MMC5 *pme, WORD addr, BYTE data )
{
//DebugOut( "$%04X:%02X\n", addr, data );
	switch( addr ) {
		// MMC5 CH0 rectangle
		case	0x5000:
			pme->ch0.reg[0] = data;
			pme->ch0.volume         = data&0x0F;
			pme->ch0.holdnote       = data&0x20;
			pme->ch0.fixed_envelope = data&0x10;
			pme->ch0.env_decay      = decay_lut[data&0x0F];
			pme->ch0.duty_flip      = duty_lut[data>>6];
			break;
		case	0x5001:
			pme->ch0.reg[1] = data;
			break;
		case	0x5002:
			pme->ch0.reg[2] = data;
			pme->ch0.freq = INT2FIX( ((pme->ch0.reg[3]&0x07)<<8)+data+1 );
			break;
		case	0x5003:
			pme->ch0.reg[3] = data;
			pme->ch0.vbl_length = vbl_lut[data>>3];
			pme->ch0.env_vol = 0;
			pme->ch0.freq = INT2FIX( ((data&0x07)<<8)+pme->ch0.reg[2]+1 );
			if( pme->reg5015 & 0x01 )
				pme->ch0.enable = 0xFF;
			break;
		// MMC5 CH1 rectangle
		case	0x5004:
			pme->ch1.reg[0] = data;
			pme->ch1.volume         = data&0x0F;
			pme->ch1.holdnote       = data&0x20;
			pme->ch1.fixed_envelope = data&0x10;
			pme->ch1.env_decay      = decay_lut[data&0x0F];
			pme->ch1.duty_flip      = duty_lut[data>>6];
			break;
		case	0x5005:
			pme->ch1.reg[1] = data;
			break;
		case	0x5006:
			pme->ch1.reg[2] = data;
			pme->ch1.freq = INT2FIX( ((pme->ch1.reg[3]&0x07)<<8)+data+1 );
			break;
		case	0x5007:
			pme->ch1.reg[3] = data;
			pme->ch1.vbl_length = vbl_lut[data>>3];
			pme->ch1.env_vol = 0;
			pme->ch1.freq = INT2FIX( ((data&0x07)<<8)+pme->ch1.reg[2]+1 );
			if( pme->reg5015 & 0x02 )
				pme->ch1.enable = 0xFF;
			break;
		case	0x5010:
			pme->reg5010 = data;
			break;
		case	0x5011:
			pme->reg5011 = data;
			break;
		case	0x5012:
		case	0x5013:
		case	0x5014:
			break;
		case	0x5015:
			pme->reg5015 = data;
			if( pme->reg5015 & 0x01 ) {
				pme->ch0.enable = 0xFF;
			} else {
				pme->ch0.enable = 0;
				pme->ch0.vbl_length = 0;
			}
			if( pme->reg5015 & 0x02 ) {
				pme->ch1.enable = 0xFF;
			} else {
				pme->ch1.enable = 0;
				pme->ch1.vbl_length = 0;
			}
			break;
	}
}

static void	SyncWrite( APU_MMC5 *pme, WORD addr, BYTE data )
{
	switch( addr ) {
		// MMC5 CH0 rectangle
		case	0x5000:
			pme->sch0.reg[0] = data;
			pme->sch0.holdnote = data&0x20;
			break;
		case	0x5001:
		case	0x5002:
			pme->sch0.reg[addr&3] = data;
			break;
		case	0x5003:
			pme->sch0.reg[3] = data;
			pme->sch0.vbl_length = vbl_length[data>>3];
			if( pme->sync_reg5015 & 0x01 )
				pme->sch0.enable = 0xFF;
			break;
		// MMC5 CH1 rectangle
		case	0x5004:
			pme->sch1.reg[0] = data;
			pme->sch1.holdnote  = data&0x20;
			break;
		case	0x5005:
		case	0x5006:
			pme->sch1.reg[addr&3] = data;
			break;
		case	0x5007:
			pme->sch1.reg[3] = data;
			pme->sch1.vbl_length = vbl_length[data>>3];
			if( pme->sync_reg5015 & 0x02 )
				pme->sch1.enable = 0xFF;
			break;
		case	0x5010:
		case	0x5011:
		case	0x5012:
		case	0x5013:
		case	0x5014:
			break;
		case	0x5015:
			pme->sync_reg5015 = data;
			if( pme->sync_reg5015 & 0x01 ) {
				pme->sch0.enable = 0xFF;
			} else {
				pme->sch0.enable = 0;
				pme->sch0.vbl_length = 0;
			}
			if( pme->sync_reg5015 & 0x02 ) {
				pme->sch1.enable = 0xFF;
			} else {
				pme->sch1.enable = 0;
				pme->sch1.vbl_length = 0;
			}
			break;
	}
}

static BYTE	SyncRead( APU_MMC5 *pme, WORD addr )
{
BYTE	data = 0;

	if( addr == 0x5015 ) {
		if( pme->sch0.enable && pme->sch0.vbl_length > 0 ) data |= (1<<0);
		if( pme->sch1.enable && pme->sch1.vbl_length > 0 ) data |= (1<<1);
	}

	return	data;
}

static BOOL	Sync( APU_MMC5 *pme, INT cycles )
{
	pme->FrameCycle += cycles;
	if( pme->FrameCycle >= 7457*5/2 ) {
		pme->FrameCycle -= 7457*5/2;

		if( pme->sch0.enable && !pme->sch0.holdnote ) {
			if( pme->sch0.vbl_length ) {
				pme->sch0.vbl_length--;
			}
		}
		if( pme->sch1.enable && !pme->sch1.holdnote ) {
			if( pme->sch1.vbl_length ) {
				pme->sch1.vbl_length--;
			}
		}
	}

	return	FALSE;
}

static INT	Process( APU_MMC5 *pme, INT channel )
{
	switch( channel ) {
		case	0:
			return	RectangleRender( pme, &pme->ch0 );
			break;
		case	1:
			return	RectangleRender( pme, &pme->ch1 );
			break;
		case	2:
			return	(INT)pme->reg5011 << DAOUT_VOL_SHIFT;
			break;
	}

	return	0;
}

static INT	GetFreq( APU_MMC5 *pme, INT channel )
{
	if( channel == 0 || channel == 1 ) {
		APU_MMC5_RECTANGLE*	ch;
		if( channel == 0 ) ch = &pme->ch0;
		else		   ch = &pme->ch1;

		if( !ch->enable || ch->vbl_length <= 0 )
			return	0;
		if( ch->freq < INT2FIX( 8 ) )
			return	0;
		if( ch->fixed_envelope ) {
			if( !ch->volume )
				return	0;
		} else {
			if( !(0x0F-ch->env_vol) )
				return	0;
		}

		return	(INT)(256.0f*pme->cpu_clock/((FLOAT)FIX2INT(ch->freq)*16.0f));
	}

	return	0;
}

static INT	RectangleRender( APU_MMC5 *pme, APU_MMC5_RECTANGLE* ch )
{
	INT	volume, output;
	if( !ch->enable || ch->vbl_length <= 0 )
		return	0;

	// vbl length counter
	if( !ch->holdnote )
		ch->vbl_length -= 5;

	// envelope unit
	ch->env_phase -= 5*4;
	while( ch->env_phase < 0 ) {
		ch->env_phase += ch->env_decay;
		if( ch->holdnote )
			ch->env_vol = (ch->env_vol+1)&0x0F;
		else if( ch->env_vol < 0x0F )
			ch->env_vol++;
	}

	if( ch->freq < INT2FIX( 8 ) )
		return	0;

	if( ch->fixed_envelope )
		volume = (INT)ch->volume;
	else
		volume = (INT)(0x0F-ch->env_vol);

	output = volume<<RECTANGLE_VOL_SHIFT;

	ch->phaseacc -= pme->cycle_rate;
	if( ch->phaseacc >= 0 ) {
		if( ch->adder < ch->duty_flip )
			ch->output_vol = output;
		else
			ch->output_vol = -output;
		return	ch->output_vol;
	}

	if( ch->freq > pme->cycle_rate ) {
		ch->phaseacc += ch->freq;
		ch->adder = (ch->adder+1)&0x0F;
		if( ch->adder < ch->duty_flip )
			ch->output_vol = output;
		else
			ch->output_vol = -output;
	} else {
	// ‰Ád•½‹Ï
		INT	num_times, total;
		num_times = total = 0;
		while( ch->phaseacc < 0 ) {
			ch->phaseacc += ch->freq;
			ch->adder = (ch->adder+1)&0x0F;
			if( ch->adder < ch->duty_flip )
				total += output;
			else
				total -= output;
			num_times++;
		}
		ch->output_vol = total/num_times;
	}

	return	ch->output_vol;
}

static INT	GetStateSize(APU_MMC5 *pme)
{
	return	3*sizeof(BYTE) + sizeof(pme->ch0) + sizeof(pme->ch1) + sizeof(pme->sch0) + sizeof(pme->sch1);
}

static void	SaveState( APU_MMC5 *pme, LPBYTE p )
{
	SETBYTE( p, pme->reg5010 );
	SETBYTE( p, pme->reg5011 );
	SETBYTE( p, pme->reg5015 );

	SETBLOCK( p, &pme->ch0, sizeof(pme->ch0) );
	SETBLOCK( p, &pme->ch1, sizeof(pme->ch1) );

	SETBYTE( p, pme->sync_reg5015 );
	SETBLOCK( p, &pme->sch0, sizeof(pme->sch0) );
	SETBLOCK( p, &pme->sch1, sizeof(pme->sch1) );
}

static void	LoadState( APU_MMC5 *pme, LPBYTE p )
{
	GETBYTE( p, pme->reg5010 );
	GETBYTE( p, pme->reg5011 );
	GETBYTE( p, pme->reg5015 );

	GETBLOCK( p, &pme->ch0, sizeof(pme->ch0) );
	GETBLOCK( p, &pme->ch1, sizeof(pme->ch1) );

	GETBYTE( p, pme->sync_reg5015 );
	GETBLOCK( p, &pme->sch0, sizeof(pme->sch0) );
	GETBLOCK( p, &pme->sch1, sizeof(pme->sch1) );
}

static APU_MMC5_vtbl vtbl = {
	NULL
	, Reset
	, Setup
	, Process
	, Write
	, NULL
	, SyncWrite
	, SyncRead
	, NULL
	, Sync
	, GetFreq
	, GetStateSize
	, SaveState
	, LoadState
};

APU_MMC5 * APU_MMC5_new(NES* parent)
{
	APU_MMC5 *pme = (APU_MMC5 *)MALLOC(sizeof(APU_MMC5));

	memset(pme, 0, sizeof(APU_MMC5));

	pme->vtbl = &vtbl;
	
	Reset( pme, APU_CLOCK, 22050 );
	
	return pme;
}


