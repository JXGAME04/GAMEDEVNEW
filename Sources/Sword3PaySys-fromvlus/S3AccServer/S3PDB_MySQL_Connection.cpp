#include <mysql.h>
#include "S3PDB_MySQL_Connection.h"
#include "S3P_MySQL_Result.h"
#include "GlobalFun.h"

static int nMySQL = 0;

S3PDB_MySQL_Connection::S3PDB_MySQL_Connection()
{
	if (nMySQL == 0)
		mysql_library_init(0, NULL, NULL);
	nMySQL++;
}

S3PDB_MySQL_Connection::~S3PDB_MySQL_Connection()
{
	if (m_mysql != NULL) {
		mysql_close(m_mysql);
		m_mysql = NULL;
		nMySQL--;
	}
	if (nMySQL == 0) {
		mysql_library_end();
	}
}

void S3PDB_MySQL_Connection::CloseConnect()
{
	if (NULL != m_mysql)
	{
		mysql_close(m_mysql);
		m_mysql = NULL;
	}
}

bool S3PDB_MySQL_Connection::OpenConnect(_LPDATABASEINFO lpDBIdentifier)
{
	assert(lpDBIdentifier);
	bool bRet = true;
	m_pInfo = new _DATABASEINFO;
	m_pInfo->strDataBase = lpDBIdentifier->strDataBase;
	m_pInfo->strPassword = lpDBIdentifier->strPassword;
	m_pInfo->strServer = lpDBIdentifier->strServer;
	m_pInfo->strUser = lpDBIdentifier->strUser;
	if (NULL == m_mysql)
	{
		bRet = false;
		m_mysql = mysql_init(NULL);
		if (m_mysql != NULL)
		{
			if (mysql_real_connect(m_mysql, lpDBIdentifier->strServer.c_str(), lpDBIdentifier->strUser.c_str(), lpDBIdentifier->strPassword.c_str(), lpDBIdentifier->strDataBase.c_str(), 3306, NULL, 0) == NULL)
			{
				gTrace("[S3PDB_MySQL_Connection::OpenConnect]Failed to connect to database: Error message: %s", mysql_error(m_mysql));
				mysql_close(m_mysql);
				m_mysql = NULL;
			}
			else
			{
				gTrace("[S3PDB_MySQL_Connection::OpenConnect]Successful to connect to database");
				m_nlastPingTime = GetTickCount64();  //get time of first mysql connection established.
				m_pStmt = mysql_stmt_init(m_mysql);
				if (m_pStmt != NULL)
				{
					gTrace("[S3PDB_MySQL_Connection::OpenConnect]Successful to initialize MYSQL Statement");
					bRet = true;
				}
				else {
					gTrace("[S3PDB_MySQL_Connection::OpenConnect]Failed to initialize MYSQL Statement: Error message: %s", mysql_error(m_mysql));
				}
			}
		}
		else
		{
			gTrace("[S3PDB_MySQL_Connection::OpenConnect]Failed to initialize MySQL connection");
		}
	}
	return bRet;
}

bool S3PDB_MySQL_Connection::Reconnect()
{
	_LPDATABASEINFO lpDBIdentifier = m_pInfo;

	// Check if the connection exists and is still valid
	if (m_mysql != NULL)
	{
		//gTrace("[S3PDB_MySQL_Connection::Reconnect]Checking if the connection is still alive...");

		// Use mysql_ping to check if the connection is alive
		if (mysql_ping(m_mysql) == 0)
		{
		//	gTrace("[S3PDB_MySQL_Connection::Reconnect]Connection is still active. No need to reconnect.");
			return true;
		}
		else
		{
			gTrace("[S3PDB_MySQL_Connection::Reconnect]Connection has timed out or is no longer valid. Closing and reconnecting...");
			mysql_close(m_mysql);
			m_mysql = NULL;
		}
	}

	// Attempt to reconnect using OpenConnect
	gTrace("[S3PDB_MySQL_Connection::Reconnect]Attempting to reconnect...");
	bool bRet = OpenConnect(lpDBIdentifier);

	if (bRet)
	{
		gTrace("[S3PDB_MySQL_Connection::Reconnect]Reconnection successful.");
	}
	else
	{
		gTrace("[S3PDB_MySQL_Connection::Reconnect]Reconnection failed.");
	}

	return bRet;
}

bool S3PDB_MySQL_Connection::Do(const char* lpszSql)
{
	Reconnect();
	if (m_pStmt)
		return DoStmt(lpszSql);
	else
		return DoResult(lpszSql);
}

bool S3PDB_MySQL_Connection::DoStmt(const char* lpszSql)
{
	BOOL bRet = false;
	if (m_mysql == NULL || m_pStmt == NULL)
		return bRet;

	try
	{
		gTrace("[S3PDB_MySQL_Connection::DoStmt] Query: %s", lpszSql);
		if (mysql_stmt_prepare(m_pStmt, lpszSql, strlen(lpszSql)) == 0)
		{
			if (mysql_stmt_execute(m_pStmt) == 0)
			{
				bRet = true;
			}
			else
			{
				gTrace("[S3PDB_MySQL_Connection::DoStmt]Failed to execute statement: Error message: %s", mysql_stmt_error(m_pStmt));
			}
		}
		else
		{
			gTrace("[S3PDB_MySQL_Connection::DoStmt]Failed to prepare statement: Error message: %s", mysql_stmt_error(m_pStmt));
		}
	}
	catch (...)
	{
		gTrace("[S3PDB_MySQL_Connection::DoStmt]Exception occurred while executing statement");
	}

	return bRet;
}

bool S3PDB_MySQL_Connection::DoResult(const char* lpszSql)
{
	bool bRet = false;
	if (m_mysql == nullptr)
		return bRet;

	MYSQL_STMT* stmt = mysql_stmt_init(m_mysql);

	if (!stmt)
		return bRet;
	gTrace("[S3PDB_MySQL_Connection::DoResult] Query: %s", lpszSql);
	if (mysql_stmt_prepare(stmt, lpszSql, strlen(lpszSql)) != 0)
	{
		gTrace("[S3PDB_MySQL_Connection::DoResult] SQL error: %s", mysql_error(m_mysql));
		mysql_stmt_close(stmt);
		return bRet;
	}

	if (mysql_stmt_execute(stmt) != 0)
	{
		gTrace("[S3PDB_MySQL_Connection::DoResult] SQL error: %s", mysql_error(m_mysql));
		mysql_stmt_close(stmt);
		return bRet;
	}

	bRet = true;

	mysql_stmt_close(stmt);

	return bRet;
}

bool S3PDB_MySQL_Connection::QuerySql(const char* lpszSql, S3PResultVBC** ppResult)
{
	*ppResult = NULL;
	bool bRet = false;
	S3P_MySQL_Result* pResult = NULL;

	Reconnect();
	if (GetFreeResult(&pResult))
	{
		if (m_pStmt)
		{
			bRet = QueryStmt(lpszSql, pResult);
		}
		else
		{
			bRet = QueryResult(lpszSql, pResult);
		}

		if (bRet)
		{
			*ppResult = pResult;
		}
		else
		{
			pResult->unuse();
		}
	}

	return bRet;
}
bool S3PDB_MySQL_Connection::QueryResult(const char* lpszSql, S3P_MySQL_Result* pRes)
{
	bool bRet = false;
	if ((NULL != m_mysql) && (NULL != lpszSql) && (NULL != pRes))
	{
		MYSQL_RES* pResult = nullptr;
		gTrace("[S3PDB_MySQL_Connection::QueryResult] Query: %s", lpszSql);
		if (mysql_query(m_mysql, lpszSql) == 0)
		{
			pResult = mysql_store_result(m_mysql);
		}
		if (pResult != nullptr)
		{
			pRes->AttachResult(pResult);
			bRet = true;
		}
		else
		{
			const char* pError = mysql_error(m_mysql);
			gTrace("[S3PDB_MySQL_Connection::QueryResult] Error: %s", pError);
			bRet = false;
		}
	}
	return bRet;
}

bool S3PDB_MySQL_Connection::QueryStmt(const char* lpszSql, S3P_MySQL_Result* pRes)
{
	bool bRet = false;
	if ((m_mysql != nullptr) && (lpszSql != nullptr) && (pRes != nullptr))
	{
		try
		{
			MYSQL_RES* pResult = nullptr;
			gTrace("[S3PDB_MySQL_Connection::QueryStmt] Query: %s", lpszSql);
			int queryResult = mysql_query(m_mysql, lpszSql);
			if (queryResult == 0)
			{
				pResult = mysql_store_result(m_mysql);
				if (pResult != nullptr)
				{
					pRes->AttachResult(pResult);
					bRet = true;
				}
				else
				{
					gTrace("[S3PDB_MySQL_Connection::QueryStmt] mysql_store_result() failed: %s", mysql_error(m_mysql));
				}
			}
			else
			{
				gTrace("[S3PDB_MySQL_Connection::QueryStmt] mysql_query() failed: %s", mysql_error(m_mysql));
			}
		}
		catch (...)
		{
			gTrace("[S3PDB_MySQL_Connection::QueryStmt] Exception in S3PDB_MySQL_Connection::QueryStmt");
		}
	}

	return bRet;
}

bool S3PDB_MySQL_Connection::GetFreeResult(S3P_MySQL_Result** ppResult)
{
	assert(ppResult);
	if (m_Result.m_pResult == NULL)
	{
		*ppResult = &m_Result;
		return true;
	}
	return false;
}