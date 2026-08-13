#include "stdafx.h"
#include "DBTable.h"
#ifndef WIN32
#include <sys/stat.h>
#else
#include <direct.h>
#endif
#include <stdio.h>
#include <string.h>
#include "../../Core/Src/GameDataDef.h"



ZDBTable::ZDBTable(const char *path, const char *name) {
#ifdef WIN32
		getcwd(env_path, MAX_TABLE_NAME);
#else
#endif
		strcat(env_path, "\\");
		strcat(env_path, path);
#ifdef WIN32
		int ret = mkdir(env_path);
#else
        int ret = mkdir(env_path, 0);
#endif
	dbcp = NULL;//初始化游标（by Fellow）
	
	if(!db_env_create(&dbenv, 0)) {
			dbenv->set_errpfx(dbenv, "index_db");
            dbenv->set_lg_regionmax(dbenv, 512 * 1024);
            dbenv->set_lg_max(dbenv, 16 * 1024 * 1024);
            dbenv->set_lg_bsize(dbenv, 2 * 1024 * 1024);
            dbenv->set_cachesize(dbenv, 0, 64 * 1024 * 1024, 1);

			if(!dbenv->open(dbenv, env_path, DB_CREATE | DB_INIT_LOG | DB_INIT_LOCK | DB_INIT_MPOOL | DB_INIT_TXN | DB_RECOVER | DB_THREAD | DB_PRIVATE, 0)) {
				ret = dbenv->set_flags(dbenv, DB_AUTO_COMMIT | DB_TXN_NOSYNC, 1);
				index_number = 0;
				strcpy(table_name, name);
				return;				//成功了
			}
		dbenv->close(dbenv, 0);
	}
	dbenv = NULL;
}

ZDBTable::~ZDBTable() {
	if(dbenv) dbenv->close(dbenv, 0);
}

int ZDBTable::addIndex(GetIndexFunc func, bool isUnique) {
	if(!dbenv) return -1;
	if(index_number + 1 >= MAX_INDEX) return index_number;
	get_index_funcs[index_number] = func;
	is_index_unique[index_number] = isUnique;
	return index_number++;
}

bool ZDBTable::open() {
	if (!dbenv) return false;
	bStop = false;
	char index_table_name[MAX_TABLE_NAME];
	int index;
	int ret;
	if (!db_create(&primary_db, dbenv, 0)) {
		if (!primary_db->open(primary_db, NULL, table_name, NULL, DB_BTREE, DB_CREATE | DB_AUTO_COMMIT | DB_THREAD, 0664)) {	//Open the master database
			for (index = 0; index < index_number; index++) {
				sprintf(index_table_name, "%s.%d", table_name, index);
				if (!db_create(&index_db[index], dbenv, 0)) {
					if (!is_index_unique[index]) {
						if (index_db[index]->set_flags(index_db[index], DB_DUP | DB_DUPSORT)) break;
					}
					if (index_db[index]->open(index_db[index], NULL, index_table_name, NULL, DB_BTREE, DB_CREATE | DB_AUTO_COMMIT | DB_THREAD, 0664)) break;
					if (ret = primary_db->associate(primary_db, NULL, index_db[index], get_index_funcs[index], DB_AUTO_COMMIT)) {
						index_db[index]->close(index_db[index], 0);
						break;
					}
				}
				else break;
			}
			if (index == index_number) return true;										//successful
			else while (--index) (index_db[index])->close(index_db[index], 0);				//Error, close previous index table
			primary_db->close(primary_db, 0);
		}
	}
	return false;
}

void ZDBTable::close() {
	if(!dbenv) return;
	primary_db->close(primary_db, 0);
	for(int index = 0; index < index_number; index++) index_db[index]->close(index_db[index], 0);
}

bool ZDBTable::commit() {
	if(!dbenv) return false;
	int ret;
	DB_TXN *tid;
	if((ret = dbenv->txn_begin(dbenv, NULL, &tid, 0)) != 0) return false;
	if((ret = primary_db->sync(primary_db, 0)) == 0) {
		ret = tid->commit(tid, 0);
		dbenv->txn_checkpoint(dbenv, 0, 0, 0);
		if(!ret) return true;
	}
	else {
		tid->abort(tid);
		return false;
	}
	return true;
}

bool ZDBTable::add(const char* key_ptr, int key_size, const char* data_ptr, int data_size) {
	DBT key, data;
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = (void*)key_ptr;
	key.size = key_size;
	data.data = (void*)data_ptr;
	data.size = data_size;

	DB_TXN* txn = NULL;  // Transaction handle
	int ret;
	int retry = 0;

	while (retry < MAX_RETRY) {
		// Begin a transaction
		ret = dbenv->txn_begin(dbenv, NULL, &txn, 0);
		if (ret != 0) {
			// Handle transaction start error (e.g., log error)
			return false;
		}

		// Perform the put operation within the transaction
		ret = primary_db->put(primary_db, txn, &key, &data, 0);
		if (ret == 0) {
			// Commit the transaction and release locks
			ret = txn->commit(txn, 0);
			if (ret == 0) {
				commit();
				return true; // Success
			}
			else {
				// Handle commit error (e.g., log error)
				return false;
			}
		}
		else if (ret == DB_LOCK_DEADLOCK) {
			// Abort the transaction and retry
			txn->abort(txn);  // Abort current transaction
			retry++;
		}
		else {
			// Abort the transaction on other errors and break the loop
			txn->abort(txn);
			return false;
		}
	}

	// If we exhausted retries, return false
	return false;
}


ZCursor* ZDBTable::_search(bool bKey, const char* key_ptr, int key_size, int index) {
	DBT key, data, pkey;
	if (index < -1 || index >= index_number) return NULL;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	key.flags = DB_DBT_MALLOC;
	data.flags = DB_DBT_MALLOC;
	pkey.flags = DB_DBT_MALLOC;
	key.data = (void*)key_ptr;
	key.size = key_size;
	DBC* dbcp = NULL;
	if (!key_ptr || !key_size) {							//No index value is set, it is required to traverse the database
		if (index_db[index]->cursor(index_db[index], NULL, &dbcp, 0)) {
			return NULL;
		}
		if (dbcp->c_get(dbcp, &key, &data, DB_FIRST)) {
			dbcp->c_close(dbcp);
			return NULL;
		}
	}
	else {
		if (index == -1) {									//primary key search
			if (primary_db->get(primary_db, NULL, &key, &data, 0)) return NULL;
		}
		else if (is_index_unique[index]) {					//no duplicate index
			if (bKey) {
				if (index_db[index]->pget(index_db[index], NULL, &key, &pkey, &data, 0)) return NULL;
			}
			else {
				if (index_db[index]->get(index_db[index], NULL, &key, &data, 0)) return NULL;
			}
		}
		else {												//open cursor
			if (index_db[index]->cursor(index_db[index], NULL, &dbcp, 0)) {
				return NULL;
			}
			if (bKey) {
				if (dbcp->c_pget(dbcp, &key, &pkey, &data, DB_SET)) {
					dbcp->c_close(dbcp);
					return NULL;
				}
			}
			else {
				if (dbcp->c_get(dbcp, &key, &data, DB_SET)) {
					dbcp->c_close(dbcp);
					return NULL;
				}
			}
		}
	}
	ZCursor* result = new ZCursor;
	result->index = 0;
	result->dbcp = dbcp;
	if (bKey)
	{
		result->data = (char*)pkey.data;
		result->size = pkey.size;

		pkey.data = NULL;
	}
	else
	{
		result->data = (char*)data.data;
		result->size = data.size;

		data.data = NULL;
	}

	if (!key_ptr || !key_size)
	{
		result->key = (char*)key.data;
		result->key_size = key.size;
		result->bTravel = true;

		key.data = NULL;
	}
	else
	{
		result->bTravel = false;
	}

	if (pkey.data) free(pkey.data);
	if (data.data) free(data.data);

	return result;
}

bool ZDBTable::remove(const char *key_ptr, int key_size, int index) {
	DBT data, key;
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = (void *)key_ptr;
	key.size = key_size;
	int ret;
	int retry = 0;
RETRY:
	ret = primary_db->del(primary_db, NULL, &key, DB_AUTO_COMMIT);
	if(ret == DB_LOCK_DEADLOCK && ++retry < MAX_RETRY) {
		if(bStop) return false;
		goto RETRY;
	}
	if(ret) return false;
	else {
		commit();
		return true;
	}
}

bool ZDBTable::_next(bool bKey, ZCursor* cursor) {
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	key.flags = DB_DBT_MALLOC;
	data.flags = DB_DBT_MALLOC;
	pkey.flags = DB_DBT_MALLOC;
	if (!cursor || !cursor->dbcp) return false;
	free(cursor->data);
	cursor->index++;
	if (cursor->bTravel) {
		free(cursor->key);
		if (cursor->dbcp->c_get(cursor->dbcp, &key, &data, DB_NEXT)) {
			cursor->dbcp->c_close(cursor->dbcp);
			delete cursor;
			return false;
		}
	}
	else {
		if (bKey) {
			if (cursor->dbcp->c_pget(cursor->dbcp, &key, &pkey, &data, DB_NEXT_DUP)) {
				cursor->dbcp->c_close(cursor->dbcp);
				delete cursor;
				return false;
			}
		}
		else {
			if (cursor->dbcp->c_get(cursor->dbcp, &key, &data, DB_NEXT_DUP)) {
				cursor->dbcp->c_close(cursor->dbcp);
				delete cursor;
				return false;
			}
		}
	}
	if (bKey)
	{
		cursor->data = (char*)pkey.data;
		cursor->size = pkey.size;

		pkey.data = NULL;
	}
	else {
		cursor->data = (char*)data.data;
		cursor->size = data.size;

		data.data = NULL;
	}

	if (cursor->bTravel)
	{
		cursor->key = (char*)key.data;
		cursor->key_size = key.size;

		key.data = NULL;
	}

	if (key.data) free(key.data);
	if (pkey.data) free(pkey.data);
	if (data.data) free(data.data);

	return true;
}


ZCursor* ZDBTable::GetRecord(int cpMode, int index )
{//取得按游标某一个数据
	DBT key, data, pkey;

	if (index < -1 || index >= index_number) return NULL;
	DBC* dbcp = NULL;

	if (index_db[index]->cursor(index_db[index], NULL, &dbcp, 0)) {
		return NULL;
	}
	if (dbcp->c_get(dbcp, &key, &data, DB_FIRST)) {
		dbcp->c_close(dbcp);
		return NULL;
	}
	bool bKey = false;
	if (index == -1) {									//primary key search
		if (primary_db->get(primary_db, NULL, &key, &data, 0)) return NULL;
	}
	else if (is_index_unique[index]) {					//no duplicate index
		if (bKey) {
			if (index_db[index]->pget(index_db[index], NULL, &key, &pkey, &data, 0)) return NULL;
		}
		else {
			if (index_db[index]->get(index_db[index], NULL, &key, &data, 0)) return NULL;
		}
	}
	else {												//open cursor
		if (index_db[index]->cursor(index_db[index], NULL, &dbcp, 0)) {
			return NULL;
		}
		if (bKey) {
			if (dbcp->c_pget(dbcp, &key, &pkey, &data, DB_SET)) {
				dbcp->c_close(dbcp);
				return NULL;
			}
		}
		else {
			if (dbcp->c_get(dbcp, &key, &data, DB_SET)) {
				dbcp->c_close(dbcp);
				return NULL;
			}
		}
	}
	ZCursor* result = new ZCursor;
	result->index = 0;
	result->dbcp = dbcp;
	if (bKey)
	{
		result->data = (char*)pkey.data;
		result->size = pkey.size;

		pkey.data = NULL;
	}
	else
	{
		result->data = (char*)data.data;
		result->size = data.size;

		data.data = NULL;
	}

	//if (!key_ptr || !key_size)
	{
		result->key = (char*)key.data;
		result->key_size = key.size;
		result->bTravel = true;

		key.data = NULL;
	}

	if (pkey.data) free(pkey.data);
	if (data.data) free(data.data);

	return result;
}


ZCursor* ZDBTable::GetRecord_key(int cpMode, int index )
{
	return GetRecord(cpMode, index);
}

bool ZDBTable::GetRecordEx(char* aBuffer, int& size,
	char* aKeyBuffer, int& keysize, int cpMode, int index, int nBufCap, int nKeyCap)
{
	// Initialize cursor if it hasn't been created yet
	if (!dbcp)
	{
		if (primary_db->cursor(primary_db, NULL, &dbcp, 0))  // Create cursor on the primary DB
		{
			dbcp = NULL;
			return false;
		}
	}

	DBT key, data;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	// Fetch record based on the cursor mode (DB_FIRST, DB_NEXT, etc.)
	int ret = dbcp->c_get(dbcp, &key, &data, cpMode);
	if (ret == DB_NOTFOUND) {
		// No more records (table is empty or we've reached the end)
		return false;
	}
	else if (ret != 0) {
		// An error occurred
		dbcp->c_close(dbcp);  // Close the cursor to avoid a dangling cursor
		dbcp = NULL;
		return false;
	}

	// Copy the key and data into the provided buffers
	// FIX bay-2: khong chep qua suc chua buffer cua caller (nBufCap/nKeyCap = 0
	// nghia la khong gioi han - giu tuong thich code cu). size/keysize van tra
	// ve co THAT cua ban ghi de caller phat hien ban ghi lech phien ban struct.
	{
		int nCopy = (int)data.size;
		if (nBufCap > 0 && nCopy > nBufCap)
			nCopy = nBufCap;
		memmove(aBuffer, data.data, nCopy);
		size = data.size;

		nCopy = (int)key.size;
		if (nKeyCap > 0 && nCopy > nKeyCap)
			nCopy = nKeyCap;
		memmove(aKeyBuffer, key.data, nCopy);
		keysize = key.size;
	}

	return true;  // Successfully retrieved a record
}



CDBTableReadOnly::CDBTableReadOnly(const char *path, const char *name) {
#ifdef WIN32
		getcwd(env_path, MAX_TABLE_NAME);
#else
#endif
		strcat(env_path, "\\");
		strcat(env_path, path);
#ifdef WIN32
		int ret = mkdir(env_path);
#else
        int ret = mkdir(env_path, 0);
#endif
	dbcp = NULL;//初始化游标（by Fellow）

	// Create the environment handle
	if ((ret = db_env_create(&dbenv, 0)) == 0) {
		dbenv->set_errpfx(dbenv, "index_db");
		dbenv->set_lg_regionmax(dbenv, 512 * 1024);
		dbenv->set_lg_max(dbenv, 16 * 1024 * 1024);
		dbenv->set_lg_bsize(dbenv, 2 * 1024 * 1024);
		dbenv->set_cachesize(dbenv, 0, 64 * 1024 * 1024, 1);

		// Open the environment in read-only mode
		if (!dbenv->open(dbenv, env_path, DB_CREATE | DB_INIT_LOG | DB_INIT_LOCK | DB_INIT_MPOOL | DB_INIT_TXN | DB_RECOVER | DB_THREAD | DB_PRIVATE, 0)) {
			strcpy(table_name, name);
			return; // Success
		}

		// In case of error, close the environment
		dbenv->close(dbenv, 0);
	}

	// Set the environment pointer to NULL in case of failure
	dbenv = NULL;
}

CDBTableReadOnly::~CDBTableReadOnly() {
	if(dbenv) dbenv->close(dbenv, 0);
}

bool CDBTableReadOnly::open() {
	if(!dbenv) return false;
	if(!db_create(&primary_db, dbenv, 0)) {
		if(!primary_db->open(primary_db, NULL, table_name, NULL, DB_BTREE, DB_RDONLY, 0664)) {	//打开主数据库
			return true;
		}
	}
	return false;
}	

void CDBTableReadOnly::close() {
	if(!dbenv) return;
	primary_db->close(primary_db, 0);
}

ZCursor* CDBTableReadOnly::_search(bool bKey, const char* key_ptr, int key_size, int index) {

	DBT key, data, pkey;
	if (index!= -1) return NULL;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	key.flags = DB_DBT_MALLOC;
	data.flags = DB_DBT_MALLOC;
	pkey.flags = DB_DBT_MALLOC;
	key.data = (void*)key_ptr;
	key.size = key_size;
	DBC* dbcp = NULL;



	dbcp = NULL;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	key.data = (void *)key_ptr;
	key.size = key_size;
	if(primary_db->get(primary_db, NULL, &key, &data, 0)) return NULL;



	ZCursor* result = new ZCursor;
	result->index = 0;
	result->dbcp = dbcp;
	if (bKey)
	{
		result->data = (char*)pkey.data;
		result->size = pkey.size;

		pkey.data = NULL;
	}
	else
	{
		result->data = (char*)data.data;
		result->size = data.size;

		data.data = NULL;
	}

	if (!key_ptr || !key_size)
	{
		result->key = (char*)key.data;
		result->key_size = key.size;
		result->bTravel = true;

		key.data = NULL;
	}
	else
	{
		result->bTravel = false;
	}

	if (pkey.data) free(pkey.data);
	if (data.data) free(data.data);

	return result;
}

/*char* CDBTableReadOnly::_next(bool bKey) {
	if(!dbcp) return NULL;
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	if(bKey) {
		if(dbcp->c_pget(dbcp, &key, &pkey, &data, DB_NEXT_DUP)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}
	}
	else {
		if(dbcp->c_get(dbcp, &key, &data, DB_NEXT_DUP)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}
	}
	char *result;
	if(bKey) {
		result = new char[pkey.size];
		memmove(result, pkey.data, pkey.size);
		size = pkey.size;
	}
	else {
		result = new char[data.size];
		memmove(result, data.data, data.size);
		size = data.size;
	}
	return result;
}

char *CDBTableReadOnly::GetRecord(int &size, int cpMode)
{
	if(!dbcp)
	{//如果数据库指针没有初始化，先初始化dbcp
		if(primary_db->cursor(primary_db, NULL, &dbcp, 0))
		{//初始化dbcp失败
			dbcp = NULL;
			return NULL;
		}
	}
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	
	if(dbcp->c_get(dbcp, &key, &data, cpMode)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}

	char *result;
	result = new char[data.size];
	memmove(result, data.data, data.size);
	size = data.size;
	
	return result;
}

char *CDBTableReadOnly::GetRecord_key(int &size, int cpMode)
{
	if(!dbcp)
	{//如果数据库指针没有初始化，先初始化dbcp
		if(primary_db->cursor(primary_db, NULL, &dbcp, 0))
		{//初始化dbcp失败
			dbcp = NULL;
			return NULL;
		}
	}
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	
	if(dbcp->c_get(dbcp, &key, &data, cpMode)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}

	char *result;
	result = new char[key.size];
	memmove(result, key.data, key.size);
	size = key.size;
	
	return result;
}

bool CDBTableReadOnly::GetRecordEx(char* aBuffer, int &size,
						   char* aKeyBuffer, int &keysize,
						int cpMode)
{//取得按游标某一个数据(新版函数)
	if(!dbcp)
	{//如果数据库指针没有初始化，先初始化dbcp
		if(primary_db->cursor(primary_db, NULL, &dbcp, 0))
		{//初始化dbcp失败
			dbcp = NULL;
			return false;
		}
	}
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	
	if(dbcp->c_get(dbcp, &key, &data, cpMode)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return false;
		}

	memmove(aBuffer, data.data, data.size);
	size = data.size;
	
	memmove(aKeyBuffer, key.data, key.size);
	keysize = key.size;

	return true;
}

*/