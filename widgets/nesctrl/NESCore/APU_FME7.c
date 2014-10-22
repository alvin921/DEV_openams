//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      SunSoft FME7                                                    //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "nes.h"
#include "APU_FME7.h"


static void	EnvelopeRender(APU_FME7 *pme);
static void	NoiseRender(APU_FME7 *pme);

static INT ChannelRender( APU_FME7 *pme, APU_FME7_CHANNEL* ch );




#define	CHANNEL_VOL_SHIFT	8

// Envelope tables
static BYTE	envelope_pulse0[] = {
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};
static BYTE	envelope_pulse1[] = {
	      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x00
};
static BYTE	envelope_pulse2[] = {
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x1F
};
static BYTE	envelope_pulse3[] = {
	      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x1F
};
static SBYTE	envstep_pulse[] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 0
};

static BYTE	envelope_sawtooth0[] = {
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};
static BYTE	envelope_sawtooth1[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};
static SBYTE	envstep_sawtooth[] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, -15
};

static BYTE	envelope_triangle0[] = {
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};
static BYTE	envelope_triangle1[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};
static SBYTE	envstep_triangle[] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, -31
};

static LPBYTE	envelope_table[16] = {
	envelope_pulse0,    envelope_pulse0, envelope_pulse0,    envelope_pulse0,
	envelope_pulse1,    envelope_pulse1, envelope_pulse1,    envelope_pulse1,
	envelope_sawtooth0, envelope_pulse0, envelope_triangle0, envelope_pulse2,
	envelope_sawtooth1, envelope_pulse3, envelope_triangle1, envelope_pulse1
};
static LPSBYTE	envstep_table[16] = {
	envstep_pulse,    envstep_pulse, envstep_pulse,    envstep_pulse,
	envstep_pulse,    envstep_pulse, envstep_pulse,    envstep_pulse,
	envstep_sawtooth, envstep_pulse, envstep_triangle, envstep_pulse,
	envstep_sawtooth, envstep_pulse, envstep_triangle, envstep_pulse
};

static void	Setup( APU_FME7 *pme, FLOAT fClock, INT nRate );

static void	Reset( APU_FME7 *pme, FLOAT fClock, INT nRate )
{
	INT	i;
	double	out;

	ZEROMEMORY( &pme->envelope, sizeof(pme->envelope) );
	ZEROMEMORY( &pme->noise, sizeof(pme->noise) );

	for( i = 0; i < 3; i++ ) {
		ZEROMEMORY( &pme->op[i], sizeof(pme->op[i]) );
	}

	pme->envelope.envtbl  = envelope_table[0];
	pme->envelope.envstep = envstep_table[0];

	pme->noise.noiserange = 1;
	pme->noise.noiseout   = 0xFF;

	pme->address = 0;

	// Volume to voltage
	out = 0x1FFF;
	for( i = 31; i > 1; i-- ) {
		pme->vol_table[i] = (INT)(out+0.5);
		out /= 1.188502227;	/* = 10 ^ (1.5/20) = 1.5dB */
	}
	pme->vol_table[1] = 0;
	pme->vol_table[0] = 0;

	Setup( pme, fClock, nRate );
}

static void	Setup( APU_FME7 *pme, FLOAT fClock, INT nRate )
{
	pme->cpu_clock = fClock;
	pme->cycle_rate = (INT)((fClock/16.0f)*(1<<16)/nRate);
}

static void	Write( APU_FME7 *pme, WORD addr, BYTE data )
{
	if( addr == 0xC000 ) {
		pme->address = data;
	} else if( addr == 0xE000 ) {
		BYTE	chaddr = pme->address;
		switch( chaddr ) {
			case	0x00: case	0x01:
			case	0x02: case	0x03:
			case	0x04: case	0x05:
				{
				APU_FME7_CHANNEL* ch = &pme->op[chaddr>>1];
				ch->reg[chaddr&0x01] = data;
				ch->freq = INT2FIX(((INT)(ch->reg[1]&0x0F)<<8)+ch->reg[0]+1);
				}
				break;
			case	0x06:
				pme->noise.freq = INT2FIX((INT)(data&0x1F)+1);
				break;
			case	0x07:
				{
				INT i;
				for( i = 0; i < 3; i++ ) {
					pme->op[i].enable   = data&(1<<i);
					pme->op[i].noise_on = data&(8<<i);
				}
				}
				break;
			case	0x08:
			case	0x09:
			case	0x0A:
				{
				APU_FME7_CHANNEL* ch = &pme->op[chaddr&3];
				ch->reg[2] = data;
				ch->env_on = data & 0x10;
				ch->volume = (data&0x0F)*2;
				}
				break;
			case	0x0B:
			case	0x0C:
				pme->envelope.reg[chaddr-0x0B] = data;
				pme->envelope.freq = INT2FIX(((INT)(pme->envelope.reg[1]&0x0F)<<8)+pme->envelope.reg[0]+1);
				break;
			case	0x0D:
				pme->envelope.envtbl  = envelope_table[data&0x0F];
				pme->envelope.envstep = envstep_table [data&0x0F];
				pme->envelope.envadr  = 0;
				break;
		}
	}
}

static INT	Process( APU_FME7 *pme, INT channel )
{
	if( channel < 3 ) {
		return	ChannelRender( pme, &pme->op[channel] );
	} else if( channel == 3 ) {
		// •K‚¸ch3‚ð1‰ñŒÄ‚ñ‚Å‚©‚çch0-2‚ðŒÄ‚ÔŽ–
		EnvelopeRender(pme);
		NoiseRender(pme);
	}

	return	0;
}

static INT	GetFreq( APU_FME7 *pme, INT channel )
{
	if( channel < 3 ) {
		APU_FME7_CHANNEL* ch = &pme->op[channel];

		if( ch->enable || !ch->freq )
			return	0;
		if( ch->env_on ) {
			if( !pme->envelope.volume )
				return	0;
		} else {
			if( !ch->volume )
				return	0;
		}

		return	(INT)(256.0f*pme->cpu_clock/((FLOAT)FIX2INT(ch->freq)*16.0f));
	}

	return	0;
}

static void	EnvelopeRender(APU_FME7 *pme)
{
	if( !pme->envelope.freq )
		return;
	pme->envelope.phaseacc -= pme->cycle_rate;
	if( pme->envelope.phaseacc >= 0 )
		return;
	while( pme->envelope.phaseacc < 0 ) {
		pme->envelope.phaseacc += pme->envelope.freq;
		pme->envelope.envadr += pme->envelope.envstep[pme->envelope.envadr];
	}
	pme->envelope.volume = pme->envelope.envtbl[pme->envelope.envadr];
}

static void	NoiseRender(APU_FME7 *pme)
{
	if( !pme->noise.freq )
		return;
	pme->noise.phaseacc -= pme->cycle_rate;
	if( pme->noise.phaseacc >= 0 )
		return;
	while( pme->noise.phaseacc < 0 ) {
		pme->noise.phaseacc += pme->noise.freq;
		if( (pme->noise.noiserange+1) & 0x02 )
			pme->noise.noiseout = ~pme->noise.noiseout;
		if( pme->noise.noiserange & 0x01 )
			pme->noise.noiserange ^= 0x28000;
		pme->noise.noiserange >>= 1;
	}
}

static INT	ChannelRender( APU_FME7 *pme, APU_FME7_CHANNEL* ch )
{
INT	output, volume;

	if( ch->enable )
		return	0;
	if( !ch->freq )
		return	0;

	ch->phaseacc -= pme->cycle_rate;
	while( ch->phaseacc < 0 ) {
		ch->phaseacc += ch->freq;
		ch->adder++;
	}

	output = volume = 0;
	volume = ch->env_on?pme->vol_table[pme->envelope.volume]:pme->vol_table[ch->volume+1];

	if( ch->adder & 0x01 ) {
		output += volume;
	} else {
		output -= volume;
	}
	if( !ch->noise_on ) {
		if( pme->noise.noiseout )
			output += volume;
		else
			output -= volume;
	}

	ch->output_vol = output;

	return	ch->output_vol;
}

static INT	GetStateSize(APU_FME7 *pme)
{
	return	sizeof(BYTE) + sizeof(pme->envelope) + sizeof(pme->noise) + 3*sizeof(pme->op);
}

static void	SaveState( APU_FME7 *pme, LPBYTE p )
{
	SETBYTE( p, pme->address );

	SETBLOCK( p, &pme->envelope, sizeof(pme->envelope) );
	SETBLOCK( p, &pme->noise, sizeof(pme->noise) );
	SETBLOCK( p, pme->op, 3*sizeof(pme->op) );
}

static void	LoadState( APU_FME7 *pme, LPBYTE p )
{
	GETBYTE( p, pme->address );

	GETBLOCK( p, &pme->envelope, sizeof(pme->envelope) );
	GETBLOCK( p, &pme->noise, sizeof(pme->noise) );
	GETBLOCK( p, pme->op, 3*sizeof(pme->op) );
}


static APU_FME7_vtbl vtbl = {
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

APU_FME7 * APU_FME7_new(NES* parent)
{
	APU_FME7 *pme = (APU_FME7 *)MALLOC(sizeof(APU_FME7));

	memset(pme, 0, sizeof(APU_FME7));

	pme->vtbl = &vtbl;
	
	Reset( pme, APU_CLOCK, 22050 );
	
	return pme;
}





