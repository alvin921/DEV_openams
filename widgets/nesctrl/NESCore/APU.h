//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES APU core                                                    //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_INCLUDED__
#define	__APU_INCLUDED__


#include "macro.h"


/*
Enable=1
DisableVolumeEffect=0
ExtraSoundEnable=1
SamplingRate=22050
SamplingBits=8
BufferSize=4
FilterType=0
Volume=640064006400640064006400640064006400640064006400640064006400640040
*/

#define APU_SOUND_RATE	22050
#define APU_SOUND_BITS	8
#define APU_SOUND_FILTER	0
#define APU_SOUND_ENABLE	1
#define APU_SOUND_VOLUME	100
#define APU_DISABLE_VOLUME_EFFECT	0
#define APU_EXTRA_SOUND_ENABLE		1
#define APU_CHANGE_TONE		0


//#define	QUEUE_LENGTH	4096
#define	QUEUE_LENGTH	8192


typedef struct APU_struct APU;

APU *APU_New( NES* parent );
void APU_Destroy( APU * pme);


BOOL	APU_SetChannelMute( APU *pme, INT nCH ) ;
void	APU_QueueClear( APU *pme );
void	APU_SoundSetup(APU *pme);
void	APU_Reset(APU *pme);
void	APU_SelectExSound( APU *pme, BYTE data );
BYTE	APU_Read( APU *pme, WORD addr );
void	APU_Write( APU *pme, WORD addr, BYTE data );
BYTE	APU_ExRead( APU *pme, WORD addr );
void	APU_ExWrite( APU *pme, WORD addr, BYTE data );
void	APU_Sync(APU *pme);
void	APU_SyncDPCM( APU *pme, INT cycles );
void	APU_Process( APU *pme, LPBYTE lpBuffer, DWORD dwSize );
// チャンネルの周波数取得サブルーチン(NSF用)
INT	APU_GetChannelFrequency( APU *pme, INT no );
LPSHORT APU_GetSoundBuffer(APU *pme);
// For State
void	APU_GetFrameIRQ( APU *pme, INT* Cycle, BYTE* Count, BYTE* Type, BYTE* IRQ, BYTE* Occur );
void	APU_SetFrameIRQ( APU *pme, INT Cycle, BYTE Count, BYTE Type, BYTE IRQ, BYTE Occur );
// State Save/Load
void	APU_SaveState( APU *pme, LPBYTE p );
void	APU_LoadState( APU *pme, LPBYTE p );



#endif	// !__APU_INCLUDED__
