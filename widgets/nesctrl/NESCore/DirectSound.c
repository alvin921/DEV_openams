//
// DirectSound class
//
#include "DirectSound.h"


#define PCM_FRAME_DURATION		40		// ms
#define PCM_N_FRAME_PER_SECOND	25



typedef struct	tagSAMPLERATE {
	DWORD	Rate;
	DWORD	Bits;
} SAMPLERATE, *LPSAMPLERATE;

static SAMPLERATE m_SampleRateTable[] = {
	11025, 8,
	22050, 8,
	32000, 8,	//
	44100, 8,
	48000, 8,
	96000, 8,
	192000, 8,
	11025, 16,
	22050, 16,
	32000, 16,	//
	44100, 16,
	48000, 16,
	96000, 16,
	192000, 16,
	0, 0
};


static SAMPLERATE		m_SampleRate;		// ���݃T���v�����O���[�g
static INT 		m_BufferSize;		// �o�b�t�@�T�C�Y(�t���[����)

static int m_waveSize;

#define ALVIN_RELEASE 0

#if ALVIN_RELEASE
static HWAVEOUT m_hWaveOut;
static BYTE *m_waveBuffer[2];
static WAVEHDR m_waveHeader[2];
#endif

volatile BOOL		m_bStreamPlay;	// �X�g���[���Đ����t���O
static volatile BOOL		m_bStreamPause; // �X�g���[���|�[�Y�t���O



void	SetBufferSize( INT nSize ) { m_BufferSize = nSize; }

BOOL	DirectSound_IsStreamPlaying() { return (m_bStreamPlay&&!m_bStreamPause); }

// DirectSound�o�b�t�@�̍쐬
BOOL	InitialBuffer(void)
{
#if ALVIN_RELEASE
	int channel = 1;
	int n_frame = 5;
	LPWAVEHDR pWaveHeader;
	
	
	m_waveSize = channel*m_SampleRate.Bits/8*m_SampleRate.Rate/PCM_N_FRAME_PER_SECOND*n_frame;

	m_waveBuffer[0] = (BYTE*)MALLOC(m_waveSize);
	memset(m_waveBuffer[0], m_SampleRate.Bits == 8?128:0, m_waveSize);
	pWaveHeader = &m_waveHeader[0];
	memset(pWaveHeader, 0, sizeof(WAVEHDR));
	pWaveHeader->dwBufferLength = m_waveSize;
	pWaveHeader->lpData = (char*)m_waveBuffer[0];
	
	m_waveBuffer[1] = (BYTE*)MALLOC(m_waveSize);
	memset(m_waveBuffer[1], m_SampleRate.Bits == 8?128:0, m_waveSize);
	pWaveHeader = &m_waveHeader[1];
	memset(pWaveHeader, 0, sizeof(WAVEHDR));
	pWaveHeader->dwBufferLength = m_waveSize;
	pWaveHeader->lpData = (char*)m_waveBuffer[1];
#endif
	return	TRUE;
}


extern int AudioCallback(void *user_data, void *buf, int size);

#if ALVIN_RELEASE
void CALLBACK waveOutProc( HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
	if(uMsg == WOM_DONE){ 
		LPWAVEHDR pWaveHeader = (LPWAVEHDR)dwParam1;//ϵͳ�Զ�ʶ������һ��WAVEHDR�������
		int nread;

		
		waveOutUnprepareHeader( hwo, pWaveHeader, sizeof(WAVEHDR) );//�����������ô˺���

		//�˴����WAVEHDR��lpdate����
	nread = AudioCallback(NULL, pWaveHeader->lpData, m_waveSize);

		if(nread <= 0){
			// no data
			//memset(pWaveHeader->lpData, DirectSound.m_SampleRate.Bits == 8?128:0, DirectSound.m_waveSize);
			//nread = DirectSound.m_waveSize;
		}else{
			pWaveHeader->dwBufferLength = nread;
			waveOutPrepareHeader( hwo, pWaveHeader, sizeof(WAVEHDR));
			waveOutWrite( hwo, pWaveHeader, sizeof(WAVEHDR) );
		}
		//...
	}
}
#endif

// �T���v�����O���[�g�̐ݒ�
BOOL	SetSamplingRate( DWORD rate, DWORD bits )
{
INT	i;

	i = 0;
	while( m_SampleRateTable[i].Rate != 0 ) {
		if( m_SampleRateTable[i].Rate == rate
		 && m_SampleRateTable[i].Bits == bits ) {
			m_SampleRate.Rate = rate;
			m_SampleRate.Bits = bits;
			return	TRUE;
		}
		i++;
	}
	return	FALSE;
}

// DirectSound�̏�����
BOOL	DirectSound_Initial( DWORD rate, DWORD bits, INT nSize )
{
#if	1
		m_SampleRate.Rate = 22050;
#else
		m_SampleRate.Rate = 44100;
#endif
	
	//	m_SampleRate.Bits = 8;
		m_SampleRate.Bits = 16;
		m_BufferSize = 1;
	//	m_BufferSize = 2;
	
		m_bStreamPlay  = FALSE;
		m_bStreamPause = FALSE;
		
	SetSamplingRate( rate, bits );
	SetBufferSize( nSize );
	if( !InitialBuffer() ) {
		return	FALSE;
	}
	return	TRUE;
}

// DirectSound�̊J��
void	DirectSound_Release(void)
{
	DirectSound_StreamStop();
	
#if ALVIN_RELEASE
	FREE(m_waveBuffer[0]);
	FREE(m_waveBuffer[1]);
#endif
}

// �X�g���[�~���O�Đ�
void	DirectSound_StreamPlay(void)
{
#if ALVIN_RELEASE
	if( !m_bStreamPlay ) {
		// Buffer clear
		WAVEFORMATEX	pcmwf;
		
		memset(&pcmwf, 0, sizeof(pcmwf));
		
		pcmwf.wFormatTag	  = WAVE_FORMAT_PCM;
		pcmwf.nChannels 	  = 1;
		pcmwf.nSamplesPerSec  = m_SampleRate.Rate;
		pcmwf.nBlockAlign	  = 1 * m_SampleRate.Bits/8;
		pcmwf.nAvgBytesPerSec = m_SampleRate.Rate * pcmwf.nBlockAlign;
		pcmwf.wBitsPerSample  = m_SampleRate.Bits;
		
		waveOutOpen( &m_hWaveOut, WAVE_MAPPER, &pcmwf, (DWORD)waveOutProc, 0, CALLBACK_FUNCTION );
		if(m_hWaveOut){
			
			waveOutPrepareHeader( m_hWaveOut, &m_waveHeader[0], sizeof(WAVEHDR));
			waveOutWrite( m_hWaveOut, &m_waveHeader[0], sizeof(WAVEHDR) );
			
			waveOutPrepareHeader( m_hWaveOut, &m_waveHeader[1], sizeof(WAVEHDR));
			waveOutWrite( m_hWaveOut, &m_waveHeader[1], sizeof(WAVEHDR) );


			m_bStreamPlay  = TRUE;
			m_bStreamPause = FALSE;
		}
	}
#endif
}

// �X�g���[�~���O��~
void	DirectSound_StreamStop(void)
{
#if ALVIN_RELEASE
	if( m_bStreamPlay ) {
		m_bStreamPlay  = FALSE;
		m_bStreamPause = FALSE;
		if(m_hWaveOut){
			waveOutPrepareHeader( m_hWaveOut, &m_waveHeader[0], sizeof(WAVEHDR));
			waveOutPrepareHeader( m_hWaveOut, &m_waveHeader[1], sizeof(WAVEHDR));
			
			//waveOutReset(m_hWaveOut);
			waveOutClose(m_hWaveOut);
			m_hWaveOut = NULL;
		}
	}
#endif
}

// �X�g���[�~���O�|�[�Y
void	DirectSound_StreamPause(void)
{
//	DebugOut( "CDirectSound::StreamPause\n" );

#if ALVIN_RELEASE
	if( m_bStreamPlay ) {
		if( !m_bStreamPause ) {
			m_bStreamPause = TRUE;
			waveOutPause(m_hWaveOut);
		}
	}
#endif
}

// �X�g���[�~���O���W���[��
void	DirectSound_StreamResume(void)
{
//	DebugOut( "CDirectSound::StreamResume\n" );
#if ALVIN_RELEASE
	if( m_bStreamPlay ) {
		if( m_bStreamPause ) {
			m_bStreamPause = FALSE;
			waveOutRestart(m_hWaveOut);
		}
	}
#endif
}









