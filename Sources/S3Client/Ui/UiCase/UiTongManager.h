/*******************************************************************************
File        : UiTongManager.h
Creator     : Fong KiÒu
create data : 08-29-2020(mm-dd-yyyy)
Description : 
********************************************************************************/

#if !defined(AFX_UITONGMANAGER_H__13BA213D_11EC_4F24_BF98_F51C3F414D6D__INCLUDED_)
#define AFX_UITONGMANAGER_H__13BA213D_11EC_4F24_BF98_F51C3F414D6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndlabeledbutton.h"
#include "../elem/wndscrollbar.h"
#include "../elem/wndimage.h"
#include "../elem/wndlist2.h"

#include "../../Engine/Src/LinkStruct.h"
#include "../../Core/Src/GameDataDef.h"

struct KPilgarlicItem : public KTongMemberItem
{
	int nHashID;
	KPilgarlicItem *next;
    KPilgarlicItem *previous;
};

class KUiTongManager : KWndImage
{
public:
	KUiTongManager();
	virtual ~KUiTongManager();
	static        KUiTongManager* OpenWindow();      
	static        KUiTongManager* OpenWindow(char* pszPlayerName);
	static        KUiTongManager* GetIfVisible();    
	static void   CloseWindow(bool bDestory = TRUE); 
	static void   LoadScheme(const char* pScheme);   
	static void   NewDataArrive(KUiGameObjectWithName *ParamInfo, KTongMemberItem *pIncome);
	static void   TongInfoArrive(KUiPlayerRelationWithOther* Relation, KTongInfo *pTongInfo);
	static int    RemoveTempFile();                 
	static void   ResponseResult(KUiGameObjectWithName *pResult, int nbIsSucceed);
public:
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void          ArrangeComposition(char* pszPlayerName);
private:
	void          Initialize();
	void          PopupConfirmWindow(const char* pszInfo, unsigned int uHandleID);
	static void   RequestData(int nType, int nIndex);
	static void   SaveNewData(KUiGameObjectWithName *ParamInfo, KTongMemberItem *pIncome);
	static void   SaveWholeData(int nType);         
	static int    IsPlayerExist(KPilgarlicItem *MeToCmp, KLinkStruct<KPilgarlicItem> *pData);
	KLinkStruct<KPilgarlicItem>* FindPlayer(char *pszName);
	KLinkStruct<KPilgarlicItem>* HandleFind(KLinkStruct<KPilgarlicItem> *pData, char *pszName);
	void          LoadData(int nType);               
	int           LoadDataHandler(KLinkStruct<KPilgarlicItem> *pData, const char* szSection);
	void          UpdateBtnTheme(int nType, BOOL IsDissable = FALSE);
	KLinkStruct<KPilgarlicItem>* GetCurrentSelectedData();
	KPilgarlicItem* GetSelectedPlayer();
	void          UpdateTongInfoView();              
	void          UpdateTongListView();              
	void          ClearTongData();                   
	void          UpdateListCheckButton(int nType);  
	void          OnAction(int nType);              
	void          OnRefresh();                       
	void          OnCheckOnline(); 
	void          ResponseLeave(                     
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);
	void          ResponseDismiss(                   
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);
	void          ResponseDemise(                    
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);
	void          ResponseAssign(                    
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);
	void          ResponseApply(                     
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);
	void		  PaintWindow();
	void		  OnMoney(int nType, int nMoney);
private:
	static        KUiTongManager* ms_pSelf;
	static int    m_nElderDataIndex;
	static int    m_nMemberDataIndex;
	static int    m_nCaptainDataIndex;
private:
	enum RESULT_TYPE
	{
		RESULT_T_DISMISS,
		RESULT_T_ASSIGN,
		RESULT_T_DEMISE,
		RESULT_T_LEAVE,
		RESULT_T_PASSWORD_TO_DEMISE,
		RESULT_T_SAVE_MONEY,
		RESULT_T_GET_MONEY,
		RESULT_T_SND_MONEY,
	};
private:
	KWndLabeledButton                               
		          m_BtnDismiss, m_BtnAssign, m_BtnDemise;
	KWndLabeledButton
		          m_BtnLeave, m_BtnRecruit;          
	KWndLabeledButton
		          m_BtnApply, m_BtnCancel;           
	KWndLabeledButton m_BtnElderList, m_BtnCaptainList, m_BtnMemberList, m_BtnLeagueList;
	KWndButton    m_BtnClose, m_BtnRefresh;
	KWndText32    m_TongName, m_MasterName;        
	KWndText32    m_Alignment, m_TongLevel, m_TongLeague;       //lien minh                
	KWndText32    m_MemberCount, m_Money;            
	KWndList2     m_List;                            
	KWndList2     m_List2;
	KWndScrollBar m_ListScroll;                      
	TONG_MEMBER_FIGURE m_Relation;                        
	KWndImage	m_TongBg;
	KWndText32	m_MoneyGuide, m_AligGuide, m_MemberText, m_RankNameText;
	KWndLabeledButton m_OnlineBtn, m_Next, m_GetMoneyBtn, m_DistriMoneyBtn, m_DonateMoneyBtn, m_FaceBtn, m_HealBtn, m_CenterBtn, m_UpdateInfoBtn;
	KWndText32	m_InfoGuide, m_StatusGuide, m_WayGuide, m_NextTarget, m_ExpGuide, m_CityGuide;
	KWndText80	m_StatusText, m_ExpText, m_CityText;
	KWndEdit512	m_WayEdit;
	KWndEdit512	m_NextTargetEdit;
	KWndButton	m_ChangeTitle, m_ChangeMaleTitle, m_ChangeFemaleTitle;
	BOOL		IsOnline;
private:
	KLinkStruct<KPilgarlicItem> m_MemberData, m_CaptainData, m_ElderData;
	static KTongInfo            m_TongData;          
private:
	unsigned int  m_uLastRequestTongDataTime;        
	unsigned int  m_uLastRequestElderListTime;       
	unsigned int  m_uLastRequestMemberListTime;      
	unsigned int  m_uLastRequestCaptainListTime;    
	int           m_nRelation;                       
	int           m_nIndex;                          
	char          m_szPlayerName[32];                
	int           m_nSelectIndex;                    
	int           m_nCurrentCheckBox;                
	char          m_szDismiss[64], m_szAssign[64];   
	char          m_szDemise[64], m_szLeave[64];     
	char          m_szJustice[16], m_szBalance[16];  
	char          m_szEvil[16];                      
	char          m_szConfirm[16], m_szCancel[16];   
	char          m_szPassword[32];                  
	char          m_szTargetPlayerName[32];          
};
unsigned long StringToHash(const char *pString, BOOL bIsCaseSensitive = TRUE);
#endif // !defined(AFX_UITONGMANAGER_H__13BA213D_11EC_4F24_BF98_F51C3F414D6D__INCLUDED_)
