//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
// File:	KNpcDeathCalcExp.cpp
// Date:	2020.07.21
// Code:	Fong KiÒu
// Desc:	TÝnh Damage qu¸i khi chÕt vµ tÝnh ®iÓm kinh nghiÖm
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
#include	"KPlayerDef.h"
#include	"KPlayer.h"
#include	"KNpcDeathCalcExp.h"

void	KNpcDeathCalcExp::Init(int nNpcIdx)
{
	m_nNpcIdx = (nNpcIdx > 0 ? nNpcIdx : 0);
	memset(m_sCalcInfo, 0, sizeof(m_sCalcInfo));
}

void	KNpcDeathCalcExp::Active()
{
	if (Npc[m_nNpcIdx].m_Kind != kind_normal)
		return;
	for (int i = 0; i < defMAX_CALC_EXP_NUM; i++)
	{
		if (m_sCalcInfo[i].m_nAttackIdx <= 0)
			continue;
		m_sCalcInfo[i].m_nTime--;
		if (m_sCalcInfo[i].m_nTime <= 0)
		{
			m_sCalcInfo[i].m_nTime = 0;
			m_sCalcInfo[i].m_nAttackIdx = 0;
			m_sCalcInfo[i].m_nTotalDamage = 0;
		}
	}
}

#ifdef _SERVER
void	KNpcDeathCalcExp::AddDamage(int nPlayerIdx, int nDamage)
{
	if (Npc[m_nNpcIdx].m_Kind != kind_normal)
		return;
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || Player[nPlayerIdx].m_nIndex <= 0)
		return;
	if (Player[nPlayerIdx].m_cTeam.m_nFlag)
	{
		nPlayerIdx = g_Team[Player[nPlayerIdx].m_cTeam.m_nID].m_nCaptain;
		if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || Player[nPlayerIdx].m_nIndex <= 0)
			return;
	}
	int		i;
	for (i = 0; i < defMAX_CALC_EXP_NUM; i++)
	{
		if (m_sCalcInfo[i].m_nAttackIdx == nPlayerIdx)
		{
			this->m_sCalcInfo[i].m_nTotalDamage += nDamage;
			this->m_sCalcInfo[i].m_nTime = defMAX_CALC_EXP_TIME;
			return;
		}
	}
	for (i = 0; i < defMAX_CALC_EXP_NUM; i++)
	{
		if (m_sCalcInfo[i].m_nAttackIdx == 0)
			break;
	}
	if (i >= defMAX_CALC_EXP_NUM)
		return;
	m_sCalcInfo[i].m_nAttackIdx = nPlayerIdx;
	m_sCalcInfo[i].m_nTotalDamage = nDamage;
	m_sCalcInfo[i].m_nTime = defMAX_CALC_EXP_TIME;
}

int		KNpcDeathCalcExp::CalcExp(int nAttacker)
{
	if (Npc[m_nNpcIdx].m_CurrentLifeMax <= 0)
		return 0;
	int i, j, nDamage = 0, nMaxPlayer = 0;
	for (i = 0; i < defMAX_CALC_EXP_NUM; i++)
	{
		if (m_sCalcInfo[i].m_nAttackIdx <= 0)
			continue;

		if (Player[m_sCalcInfo[i].m_nAttackIdx].m_cTeam.m_nFlag && Player[m_sCalcInfo[i].m_nAttackIdx].m_cTeam.m_nID >= 0)
		{
			int		nTeam, nPlayer, nDistance, nMinDistance, nPlayerP;

			nPlayer = 0; nPlayerP = 0;
			nMinDistance = PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE;
			nTeam = Player[m_sCalcInfo[i].m_nAttackIdx].m_cTeam.m_nID;

			nDistance = KNpcSet::GetDistanceSquare(m_nNpcIdx, Player[g_Team[nTeam].m_nCaptain].m_nIndex);
			if (nDistance >= 0 && nDistance < nMinDistance)
			{
				nMinDistance = nDistance;
				nPlayerP = g_Team[nTeam].m_nCaptain;
			}

			if (nAttacker == Player[g_Team[nTeam].m_nCaptain].m_nIndex)
			{
				nPlayer = g_Team[nTeam].m_nCaptain;
			}

			for (j = 0; j < MAX_TEAM_MEMBER; j++)
			{
				if (g_Team[nTeam].m_nMember[j] <= 0)
					continue;
				nDistance = KNpcSet::GetDistanceSquare(m_nNpcIdx, Player[g_Team[nTeam].m_nMember[j]].m_nIndex);
				if (nDistance >= 0 && nDistance < nMinDistance)
				{
					nMinDistance = nDistance;
					nPlayerP = g_Team[nTeam].m_nMember[j];
				}

				if (nAttacker == Player[g_Team[nTeam].m_nMember[j]].m_nIndex)
				{
				    nPlayer = g_Team[nTeam].m_nMember[j];
				}
			}

			if (nPlayer > 0)
			{
				if (m_sCalcInfo[i].m_nTotalDamage > nDamage)
				{
					nDamage = m_sCalcInfo[i].m_nTotalDamage;
					nMaxPlayer = nPlayer;
				}
				double nExpAdd = (double)m_sCalcInfo[i].m_nTotalDamage * Npc[m_nNpcIdx].m_Experience / Npc[m_nNpcIdx].m_CurrentLifeMax;
				Player[nPlayer].AddExp(nExpAdd, Npc[m_nNpcIdx].m_Level);
			}
			else if (nPlayerP > 0)
			{
				nPlayer = nPlayerP;
				if (m_sCalcInfo[i].m_nTotalDamage > nDamage)
				{
					nDamage = m_sCalcInfo[i].m_nTotalDamage;
					nMaxPlayer = nPlayer;
				}
				Player[nPlayer].AddExp(m_sCalcInfo[i].m_nTotalDamage * Npc[m_nNpcIdx].m_Experience / Npc[m_nNpcIdx].m_CurrentLifeMax, Npc[m_nNpcIdx].m_Level, TRUE);
			}
		}
		else
		{
			int nDistance = KNpcSet::GetDistanceSquare(m_nNpcIdx, Player[m_sCalcInfo[i].m_nAttackIdx].m_nIndex);
			if (nDistance >= PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE)
				continue;
			if (m_sCalcInfo[i].m_nTotalDamage > nDamage)
			{
				nDamage = m_sCalcInfo[i].m_nTotalDamage;
				nMaxPlayer = m_sCalcInfo[i].m_nAttackIdx;
			}
			double nExpAdd = (double)m_sCalcInfo[i].m_nTotalDamage * Npc[m_nNpcIdx].m_Experience / Npc[m_nNpcIdx].m_CurrentLifeMax;
			Player[m_sCalcInfo[i].m_nAttackIdx].AddExp(nExpAdd, Npc[m_nNpcIdx].m_Level);
		}
	}
	Clear();
	return nMaxPlayer;
}

#endif

void	KNpcDeathCalcExp::Clear()
{
	memset(m_sCalcInfo, 0, sizeof(m_sCalcInfo));
}



