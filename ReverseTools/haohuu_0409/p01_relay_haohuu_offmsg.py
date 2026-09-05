# -*- coding: ascii -*-
"""p01_relay_haohuu_offmsg.py -- BO VA S3Relay 04/09/2026: HAO HUU LUU DB + LOI NHAN NGOAI TUYEN

Chu bao: "he thong hao huu - cuu nhan - than nhan cua du an: hao huu xong khong luu" va
"khi thoat game nguoi khac nhan tin mat, khi online lai se thong bao tin nhan" (ban Linux co san).

DO DUOC (xem BANGIAO_HAOHUU_OFFMSG_0409.md):
  1. CFriendMgr::SetFriendGroup KHONG bat co dirty, _StoreSomeone/BkgrndUpdateDB chi so cheating
     => doi nhom Than Nhan/Cuu Nhan tren client KHONG BAO GIO xuong DB.
  2. SomeoneLogout khong ghi DB (dong bi chu thich); chi timer 5 phut/8 nguoi + luc tat relay.
  3. GameServer MakeEnemy/MakeBrother gui ten nhom GBK "\263\360\310\313\n"/"\307\327\310\313\n",
     client UiChatCentre chi nhan TCVN3 "C\365u Nh\251n"/"Th\251n Nh\251n" => 93.140 ban ghi cuu nhan
     trong relay_kv (store=friend) VO HINH tren client (FindUnitIndex tra -1).
     Ban Linux doi ten trong relay bang stringtable (K_PR_ENEMY/K_PR_BROTHER) -> o day cung doi trong relay.
  4. Loi nhan mat toi nguoi ngoai tuyen bi vut (codeFail). Linux: KOfflineMsgStore (MySQL OfflineMsg),
     toi da 10 loi nhan/nguoi nhan (s3relay_y 0x80e65dd cmp 9;jg), giu 30 ngay (0x80e5c29 sub 0x278d00).

VA:  FriendMgr.h/.cpp, TongConnect.cpp, ChannelMgr.h/.cpp, ChatConnect.cpp, Global.cpp, S3Relay.vcxproj
MOI: OfflineMsgDB.h, OfflineMsgDB.cpp
Chay lai duoc (idempotent, dau [HAOHUU 04/09]). Chi chen ASCII; so byte cao moi tep KHONG doi.

dung: python p01_relay_haohuu_offmsg.py [<goc worktree>]
"""
import io
import os
import re
import sys

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_haohuu"
RELAY = os.path.join(ROOT, "Sources", "MultiServer", "S3Relay")
MARK = "[HAOHUU 04/09]"


def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f:
        return f.read()


def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f:
        f.write(s)


def eol_of(s):
    return "\r\n" if "\r\n" in s else "\n"


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def patch(name, edits, marker=MARK):
    """edits: [(old, new, count)] -- old/new viet voi '\n', tu doi sang EOL cua tep.
    marker: chuoi chung to tep DA vaa (idempotent); mac dinh la dau [HAOHUU 04/09]."""
    path = os.path.join(RELAY, name)
    d = rd(path)
    if marker in d:
        print("  da va tu truoc, bo qua:", name)
        return
    eol = eol_of(d)
    before = hb(d)
    for old, new, cnt in edits:
        assert all(ord(ch) < 0x80 for ch in new), "ma chen phai ASCII thuan: %s" % name
        o = old.replace("\n", eol)
        n = new.replace("\n", eol)
        c = d.count(o)
        assert c == cnt, "NEO KHONG DUY NHAT (%d != %d) trong %s:\n%r" % (c, cnt, name, old)
        d = d.replace(o, n)
    assert hb(d) == before, "SO BYTE CAO THAY DOI trong %s" % name
    assert marker in d
    wr(path, d)
    print("  da va:", name)


# ---------------------------------------------------------------------------
# 1. TEP MOI: OfflineMsgDB.h / OfflineMsgDB.cpp
# ---------------------------------------------------------------------------
OFFMSG_H = r'''//////////////////////////////////////////////////////////////////////////////
// OfflineMsgDB.h  (S3Relay)  [HAOHUU 04/09]
//
// LOI NHAN MAT CHO NGUOI CHOI NGOAI TUYEN -- port tu S3Relay Linux (s3relay_y):
//   KOfflineMsgStore / KQuery_OfflineMsg, bang MySQL
//   OfflineMsg(ID, Receiver, Sender, Msg, LastModify)
//   - nguoi nhan khong truc tuyen -> luu; khi ho dang nhap -> giao het roi xoa
//   - toi da 10 loi nhan / nguoi nhan   (ma may 0x80e65dd: cmp [ebp-20h],9 ; jg bo)
//   - qua 30 ngay thi xoa                (ma may 0x80e5c29: now - 0x278d00 giay)
// Ket noi MySQL doc o DataBase.ini [relaydb] (cung kho relay_kv). Bang tu tao
// (CREATE TABLE IF NOT EXISTS) luc khoi dong relay. Cau hinh o relay_friendcfg.ini:
//   [offlinemsg]
//   enable         = 1      ; 0 = tat, relay chay nhu cu
//   maxperreceiver = 10     ; mac dinh = ban Linux
//   keepdays       = 30     ; mac dinh = ban Linux
//////////////////////////////////////////////////////////////////////////////
#ifndef OFFLINEMSGDB_H
#define OFFLINEMSGDB_H

#define OFFMSG_MAX_LOAD		64		// tran so loi nhan doc mot lan (>= maxperreceiver)
#define OFFMSG_TEXT_MAX		255		// CHAT_SOMEONECHAT_SYNC.sentlen la BYTE
#define OFFMSG_NAME_MAX		32		// _NAME_LEN

struct OFFMSG_REC
{
	char	sender[OFFMSG_NAME_MAX];
	char	text[OFFMSG_TEXT_MAX + 1];
	int		textlen;
	int		year, mon, day, hour, minute;	// gio may chu MySQL luc luu
};

BOOL	OfflineMsg_Init();		// doc cau hinh, noi MySQL, tao bang, xoa loi nhan het han. Loi -> tu tat tinh nang
void	OfflineMsg_Uninit();
BOOL	OfflineMsg_Enabled();
int		OfflineMsg_Store(const char* receiver, const char* sender, const void* msg, int len);	// 1 = da luu
int		OfflineMsg_Load(const char* receiver, OFFMSG_REC* out, int max);						// so ban ghi doc duoc
BOOL	OfflineMsg_Clear(const char* receiver);												// xoa het cua receiver

#endif
'''

OFFMSG_CPP = r'''//////////////////////////////////////////////////////////////////////////////
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
'''


def write_new(name, body):
    path = os.path.join(RELAY, name)
    assert all(ord(ch) < 0x80 for ch in body)
    if os.path.isfile(path) and MARK in rd(path):
        print("  da co:", name)
        return
    wr(path, body.replace("\n", "\r\n"))
    print("  tao moi:", name)


# ---------------------------------------------------------------------------
# 2. FriendMgr.h / FriendMgr.cpp
# ---------------------------------------------------------------------------
FRIENDMGR_H = [
    ("\tBOOL PlayerAssociateBevy(const _BEVY& bevy, const std::_tstring& group);\n",
     "\tBOOL PlayerAssociateBevy(const _BEVY& bevy, const std::_tstring& group);\n"
     "\n"
     "\t// [HAOHUU 04/09] doi ten nhom GBK cu cua GameServer sang ten don vi ma client hieu (xem FriendMgr.cpp)\n"
     "\tstatic std::_tstring NormalizeGroup(const std::_tstring& group);\n", 1),
]

NORMALIZE_IMPL = r'''#define STR_NULL	_T("")

//////////////////////////////////////////////////////////////////////
// [HAOHUU 04/09] TEN NHOM QUAN HE (unit "\n" nhom con -- client tach o '\n', xem sParseUGName)
// GameServer (Core\Src\KPlayerChat.cpp MakeEnemy/MakeBrother) gui ten don vi bang chu Han GBK
//   "\263\360\310\313\n" (Cuu nhan) va "\307\327\310\313\n" (Than nhan)
// trong khi client (S3Client\Ui\UiCase\UiChatCentre.cpp ENEMY_UNITNAME/BROTHER_UNITNAME) chi co
//   "C\365u Nh\251n" va "Th\251n Nh\251n" (TCVN3) => FindUnitIndex tra -1 => moi cuu nhan/than nhan
// do may chu tao ra deu VO HINH tren client (do 04/09: 93.140 ban ghi nhom GBK trong relay_kv).
// Ban Linux doi ten ngay trong relay (stringtable_relay.txt K_PR_ENEMY/K_PR_BROTHER) -> lam nhu vay:
// doi khi nhan goi (TongConnect) VA khi nap tu DB (_LoadSomeone) de du lieu cu cung hien ra.
//////////////////////////////////////////////////////////////////////
struct LEGACY_GROUP { const char* legacy; const char* client; };
static const LEGACY_GROUP s_LegacyGroups[] =
{
	{ "\263\360\310\313\n",	"C\365u Nh\251n\n" },	// GBK "Cuu nhan"  -> client ENEMY_UNITNAME
	{ "\307\327\310\313\n",	"Th\251n Nh\251n\n" },	// GBK "Than nhan" -> client BROTHER_UNITNAME
};

std::_tstring CFriendMgr::NormalizeGroup(const std::_tstring& group)
{
	for (size_t i = 0; i < sizeof(s_LegacyGroups) / sizeof(s_LegacyGroups[0]); i++)
	{
		const char* legacy = s_LegacyGroups[i].legacy;
		size_t len = strlen(legacy);
		// khop ca "GBK\n" (nhom mac dinh) lan "GBK\n<ten nhom con>"
		if (group.size() >= len && memcmp(group.data(), legacy, len) == 0)
			return std::_tstring(s_LegacyGroups[i].client) + group.substr(len);
	}
	return group;
}
'''

FRIENDMGR_CPP = [
    # (a) NormalizeGroup + bang doi ten
    ("#define STR_NULL	_T(\"\")\n", NORMALIZE_IMPL, 1),

    # (b) SomeoneLogout: ghi DB ngay khi thoat
    ("\t//_StoreSomeone(itPlayer);\n",
     "\t// [HAOHUU 04/09] ghi DB ngay khi thoat. Truoc day chi ghi theo timer (5 phut, toi da 8 nguoi/lan)\n"
     "\t// va luc tat relay => relay sap/kill la mat het thay doi trong khoang do. _StoreSomeone chi ghi khi\n"
     "\t// co thay doi that (dirty / cheating doi) nen khong ton them truy van neu khong co gi moi.\n"
     "\t_StoreSomeone(itPlayer);\n", 1),

    # (c) PlayerAddFriend: dirty + ghi ngay ca hai
    ("\trTRACE(\"Player Add Friend: %s [%s]\", someone.c_str(), dst.c_str());\n",
     "\t// [HAOHUU 04/09] danh dau + ghi DB ngay cho CA HAI phia (truoc day chi trong timer/luc tat)\n"
     "\trSrcPlayerInfo.dirty = TRUE;\n"
     "\trDstPlayerInfo.dirty = TRUE;\n"
     "\t_StoreSomeone(m_mapPlayers.find(someone));\n"
     "\tif (rDstPlayerInfo.loaded)\n"
     "\t\t_StoreSomeone(m_mapPlayers.find(dst));\n"
     "\n"
     "\trTRACE(\"Player Add Friend: %s [%s]\", someone.c_str(), dst.c_str());\n", 1),

    # (d) PlayerDelFriend: xoa mot phia -> ghi ngay
    ("\trSrcInfo.cheating = true;\n\tif (!*rSrcInfo.pCheated)\n\t\treturn TRUE;\n",
     "\trSrcInfo.cheating = true;\n"
     "\trSrcPlayerInfo.dirty = TRUE;\t// [HAOHUU 04/09]\n"
     "\tif (!*rSrcInfo.pCheated)\n"
     "\t{\n"
     "\t\t_StoreSomeone(itSrcPlayer);\t// [HAOHUU 04/09] xoa mot phia: ghi ngay\n"
     "\t\treturn TRUE;\n"
     "\t}\n", 1),
    #     xoa ca hai phia: ban ghi bi erase khoi bo nho -> phai bat dirty, needstore khong tu thay
    ("\t\t\trDstPlayerInfo.friends.erase(someone);\n",
     "\t\t{\n"
     "\t\t\trDstPlayerInfo.friends.erase(someone);\n"
     "\t\t\trDstPlayerInfo.dirty = TRUE;\t// [HAOHUU 04/09] muc da bi xoa khoi map -> so cheating khong con de so\n"
     "\t\t}\n", 1),
    ("\trTRACE(\"Player Del Friend: %s [%s]\", someone.c_str(), dst.c_str());\n",
     "\t// [HAOHUU 04/09] ghi DB ngay cho ca hai phia\n"
     "\t_StoreSomeone(itSrcPlayer);\n"
     "\tif (itDstPlayer != m_mapPlayers.end() && (*itDstPlayer).second.loaded)\n"
     "\t\t_StoreSomeone(itDstPlayer);\n"
     "\n"
     "\trTRACE(\"Player Del Friend: %s [%s]\", someone.c_str(), dst.c_str());\n", 1),

    # (e) SetFriendGroup: GOC "khong luu" -- doi nhom khong bat dirty, needstore chi so cheating
    ("\trInfo.group = group;\n\n\trTRACE(\"Player Group Friend: %s [%s] (%s)\", someone.c_str(), dst.c_str(), group.c_str());\n",
     "\trInfo.group = NormalizeGroup(group);\n"
     "\t// [HAOHUU 04/09] GOC 'doi sang Than Nhan/Cuu Nhan khong luu': khong bat dirty, con needstore trong\n"
     "\t// _StoreSomeone/BkgrndUpdateDB chi so co cheating => nhom KHONG BAO GIO xuong DB. Bat co + ghi ngay.\n"
     "\trPlayerInfo.dirty = TRUE;\n"
     "\t_StoreSomeone(itPlayer);\n"
     "\n"
     "\trTRACE(\"Player Group Friend: %s [%s] (%s)\", someone.c_str(), dst.c_str(), group.c_str());\n", 1),

    # (f) _LoadSomeone: doi ten nhom GBK cu khi nap (2 cho)
    ("\t\t\trInfo.group = rmemFriendRec.group;\n",
     "\t\t\trInfo.group = NormalizeGroup(rmemFriendRec.group);\t// [HAOHUU 04/09] ten nhom GBK cu -> ten client\n"
     "\t\t\tif (rInfo.group != rmemFriendRec.group)\n"
     "\t\t\t\trPlayerInfo.dirty = TRUE;\n", 2),

    # (g) PlayerAssociate / PlayerAssociateBevy: bat dirty (ghi theo logout/timer -- PK bot sinh rat nhieu)
    ("\trTRACE(\"Player Associate: %s [%s] on <%s>, bidir: %u\", someone.c_str(), dst.c_str(), group.c_str(), int(bidir));\n",
     "\trSrcPlayerInfo.dirty = TRUE;\t// [HAOHUU 04/09] ghi luc logout/timer (PK bot sinh hang loat, khong ghi tung cai)\n"
     "\trDstPlayerInfo.dirty = TRUE;\n"
     "\n"
     "\trTRACE(\"Player Associate: %s [%s] on <%s>, bidir: %u\", someone.c_str(), dst.c_str(), group.c_str(), int(bidir));\n", 1),
    ("\t\t\t\trDstInfo.pCheated = &rSrcInfo.cheating;\n",
     "\t\t\t\trDstInfo.pCheated = &rSrcInfo.cheating;\n"
     "\t\t\t\trSrcPlayerInfo.dirty = TRUE;\t// [HAOHUU 04/09]\n"
     "\t\t\t\trDstPlayerInfo.dirty = TRUE;\n", 1),
]

# ---------------------------------------------------------------------------
# 3. TongConnect.cpp: doi ten nhom ngay khi nhan goi tu GameServer / client
# ---------------------------------------------------------------------------
TONGCONNECT = [
    ("\tstd::_tstring group(pGroup);\n",
     "\tstd::_tstring group(CFriendMgr::NormalizeGroup(std::_tstring(pGroup)));\t// [HAOHUU 04/09]\n", 1),
    ("\tstd::_tstring group(szGroup);\n",
     "\tstd::_tstring group(CFriendMgr::NormalizeGroup(std::_tstring(szGroup)));\t// [HAOHUU 04/09] GBK GS -> ten don vi client\n", 1),
    ("\tgroup.assign(szGroup);\n",
     "\tgroup = CFriendMgr::NormalizeGroup(std::_tstring(szGroup));\t// [HAOHUU 04/09] GBK GS -> ten don vi client\n", 1),
]

# ---------------------------------------------------------------------------
# 4. ChannelMgr.h / ChannelMgr.cpp: luu loi nhan khi nguoi nhan ngoai tuyen + giao khi dang nhap
# ---------------------------------------------------------------------------
CHANNELMGR_H = [
    ("\tBOOL SayOnSomeone(DWORD ip, unsigned long param, DWORD nameid, const std::_tstring& name, const std::_tstring& sent);\n",
     "\tBOOL SayOnSomeone(DWORD ip, unsigned long param, DWORD nameid, const std::_tstring& name, const std::_tstring& sent);\n"
     "\tint  DeliverOfflineMsgs(const char* role, DWORD ip, unsigned long param, DWORD nameid);\t// [HAOHUU 04/09] giao loi nhan luu khi ngoai tuyen\n", 1),
]

NOTICE_TCVN3 = (r'[H\326 th\350ng] %s hi\326n kh\253ng tr\371c tuy\325n, l\352i nh\276n \256\267 \256\255\356c '
                r'l\255u v\265 s\317 chuy\323n khi %s \256\250ng nh\313p.')
PREFIX_TCVN3 = r'[L\352i nh\276n l\363c %02d/%02d %02d:%02d] '

DELIVER_IMPL = r'''// [HAOHUU 04/09] Giao loi nhan mat da luu khi nguoi choi ngoai tuyen (port KOfflineMsgStore Linux).
// Goi luc EnterGame tren ket noi chat -- GameServer chi bao relay sau enumPlayerSyncEnd, luc do client
// da UiStartGame() -> KUiMsgCentrePad da mo, nen goi chat_someonechat hien ngay trong cua so chat mat.
// Moi loi nhan = mot goi tu chinh nguoi gui, dau cau co moc thoi gian. Giao du het moi xoa trong DB.
int CChannelMgr::DeliverOfflineMsgs(const char* role, DWORD ip, unsigned long param, DWORD nameid)
{
	if (!role || !role[0] || !OfflineMsg_Enabled())
		return 0;

	OFFMSG_REC recs[OFFMSG_MAX_LOAD];
	int n = OfflineMsg_Load(role, recs, OFFMSG_MAX_LOAD);
	if (n <= 0)
		return 0;

	char buffer[sizeof(CHAT_SOMEONECHAT_SYNC) + MAX_SENTLEN];
	int nSent = 0;
	for (int i = 0; i < n; i++)
	{
		OFFMSG_REC& r = recs[i];
		char szText[MAX_SENTLEN + 64];
		int nPre = _snprintf(szText, 63, "PREFIX_TCVN3", r.day, r.mon, r.hour, r.minute);
		if (nPre < 0) nPre = 0;
		int nLen = r.textlen;
		if (nPre + nLen > (int)MAX_SENTLEN - 1) nLen = (int)MAX_SENTLEN - 1 - nPre;
		if (nLen < 0) nLen = 0;
		memcpy(szText + nPre, r.text, nLen);
		int nTotal = nPre + nLen;

		size_t syncsize = sizeof(CHAT_SOMEONECHAT_SYNC) + nTotal;
		CHAT_SOMEONECHAT_SYNC* pCscSync = (CHAT_SOMEONECHAT_SYNC*)buffer;
		pCscSync->ProtocolType = chat_someonechat;
		pCscSync->wSize = (WORD)(syncsize - 1);
		pCscSync->packageID = -1;
		memset(pCscSync->someone, 0, sizeof(pCscSync->someone));
		strncpy(pCscSync->someone, r.sender, _NAME_LEN - 1);
		pCscSync->sentlen = (BYTE)nTotal;
		memcpy(pCscSync + 1, szText, nTotal);

		if (!PassToSpecMan(ip, param, nameid, pCscSync, syncsize))
			break;
		nSent++;
	}

	if (nSent >= n)
		OfflineMsg_Clear(role);

	rTRACE("[OfflineMsg] giao %d/%d loi nhan cho %s", nSent, n, role);
	return nSent;
}

'''.replace("PREFIX_TCVN3", PREFIX_TCVN3)

STORE_BLOCK = r'''	// [HAOHUU 04/09] nguoi nhan NGOAI TUYEN -> luu loi nhan vao MySQL, giao khi ho dang nhap
	// (port KOfflineMsgStore cua S3Relay Linux). codeStore (co san trong enum) != codeFail nen client
	// van hien cau vua gui trong cua so chat, khong hien "khong co tren mang".
	BOOL stored = FALSE;
	if (!sent && OfflineMsg_Enabled())
	{
		if (OfflineMsg_Store(pSomeoneChatCmd->someone, srcrole.c_str(), pSomeoneChatCmd + 1, pSomeoneChatCmd->sentlen))
			stored = TRUE;
	}

	{{
	size_t _exsize = sizeof(DWORD) + _NAME_LEN + sizeof(BYTE) + pSomeoneChatCmd->sentlen;'''

NOTICE_BLOCK = r'''	memcpy(pSent, pSomeoneChatCmd + 1, pSomeoneChatCmd->sentlen);
	////X

	PassToSpecMan(srcIP, SrcInfo.lnID, SrcInfo.nameid, pCfb, feedbacksize);
	}}

	if (stored)
	{	// [HAOHUU 04/09] bao nguoi gui biet loi nhan da duoc luu -- hien trong chinh cua so chat voi nguoi nhan
		char szNotice[400];
		int nNotice = _snprintf(szNotice, sizeof(szNotice) - 1,
			"NOTICE_TCVN3",
			pSomeoneChatCmd->someone, pSomeoneChatCmd->someone);
		if (nNotice < 0 || nNotice > (int)MAX_SENTLEN - 1) nNotice = (int)MAX_SENTLEN - 1;
		size_t nsize = sizeof(CHAT_SOMEONECHAT_SYNC) + nNotice;
		CHAT_SOMEONECHAT_SYNC* pNt = (CHAT_SOMEONECHAT_SYNC*)_alloca(nsize);
		pNt->ProtocolType = chat_someonechat;
		pNt->wSize = (WORD)(nsize - 1);
		pNt->packageID = -1;
		memset(pNt->someone, 0, sizeof(pNt->someone));
		strncpy(pNt->someone, pSomeoneChatCmd->someone, _NAME_LEN - 1);
		pNt->sentlen = (BYTE)nNotice;
		memcpy(pNt + 1, szNotice, nNotice);
		PassToSpecMan(srcIP, SrcInfo.lnID, SrcInfo.nameid, pNt, nsize);
	}
'''.replace("NOTICE_TCVN3", NOTICE_TCVN3)

CHANNELMGR_CPP = [
    ("#include \"malloc.h\"\n",
     "#include \"malloc.h\"\n#include \"OfflineMsgDB.h\"\t// [HAOHUU 04/09] loi nhan ngoai tuyen\n", 1),
    ("\t{{\n\tsize_t _exsize = sizeof(DWORD) + _NAME_LEN + sizeof(BYTE) + pSomeoneChatCmd->sentlen;",
     STORE_BLOCK, 1),
    ("\tpCfb->packageID = pSomeoneChatCmd->packageID;\n\tpCfb->code = sent ? codeSucc : codeFail;\n",
     "\tpCfb->packageID = pSomeoneChatCmd->packageID;\n"
     "\tpCfb->code = sent ? codeSucc : (stored ? codeStore : codeFail);\t// [HAOHUU 04/09]\n", 1),
    ("\tmemcpy(pSent, pSomeoneChatCmd + 1, pSomeoneChatCmd->sentlen);\n\t////X\n\n\tPassToSpecMan(srcIP, SrcInfo.lnID, SrcInfo.nameid, pCfb, feedbacksize);\n\t}}\n",
     NOTICE_BLOCK, 1),
    ("BOOL CChannelMgr::SayOnSomeone(DWORD ip, unsigned long param, DWORD nameid, const std::_tstring& name, const std::_tstring& sent)\n{\n",
     DELIVER_IMPL + "BOOL CChannelMgr::SayOnSomeone(DWORD ip, unsigned long param, DWORD nameid, const std::_tstring& name, const std::_tstring& sent)\n{\n", 1),
]

# ---------------------------------------------------------------------------
# 5. ChatConnect.cpp: giao loi nhan luc dang nhap (ket noi chat = duong di cua chat mat)
# ---------------------------------------------------------------------------
CHATCONNECT = [
    ("#include \"S3Relay.h\"\n",
     "#include \"S3Relay.h\"\n#include \"OfflineMsgDB.h\"\t// [HAOHUU 04/09]\n", 1),
    ("\tg_FriendMgr.SomeoneLogin(strRole);\n\t}}\n\n\trTRACE(\"Chat:Player Login",
     "\tg_FriendMgr.SomeoneLogin(strRole);\n\t}}\n\n"
     "\t{{\n"
     "\t// [HAOHUU 04/09] giao loi nhan mat nhan duoc trong luc ngoai tuyen (port Linux KOfflineMsgStore)\n"
     "\tg_ChannelMgr.DeliverOfflineMsgs(strRole.c_str(), GetIP(), pEnterGame2->lnID, pEnterGame2->dwNameID);\n"
     "\t}}\n\n"
     "\trTRACE(\"Chat:Player Login", 1),
]

# ---------------------------------------------------------------------------
# 6. Global.cpp: mo/dong kho loi nhan
# ---------------------------------------------------------------------------
GLOBAL_CPP = [
    ("\tif (!g_FriendMgr.Initialize())\n\t\treturn FALSE;\n",
     "\tif (!g_FriendMgr.Initialize())\n\t\treturn FALSE;\n"
     "\t// [HAOHUU 04/09] kho loi nhan ngoai tuyen: loi MySQL thi tu tat tinh nang, relay van chay\n"
     "\tOfflineMsg_Init();\n", 1),
    ("\t\tg_FriendMgr.Uninitialize();\n",
     "\t\tg_FriendMgr.Uninitialize();\n\t\tOfflineMsg_Uninit();\t// [HAOHUU 04/09]\n", 1),
]

# ---------------------------------------------------------------------------
# 7. S3Relay.vcxproj
# ---------------------------------------------------------------------------
VCXPROJ = [
    ("    <ClCompile Include=\"_neo_libdb.cpp\" />\n",
     "    <ClCompile Include=\"_neo_libdb.cpp\" />\n    <ClCompile Include=\"OfflineMsgDB.cpp\" />\n", 1),
    ("    <ClInclude Include=\"FriendMgr.h\" />\n",
     "    <ClInclude Include=\"FriendMgr.h\" />\n    <ClInclude Include=\"OfflineMsgDB.h\" />\n", 1),
]


def patch_global_include():
    """Global.cpp: chen #include sau dong #include cuoi cung cua khoi include dau tep."""
    path = os.path.join(RELAY, "Global.cpp")
    d = rd(path)
    if "OfflineMsgDB.h" in d:
        return
    eol = eol_of(d)
    before = hb(d)
    lines = d.split(eol)
    last = -1
    for i, l in enumerate(lines[:120]):
        if l.startswith("#include"):
            last = i
    assert last >= 0, "Global.cpp: khong thay #include o dau tep"
    lines.insert(last + 1, "#include \"OfflineMsgDB.h\"\t// [HAOHUU 04/09] loi nhan ngoai tuyen")
    d = eol.join(lines)
    assert hb(d) == before
    wr(path, d)
    print("  da chen include: Global.cpp")


def main():
    assert os.path.isdir(RELAY), RELAY
    print("goc:", ROOT)
    write_new("OfflineMsgDB.h", OFFMSG_H)
    write_new("OfflineMsgDB.cpp", OFFMSG_CPP)
    patch("FriendMgr.h", FRIENDMGR_H)
    patch("FriendMgr.cpp", FRIENDMGR_CPP)
    patch("TongConnect.cpp", TONGCONNECT)
    patch("ChannelMgr.h", CHANNELMGR_H)
    patch("ChannelMgr.cpp", CHANNELMGR_CPP)
    patch("ChatConnect.cpp", CHATCONNECT)
    patch("Global.cpp", GLOBAL_CPP, marker="OfflineMsg_Init();")
    patch_global_include()
    patch("S3Relay.vcxproj", VCXPROJ, marker="OfflineMsgDB.cpp")
    print("XONG")


if __name__ == "__main__":
    main()
