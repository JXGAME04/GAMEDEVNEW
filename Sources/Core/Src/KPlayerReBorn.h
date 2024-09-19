//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerReBorn.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerRepute Class
//---------------------------------------------------------------------------

#ifndef KPLAYERREBORN_H
#define KPLAYERREBORN_H




#ifdef _SERVER

class KPlayerReBorn
{
	friend class KPlayer;
private:
	int		m_nPlayerIndex;					
	int		m_nReBornValue;
	
public:
	KPlayerReBorn();							
	void	Init(int nPlayerIdx);			
	void	SetReBornValue(int nValue);			
	int		GetReBornValue();					
	void	AddReBornValue(int nAdd);			
};

#endif

#ifndef _SERVER
class KPlayerReBorn
{
	friend class KPlayer;
private:
	int		m_nReBornValue;						// gia tri
public:
	void	SetReBornValue(int nValue);
	void	AddReBornValue(int nAdd);	
	int		GetReBornValue()	{return m_nReBornValue;}	

	
};
#endif

#endif
