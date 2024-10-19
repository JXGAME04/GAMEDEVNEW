// TongDB.cpp: implementation of the CTongDB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "db.h"
#include "DBTable.h"
#include "TongDB.h"
#include "S3Relay.h"
#include "COMUtil.h"

CTongDB		g_cTongDB;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int get_TongName(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey) 
{

	memset( ikey, 0, sizeof( DBT ) );

	TMemberStruct* pMemberStruct = (TMemberStruct*)pdata->data;
	ikey->data = pMemberStruct->szTong;
	ikey->size = strlen( pMemberStruct->szTong ) + 1;
	return 0;
}

CTongDB::CTongDB()
{
	m_TongListCount = 0;
	memset(m_TongList,0,sizeof(TTongList) * MAX_TONG_COUNT);
}

bool CTongDB::Open()
{
	m_TongTable = new ZDBTable("TongDB","TongTable");
	if(!m_TongTable->open())
	return false;

	m_MemberTable = new ZDBTable("TongMemberDB","MemberTable");
	m_MemberTable->addIndex(get_TongName);
	if(!m_MemberTable->open())
	{
		m_MemberTable->close();
		return false;
	}
	m_MoneyTable = new ZDBTable("TongMoneyDB","MoneyTable");
	if(!m_MoneyTable->open())
	{
		m_MoneyTable->close();
		return false;
	}
	return true;
}

bool CTongDB::OpenNew()
{
	//DeleteFile("TongDB");
	//DeleteFile("TongMemberDB");
	m_TongTable = new ZDBTable("TongDB","TongTable");		//Open the gang table
	if(!m_TongTable->open())
		return false;
	
	m_MemberTable = new ZDBTable("TongMemberDB","MemberTable");	//Gang member table
	m_MemberTable->addIndex(get_TongName);
	if(!m_MemberTable->open())
	{
		m_MemberTable->close();
		return false;
	}
	m_MoneyTable = new ZDBTable("TongMoneyDB","MoneyTable");
	if(!m_MoneyTable->open())
	{
		m_MoneyTable->close();
		return false;
	}
	return true;
}

void CTongDB::Close()
{
	if (m_TongTable) {
		m_TongTable->commit();
		m_TongTable->close();
	}
	if (m_MemberTable) {
		m_MemberTable->commit();
		m_MemberTable->close();
	}
	if (m_MoneyTable) {
		m_MoneyTable->commit();
		m_MoneyTable->close();
	}
	delete m_TongTable;
	delete m_MemberTable;
	delete m_MoneyTable;
}

bool CTongDB::ChangeTong(CTongControl& aTongControl)
{//Create and change gangs
	bool aResult = true;
	int aKeySize = strlen(aTongControl.m_szName) + 1;
	int aSize = sizeof(TTongStruct);
	TTongStruct aTongStruct;
	memset(&aTongStruct, 0, aSize);
	aTongStruct.nCamp = aTongControl.m_nCamp;
	aTongStruct.dwMoney = aTongControl.m_dwMoney;
	aTongStruct.nCredit = aTongControl.m_nCredit;	//tÝn dông
	aTongStruct.nLevel = aTongControl.m_nLevel;	//®¼ng cÊp
	aTongStruct.nRecruit = aTongControl.m_nRecruit; //tr¹ng th¸i bang héi tuyÓn dông
	//======== add by Fong Kieu ========
	strcpy(aTongStruct.szWayEdit, aTongControl.m_szWayEdit);	//tiªu ch
	strcpy(aTongStruct.szNextTargetEdit, aTongControl.m_szNextTargetEdit); //môc tiªu
	aTongStruct.nExpGuide = aTongControl.m_nExpGuide; //®iÓm
	aTongStruct.nCityGuide = aTongControl.m_nCityGuide; //thµnh chiÕm lÜnh
	strcpy(aTongStruct.szLeagueTName, aTongControl.m_szLeagueTName); //liªn minh
	//======== end add by Fong Kieu ========
	strcpy(aTongStruct.szName, aTongControl.m_szName);
	strcpy(aTongStruct.szMasterName, aTongControl.m_szMasterName);
	strcpy(aTongStruct.szMasterTitle, aTongControl.m_szMasterTitle);
	strcpy(aTongStruct.szNormalTitle, aTongControl.m_szNormalTitle);
	strcpy(aTongStruct.szNormalBoyTitle, aTongControl.m_szNormalBoyTitle);
	strcpy(aTongStruct.szNormalGirlTitle, aTongControl.m_szNormalGirlTitle);
	int i;
	for(i=0;i<defTONG_MAX_DIRECTOR;++i)
	{
		strcpy(aTongStruct.szDirectorTitle[i], aTongControl.m_szDirectorTitle[i]);
	}
	for(i=0;i<defTONG_MAX_MANAGER;++i)
	{
		strcpy(aTongStruct.szManagerTitle[i], aTongControl.m_szManagerTitle[i]);
	}
	aResult = m_TongTable->add(aTongControl.m_szName, aKeySize,(char*)(&aTongStruct), aSize);
	
	return aResult;
}

bool CTongDB::DelTong(char* aTongName)
{//Delete gang
	bool aResult = true;
	int aKeySize = strlen(aTongName) + 1;

	aResult = m_TongTable->remove(aTongName, aKeySize);
	if(!aResult) return aResult;

	aResult = m_MoneyTable->remove(aTongName, aKeySize);
	if(!aResult) return aResult;
	while(!m_MemberTable->remove(aTongName, aKeySize, 0))
		{}
	return true;
}

bool CTongDB::SearchTong(char* aTongName, CTongControl& aTongControl)
{//Get gang from database
	bool aResult = true;
	int aSize = sizeof(TTongStruct);
	int aKeySize = strlen(aTongName) + 1;
	char* aBuffer;
	ZCursor* cursor = m_TongTable->search(aTongName, aKeySize);

	if (cursor)
	{
		aBuffer = cursor->data;
	}
	else
	{
		return false;	//Can't find gang
	}

	//if(!aBuffer) 
	//	return false;	//Can't find gang

	TTongStruct* pTongStruct = (TTongStruct*)aBuffer;
	aTongControl.m_nCamp = pTongStruct->nCamp;
	aTongControl.m_nRecruit = pTongStruct->nRecruit; // tuyÓn dông
	strcpy(aTongControl.m_szName, pTongStruct->szName);
	strcpy(aTongControl.m_szPassword, pTongStruct->szPassword);
	strcpy(aTongControl.m_szMasterTitle, pTongStruct->szMasterTitle);
	strcpy(aTongControl.m_szNormalTitle, pTongStruct->szNormalTitle);
	strcpy(aTongControl.m_szNormalBoyTitle, pTongStruct->szNormalBoyTitle);
	strcpy(aTongControl.m_szNormalGirlTitle, pTongStruct->szNormalGirlTitle);
	aTongControl.m_nLevel = pTongStruct->nLevel; // ==== add by Fong Kieu ===
	strcpy(aTongControl.m_szWayEdit, pTongStruct->szWayEdit);
	strcpy(aTongControl.m_szNextTargetEdit, pTongStruct->szNextTargetEdit);
	aTongControl.m_nExpGuide = pTongStruct->nExpGuide;
	aTongControl.m_nCityGuide = pTongStruct->nCityGuide;
	strcpy(aTongControl.m_szLeagueTName, pTongStruct->szLeagueTName); // ==== end add by Fong Kieu ===

	int i;
	for(i=0;i<defTONG_MAX_DIRECTOR;++i)
	{
		strcpy(aTongControl.m_szDirectorTitle[i], pTongStruct->szDirectorTitle[i]);
	}
	for(i=0;i<defTONG_MAX_MANAGER;++i)
	{
		strcpy(aTongControl.m_szManagerTitle[i], pTongStruct->szManagerTitle[i]);
	}

	int aDirectorNum = 0;
	int aManagerNum = 0;
	int aMemberNum = 0;

	m_TongTable->closeCursor(cursor);

	cursor = m_MemberTable->search(aTongName, aKeySize, 0);

	if (cursor)
	{
		aBuffer = cursor->data;
		//m_MemberTable->closeCursor(cursor);
	}


	while(aBuffer)
	{
		TMemberStruct* pMemberStruct = (TMemberStruct*)aBuffer;
		switch(pMemberStruct->MemberClass)
		{
		case enumTONG_FIGURE_MASTER:
			strcpy(aTongControl.m_szMasterName, pMemberStruct->szName);
			aTongControl.m_dwMasterID = g_String2Id(pMemberStruct->szName);
			break;
			
		case enumTONG_FIGURE_DIRECTOR:
			if (pMemberStruct->nTitleIndex < 0 || pMemberStruct->nTitleIndex >= defTONG_MAX_DIRECTOR)
				break;
			strcpy(aTongControl.m_szDirectorName[pMemberStruct->nTitleIndex], pMemberStruct->szName);
			aTongControl.m_dwDirectorID[pMemberStruct->nTitleIndex] = g_String2Id(pMemberStruct->szName);
			aDirectorNum++;
			break;
			
		case enumTONG_FIGURE_MANAGER:
			if (pMemberStruct->nTitleIndex < 0 || pMemberStruct->nTitleIndex >= defTONG_MAX_MANAGER)
				break;
			strcpy(aTongControl.m_szManagerName[pMemberStruct->nTitleIndex], pMemberStruct->szName);
			aTongControl.m_dwManagerID[pMemberStruct->nTitleIndex] = g_String2Id(pMemberStruct->szName);
			aManagerNum++;
			break;
			
		case enumTONG_FIGURE_MEMBER:
			aTongControl.AddMember(pMemberStruct->szName, pMemberStruct->nSex);
			aMemberNum++;
			break;
		}
		if (!m_MemberTable->next(cursor)) {
			break;
		}
		aBuffer = cursor->data; //get next
	}
	aTongControl.m_nDirectorNum = aDirectorNum;
	aTongControl.m_nManagerNum = aManagerNum;
	aTongControl.m_nMemberNum = aMemberNum;

	
	aSize = sizeof(TMoneyStruct);

	cursor = m_MoneyTable->search(aTongName, aKeySize);

	if (cursor)
	{
		aBuffer = cursor->data;
	}
	else
		aBuffer = NULL;

	int dwMoney = 0;
	if (aBuffer)
	{
		TMoneyStruct* pMoneyStruct = (TMoneyStruct*)aBuffer;
		dwMoney = pMoneyStruct->m_dwMoney;
	}
	
	aTongControl.m_dwMoney = dwMoney;

	m_MemberTable->closeCursor(cursor);
	//delete [] aBuffer;

	return aResult;
}

int CTongDB::GetTongCount()
{//Get the number of sects
	int aCount = 0;
	int aTongSize;
	int aTongKeySize;
	char aTongKeyBuffer[defTONG_STR_LENGTH];
	char aTongBuffer[sizeof(TTongStruct)];
	bool aResult = m_TongTable->GetRecordEx(aTongBuffer, aTongSize, aTongKeyBuffer, aTongKeySize, DB_FIRST);
	while(aResult)
	{
		aCount++;
		aResult = m_TongTable->GetRecordEx(aTongBuffer, aTongSize, aTongKeyBuffer, aTongKeySize, DB_NEXT);
	}

	return aCount;
}

int CTongDB::GetTongList(TTongStruct* aTongList, int aMaxNum)
{//Get the list of guilds
	int aCount = 0;
	int aTongSize;
	int aTongKeySize;
	char aTongKeyBuffer[defTONG_STR_LENGTH];
	char aTongBuffer[sizeof(TTongStruct)];
	bool aResult = m_TongTable->GetRecordEx(aTongBuffer, aTongSize, aTongKeyBuffer, aTongKeySize,DB_FIRST);
	while(aResult)
	{
		TTongStruct* pTong = (TTongStruct*)aTongBuffer;
		strcpy(aTongList[aCount].szName, pTong->szName);

		memcpy(&aTongList[aCount], pTong, sizeof(TTongStruct));
		int aMemCount = 0;
		int aMemSize;
		char* aMemBuffer;
		//aMemBuffer = m_MemberTable->search(aTongKeyBuffer, aTongKeySize, aMemSize, 0);
		ZCursor* cursor = m_MemberTable->search(aTongKeyBuffer, aTongKeySize, 0);

		if (cursor)
		{
			aMemBuffer = cursor->data;
		}
		while(aMemBuffer)
		{
			TMemberStruct* pMember = (TMemberStruct*)aMemBuffer;
			if(pMember->MemberClass == enumTONG_FIGURE_MEMBER)
					aMemCount++;
			//delete [] aMemBuffer;
			if(!m_MemberTable->next(cursor)) break;
			aMemBuffer = cursor->data;
		}

		aTongList[aCount].MemberCount = aMemCount;

		aCount++;
		if( aCount>= aMaxNum ) break;
		aResult = m_TongTable->GetRecordEx(aTongBuffer, aTongSize, aTongKeyBuffer, aTongKeySize, DB_NEXT);
	}

	return aCount;
}

bool CTongDB::ChangeMember(TMemberStruct aMember)
{//Add guild members
	return m_MemberTable->add(aMember.szName, strlen(aMember.szName) + 1, (char*)(&aMember), sizeof(TMemberStruct));
}

bool CTongDB::DelMember(char* aMemberName)
{//Delete guild members
	int aKeySize = strlen(aMemberName) + 1;

	return m_MemberTable->remove(aMemberName, aKeySize);
}
bool CTongDB::ChangeMoney( TMoneyStruct aMoney )
{
	return m_MoneyTable->add(aMoney.szTong, strlen(aMoney.szTong) + 1, (char*)(&aMoney), sizeof(TMoneyStruct));
}

bool CTongDB::DelMoney( char* aTongName )
{
	int aKeySize = strlen(aTongName) + 1;
	
	return m_MoneyTable->remove(aTongName, aKeySize);	
}

bool CTongDB::UpdateExtPoint(char* szAccount, int nExtPoint)
{
	bool aResult = false;
	if (pConnectDB->OpenConnect(&m_DatabaseInfo))
	{
		char strSQL[MAX_PATH];
		sprintf(strSQL, "SELECT cAccName FROM Account_Habitus WHERE cAccName = '%s'", szAccount);
		S3PResultVBC* pResult = NULL;
		if (pConnectDB->QuerySql(strSQL, &pResult))
		{
			if (pResult->num_rows() > 0)
			{
				sprintf(strSQL, "UPDATE Account_Habitus SET nExtPoint = %d WHERE cAccName = '%s'", nExtPoint, szAccount);
				if (!(pConnectDB->Do(strSQL)))
					rTRACE("[UpdateExtPoint][%s] Update Coin [%d] to database failed!!!", szAccount, nExtPoint);
				else
					rTRACE("[UpdateExtPoint][%s] Update Coin [%d] into database successfully!!!", szAccount, nExtPoint);
				aResult = true;
			}
			else
			{
				rTRACE("[UpdateExtPoint] Can not find account name [%s]!!!", szAccount);
				return false;
			}
		}
		if(pResult)
			pResult->unuse();
		aResult = true;
		pConnectDB->CloseConnect();
	}
	else
	{
		rTRACE("[UpdateExtPoint] Connect database error!!!");
		return false;
	}
	return aResult;
}

int CTongDB::GetExtPoint(char* szAccount)
{
	int nExtPoint = 0;
	if (pConnectDB->OpenConnect(&m_DatabaseInfo))
	{
		char strSQL[MAX_PATH];
		sprintf(strSQL, "SELECT nExtPoint FROM Account_Habitus WHERE cAccname = '%s'",szAccount);
		S3PResultVBC* pResult = NULL;
		if (pConnectDB->QuerySql(strSQL, &pResult))
		{
			if (pResult->num_rows() <= 0)
			{
				rTRACE("[GetCoin] No result return!!!");
			}
			else
			{
				_variant_t extpoint = 0L;
				pResult->get_field_data(0, &extpoint, sizeof(_variant_t));
				nExtPoint = extpoint.lVal;
			}
		}
		if (pResult)
			pResult->unuse();
		pConnectDB->CloseConnect();
	}
	else
	{
		rTRACE("[GetCoin] Connect database error!!!");
		return false;
	}
	return nExtPoint;
}