//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES APU core                                                    //
//                                                           Norix      //
//                                               written     2002/06/27 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#include "apu.h"
#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"

#include "APU_INTERNAL.h"
#include "APU_VRC6.h"
#include "APU_VRC7.h"
#include "APU_MMC5.h"
#include "APU_FDS.h"
#include "APU_N106.h"
#include "APU_FME7.h"


typedef struct {
	INT time;
	WORD	addr;
	BYTE	data;
	BYTE	reserved;
} QUEUEDATA, *LPQUEUEDATA;

typedef struct {
	INT 	rdptr;
	INT 	wrptr;
	QUEUEDATA	data[QUEUE_LENGTH];
} QUEUE, *LPQUEUE;

struct APU_struct{
		NES*	nes;
	
		QUEUE	queue;
		QUEUE	exqueue;
	
		BYTE	exsound_select;
	
		double	elapsed_time;
	//	INT elapsed_time;
	
		// Filter
		INT last_data, last_diff;
		INT lowpass_filter[4];
	
		// Sound core
		APU_INTERNAL *	internal;
		APU_VRC6 *	vrc6;
		APU_VRC7 *	vrc7;
		APU_MMC5 *	mmc5;
		APU_FDS * 	fds;
		APU_N106 *	n106;
		APU_FME7 *	fme7;
	
		// Channel mute
		BOOL	m_bMute[16];
	
		// おまけ
		SHORT	m_SoundBuffer[0x100];
};




// Volume adjust
// Internal sounds
#define	RECTANGLE_VOL	(0x0F0)
#define	TRIANGLE_VOL	(0x130)
#define	NOISE_VOL	(0x0C0)
#define	DPCM_VOL	(0x0F0)
// Extra sounds
#define	VRC6_VOL	(0x0F0)
#define	VRC7_VOL	(0x130)
#define	FDS_VOL		(0x0F0)
#define	MMC5_VOL	(0x0F0)
#define	N106_VOL	(0x088)
#define	FME7_VOL	(0x130)

void	APU_SetQueue( APU *pme, INT writetime, WORD addr, BYTE data );
BOOL	APU_GetQueue( APU *pme, INT writetime, QUEUEDATA* ret );

void	APU_SetExQueue( APU *pme, INT writetime, WORD addr, BYTE data );
BOOL	APU_GetExQueue( APU *pme, INT writetime, QUEUEDATA* ret );

void	APU_QueueFlush(APU *pme );

void	APU_WriteProcess( APU *pme, WORD addr, BYTE data );
void	APU_WriteExProcess( APU *pme, WORD addr, BYTE data );


void APU_Destroy( APU * pme)
{
	if(pme){
		IAPU_Destroy(pme->internal);
		IAPU_Destroy(pme->vrc6);
		IAPU_Destroy(pme->vrc7);
		IAPU_Destroy(pme->mmc5);
		IAPU_Destroy(pme->fds);
		IAPU_Destroy(pme->n106);
		IAPU_Destroy(pme->fme7);

		FREE(pme);
	}
}

APU *APU_New( NES* parent )
{
	APU *pme;
	INT i;

	pme = (APU *)MALLOC(sizeof(APU));
	memset(pme, 0, sizeof(APU));
	pme->exsound_select = 0;

	pme->nes = parent;
	pme->internal = APU_INTERNAL_new(parent);
	pme->vrc6 = APU_VRC6_new(parent);
	pme->vrc7 = APU_VRC7_new(parent);
	pme->mmc5 = APU_MMC5_new(parent);
	pme->fds = APU_FDS_new(parent);
	pme->n106 = APU_N106_new(parent);
	pme->fme7 = APU_FME7_new(parent);





	pme->last_data = pme->last_diff = 0;

	ZEROMEMORY( pme->m_SoundBuffer, sizeof(pme->m_SoundBuffer) );

	ZEROMEMORY( pme->lowpass_filter, sizeof(pme->lowpass_filter) );
	ZEROMEMORY( &pme->queue, sizeof(pme->queue) );
	ZEROMEMORY( &pme->exqueue, sizeof(pme->exqueue) );

	for( i = 0; i < 16; i++ ) {
		pme->m_bMute[i] = TRUE;
	}
	return pme;
}

void	APU_SetQueue( APU *pme, INT writetime, WORD addr, BYTE data )
{
	pme->queue.data[pme->queue.wrptr].time = writetime;
	pme->queue.data[pme->queue.wrptr].addr = addr;
	pme->queue.data[pme->queue.wrptr].data = data;
	pme->queue.wrptr++;
	pme->queue.wrptr&=QUEUE_LENGTH-1;
	if( pme->queue.wrptr == pme->queue.rdptr ) {
		DebugOut( "queue overflow.\n" );
	}
}

BOOL	APU_GetQueue( APU *pme, INT writetime, QUEUEDATA* ret )
{
	if( pme->queue.wrptr == pme->queue.rdptr ) {
		return	FALSE;
	}
	if( pme->queue.data[pme->queue.rdptr].time <= writetime ) {
		*ret = pme->queue.data[pme->queue.rdptr];
		pme->queue.rdptr++;
		pme->queue.rdptr&=QUEUE_LENGTH-1;
		return	TRUE;
	}
	return	FALSE;
}

void	APU_SetExQueue( APU *pme, INT writetime, WORD addr, BYTE data )
{
	pme->exqueue.data[pme->exqueue.wrptr].time = writetime;
	pme->exqueue.data[pme->exqueue.wrptr].addr = addr;
	pme->exqueue.data[pme->exqueue.wrptr].data = data;
	pme->exqueue.wrptr++;
	pme->exqueue.wrptr&=QUEUE_LENGTH-1;
	if( pme->exqueue.wrptr == pme->exqueue.rdptr ) {
		DebugOut( "exqueue overflow.\n" );
	}
}

BOOL	APU_GetExQueue( APU *pme, INT writetime, QUEUEDATA* ret )
{
	if( pme->exqueue.wrptr == pme->exqueue.rdptr ) {
		return	FALSE;
	}
	if( pme->exqueue.data[pme->exqueue.rdptr].time <= writetime ) {
		*ret = pme->exqueue.data[pme->exqueue.rdptr];
		pme->exqueue.rdptr++;
		pme->exqueue.rdptr&=QUEUE_LENGTH-1;
		return	TRUE;
	}
	return	FALSE;
}

BOOL	APU_SetChannelMute( APU *pme, INT nCH )
{ 
	pme->m_bMute[nCH] = !pme->m_bMute[nCH]; 
	return pme->m_bMute[nCH]; 
}

void	APU_QueueClear( APU *pme )
{
	ZEROMEMORY( &pme->queue, sizeof(pme->queue) );
	ZEROMEMORY( &pme->exqueue, sizeof(pme->exqueue) );
}

void	APU_QueueFlush(APU *pme)
{
	while( pme->queue.wrptr != pme->queue.rdptr ) {
		APU_WriteProcess( pme, pme->queue.data[pme->queue.rdptr].addr, pme->queue.data[pme->queue.rdptr].data );
		pme->queue.rdptr++;
		pme->queue.rdptr&=QUEUE_LENGTH-1;
	}

	while( pme->exqueue.wrptr != pme->exqueue.rdptr ) {
		APU_WriteExProcess( pme, pme->exqueue.data[pme->exqueue.rdptr].addr, pme->exqueue.data[pme->exqueue.rdptr].data );
		pme->exqueue.rdptr++;
		pme->exqueue.rdptr&=QUEUE_LENGTH-1;
	}
}

void	APU_SoundSetup(APU *pme)
{
	FLOAT	fClock = pme->nes->nescfg->CpuClock;
	INT	nRate = (INT)APU_SOUND_RATE/*alvin: Config.sound.nRate*/;
	
	IAPU_Setup( pme->internal, fClock, nRate );
	IAPU_Setup( pme->vrc6, fClock, nRate );
	IAPU_Setup( pme->vrc7, fClock, nRate );
	IAPU_Setup( pme->mmc5, fClock, nRate );
	IAPU_Setup ( pme->fds, fClock, nRate );
	IAPU_Setup( pme->n106, fClock, nRate );
	IAPU_Setup( pme->fme7, fClock, nRate );
}

void	APU_Reset(APU *pme)
{
	FLOAT	fClock = pme->nes->nescfg->CpuClock;
	INT	nRate = (INT)APU_SOUND_RATE/*alvin: Config.sound.nRate*/;
	
	ZEROMEMORY( &pme->queue, sizeof(pme->queue) );
	ZEROMEMORY( &pme->exqueue, sizeof(pme->exqueue) );

	pme->elapsed_time = 0;

	IAPU_Reset( pme->internal, fClock, nRate );
	IAPU_Reset( pme->vrc6, fClock, nRate );
	IAPU_Reset( pme->vrc7, fClock, nRate );
	IAPU_Reset( pme->mmc5, fClock, nRate );
	IAPU_Reset ( pme->fds, fClock, nRate );
	IAPU_Reset( pme->n106, fClock, nRate );
	IAPU_Reset( pme->fme7, fClock, nRate );

	APU_SoundSetup(pme);
}

void	APU_SelectExSound( APU *pme, BYTE data )
{
	pme->exsound_select = data;
}

BYTE	APU_Read( APU *pme, WORD addr )
{
	return IAPU_SyncRead( pme->internal, addr );
}

void	APU_Write( APU *pme, WORD addr, BYTE data )
{
	// $4018はVirtuaNES固有ポート
	if( addr >= 0x4000 && addr <= 0x401F ) {
		IAPU_SyncWrite(	pme->internal, addr, data );
		APU_SetQueue( pme, CPU_GetTotalCycles(pme->nes->cpu), addr, data );
	}
}

BYTE	APU_ExRead( APU *pme, WORD addr )
{
BYTE	data = 0;

	if( pme->exsound_select & 0x10 ) {
		if( addr == 0x4800 ) {
			APU_SetExQueue( pme, CPU_GetTotalCycles(pme->nes->cpu), 0, 0 );
		}
	}
	if( pme->exsound_select & 0x04 ) {
		if( addr >= 0x4040 && addr < 0x4100 ) {
			data = IAPU_SyncRead( pme->fds, addr );
		}
	}
	if( pme->exsound_select & 0x08 ) {
		if( addr >= 0x5000 && addr <= 0x5015 ) {
			data = IAPU_SyncRead( pme->mmc5, addr );
		}
	}

	return	data;
}

void	APU_ExWrite( APU *pme, WORD addr, BYTE data )
{
	APU_SetExQueue( pme, CPU_GetTotalCycles(pme->nes->cpu), addr, data );

	if( pme->exsound_select & 0x04 ) {
		if( addr >= 0x4040 && addr < 0x4100 ) {
			IAPU_SyncWrite( pme->fds, addr, data );
		}
	}

	if( pme->exsound_select & 0x08 ) {
		if( addr >= 0x5000 && addr <= 0x5015 ) {
			IAPU_SyncWrite( pme->mmc5, addr, data );
		}
	}
}

void	APU_Sync(APU *pme)
{
}

void	APU_SyncDPCM( APU *pme, INT cycles )
{
	IAPU_Sync( pme->internal, cycles );

	if( pme->exsound_select & 0x04 ) {
		IAPU_Sync( 	pme->fds, cycles );
	}
	if( pme->exsound_select & 0x08 ) {
		IAPU_Sync( 	pme->mmc5, cycles );
	}
}

void	APU_WriteProcess( APU *pme, WORD addr, BYTE data )
{
	// $4018はVirtuaNES固有ポート
	if( addr >= 0x4000 && addr <= 0x401F ) {
		IAPU_Write( pme->internal, addr, data );
	}
}

void	APU_WriteExProcess( APU *pme, WORD addr, BYTE data )
{
	if( pme->exsound_select & 0x01 ) {
		IAPU_Write( pme->vrc6, addr, data );
	}
	if( pme->exsound_select & 0x02 ) {
		IAPU_Write( pme->vrc7, addr, data );
	}
	if( pme->exsound_select & 0x04 ) {
		IAPU_Write( pme->fds, addr, data );
	}
	if( pme->exsound_select & 0x08 ) {
		IAPU_Write( pme->mmc5, addr, data );
	}
	if( pme->exsound_select & 0x10 ) {
		if( addr == 0x0000 ) {
			BYTE	dummy = IAPU_Read( pme->n106, addr );
		} else {
			IAPU_Write( pme->n106, addr, data );
		}
	}
	if( pme->exsound_select & 0x20 ) {
		IAPU_Write( pme->fme7, addr, data );
	}
}

void	APU_Process( APU *pme, LPBYTE lpBuffer, DWORD dwSize )
{
INT	nBits = APU_SOUND_BITS;
DWORD	dwLength = dwSize / (nBits/8);
INT	output;
QUEUEDATA q;
DWORD	writetime;

LPSHORT	pSoundBuf = pme->m_SoundBuffer;
INT	nCcount = 0;

INT	nFilterType = APU_SOUND_FILTER;

	// Volume setup
	//  0:Master
	//  1:Rectangle 1
	//  2:Rectangle 2
	//  3:Triangle
	//  4:Noise
	//  5:DPCM
	//  6:VRC6
	//  7:VRC7
	//  8:FDS
	//  9:MMC5
	// 10:N106
	// 11:FME7
	INT	vol[24];
	BOOL*	bMute = pme->m_bMute;
	SHORT	nVolume[16];
	int i;
	INT	nMasterVolume; 
	//double	cycle_rate = ((double)FRAME_CYCLES*60.0/12.0)/(double)Config.sound.nRate;
	double	cycle_rate = ((double)pme->nes->nescfg->FrameCycles*60.0/12.0)/(double)APU_SOUND_RATE;
	


	if( !APU_SOUND_ENABLE ) {
		memset( lpBuffer, dwSize, (BYTE)(APU_SOUND_BITS==8?128:0) ); // APU_SOUND_RATE?
		return;
	}
	for(i = 0; i < 16; i++)
		nVolume[i] = APU_SOUND_VOLUME;
	nMasterVolume = bMute[0]?nVolume[0]:0;
	// Internal
	vol[ 0] = bMute[1]?(RECTANGLE_VOL*nVolume[1]*nMasterVolume)/(100*100):0;
	vol[ 1] = bMute[2]?(RECTANGLE_VOL*nVolume[2]*nMasterVolume)/(100*100):0;
	vol[ 2] = bMute[3]?(TRIANGLE_VOL *nVolume[3]*nMasterVolume)/(100*100):0;
	vol[ 3] = bMute[4]?(NOISE_VOL    *nVolume[4]*nMasterVolume)/(100*100):0;
	vol[ 4] = bMute[5]?(DPCM_VOL     *nVolume[5]*nMasterVolume)/(100*100):0;

	// VRC6
	vol[ 5] = bMute[6]?(VRC6_VOL*nVolume[6]*nMasterVolume)/(100*100):0;
	vol[ 6] = bMute[7]?(VRC6_VOL*nVolume[6]*nMasterVolume)/(100*100):0;
	vol[ 7] = bMute[8]?(VRC6_VOL*nVolume[6]*nMasterVolume)/(100*100):0;

	// VRC7
	vol[ 8] = bMute[6]?(VRC7_VOL*nVolume[7]*nMasterVolume)/(100*100):0;

	// FDS
	vol[ 9] = bMute[6]?(FDS_VOL*nVolume[8]*nMasterVolume)/(100*100):0;

	// MMC5
	vol[10] = bMute[6]?(MMC5_VOL*nVolume[9]*nMasterVolume)/(100*100):0;
	vol[11] = bMute[7]?(MMC5_VOL*nVolume[9]*nMasterVolume)/(100*100):0;
	vol[12] = bMute[8]?(MMC5_VOL*nVolume[9]*nMasterVolume)/(100*100):0;

	// N106
	vol[13] = bMute[ 6]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[14] = bMute[ 7]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[15] = bMute[ 8]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[16] = bMute[ 9]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[17] = bMute[10]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[18] = bMute[11]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[19] = bMute[12]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[20] = bMute[13]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;

	// FME7
	vol[21] = bMute[6]?(FME7_VOL*nVolume[11]*nMasterVolume)/(100*100):0;
	vol[22] = bMute[7]?(FME7_VOL*nVolume[11]*nMasterVolume)/(100*100):0;
	vol[23] = bMute[8]?(FME7_VOL*nVolume[11]*nMasterVolume)/(100*100):0;

	// CPUサイクル数がループしてしまった時の対策処理
	if( pme->elapsed_time > CPU_GetTotalCycles(pme->nes->cpu) ) {
		APU_QueueFlush(pme);
	}

	while( dwLength-- ) {
		writetime = (DWORD)pme->elapsed_time;

		while( APU_GetQueue( pme, writetime, &q ) ) {
			APU_WriteProcess( pme, q.addr, q.data );
		}

		while( APU_GetExQueue( pme, writetime, &q ) ) {
			APU_WriteExProcess( pme, q.addr, q.data );
		}

		// 0-4:pme->internal 5-7:VRC6 8:VRC7 9:FDS 10-12:MMC5 13-20:N106 21-23:FME7
		output = 0;
		output += IAPU_Process( pme->internal, 0 )*vol[0];
		output += IAPU_Process( pme->internal, 1 )*vol[1];
		output += IAPU_Process( pme->internal, 2 )*vol[2];
		output += IAPU_Process( pme->internal, 3 )*vol[3];
		output += IAPU_Process( pme->internal, 4 )*vol[4];

		if( pme->exsound_select & 0x01 ) {
			output += IAPU_Process( pme->vrc6, 0 )*vol[5];
			output += IAPU_Process( pme->vrc6, 1 )*vol[6];
			output += IAPU_Process( pme->vrc6, 2 )*vol[7];
		}
		if( pme->exsound_select & 0x02 ) {
			output += IAPU_Process( pme->vrc7, 0 )*vol[8];
		}
		if( pme->exsound_select & 0x04 ) {
			output += IAPU_Process( pme->fds, 0 )*vol[9];
		}
		if( pme->exsound_select & 0x08 ) {
			output += IAPU_Process( pme->mmc5, 0 )*vol[10];
			output += IAPU_Process( pme->mmc5, 1 )*vol[11];
			output += IAPU_Process( pme->mmc5, 2 )*vol[12];
		}
		if( pme->exsound_select & 0x10 ) {
			output += IAPU_Process( pme->n106, 0 )*vol[13];
			output += IAPU_Process( pme->n106, 1 )*vol[14];
			output += IAPU_Process( pme->n106, 2 )*vol[15];
			output += IAPU_Process( pme->n106, 3 )*vol[16];
			output += IAPU_Process( pme->n106, 4 )*vol[17];
			output += IAPU_Process( pme->n106, 5 )*vol[18];
			output += IAPU_Process( pme->n106, 6 )*vol[19];
			output += IAPU_Process( pme->n106, 7 )*vol[20];
		}
		if( pme->exsound_select & 0x20 ) {
			IAPU_Process( pme->fme7, 3 );	// Envelope & Noise
			output += IAPU_Process( pme->fme7, 0 )*vol[21];
			output += IAPU_Process( pme->fme7, 1 )*vol[22];
			output += IAPU_Process( pme->fme7, 2 )*vol[23];
		}

		output >>= 8;

		if( nFilterType == 1 ) {
			//ローパスフィルターTYPE 1(Simple)
			output = (pme->lowpass_filter[0]+output)/2;
			pme->lowpass_filter[0] = output;
		} else if( nFilterType == 2 ) {
			//ローパスフィルターTYPE 2(Weighted type 1)
			output = (pme->lowpass_filter[1]+pme->lowpass_filter[0]+output)/3;
			pme->lowpass_filter[1] = pme->lowpass_filter[0];
			pme->lowpass_filter[0] = output;
		} else if( nFilterType == 3 ) {
			//ローパスフィルターTYPE 3(Weighted type 2)
			output = (pme->lowpass_filter[2]+pme->lowpass_filter[1]+pme->lowpass_filter[0]+output)/4;
			pme->lowpass_filter[2] = pme->lowpass_filter[1];
			pme->lowpass_filter[1] = pme->lowpass_filter[0];
			pme->lowpass_filter[0] = output;
		} else if( nFilterType == 4 ) {
			//ローパスフィルターTYPE 4(Weighted type 3)
			output = (pme->lowpass_filter[1]+pme->lowpass_filter[0]*2+output)/4;
			pme->lowpass_filter[1] = pme->lowpass_filter[0];
			pme->lowpass_filter[0] = output;
		}

#if	0
		// DC成分のカット
		{
		static double ave = 0.0, max=0.0, min=0.0;
		double delta;
		delta = (max-min)/32768.0;
		max -= delta;
		min += delta;
		if( output > max ) max = output;
		if( output < min ) min = output;
		ave -= ave/1024.0;
		ave += (max+min)/2048.0;
		output -= (INT)ave;
		}
#endif
#if	1
		// DC成分のカット(HPF TEST)
		{
//		static	double	cutoff = (2.0*3.141592653579*40.0/44100.0);
		static	double	cutofftemp = (2.0*3.141592653579*40.0);
		double	cutoff = cutofftemp/(double)APU_SOUND_RATE;
		static	double	tmp = 0.0;
		double	in, out;

		in = (double)output;
		out = (in - tmp);
		tmp = tmp + cutoff * out;

		output = (INT)out;
		}
#endif
#if	0
		// スパイクノイズの除去(AGC TEST)
		{
		INT	diff = abs(output-pme->last_data);
		if( diff > 0x4000 ) {
			output /= 4;
		} else 
		if( diff > 0x3000 ) {
			output /= 3;
		} else
		if( diff > 0x2000 ) {
			output /= 2;
		}
		pme->last_data = output;
		}
#endif
		// Limit
		if( output > 0x7FFF ) {
			output = 0x7FFF;
		} else if( output < -0x8000 ) {
			output = -0x8000;
		}

		if( nBits != 8 ) {
			*(SHORT*)lpBuffer = (SHORT)output;
			lpBuffer += sizeof(SHORT);
		} else {
			*lpBuffer++ = (output>>8)^0x80;
		}

		if( nCcount < 0x0100 )
			pSoundBuf[nCcount++] = (SHORT)output;

//		elapsedtime += cycle_rate;
		pme->elapsed_time += cycle_rate;
	}

#if	1
	if( pme->elapsed_time > ((pme->nes->nescfg->FrameCycles/24)+CPU_GetTotalCycles(pme->nes->cpu)) ) {
		pme->elapsed_time = CPU_GetTotalCycles(pme->nes->cpu);
	}
	if( (pme->elapsed_time+(pme->nes->nescfg->FrameCycles/6)) < CPU_GetTotalCycles(pme->nes->cpu) ) {
		pme->elapsed_time = CPU_GetTotalCycles(pme->nes->cpu);
	}
#else
	pme->elapsed_time = pme->nes->cpu->GetTotalCycles();
#endif
}

// チャンネルの周波数取得サブルーチン(NSF用)
INT	APU_GetChannelFrequency( APU *pme, INT no )
{
	if( !pme->m_bMute[0] )
		return	0;

	// Internal
	if( no < 5 ) {
		return	pme->m_bMute[no+1]?IAPU_GetFreq( pme->internal, no ):0;
	}
	// VRC6
	if( (pme->exsound_select & 0x01) && no >= 0x0100 && no < 0x0103 ) {
		return	pme->m_bMute[6+(no&0x03)]?IAPU_GetFreq( pme->vrc6, no & 0x03 ):0;
	}
	// FDS
	if( (pme->exsound_select & 0x04) && no == 0x300 ) {
		return	pme->m_bMute[6]?IAPU_GetFreq( pme->fds, 0 ):0;
	}
	// MMC5
	if( (pme->exsound_select & 0x08) && no >= 0x0400 && no < 0x0402 ) {
		return	pme->m_bMute[6+(no&0x03)]?IAPU_GetFreq( pme->mmc5, no & 0x03 ):0;
	}
	// N106
	if( (pme->exsound_select & 0x10) && no >= 0x0500 && no < 0x0508 ) {
		return	pme->m_bMute[6+(no&0x07)]?IAPU_GetFreq( pme->n106, no & 0x07 ):0;
	}
	// FME7
	if( (pme->exsound_select & 0x20) && no >= 0x0600 && no < 0x0603 ) {
		return	pme->m_bMute[6+(no&0x03)]?IAPU_GetFreq( pme->fme7, no & 0x03 ):0;
	}
	// VRC7
	if( (pme->exsound_select & 0x02) && no >= 0x0700 && no < 0x0709 ) {
		return	pme->m_bMute[6]?IAPU_GetFreq(pme->vrc7, no&0x0F):0;
	}
	return	0;
}
LPSHORT APU_GetSoundBuffer(APU *pme)
{
	return pme->m_SoundBuffer; 
}

// For State
void	APU_GetFrameIRQ( APU *pme, INT* Cycle, BYTE* Count, BYTE* Type, BYTE* IRQ, BYTE* Occur )
{
	pme->internal->vtbl->GetFrameIRQ( pme->internal, Cycle, Count, Type, IRQ, Occur );
}

void	APU_SetFrameIRQ( APU *pme, INT Cycle, BYTE Count, BYTE Type, BYTE IRQ, BYTE Occur )
{
	pme->internal->vtbl->SetFrameIRQ( pme->internal, Cycle, Count, Type, IRQ, Occur );
}

// State Save/Load
void	APU_SaveState( APU *pme, LPBYTE p )
{
#ifdef	_DEBUG
LPBYTE	pold = p;
#endif

	// 時間軸を同期させる為Flushする
	APU_QueueFlush(pme);

	IAPU_SaveState( pme->internal, p );
	p += (IAPU_GetStateSize(pme->internal)+15)&(~0x0F);	// Padding

	// VRC6
	if( pme->exsound_select & 0x01 ) {
		IAPU_SaveState( pme->vrc6, p );
		p += (IAPU_GetStateSize(pme->vrc6)+15)&(~0x0F);	// Padding
	}
	// VRC7 (not support)
	if( pme->exsound_select & 0x02 ) {
		IAPU_SaveState( pme->vrc7, p );
		p += (IAPU_GetStateSize(pme->vrc7)+15)&(~0x0F);	// Padding
	}
	// FDS
	if( pme->exsound_select & 0x04 ) {
		IAPU_SaveState( pme->fds, p );
		p += (IAPU_GetStateSize(pme->fds)+15)&(~0x0F);	// Padding
	}
	// MMC5
	if( pme->exsound_select & 0x08 ) {
		IAPU_SaveState( pme->mmc5, p );
		p += (IAPU_GetStateSize(pme->mmc5)+15)&(~0x0F);	// Padding
	}
	// N106
	if( pme->exsound_select & 0x10 ) {
		IAPU_SaveState( pme->n106, p );
		p += (IAPU_GetStateSize(pme->n106)+15)&(~0x0F);	// Padding
	}
	// FME7
	if( pme->exsound_select & 0x20 ) {
		IAPU_SaveState( pme->fme7, p );
		p += (IAPU_GetStateSize(pme->fme7)+15)&(~0x0F);	// Padding
	}

#ifdef	_DEBUG
DebugOut( "SAVE APU SIZE:%d\n", p-pold );
#endif
}

void	APU_LoadState( APU *pme, LPBYTE p )
{
	// 時間軸を同期させる為に消す
	APU_QueueClear(pme);

	IAPU_LoadState( pme->internal, p );
	p += (IAPU_GetStateSize(pme->internal)+15)&(~0x0F);	// Padding

	// VRC6
	if( pme->exsound_select & 0x01 ) {
		IAPU_LoadState( pme->vrc6, p );
		p += (IAPU_GetStateSize(pme->vrc6)+15)&(~0x0F);	// Padding
	}
	// VRC7 (not support)
	if( pme->exsound_select & 0x02 ) {
		IAPU_LoadState( pme->vrc7, p );
		p += (IAPU_GetStateSize(pme->vrc7)+15)&(~0x0F);	// Padding
	}
	// FDS
	if( pme->exsound_select & 0x04 ) {
		IAPU_LoadState( pme->fds, p );
		p += (IAPU_GetStateSize(pme->fds)+15)&(~0x0F);	// Padding
	}
	// MMC5
	if( pme->exsound_select & 0x08 ) {
		IAPU_LoadState( pme->mmc5, p );
		p += (IAPU_GetStateSize(pme->mmc5)+15)&(~0x0F);	// Padding
	}
	// N106
	if( pme->exsound_select & 0x10 ) {
		IAPU_LoadState( pme->n106, p );
		p += (IAPU_GetStateSize(pme->n106)+15)&(~0x0F);	// Padding
	}
	// FME7
	if( pme->exsound_select & 0x20 ) {
		IAPU_LoadState( pme->fme7, p );
		p += (IAPU_GetStateSize(pme->fme7)+15)&(~0x0F);	// Padding
	}
}
