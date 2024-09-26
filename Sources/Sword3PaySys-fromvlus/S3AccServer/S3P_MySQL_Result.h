// S3P_MySQL_Result.h: interface for the S3P_MySQL_Result class.
//
//////////////////////////////////////////////////////////////////////

#ifndef S3P_MYSQL_RESULT_H
#define S3P_MYSQL_RESULT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KStdAfx.h"

#include <mysql.h>

#include "S3PResultVBC.h"

#include <iostream>

class S3P_MySQL_Result : public S3PResultVBC
{
public:
	/*S3P_MySQL_Result::S3P_MySQL_Result()
	The constructor for the S3P_MySQL_Result class, which initializes the MYSQL_RES pointer to NULL.
	*/
	S3P_MySQL_Result() : m_pResult(nullptr) {}

	/*S3P_MySQL_Result::~S3P_MySQL_Result()
	The destructor for the S3P_MySQL_Result class, which frees the MYSQL_RES pointer if it is not NULL.
	*/
	virtual ~S3P_MySQL_Result() {}

	/*void S3P_MySQL_Result::AttachResult(MYSQL_RES* pResult)
	This function is used to attach a MYSQL_RES pointer to the S3P_MySQL_Result object.
	*/
	void AttachResult(MYSQL_RES* pResult)
	{
		m_pResult = pResult;
	}

	S3P_MySQL_Result(MYSQL_RES* pResult);

	/*int S3P_MySQL_Result::num_rows() const
	This function returns the number of rows in the result set. It calls the mysql_num_rows() function on the attached MYSQL_RES pointer.
	*/
	virtual int num_rows() const;

	/*int S3P_MySQL_Result::num_fields() const
	This function returns the number of fields in the result set. It calls the mysql_num_fields() function on the attached MYSQL_RES pointer.
	*/
	virtual int num_fields() const;

	/*void S3P_MySQL_Result::data_seek(unsigned int offset, int nType) const
	This function moves the result set pointer to the specified row.
	The offset parameter specifies the row number to move to,
	and the nType parameter specifies the type of seek operation to perform (begin, end, next, or previous).
	It calls the mysql_data_seek() function on the attached MYSQL_RES pointer.
	*/
	virtual void data_seek(unsigned int offset, int nType) const;

	/*bool S3P_MySQL_Result::get_field_data(unsigned int nfieldindex, void* pData, unsigned long nsize)
	This function retrieves the data for the specified field and places it in the provided buffer.
	The nfieldindex parameter specifies the zero-based index of the field to retrieve,
	and the pData parameter points to the buffer to receive the data.
	The nsize parameter specifies the size of the buffer in bytes.
	The function returns true if the data was retrieved successfully, and false otherwise.
	It calls the mysql_fetch_field_direct() and mysql_fetch_row() functions on the attached MYSQL_RES pointer.
	*/
	virtual bool get_field_data(unsigned int nfieldindex, void* pData, unsigned long nsize);

	/*This function frees the MYSQL_RES pointer and sets it to NULL.
	It is called when the S3P_MySQL_Result object is no longer needed.
	*/
	virtual int unuse();

	MYSQL_RES* m_pResult;
	MYSQL_RES* m_pResult2;
};

#endif // S3P_MYSQL_RESULT_H