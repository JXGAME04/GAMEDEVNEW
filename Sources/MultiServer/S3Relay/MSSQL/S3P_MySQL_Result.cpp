#include "S3P_MySQL_Result.h"
#include <iostream>
#include "COMUtil.h"
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include "GlobalFun.h"
#include "../S3Relay.h"

S3P_MySQL_Result::S3P_MySQL_Result(MYSQL_RES* pResult) : m_pResult(pResult) {}

int S3P_MySQL_Result::num_rows() const {
	int iRet = 0;
	try {
		if (m_pResult) iRet = mysql_num_rows(m_pResult);
	}
	catch (sql::SQLException& e) {
		std::cerr << e.what() << std::endl;
	}
	return iRet;
}

int S3P_MySQL_Result::num_fields() const {
	int iRet = 0;
	try {
		if (m_pResult) iRet = mysql_num_fields(m_pResult);
	}
	catch (sql::SQLException& e) {
		std::cerr << e.what() << std::endl;
	}
	return iRet;
}

void S3P_MySQL_Result::data_seek(unsigned int offset, int nType) const {
	try {
		if (m_pResult) {
			switch (nType) {
			case SEEK_SET:
				if (offset > 0 && offset < mysql_num_rows(m_pResult)) {
					mysql_data_seek(m_pResult, offset);
				}
				else {
					mysql_data_seek(m_pResult, 0);
				}
				break;
			case SEEK_CUR:
				mysql_data_seek(m_pResult, offset);
				break;
			case SEEK_END:
				mysql_data_seek(m_pResult, mysql_num_rows(m_pResult));
				break;
			default:
				assert(false); // Invalid seek type
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

bool S3P_MySQL_Result::get_field_data(unsigned int nfieldindex, void* pData, unsigned long nsize) {
	try {
		if (m_pResult && nsize > 0) {
			MYSQL_FIELD* field = mysql_fetch_field_direct(m_pResult, nfieldindex);
			if (field) {
				mysql_data_seek(m_pResult, 0);
				MYSQL_ROW row = mysql_fetch_row(m_pResult);
				if (row) {
					unsigned long* lengths = mysql_fetch_lengths(m_pResult);
					unsigned long field_size = lengths[nfieldindex];
					if (field_size <= nsize) {
						switch (field->type) {
						case MYSQL_TYPE_STRING:
						case MYSQL_TYPE_VAR_STRING: {
							char* str = "";
							if (row[nfieldindex] != NULL) {
								str = reinterpret_cast<char*>(row[nfieldindex]);
								(*reinterpret_cast<_variant_t*>(pData)) = str;
							}
							break;
						}
						case MYSQL_TYPE_LONG:
						case MYSQL_TYPE_TINY:
						case MYSQL_TYPE_SHORT: {
							long int_val = 0;
							if (row[nfieldindex] != NULL)
								int_val = strtol(row[nfieldindex], nullptr, 10);
							(*reinterpret_cast<_variant_t*>(pData)) = int_val;
							break;
						}
						case MYSQL_TYPE_LONGLONG: {
							__int64 int64_val = 0;
							if (row[nfieldindex] != NULL)
								int64_val = _strtoi64(row[nfieldindex], nullptr, 10);
							(*reinterpret_cast<_variant_t*>(pData)) = int64_val;
							break;
						}
						case MYSQL_TYPE_FLOAT: {
							float float_val = strtof(row[nfieldindex], nullptr);
							(*reinterpret_cast<_variant_t*>(pData)) = float_val;
							break;
						}
						case MYSQL_TYPE_DOUBLE: {
							double double_val = 0;
							if (row[nfieldindex] != NULL)
								double_val = strtod(row[nfieldindex], nullptr);
							(*reinterpret_cast<_variant_t*>(pData)) = double_val;
							break;
						}
						case MYSQL_TYPE_DATETIME:
						case MYSQL_TYPE_TIMESTAMP: {
							MYSQL_TIME* mysql_time = reinterpret_cast<MYSQL_TIME*>(row[nfieldindex]);
							SYSTEMTIME system_time = { 0 };
							system_time.wYear = mysql_time->year;
							system_time.wMonth = mysql_time->month;
							system_time.wDay = mysql_time->day;
							system_time.wHour = mysql_time->hour;
							system_time.wMinute = mysql_time->minute;
							system_time.wSecond = mysql_time->second;
							DATE date;
							SystemTimeToVariantTime(&system_time, &date);
							(*reinterpret_cast<_variant_t*>(pData)) = date;
							break;
						}
						case MYSQL_TYPE_DATE:
						case MYSQL_TYPE_NEWDATE: {
							MYSQL_TIME* mysql_time = reinterpret_cast<MYSQL_TIME*>(row[nfieldindex]);
							SYSTEMTIME system_time = { 0 };
							system_time.wYear = mysql_time->year;
							system_time.wMonth = mysql_time->month;
							system_time.wDay = mysql_time->day;
							DATE date;
							SystemTimeToVariantTime(&system_time, &date);
							(*reinterpret_cast<_variant_t*>(pData)) = date;
							break;
						}
						case MYSQL_TYPE_TIME: {
							MYSQL_TIME* mysql_time = reinterpret_cast<MYSQL_TIME*>(row[nfieldindex]);
							SYSTEMTIME system_time = { 0 };
							system_time.wHour = mysql_time->hour;
							system_time.wMinute = mysql_time->minute;
							system_time.wSecond = mysql_time->second;
							DATE date;
							SystemTimeToVariantTime(&system_time, &date);
							(*reinterpret_cast<_variant_t*>(pData)) = date;
							break;
						}
						default:
							gTrace("[S3P_MySQL_Result::get_field_data] Unsupported type: %d", field->type);
							return false;
						}
						return true;
					}
				}
			}
		}
	}
	catch (std::exception& e) {
		gTrace("[S3P_MySQL_Result::get_field_data] Error: %s", e.what());
		std::cerr << e.what() << std::endl;
	}
	return false;
}

int S3P_MySQL_Result::unuse() {
	int nRet = 0;
	if (m_pResult) {
		mysql_free_result(m_pResult);
		m_pResult = NULL;
		nRet = 1;
	}
	return nRet;
}