#ifndef DBTABLE_H
#define DBTABLE_H
//定义支持复制、事务处理的，多索引记录的数据表类

#include "db_cxx.h"

#define MAX_INDEX			16
#define MAX_TABLE_NAME		256

typedef int (*GetIndexFunc)(DB *, const DBT *, const DBT *, DBT *);

typedef struct ZCursor {
	bool bTravel;								//Whether to traverse
	DBC* dbcp;									//Cursor currently used for traversal
	int index;									//current index
	char* key;
	int key_size;
	char* data;									//returned data
	int size;									//data size
}tagZCursor;

class ZDBTable {
	DB *primary_db;												//存放主键-数据的数据库
	DB *index_db[MAX_INDEX];									//存放索引-主键的数据库
	GetIndexFunc get_index_funcs[MAX_INDEX];					//从数据中获得二级key的的函数列表
	bool is_index_unique[MAX_INDEX];							//索引是否唯一
	int index_number;											//二级索引数目
	char table_name[MAX_TABLE_NAME];
	DBC *dbcp;													//目前考虑单线程，使用一个CURSOR
protected:
	char env_path[MAX_TABLE_NAME];
	DB_ENV *dbenv;												//数据库环境
	ZCursor *_search(bool bKey, const char *key_ptr, int key_size, int index);		//搜索指定记录
	bool _next(bool bKey, ZCursor* cursor);															//下一个记录
public:
	bool bStop;
	ZDBTable(const char *path, const char *name);			//环境目录和数据表的名字
	virtual ~ZDBTable();
	
	int addIndex(GetIndexFunc func, bool isUnique = false);		//增加二级索引
	bool open();												//打开数据表
	void close();												//关闭数据表
	bool commit();												//基于事务的提交
//基本记录操作
	bool add(const char *key_ptr, int key_size, const char *data_ptr, int data_size);
	bool remove(const char* key_ptr, int key_size, int index = -1);

	void closeCursor(ZCursor* cursor) {
		if (!cursor) return;
		if (cursor->bTravel) {
			free(cursor->key);
		}
		free(cursor->data);

		delete cursor;
	}

	ZCursor* first() {											//Traversing the database to get the first record
		return _search(false, NULL, 0, -1);
	}

	ZCursor* search(const char* key_ptr, int key_size, int index = -1) {
		return _search(false, key_ptr, key_size, index);		//Search for specific records
	}
	bool next(ZCursor* cursor) {											//next record
		return _next(false, cursor);
	}
	ZCursor* search_key(const char* key_ptr, int key_size, int index = -1) {	//Search for the specified record and return the primary key value
		return _search(true, key_ptr, key_size, index);
	}
	bool next_key(ZCursor* cursor) {															//Next record, return the primary key value
		return _next(true, cursor);
	}
	
	//遍历纪录记录(by Fellow)
	ZCursor* GetRecord(int cpMode, int index = -1);//取得按游标某一个数据
	ZCursor* GetRecord_key(int cpMode, int index = -1);	//取得按游标某一个数据的Key值
	
	bool GetRecordEx(char* aBuffer, int& size,
				char* aKeyBuffer, int &keysize,
				int cpMode, int index = -1);		//取得按游标某一个数据和key(新版函数)

	//Here are some maintenance operations
	void deadlock() {						//remove deadlock
		dbenv->lock_detect(dbenv, 0, DB_LOCK_DEFAULT, NULL);
	}
	void removeLog();						//clear log file

};


//数据库操作类（只读）		Add By Fellow At 2003.08.14
//支持并发读取
class CDBTableReadOnly  
{	
	DB *primary_db;												//存放主键-数据的数据库
	DBC *dbcp;													//目前考虑单线程，使用一个CURSOR
	char table_name[MAX_TABLE_NAME];
protected:
	char env_path[MAX_TABLE_NAME];
	DB_ENV *dbenv;												//数据库环境
	ZCursor* _search(bool bKey, const char* key_ptr, int key_size, int index);		//搜索指定记录
	//bool _next(bool bKey, ZCursor* cursor);							//下一个记录
public:
	CDBTableReadOnly(const char *path, const char *name);			//环境目录和数据表的名字
	virtual ~CDBTableReadOnly();
	
	bool open();												//打开数据表
	void close();												//关闭数据表
//基本记录操作
	ZCursor* search(const char* key_ptr, int key_size, int index = -1) {
		return _search(false, key_ptr, key_size, index);		//Search for specific records
	}

	void closeCursor(ZCursor* cursor) {
		if (!cursor) return;
		if (cursor->bTravel) {
			free(cursor->key);
		}
		free(cursor->data);

		delete cursor;
	}


	//bool next(ZCursor* cursor) {											//next record
	//	return _next(false, cursor);
	//}

	//遍历纪录记录(by Fellow)
	//ZCursor* GetRecord(int cpMode);//取得按游标某一个数据
	//ZCursor* GetRecord_key(int cpMode);	//取得按游标某一个数据的Key值
	//ZCursor* GetRecordEx(char* aKeyBuffer, int &keysize,
	//	int cpMode);

	//ZCursor* search_key(const char* key_ptr, int key_size, int index = -1) {	//Search for the specified record and return the primary key value
	//	return _search(true, key_ptr, key_size, index);
	//}
	//bool next_key(ZCursor* cursor) {															//Next record, return the primary key value
	//	return _next(true, cursor);
	//}
};
#define MAX_RETRY	16
#endif
