//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTimer.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KTimer_H
#define KTimer_H
//---------------------------------------------------------------------------
#if defined(WIN32) || defined(JX_PLATFORM_SDL)
class ENGINE_API KTimer
#else
class KTimer
#endif
{
private:
#if defined(WIN32) || defined(JX_PLATFORM_SDL)
	LARGE_INTEGER	m_nFrequency;
	LARGE_INTEGER	m_nTimeStart;
	LARGE_INTEGER	m_nTimeStop;
#else
	timeval m_nTimeStart;
	timeval m_nTimeStop;
#endif
	int				m_nFPS;
public:
	KTimer();
	void			Start();
	void			Stop();
	DWORD			GetElapse();
	DWORD			GetElapseFrequency();
	DWORD			GetInterval();
	BOOL			Passed(int nTime);
	BOOL			GetFPS(int* nFPS);
};
#endif
