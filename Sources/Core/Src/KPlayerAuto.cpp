//---------------------------------------------------------------------------
// File:	KPlayerAuto.cpp
// Date:	2020.01.05
// Code:	Fong KiÒu
// Desc:	KPlayerAuto.cpp
//---------------------------------------------------------------------------
#include	"KCore.h"
#ifndef _SERVER
#include	"KPlayerAuto.h"
#include	"CoreShell.h"
#include	"KPlayer.h"
#include	"Scene/KScenePlaceC.h"

#include	"KObjSet.h"
#include	"KEngine.h"
#include	"KSubWorldSet.h"
#include	"KTaskFuns.h"
#include	"KPlayerChat.h"
#include	"Math.h"

KPlayerAuto::KPlayerAuto()
{
	Release();
}

KPlayerAuto::~KPlayerAuto()
{
	Release();
}
static bool bMoveToCalled = false; // Flag to track if MoveTo has been called
void KPlayerAuto::Release()
{	
	// Trang thai auto
	
	this->mfk_btState		= FALSE;
	this->mfk_bPause		= FALSE;
	this->m_nAttackType = ATTACK_TYPE_FREE;
	// Tu danh
	this->m_bFightCheckBox	= FALSE;
	this->m_nFightRange		= 1000;
	this->m_bFightSelect = 0;
	//
	this->m_bFightDistance = FALSE;
	this->m_nFightDistance	= 75;
	this->m_bFightDistanceSelect = 0;

	this->m_bFightSelfDef = FALSE;
	this->m_nFightSelfDefValue	= 600;
	this->m_bFightSelfDefSelect = 0;
	
	memset(m_nArrayFocus, 0, sizeof(m_nArrayFocus));
	memset(m_ArrayStateSkill, 0, sizeof(m_ArrayStateSkill));

	m_LeftSkillID = 0;
	m_RightSkillID = 0;
	m_SkillSelfDefID = 0;
	m_SkillBossID = 0;

	m_ShortcutKeySelect1 = 0;
	m_ShortcutKeySelect2 = 0;

	m_nShortcutEditBox1 = 0;
	m_nShortcutEditBox2 = 0;

	m_ShortcutOptSelect1 = 0;
	m_ShortcutOptSelect2 = 0;

	this->m_bSkillRightCheckBox = FALSE;
	this->m_bNRDoSkillCheckBox = FALSE;

	// Hoi phuc sinh luc
	this->m_bReHPCheckBox	= FALSE;
	this->m_nReHPEditBox1		= 100;
	this->m_nReHPEditBox2		= 200;
	this->m_nReHPEditBox3		= 3000;
	// Hoi phuc noi luc
	this->m_bReMPCheckBox	= FALSE;
	this->m_nReMPEditBox1		= 100;
	this->m_nReMPEditBox2		= 50;
	this->m_nReMPEditBox3		= 3000;
	// Tho dia phu
	this->m_bTownHPVCheckBox		= FALSE;
	this->m_nTownHPVEditBox		= 100;
	this->m_bTownMPVCheckBox		= FALSE;
	this->m_nTownMPVEditBox		= 50;
	this->m_bTownHPNCheckBox	= FALSE;
	this->m_bTownMPNCheckBox	= FALSE;
	this->m_bTownFCellCheckBox = FALSE;//®Çy r­¬ng
	this->m_nTownFCellTxtSelect = 0;
	this->m_bTowMoneyCheckBox = FALSE;
	this->m_nTownMoneyEditBox	= 0;
	this->m_bTownAbradeCheckBox = FALSE;
	this->m_nTownAbradeEditBox	= 0;
	//
	this->m_bUseHPCheckBox = FALSE; //c¾n m¸u khi ®Çy r­¬ng
	this->m_bUseAntiToxicCheckBox = FALSE;
	this->m_bUseExpCheckBox = FALSE;	
	this->m_bUseSkillCheckBox = FALSE;
	//
	this->m_bBuffCheckBox = FALSE;
	this->m_nBuffEditBox = 60;
	this->m_bBuffTeamCheckBox = FALSE;
	//
	this->m_bOpenBagHPCheckBox = FALSE;
	this->m_nOpenBagHPEditBox = 6;
	//
	this->m_RingTDPCheckBox = FALSE;
	//
	m_bAutoPickCheckBox = FALSE;			//------------NhÆt ®å --------------------
	m_nAutoPickEditBox = 500;
	m_bPickAllCheckBox = FALSE;
	m_bPickSpeCheckBox = FALSE; //tiÒn vµ ®Æc phÈm
	m_bNoneFightCheckBox = FALSE;
	m_nPickSelTxtSelect = 0;
	m_bFillterItemCheckBox = FALSE;
	memset(m_FilterMagic, 0, sizeof(m_FilterMagic));
	m_bPriceCellCheckBox = FALSE;
	m_nPriceCellEditBox = 0;
	m_bLevelCheckBox = FALSE;
	m_nLevelEditBox = 0;
	m_bRiAmPeCheckBox = FALSE;//gi÷ trang søc
	m_nRiEditBox = 0;
	m_nAmPeEditBox = 0;
	m_bSortBagCheckBox = FALSE;
	m_nSortBagEditBox = 6; //phót tù xÕp hµnh trang
	m_bSortEquipment = FALSE; //biÕn ®iÒu khiÓn khi ®ang xÕp hµnh trang sö dông néi bé ë class nµy
	m_bNPickBackLCheckBox = FALSE;
	memset(m_BlackItemList, 0, sizeof(m_BlackItemList));
	m_bKeepPurpleCheckBox = FALSE;
	m_nKeepPurpleEditBox = 0;
	m_sListEquipment.m_Link.Init(MAX_ITEM);
	//
	m_bFollowCheckBox = FALSE;
	m_nFollowEditBox = 200;
	memset(m_nFollowNameTxt, 0, sizeof(m_nFollowNameTxt));
	m_bLoopPosCheckBox = FALSE;
	m_nXLoopPosTxt = 0;
	m_nYLoopPosTxt = 0;
	m_nLoopPosRangEBox = 600;//ph¹m vi quanh ®iÓm
	m_bConstPosCheckBox = FALSE;
	m_MoveStepTrain 		= 0; //di chuyÓn theo to¹ ®é
	memset(m_MoveMpsTrain, 0, sizeof(m_MoveMpsTrain));
	m_bMoveMapsCheckBox = FALSE;
	m_nMoveMapID = 53; //Ba l¨ng huyÖn
	m_bIRMoveCheckBox = FALSE;	 //lªn ngùa khi di chuyÓn
	m_bFMORCheckBox = FALSE;	//®¸nh qu¸i trªn ®­êng ®i
	m_bDamnMonterCheckBox = FALSE; //qu©y qu¸i
	m_bTalkAnnyCheckBox = FALSE; //nãi vu v¬
	m_bFlCaptainCheckBox = FALSE; //theo sau ®éi tr­ëng
	m_bFlAnnyPTCheckBox = FALSE; //theo sau bÊt kú ai
	//
	m_bReturnCheckBox = FALSE;
	m_bSellCheckBox = FALSE;
	m_bRepairCheckBox = FALSE;
	m_bQuickRepairCheckBox = FALSE;
	m_bGetMoneyCheckBox = FALSE;
	m_nGetMoneyEditBox = 0;
	m_nPwdMoneyEditBox = 123456;
	m_bBuyHPCheckBox = FALSE;
	m_nBuyHPEditBox = 0;
	memset(m_BuyHPTxtSelect, 0, sizeof(m_BuyHPTxtSelect));
	m_bBuyMPCheckBox = FALSE;
	m_nBuyMPEditBox = 0;
	memset(m_BuyMPTxtSelect, 0, sizeof(m_BuyMPTxtSelect));
	m_bBuyToxicCheckBox = FALSE;
	m_nBuyToxicEditBox = 0;
	memset(m_BuyToxicTxtSelect, 0, sizeof(m_BuyToxicTxtSelect));
	m_bBuyTownCheckBox = FALSE;
	m_nBuyTownEditBox = 0;
	m_bMapRunPosCheckBox = FALSE;
	m_nMapTxtId = 53;
	m_MoveMapStep 				= 0; //di chuyÓn ra b·i
	memset(m_MoveMpsMap, 0, sizeof(m_MoveMpsMap));	//to¹ ®é ch¹y ra b·i
	m_AutoMap_Status = AUTO_MAP_STATUS_IDLE;
	m_bKeepMoneyCheckBox = FALSE;
	m_nKeepMoneyEditBox = 0;
	m_bGetFYCheckBox = FALSE;
	m_bGoFarAwayCheckBox = FALSE;
	m_GoFarAwayTxtSelect = 0;
	// To doi
	m_bAutoCTeamCheckBox = FALSE;
	m_bAlwayLeaderCheckBox = FALSE;
	m_bPTAllCheckBox = FALSE;
	m_bDecAllInviCheckBox = FALSE;
	memset(m_AutoPT_PlayerList, 0, sizeof(m_AutoPT_PlayerList));//qu¶n lý tæ ®éi
	m_bLeaveTeamCheckBox = FALSE;
	m_nLeaveTeamEditBox1 = 0;
	m_nLeaveTeamEditBox2= 0;
	m_nLeaveTeamCountDown = 0;
	m_bKickTeamCheckBox = FALSE;
	m_nKickTeamEditBox = 0;
	m_bSCountTeamCheckBox = FALSE;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ClearSzTeamMem();
	m_bPTTongCheckBox = FALSE;
	bMoveToCalled = false; // Flag to track if MoveTo has been called
	//
	ClearNpcArrLag();
	//
	InitFkAutoPos(false);

}

void KPlayerAuto::InitFkAuto(BOOL m_bActive)
{
	mfk_bActive		= m_bActive;
	mfk_bPause		= FALSE;
	mfk_btState		= STATE_WAITING_JOB;
	// Npc
	m_nIndexFocus	= 0;
	memset(m_nArrayFocus, 0, sizeof(m_nArrayFocus)); //m¶ng qu¸i cÇn qu©y auto qu©y qu¸i quay quai
	m_nArrayIndex = 0;	//thø tù tõng con qu¸i trong m¶ng cÇn qu©y
	m_nLifeNpc		= 0;
	m_nTimeRunLag	= 0;
	m_nTimeRunNpc	= 0;
	ClearNpcArrLag();
	// Object
	m_nObjectIndex  = 0;
	m_nHitObject	= 0;
	//
	FkAutoMapSet_StepOne();
	//
	if(m_bActive)
	{
		AutoSendMsg("<color=green>khëi ®éng.<color>");
		InitFkAutoPos(true);
	}
	else
	{
		AutoSendMsg("<color=cyan>kÕt thóc.<color>");
		InitFkAutoPos(false);
	}
}

void KPlayerAuto::InitFkAutoPos(bool b) 
{
    if (b) {
        if (fk_autopos == NULL) 
		{
            fk_autopos = (FK_AUTO_POS*)malloc(FKAUTO_MAX_POS * sizeof(FK_AUTO_POS));
        }

        int count = 0;
        memset(fk_autopos, 0, FKAUTO_MAX_POS * sizeof(FK_AUTO_POS));

        fk_autopos[count].fk_mapID = 78;
        fk_autopos[count].fk_npcType = NPC_PHARMACIES;
        fk_autopos[count].fk_nX[0] = 51552;
        fk_autopos[count].fk_nY[0] = 103840;
        count++;

        fk_autopos[count].fk_mapID = 78;
        fk_autopos[count].fk_npcType = NPC_GROCERY;
        fk_autopos[count].fk_nX[0] = 52096;
        fk_autopos[count].fk_nY[0] = 104352;
        count++;

        fk_autopos[count].fk_mapID = 78;
        fk_autopos[count].fk_npcType = NPC_STORE_BOX;
        fk_autopos[count].fk_nX[0] = 50080;
        fk_autopos[count].fk_nY[0] = 102976;
        count++;

        fk_autopos[count].fk_mapID = 78;
        fk_autopos[count].fk_npcType = NPC_STATION;
        fk_autopos[count].fk_nX[0] = 48320;
        fk_autopos[count].fk_nY[0] = 100512;
        count++;

        fk_autopos[count].fk_mapID = 1;
        fk_autopos[count].fk_npcType = NPC_PHARMACIES;
        fk_autopos[count].fk_nX[0] = 51264;
        fk_autopos[count].fk_nY[0] = 102112;
        count++;

        fk_autopos[count].fk_mapID = 1;
        fk_autopos[count].fk_npcType = NPC_GROCERY;
        fk_autopos[count].fk_nX[0] = 49888;
        fk_autopos[count].fk_nY[0] = 102528;
        count++;

        fk_autopos[count].fk_mapID = 1;
        fk_autopos[count].fk_npcType = NPC_STORE_BOX;
        fk_autopos[count].fk_nX[0] = 50944;
        fk_autopos[count].fk_nY[0] = 101440;
        count++;

        fk_autopos[count].fk_mapID = 1;
        fk_autopos[count].fk_npcType = NPC_STATION;
        fk_autopos[count].fk_nX[0] = 48544;
        fk_autopos[count].fk_nY[0] = 103296;
        count++;

        fk_autopos[count].fk_mapID = 11;
        fk_autopos[count].fk_npcType = NPC_PHARMACIES;
        fk_autopos[count].fk_nX[0] = 100480;
        fk_autopos[count].fk_nY[0] = 164320;
        count++;

        fk_autopos[count].fk_mapID = 11;
        fk_autopos[count].fk_npcType = NPC_GROCERY;
        fk_autopos[count].fk_nX[0] = 99040;
        fk_autopos[count].fk_nY[0] = 164352;
        count++;

        fk_autopos[count].fk_mapID = 11;
        fk_autopos[count].fk_npcType = NPC_STORE_BOX;
        fk_autopos[count].fk_nX[0] = 100960;
        fk_autopos[count].fk_nY[0] = 162464;
        count++;

        fk_autopos[count].fk_mapID = 11;
        fk_autopos[count].fk_npcType = NPC_STATION;
        fk_autopos[count].fk_nX[0] = 96896;
        fk_autopos[count].fk_nY[0] = 158752;
        count++;

        fk_autopos[count].fk_mapID = 162;
        fk_autopos[count].fk_npcType = NPC_PHARMACIES;
        fk_autopos[count].fk_nX[0] = 47936;
        fk_autopos[count].fk_nY[0] = 102720;
        count++;

        fk_autopos[count].fk_mapID = 162;
        fk_autopos[count].fk_npcType = NPC_GROCERY;
        fk_autopos[count].fk_nX[0] = 49152;
        fk_autopos[count].fk_nY[0] = 102432;
        count++;

        fk_autopos[count].fk_mapID = 162;
        fk_autopos[count].fk_npcType = NPC_STORE_BOX;
        fk_autopos[count].fk_nX[0] = 52096;
        fk_autopos[count].fk_nY[0] = 100864;
        count++;

        fk_autopos[count].fk_mapID = 162;
        fk_autopos[count].fk_npcType = NPC_STATION;
        fk_autopos[count].fk_nX[0] = 53376;
        fk_autopos[count].fk_nY[0] = 100032;
        count++;

        fk_autopos[count].fk_mapID = 37;
        fk_autopos[count].fk_npcType = NPC_PHARMACIES;
        fk_autopos[count].fk_nX[0] = 56768;
        fk_autopos[count].fk_nY[0] = 98912;
        count++;

        fk_autopos[count].fk_mapID = 37;
        fk_autopos[count].fk_npcType = NPC_GROCERY;
        fk_autopos[count].fk_nX[0] = 57120;
        fk_autopos[count].fk_nY[0] = 99264;
        count++;

        fk_autopos[count].fk_mapID = 37;
        fk_autopos[count].fk_npcType = NPC_STORE_BOX;
        fk_autopos[count].fk_nX[0] = 55232;
        fk_autopos[count].fk_nY[0] = 98560;
        count++;

        fk_autopos[count].fk_mapID = 37;
        fk_autopos[count].fk_npcType = NPC_STATION;
        fk_autopos[count].fk_nX[0] = 52192;
        fk_autopos[count].fk_nY[0] = 102016;
        count++;

        fk_autopos[count].fk_mapID = 80;
        fk_autopos[count].fk_npcType = NPC_PHARMACIES;
        fk_autopos[count].fk_nX[0] = 56736;
        fk_autopos[count].fk_nY[0] = 98560;
        count++;

        fk_autopos[count].fk_mapID = 80;
        fk_autopos[count].fk_npcType = NPC_GROCERY;
        fk_autopos[count].fk_nX[0] = 54432;
        fk_autopos[count].fk_nY[0] = 96704;
        count++;

        fk_autopos[count].fk_mapID = 80;
        fk_autopos[count].fk_npcType = NPC_STORE_BOX;
        fk_autopos[count].fk_nX[0] = 56192;
        fk_autopos[count].fk_nY[0] = 96224;
        count++;

        fk_autopos[count].fk_mapID = 80;
        fk_autopos[count].fk_npcType = NPC_STATION;
        fk_autopos[count].fk_nX[0] = 58400;
        fk_autopos[count].fk_nY[0] = 98048;
        count++;

        fk_autopos[count].fk_mapID = 176;
        fk_autopos[count].fk_npcType = NPC_PHARMACIES;
        fk_autopos[count].fk_nX[0] = 49440;
        fk_autopos[count].fk_nY[0] = 94816;
        count++;

        fk_autopos[count].fk_mapID = 176;
        fk_autopos[count].fk_npcType = NPC_GROCERY;
        fk_autopos[count].fk_nX[0] = 42880;
        fk_autopos[count].fk_nY[0] = 101344;
        count++;

        fk_autopos[count].fk_mapID = 176;
        fk_autopos[count].fk_npcType = NPC_STORE_BOX;
        fk_autopos[count].fk_nX[0] = 50368;
        fk_autopos[count].fk_nY[0] = 93920;
        count++;

        fk_autopos[count].fk_mapID = 176;
        fk_autopos[count].fk_npcType = NPC_STATION;
        fk_autopos[count].fk_nX[0] = 51296;
        fk_autopos[count].fk_nY[0] = 93216;
        count++;

    } else {
        if (fk_autopos) {
            memset(fk_autopos, 0, FKAUTO_MAX_POS * sizeof(FK_AUTO_POS));
            free(fk_autopos);
            fk_autopos = NULL;
        }
    }
}

int KPlayerAuto::GetFkAutoPos(int npcType)
{
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int m_SubWorldID = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;
	for(int i = 0; i < FKAUTO_MAX_POS; i++)
	{
		if(fk_autopos[i].fk_mapID == m_SubWorldID && fk_autopos[i].fk_npcType == npcType)
		{
			return i;
		}
	}
	return -1;
}

void KPlayerAuto::PauseFkAuto(BOOL bPause)
{ 
	mfk_bPause = bPause;
}

void KPlayerAuto::Active()
{
	if(!this->mfk_bActive)
		return;
	if(this->mfk_bPause)
		return;

	if(!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode) //®ang ë trong thµnh
	{
		FkDoAutoMap();

		DoRestoreHP();	//phôc håi sl

		DoRestoreMP(); //phôc håi nl

		myCountDownTimerAutoMap();
	}
	else //ë ngoµi thµnh
	{
		if(!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsAlive()) // nÕu player bÞ chÕt
		{
			if (!(g_SubWorldSet.GetGameTime() % 90))
			{
				FkAutoMapSet_StepOne();
				CoreDataChanged(GDCNI_FK_AUTO_SELECTUI, 0, 1); //tù ®éng bÊm vÒ thµnh d­ìng søc
				return; //kh«ng thùc hiÖn c¸c lÖnh d­íi n÷a
			}
		} 

		if (!(g_SubWorldSet.GetGameTime() % 90))
		{
			DoAutoX2();

			DoAutoX2Skill();
		} 
		
		if (!(g_SubWorldSet.GetGameTime() % 6))
		{
			FKAutoFilterEquip(); //läc ®å
			FKAutoFilterBlackItem(); //läc black item
		}

		if (!(g_SubWorldSet.GetGameTime() % 2))
		{
			SortEquipment();
		}

		CheckState();

		CheckAttackType();

		DoShortKeyShortCut();

		DoRestoreHP();	//phôc håi sl

		DoRestoreMP(); //phôc håi nl

		Func_TwCheck(); //thæ ®Þa phï

		DoUseItemHMPFCell();//c¾n m¸u khi ®Çy r­¬ng

		DoRestoreToxic();//sö dông gi¶i ®éc

		if (!(g_SubWorldSet.GetGameTime() % 30))
		{
			DoOpenHMPBag();
		}

		DoSortEquipment(); //kÝch ho¹t s¾p xÕp hµnh trang

		myCountDownTimerRecover();
		
		if(mfk_btState == STATE_PICKUP_OBJ)
		{
			DoActackObject();
		} 
		else if(mfk_btState == STATE_WAITING_JOB) // cã else ­u tiªn nhÆt ®å
		{
			ReturnMap();
		}
		else if(mfk_btState == STATE_ATTACK_NPC)
		{
			DoActackNpc();
			if(!(g_SubWorldSet.GetGameTime() % 18))
				AutoCheckNpcLag();
			if(!(g_SubWorldSet.GetGameTime() % 180))
				FkAutoDamnMonter();//qu©y qu¸i sau 10 gi©y
		}
		//---Tæ ®éi
		if (!(g_SubWorldSet.GetGameTime() % 2))
		{
			DoAutoParty();

			DoAutoKickTeamOut();
		}
		//
		FkAutoIdleTalk();
		//
	}
}

void KPlayerAuto::FkAutoDamnMonter()
{
	if(!m_bDamnMonterCheckBox)//cã sö dông qu©y qu¸i
		return;
	//
	int nTeamCount2 = 8;//--danh s¸ch npc qu¸i xung quanh
	KUiPlayerItem*	m_pPlayersList2 = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nTeamCount2);
	int nRetAround = NpcSet.GetAroundNpcMonster(m_pPlayersList2, nTeamCount2, m_nFightRange);
	if(nRetAround >= 3)//nÕu xuÊt hiÖn ®¸m qu¸i 3 con trë lªn b¾t ®Çu qu©y
	{
		for(int i = 0; i < 3; i++)
		{
			int m_fkIndexFocus = m_pPlayersList2[i].nIndex;//NpcSet.SearchID(m_pPlayersList2[i].uId);
			if(m_fkIndexFocus)
			{
				m_nArrayFocus[i] = m_fkIndexFocus;
			}
		}
	}
	if(m_pPlayersList2)
	{
		memset(m_pPlayersList2, 0, sizeof(m_pPlayersList2) * nTeamCount2);
		free(m_pPlayersList2);
		m_pPlayersList2 = NULL;
	}
}

char g_Array_IdleTalk[10][Def_MAXLEN_STRING_CHAT] = 
{
	"Thµ mÊt thµnh T­¬ng D­¬ng kh«ng ®Ó em lªn d­êng víi th»ng kh¸c",
	"Huynh ®Ö lµ khi thÊy nhau ®au, khæ th× gióp ®ì lÉn nhau",
	"Lóc ho¹n n¹n th× cã tao s¸t c¸nh, khi gôc ng· cã tao bªn c¹nh",
	"N­íc qu¸ trong th× kh«ng cã c¸, ng­êi qu¸ tèt th× mÐo ai ch¬i",
	"Khi tiÖc tïng cïng nh¶y ®ã chØ lµ b¹n, tiÖc tµn ngåi bµn míi lµ anh em",
	"Lßng chã khã nhai, lßng ng­êi khã ®o¸n",
	"Chóng mµy buån nh­ng ch¾c g× ®· khæ, cßn tao c­êi nh­ng lÖ ®æ trong tim",
	"Giang hå hiÓm ¸c anh kh«ng sî, chØ sî ®­êng vÒ thiÕu bãng em",
	"Cuéc sèng giang hå nu«i ta lín, lßng ng­êi phô b¹c d¹y ta kh«n",
	"Khi nghÌo th× ch¼ng ai nh×n, ®Õn khi tróng sè th× ngh×n anh em",
};

void KPlayerAuto::FkAutoIdleTalk()
{
	if(!m_bTalkAnnyCheckBox)
		return;

	if (!(g_SubWorldSet.GetGameTime() % 600))
	{
		BYTE fkIndex_Ran = g_Random(10);
		char fk_cIdleTalk[Def_MAXLEN_STRING_CHAT];
		strcpy(fk_cIdleTalk, g_Array_IdleTalk[fkIndex_Ran]);
		//
		CoreDataChanged(GDCNI_FK_AUTO_TALK, (unsigned int)&fk_cIdleTalk, 0);
	}
}

void KPlayerAuto::DoShortKeyShortCut()
{
	if(m_ShortcutKeySelect1 && m_ShortcutKeySelect2 //cã thiÕt lËp 2 shortkey ®Çy ®ñ th«ng sè
		&& m_nShortcutEditBox1 && m_nShortcutEditBox2
		&& m_ShortcutOptSelect1 && m_ShortcutOptSelect2)
	{

		if(m_ShortcutOptSelect1 == 1 && m_ShortcutOptSelect2 == 1) // xuÊt chiªu
		{
			if (!myLockActionTabRecover(TIME_USE_SHORTCUT1))
			{
				mySetValueCountDown(TIME_USE_SHORTCUT1, (int)((m_nShortcutEditBox1 * 18) / 1000));
				CoreDataChanged(GDCNI_AUTO_SET_HOTKEY_DR, m_ShortcutKeySelect1 -1, 0);
			}
			if (!myLockActionTabRecover(TIME_USE_SHORTCUT2))
			{
				mySetValueCountDown(TIME_USE_SHORTCUT2, (int)((m_nShortcutEditBox2 * 18) / 1000));
				CoreDataChanged(GDCNI_AUTO_SET_HOTKEY_DR, m_ShortcutKeySelect2 -1, 0);
			}
		}
		else if(m_ShortcutOptSelect1 == 2 && m_ShortcutOptSelect2 == 2) //0 kh«ng thiÕt lËp, 1 xuÊt chiªu, 2 thiÕt lËp thêi gian
		{
			SYSTEMTIME systm;
			GetLocalTime(&systm);
			if(systm.wSecond % 2)
				CoreDataChanged(GDCNI_AUTO_SET_HOTKEY, m_ShortcutKeySelect1 -1, 0);
			else
				CoreDataChanged(GDCNI_AUTO_SET_HOTKEY, m_ShortcutKeySelect2 -1, 0);
		}
		else if(m_ShortcutOptSelect1 == 3 && m_ShortcutOptSelect2 == 3) // sl < vµ sl >
		{
			if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife <= m_nShortcutEditBox1)
				CoreDataChanged(GDCNI_AUTO_SET_HOTKEY, m_ShortcutKeySelect1 -1, 0);
			if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife > m_nShortcutEditBox2)
				CoreDataChanged(GDCNI_AUTO_SET_HOTKEY, m_ShortcutKeySelect2 -1, 0);
		}
		else if(m_ShortcutOptSelect1 == 4 && m_ShortcutOptSelect2 == 4) // nl < vµ nl >
		{
			if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana <= m_nShortcutEditBox1)
				CoreDataChanged(GDCNI_AUTO_SET_HOTKEY, m_ShortcutKeySelect1 -1, 0);
			if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana > m_nShortcutEditBox2)
				CoreDataChanged(GDCNI_AUTO_SET_HOTKEY, m_ShortcutKeySelect2 -1, 0);			
		}
		else if(m_ShortcutOptSelect1 == 5 && m_ShortcutOptSelect2 == 5) // cast bïa
		{
			if (!myLockActionTabRecover(TIME_USE_SHORTCUT1))
			{
				mySetValueCountDown(TIME_USE_SHORTCUT1, (int)((m_nShortcutEditBox1 * 18) / 1000));
				CoreDataChanged(GDCNI_AUTO_HOTKEY_CAST_B, m_ShortcutKeySelect1 -1, m_nIndexFocus);
			}
			if (!myLockActionTabRecover(TIME_USE_SHORTCUT2))
			{
				mySetValueCountDown(TIME_USE_SHORTCUT2, (int)((m_nShortcutEditBox2 * 18) / 1000));
				CoreDataChanged(GDCNI_AUTO_HOTKEY_CAST_B, m_ShortcutKeySelect2 -1, m_nIndexFocus);
			}
		}
	}
}

void KPlayerAuto::CheckAttackType()
{
	if(FkAutoMoveMapMps() == FALSE) //®ang ch¹y ra b·i kh«ng lµm bªn d­íi
	{
		m_nAttackType = ATTACK_TYPE_MAP_MPS;
		return;
	}
	//
	int m_FollowPeopleIdx = 0;
	//
	m_nAttackType = ATTACK_TYPE_FREE; //mÆc ®Þnh ®¸nh tù do
	if(m_bLoopPosCheckBox)
		m_nAttackType = ATTACK_TYPE_AROUND_POINT;
	//
	if(m_bFlCaptainCheckBox && Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure == TEAM_MEMBER)//theo sau ®éi tr­ëng
	{
		int nghkTeamCount = 8;
		KUiPlayerItem*	m_pPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nghkTeamCount);
		int nghkRet = g_Team[0].GetMemberInfo(m_pPlayersList, nghkTeamCount);
		if(nghkRet)
		{
			//DWORD mfk_nCaptain = g_Team[0].m_nCaptain;
			if(m_pPlayersList[0].nIndex)
				m_FollowPeopleIdx = m_pPlayersList[0].nIndex; //NpcSet.SearchID(mfk_nCaptain);
		}
		if(m_pPlayersList)
		{
			memset(m_pPlayersList, 0, sizeof(m_pPlayersList) * nghkTeamCount);
			free(m_pPlayersList);
			m_pPlayersList = NULL;
		}
	}
	//
	if(m_bFollowCheckBox && !m_FollowPeopleIdx && m_nFollowNameTxt[0] //theo sau PlayerName
		&& strcmp(m_nFollowNameTxt, MSG_NON_SETTINGS) != 0)
	{
		m_FollowPeopleIdx = NpcSet.SearchName(m_nFollowNameTxt);
	}
	//
	if(m_bFlAnnyPTCheckBox && !m_FollowPeopleIdx)	//theo sau bÊt kú ai
	{
		int nTeamCount2 = 8;	//--danh s¸ch ng­êi xung quanh
		KUiPlayerItem*	m_pPlayersList2 = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nTeamCount2);
		int nRetAround = NpcSet.GetAroundPlayer(m_pPlayersList2, nTeamCount2);
		if(nRetAround > 0 && m_pPlayersList2[0].nIndex)
			m_FollowPeopleIdx = m_pPlayersList2[0].nIndex;//NpcSet.SearchID(m_pPlayersList2[0].uId);
		if(m_pPlayersList2)
		{
			memset(m_pPlayersList2, 0, sizeof(m_pPlayersList2) * nTeamCount2);
			free(m_pPlayersList2);
			m_pPlayersList2 = NULL;
		}
	}
	//
	if(m_FollowPeopleIdx && Npc[m_FollowPeopleIdx].Name[0] && Npc[m_FollowPeopleIdx].m_nPKFlag != enumPKNormal)
	{
		if (!(g_SubWorldSet.GetGameTime() % 18*5))
			AutoSendMsg("<color=gold>Kh«ng thÓ theo sau nh©n vËt tr¹ng th¸i PK phi chiÕn ®Êu<color>");
	}
	else if(m_FollowPeopleIdx && Npc[m_FollowPeopleIdx].Name[0] && mfk_btState != STATE_PICKUP_OBJ) //tr¹ng th¸i kh¸c ®ang nhÆt ®å
	{
		int nX, nY;
		Npc[m_FollowPeopleIdx].GetMpsPos(&nX,&nY);
		int dZ = NpcSet.GetDistance(m_FollowPeopleIdx , Player[CLIENT_PLAYER_INDEX].m_nIndex);
		if(dZ >= m_nFollowEditBox && nX && nY) //kho¶ng c¸ch vµ c¸c th«ng tin hîp lÖ
		{
			if (!(g_SubWorldSet.GetGameTime() % 2)) //fix by phong kiÒu theo sau liªn tôc ­u tiªn tr­íc
			{
				MoveTo(nX,nY);	//theo sau
				m_nXLoopPosTxt = nX;	//cËp nhËt l¹i to¹ ®é x y
				m_nYLoopPosTxt = nY;
			}
			m_nAttackType = ATTACK_TYPE_FOLLOW_PEOPLE; //x¸c ®Þnh kiÓu ®ang theo sau
		}
	}
	if(m_bConstPosCheckBox && m_MoveMpsTrain[0][0])
		m_nAttackType = ATTACK_TYPE_COORDINATE;
}

BOOL KPlayerAuto::FkAutoMoveMapMps()
{
	if(!m_bMapRunPosCheckBox) //kh«ng sö dông ch¹y ra b·i
		return TRUE;
	//
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (Npc[m_nIndex].m_FightMode == fight_active)
	{
		int nMoveCount = FkGetMoveMpsMapCount();
		if (m_MoveMapStep >= nMoveCount)
		{
			return TRUE;
		}

		if (!m_MoveMpsMap[m_MoveMapStep][0] || (m_MoveMpsMap[m_MoveMapStep][0] != SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID))
		{
			m_MoveMapStep++;
			return FALSE;
		}
		int nMapX, nMapY, dX, dY, dZ;
		Npc[m_nIndex].GetMpsPos(&nMapX, &nMapY);
		dX = nMapX - m_MoveMpsMap[m_MoveMapStep][1];
		dY = nMapY - m_MoveMpsMap[m_MoveMapStep][2];
		dZ = (int)sqrt((float)dX*dX+(float)dY*dY);
		if (dZ>=64)
		{
			if (!(g_SubWorldSet.GetGameTime() % 18)) //fix by phong kiÒu
			{
				if(!Npc[m_nIndex].m_bRideHorse) // lªn ngùa khi ch¹y ra b·i
				{	
					SendClientCmdRide(FALSE);
				}
				MoveTo(m_MoveMpsMap[m_MoveMapStep][1], m_MoveMpsMap[m_MoveMapStep][2]);
			}
		}
		else
		{
			m_MoveMapStep++;
		}
	}
	return FALSE;
}

int KPlayerAuto::FkGetMoveMpsMapCount()
{
	int nCount = 0;
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		if (m_MoveMpsMap[i][0])
			nCount++;
	}
	return nCount;
}

void KPlayerAuto::CheckState()
{
	if(FkAutoMoveMapMps() == FALSE) //®ang ch¹y ra b·i kh«ng lµm bªn d­íi
		return;

	if (!(g_SubWorldSet.GetGameTime() % 9) && BuffSkill() == TRUE) // ®ang buff kh«ng lµm bªn d­íi
		return;

	RefreshObject();
	//RefreshNpc(); //­u tiªn nhÆt ®å ®ãng l¹i
	//if(m_bPickAllCheckBox)
	DoActackObject();
	
	if(mfk_btState)
		return;

	RefreshObject();
	RefreshNpc();
	
	if(m_nObjectIndex)
	{
		mfk_btState = STATE_PICKUP_OBJ;
		return;
	}

	if(m_nIndexFocus)
	{
		mfk_btState = STATE_ATTACK_NPC;
		return;
	}

	mfk_btState = STATE_WAITING_JOB;
}

void KPlayerAuto::ReturnMap()
{
	if(m_nAttackType == ATTACK_TYPE_AROUND_POINT) //chÕ ®é quanh ®iÓm
	{
		int x,y;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&x, &y);
		if (m_nXLoopPosTxt >= (x - 50) && m_nXLoopPosTxt <= (x + 50) 
			&& m_nYLoopPosTxt >= (y - 50) && m_nYLoopPosTxt <= (y + 50)) //Quay vÒ ®iÓm cè ®Þnh trong ph¹m vi 50
		{
			FkAutoShitDown();
		}
		else
		{
			if (!(g_SubWorldSet.GetGameTime() % 18)) //fix by phong kiÒu
				MoveTo(m_nXLoopPosTxt, m_nYLoopPosTxt);
		}
	}
	else if(m_nAttackType == ATTACK_TYPE_FREE)//chÕ ®é tù do cËp nhËt l¹i to¹ ®é hiÖn t¹i cho ch¹y tiÕp
	{
		int x,y;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&x, &y);
		m_nXLoopPosTxt = x;
		m_nYLoopPosTxt = y;
		//FkAutoShitDown();
	}
	else if(m_nAttackType == ATTACK_TYPE_COORDINATE)//®¸nh theo to¹ ®é
	{
		FkAutoMoveMps();
	}
}

void KPlayerAuto::FkAutoShitDown()
{
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
	{	
		if(!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
		{
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_sit);  //th× ngåi xuèng phôc håi l¹i mana vµ m¸u
			SendClientCmdSit(TRUE);
		}
	}
}

int KPlayerAuto::FkGetMoveMpsTrainCount()
{
	int nCount = 0;
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		if (m_MoveMpsTrain[i][0])
			nCount++;
	}
	return nCount;
}

BOOL KPlayerAuto::FkAutoMoveMps()
{
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (Npc[m_nIndex].m_FightMode == fight_active)
	{
		int nMoveCount = FkGetMoveMpsTrainCount();
		if (m_MoveStepTrain >= nMoveCount)
			m_MoveStepTrain = 0;

		if (!m_MoveMpsTrain[m_MoveStepTrain][0] || (m_MoveMpsTrain[m_MoveStepTrain][0] != SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID))
		{
			m_MoveStepTrain++;
			return FALSE;
		}
		int nMapX, nMapY, dX, dY, dZ;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nMapX, &nMapY);
		dX = nMapX - m_MoveMpsTrain[m_MoveStepTrain][1];
		dY = nMapY - m_MoveMpsTrain[m_MoveStepTrain][2];
		dZ = (int)sqrt((float)dX*dX+(float)dY*dY);
		if (dZ>=64)
		{
			if (!(g_SubWorldSet.GetGameTime() % 18)) //fix by phong kiÒu
			{
				MoveTo(m_MoveMpsTrain[m_MoveStepTrain][1], m_MoveMpsTrain[m_MoveStepTrain][2]);
				m_nXLoopPosTxt = m_MoveMpsTrain[m_MoveStepTrain][1]; //cËp nhËt l¹i to¹ ®é míi
				m_nYLoopPosTxt = m_MoveMpsTrain[m_MoveStepTrain][2];
			}
		}
		else
		{
			m_MoveStepTrain++;
		}
	}
	return FALSE;
}
void KPlayerAuto::MoveTo(int nX, int nY) 
{
    int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

    if(m_bIRMoveCheckBox && !Npc[nIndex].m_bRideHorse) 
	{
        SendClientCmdRide(FALSE);
    }

  
    if(Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames < defMAX_PLAYER_SEND_MOVE_FRAME) 
	{
        return; 
    }

    
   bool isRunning = (Player[CLIENT_PLAYER_INDEX].m_RunStatus != 0);
    Npc[nIndex].SendCommand(isRunning ? do_run : do_walk, nX, nY);
    if (isRunning) 
	{
        SendClientCmdRun(nX, nY);
    }
	else 
	{
        SendClientCmdWalk(nX, nY);
    }


    Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
}


void KPlayerAuto::DoActackNpc()
{
	int nfkDistance = NpcSet.GetDistance(m_nIndexFocus, Player[CLIENT_PLAYER_INDEX].m_nIndex);
	//
	if(!Npc[m_nIndexFocus].IsAlive())
	{
		m_nIndexFocus = 0;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
		mfk_btState = STATE_WAITING_JOB;
		return;
	}
	
	if(Npc[m_nIndexFocus].m_Kind == kind_normal && m_bFightDistanceSelect == 1 && nfkDistance <= m_nFightSelfDefValue &&
		(Npc[m_nIndexFocus].m_Type == boss_blue ||  Npc[m_nIndexFocus].m_Type == boss_gold))//lùa chän tiÕp cËn tr¸nh boss xanh vµng
	{
		AutoMakeAwayNpc();//tr¸nh xa npc
		m_nIndexFocus = 0;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
		mfk_btState = STATE_WAITING_JOB;
		return;
	}

	if(m_bFightSelfDef == TRUE && Npc[m_nIndexFocus].m_Kind == kind_player && nfkDistance <= m_nFightSelfDefValue)//chÕ ®é tù vÖ //cã sö dông ph¹m vi tù vÖ
	{
		if(m_bFightSelfDefSelect == 0)//tr¸nh xa
		{
			AutoMakeAwayNpc(); //tr¸nh xa npc
			m_nIndexFocus = 0;
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
			mfk_btState = STATE_WAITING_JOB;
			return;
		}
		else if(m_bFightSelfDefSelect == 1)//thæ ®Þa phï
		{
			DoUseTownPortal();
			mySetValueCountDown(TIME_USE_TW, 18);
			AutoSendMsg("<color=green>Thæ ®Þa phï tù vÖ<color>");
			return;
		}
		else if(m_bFightSelfDefSelect == 2)//tho¸t game
		{
			CoreDataChanged(GDCNI_S2C_EXIT_GAME, NULL, NULL);
			return;
		}
		else if(m_bFightSelfDefSelect == 3)//®¸nh tr¶
		{
			if(Npc[m_nIndexFocus].m_HideState.nTime)//fix by Phong KiÒu ®ang tµng h×nh kh«ng ®¸nh n÷a
			{
				m_nIndexFocus = 0;
				mfk_btState = STATE_WAITING_JOB;
				return;
			}
			//thùc hiÖn tiÕp lÖnh d­íi
		}
	}
	//
	int nFocusX = 0;
	int nFocusY = 0;
	int nDistance = 0;
	Npc[m_nIndexFocus].GetMpsPos(&nFocusX, &nFocusY);
	nDistance = NpcSet.GetDistance(m_nIndexFocus, Player[CLIENT_PLAYER_INDEX].m_nIndex);
	if (m_bFightDistance && nDistance > m_nFightDistance)//cã sö dông ph¹m vi tiÕp cËn
	{
		if(m_bFightSelfDef == TRUE && Npc[m_nIndexFocus].m_Kind == kind_player) //cã tù vÖ vµ môc tiªu lµ player
		{
			if (!(g_SubWorldSet.GetGameTime() % 18)) //fix by phong kiÒu
				MoveTo(nFocusX, nFocusY);
		}
		if( m_bFightCheckBox == TRUE && Npc[m_nIndexFocus].m_Kind == kind_normal) //cã tù ®¸nh vµ môc tiªu lµ npc
		{
			if(Npc[m_nIndexFocus].m_dwID) //fix lçi qu©y qu¸i kh«ng cã tªn
			{
				if (!(g_SubWorldSet.GetGameTime() % 2)) //fix by phong kiÒu
					MoveTo(nFocusX, nFocusY);
			}
			else
				ReturnMap();
		}
	}
	else
	{
		if(m_bFightSelfDef == TRUE && Npc[m_nIndexFocus].m_Kind == kind_player) //cã tù vÖ vµ môc tiªu lµ player
			FollowAtackNpc(m_nIndexFocus);
		if( m_bFightCheckBox == TRUE && Npc[m_nIndexFocus].m_Kind == kind_normal) //cã tù ®¸nh vµ môc tiªu lµ npc
		{
			if(m_nArrayFocus[m_nArrayIndex]) //ch¹y tõng npc trong m¶ng qu©y
			{
				if(!(g_SubWorldSet.GetGameTime() % 2))
				{
					m_nIndexFocus = m_nArrayFocus[m_nArrayIndex];
					m_nArrayIndex++;
				}
			}
			else //ch¹y hÕt m¶ng qu©y chuyÓn qua ®¸nh qu¸i
			{
				FollowAtackNpc(m_nIndexFocus);
				m_nArrayIndex = 0;
				memset(m_nArrayFocus, 0, sizeof(m_nArrayFocus));
			}
		}
	}
}

void KPlayerAuto::AutoMakeAwayNpc()//tr¸nh xa npc
{
	int nMX = 0;
	int nMY = 0;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nMX, &nMY);//lÊy to¹ ®é hiÖn t¹i chÝnh m×nh
	int rantx = g_Random(70);
	if(rantx >= 10)
	{
		nMX = nMX + (8*32); //ngÉu nhiªn céng thªm X
	}
	else if(rantx > 10 && rantx <= 20)
	{
		nMY = nMY + (16*32); //ngÉu nhiªn céng thªm Y
	}
	else if(rantx > 20 && rantx <= 30)
	{
		nMX = nMX + (8*32); //ngÉu nhiªn céng thªm X vµ Y
		nMY = nMY + (16*32);
	}
	else if(rantx > 30 && rantx <= 40)
	{
		nMX = nMX - (8*32); //ngÉu nhiªn trõ X
	}
	else if(rantx > 50 && rantx <= 60)
	{
		nMY = nMY - (16*32); //ngÉu nhiªn trõ Y
	}
	else
	{
		nMX = nMX - (8*32); //ngÉu nhiªn trõ X vµ Y
		nMY = nMY - (16*32);
	}
	if (!(g_SubWorldSet.GetGameTime() % 18)) //fix by phong kiÒu
		MoveTo(nMX, nMY); //di chuyÓn ®Õn chç míi tr¸nh kÎ tÊn c«ng
}

void KPlayerAuto::FollowAtackNpc(int nTargetIndex)
{
	if(m_nAttackType == ATTACK_TYPE_FOLLOW_PEOPLE)//®ang theo sau kh«ng ®¸nh
		return;
	//
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading()) 
		return;

	int nSkillID = Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
	int nRSkillID = Player[CLIENT_PLAYER_INDEX].GetRightSkill();
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if(m_bSkillRightCheckBox > 0)//®¸nh chiªu bªn ph¶i
	{
		nSkillID = Player[CLIENT_PLAYER_INDEX].GetRightSkill();
	}
	if(m_bFightSelfDef && m_bFightSelfDefSelect == 3 && Npc[nTargetIndex].m_Kind == kind_player 
		&& m_SkillSelfDefID) //cã g¸n chiªu tù vÖ ®¸nh tr¶
	{
		nSkillID = m_SkillSelfDefID;
	}
	if(Npc[nTargetIndex].m_Kind == kind_normal && Npc[nTargetIndex].m_Type == boss_blue
		&& m_SkillBossID) //cã g¸n skill ®¸nh boss
	{
		nSkillID = m_SkillBossID;
	}

	if(Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		return;
	}
	//
	KSkill * pISkill =  (KSkill *)g_SkillManager.GetSkill(Npc[nIndex].m_ActiveSkillID, 1);
	if (!pISkill) 
        return;
	//
	if (pISkill->IsAura())
		return;
	//
	int nAttackRange = pISkill->GetAttackRadius();
	// Set len xuong ngua
	if(m_bFightSelect == F_IsRide)
	{
		if(!Npc[nIndex].m_bRideHorse)
		{	
			SendClientCmdRide(FALSE);
		}
	}
	else if(m_bFightSelect == F_IsNotRide)
	{
		if(Npc[nIndex].m_bRideHorse)
		{	
			SendClientCmdRide(TRUE);
		}
	}
	if (pISkill->GetHorseLimit() && m_bFightSelect == F_Auto)//tù ®éng lªn xuèng ngùa
	{	
		switch(pISkill->GetHorseLimit())
		{
			case 1:
				{
					if(Npc[nIndex].m_bRideHorse)
					{	
						SendClientCmdRide(TRUE);
					}
				}
				break;
			case 2:
				{
					if(!Npc[nIndex].m_bRideHorse)
					{	
						SendClientCmdRide(FALSE);
					}
				}
				break;
			default:
				break;
		}
	}
	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	int nRangePlayer = NpcSet.GetDistance(nIndex, nTargetIndex);
	if(nRelation == relation_enemy && nRangePlayer > 0 && nRangePlayer <= m_nFightRange) //®ang n»m trong ph¹m vi vµ mèi quan hÖ kÎ thï
	{
		if(nAttackRange >= nRangePlayer) //npc n»m ph¹m vi cña skill th× ®¸nh
		{
			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nTargetIndex);//auto skill ë ®©y
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nTargetIndex].m_dwID);
		}
		else //chuyÓn qua tÊn c«ng npc kh¸c
		{
			m_nIndexFocus = 0;
			mfk_btState = STATE_WAITING_JOB;
		}
	}
	else //chuyÓn qua tÊn c«ng npc kh¸c
	{
		m_nIndexFocus = 0;
		mfk_btState = STATE_WAITING_JOB;
	}
}

void KPlayerAuto::RefreshNpc()
{	
	if(!m_bFightCheckBox && !m_bFightSelfDef)
		return;
	//
	
	if(m_bFMORCheckBox)//®¸nh qu¸i trªn ®­êng ®i cËp nhËt liªn tôc to¹ ®é x y
	{
		int nX, nY;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX,&nY);
		m_nXLoopPosTxt = nX;
		m_nYLoopPosTxt = nY;
	}
	//
	if(m_nIndexFocus == 0)
	{
		m_nIndexFocus = NpcSet.AutoGetNpcNear(m_nXLoopPosTxt, m_nYLoopPosTxt, relation_enemy, 
			m_nFightRange, m_nArrayLagNpc, defARRAY_LAG, m_bFightCheckBox, m_bFightSelfDef);//add by Fong KiÒu
		m_nTimeRunNpc	= 0;
		m_nLifeNpc		= Npc[m_nIndexFocus].m_CurrentLife;
	}
	else
	{
		if(!Npc[m_nIndexFocus].IsAlive())
		{
			mfk_btState		= STATE_WAITING_JOB;
			m_nTimeRunLag	= 0;
			m_nIndexFocus	= 0;
			return;
		}
	}

	if(m_nIndexFocus == 0)
	{
		ClearNpcArrLag();
		mfk_btState		= STATE_WAITING_JOB;
		m_nTimeRunNpc	= 0;
	}
}

void KPlayerAuto::AutoCheckNpcLag()
{
	m_nTimeRunNpc++;
	if(m_nLifeNpc != Npc[m_nIndexFocus].m_CurrentLife)
	{
		m_nTimeRunNpc = 0;
		m_nLifeNpc = Npc[m_nIndexFocus].m_CurrentLife;
	}

	if(m_nTimeRunNpc >= 6)
	{
		int fk_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		//char szTemp[128];
		//sprintf(szTemp, "<color=yellow> %s <color> bÞ lag", Npc[m_nIndexFocus].Name);
		//AutoSendMsg(szTemp);
		AddNpc2ArrLag(m_nIndexFocus);
		m_nIndexFocus = 0;
		Npc[fk_nIndex].m_nPeopleIdx = 0;
		mfk_btState = STATE_WAITING_JOB;
	}
}

void KPlayerAuto::ClearNpcArrLag()
{	
	int i = 0;
	for(i = 0; i < defARRAY_LAG; i++)
	{
		m_nArrayLagNpc[i] = 0;
	}
}

void KPlayerAuto::AddNpc2ArrLag(int nIdx)
{
	bool bFindFree = false;
	int i = 0;
	for (i = 0; i < defARRAY_LAG; i++)
	{
		if (m_nArrayLagNpc[i] == 0)
		{
			bFindFree = true;
			break;
		}
	}
	if (bFindFree == true)
	{
		m_nArrayLagNpc[i] = nIdx;
	}
	else
	{
		ClearNpcArrLag();
		m_nArrayLagNpc[0] = nIdx;
	}
}

void	KPlayerAuto::RefreshObject()
{
	if(!m_bAutoPickCheckBox)
		return;
	if (m_nObjectIndex == 0)
	{ 
		if(m_bPickAllCheckBox)//kh«ng bá sãt
		{
			m_bPickSpeCheckBox = TRUE;
			m_nPickSelTxtSelect = 0; //nhÆt tÊt c¶ c¸c lo¹i ®å
		}
		//
		int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		m_nObjectIndex = ObjSet.AutoGetObjNear(m_nXLoopPosTxt, m_nYLoopPosTxt, m_nAutoPickEditBox, m_nPickSelTxtSelect, m_bPickSpeCheckBox);
		if (m_bNPickBackLCheckBox && IsBlackObjectName(m_nObjectIndex)) //kh«ng nhÆt vËt phÈm ®en
		{
			Object[m_nObjectIndex].m_bAutoLag = TRUE;
			m_nObjectIndex = 0;
			mfk_btState = STATE_WAITING_JOB;
			return;
		}
		//
		if(Npc[m_nIndex].m_FightMode) //tr¹ng th¸i chiÕn ®Êu míi kiÓm tra
		{
			if(m_bPickAllCheckBox == TRUE) // kh«ng bá sãt //nhÆt hÕt vËt phÈm cña ng­êi kh¸c
			{
				
			}
			else
			{			
				if (Object[m_nObjectIndex].m_dwNpcId2 > 0 && 
					Object[m_nObjectIndex].m_dwNpcId2 != Npc[m_nIndex].m_dwID) //vËt phÈm cña ng­êi kh¸c nÐm ra kh«ng nhÆt vµo
				{
					Object[m_nObjectIndex].m_bAutoLag = TRUE;
					m_nObjectIndex = 0;
					mfk_btState = STATE_WAITING_JOB;
					return;
				}
			}
			//
			if (Object[m_nObjectIndex].m_dwNpcId2 > 0 && 
				Object[m_nObjectIndex].m_dwNpcId2 == Npc[m_nIndex].m_dwID) //vËt phÈm cña m×nh nÐm ra kh«ng nhÆt vµo
			{
				Object[m_nObjectIndex].m_bAutoLag = TRUE;
				m_nObjectIndex = 0;
				mfk_btState = STATE_WAITING_JOB;
				return;
			}
		}
		//
		m_nHitObject		= 0;
		m_nTimeRunLag		= 0;
		Object[m_nObjectIndex].m_bAutoCheck = TRUE;
		Object[m_nObjectIndex].m_bAutoLag = FALSE;
	}
	else
	{
		if (Object[m_nObjectIndex].m_bAutoCheck == FALSE)
		{
			mfk_btState			= STATE_WAITING_JOB;
			m_nTimeRunLag		= 0;
			m_nObjectIndex		= 0;
			return;
		}
	}
	if (m_nObjectIndex == 0)
	{
		mfk_btState		= STATE_WAITING_JOB;
		m_nHitObject	= 0;
		m_nTimeRunLag	= 0;
	}
}

void KPlayerAuto::DoActackObject()
{
	if (m_nObjectIndex == 0)
		return;
	//--¦u tiªn nhÆt ®å
	m_nIndexFocus = 0;
	mfk_btState = STATE_PICKUP_OBJ;
	//
	int nX1, nY1, nX2, nY2;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX1, &nY1);
	Object[m_nObjectIndex].GetMpsPos(&nX2, &nY2);
	if ((nX1 - nX2) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2) < PLAYER_PICKUP_CLIENT_DISTANCE * PLAYER_PICKUP_CLIENT_DISTANCE)
	{
		Player[CLIENT_PLAYER_INDEX].CheckObject(m_nObjectIndex);
		m_nHitObject++;
		AutoCheckObjectLag();
	}
	else
	{
		if (!(g_SubWorldSet.GetGameTime() % 2)) //fix by phong kiÒu ­u tiªn nhÆt ®å
		{
			MoveTo(nX2, nY2);
			m_nTimeRunLag++;
			AutoCheckObjectLag();
		}
	}
}

void KPlayerAuto::AutoCheckObjectLag()
{
	if (m_nHitObject == MAX_HIT_OBJECT || m_nTimeRunLag > MAX_TIME_RUN_TO_NPC)
	{
		Object[m_nObjectIndex].m_bAutoLag = TRUE;
		m_nObjectIndex = 0;
		mfk_btState = STATE_WAITING_JOB;
		//char szTemp[128];
		//sprintf(szTemp, "<color=green> %s <color> bÞ lag", Object[m_nObjectIndex].m_szName);
		//AutoSendMsg(szTemp);
	}
}

extern IClientCallback* l_pDataChangedNotifyFunc;

void KPlayerAuto::AutoSendMsg(char* pszMsg)
{
	try
	{
		l_pDataChangedNotifyFunc->ChannelMessageArrival(0, "fkauto", pszMsg, strlen(pszMsg), TRUE);
	} 
	catch(...){}
}

void KPlayerAuto::DoRestoreToxic()
{
	if (!m_bUseAntiToxicCheckBox)
		return;

	if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PoisonState.nTime)
	{
		if (myLockActionTabRecover(TIME_USE_TOXIC))
			return;
		
		if (DoUseToxic() == TRUE)
		{
			mySetValueCountDown(TIME_USE_TOXIC, (int)((m_nReHPEditBox3 * 18) / 1000)); //sö dông theo thêi gian b¬m m¸u
		}
	}
}

BOOL KPlayerAuto::DoUseToxic()
{
	BOOL bEc = FALSE;
	
	if (!Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_medicine, 4, 0))
	{
		if (!Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_medicine, 4, 0))
		{
			if (!Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_medicine, 4, 0))
			{
				bEc = FALSE;
			}
			else
			{
				bEc = TRUE;
			}
		}
		else
		{
			bEc = TRUE;
		}
	}
	else
	{
		bEc = TRUE;
	}
	return bEc;
}

void KPlayerAuto::DoUseItemHMPFCell()//c¾n m¸u khi ®Çy r­¬ng
{
	if (!m_bUseHPCheckBox)
		return;

	int fkW = 1; int fkH = 1;												//1x1
	if(m_nTownFCellTxtSelect == 1){ fkW = 1; fkH = 2; } //1x2
	if(m_nTownFCellTxtSelect == 2){ fkW = 1; fkH = 3; } //1x3
	if(m_nTownFCellTxtSelect == 3){ fkW = 1; fkH = 4; }	//1x4
	if(m_nTownFCellTxtSelect == 4){ fkW = 2; fkH = 1; }	//2x1
	if(m_nTownFCellTxtSelect == 5){ fkW = 2; fkH = 2; }	//2x2
	if(m_nTownFCellTxtSelect == 6){ fkW = 2; fkH = 3; }	//2x3
	if(m_nTownFCellTxtSelect == 7){ fkW = 2; fkH = 4; }	//2x4
	int nCount = Player[CLIENT_PLAYER_INDEX].m_ItemList.CalcFreeItemCellCount(fkW, fkH, room_equipment);
	if(nCount <= 0) //hÕt kho¶ng trèng
	{
		DoUseHeath();//c¾n m¸u
		DoUseMana();
	}
}

void KPlayerAuto::DoOpenHMPBag()
{
	if(!m_bOpenBagHPCheckBox)
		return;

	if(Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(0, item_medicine, 2) <= m_nOpenBagHPEditBox)//2 vua mau va mana
	{
		if(myLockActionTabRecover(TIME_USE_BAG_HP))
			return;

		if(Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_magicscript, 1, 4813) == TRUE)
		{
			mySetValueCountDown(TIME_USE_BAG_HP, (int)((m_nReHPEditBox3 * 18) / 1000));
		}
	}
}

void KPlayerAuto::DoRestoreHP()
{
	if (!m_bReHPCheckBox)
		return;

	// Check Recover Level One
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife < m_nReHPEditBox1)
	{	
		if (myLockActionTabRecover(TIME_USE_HP1))
			return;
		
		if (DoUseHeath() == TRUE)
		{
			mySetValueCountDown(TIME_USE_HP1, (int)((m_nReHPEditBox3 * 18) / 1000));
		}
	}
	
	// Check Recover Level Two
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife < m_nReHPEditBox2)
	{

		if(myLockActionTabRecover(TIME_USE_HP2))
			return;

		if(DoUseHeath() == TRUE)
		{
			mySetValueCountDown(TIME_USE_HP2, (int)((m_nReHPEditBox3 * 18) / 1000));
		}
	}
}

BOOL KPlayerAuto::DoUseHeath()
{
	BOOL bEc = FALSE;
	
	if (!Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_medicine, 0, 0))
	{
		if (!Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_medicine, 2, 0))
		{
			if (!Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_medicine, 8, 0))
			{
				bEc = FALSE;
			}
			else
			{
				bEc = TRUE;
			}
		}
		else
		{
			bEc = TRUE;
		}
	}
	else
	{
		bEc = TRUE;
	}
	return bEc;
}

void KPlayerAuto::DoRestoreMP()
{
	if (!m_bReMPCheckBox)
		return;

	// Check Recover Level One
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana < m_nReMPEditBox1)
	{
		if (myLockActionTabRecover(TIME_USE_MP1))
			return;
		if (DoUseMana() == TRUE)
		{
			mySetValueCountDown(TIME_USE_MP1, (int)((m_nReMPEditBox3 * 18) / 1000));
		}
	}
	
	// Check Recover Level Two
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana < m_nReMPEditBox2)
	{
		if (myLockActionTabRecover(TIME_USE_MP2))
			return;
		if (DoUseMana() == TRUE)
		{
			mySetValueCountDown(TIME_USE_MP2, (int)((m_nReMPEditBox3 * 18) / 1000));
		}
	}
}

BOOL KPlayerAuto::DoUseMana()
{
	BOOL bEc = FALSE;
	
	if (Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_medicine, 1, 0) == FALSE)
	{
		if (Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_medicine, 2, 0) == FALSE)
		{
			if (!Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_medicine, 8, 0))
			{
				bEc = FALSE;
			}
			else
			{
				bEc = TRUE;
			}
		}
		else
		{
			bEc = TRUE;
		}
	}
	else
	{
		bEc = TRUE;
	}
	return bEc;
}

BOOL KPlayerAuto::myLockActionTabRecover(AUTO_COUNT_DOWN pCase)
{
	BOOL bEC = TRUE;
	int	nTime = 0;
	switch(pCase)
	{
		case TIME_USE_HP1:
			nTime = m_nCoolDownHP1;
			break;
		case TIME_USE_HP2:
			nTime = m_nCoolDownHP2;
			break;
		case TIME_USE_MP1:
			nTime = m_nCoolDownMP1;
			break;
		case TIME_USE_MP2:
			nTime = m_nCoolDownMP2;
			break;
		case TIME_USE_TW:
			nTime = m_nCoolDownTW;
			break;
		case TIME_USE_TOXIC:
			nTime = m_nCoolDownTX;
			break;
		case TIME_USE_BAG_HP:
			nTime = m_nCoolDownBagHP;
			break;
		case TIME_USE_SHORTCUT1:
			nTime = m_nCoolDownSC1;
			break;
		case TIME_USE_SHORTCUT2:
			nTime = m_nCoolDownSC2;
			break;
		case TIME_SORT_ROOM_E:
			nTime = m_nCoolDownSortRE;
			break;
		case TIME_AUTO_MAP:
			nTime = m_nCoolDownAutoMap;
		default:
			break;
	}
	
	if (nTime <= 0)
		bEC = FALSE;

	return bEC;
}

void KPlayerAuto::mySetValueCountDown(AUTO_COUNT_DOWN pCase, int pValue)
{
	switch(pCase)
	{
		case TIME_USE_HP1:
			m_nCoolDownHP1 = pValue;
			break;
		case TIME_USE_HP2:
			m_nCoolDownHP2 = pValue;
			break;
		case TIME_USE_MP1:
			m_nCoolDownMP1 = pValue;
			break;
		case TIME_USE_MP2:
			m_nCoolDownMP2 = pValue;
			break;
		case TIME_USE_TW:
			m_nCoolDownTW = pValue;
			break;
		case TIME_USE_TOXIC:
			m_nCoolDownTX = pValue;
			break;
		case TIME_USE_BAG_HP:
			m_nCoolDownBagHP = pValue;
			break;
		case TIME_USE_SHORTCUT1:
			m_nCoolDownSC1 = pValue;
			break;
		case TIME_USE_SHORTCUT2:
			m_nCoolDownSC2 = pValue;
			break;
		case TIME_SORT_ROOM_E:
			m_nCoolDownSortRE = pValue;
			break;
		case TIME_AUTO_MAP:
			m_nCoolDownAutoMap = pValue;
		default:
			break;
	}
}

void KPlayerAuto::myCountDownTimerRecover()
{
	if (m_nCoolDownHP1 > 0) m_nCoolDownHP1 --;

	if (m_nCoolDownHP2 > 0) m_nCoolDownHP2 --;

	if (m_nCoolDownMP1 > 0) m_nCoolDownMP1 --;

	if (m_nCoolDownMP2 > 0) m_nCoolDownMP2 --;

	if(m_nCoolDownTW > 0) m_nCoolDownTW--;
	
	if(m_nCoolDownTX > 0) m_nCoolDownTX --;

	if(m_nCoolDownBagHP > 0) m_nCoolDownBagHP --;

	if(m_nCoolDownSC1 > 0) m_nCoolDownSC1 --;

	if(m_nCoolDownSC2 > 0) m_nCoolDownSC2 --;

	if(m_nCoolDownSortRE > 0) m_nCoolDownSortRE --;

}

void KPlayerAuto::myCountDownTimerAutoMap()
{
	if(m_nCoolDownAutoMap > 0) m_nCoolDownAutoMap --;
}

void	KPlayerAuto::Func_TwCheck()
{
	if (myLockActionTabRecover(TIME_USE_TW))
		return;

	if(m_bTownHPVCheckBox)
	{
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife < m_nTownHPVEditBox)
		{
			DoUseTownPortal();
			mySetValueCountDown(TIME_USE_TW, 18);
			AutoSendMsg("<color=green>Thæ ®Þa phï sinh lùc qu¸ thÊp.<color>");
			return;
		}
	}
	if(m_bTownMPVCheckBox)
	{
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana <= m_nTownMPVEditBox)
		{
			DoUseTownPortal();
			mySetValueCountDown(TIME_USE_TW, 18);
			AutoSendMsg("<color=green>Thæ ®Þa phï néi lùc lùc qu¸ thÊp.<color>");
			return;
		}
	}
	if(m_bTownHPNCheckBox)
	{
		BOOL bEc = FALSE;
		if(!Player[CLIENT_PLAYER_INDEX].AutoCheckItem(item_medicine, 0, 0))
		{
			if(!Player[CLIENT_PLAYER_INDEX].AutoCheckItem(item_medicine, 2, 0))
			{
				bEc = TRUE;
			}
		}
		if (bEc)
		{
			DoUseTownPortal();
			mySetValueCountDown(TIME_USE_TW, 18);
			AutoSendMsg("<color=green>Thæ ®Þa phï hÕt b×nh sinh lùc.<color>");
			return;
		}
	}
	if(m_bTownMPNCheckBox)
	{
		BOOL bEc = FALSE;

		if(!Player[CLIENT_PLAYER_INDEX].AutoCheckItem(item_medicine, 1, 0))
		{
			if(!Player[CLIENT_PLAYER_INDEX].AutoCheckItem(item_medicine, 2, 0))
			{
				bEc = TRUE;
			}
		}
		if(bEc)
		{
			DoUseTownPortal();
			mySetValueCountDown(TIME_USE_TW, 18);
			AutoSendMsg("<color=green>Thæ ®Þa phï hÕt b×nh néi lùc.<color>");
			return;
		}	
	}
	if(m_bTownFCellCheckBox == TRUE)//kiÓm tra ®Çy r­¬ng
	{
		int fkW = 1; int fkH = 1;												//1x1
		if(m_nTownFCellTxtSelect == 1){ fkW = 1; fkH = 2; } //1x2
		if(m_nTownFCellTxtSelect == 2){ fkW = 1; fkH = 3; } //1x3
		if(m_nTownFCellTxtSelect == 3){ fkW = 1; fkH = 4; }	//1x4
		if(m_nTownFCellTxtSelect == 4){ fkW = 2; fkH = 1; }	//2x1
		if(m_nTownFCellTxtSelect == 5){ fkW = 2; fkH = 2; }	//2x2
		if(m_nTownFCellTxtSelect == 6){ fkW = 2; fkH = 3; }	//2x3
		if(m_nTownFCellTxtSelect == 7){ fkW = 2; fkH = 4; }	//2x4
		int nCount = Player[CLIENT_PLAYER_INDEX].m_ItemList.CalcFreeItemCellCount(fkW, fkH, room_equipment);
		if(nCount <= 0)
		{
			DoUseTownPortal();
			mySetValueCountDown(TIME_USE_TW, 18);
			AutoSendMsg("<color=blue>Thæ ®Þa phï hµnh trang hÕt kho¶ng trèng.<color>");
			return;
		}
	}
	if(m_bTowMoneyCheckBox == TRUE && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() > this->m_nTownMoneyEditBox)
	{
		DoUseTownPortal();
		mySetValueCountDown(TIME_USE_TW, 18);
		AutoSendMsg("<color=green>Thæ ®Þa phï tiÒn hµnh trang qu¸ nhiÒu.<color>");
		return;
	}

	if(m_bTownAbradeCheckBox == TRUE && m_nTownAbradeEditBox)
	{
		int i = 0, nItemIdx = 0;
		for(i = itempart_head; i < itempart_amulet; i++)
		{	
			nItemIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);
			if(nItemIdx > 0 && Item[nItemIdx].GetDurability() > 0 && Item[nItemIdx].GetDurability() <= m_nTownAbradeEditBox)
			{	
				DoUseTownPortal();
				mySetValueCountDown(TIME_USE_TW, 18);
				AutoSendMsg("<color=green>Thæ ®Þa phï ®é bÒn qu¸ thÊp.<color>");
				return;
			}
		}
	}
}

void KPlayerAuto::DoUseTownPortal()
{
	bool checkTDP = true;
	if(!Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_townportal, 0, 0))
	{
		if(!Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_magicscript, 1, 1083))
		{
			if(!Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_magicscript, 1, 1084))
			{
				AutoSendMsg("<color=yellow>Kh«ng cã thæ ®Þa phï trong hµnh trang.<color>");
				checkTDP = false;
			}
		}
	}
	//
	if(checkTDP && this->m_RingTDPCheckBox)//rung chu«ng khi thæ ®Þa phï
	{
		char * SoundFileName = SOUND_RING_TDP;
		CoreDataChanged(GDCNI_PLAY_SOUND, (unsigned int)(SoundFileName), 0);
	}
	//
	if(checkTDP)
	{
		FkAutoMapSet_StepOne();
	}
}

BOOL KPlayerAuto::BuffSkill()
{
	BOOL _break = FALSE;
	//
	for (int i = 0; i < MAX_ARRAY_STATESKILL; i ++)
	{
		if (m_ArrayStateSkill[i] /*&& m_bBuffSkill[i]*/)//buff phô trî 1 2 3
		{
			ISkill * pSkill = g_SkillManager.GetSkill(m_ArrayStateSkill[i], 1);

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.CanCast(m_ArrayStateSkill[i], 
				SubWorld[0].m_dwCurrentTime) && 
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this), TRUE) && 
				!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].FindStateSkill(m_ArrayStateSkill[i]))
			{
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_skill, m_ArrayStateSkill[i], -1, Player[CLIENT_PLAYER_INDEX].m_nIndex);
				SendClientCmdSkill(m_ArrayStateSkill[i], -1, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID);
				return TRUE;
			}
		}
	}

	if (m_bBuffCheckBox)//nga my auto buff m¸u
	{
		int nSkillLifeReplenishID = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSkillLifeReplenish();
		ISkill * pSkill = g_SkillManager.GetSkill(nSkillLifeReplenishID, 1);
		if (nSkillLifeReplenishID && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this), TRUE))
		{
			int pLifePlayer = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife * MAX_PERCENT / Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax;
			if (pLifePlayer < m_nBuffEditBox)
			{
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_skill, nSkillLifeReplenishID, -1, m_nPlayerIndex);
				SendClientCmdSkill(nSkillLifeReplenishID, -1, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID);
				_break = TRUE;
			}
			if(m_bBuffTeamCheckBox == TRUE)//nga my buff mau cho to doi
			{
				int nghkTeamCount = 8;
				KUiPlayerItem*	m_pPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nghkTeamCount);
				int nghkRet = g_Team[0].GetMemberInfo(m_pPlayersList, nghkTeamCount);
				for(int m = 0; m < nghkRet; m++)//add by phong kiÒu nga my buff m¸u cho tæ ®éi
				{
					if(m_pPlayersList[m].nPercenLife < m_nBuffEditBox)
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_skill, nSkillLifeReplenishID, -1, m_pPlayersList[m].nIndex);
						SendClientCmdSkill(nSkillLifeReplenishID, -1, m_pPlayersList[m].uId);				
					}
				}
				if(m_pPlayersList)
				{
					memset(m_pPlayersList, 0, sizeof(m_pPlayersList) * nghkTeamCount);
					free(m_pPlayersList);
					m_pPlayersList = NULL;
				}
			}
		}
	}
	return _break;
}

void KPlayerAuto::DoAutoX2()
{
	if(!m_bUseExpCheckBox)
		return;
	if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpEnhance > 0) 
		return;
	if (Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_magicscript, 1, 71))
	{
		AutoSendMsg("Sö dông thµnh c«ng <color=yellow>Tiªn th¶o lé <color>");
	}
}

void KPlayerAuto::DoAutoX2Skill()
{
	if(!m_bUseSkillCheckBox)
		return;
	if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpEnhance > 0) 
		return;
	if (Player[CLIENT_PLAYER_INDEX].AutoUseItem(item_magicscript, 1, 1182))
	{
		AutoSendMsg("Sö dông thµnh c«ng <color=yellow>Tiªn th¶o lé ®Æc biÖt<color>");
	}
}

void KPlayerAuto::FKAutoFilterBlackItem()
{
	if (!m_bNPickBackLCheckBox)
		return;

	int nHand = Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
	if (nHand) //nÐm ra ngoµi
	{
		if (IsBlackItemName(nHand) == TRUE)
		{
			PLAYER_THROW_AWAY_ITEM_COMMAND	sThrow;
			sThrow.ProtocolType = c2s_playerthrowawayitem;
			if (g_pClient)
				g_pClient->SendPackToServer(&sThrow, sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND));
		}
	}
	else //läc trong hµnh trong pos_equiproom
	{
		ItemPos	P;
		PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
		if (pItem)
		{
			if (pItem->nPlace == pos_equiproom && IsBlackItemName(pItem->nIdx) == TRUE)
			{
				P.nPlace = pItem->nPlace;
				P.nX = pItem->nX;
				P.nY = pItem->nY;
				Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
				return;
			}
		}
		int n = 0;
		while(pItem) 
		{ 
			pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
			n++;
			if (pItem)
			{
				if (pItem->nPlace == pos_equiproom && IsBlackItemName(pItem->nIdx) == TRUE)
				{
					P.nPlace = pItem->nPlace;
					P.nX = pItem->nX;
					P.nY = pItem->nY;
					Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
					return;
				}
			}
		}
	}
}

void KPlayerAuto::FKAutoFilterEquip()
{
	if (!m_bFillterItemCheckBox)
		return;

	int nHand = Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
	if (nHand) //nÐm ra ngoµi
	{
		if (Item[nHand].GetGenre() == item_equip && Item[nHand].GetDetailType() < equip_horse) 
		{
			if(Item[nHand].GetDurability() == 0)
				return;
			if(m_bRiAmPeCheckBox && IsRAPEquip(Item[nHand].GetDetailType()) 
				&& Item[nHand].GetLevel() >= m_nRiEditBox && Item[nHand].GetLevel() <= m_nAmPeEditBox)
				return;
			if(IsEquipSatisfyCondition(nHand))
				return;
			if (m_bPriceCellCheckBox && Item[nHand].GetSalePrice() >= m_nPriceCellEditBox) //gi÷ l¹i vËt phÈm cã gi¸ 
				return;
			if (m_bLevelCheckBox && Item[nHand].GetLevel() >= m_nLevelEditBox) //gi÷ l¹i vËt phÈm cã cÊp 
				return;
			if (m_bKeepPurpleCheckBox && Item[nHand].GetKind() == purple_item && IsRowMagicItem(nHand, m_nKeepPurpleEditBox)) //gi÷ l¹i ®å tÝm n dßng
				return;

			PLAYER_THROW_AWAY_ITEM_COMMAND	sThrow;
			sThrow.ProtocolType = c2s_playerthrowawayitem;
			if (g_pClient)
				g_pClient->SendPackToServer(&sThrow, sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND));
		}
	}
	else //läc trong hµnh trong pos_equiproom
	{
		ItemPos	P;
		PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
		if (pItem)
		{
			if (pItem->nPlace == pos_equiproom && Item[pItem->nIdx].GetGenre() == item_equip && Item[pItem->nIdx].GetDetailType() < equip_horse)
			{
				BOOL bFilter = TRUE;
				if (Item[pItem->nIdx].GetKind() == gold_item)
					bFilter = FALSE;
				else if (Item[pItem->nIdx].GetPlayerItemLock() > 0)
					bFilter = FALSE;
				else if (Item[pItem->nIdx].GetPlayerItemLock()  == -2)
					bFilter = FALSE;
				else if (Item[pItem->nIdx].GetDurability() == 0)	//#trang bi hong khong vut ra
					bFilter = FALSE;
				else if (m_bRiAmPeCheckBox && IsRAPEquip(Item[pItem->nIdx].GetDetailType()) //gi÷ l¹i trang søc cã level tõ a ®Õn b
						&& Item[pItem->nIdx].GetLevel() >= m_nRiEditBox && Item[pItem->nIdx].GetLevel() <= m_nAmPeEditBox)
					bFilter = FALSE;
				else if (IsEquipSatisfyCondition(pItem->nIdx))
					bFilter = FALSE;
				else if (m_bPriceCellCheckBox && Item[pItem->nIdx].GetSalePrice() >= m_nPriceCellEditBox) //gi÷ l¹i vËt phÈm cã gi¸ 
					bFilter = FALSE;
				else if (m_bLevelCheckBox && Item[pItem->nIdx].GetLevel() >= m_nLevelEditBox) //gi÷ l¹i vËt phÈm cã cÊp 
					bFilter = FALSE;
				else if (m_bKeepPurpleCheckBox && Item[pItem->nIdx].GetKind() == purple_item && IsRowMagicItem(pItem->nIdx, m_nKeepPurpleEditBox)) //gi÷ l¹i ®å tÝm n dßng
					bFilter = FALSE;

				if (bFilter)
				{					
					P.nPlace = pItem->nPlace;
					P.nX = pItem->nX;
					P.nY = pItem->nY;
					Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
					return;
				}
			}
		}
		int n = 0;
		while(pItem) 
		{ 
			pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
			n++;
			if (pItem)
			{
				if (pItem->nPlace == pos_equiproom && Item[pItem->nIdx].GetGenre() == item_equip && Item[pItem->nIdx].GetDetailType() < equip_horse)
				{
					BOOL bFilter = TRUE;
					if (Item[pItem->nIdx].GetKind() == gold_item)
						bFilter = FALSE;
					else if (Item[pItem->nIdx].GetPlayerItemLock() > 0)
						bFilter = FALSE;
					else if (Item[pItem->nIdx].GetPlayerItemLock()  == -2)
						bFilter = FALSE;
					else if (Item[pItem->nIdx].GetDurability() == 0)	//#trang bi hong khong vut ra
						bFilter = FALSE;
					else if (m_bRiAmPeCheckBox && IsRAPEquip(Item[pItem->nIdx].GetDetailType()) //gi÷ l¹i trang søc cã level tõ a ®Õn b
						&& Item[pItem->nIdx].GetLevel() >= m_nRiEditBox && Item[pItem->nIdx].GetLevel() < m_nAmPeEditBox)
						bFilter = FALSE;							
					else if (IsEquipSatisfyCondition(pItem->nIdx))
						bFilter = FALSE;
					else if (m_bPriceCellCheckBox && Item[pItem->nIdx].GetSalePrice() >= m_nPriceCellEditBox) //gi÷ l¹i vËt phÈm cã gi¸ 
						bFilter = FALSE;
					else if (m_bLevelCheckBox && Item[pItem->nIdx].GetLevel() >= m_nLevelEditBox) //gi÷ l¹i vËt phÈm cã cÊp 
						bFilter = FALSE;
					else if (m_bKeepPurpleCheckBox && Item[pItem->nIdx].GetKind() == purple_item && IsRowMagicItem(pItem->nIdx, m_nKeepPurpleEditBox)) //gi÷ l¹i ®å tÝm n dßng
						bFilter = FALSE;
							
					if (bFilter) //®­a lªn tay
					{												
						P.nPlace = pItem->nPlace;
						P.nX = pItem->nX;
						P.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
						return;
					}
				}
			}
		}
	}
}

BOOL KPlayerAuto::IsRAPEquip(BYTE btDetail)
{
	if (btDetail == equip_ring || btDetail == equip_amulet || btDetail == equip_pendant)
		return TRUE;
	return FALSE;
}

BOOL KPlayerAuto::IsBlackObjectName(int nObjIndex)
{
	for(int i = 0; i < defMAX_AUTO_FILTERL; i++)
	{
		if(strcmp(Object[nObjIndex].m_szName, m_BlackItemList[i]) == 0)//vËt phÈm ®en
			return TRUE;
	}
	return FALSE;
}

BOOL KPlayerAuto::IsBlackItemName(int nItemIndex)
{
	for(int i = 0; i < defMAX_AUTO_FILTERL; i++)
	{
		if(strcmp(Item[nItemIndex].GetName(), m_BlackItemList[i]) == 0)//vËt phÈm ®en
			return TRUE;
	}
	return FALSE;
}

BOOL KPlayerAuto::IsRowMagicItem(int nIdx, int row)//kiÓm tra item cã n dßng thuéc tÝnh xanh, vµng, tÝm, b¹ch kim
{
	int checkR = 0;

	if (nIdx <= 0 || nIdx >= MAX_ITEM)
		return FALSE;	
	
	for (int j = 0; j < MAX_ITEM_MAGICLEVEL; j++)
	{
		if (Item[nIdx].m_aryMagicAttrib[j].nAttribType >= 0 && Item[nIdx].m_aryMagicAttrib[j].nValue[0] >= 0)
			checkR = checkR + 1;
	}

	if(checkR >= row)
		return TRUE;

	return FALSE;
}

BOOL KPlayerAuto::IsEquipSatisfyCondition(int nIdx) //läc ®å
{
	if (nIdx <= 0 || nIdx >= MAX_ITEM)
		return FALSE;
		
	int i, j;
	for (i = 0; i < defMAX_AUTO_FILTERL; i++)
	{
		if (m_FilterMagic[i][0])
		{
			for (j = 0; j < MAX_ITEM_MAGICLEVEL; j++)
			{
				if (Item[nIdx].m_aryMagicAttrib[j].nAttribType == m_FilterMagic[i][0] && 
					Item[nIdx].m_aryMagicAttrib[j].nValue[0] >= m_FilterMagic[i][1])
					return TRUE;
			}
		}
		else
		{

		}
	}
	return FALSE;
}

void KPlayerAuto::DoSortEquipment() //kÝch ho¹t chÕ ®é xÕp hµnh trang
{
	if(!m_bSortBagCheckBox)
		return;

	if(!m_bSortEquipment)//khi ®ang nghØ kh«ng xÕp
	{
		if(myLockActionTabRecover(TIME_SORT_ROOM_E))
			return;

		mySetValueCountDown(TIME_SORT_ROOM_E, 18*60*m_nSortBagEditBox);

		for (int i = MAX_ITEM - 1; i > 0 ; i--)
		{
			m_sListEquipment.m_Link.Remove(i);
		}
		m_bSortEquipment = TRUE;
	}
}

void KPlayerAuto::SortEquipment() //s¾p xÕp hµnh trang
{
	if (m_bSortEquipment)
	{
		int nHand = Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();//dang o tren tay
		if (nHand)
		{
			ItemPos	P;
			if ( FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(Item[nHand].GetWidth(), Item[nHand].GetHeight(), &P) )		
			{
				m_bSortEquipment = FALSE;
				return;
			}		
			Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
			if (!m_sListEquipment.FindSame(nHand))
			{
				m_sListEquipment.m_Link.Insert(nHand);
			}				
		}
		else//trong hanh trang
		{
			ItemPos	P;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			
			if (pItem)
			{
				if (pItem->nPlace == pos_equiproom)
				{
					if (!m_sListEquipment.FindSame(pItem->nIdx))
					{
						P.nPlace = pItem->nPlace;
						P.nX = pItem->nX;
						P.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
						return;
					}
				}
			}

			while(pItem) 
			{ 
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem)
				{
					if (pItem->nPlace == pos_equiproom)
					{
						if (!m_sListEquipment.FindSame(pItem->nIdx))
						{
							P.nPlace = pItem->nPlace;
							P.nX = pItem->nX;
							P.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
							return;
						}
					}
				}
			}
			m_bSortEquipment = FALSE;
		}
	}
}

int K2EquipmentArray::FindSame(int i)
{
	int nIdx = 0;
	while(1)
	{
		nIdx = m_Link.GetNext(nIdx);
		if (!nIdx)
			break;

		if (nIdx == i)
			return nIdx;
	}
	return 0;
}

//--------------------fkauto Begin tæ ®éi ------------------
void KPlayerAuto::DoAutoKickTeamOut()//Kick out thµnh viªn v¾ng mÆt 
{/* t¹m ®ãng ch­a sö dông ®­îc
	if(!m_bAutoCTeamCheckBox)
		return;

	if(!m_bKickTeamCheckBox)
		return;

	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag) //lµ ®éi tr­ëng
	{
		BYTE t_Mem = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetTotalMemer();
		if(t_Mem <= 0)
			return;
		//
		int nTeamCount1 = 8;	//--danh s¸ch thµnh viªn nhãm hiÖn hµnh
		KUiPlayerItem*	m_pPlayersList1 = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nTeamCount1);
		int nRetTeam = g_Team[0].GetMemberInfo(m_pPlayersList1, nTeamCount1);
		//
		int nTeamCount2 = 16;//--danh s¸ch ng­êi xung quanh
		KUiPlayerItem*	m_pPlayersList2 = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nTeamCount2);
		int nRetAround = NpcSet.GetAroundPlayer(m_pPlayersList2, nTeamCount2);
		//
		for(int i = 0; i < nRetTeam; i++)
		{
			BOOL checkPresent = FALSE;
			int nNpcIdx = NpcSet.SearchID(m_pPlayersList1[i].uId);
			for(int j = 0; j < nRetAround; j++)
			{
				if(strcmp(m_pPlayersList1[i].Name,  m_pPlayersList2[j].Name) == 0) //cã mÆt xung quanh
				{
					Npc[nNpcIdx].m_nTimeAbsent = 0;
					checkPresent = TRUE;
				}
			}
			//
			if(!checkPresent) //v¾ng mÆt
			{
				if(strcmp(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, m_pPlayersList1[i].Name) != 0) //kh«ng tÝnh thêi gian víi ®éi tr­ëng
				{
					Npc[nNpcIdx].m_nTimeAbsent++;
					if(Npc[nNpcIdx].m_nTimeAbsent >= m_nKickTeamEditBox * 100)//v¾ng mÆt qu¸ thêi gian quy ®Þnh
					{
						Npc[nNpcIdx].m_nTimeAbsent = 0;
						Player[CLIENT_PLAYER_INDEX].TeamKickMember(m_pPlayersList1[i].uId);//§uæi ra khái nhãm
						char szTemp[128];
						sprintf(szTemp, "<color=yellow>%s bÞ ®uæi khái nhãm v× v¾ng mÆt qu¸ l©u.<color>", m_pPlayersList1[i].Name);
						AutoSendMsg(szTemp);
					}
				}
			}
		}
		//
		if(m_pPlayersList1)
		{
			memset(m_pPlayersList1, 0, sizeof(m_pPlayersList1) * nTeamCount1);
			free(m_pPlayersList1);
			m_pPlayersList1 = NULL;
		}
		if(m_pPlayersList2)
		{
			memset(m_pPlayersList2, 0, sizeof(m_pPlayersList2) * nTeamCount2);
			free(m_pPlayersList2);
			m_pPlayersList2 = NULL;
		}
	}*/
}

void KPlayerAuto::DoAutoParty() //---tæ ®éi---
{
	if(!m_bAutoCTeamCheckBox)
		return;
	//
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecpt = m_bPTAllCheckBox;	//bËt t¾t tù ®éng nhËn lêi mêi
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecptAll = m_bPTAllCheckBox; //bËt t¾t tù ®éng vµo tÊt c¶ c¸c nhãm
	//
	if(m_bAlwayLeaderCheckBox)//lu«n lµm nhãm tr­ëng
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecpt = FALSE;	//t¾t tù ®éng nhËn lêi mêi
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecptAll = FALSE; //t¾t tù ®éng vµo tÊt c¶ c¸c nhãm
		if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag) //lµ ®éi tr­ëng
			AutoParty();
		else //ch­a ph¶i lµ ®éi tr­ëng
			Player[CLIENT_PLAYER_INDEX].ApplyCreateTeam(); 
	}
	else
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecpt = TRUE; //Tù ®éng vµo nhãm nÕu kh«ng ph¶i lµ ®éi tr­ëng
	}
	//
	BYTE t_Mem = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetTotalMemer();
	if(t_Mem > 0) //khi ®· cã nhãm
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecpt = FALSE;	//t¾t tù ®éng nhËn lêi mêi
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecptAll = FALSE; //t¾t tù ®éng vµo tÊt c¶ c¸c nhãm
	}
	//
	if(m_bLeaveTeamCheckBox && t_Mem > 0 && t_Mem < m_nLeaveTeamEditBox1)//rêi nhãm khi thµnh viªn Ýt h¬n n ng­êi sau n phót
	{
		if(Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_CAPTAIN)
		{
			m_nLeaveTeamCountDown ++;
			if(m_nLeaveTeamCountDown >= m_nLeaveTeamEditBox2*100) //qu¸ sè phót thiÕt lËp sè ng­êi vÉn Ýt h¬n
			{
				Player[CLIENT_PLAYER_INDEX].LeaveTeam();
				m_nLeaveTeamCountDown = 0;
				char szTemp[128];
				sprintf(szTemp, "<color=yellow>Tù rêi tæ ®éi khi thµnh viªn qu¸ Ýt.<color>");
				AutoSendMsg(szTemp);
			}
		}
	}
	else//tÝnh l¹i tõ ®Çu sè thµnh viªn ®ñ ®iÒu kiÖn
	{
		m_nLeaveTeamCountDown = 0;
	}
	//
	if(m_bSCountTeamCheckBox)
	{
		char szTeamMem[64];
		memset(szTeamMem, 0, sizeof(szTeamMem));
		sprintf(szTeamMem, "Thµnh viªn nhãm %d ng­êi", t_Mem);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetSzTeamMem(szTeamMem);
	}
	else
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ClearSzTeamMem();
	}
	//
	if(!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_AutoPT_PlayerList[0][0] && m_AutoPT_PlayerList[0][0]) //fix by phong kiÒu khi gi¶i t¸n PT hoÆc rít m¹ng bÞ mÊt listPT
	{
		for(int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			strcpy(Player[CLIENT_PLAYER_INDEX].m_cTeam.m_AutoPT_PlayerList[i], m_AutoPT_PlayerList[i]);
		}
	}
	//
	if(m_bPTTongCheckBox != Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bPTTongCheckBox)//fix by phong kiÒu khi gi¶i t¸n PT hoÆc rít m¹ng bÞ mÊt PT theo bang
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bPTTongCheckBox = m_bPTTongCheckBox;
}

void KPlayerAuto::AutoParty()
{
	int nCount = NpcSet.GetAroundPlayerForTeamInvite(NULL, 0);
	KUiPlayerItem*	m_pNearbyPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nCount);
	NpcSet.GetAroundPlayerForTeamInvite(m_pNearbyPlayersList, nCount);
	for (int i = 0; i < nCount; i++)
	{
		if(m_bPTAllCheckBox)//tù ®éng mêi tÊt c¶
			Player[CLIENT_PLAYER_INDEX].TeamInviteAdd(m_pNearbyPlayersList[i].uId);
		else
		{
			if(m_bPTTongCheckBox)//mêi PT cïng bang
			{
				if(strcmp(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_szTongName, 
					Npc[m_pNearbyPlayersList[i].nIndex].m_szTongName) == 0)
				{
					Player[CLIENT_PLAYER_INDEX].TeamInviteAdd(m_pNearbyPlayersList[i].uId);
					return;//kh«ng mêi bªn d­íi n÷a
				}
			}
			//
			for(int j = 0; j < defMAX_AUTO_MOVEMPSL; j++) //add by phong kiÒu qu¶n lý tæ ®éi
			{
				if(m_AutoPT_PlayerList[j][0] && strcmp(m_pNearbyPlayersList[i].Name,m_AutoPT_PlayerList[j]) ==0)//chØ mêi nh÷ng thµnh viªn cã tªn trong ds
					Player[CLIENT_PLAYER_INDEX].TeamInviteAdd(m_pNearbyPlayersList[i].uId);
			}
		}
	}
	if(m_pNearbyPlayersList)
	{
		memset(m_pNearbyPlayersList, 0, sizeof(m_pNearbyPlayersList) * nCount);
		free(m_pNearbyPlayersList);
		m_pNearbyPlayersList = NULL;
	}
}
//--------------------fkauto End tæ ®éi ------------------

//--------------------fkauto Begin trong thµnh ------------------
BYTE checkOpenDialog = 0;
KSystemMessage	sMsg;
int	nfkStepPos = 0;
//
void KPlayerAuto::FkDoAutoMap()
{
	if (!(g_SubWorldSet.GetGameTime() % 10))
	{
		if(!m_bReturnCheckBox)
			return;
		//
		if(!FkAutoMapCheckWorld())
		{
			AutoSendMsg("<color=yellow>vÒ 7 thµnh thÞ trung t©m më r­¬ng.<color>");
			return;
		}
		//nhÆt trong thµnh
		if(m_bNoneFightCheckBox)
		{
			RefreshObject();
			DoActackObject();
		}
		
		FkAutoMapSellItem();
		FkAutoMapGetMoney();
		FkAutoMapRepairItem();
		FkAutoMapBuyItemMM();
		FkAutoMapBuyItemTDP();
		FkAutoMapHoldMoney();
		FkAutoMapStoreItem();
		FkAutoMapReturn();
	}
}

BOOL KPlayerAuto::FkAutoMapCheckWorld()
{
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int m_SubWorldID = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;
	if(!m_SubWorldID)
		return FALSE;
	if(m_SubWorldID == 1 || m_SubWorldID == 11 || m_SubWorldID == 37 || m_SubWorldID == 78 
		|| m_SubWorldID == 80 || m_SubWorldID == 162 || m_SubWorldID == 176)//thµnh thÞ
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void KPlayerAuto::FkAutoMapSellItem()
{
	BOOL m_bCheck = FALSE;
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	//
	if(!Npc[m_nIndex].m_bRideHorse)//lªn ngùa di chuyÓn
	{	
		SendClientCmdRide(FALSE);
	}
	if(m_AutoMap_Status == AUTO_MAP_STATUS_IDLE)
	{
		m_AutoMap_Status = AUTO_MAP_STATUS_SELL;
		nfkStepPos = 0;
		mySetValueCountDown(TIME_AUTO_MAP, 18);
		char pMsgBuff[Def_MAXLEN_STRING_CHAT] = "chuÈn bÞ...";
		Npc[m_nIndex].SetChatInfo("fkauto", pMsgBuff, strlen(pMsgBuff));
	}
	if(m_AutoMap_Status == AUTO_MAP_STATUS_SELL)
	{
		//Npc[m_nIndex].FkAutoSetBlur(FALSE); // fix lçi thiªn v­¬ng bang phï vÒ thµnh bÞ ¶o ¶nh
		//
		if(myLockActionTabRecover(TIME_AUTO_MAP))
		{
			return;
		}
		//if(m_bSellCheckBox || m_bRepairCheckBox || m_bBuyHPCheckBox || m_bBuyMPCheckBox || m_bBuyToxicCheckBox)
		{
			char pMsgBuff[Def_MAXLEN_STRING_CHAT] = "t×m hiÖu thuèc.";
			Npc[m_nIndex].SetChatInfo("fkauto", pMsgBuff, strlen(pMsgBuff));
			if(FkAutoMapGotoPharmacies() == TRUE)//Check ch¹y ®Õn hiÖu thuèc// më shop //b¸n hÕn ®å
			{
				m_bCheck = TRUE;
			}
		}
		//else
		//	m_bCheck = TRUE;
		if(m_bCheck)
			m_AutoMap_Status = AUTO_MAP_STATUS_GETMONEY;
	}
}

void KPlayerAuto::FkAutoMapGetMoney()
{
	BOOL m_bCheck = FALSE;
	if(m_AutoMap_Status == AUTO_MAP_STATUS_GETMONEY)
	{
		if(m_bGetMoneyCheckBox)
		{
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			strcpy(sMsg.szMessage, "fkauto ®ang rót tiÒn.");
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			//nÕu kho¸ r­¬ng th× më b»ng mËt khÈu thiÕt lËp trong auto
			if(!Player[CLIENT_PLAYER_INDEX].m_CUnlocked)
			{
				strcpy(sMsg.szMessage, "fkauto ®ang më r­¬ng.");
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				SendClientCPUnlockCmd(m_nPwdMoneyEditBox);
			}
			//
			if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < m_nGetMoneyEditBox*10000)
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(room_repository,room_equipment, 
				(m_nGetMoneyEditBox*10000) - Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney());
			else
			{
				m_bCheck = TRUE;
			}
		}
		else
			m_bCheck = TRUE;
		if(m_bCheck)
			m_AutoMap_Status = AUTO_MAP_STATUS_REPAIR;
	}
}

void KPlayerAuto::FkAutoMapRepairItem()
{
	BOOL m_bCheck = FALSE;
	if(m_AutoMap_Status == AUTO_MAP_STATUS_REPAIR)
	{
		if(m_bRepairCheckBox)
		{
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			strcpy(sMsg.szMessage, "fkauto ®ang söa ®å.");
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			//
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if(pItem)
			{
				if(pItem->nPlace == pos_equip && FkAutoCheckItemRepair(pItem->nIdx) == TRUE)
				CoreDataChanged(GDCNI_FK_AUTO_ITEM, pItem->nIdx, 1); //söa
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if(pItem)
				{
					if(pItem->nPlace == pos_equip  && FkAutoCheckItemRepair(pItem->nIdx) == TRUE)
						CoreDataChanged(GDCNI_FK_AUTO_ITEM, pItem->nIdx, 1);//söa
				}
			}
			m_bCheck = TRUE;
		}
		else
			m_bCheck = TRUE;
		if(m_bCheck)
			m_AutoMap_Status = AUTO_MAP_STATUS_BUYITEM_MM;
	}
}

void KPlayerAuto::FkAutoMapBuyItemMM()
{
	BOOL m_bCheck = FALSE;
	if(m_AutoMap_Status == AUTO_MAP_STATUS_BUYITEM_MM)
	{
		if(m_bBuyHPCheckBox || m_bBuyMPCheckBox || m_bBuyToxicCheckBox)
		{
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			strcpy(sMsg.szMessage, "fkauto ®ang m¸u mana gi¶i ®éc.");
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			//
			if(m_bBuyHPCheckBox && FkAutoCountItemName(pos_equiproom, m_BuyHPTxtSelect) < m_nBuyHPEditBox)
				CoreDataChanged(GDCNI_FK_AUTO_ITEM, (unsigned int)&m_BuyHPTxtSelect, 2); // mua m¸u
			else if(m_bBuyMPCheckBox && FkAutoCountItemName(pos_equiproom, m_BuyMPTxtSelect) < m_nBuyMPEditBox)
				CoreDataChanged(GDCNI_FK_AUTO_ITEM, (unsigned int)&m_BuyMPTxtSelect, 2); // mua mana
			else if(m_bBuyToxicCheckBox && FkAutoCountItemName(pos_equiproom, m_BuyToxicTxtSelect) < m_nBuyToxicEditBox)
				CoreDataChanged(GDCNI_FK_AUTO_ITEM, (unsigned int)&m_BuyToxicTxtSelect, 2); // mua gi¶i ®éc
			else
				m_bCheck = TRUE;
		}
		else
			m_bCheck = TRUE;
		if(m_bCheck)
		{
			m_AutoMap_Status = AUTO_MAP_STATUS_BUYITEM_TDP;
			CoreDataChanged(GDCNI_FK_AUTO_ITEM, 0, -1); //®ãng shop
			checkOpenDialog = 0;
			nfkStepPos = 0;
			bMoveToCalled = false; // Flag to track if MoveTo has been called
		}
	}
}

void KPlayerAuto::FkAutoMapBuyItemTDP()
{
	BOOL m_bCheck = FALSE;
	if(m_AutoMap_Status == AUTO_MAP_STATUS_BUYITEM_TDP)
	{
		if(m_bBuyTownCheckBox)
		{
			char pMsgBuff[Def_MAXLEN_STRING_CHAT] = "®ang t×m t¹p ho¸.";
			int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			Npc[m_nIndex].SetChatInfo("fkauto", pMsgBuff, strlen(pMsgBuff));
			if(FkAutoMapGotoGroceryStore() == TRUE) //Check ch¹y ®Õn tiÖm t¹p ho¸// më shop
			{
				char m_BuyTownTxtSelect[64];
				strcpy(m_BuyTownTxtSelect, "Thæ §Þa Phï ");
				if(m_bBuyTownCheckBox && FkAutoCountItemName(pos_equiproom, m_BuyTownTxtSelect) < m_nBuyTownEditBox)
				{
					CoreDataChanged(GDCNI_FK_AUTO_ITEM, (unsigned int)&m_BuyTownTxtSelect, 2); // mua thæ ®Þa phï
					//
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					strcpy(sMsg.szMessage, "fkauto ®ang mua thæ ®Þa phï.");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				else
					m_bCheck = TRUE;
			}
		}
		else
			m_bCheck = TRUE;
		if(m_bCheck)
		{
			m_AutoMap_Status = AUTO_MAP_STATUS_HOLDMONEY;
			CoreDataChanged(GDCNI_FK_AUTO_ITEM, 0, -1); //®ãng shop
			checkOpenDialog = 0;
			nfkStepPos = 0;
			bMoveToCalled = false; // Flag to track if MoveTo has been called
		}
	}
}

void KPlayerAuto::FkAutoMapHoldMoney()
{
	BOOL m_bCheck = FALSE;
	if(m_AutoMap_Status == AUTO_MAP_STATUS_HOLDMONEY)
	{
		//if(m_bKeepMoneyCheckBox) //check box cÊt tiÒn cã check
		{
			int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			char pMsgBuff[Def_MAXLEN_STRING_CHAT] = "®ang t×m r­¬ng.";
			Npc[m_nIndex].SetChatInfo("fkauto", pMsgBuff, strlen(pMsgBuff));
			if(FkAutoMapGotoStoreBox() == TRUE)//ch¹y ®Õn r­¬ng // më r­¬ng // cÊt tiÒn
			{
				if(myLockActionTabRecover(TIME_AUTO_MAP))
					return;
				//
				if(m_bKeepMoneyCheckBox && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() > m_nKeepMoneyEditBox*10000)
				{
					char pMsgBuff[Def_MAXLEN_STRING_CHAT] = "®ang cÊt tiÒn.";
					Npc[m_nIndex].SetChatInfo("fkauto", pMsgBuff, strlen(pMsgBuff));
					//
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(room_equipment, room_repository, 
					Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() - (m_nKeepMoneyEditBox*10000));
				}
				else
				{
					m_bCheck = TRUE;
				}
			}
		}
		//else
		//	m_bCheck = TRUE;
		if(m_bCheck)
		{ 
			m_AutoMap_Status = AUTO_MAP_STATUS_STOREITEM; //cÊt ®å
			mySetValueCountDown(TIME_AUTO_MAP, 18);
			bMoveToCalled = false; // Flag to track if MoveTo has been called
		}
	}
}

void KPlayerAuto::FkAutoMapStoreItem()//cÊt ®å
{
	BOOL m_bCheck = FALSE;
	if(m_AutoMap_Status == AUTO_MAP_STATUS_STOREITEM)
	{
		if(m_bGetFYCheckBox)//checkbox cÊt ®å ®­îc tick
		{
			int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			char pMsgBuff[Def_MAXLEN_STRING_CHAT] = "®ang cÊt ®å.";
			Npc[m_nIndex].SetChatInfo("fkauto", pMsgBuff, strlen(pMsgBuff));
			//
			if(myLockActionTabRecover(TIME_AUTO_MAP))
				return;
			//
			if(FKAutoCheckStoreItemAll() == TRUE)
			{
				m_bCheck = TRUE;
			}
		}
		else
			m_bCheck = TRUE;
		if(m_bCheck)
		{
			CoreDataChanged(GDCNI_FK_AUTO_ITEM, 0, -2); //®ãng r­¬ng
			checkOpenDialog = 0;
			m_AutoMap_Status = AUTO_MAP_STATUS_RETURN;
			mySetValueCountDown(TIME_AUTO_MAP, 18);
			nfkStepPos = 0;
			bMoveToCalled = false; // Flag to track if MoveTo has been called
		}
	}
}

void KPlayerAuto::FkAutoMapReturn()
{
	BOOL m_bCheck = FALSE;
	if(m_AutoMap_Status == AUTO_MAP_STATUS_RETURN)
	{
		//if(m_bReturnCheckBox)
		{
			if(FkAutoMapGotoStation() == TRUE)//ch¹y ®Õn xa phu quay l¹i b·i
			{
				char pMsgBuff[Def_MAXLEN_STRING_CHAT] = "trë l¹i b·i train.";
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetChatInfo("fkauto", pMsgBuff, strlen(pMsgBuff));
				m_bCheck = TRUE;
				mySetValueCountDown(TIME_AUTO_MAP, 18);
			}
		}
		//else
		//	m_bCheck = TRUE;
		if(m_bCheck)
		{
			if(myLockActionTabRecover(TIME_AUTO_MAP))
				return;
			//
			FkAutoMapSet_StepOne();
		}
	}
}

BOOL KPlayerAuto::FkAutoMapGotoPharmacies()//ch¹y ®Õn hiÖu thuèc
{

	int pIndex = GetFkAutoPos(NPC_PHARMACIES);
	int n1X = 0, n1Y = 0;
	int n2X = 51552, n2Y = 103840;
	if(fk_autopos && pIndex >= 0) 
	{
		n2X = fk_autopos[pIndex].fk_nX[nfkStepPos];
		n2Y = fk_autopos[pIndex].fk_nY[nfkStepPos];
	}
	//
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	Npc[m_nIndex].GetMpsPos(&n1X, &n1Y);
	if(NpcSet.GetDistanceMps(n1X, n1Y, n2X, n2Y) < 100)
	{
		int nNpcIdx = NpcSet.SearchName("¤ng chñ d­îc ®iÕm");
		if(nNpcIdx && Npc[nNpcIdx].m_Kind == kind_dialoger)
		{
			int distance = NpcSet.GetDistance(nNpcIdx, m_nIndex);
			if (distance <= Npc[nNpcIdx].m_DialogRadius)
			{
				if(distance && checkOpenDialog == 0)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					Player[CLIENT_PLAYER_INDEX].DialogNpc(nNpcIdx); //më diaglog
					if(g_bUISelIntelActiveWithServer)
						checkOpenDialog = 1;
					mySetValueCountDown(TIME_AUTO_MAP, 18);
				}
				if(g_bUISelIntelActiveWithServer && checkOpenDialog == 1)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					CoreDataChanged(GDCNI_FK_AUTO_SELECTUI, 0, 0);//chän giao dÞch
					checkOpenDialog = 2;
					mySetValueCountDown(TIME_AUTO_MAP, 18);
				}
				if(checkOpenDialog == 2)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					//
					if(FkAutoCheckItemSellAll() == TRUE)
					{
						return TRUE;
					}
				}
			}
		}
	}
	//
      if (!bMoveToCalled)
        {
        
			KScenePlaceMapC sceneMap;  // Create an instance if not static
			sceneMap.AutoRunTo(n2X/16, n2Y/32);
			//Player[CLIENT_PLAYER_INDEX].m_cAutoMove.AutoMoveTo(n2X, n2Y);
            bMoveToCalled = true; // Set the flag to true after calling MoveTo
       }
    return FALSE;
}

BOOL KPlayerAuto::FkAutoMapGotoGroceryStore()//ch¹y ®Õn tiÖm t¹p ho¸
{
	int pIndex = GetFkAutoPos(NPC_GROCERY);
	int n1X = 0, n1Y = 0;
	int n2X = 51552, n2Y = 103840;
	if(fk_autopos && pIndex >= 0) 
	{
		n2X = fk_autopos[pIndex].fk_nX[nfkStepPos];
		n2Y = fk_autopos[pIndex].fk_nY[nfkStepPos];
	}
	//
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	Npc[m_nIndex].GetMpsPos(&n1X, &n1Y);
	if(NpcSet.GetDistanceMps(n1X, n1Y, n2X, n2Y) < 100)
	{
		int nNpcIdx;
		if(fk_autopos[pIndex].fk_mapID == 80)//d­¬ng ch©u
			nNpcIdx = NpcSet.SearchName("T¹p Hãa");
		else
			nNpcIdx = NpcSet.SearchName("Chñ tiÖm t¹p hãa");
		//
		if(nNpcIdx && Npc[nNpcIdx].m_Kind == kind_dialoger)
		{
			int distance = NpcSet.GetDistance(nNpcIdx, m_nIndex);
			if (distance <= Npc[nNpcIdx].m_DialogRadius)
			{
				if(distance && checkOpenDialog == 0)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					Player[CLIENT_PLAYER_INDEX].DialogNpc(nNpcIdx); //më diaglog
					if(g_bUISelIntelActiveWithServer)
						checkOpenDialog = 1;
					mySetValueCountDown(TIME_AUTO_MAP, 18);
				}
				if(g_bUISelIntelActiveWithServer && checkOpenDialog == 1)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					CoreDataChanged(GDCNI_FK_AUTO_SELECTUI, 0, 0); //chän giao dÞch
					checkOpenDialog = 2;
					mySetValueCountDown(TIME_AUTO_MAP, 18);
				}
				if(checkOpenDialog == 2)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;

					return TRUE;
				}
			}
		}
	}
  if (!bMoveToCalled)
        {
            // Call AutoRunTo to find and set the path
          //  MoveTo(n2X, n2Y);
			KScenePlaceMapC sceneMap;  // Create an instance if not static
			sceneMap.AutoRunTo(n2X/16, n2Y/32);
            bMoveToCalled = true; // Set the flag to true after calling MoveTo
       }
	return FALSE;
}

BOOL KPlayerAuto::FkAutoMapGotoStoreBox()//ch¹y ®Õn r­¬ng // më r­¬ng
{
	int pIndex = GetFkAutoPos(NPC_STORE_BOX);
	int n1X = 0, n1Y = 0;
	int n2X = 51552, n2Y = 103840;
	if(fk_autopos && pIndex >= 0) 
	{
		n2X = fk_autopos[pIndex].fk_nX[nfkStepPos];
		n2Y = fk_autopos[pIndex].fk_nY[nfkStepPos];
	}
	//
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	Npc[m_nIndex].GetMpsPos(&n1X, &n1Y);
	if(NpcSet.GetDistanceMps(n1X, n1Y, n2X, n2Y) < 100)
	{
		int nIdx = ObjSet.FindName("±¦Ïä1");//r­¬ng chøa ®å
		if(nIdx && Object[nIdx].m_nKind == Obj_Kind_Prop)
		{
			int nX1, nY1, nX2, nY2;
			Npc[m_nIndex].GetMpsPos(&nX1, &nY1);
			Object[nIdx].GetMpsPos(&nX2, &nY2);
			if ((nX1 - nX2) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2) < 150 * 150)
			{
				if(checkOpenDialog == 0)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					if(g_bUISelIntelActiveWithServer)
						checkOpenDialog = 1;
					mySetValueCountDown(TIME_AUTO_MAP, 18);
				}
				if(g_bUISelIntelActiveWithServer && checkOpenDialog == 1)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					Player[CLIENT_PLAYER_INDEX].CheckObject(nIdx); //më dialog
					checkOpenDialog = 2;
					mySetValueCountDown(TIME_AUTO_MAP, 18);
				}
				if(checkOpenDialog == 2)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					return TRUE;
				}
			}
		}
	}
	//
	int dX, dY, dZ;
    dX = n1X - n2X;
    dY = n1Y - n2Y;
    dZ = (int)sqrt((float)dX * dX + (float)dY * dY);
     if (!bMoveToCalled)
        {
            // Call AutoRunTo to find and set the path
          //  MoveTo(n2X, n2Y);
			KScenePlaceMapC sceneMap;  // Create an instance if not static
			sceneMap.AutoRunTo(n2X/16, n2Y/32);
            bMoveToCalled = true; // Set the flag to true after calling MoveTo
       }
	return FALSE;
}

BOOL KPlayerAuto::FkAutoMapGotoStation()//ch¹y ®Õn xa phu
{
	int pIndex = GetFkAutoPos(NPC_STATION);
	int n1X = 0, n1Y = 0;
	int n2X = 51552, n2Y = 103840;
	if(fk_autopos && pIndex >= 0) 
	{
		n2X = fk_autopos[pIndex].fk_nX[nfkStepPos];
		n2Y = fk_autopos[pIndex].fk_nY[nfkStepPos];
	}
	//
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	Npc[m_nIndex].GetMpsPos(&n1X, &n1Y);
	if(NpcSet.GetDistanceMps(n1X, n1Y, n2X, n2Y) < 100)
	{
		int nNpcIdx = NpcSet.SearchName("Xa phu");
		if(nNpcIdx && Npc[nNpcIdx].m_Kind == kind_dialoger)
		{
			int distance = NpcSet.GetDistance(nNpcIdx, m_nIndex);
			if (distance <= Npc[nNpcIdx].m_DialogRadius)
			{
				if(checkOpenDialog == 0)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					Player[CLIENT_PLAYER_INDEX].DialogNpc(nNpcIdx); //më diaglog
					if(g_bUISelIntelActiveWithServer)
						checkOpenDialog = 1;
					mySetValueCountDown(TIME_AUTO_MAP, 18);
				}
				if(g_bUISelIntelActiveWithServer && checkOpenDialog == 1)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					if(m_bGoFarAwayCheckBox && m_GoFarAwayTxtSelect == 0)
					{
						CoreDataChanged(GDCNI_FK_AUTO_SELECTUI, 2, 0); //2 quay l¹i ®iÓm cò
						checkOpenDialog = 0;
						return TRUE;
					}
					else if(m_bGoFarAwayCheckBox && m_GoFarAwayTxtSelect > 0)
					{
						CoreDataChanged(GDCNI_FK_AUTO_SELECTUI, 0, 0); //0 n¬i ®· ®i qua
						checkOpenDialog = 2;
						mySetValueCountDown(TIME_AUTO_MAP, 18);
					}
				}
				if(g_bUISelIntelActiveWithServer && checkOpenDialog == 2)
				{
					if(myLockActionTabRecover(TIME_AUTO_MAP))
						return FALSE;
					if(m_bGoFarAwayCheckBox && m_GoFarAwayTxtSelect == 1)
					{
						m_MoveMapStep = 0; //khi b¾t ®Çu lªn b·i th× set b¾t ®Çu ch¹y step 0 to¹ ®é ra b·i
						CoreDataChanged(GDCNI_FK_AUTO_SELECTUI, 0, 0); //n¬i ®· ®i qua 1
						checkOpenDialog = 0;
						return TRUE;
					}
					else if(m_bGoFarAwayCheckBox && m_GoFarAwayTxtSelect == 2)
					{
						m_MoveMapStep = 0; //khi b¾t ®Çu lªn b·i th× set b¾t ®Çu ch¹y step 0 to¹ ®é ra b·i
						CoreDataChanged(GDCNI_FK_AUTO_SELECTUI, 1, 0); //n¬i ®· ®i qua 2
						checkOpenDialog = 0;
						return TRUE;
					}
					else if(m_bGoFarAwayCheckBox &&  m_GoFarAwayTxtSelect == 3)
					{
						m_MoveMapStep = 0; //khi b¾t ®Çu lªn b·i th× set b¾t ®Çu ch¹y step 0 to¹ ®é ra b·i
						CoreDataChanged(GDCNI_FK_AUTO_SELECTUI, 2, 0); //n¬i ®· ®i qua 3
						checkOpenDialog = 0;
						return TRUE;
					}
				}
			}
		}
	}
	char pMsgBuff[Def_MAXLEN_STRING_CHAT] = "®ang t×m xa phu.";
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetChatInfo("fkauto", pMsgBuff, strlen(pMsgBuff));
	
	 if (!bMoveToCalled)
        {
            // Call AutoRunTo to find and set the path
          //  MoveTo(n2X, n2Y);
			KScenePlaceMapC sceneMap;  // Create an instance if not static
			sceneMap.AutoRunTo(n2X/16, n2Y/32);
            bMoveToCalled = true; // Set the flag to true after calling MoveTo
       }
	return FALSE;
}

BOOL KPlayerAuto::FkAutoCheckItemSell(int nIdx)
{
	if (Item[nIdx].GetGoldId() || Item[nIdx].GetPlayerItemLock() > 0 
	|| Item[nIdx].GetPlayerItemHLock() > 0
	|| Item[nIdx].GetPlayerItemLock() == -2
	|| FkAutoCheckEquipMagic(nIdx) //vËt phÈm cã trong magic trong bé läc kh«ng b¸n
	|| Item[nIdx].GetGenre() == item_task
	|| Item[nIdx].GetGenre() == item_medicine
	|| Item[nIdx].GetGenre() == item_mine
	|| Item[nIdx].GetGenre() == item_materials
	|| Item[nIdx].GetGenre() == item_townportal
	|| Item[nIdx].GetGenre() == item_magicscript )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL KPlayerAuto::FkAutoCheckItemRepair(int nIdx)
{
	if (Item[nIdx].GetGenre() != item_equip
	|| Item[nIdx].GetDurability() == -1 || Item[nIdx].GetMaxDurability() == -1 
	|| Item[nIdx].GetDurability() == Item[nIdx].GetMaxDurability()
	|| Item[nIdx].GetDurability() == 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BYTE KPlayerAuto::FkAutoCountItemName(int nRoom, char* nszItemName)
{
	BYTE nCountRes = 0;
	PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
	if(pItem)
	{
		if(pItem->nPlace == nRoom  && strcmp(Item[pItem->nIdx].GetName(), nszItemName) == 0)
			nCountRes++;
	}
	while(pItem)
	{
		pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
		if(pItem)
		{
			if(pItem->nPlace == nRoom  && strcmp(Item[pItem->nIdx].GetName(), nszItemName) == 0)
				nCountRes++;
		}
	}
	return nCountRes;
}

BOOL KPlayerAuto::FkAutoCheckItemSellAll()
{
	if(!m_bSellCheckBox) // kh«ng b¸n ®å bá qua c¸c b­íc d­íi
		return TRUE;
	//
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	strcpy(sMsg.szMessage, "fkauto ®ang b¸n ®å.");
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	//
	PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
	if(pItem)
	{
		if(pItem->nPlace == pos_equiproom && FkAutoCheckItemSell(pItem->nIdx) == TRUE)
		{
			CoreDataChanged(GDCNI_FK_AUTO_ITEM, pItem->nIdx, 0); //b¸n
			return FALSE;
		}
	}
	while(pItem)
	{
		pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
		if(pItem)
		{
			if(pItem->nPlace == pos_equiproom  && FkAutoCheckItemSell(pItem->nIdx) == TRUE)
			{
				CoreDataChanged(GDCNI_FK_AUTO_ITEM, pItem->nIdx, 0); //b¸n
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL KPlayerAuto::FKAutoCheckStoreItemAll()
{
	int nHand = Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
	if (nHand && FKAutoCheckStoreItem(nHand) == TRUE)
	{
		POINT pPos;
		for(int nRoom = room_repository; nRoom <= room_repository+Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId && nRoom <= room_exbox3; nRoom++)
		{
			if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[nRoom].FindRoom(Item[nHand].GetWidth(), Item[nHand].GetHeight(), &pPos))//t×m ®­îc kho¶ng trèng trong r­¬ng
			{
				ItemPos	P;
				P.nPlace = pos_repositoryroom+nRoom-room_repository; //thay ®æi vÞ trÝ item
				P.nX = pPos.x;
				P.nY = pPos.y;
				Player[CLIENT_PLAYER_INDEX].MoveItem(P, P); //chuyÓn item vµo r­¬ng
				return FALSE;
			}
		}
	}
	PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
	if(pItem)
	{
		if(pItem->nPlace == pos_equiproom && FKAutoCheckStoreItem(pItem->nIdx) == TRUE) //hîp lÖ míi ®­a lªn tay bá vµo r­¬ng
		{
			POINT pPos; //chuyÓn item lªn tay
			for(int nRoom = room_repository; nRoom <= room_repository+Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId && nRoom <= room_exbox3; nRoom++)
			{
				if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[nRoom].FindRoom(Item[pItem->nIdx].GetWidth(), Item[pItem->nIdx].GetHeight(), &pPos))
				{
					ItemPos	P;
					P.nPlace = pItem->nPlace;
					P.nX = pItem->nX;
					P.nY = pItem->nY;
					Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
					return FALSE;
				}
			}
		}
	}
	while(pItem)
	{
		pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
		if(pItem)
		{
			if(pItem->nPlace == pos_equiproom  && FKAutoCheckStoreItem(pItem->nIdx) == TRUE) //hîp lÖ míi ®­a lªn tay bá vµo r­¬ng
			{
				POINT pPos; //chuyÓn item lªn tay
				for(int nRoom = room_repository; nRoom <= room_repository+Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId && nRoom <= room_exbox3; nRoom++)
				{
					if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[nRoom].FindRoom(Item[pItem->nIdx].GetWidth(), Item[pItem->nIdx].GetHeight(), &pPos))
					{
						ItemPos	P;
						P.nPlace = pItem->nPlace;
						P.nX = pItem->nX;
						P.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
						return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL KPlayerAuto::FKAutoCheckStoreItem(int nIdx)
{
	if(FkAutoCheckEquipMagic(nIdx) == TRUE)
		return TRUE;
	//
	if(Item[nIdx].GetGoldId())
		return TRUE;
	//
	if (Item[nIdx].GetGenre() == item_task
	|| Item[nIdx].GetGenre() == item_medicine
	|| Item[nIdx].GetGenre() == item_mine
	|| Item[nIdx].GetGenre() == item_materials
	|| Item[nIdx].GetGenre() == item_townportal
	|| Item[nIdx].GetGenre() == item_magicscript )
	{
		return FALSE;
	}
	else 
	{
		return FALSE; //chØ cÊt nh÷ng vËt phÈm HKMP vµ cã trong bé läc
		//return TRUE;	//cÊt hÕt kÓ c¶ nh÷ng vËt phÈm kh«ng cã trong bé läc
	}
}

BOOL KPlayerAuto::FkAutoCheckEquipMagic(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_ITEM)
		return FALSE;

	int i, j;
	for (i = 0; i < defMAX_AUTO_FILTERL; i++)
	{
		if (m_FilterMagic[i][0])
		{
			for (j = 0; j < MAX_ITEM_MAGICATTRIB; j++)
			{
				if (Item[nIdx].m_aryMagicAttrib[j].nAttribType == m_FilterMagic[i][0] && 
					Item[nIdx].m_aryMagicAttrib[j].nValue[0] >= m_FilterMagic[i][1])
					return TRUE;
			}
		}
	}
	return FALSE;
}

void KPlayerAuto::FkAutoMapSet_StepOne()
{
	m_AutoMap_Status = AUTO_MAP_STATUS_IDLE;
	mySetValueCountDown(TIME_AUTO_MAP, 0);
	bMoveToCalled = false; // Flag to track if MoveTo has been called
}
//--------------------fkauto End trong thµnh ------------------

int	KPlayerAuto::AllItemNameEquipRoom(FKUiObjectName *pList, int nCount)
{
	FKUiObjectName *pLTemp;
	pLTemp = (FKUiObjectName*)malloc(60 * sizeof(FKUiObjectName)); //cÊp ph¸t 60 item name
	//
	BYTE nNum = 0;
	if(nCount <= 0)
	{
		PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
		if(pItem)
		{
			if(pItem->nPlace == pos_equiproom)
			{
				strcpy(pLTemp[nNum].FkName, Item[pItem->nIdx].GetName());
				nNum++;
			}
		}
		while(pItem)
		{
			pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
			if(pItem)
			{
				if(pItem->nPlace == pos_equiproom)
				{
					bool bCheck = true;
					for(int i = 0; i < nNum; i++)
					{
						if(strcmp(pLTemp[i].FkName, Item[pItem->nIdx].GetName()) == 0)
							bCheck = false;
					}
					if(bCheck)
					{
						strcpy(pLTemp[nNum].FkName, Item[pItem->nIdx].GetName());
						nNum++;
					}
				}
			}
		}
		if(pLTemp)
		{
			free(pLTemp); //gi¶i phãng vïng ®· cÊp ph¸t
			pLTemp = NULL;
		}
		return nNum;
	}
	//
	if (!pList)
		return 0;
	//
	PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
	if(pItem)
	{
		if(pItem->nPlace == pos_equiproom)
		{
			strcpy(pList[nNum].FkName, Item[pItem->nIdx].GetName());
			nNum++;
		}
	}
	while(pItem && nNum <= nCount)
	{
		pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
		if(pItem)
		{
			if(pItem->nPlace == pos_equiproom)
			{
				bool bCheck = true;
				for(int i = 0; i < nNum; i++)
				{
					if(strcmp(pList[i].FkName, Item[pItem->nIdx].GetName()) == 0)
						bCheck = false;
				}
				//
				if(bCheck) //nÕu ch­a cã trong list th× míi thªm vµo
				{
					strcpy(pList[nNum].FkName,  Item[pItem->nIdx].GetName());
					nNum++;
				}
			}
		}
	}
	return nNum;	
}

#endif
