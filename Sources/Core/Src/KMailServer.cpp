// KMailServer.cpp - [MAIL 03/09] kho THU tren MySQL cho may chu (chu chot: "Luu thu bang bang MySQL").
// Web admin / hoat dong / top tuan-thang / dua top ghi thang vao bang `mail` (state 0); may chu doc bang
// nay khi nguoi choi dang nhap, mo hop thu (NPC Tin Su) va do moi 30 giay (script\mail\mailpoll.lua).
// Moi tham so di qua prepared statement cua KMySQLDB (ten nhan vat la byte TCVN3 tho -> VARBINARY).
// Doc DONG BO (Query) chi khi nguoi choi thao tac (it, co index); ghi trang thai dung Post (bat dong bo),
// rieng nhan dinh kem / xoa dung Exec dong bo de kiem affected_rows (chong nhan hai lan).
//
// CREATE TABLE IF NOT EXISTS mail (
//   id INT AUTO_INCREMENT PRIMARY KEY,
//   role_name VARBINARY(32) NOT NULL,
//   sender VARBINARY(64) NOT NULL DEFAULT '',
//   title VARBINARY(128) NOT NULL DEFAULT '',
//   content BLOB,                       -- toi da 2048 byte khi gui xuong client, <enter> = xuong dong
//   award VARCHAR(512) NOT NULL DEFAULT '',   -- "item:genre,detail,particular,level,series,luck,count;money:N;xu:N;exp:N"
//   award_count INT NOT NULL DEFAULT 0,
//   state TINYINT NOT NULL DEFAULT 0,   -- 0 chua nhan, 1 chua doc, 2 da doc, 3 da nhan dinh kem, 4 da xoa
//   send_time INT NOT NULL DEFAULT 0,   -- unix
//   expire_time INT NOT NULL DEFAULT 0, -- unix, 0 = khong het han
//   source VARBINARY(32) NOT NULL DEFAULT '',
//   KEY idx_role_state (role_name, state),
//   KEY idx_state_id (state, id)
// ) ENGINE=InnoDB DEFAULT CHARSET=latin1
#include "KCore.h"
#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KMailServer.h"
#ifdef _SERVER
#include "KMySQLDB.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAILDB_MAX_MAIL		100		// MAILDEF.PLAYER_MAX_MAIL cua 2.0
#define MAILDB_DEF_EXPIRE	(30 * 86400)
// [CL 04/09 V2] tran do dai cot `award` VARCHAR(512); de 480 cho du cho dau ';'
// cuoi va cho nguoi goi noi them mot muc ngan ma khong cham tran that.
#define MAILDB_MAX_AWARD	480

static bool s_bTableOk = false;

static bool sEnsureTable()
{
	if (!g_MySQLDB.IsReady())
		return false;
	if (s_bTableOk)
		return true;
	const char* szDDL =
		"CREATE TABLE IF NOT EXISTS mail ("
		" id INT AUTO_INCREMENT PRIMARY KEY,"
		" role_name VARBINARY(32) NOT NULL,"
		" sender VARBINARY(64) NOT NULL DEFAULT '',"
		" title VARBINARY(128) NOT NULL DEFAULT '',"
		" content BLOB,"
		" award VARCHAR(512) NOT NULL DEFAULT '',"
		" award_count INT NOT NULL DEFAULT 0,"
		" state TINYINT NOT NULL DEFAULT 0,"
		" send_time INT NOT NULL DEFAULT 0,"
		" expire_time INT NOT NULL DEFAULT 0,"
		" source VARBINARY(32) NOT NULL DEFAULT '',"
		" KEY idx_role_state (role_name, state),"
		" KEY idx_state_id (state, id)"
		") ENGINE=InnoDB DEFAULT CHARSET=latin1";
	s_bTableOk = g_MySQLDB.Exec(szDDL, 0, 0);
	if (!s_bTableOk)
		g_DebugLog((LPSTR)"[MAIL] khong tao duoc bang mail");
	return s_bTableOk;
}

static const char* sArgStr(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsString(L, n)) ? Lua_ValueToString(L, n) : "";
}

static int sArgInt(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsNumber(L, n)) ? (int)Lua_ValueToNumber(L, n) : 0;
}

static std::string sCol(const KDBRow& row, int c)
{
	if (c < 0 || c >= row.nCol || !row.pVal[c] || row.pLen[c] <= 0)
		return std::string();
	return std::string(row.pVal[c], row.pLen[c]);
}

static int sColInt(const KDBRow& row, int c)
{
	std::string s = sCol(row, c);
	return s.empty() ? 0 : atoi(s.c_str());
}

// dat truong chuoi / so vao bang dang o dinh stack
static void sSetStr(Lua_State* L, const char* szKey, const std::string& s)
{
	Lua_PushString(L, (char*)szKey);
	Lua_PushString(L, (char*)s.c_str());
	Lua_SetTable(L, -3);
}

static void sSetNum(Lua_State* L, const char* szKey, double v)
{
	Lua_PushString(L, (char*)szKey);
	Lua_PushNumber(L, v);
	Lua_SetTable(L, -3);
}

struct KMailRow
{
	int			nId;
	std::string	sRole;
	std::string	sSender;
	std::string	sTitle;
	std::string	sContent;
	std::string	sAward;
	int			nAwardCount;
	int			nState;
	int			nSend;
	int			nExpire;
};

static bool _RowHeader(const KDBRow& row, void* p)
{
	// id, sender, title, state, send_time, expire_time, award_count
	std::vector<KMailRow>* pv = (std::vector<KMailRow>*)p;
	KMailRow r;
	r.nId = sColInt(row, 0);
	r.sSender = sCol(row, 1);
	r.sTitle = sCol(row, 2);
	r.nState = sColInt(row, 3);
	r.nSend = sColInt(row, 4);
	r.nExpire = sColInt(row, 5);
	r.nAwardCount = sColInt(row, 6);
	pv->push_back(r);
	return true;
}

static bool _RowFull(const KDBRow& row, void* p)
{
	// id, sender, title, content, award, award_count, state, send_time, expire_time
	std::vector<KMailRow>* pv = (std::vector<KMailRow>*)p;
	KMailRow r;
	r.nId = sColInt(row, 0);
	r.sSender = sCol(row, 1);
	r.sTitle = sCol(row, 2);
	r.sContent = sCol(row, 3);
	r.sAward = sCol(row, 4);
	r.nAwardCount = sColInt(row, 5);
	r.nState = sColInt(row, 6);
	r.nSend = sColInt(row, 7);
	r.nExpire = sColInt(row, 8);
	pv->push_back(r);
	return true;
}

static bool _RowIdRole(const KDBRow& row, void* p)
{
	std::vector<KMailRow>* pv = (std::vector<KMailRow>*)p;
	KMailRow r;
	r.nId = sColInt(row, 0);
	r.sRole = sCol(row, 1);
	r.nAwardCount = 0;
	r.nState = 0;
	r.nSend = 0;
	r.nExpire = 0;
	pv->push_back(r);
	return true;
}

struct KMailCount { __int64 n; };
static bool _RowCount(const KDBRow& row, void* p)
{
	((KMailCount*)p)->n = (row.nCol > 0 && row.pLen[0] > 0) ? _atoi64(row.pVal[0]) : 0;
	return true;
}

int LuaMailDB_Ready(Lua_State* L)
{
	Lua_PushNumber(L, sEnsureTable() ? 1 : 0);
	return 1;
}

// [MAIL 04/09 D10] loi C++ (ham Lua ben duoi chi boc tham so). Xem KMailServer.h.
int Mail_Send(const char* szRole, const char* szSender, const char* szTitle, const char* szContent,
	const char* szAward, int nAwardCount, int nExpireSec, const char* szSource)
{
	if (!szRole || !szRole[0] || !sEnsureTable())
		return 0;
	if (!szSender) szSender = "";
	if (!szTitle) szTitle = "";
	if (!szContent) szContent = "";
	if (!szAward) szAward = "";
	if (!szSource) szSource = "";
	// [CL 04/09 V2] cot `award` la VARCHAR(512). Voi sql_mode STRICT_TRANS_TABLES thi
	// chuoi dai hon lam HONG CA CAU INSERT (thu khong den tay ai) chu khong cat bot.
	// Chan som va tra 0 de nguoi goi biet, thay vi de MySQL tu choi khong loi giai thich.
	if ((int)strlen(szAward) > MAILDB_MAX_AWARD)
		return 0;
	int nNow = (int)time(NULL);
	int nExpire = (nExpireSec > 0) ? nNow + nExpireSec : nNow + MAILDB_DEF_EXPIRE;
	// tran 100 thu: danh dau xoa thu CU NHAT (ly do OVERFLOW cua 2.0)
	KMailCount c;
	c.n = 0;
	KDBParam pc[1];
	pc[0] = KDBParam::S(szRole);
	if (g_MySQLDB.Query("SELECT COUNT(*) FROM mail WHERE role_name=? AND state<4", pc, 1, _RowCount, &c)
		&& c.n >= MAILDB_MAX_MAIL)
	{
		// [CL 04/09 V2] CHI duoc cat thu KHONG con dinh kem (award_count = 0).
		// Truoc day cau nay khong nhin award_count nen no cat ca thu con vat pham
		// CHUA NHAN: nguoi choi it vao game, hom thu day (du an dang gui thu tu 8
		// nguon), mot thu thuong moi day vao la GIET mot thu con do. Neu khong con
		// thu trong nao de cat thi TU CHOI GUI va tra 0 - nguoi goi thu lai duoc,
		// con do cua nguoi choi thi khong lay lai duoc.
		__int64 nCut = 0;
		g_MySQLDB.Exec("UPDATE mail SET state=4 WHERE role_name=? AND state<4 AND award_count=0"
			" ORDER BY id LIMIT 1", pc, 1, &nCut);
		if (nCut < 1)
			return 0;
	}
	KDBParam p[9];
	p[0] = KDBParam::S(szRole);
	p[1] = KDBParam::S(szSender);
	p[2] = KDBParam::S(szTitle);
	p[3] = KDBParam::S(szContent);
	p[4] = KDBParam::S(szAward);
	p[5] = KDBParam::I(nAwardCount);
	p[6] = KDBParam::I(nNow);
	p[7] = KDBParam::I(nExpire);
	p[8] = KDBParam::S(szSource);
	__int64 nInsertId = 0;
	bool bOk = g_MySQLDB.Exec(
		"INSERT INTO mail (role_name, sender, title, content, award, award_count, state, send_time, expire_time, source)"
		" VALUES (?, ?, ?, ?, ?, ?, 0, ?, ?, ?)", p, 9, 0, &nInsertId);
	return bOk ? (int)nInsertId : 0;
}

// MailDB_Send(szRole, szSender, szTitle, szContent, szAward, nAwardCount, nExpireSec, szSource) -> id / 0
int LuaMailDB_Send(Lua_State* L)
{
	int nId = Mail_Send(sArgStr(L, 1), sArgStr(L, 2), sArgStr(L, 3), sArgStr(L, 4),
		sArgStr(L, 5), sArgInt(L, 6), sArgInt(L, 7), sArgStr(L, 8));
	Lua_PushNumber(L, (double)nId);
	return 1;
}

// MailDB_Headers(szRole, nMinId, nMax) -> bang; thu het han bi danh dau xoa (bat dong bo) va bo qua;
// thu state 0 tra ve thi doi sang 1 (client da nhan header).
int LuaMailDB_Headers(Lua_State* L)
{
	const char* szRole = sArgStr(L, 1);
	int nMinId = sArgInt(L, 2);
	int nMax = sArgInt(L, 3);
	if (nMax <= 0 || nMax > MAILDB_MAX_MAIL)
		nMax = MAILDB_MAX_MAIL;
	Lua_NewTable(L);
	if (!szRole[0] || !sEnsureTable())
		return 1;
	std::vector<KMailRow> rows;
	KDBParam p[3];
	p[0] = KDBParam::S(szRole);
	p[1] = KDBParam::I(nMinId);
	p[2] = KDBParam::I(nMax);
	if (!g_MySQLDB.Query(
			"SELECT id, sender, title, state, send_time, expire_time, award_count FROM mail"
			" WHERE role_name=? AND state<4 AND id>? ORDER BY id LIMIT ?", p, 3, _RowHeader, &rows))
		return 1;
	int nNow = (int)time(NULL);
	int n = 0;
	for (size_t i = 0; i < rows.size(); i++)
	{
		KMailRow& r = rows[i];
		KDBParam q[1];
		q[0] = KDBParam::I(r.nId);
		if (r.nExpire > 0 && r.nExpire < nNow)
		{
			g_MySQLDB.Post("UPDATE mail SET state=4 WHERE id=? AND state<4", q, 1);
			continue;
		}
		if (r.nState == 0)
		{
			g_MySQLDB.Post("UPDATE mail SET state=1 WHERE id=? AND state=0", q, 1);
			r.nState = 1;
		}
		Lua_NewTable(L);
		sSetNum(L, "id", r.nId);
		sSetStr(L, "sender", r.sSender);
		sSetStr(L, "title", r.sTitle);
		sSetNum(L, "state", r.nState);
		sSetNum(L, "send", r.nSend);
		sSetNum(L, "expire", r.nExpire);
		sSetNum(L, "award_count", r.nAwardCount);
		Lua_RawSetI(L, -2, ++n);
	}
	return 1;
}

// MailDB_Get(szRole, nId) -> bang | nil
int LuaMailDB_Get(Lua_State* L)
{
	const char* szRole = sArgStr(L, 1);
	int nId = sArgInt(L, 2);
	if (!szRole[0] || nId <= 0 || !sEnsureTable())
	{
		Lua_PushNil(L);
		return 1;
	}
	std::vector<KMailRow> rows;
	KDBParam p[2];
	p[0] = KDBParam::S(szRole);
	p[1] = KDBParam::I(nId);
	if (!g_MySQLDB.Query(
			"SELECT id, sender, title, content, award, award_count, state, send_time, expire_time FROM mail"
			" WHERE role_name=? AND id=? AND state<4", p, 2, _RowFull, &rows) || rows.empty())
	{
		Lua_PushNil(L);
		return 1;
	}
	KMailRow& r = rows[0];
	Lua_NewTable(L);
	sSetNum(L, "id", r.nId);
	sSetStr(L, "sender", r.sSender);
	sSetStr(L, "title", r.sTitle);
	sSetStr(L, "content", r.sContent);
	sSetStr(L, "award", r.sAward);
	sSetNum(L, "award_count", r.nAwardCount);
	sSetNum(L, "state", r.nState);
	sSetNum(L, "send", r.nSend);
	sSetNum(L, "expire", r.nExpire);
	return 1;
}

// MailDB_SetState(szRole, nId, nState, nBelow) -> 1 neu doi duoc (dong bo, nguyen tu)
int LuaMailDB_SetState(Lua_State* L)
{
	const char* szRole = sArgStr(L, 1);
	int nId = sArgInt(L, 2);
	int nState = sArgInt(L, 3);
	int nBelow = sArgInt(L, 4);
	if (!szRole[0] || nId <= 0 || !sEnsureTable())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KDBParam p[4];
	p[0] = KDBParam::I(nState);
	p[1] = KDBParam::I(nId);
	p[2] = KDBParam::S(szRole);
	p[3] = KDBParam::I(nBelow);
	__int64 nAffected = 0;
	bool bOk = g_MySQLDB.Exec("UPDATE mail SET state=? WHERE id=? AND role_name=? AND state<?", p, 4, &nAffected);
	Lua_PushNumber(L, (bOk && nAffected == 1) ? 1 : 0);
	return 1;
}

// MailDB_Delete(szRole, nId) -> 1/0
int LuaMailDB_Delete(Lua_State* L)
{
	const char* szRole = sArgStr(L, 1);
	int nId = sArgInt(L, 2);
	if (!szRole[0] || nId <= 0 || !sEnsureTable())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KDBParam p[2];
	p[0] = KDBParam::I(nId);
	p[1] = KDBParam::S(szRole);
	__int64 nAffected = 0;
	bool bOk = g_MySQLDB.Exec("UPDATE mail SET state=4 WHERE id=? AND role_name=? AND state<4", p, 2, &nAffected);
	Lua_PushNumber(L, (bOk && nAffected == 1) ? 1 : 0);
	return 1;
}

// MailDB_Count(szRole) -> so thu con song
int LuaMailDB_Count(Lua_State* L)
{
	const char* szRole = sArgStr(L, 1);
	if (!szRole[0] || !sEnsureTable())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KMailCount c;
	c.n = 0;
	KDBParam p[1];
	p[0] = KDBParam::S(szRole);
	g_MySQLDB.Query("SELECT COUNT(*) FROM mail WHERE role_name=? AND state<4", p, 1, _RowCount, &c);
	Lua_PushNumber(L, (double)c.n);
	return 1;
}

// MailDB_PollNew(nAfterId, nMax) -> bang {[i] = {id, role}} thu state 0 co id > nAfterId
int LuaMailDB_PollNew(Lua_State* L)
{
	int nAfter = sArgInt(L, 1);
	int nMax = sArgInt(L, 2);
	if (nMax <= 0 || nMax > 200)
		nMax = 50;
	Lua_NewTable(L);
	if (!sEnsureTable())
		return 1;
	std::vector<KMailRow> rows;
	KDBParam p[2];
	p[0] = KDBParam::I(nAfter);
	p[1] = KDBParam::I(nMax);
	if (!g_MySQLDB.Query("SELECT id, role_name FROM mail WHERE state=0 AND id>? ORDER BY id LIMIT ?",
			p, 2, _RowIdRole, &rows))
		return 1;
	for (size_t i = 0; i < rows.size(); i++)
	{
		Lua_NewTable(L);
		sSetNum(L, "id", rows[i].nId);
		sSetStr(L, "role", rows[i].sRole);
		Lua_RawSetI(L, -2, (int)i + 1);
	}
	return 1;
}

// MailDB_MaxId() -> id lon nhat (0 neu rong)
int LuaMailDB_MaxId(Lua_State* L)
{
	KMailCount c;
	c.n = 0;
	if (sEnsureTable())
		g_MySQLDB.Query("SELECT IFNULL(MAX(id), 0) FROM mail", 0, 0, _RowCount, &c);
	Lua_PushNumber(L, (double)c.n);
	return 1;
}

// MailDB_Sweep() -> danh dau het han (bat dong bo)
int LuaMailDB_Sweep(Lua_State* L)
{
	if (sEnsureTable())
	{
		KDBParam p[1];
		p[0] = KDBParam::I((int)time(NULL));
		g_MySQLDB.Post("UPDATE mail SET state=4 WHERE state<4 AND expire_time>0 AND expire_time<?", p, 1);
	}
	Lua_PushNumber(L, 0);
	return 1;
}

#endif // _SERVER
