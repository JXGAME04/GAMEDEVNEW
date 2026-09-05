//////////////////////////////////////////////////////////////////////////////
// OfflineMsgDB.cpp  (S3Relay)  [HAOHUU 04/09] -- xem OfflineMsgDB.h
// Viet kieu C + mysql C API nhu DBTable_MySQL.cpp: moi du lieu nguoi dung deu qua
// prepared statement (khong noi chuoi SQL), ket noi rieng, tu noi lai khi mat.
//////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Global.h"
#include "S3Relay.h"
#include "OfflineMsgDB.h"
#include <mysql.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define OM_INI			".\\DataBase.ini"		// cung tep/section voi DBTable_MySQL.cpp
#define OM_SECTION		"relaydb"
#define OM_CFGFILE		"relay_friendcfg.ini"
#define OM_CFGSEC		"offlinemsg"
#define OM_TABLE		"relay_offline_msg"
#define OM_STR			128
#define OM_PURGE_SECS	3600					// don loi nhan het han toi da 1 lan / gio

static MYSQL*	s_conn = NULL;
static int		s_enable = 0;
static int		s_maxPerReceiver = 10;		// Linux: cmp 9 / jg
static int		s_keepDays = 30;			// Linux: 0x278d00 giay = 30 ngay
static char		s_dbHost[OM_STR], s_dbUser[OM_STR], s_dbPass[OM_STR], s_dbName[OM_STR];
static unsigned	s_dbPort = 3306;
static time_t	s_lastPurge = 0;

static void OmLog(const char* fmt, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, fmt);
	_vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
	va_end(ap);
	buf[sizeof(buf) - 1] = 0;
	rTRACE("[OfflineMsg] %s", buf);
}

static void OmIniStr(const char* key, const char* def, char* out, int cap)
{
	GetPrivateProfileStringA(OM_SECTION, key, def, out, cap - 1, OM_INI);
	out[cap - 1] = 0;
	int n = (int)strlen(out);
	while (n > 0 && (out[n - 1] == ' ' || out[n - 1] == '\t' || out[n - 1] == '\r' || out[n - 1] == '\n'))
		out[--n] = 0;
	int i = 0;
	while (out[i] == ' ' || out[i] == '\t') i++;
	if (i) memmove(out, out + i, strlen(out + i) + 1);
}

static int OmConnect()
{
	if (s_conn) { mysql_close(s_conn); s_conn = NULL; }
	s_conn = mysql_init(NULL);
	if (!s_conn) { OmLog("mysql_init that bai"); return 0; }
	mysql_options(s_conn, MYSQL_SET_CHARSET_NAME, "latin1");
	unsigned int to = 5;
	mysql_options(s_conn, MYSQL_OPT_CONNECT_TIMEOUT, &to);
	// tham so cuoi PHAI la 0: khong mo CLIENT_MULTI_STATEMENTS
	if (!mysql_real_connect(s_conn, s_dbHost, s_dbUser, s_dbPass, s_dbName, s_dbPort, NULL, 0))
	{
		OmLog("mysql_real_connect %s:%u/%s LOI: %s", s_dbHost, s_dbPort, s_dbName, mysql_error(s_conn));
		mysql_close(s_conn);
		s_conn = NULL;
		return 0;
	}
	mysql_autocommit(s_conn, 1);
	return 1;
}

static int OmEnsure()
{
	if (!s_enable) return 0;
	if (s_conn && mysql_ping(s_conn) == 0) return 1;
	OmLog("mat ket noi MySQL -- dang noi lai");
	return OmConnect();
}

static int OmExec(const char* sql)		// chi cho SQL KHONG chua du lieu nguoi dung
{
	if (mysql_query(s_conn, sql))
	{
		OmLog("SQL LOI %u: %s | %.80s", mysql_errno(s_conn), mysql_error(s_conn), sql);
		return 0;
	}
	return 1;
}

static void OmPurgeExpired()
{
	time_t now = time(NULL);
	if (s_lastPurge && now - s_lastPurge < OM_PURGE_SECS) return;
	s_lastPurge = now;
	char sql[256];
	_snprintf(sql, sizeof(sql) - 1, "DELETE FROM " OM_TABLE " WHERE created < DATE_SUB(NOW(), INTERVAL %d DAY)", s_keepDays);
	sql[sizeof(sql) - 1] = 0;
	if (OmExec(sql))
	{
		my_ulonglong n = mysql_affected_rows(s_conn);
		if (n > 0) OmLog("xoa %I64u loi nhan qua %d ngay", (unsigned __int64)n, s_keepDays);
	}
}

static void OmBindBlob(MYSQL_BIND* b, const void* p, unsigned long* plen)
{
	memset(b, 0, sizeof(*b));
	b->buffer_type = MYSQL_TYPE_BLOB;
	b->buffer = (void*)p;
	b->buffer_length = *plen;
	b->length = plen;
}

BOOL OfflineMsg_Init()
{
	s_enable = gGetPrivateProfileIntEx(OM_CFGSEC, "enable", OM_CFGFILE, 1);
	s_maxPerReceiver = gGetPrivateProfileIntEx(OM_CFGSEC, "maxperreceiver", OM_CFGFILE, 10);
	s_keepDays = gGetPrivateProfileIntEx(OM_CFGSEC, "keepdays", OM_CFGFILE, 30);
	if (s_maxPerReceiver < 1) s_maxPerReceiver = 1;
	if (s_maxPerReceiver > OFFMSG_MAX_LOAD) s_maxPerReceiver = OFFMSG_MAX_LOAD;
	if (s_keepDays < 1) s_keepDays = 1;
	if (!s_enable)
	{
		OmLog("TAT theo cau hinh (%s [%s] enable=0)", OM_CFGFILE, OM_CFGSEC);
		return TRUE;
	}

	OmIniStr("Server", "127.0.0.1", s_dbHost, OM_STR);
	OmIniStr("User", "root", s_dbUser, OM_STR);
	OmIniStr("PassWord", "123456", s_dbPass, OM_STR);
	OmIniStr("DataBase", "jx1_role", s_dbName, OM_STR);
	s_dbPort = (unsigned)GetPrivateProfileIntA(OM_SECTION, "Port", 3306, OM_INI);

	if (!OmConnect())
	{
		s_enable = 0;
		OmLog("KHONG noi duoc MySQL -> tat tinh nang loi nhan ngoai tuyen (relay van chay binh thuong)");
		return FALSE;
	}
	if (!OmExec("CREATE TABLE IF NOT EXISTS " OM_TABLE " ("
			"id BIGINT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
			"receiver VARBINARY(32) NOT NULL,"
			"sender VARBINARY(32) NOT NULL,"
			"msg VARBINARY(512) NOT NULL,"
			"created DATETIME NOT NULL,"
			"KEY idx_receiver (receiver),"
			"KEY idx_created (created)"
			") ENGINE=InnoDB DEFAULT CHARSET=binary"))
	{
		s_enable = 0;
		OmLog("KHONG tao duoc bang %s -> tat tinh nang", OM_TABLE);
		return FALSE;
	}
	OmPurgeExpired();
	OmLog("MO: %s:%u/%s bang %s, toi da %d loi nhan/nguoi nhan, giu %d ngay",
		s_dbHost, s_dbPort, s_dbName, OM_TABLE, s_maxPerReceiver, s_keepDays);
	return TRUE;
}

void OfflineMsg_Uninit()
{
	if (s_conn) { mysql_close(s_conn); s_conn = NULL; }
	s_enable = 0;
}

BOOL OfflineMsg_Enabled()
{
	return s_enable ? TRUE : FALSE;
}

static long OmCount(const char* receiver)
{
	const char* sql = "SELECT COUNT(*) FROM " OM_TABLE " WHERE receiver=?";
	MYSQL_STMT* st = mysql_stmt_init(s_conn);
	if (!st) return -1;
	long ret = -1;
	if (mysql_stmt_prepare(st, sql, (unsigned long)strlen(sql)) == 0)
	{
		unsigned long lr = (unsigned long)strlen(receiver);
		MYSQL_BIND p[1];
		OmBindBlob(&p[0], receiver, &lr);
		long long cnt = 0;
		MYSQL_BIND r[1];
		memset(r, 0, sizeof(r));
		r[0].buffer_type = MYSQL_TYPE_LONGLONG;
		r[0].buffer = &cnt;
		if (mysql_stmt_bind_param(st, p) == 0 && mysql_stmt_execute(st) == 0
			&& mysql_stmt_bind_result(st, r) == 0 && mysql_stmt_store_result(st) == 0)
		{
			int rc = mysql_stmt_fetch(st);
			if (rc == 0 || rc == MYSQL_DATA_TRUNCATED) ret = (long)cnt;
		}
	}
	if (ret < 0) OmLog("COUNT LOI: %s", mysql_stmt_error(st));
	mysql_stmt_close(st);
	return ret;
}

int OfflineMsg_Store(const char* receiver, const char* sender, const void* msg, int len)
{
	if (!s_enable || !receiver || !sender || !msg) return 0;
	size_t lr = strlen(receiver), ls = strlen(sender);
	if (lr < 1 || lr >= OFFMSG_NAME_MAX || ls < 1 || ls >= OFFMSG_NAME_MAX) return 0;
	if (len < 1 || len > OFFMSG_TEXT_MAX) return 0;
	if (!OmEnsure()) return 0;

	OmPurgeExpired();

	long cnt = OmCount(receiver);
	if (cnt < 0) return 0;
	if (cnt >= s_maxPerReceiver)
	{
		OmLog("hop cua %s DAY (%ld/%d) -> bo loi nhan tu %s", receiver, cnt, s_maxPerReceiver, sender);
		return 0;
	}

	const char* sql = "INSERT INTO " OM_TABLE " (receiver, sender, msg, created) VALUES (?,?,?,NOW())";
	MYSQL_STMT* st = mysql_stmt_init(s_conn);
	if (!st) return 0;
	int ok = 0;
	if (mysql_stmt_prepare(st, sql, (unsigned long)strlen(sql)) == 0)
	{
		unsigned long l0 = (unsigned long)lr, l1 = (unsigned long)ls, l2 = (unsigned long)len;
		MYSQL_BIND p[3];
		OmBindBlob(&p[0], receiver, &l0);
		OmBindBlob(&p[1], sender, &l1);
		OmBindBlob(&p[2], msg, &l2);
		if (mysql_stmt_bind_param(st, p) == 0 && mysql_stmt_execute(st) == 0)
			ok = 1;
	}
	if (!ok) OmLog("INSERT LOI: %s", mysql_stmt_error(st));
	mysql_stmt_close(st);
	if (ok) OmLog("luu loi nhan %s -> %s (%d byte, hop %ld/%d)", sender, receiver, len, cnt + 1, s_maxPerReceiver);
	return ok;
}

int OfflineMsg_Load(const char* receiver, OFFMSG_REC* out, int max)
{
	if (!s_enable || !receiver || !receiver[0] || !out || max <= 0) return 0;
	if (max > OFFMSG_MAX_LOAD) max = OFFMSG_MAX_LOAD;
	if (!OmEnsure()) return 0;

	char sql[320];
	_snprintf(sql, sizeof(sql) - 1,
		"SELECT sender, msg, YEAR(created), MONTH(created), DAY(created), HOUR(created), MINUTE(created) "
		"FROM " OM_TABLE " WHERE receiver=? ORDER BY id LIMIT %d", max);
	sql[sizeof(sql) - 1] = 0;

	MYSQL_STMT* st = mysql_stmt_init(s_conn);
	if (!st) return 0;
	int n = 0;
	if (mysql_stmt_prepare(st, sql, (unsigned long)strlen(sql)) == 0)
	{
		unsigned long lr = (unsigned long)strlen(receiver);
		MYSQL_BIND p[1];
		OmBindBlob(&p[0], receiver, &lr);

		char bufSender[OFFMSG_NAME_MAX + 1];
		char bufMsg[512];
		unsigned long lSender = 0, lMsg = 0;
		my_bool nSender = 0, nMsg = 0;
		int y = 0, mo = 0, d = 0, h = 0, mi = 0;
		MYSQL_BIND r[7];
		memset(r, 0, sizeof(r));
		r[0].buffer_type = MYSQL_TYPE_BLOB; r[0].buffer = bufSender; r[0].buffer_length = OFFMSG_NAME_MAX; r[0].length = &lSender; r[0].is_null = &nSender;
		r[1].buffer_type = MYSQL_TYPE_BLOB; r[1].buffer = bufMsg;    r[1].buffer_length = sizeof(bufMsg); r[1].length = &lMsg;    r[1].is_null = &nMsg;
		r[2].buffer_type = MYSQL_TYPE_LONG; r[2].buffer = &y;
		r[3].buffer_type = MYSQL_TYPE_LONG; r[3].buffer = &mo;
		r[4].buffer_type = MYSQL_TYPE_LONG; r[4].buffer = &d;
		r[5].buffer_type = MYSQL_TYPE_LONG; r[5].buffer = &h;
		r[6].buffer_type = MYSQL_TYPE_LONG; r[6].buffer = &mi;

		if (mysql_stmt_bind_param(st, p) == 0 && mysql_stmt_execute(st) == 0
			&& mysql_stmt_bind_result(st, r) == 0 && mysql_stmt_store_result(st) == 0)
		{
			int rc;
			while (n < max && ((rc = mysql_stmt_fetch(st)) == 0 || rc == MYSQL_DATA_TRUNCATED))
			{
				OFFMSG_REC& rec = out[n];
				memset(&rec, 0, sizeof(rec));
				unsigned long ls = nSender ? 0 : lSender;
				if (ls > OFFMSG_NAME_MAX - 1) ls = OFFMSG_NAME_MAX - 1;
				memcpy(rec.sender, bufSender, ls);
				rec.sender[ls] = 0;
				unsigned long lm = nMsg ? 0 : lMsg;
				if (lm > OFFMSG_TEXT_MAX) lm = OFFMSG_TEXT_MAX;
				if (lm > sizeof(bufMsg)) lm = sizeof(bufMsg);
				memcpy(rec.text, bufMsg, lm);
				rec.text[lm] = 0;
				rec.textlen = (int)lm;
				rec.year = y; rec.mon = mo; rec.day = d; rec.hour = h; rec.minute = mi;
				if (rec.sender[0] && rec.textlen > 0)
					n++;
				lSender = lMsg = 0;
				nSender = nMsg = 0;
			}
		}
		else
			OmLog("SELECT LOI: %s", mysql_stmt_error(st));
	}
	else
		OmLog("prepare SELECT LOI: %s", mysql_stmt_error(st));
	mysql_stmt_close(st);
	return n;
}

BOOL OfflineMsg_Clear(const char* receiver)
{
	if (!s_enable || !receiver || !receiver[0]) return FALSE;
	if (!OmEnsure()) return FALSE;
	const char* sql = "DELETE FROM " OM_TABLE " WHERE receiver=?";
	MYSQL_STMT* st = mysql_stmt_init(s_conn);
	if (!st) return FALSE;
	BOOL ok = FALSE;
	if (mysql_stmt_prepare(st, sql, (unsigned long)strlen(sql)) == 0)
	{
		unsigned long lr = (unsigned long)strlen(receiver);
		MYSQL_BIND p[1];
		OmBindBlob(&p[0], receiver, &lr);
		if (mysql_stmt_bind_param(st, p) == 0 && mysql_stmt_execute(st) == 0)
			ok = TRUE;
	}
	if (!ok) OmLog("DELETE LOI: %s", mysql_stmt_error(st));
	mysql_stmt_close(st);
	return ok;
}
