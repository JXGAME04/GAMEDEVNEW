/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2020-9-12
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KCore.h"
#include "GameDataDef.h"
#include "CoreShell.h"
#include "CoreDrawGameObj.h"
#include "ImgRef.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KItemSet.h"
#include "KItemList.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#include "KItemGenerator.h"
#include "KNpcResList.h"
#include "Scene/KScenePlaceC.h"
#include "kskills.h"
#include "GameDataDef.h"
#include "MsgGenreDef.h"
#include "KOption.h"
#include "KSubWorld.h"
#include "KViewItem.h"
#include "KTongProtocol.h"
#include "malloc.h"
#include "KMagicDesc.h"
#include "KBuySell.h"
#include "KThiefSkill.h"
#include "KObjSet.h"
#include "KTaskFuns.h"
#include "KSellItem.h"

#include <cmath>
#include "KJXPathFinder.h"

#define	NPC_TRADE_BOX_WIDTH		6
#define	NPC_TRADE_BOX_HEIGHT	10
#define	MAX_TRADE_ITEM_WIDTH	2
#define	MAX_TRADE_ITEM_HEIGHT	4

IClientCallback* l_pDataChangedNotifyFunc = 0;

class KCoreShell : public iCoreShell
{
public:
	int	 GetProtocolSize(BYTE byProtocol);
	int	 Debug(unsigned int uDataId, unsigned int uParam, int nParam);
	int	 OperationRequest(unsigned int uOper, unsigned int uParam, int nParam);
	void ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam);
	int	 FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind);
	int FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind);
	int FindSpecialNPC(char* Name, void* pReturn, int& nKind);
	int ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength);
	void ApplyAddTeam(void* pPlayer);
	void TradeApplyStart(void* pPlayer);
	int UseSkill(int x, int y, int nSkillID);
	int UseSkillCastB(int x, int y, int nSkillID, int nNpcIdx);
	int LockSomeoneUseSkill(int nTargetIndex, int nSkillID);
	int LockSomeoneAction(int nTargetIndex);
	int LockObjectAction(int nTargetIndex);
	void GotoWhere(int x, int y, int mode);	//mode 0 is auto, 1 is walk, 2 is run
	void Goto(int nDir, int mode);	//nDir 0~63, mode 0 is auto, 1 is walk, 2 is run
	void Turn(int nDir);	//nDir 0 is left, 1 is right, 2 is back
	int ThrowAwayItem();
	int GetNPCRelation(int nIndex);
	int GetNPCBAITAN(int nIndex);
	int	GetNPCBAITAN2();
	int GetGenreItem2(unsigned int uId);
	int GetPriceSell2(unsigned int uId);
	int GetTypeItem(unsigned int uId);
	int GetStallState();
	int GetPriceSell(unsigned int uId);
	int GetNatureItem(unsigned int uItemId, unsigned int uGenre);
	int GetGenreItem(unsigned int uItemId, unsigned int uGenre);
	int	SceneMapOperation(unsigned int uOper, unsigned int uParam, int nParam);
	int	TongOperation(unsigned int uOper, unsigned int uParam, int nParam);
	int TeamOperation(unsigned int uOper, unsigned int uParam, int nParam);
	int	 GetGameData(unsigned int uDataId, unsigned int uParam, int nParam);
	void DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam);
	void DrawGameSpace();
	DWORD GetPing();
	//void SendPing();
	int	 SetCallDataChangedNofify(IClientCallback* pNotifyFunc);
	void NetMsgCallbackFunc(void* pMsgData);
	void SetRepresentShell(struct iRepresentShell* pRepresent);
	void SetMusicInterface(void* pMusicInterface);
	void SetRepresentAreaSize(int nWidth, int nHeight);
	int  Breathe();
	void Release();	
	void SetClient(LPVOID pClient);
	void SendNewDataToServer(void* pData, int nLength);
	int	GetOwnValue(int nMoneyUnit);
	int GetDataSuperShop(int nSaleId, unsigned int uParam, int nParam);
	int GetObjAtCountRegionInSuperShop(int nSaleId, unsigned int uParam, int nParam);
	int GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam);
	int GetItemIdxNpcShop(char* szItemName);//fkauto
	BOOL GetAutoFlag();
	BOOL GetFightFlag();
	void SetActiveAutoPlay(BOOL nActive = FALSE);
	void SetMoveMap(int nType, int nPos, int nValue);
	void SetSortItem(int nType, int nPos, int nValue);
	void FkAutoSetFillterMagic(int nType, int nPos, int nValue);
	int FindSkillInfo(int nType, int nIndex);
	void GetSkillName(int nSkillId, char* szSkillName);
	BOOL GetSkillData(int nSkillId, int *nLevel);
	BOOL GetFlagMode();
	void SetFlagMode(bool nIndex);
	void DirectFindPos(unsigned int uParam, int nParam, BOOL bSync, BOOL bPaintLine);
	BYTE GetPaintMode();
	void SetPaintMode(BYTE nIndex);
	int AutoPlayOperation(unsigned int uOper, unsigned int uParam, int nParam);//fkauto
	BOOL AutoMove();
	void ClearPathFinder();
	void GotoWhereDirect(int x, int y, int mode);	//mode 0 is auto, 1 is walk, 2 is run
};

static KCoreShell	g_CoreShell;

CORE_API void g_InitCore(char * nParmName);
#ifndef _STANDALONE
extern "C" __declspec(dllexport)
#endif
iCoreShell* CoreGetShell(char * nParmName)
{
	g_InitCore(nParmName);
	return &g_CoreShell;
}

void CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam)
{
	if (l_pDataChangedNotifyFunc)
		l_pDataChangedNotifyFunc->CoreDataChanged(uDataId, uParam, nParam);
}

void KCoreShell::Release()
{
	g_ReleaseCore();
}

void KCoreShell::NetMsgCallbackFunc(void* pMsgData)
{
	g_ProtocolProcess.ProcessNetMsg((BYTE *)pMsgData);
}

int	KCoreShell::SetCallDataChangedNofify(IClientCallback* pNotifyFunc)
{
	l_pDataChangedNotifyFunc = pNotifyFunc;
	return true;
}

BOOL KCoreShell::GetSkillData(int nSkillId, int *nLevel)
{
	int nList = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(nSkillId);
	if (nList > 0)
	{
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(nSkillId) > 0)
		{
			*nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillId);
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAddLevelIndex(nList) > 0)
				return TRUE;
			else
				return FALSE;
		}
	}
	return FALSE;
}

BOOL KCoreShell::GetFlagMode()
{
	return g_ScenePlace.bFlagMode;
}

void KCoreShell::SetFlagMode(bool bFlag)
{
	g_ScenePlace.bFlagMode = bFlag;
}

void KCoreShell::DirectFindPos(unsigned int uParam, int nParam, BOOL bSync, BOOL bPaintLine)
{
	g_ScenePlace.DirectFindPos(uParam, nParam, bSync, bPaintLine);
}

BYTE KCoreShell::GetPaintMode()
{
	return g_ScenePlace.bPaintMode;
}

void KCoreShell::SetPaintMode(BYTE bFlag)
{
	g_ScenePlace.DirectFindPos(0, 0, FALSE, FALSE);
	g_ScenePlace.bPaintMode = bFlag;
}

int	KCoreShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uDataId)
	{
	case GDI_PLAYER_IS_MALE:
		{
			int nIndex = 0;
			if (nParam == 0)
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			else
				nIndex = NpcSet.SearchID(nParam);

			if (nIndex)
				nRet = (Npc[nIndex].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID);
			else
				nRet = 1;	
		}
		break;
	case GDI_REPAIR_ITEM_PRICE:
		if (uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			KItem*	pItem = NULL;

			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						pItem = &Item[pObj->Obj.uId];
					}
				}
				break;
			default:
				break;
			}

			if (!pItem)
				break;

			KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
		
			if (pObj->eContainer == UOC_NPC_SHOP)
				break;
			pInfo->nCurPrice = pItem->GetRepairPrice();
			strcpy(pInfo->szItemName, pItem->GetName());
			nRet = pItem->CanBeRepaired();
			if(!nRet)//#thong bao khong the sua trang bi hong
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_NO_REPAIR_MONEY);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
		else
		{
			nParam = 0;
			nRet = 0;
		}
		break;
	case GDI_TRADE_ITEM_PRICE:
		if (uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			KItem*	pItem = NULL;

			switch(pObj->Obj.uGenre)
			{
			case CGOG_PLAYERSELLITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
						pInfo->nPrice = Item[pObj->Obj.uId].GetSetPrice();
						pInfo->nCurPrice = Item[pObj->Obj.uId].GetSetPrice();
						strcpy(pInfo->szItemName, Item[pObj->Obj.uId].GetName());
						return 1;
					}
				}
				break;
			case CGOG_ITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						pItem = &Item[pObj->Obj.uId];
					}
				}
				break;
			case CGOG_NPCSELLITEM:
				{
					int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
					if (nBuyIdx != -1)
					{
						int nIndex = BuySell.GetItemIndex(nBuyIdx, pObj->Obj.uId);
						if (nIndex >= 0)
							pItem = BuySell.GetItem(nIndex);
					}
				}
				break;
			default:
				break;
			}

			if (!pItem)
				break;

			KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
			pInfo->nItemNature = pItem->GetNature();
			pInfo->nOldPrice = pItem->GetOrgPrice();		
			if (pObj->eContainer == UOC_NPC_SHOP)
				pInfo->nCurPrice = pItem->GetCurPrice();
			else
				pInfo->nCurPrice = pItem->GetSalePrice();
			strcpy(pInfo->szItemName, pItem->GetName());
			pInfo->bNewArrival = pItem->GetNewArrival();
			pInfo->nMoneyUnit = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit;
			nRet = 1;
		}
		else
		{
			nParam = 0;
			nRet = 0;
		}
		break;

	case GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO:
	case GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO:
		if (nParam && uParam)
		{
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_PLAYERSELLITEM:
				{
					Item[pObj->Obj.uId].GetDesc(pszDescript, true, BUY_SELL_SCALE);
				}
				break;
			case CGOG_ITEM:
				{
					if (pObj->eContainer == UOC_EQUIPTMENT)
					{
						int nActive = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetActiveAttribNum(pObj->Obj.uId);
						Item[pObj->Obj.uId].GetDesc(pszDescript, true, BUY_SELL_SCALE, nActive);
					}
					else
					{
						int money_unit = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit;
						int item_index = pObj->Obj.uId;
						int item_place = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[item_index].nPlace;
						if(money_unit == moneyunit_money && item_place == pos_equiproom)//#chØ hiÖn thØ gi¸ b¸n b»ng tiÒn v¹n
						{
							Item[pObj->Obj.uId].GetDesc(pszDescript, true, BUY_SELL_SCALE);
						}
						else
						{
							Item[pObj->Obj.uId].GetDesc(pszDescript, false);
						}
					}
				}
				break;			
			case CGOG_NPCSELLITEM:
				{
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop])
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop], pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItem(nIdx);

					if (!pItem)
						break;
					pItem->GetDesc(pszDescript, true);
				}
				break;
			}
		}
		break;
	case GDI_GAME_OBJ_DESC:
		if (nParam && uParam)
		{
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					if (pObj->eContainer == UOC_EQUIPTMENT)
					{
						int nActive = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetActiveAttribNum(pObj->Obj.uId);
						if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetIfActive())
						{
							nActive = 4;
						}
						Item[pObj->Obj.uId].GetDesc(pszDescript, false, 1, nActive);
					}
					else
					{
						if(Item[pObj->Obj.uId].IsFkItemSkill() == true)
						{
							int nSkillId = Item[pObj->Obj.uId].GetParticular();
							int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(nSkillId);
							if(nLevel > 0)
								nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillId);
							KSkill::GetDesc(nSkillId, nLevel, pszDescript, Player[CLIENT_PLAYER_INDEX].m_nIndex, false);
						}
						else
							Item[pObj->Obj.uId].GetDesc(pszDescript);
					}

					if(Item[pObj->Obj.uId].GetPlayerItemIsHoureOpen())//xu ly vat pham cho mo khoa bao hiem
					{
						PLAYER_LOCK_UNLOCK_ITEM lockCmd;
						lockCmd.ProtocolType = c2s_plockitem;
						lockCmd.itemIdx = Item[pObj->Obj.uId].GetID();
						lockCmd.islock = 0;
						if (g_pClient)
							g_pClient->SendPackToServer((BYTE*)&lockCmd, sizeof(PLAYER_LOCK_UNLOCK_ITEM));
					}

					if(Item[pObj->Obj.uId].GetTimeYearIsExp())//xu ly vat pham het han su dung
					{
						REMOVE_ITEM_YEAR_EXP lockCmd;
						lockCmd.ProtocolType = c2s_itemyearexp;
						lockCmd.itemIdx = Item[pObj->Obj.uId].GetID();
						if (g_pClient)
							g_pClient->SendPackToServer((BYTE*)&lockCmd, sizeof(REMOVE_ITEM_YEAR_EXP));
					}
				}
				break;
			case CGOG_SKILL:
			case CGOG_SKILL_FIGHT:
			case CGOG_SKILL_LIVE:
			case CGOG_SKILL_SHORTCUT:
				{
					int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(pObj->Obj.uId);
					if(nLevel > 0)
						nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(pObj->Obj.uId);
					_ASSERT(nLevel >= 0);
					if (pObj->Obj.uId >0)
					{						
						ISkill * pISkill = g_SkillManager.GetSkill(pObj->Obj.uId, 1);
						if (!pISkill)
							break;
						eSkillStyle eStyle = (eSkillStyle) pISkill->GetSkillStyle();
						
						switch(eStyle)
						{
						case SKILL_SS_Missles:			
						case SKILL_SS_Melee:
						case SKILL_SS_InitiativeNpcState:	
						case SKILL_SS_PassivityNpcState:		
							{
								int nList = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(pObj->Obj.uId);
									KSkill::GetDesc(
									pObj->Obj.uId,
									nLevel,
									pszDescript,
									Player[CLIENT_PLAYER_INDEX].m_nIndex,
									(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true
									);
							}
							break;
							
						case SKILL_SS_Thief:
							{
								((KThiefSkill *)pISkill)->GetDesc(
									pObj->Obj.uId,
									nLevel,
									pszDescript,
									Player[CLIENT_PLAYER_INDEX].m_nIndex,
									(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true
								);
			
							}break;
							
						}
					}
				}
				break;
			case CGOG_PLAYER_FACE:
				break;
			case CGOG_NPCSELLITEM:
				{
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop])
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop], pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItem(nIdx);

					if (!pItem)
						break;
					pItem->GetDesc(pszDescript);
				}
				break;
			case CGOG_IME_ITEM:
				{
					if (pObj->Obj.uId <= 0)
						return 0;
					Item[pObj->Obj.uId].GetDesc(pszDescript);
				}
				break;
			case CGOG_PLAYERSELLITEM:
				{
					Item[pObj->Obj.uId].GetDesc(pszDescript);
				}
				break;
			}
		}
		break;

	case GDI_PLAYER_BASE_INFO:
		if (uParam)
		{
			KUiPlayerBaseInfo* pInfo = (KUiPlayerBaseInfo*)uParam;
			int nIndex = 0;
			if (nParam == 0)
			{
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
				pInfo->nCurFaction = Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction;
				pInfo->nCurTong = Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongNameID();
				pInfo->nMissionGroup = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup;
			}
			else
			{
				nIndex = NpcSet.SearchID(nParam);
				pInfo->nCurFaction = -1;
				pInfo->nCurTong = 0;
				pInfo->nMissionGroup = -1;
			}
			if (nIndex)
			{
				strcpy(pInfo->Name, Npc[nIndex].Name);
				//to do:no implements in this version
				pInfo->Agname[0] = 0;
				pInfo->Title[0] = 0;
				strcpy(pInfo->szMateName, Npc[nIndex].MateName);//#MateName
				if (Npc[nIndex].m_btRankId)
				{
					char szRankId[5];
					itoa(Npc[nIndex].m_btRankId, szRankId, 10);
					g_RankTabSetting.GetString(szRankId, "RANKSTR", "", pInfo->Title, 32);
				}
				pInfo->nRankInWorld = Player[CLIENT_PLAYER_INDEX].m_nWorldStat; //xÕp h¹ng thÕ giíi
				pInfo->nRankInWorld = Npc[nIndex].nRankInWorld;		//tnxh
				pInfo->nRepute = Npc[nIndex].nRepute; // danh vong
				pInfo->nFuYuan = Npc[nIndex].nFuYuan;// phuc duyen
				pInfo->nPKValue = Npc[nIndex].nPKValue; // PK
				pInfo->nReBorn = Npc[nIndex].nReBorn; // trung sinh
			}
		}
		break;

	case GDI_PLAYER_RT_INFO:
		if (uParam)
		{
			KUiPlayerRuntimeInfo* pInfo = (KUiPlayerRuntimeInfo*)uParam;
			pInfo->nLifeFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax;		
			pInfo->nLife = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife;				
			pInfo->nManaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax;	
			pInfo->nMana = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana;				
			pInfo->nStaminaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaminaMax;
			pInfo->nStamina = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina;		
			pInfo->nAngryFull = 0;		
			pInfo->nAngry = 0;			
			pInfo->nExperienceFull = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;		
			pInfo->nExperience = Player[CLIENT_PLAYER_INDEX].m_nExp;					
			pInfo->nCurLevelExperience = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;
			pInfo->byActionDisable = 0;
			pInfo->byAction = PA_NONE;

			if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
				pInfo->byAction |= PA_RUN;

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
				pInfo->byAction |= PA_SIT;
			
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
				pInfo->byAction |= PA_RIDE;

			pInfo->wReserved = 0;
		}
		break;

	case GDI_PLAYER_RT_ATTRIBUTE:
		if (uParam)
		{
			KUiPlayerAttribute* pInfo = (KUiPlayerAttribute*)uParam;
			KNpc*	pNpc = &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];
			pInfo->nMoney = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);				
			pInfo->nBARemainPoint = Player[CLIENT_PLAYER_INDEX].m_nAttributePoint;					
			pInfo->nStrength = Player[CLIENT_PLAYER_INDEX].m_nCurStrength;								
			pInfo->nDexterity = Player[CLIENT_PLAYER_INDEX].m_nCurDexterity;								
			pInfo->nVitality = Player[CLIENT_PLAYER_INDEX].m_nCurVitality;								
			pInfo->nEnergy = Player[CLIENT_PLAYER_INDEX].m_nCurEngergy;									
			pInfo->nPKValue = Player[CLIENT_PLAYER_INDEX].m_cPK.GetPKValue();	//tnpk
			//pInfo->nRepute = Player[CLIENT_PLAYER_INDEX].m_cRepute.GetReputeValue(); // danh vong
			pInfo->nRepute = pNpc->nRepute;
			//pInfo->nFuYuan = Player[CLIENT_PLAYER_INDEX].m_cFuYuan.GetFuYuanValue(); // phuc duyen
			pInfo->nFuYuan = pNpc->nFuYuan;
			pInfo->nReBorn = Player[CLIENT_PLAYER_INDEX].m_cReBorn.GetReBornValue(); // trung sinh
			pInfo->nRankInWorld = Player[CLIENT_PLAYER_INDEX].m_nWorldStat; //xÕp h¹ng thÕ giíi

			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nKillMIN, &pInfo->nKillMAX, 0);				
			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nRightKillMin , &pInfo->nRightKillMax, 1);
			pInfo->nAttack = pNpc->m_CurrentAttackRating;				
			pInfo->nDefence = pNpc->m_CurrentDefend;					
			pInfo->nMoveSpeed = pNpc->m_CurrentRunSpeed;				
			pInfo->nAttackSpeed = pNpc->m_CurrentAttackSpeed;			
			pInfo->nCastSpeed = pNpc->m_CurrentCastSpeed;

			if (pNpc->m_CurrentPhysicsResistMax >= pNpc->m_CurrentPhysicsResist)
			{
				pInfo->nPhyDef = pNpc->m_CurrentPhysicsResist;
				pInfo->nPhyDefPlus = 0;
			}
			else
			{
				pInfo->nPhyDef = pNpc->m_CurrentPhysicsResistMax;
				pInfo->nPhyDefPlus = (pNpc->m_CurrentPhysicsResist - pNpc->m_CurrentPhysicsResistMax) / RESIST_PLUS_SCALE;
			}

			if (pNpc->m_CurrentColdResistMax >= pNpc->m_CurrentColdResist)
			{
				pInfo->nCoolDef = pNpc->m_CurrentColdResist;
				pInfo->nCoolDefPlus = 0;
			}
			else
			{
				pInfo->nCoolDef = pNpc->m_CurrentColdResistMax;
				pInfo->nCoolDefPlus = (pNpc->m_CurrentColdResist - pNpc->m_CurrentColdResistMax) / RESIST_PLUS_SCALE;
			}

			if (pNpc->m_CurrentLightResistMax >= pNpc->m_CurrentLightResist)
			{
				pInfo->nLightDef = pNpc->m_CurrentLightResist;
				pInfo->nLightDefPlus = 0;
			}
			else
			{
				pInfo->nLightDef = pNpc->m_CurrentLightResistMax;
				pInfo->nLightDefPlus = (pNpc->m_CurrentLightResist - pNpc->m_CurrentLightResistMax) / RESIST_PLUS_SCALE;
			}

			if (pNpc->m_CurrentFireResistMax >= pNpc->m_CurrentFireResist)
			{
				pInfo->nFireDef = pNpc->m_CurrentFireResist;
				pInfo->nFireDefPlus = 0;
			}
			else
			{
				pInfo->nFireDef = pNpc->m_CurrentFireResistMax;
				pInfo->nFireDefPlus = (pNpc->m_CurrentFireResist - pNpc->m_CurrentFireResistMax) / RESIST_PLUS_SCALE;
			}

			if (pNpc->m_CurrentPoisonResistMax >= pNpc->m_CurrentPoisonResist)
			{
				pInfo->nPoisonDef = pNpc->m_CurrentPoisonResist;
				pInfo->nPoisonDefPlus = 0;
			}
			else
			{
				pInfo->nPoisonDef = pNpc->m_CurrentPoisonResistMax;
				pInfo->nPoisonDefPlus = (pNpc->m_CurrentPoisonResist - pNpc->m_CurrentPoisonResistMax) / RESIST_PLUS_SCALE;
			}
			pInfo->nLevel = pNpc->m_Level;

			//edit by phong kieu ngu hanh
			memset(pInfo->StatusDesc, 0, sizeof(pInfo->StatusDesc));
			switch(pNpc->m_Series)
			{
			case series_water:
				strcpy(pInfo->StatusDesc, "HÖ Thñy");
				break;
			case series_wood:
				strcpy(pInfo->StatusDesc, "HÖ Méc");
				break;
			case series_metal:
				strcpy(pInfo->StatusDesc, "HÖ Kim");
				break;
			case series_fire:
				strcpy(pInfo->StatusDesc, "HÖ Háa");
				break;
			case series_earth:
				strcpy(pInfo->StatusDesc, "HÖ Thæ");
				break;
			}
		}
		break;

	case GDI_PLAYER_IMMED_ITEMSKILL:
		if (uParam)
		{
			KUiPlayerImmedItemSkill* pInfo = (KUiPlayerImmedItemSkill*)uParam;
			memset(pInfo,0,sizeof(KUiPlayerImmedItemSkill));
			pInfo->IMmediaSkill[0].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[0].uId		= Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
			pInfo->IMmediaSkill[1].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[1].uId		= Player[CLIENT_PLAYER_INDEX].GetRightSkill();

			for (int i = 0; i < MAX_IMMEDIACY_ITEM; i++)
			{
				pInfo->ImmediaItem[i].uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[room_immediacy].FindItem(i, 0);
				if (pInfo->ImmediaItem[i].uId > 0)
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_ITEM;
				}
				else
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_NOTHING;
				}
			}
		}
		break;

	case GDI_TRADE_PLAYER_ITEM_COUNT:
		nRet = 0;
		SendClientCmdGetCount(uParam);
		break;

	case GDI_TRADE_PLAYER_ITEM:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion*)uParam;
			
			for (int i = 0; i < 60; i++)
			{
				if (g_cSellItem.m_sItem[i].nIdx)
				{
					pInfo->Obj.uGenre = CGOG_PLAYERSELLITEM;
					pInfo->Obj.uId = g_cSellItem.m_sItem[i].nIdx;
					
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = g_cSellItem.m_sItem[i].nX;
					pInfo->Region.v = g_cSellItem.m_sItem[i].nY;
					pInfo->nContainer = g_cSellItem.m_sItem[i].nPrice;
					nCount++;
					pInfo++;
				}		
			}
			
			nRet = nCount;
		} 
		else
		{
			nRet = g_cSellItem.GetCount();
		}
		break;

	case GDI_PLAYER_IS_BAITAN:
		nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan;
		break;
	case GDI_AFFAIR_ITEM:		// pos tra vat pham nhiem vu
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_affairitem)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_affairitem)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_affairitem)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_affairitem)
					nCount++;
			}
			nRet = nCount;
		}
		break;

	case GDI_PLAYER_HOLD_MONEY:	
		nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);
		break;

	case GDI_PLAYER_HOLD_FKCOIN:	//add by phong kiÒu sè xu ng­êi trªn player
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_XU);
		break;

	case GDI_PLAYER_CAN_RIDE:
		break;

	case GDI_ITEM_TAKEN_WITH:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
					nCount++;
			}
			nRet = nCount;
		}
		break;

	case GDI_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{

			if (nParam == 1)
				break;

			int PartConvert[itempart_num] = 
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,		UIEP_MASK,	// mat na
				UIEP_FIFONG
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; i++)
			{
				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
				
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
	case GDI_PARADE_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{

			if (nParam == 1)
				break;

			int PartConvert[itempart_num] = 
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,		UIEP_MASK,
				UIEP_FIFONG
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; i++)
			{
				pInfo->Obj.uId = g_cViewItem.m_sItem[i].nIdx;
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
				
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
		
	case GDI_TRADE_OPER_DATA:
		if (uParam == UTOD_IS_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock;
		else if (uParam == UTOD_IS_TRADING)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState;
		else if (uParam == UTOD_IS_OTHER_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock;
		else if (uParam == UTOD_IS_WILLING)
		{
			nRet = (Player[CLIENT_PLAYER_INDEX].m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADEOPEN);
		}
		break;

	case GDI_LIVE_SKILL_BASE:
		if (uParam)
		{
			KUiPlayerLiveSkillBase* pInfo = (KUiPlayerLiveSkillBase*)uParam;
			//to do:no implements in this version;
			pInfo->nLiveExperience = 0 ;
			pInfo->nRemainPoint = 0 ;
			pInfo->nLiveExperienceFull = 0 ;
			memset(pInfo,0,sizeof(KUiPlayerLiveSkillBase));
		}
		break;

	case GDI_LIVE_SKILLS:
		if (uParam)
		{
			//to do:no implements in this version;
			KUiSkillData* pInfo = (KUiSkillData*)uParam;
			memset(pInfo,0,sizeof(KUiSkillData)*10);
		}
		break;

	case GDI_FIGHT_SKILL_POINT:
		//to do:no implements in this version;
		nRet = Player[CLIENT_PLAYER_INDEX].m_nSkillPoint;
		break;

	case GDI_FIGHT_SKILLS:
		if (uParam)
		{
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetSkillSortList(pSkills);
		}
		break;

	case GDI_LEFT_ENABLE_SKILLS:
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLeftSkillSortList(pSkills);
			return nCount;
		}
		break;

	case GDI_RIGHT_ENABLE_SKILLS:
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetRightSkillSortList(pSkills);
			return nCount;
		}
		break;

	case GDI_CUSTOM_SKILLS:
		if (uParam)
		{
			//to do: no implements in this version;
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			memset(pSkills,0,sizeof(KUiSkillData)*5);
		}
		break;

	case GDI_NEARBY_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayer((KUiPlayerItem*)uParam, nParam);
		break;

	case GDI_ITEM_EQUIP_ROOM_LIST:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cAuto.AllItemNameEquipRoom((FKUiObjectName*)uParam, nParam);
		break;

	case GDI_NEARBY_IDLE_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayerForTeamInvite((KUiPlayerItem*)uParam, nParam);
		break;

	case GDI_PLAYER_LEADERSHIP:
		if (uParam)
		{
			KUiPlayerLeaderShip* pInfo = (KUiPlayerLeaderShip*)uParam ;
			pInfo->nLeaderShipExperience = Player[CLIENT_PLAYER_INDEX].m_dwLeadExp ;		
			//to do: waiting for...;
			pInfo->nLeaderShipExperienceFull = Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp;
			pInfo->nLeaderShipLevel = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel ;			
		}
		break;

	case GDI_ITEM_IN_ENVIRO_PROP:
		{
			if (!nParam)
			{
				KUiGameObject *pObj = (KUiGameObject *)uParam;
				if (pObj->uGenre != CGOG_ITEM && pObj->uGenre != CGOG_NPCSELLITEM)
					break;

				KItem* pItem = NULL;

				if (pObj->uGenre == CGOG_ITEM && pObj->uId > 0 && pObj->uId < MAX_ITEM)
				{
					pItem = &Item[pObj->uId];
				}
				else if (pObj->uGenre == CGOG_NPCSELLITEM)
				{
					int nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop], pObj->uId);
					if(nIdx < 0) //#can kiem tra
						break;
					pItem = BuySell.GetItem(nIdx);
				}

				_ASSERT(pItem);
				if (!pItem || pItem->GetGenre() != item_equip)
					break;

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pItem))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
			else
			{
				KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
				if (pObj->Obj.uGenre != CGOG_ITEM || pObj->Obj.uId >= MAX_ITEM)
					break;

				int PartConvert[itempart_num] = 
				{ 
					itempart_head,		itempart_weapon,
					itempart_amulet,	itempart_cuff,
					itempart_body,		itempart_belt,
					itempart_ring1,		itempart_ring2,
					itempart_pendant,	itempart_foot,
					itempart_horse, itempart_mask,
				};

				_ASSERT(pObj->eContainer < itempart_num);
				if (pObj->eContainer >= itempart_num || pObj->eContainer < 0)
					break;

				if (Item[pObj->Obj.uId].GetGenre() != item_equip)
					break;

				int nPlace = PartConvert[pObj->eContainer];

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pObj->Obj.uId, nPlace))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
		}
		break;

	case GDI_IMMEDIATEITEM_NUM:
		if (uParam >= 0 && uParam < IMMEDIACY_ROOM_WIDTH)
			nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSameDetailItemNum(uParam);
		break;

	case GDI_TRADE_NPC_ITEM:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			int nPage = 0;
			int nIndex = 0;
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
			int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];

			if (nBuyIdx == -1)
				break;
			if (nBuyIdx >= BuySell.GetHeight())
				break;
			if (!BuySell.m_pShopRoom)
				break;
			BuySell.m_pShopRoom->Clear();
			for (int i = 0; i < BuySell.GetWidth(); i++)
			{
				nIndex = BuySell.GetItemIndex(nBuyIdx, i);
				KItem* pItem = BuySell.GetItem(nIndex);
				
				if (nIndex >= 0 && pItem)
				{
					// Set pInfo->Obj.uGenre
					pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
					// Set pInfo->Obj.uId
					pInfo->Obj.uId = i;

					POINT	Pos;
					if (BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
					{

						BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
					}
					else
					{
						nPage++;
						BuySell.m_pShopRoom->Clear();

						BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);
						BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
					}
					pInfo->Region.h = Pos.x;
					pInfo->Region.v = Pos.y;
					pInfo->Region.Width = pItem->GetWidth();
					pInfo->Region.Height = pItem->GetHeight();
					pInfo->nContainer = nPage;
					nCount++;
					pInfo++;
				}
			}			
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			int nIndex = 0;
			int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
			if (nBuyIdx == -1)
				break;
			if (nBuyIdx >= BuySell.GetHeight())
				break;
			for (int i = 0; i < BuySell.GetWidth(); i++)
			{
				nIndex = BuySell.GetItemIndex(nBuyIdx, i);
				KItem* pItem = BuySell.GetItem(nIndex);
				
				if (nIndex >= 0 && pItem)
				{
					nCount++;
				}
			}
			nRet = nCount;
		}
		break;
	case GDI_CHAT_SEND_CHANNEL_LIST:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.SendSelectChannelList((KUiChatChannel*)uParam, nParam);
		break;
	case GDI_CHAT_RECEIVE_CHANNEL_LIST:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.SendTakeChannelList((KUiChatChannel*)uParam, nParam);
		break;
	case GDI_CHAT_CURRENT_SEND_CHANNEL:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.GetCurChannel((KUiChatChannel*)uParam);
		break;
	case GDI_CHAT_GROUP_INFO:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.IGetTeamInfo(nParam, (KUiChatGroupInfo*)uParam);
		break;

	case GDI_CHAT_FRIENDS_IN_AGROUP:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.IGetTeamFriendInfo(nParam, (KUiPlayerItem*)uParam);
		break;
	case GDI_ITEM_IN_STORE_BOX:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX1:	// mo rong ruong 1
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_exbox1);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox1room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox1room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox1room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox1room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX2:	// mo rong ruong 2
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_exbox2);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox2room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox2room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox2room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox2room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX3:	// mo rong ruong 3
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_exbox3);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox3room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox3room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox3room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox3room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_EX:	// mo rong hanh trang
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipmentex);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproomex)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproomex)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproomex)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproomex)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_PK_SETTING:					
		nRet = Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState();
		break;
	case GDI_SHOW_PLAYERS_NAME:			
		nRet = NpcSet.CheckShowName();
		break;
	case GDI_SHOW_PLAYERS_LIFE:			
		nRet = NpcSet.CheckShowLife();
		break;
	case GDI_SHOW_PLAYERS_MANA:			
		nRet = NpcSet.CheckShowMana();
		break;
	case GDI_SHOW_OBJ_NAME:            //hien ten obj edit by phong kieu
        nRet = ObjSet.CheckShowName();
		break;
	case GDI_IS_CHEST_UNLOCKED:			//Kiem tra xem nguoi choi da khoa ruong lai hay chua
		nRet = Player[CLIENT_PLAYER_INDEX].m_CUnlocked;
		break;
	case GDI_IS_CHECK_IMAGE:			//Kiem tra hinh anh nguoi choi da chon
		nRet = Player[CLIENT_PLAYER_INDEX].m_ImagePlayer;
		break;
	case GDI_ITEM_CHAT:			//tao ra itemchat
		{
			if(uParam)
			{
				if(nParam)
				{	
					ChatItem* m_sInfo = (ChatItem*)nParam;
					//Create ItemSet
					if(m_sInfo->m_nID == 0)
						return 0;
					int nMagicParam[MAX_ITEM_MAGICLEVEL], nItemIdx;
					for (int j = 0; j < MAX_ITEM_MAGICLEVEL; j++)
						nMagicParam[j] = m_sInfo->m_btMagicLevel[j];
					//
					if (m_sInfo->m_nGoldId)
					{
							nItemIdx = ItemSet.AddGoldItem(
							m_sInfo->m_nGoldId,
							nMagicParam,
							m_sInfo->m_btSeries,
							m_sInfo->m_bEnChance
							);
					}
					else
					{
							nItemIdx = ItemSet.AddItemSet2(
							m_sInfo->m_btGenre,
							m_sInfo->m_btSeries,
							m_sInfo->m_btLevel,
							m_sInfo->m_btLuck,
							m_sInfo->m_btDetail,
							m_sInfo->m_btParticur,
							nMagicParam,
							m_sInfo->m_wVersion,
							m_sInfo->m_dwRandomSeed,
							m_sInfo->m_bStack,
							m_sInfo->m_bEnChance,
							m_sInfo->m_bPoint //®å tÝm sö dông ®iÓm nµy tèi ®a 6 dßng
							);						
					}
					if (nItemIdx > 0)
					{
						Item[nItemIdx].SetID(m_sInfo->m_nID);
						Item[nItemIdx].SetPrice(m_sInfo->m_uPrice);
						Item[nItemIdx].SetExpTime(m_sInfo->m_YearExp,0,0,0);
						Item[nItemIdx].SetPlayerItemLock(m_sInfo->m_Lock);
						Item[nItemIdx].SetPlayerItemHLock(m_sInfo->m_HLock);
						Item[nItemIdx].SetDurability(m_sInfo->m_nDurability);
						nRet = nItemIdx;
					}
				}
			}
			else
			{
				ItemSet.Remove(nParam);
			}
		}
		break;
	case GDI_ITEM_NAME:			//lay ten item
		if(uParam)
		{
			char* pzName = (char*)uParam;
			if(nParam)
				strcpy(pzName, Item[nParam].GetName());
		}
		break;
	case GDI_GET_ITEM_PARAM:			//lay thong so cua item
		if(uParam && nParam)
		{
			DWORD nIdx = nParam;
			ChatItem *pInfo = (ChatItem *)uParam;
			pInfo->m_uPrice = 0;
			pInfo->m_bX = 0;
			pInfo->m_bY = 0;
			pInfo->m_nIdx = 0;
			pInfo->m_nID			= Item[nIdx].GetID();
			pInfo->m_btGenre		= Item[nIdx].GetGenre();
			pInfo->m_btDetail		= Item[nIdx].GetDetailType();
			pInfo->m_btParticur		= Item[nIdx].GetParticular();
			pInfo->m_btSeries		= Item[nIdx].GetSeries();
			pInfo->m_btLevel		= Item[nIdx].GetLevel();
			pInfo->m_bStack			= Item[nIdx].GetStackNum();
			pInfo->m_bEnChance		= Item[nIdx].GetEnChance();
			pInfo->m_nGoldId		= Item[nIdx].GetGoldId();
			pInfo->m_btLuck			= Item[nIdx].m_GeneratorParam.nLuck;
			pInfo->m_dwRandomSeed	= Item[nIdx].m_GeneratorParam.uRandomSeed;
			pInfo->m_wVersion		= Item[nIdx].m_GeneratorParam.nVersion;
			pInfo->m_YearExp			= Item[nIdx].GetTime()->bYear;
			pInfo->m_Lock					= Item[nIdx].GetPlayerItemLock();
			pInfo->m_HLock					= Item[nIdx].GetPlayerItemHLock();
			for (int i = 0; i < MAX_ITEM_MAGICLEVEL; i++)
			{
				pInfo->m_btMagicLevel[i]		= Item[nIdx].m_GeneratorParam.nGeneratorLevel[i];
			}
			pInfo->m_bPoint = Item[nIdx].IsPurple();
			pInfo->m_nDurability = Item[nIdx].GetDurability();
			nRet = 1;
		}
		break;
	case GDI_CHAT_ITEM_DESC:
		if (nParam && uParam)
		{
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			Item[uParam].GetDesc(pszDescript);
		}
		break;
	case GDI_IS_TONG_MASTER:
		if (uParam)
		{
			char* pszPlayerName = (char *)uParam;
			Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongMasterName(pszPlayerName);
		}
		break;
	case GDI_MISSION_SELFDATA: //add by phong kiÒu Tèng Kim Battle
		if (uParam)
		{
			TMissionLadderSelfInfo *pRankData = (TMissionLadderSelfInfo *)uParam;
			*pRankData = Player[CLIENT_PLAYER_INDEX].m_MissionData;
		}
		break;	
	case GDI_MISSION_RANKDATA:
		if (uParam)
		{
			TMissionLadderInfo *pRankData = (TMissionLadderInfo *)uParam;
			*pRankData = Player[CLIENT_PLAYER_INDEX].m_MissionRank[nParam];
		}
		break;	
	case GDI_TREMBLE_ITEM: 
		nRet = 0;
		if (uParam)
		{
			if (nParam == 1)
				break;

			int PartTrembleConvert[tremblepart_num] = 
			{
				UIEP_BLUEITEM,	UIEP_GEMLEVEL,
				UIEP_GEMSPIRIT,	UIEP_GEMMETAL,
				UIEP_GEMWOOD,	UIEP_GEMWATER,
				UIEP_GEMFIRE,	UIEP_GEMEARTH,
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < tremblepart_num; i++)
			{
				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetTrembleItem(i);
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
				
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartTrembleConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
	case GDI_CHAT_ITEM_IMAGE:
		if (nParam && uParam)
		{
			char* pszImage = (char *)uParam;
			pszImage[0] = 0;
			strcpy(pszImage, Item[nParam].GetImageName());
			if(pszImage[0]) 
				nRet = 1;
		}
		break;
	case GDI_NPC_STATE_SKILL:
		nRet = 0;
		if (uParam)
		{
			KStateNode* pNode;
			pNode = (KStateNode *)Npc[CLIENT_PLAYER_INDEX].m_StateSkillList.GetTail();
			KStateTempNode* pInfo = (KStateTempNode *)uParam;
			
			while(pNode)
			{
				if (pNode->m_SkillID > 0 && pNode->m_SkillID < MAX_SKILL && pNode->m_Level > 0 && pNode->m_Level < MAX_SKILLLEVEL)
				{
					KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(pNode->m_SkillID, pNode->m_Level);
					if (pOrdinSkill)
					{
						if(pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles || pOrdinSkill->GetSkillStyle() == SKILL_SS_InitiativeNpcState)
						{
							pInfo->nSkillId = pNode->m_SkillID;
							pInfo->nLeftTime = pNode->m_LeftTime;
							pInfo++;
						}
					}
				}
				pNode = (KStateNode *)pNode->GetPrev();
			}	
		}
		else
		{
			int nCount = 0;
			KStateNode* pNode;
			pNode = (KStateNode *)Npc[CLIENT_PLAYER_INDEX].m_StateSkillList.GetTail();
			while(pNode)
			{
				if (pNode->m_SkillID > 0 && pNode->m_SkillID < MAX_SKILL && pNode->m_Level > 0 && pNode->m_Level < MAX_SKILLLEVEL)
				{
					KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(pNode->m_SkillID, pNode->m_Level);
					if (pOrdinSkill)
					{
						if(pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles || pOrdinSkill->GetSkillStyle() == SKILL_SS_InitiativeNpcState)
						{
							nCount++;
						}
					}
				}
				pNode = (KStateNode *)pNode->GetPrev();
			}
			nRet = nCount;
		}
		break;	
	case GDI_EXBOX_ID: 				// truyen id mo rong ruong
		{	
			int nExBoxId = 0;
			if (nExBoxId < 0)
				return 0;
			nExBoxId = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
				return nExBoxId;
			break;
		}
	}

	return nRet;
}

int	KCoreShell::OperationRequest(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 1;
	switch(uOper)
	{
	case GOI_CP_UNLOCK:						//open ruong
		SendClientCPUnlockCmd(uParam);
		break;
	case GOI_CP_LOCK:						//khoa ruong
		SendClientCPLockCmd();
		break;
	case GOI_CP_CHANGE:						//doi mk ruong
		SendClientCPChangeCmd(uParam, nParam);
		break;
	case GOI_CP_RESET:						//reset mk ruong
		SendClientCPResetCmd(nParam);
		break;
	case GOI_CP_SET_IMAGE_PLAYER:
		SendClientCPSetImageCmd(uParam);
		break;
	case GOI_TRADE_PLAYER_BUY:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			if (CGOG_PLAYERSELLITEM != pObject1->Obj.uGenre)
				break;			
			
			int nWidth, nHeight;
			ItemPos	Pos;
			
			nWidth = Item[pObject1->Obj.uId].GetWidth();
			nHeight = Item[pObject1->Obj.uId].GetHeight();
			if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
			{
				nRet = 0;
				break;
			}
			if (Pos.nPlace != pos_equiproom)
			{
				nRet = 0;
				
				KSystemMessage	sMsg;
				
				strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}
			
			if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < Item[pObject1->Obj.uId].GetSetPrice())
			{
				nRet = 0;
				KSystemMessage	sMsg;
				
				strcpy(sMsg.szMessage, MSG_SHOP_NO_MONEY);
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
				
			}
			
			nRet = 1;
			SendClientCmdPlayerBuy(g_cSellItem.FindIdx(pObject1->Obj.uId), nParam, pos_equiproom, Pos.nX, Pos.nY);
		}
		break;

	case GOI_VIEW_PLAYERSELLITEM_END:
		{
			g_cSellItem.DeleteAll();
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetMenuState(PLAYER_MENU_STATE_NORMAL);
		}
		break;
	case GOI_VIEW_PLAYERSELLITEM:
		if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan)
		{
			g_cSellItem.ApplyViewItem(uParam);
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetMenuState(PLAYER_MENU_STATE_TRADING);
		}
		break;
	case GDI_SET_TRADE_ITEM:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				if(Item[nIdx].GetPlayerItemLock() > 0 || Item[nIdx].GetPlayerItemHLock() >0 || Item[nIdx].GetPlayerItemLock() == -2)
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_TRADE_TASK_ITEM);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				Item[nIdx].SetPrice(nParam);
				SendClientCmdSetPrice(Item[nIdx].GetID(),nParam);
			}
		}
		break;
	case GDI_PLAYER_TRADE:
		{
			char* sShopName = (char *)uParam;
			
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, "§ang c­ëi ngùa kh«ng thÓ bµy b¸n!");
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return 0;
			}
			
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, "Kh«ng thÓ bµy b¸n ë khu vùc chiÕn ®Êu!");
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return 0;
			}
			
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan == 0)
			{
				int nCount = 0;//#check neu khong co item dinh gia thong bao
				PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
				if(pItem && pItem->nIdx && Item[pItem->nIdx].m_CommonAttrib.uPrice && pItem->nPlace == pos_equiproom)
				{
					nCount++;
				}
				while(pItem)
				{
					pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
					if(pItem && pItem->nIdx && Item[pItem->nIdx].m_CommonAttrib.uPrice && pItem->nPlace == pos_equiproom)
					{
						nCount++;
					}
				}
				if(nCount)
				{
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_sit);
						SendClientCmdSit(TRUE);
					}
					SendClientCmdStartTrade(1, sShopName);
				}
				else
				{
					//thong bao chua dinh gia vat pham
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, "Ch­a ®Þnh gi¸ vËt phÈm !!! ");
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
			}
			else
				SendClientCmdStartTrade(0, "");
		}
		break;
	case GOI_QUERY_RANK_INFORMATION:
		SendClientCmdQueryLadder(uParam);
		break;
	//uParam = (const char*)pszFileName
	case GOI_PLAY_SOUND:
		if (uParam)
		{
			static KCacheNode* pSndNode = NULL;
			KWavSound* pSound = NULL;
			pSndNode	= (KCacheNode*)g_SoundCache.GetNode((char *)uParam, (KCacheNode * )pSndNode);
			pSound		= (KWavSound*)pSndNode->m_lpData;
			if (pSound)
			{
				if (pSound->IsPlaying())
					break;
				pSound->Play(0, -10000 + Option.GetSndVolume() * 100, 0);
			}
		}
		break;
	case GOI_PLAYER_RENASCENCE:
		{
			int nReviveType;
			if (nParam)	// bBackTown
			{
				nReviveType = REMOTE_REVIVE_TYPE;
			}
			else
			{
				nReviveType = LOCAL_REVIVE_TYPE;
			}
			SendClientCmdRevive();
		}
		break;
	case GOI_MONEY_INOUT_STORE_BOX:
		{
			BOOL	bIn = (BOOL)uParam;
			int		nMoney = nParam;
			int		nSrcRoom, nDesRoom;


			if (bIn)
			{
				nSrcRoom = room_equipment;
				nDesRoom = room_repository;
			}
			else
			{
				nDesRoom = room_equipment;
				nSrcRoom = room_repository;
			}
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(nSrcRoom, nDesRoom, nMoney);
		}
		break;
	case GOI_LIXIAN:
		SendClientCmdLiXian();
		break;
	case GOI_EXIT_GAME:
		g_SubWorldSet.Close();
		g_ScenePlace.ClosePlace();
		break;
	case GOI_GAMESPACE_DISCONNECTED:
		g_SubWorldSet.Close();
		break;
	case GOI_TRADE_NPC_BUY:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			if (pObject1->Obj.uGenre != CGOG_NPCSELLITEM)
				break;
			//
			if (nParam <= 0)
				break;
			//
			SendClientCmdBuy(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop, pObject1->Obj.uId, nParam, 0);
		}
		break;
	case GOI_SUPPERSHOP_TRADE_NPC_BUY:
		{
			FKGioHang* pGioHang = (FKGioHang*)uParam;

			if (nParam <= 0)
				break;
			
			for(int i = 0; i < MAX_ITEM_SPC; i ++)
			{
				if(pGioHang->m_ListCount[i] > 0)
				{
					SendClientCmdBuy(pGioHang->m_ListShopId[i], pGioHang->m_ListItemInfo[i].Obj.uId, pGioHang->m_ListCount[i], 1);
				}
			}
		}
		break;
	case GOI_TRADE_NPC_SELL:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			//
			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				if (//Item[nIdx].GetGenre() == item_task || //Vat pham task event Item khong giao dich
					Item[nIdx].GetPlayerItemLock() > 0 
					|| Item[nIdx].GetPlayerItemHLock() > 0 
					|| Item[nIdx].GetPlayerItemLock() == -2)
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_TRADE_TASK_ITEM);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				SendClientCmdSell(Item[nIdx].GetID());
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_ADD_UI_CMD_SCRIPT:
		if(uParam && nParam)
		{
			SendUiCmdScript(uParam, (char*)nParam);
		}
		break;
	case GDI_THROW_ALL_ITEM:
		if(uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			Player[CLIENT_PLAYER_INDEX].ThrowAllItem(pObj->Obj.uId, pObj->Obj.uGenre, 0);
		}
		break;
	case GOI_RECOVER_ITEM:
		if(uParam)
		{
			BOOL bExistId = FALSE;
			switch(uParam)
			{	
				int i;
				/*case pos_give:
					if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetGiveItem())
					{
						bExistId = TRUE;
					}
					break;*/
				case pos_tremble:
					{
						for(i = 0; i < tremblepart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetTrembleItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				/*case pos_compone:
					{
						for(i = 0; i < compoundpart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetCompOneItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				case pos_comptwo:
					{
						for(i = 0; i < compoundpart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetCompTwoItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				case pos_compthree:
					{
						for(i = 0; i < compoundpart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetCompThreeItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				case pos_distill:
					{
						for(i = 0; i < outinpart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetDistillItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				case pos_forge:
					{
						for(i = 0; i < forgepart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetForgeItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				case pos_enchase:
					{
						for(i = 0; i < outinpart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEnchaseItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;*/
				default:
					break;
			}
			if(!bExistId)
			{
				break;
			}
			Player[CLIENT_PLAYER_INDEX].m_ItemList.RecoverItem(uParam);
			SendClientCmdRecoverItem(uParam);
		}
		break;
	case GOI_GET_CITY_OWN_TONG:
		SendClientCmdGetCityOwnTong();
		break;
	case GOI_RECOVERY_BOX_COMMAND:
		if (uParam)
		{
			DWORD dwID;
			int nW, nH;		
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == uParam)
			{
				dwID = Item[pItem->nIdx].GetID();
				nW = Item[pItem->nIdx].GetWidth();
				nH = Item[pItem->nIdx].GetHeight();
				
				SendClientRecoveryBox(dwID, nW, nH);
			}
			
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == uParam)
				{
					dwID = Item[pItem->nIdx].GetID();
					nW = Item[pItem->nIdx].GetWidth();
					nH = Item[pItem->nIdx].GetHeight();
									
					SendClientRecoveryBox(dwID, nW, nH);	
				}
			}
		}	
		break;
	case GOI_INPUT_INFO:
		{
			KUiInPutBoxCmd* pInPutCmd = (KUiInPutBoxCmd*)nParam;
			SendClientCmdInputBox(uParam,pInPutCmd->nNum,(char*)pInPutCmd->szAction,(char*)pInPutCmd->nValue);
		}
		break;
	case GOI_NPC_ITEM_BREAK:																		//tach vat pham xep chong
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//
			KUiBreakItemOption* pBreakOption = (KUiBreakItemOption*)nParam;
			//
			int nIdx = pObject1->Obj.uId;
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				
				if (Item[nIdx].GetStackNum() > pBreakOption->num)
				{
					//Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - pBreakOption->num);
					SendClientCmdBreak(Item[nIdx].GetID(), pBreakOption->num, pBreakOption->isbreakall);
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_BREAK_ITEM_NOT);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_TRADE_NPC_REPAIR:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{		
				if (Item[nIdx].GetGenre() != item_equip) //vat pham khong the sua
				{
					return 0;
				}
				else if (Item[nIdx].GetDurability() == -1 || Item[nIdx].GetMaxDurability() == -1 || Item[nIdx].GetDurability() == Item[nIdx].GetMaxDurability()) //#chua bi giam sut do ben
				{
					return 0;
				}
				else if(Item[nIdx].GetDurability() == 0)//#do ben trang bi bang 0 trang bi hong khong sua bang tien van
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_NO_REPAIR_MONEY);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				else if (Item[nIdx].GetRepairPrice() <= Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney())
				{
					SendClientCmdRepair(Item[nIdx].GetID());
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_SHOP_NO_MONEY);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_SWITCH_OBJECT:
		{
			ItemPos	P1, P2;
			int PartConvert[itempart_num] = 
			{ 
				itempart_head,		itempart_weapon,
				itempart_amulet,	itempart_cuff,
				itempart_body,		itempart_belt,
				itempart_ring1,		itempart_ring2,
				itempart_pendant,	itempart_foot,
				itempart_horse, itempart_mask,
			};
			int PartTrembleConvert[tremblepart_num] = 
			{
				tremblepart_item,	tremblepart_gemlevel,
				tremblepart_gemspirit,	tremblepart_gemmetal,
				tremblepart_gemwood,	tremblepart_gemwater,
				tremblepart_gemfire,	tremblepart_gemearth,
			};
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			KUiObjAtContRegion* pObject2 = (KUiObjAtContRegion*)nParam;
			
			if (!pObject1 && !pObject2)
				break;
			
			if (pObject1)
			{
				switch(pObject1->eContainer)
				{
				case UOC_STORE_BOX:
					P1.nPlace = pos_repositoryroom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_EX_BOX1:	// mo rong ruong 1
					P1.nPlace = pos_exbox1room;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_EX_BOX2:	// mo rong ruong 2
					P1.nPlace = pos_exbox2room;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_EX_BOX3:	// mo rong ruong 3
					P1.nPlace = pos_exbox3room;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_ITEM_EX:	// mo rong hanh trang 
					P1.nPlace = pos_equiproomex;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_IMMEDIA_ITEM:
					P1.nPlace = pos_immediacy;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_IMMEDIA_SKILL:
					P1.nPlace = pos_immediacyskill;
					P1.nX = pObject1->Obj.uGenre;
					P1.nY = pObject1->Obj.uId;
					break;
				case UOC_SKILL_TAKE_WITH:
					P1.nPlace = pos_skilltakewith;
					P1.nX = pObject1->Obj.uGenre;
					P1.nY =pObject1->Obj.uId;
					break;
				case UOC_ITEM_TAKE_WITH:
					P1.nPlace = pos_equiproom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_EQUIPTMENT:
					{
						// TODO
						if (pObject1->Region.h == 1)
							break;
						P1.nPlace = pos_equip;
						P1.nX = PartConvert[pObject1->Region.v];
					}
					break;
				case UOC_TO_BE_TRADE:
					P1.nPlace = pos_traderoom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_AFFAIR_ITEM: // pos tra vat pham nhiem vu
					P1.nPlace = pos_affairitem;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_TREMBLE_ITEM: 
					{
						if (pObject1->Region.h == 1)
							break;
						P1.nPlace = pos_tremble;
						P1.nX = PartTrembleConvert[pObject1->Region.v];
					}
					break;
				case UOC_NPC_SHOP:
					if (CGOG_NPCSELLITEM != pObject1->Obj.uGenre)
						break;

					int nIdx = 0;
					KItem* pItem = NULL;
					
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop, pObject1->Obj.uId);
					pItem = BuySell.GetItem(nIdx);
					
					int nWidth, nHeight;
					ItemPos	Pos;
					
					nWidth = pItem->GetWidth();
					nHeight = pItem->GetHeight();
					if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Pos.nPlace != pos_equiproom)
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < pItem->GetPrice())
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_MONEY);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					
					SendClientCmdBuy(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop, pObject1->Obj.uId, nParam, 0);
					break;
				}
			}
			
			if (pObject2)
			{
				switch(pObject2->eContainer)
				{
				case UOC_STORE_BOX:
					P2.nPlace = pos_repositoryroom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_EX_BOX1:	// mo rong ruong 1
					P2.nPlace = pos_exbox1room;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_EX_BOX2:	// mo rong ruong 2
					P2.nPlace = pos_exbox2room;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_EX_BOX3:	// mo rong ruong 3
					P2.nPlace = pos_exbox3room;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_ITEM_EX:	// mo rong hanh trang
					P2.nPlace = pos_equiproomex;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_IMMEDIA_ITEM:
					P2.nPlace = pos_immediacy;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_IMMEDIA_SKILL:
					P2.nPlace = pos_immediacyskill;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_SKILL_TAKE_WITH:
					P2.nPlace = pos_skilltakewith;
					P2.nX = pObject1->Obj.uGenre;
					P2.nY =pObject1->Obj.uId;
					break;
				case UOC_ITEM_TAKE_WITH:
					P2.nPlace = pos_equiproom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_EQUIPTMENT:
					{
						if (pObject2->Region.h == 1)
							break;
						P2.nPlace = pos_equip;
						P2.nX = PartConvert[pObject2->Region.v];
					}
					break;
				case UOC_TO_BE_TRADE:
					P2.nPlace = pos_traderoom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_AFFAIR_ITEM: // pos tra vat pham nhiem vu
					P2.nPlace = pos_affairitem;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_TREMBLE_ITEM: 
					{
						if (pObject2->Region.h == 1)
							break;
						P2.nPlace = pos_tremble;
						P2.nX = PartTrembleConvert[pObject2->Region.v];
					}
					break;
				case UOC_NPC_SHOP:
					break;
				}
			}
			if (!pObject1)
			{
				memcpy(&P1, &P2, sizeof(P1));
			}
			if (!pObject2)
			{
				memcpy(&P2, &P1, sizeof(P1));
			}
			Player[CLIENT_PLAYER_INDEX].MoveItem(P1, P2);
		}
		break;

	case GOI_INFORMATION_CONFIRM_NOTIFY:
	{
		PLAYER_SELECTUI_COMMAND command;
		command.nSelectIndex = 0;
		Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_TALKDIALOG);
		break;
	}
	
	case GOI_QUESTION_CHOOSE:
		if (g_bUISelLastSelCount == 0 )
			break;	
		{
			PLAYER_SELECTUI_COMMAND command;
			command.nSelectIndex = nParam;
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_SELECTDIALOG);
		}
		break;

	case GOI_SAY_NEW: //Say New
		if (g_bUISelLastSelCount == 0 )
			break;	
		{
			PLAYER_SELECTUI_COMMAND command;
			command.nSelectIndex = nParam;
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_SAYNEW);
		}
		break;

	case GOI_LOCK_PLAYER_ITEM:
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			if(pInfo->Obj.uId)
			{
				int lock = nParam;
				SendClientLockPlayerItemCmd(pInfo->Obj.uId, lock);
			}
		}
		break;
	case GOI_RCLICK_MOVE_ITEM:
		if(uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			int nPlace = nParam;
			ItemPos	Pos;
			switch(nPlace)
			{
			case UOC_ITEM_TAKE_WITH:
				Pos.nPlace = pos_equiproom;
				break;
			default:
				Pos.nPlace = -1;
				break;
			}

			Pos.nX = pInfo->Region.h;
			Pos.nY = pInfo->Region.v;

			if ((pInfo->Obj.uGenre == CGOG_ITEM || pInfo->Obj.uGenre == CGOG_IME_ITEM) && pInfo->Obj.uId > 0 && Pos.nPlace != -1)
			{
				Player[CLIENT_PLAYER_INDEX].ApplyAutoMoveItem(pInfo->Obj.uId, Pos);					
			}
		}
		break;
	case GOI_USE_ITEM:
		//to do: waiting for...
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			int nPlace = nParam;
			ItemPos	Pos;
			switch(nPlace)
			{
			case UOC_ITEM_TAKE_WITH:
				Pos.nPlace = pos_equiproom;
				break;
			case UOC_IMMEDIA_ITEM:
				Pos.nPlace = pos_immediacy;
				break;
			default:
				Pos.nPlace = -1;
				break;
			}

			Pos.nX = pInfo->Region.h;
			Pos.nY = pInfo->Region.v;

			if ((pInfo->Obj.uGenre == CGOG_ITEM || pInfo->Obj.uGenre == CGOG_IME_ITEM) && pInfo->Obj.uId > 0 && Pos.nPlace != -1)
			{
				if(Pos.nPlace == pos_immediacy && Item[pInfo->Obj.uId].IsFkItemSkill())
					CoreDataChanged(GDCNI_USE_SHORCUT_SKILL, Item[pInfo->Obj.uId].GetParticular(), 0);
				else
					Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pInfo->Obj.uId, Pos);
			}
		}
		break;

	case GOI_WEAR_EQUIP:
		//to do: waiting for...
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
		}
		break;

	case GOI_USE_SKILL:	
		if (uParam)
		{
			//to do:wating for...
			KUiGameObject* pInfo = (KUiGameObject*)uParam;
		}
		break;

	case GOI_SET_IMMDIA_SKILL:
		if (uParam)
		{
			KUiGameObject* pSkill = (KUiGameObject*)uParam;
			if (nParam == 0)
				//to do : modify;
			{
				if ( (int)pSkill->uId > 0 )
					Player[CLIENT_PLAYER_INDEX].SetLeftSkill((int)pSkill->uId);
				else if ((int)(pSkill->uId) == -1) 
				{
					int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
					int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
					
					if (nDetailType == 0)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nMeleeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == 1)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nRangeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == -1)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nHandSkill);
					}
					
				}
			}
			else if (nParam == 1)
			{
				if ((int)pSkill->uId > 0)
					Player[CLIENT_PLAYER_INDEX].SetRightSkill((int)pSkill->uId);
				else if ((int)(pSkill->uId) == -1)
				{
					int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
					int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
					
					if (nDetailType == 0)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nMeleeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == 1)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nRangeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == -1)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nHandSkill);
					}
				}
			}
		}
		break;

	case GOI_TONE_UP_SKILL:
		Player[CLIENT_PLAYER_INDEX].ApplyAddSkillLevel((int)nParam, 1);
		break;

	case GOI_TONE_UP_ATTRIBUTE:
		int numpoint2;
		numpoint2 = nParam;
		if(numpoint2 <= 0 || numpoint2 >= 256)
			break;
		switch (uParam)
		{
		case UIPA_STRENGTH:		
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(0, numpoint2);//Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(0, 1);
			break;
		case UIPA_DEXTERITY:	
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(1, numpoint2);//Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(1, 1);
			break;		
		case UIPA_VITALITY:		
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(2, numpoint2);//Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(2, 1);
			break;
		case UIPA_ENERGY:		
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(3, numpoint2);//Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(3, 1);
			break;		
		}
		break;

	case GOI_TRADE_INVITE_RESPONSE:
		if (uParam)
		{
			KTrade::ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
		}
		break;

	case GOI_TRADE_DESIRE_ITEM:
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			if (pInfo->Obj.uGenre != CGOG_MONEY)
				break;
			Player[CLIENT_PLAYER_INDEX].TradeMoveMoney(pInfo->Obj.uId);
		}
		break;

	case GOI_TRADE_WILLING: //giao dich giao dÞch
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyClose();
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyOpen((char*)uParam, nParam);
		}
		break;

	case GOI_TRADE_LOCK:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock)
			Player[CLIENT_PLAYER_INDEX].TradeApplyLock(0);
		else
			Player[CLIENT_PLAYER_INDEX].TradeApplyLock(1);
		break;

	case GOI_TRADE:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock != 1 || Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock != 1)
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState == 0)
		{
			Player[CLIENT_PLAYER_INDEX].TradeDecision(1);		
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].TradeDecision(2);		
		}
		break;

	case GOI_TRADE_CANCEL:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		Player[CLIENT_PLAYER_INDEX].TradeDecision(0);		
		break;

	case GOI_DROP_ITEM_QUERY:
		//to do : waiting for...
		break;

	case GOI_SEND_MSG:
		if (uParam && nParam)
			Player[CLIENT_PLAYER_INDEX].SendChat((KUiMsgParam*)nParam, (char*)uParam);
		break;
	case GOI_SET_SEND_CHAT_CHANNEL:	
		if (uParam)
		{
			KUiChatChannel* pChannelInfo = (KUiChatChannel*)uParam;// pChannelInfo
			Player[CLIENT_PLAYER_INDEX].m_cChat.SetCurChannel(pChannelInfo->nChannelNo, pChannelInfo->uChannelId, pChannelInfo->nChannelIndex);
		}
		break;

	case GOI_SET_SEND_WHISPER_CHANNEL:
		if (uParam)
		{
			if (((KUiPlayerItem*)uParam)->nIndex)
			{
				int	nIdx = NpcSet.SearchName(((KUiPlayerItem*)uParam)->Name);
				if (nIdx > 0)
				{
					Player[CLIENT_PLAYER_INDEX].m_cChat.SetCurChannel(CHAT_CUR_CHANNEL_SCREENSINGLE, Npc[nIdx].m_dwID, 0);
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_NOT_NEAR, ((KUiPlayerItem*)uParam)->Name);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
			}
			else
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_NOT_ONLINE, ((KUiPlayerItem*)uParam)->Name);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
		break;

	case GOI_SET_RECEIVE_CHAT_CHANNEL:
		if (uParam)
		{
			KUiChatChannel* pChannelInfo = (KUiChatChannel*)uParam;// pChannelInfo
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].m_cChat.AddChannel(pChannelInfo->nChannelNo);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cChat.SubChannel(pChannelInfo->nChannelNo);
			}
		}
		break;

	case GOI_CHAT_GROUP_NEW:
		Player[CLIENT_PLAYER_INDEX].m_cChat.AddTeam(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, (char*)uParam);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	case GOI_CHAT_GROUP_RENAME:
		Player[CLIENT_PLAYER_INDEX].m_cChat.RenameTeam(nParam, (char*)uParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	case GOI_CHAT_GROUP_DELETE:
		Player[CLIENT_PLAYER_INDEX].m_cChat.DeleteTeam(nParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	case GOI_CHAT_FRIEND_ADD:
		{
			if (g_pClient)
			{
				size_t pckgsize = sizeof(tagExtendProtoHeader) + sizeof(ASK_ADDFRIEND_CMD);

				tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
				pExHeader->ProtocolType = c2s_extendfriend;
				pExHeader->wLength = pckgsize - 1;

				ASK_ADDFRIEND_CMD* pAafCmd = (ASK_ADDFRIEND_CMD*)(pExHeader + 1);
				pAafCmd->ProtocolFamily = pf_friend;
				pAafCmd->ProtocolID = friend_c2c_askaddfriend;
				pAafCmd->pckgid = -1;
				strncpy(pAafCmd->dstrole, ((KUiPlayerItem*)uParam)->Name, _NAME_LEN);
				g_pClient->SendPackToServer(pExHeader, pckgsize);
		
				//Player[CLIENT_PLAYER_INDEX].m_cChat.ApplyAddFriend(((KUiPlayerItem*)uParam)->uId);

				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_APPLY_ADD_FRIEND, ((KUiPlayerItem*)uParam)->Name);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
		break;

	case GOI_CHAT_FRIEND_DELETE:
		{
			Player[CLIENT_PLAYER_INDEX].m_cChat.ApplyDeleteFriend(((KUiPlayerItem*)uParam)->uId, nParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_DELETE_FRIEND, ((KUiPlayerItem*)uParam)->Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;

	case GOI_CHAT_FRIEND_MOVE:
		{
			KUiPlayerItem	*pPlayer = (KUiPlayerItem*)uParam;
			int		nSrcTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.GetTeamNo(pPlayer->uId);
			if (nSrcTeamNo == -1)
			{
				break;
			}
			if (Player[CLIENT_PLAYER_INDEX].m_cChat.OneFriendChangeTeam(pPlayer->uId, nSrcTeamNo, nParam))
			{	
				CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
			}
			else
			{

			}
		}
		break;

	case GOI_CHAT_WORDS_TO_FRIEND:
		{
			char	szSentence[MAX_SENTENCE_LENGTH];
			KUiChatMessage	*pMsg = (KUiChatMessage*)uParam;
			KUiMsgParam	sMsg;

			sMsg.eGenre = MSG_G_CHAT;
			sMsg.szName[0] = 0;
			memset(sMsg.cChatPrefix, 0, sizeof(sMsg.cChatPrefix));
			sMsg.cChatPrefixLen = sizeof(unsigned int);
			sMsg.cChatPrefix[0] = ((char*)(&pMsg->uColor))[3];
			sMsg.cChatPrefix[1] = ((char*)(&pMsg->uColor))[2];
			sMsg.cChatPrefix[2] = ((char*)(&pMsg->uColor))[1];
			sMsg.cChatPrefix[3] = ((char*)(&pMsg->uColor))[0];

			if (pMsg->nContentLen >= MAX_SENTENCE_LENGTH)
			{
				memcpy(szSentence, pMsg->szContent, MAX_SENTENCE_LENGTH - 1);
				szSentence[MAX_SENTENCE_LENGTH - 1] = 0;
				sMsg.nMsgLength = MAX_SENTENCE_LENGTH - 1;
			}
			else
			{
				memcpy(szSentence, pMsg->szContent, pMsg->nContentLen);
				szSentence[pMsg->nContentLen] = 0;
				sMsg.nMsgLength = pMsg->nContentLen;
			}

			Player[CLIENT_PLAYER_INDEX].m_cChat.QQSendSentence(
				((KUiPlayerItem*)nParam)->uId,
				((KUiPlayerItem*)nParam)->nIndex, &sMsg, szSentence);
		}
		break;

	case GOI_CHAT_FRIEND_INVITE:
		if (uParam)
		{
			if (g_pClient)
			{
				size_t pckgsize = sizeof(tagExtendProtoHeader) + sizeof(REP_ADDFRIEND_CMD);

				tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
				pExHeader->ProtocolType = c2s_extendfriend;
				pExHeader->wLength = pckgsize - 1;
				
				REP_ADDFRIEND_CMD* pRafCmd = (REP_ADDFRIEND_CMD*)(pExHeader + 1);
				pRafCmd->ProtocolFamily = pf_friend;
				pRafCmd->ProtocolID = friend_c2c_repaddfriend;
				pRafCmd->pckgid = -1;
				strncpy(pRafCmd->dstrole, ((KUiPlayerItem*)uParam)->Name, _NAME_LEN);
				pRafCmd->answer = nParam ? answerAgree : answerDisagree;
				g_pClient->SendPackToServer(pExHeader, pckgsize);

			}
		}
		break;

	case GOI_OPTION_SETTING:			
		if (uParam == OPTION_DYNALIGHT)
		{
			g_ScenePlace.EnableDynamicLights(nParam != 0);
			if (g_pRepresent)
				g_pRepresent->SetOption(DYNAMICLIGHT, nParam != 0);
		}
		else if (uParam == OPTION_PERSPECTIVE)
		{
			if (g_pRepresent)
				g_pRepresent->SetOption(PERSPECTIVE, nParam != 0);
		}
		else if (uParam == OPTION_MUSIC_VALUE)
			Option.SetMusicVolume(nParam);
		else if (uParam == OPTION_SOUND_VALUE)
			Option.SetSndVolume(nParam);
		else if (uParam == OPTION_BRIGHTNESS)
			Option.SetGamma(nParam);
		else if (uParam == OPTION_WEATHER)
			g_ScenePlace.EnableWeather(nParam);

		else if(uParam == OPTION_QUALITY_GIAMPLAYER)//add by phong kiÒu
		{
			Option.SetLow(LowPlayer, nParam);
		}
		else if(uParam == OPTION_QUALITY_MATNPC)
		{
			Option.SetLow(LowNpc, nParam);
		}
		else if(uParam == OPTION_QUALITY_MATPLAYER)
		{
			Option.SetLow(LowEstPlayer, nParam);
		}
		else if(uParam == OPTION_QUALITY_GIAMSKILL)
		{
			Option.SetLow(LowMissle, nParam);
		}
		break;

	case GOI_VIEW_PLAYERITEM:
		{
			g_cViewItem.ApplyViewEquip(uParam);
		}
		break;
	case GOI_VIEW_PLAYERITEM_END:
		g_cViewItem.DeleteAll();
		break;
	case GOI_AUTO_PLAY: 
		{ 
			switch(uParam) 
			{ 
			case 0:
				Player[CLIENT_PLAYER_INDEX].m_bBuffSkill[0] = nParam;
				break;
			case 1:
				Player[CLIENT_PLAYER_INDEX].m_bBuffSkill[1] = nParam;
				break;
			case 2:
				Player[CLIENT_PLAYER_INDEX].m_bBuffSkill[2] = nParam;
				break;
			case 3:
				Player[CLIENT_PLAYER_INDEX].m_bFollowPeople = nParam;
				break;
			case 4:	
				strcpy(Player[CLIENT_PLAYER_INDEX].m_FollowPeopleName,(char*)nParam);
				Player[CLIENT_PLAYER_INDEX].m_FollowPeopleIdx = NpcSet.SearchName(Player[CLIENT_PLAYER_INDEX].m_FollowPeopleName);
				break;
			case 5:
				Player[CLIENT_PLAYER_INDEX].m_AutoAttack = nParam;
				break;
			case 6:
				Player[CLIENT_PLAYER_INDEX].m_bAttackAround = nParam;
				if(nParam)
				{
					Player[CLIENT_PLAYER_INDEX].SendInfoAuto();
				}
				break;
			case 7:
				Player[CLIENT_PLAYER_INDEX].m_RadiusAuto = nParam;
				break;
			case 8:
				Player[CLIENT_PLAYER_INDEX].m_btDurabilityItem = nParam;
				break;
			case 9:
				Player[CLIENT_PLAYER_INDEX].m_btDurabilityValue = nParam;
				break;
			case 10:
				Player[CLIENT_PLAYER_INDEX].m_AuraSkill[0] = nParam;
				break;
			case 11:
				Player[CLIENT_PLAYER_INDEX].m_AuraSkill[1] = nParam;
				break;
			case 12:
				Player[CLIENT_PLAYER_INDEX].m_bActiveSwitchAura = nParam;
				break;
			case 13:
				Player[CLIENT_PLAYER_INDEX].m_ArrayStateSkill[0] = nParam;
				break;
			case 14:
				Player[CLIENT_PLAYER_INDEX].m_AutoLifeReplenish = nParam; //auto buff
				break;
			case 15:	
				Player[CLIENT_PLAYER_INDEX].m_AutoLifeReplenishP = nParam; //buff theo phÇn tr¨m m¸u
				break;
			case 16:
				Player[CLIENT_PLAYER_INDEX].m_AutoMove = nParam;
				break;
			case 17:
				Player[CLIENT_PLAYER_INDEX].m_bPickItem = nParam;
				break;
			case 18:
				Player[CLIENT_PLAYER_INDEX].m_btPickUpKind = nParam;
				break;
			case 19:
				Player[CLIENT_PLAYER_INDEX].m_bFilterEquipment = nParam;
				break;
			case 20://gi÷ trang søc
				{
					Player[CLIENT_PLAYER_INDEX].m_SaveRAP = nParam;
					Player[CLIENT_PLAYER_INDEX].m_bSaveJewelry = nParam;
					break;
				}
			case 21:
				Player[CLIENT_PLAYER_INDEX].m_EatLife = nParam;
				break;
			case 22:
				Player[CLIENT_PLAYER_INDEX].m_EatMana = nParam;
				break;
			case 23:	
				Player[CLIENT_PLAYER_INDEX].m_LifeAutoV = nParam;
				break;
			case 24:	
				Player[CLIENT_PLAYER_INDEX].m_LifeTimeUse = nParam;
				break;
			case 25:	
				Player[CLIENT_PLAYER_INDEX].m_ManaAutoV = nParam;
				break;
			case 26:	
				Player[CLIENT_PLAYER_INDEX].m_ManaTimeUse = nParam;
				break;
			case 27:
				Player[CLIENT_PLAYER_INDEX].m_TPLife = nParam;
				break;
			case 28:
				Player[CLIENT_PLAYER_INDEX].m_TPMana = nParam;
				break;
			case 29:	
				Player[CLIENT_PLAYER_INDEX].m_TPLifeV = nParam;
				break;
			case 30:	
				Player[CLIENT_PLAYER_INDEX].m_TPManaV = nParam;
				break;
			case 31:
				Player[CLIENT_PLAYER_INDEX].m_TPNotMedicineBlood = nParam;
				break;
			case 32:
				Player[CLIENT_PLAYER_INDEX].m_TPNotMedicineMana = nParam;
				break;
			case 33:
				Player[CLIENT_PLAYER_INDEX].m_TPHightMoney = nParam;
				break;
			case 34:	
				Player[CLIENT_PLAYER_INDEX].m_TPHightMoneyV = nParam;
				break;
			case 35:
				Player[CLIENT_PLAYER_INDEX].m_AutoAntiPoison = nParam;
				break;
			case 36:
				Player[CLIENT_PLAYER_INDEX].m_AutoTTL = nParam;
				break;
			case 37:
				Player[CLIENT_PLAYER_INDEX].m_bActiveAutoParty = nParam;	//kÝch ho¹t tÝnh n¨ng tæ ®éi
				break;
			case 38:
				Player[CLIENT_PLAYER_INDEX].m_bAutoParty = nParam;	//tù ®éng tæ ®éi
				break;
			case 39:
				Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecpt = nParam;	//tù ®éng nhËn lêi mêi
				break;
			case 40:
				Player[CLIENT_PLAYER_INDEX].m_DistanceAuto = nParam;
				break;
			case 41:
				Player[CLIENT_PLAYER_INDEX].m_bFightDistance = nParam;
				break;
			case 42:
				Player[CLIENT_PLAYER_INDEX].m_ArrayStateSkill[1] = nParam;
				break;
			case 43:
				Player[CLIENT_PLAYER_INDEX].m_ArrayStateSkill[2] = nParam;
				break;
			case 44:
				Player[CLIENT_PLAYER_INDEX].m_bAttackPeople = !nParam;
				Player[CLIENT_PLAYER_INDEX].m_bAttackNpc = nParam;
				break;
			case 45:
				Player[CLIENT_PLAYER_INDEX].m_bAttackPeople = nParam;
				Player[CLIENT_PLAYER_INDEX].m_bAttackNpc = !nParam;
				break;
			case 46:
				Player[CLIENT_PLAYER_INDEX].m_bAttackPeople = nParam;
				Player[CLIENT_PLAYER_INDEX].m_bAttackNpc = nParam;
				break;
			case 47:
				if(nParam)
				{
					Player[CLIENT_PLAYER_INDEX].GetAutoQDXY((char *)nParam);
					break;
				}
			case eSortItem:
				{
					Player[CLIENT_PLAYER_INDEX].m_bSortEquipment_Active = nParam;//Set tu xep do
					break;
				}
			case eBuyItem:
				{
					Player[CLIENT_PLAYER_INDEX].m_bBuyEquip = nParam;//Set tu mua do
					break;
				}
			case eInventoryIM:
				{
					Player[CLIENT_PLAYER_INDEX].m_bInventoryMoney = nParam;//Set tu cat item money vao ruong
					Player[CLIENT_PLAYER_INDEX].m_bInventoryItem = nParam;
					break;
				}
			case eRepairEquip:
				{
					Player[CLIENT_PLAYER_INDEX].m_bRepairEquip = nParam;
					break;
				}
			case eReturnPortal:
				{
					Player[CLIENT_PLAYER_INDEX].m_bReturnPortal = nParam;
					break;
				}
			case eAutoTuiDuocPham:
				{
					Player[CLIENT_PLAYER_INDEX].m_Auto_TuiDuocPham = nParam;
					break;
				}
			case eABanItem:
				{
					Player[CLIENT_PLAYER_INDEX].m_Auto_BanItem = nParam;
					break;
				}
			/*case ePickInFightState://nhÆt ®å trong thµnh
				{
					Player[CLIENT_PLAYER_INDEX].m_Auto_PickInFightState = nParam;
					break;
				}*/
			case eAutoRightSkill: //sö dông chiªu bªn ph¶i
				{
					Player[CLIENT_PLAYER_INDEX].m_Auto_SkillRight = nParam;
					break;
				}
			case eAutoPTAll://pt tÊt c¶
				{
					Player[CLIENT_PLAYER_INDEX].m_bAutoAccecptAll = nParam;
					Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecptAll = nParam;
					break;
				}
			default:
				break; 
			} 
		} 
		break;
	case GOI_SUPERSHOP:
		{
			PLAYER_COMMAND	sSS;
			sSS.ProtocolType = c2s_playercommand;
			sSS.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_SUPERSHOP;
			sSS.m_lpBuf = 0;
			sSS.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			g_pClient->SendPackToServer((BYTE*)&sSS, sSS.m_wLength + 1);
		}
		break;
	case GOI_PLAYER_ACTIONCHAT:
		SendClientActionChatCmd((char *)uParam);
		break;
	case GOI_PLAYER_ACTION:
		{
			switch(uParam)
			{
			case PA_RUN:
				Player[CLIENT_PLAYER_INDEX].m_RunStatus = !Player[CLIENT_PLAYER_INDEX].m_RunStatus;
				break;
			case PA_RIDE:// len xuong ngua
				if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_horse) <= 0)
					break;
				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
				{
				   if(GetTickCount() - Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeHorse >= TIME_RIDE)
					{	
					   if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
					   {
						   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeHorse = GetTickCount();
						   SendClientCmdRide(FALSE);// len xuong ngua
					   }
					   else
					   {
						   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeHorse = GetTickCount();
						   SendClientCmdRide(TRUE);// len xuong ngua
					   }
					}
					else
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMT_NORMAL;
						Msg.eType = SMCT_NONE;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						sprintf(Msg.szMessage, "B¹n qu¸ mÖt mái, kh«ng thÓ tiÕp tôc lªn xuèng ngùa!");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					 }
				}
				else
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMT_NORMAL;
					Msg.eType = SMCT_NONE;
					Msg.byPriority = 1;
					Msg.byParamSize = 0;
					sprintf(Msg.szMessage, "B¹n ®ang ngåi thiÒn kh«ng thÓ lªn ngùa");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
				}
				break;
			case PW_NOT_SAME: // mat khau ko trung nhau
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "MËt khÈu x¸c thùc kh«ng gièng nhau");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case PW_NOT_LONG: // mat khau ko du do dai
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "MËt khÈu ph¶i ®ñ 6 ch÷ sè");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case PW_ACCEPTED: // mat khau dc chap nhan
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "MËt khÈu ®­îc chÊp nhËn");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case HT_CN:
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "Chøc n¨ng nµy ®ang ®­îc hoµn thiÖn");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;

			case CN_GH:
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "Ch­a më khãa r­¬ng. Kh«ng thÓ thùc hiÖn thao t¸c nµy !");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case EX_BOX:
				{	
					int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
					if(nExbox == 0)
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "B¹n ch­a më réng r­¬ng ®Õn gÆp ThÈm Cöu t¹i Ba L¨ng HuyÖn 188/198");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
					else 
						CoreDataChanged(GDCNI_OPEN_EX_BOX, NULL, NULL);
				}
				break;
			case EX_BOX2:
				{	
					int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
					if(nExbox == 1)
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "B¹n ch­a më réng r­¬ng 2 ®Õn gÆp ThÈm Cöu t¹i Ba L¨ng HuyÖn 188/198");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
					else if(nExbox == 1 || nExbox == 2 || nExbox == 3)
							CoreDataChanged(GDCNI_OPEN_EX_BOX2, NULL, NULL);
				}
				break;
			case EX_BOX3:
				{	
					int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
					if(nExbox == 2 || nExbox == 1)
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "B¹n ch­a më réng r­¬ng 3 ®Õn gÆp ThÈm Cöu t¹i Ba L¨ng HuyÖn 188/198");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
					else if(nExbox == 1 || nExbox == 2 || nExbox == 3)
							CoreDataChanged(GDCNI_OPEN_EX_BOX3, NULL, NULL);
				}
				break;
			case ITEMEX:
				{	
					int nItemEX = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId;
					if(nItemEX == 0)
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "B¹n ch­a mua më réng hµnh trang ë ThÈm Cöu");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
					else if(nItemEX >= 1)
							CoreDataChanged(GDCNI_OPEN_ITEMEX, NULL, NULL);
				}
				break;

			case PA_SIT:
				if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
				{
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_sit);
						SendClientCmdSit(TRUE);
					}
					else
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
						SendClientCmdSit(FALSE);
					}
				}
				else
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMCT_CLICK;
					Msg.eType = SMT_PLAYER;
					Msg.byPriority = 1;
					Msg.byParamSize = 0;
					strcpy(Msg.szMessage, "Trong lóc ®i ngùa kh«ng thÓ thùc hiÖn");//edit by phong kieu dang tren ngua khong the ngoi
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
				}
				break;
			}
		}
		break;
	case GOI_PK_SETTING:		
		{
			if(!Player[CLIENT_PLAYER_INDEX].m_cPK.GetLockPKState())
			{
				if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == enumPKNormal) //Neu dang o trong thanh
				{
					Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(nParam, FALSE);
				}
				else
				{
					if(nParam != enumPKNormal)
					{
						Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(nParam, FALSE);
					}
					else
					{
						if(GetTickCount() - Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeSWPK >= TIME_PK)
						{
							Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(nParam, FALSE);
							Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeSWPK = GetTickCount();
						}
						else
						{
							KSystemMessage	Msg;
							Msg.byConfirmType = SMT_NORMAL;
							Msg.eType = SMCT_NONE;
							Msg.byPriority = 1;
							Msg.byParamSize = 0;
							sprintf(Msg.szMessage, "PK luyÖn c«ng chuyÓn sang phi luyÖn c«ng mÊt 3 phót !");
							CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
						}
					}
				}
			}
			else
			{
				KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "Khu vùc nµy kh«ng cho phÐp ®æi PK!");
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
			}
		}
		break;

	case GOI_REVENGE_SOMEONE:
		if (uParam)
		{
			KUiPlayerItem	*pTarget = (KUiPlayerItem*)uParam;
			Player[CLIENT_PLAYER_INDEX].m_cPK.ApplyEnmityPK(pTarget->uId);
		}
		break;

	case GOI_FOLLOW_SOMEONE:
		if (uParam)
		{
			KUiPlayerItem	*pTarget = (KUiPlayerItem*)uParam;
			if (Npc[pTarget->nIndex].m_Kind == kind_player)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = pTarget->nIndex;
		}
		break;

	case GOI_SHOW_PLAYERS_NAME:
		NpcSet.SetShowNameFlag(nParam);
		break;

	case GOI_SHOW_PLAYERS_LIFE:
		NpcSet.SetShowLifeFlag(nParam);
		break;

	case GOI_SHOW_PLAYERS_MANA:
		NpcSet.SetShowManaFlag(nParam);
		break;

	case GOI_SHOW_OBJ_NAME:
        ObjSet.SetShowNameFlag(nParam); // hien ten obj duoi dat edit by phong kieu
        break;

	case GOI_DATAU:
		//SendClientDaTau(uParam);
		break;

	case GOI_DATAU1:
		//SendClientDaTau1(uParam);
		break;

	default:
		nRet = 0;
		break;
	}

	return nRet;
}

void KCoreShell::ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam)
{
	Player[CLIENT_PLAYER_INDEX].ProcessInputMsg(uMsg, uParam, nParam);
}

int KCoreShell::FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind)
{
	Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, nRelation);
	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();

	if (!bSelect)
		Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);

	if (nT > 0)
	{
		if (pReturn)
		{
			KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
			strncpy(p->Name, Npc[nT].Name, 32);
			p->nIndex = Npc[nT].m_Index;
			p->uId = Npc[nT].m_dwID;
			p->nData = Npc[nT].GetMenuState();
		}
		nKind = Npc[nT].m_Kind;
		return true;
	}
	return false;
}

int KCoreShell::FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind)
{
	Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetObj();
	
	if (!bSelect)
		Player[CLIENT_PLAYER_INDEX].SetTargetObj(0);

	if (nT > 0)
	{
		nObjectIdx = nT;
		nKind = Object[nT].m_nKind;
		return true;
	}
	return false;
}

int KCoreShell::FindSpecialNPC(char* Name, void* pReturn, int& nKind)
{
	if (Name == NULL || Name[0] == 0)
		return false;
	for (int nT = 0; nT < MAX_NPC; nT++)
	{
		if	(strcmp(Npc[nT].Name, Name) == 0)
		{
			if (pReturn)
			{
				KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
				strncpy(p->Name, Npc[nT].Name, 32);
				p->nIndex = Npc[nT].m_Index;
				p->uId = Npc[nT].m_dwID;
				p->nData = Npc[nT].GetMenuState();
			}
			nKind = Npc[nT].m_Kind;
			return true;
		}
	}
	return false;
}

int KCoreShell::ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC)
		{
			int nTalker = p->nIndex;
			if (Npc[nTalker].m_Kind == kind_player && Npc[nTalker].m_dwID == p->uId)
			{
				char * pszCheck1 = NULL;
				char * pszCheck2 = NULL;
				unsigned int uMsgLen1 = 0;
				unsigned int uMsgLen2 = 0;
				int nCount = 0;
				int i = 0;
				int j = 0;
				pszCheck1 = (char*)pMsgBuff; //bat dau tu ky tu dau tien cua msg
				BOOL bOk = FALSE;
				while(i < nMsgLength)
				{
					uMsgLen2 = 0;
					if(*pszCheck1 == '[') //khi tim thay ky tu "["
					{
						pszCheck2 = (char*)(pMsgBuff + i);
						nCount = 0;
						j = i;
						while(j < nMsgLength)
						{
							uMsgLen2++;
							if(*pszCheck2 == ']' && nCount == NUM_INFO_ITEM_CHAT) {bOk = TRUE; break;}
							//[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26] // doc tu tu se hieu thoi, cai nay de, phan gia tri nhat thi chi co cho nay
							if(*pszCheck2 == ',') nCount++;
							pszCheck2++;
							j++;
						}
					}
					pszCheck1++;
					if(bOk) {uMsgLen1 = i; break;}
					i++;
				}
				if(bOk)
				{
					return true;
				}

				if(nMsgLength >= 90)	//edit by phong kieu xu ly input lon hon 90 ky tu
				{
					Npc[nTalker].SetChatInfo(p->Name, "JX GiangHoKy", 15);
					return true;
				}
				Npc[nTalker].SetChatInfo(p->Name, pMsgBuff, nMsgLength);
				return true;
			}
		}
	}

	return false;
}

void KCoreShell::ApplyAddTeam(void* pPlayer)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			Player[CLIENT_PLAYER_INDEX].ApplyAddTeam(p->nIndex);
		}
	}
}

void KCoreShell::TradeApplyStart(void* pPlayer)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyStart(p->nIndex);
		}
	}
}

int KCoreShell::UseSkillCastB(int x, int y, int nSkillID, int nNpcIdx)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;

	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		g_DebugLog("[skill]Active");
		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		g_DebugLog("[skill]return");
		return 0;
	}

	if (Npc[nIndex].m_ActiveSkillID > 0)
	{
		ISkill * pISkill =  g_SkillManager.GetSkill(Npc[nIndex].m_ActiveSkillID, 1);
		if (!pISkill) 
            return 0;

		if (pISkill->IsAura())
			return 0;

		int nTargetIdx = nNpcIdx;
		if (pISkill->IsTargetOnly() && !nTargetIdx)
        {
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		if (nIndex == nTargetIdx)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		if ((!Npc[nIndex].m_SkillList.CanCast(Npc[nIndex].m_ActiveSkillID, SubWorld[Npc[nIndex].m_SubWorldIndex].m_dwCurrentTime))
			||
			(!Npc[nIndex].Cost(pISkill->GetSkillCostType(), pISkill->GetSkillCost(&Npc[nIndex]), TRUE))
			)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		if (!nTargetIdx)
		{
			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nX, nY);
			// Send to Server
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nX, nY);
		}
		else
		{
			if (pISkill->IsTargetOnly())
			{
				int distance = NpcSet.GetDistance(nIndex , nTargetIdx);
				if (distance > pISkill->GetAttackRadius())
				{
					Player[CLIENT_PLAYER_INDEX].SetTargetNpc(nTargetIdx);
					return 0;
				}
			}

			if (nIndex == nTargetIdx && pISkill->GetSkillStyle() == SKILL_SS_Missles) 
				return 0;
			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nTargetIdx);
			// Send to Server		
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nTargetIdx].m_dwID);
		}
	}
	Npc[nIndex].m_nPeopleIdx = 0;
	return 1;
}

int KCoreShell::UseSkill(int x, int y, int nSkillID)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		g_DebugLog("[skill]Active");
		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		g_DebugLog("[skill]return");
		return 0;
	}

	if (Npc[nIndex].m_ActiveSkillID > 0)
	{
		ISkill * pISkill =  g_SkillManager.GetSkill(Npc[nIndex].m_ActiveSkillID, 1);
		if (!pISkill) 
            return 0;

		if (pISkill->IsAura())
			return 0;

		int nAttackRange = pISkill->GetAttackRadius();

		int nTargetIdx = 0;
		
		if (pISkill->IsTargetAlly())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_ally);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}

		if (pISkill->IsTargetEnemy())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_enemy);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}

		if (pISkill->IsTargetObj())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetObj())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetObj();
			}
		}

		if (pISkill->IsTargetOnly() && !nTargetIdx)
        {
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		
		if (nIndex == nTargetIdx)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}

		if ((!Npc[nIndex].m_SkillList.CanCast(Npc[nIndex].m_ActiveSkillID, SubWorld[Npc[nIndex].m_SubWorldIndex].m_dwCurrentTime))
			||
			(!Npc[nIndex].Cost(pISkill->GetSkillCostType(), pISkill->GetSkillCost(&Npc[nIndex]), TRUE))
			)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}

		if (!nTargetIdx)
		{
			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nX, nY);
			// Send to Server
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nX, nY);
		}
		else
		{
			if (pISkill->IsTargetOnly())
			{
				int distance = NpcSet.GetDistance(nIndex , nTargetIdx);
				if (distance > pISkill->GetAttackRadius())
				{
					Player[CLIENT_PLAYER_INDEX].SetTargetNpc(nTargetIdx);
					return 0;
				}
			}

			if (nIndex == nTargetIdx && pISkill->GetSkillStyle() == SKILL_SS_Missles) 
				return 0;
			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nTargetIdx);
			// Send to Server		
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nTargetIdx].m_dwID);
		}
	}
	Npc[nIndex].m_nPeopleIdx = 0;
	return 1;
}

int KCoreShell::LockSomeoneUseSkill(int nTargetIndex, int nSkillID)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)
		return 0;

	if (Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;
		
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		g_DebugLog("[skill]Active");

		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		g_DebugLog("[skill]return");
		return 0;
	}

	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	if (nRelation == relation_enemy)
	{
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}

	return 0;
}

int KCoreShell::LockSomeoneAction(int nTargetIndex)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)
		return 0;
	if (nTargetIndex <= 0 || nTargetIndex >= MAX_NPC)	
	{
		Npc[nIndex].m_nPeopleIdx = 0;
		return 1;
	}

	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	if (nRelation != relation_enemy)
	{
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}

	return 0;
}

int KCoreShell::LockObjectAction(int nTargetIndex)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex <= 0)	
		Npc[nIndex].m_nObjectIdx = 0;
	else
		Npc[nIndex].m_nObjectIdx = nTargetIndex;

	return 1;
}

void KCoreShell::GotoWhere(int x, int y, int mode)
{
	if (mode < 0 || mode > 2)
		return;

	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		int bRun = false;

		if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
			mode == 2)
			bRun = true;

		int nX = x;
		int nY = y;
		int nZ = 0;
		g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
		int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

		if (!bRun)
		{
			Npc[nIndex].SendCommand(do_walk, nX, nY);
			// Send to Server
			SendClientCmdWalk(nX, nY);
		}
		else
		{
			Npc[nIndex].SendCommand(do_run, nX, nY);
			// Send to Server
			SendClientCmdRun(nX, nY);
		}
		Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
	}
}

void KCoreShell::Goto(int nDir, int mode)
{
	if (nDir < 0 || nDir > 63)
		return;

	if (mode < 0 || mode > 2)
		return;

	int bRun = false;

	if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
		mode == 2)
		bRun = true;

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	int nSpeed;
	if (bRun)
		nSpeed = Npc[nIndex].m_CurrentRunSpeed;
	else
		nSpeed = Npc[nIndex].m_CurrentWalkSpeed;

	Player[CLIENT_PLAYER_INDEX].Walk(nDir, nSpeed);

	Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
}

void KCoreShell::Turn(int nDir)
{
	if (nDir < 0 || nDir > 3)
		return;

	if (nDir == 0)
		Player[CLIENT_PLAYER_INDEX].TurnLeft();
	else if (nDir == 1)
		Player[CLIENT_PLAYER_INDEX].TurnRight();
	else
		Player[CLIENT_PLAYER_INDEX].TurnBack();
}

int KCoreShell::ThrowAwayItem()
{
	return Player[CLIENT_PLAYER_INDEX].ThrowAwayItem();
}

int KCoreShell::GetNPCRelation(int nIndex)
{
	return NpcSet.GetRelation(Player[CLIENT_PLAYER_INDEX].m_nIndex, nIndex);
}

void KCoreShell::DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam)
{
	if (g_pRepresent)
		CoreDrawGameObj(uObjGenre, uId, x, y, Width, Height, nParam);
}

#include "../../Represent/iRepresent/iRepresentshell.h"

void KCoreShell::DrawGameSpace()
{
	if (g_pRepresent)
	{
		g_ScenePlace.Paint();
		Player[CLIENT_PLAYER_INDEX].DrawSelectInfo();
	}
}

void KCoreShell::SetRepresentShell(struct iRepresentShell* pRepresent)
{
	g_pRepresent = pRepresent;
	g_ScenePlace.RepresentShellReset();
	if (g_pAdjustColorTab && g_ulAdjustColorCount && g_pRepresent)
		g_pRepresent->SetAdjustColorList(g_pAdjustColorTab, g_ulAdjustColorCount);
}

void KCoreShell::SetMusicInterface(void* pMusicInterface)
{
	g_pMusic = (KMusic*)pMusicInterface;
	Option.SetMusicVolume(Option.GetMusicVolume());
}

int KCoreShell::Breathe()
{
	g_SubWorldSet.MessageLoop();
	g_SubWorldSet.MainLoop();
	g_ScenePlace.Breathe();
	return true;
}

int KCoreShell::FindSkillInfo(int nType, int nIndex)
{
	if(nType == 0)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextSkillState(nIndex);
	else if(nType == 1)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextSkillFight(nIndex);
	else if(nType == 2)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextSkillAura(nIndex);
	else if(nType == 3)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextAllSkill(nIndex);
	return 0;
}

void KCoreShell::GetSkillName(int nSkillId, char* szSkillName)
{
	if (nSkillId)
	{
		ISkill * pISkill = g_SkillManager.GetSkill(nSkillId, 1);
		if(pISkill) //fix by phong kiÒu
			strcpy(szSkillName, pISkill->GetSkillName());
	}
}

BOOL KCoreShell::GetAutoFlag()
{
	return Player[CLIENT_PLAYER_INDEX].GetAutoFlag();
	return TRUE;
}

BOOL KCoreShell::GetFightFlag()
{
	return Player[CLIENT_PLAYER_INDEX].GetFightFlag();
}

void KCoreShell::SetMoveMap(int nType, int nPos, int nValue)
{
	switch(nType)
	{
		case GAUTO_AUTO_MOVEMPSID: //to¹ ®é train
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				//Player[CLIENT_PLAYER_INDEX].m_MoveMps[nPos][0] = nValue;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsTrain[nPos][0] = nValue;
			break;
		case GAUTO_AUTO_MOVEMPSX:
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				//Player[CLIENT_PLAYER_INDEX].m_MoveMps[nPos][1] = nValue;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsTrain[nPos][1] = nValue;
			break;
		case GAUTO_AUTO_MOVEMPSY:
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				//Player[CLIENT_PLAYER_INDEX].m_MoveMps[nPos][2] = nValue;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsTrain[nPos][2] = nValue;
			break;
		case GAUTO_AUTO_PT_PLAYERTEAM://qu¶n lý tæ ®éi
			{
				if(nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL &&  nValue)
				{
					char * nPlayerName = (char*)nValue;
					strcpy(Player[CLIENT_PLAYER_INDEX].m_AutoPT_PlayerList[nPos], nPlayerName);
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cTeam.m_AutoPT_PlayerList[nPos], nPlayerName);
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_AutoPT_PlayerList[nPos], nPlayerName);
				}
				break;
			}
		case GAUTO_AUTO_BLACK_ITEM:
			{
				if(nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL &&  nValue)
				{
					char * nItemName = (char*)nValue;
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_BlackItemList[nPos], nItemName);
				}
				break;
			}
		case GAUTO_AUTO_MOVETPSID: //to¹ ®é ra b·i
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsMap[nPos][0] = nValue;
			break;
		case GAUTO_AUTO_MOVETPSX:
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsMap[nPos][1] = nValue;
			break;
		case GAUTO_AUTO_MOVETPSY:
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsMap[nPos][2] = nValue;
			break;
	}
}

void KCoreShell::SetSortItem(int nType, int nValue, int nPos)
{
	int nIndex = (int)g_MagicDesc.String2MagicID((char*)nType);
	if (nIndex >= magic_skill_begin && nIndex < magic_normal_end)
	{
		Player[CLIENT_PLAYER_INDEX].m_FilterMagic[nPos][0] = nIndex;
		Player[CLIENT_PLAYER_INDEX].m_FilterMagic[nPos][1] = nValue;
	}
	else
	{
		Player[CLIENT_PLAYER_INDEX].m_FilterMagic[nPos][0] = 0;
		Player[CLIENT_PLAYER_INDEX].m_FilterMagic[nPos][1] = 0;
	}
}

void KCoreShell::FkAutoSetFillterMagic(int nType, int nValue, int nPos)//fkauto
{
	int nIndex = (int)g_MagicDesc.String2MagicID((char*)nType);
	if (nIndex >= magic_skill_begin && nIndex < magic_normal_end)
	{
		Player[CLIENT_PLAYER_INDEX].m_cAuto.m_FilterMagic[nPos][0] = nIndex;
		Player[CLIENT_PLAYER_INDEX].m_cAuto.m_FilterMagic[nPos][1] = nValue;
	}
	else
	{
		Player[CLIENT_PLAYER_INDEX].m_cAuto.m_FilterMagic[nPos][0] = 0;
		Player[CLIENT_PLAYER_INDEX].m_cAuto.m_FilterMagic[nPos][1] = 0;
	}
}

void KCoreShell::SetActiveAutoPlay(BOOL nActive)
{
	/*Player[CLIENT_PLAYER_INDEX].SendInfoAuto();
	if (nActive)
		SendClientCmdAutoPlay(1,1);
	else
		SendClientCmdAutoPlay(1,0);	*/
}

int KCoreShell::GetProtocolSize(BYTE byProtocol)
{
	if (byProtocol <= s2c_clientbegin || byProtocol >= s2c_end)
		return -1;
	return g_nProtocolSize[byProtocol - s2c_clientbegin - 1];
}

#ifdef SWORDONLINE_SHOW_DBUG_INFO
extern int		g_bShowObstacle;
extern bool		g_bShowGameInfo;	
#endif
int KCoreShell::Debug(unsigned int uDataId, unsigned int uParam, int nParam)
{
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	switch(uDataId)
	{
	case DEBUG_SHOWINFO:
		Player[CLIENT_PLAYER_INDEX].m_DebugMode = !Player[CLIENT_PLAYER_INDEX].m_DebugMode;
		g_bShowGameInfo = !g_bShowGameInfo;
		break;
	case DEBUG_SHOWOBSTACLE:
		g_bShowObstacle = !g_bShowObstacle;
		break;
	}
#endif
	return 0;
}

DWORD KCoreShell::GetPing()
{
	return g_SubWorldSet.GetPing();
}

//void KCoreShell::SendPing()
//{
//	SendClientCmdPing();
//}

void KCoreShell::SetRepresentAreaSize(int nWidth, int nHeight)
{
	g_ScenePlace.SetRepresentAreaSize(nWidth, nHeight);
}

void KCoreShell::SetClient(LPVOID pClient)
{
	g_SetClient(pClient);
}

void KCoreShell::SendNewDataToServer(void* pData, int nLength)
{
	if (g_pClient)
		g_pClient->SendPackToServer(pData, nLength);
}

int	KCoreShell::SceneMapOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case GSMOI_SCENE_TIME_INFO:
		if (uParam)
		{
			KUiSceneTimeInfo* pInfo = (KUiSceneTimeInfo*)uParam;
			g_ScenePlace.GetSceneNameAndFocus(pInfo->szSceneName, pInfo->nSceneId,
				pInfo->nScenePos0, pInfo->nScenePos1);
			pInfo->nGameSpaceTime = (SubWorld[0].m_dwCurrentTime / 100) % 1440;
		}
		break;
	case GSMOI_SCENE_TIME_INFO_OFTEN:
		if (uParam)
		{
			KUiSceneTimeInfoOften* pInfo = (KUiSceneTimeInfoOften*)uParam;
			g_ScenePlace.GetSceneNameAndFocusOften(pInfo->szSceneName, pInfo->nSceneId,
				pInfo->nScenePos0, pInfo->nScenePos1);
			pInfo->nGameSpaceTime = (SubWorld[0].m_dwCurrentTime / 100) % 1440;
		}
		break;
	case GSMOI_SCENE_MAP_INFO:
		nRet = g_ScenePlace.GetMapInfo((KSceneMapInfo*)uParam);
		break;
	case GSMOI_IS_SCENE_MAP_SHOWING:
		g_ScenePlace.SetMapParam(uParam, nParam);
		break;
	case GSMOI_PAINT_SCENE_MAP:
		g_ScenePlace.PaintMap(uParam, nParam);
		break;
	case GSMOI_SCENE_MAP_FOCUS_OFFSET:
		g_ScenePlace.SetMapFocusPositionOffset((int)uParam, nParam);
		break;
	case GSMOI_SCENE_FOLLOW_WITH_MAP:	
		g_ScenePlace.FollowMapMove(nParam);
		break;
	case GSMOI_IS_SCENE_DIRECT_MAP:
		nRet = g_ScenePlace.OnDirectMap((int)uParam, nParam);
		break;
	case GSMOI_IS_SCENE_DO_DIRECT_MAP:
		g_ScenePlace.DoDirectMap((int)uParam, nParam);
		break;
	}
	return nRet;
}

int	KCoreShell::TongOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case GTOI_TONG_CREATE:		
		Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyCreateTong(nParam, (char *)uParam);
		break;
	case GTOI_TONG_IS_RECRUIT:		
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			nRet = Npc[pItem->nIndex].m_nTongFlag;
		}
		break;
	case GTOI_TONG_GET_RECRUIT:
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			//nRet = Npc[pItem->nIndex].GetRecruit();
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTong.GetRecruit();
		}
		break;
	case GTOI_TONG_RECRUIT:     
		if (uParam)
		{
			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeRecruit(1);
			//Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeLevel(10);
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeRecruit(0);
			//Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeLevel(9);
		}
		break;
	case GTOI_TONG_ACTION:         
		{
    		KTongOperationParam *Oper = (KTongOperationParam *)uParam;
		    KTongMemberItem *TargetInfo = (KTongMemberItem *)nParam;
		    KUiPlayerItem TargetPlayer;
		    int nKind;

		    switch(Oper->eOper)
		    {
		    case TONG_ACTION_DISMISS:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyKick(Oper->nData[0], Oper->nData[1], TargetInfo->Name);
    			break;
		    case TONG_ACTION_ASSIGN:
    			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInstate(Oper->nData[0], Oper->nData[1], Oper->nData[2], Oper->nData[3], TargetInfo->Name);
			    break;
		    case TONG_ACTION_DEMISE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyChangeMaster(Oper->nData[0], Oper->nData[1], TargetInfo->Name);
    			break;
		    case TONG_ACTION_LEAVE:
    			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyLeave();
    			break;
		    case TONG_ACTION_RECRUIT:
    			break;
		    case TONG_ACTION_APPLY:
				if(!FindSpecialNPC(TargetInfo->Name, &TargetPlayer, nKind))
				{
    				break;
				}
    			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyAddTong(TargetPlayer.uId);
			    break;
			case TONG_ACTION_CHANGE_TITLE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyChangeTitle(Oper->nData[0], Oper->nData[1], TargetInfo->Name, Oper->szPassword);
				break;
			case TONG_ACTION_CHANGE_MALE_TITLE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyChangeSexTitle(Oper->szPassword, 0);
				break;
			case TONG_ACTION_CHANGE_FEMALE_TITLE:
				{
					Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyChangeSexTitle(Oper->szPassword, 1);
					g_DebugLog("gia tri title nu truyen len: %s", Oper->szPassword);
				}
   				break;
			case TONG_ACTION_CHANGE_CAMP_JUSTIE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeCamp(1);	
   				break;
			case TONG_ACTION_CHANGE_CAMP_EVIL:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeCamp(2);
   				break;
			case TONG_ACTION_CHANGE_CAMP_BALANCE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeCamp(3);
   				break;
			case TONG_ACTION_CHANGE_WAYEDIT:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeWayEdit(Oper->lpszParam);
				break;
			case TONG_ACTION_CHANGE_NEXTTARGET:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeNextTarget(Oper->lpszParam);
				break;
		    }
		}
		break;

	case GTOI_TONG_JOIN_REPLY:
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			Player[CLIENT_PLAYER_INDEX].m_cTong.AcceptMember(pItem->nIndex, g_FileName2Id(pItem->Name), nParam);
		}
		break;

	case GTOI_REQUEST_PLAYER_TONG:	
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			if (pItem->nIndex == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_TONG_HEAD,
					pItem->nIndex, 0, 0);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_TONG_HEAD,
					pItem->nIndex, 0, 0);
			}
		}
		break;
	case GTOI_REQUEST_TONG_DATA:     
		if (uParam)
		{
			if (Player[CLIENT_PLAYER_INDEX].m_cTong.CheckIn() == 0)
				break;

			KUiGameObjectWithName	*pObj = (KUiGameObjectWithName*)uParam;
			char	szTongName[32];
			DWORD	dwTongNameID;

			szTongName[0] = 0;
			Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongName(szTongName);
			if (!szTongName[0])
				break;

			dwTongNameID = g_FileName2Id(pObj->szName);
			if (dwTongNameID != Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongNameID())
				break;

			switch (pObj->nData)
			{
			case enumTONG_FIGURE_MASTER:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_TONG_HEAD,
					Player[CLIENT_PLAYER_INDEX].m_nIndex, 0, 0);
				break;
			case enumTONG_FIGURE_DIRECTOR:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_TONG_HEAD,
					Player[CLIENT_PLAYER_INDEX].m_nIndex, 0, 0);
				break;
			case enumTONG_FIGURE_MANAGER:
				if (!Player[CLIENT_PLAYER_INDEX].m_cTong.CanGetManagerInfo(dwTongNameID))
					break;
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_MANAGER,
					dwTongNameID, nParam, defTONG_ONE_PAGE_MAX_NUM);
				break;
			case enumTONG_FIGURE_MEMBER:
				if (!Player[CLIENT_PLAYER_INDEX].m_cTong.CanGetMemberInfo(dwTongNameID))
					break;
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_MEMBER,
					dwTongNameID, nParam, defTONG_ONE_PAGE_MAX_NUM);
				break;
			}
		}
		break;
	case GTOI_TONG_MONEY_ACTION:
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			switch(nParam)
			{
 			case TONG_ACTION_SAVE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplySaveMoney(pItem->nData);
				break;
			case TONG_ACTION_GET:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyGetMoney(pItem->nData);
				break;
			case TONG_ACTION_SND:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplySndMoney(pItem->nData);
				break;
			}
		}
		break;
	}
	return nRet;
}

int KCoreShell::TeamOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case TEAM_OI_GD_INFO:		
		if (uParam)
		{
			KUiPlayerTeam* pTeam = (KUiPlayerTeam*)uParam;
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetInfo(pTeam);
		}
		break;
	case TEAM_OI_GD_MEMBER_LIST:
		nRet = g_Team[0].GetMemberInfo((KUiPlayerItem *)uParam, nParam);
		break;
	case TEAM_OI_GD_REFUSE_INVITE_STATUS:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetAutoRefuseState();
		break;
	case TEAM_OI_COLLECT_NEARBY_LIST:
		NpcSet.GetAroundOpenCaptain(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp);
		break;
	case TEAM_OI_APPLY:			
		if (uParam)
			Player[CLIENT_PLAYER_INDEX].ApplyAddTeam(((KUiTeamItem*)uParam)->Leader.nIndex);
		break;
	case TEAM_OI_CREATE:		
		Player[CLIENT_PLAYER_INDEX].ApplyCreateTeam();
		break;
	case TEAM_OI_APPOINT:		
		Player[CLIENT_PLAYER_INDEX].ApplyTeamChangeCaptain(((KUiPlayerItem*)uParam)->uId);		
		break;
	case TEAM_OI_INVITE:		
		if (uParam)
		{
			Player[CLIENT_PLAYER_INDEX].TeamInviteAdd(((KUiPlayerItem*)uParam)->uId);

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_SEND_INVITE, ((KUiPlayerItem*)uParam)->Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case TEAM_OI_KICK:			
		Player[CLIENT_PLAYER_INDEX].TeamKickMember(((KUiPlayerItem*)uParam)->uId);
		break;
	case TEAM_OI_LEAVE:			
		Player[CLIENT_PLAYER_INDEX].LeaveTeam();
		break;
	case TEAM_OI_CLOSE:			
		Player[CLIENT_PLAYER_INDEX].ApplyTeamOpenClose(nParam);
		break;
	case TEAM_OI_REFUSE_INVITE:		
		Player[CLIENT_PLAYER_INDEX].m_cTeam.SetAutoRefuseInvite(nParam);//0 t¾t tù ®éng tõ chèi lêi mêi//1 bËt tù ®éng tõ chèi lêi mêi
		break;
	case TEAM_OI_APPLY_RESPONSE:		
		if (uParam)
		{
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].AcceptTeamMember(((KUiPlayerItem*)uParam)->uId);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.DeleteOneFromApplyList(((KUiPlayerItem*)uParam)->uId);
				//Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
			}
		}
		break;
	case TEAM_OI_INVITE_RESPONSE:
		if (uParam)
			Player[CLIENT_PLAYER_INDEX].m_cTeam.ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
		break;
	case TEAM_OI_GET_NPC_MAP_POS:
		{
			if (uParam && nParam)
			{
				KUiPlayerItem *m_pPlayersList = (KUiPlayerItem*)uParam;
			}
		}
		break;
	}
	return nRet;
}

int KCoreShell::GetNPCBAITAN2()
{
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan;
}

int KCoreShell::GetNPCBAITAN(int nIndex)
{
	int nID = NpcSet.SearchID(nIndex);
	return Npc[nID].m_BaiTan;
}

int KCoreShell::GetPriceSell(unsigned int uId)
{
	return Item[uId].GetCurPrice();
}

int KCoreShell::GetStallState()
{
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan;
}

int KCoreShell::GetGenreItem2(unsigned int uId)
{
	return Item[uId].GetGenre();
}

int KCoreShell::GetPriceSell2(unsigned int uId)
{
	return Item[uId].GetSetPrice();
}

int KCoreShell::GetTypeItem(unsigned int uId)
{
	return Item[uId].GetColorItem();
}

int KCoreShell::GetNatureItem(unsigned int uItemId, unsigned int uGenre /* = CGOG_ITEM*/)
{
	int nIndex = uItemId;
	KItem*	pItem = NULL;
	if (uGenre == CGOG_ITEM)
		return Item[nIndex].GetColorItem();
	else if (uGenre == CGOG_NPCSELLITEM)
	{
		int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
		if (nBuyIdx != -1)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, uItemId);
			if (nIndex >= 0)
			{
				pItem = BuySell.GetItem(nIndex);
				return pItem->GetColorItem();
			}
		}
	}
	return Item[nIndex].GetColorItem();
}

int KCoreShell::GetGenreItem(unsigned int uItemId, unsigned int uGenre /* = CGOG_ITEM*/)
{
	int nIndex = uItemId;
	KItem*	pItem = NULL;
	if (uGenre == CGOG_ITEM)
		return Item[nIndex].GetGenre();
	else if (uGenre == CGOG_NPCSELLITEM)
	{
		int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
		if (nBuyIdx != -1)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, uItemId);
			if (nIndex >= 0)
			{
				pItem = BuySell.GetItem(nIndex);
				return pItem->GetGenre();
			}
		}
	}
	return Item[nIndex].GetGenre();
}

int KCoreShell::GetObjAtCountRegionInSuperShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	int nIndex = 0;
	if(uParam)
	{
		int nPage = 0;
		char* pInfo = (char *)uParam;
		//KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		if(nParam < BuySell.GetWidth())
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, nParam);
			KItem* pItem = BuySell.GetItem(nIndex);
			if (nIndex >= 0 && pItem)
			{
				strcpy(pInfo, pItem->GetImageName());
				//pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				//pInfo->Obj.uId = nParam;
				//pInfo->Region.h = 0;
				//pInfo->Region.v = 0;
				//pInfo->Region.Width = pItem->GetWidth();
				//pInfo->Region.Height = pItem->GetHeight();
				//pInfo->nContainer = nPage;
				return 1;
			}
		}
	}
	return nRet;
}

int KCoreShell::GetDataSuperShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop = nSaleId;
	int nIndex = 0;
	int nCount = 0;
	if (uParam)
	{
		int nPage = 0;
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;
				pInfo->Region.h = 0;
				pInfo->Region.v = 0;
				pInfo->Region.Width = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();
				pInfo->nContainer = nPage;
				nCount++;
				pInfo++;
				if (nCount %15 == 0)
					nPage ++;
			}
		}			
		nRet = nPage;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}

int KCoreShell::GetItemIdxNpcShop(char* szItemName)//fkauto
{
	int nIndex = -1;
	int curShop = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop;
	int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[curShop];
	if (nBuyIdx == -1)
		return -1;
	if (nBuyIdx >= BuySell.GetHeight())
		return -1;
	for (int i = 0; i < BuySell.GetWidth(); i++)
	{
		nIndex = BuySell.GetItemIndex(nBuyIdx, i);
		KItem* pItem = BuySell.GetItem(nIndex);
		if (nIndex >= 0 && pItem)
		{
			if(strcmp(pItem->GetName(), szItemName) == 0)
				return i;
		}
	}
	return -1;
}

int KCoreShell::GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop = nSaleId;
	int nIndex = 0;
	int nCount = 0;
	if (uParam)
	{
		int nPage = 0;
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		if (!BuySell.m_pSShopRoom)
			return 0;
		BuySell.m_pSShopRoom->Clear();
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;
				POINT	Pos;
				if (BuySell.m_pSShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
				{
					BuySell.m_pSShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				else
				{
					nPage++;
					BuySell.m_pSShopRoom->Clear();
					BuySell.m_pSShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);
					BuySell.m_pSShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				pInfo->Region.h = Pos.x;
				pInfo->Region.v = Pos.y;
				pInfo->Region.Width = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();
				pInfo->nContainer = nPage;
				nCount++;
				pInfo++;
			}
		}			
		nRet = nPage;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}

int KCoreShell::GetOwnValue(int nMoneyUnit)
{
	int nRet = 0;
	switch (nMoneyUnit)
	{
		case moneyunit_money://tien van
			nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);
			break;
		case moneyunit_extpoint://tien xu
			//nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(0, 4, Def_ID_XU_TIENDONG);
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_XU);
			break;
		case moneyunit_fuyuan://phuc duyen
			nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].nFuYuan;
			break;
		case moneyunit_repute://danh vong
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
			break;
		case moneyunit_accum: //tong kim
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_ACCUM);
			break;
		case moneyunit_honor: //lien dau
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_HONOR);
			break;
		case moneyunit_respect: //c«ng tr¹ng uy danh
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT);
			break;
		default:
			break;
	}
	return nRet;
}

int KCoreShell::AutoPlayOperation(unsigned int uOper, unsigned int uParam, int nParam)//fkauto
{
	int nRet = 0, i = 0;
	switch(uOper)
	{	
		case AUTOPLAY_OI_ACTIVE:	//-----------chiÕn ®Êu--------------
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.InitFkAuto(uParam);
			}
			break;
		case AUTOPLAY_OI_PAUSE:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.PauseFkAuto(uParam);
			}
			break;
		case AUTOPLAY_OI_FIGHT_B: //chiÕn ®Êu
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_FIGHT_V:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nFightRange = uParam;
			}
			break;
		case AUTOPLAY_OI_FIGHT_S:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightSelect = uParam;
			}
			break;
		case AUTOPLAY_OI_DISTANCE_B: //tiÕp cËn
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightDistance = uParam;
			}
			break;
		case AUTOPLAY_OI_DISTANCE_V:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nFightDistance = uParam;
			}
			break;
		case AUTOPLAY_OI_DISTANCE_S:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightDistanceSelect = uParam;
			}
			break;
		case AUTOPLAY_OI_SELFDEF_B://tù vÖ
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightSelfDef = uParam;
			}
			break;
		case AUTOPLAY_OI_SELFDEF_V:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nFightSelfDefValue = uParam;
			}
			break;
		case AUTOPLAY_OI_SELFDEF_S:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightSelfDefSelect = uParam;
			}
			break;
		case AUTOPLAY_OI_SUPPORT_SKILL:
			{
				switch(uParam)
				{
					case 1:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ArrayStateSkill[0] = nParam;
						break;
					case 2:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ArrayStateSkill[1] = nParam;
						break;
					case 3:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ArrayStateSkill[2] = nParam;
						break;
					case 4:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_LeftSkillID = nParam;
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(nParam);//set chiªu tay tr¸i
						break;
					case 5:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_RightSkillID = nParam;
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(nParam);//set chiªu tay ph¶i
						break;
					case 6:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_SkillSelfDefID = nParam;
						break;
					case 7:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_SkillBossID = nParam;
						break;						
				}
			}
			break;
		case AUTOPLAY_OI_SHORTKEY:
			{
				if(uParam == 1)
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ShortcutKeySelect1 = nParam;
				}
				else
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ShortcutKeySelect2 = nParam;
				}
			}
			break;
		case AUTOPLAY_OI_SHORTCUT_EDIT:
			{
				if(uParam == 1)
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nShortcutEditBox1 = nParam;
				}
				else
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nShortcutEditBox2 = nParam;
				}
			}
			break;
		case AUTOPLAY_OI_SHORTCUT:
			{
				if(uParam == 1)
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ShortcutOptSelect1 = nParam;
				}
				else
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ShortcutOptSelect2 = nParam;
				}
			}
			break;
		case AUTOPLAY_OI_USKILL_RIGHT_B:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bSkillRightCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_NR_DO_SKILL_B:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bNRDoSkillCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_HP_B://b¬m sl //-----------phôc håi---------------
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bReHPCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_HP_1:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReHPEditBox1 = uParam;
			}
			break;
		case AUTOPLAY_OI_HP_2:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReHPEditBox2 = uParam;
			}
			break;
		case AUTOPLAY_OI_HP_3:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReHPEditBox3 = uParam;
			}
			break;
		case AUTOPLAY_OI_MP_B://b¬m nl
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bReMPCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_MP_1:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReMPEditBox1 = uParam;
			}
			break;
		case AUTOPLAY_OI_MP_2:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReMPEditBox2 = uParam;
			}
			break;
		case AUTOPLAY_OI_MP_3:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReMPEditBox3 = uParam;
			}
			break;
		case AUTOPLAY_OI_TP_1://tdp sl <
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownHPVCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nTownHPVEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_TP_2://tdp nl <
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownMPVCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nTownMPVEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_TP_3://tdp het binh sl
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownHPNCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_TP_4://tdp het binh nl
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownMPNCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_TP_5:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownFCellCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nTownFCellTxtSelect = nParam;
			}
			break;
		case AUTOPLAY_OI_TP_6:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTowMoneyCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nTownMoneyEditBox = nParam * 10000;//®¬n vÞ tÝnh v¹n l­îng
			}
			break;
		case AUTOPLAY_OI_TP_7:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownAbradeCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nTownAbradeEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_USEM_FCELL:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bUseHPCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_ANTI_TOXIC:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bUseAntiToxicCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_UX2ITEM:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bUseExpCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_UX2SKILL:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bUseSkillCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_NMBUFF:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuffCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nBuffEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_BUFF_TEAM:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuffTeamCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_OPENBAGHP:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bOpenBagHPCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nOpenBagHPEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_RING_TDP:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_RingTDPCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_1:	//-----------nhÆt ®å---------------
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bAutoPickCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nAutoPickEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_2:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bPickAllCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_3:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bPickSpeCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_4:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bNoneFightCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_5:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nPickSelTxtSelect = uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_6:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bPriceCellCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nPriceCellEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_7:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bLevelCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nLevelEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_8:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bRiAmPeCheckBox= uParam;//gi÷ trang søc
			}
			break;
		case AUTOPLAY_OI_OBJ_9:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nRiEditBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nAmPeEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_10:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bSortBagCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nSortBagEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_11:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bNPickBackLCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_12:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFillterItemCheckBox = uParam; //läc ®å
			}
		case AUTOPLAY_OI_OBJ_13:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bKeepPurpleCheckBox= uParam; //nhÆt ®å tÝm
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nKeepPurpleEditBox= nParam; // sè dßng
			}
			break;
		case AUTOPLAY_OI_MOVE_1: //-- di chuyÓn
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFollowCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nFollowEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_2:
			{
				strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nFollowNameTxt, (char*)uParam);
			}
			break;
		case AUTOPLAY_OI_MOVE_3://quanh ®iÓm
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bLoopPosCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nLoopPosRangEBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_4:
			{
				if(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bLoopPosCheckBox)
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nXLoopPosTxt = uParam;
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nYLoopPosTxt = nParam;
				}
				else
				{
					int nX = 0; int nY = 0;
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX, &nY);
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nXLoopPosTxt = nX;
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nYLoopPosTxt = nY;
				}
			}
			break;
		case AUTOPLAY_OI_MOVE_5: //theo to¹ ®é
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bConstPosCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_6:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bMoveMapsCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nMoveMapID= nParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_7:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bIRMoveCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_8://®¸nh qu¸i trªn ®­êng ®i
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFMORCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_9:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bDamnMonterCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_10:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTalkAnnyCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_11:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFlCaptainCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_12:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFlAnnyPTCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_1://--b¶n ®å--
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bReturnCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_2:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bSellCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_3:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bRepairCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_4:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bQuickRepairCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_5:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bGetMoneyCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_6:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nGetMoneyEditBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nPwdMoneyEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_7:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuyHPCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nBuyHPEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_8: //tªn m¸u
			{
				if(uParam)
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_BuyHPTxtSelect, (char*)uParam);
			}
			break;
		case AUTOPLAY_OI_MAP_9:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuyMPCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nBuyMPEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_10://tªn mana
			{
				if(uParam)
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_BuyMPTxtSelect, (char*)uParam);
			}
			break;
		case AUTOPLAY_OI_MAP_11:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuyToxicCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nBuyToxicEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_12:
			{
				if(uParam)
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_BuyToxicTxtSelect, (char*)uParam);
			}
			break;
		case AUTOPLAY_OI_MAP_13:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuyTownCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nBuyTownEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_14:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bMapRunPosCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nMapTxtId= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_15:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bKeepMoneyCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nKeepMoneyEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_16:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bGetFYCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_17:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bGoFarAwayCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_GoFarAwayTxtSelect= nParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_1://---tæ ®éi---
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bAutoCTeamCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_2: //lu«n lµm nhãm tr­ëng
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bAlwayLeaderCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_3: //tù ®éng PT tÊt c¶
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bPTAllCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_4: //tõ chèi mäi lêi mêi
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bDecAllInviCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cTeam.SetAutoRefuseInvite(uParam);//0 t¾t tù ®éng tõ chèi lêi mêi//1 bËt tù ®éng tõ chèi lêi mêi
			}
			break;
		case AUTOPLAY_OI_TEAM_5:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bLeaveTeamCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_6:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nLeaveTeamEditBox1= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nLeaveTeamEditBox2= nParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_7:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bKickTeamCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nKickTeamEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_8:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bSCountTeamCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_9://PT cïng bang
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bPTTongCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bPTTongCheckBox = uParam;
			}
			break;
	}
	return nRet;
}

void KCoreShell::GotoWhereDirect(int x, int y, int mode)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;

	if (mode < 0 || mode > 2)
		return;

	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		int bRun = false;

		if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
			mode == 2)
			bRun = true;

		int nX = x;
		int nY = y;
		int nZ = 0;

		int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

		if (!bRun)
		{
			Npc[nIndex].SendCommand(do_walk, nX, nY);
			// Send to Server
			SendClientCmdWalk(nX, nY);
		}
		else
		{
			Npc[nIndex].SendCommand(do_run, nX, nY);
			// Send to Server
			SendClientCmdRun(nX, nY);
		}
		Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
	}
}
bool CloseToTarget(const FindPathNode& point1, const FindPathNode& point2, int delta)
{
	int distance = sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2));
	g_DebugLog("Distance %d\n", distance);
	return distance <= delta;
}

#ifndef _SERVER
BOOL KCoreShell::AutoMove()
{
    FindPathNode nextPoint;
    FindPathNode currentPoint;

    int dX, dY;
    INT nCurX, nCurY;
    Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nCurX, &nCurY);
    BOOL nRet = FALSE;
	int delta = 15;
   // if (!GetPaintMode())
      // return nRet;
    if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.empty())
    {
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoMoveTemp.x == nCurX && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoMoveTemp.y == nCurY) //in lag point, can not move
        {
            Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].AutoMoveStuckCount += 1;
            if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].AutoMoveStuckCount > 10) {
				srand(static_cast<unsigned>(time(NULL)));
				dX = rand() % 201 - 100; // [-100, 100]
				dY = rand() % 201 - 100;
				GotoWhereDirect(nCurX + dX, nCurY + dY, 0);
                Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].AutoMoveStuckCount = 0;
            }
        }
        else {
            Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoMoveTemp.x = nCurX;
            Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoMoveTemp.y = nCurY;
            Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].AutoMoveStuckCount = 0;
        }
        // Go to the next point in auto move path
        nextPoint = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.front();

        currentPoint.x = nCurX;
        currentPoint.y = nCurY;
        dX = currentPoint.x & 0x1F;
        dY = currentPoint.y & 0x1F;
		//nextPoint.x = nextPoint.x + 0x10 - dX; // Anti-LAG
		//nextPoint.y = nextPoint.y + 0x10 - dY; // Anti-LAG

		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.size() == 1) { // 1 point to move, the target point
			g_DebugLog("1 target point left, running to %d, %d from Current %d, %d\n", nextPoint.x, nextPoint.y, currentPoint.x, currentPoint.y);
			GotoWhereDirect(nextPoint.x, nextPoint.y, 0);
			if (CloseToTarget(currentPoint, nextPoint, delta))
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.erase(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.begin()); //remove all point in path find
			return false;
		}
        if (g_JXPathFinder.GetNextStep(currentPoint, nextPoint, 2) == emKNEXTSTEP_RESULT_SUCCESS) 
        {
            // Go to next point in m_PathFind
            if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.empty())
                return FALSE;

            nextPoint = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.front();
            if (CloseToTarget(currentPoint, nextPoint, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentRunSpeed * 7) || (g_JXPathFinder.getStand().x == currentPoint.x && g_JXPathFinder.getStand().y == currentPoint.y)) {
                Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.erase(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.begin());
            }
            g_DebugLog("Running to %d, %d from Current %d, %d\n", nextPoint.x, nextPoint.y, currentPoint.x, currentPoint.y);
            GotoWhereDirect(nextPoint.x, nextPoint.y, 0);
        }
        else if (g_JXPathFinder.GetNextStep(currentPoint, nextPoint, 1) == emKNEXTSTEP_RESULT_ARRIVAL)
        {
            GotoWhereDirect(nextPoint.x, nextPoint.y, 0);
            return FALSE; // Still moving
        }
        return TRUE; // Auto moving
    }
    else
        return FALSE; // Nothing in auto move path
}

void KCoreShell::ClearPathFinder() {
	g_ScenePlace.bFlagMode = FALSE;
	g_ScenePlace.bPaintMode = FALSE;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ResetPathFind();
}

#endif