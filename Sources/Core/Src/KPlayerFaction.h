//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerFaction.h
// Date:	2020.09.26
// Code:	Fong Ki“u
// Desc:	PlayerFaction Class
//---------------------------------------------------------------------------

#ifndef KPLAYERFACTION_H
#define KPLAYERFACTION_H

#include	"KFaction.h"
#include	"KSkillList.h"

class KPlayerFaction
{
public:
	int		m_nCurFaction;					
	int		m_nFirstAddFaction;				
	int		m_nAddTimes;			
public:
	KPlayerFaction();							
	void	Release();
	void	SetSeries(int nSeries);				
	BOOL	AddFaction(int nSeries, char *lpszFactionName);
	BOOL	AddFaction(int nSeries, int nFactionID);
	void	LeaveFaction();						
	BOOL	OpenCurSkillLevel(int nLevel, KSkillList *pSkillList);
	int		GetGurFactionCamp();				
	int		GetCurFactionNo();					
	void	GetCurFactionName(char *lpszGetName);
	void	GetCurFactionValueName(char *lpszGetName);
};

#endif
