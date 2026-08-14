// TongDB.h: interface for the CTongDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TONGDB_H__8C487F07_C490_4562_AD01_77953CCCBCE2__INCLUDED_)
#define AFX_TONGDB_H__8C487F07_C490_4562_AD01_77953CCCBCE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DBTable.h"
#include "KTongControl.h"

#define MAX_TONG_COUNT 10000

/*
enum enumMemberClass		//帮众类型
{
	mcMaster = 0,			//帮主
	mcDirector,			//长老
	mcManager,			//长老
	mcMember			//帮众
};
*/


typedef struct
{
	char		szName[defTONG_STR_LENGTH];		// 帮众名
	char		szTong[defTONG_STR_LENGTH];		// 帮会名
	TONG_MEMBER_FIGURE		MemberClass;			//帮众类型
	int			nTitleIndex;					//帮众称号索引
	int			nSex;
	// ==== JX2 port: KV + quyen thanh vien (them o CUOI struct - theo bay-2) ====
	int			nJX2FieldCount;
	WORD		wJX2FieldKey[defTONG_JX2_MEMBER_FIELDS];
	DWORD		dwJX2FieldVal[defTONG_JX2_MEMBER_FIELDS];
	int			nJX2RightCount;
	DWORD		dwJX2Right[defTONG_JX2_MEMBER_RIGHTS];
}TMemberStruct;	//用作存入数据库的结构

typedef struct
{
	char		szTong[defTONG_STR_LENGTH];
	DWORD		m_dwMoney;
}TMoneyStruct;
// Ban ghi don xin vao bang (JX2) - bang TongZhaoMuDB, khoa = ten nguoi
// xin (moi nguoi toi da 1 don), index phu = ten bang de keo ca hang doi.
typedef struct
{
	char	szTong[32];
	char	szName[32];
	unsigned long	dwNameID;
	unsigned short	wLevel;
	unsigned char	btSex;
	unsigned long	dwApplyTime;
} TZhaoMuStruct;

class CTongDB  
{
private:
	ZDBTable* m_TongTable;						//帮会数据表
	ZDBTable* m_MemberTable;					//帮众数据表
	ZDBTable* m_MoneyTable;
	ZDBTable* m_ZhaoMuTable;	// don xin vao bang (JX2)
	
	TTongList m_TongList[MAX_TONG_COUNT];	//帮会列表
	int m_TongListCount;						//帮会列表数
public:
	CTongDB();
	virtual ~CTongDB(){};

	bool Open();
	bool OpenNew();
	void Close();
	
	bool ChangeTong(CTongControl& aTongControl);	//创建和改变帮派
	bool DelTong(char* aTongName);	//删除帮派
	bool SearchTong(char* aTongName, CTongControl& aTongControl);	//从数据库中取得帮派
	int GetTongCount();							//取得门派数
	int GetTongList(TTongStruct* aTongList, int aMaxNum);	//取得帮会列表

	bool ChangeMember(TMemberStruct aMember);//增加帮众
	bool DelMember(char* aMemberName);		//删除帮众
	bool ChangeMoney(TMoneyStruct aMoney);
	bool DelMoney(char* aTongName);

	bool UpdateExtPoint(char* szAccount, int nExtPoint);

	// ==== hang doi don xin vao bang (JX2) ====
	bool ChangeZhaoMu(const TZhaoMuStruct* pRec);
	bool DelZhaoMu(const char* szMemberName);
	int  GetZhaoMuList(const char* szTong, TZhaoMuStruct* pOut, int nMax);

	// ==== JX2 port ====
	void JX2_PreserveMemberTail(TMemberStruct* pMember);
	bool JX2_UpdateMemberTail(const char* szName, const void* pJX2Data);
	bool JX2_UpdateMemberClass(const char* szName, int nClass, int nTitleIdx);
	int	 GetExtPoint(char* szAccount);
};

#endif // !defined(AFX_TONGDB_H__8C487F07_C490_4562_AD01_77953CCCBCE2__INCLUDED_)
