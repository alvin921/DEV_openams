//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      FDS sound                                                       //
//                                                           Norix      //
//                                               written     2002/06/30 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#include "nes.h"
#include "APU_FDS.h"
#include "state.h"

static void	WriteSub( APU_FDS *pme, WORD addr, BYTE data, FDSSOUND* ch, double rate );

static void	Reset( APU_FDS *pme, FLOAT fClock, INT nRate )
{
	ZEROMEMORY( &pme->fds, sizeof(pme->fds) );
	ZEROMEMORY( &pme->fds_sync, sizeof(pme->fds) );

	pme->sampling_rate = nRate;
}

static void	Setup( APU_FDS *pme, FLOAT fClock, INT nRate )
{
	pme->sampling_rate = nRate;
}

static void	WriteSub( APU_FDS *pme, WORD addr, BYTE data, FDSSOUND* ch, double rate )
{
	if( addr < 0x4040 || addr > 0x40BF )
		return;

	ch->reg[addr-0x4040] = data;
	if( addr >= 0x4040 && addr <= 0x407F ) {
		if( ch->wave_setup ) {
			ch->main_wavetable[addr-0x4040] = 0x20-((INT)data&0x3F);
		}
	} else {
		int i;
		switch( addr ) {
			case	0x4080:	// Volume Envelope
				ch->volenv_mode = data>>6;
				if( data&0x80 ) {
					ch->volenv_gain = data&0x3F;

					// 即時反映
					if( !ch->main_addr ) {
						ch->now_volume = (ch->volenv_gain<0x21)?ch->volenv_gain:0x20;
					}
				}
				// エンベロープ1段階の演算
				ch->volenv_decay    = data&0x3F;
				ch->volenv_phaseacc = (double)ch->envelope_speed * (double)(ch->volenv_decay+1) * rate / (232.0*960.0);
				break;

			case	0x4082:	// Main Frequency(Low)
				ch->main_frequency = (ch->main_frequency&~0x00FF)|(INT)data;
				break;
			case	0x4083:	// Main Frequency(High)
				ch->main_enable     = (~data)&(1<<7);
				ch->envelope_enable = (~data)&(1<<6);
				if( !ch->main_enable ) {
					ch->main_addr = 0;
					ch->now_volume = (ch->volenv_gain<0x21)?ch->volenv_gain:0x20;
				}
//				ch->main_frequency  = (ch->main_frequency&0x00FF)|(((INT)data&0x3F)<<8);
				ch->main_frequency  = (ch->main_frequency&0x00FF)|(((INT)data&0x0F)<<8);
				break;

			case	0x4084:	// Sweep Envelope
				ch->swpenv_mode = data>>6;
				if( data&0x80 ) {
					ch->swpenv_gain = data&0x3F;
				}
				// エンベロープ1段階の演算
				ch->swpenv_decay    = data&0x3F;
				ch->swpenv_phaseacc = (double)ch->envelope_speed * (double)(ch->swpenv_decay+1) * rate / (232.0*960.0);
				break;

			case	0x4085:	// Sweep Bias
				if( data&0x40 ) ch->sweep_bias = (data&0x3f)-0x40;
				else		ch->sweep_bias =  data&0x3f;
				ch->lfo_addr = 0;
				break;

			case	0x4086:	// Effector(LFO) Frequency(Low)
				ch->lfo_frequency = (ch->lfo_frequency&(~0x00FF))|(INT)data;
				break;
			case	0x4087:	// Effector(LFO) Frequency(High)
				ch->lfo_enable    = (~data&0x80);
				ch->lfo_frequency = (ch->lfo_frequency&0x00FF)|(((INT)data&0x0F)<<8);
				break;

			case	0x4088:	// Effector(LFO) wavetable
				if( !ch->lfo_enable ) {
					// FIFO?
					for( i = 0; i < 31; i++ ) {
						ch->lfo_wavetable[i*2+0] = ch->lfo_wavetable[(i+1)*2+0];
						ch->lfo_wavetable[i*2+1] = ch->lfo_wavetable[(i+1)*2+1];
					}
					ch->lfo_wavetable[31*2+0] = data&0x07;
					ch->lfo_wavetable[31*2+1] = data&0x07;
				}
				break;

			case	0x4089:	// Sound control
				{
				INT	tbl[] = {30, 20, 15, 12};
				ch->master_volume = tbl[data&3];
				ch->wave_setup    = data&0x80;
				}
				break;

			case	0x408A:	// Sound control 2
				ch->envelope_speed = data;
				break;

			default:
				break;
		}
	}
}

// APUレンダラ側から呼ばれる
static void	Write( APU_FDS *pme, WORD addr, BYTE data )
{
	// サンプリングレート基準
	WriteSub( pme, addr, data, &pme->fds, (double)pme->sampling_rate );
}

static BYTE	Read ( APU_FDS *pme, WORD addr )
{
BYTE	data = addr>>8;

	if( addr >= 0x4040 && addr <= 0x407F ) {
		data = pme->fds.main_wavetable[addr&0x3F] | 0x40;
	} else
	if( addr == 0x4090 ) {
		data = (pme->fds.volenv_gain&0x3F)|0x40;
	} else
	if( addr == 0x4092 ) {
		data = (pme->fds.swpenv_gain&0x3F)|0x40;
	}

	return	data;
}

static INT	Process( APU_FDS *pme, INT channel )
{
	INT	sub_freq;
	INT	output;

	// Envelope unit
	if( pme->fds.envelope_enable && pme->fds.envelope_speed ) {
		// Volume envelope
		if( pme->fds.volenv_mode < 2 ) {
			double	decay = ((double)pme->fds.envelope_speed * (double)(pme->fds.volenv_decay+1) * (double)pme->sampling_rate) / (232.0*960.0);
			pme->fds.volenv_phaseacc -= 1.0;
			while( pme->fds.volenv_phaseacc < 0.0 ) {
				pme->fds.volenv_phaseacc += decay;

				if( pme->fds.volenv_mode == 0 ) {
				// 減少モード
					if( pme->fds.volenv_gain )
						pme->fds.volenv_gain--;
				} else
				if( pme->fds.volenv_mode == 1 ) {
					if( pme->fds.volenv_gain < 0x20 )
						pme->fds.volenv_gain++;
				}
			}
		}

		// Sweep envelope
		if( pme->fds.swpenv_mode < 2 ) {
			double	decay = ((double)pme->fds.envelope_speed * (double)(pme->fds.swpenv_decay+1) * (double)pme->sampling_rate) / (232.0*960.0);
			pme->fds.swpenv_phaseacc -= 1.0;
			while( pme->fds.swpenv_phaseacc < 0.0 ) {
				pme->fds.swpenv_phaseacc += decay;

				if( pme->fds.swpenv_mode == 0 ) {
				// 減少モード
					if( pme->fds.swpenv_gain )
						pme->fds.swpenv_gain--;
				} else
				if( pme->fds.swpenv_mode == 1 ) {
					if( pme->fds.swpenv_gain < 0x20 )
						pme->fds.swpenv_gain++;
				}
			}
		}
	}

	// Effector(LFO) unit
	sub_freq = 0;
//	if( pme->fds.lfo_enable && pme->fds.envelope_speed && pme->fds.lfo_frequency ) {
	if( pme->fds.lfo_enable ) {
		INT	sub_multi;
		if (pme->fds.lfo_frequency)
		{
			static int tbl[8] = { 0, 1, 2, 4, 0, -4, -2, -1};

			pme->fds.lfo_phaseacc -= (1789772.5*(double)pme->fds.lfo_frequency)/65536.0;
			while( pme->fds.lfo_phaseacc < 0.0 ) {
				pme->fds.lfo_phaseacc += (double)pme->sampling_rate;

				if( pme->fds.lfo_wavetable[pme->fds.lfo_addr] == 4 )
					pme->fds.sweep_bias = 0;
				else
					pme->fds.sweep_bias += tbl[pme->fds.lfo_wavetable[pme->fds.lfo_addr]];

				pme->fds.lfo_addr = (pme->fds.lfo_addr+1)&63;
			}
		}

		if( pme->fds.sweep_bias > 63 )
			pme->fds.sweep_bias -= 128;
		else if( pme->fds.sweep_bias < -64 )
			pme->fds.sweep_bias += 128;

		sub_multi = pme->fds.sweep_bias * pme->fds.swpenv_gain;

		if( sub_multi & 0x0F ) {
			// 16で割り切れない場合
			sub_multi = (sub_multi / 16);
			if( pme->fds.sweep_bias >= 0 )
				sub_multi += 2;    // 正の場合
			else
				sub_multi -= 1;    // 負の場合
		} else {
			// 16で割り切れる場合
			sub_multi = (sub_multi / 16);
		}
		// 193を超えると-258する(-64へラップ)
		if( sub_multi > 193 )
			sub_multi -= 258;
		// -64を下回ると+256する(192へラップ)
	        if( sub_multi < -64 )
			sub_multi += 256;

		sub_freq = (pme->fds.main_frequency) * sub_multi / 64;
	}

	// Main unit
	output = 0;
	if( pme->fds.main_enable && pme->fds.main_frequency && !pme->fds.wave_setup ) {
		INT	freq;
		INT	main_addr_old = pme->fds.main_addr;

		freq = (pme->fds.main_frequency+sub_freq)*1789772.5/65536.0;

		pme->fds.main_addr = (pme->fds.main_addr+freq+64*pme->sampling_rate)%(64*pme->sampling_rate);

		// 1周期を超えたらボリューム更新
		if( main_addr_old > pme->fds.main_addr )
			pme->fds.now_volume = (pme->fds.volenv_gain<0x21)?pme->fds.volenv_gain:0x20;

		output = pme->fds.main_wavetable[(pme->fds.main_addr / pme->sampling_rate)&0x3f] * 8 * pme->fds.now_volume * pme->fds.master_volume / 30;

		if( pme->fds.now_volume )
			pme->fds.now_freq = freq * 4;
		else
			pme->fds.now_freq = 0;
	} else {
		pme->fds.now_freq = 0;
		output = 0;
	}

	// LPF
#if	1
	output = (pme->output_buf[0] * 2 + output) / 3;
	pme->output_buf[0] = output;
#else
	output = (pme->output_buf[0] + pme->output_buf[1] + output) / 3;
	pme->output_buf[0] = pme->output_buf[1];
	pme->output_buf[1] = output;
#endif

	pme->fds.output = output;
	return	pme->fds.output;
}

// CPU側から呼ばれる
static void	SyncWrite( APU_FDS *pme, WORD addr, BYTE data )
{
	// クロック基準
	WriteSub( pme, addr, data, &pme->fds_sync, 1789772.5 );
}

static BYTE	SyncRead( APU_FDS *pme, WORD addr )
{
BYTE	data = addr>>8;

	if( addr >= 0x4040 && addr <= 0x407F ) {
		data = pme->fds_sync.main_wavetable[addr&0x3F] | 0x40;
	} else
	if( addr == 0x4090 ) {
		data = (pme->fds_sync.volenv_gain&0x3F)|0x40;
	} else
	if( addr == 0x4092 ) {
		data = (pme->fds_sync.swpenv_gain&0x3F)|0x40;
	}

	return	data;
}

static BOOL	Sync( APU_FDS *pme, INT cycles )
{
	// Envelope unit
	if( pme->fds_sync.envelope_enable && pme->fds_sync.envelope_speed ) {
		// Volume envelope
		double	decay;
		if( pme->fds_sync.volenv_mode < 2 ) {
			decay = ((double)pme->fds_sync.envelope_speed * (double)(pme->fds_sync.volenv_decay+1) * 1789772.5) / (232.0*960.0);
			pme->fds_sync.volenv_phaseacc -= (double)cycles;
			while( pme->fds_sync.volenv_phaseacc < 0.0 ) {
				pme->fds_sync.volenv_phaseacc += decay;

				if( pme->fds_sync.volenv_mode == 0 ) {
				// 減少モード
					if( pme->fds_sync.volenv_gain )
						pme->fds_sync.volenv_gain--;
				} else
				if( pme->fds_sync.volenv_mode == 1 ) {
				// 増加モード
					if( pme->fds_sync.volenv_gain < 0x20 )
						pme->fds_sync.volenv_gain++;
				}
			}
		}

		// Sweep envelope
		if( pme->fds_sync.swpenv_mode < 2 ) {
			decay = ((double)pme->fds_sync.envelope_speed * (double)(pme->fds_sync.swpenv_decay+1) * 1789772.5) / (232.0*960.0);
			pme->fds_sync.swpenv_phaseacc -= (double)cycles;
			while( pme->fds_sync.swpenv_phaseacc < 0.0 ) {
				pme->fds_sync.swpenv_phaseacc += decay;

				if( pme->fds_sync.swpenv_mode == 0 ) {
				// 減少モード
					if( pme->fds_sync.swpenv_gain )
						pme->fds_sync.swpenv_gain--;
				} else
				if( pme->fds_sync.swpenv_mode == 1 ) {
				// 増加モード
					if( pme->fds_sync.swpenv_gain < 0x20 )
						pme->fds_sync.swpenv_gain++;
				}
			}
		}
	}

	return	FALSE;
}

static INT	GetFreq( APU_FDS *pme, INT channel )
{
	return	pme->fds.now_freq;
}

static INT	GetStateSize(APU_FDS *pme)
{
	return	sizeof(pme->fds) + sizeof(pme->fds_sync);
}

static void	SaveState( APU_FDS *pme, LPBYTE p )
{
	SETBLOCK( p, &pme->fds, sizeof(pme->fds) );
	SETBLOCK( p, &pme->fds_sync, sizeof(pme->fds_sync) );
}

static void	LoadState( APU_FDS *pme, LPBYTE p )
{
	GETBLOCK( p, &pme->fds, sizeof(pme->fds) );
	GETBLOCK( p, &pme->fds_sync, sizeof(pme->fds_sync) );
}

static APU_FDS_vtbl vtbl = {
	NULL
	, Reset
	, Setup
	, Process
	, Write
	, Read
	, SyncWrite
	, SyncRead
	, NULL
	, Sync
	, GetFreq
	, GetStateSize
	, SaveState
	, LoadState
};

APU_FDS * APU_FDS_new(NES* parent)
{
	APU_FDS *pme = (APU_FDS *)MALLOC(sizeof(APU_FDS));

	memset(pme, 0, sizeof(APU_FDS));

	pme->vtbl = &vtbl;
	
	ZEROMEMORY( &pme->fds, sizeof(pme->fds) );
	ZEROMEMORY( &pme->fds_sync, sizeof(pme->fds) );

	ZEROMEMORY( pme->output_buf, sizeof(pme->output_buf) );

	pme->sampling_rate = 22050;
	
	return pme;
}


