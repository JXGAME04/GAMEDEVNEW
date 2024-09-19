//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerRepute.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerRepute Class
//---------------------------------------------------------------------------

#ifndef KPLAYERREPUTE_H
#define KPLAYERREPUTE_H




#ifdef _SERVER

class KPlayerRepute
{
	friend class KPlayer;
private:
	int		m_nPlayerIndex;					
	int		m_nReputeValue;
	
public:
	KPlayerRepute();							
	void	Init(int nPlayerIdx);			
	void	SetReputeValue(int nValue);			
	int		GetReputeValue();					
	void	AddReputeValue(int nAdd);			
};

#endif

#ifndef _SERVER
class KPlayerRepute
{
	friend class KPlayer;
private:
	int		m_nReputeValue;						// gia tri
public:
	void	SetReputeValue(int nValue);
	void	AddReputeValue(int nAdd);	
	int		GetReputeValue()	{return m_nReputeValue;}	

	
};
#endif

#endif
