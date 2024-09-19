//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerFuYuan.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerFuYuan Class
//---------------------------------------------------------------------------

#ifndef KPLAYERFUYUAN_H
#define KPLAYERFUYUAN_H




#ifdef _SERVER

class KPlayerFuYuan
{
	friend class KPlayer;
private:
	int		m_nPlayerIndex;					
	int		m_nFuYuanValue;
	
public:
	KPlayerFuYuan();							
	void	Init(int nPlayerIdx);			
	void	SetFuYuanValue(int nValue);			
	int		GetFuYuanValue();					
	void	AddFuYuanValue(int nAdd);			
};

#endif

#ifndef _SERVER
class KPlayerFuYuan
{
	friend class KPlayer;
private:
	int		m_nFuYuanValue;						// gia tri
public:
	void	SetFuYuanValue(int nValue);
	void	AddFuYuanValue(int nAdd);	
	int		GetFuYuanValue()	{return m_nFuYuanValue;}	

	
};
#endif

#endif
