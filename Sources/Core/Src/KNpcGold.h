//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcGold.h
// Date:	2020.07.23
// Code:	Fong Ki襲
// Desc:	KNpcGold Class
//---------------------------------------------------------------------------

#ifndef _KNPCGOLD_H
#define	_KNPCGOLD_H

#define		defMAX_NPC_GOLD_TYEP	20//8

class KNpcGold
{
	friend class KNpc;
private:
	int		m_nNpcIdx;
	int		m_nIsGold;			
	int		m_nIsGolding;		
	int		m_nGoldType;		

	DWORD	m_dwSkill5ID;			
	int		m_nSkill5Level;				
	int		m_nFireResist;				
	int		m_nFireResistMax;			
	int		m_nColdResist;				
	int		m_nColdResistMax;			
	int		m_nLightingResist;			
	int		m_nLightingResistMax;		
	int		m_nPoisonResist;			
	int		m_nPoisonResistMax;			
	int		m_nPhycicsResist;			
	int		m_nPhycicsResistMax;		
public:
	void	Init(int nIdx);
	void	SetGoldType(BOOL bFlag);	
	void	SetGoldTypeAndBackData();	
	void	RecoverBackData();			
	void	RandChangeGold(unsigned int ran_Range, BYTE m_nGType);			
	int		GetGoldType();			
	void	ChangeGold();
};

struct	KNpcGoldTemplateInfo
{
	int		m_nExp;						// 经验
	int		m_nLife;					// 生命
	int		m_nLifeReplenish;			// 回血
	int		m_nAttackRating;			// 命中
	int		m_nDefense;					// 防御
	int		m_nMinDamage;
	int		m_nMaxDamage;

	int		m_nTreasure;				
	int		m_nWalkSpeed;
	int		m_nRunSpeed;
	int		m_nAttackSpeed;
	int		m_nCastSpeed;

	DWORD	m_dwSkill5ID;			
	char	m_szSkill5Level[32];		
	int		m_nFireResist;				
	int		m_nFireResistMax;			
	int		m_nColdResist;				
	int		m_nColdResistMax;			
	int		m_nLightingResist;			
	int		m_nLightingResistMax;		
	int		m_nPoisonResist;			
	int		m_nPoisonResistMax;			
	int		m_nPhycicsResist;			
	int		m_nPhycicsResistMax;		
};

class KNpcGoldTemplate
{
	friend class KNpcSet;
public:
	KNpcGoldTemplateInfo	m_sInfo[defMAX_NPC_GOLD_TYEP];
	int		m_nEffectTypeNum;
public:
	KNpcGoldTemplate();
	BOOL	Init();
};

#endif
