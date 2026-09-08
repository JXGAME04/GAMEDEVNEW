//---------------------------------------------------------------------------
// [SDL 08/09 2b-3] Am thanh qua miniaudio (ThirdParty\miniaudio, MIT-0) khi JX_PLATFORM_SDL - thay DirectSound + mp3lib.
// Giu NGUYEN giao dien/bo cuc cac lop trong KDSound.h / KWavSound.h / KMusic.h / KMp3Music.h (Core va S3Client khong doi):
//  - KDirectSound : m_pDirectSound chua ma_engine*
//  - KWavSound    : m_Buffer[0] chua MaWav* (PCM + BUFFER_COUNT ma_sound de phat chong)
//  - KMusic/KMp3Music : m_pSoundBuffer chua MaMusic* (ma_decoder tu bo nho + ma_sound stream) -> mp3 CHAY LAI tren x64 (truoc la stub)
// Don vi: nVolume/nPan theo DirectSound (1/100 dB: -10000..0; pan -10000..10000) -> tuyen tinh 10^(v/2000), pan/10000.
// Ban Win32/x64 thuong: tep nay rong (khong dinh nghia JX_PLATFORM_SDL); cac tep KDSound.cpp/KWavSound.cpp/KMusic.cpp/KMp3Music.cpp
// duoc chan #ifndef JX_PLATFORM_SDL nen khong trung dinh nghia.
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_PLATFORM_SDL

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#define MA_NO_FLAC
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_WASAPI
#define MA_ENABLE_AAUDIO
#define MA_ENABLE_OPENSL
#define MA_ENABLE_NULL
#pragma warning(push, 0)
#include "../../../ThirdParty/miniaudio/miniaudio.h"
#pragma warning(pop)

#include <math.h>
#include "KDebug.h"
#include "KMemBase.h"
#include "KPakFile.h"
#include "KWavFile.h"
#include "KDSound.h"
#include "KWavSound.h"
#include "KMusic.h"
#include "KMp3Music.h"

//---------------------------------------------------------------------------
ENGINE_API KDirectSound* g_pDirectSound = NULL;
static ma_engine* s_pMaEngine = NULL;

static float MaVolFromDs(int nVol)
{
	if (nVol <= -10000) return 0.0f;
	if (nVol >= 0) return 1.0f;
	return powf(10.0f, (float)nVol / 2000.0f);
}
static float MaPanFromDs(int nPan)
{
	if (nPan <= -10000) return -1.0f;
	if (nPan >= 10000) return 1.0f;
	return (float)nPan / 10000.0f;
}

//===========================================================================
// KDirectSound -> ma_engine
//===========================================================================
KDirectSound::KDirectSound()
{
	m_pDirectSound = NULL;
	m_pPrimarySoundBuffer = NULL;
}
KDirectSound::~KDirectSound()
{
	Exit();
}
BOOL KDirectSound::Init()
{
	if (s_pMaEngine)
		return TRUE;
	ma_engine* pEngine = new ma_engine;
	ma_engine_config cfg = ma_engine_config_init();
	ma_result r = ma_engine_init(&cfg, pEngine);
	if (r != MA_SUCCESS)
	{
		SDL_Log("[SDL] miniaudio ma_engine_init loi %d", (int)r);
		delete pEngine;
		return FALSE;
	}
	s_pMaEngine = pEngine;
	m_pDirectSound = (LPDIRECTSOUND)pEngine;
	SDL_Log("[SDL] miniaudio %s san sang: %u Hz, %u kenh", MA_VERSION_STRING, ma_engine_get_sample_rate(pEngine), ma_engine_get_channels(pEngine));
	return TRUE;
}
void KDirectSound::Exit()
{
	if (s_pMaEngine)
	{
		ma_engine_uninit(s_pMaEngine);
		delete s_pMaEngine;
		s_pMaEngine = NULL;
	}
	m_pDirectSound = NULL;
}
BOOL KDirectSound::CreateDirectSound() { return Init(); }
BOOL KDirectSound::CreatePrimarySoundBuffer() { return TRUE; }

//===========================================================================
// KWavSound -> PCM trong bo nho + BUFFER_COUNT ma_sound (moi khe mot ma_audio_buffer_ref rieng)
//===========================================================================
struct MaWav
{
	void*				pPcm;
	ma_uint64			uFrames;
	ma_format			fmt;
	ma_uint32			uChannels;
	ma_uint32			uRate;
	ma_audio_buffer_ref	ref[BUFFER_COUNT];
	ma_sound			snd[BUFFER_COUNT];
	bool				bSnd[BUFFER_COUNT];
};
static inline MaWav* WAV(LPDIRECTSOUNDBUFFER* pSlot) { return (MaWav*)pSlot[0]; }

KWavSound::KWavSound()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
		m_Buffer[i] = NULL;
}
KWavSound::~KWavSound()
{
	Free();
}
BOOL KWavSound::Load(LPSTR FileName)
{
	Free();
	if (!s_pMaEngine)
		return FALSE;
	KWavFile WavFile;
	if (!WavFile.Open(FileName))
		return FALSE;
	WAVEFORMATEX fmt;
	g_MemZero(&fmt, sizeof(fmt));
	WavFile.GetPcmWavFormat(&fmt);
	DWORD dwSize = WavFile.GetWaveSize();
	if (dwSize == 0 || fmt.nChannels == 0 || fmt.nChannels > 2 || (fmt.wBitsPerSample != 8 && fmt.wBitsPerSample != 16))
	{
		WavFile.Close();
		return FALSE;
	}
	MaWav* p = new MaWav;
	g_MemZero(p, sizeof(MaWav));
	p->pPcm = malloc(dwSize);
	if (!p->pPcm)
	{
		delete p; WavFile.Close();
		return FALSE;
	}
	DWORD dwRead = WavFile.Read((PBYTE)p->pPcm, dwSize);
	WavFile.Close();
	p->fmt = (fmt.wBitsPerSample == 8) ? ma_format_u8 : ma_format_s16;
	p->uChannels = fmt.nChannels;
	p->uRate = fmt.nSamplesPerSec ? fmt.nSamplesPerSec : 22050;
	p->uFrames = (ma_uint64)dwRead / (fmt.nChannels * (fmt.wBitsPerSample / 8));
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		if (ma_audio_buffer_ref_init(p->fmt, p->uChannels, p->pPcm, p->uFrames, &p->ref[i]) != MA_SUCCESS)
			continue;
		p->ref[i].sampleRate = p->uRate;
		if (ma_sound_init_from_data_source(s_pMaEngine, &p->ref[i], MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_NO_PITCH, NULL, &p->snd[i]) == MA_SUCCESS)
			p->bSnd[i] = true;
	}
	m_Buffer[0] = (LPDIRECTSOUNDBUFFER)p;
	return TRUE;
}
void KWavSound::Free()
{
	MaWav* p = WAV(m_Buffer);
	if (!p)
		return;
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		if (p->bSnd[i])
		{
			ma_sound_stop(&p->snd[i]);
			ma_sound_uninit(&p->snd[i]);
			p->bSnd[i] = false;
		}
		ma_audio_buffer_ref_uninit(&p->ref[i]);
	}
	if (p->pPcm)
		free(p->pPcm);
	delete p;
	m_Buffer[0] = NULL;
}
int KWavSound::GetFreeBuffer()
{
	MaWav* p = WAV(m_Buffer);
	if (!p) return BUFFER_COUNT;
	for (int i = 0; i < BUFFER_COUNT; i++)
		if (p->bSnd[i] && !ma_sound_is_playing(&p->snd[i]))
			return i;
	return BUFFER_COUNT;
}
int KWavSound::GetPlayBuffer()
{
	MaWav* p = WAV(m_Buffer);
	if (!p) return BUFFER_COUNT;
	for (int i = 0; i < BUFFER_COUNT; i++)
		if (p->bSnd[i] && ma_sound_is_playing(&p->snd[i]))
			return i;
	return BUFFER_COUNT;
}
void KWavSound::Play(int nPan, int nVolume, BOOL bLoop)
{
	MaWav* p = WAV(m_Buffer);
	int i = GetFreeBuffer();
	if (!p || i >= BUFFER_COUNT)
		return;
	ma_sound_seek_to_pcm_frame(&p->snd[i], 0);
	ma_sound_set_pan(&p->snd[i], MaPanFromDs(nPan));
	ma_sound_set_volume(&p->snd[i], MaVolFromDs(nVolume));
	ma_sound_set_looping(&p->snd[i], bLoop ? MA_TRUE : MA_FALSE);
	ma_sound_start(&p->snd[i]);
}
void KWavSound::Stop()
{
	MaWav* p = WAV(m_Buffer);
	if (!p) return;
	for (int i = 0; i < BUFFER_COUNT; i++)
		if (p->bSnd[i])
			ma_sound_stop(&p->snd[i]);
}
void KWavSound::SetPan(int nPan)
{
	MaWav* p = WAV(m_Buffer);
	int i = GetPlayBuffer();
	if (!p || i >= BUFFER_COUNT) return;
	ma_sound_set_pan(&p->snd[i], MaPanFromDs(nPan));
}
void KWavSound::SetVolume(int nVolume)
{
	MaWav* p = WAV(m_Buffer);
	int i = GetPlayBuffer();
	if (!p || i >= BUFFER_COUNT) return;
	ma_sound_set_volume(&p->snd[i], MaVolFromDs(nVolume));
}
BOOL KWavSound::IsPlaying()
{
	return GetPlayBuffer() < BUFFER_COUNT;
}

//===========================================================================
// KMusic / KMp3Music -> ma_decoder (tu bo nho, doc qua KPakFile) + ma_sound
//===========================================================================
struct MaMusic
{
	void*		pData;
	size_t		uSize;
	ma_decoder	dec;
	bool		bDec;
	ma_sound	snd;
	bool		bSnd;
	float		fVolume;
};
static inline MaMusic* MUS(LPDIRECTSOUNDBUFFER pSlot) { return (MaMusic*)pSlot; }

static MaMusic* MaMusicOpen(LPSTR FileName)
{
	if (!s_pMaEngine || !FileName || !FileName[0])
		return NULL;
	KPakFile File;
	if (!File.Open(FileName))
	{
		SDL_Log("[SDL] nhac: khong mo duoc %s", FileName);
		return NULL;
	}
	DWORD dwSize = File.Size();
	MaMusic* p = new MaMusic;
	g_MemZero(p, sizeof(MaMusic));
	p->fVolume = 1.0f;
	p->pData = (dwSize > 0) ? malloc(dwSize) : NULL;
	if (!p->pData || File.Read(p->pData, dwSize) != dwSize)
	{
		File.Close();
		if (p->pData) free(p->pData);
		delete p;
		return NULL;
	}
	File.Close();
	p->uSize = dwSize;
	ma_decoder_config dc = ma_decoder_config_init_default();
	if (ma_decoder_init_memory(p->pData, p->uSize, &dc, &p->dec) != MA_SUCCESS)
	{
		SDL_Log("[SDL] nhac: miniaudio khong giai ma duoc %s", FileName);
		free(p->pData); delete p;
		return NULL;
	}
	p->bDec = true;
	if (ma_sound_init_from_data_source(s_pMaEngine, &p->dec, MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_NO_PITCH, NULL, &p->snd) != MA_SUCCESS)
	{
		ma_decoder_uninit(&p->dec); free(p->pData); delete p;
		return NULL;
	}
	p->bSnd = true;
	SDL_Log("[SDL] nhac: %s (%u byte, %u Hz, %u kenh)", FileName, (unsigned)dwSize, p->dec.outputSampleRate, p->dec.outputChannels);
	return p;
}
static void MaMusicClose(MaMusic* p)
{
	if (!p) return;
	if (p->bSnd) { ma_sound_stop(&p->snd); ma_sound_uninit(&p->snd); }
	if (p->bDec) ma_decoder_uninit(&p->dec);
	if (p->pData) free(p->pData);
	delete p;
}

KMusic::KMusic()
{
	g_MemZero(&m_WaveFormat, sizeof(m_WaveFormat));
	m_pSoundBuffer = NULL;
	m_pSoundNotify = NULL;
	for (int i = 0; i < NUMEVENTS; i++) { m_hEvent[i] = NULL; m_PosNotify[i].dwOffset = 0; m_PosNotify[i].hEventNotify = NULL; }
	m_dwBufferSize = 0;
	m_bLoop = FALSE;
}
KMusic::~KMusic()
{
	Close();
}
BOOL KMusic::Open(LPSTR FileName)
{
	Close();
	MaMusic* p = MaMusicOpen(FileName);
	if (!p) return FALSE;
	m_pSoundBuffer = (LPDIRECTSOUNDBUFFER)p;
	return TRUE;
}
void KMusic::Close()
{
	MaMusicClose(MUS(m_pSoundBuffer));
	m_pSoundBuffer = NULL;
}
void KMusic::Play(BOOL bLoop)
{
	MaMusic* p = MUS(m_pSoundBuffer);
	if (!p) return;
	m_bLoop = bLoop;
	ma_sound_set_looping(&p->snd, bLoop ? MA_TRUE : MA_FALSE);
	ma_sound_set_volume(&p->snd, p->fVolume);
	ma_sound_start(&p->snd);
}
void KMusic::Stop()
{
	MaMusic* p = MUS(m_pSoundBuffer);
	if (p) ma_sound_stop(&p->snd);
}
void KMusic::Rewind()
{
	MaMusic* p = MUS(m_pSoundBuffer);
	if (p) ma_sound_seek_to_pcm_frame(&p->snd, 0);
}
void KMusic::SetVolume(LONG lVolume)
{
	MaMusic* p = MUS(m_pSoundBuffer);
	if (!p) return;
	p->fVolume = MaVolFromDs((int)lVolume);
	ma_sound_set_volume(&p->snd, p->fVolume);
}
BOOL KMusic::HandleNotify() { return TRUE; }
BOOL KMusic::IsPlaying()
{
	MaMusic* p = MUS(m_pSoundBuffer);
	return (p && ma_sound_is_playing(&p->snd) && !ma_sound_at_end(&p->snd)) ? TRUE : FALSE;
}
BOOL KMusic::Init() { return TRUE; }
void KMusic::InitSoundFormat() {}
BOOL KMusic::InitSoundBuffer() { return TRUE; }
void KMusic::FreeSoundBuffer() {}
BOOL KMusic::InitSoundNotify() { return TRUE; }
void KMusic::FreeSoundNotify() {}
BOOL KMusic::InitEventHandle() { return TRUE; }
void KMusic::FreeEventHandle() {}
BOOL KMusic::FillBufferWithSound(DWORD dwPos) { return TRUE; }
BOOL KMusic::FillBufferWithSilence() { return TRUE; }
DWORD KMusic::ReadWaveData(LPBYTE lpBuf, DWORD dwLen) { return 0; }
void KMusic::ThreadFunction(void* lpParam) {}

// KMp3Music: cung duong (miniaudio tu nhan mp3/wav theo noi dung)
BOOL KMp3Music::Open(LPSTR FileName)
{
	return KMusic::Open(FileName);
}
void KMp3Music::Seek(int nPercent)
{
	MaMusic* p = MUS(m_pSoundBuffer);
	if (!p) return;
	ma_uint64 uLen = 0;
	if (ma_decoder_get_length_in_pcm_frames(&p->dec, &uLen) == MA_SUCCESS && uLen > 0)
	{
		if (nPercent < 0) nPercent = 0;
		if (nPercent > 100) nPercent = 100;
		ma_sound_seek_to_pcm_frame(&p->snd, uLen * (ma_uint64)nPercent / 100);
	}
}
void KMp3Music::Rewind()
{
	KMusic::Rewind();
}
BOOL  KMp3Music::Mp3Init() { return TRUE; }
BOOL  KMp3Music::Mp3FillBuffer() { return TRUE; }
DWORD KMp3Music::Mp3Decode(PBYTE lpPcmBuf, DWORD dwBufLen) { return 0; }
BOOL  KMp3Music::Mp3FileOpen(LPSTR FileName) { return m_Mp3File.Open(FileName); }
DWORD KMp3Music::Mp3FileRead(PBYTE pBuf, DWORD dwLen) { return m_Mp3File.Read(pBuf, dwLen); }
DWORD KMp3Music::Mp3FileSeek(LONG lOffset) { return m_Mp3File.Seek(lOffset, 0); }
void  KMp3Music::InitSoundFormat() {}
DWORD KMp3Music::ReadWaveData(LPBYTE lpBuf, DWORD dwLen) { return 0; }

#endif // JX_PLATFORM_SDL
