// db_Tran.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include "DBTable.h"

#define MAX_TASK 2000

using namespace std;

#pragma pack( push, 1 )

struct TRoleBaseInfoOld
{

};

struct TRoleDataOld
{

};

struct TDBItemDataOld
{

};

struct TRoleBaseInfoNew
{

};

struct TRoleDataNew
{

};

struct TDBItemDataNew
{

};

struct TDBTaskData
{
	int		m_nTaskId;
	int		m_nTaskValue;
};

struct TDBSkillData
{
	short	m_nSkillId;
	short	m_nSkillLevel;
	int		m_nSkillExp;		// 技能等级
};

#pragma pack( pop )

int get_account(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey) 
{
	memset( ikey, 0, sizeof( DBT ) );
	TRoleDataNew *pRoleData = (TRoleDataNew *)pdata->data;

	ikey->data = pRoleData->BaseInfo.caccname;
	ikey->size = strlen( pRoleData->BaseInfo.caccname ) + 1;

	return 0;
}

void DBTranslate()
{

}

int main(int argc, char **argv)
{
	system("color 1E");
	system("title Database Translate By Phong Kieu (c) 2021");
	cout<<"Author: Phong Kieu"<<endl;
	cout<<"Coder: Phong Kieu"<<endl;
	char charin;
	do 
	{
		cout<<"Please press 'y' to continue: ";
		cin>>charin;
	} while (charin != 'y');
	
	DBTranslate();

	system("pause");
	return 0;
}