// S3PDB_MySQL_Connection.h: interface for the S3PDB_MySQL_Connection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_S3PDB_MYSQL_CONNECTION_H__BFEECC7C_6B4A_466E_80BF_26AD4F94E506__INCLUDED_)
#define AFX_S3PDB_MYSQL_CONNECTION_H__BFEECC7C_6B4A_466E_80BF_26AD4F94E506__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KStdAfx.h"
#include <string>
#include "S3PDBConVBC.h"
#include "S3P_MySQL_Result.h"

#include <mysql.h>

class S3PDB_MySQL_Connection : public S3PDBConVBC
{
public:
	S3PDB_MySQL_Connection();
	virtual ~S3PDB_MySQL_Connection();

	// Open a connection to the MySQL database
	virtual bool OpenConnect(_LPDATABASEINFO pInfo);

	// Reconnect a connection to the MySQL database after timeout
	virtual bool Reconnect();

	// Close the connection to the MySQL database
	virtual void CloseConnect();

	// Execute a SQL query and return the results in a S3PResultVBC object
	virtual bool QuerySql(const char* lpszSql, S3PResultVBC** ppResult);

	// Execute a SQL command
	virtual bool Do(const char* lpszSql);

protected:
	unsigned long long m_nlastPingTime;
	_LPDATABASEINFO m_pInfo;
	// The MySQL database connection object
	MYSQL* m_mysql = NULL;
	MYSQL_STMT* m_pStmt;

	// The result object used to store query results
	S3P_MySQL_Result m_Result;

	// Get a free result object for storing query results
	bool GetFreeResult(S3P_MySQL_Result** ppResult);

	// Execute a SQL query and store the results in a S3P_MySQL_Result object
	bool QueryResult(const char* lpszSql, S3P_MySQL_Result* pRes);

	// Execute a SQL command
	bool DoResult(const char* lpszSql);

	// Execute a SQL query using a prepared statement and store the results in a S3P_MySQL_Result object
	bool QueryStmt(const char* lpszSql, S3P_MySQL_Result* pRes);

	// Execute a SQL command using a prepared statement
	bool DoStmt(const char* lpszSql);
};

#endif // !defined(AFX_S3PDB_MYSQL_CONNECTION_H__BFEECC7C_6B4A_466E_80BF_26AD4F94E506__INCLUDED_)