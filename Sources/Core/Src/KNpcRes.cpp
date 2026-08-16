//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcRes.cpp
// Date:	2002.01.06
// Code:	Fong KiÒu
// Desc:	Obj Class
//---------------------------------------------------------------------------

#include "KCore.h"

#ifndef _SERVER

#include "KSprite.h"
#include "KNpc.h"
#include "KNpcResList.h"
#include "KNpcRes.h"
#include "ImgRef.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "scene/KScenePlaceC.h"
#include "KSubWorld.h"
#include "KOption.h"
#include "KMath.h"

#define	PREFIX_MANTLE_LEVEL 1

KNpcRes::KNpcRes()
{
	m_nAction = 0;
	m_nNpcKind = 1;
	m_nHelmType = 0;
	m_nArmorType = 0;
	m_nMantleType = 0;
	m_nWeaponType = 0;
	m_nHorseType = 0;
	m_bRideHorse = FALSE;
	m_nBlurState = 0;
	memset(m_szSoundName, 0, sizeof(m_szSoundName));
	memset(m_nSortTable, 0, sizeof(m_nSortTable));
	m_pSoundNode = NULL;
	m_pWave = NULL;

	m_SceneID_NPCIdx = 0;
	m_SceneID = 0;

	m_nMenuState = 0;
	m_nBackMenuState = 0;
	m_nSleepState = 0;
	//memset(m_szSentence, 0, sizeof(m_szSentence));
	//memset(m_szBackSentence, 0, sizeof(m_szBackSentence));
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	³õÊ¼»¯
//---------------------------------------------------------------------------
BOOL	KNpcRes::Init(char *lpszNpcName, KNpcResList *pNpcResList)
{
	// ³õÊ¼»¯ NpcResNode
	if (!lpszNpcName || !lpszNpcName[0])
		return FALSE;
	m_pcResNode = pNpcResList->AddNpcRes(lpszNpcName);
	if ( m_pcResNode == NULL )
		return FALSE;

	m_nNpcKind = m_pcResNode->GetNpcKind();
	m_nAction = 0;
	m_nHelmType = 0;
	m_nArmorType = 0;
	m_nMantleType = 0;
	m_nWeaponType = 0;
	m_nHorseType = 0;
	m_bRideHorse = FALSE;
	memset(m_szSoundName, 0, sizeof(m_szSoundName));
	memset(m_nSortTable, 0, sizeof(m_nSortTable));
	m_pSoundNode = NULL;
	m_pWave = NULL;

	m_SceneID_NPCIdx = 0;
	m_SceneID = 0;


//	m_pSprNode = NULL;

	int		i;
	char	szBuffer[80];
	for (i = 0; i < MAX_PART; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, 0, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, 0, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, 0, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, 0, 0));

			m_pcResNode->GetFileName(i, m_nAction, 0, "", szBuffer, sizeof(szBuffer), true);
			m_cNpcEffectImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, 0, MAX_PART, true), m_pcResNode->GetTotalDirs(i, m_nAction, 0, MAX_PART, true), m_pcResNode->GetInterval(i, m_nAction, 0, 0, true));
		}
		// Èç¹û´Ë²¿¼þ²»´æÔÚ£¬¶ÔÓ¦µÄÎÄ¼þÃû¶¼Ìî¿Õ
		else
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
	}

	int		nShadowFrame, nShadowDir, nShadowInterval, nShadowCgX, nShadowCgY;
	if ( m_pcResNode->m_cShadowInfo.GetFile(
		m_nAction,
		&nShadowFrame,
		&nShadowDir,
		&nShadowInterval,
		&nShadowCgX,
		&nShadowCgY,
		szBuffer) )
	{
		this->m_cNpcShadow.SetSprFile(szBuffer, nShadowFrame, nShadowDir, nShadowInterval);
		this->m_cNpcShadow.SetCenterPos(nShadowCgX, nShadowCgY);
	}
	else
	{
		this->m_cNpcShadow.Release();
	}

	for (i = 0; i < MAX_SKILL_STATE; i++)
	{
		m_cStateSpr[i].Release();
	}
	m_cSpecialSpr.Release();
	m_cFrameSpr.Release();
	m_cMenuStateSpr.Release();
	m_nMenuState = 0;
	m_nBackMenuState = 0;
	m_nSleepState = 0;
	//memset(m_szSentence, 0, sizeof(m_szSentence));
	//memset(m_szBackSentence, 0, sizeof(m_szBackSentence));

	for (i = 0; i < MAX_NPC_IMAGE_NUM; i++)
	{
		m_cDrawFile[i].nType = ISI_T_SPR;
		m_cDrawFile[i].Color.Color_b.a = 255;
		m_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_cDrawFile[i].uImage = 0;
		m_cDrawFile[i].nISPosition = IMAGE_IS_POSITION_INIT;
		m_cDrawFile[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	}
	return m_cNpcBlur.Init();

}

void	KNpcRes::Remove(int nNpcIdx)
{
	if (m_SceneID)
	{
		g_ScenePlace.RemoveObject(CGOG_NPC, nNpcIdx, m_SceneID);
		m_SceneID = 0;
	}
	m_cNpcBlur.Remove();
}

BOOL	KNpcRes::IgnoreShowRes()
{
	if (m_nNpcKind == NPC_RES_SPECIAL)
	{
		if (!m_bRideHorse && 
			(m_nDoing == cdo_fightstand || 
			m_nDoing == cdo_stand || 
			m_nDoing == cdo_stand1 || 
			m_nDoing == cdo_fightrun || 
			m_nDoing == cdo_walk || 
			m_nDoing == cdo_fightwalk || 
			m_nDoing == cdo_run || 
			m_nDoing == cdo_sit) && 
			(m_pcResNode->GetInterval(5, m_nAction, m_nArmorType, 0) > 1))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void	KNpcRes::Draw(int nNpcIdx, int nDir, int nAllFrame, int nCurFrame, BOOL bInMenu, BOOL bPaintBody)
{
	int		i, nGetFrame = 1, nGetDir = 1, nFirst, nPos;
	int		nCurFrameNo = 0, nCurDirNo = 0;
	int		nScreenX = m_nXpos, nScreenY = m_nYpos, nScreenZ = m_nZpos;

	if (nDir < 0 || nAllFrame < 0 || nCurFrame < 0)
		return;

	if (!m_pcResNode)
		return;

	if (nAllFrame > 0)
	{
		nGetDir = this->m_cNpcShadow.m_nTotalDir;
		if (nGetDir <= 0)
			nGetDir = 1;
		nCurDirNo = (nDir + (32 / nGetDir)) / (64 / nGetDir);
		if (nCurDirNo >= nGetDir)
			nCurDirNo -= nGetDir;
		nGetFrame = this->m_cNpcShadow.m_nTotalFrame;
		nCurFrameNo = nCurDirNo * (nGetFrame / nGetDir) + (nGetFrame / nGetDir) * nCurFrame / nAllFrame;
		this->m_cNpcShadow.SetCurFrame(nCurFrameNo);

		nFirst = 0;

		for (i = 0; i < MAX_PART; i++)
		{
			if (m_cNpcImage[i].CheckExist())
			{
				if (nFirst == 0)
				{
#ifdef TOOLVERSION
					KImageParam	sImage;
					if (g_pRepresent->GetImageParam(m_cNpcImage[i].m_szName, &sImage, ISI_T_SPR))
					{
						m_cNpcImage[i].m_nTotalDir = sImage.nNumFramesGroup;
						m_cNpcImage[i].m_nTotalFrame = sImage.nNumFrames;
					}
					else continue;
#endif					

					nGetDir = m_cNpcImage[i].m_nTotalDir;
					if (nGetDir <= 0)
						nGetDir = 1;

					nCurDirNo = (nDir + (32 / nGetDir)) / (64 / nGetDir);
					if (nCurDirNo >= nGetDir)
						nCurDirNo -= nGetDir;

					nGetFrame = m_cNpcImage[i].m_nTotalFrame;

					nCurFrameNo = nCurDirNo * (nGetFrame / nGetDir) + (nGetFrame / nGetDir) * nCurFrame / nAllFrame;
					m_cNpcImage[i].SetCurFrame(nCurFrameNo);
					nFirst = 1;
				}
				else
				{
					KImageParam	sImage;
					if (g_pRepresent->GetImageParam(m_cNpcImage[i].m_szName, &sImage, ISI_T_SPR))
					{
						m_cNpcImage[i].m_nTotalDir = sImage.nNumFramesGroup;
						m_cNpcImage[i].m_nTotalFrame = sImage.nNumFrames;
						m_cNpcImage[i].SetCurFrame(nCurFrameNo);
					}
				}
			}
			if (m_cNpcEffectImage[i].CheckExist())
			{
				KImageParam	sImage;
				if (g_pRepresent->GetImageParam(m_cNpcEffectImage[i].m_szName, &sImage, ISI_T_SPR))
				{
					m_cNpcEffectImage[i].m_nTotalDir = sImage.nNumFramesGroup;
					m_cNpcEffectImage[i].m_nTotalFrame = sImage.nNumFrames;
					m_cNpcEffectImage[i].m_nTotalDir > 1 ? m_cNpcEffectImage[i].SetCurFrame(nCurFrameNo) : m_cNpcEffectImage[i].GetNextFrame();
				}
			}
		}
	}
	else
	{
		if (m_cNpcShadow.SetCurDir64(nDir))
		{
			m_cNpcShadow.GetNextFrame();
		}

		for (i = 0; i < MAX_PART; i++)
		{
			if (m_cNpcEffectImage[i].CheckExist())
			{
				if (m_cNpcEffectImage[i].SetCurDir64(nDir))
				{
					m_cNpcEffectImage[i].GetNextFrame();
				}
			}
			if ( !m_cNpcImage[i].CheckExist() )
				continue;
			if ( m_cNpcImage[i].SetCurDir64(nDir) )
			{
				m_cNpcImage[i].GetNextFrame();

#ifdef TOOLVERSION
				KImageParam	sImage;
				g_pRepresent->GetImageParam(m_cNpcImage[i].m_szName, &sImage, ISI_T_SPR);
				{
					m_cNpcImage[i].m_nTotalDir = sImage.nNumFramesGroup;
					m_cNpcImage[i].m_nTotalFrame = sImage.nNumFrames;
					nGetDir = sImage.nNumFramesGroup;
					nCurFrameNo = sImage.nNumFrames;
				}
#else
				nCurDirNo = m_cNpcImage[i].m_nCurDir;
				nCurFrameNo = m_cNpcImage[i].m_nCurFrame;
#endif
			}
			
		}
	}

	for (i = 0; i < 18; i++)
		m_cStateSpr[i].m_SprContrul.GetNextFrame();
	if ( m_cSpecialSpr.GetNextFrame(FALSE) )
	{
		if ( m_cSpecialSpr.CheckEnd() )
			m_cSpecialSpr.Release();
	}

	if ( m_cFrameSpr.GetNextFrame(FALSE) )
	{
		if ( m_cFrameSpr.CheckEnd() )
			m_cFrameSpr.Release();
	}

	if (nCurFrame < nAllFrame / 4)
	{
		this->GetSoundName();
		this->PlaySound(this->m_nXpos, this->m_nYpos);
	}

	if ( !m_pcResNode->GetSort(m_nAction, nCurDirNo, nCurFrameNo, m_nSortTable, MAX_PART) )
		return;

// ---------------------------------- ´¦Àí»æÖÆÁÐ±í -------------------------------
	nPos = 0;

	strcpy(m_cDrawFile[nPos].szImage, this->m_cNpcShadow.m_szName);
	m_cDrawFile[nPos].uImage = m_cNpcShadow.m_dwNameID;
	m_cDrawFile[nPos].nFrame = this->m_cNpcShadow.m_nCurFrame;
	m_cDrawFile[nPos].oPosition.nX = nScreenX;
	m_cDrawFile[nPos].oPosition.nY = nScreenY;
	m_cDrawFile[nPos].oPosition.nZ = 0;//nScreenZ;
	nPos++;

	int tmp = -1;
	int tmp1 = -1;
	int tmp2 = -1;
	int tmp3 = -1;
	for (i = 12; i < 18; i++)
	{
		if (m_cStateSpr[i].m_nID && !Option.GetLow(LowMissle))//add by phong kiÒu xö lý kh«ng vÏ skill hiÖu øng d­íi ch©n
		{
			//if (m_cStateSpr[i].m_nID == 56) {
			//	tmp = i;
			//	continue;
			//}
			//if (strstr(m_cStateSpr[i].m_SprContrul.m_szName, "em_10") != NULL) {
			//	tmp3 = i;
			//	continue;
			//}
			//if (strstr(m_cStateSpr[i].m_SprContrul.m_szName, "em_09") != NULL) {
			//	tmp2 = i;
			//	continue;
			//}
			//if (strstr(m_cStateSpr[i].m_SprContrul.m_szName, "em_08") != NULL) {
			//	tmp1 = i;
			//	continue;
			//}
			strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			m_cDrawFile[nPos].oPosition.nZ = 0;
			nPos++;
		}
	}
	/*if (tmp1 != -1 && tmp2 != -1 && tmp3 != -1 && tmp != -1) {
		strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[tmp].m_SprContrul.m_szName);
		m_cDrawFile[nPos].uImage = m_cStateSpr[tmp].m_SprContrul.m_dwNameID;
		m_cDrawFile[nPos].nFrame = m_cStateSpr[tmp].m_SprContrul.m_nCurFrame;
		m_cDrawFile[nPos].oPosition.nX = nScreenX;
		m_cDrawFile[nPos].oPosition.nY = nScreenY;
		m_cDrawFile[nPos].oPosition.nZ = 0;
		nPos++;

		//strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[tmp1].m_SprContrul.m_szName);
		//m_cDrawFile[nPos].uImage = m_cStateSpr[tmp1].m_SprContrul.m_dwNameID;
		//m_cDrawFile[nPos].nFrame = m_cStateSpr[tmp1].m_SprContrul.m_nCurFrame;
		//m_cDrawFile[nPos].oPosition.nX = nScreenX;
		//m_cDrawFile[nPos].oPosition.nY = nScreenY;
		//m_cDrawFile[nPos].oPosition.nZ = 0;
		//nPos++;

		strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[tmp3].m_SprContrul.m_szName);
		m_cDrawFile[nPos].uImage = m_cStateSpr[tmp3].m_SprContrul.m_dwNameID;
		m_cDrawFile[nPos].nFrame = m_cStateSpr[tmp3].m_SprContrul.m_nCurFrame;
		m_cDrawFile[nPos].oPosition.nX = nScreenX;
		m_cDrawFile[nPos].oPosition.nY = nScreenY;
		m_cDrawFile[nPos].oPosition.nZ = 0;
		nPos++;

		//strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[tmp2].m_SprContrul.m_szName);
		//m_cDrawFile[nPos].uImage = m_cStateSpr[tmp2].m_SprContrul.m_dwNameID;
		//m_cDrawFile[nPos].nFrame = m_cStateSpr[tmp2].m_SprContrul.m_nCurFrame;
		//m_cDrawFile[nPos].oPosition.nX = nScreenX;
		//m_cDrawFile[nPos].oPosition.nY = nScreenY;
		//m_cDrawFile[nPos].oPosition.nZ = 0;
		//nPos++;
	}
	else {
		if (tmp1 != -1) {
			strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[tmp1].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage = m_cStateSpr[tmp1].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cStateSpr[tmp1].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			m_cDrawFile[nPos].oPosition.nZ = 0;
			nPos++;
		}
		if (tmp3 != -1) {
			strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[tmp3].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage = m_cStateSpr[tmp3].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cStateSpr[tmp3].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			m_cDrawFile[nPos].oPosition.nZ = 0;
			nPos++;
		}
		if (tmp2 != -1) {
			strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[tmp2].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage = m_cStateSpr[tmp2].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cStateSpr[tmp2].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			m_cDrawFile[nPos].oPosition.nZ = 0;
			nPos++;
		}
		if (tmp != -1) {
			strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[tmp].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage = m_cStateSpr[tmp].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cStateSpr[tmp].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			m_cDrawFile[nPos].oPosition.nZ = 0;
			nPos++;
		}
	}*/
	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu); //vÏ hiÖu øng d­íi ch©n
	nPos = 0;

	for (i = 6; i < 12; i++)
	{
		if (m_cStateSpr[i].m_nID)
		{
			if (m_cStateSpr[i].m_nBackStart <= m_cStateSpr[i].m_SprContrul.m_nCurFrame && 
				m_cStateSpr[i].m_SprContrul.m_nCurFrame < m_cStateSpr[i].m_nBackEnd)
			{
				strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
				m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
				m_cDrawFile[nPos].nFrame = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
				m_cDrawFile[nPos].oPosition.nX = nScreenX;
				m_cDrawFile[nPos].oPosition.nY = nScreenY;
				int nHeightOff = 0;
				if (m_bRideHorse && !Npc[nNpcIdx].m_MaskType)
					nHeightOff += 38;
				m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
				nPos++;
			}
		}
	}

	if(!Option.GetLow(LowMissle))//add by phong kiÒu xö lý kh«ng vÏ skill hiÖu øng gi÷a ng­êi body gåm 2 phÇn ®©y lµ phÇn 2
		g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);
	nPos = 0;

	if (bPaintBody)
	{
		for (i = 0; i < MAX_PART; i++)
		{
			if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
			{
				if (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)
				{
					m_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;
					m_cDrawFile[i].Color.Color_dw = g_pAdjustColorTab[m_ulAdjustColorId - 1];
				}
				else
				{
					m_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
					if (Npc[nNpcIdx].m_HideState.nTime)
						m_cDrawFile[i].Color.Color_b.a = START_BLUR_ALPHA;
					else
						m_cDrawFile[i].Color.Color_b.a = 255;
				}
				strcpy(m_cDrawFile[nPos].szImage, m_cNpcEffectImage[m_nSortTable[i]].m_szName);
				m_cDrawFile[nPos].uImage = m_cNpcEffectImage[m_nSortTable[i]].m_dwNameID;
				m_cDrawFile[nPos].nFrame = m_cNpcEffectImage[m_nSortTable[i]].m_nCurFrame;
				m_cDrawFile[nPos].oPosition.nX = nScreenX;
				m_cDrawFile[nPos].oPosition.nY = nScreenY;
				m_cDrawFile[nPos].oPosition.nZ = nScreenZ;
				nPos++;
			}
		}
		g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);
		nPos = 0;
		for (i = 0; i < MAX_PART; i++)
		{
			if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
			{
				if (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)
				{
					m_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;
					m_cDrawFile[i].Color.Color_dw = g_pAdjustColorTab[m_ulAdjustColorId - 1];
				}
				else
				{
					m_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
					if (Npc[nNpcIdx].m_HideState.nTime)
						m_cDrawFile[i].Color.Color_b.a = START_BLUR_ALPHA;
					else
						m_cDrawFile[i].Color.Color_b.a = 255;
				}

				strcpy(m_cDrawFile[nPos].szImage, m_cNpcImage[m_nSortTable[i]].m_szName);
				m_cDrawFile[nPos].uImage = m_cNpcImage[m_nSortTable[i]].m_dwNameID;
				m_cDrawFile[nPos].nFrame = m_cNpcImage[m_nSortTable[i]].m_nCurFrame;
				m_cDrawFile[nPos].oPosition.nX = nScreenX;
				m_cDrawFile[nPos].oPosition.nY = nScreenY;
				m_cDrawFile[nPos].oPosition.nZ = nScreenZ;
				nPos++;
			}
		}
	}

	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);
	nPos = 0;
	bool gb_skill_150_draw = true;
	for (i = 6; i < 12; i++)
	{
		if (m_cStateSpr[i].m_nID)
		{
			if (m_cStateSpr[i].m_SprContrul.m_nCurFrame < m_cStateSpr[i].m_nBackStart || 
				m_cStateSpr[i].m_SprContrul.m_nCurFrame >= m_cStateSpr[i].m_nBackEnd)
			{
				if (strstr(m_cStateSpr[i].m_SprContrul.m_szName, "em") != NULL  //skill quanh nguoi nga mi
					|| strstr(m_cStateSpr[i].m_SprContrul.m_szName, "others\\c.spr") != NULL //skill quanh nguoi thieu lam
					|| strstr(m_cStateSpr[i].m_SprContrul.m_szName, "haoquang\\tim.spr") != NULL //skill quanh nguoi thieu lam
					|| strstr(m_cStateSpr[i].m_SprContrul.m_szName, "kl_06") != NULL) // skill quanh nguoi con lon
					gb_skill_150_draw = false;
				strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
				m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
				m_cDrawFile[nPos].nFrame = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
				m_cDrawFile[nPos].oPosition.nX = nScreenX;
				m_cDrawFile[nPos].oPosition.nY = nScreenY;
				int nHeightOff = 0;
				if (m_bRideHorse && !Npc[nNpcIdx].m_MaskType)
					nHeightOff += 38;
				m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
				nPos++;
			}
		}
	}

	if (m_cSpecialSpr.m_szName[0])
	{
		if (strstr(m_cSpecialSpr.m_szName, "gb_150_shichengjiulong_a.spr") != NULL) //Drawing gb_150_shichengjiulong_a.spr
		{
			if (gb_skill_150_draw) {
				strcpy(m_cDrawFile[nPos].szImage, m_cSpecialSpr.m_szName);
				m_cDrawFile[nPos].uImage = m_cSpecialSpr.m_dwNameID;
				m_cDrawFile[nPos].nFrame = m_cSpecialSpr.m_nCurFrame;
				m_cDrawFile[nPos].oPosition.nX = nScreenX;
				m_cDrawFile[nPos].oPosition.nY = nScreenY;
				int nHeightOff = 0;
				if (m_bRideHorse && !Npc[nNpcIdx].m_MaskType)
					nHeightOff += 38;
				m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
				nPos++;
			}
		}
		else {
			strcpy(m_cDrawFile[nPos].szImage, m_cSpecialSpr.m_szName);
			m_cDrawFile[nPos].uImage = m_cSpecialSpr.m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cSpecialSpr.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			int nHeightOff = 0;
			if (m_bRideHorse && !Npc[nNpcIdx].m_MaskType)
				nHeightOff += 38;
			m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
			nPos++;
		}
	}

	if (m_cFrameSpr.m_szName[0])
	{
		strcpy(m_cDrawFile[nPos].szImage, m_cFrameSpr.m_szName);
		m_cDrawFile[nPos].uImage = m_cFrameSpr.m_dwNameID;
		m_cDrawFile[nPos].nFrame = m_cFrameSpr.m_nCurFrame;
		m_cDrawFile[nPos].oPosition.nX = nScreenX - m_cFrameSpr.m_nXpos;
		m_cDrawFile[nPos].oPosition.nY = nScreenY - m_cFrameSpr.m_nYpos;
		m_cDrawFile[nPos].oPosition.nZ = nScreenZ + m_nHeightOff; 
		nPos++;  
	}

	if(!Option.GetLow(LowMissle))//add by phong kiÒu xö lý kh«ng vÏ skill hiÖu øng gi÷a ng­êi body
		g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);
	nPos = 0;

	for ( i = 0; i < 6; i++)
	{
		if (m_cStateSpr[i].m_nID)
		{
			strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			int nHeightOff = 22;
			if (m_bRideHorse && !Npc[nNpcIdx].m_MaskType)//#mat na
				nHeightOff += 38;
			if (Npc[nNpcIdx].m_btPlayerTitle || Npc[nNpcIdx].m_btRankBattleId || Npc[nNpcIdx].m_szTongName[0] || Npc[nNpcIdx].m_szTeamMem[0]) //#PlayerTitle
				nHeightOff += 38;
			if (Npc[nNpcIdx].m_MaskType)//#mat na
				nHeightOff += 22;
			if (Npc[nNpcIdx].m_szGameTitle)
				nHeightOff += 19;  // GameTitle (faction level text) - third text line
			if(nScreenZ == 0) 
				nScreenZ = 22; //fix by phong kiÒu 23/08/2020
			m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
			nPos++;
		}
	}

	if(!Option.GetLow(LowMissle))//add by phong kiÒu xö lý kh«ng vÏ skill hiÖu øng trªn ®Çu head
		g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);
	nPos = 0;

// -------------------------------- ´¦Àí»æÖÆÁÐ±í end -----------------------------

// ----------------------------------- ´¦Àí²ÐÓ° ----------------------------------
	int j = 0;
	m_cNpcBlur.ChangeAlpha();
	if (m_nBlurState == TRUE && m_cNpcBlur.NowGetBlur())
	{
		m_cNpcBlur.ClearCurNo();
		for (i = 0, j = 0; i < MAX_PART; i++)
		{
			if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
			{
				m_cNpcBlur.SetFile(j, m_cNpcImage[m_nSortTable[i]].m_szName, m_cNpcImage[m_nSortTable[i]].m_dwNameID, m_cNpcImage[m_nSortTable[i]].m_nCurFrame, nScreenX, nScreenY, nScreenZ);
				j++;
			}
		}
		m_cNpcBlur.SetMapPos(m_nXpos, m_nYpos, m_nZpos, nNpcIdx);

		m_cNpcBlur.SetNextNo();
	}
//	m_cNpcBlur.Draw();
// --------------------------------- ´¦Àí²ÐÓ° end --------------------------------
	// »æÖÆ

//	g_pRepresent->DrawPrimitives(MAX_NPC_IMAGE_NUM, m_cDrawFile, RU_T_IMAGE, bInMenu);	

}

void	KNpcRes::GetShadowName(char *lpszShadow, char *lpszSprName)
{
	KNpcResNode::GetShadowName(lpszShadow, lpszSprName);
}

KNpcRes::~KNpcRes()
{
    // ÒòÎªNPCÖÐ»á×Ô¶¯µ÷ÓÃRemove
    //if (m_SceneID)
    //{
	//    //Remove(m_SceneID_NPCIdx);
    //    m_SceneID_NPCIdx = 0;
    //}
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Éè¶¨Í·¿øÀàÐÍ
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetHelm(int nHelmType)
{
	int		i;
	char	szBuffer[80];

	if (nHelmType < 0)
		return FALSE;
	if ( !m_pcResNode )
		return FALSE;
	if (m_nHelmType == nHelmType)
		return TRUE;
	m_nHelmType = nHelmType;

	for (i = MAX_BODY_PART_SECT * 0; i < MAX_BODY_PART_SECT * 0 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, nHelmType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, nHelmType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, nHelmType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, nHelmType, 0));
		}
		// Èç¹û´Ë²¿¼þ²»´æÔÚ£¬¶ÔÓ¦µÄÎÄ¼þÃû¶¼Ìî¿Õ
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	return TRUE;
}


//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Éè¶¨¿ø¼×ÀàÐÍ
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetArmor(int nArmorType, int nMantleType)
{
	int		i;
	int	nCurMantle;

	if (nArmorType < 0)
		return FALSE;
	if (nMantleType < 0)
		return FALSE;
	if ( !m_pcResNode )
		return FALSE;
	if (m_nNpcKind == NPC_RES_NORMAL && m_nArmorType == nArmorType && m_nMantleType == nMantleType)
		return TRUE;

	m_nArmorType = nArmorType;
	m_nMantleType = nMantleType;

	char	szBuffer[80];
	for (i = MAX_BODY_PART_SECT * 1; i < MAX_BODY_PART_SECT * 1 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			if (i ==4)
			{
				if (m_nMantleType && (IgnoreShowRes()==FALSE))
				{
					m_pcResNode->GetFileName(i, m_nAction, m_nMantleType, "", szBuffer, sizeof(szBuffer));
					m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nMantleType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nMantleType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nMantleType, 0));
				}
				else
				{
					m_cNpcImage[i].Release();
				}
			}
			else
			{
				m_pcResNode->GetFileName(i, m_nAction, nArmorType, "", szBuffer, sizeof(szBuffer));
				m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, nArmorType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, nArmorType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, nArmorType, 0));
			}
		}
		// Èç¹û´Ë²¿¼þ²»´æÔÚ£¬¶ÔÓ¦µÄÎÄ¼þÃû¶¼Ìî¿Õ
		else
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 4; i < MAX_BODY_PART_SECT * 4 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			if ((IgnoreShowRes()==FALSE))
			{
				nCurMantle = m_nMantleType-PREFIX_MANTLE_LEVEL;
				m_pcResNode->GetFileName(i, m_nAction, nCurMantle, "", szBuffer, sizeof(szBuffer));
				m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, nCurMantle, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, nCurMantle, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, nCurMantle, 0));
			}
			else
				m_cNpcImage[i].Release();
		}
		// Èç¹û´Ë²¿¼þ²»´æÔÚ£¬¶ÔÓ¦µÄÎÄ¼þÃû¶¼Ìî¿Õ
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Éè¶¨ÎäÆ÷ÀàÐÍ Res vò khÝ
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetWeapon(int nWeaponType)
{
	int		i;

	if (nWeaponType < 0)
		return FALSE;
	if ( !m_pcResNode )
		return FALSE;
	if (m_nNpcKind == NPC_RES_NORMAL && m_nWeaponType == nWeaponType)
		return TRUE;

	m_nWeaponType = nWeaponType;

	m_nAction = m_pcResNode->GetActNo(m_nDoing, m_nWeaponType, m_bRideHorse);

	char	szBuffer[80];
	int		nFrame, nDir, nInterval, nCgX, nCgY, nCurMantle;

	if ( m_pcResNode->m_cShadowInfo.GetFile(m_nAction, &nFrame, &nDir, &nInterval, &nCgX, &nCgY, szBuffer) )
	{
		m_cNpcShadow.SetSprFile(szBuffer, nFrame, nDir, nInterval);
		m_cNpcShadow.SetCenterPos(nCgX, nCgY);
	}
	else
	{
		m_cNpcShadow.Release();
	}

	for (i = MAX_BODY_PART_SECT * 0; i < MAX_BODY_PART_SECT * 0 + MAX_BODY_PART_SECT; i++)
	{
		if (IgnoreShowRes())
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
		else
		{
			if ( m_pcResNode->CheckPartExist(i) )
			{
				m_pcResNode->GetFileName(i, m_nAction, m_nHelmType, "", szBuffer, sizeof(szBuffer));
				m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHelmType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHelmType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nHelmType, 0));
			}
			else
			{
				m_cNpcImage[i].Release();
			}
		}
	}
	for (i = MAX_BODY_PART_SECT * 1; i < MAX_BODY_PART_SECT * 1 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			if (i ==4)
			{
				if (m_nMantleType && (IgnoreShowRes()==FALSE))
				{
					m_pcResNode->GetFileName(i, m_nAction, m_nMantleType, "", szBuffer, sizeof(szBuffer));
					m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nMantleType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nMantleType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nMantleType, 0));
				}
				else
				{
					m_cNpcImage[i].Release();
					m_cNpcEffectImage[i].Release();
				}
			}
			else
			{
				m_pcResNode->GetFileName(i, m_nAction, m_nArmorType, "", szBuffer, sizeof(szBuffer));
				m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nArmorType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nArmorType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nArmorType, 0));
			}
		}
		else
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 2; i < MAX_BODY_PART_SECT * 2 + MAX_BODY_PART_SECT; i++)
	{
		if (IgnoreShowRes())
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
		else
		{
			if ( m_pcResNode->CheckPartExist(i) )
			{
				m_pcResNode->GetFileName(i, m_nAction, m_nWeaponType, "", szBuffer, sizeof(szBuffer));
				m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nWeaponType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nWeaponType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nWeaponType, 0));

				m_pcResNode->GetFileName(i, m_nAction, m_nWeaponType, "", szBuffer, sizeof(szBuffer),true);
				m_cNpcEffectImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nWeaponType, MAX_PART, true), m_pcResNode->GetTotalDirs(i, m_nAction, m_nWeaponType, MAX_PART, true), m_pcResNode->GetInterval(i, m_nAction, m_nWeaponType, 0,true));
			}
			else
			{
				m_cNpcImage[i].Release();
				m_cNpcEffectImage[i].Release();
			}
		}
	}
	for (i = MAX_BODY_PART_SECT * 3; i < MAX_BODY_PART_SECT * 3 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0));

			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer),true);
			m_cNpcEffectImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, MAX_PART, true), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, MAX_PART, true), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0,true));
		}
		else
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 4; i < MAX_BODY_PART_SECT * 4 + MAX_BODY_PART_SECT; i++)
	{
		if (IgnoreShowRes())
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
		else
		{
			if ( m_pcResNode->CheckPartExist(i) )
			{
				nCurMantle = m_nMantleType-PREFIX_MANTLE_LEVEL;
				m_pcResNode->GetFileName(i, m_nAction, nCurMantle, "", szBuffer, sizeof(szBuffer));
				m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, nCurMantle, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, nCurMantle, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, nCurMantle, 0));
			}
			else
			{
				m_cNpcImage[i].Release();
			}
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Éè¶¨ÂíÆ¥ÀàÐÍ
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetHorse(int nHorseType)
{
	int		i;

	if (nHorseType < 0)
		return FALSE;
	if ( !m_pcResNode )
		return FALSE;
	if (m_nHorseType == nHorseType)
		return TRUE;
	m_nHorseType = nHorseType;

	char	szBuffer[80];
	for (i = MAX_BODY_PART_SECT * 3; i < MAX_BODY_PART_SECT * 3 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0));

			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer), true);
			m_cNpcEffectImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, MAX_PART, true), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, MAX_PART, true), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0, true));
		}
		else
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Éè¶¨¶¯×÷ÀàÐÍ Set hanh dong nhan vat npc
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetAction(int nDoing)
{
	if (!m_pcResNode)
		return FALSE;
	if (nDoing < 0)
		return FALSE;
	if (m_nDoing == nDoing)
		return TRUE;
	m_nDoing = nDoing;

	if (m_pcResNode)
		m_nAction = m_pcResNode->GetActNo(nDoing, m_nWeaponType, m_bRideHorse);

	int		i;
	char	szBuffer[80];
	int		nFrame, nDir, nInterval, nCgX, nCgY, nCurMantle;

	if ( m_pcResNode->m_cShadowInfo.GetFile(m_nAction, &nFrame, &nDir, &nInterval, &nCgX, &nCgY, szBuffer) )
	{
		m_cNpcShadow.SetSprFile(szBuffer, nFrame, nDir, nInterval);
		m_cNpcShadow.SetCenterPos(nCgX, nCgY);
	}
	else
	{
		m_cNpcShadow.Release();
	}

	for (i = MAX_BODY_PART_SECT * 0; i < MAX_BODY_PART_SECT * 0 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHelmType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHelmType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHelmType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nHelmType, 0));
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 1; i < MAX_BODY_PART_SECT * 1 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nArmorType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nArmorType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nArmorType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nArmorType, 0));
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 2; i < MAX_BODY_PART_SECT * 2 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nWeaponType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nWeaponType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nWeaponType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nWeaponType, 0));

			m_pcResNode->GetFileName(i, m_nAction, m_nWeaponType, "", szBuffer, sizeof(szBuffer), true);
			m_cNpcEffectImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nWeaponType, MAX_PART, true), m_pcResNode->GetTotalDirs(i, m_nAction, m_nWeaponType, MAX_PART, true), m_pcResNode->GetInterval(i, m_nAction, m_nWeaponType, 0, true));
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 3; i < MAX_BODY_PART_SECT * 3 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0));

			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer), true);
			m_cNpcEffectImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, MAX_PART, true), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, MAX_PART, true), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0, true));
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 4; i < MAX_BODY_PART_SECT * 4 + MAX_BODY_PART_SECT; i++)
	{
		if (IgnoreShowRes())
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
		else
		{
			if ( m_pcResNode->CheckPartExist(i) )
			{
				nCurMantle = m_nMantleType-PREFIX_MANTLE_LEVEL;
				m_pcResNode->GetFileName(i, m_nAction, nCurMantle, "", szBuffer, sizeof(szBuffer));
				m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, nCurMantle, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, nCurMantle, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, nCurMantle, 0));
			}
			else
			{
				m_cNpcImage[i].Release();
			}
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Éè¶¨ÊÇ·ñÆïÂí
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetRideHorse(BOOL bRideHorse)
{
	if (!m_pcResNode)
		return FALSE;
	if (m_bRideHorse == bRideHorse)
		return TRUE;

	m_bRideHorse = bRideHorse;
	if (m_pcResNode)
		m_nAction = m_pcResNode->GetActNo(m_nDoing, m_nWeaponType, m_bRideHorse);

	int		i;
	char	szBuffer[80];
	int		nFrame, nDir, nInterval, nCgX, nCgY, nCurMantle;

	if ( m_pcResNode->m_cShadowInfo.GetFile(m_nAction, &nFrame, &nDir, &nInterval, &nCgX, &nCgY, szBuffer) )
	{
		m_cNpcShadow.SetSprFile(szBuffer, nFrame, nDir, nInterval);
		m_cNpcShadow.SetCenterPos(nCgX, nCgY);
	}
	else
	{
		m_cNpcShadow.Release();
	}

	for (i = MAX_BODY_PART_SECT * 0; i < MAX_BODY_PART_SECT * 0 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHelmType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHelmType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHelmType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nHelmType, 0));
		}
		else
		{
			m_cNpcEffectImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 1; i < MAX_BODY_PART_SECT * 1 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nArmorType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nArmorType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nArmorType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nArmorType, 0));
		}
		else
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 2; i < MAX_BODY_PART_SECT * 2 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nWeaponType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nWeaponType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nWeaponType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nWeaponType, 0));

			m_pcResNode->GetFileName(i, m_nAction, m_nWeaponType, "", szBuffer, sizeof(szBuffer), true);
			m_cNpcEffectImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nWeaponType, MAX_PART, true), m_pcResNode->GetTotalDirs(i, m_nAction, m_nWeaponType, MAX_PART, true), m_pcResNode->GetInterval(i, m_nAction, m_nWeaponType, 0, true));
		}
		else
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 3; i < MAX_BODY_PART_SECT * 3 + MAX_BODY_PART_SECT; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0));

			m_pcResNode->GetFileName(i, m_nAction, m_nWeaponType, "", szBuffer, sizeof(szBuffer), true);
			m_cNpcEffectImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nWeaponType, MAX_PART, true), m_pcResNode->GetTotalDirs(i, m_nAction, m_nWeaponType, MAX_PART, true), m_pcResNode->GetInterval(i, m_nAction, m_nWeaponType, 0, true));
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 4; i < MAX_BODY_PART_SECT * 4 + MAX_BODY_PART_SECT; i++)
	{
		if (IgnoreShowRes())
		{
			m_cNpcImage[i].Release();
			m_cNpcEffectImage[i].Release();
		}
		else
		{
			if ( m_pcResNode->CheckPartExist(i) )
			{
				nCurMantle = m_nMantleType-PREFIX_MANTLE_LEVEL;
				m_pcResNode->GetFileName(i, m_nAction, nCurMantle, "", szBuffer, sizeof(szBuffer));
				m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, nCurMantle, MAX_PART), m_pcResNode->GetTotalDirs(i, m_nAction, nCurMantle, MAX_PART), m_pcResNode->GetInterval(i, m_nAction, nCurMantle, 0));

				m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer), true);
				m_cNpcEffectImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, MAX_PART, true), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, MAX_PART, true), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0, true));
			}
			else
			{
				m_cNpcImage[i].Release();
				m_cNpcEffectImage[i].Release();
			}
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Éè¶¨ npc Î»ÖÃ
//---------------------------------------------------------------------------
void	KNpcRes::SetPos(int nNpcIdx, int x, int y, int z, BOOL bFocus, BOOL bMenu)
{
	m_nXpos = x;
	m_nYpos = y;
	m_nZpos = z;

	if (bFocus)
		g_ScenePlace.SetFocusPosition(x, y, z);

	if (!bMenu)
    {
		m_SceneID_NPCIdx = nNpcIdx; 
        g_ScenePlace.MoveObject(CGOG_NPC, nNpcIdx, x, y, z, m_SceneID, IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL | IPOT_RL_LIGHT_PROP);

    }
}

void KNpcRes::SetState(BYTE *pNpcStateList, KNpcResList *pNpcResList)//edit by phong kiÒu
{
	if ( !pNpcStateList || !pNpcResList)
		return;

	int		i,j, nFind, nFindFlag[MAX_SKILL_STATE];
	//KStateNode	*pNode;
	int		nType, nPlayType, nBackStart, nBackEnd, nTotalFrame, nTotalDir, nInterVal;
	char	szBuffer[80];

	// ¼ì²éÏÖÓÐµÄÊÇ·ñÒªÍ£Ö¹
	memset(nFindFlag, 0, sizeof(nFindFlag));

	for (i = 0; i < MAX_SKILL_STATE; i++)
	{
		if(*(pNpcStateList + i))
		{
			for(j = 0; j < MAX_SKILL_STATE; j++)
			{
				if (*(pNpcStateList + i) == m_cStateSpr[j].m_nID)
					nFindFlag[j] = 1;
			}
		}
	}
	for (i = 0; i < MAX_SKILL_STATE; i++)
	{
		if ( !nFindFlag[i] && m_cStateSpr[i].m_nID)
			m_cStateSpr[i].Release();
	}

	for (i = 0; i < MAX_SKILL_STATE; i++)
	{
		if (*(pNpcStateList + i) <= 0)
			continue;
		// ¼ì²éÊÇ·ñÒÑ¾­´æÔÚ
		nFind = 0;
		for (j = 0; j < MAX_SKILL_STATE; j++)
		{
			if (*(pNpcStateList + i) == m_cStateSpr[j].m_nID)
			{
				nFind = 1;
				break;
			}
		}
		if (nFind > 0)	
			continue;

		szBuffer[0] = 0;
		pNpcResList->m_cStateTable.GetInfo(*(pNpcStateList + i), szBuffer, &nType, &nPlayType, &nBackStart, &nBackEnd, &nTotalFrame, &nTotalDir, &nInterVal);
		if ( !szBuffer[0] )
			continue;

		if (nType < 0 || nType >= STATE_MAGIC_TYPE_NUM)
			continue;

		for (j = nType * (MAX_SKILL_STATE/3); j < nType * (MAX_SKILL_STATE/3) + (MAX_SKILL_STATE/3); j++)
		{
			if (m_cStateSpr[j].m_nID == 0)
			{
				// Ìí¼ÓÐÂµÄ
				m_cStateSpr[j].Release();
				m_cStateSpr[j].m_nID = *(pNpcStateList + i);
				m_cStateSpr[j].m_nType = nType;
				m_cStateSpr[j].m_nPlayType = nPlayType;
				m_cStateSpr[j].m_nBackStart = nBackStart;
				m_cStateSpr[j].m_nBackEnd = nBackEnd;
				m_cStateSpr[j].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);
				break;
			}
		}
	}
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Éè¶¨ÌØÊâµÄÖ»²¥·ÅÒ»±éµÄËæÉísprÎÄ¼þ
//---------------------------------------------------------------------------
void	KNpcRes::SetSpecialSpr(char *lpszSprName)
{
	KImageParam	sImage;
	g_pRepresent->GetImageParam(lpszSprName, &sImage, ISI_T_SPR);
	if (sImage.nInterval <= 0)
		sImage.nInterval = 1;
	if (sImage.nInterval > 1000)
		sImage.nInterval = 1000;
	if (sImage.nNumFramesGroup <= 0)
		sImage.nNumFramesGroup = 1;
	if (sImage.nNumFrames < sImage.nNumFramesGroup)
		sImage.nNumFrames = sImage.nNumFramesGroup;
	m_cSpecialSpr.SetSprFile(lpszSprName, sImage.nNumFrames, sImage.nNumFramesGroup, (sImage.nNumFrames / sImage.nNumFramesGroup) * sImage.nInterval / 50);
}
void	KNpcRes::SetFrameSpr(char *lpszSprName, int nX, int nY, int nHeight)
{
	KImageParam	sImage;
	g_pRepresent->GetImageParam(lpszSprName, &sImage, ISI_T_SPR);
	if (sImage.nInterval <= 0)
		sImage.nInterval = 1;
	if (sImage.nInterval > 1000)
		sImage.nInterval = 1000;
	if (sImage.nNumFramesGroup <= 0)
		sImage.nNumFramesGroup = 1;
	if (sImage.nNumFrames < sImage.nNumFramesGroup)
		sImage.nNumFrames = sImage.nNumFramesGroup;
	m_cFrameSpr.m_nXpos = nX;
	m_cFrameSpr.m_nYpos = nY;
	m_nHeightOff = nHeight-84;
	m_cFrameSpr.SetSprFile(lpszSprName, sImage.nNumFrames, sImage.nNumFramesGroup, (sImage.nNumFrames / sImage.nNumFramesGroup) * sImage.nInterval / 50);
}
//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºset menu state spr
//---------------------------------------------------------------------------
void	KNpcRes::SetMenuStateSpr(int nMenuState)
{
	if (nMenuState < PLAYER_MENU_STATE_NORMAL || nMenuState >= PLAYER_MENU_STATE_NUM)
	{
		this->m_cMenuStateSpr.Release();
		return;
	}
	char	szName[80];
	g_NpcResList.m_cMenuState.GetStateSpr(nMenuState, szName);
	if (szName[0])
	{
		KImageParam	sImage;
		g_pRepresent->GetImageParam(szName, &sImage, ISI_T_SPR);
		if (sImage.nInterval <= 0)
			sImage.nInterval = 1;
		if (sImage.nInterval > 1000)
			sImage.nInterval = 1000;
		if (sImage.nNumFramesGroup <= 0)
			sImage.nNumFramesGroup = 1;
		if (sImage.nNumFrames < sImage.nNumFramesGroup)
			sImage.nNumFrames = sImage.nNumFramesGroup;
		m_cMenuStateSpr.SetSprFile(szName, sImage.nNumFrames, sImage.nNumFramesGroup, (sImage.nNumFrames / sImage.nNumFramesGroup) * sImage.nInterval / 50);
	}
	else
	{
		this->m_cMenuStateSpr.Release();
	}
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	²ÐÓ°´ò¿ª¹Ø±Õ
//	²ÎÊý£º	bBlur	if == TRUE  ´ò¿ª  if == FLASE  ¹Ø±Õ
//---------------------------------------------------------------------------
void	KNpcRes::SetBlur(BOOL bBlur)
{
	if (m_nBlurState == bBlur)
		return;

	m_nBlurState = bBlur;
	if (bBlur)
	{
		m_cNpcBlur.AddObj();
	}
	else
	{
		m_cNpcBlur.RemoveObj();
	}
}

void KNpcRes::CreateBlur(int nNpcIdx, int nRange, int nDir)
{
	if(nNpcIdx <= 0 || nRange <= 0)
		return;

	int	nSin = g_DirSin(nDir, 64);
	int	nCos = g_DirCos(nDir, 64);

	int nNo, i, j, nBlurRange = 50, nBlurNum = nRange / nBlurRange;
	if(nBlurNum > MAX_BLUR_FRAME)
	{
		nBlurRange += (nBlurNum - MAX_BLUR_FRAME) * nBlurRange / MAX_BLUR_FRAME;
		nBlurNum = MAX_BLUR_FRAME;
	}
	for (nNo = 0; nNo < nBlurNum +1; nNo++)
	{
		int		nScreenX = m_nXpos + ((nCos * nNo * nBlurRange) >> 10);
		int		nScreenY = m_nYpos + ((nSin * nNo * nBlurRange) >> 10);
		int		nScreenZ = m_nZpos;

		//m_cNpcBlur.ClearCurNo();
		for (i = 0, j = 0; i < MAX_PART; i++)
		{
			if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
			{
				m_cNpcBlur.SetFile(j, m_cNpcImage[m_nSortTable[i]].m_szName, m_cNpcImage[m_nSortTable[i]].m_dwNameID, m_cNpcImage[m_nSortTable[i]].m_nCurFrame, nScreenX, nScreenY, nScreenZ, START_BLUR_ALPHA + nNo * BLUR_ALPHA_CHANGE);
				j++;
			}
		}
		m_cNpcBlur.SetMapPos(nScreenX, nScreenY, m_nZpos, nNpcIdx);
		m_cNpcBlur.SetNextNo();
	}
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	»ñµÃµ±Ç°¶¯×÷µÄÒôÐ§ÎÄ¼þÃû
//---------------------------------------------------------------------------
void	KNpcRes::GetSoundName()
{
	if (m_pcResNode)
		m_pcResNode->GetActionSoundName(this->m_nAction, this->m_szSoundName);
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	²¥·Åµ±Ç°¶¯×÷µÄÒôÐ§
//---------------------------------------------------------------------------
void	KNpcRes::PlaySound(int nX, int nY)
{
	if (!m_szSoundName[0])
		return;

	int		nCenterX = 0, nCenterY = 0, nCenterZ = 0;

	g_ScenePlace.GetFocusPosition(nCenterX, nCenterY, nCenterZ);

	m_pSoundNode = (KCacheNode*) g_SoundCache.GetNode(m_szSoundName, (KCacheNode*)m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (m_pWave->IsPlaying())
			return;
		int nVol = -(abs(nX - nCenterX) + abs(nY - nCenterY));
		m_pWave->Play((nX - nCenterX) * 5,  GetSndVolume(nVol), 0);
	}
}

int	KNpcRes::GetSndVolume(int nVol)
{
	return (10000 + nVol) * Option.GetSndVolume() / 100 - 10000;
}

void	KNpcRes::StopSound()
{
	m_pSoundNode = (KCacheNode*)g_SoundCache.GetNode(m_szSoundName, (KCacheNode*)m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		m_pWave->Stop();
	}
}
//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉè¶¨Í·¶¥×´Ì¬	//Head
//---------------------------------------------------------------------------
void	KNpcRes::SetMenuState(int nState, char *lpszSentence, int nSentenceLength)
{
	if (nState < PLAYER_MENU_STATE_NORMAL || nState >= PLAYER_MENU_STATE_NUM)
		return;

	if (nState != m_nMenuState)
	{
		m_nBackMenuState = m_nMenuState;
		//strcpy(m_szBackSentence, m_szSentence);
		m_nMenuState = nState;
	}

	if (nSentenceLength > 0 && lpszSentence)
	{
		if (nSentenceLength >= MAX_SENTENCE_LENGTH)
			nSentenceLength = MAX_SENTENCE_LENGTH - 1;
		//memcpy(m_szSentence, lpszSentence, nSentenceLength);
		//m_szSentence[nSentenceLength] = 0;
	}
	else
	{
		//m_szSentence[0] = 0;
	}

	if (!m_nSleepState)
		SetMenuStateSpr(m_nMenuState);
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º»ñµÃÍ·¶¥×´Ì¬	//Head
//---------------------------------------------------------------------------
int		KNpcRes::GetMenuState()
{
	if (m_nSleepState)
		return m_nSleepState;
	return this->m_nMenuState;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉè¶¨Ë¯Ãß×´Ì¬
//---------------------------------------------------------------------------
void	KNpcRes::SetSleepState(BOOL bFlag)
{
	if (bFlag)
	{
		m_nSleepState = PLAYER_MENU_STATE_IDLE;
		SetMenuStateSpr(m_nSleepState);
	}
	else
	{
		m_nSleepState = 0;
		if (m_nMenuState)
		{
			SetMenuStateSpr(m_nMenuState);
		}
		else
		{
			SetMenuStateSpr(PLAYER_MENU_STATE_NORMAL);
		}
	}
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º»ñµÃË¯Ãß×´Ì¬
//---------------------------------------------------------------------------
BOOL	KNpcRes::GetSleepState()
{
	return (m_nSleepState ? 1 : 0);
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º»æÖÆnpcµÄ±ß¿ò(3DÄ£Ê½ÖÐ¸ÄÎª¼ÓÁÁ)
//---------------------------------------------------------------------------
void	KNpcRes::DrawBorder()
{
	if (!m_pcResNode)
		return;

	int		i, nPos = 0;

	for (i = 0; i < MAX_PART; i++)
	{
		if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
		{
			strcpy(m_cDrawFile[nPos].szImage, m_cNpcImage[m_nSortTable[i]].m_szName);
			m_cDrawFile[nPos].uImage = m_cNpcImage[m_nSortTable[i]].m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cNpcImage[m_nSortTable[i]].m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = m_nXpos;
			m_cDrawFile[nPos].oPosition.nY = m_nYpos;
			m_cDrawFile[nPos].oPosition.nZ = m_nZpos;
			m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_BORDER;
			nPos++;
		}
	}
	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, FALSE);
	for (i = 0; i < nPos; i++)
		m_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	nPos = 0;
}

int	KNpcRes::DrawMenuState(int nHeightOffset) // vÏ tr¹ng th¸i më giao dÞch, ngñ, ®ang giao dÞch
{
	// Add
	if(m_bHideNpc)
	{
		return nHeightOffset;
	}
	if(m_bHidePlayer)
	{
		return nHeightOffset;
	}
	// End add

	int		nScreenX = m_nXpos, nScreenY = m_nYpos, nScreenZ = 0;

	if (!m_pcResNode)
		return nHeightOffset;

	// Í·¶¥×´Ì¬ÌØÐ§ //Head
	int i;
	for ( i = 0; i < 6; i++) //edit by Fong Kieu mac dinh la 2
	{
		if (m_cStateSpr[i].m_nID)
		{
			return nHeightOffset;	//bá dßng nµy ®Ó hiÖn tr¹ng th¸i khi cã hiÖu øng trªn ®Çu
		}
	}

	int nPos = 0;
	nHeightOffset += 10;
	// MenuState
	if (m_cMenuStateSpr.m_szName[0])
	{
		m_cMenuStateSpr.GetNextFrame();

		strcpy(m_cDrawFile[nPos].szImage, m_cMenuStateSpr.m_szName);
		m_cDrawFile[nPos].uImage = m_cMenuStateSpr.m_dwNameID;
		m_cDrawFile[nPos].nFrame = m_cMenuStateSpr.m_nCurFrame;
		m_cDrawFile[nPos].oPosition.nX = nScreenX;
		m_cDrawFile[nPos].oPosition.nY = nScreenY;
		m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOffset;
		nPos++;
	}
	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, false);

	return nHeightOffset;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º¶¯»­Ö¡Êý×ª»»³ÉÂß¼­·½Ïò(0 - 63)
//---------------------------------------------------------------------------
int		KNpcRes::GetNormalNpcStandDir(int nFrame)
{
	if (!m_pcResNode)
		return 0;

	int nTotalFrames = m_pcResNode->GetTotalFrames(NORMAL_NPC_PART_NO, cdo_stand, m_nHelmType, MAX_PART);
	if (nTotalFrames <= 0)
		return 0;

	nFrame %= nTotalFrames;

	return (MAX_NPC_DIR * nFrame) / nTotalFrames;
}


//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	¹¹Ôìº¯Êý
//---------------------------------------------------------------------------
KStateSpr::KStateSpr()
{
	Release();
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Çå¿Õ£¬³õÊ¼»¯
//---------------------------------------------------------------------------
void	KStateSpr::Release()
{
	m_nID = 0;
	m_nType = 0;
	m_nPlayType = 0;
	m_nBackStart = 0;
	m_nBackEnd = 0;
	m_SprContrul.Release();
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	¹¹Ôìº¯Êý
//---------------------------------------------------------------------------
KNpcBlur::KNpcBlur()
{
	m_nActive = 0;
	m_nCurNo = 0;
	m_dwInterval = 3;
	m_dwTimer = 0;
	m_dwLastTick = 0;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Îö¹¹º¯Êý
//---------------------------------------------------------------------------
KNpcBlur::~KNpcBlur()
{
    //Remove();
}


//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	µ±Ç°±àºÅÖ¸ÕëÖ¸ÏòÏÂÒ»¸ö(×Ü¹²7¸ö£¬Ö¸ÕëÑ­»·)
//---------------------------------------------------------------------------
void	KNpcBlur::SetNextNo()
{
	m_nCurNo++;
	if (m_nCurNo >= MAX_BLUR_FRAME)
		m_nCurNo = 0;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Éè¶¨µ±Ç°²ÐÓ°Ö¡µØÍ¼×ø±ê
//---------------------------------------------------------------------------
void	KNpcBlur::SetMapPos(int x, int y, int z, int nNpcIdx)
{
	m_nMapXpos[m_nCurNo] = x;
	m_nMapYpos[m_nCurNo] = y;
	m_nMapZpos[m_nCurNo] = z;
    m_SceneIDNpcIdx[m_nCurNo] = nNpcIdx;
	g_ScenePlace.MoveObject(CGOG_NPC_BLUR_DETAIL(m_nCurNo), nNpcIdx, x, y, z, m_SceneID[m_nCurNo]);
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	¸Ä±äalpha¶È
//---------------------------------------------------------------------------
void	KNpcBlur::ChangeAlpha()
{
	if (m_nActive == 0)
		return;

	int		i, j;
	int		nScreenX, nScreenY, nScreenZ;
	for (i = 0; i < MAX_BLUR_FRAME; i++)
	{
		nScreenX = m_nMapXpos[i];
		nScreenY = m_nMapYpos[i];
		nScreenZ = m_nMapZpos[i];
//		SubWorld[0].Mps2Screen(&nScreenX, &nScreenY);
		for (j = 0; j < MAX_PART; j++)
		{
			if (m_Blur[i][j].Color.Color_b.a)
			{
				m_Blur[i][j].oPosition.nX = nScreenX;
				m_Blur[i][j].oPosition.nY = nScreenY;
				m_Blur[i][j].oPosition.nZ = nScreenZ;
			}
		}
	}

	// Ban goc dem bang so lan VE, nen PaintFps cao lam vet mo tan nhanh hon.
	// Dem theo tick logic de toc do tan giong het ban goc 18 khung/giay.
	if (m_dwLastTick == SubWorld[0].m_dwCurrentTime)
		return;
	m_dwLastTick = SubWorld[0].m_dwCurrentTime;
	m_dwTimer++;
	if (m_dwTimer < m_dwInterval)
		return;
	m_dwTimer = 0;

	m_nActive = 0;
	for (i = 0; i < MAX_BLUR_FRAME; i++)
	{
		for (j = 0; j < MAX_PART; j++)
		{
			if (m_Blur[i][j].Color.Color_b.a)
			{
				if (m_Blur[i][j].Color.Color_b.a > BLUR_ALPHA_CHANGE)
					m_Blur[i][j].Color.Color_b.a -= BLUR_ALPHA_CHANGE;
				else
					m_Blur[i][j].Color.Color_b.a = 0;
				m_nActive = 1;
			}
		}
	}
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Çå¿Õµ±Ç°Ö¸ÕëÖ¸ÏòµÄÄÚÈÝ
//---------------------------------------------------------------------------
void	KNpcBlur::ClearCurNo()
{
	for (int i = 0; i < MAX_PART; i++)
	{
//		m_Blur[m_nCurNo][i].Release();
		m_Blur[m_nCurNo][i].Color.Color_b.a = 0;
	}
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	Éè¶¨µ±Ç°Ä³Ò»ÏîµÄÄÚÈÝ
//---------------------------------------------------------------------------
void	KNpcBlur::SetFile(int nNo, char *lpszFileName, int nSprID, int nFrameNo, int nXpos, int nYpos, int nZpos, int nBlurAlpha/* = START_BLUR_ALPHA*/)
{
	if (nNo < 0 || nNo >= MAX_PART)
		return;
	if (!lpszFileName)
		return;
	strcpy(m_Blur[m_nCurNo][nNo].szImage, lpszFileName);
	m_Blur[m_nCurNo][nNo].uImage = nSprID;
	m_Blur[m_nCurNo][nNo].nFrame = nFrameNo;
	m_Blur[m_nCurNo][nNo].oPosition.nX = nXpos;
	m_Blur[m_nCurNo][nNo].oPosition.nY = nYpos;
	m_Blur[m_nCurNo][nNo].oPosition.nZ = nZpos;
	m_Blur[m_nCurNo][nNo].Color.Color_b.a = nBlurAlpha;
	m_nActive = 1;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	»æÖÆ²ÐÓ°
//---------------------------------------------------------------------------
void	KNpcBlur::Draw(int nIdx)
{
	if (m_nActive == 0)
		return;

	g_pRepresent->DrawPrimitives(MAX_PART, m_Blur[nIdx], RU_T_IMAGE, FALSE);
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º	ÒÀ¾ÝÊ±¼äÅÐ¶ÏÊÇ·ñÈ¡²ÐÓ°
//---------------------------------------------------------------------------
BOOL	KNpcBlur::NowGetBlur()
{
	if (m_dwTimer == 0)
		return TRUE;
	return FALSE;
}

BOOL	KNpcBlur::Init()
{
	for (int i = 0; i < MAX_BLUR_FRAME; i++)
	{
		for (int j = 0; j < MAX_PART; j++)
		{
			m_Blur[i][j].nType = ISI_T_SPR;
			m_Blur[i][j].uImage = 0;
			m_Blur[i][j].nISPosition = IMAGE_IS_POSITION_INIT;
			m_Blur[i][j].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		}
	}
	return TRUE;
}

void	KNpcBlur::Remove()
{
	for (int i = 0; i < MAX_BLUR_FRAME; i++)
	{
		if (m_SceneID[i])
		{
			g_ScenePlace.RemoveObject(CGOG_NPC_BLUR_DETAIL(i), m_SceneIDNpcIdx[i], m_SceneID[i]);
			m_SceneID[i] = 0;
		}
	}
}

void	KNpcBlur::AddObj()
{

}

void	KNpcBlur::RemoveObj()
{

}

#endif