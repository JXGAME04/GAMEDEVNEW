//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2020 by Kingsoft
// File:	KNpcGold.cpp
// Date:	2003.07.23
// Code:	Fong KiÒu
// Desc:	KNpcGold Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpcTemplate.h"
#include	"KNpc.h"
#include	"KNpcGold.h"

#ifdef _SERVER
extern KLuaScript		*g_pNpcLevelScript;
#endif

#ifdef _SERVER
void	KNpcGold::Init(int nIdx)
{
	this->m_nNpcIdx				= nIdx;
	this->m_nIsGold				= FALSE;
	this->m_nIsGolding			= FALSE;
	this->m_nGoldType			= boss_none;

	this->m_dwSkill5ID			= 0;
	this->m_nSkill5Level		= 0;
	this->m_nFireResist			= 0;
	this->m_nFireResistMax		= 0;
	this->m_nColdResist			= 0;
	this->m_nColdResistMax		= 0;
	this->m_nLightingResist		= 0;
	this->m_nLightingResistMax	= 0;
	this->m_nPoisonResist		= 0;
	this->m_nPoisonResistMax	= 0;
	this->m_nPhycicsResist		= 0;
	this->m_nPhycicsResistMax	= 0;
}

void	KNpcGold::SetGoldType(BOOL bFlag)
{
	this->m_nIsGold = (bFlag != 0 ? 1 : 0);
}

void	KNpcGold::SetGoldTypeAndBackData()
{
	m_nIsGold = 1;
	m_nIsGolding = 0;
	m_nGoldType = boss_none;

	m_nFireResist			= Npc[m_nNpcIdx].m_CurrentFireResist;
	m_nFireResistMax		= Npc[m_nNpcIdx].m_CurrentFireResistMax;
	m_nColdResist			= Npc[m_nNpcIdx].m_CurrentColdResist;
	m_nColdResistMax		= Npc[m_nNpcIdx].m_CurrentColdResistMax;
	m_nLightingResist		= Npc[m_nNpcIdx].m_CurrentLightResist;
	m_nLightingResistMax	= Npc[m_nNpcIdx].m_CurrentLightResistMax;
	m_nPoisonResist			= Npc[m_nNpcIdx].m_CurrentPoisonResist;
	m_nPoisonResistMax		= Npc[m_nNpcIdx].m_CurrentPoisonResistMax;
	m_nPhycicsResist		= Npc[m_nNpcIdx].m_CurrentPhysicsResist;
	m_nPhycicsResistMax		= Npc[m_nNpcIdx].m_CurrentPhysicsResistMax;
}

void	KNpcGold::RecoverBackData()
{
	if (!m_nIsGold || !m_nIsGolding)
		return;

	m_nIsGolding = 0;

	Npc[m_nNpcIdx].m_CurrentFireResist			= m_nFireResist;
	Npc[m_nNpcIdx].m_CurrentFireResistMax		= m_nFireResistMax;
	Npc[m_nNpcIdx].m_CurrentColdResist			= m_nColdResist;
	Npc[m_nNpcIdx].m_CurrentColdResistMax		= m_nColdResistMax;
	Npc[m_nNpcIdx].m_CurrentLightResist			= m_nLightingResist;
	Npc[m_nNpcIdx].m_CurrentLightResistMax		= m_nLightingResistMax;
	Npc[m_nNpcIdx].m_CurrentPoisonResist		= m_nPoisonResist;
	Npc[m_nNpcIdx].m_CurrentPoisonResistMax		= m_nPoisonResistMax;
	Npc[m_nNpcIdx].m_CurrentPhysicsResist		= m_nPhycicsResist;
	Npc[m_nNpcIdx].m_CurrentPhysicsResistMax	= m_nPhycicsResistMax;

	Npc[m_nNpcIdx].m_SkillList.RemoveNpcSkill(defNPC_GOLD_SKILL_NO);
	Npc[m_nNpcIdx].SetAuraSkill(0);
	//KNpcTemplate::GenNpcDropRate(&Npc[m_nNpcIdx],SubWorld[Npc[m_nNpcIdx].m_SubWorldIndex].szNormalDropRate);

	if (m_nGoldType < boss_none || m_nGoldType >= NpcSet.m_cGoldTemplate.m_nEffectTypeNum)
		return;

	KNpcGoldTemplateInfo	*pInfo = &NpcSet.m_cGoldTemplate.m_sInfo[m_nGoldType];

	Npc[m_nNpcIdx].m_Experience					/= pInfo->m_nExp;
	Npc[m_nNpcIdx].m_CurrentExperience		/= pInfo->m_nExp;
	Npc[m_nNpcIdx].m_CurrentLifeMax				/= pInfo->m_nLife;
	Npc[m_nNpcIdx].m_CurrentLifeReplenish		/= pInfo->m_nLifeReplenish;
	Npc[m_nNpcIdx].m_CurrentAttackRating		/= pInfo->m_nAttackRating;
	Npc[m_nNpcIdx].m_CurrentDefend				/= pInfo->m_nDefense;
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[0]	/= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[2]	/= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentFireDamage.nValue[0]	/= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentFireDamage.nValue[2]	/= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentColdDamage.nValue[0]	/= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentColdDamage.nValue[2]	/= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentLightDamage.nValue[0]	/= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentLightDamage.nValue[2]	/= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentPoisonDamage.nValue[0]	/= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentPoisonDamage.nValue[2]	/= pInfo->m_nMaxDamage;

	//#magic noi cong
	Npc[m_nNpcIdx].m_PhysicsMagic.nValue[0]	/= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_PhysicsMagic.nValue[2]	/= pInfo->m_nMaxDamage;

	if(Npc[m_nNpcIdx].m_PhysicsMagic.nValue[0] == defNPC_GOLD_FIST_MAGIC_DAMGE)
		Npc[m_nNpcIdx].m_PhysicsMagic.nValue[0] = 0;
	if(Npc[m_nNpcIdx].m_PhysicsMagic.nValue[2] == defNPC_GOLD_FIST_MAGIC_DAMGE)
		Npc[m_nNpcIdx].m_PhysicsMagic.nValue[2] = 0;

	Npc[m_nNpcIdx].m_CurrentFireMagic.nValue[0]	/= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentFireMagic.nValue[2]	/= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentColdMagic.nValue[0]	/= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentColdMagic.nValue[2]	/= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentLightMagic.nValue[0]	/= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentLightMagic.nValue[2]	/= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentPoisonMagic.nValue[0]	/= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentPoisonMagic.nValue[2]	/= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentTreasure	-= pInfo->m_nTreasure;
	Npc[m_nNpcIdx].m_CurrentWalkSpeed	-= pInfo->m_nWalkSpeed;
	Npc[m_nNpcIdx].m_CurrentRunSpeed	-= pInfo->m_nRunSpeed;
	Npc[m_nNpcIdx].m_CurrentAttackSpeed	-= pInfo->m_nAttackSpeed;
	Npc[m_nNpcIdx].m_CurrentCastSpeed	-= pInfo->m_nCastSpeed;

}

void	KNpcGold::ChangeGold()
{
	m_nGoldType = defNPC_GOLD_TYE;
	m_nIsGolding = 1;
}

//---------------------------------------------------------------------------
//edit by phong kieu skill npc gold boss xanh
//---------------------------------------------------------------------------
void	KNpcGold::RandChangeGold(unsigned int ran_Range, BYTE m_nGType)
{
	if (!m_nIsGold || this->m_nIsGolding)
		return;

	unsigned int NPC_GOLD_RATE_2 = SubWorld[Npc[m_nNpcIdx].m_SubWorldIndex].nzAutoGoldenNpc;
	if(ran_Range > 0)
	{
		NPC_GOLD_RATE_2 = ran_Range;
	}
	if (g_Random(200000) >=  (NPC_GOLD_RATE_2))//edit by phong kieu ti le boss xanh
		return;

	if (NpcSet.m_cGoldTemplate.m_nEffectTypeNum <= 0)
		return;

	m_nGoldType = SubWorld[Npc[m_nNpcIdx].m_SubWorldIndex].nzGoldenType;

	if(m_nGType)
		m_nGoldType = m_nGType; //boss vang

	if(m_nGoldType <= boss_none)
		m_nGoldType = g_Random(NpcSet.m_cGoldTemplate.m_nEffectTypeNum);

	m_nIsGolding = 1;

	KNpcGoldTemplateInfo	*pInfo = &NpcSet.m_cGoldTemplate.m_sInfo[m_nGoldType];

	Npc[m_nNpcIdx].m_CurrentFireResist			= pInfo->m_nFireResist;
	Npc[m_nNpcIdx].m_CurrentFireResistMax		= pInfo->m_nFireResistMax;
	Npc[m_nNpcIdx].m_CurrentColdResist			= pInfo->m_nColdResist;
	Npc[m_nNpcIdx].m_CurrentColdResistMax		= pInfo->m_nColdResistMax;
	Npc[m_nNpcIdx].m_CurrentLightResist			= pInfo->m_nLightingResist;
	Npc[m_nNpcIdx].m_CurrentLightResistMax		= pInfo->m_nLightingResistMax;
	Npc[m_nNpcIdx].m_CurrentPoisonResist		= pInfo->m_nPoisonResist;
	Npc[m_nNpcIdx].m_CurrentPoisonResistMax		= pInfo->m_nPoisonResistMax;
	Npc[m_nNpcIdx].m_CurrentPhysicsResist		= pInfo->m_nPhycicsResist;
	Npc[m_nNpcIdx].m_CurrentPhysicsResistMax	= pInfo->m_nPhycicsResistMax;

	if (pInfo->m_dwSkill5ID && pInfo->m_szSkill5Level[0])
	{
		Npc[m_nNpcIdx].m_SkillList.SetNpcSkill(
			defNPC_GOLD_SKILL_NO,
			pInfo->m_dwSkill5ID,
			atoi(pInfo->m_szSkill5Level)
			);
		Npc[m_nNpcIdx].SetAuraSkill(pInfo->m_dwSkill5ID);
	}
	
	//KNpcTemplate::GenNpcDropRate(&Npc[m_nNpcIdx],SubWorld[Npc[m_nNpcIdx].m_SubWorldIndex].szGoldenDropRate);

	Npc[m_nNpcIdx].m_Experience					*= pInfo->m_nExp;
	Npc[m_nNpcIdx].m_CurrentExperience		*= pInfo->m_nExp;
	//Npc[m_nNpcIdx].m_CurrentLifeMax				*= pInfo->m_nLife;
	Npc[m_nNpcIdx].m_CurrentLifeMax = (Npc[m_nNpcIdx].m_CurrentLifeMax / pInfo->m_nLife) * 5;
	Npc[m_nNpcIdx].m_CurrentLifeReplenish		*= pInfo->m_nLifeReplenish;
	Npc[m_nNpcIdx].m_CurrentAttackRating		*= pInfo->m_nAttackRating;
	Npc[m_nNpcIdx].m_CurrentDefend				*= pInfo->m_nDefense;
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[0]	*= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[2]	*= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentFireDamage.nValue[0]	*= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentFireDamage.nValue[2]	*= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentColdDamage.nValue[0]	*= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentColdDamage.nValue[2]	*= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentLightDamage.nValue[0]	*= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentLightDamage.nValue[2]	*= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentPoisonDamage.nValue[0]	*= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentPoisonDamage.nValue[2]	*= pInfo->m_nMaxDamage;

	//#magic noi cong
	if(Npc[m_nNpcIdx].m_PhysicsMagic.nValue[0] == 0)
		Npc[m_nNpcIdx].m_PhysicsMagic.nValue[0] = defNPC_GOLD_FIST_MAGIC_DAMGE;
	if(Npc[m_nNpcIdx].m_PhysicsMagic.nValue[2] == 0)
		Npc[m_nNpcIdx].m_PhysicsMagic.nValue[2] = defNPC_GOLD_FIST_MAGIC_DAMGE;

	Npc[m_nNpcIdx].m_PhysicsMagic.nValue[0]	*= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_PhysicsMagic.nValue[2]	*= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentFireMagic.nValue[0]	*= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentFireMagic.nValue[2]	*= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentColdMagic.nValue[0]	*= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentColdMagic.nValue[2]	*= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentLightMagic.nValue[0]	*= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentLightMagic.nValue[2]	*= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentPoisonMagic.nValue[0]	*= pInfo->m_nMinDamage;
	Npc[m_nNpcIdx].m_CurrentPoisonMagic.nValue[2]	*= pInfo->m_nMaxDamage;

	Npc[m_nNpcIdx].m_CurrentTreasure	+= pInfo->m_nTreasure;
	Npc[m_nNpcIdx].m_CurrentWalkSpeed	+= pInfo->m_nWalkSpeed;
	Npc[m_nNpcIdx].m_CurrentRunSpeed	+= pInfo->m_nRunSpeed;
	Npc[m_nNpcIdx].m_CurrentAttackSpeed	+= pInfo->m_nAttackSpeed;
	Npc[m_nNpcIdx].m_CurrentCastSpeed	+= pInfo->m_nCastSpeed;
	Npc[m_nNpcIdx].m_CurrentLife = Npc[m_nNpcIdx].m_CurrentLifeMax;
}

int		KNpcGold::GetGoldType()
{
	if (!m_nIsGold || !m_nIsGolding)
		return boss_none;

	if(m_nGoldType < defNPC_GOLD_TYE)
		return boss_blue;

	return boss_gold;
}
#endif

KNpcGoldTemplate::KNpcGoldTemplate()
{
	memset(this->m_sInfo, 0, sizeof(this->m_sInfo));
	this->m_nEffectTypeNum = 0;
}

BOOL	KNpcGoldTemplate::Init()
{
	KTabFile	cFile;
	if (!cFile.Load(NPC_GOLD_TEMPLATE_FILE))
		return FALSE;

	char	szTemp[80];
	int i;
	for (i = 0; i < defMAX_NPC_GOLD_TYEP; i++)
	{
		cFile.GetString(i + 2, 1, "", szTemp, sizeof(szTemp));
		if (szTemp[0] == 0)
			break;

		cFile.GetInteger(i + 2,  2, 1, &m_sInfo[i].m_nExp);
		cFile.GetInteger(i + 2,  3, 1, &m_sInfo[i].m_nLife);
		cFile.GetInteger(i + 2,  4, 1, &m_sInfo[i].m_nLifeReplenish);
		cFile.GetInteger(i + 2,  5, 1, &m_sInfo[i].m_nAttackRating);
		cFile.GetInteger(i + 2,  6, 1, &m_sInfo[i].m_nDefense);
		cFile.GetInteger(i + 2,  7, 1, &m_sInfo[i].m_nMinDamage);
		cFile.GetInteger(i + 2,  8, 1, &m_sInfo[i].m_nMaxDamage);

		cFile.GetInteger(i + 2,  9, 0, &m_sInfo[i].m_nTreasure);
		cFile.GetInteger(i + 2, 10, 0, &m_sInfo[i].m_nWalkSpeed);
		cFile.GetInteger(i + 2, 11, 0, &m_sInfo[i].m_nRunSpeed);
		cFile.GetInteger(i + 2, 12, 0, &m_sInfo[i].m_nAttackSpeed);
		cFile.GetInteger(i + 2, 13, 0, &m_sInfo[i].m_nCastSpeed);

		cFile.GetString(i + 2, 14, "", szTemp, sizeof(szTemp));
		m_sInfo[i].m_dwSkill5ID = KNpcTemplate::SkillString2Id(szTemp);
		cFile.GetString(i + 2, 15, "", m_sInfo[i].m_szSkill5Level, sizeof(m_sInfo[i].m_szSkill5Level));
		cFile.GetInteger(i + 2, 16, 0, &m_sInfo[i].m_nFireResist);
		cFile.GetInteger(i + 2, 17, 0, &m_sInfo[i].m_nFireResistMax);
		cFile.GetInteger(i + 2, 18, 0, &m_sInfo[i].m_nColdResist);
		cFile.GetInteger(i + 2, 19, 0, &m_sInfo[i].m_nColdResistMax);
		cFile.GetInteger(i + 2, 20, 0, &m_sInfo[i].m_nLightingResist);
		cFile.GetInteger(i + 2, 21, 0, &m_sInfo[i].m_nLightingResistMax);
		cFile.GetInteger(i + 2, 22, 0, &m_sInfo[i].m_nPoisonResist);
		cFile.GetInteger(i + 2, 23, 0, &m_sInfo[i].m_nPoisonResistMax);
		cFile.GetInteger(i + 2, 24, 0, &m_sInfo[i].m_nPhycicsResist);
		cFile.GetInteger(i + 2, 25, 0, &m_sInfo[i].m_nPhycicsResistMax);
	}
	this->m_nEffectTypeNum = i;
	return TRUE;
}
















