#include "KCore.h"
#include "KSubWorldSet.h"
#include "KOption.h"
#include "../../Represent/iRepresent/iRepresentshell.h"

extern struct iRepresentShell*	g_pRepresent;

#ifndef _SERVER
KOption	Option;

void	KOption::SetGamma(int nGamma)
{
	m_nGamma = nGamma;
	g_pRepresent->SetGamma(nGamma);
}

void KOption::SetSndVolume(int nSndVolume)
{
	if (nSndVolume <= 3)
		m_nSndVolume = 0;
	else if (nSndVolume >= 100)
		m_nSndVolume = 100;
	else if (nSndVolume >= 40)
		m_nSndVolume = 80 + (nSndVolume - 50) * 2 / 5;
	else
		m_nSndVolume = 40 + nSndVolume;
}

//KMapMusic::SetGameVolume modify by wooy
void KOption::SetMusicVolume(int nMusicVolume)
{
	if (nMusicVolume <= 3)
		m_nMusicVolume = 0;
	else if (nMusicVolume >= 100)
		m_nMusicVolume = 100;
	else if (nMusicVolume >= 40)
		m_nMusicVolume = 80 + (nMusicVolume - 50) * 2 / 5;
	else
		m_nMusicVolume = 40 + nMusicVolume;

	g_SubWorldSet.m_cMusic.SetGameVolume(m_nMusicVolume);
}

#endif