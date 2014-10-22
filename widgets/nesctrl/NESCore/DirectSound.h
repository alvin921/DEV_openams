//
// DirectSound class
//
#ifndef	__DIRECTSOUND_INCLUDED__
#define	__DIRECTSOUND_INCLUDED__

#include "macro.h"



	BOOL	DirectSound_Initial( DWORD rate, DWORD bits, INT nSize );
	void	DirectSound_Release(void);

	BOOL	DirectSound_IsStreamPlaying(void) ;
	void	DirectSound_StreamPlay(void);
	void	DirectSound_StreamStop(void);
	void	DirectSound_StreamPause(void);
	void	DirectSound_StreamResume(void);





#endif	// !__DIRECTSOUND_INCLUDED__
