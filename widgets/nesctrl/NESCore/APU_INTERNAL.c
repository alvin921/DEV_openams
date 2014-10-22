//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      APU Internal                                                    //
//                                                           Norix      //
//                                               written     2002/06/27 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#include "nes.h"
#include "APU_INTERNAL.h"

#include "state.h"
#include "rom.h"


static 	void	SyncWrite4017( APU_INTERNAL *pme, BYTE data );
static 	void	UpdateFrame(APU_INTERNAL *pme);

	// Rectangle
static 	void	WriteRectangle ( APU_INTERNAL *pme, INT no, WORD addr, BYTE data );
static 	void	UpdateRectangle( APU_INTERNAL *pme, RECTANGLE* ch, INT type );
static 	INT		RenderRectangle( APU_INTERNAL *pme, RECTANGLE* ch );
	// For Sync
static 	void	SyncWriteRectangle ( APU_INTERNAL *pme, INT no, WORD addr, BYTE data );
static 	void	SyncUpdateRectangle( APU_INTERNAL *pme, RECTANGLE* ch, INT type );

	// Triangle
static 	void	WriteTriangle ( APU_INTERNAL *pme, WORD addr, BYTE data );
static 	void	UpdateTriangle( APU_INTERNAL *pme, INT type );
static 	INT		RenderTriangle(APU_INTERNAL *pme);
	// For Sync
static 	void	SyncWriteTriangle ( APU_INTERNAL *pme, WORD addr, BYTE data );
static 	void	SyncUpdateTriangle( APU_INTERNAL *pme, INT type );

	// Noise
static 	void	WriteNoise ( APU_INTERNAL *pme, WORD addr, BYTE data );
static 	void	UpdateNoise( APU_INTERNAL *pme, INT type );
static 	BYTE	NoiseShiftreg( APU_INTERNAL *pme, BYTE xor_tap );
static 	INT	RenderNoise(APU_INTERNAL *pme);
	// For Sync
static 	void	SyncWriteNoise ( APU_INTERNAL *pme, WORD addr, BYTE data );
static 	void	SyncUpdateNoise( APU_INTERNAL *pme, INT type );

	// DPCM
static 	void	WriteDPCM ( APU_INTERNAL *pme, WORD addr, BYTE data );
//	void	UpdateDPCM();
static 	INT	RenderDPCM(APU_INTERNAL *pme);
	// For Sync
static 	void	SyncWriteDPCM (APU_INTERNAL *pme,  WORD addr, BYTE data );
static 	BOOL	SyncUpdateDPCM( APU_INTERNAL *pme, INT cycles );




// Volume shift
#define	RECTANGLE_VOL_SHIFT	8
#define	TRIANGLE_VOL_SHIFT	9
#define	NOISE_VOL_SHIFT		8
#define	DPCM_VOL_SHIFT		8

static const INT	vbl_length[32] = {
    5, 127,   10,   1,   19,   2,   40,   3,
   80,   4,   30,   5,    7,   6,   13,   7,
    6,   8,   12,   9,   24,  10,   48,  11,
   96,  12,   36,  13,    8,  14,   16,  15
};

static INT	freq_limit[8] = {
	0x03FF, 0x0555, 0x0666, 0x071C, 0x0787, 0x07C1, 0x07E0, 0x07F0
};
static INT	duty_lut[4] = {
	 2,  4,  8, 12
};

static INT	noise_freq[16] = {
	  4,    8,   16,   32,   64,   96,  128,  160,
	202,  254,  380,  508,  762, 1016, 2034, 4068
};

// DMC 転送クロック数テーブル
static INT	dpcm_cycles[16] = {
	428, 380, 340, 320, 286, 254, 226, 214,
	190, 160, 142, 128, 106,  85,  72,  54
};

static INT	dpcm_cycles_pal[16] = {
	397, 353, 315, 297, 265, 235, 209, 198,
	176, 148, 131, 118,  98,  78,  66,  50
};


//INT	vol_effect[16] = {
//	100,  94,  88,  83,  78,  74,  71,  67,
//	 64,  61,  59,  56,  54,  52,  50,  48
//};

static void	Setup( APU_INTERNAL *pme, FLOAT fClock, INT nRate );
static void	Write( APU_INTERNAL *pme, WORD addr, BYTE data );
static void	SyncWrite( APU_INTERNAL *pme, WORD addr, BYTE data );
static void	ToneTableLoad(APU_INTERNAL *pme);

static void	Reset( APU_INTERNAL *pme, FLOAT fClock, INT nRate )
{
	WORD	addr;
	ZEROMEMORY( &pme->ch0, sizeof(pme->ch0) );
	ZEROMEMORY( &pme->ch1, sizeof(pme->ch1) );
	ZEROMEMORY( &pme->ch2, sizeof(pme->ch2) );
	ZEROMEMORY( &pme->ch3, sizeof(pme->ch3) );
//	ZEROMEMORY( &pme->ch4, sizeof(pme->ch4) );

	ZEROMEMORY( pme->bToneTableEnable, sizeof(pme->bToneTableEnable) );
	ZEROMEMORY( pme->ToneTable, sizeof(pme->ToneTable) );
	ZEROMEMORY( pme->ChannelTone, sizeof(pme->ChannelTone) );

	pme->reg4015 = pme->sync_reg4015 = 0;

	// Sweep complement
	pme->ch0.complement = 0x00;
	pme->ch1.complement = 0xFF;

	// Noise shift register
	pme->ch3.shift_reg = 0x4000;

	Setup( pme, fClock, nRate );

	// $4011は初期化しない
	for( addr = 0x4000; addr <= 0x4010; addr++ ) {
		Write( pme, addr, 0x00 );
		SyncWrite( pme, addr, 0x00 );
	}
//	Write( pme, 0x4001, 0x08 );	// Reset時はincモードになる?
//	Write( pme, 0x4005, 0x08 );	// Reset時はincモードになる?
	Write( pme, 0x4012, 0x00 );
	Write( pme, 0x4013, 0x00 );
	Write( pme, 0x4015, 0x00 );
	SyncWrite( pme, 0x4012, 0x00 );
	SyncWrite( pme, 0x4013, 0x00 );
	SyncWrite( pme, 0x4015, 0x00 );

	// $4017は書き込みで初期化しない(初期モードが0であるのを期待したソフトがある為)
	pme->FrameIRQ = 0xC0;
	pme->FrameCycle = 0;
	pme->FrameIRQoccur = 0;
	pme->FrameCount = 0;
	pme->FrameType  = 0;

	// ToneLoad
	ToneTableLoad(pme);
}

static void	Setup( APU_INTERNAL *pme, FLOAT fClock, INT nRate )
{
	pme->cpu_clock = fClock;
	pme->sampling_rate = nRate;

	pme->cycle_rate = (INT)(fClock*65536.0f/(float)nRate);
}

//
// Wavetable loader
//
static void	ToneTableLoad(APU_INTERNAL *pme)
{
char fname[200];
t_HIO gio;
char	buf[512];
char c;

	sprintf(fname, "%s/%s.vtd", ROM_GetRomPath(pme->nes->rom), ROM_GetRomName(pme->nes->rom));
	DebugOut( "Path: %s\n", fname );
	gio = gio_new_file(fname, _O_RDONLY);
	if(!g_object_valid(gio)){
		sprintf(fname, "%s/Default.vtd", ROM_GetRomPath(pme->nes->rom), ROM_GetRomName(pme->nes->rom));
		DebugOut( "Path: %s\n", fname );
		gio = gio_new_file(fname, _O_RDONLY);
		if(!g_object_valid(gio)){
			DebugOut( "File not found.\n" );
			return;
		}
	}
	DebugOut( "Find.\n" );

	// 定義ファイルを読み込む
	while( gio_getln( gio, buf, 512 ) != NULL ) {
		if( buf[0] == ';' || strlen(buf) <= 0 )
			continue;

		c = toupper( buf[0] );

		if( c == '@' ) {
			char*	pbuf = &buf[1];
			char*	p;
			INT no, val, i;

			// 音色ナンバー取得
			no = strtol( pbuf, &p, 10 );
			if( pbuf == p )
				continue;
			if( no < 0 || no > TONEDATA_MAX-1 )
				continue;

			// '='を見つける
			p = strchr( pbuf, '=' );
			if( p == NULL )
				continue;
			pbuf = p+1;	// 次

			// 音色データを取得
			for( i = 0; i < TONEDATA_LEN; i++ ) {
				val = strtol( pbuf, &p, 10 );
				if( pbuf == p )	// 取得失敗？
					break;
				if( *p == ',' )	// カンマを飛ばす…
					pbuf = p+1;
				else
					pbuf = p;

				pme->ToneTable[no][i] = val;
			}
			if( i >= TONEDATA_MAX )
				pme->bToneTableEnable[no] = TRUE;
		} else
		if( c == 'A' || c == 'B' ) {
		// 各チャンネル音色定義
		char*	pbuf = &buf[1];
		char*	p;
		INT	no, val;

			// 内部音色ナンバー取得
			no = strtol( pbuf, &p, 10 );
			if( pbuf == p )
				continue;
			pbuf = p;
			if( no < 0 || no > TONE_MAX-1 )
				continue;

			// '='を見つける
			p = strchr( pbuf, '=' );
			if( p == NULL )
				continue;
			pbuf = p+1;	// 次

			// 音色ナンバー取得
			val = strtol( pbuf, &p, 10 );
			if( pbuf == p )
				continue;
			pbuf = p;

			if( val > TONEDATA_MAX-1 )
				continue;

			if( val >= 0 && pme->bToneTableEnable[val] ) {
				if( c == 'A' ) {
					pme->ChannelTone[0][no] = val+1;
				} else {
					pme->ChannelTone[1][no] = val+1;
				}
			} else {
				if( c == 'A' ) {
					pme->ChannelTone[0][no] = 0;
				} else {
					pme->ChannelTone[1][no] = 0;
				}
			}
		} else
		if( c == 'C' ) {
		// 各チャンネル音色定義
		char*	pbuf = &buf[1];
		char*	p;
		INT	val;

			// '='を見つける
			p = strchr( pbuf, '=' );
			if( p == NULL )
				continue;
			pbuf = p+1;	// 次

			// 音色ナンバー取得
			val = strtol( pbuf, &p, 10 );
			if( pbuf == p )
				continue;
			pbuf = p;

			if( val > TONEDATA_MAX-1 )
				continue;

			if( val >= 0 && pme->bToneTableEnable[val] ) {
				pme->ChannelTone[2][0] = val+1;
			} else {
				pme->ChannelTone[2][0] = 0;
			}
		}
	}

	g_object_unref(gio);
}

static INT	Process( APU_INTERNAL *pme, INT channel )
{
	switch( channel ) {
		case	0:
			return	RenderRectangle( pme, &pme->ch0 );
		case	1:
			return	RenderRectangle( pme, &pme->ch1 );
		case	2:
			return	RenderTriangle(pme);
		case	3:
			return	RenderNoise(pme);
		case	4:
			return	RenderDPCM(pme);
		default:
			return	0;
	}

	return	0;
}

static void	Write( APU_INTERNAL *pme, WORD addr, BYTE data )
{
	switch( addr ) {
		// CH0,1 rectangle
		case	0x4000:	case	0x4001:
		case	0x4002:	case	0x4003:
		case	0x4004:	case	0x4005:
		case	0x4006:	case	0x4007:
			WriteRectangle( pme, (addr<0x4004)?0:1, addr, data );
			break;

		// CH2 triangle
		case	0x4008:	case	0x4009:
		case	0x400A:	case	0x400B:
			WriteTriangle( pme, addr, data );
			break;

		// CH3 noise
		case	0x400C:	case	0x400D:
		case	0x400E:	case	0x400F:
			WriteNoise( pme, addr, data );
			break;

		// CH4 DPCM
		case	0x4010:	case	0x4011:
		case	0x4012:	case	0x4013:
			WriteDPCM( pme, addr, data );
			break;

		case	0x4015:
			pme->reg4015 = data;

			if( !(data&(1<<0)) ) {
				pme->ch0.enable    = 0;
				pme->ch0.len_count = 0;
			}
			if( !(data&(1<<1)) ) {
				pme->ch1.enable    = 0;
				pme->ch1.len_count = 0;
			}
			if( !(data&(1<<2)) ) {
				pme->ch2.enable        = 0;
				pme->ch2.len_count     = 0;
				pme->ch2.lin_count     = 0;
				pme->ch2.counter_start = 0;
			}
			if( !(data&(1<<3)) ) {
				pme->ch3.enable    = 0;
				pme->ch3.len_count = 0;
			}
			if( !(data&(1<<4)) ) {
				pme->ch4.enable    = 0;
				pme->ch4.dmalength = 0;
			} else {
				pme->ch4.enable = 0xFF;
				if( !pme->ch4.dmalength ) {
					pme->ch4.address   = pme->ch4.cache_addr;
					pme->ch4.dmalength = pme->ch4.cache_dmalength;
					pme->ch4.phaseacc  = 0;
				}
			}
			break;

		case	0x4017:
			break;

		// VirtuaNES固有ポート
		case	0x4018:
			UpdateRectangle( pme, &pme->ch0, (INT)data );
			UpdateRectangle( pme, &pme->ch1, (INT)data );
			UpdateTriangle ( pme, (INT)data );
			UpdateNoise    ( pme, (INT)data );
			break;

		default:
			break;
	}
}

static BYTE	Read( APU_INTERNAL *pme, WORD addr )
{
BYTE	data = addr>>8;

	if( addr == 0x4015 ) {
		data = 0;
		if( pme->ch0.enable && pme->ch0.len_count > 0 ) data |= (1<<0);
		if( pme->ch1.enable && pme->ch1.len_count > 0 ) data |= (1<<1);
		if( pme->ch2.enable && pme->ch2.len_count > 0 ) data |= (1<<2);
		if( pme->ch3.enable && pme->ch3.len_count > 0 ) data |= (1<<3);
	}
	return	data;
}

static void	SyncWrite( APU_INTERNAL *pme, WORD addr, BYTE data )
{
//DebugOut( "$%04X=$%02X\n", addr, data );

	switch( addr ) {
		// CH0,1 rectangle
		case	0x4000:	case	0x4001:
		case	0x4002:	case	0x4003:
		case	0x4004:	case	0x4005:
		case	0x4006:	case	0x4007:
			SyncWriteRectangle( pme, (addr<0x4004)?0:1, addr, data );
			break;

		// CH2 triangle
		case	0x4008:	case	0x4009:
		case	0x400A:	case	0x400B:
			SyncWriteTriangle( pme, addr, data );
			break;

		// CH3 noise
		case	0x400C:	case	0x400D:
		case	0x400E:	case	0x400F:
			SyncWriteNoise( pme, addr, data );
			break;

		// CH4 DPCM
		case	0x4010:	case	0x4011:
		case	0x4012:	case	0x4013:
			SyncWriteDPCM( pme, addr, data );
			break;

		case	0x4015:
			pme->sync_reg4015 = data;

			if( !(data&(1<<0)) ) {
				pme->ch0.sync_enable    = 0;
				pme->ch0.sync_len_count = 0;
			}
			if( !(data&(1<<1)) ) {
				pme->ch1.sync_enable    = 0;
				pme->ch1.sync_len_count = 0;
			}
			if( !(data&(1<<2)) ) {
				pme->ch2.sync_enable        = 0;
				pme->ch2.sync_len_count     = 0;
				pme->ch2.sync_lin_count     = 0;
				pme->ch2.sync_counter_start = 0;
			}
			if( !(data&(1<<3)) ) {
				pme->ch3.sync_enable    = 0;
				pme->ch3.sync_len_count = 0;
			}
			if( !(data&(1<<4)) ) {
				pme->ch4.sync_enable     = 0;
				pme->ch4.sync_dmalength  = 0;
				pme->ch4.sync_irq_enable = 0;

				CPU_ClrIRQ( pme->nes->cpu, IRQ_DPCM );
			} else {
				pme->ch4.sync_enable = 0xFF;
				if( !pme->ch4.sync_dmalength ) {
//					pme->ch4.sync_cycles    = pme->ch4.sync_cache_cycles;
					pme->ch4.sync_dmalength = pme->ch4.sync_cache_dmalength;
					pme->ch4.sync_cycles    = 0;
				}
			}
			break;

		case	0x4017:
			SyncWrite4017( pme, data );
			break;

		// VirtuaNES固有ポート
		case	0x4018:
			SyncUpdateRectangle( pme, &pme->ch0, (INT)data );
			SyncUpdateRectangle( pme, &pme->ch1, (INT)data );
			SyncUpdateTriangle ( pme, (INT)data );
			SyncUpdateNoise    ( pme, (INT)data );
			break;

		default:
			break;
	}
}

// $4017 Write
static void	SyncWrite4017( APU_INTERNAL *pme, BYTE data )
{
	pme->FrameCycle = 0;
	pme->FrameIRQ = data;
	pme->FrameIRQoccur = 0;

	CPU_ClrIRQ( pme->nes->cpu, IRQ_FRAMEIRQ );

	pme->FrameType = (data & 0x80) ? 1 : 0;
	pme->FrameCount = 0;
	if( data & 0x80 ) {
		UpdateFrame(pme);
	}
	pme->FrameCount = 1;
	pme->FrameCycle = 14915;
}

static void	UpdateFrame(APU_INTERNAL *pme)
{
	if( !pme->FrameCount ) {
		if( !(pme->FrameIRQ&0xC0) && NES_GetFrameIRQmode(pme->nes) ) {
			pme->FrameIRQoccur = 0xFF;
			CPU_SetIRQ( pme->nes->cpu, IRQ_FRAMEIRQ );
		}
	}

	if( pme->FrameCount == 3 ) {
		if( pme->FrameIRQ & 0x80 ) {
			pme->FrameCycle += 14915;
		}
	}

	// Counters Update
	NES_Write( pme->nes, 0x4018, (BYTE)pme->FrameCount );

	pme->FrameCount = (pme->FrameCount + 1) & 3;
}

static BYTE	SyncRead( APU_INTERNAL *pme, WORD addr )
{
BYTE	data = addr>>8;

	if( addr == 0x4015 ) {
		data = 0;
		if( pme->ch0.sync_enable && pme->ch0.sync_len_count > 0 ) data |= (1<<0);
		if( pme->ch1.sync_enable && pme->ch1.sync_len_count > 0 ) data |= (1<<1);
		if( pme->ch2.sync_enable && pme->ch2.sync_len_count > 0 ) data |= (1<<2);
		if( pme->ch3.sync_enable && pme->ch3.sync_len_count > 0 ) data |= (1<<3);
		if( pme->ch4.sync_enable && pme->ch4.sync_dmalength )     data |= (1<<4);
		if( pme->FrameIRQoccur )                             data |= (1<<6);
		if( pme->ch4.sync_irq_enable )                       data |= (1<<7);
		pme->FrameIRQoccur = 0;

		CPU_ClrIRQ( pme->nes->cpu, IRQ_FRAMEIRQ );
//DebugOut( "R 4015 %02X\n", data );
	}
	if( addr == 0x4017 ) {
		if( pme->FrameIRQoccur ) {
			data = 0;
		} else {
			data |= (1<<6);
		}
//DebugOut( "R 4017 %02X\n", data );
	}
	return	data;
}

static BOOL	Sync( APU_INTERNAL *pme, INT cycles )
{
	pme->FrameCycle -= cycles * 2;
	if( pme->FrameCycle <= 0 ) {
		pme->FrameCycle += 14915;

		UpdateFrame(pme);
	}

	return	pme->FrameIRQoccur | SyncUpdateDPCM( pme, cycles );
}

static INT	GetFreq( APU_INTERNAL *pme, INT channel )
{
INT	freq = 0;

	// Rectangle
	if( channel == 0 || channel == 1 ) {
		RECTANGLE* ch;
		if( channel == 0 ) ch = &pme->ch0;
		else		   ch = &pme->ch1;
		if( !ch->enable || ch->len_count <= 0 )
			return	0;
		if( (ch->freq < 8) || (!ch->swp_inc && ch->freq > ch->freqlimit) )
			return	0;

		if( !ch->volume )
			return	0;

//		freq = (((INT)ch->reg[3]&0x07)<<8)+(INT)ch->reg[2]+1;
		freq = (INT)(16.0f*pme->cpu_clock/(FLOAT)(ch->freq+1));
		return	freq;
	}

	// Triangle
	if( channel == 2 ) {
		if( !pme->ch2.enable || pme->ch2.len_count <= 0 )
			return	0;
		if( pme->ch2.lin_count <= 0 || pme->ch2.freq < INT2FIX(8) )
			return	0;
		freq = (((INT)pme->ch2.reg[3]&0x07)<<8)+(INT)pme->ch2.reg[2]+1;
		freq = (INT)(8.0f*pme->cpu_clock/(FLOAT)freq);
		return	freq;
	}

	// Noise
	if( channel == 3 ) {
		if( !pme->ch3.enable || pme->ch3.len_count <= 0 )
			return	0;
		if( pme->ch3.env_fixed ) {
			if( !pme->ch3.volume )
				return	0;
		} else {
			if( !pme->ch3.env_vol )
				return	0;
		}
		return	1;
	}

	// DPCM
	if( channel == 4 ) {
		if( pme->ch4.enable && pme->ch4.dmalength )
			return	1;
	}

	return	0;
}

// Write Rectangle
static void	WriteRectangle( APU_INTERNAL *pme, INT no, WORD addr, BYTE data )
{
	RECTANGLE* ch = (no==0)?&pme->ch0:&pme->ch1;

	ch->reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			ch->holdnote  = data&0x20;
			ch->volume    = data&0x0F;
			ch->env_fixed = data&0x10;
			ch->env_decay = (data&0x0F)+1;
			ch->duty      = duty_lut[data>>6];
			break;
		case	1:
			ch->swp_on    = data&0x80;
			ch->swp_inc   = data&0x08;
			ch->swp_shift = data&0x07;
			ch->swp_decay = ((data>>4)&0x07)+1;
			ch->freqlimit = freq_limit[data&0x07];
			break;
		case	2:
			ch->freq = (ch->freq&(~0xFF))+data;
			break;
		case	3: // Master
			ch->freq      = ((data&0x07)<<8)+(ch->freq&0xFF);
			ch->len_count = vbl_length[data>>3]*2;
			ch->env_vol   = 0x0F;
			ch->env_count = ch->env_decay+1;
			ch->adder     = 0;

			if( pme->reg4015&(1<<no) )
				ch->enable    = 0xFF;
			break;
	}
}

// Update Rectangle
static void	UpdateRectangle( APU_INTERNAL *pme, RECTANGLE* ch, INT type )
{
	if( !ch->enable || ch->len_count <= 0 )
		return;

	// Update Length/Sweep
	if( !(type & 1) ) {
		// Update Length
		if( ch->len_count && !ch->holdnote ) {
			// Holdnote
			if( ch->len_count ) {
				ch->len_count--;
			}
		}

		// Update Sweep
		if( ch->swp_on && ch->swp_shift ) {
			if( ch->swp_count ) {
				ch->swp_count--;
			}
			if( ch->swp_count == 0 ) {
				ch->swp_count = ch->swp_decay;
				if( ch->swp_inc ) {
				// Sweep increment(to higher frequency)
					if( !ch->complement )
						ch->freq += ~(ch->freq >> ch->swp_shift); // CH 0
					else
						ch->freq -=  (ch->freq >> ch->swp_shift); // CH 1
				} else {
				// Sweep decrement(to lower frequency)
					ch->freq += (ch->freq >> ch->swp_shift);
				}
			}
		}
	}

	// Update Envelope
	if( ch->env_count ) {
		ch->env_count--;
	}
	if( ch->env_count == 0 ) {
		ch->env_count = ch->env_decay;

		// Holdnote
		if( ch->holdnote ) {
			ch->env_vol = (ch->env_vol-1)&0x0F;
		} else if( ch->env_vol ) {
			ch->env_vol--;
		}
	}

	if( !ch->env_fixed ) {
		ch->nowvolume = ch->env_vol<<RECTANGLE_VOL_SHIFT;
	}
}

// Sync Write Rectangle
static void	SyncWriteRectangle( APU_INTERNAL *pme, INT no, WORD addr, BYTE data )
{
	RECTANGLE* ch = (no==0)?&pme->ch0:&pme->ch1;

	ch->sync_reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			ch->sync_holdnote = data&0x20;
			break;
		case	1:
		case	2:
			break;
		case	3: // Master
			ch->sync_len_count = vbl_length[data>>3]*2;
			if( pme->sync_reg4015&(1<<no) )
				ch->sync_enable = 0xFF;
			break;
	}
}

// Sync Update Rectangle
static void	SyncUpdateRectangle( APU_INTERNAL *pme, RECTANGLE* ch, INT type )
{
	if( !ch->sync_enable || ch->sync_len_count <= 0 )
		return;

	// Update Length
	if( ch->sync_len_count && !ch->sync_holdnote ) {
		if( !(type & 1) && ch->sync_len_count ) {
			ch->sync_len_count--;
		}
	}
}

// Render Rectangle
static INT	RenderRectangle( APU_INTERNAL *pme, RECTANGLE* ch )
{
	INT	volume;
	INT	freq;

	if( !ch->enable || ch->len_count <= 0 )
		return	0;

	// Channel disable?
	if( (ch->freq < 8) || (!ch->swp_inc && ch->freq > ch->freqlimit) ) {
		return	0;
	}

	if( ch->env_fixed ) {
		ch->nowvolume = ch->volume<<RECTANGLE_VOL_SHIFT;
	}
	volume = ch->nowvolume;

	if( !(APU_CHANGE_TONE && pme->ChannelTone[(!ch->complement)?0:1][ch->reg[0]>>6]) ) {
		// 補間処理
		double	total;
		double	sample_weight = ch->phaseacc;
		if( sample_weight > pme->cycle_rate ) {
			sample_weight = pme->cycle_rate;
		}
		total = (ch->adder < ch->duty)?sample_weight:-sample_weight;

		freq = INT2FIX( ch->freq+1 );
		ch->phaseacc -= pme->cycle_rate;
		while( ch->phaseacc < 0 ) {
			ch->phaseacc += freq;
			ch->adder = (ch->adder+1)&0x0F;

			sample_weight = freq;
			if( ch->phaseacc > 0 ) {
				sample_weight -= ch->phaseacc;
			}
			total += (ch->adder < ch->duty)?sample_weight:-sample_weight;
		}
		return	(INT)( volume*total/pme->cycle_rate + 0.5 );
	} else {
		INT*	pTone = pme->ToneTable[pme->ChannelTone[(!ch->complement)?0:1][ch->reg[0]>>6]-1];
		INT	freq;
		INT	num_times, total;

		// 更新無し
		ch->phaseacc -= pme->cycle_rate*2;
		if( ch->phaseacc >= 0 ) {
			return	pTone[ch->adder&0x1F]*volume/((1<<RECTANGLE_VOL_SHIFT)/2);
		}

		// 1ステップだけ更新
		freq = INT2FIX( ch->freq+1 );
		if( freq > pme->cycle_rate*2 ) {
			ch->phaseacc += freq;
			ch->adder = (ch->adder+1)&0x1F;
			return	pTone[ch->adder&0x1F]*volume/((1<<RECTANGLE_VOL_SHIFT)/2);
		}

		// 加重平均
		num_times = total = 0;
		while( ch->phaseacc < 0 ) {
			ch->phaseacc += freq;
			ch->adder = (ch->adder+1)&0x1F;
			total += pTone[ch->adder&0x1F]*volume/((1<<RECTANGLE_VOL_SHIFT)/2);
			num_times++;
		}
		return	total/num_times;
	}
}

/////////////

// Write Triangle
static void	WriteTriangle( APU_INTERNAL *pme, WORD addr, BYTE data )
{
	pme->ch2.reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			pme->ch2.holdnote = data&0x80;
			break;
		case	1: // Unused
			break;
		case	2:
			pme->ch2.freq = INT2FIX( ((((INT)pme->ch2.reg[3]&0x07)<<8)+(INT)data+1) );
			break;
		case	3: // Master
			pme->ch2.freq      = INT2FIX( ((((INT)data&0x07)<<8)+(INT)pme->ch2.reg[2]+1) );
			pme->ch2.len_count = vbl_length[data>>3]*2;
			pme->ch2.counter_start = 0x80;

			if( pme->reg4015&(1<<2) )
				pme->ch2.enable = 0xFF;
			break;
	}
}

// Update Triangle
static void	UpdateTriangle( APU_INTERNAL *pme, INT type )
{
	if( !pme->ch2.enable )
		return;

	if( !(type & 1) && !pme->ch2.holdnote ) {
		if( pme->ch2.len_count ) {
			pme->ch2.len_count--;
		}
	}

//	if( !pme->ch2.len_count ) {
//		pme->ch2.lin_count = 0;
//	}

	// Update Length/Linear
	if( pme->ch2.counter_start ) {
		pme->ch2.lin_count = pme->ch2.reg[0] & 0x7F;
	} else if( pme->ch2.lin_count ) {
		pme->ch2.lin_count--;
	}
	if( !pme->ch2.holdnote && pme->ch2.lin_count ) {
		pme->ch2.counter_start = 0;
	}
}

// Sync Write Triangle
static void	SyncWriteTriangle( APU_INTERNAL *pme, WORD addr, BYTE data )
{
	pme->ch2.sync_reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			pme->ch2.sync_holdnote = data&0x80;
			break;
		case	1:
			break;
		case	2:
			break;
		case	3: // Master
			pme->ch2.sync_len_count = vbl_length[pme->ch2.sync_reg[3]>>3]*2;
			pme->ch2.sync_counter_start = 0x80;

			if( pme->sync_reg4015&(1<<2) )
				pme->ch2.sync_enable = 0xFF;
			break;
	}
}

// Sync Update Triangle
static void	SyncUpdateTriangle( APU_INTERNAL *pme, INT type )
{
	if( !pme->ch2.sync_enable )
		return;

	if( !(type & 1) && !pme->ch2.sync_holdnote ) {
		if( pme->ch2.sync_len_count ) {
			pme->ch2.sync_len_count--;
		}
	}

//	if( !pme->ch2.sync_len_count ) {
//		pme->ch2.sync_lin_count = 0;
//	}

	// Update Length/Linear
	if( pme->ch2.sync_counter_start ) {
		pme->ch2.sync_lin_count = pme->ch2.sync_reg[0] & 0x7F;
	} else if( pme->ch2.sync_lin_count ) {
		pme->ch2.sync_lin_count--;
	}
	if( !pme->ch2.sync_holdnote && pme->ch2.sync_lin_count ) {
		pme->ch2.sync_counter_start = 0;
	}
}

// Render Triangle
static INT	RenderTriangle(APU_INTERNAL *pme)
{
	INT	vol;
	if( APU_DISABLE_VOLUME_EFFECT) {
		vol = 256;
	} else {
		vol = 256-(INT)((pme->ch4.reg[1]&0x01)+pme->ch4.dpcm_value*2);
	}

	if( !pme->ch2.enable || (pme->ch2.len_count <= 0) || (pme->ch2.lin_count <= 0) ) {
		return	pme->ch2.nowvolume*vol/256;
	}

	if( pme->ch2.freq < INT2FIX(8) ) {
		return	pme->ch2.nowvolume*vol/256;
	}

	if( !(APU_CHANGE_TONE && pme->ChannelTone[2][0]) ) {
		INT	num_times, total;
		pme->ch2.phaseacc -= pme->cycle_rate;
		if( pme->ch2.phaseacc >= 0 ) {
			return	pme->ch2.nowvolume*vol/256;
		}

		if( pme->ch2.freq > pme->cycle_rate ) {
			pme->ch2.phaseacc += pme->ch2.freq;
			pme->ch2.adder = (pme->ch2.adder+1)&0x1F;

			if( pme->ch2.adder < 0x10 ) {
				pme->ch2.nowvolume = (pme->ch2.adder&0x0F)<<TRIANGLE_VOL_SHIFT;
			} else {
				pme->ch2.nowvolume = (0x0F-(pme->ch2.adder&0x0F))<<TRIANGLE_VOL_SHIFT;
			}

			return	pme->ch2.nowvolume*vol/256;
		}

		// 加重平均
		num_times = total = 0;
		while( pme->ch2.phaseacc < 0 ) {
			pme->ch2.phaseacc += pme->ch2.freq;
			pme->ch2.adder = (pme->ch2.adder+1)&0x1F;

			if( pme->ch2.adder < 0x10 ) {
				pme->ch2.nowvolume = (pme->ch2.adder&0x0F)<<TRIANGLE_VOL_SHIFT;
			} else {
				pme->ch2.nowvolume = (0x0F-(pme->ch2.adder&0x0F))<<TRIANGLE_VOL_SHIFT;
			}

			total += pme->ch2.nowvolume;
			num_times++;
		}

		return	(total/num_times)*vol/256;
	} else {
		INT*	pTone = pme->ToneTable[pme->ChannelTone[2][0]-1];
		INT	num_times, total;

		pme->ch2.phaseacc -= pme->cycle_rate;
		if( pme->ch2.phaseacc >= 0 ) {
			return	pme->ch2.nowvolume*vol/256;
		}

		if( pme->ch2.freq > pme->cycle_rate ) {
			pme->ch2.phaseacc += pme->ch2.freq;
			pme->ch2.adder = (pme->ch2.adder+1)&0x1F;
			pme->ch2.nowvolume = pTone[pme->ch2.adder&0x1F]*0x0F;
			return	pme->ch2.nowvolume*vol/256;
		}

		// 加重平均
		num_times = total = 0;
		while( pme->ch2.phaseacc < 0 ) {
			pme->ch2.phaseacc += pme->ch2.freq;
			pme->ch2.adder = (pme->ch2.adder+1)&0x1F;
			total += pTone[pme->ch2.adder&0x1F]*0x0F;
			num_times++;
		}

		return	(total/num_times)*vol/256;
	}
}

//////////////

// Write Noise
static void	WriteNoise( APU_INTERNAL *pme, WORD addr, BYTE data )
{
	pme->ch3.reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			pme->ch3.holdnote  = data&0x20;
			pme->ch3.volume    = data&0x0F;
			pme->ch3.env_fixed = data&0x10;
			pme->ch3.env_decay = (data&0x0F)+1;
			break;
		case	1: // Unused
			break;
		case	2:
			pme->ch3.freq    = INT2FIX(noise_freq[data&0x0F]);
			pme->ch3.xor_tap = (data&0x80)?0x40:0x02;
			break;
		case	3: // Master
			pme->ch3.len_count = vbl_length[data>>3]*2;
			pme->ch3.env_vol   = 0x0F;
			pme->ch3.env_count = pme->ch3.env_decay+1;

			if( pme->reg4015&(1<<3) )
				pme->ch3.enable    = 0xFF;
			break;
	}
}

// Update Noise
static void	UpdateNoise( APU_INTERNAL *pme, INT type )
{
	if( !pme->ch3.enable || pme->ch3.len_count <= 0 )
		return;

	// Update Length
	if( !pme->ch3.holdnote ) {
		// Holdnote
		if( !(type & 1) && pme->ch3.len_count ) {
			pme->ch3.len_count--;
		}
	}

	// Update Envelope
	if( pme->ch3.env_count ) {
		pme->ch3.env_count--;
	}
	if( pme->ch3.env_count == 0 ) {
		pme->ch3.env_count = pme->ch3.env_decay;

		// Holdnote
		if( pme->ch3.holdnote ) {
			pme->ch3.env_vol = (pme->ch3.env_vol-1)&0x0F;
		} else if( pme->ch3.env_vol ) {
			pme->ch3.env_vol--;
		}
	}

	if( !pme->ch3.env_fixed ) {
		pme->ch3.nowvolume = pme->ch3.env_vol<<RECTANGLE_VOL_SHIFT;
	}
}

// Sync Write Noise
static void	SyncWriteNoise( APU_INTERNAL *pme, WORD addr, BYTE data )
{
	pme->ch3.sync_reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			pme->ch3.sync_holdnote = data&0x20;
			break;
		case	1:
			break;
		case	2:
			break;
		case	3: // Master
			pme->ch3.sync_len_count = vbl_length[data>>3]*2;
			if( pme->sync_reg4015&(1<<3) )
				pme->ch3.sync_enable = 0xFF;
			break;
	}
}

// Sync Update Noise
static void	SyncUpdateNoise( APU_INTERNAL *pme, INT type )
{
	if( !pme->ch3.sync_enable || pme->ch3.sync_len_count <= 0 )
		return;

	// Update Length
	if( pme->ch3.sync_len_count && !pme->ch3.sync_holdnote ) {
		if( !(type & 1) && pme->ch3.sync_len_count ) {
			pme->ch3.sync_len_count--;
		}
	}
}

// Noise ShiftRegister
static BYTE	NoiseShiftreg( APU_INTERNAL *pme, BYTE xor_tap )
{
int	bit0, bit14;

	bit0 = pme->ch3.shift_reg & 1;
	if( pme->ch3.shift_reg & xor_tap ) bit14 = bit0^1;
	else			      bit14 = bit0^0;
	pme->ch3.shift_reg >>= 1;
	pme->ch3.shift_reg |= (bit14<<14);
	return	(bit0^1);
}

// Render Noise
static INT	RenderNoise(APU_INTERNAL *pme)
{
	INT	vol;
	INT	num_times, total;
	if( !pme->ch3.enable || pme->ch3.len_count <= 0 )
		return	0;

	if( pme->ch3.env_fixed ) {
		pme->ch3.nowvolume = pme->ch3.volume<<RECTANGLE_VOL_SHIFT;
	}

	vol = 256-(INT)((pme->ch4.reg[1]&0x01)+pme->ch4.dpcm_value*2);

	pme->ch3.phaseacc -= pme->cycle_rate;
	if( pme->ch3.phaseacc >= 0 )
		return	pme->ch3.output*vol/256;

	if( pme->ch3.freq > pme->cycle_rate ) {
		pme->ch3.phaseacc += pme->ch3.freq;
		if( NoiseShiftreg(pme, pme->ch3.xor_tap) )
			pme->ch3.output = pme->ch3.nowvolume;
		else
			pme->ch3.output = -pme->ch3.nowvolume;

		return	pme->ch3.output*vol/256;
	}

	num_times = total = 0;
	while( pme->ch3.phaseacc < 0 ) {
		pme->ch3.phaseacc += pme->ch3.freq;
		if( NoiseShiftreg(pme, pme->ch3.xor_tap) )
			pme->ch3.output = pme->ch3.nowvolume;
		else
			pme->ch3.output = -pme->ch3.nowvolume;

		total += pme->ch3.output;
		num_times++;
	}

	return	(total/num_times)*vol/256;
}

//////////

static void	WriteDPCM( APU_INTERNAL *pme, WORD addr, BYTE data )
{
	pme->ch4.reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			pme->ch4.freq    = INT2FIX( NES_GetVideoMode(pme->nes)?dpcm_cycles_pal[data&0x0F]:dpcm_cycles[data&0x0F] );
//			pme->ch4.freq    = INT2FIX( dpcm_cycles[data&0x0F] );
////			pme->ch4.freq    = INT2FIX( (dpcm_cycles[data&0x0F]-((data&0x0F)^0x0F)*2-2) );
			pme->ch4.looping = data&0x40;
			break;
		case	1:
			pme->ch4.dpcm_value = (data&0x7F)>>1;
			break;
		case	2:
			pme->ch4.cache_addr = 0xC000+(WORD)(data<<6);
			break;
		case	3:
			pme->ch4.cache_dmalength = ((data<<4)+1)<<3;
			break;
	}
}

static INT	RenderDPCM(APU_INTERNAL *pme)
{
	if( pme->ch4.dmalength ) {
		pme->ch4.phaseacc -= pme->cycle_rate;

		while( pme->ch4.phaseacc < 0 ) {
			pme->ch4.phaseacc += pme->ch4.freq;
			if( !(pme->ch4.dmalength&7) ) {
				pme->ch4.cur_byte = NES_Read( pme->nes, pme->ch4.address );
				if( 0xFFFF == pme->ch4.address )
					pme->ch4.address = 0x8000;
				else
					pme->ch4.address++;
			}

			if( !(--pme->ch4.dmalength) ) {
				if( pme->ch4.looping ) {
					pme->ch4.address = pme->ch4.cache_addr;
					pme->ch4.dmalength = pme->ch4.cache_dmalength;
				} else {
					pme->ch4.enable = 0;
					break;
				}
			}
			// positive delta
			if( pme->ch4.cur_byte&(1<<((pme->ch4.dmalength&7)^7)) ) {
				if( pme->ch4.dpcm_value < 0x3F )
					pme->ch4.dpcm_value += 1;
			} else {
			// negative delta
				if( pme->ch4.dpcm_value > 1 )
					pme->ch4.dpcm_value -= 1;
			}
		}
	}

#if	1
	// インチキ臭いプチノイズカット(TEST)
	pme->ch4.dpcm_output_real = (INT)((pme->ch4.reg[1]&0x01)+pme->ch4.dpcm_value*2)-0x40;
	if( abs(pme->ch4.dpcm_output_real-pme->ch4.dpcm_output_fake) <= 8 ) {
		pme->ch4.dpcm_output_fake = pme->ch4.dpcm_output_real;
		pme->ch4.output = (INT)pme->ch4.dpcm_output_real<<DPCM_VOL_SHIFT;
	} else {
		if( pme->ch4.dpcm_output_real > pme->ch4.dpcm_output_fake )
			pme->ch4.dpcm_output_fake += 8;
		else
			pme->ch4.dpcm_output_fake -= 8;
		pme->ch4.output = (INT)pme->ch4.dpcm_output_fake<<DPCM_VOL_SHIFT;
	}
#else
	pme->ch4.output = (((INT)pme->ch4.reg[1]&0x01)+(INT)pme->ch4.dpcm_value*2)<<DPCM_VOL_SHIFT;
//	pme->ch4.output = ((((INT)pme->ch4.reg[1]&0x01)+(INT)pme->ch4.dpcm_value*2)-0x40)<<DPCM_VOL_SHIFT;
#endif
	return	pme->ch4.output;
}

static void	SyncWriteDPCM( APU_INTERNAL *pme, WORD addr, BYTE data )
{
	pme->ch4.reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
////			pme->ch4.sync_cache_cycles = dpcm_cycles[data&0x0F] * 8 - ((data&0x0F)^0x0F)*16 - 0x10;
////			pme->ch4.sync_cycles       = 0;
//			pme->ch4.sync_cache_cycles = dpcm_cycles[data&0x0F] * 8;
			pme->ch4.sync_cache_cycles = NES_GetVideoMode(pme->nes)?dpcm_cycles_pal[data&0x0F]*8:dpcm_cycles[data&0x0F]*8;
			pme->ch4.sync_looping      = data&0x40;
			pme->ch4.sync_irq_gen      = data&0x80;
			if( !pme->ch4.sync_irq_gen ) {
				pme->ch4.sync_irq_enable = 0;
				CPU_ClrIRQ( pme->nes->cpu, IRQ_DPCM );
			}
			break;
		case	1:
			break;
		case	2:
			break;
		case	3:
			pme->ch4.sync_cache_dmalength = (data<<4)+1;
			break;
	}
}

static BOOL	SyncUpdateDPCM( APU_INTERNAL *pme, INT cycles )
{
BOOL	bIRQ = FALSE;

	if( pme->ch4.sync_enable ) {
		pme->ch4.sync_cycles -= cycles;
		while( pme->ch4.sync_cycles < 0 ) {
			pme->ch4.sync_cycles += pme->ch4.sync_cache_cycles;
			if( pme->ch4.sync_dmalength ) {
//				if( !(--pme->ch4.sync_dmalength) ) {
				if( --pme->ch4.sync_dmalength < 2 ) {
					if( pme->ch4.sync_looping ) {
						pme->ch4.sync_dmalength = pme->ch4.sync_cache_dmalength;
					} else {
						pme->ch4.sync_dmalength = 0;

						if( pme->ch4.sync_irq_gen ) {
							pme->ch4.sync_irq_enable = 0xFF;
							CPU_SetIRQ( pme->nes->cpu, IRQ_DPCM );
						}
					}
				}
			}
		}
	}
	if( pme->ch4.sync_irq_enable ) {
		bIRQ = TRUE;
	}

	return	bIRQ;
}

static INT	GetStateSize(APU_INTERNAL *pme)
{
	return	4*sizeof(BYTE) + 3*sizeof(INT)
		+ sizeof(pme->ch0) + sizeof(pme->ch1)
		+ sizeof(pme->ch2) + sizeof(pme->ch3)
		+ sizeof(pme->ch4);
}

static void	SaveState( APU_INTERNAL *pme, LPBYTE p )
{
	SETBYTE( p, pme->reg4015 );
	SETBYTE( p, pme->sync_reg4015 );

	SETINT( p, pme->FrameCycle );
	SETINT( p, pme->FrameCount );
	SETINT( p, pme->FrameType );
	SETBYTE( p, pme->FrameIRQ );
	SETBYTE( p, pme->FrameIRQoccur );

	SETBLOCK( p, &pme->ch0, sizeof(pme->ch0) );
	SETBLOCK( p, &pme->ch1, sizeof(pme->ch1) );
	SETBLOCK( p, &pme->ch2, sizeof(pme->ch2) );
	SETBLOCK( p, &pme->ch3, sizeof(pme->ch3) );
	SETBLOCK( p, &pme->ch4, sizeof(pme->ch4) );
}

static void	LoadState( APU_INTERNAL *pme, LPBYTE p )
{
	GETBYTE( p, pme->reg4015 );
	GETBYTE( p, pme->sync_reg4015 );

	GETINT( p, pme->FrameCycle );
	GETINT( p, pme->FrameCount );
	GETINT( p, pme->FrameType );
	GETBYTE( p, pme->FrameIRQ );
	GETBYTE( p, pme->FrameIRQoccur );

	GETBLOCK( p, &pme->ch0, sizeof(pme->ch0) );
	GETBLOCK( p, &pme->ch1, sizeof(pme->ch1) );
	GETBLOCK( p, &pme->ch2, sizeof(pme->ch2) );
	GETBLOCK( p, &pme->ch3, sizeof(pme->ch3) );
//	p += sizeof(pme->ch3);
	GETBLOCK( p, &pme->ch4, sizeof(pme->ch4) );
}

static void	GetFrameIRQ( APU_INTERNAL *pme, INT* Cycle, BYTE* Count, BYTE* Type, BYTE* IRQ, BYTE* Occur ) {
	*Cycle = pme->FrameCycle;
	*Count = (BYTE)pme->FrameCount;
	*Type  = (BYTE)pme->FrameType;
	*IRQ   = pme->FrameIRQ;
	*Occur = pme->FrameIRQoccur;
}

static void	SetFrameIRQ( APU_INTERNAL *pme, INT Cycle, BYTE Count, BYTE Type, BYTE IRQ, BYTE Occur ) {
	pme->FrameCycle	  = Cycle;
	pme->FrameCount	  = (INT)Count;
	pme->FrameType	  = (INT)Type;
	pme->FrameIRQ	  = IRQ;
	pme->FrameIRQoccur = Occur;
}

static APU_INTERNAL_vtbl vtbl = {
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
	
	, GetFrameIRQ
	, SetFrameIRQ
};

APU_INTERNAL * APU_INTERNAL_new(NES* parent)
{
	APU_INTERNAL *pme = (APU_INTERNAL *)MALLOC(sizeof(APU_INTERNAL));

	memset(pme, 0, sizeof(APU_INTERNAL));
	
	pme->nes = parent;

	pme->vtbl = &vtbl;
	
	ZEROMEMORY( &pme->ch0, sizeof(pme->ch0) );
	ZEROMEMORY( &pme->ch1, sizeof(pme->ch1) );
	ZEROMEMORY( &pme->ch2, sizeof(pme->ch2) );
	ZEROMEMORY( &pme->ch3, sizeof(pme->ch3) );
	ZEROMEMORY( &pme->ch4, sizeof(pme->ch4) );

	pme->FrameIRQ = 0xC0;
	pme->FrameCycle = 0;
	pme->FrameIRQoccur = 0;
	pme->FrameCount = 0;
	pme->FrameType  = 0;

	pme->reg4015 = pme->sync_reg4015 = 0;

	pme->cpu_clock = APU_CLOCK;
	pme->sampling_rate = 22050;

	// 仮設定
	pme->cycle_rate = (INT)(pme->cpu_clock*65536.0f/22050.0f);
	
	return pme;
}


