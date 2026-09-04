#include "KWin32.h"

#include "Windows.h"
#include "../../core/src/coreshell.h"
#include "../../core/src/GameDataDef.h"
#include "../../core/src/CoreObjGenreDef.h"
#include "Windows.h"
#include "KEngine.h"
#include "Elem/Wnds.h"
#include "Elem/MouseHover.h"
#include "UiCase/UiPlayerBar.h"
#include "UiCase/UiFaceSelector.h" //add by phong kiÒu
#include "UiCase/UiStatus.h"
#include "UiCase/UiTrade.h"
#include "UiCase/UiGamble.h"
#include "UiCase/UiSkills.h"
#include "UiCase/UiItem.h"
#include "UiCase/UiShop.h"
#include "UiCase/UiMsgSel.h"
#include "UiCase/UiMsgSel2.h"
#include "UiCase/UiMsgSel3.h"
#include "UiCase/UiMsgSel4.h"
#include "UiCase/UiMsgCentrePad.h"
#include "UiCase/UiInformation.h" //add by phong kiÒu using fkauto autoclick vÒ thµnh d­ìng søc
#include "UiCase/UiInformation2.h"
#include "UiCase/UiSysMsgCentre.h"
#include "UiCase/UiTeamManage.h"
#include "UiCase/UiSelPlayerNearby.h"
#include "UiCase/UiChatCentre.h"
#include "UiCase/UiStoreBox.h"
#include "UiCase/UiTaskNote.h"
#include "UiCase/UiTaskGuide.h"
#include "UiCase/UiTaskTrace.h"
#include "UiCase/UiNewsMessage.h"
#include "UiCase/UiNewsMessage1.h"
#include "UiCase/UiStrengthRank.h"
#include "UiCase/UiTongManager.h"
#include "UiCase/UiTongCreateSheet.h"
#include "UiCase/UiTongJX2.h"	// JX2 port
#include "UiCase/UiResetPass.h"
#include "UiCase/UiExBox1.h"
#include "UiCase/UiExBox2.h"
#include "UiCase/UiExBox3.h"
#include "UiCase/UiItemEX.h"
#include "UiCase/UiPlayerShop.h"
#include "UiCase/UiInit.h"
#include "UiCase/UiQuestDT.h"
#include "UiCase/UiQuestDT1.h"
#include "UiCase/UiBattleReport.h"
#include "UiCase/UiSuperShop.h"
#include "UiCase/UiAffairItem.h"
#include "UiCase/UiMantleInlay.h"
#include "UiCase/UiMantleWash.h"	// [PF13 01/09] panel Tay Luyen thuoc tinh an	// [PHI PHONG] panel kham Tinh Than Thach
#include "UiCase/UiSmelt.h"	// [DUNGLUYEN 01/09] box dung luyen Van Cuong
#include "UiCase/UiConnectInfo.h"
#include "UiCase/UiTimeBox.h"
#include "UiCase/UiInformation3.h"
#include "UiCase/UiGetString2.h"
#include "UiCase/UiGetNumber.h"
#include "UiCase/UiWorldmap.h"
#include "UiCase/UiMiniMap.h"
#include "UiCase/UiFinishQuest.h"
#include "UiCase/UiTrembleItem.h"
#include "UiCase/UiDiceItem.h"	// DICEITEM 26/08
#include "UiCase/UiMail.h"
#include "UiCase/UiAuction.h"	// [DAUGIA 04/09 A3]	// [MAIL 03/09 D2] cua so thu
#include "UiCase/UiChienLenh.h"	// [CL 04/09 DOT2] cua so Chien Lenh
#include "UiCase/UiPartnerCommon.h"	// [BDH-G4]
#include "UiCase/UiPartnerAttr.h"
#include "UiCase/UiPartnerSkill.h"
#include "UiCase/UiPartnerBag.h"
#include "UiCase/UiPartnerBar.h"
#include "UiCase/UiPet.h"	// [PETSYS]
#include "UiCase/UiCompoundItem.h"
#include "UiCase/UiOptions2.h"
#include "UiSoundSetting.h"
#include "UiCase/UiSkillTree.h"

#include "../Login/Login.h"
#include "UiCase/UiParadeItem.h"
#include "KTongProtocol.h"

#include "../S3Client.h"
#include "UiShell.h"

#include "../../Engine/Src/Text.h"
#include "UiCase/UiMeridian.h"
#include "UiCase/UiRankData.h"
#include "UiCase/UiSkillsNew.h"
#include "UiCase/SpringGame.h"

bool UiCloseWndsInGame(bool bAll);

extern iCoreShell* g_pCoreShell;

void GameWorldTips(unsigned int uParam, int nParam);

int CoreDataChangedCallback(unsigned int uDataId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	KUiTrade* pTradeBar = NULL;
	KUiGamble* pGamble = NULL;
	switch (uDataId)
	{
	case GDCNI_HOLD_OBJECT:
		Wnd_DragFinished();
		if (uParam && ((KUiObjAtRegion*)uParam)->Obj.uGenre != CGOG_NOTHING)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = ((KUiObjAtRegion*)uParam)->Obj.uGenre;
			Obj.uId = ((KUiObjAtRegion*)uParam)->Obj.uId;
			Obj.DataW = ((KUiObjAtRegion*)uParam)->Region.Width;
			Obj.DataH = ((KUiObjAtRegion*)uParam)->Region.Height;
			Wnd_DragBegin(&Obj, DrawDraggingGameObjFunc);
		}
		break;
	case GDCNI_PLAYER_BASE_INFO:
		{
			KUiStatus* pBar = KUiStatus::GetIfVisible();
			if (pBar)
				pBar->UpdateBaseData();

			KUiPlayerBaseInfo	Info;
			memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
			g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);

			if ((Info.nCurFaction >= 0) || 
				(Info.nCurTong != 0) 
				|| (Info.nMissionGroup >= 0)
				// || (Info.nRoomId >= 0)
				)
				KUiMsgCentrePad::QueryAllChannel();

			if (Info.nCurFaction < 0)
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Faction);

			if (Info.nCurTong == 0)
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Tong);

			if (Info.nMissionGroup < 0)
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Msgr);

			//if (Info.nRoomId < 0)
			//	KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Cr);	
		}
		break;
	case GDCNI_PLAYER_RT_ATTRIBUTE:
		{
			KUiStatus* pBar = KUiStatus::GetIfVisible();
			if (pBar)
				pBar->UpdateData();
		}
		break;
	case GDCNI_PLAYER_MERIDIAN_SYNC:
		{
		KUiMeridian* pMeridian = KUiMeridian::GetIfVisible();
		if (pMeridian)
			pMeridian->UpdateMeridianLevel();
		}
		break;
	case GDCNI_PLAYER_IMMED_ITEMSKILL://Ö÷½ÇµÄÁ¢¼´Ê¹ÓÃÎïÆ·ÓëÎä¹¦
		if (uParam)
		{
			KUiGameObject* pObj = (KUiGameObject*)uParam;
			KUiPlayerBar* pBar = KUiPlayerBar::GetIfVisible();
			if (pBar)
			{
				if (nParam >= 0)
					pBar->UpdateItem(nParam, pObj->uGenre, pObj->uId);
				else
					pBar->UpdateSkill(-nParam - 1, pObj->uGenre, pObj->uId);
			}
		}
		break;
	case GDCNI_OBJECT_CHANGED:
		if (uParam)
		{
			KUiObjAtContRegion* pObject = (KUiObjAtContRegion*)uParam;
			if (pObject->eContainer == UOC_ITEM_TAKE_WITH)
			{
				pTradeBar = KUiTrade::GetIfVisible();
				pGamble = KUiGamble::GetIfVisible();
				if (pTradeBar)
				{
					pTradeBar->OnChangedTakewithItem((KUiObjAtRegion*)uParam, nParam);
				}
				else if (pGamble)
				{
					pGamble->OnChangedTakewithItem((KUiObjAtRegion*)uParam, nParam);
				}
				else
				{
					KUiItem* pItemsBar = KUiItem::GetIfVisible();
					if (pItemsBar)
						pItemsBar->UpdateItem((KUiObjAtRegion*)uParam, nParam);
				}
			}
			else if (pObject->eContainer == UOC_EQUIPTMENT)
			{
				KUiStatus* pEquips = KUiStatus::GetIfVisible();
				if (pEquips)
					pEquips->UpdateEquip((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_STORE_BOX)
			{
				KUiStoreBox* pStoreBox = KUiStoreBox::GetIfVisible();
				if (pStoreBox)
					pStoreBox->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_EX_BOX1)
			{
				KUiExBox1* pExBox1 = KUiExBox1::GetIfVisible();
				if (pExBox1)
					pExBox1->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_EX_BOX2)
			{
				KUiExBox2* pExBox2 = KUiExBox2::GetIfVisible();
				if (pExBox2)
					pExBox2->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_EX_BOX3)
			{
				KUiExBox3* pExBox3 = KUiExBox3::GetIfVisible();
				if (pExBox3)
					pExBox3->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_PARTNER_BAG)	// [BDH-G4]
			{
				KUiPartnerBag* pPartnerBag = KUiPartnerBag::GetIfVisible();
				if (pPartnerBag)
					pPartnerBag->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_ITEM_EX)
			{
				KUiItemEX* pItemEX = KUiItemEX::GetIfVisible();
				if (pItemEX)
					pItemEX->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_TO_BE_TRADE)
			{
				KUiTrade* pTrade = KUiTrade::GetIfVisible();
				if (pTrade)
					pTrade->OnSelfChangedItem(pObject, nParam);
			}
			else if (pObject->eContainer == UOC_TO_BE_GAMBLE)
			{
				KUiGamble* pGamble = KUiGamble::GetIfVisible();
				if (pGamble)
					pGamble->OnSelfChangedItem(pObject, nParam);
			}
			else if (pObject->eContainer == UOC_AFFAIR_ITEM)
			{
				KUiAffairItem* pItem = KUiAffairItem::GetIfVisible();
				if (pItem)
					pItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);
				// [PHI PHONG] bang kham dung CHUNG khoang chua nay. Truoc day chi
				// KUiAffairItem duoc bao, nen bo do vao bang kham thi khong hien.
				KUiMantleInlay* pInlay = KUiMantleInlay::GetIfVisible();
				if (pInlay)
					pInlay->UpdateItem((KUiObjAtRegion*)uParam, nParam);
				// [PF13 01/09] panel tay luyen cung dung khoang chua nay
				KUiMantleWash* pWash = KUiMantleWash::GetIfVisible();
				if (pWash)
					pWash->UpdateItem((KUiObjAtRegion*)uParam, nParam);
				// [DUNGLUYEN 01/09] box dung luyen cung dung khoang chua nay
				KUiSmelt* pSmelt = KUiSmelt::GetIfVisible();
				if (pSmelt)
					pSmelt->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_TREMBLE_ITEM)
			{
				KUiTrembleItem* pItem = KUiTrembleItem::GetIfVisible();
				if (pItem)
					pItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_COMPONE_ITEM)	// [UILOREN] khuon UOC_TREMBLE_ITEM
			{
				KUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();
				if (pCompItem)
					pCompItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_COMPTWO_ITEM)	// [UILOREN] khuon UOC_TREMBLE_ITEM
			{
				KUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();
				if (pCompItem)
					pCompItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_COMPTHREE_ITEM)	// [UILOREN] khuon UOC_TREMBLE_ITEM
			{
				KUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();
				if (pCompItem)
					pCompItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_DISTILL_ITEM)	// [UILOREN] khuon UOC_TREMBLE_ITEM
			{
				KUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();
				if (pCompItem)
					pCompItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_FORGE_ITEM)	// [UILOREN] khuon UOC_TREMBLE_ITEM
			{
				KUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();
				if (pCompItem)
					pCompItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_ENCHASE_ITEM)	// [UILOREN] khuon UOC_TREMBLE_ITEM
			{
				KUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();
				if (pCompItem)
					pCompItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			// [LOREN 28/08] Do pho: bao UI khi o doi - THIEU nhanh nay thi tin "o vua doi" cua
			// the Do pho roi vao hu khong, cua so khong bao gio biet ma ve lai
			// (do bang log: ca tep chi co DUNG MOT lan UpdateAllItem luc mo the).
			else if (pObject->eContainer == UOC_ATLAS_ITEM)
			{
				KUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();
				if (pCompItem)
					pCompItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_IN_HAND)
			{
				Wnd_DragFinished();
				if (pObject->Obj.uGenre != CGOG_NOTHING)
				{
					KUiDraggedObject Obj;
					Obj.uGenre = pObject->Obj.uGenre;
					Obj.uId = pObject->Obj.uId;
					Obj.DataW = pObject->Region.Width;
					Obj.DataH = pObject->Region.Height;
					Wnd_DragBegin(&Obj, DrawDraggingGameObjFunc);
				}
			}
			else if (pObject->eContainer == UOC_IMMEDIA_ITEM)
			{
				KUiPlayerBar* pBar = KUiPlayerBar::GetIfVisible();
				if (pBar)
				{
					if (nParam)
						pBar->UpdateItem(pObject->Region.h, pObject->Obj.uGenre, pObject->Obj.uId);
					else
						pBar->UpdateItem(pObject->Region.h, CGOG_NOTHING, 0);
				}
			}
			else if (pObject->eContainer == UOC_IMMEDIA_SKILL)
			{
				KUiPlayerBar* pBar = KUiPlayerBar::GetIfVisible();
				if (pBar)
				{
					if (nParam)
						pBar->UpdateSkill(pObject->Region.h, pObject->Obj.uGenre, pObject->Obj.uId);
					else
						pBar->UpdateSkill(pObject->Region.h, CGOG_NOTHING, 0);
				}
			}
		}
		break;
	case GDCNI_LIVE_SKILL_BASE:
	{
		KUiSkills* pPad = KUiSkills::GetIfVisible();
		if (pPad)
			pPad->UpdateLiveBaseData();
	}
	break;
	case GDCNI_FIGHT_SKILL_POINT:	
		{
			KUiSkills* pPad = KUiSkills::GetIfVisible();
			if (pPad)
			{
				pPad->UpdateFightRemainPoint(nParam);
			}
			KUiSkillsNew* pPad1 = KUiSkillsNew::GetIfVisible();
			if (pPad1)
			{
				pPad1->UpdateFightRemainPoint(nParam);
			}
		}
		break;
	case GDCNI_SKILL_CHANGE:
		if (uParam)
		{
			KUiSkills::UpdateSkill((KUiSkillData*)uParam, nParam);
		}
		break;
		/*	case GDCNI_PLAYER_LEADERSHIP:	//Ö÷½ÇÍ³Ë§ÄÜÁ¦Ïà¹ØµÄÊý¾Ý·¢Éú±ä»¯
				{
					KUiManage* pBar = KUiManage::GetIfVisible();
					if (pBar)
						pBar->UpdateLeaderData();
				}
				break;
		*/
	case GDCNI_TRADE_START:
		if (uParam)
		{
			UiCloseWndsInGame(false);
			KUiTrade::OpenWindow((KUiPlayerItem*)uParam);
		}
		break;
	case GDCNI_TRADE_DESIRE_ITEM:
		pTradeBar = KUiTrade::GetIfVisible();
		if (pTradeBar)
			pTradeBar->OnOppositeChangedItem((KUiObjAtRegion*)uParam, nParam);
		break;
	case GDCNI_TRADE_OPER_DATA:
		pTradeBar = KUiTrade::GetIfVisible();
		if (pTradeBar)
			pTradeBar->UpdateOperData();
		pGamble = KUiGamble::GetIfVisible();
		if (pGamble)
			pGamble->UpdateOperData();
		break;
	case GDCNI_TRADE_END:
		pTradeBar = KUiTrade::GetIfVisible();
		if (pTradeBar)
			pTradeBar->UpdateTradeEnd(nParam);
		break;
	case GDCNI_GAMBLE_START:
		if (uParam)
		{
			UiCloseWndsInGame(false);
			KUiGamble::OpenWindow((KUiPlayerItem*)uParam);
		}
		break;
	case GDCNI_GAMBLE_RESET:
		pGamble = KUiGamble::GetIfVisible();
		if (pGamble)
			pGamble->GambleResetWindow((KUiPlayerItem*)uParam);
		break;
	case GDCNI_GAMBLE_RESULT:
		pGamble = KUiGamble::GetIfVisible();
		if (pGamble) {
			if (nParam)
				pGamble->setResult(nParam);
			pGamble->OnChangedTakewithItem(NULL, 0); //reset takewith view
		}
		break;
	case GDCNI_GAMBLE_DESIRE_ITEM:
		pGamble = KUiGamble::GetIfVisible();
		if (pGamble)
			pGamble->OnOppositeChangedItem((KUiObjAtRegion*)uParam, nParam);
		break;
	case GDCNI_GAMBLE_OPER_DATA:
		pGamble = KUiGamble::GetIfVisible();
		if (pGamble)
			pGamble->UpdateOperData();
		break;
	case GDCNI_GAMBLE_END:
		pGamble = KUiGamble::GetIfVisible();
		if (pGamble)
			pGamble->UpdateTradeEnd(nParam);
		break;
	case GDCNI_NPC_TRADE:
		if (nParam)
		{
			KUiStatus::OpenWindow(); 	//open F3 khi mo shop edit by phong kieu
			KUiShop::OpenWindow();
		}
		else
			KUiShop::CloseWindow();
		break;
	case GDCNI_NPC_TRADE_ITEM:
	{
		KUiShop* pShop = KUiShop::GetIfVisible();
		if (pShop)
			pShop->UpdateData();
	}
	break;
	case GDCNI_QUESTION_CHOOSE:
	{
		if (nParam)
		{
			KUiMsgSel2::OpenWindow((KUiQuestionAndAnswer*)uParam, (KUiNpcSpr*)nParam);
		}
		else
		{
			KUiMsgSel::OpenWindow((KUiQuestionAndAnswer*)uParam);
		}
	}
	break;
	case GDCNI_QUESTION_CHOOSE_3:
	{
		if (uParam && nParam)
		{
			KUiMsgSel3::OpenWindow((KUiQuestionAndAnswer*)uParam, nParam);
		}
	}
	break;
	case GDCNI_QUESTION_CHOOSE_4:
	{
		if (uParam && nParam)
		{
			KUiMsgSel4::OpenWindow((KUiQuestionAndAnswer*)uParam, g_Random(nParam));
		}
	}
	break;
	case GDCNI_GAME_START:
	{
		g_LoginLogic.NotifyToStartGame();
		Wnd_GameSpaceHandleInput(true);
		KUiMsgCentrePad::ReleaseActivateChannelAll();
		KUiMsgCentrePad::QueryAllChannel();
		KUiMail_OnGameStart();	// [MAIL 03/09 D4] bieu tuong thu + hop thu cho toi khi vao game
		KUiAuction_OnGameStart();	// [DAUGIA 04/09 A3]
		KUiChienLenh_OnGameStart();	// [CL 04/09 DOT2]
		KUiOptions2::LoadSetting(true, true);//add by phong kiÒu 24/08/2021
	}
	break;
	case GDCNI_SPEAK_WORDS:			//npc
		//uParam = (KUiInformationParam*) pWordDataList Ö¸ÏòKUiInformationParam
		//nParam = pWordDataList KUiInformationParam
		if (uParam && nParam)
			g_UiInformation2.SpeakWords((KUiInformationParam*)uParam, nParam);
		break;
	case GDCNI_INFORMATION:
		if (uParam)
		{
			KUiInformationParam* pInformation = (KUiInformationParam*)uParam;
			KWndWindow* pCaller = pInformation->bNeedConfirmNotify ? ((KWndWindow*)WND_GAMESPACE) : 0;
			UIMessageBox2(pInformation->sInformation, pInformation->nInforLen, pInformation->sConfirmText, pCaller, 0);
		}
		break;
	case GDCNI_CHAT_GROUP:
		KUiChatCentre::UpdateData(UICC_U_ALL, 0, 0);
		break;
	case GDCNI_CHAT_FRIEND:
		KUiChatCentre::UpdateData(UICC_U_GROUP, 0, nParam);
		break;
	case GDCNI_CHAT_FRIEND_STATUS:
		KUiChatCentre::UpdateData(UICC_U_FRIEND, uParam, nParam);
		break;
	case GDCNI_TEAM:
	{
		KUiTeamManage* pPad = KUiTeamManage::GetIfVisible();
		if (pPad)
			pPad->UpdateData((KUiPlayerTeam*)uParam);
		if (uParam)
			KUiMsgCentrePad::QueryAllChannel();
		else
			KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Team);
	}
	break;
	//	case GDCNI_TEAM_NEARBY_LIST:
	//		KUiTeamManage::UpdateNearbyTeams((KUiTeamItem*)uParam, nParam);
	//		break;
	case GDCNI_SWITCH_CURSOR:
		Wnd_SwitchCursor(nParam);
		break;
	case GDCNI_SYSTEM_MESSAGE:
		if (uParam)
			KUiSysMsgCentre::AMessageArrival((KSystemMessage*)uParam, (void*)nParam);
		break;
	case GDCNI_OPEN_STORE_BOX:
		//KUiStatus::OpenWindow(); 	//open F3 khi më r­¬ng edit by Fong KiÒu
		KUiStoreBox::OpenWindow();
		KUiItem::OpenWindow();
		break;
	case GDCNI_OPEN_RESET_PASS:
		KUiResetPass::OpenWindow();
		break;
	case GDCNI_S2C_EXIT_GAME:
	{
		g_pCoreShell->OperationRequest(GOI_EXIT_GAME, 0, 0);
		g_LoginLogic.ReturnToIdle();
		UiEndGame();
		KUiConnectInfo::CloseWindow(true);
		KUiInit::OpenWindow(true, false);
	}
	break;
	case GDCNI_SWITCHING_SCENEPLACE:
		break;
	case GDCNI_MISSION_RECORD:
		if (uParam)
		{
			KMissionRecord* pRecord = (KMissionRecord*)uParam;
			KUiTaskNote::WakeUp(pRecord->sContent, pRecord->nContentLen, pRecord->uValue);
		}
		break;
	case GDCNI_TASK_VALUE_UPDATE:	// [TaskGuide] task value ve tu server
		KUiTaskGuide::AutoTraceOnTask((int)uParam);	// [C35] nhan nhiem vu -> tu theo doi
		KUiTaskGuide::OnTaskValueChanged((int)uParam);
		KUiTaskTrace::OnTaskValueChanged((int)uParam);
		UiPartner_OnTaskValueChanged((int)uParam);	// [BDH-G4]
		KUiPet::OnTaskValueChanged((int)uParam);	// [PETSYS]
		break;
	case GDCNI_PK_SETTING:
		break;
	case GDCNI_VIEW_PLAYERITEM:
		KUiParadeItem::OpenWindow((KUiPlayerItem*)uParam);
		break;
	case GDCNI_PLAYER_BRIEF_PROP:
		GameWorldTips(uParam, nParam);
		break;
	case GDCNI_NEWS_MESSAGE:
		if (uParam)
			KUiNewsMessage::MessageArrival((KNewsMessage*)uParam, (SYSTEMTIME*)nParam);
		break;
	case GDCNI_NEWS_MESSAGE_1:
		if (uParam)
			KUiNewsMessage1::MessageArrival1((KNewsMessage1*)uParam, (SYSTEMTIME*)nParam);
		break;
	case GDCNII_RANK_INDEX_LIST_ARRIVE:
		KUiStrengthRank::OpenWindow();
		KUiStrengthRank::NewIndexArrive(uParam, (KRankIndex*)nParam);
		break;
	case GDCNII_RANK_INFORMATION_ARRIVE:
		KUiStrengthRank::NewRankArrive(uParam, (KRankMessage*)nParam);
		break;
	case GDCNI_TONG_JX2:	// JX2 port
		KUiTongJX2::DataArrive((unsigned char*)uParam, nParam);
		break;
	case GDCNI_TONG_INFO:
		KUiTongManager::TongInfoArrive((KUiPlayerRelationWithOther*)uParam, (KTongInfo*)nParam);
		break;
	case GDCNI_TONG_MEMBER_LIST:
		KUiTongManager::NewDataArrive((KUiGameObjectWithName*)uParam, (KTongMemberItem*)nParam);
		break;
	case GDCNI_TONG_ACTION_RESULT:
		KUiTongManager::ResponseResult((KUiGameObjectWithName*)uParam, nParam);
		break;
	case GDCNI_OPEN_TONG_CREATE_SHEET:
		if (uParam)
			KUiTongCreateSheet::OpenWindow();
		else
			KUiTongCreateSheet::CloseWindow();
		break;
	case GDCNI_OPEN_EX_BOX:
		KUiExBox1::OpenWindow();
		break;
	case GDCNI_OPEN_EX_BOX2:
		KUiExBox2::OpenWindow();
		KUiExBox1::CloseWindow();
		KUiExBox3::CloseWindow();
		break;
	case GDCNI_OPEN_EX_BOX3:
		KUiExBox3::OpenWindow();
		KUiExBox2::CloseWindow();
		KUiExBox1::CloseWindow();
		break;
	case GDCNI_OPEN_ITEMEX:
		KUiItemEX::OpenWindow();
		break;
	case GDCNI_OPEN_DATAU_BOX:
		//KUiDaTau::OpenWindow();
		break;
	case GDCNI_OPEN_DATAU_BOX1:
		//KUiDaTau1::OpenWindow();
		break;
	case GDCNI_FINISH_QUEST_DLG:
		//KUiFinishQuest::OpenWindow((char *)uParam, nParam);
		if (nParam <= 4)
			KUiDaTau::OpenWindow((char*)uParam, nParam);
		else
			KUiDaTau1::OpenWindow((char*)uParam, nParam);
		break;
	case GDCNI_DICE_ITEM:	// DICEITEM 26/08: uParam = DICE_ITEM_SYNC*
		if (uParam)
		{
			KUiDiceItem::OnDiceMsg((void*)uParam);
		}
		break;
	case GDCNI_MAIL_UI:	// [MAIL 03/09 D2] uParam = MAILUI_CMD_*, nParam = con tro / so (song trong loi goi)
		KUiMail_OnCoreCmd(uParam, nParam);
		break;
	case GDCNI_AUCTION_UI:	// [DAUGIA 04/09 A3] uParam = AUCUI_CMD_*, nParam = con tro / so (song trong loi goi)
		KUiAuction_OnCoreCmd(uParam, nParam);
		break;
	case GDCNI_CHIENLENH_UI:		// [CL 04/09 DOT2]
		KUiChienLenh_OnCoreCmd(uParam, nParam);
		break;
	case GDCNI_OPEN_TREMBLE_ITEM:
		if (uParam > 0)
		{
			KUiTrembleItem::OpenWindow();
		}
		else
		{
			KUiTrembleItem::CloseWindow(true);
		}
		break;
	case GDCNI_OPEN_COMPOUND_ITEM:
		KUiCompoundItem::OpenWindow();
		break;
	case GDCNI_PLAY_SOUND:
		if (uParam)
		{
			UiSoundPlay((char*)uParam);
		}
		break;
	case GDCNI_AUTO_SET_HOTKEY:
	{
		if (uParam >= 0)
			KUiSkillTree::HandleShortcutKey(uParam);
	}
	case GDCNI_AUTO_SET_HOTKEY_DR:
	{
		if (uParam >= 0)
			KUiSkillTree::DirectHandleShortcutKey(uParam);
	}
	break;
	case GDCNI_AUTO_HOTKEY_CAST_B:
	{
		if (uParam >= 0 && nParam > 0)
			KUiSkillTree::DirectHandleShortcutKeyCastB(uParam, nParam);
	}
	break;
	case GDCNI_USE_SHORCUT_SKILL:
	{
		if (uParam >= 0)
			KUiSkillTree::DirectSkillShortcutKey(uParam);
	}
	break;
	case GDCNI_FK_AUTO_ITEM:
	{
		if (nParam == 0 && uParam)	//b¸n
			KUiItem::FkAutoSellItem(uParam);
		else if (nParam == 1 && uParam) //söa
			KUiItem::FkAutoRepairItem(uParam);
		else if (nParam == 2 && uParam) //mua
			KUiShop::FkAutoOnBuyItem(uParam);
		else if (nParam == -1)	//®ãng shop
		{
			if (KUiItem::GetIfVisible()) KUiItem::CloseWindow(true);
			if (KUiShop::GetIfVisible()) KUiShop::CloseWindow();
			if (KUiStatus::GetIfVisible()) KUiStatus::CloseWindow(true);
		}
		else if (nParam == -2) // ®ãng r­¬ng
		{
			if (KUiItem::GetIfVisible()) KUiItem::CloseWindow(true);
			if (KUiStoreBox::GetIfVisible()) KUiStoreBox::CloseWindow();
		}
	}
	break;
	case GDCNI_FK_AUTO_SELECTUI:
	{
		if (nParam == 0) //tù ®éng bÊm chän c¸c « ®èi tho¹i mua thuèc, mua thæ ®Þa phï, ®i xa phu
		{
			if (uParam >= 0) KUiMsgSel::OnClickAutoMsg(uParam);
		}
		else if (nParam == 1) // tù ®éng bÊm vÒ thµnh d­ìng søc
		{
			if (uParam >= 0) g_UiInformation.FkAutoHideClickBtn(uParam);
		}
	}
	break;
	case GDCNI_FK_AUTO_TALK:
	{
		if (uParam)
		{
			char* strMessage = (char*)uParam;
			int nLen = strlen(strMessage);
			//
			DWORD nChannelID = -1;
			int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
			int n = 0;
			for (n = 0; n < nChannelDataCount; n++)
			{
				if (KUiMsgCentrePad::IsChannelType(n, KUiMsgCentrePad::ch_Screen))
				{
					nChannelID = KUiMsgCentrePad::GetChannelID(n);
					break;
				}
			}
			//
			if (nChannelID != -1)
			{
				if (KUiPlayerBar::IsCanSendMessage(strMessage, nLen, KUiMsgCentrePad::GetChannelTitle(KUiMsgCentrePad::GetChannelIndex(nChannelID)), nChannelID))
				{
					char Buffer[256];
					nLen = KUiFaceSelector::ConvertFaceText(Buffer, strMessage, nLen);
					nLen = TEncodeText(Buffer, nLen);
					KUiMsgCentrePad::CheckChannel(n, true);
					KUiPlayerBar::OnSendChannelMessage(nChannelID, Buffer, nLen);
				}
			}
		}
	}
	break;
	case GDCNI_VIEW_PLAYERSELLITEM:
		KUiPlayerShop::OpenWindow((KUiPlayerItem*)uParam);
		break;
	case GDCNI_VIEW_PLAYERUPDATEITEM:
	{
		KUiPlayerShop* pShop = KUiPlayerShop::GetIfVisible();
		if (pShop)
		{
			pShop->UpdateItem();
		}
	}
	break;
	case GDCNI_CLOSE_BAITAN:
	{
		KUiPlayerShop* pShop = KUiPlayerShop::GetIfVisible();
		if (pShop)
		{
			pShop->CloseWindow();
		}
	}
	break;
	case GDCNI_UPDATE_BATTLE_BOX:
		if (nParam)
			KUiBattleReport::UpdateRankWorld((char*)uParam, nParam);
		else
			KUiBattleReport::OpenWindow();
		break;
	case GDCNI_OPEN_AFFAIR_BOX:
	{
		KUiGiveBox* pInfo = (KUiGiveBox*)uParam;;
		KUiAffairItem::OpenWindow(pInfo->szTitle, pInfo->szInitString, pInfo->szAction1);
	}
	break;
	case GDCNI_END_AFFAIR_BOX:
		if (KUiAffairItem::GetIfVisible())
			KUiAffairItem::CloseWindow(false);
		if (KUiMantleInlay::GetIfVisible())
			KUiMantleInlay::CloseWindow(false);
		if (KUiMantleWash::GetIfVisible())
			KUiMantleWash::CloseWindow(false);
		if (KUiSmelt::GetIfVisible())
			KUiSmelt::CloseWindow(false);	// [DUNGLUYEN 01/09]
		break;
	case GDCNI_OPEN_MANTLE_INLAY:
	{
		// [PHI PHONG] panel kham Tinh Than Thach
		KUiGiveBox* pInfo = (KUiGiveBox*)uParam;
		KUiMantleInlay::OpenWindow(pInfo->szTitle, pInfo->szInitString, pInfo->szAction1);
	}
	break;
	case GDCNI_OPEN_MANTLE_WASH:
	{
		// [PF13 01/09] panel Tay Luyen thuoc tinh an
		KUiGiveBox* pInfo = (KUiGiveBox*)uParam;
		KUiMantleWash::OpenWindow(pInfo->szTitle, pInfo->szInitString, pInfo->szAction1);
	}
	break;
	case GDCNI_OPEN_SMELT_BOX:
	{
		// [DUNGLUYEN 01/09] box dung luyen Van Cuong (2 the)
		KUiGiveBox* pInfo = (KUiGiveBox*)uParam;
		KUiSmelt::OpenWindow(pInfo->szTitle, pInfo->szInitString, pInfo->szAction1);
	}
	break;
	case GDCNI_OPEN_TALK_EX:
		KUiInformation3::OpenWindow((char*)uParam, nParam);
		break;
	case GDCNI_OPEN_INPUT:
		KUiGetString2::OpenWindow((char*)uParam, (char*)nParam);
		break;
	case GDCNI_OPEN_INPUT2:
		KUiGetNumber::OpenWindow((char*)uParam, (char*)nParam);
		break;
	case GDCNI_PLAYER_LOGIN_REPLAY: //fix by phong kiÒu chuyÓn gs bÞ mÊt skill
		if (uParam)
		{
			UiOnGameServerStartSyncEnd(true);
		}
		break;
	case GDCNI_RETURN_CITY_OWN_TONG:
	{
		KUiWorldmap* worldmap = KUiWorldmap::GetIfVisible();
		if (worldmap)
		{
			worldmap->SetCityOwnTong((char*)nParam);
		}
	}
	break;
	case GDCNI_CITY_INFO_UPDATE:	// [CITYINFO 21/08] server day 1 thanh -> ve lai ban do + khung thue
	{
		KUiWorldmap* worldmap = KUiWorldmap::GetIfVisible();
		if (worldmap)
			worldmap->RefreshCityLabels();
		KUiMiniMap::UpdateCityInfo();
	}
	break;
	case GDCNI_SWITCHING_MAPMODE:
		if (uParam)
			MapSetMode(MINIMAP_M_BRIEF_PIC);
		else
			MapSetMode(MINIMAP_M_BRIEF_NOT_PIC);
		break;
	case GDCNI_OPEN_TIME_BOX:
	{
		KUiTimeBoxInfo* pInfo = (KUiTimeBoxInfo*)uParam;
		if (pInfo->nTime == -1)
		{
			if (KUiTimeBox::GetIfVisible())
			{
				KUiTimeBox::CloseWindow(false);
				char szInfo[256];
				int n = sprintf(szInfo, "BÞ gi¸n ®o¹n !!!");
				KUiMsgCentrePad::SystemMessageArrival(szInfo, n);
			}
		}
		else if (pInfo->nTime > 0)
		{
			KUiTimeBox::OpenWindow(pInfo->szTitle, pInfo->nTime, pInfo->szAction);
		}
	}
	break;
	case GDCNI_CHATROOM_UPDATE_INTERFACE:
		//#chua hoan thien
		break;
	case GDCNI_SUPERSHOP:
	{
		if (nParam) //#type shop = 1 shop dynamic 0 lµ kú tr©n c¸c chuÈn VNG
		{
			if (!KUiDynamicShop::GetIfVisible())
				KUiDynamicShop::OpenWindow((BuySellInfo*)uParam);
		}
		else
		{
			if (KUiSuperShop::GetIfVisible())
				KUiSuperShop::UpdateShop((BuySellInfo*)uParam);
			else
				KUiSuperShop::OpenWindow((BuySellInfo*)uParam);
		}
	}
	break;
	case GDCNI_EXIT_GAME:
		KUiMail_OnGameExit();	// [MAIL 03/09 D4] don hop thu + xoa danh sach trong state Lua truoc khi thoat
		KUiAuction_OnGameExit();	// [DAUGIA 04/09 A3]
		KUiChienLenh_OnGameExit();	// [CL 04/09 DOT2]
		if (g_pCoreShell)
		{
			g_pCoreShell->OperationRequest(GOI_EXIT_GAME, 0, 0);
		}

		//g_LoginLogic.ReturnToIdle();
		UiEndGame();
		KUiInit::OpenWindow(true, false);
		break;
	//case GDCNI_GIVE:
	//	if (uParam && nParam)
	//		KUiGive::OpenWindow((char*)uParam, (char*)nParam);
	//	else
	//	{
	//		if (KUiGive::GetIfVisible())
	//			KUiGive::CloseWindow(true);
	//	}
	//	break;
	case GDCNI_RANKDATA:
		KUiRankData::OpenWindow();
		break;
	case GDCNI_MSG_ARRIVAL:

		break;
	case GDCNI_PLAYER_BAUCUA_RESULT_SYNC:
		if (nParam) {
			BAUCUA_RESULT_SYNC* pBauCuaResultSync = (BAUCUA_RESULT_SYNC*)uParam;
			if (pBauCuaResultSync->nResultType == BAUCUA_RESULT_DEPOSIT) {
				if (pBauCuaResultSync->nResultValue == -1) //deposit failed
				{
					char szInfo[256];
					int n = sprintf(szInfo, "N¹p xu kh«ng thµnh c«ng.");
					KUiMsgCentrePad::SystemMessageArrival(szInfo, n);
				}
				else {
					char szInfo[256];
					int n = sprintf(szInfo, "N¹p thµnh c«ng %d xu.", pBauCuaResultSync->nResultValue);
					KUiMsgCentrePad::SystemMessageArrival(szInfo, n);
				}
			}
			else if (pBauCuaResultSync->nResultType == BAUCUA_RESULT_INFO) {
				BAUCUA_INFO_SYNC* pBauCuaInfoSync = (BAUCUA_INFO_SYNC*)uParam;
				KUiSpringGame* pBauCua = KUiSpringGame::GetIfVisible();
				if (pBauCua) {
					pBauCua->UpdateInfo(pBauCuaInfoSync->m_Status);
				}
			}
		}
		break;
	case GDCNI_UI_ACT:
		{
			// (25/08) PHAI biet CA HAI lop hop thoai.
			// Thoai co the tag <link=image[...]> trong van ban (vi du
			// DescLink_NieShiChen o newtask_head.lua:13 - MOI cau cua NPC Nhiep Thi
			// Tran deu co) thi KPlayer.cpp:7785 mo bang KUiMsgSel2 (hop CO ANH),
			// khong phai KUiMsgSel - xem GameSpaceChangedNotify.cpp:397.
			// Truoc day o day chi biet KUiMsgSel nen lenh dong cua auto bay vao lop
			// khac: khung Nhiep Thi Tran khong ai dong, va cau hoi "con hien khong"
			// luon tra 0 nen auto tuong da dong roi. Cau tra loi van gui duoc
			// (DT_Answer goi thang OnSelectFromUI) nen nhiem vu VAN NHAN DUOC, chi
			// moi cai khung la treo - dung canh chu game bao 25/08.
			if(uParam == 0)
			{
				nRet = (KUiMsgSel::GetIfVisible() || KUiMsgSel2::GetIfVisible())?1:0;
			}
			else if(uParam == 1)
			{
				KUiMsgSel::CloseWindow(false);
				KUiMsgSel2::CloseWindow(false);
				g_UiInformation2.Close();
			}
			else if(uParam == 2)
			{
				nRet = KUiShop::GetIfVisible()?1:0;
			}
			else if(uParam == 3)
			{
				KUiItem::CloseWindow(false);
				KUiStatus::CloseWindow(false);
				KUiShop::CloseWindow();
			}
			else if(uParam == 4)
			{
				nRet = KUiMsgSel::GetAnswerCount();
			}
			else if(uParam == 5)
			{
				KUiMsgSel::SetMsgToGet(nParam);
			}
			else if(uParam == 6)
			{
				KUiMsgSel::GetMsg((char*)nParam);
			}
			else if(uParam == 7)
			{
				// [DaTau] auto bam nut THAT tren cua so ruong thuong dang mo.
				// nParam = nhom*10 + nut: 30..32 = KUiDaTau (exp/tien/ngau nhien),
				// 40..42 = KUiDaTau1 (diem/may man/vat pham). Tra 1 = da bam;
				// 0 = cua so nhom do khong mo - khi do an not cua so thua (neu co)
				// de Core gui thang script du phong ma man hinh van sach.
				if (nParam >= 30 && nParam <= 32)
					nRet = KUiDaTau::AutoPick(nParam - 30);
				else if (nParam >= 40 && nParam <= 42)
					nRet = KUiDaTau1::AutoPick(nParam - 40);
				if (nRet == 0)
				{
					KUiDaTau::AutoHide();
					KUiDaTau1::AutoHide();
				}
			}
			break;
		}
	}
	return nRet;
}

//////////////////////////////////////////////////////

int KClientCallback::CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam)
{
	return CoreDataChangedCallback(uDataId, uParam, nParam);
}

void KClientCallback::SendDataToTool(const void * const pData, const size_t &datalength)
{
	SendInfoToTool(pData, datalength);
}

typedef std::map<std::string, std::string> BLACKLIST;
BLACKLIST g_BlackListUserNames;

#define BLACKLIST_UNITNAME	 "Sæ ®en"

struct BlacklistNotify : public AddinNotify
{
	virtual int	RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends);
	virtual int	MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends);
	virtual int	DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends);
	virtual int	DeleteUnitMember(const char* Unit, const char* Name);
	virtual int	MoveUnitMember(const char* Unit, const char* Name, const char* Group);
	void	SendNotifyGroupFriend(const char* Unit, const std::string& group, const STRINGLIST& friends);
	void	SendNotifyDeleteFriend(const char* Unit, const char* Name);
};

BlacklistNotify s_LNotify;

void ClearBlackList()
{
	g_BlackListUserNames.clear();
}

void CreateBlackListUnit()
{
	ClearBlackList();
	KUiChatCentre::AddAddinUnit(BLACKLIST_UNITNAME, &s_LNotify);
}

void AddBlackList(const char* strName, const char* strGroup)
{
	if (strName && strName[0] != 0 && strGroup)
	{
		g_BlackListUserNames[strName] = strGroup;

		if (g_BlackListUserNames.size() > 0)
		{
			int nUnit = KUiChatCentre::FindUnitIndex(BLACKLIST_UNITNAME);
			if (nUnit >= 0)
			{
				KUiChatCentre::AddFriendInfo(nUnit, (char*)strName, (char*)strGroup);
			}
		}
	}
}

void RemoveBlackList(char* strName)
{
	if (strName && strName[0] != 0)
	{
		BLACKLIST::iterator i = g_BlackListUserNames.find(strName);
		if (i != g_BlackListUserNames.end())
		{
			g_BlackListUserNames.erase(i);

			int nUnit = KUiChatCentre::FindUnitIndex(BLACKLIST_UNITNAME);
			if (nUnit >= 0)
			{
				KUiChatCentre::DeleteFriend(nUnit, strName, true);
			}
		}
	}
}

bool IsInBlackName(char* strName)
{
	if (strName && strName[0] != 0)
	{
		return g_BlackListUserNames.find(strName) != g_BlackListUserNames.end();
	}

	return false;
}

int	SaveBlackListPrivateSetting(KIniFile* pFile, LPCSTR lpSection, int nStart)
{
	int i = nStart;
	char szKey[10];
	char szLine[256];
	BLACKLIST::iterator iU = g_BlackListUserNames.begin();
	while (iU != g_BlackListUserNames.end())
	{
		sprintf(szKey, "%d", i);
		sprintf(szLine, "AddBlackList(\"%s\", \"%s\")", iU->first.c_str(), iU->second.c_str());
		pFile->WriteString(lpSection, szKey, szLine);
		i++;
		iU++;
	}
	return i;
}

int	BlacklistNotify::RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends)
{
	AddinNotify::RenameUnitGroup(Unit, Name, NewName, friends);

	if (NewName && NewName[0])
		SendNotifyGroupFriend(Unit, std::string(NewName), friends);

	return 0;
}

int	BlacklistNotify::MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends)
{
	AddinNotify::MoveUnitGroup(Unit, Name, Name2, friends);

	if (Name2 && Name2[0])
		SendNotifyGroupFriend(Unit, std::string(Name2), friends);

	return 0;
}

int	BlacklistNotify::DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends)
{
	AddinNotify::DeleteUnitGroup(Unit, Name, friends);

	if (Name && Name[0])
		SendNotifyGroupFriend(Unit, "", friends);

	return 0;
}

int	BlacklistNotify::DeleteUnitMember(const char* Unit, const char* Name)
{
	AddinNotify::DeleteUnitMember(Unit, Name);

	SendNotifyDeleteFriend(Unit, Name);

	return 0;
}

int	BlacklistNotify::MoveUnitMember(const char* Unit, const char* Name, const char* Group)
{
	AddinNotify::MoveUnitMember(Unit, Name, Group);

	if (Name && Name[0])
	{
		STRINGLIST friends;
		friends.push_back(Name);
		SendNotifyGroupFriend(Unit, Group, friends);
	}

	return 0;
}

void BlacklistNotify::SendNotifyGroupFriend(const char* Unit, const std::string& group, const STRINGLIST& friends)
{
	if (strcmp(Unit, BLACKLIST_UNITNAME) == 0)
	{
		STRINGLIST::const_iterator i = friends.begin();
		while (i != friends.end())
		{
			g_BlackListUserNames[(*i)] = group;
			i++;
		}
	}
}

void BlacklistNotify::SendNotifyDeleteFriend(const char* Unit, const char* Name)
{
	if (strcmp(Unit, BLACKLIST_UNITNAME) == 0)
	{
		RemoveBlackList((char*)Name);
	}
}

///////////////////////////////////////////////////////////////////////////////

void KClientCallback::ChannelMessageArrival(DWORD nChannelID, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, bool bSucc, bool bIsNpcChat, bool bIsShowMsgPad)
{
	if (!bSucc)
	{
		char szInfo[256];
		int n = sprintf(szInfo, "Lçi kh«ng göi ®­îc th«ng tin!");
		KUiMsgCentrePad::SystemMessageArrival(szInfo, n);
		return;
	}

	int nIndex = -1;
	char	Buffer2[1536];
	memset(Buffer2, 0, sizeof(Buffer2));

	if (nChannelID == -1)	//gm alias ID
	{
		int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
		for (int n = 0; n < nChannelDataCount; n++)
		{
			if (KUiMsgCentrePad::IsChannelType(n, KUiMsgCentrePad::ch_GM))
			{
				nIndex = n;
				break;
			}
		}
		if (nIndex < 0)
			return;
		nChannelID = KUiMsgCentrePad::GetChannelID(nIndex);
		if (nChannelID == -1)
			return;
	}
	else if (bIsNpcChat) {
		DWORD ntmpChannelID = -1;
		int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
		int n = 0;
		for (n = 0; n < nChannelDataCount; n++)
		{
			if (KUiMsgCentrePad::IsChannelType(n, KUiMsgCentrePad::ch_Screen))
			{
				ntmpChannelID = KUiMsgCentrePad::GetChannelID(n);
				nIndex = KUiMsgCentrePad::GetChannelIndex(ntmpChannelID);

				nMsgLength = KUiFaceSelector::ConvertFaceText(Buffer2, pMsgBuff, nMsgLength + 1);
				if(bIsShowMsgPad)
					KUiMsgCentrePad::NewChannelMessageArrival(ntmpChannelID, szSendName, Buffer2, nMsgLength);
				break;
			}
		}
	}
	else
	{
		nIndex = KUiMsgCentrePad::GetChannelIndex(nChannelID);

		if (nIndex < 0)
			return;

		if (IsInBlackName(szSendName))
			return;
	}
	if (!bIsNpcChat) {
		KUiMsgCentrePad::NewChannelMessageArrival(nChannelID, szSendName, pMsgBuff, nMsgLength);
	}
	else {
		nMsgLength = TEncodeText(Buffer2, nMsgLength);
	}

	if (KUiMsgCentrePad::GetChannelSubscribe(nIndex) &&
		KUiMsgCentrePad::IsChannelType(nIndex, KUiMsgCentrePad::ch_Screen))
	{
		KUiPlayerItem SelectPlayer;
		int nKind = -1;
		if (g_pCoreShell->FindSpecialNPC(szSendName, &SelectPlayer, nKind) && (nKind == kind_player || bIsNpcChat))
		{
			if(bIsNpcChat)
				g_pCoreShell->ChatSpecialPlayer(&SelectPlayer, Buffer2, nMsgLength);
			else
				g_pCoreShell->ChatSpecialPlayer(&SelectPlayer, pMsgBuff, nMsgLength);
		}
	}
}

void KClientCallback::MSNMessageArrival(char* szSourceName, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, bool bSucc)
{
	if (!bSucc)
	{
		char szInfo[256];
		int n = sprintf(szInfo, "Ng­êi ch¬i, %s kh«ng cã trªn m¹ng!", szSendName);
		KUiMsgCentrePad::SystemMessageArrival(szInfo, n);
		return;
	}

	if (IsInBlackName(szSourceName))
		return;

	KUiMsgCentrePad::NewMSNMessageArrival(szSourceName, szSendName, pMsgBuff, nMsgLength);
	
	//if (!KUiPlayerBar::IsSelfName(szSourceName))	//²»ÊÇ×Ô¼ºËµµÄ·Åµ½Í·¶¥
	//{
	//	KUiPlayerItem SelectPlayer;
	//	int nKind = -1;
	//	if (g_pCoreShell->FindSpecialNPC((char*)KUiPlayerBar::SelfName(), &SelectPlayer, nKind) && nKind == kind_player)
	//	{
	//		strncpy(SelectPlayer.Name, szSourceName, 32);	//ÎªÁËÏÔÊ¾±ðÈËµÄÃû×Ö
	//		g_pCoreShell->ChatSpecialPlayer(&SelectPlayer, pMsgBuff, nMsgLength);
	//	}
	//}
}

void KClientCallback::NotifyChannelID(char* ChannelName, DWORD channelid, BYTE cost)
{
	KUiMsgCentrePad::OpenChannel(ChannelName, channelid, cost);
}

void KClientCallback::FriendInvite(char* roleName)
{
	if (roleName && roleName[0] != 0)
	{
		// Í¨Öª½çÃæÓÐÈËÉêÇëÌí¼ÓÁÄÌìºÃÓÑ
		KSystemMessage	sMsg;
		KUiPlayerItem	sPlayer;

		strcpy(sPlayer.Name, roleName);
		sPlayer.nIndex = -1;
		sPlayer.uId = 0;
		sPlayer.nData = 0;

		sprintf(sMsg.szMessage, MSG_CHAT_GET_FRIEND_APPLY, roleName);
		sMsg.eType = SMT_FRIEND;
		sMsg.byConfirmType = SMCT_UI_FRIEND_INVITE;
		sMsg.byPriority = 3;
		sMsg.byParamSize = sizeof(KUiPlayerItem);
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
	}
}

void KClientCallback::AddFriend(char* roleName, BYTE answer)
{
	if (roleName && roleName[0] != 0)
	{
		if (answer == answerAgree)
		{
			KSystemMessage	sMsg;
			KUiPlayerItem	sPlayer;

			memset(&sPlayer, 0, sizeof(KUiPlayerItem));
			strcpy(sPlayer.Name, roleName);
			sPlayer.uId = 0;
			sPlayer.nIndex = -1;

			sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_SUCCESS, roleName);
			sMsg.eType = SMT_FRIEND;
			sMsg.byConfirmType = SMCT_UI_INTERVIEW;
			sMsg.byPriority = 2;
			sMsg.byParamSize = sizeof(KUiPlayerItem);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);

			int nUnit = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
			if (nUnit >= 0)
			{
				KUiChatCentre::DeleteFriend(nUnit, roleName, false);
			}

			KUiChatCentre::AddFriendInfo(0, roleName, "");
			KUiChatCentre::FriendStatus(0, roleName, stateOnline);
		}
		else if (answer == answerDisagree)
		{
			// Í¨Öª½çÃæpRefuse->m_szName¾Ü¾øÁËËûµÄ½»ÓÑÉêÇë
			KSystemMessage	sMsg;

			sprintf(sMsg.szMessage, MSG_CHAT_REFUSE_FRIEND, roleName);
			sMsg.eType = SMT_FRIEND;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		else if (answer == answerUnable)
		{
			// Í¨Öª½çÃæÌí¼ÓÄ³ÈËÎªºÃÓÑ²Ù×÷Ê§°Ü
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_FAIL, roleName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}
}

void KClientCallback::FriendStatus(char* roleName, BYTE state)
{
	if (roleName && roleName[0] != 0)
	{
		int nUnit = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
		if (nUnit >= 0 && KUiChatCentre::FriendStatus(nUnit, roleName, state))
		{
			// Í¨Öª½çÃæÓÐºÃÓÑÉÏÏß
			KSystemMessage	sMsg;
			if (state == stateOnline)
				sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_ONLINE, roleName);
			else if (state == stateOffline)
				sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_OFFLINE, roleName);
			else
				return;

			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}
}

void KClientCallback::FriendInfo(char* roleName, char* unitName, char* groupName, BYTE state)
{
	if (roleName && roleName[0] != 0 && unitName && groupName)
	{
		int nUnit = 0;
		if (unitName[0])
			nUnit = KUiChatCentre::FindUnitIndex(unitName);
		if (nUnit >= 0)
		{
			int nUnitOld = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
			if (nUnitOld >= 0 && nUnitOld != nUnit)
			{
				KUiChatCentre::DeleteFriend(nUnitOld, roleName, false);
			}

			KUiChatCentre::AddFriendInfo(nUnit, roleName, groupName);
			KUiChatCentre::FriendStatus(nUnit, roleName, state);
		}
	}
}

void KClientCallback::AddPeople(char* unitName, char* roleName)
{
	if (roleName && roleName[0] != 0 && unitName)
	{
		int nUnit = KUiChatCentre::FindUnitIndex(unitName);
		if (nUnit >= 0)
		{
			int nUnitOld = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
			if (nUnitOld >= 0 && nUnitOld != nUnit)
			{
				KUiChatCentre::DeleteFriend(nUnitOld, roleName, false);
			}
			
			KUiChatCentre::AddFriendInfo(nUnit, roleName, "");
			KUiChatCentre::FriendStatus(nUnit, roleName, stateOnline);
		}
	}
}

#define LEVEL_TIPS_INI "\\Ui\\GameIni.ini"

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºµÈ¼¶µÄÌùÊ¿
//--------------------------------------------------------------------------
void LevelTips(int nNewLevel)
{
	char szMsg[256], szBuf[16];
	KIniFile Ini;
	int nMsgLen;

	if(Ini.Load(LEVEL_TIPS_INI))
	{
		Ini.GetString("LevelUpTips", itoa(nNewLevel, szBuf, 10), "", szMsg, sizeof(szMsg));

		nMsgLen = TEncodeText(szMsg, strlen(szMsg));

		if(nMsgLen)
		{
		    KUiMsgCentrePad::SystemMessageArrival(szMsg, nMsgLen);
		}
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÃÅÅÉµÄÌùÊ¿
//--------------------------------------------------------------------------
void FactionTips(int nbJoin)
{
	char szMsg[256] = "";
	KIniFile Ini;
	int nMsgLen;

	if(Ini.Load(LEVEL_TIPS_INI))
	{
		if (nbJoin)
			Ini.GetString("FactionTips", "Join", "", szMsg, sizeof(szMsg));

		nMsgLen = TEncodeText(szMsg, strlen(szMsg));

		if(nMsgLen)
		{
		    KUiMsgCentrePad::SystemMessageArrival(szMsg, nMsgLen);
		}
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÌùÊ¿ÀÏ´ó
//--------------------------------------------------------------------------
void GameWorldTips(unsigned int uParam, int nParam)
{
	if(uParam == PBP_LEVEL)
		LevelTips(nParam);
	else if(uParam == PBP_FACTION)
		FactionTips(nParam);
}
