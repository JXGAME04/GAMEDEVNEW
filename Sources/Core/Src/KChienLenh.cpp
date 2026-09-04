//////////////////////////////////////////////////////////////////////////////
// KChienLenh.cpp - [CL 04/09] He CHIEN LENH tren MySQL. Xem KChienLenh.h.
//
// DOT 1a: tao 7 bang + nap cau hinh + 14 luat kiem + ghi nguoc st_cfg_log.
// (Bo dem trong RAM, 5 cho moc va duong trao thuong lam o dot 1b.)
//
// BA LUAT HIEU NANG (do that tren may chu nay, MySQL 5.7.44):
//   ghi cam ket 2,5 ms | doc theo chi muc 0,08 ms | nhip game 55,5 ms/khung
//   -> 22 lan ghi DONG BO trong mot khung la DUNG MAY.
//   1. KHONG BAO GIO ghi CSDL theo su kien chien dau -> dem RAM, xa theo moc.
//   2. Trong luc choi chi dung Post() (bat dong bo). Query/Exec chi o: luc khoi
//      dong (nap cau hinh - tep nay), luc bam mo cua so, luc nhan thuong.
//   3. Chu tieng Viet trong cot VARBINARY la byte TCVN3 tho; CHI sua qua trang
//      admin, KHONG go thang phpMyAdmin (go UTF-8 vao do thi MySQL nhan, khong
//      canh bao gi, ma trong game hien "Chi<rac>n L<rac>nh").
//
// NAP LAI: web tang st_config['cfg_version'] sau moi lan luu; tep nay do khoa do
// (mot cau SELECT 0,08 ms) roi nap lai TOAN BO neu so doi. Cau hinh moi chi duoc
// HOAN DOI khi qua het 14 luat kiem; hong thi GIU NGUYEN ban cu va ghi st_cfg_log.
//////////////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KChienLenh.h"

#ifdef _SERVER
#include "KMySQLDB.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KMailServer.h"	// Mail_Send: trao thuong qua he thu
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////////////
// 1. BAY BANG - GAME tao, WEB chi doc/ghi noi dung
//////////////////////////////////////////////////////////////////////////////

static bool s_bTableOk = false;

static const char* s_szDDL[] =
{
	// --- mua giai: CHI MOT dong duoc active = 1 ---
	"CREATE TABLE IF NOT EXISTS st_season ("
	" id INT AUTO_INCREMENT PRIMARY KEY,"
	" name VARBINARY(64) NOT NULL DEFAULT '',"
	" start_time INT NOT NULL DEFAULT 0,"
	" close_time INT NOT NULL DEFAULT 0,"
	" grace_days INT NOT NULL DEFAULT 7,"
	" level_score INT NOT NULL DEFAULT 400,"
	" vip_bonus INT NOT NULL DEFAULT 1200,"
	" vip_price_xu INT NOT NULL DEFAULT 500,"
	" reset_hour TINYINT NOT NULL DEFAULT 5,"
	" active TINYINT NOT NULL DEFAULT 0,"
	" KEY idx_active (active)"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1",

	// --- 24 moc thuong; cot award dung Y NGUYEN ngu phap cot mail.award ---
	"CREATE TABLE IF NOT EXISTS st_award ("
	" season_id INT NOT NULL,"
	" idx INT NOT NULL,"
	" need_score INT NOT NULL DEFAULT 0,"
	" branch TINYINT NOT NULL DEFAULT 0,"
	" award VARCHAR(512) NOT NULL DEFAULT '',"
	" award_count INT NOT NULL DEFAULT 0,"
	" mail_title VARBINARY(128) NOT NULL DEFAULT '',"
	" mail_content BLOB,"
	" note VARBINARY(128) NOT NULL DEFAULT '',"
	" PRIMARY KEY (season_id, idx)"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1",

	// --- 20 nhiem vu; id 1..20 GAN CUNG voi cho moc trong ma may chu ---
	"CREATE TABLE IF NOT EXISTS st_mission ("
	" season_id INT NOT NULL,"
	" id INT NOT NULL,"
	" kind TINYINT NOT NULL DEFAULT 2,"
	" score INT NOT NULL DEFAULT 0,"
	" target INT NOT NULL DEFAULT 1,"
	" title VARBINARY(160) NOT NULL DEFAULT '',"
	" tips BLOB,"
	" enabled TINYINT NOT NULL DEFAULT 1,"
	" PRIMARY KEY (season_id, id)"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1",

	// --- trang thai nguoi choi ---
	"CREATE TABLE IF NOT EXISTS st_player ("
	" season_id INT NOT NULL,"
	" role_name VARBINARY(32) NOT NULL,"
	" score INT NOT NULL DEFAULT 0,"
	" vip TINYINT NOT NULL DEFAULT 0,"
	" got_low INT NOT NULL DEFAULT 0,"
	" got_vip INT NOT NULL DEFAULT 0,"
	" day_key INT NOT NULL DEFAULT 0,"
	" week_key INT NOT NULL DEFAULT 0,"
	" day_state BIGINT NOT NULL DEFAULT 0,"
	" week_state BIGINT NOT NULL DEFAULT 0,"
	" prog VARBINARY(255) NOT NULL DEFAULT '',"
	" updated_at INT NOT NULL DEFAULT 0,"
	" PRIMARY KEY (season_id, role_name)"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1",

	// --- so cai: UNIQUE KEY la thu DUY NHAT chong nhan hai lan ---
	"CREATE TABLE IF NOT EXISTS st_ledger ("
	" id INT AUTO_INCREMENT PRIMARY KEY,"
	" season_id INT NOT NULL,"
	" role_name VARBINARY(32) NOT NULL,"
	" idx INT NOT NULL,"
	" branch TINYINT NOT NULL DEFAULT 0,"
	" award VARCHAR(512) NOT NULL DEFAULT '',"
	" mail_id INT NOT NULL DEFAULT 0,"
	" ts INT NOT NULL DEFAULT 0,"
	" UNIQUE KEY uk_claim (season_id, role_name, idx, branch)"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1",

	// --- co nap lai ---
	"CREATE TABLE IF NOT EXISTS st_config ("
	" k VARBINARY(32) NOT NULL PRIMARY KEY,"
	" v VARBINARY(255) NOT NULL DEFAULT '',"
	" updated_at INT NOT NULL DEFAULT 0"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1",

	// --- nhat ky cau hinh: MAY CHU ghi, WEB doc ---
	"CREATE TABLE IF NOT EXISTS st_cfg_log ("
	" id INT AUTO_INCREMENT PRIMARY KEY,"
	" ts INT NOT NULL DEFAULT 0,"
	" level TINYINT NOT NULL DEFAULT 0,"
	" msg VARBINARY(255) NOT NULL DEFAULT ''"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1",
};

bool ChienLenh_EnsureTables()
{
	if (!g_MySQLDB.IsReady())
		return false;
	if (s_bTableOk)
		return true;
	int nSo = (int)(sizeof(s_szDDL) / sizeof(s_szDDL[0]));
	for (int i = 0; i < nSo; i++)
	{
		if (!g_MySQLDB.Exec(s_szDDL[i], 0, 0))
		{
			g_DebugLog((LPSTR)"[CL] khong tao duoc bang st_* thu %d", i + 1);
			return false;
		}
	}
	// khoa cfg_version phai co san de web INCREMENT duoc ngay lan luu dau
	g_MySQLDB.Exec("INSERT IGNORE INTO st_config (k, v, updated_at) VALUES ('cfg_version', '0', 0)", 0, 0);
	s_bTableOk = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// 2. GHI NGUOC NHAT KY - vi sai du lieu trong MySQL thi IM LANG
//////////////////////////////////////////////////////////////////////////////

void ChienLenh_Log(int nLevel, const char* szMsg)
{
	if (!szMsg || !szMsg[0])
		return;
	g_DebugLog((LPSTR)"[CL] %s", szMsg);
	if (!g_MySQLDB.IsReady() || !s_bTableOk)
		return;
	KDBParam p[3];
	p[0] = KDBParam::I((__int64)time(NULL));
	p[1] = KDBParam::I(nLevel);
	p[2] = KDBParam::S(szMsg);
	// Post = bat dong bo: ghi nhat ky KHONG BAO GIO duoc chan vong lap game
	g_MySQLDB.Post("INSERT INTO st_cfg_log (ts, level, msg) VALUES (?, ?, ?)", p, 3);
}

//////////////////////////////////////////////////////////////////////////////
// 3. CAU HINH TRONG BO NHO
//////////////////////////////////////////////////////////////////////////////

struct KCLSeason
{
	int nId;
	int nStart;
	int nClose;
	int nGraceDays;
	int nLevelScore;
	int nVipBonus;
	int nVipPriceXu;
	int nResetHour;
	std::string sName;
	KCLSeason() : nId(0), nStart(0), nClose(0), nGraceDays(7), nLevelScore(400),
		nVipBonus(1200), nVipPriceXu(500), nResetHour(5) {}
};

struct KCLAward
{
	int nIdx;
	int nNeedScore;
	int nBranch;			// 0 = Thuong (mien phi), 1 = Hao Hoa (VIP)
	int nAwardCount;
	std::string sAward;		// ngu phap y het cot mail.award
	std::string sTitle;
	std::string sContent;
	KCLAward() : nIdx(0), nNeedScore(0), nBranch(0), nAwardCount(0) {}
};

struct KCLMission
{
	int nId;
	int nKind;				// 1 = tuan, 2 = ngay
	int nScore;
	int nTarget;
	int nEnabled;
	std::string sTitle;
	std::string sTips;
	KCLMission() : nId(0), nKind(2), nScore(0), nTarget(1), nEnabled(0) {}
};

static KCLSeason				s_Season;
static std::vector<KCLAward>	s_Award;
static KCLMission				s_Mission[CL_MAX_MISSION + 1];	// 1..20
static int						s_nCfgVer = -1;
static bool						s_bCfgOk = false;

//////////////////////////////////////////////////////////////////////////////
// 4. HAM PHU
//////////////////////////////////////////////////////////////////////////////

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

static const char* sArgStr(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsString(L, n)) ? Lua_ValueToString(L, n) : "";
}

static int sArgInt(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsNumber(L, n)) ? (int)Lua_ValueToNumber(L, n) : 0;
}

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

// Do chuoi co lot byte UTF-8 khong (chu go nham o phpMyAdmin thay vi qua trang
// admin). TCVN3 la MOT byte moi chu; UTF-8 tieng Viet luon la cap 0xC3/0xE1 + byte
// tiep 0x80..0xBF. Chi CANH BAO, khong tu choi - de chu con nhin thay chu ma sua.
static bool sCoMuiUtf8(const std::string& s)
{
	for (size_t i = 0; i + 1 < s.size(); i++)
	{
		unsigned char a = (unsigned char)s[i];
		unsigned char b = (unsigned char)s[i + 1];
		if ((a == 0xC3 || a == 0xC4 || a == 0xC5 || a == 0xE1) && b >= 0x80 && b <= 0xBF)
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// 5. DOC BANG
//////////////////////////////////////////////////////////////////////////////

struct KCLIntBox { int n; bool bCo; };

static bool _RowInt(const KDBRow& row, void* p)
{
	((KCLIntBox*)p)->n = sColInt(row, 0);
	((KCLIntBox*)p)->bCo = true;
	return true;
}

struct KCLSeasonBox { std::vector<KCLSeason> v; };

static bool _RowSeason(const KDBRow& row, void* p)
{
	KCLSeason s;
	s.nId			= sColInt(row, 0);
	s.sName			= sCol(row, 1);
	s.nStart		= sColInt(row, 2);
	s.nClose		= sColInt(row, 3);
	s.nGraceDays	= sColInt(row, 4);
	s.nLevelScore	= sColInt(row, 5);
	s.nVipBonus		= sColInt(row, 6);
	s.nVipPriceXu	= sColInt(row, 7);
	s.nResetHour	= sColInt(row, 8);
	((KCLSeasonBox*)p)->v.push_back(s);
	return true;
}

struct KCLAwardBox { std::vector<KCLAward> v; };

static bool _RowAward(const KDBRow& row, void* p)
{
	KCLAward a;
	a.nIdx			= sColInt(row, 0);
	a.nNeedScore	= sColInt(row, 1);
	a.nBranch		= sColInt(row, 2);
	a.sAward		= sCol(row, 3);
	a.nAwardCount	= sColInt(row, 4);
	a.sTitle		= sCol(row, 5);
	a.sContent		= sCol(row, 6);
	((KCLAwardBox*)p)->v.push_back(a);
	return true;
}

struct KCLMissionBox { std::vector<KCLMission> v; };

static bool _RowMission(const KDBRow& row, void* p)
{
	KCLMission m;
	m.nId		= sColInt(row, 0);
	m.nKind		= sColInt(row, 1);
	m.nScore	= sColInt(row, 2);
	m.nTarget	= sColInt(row, 3);
	m.sTitle	= sCol(row, 4);
	m.sTips		= sCol(row, 5);
	m.nEnabled	= sColInt(row, 6);
	((KCLMissionBox*)p)->v.push_back(m);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// 6. MUOI BON LUAT KIEM
//
// Nguyen tac: hong thi GIU NGUYEN cau hinh cu (thay vi chay nua voi) va ghi ly do
// vao st_cfg_log de trang admin hien ra. Sai du lieu o MySQL khong co ScriptError
// nao keu, nen day la kenh bao loi DUY NHAT.
//////////////////////////////////////////////////////////////////////////////

// Cac nhom vat pham co nhanh sinh THAT trong KItemSet::AddItemSet2.
// Nhom 2 (item_mine) va 3 (item_materials) la case RONG - khong sinh duoc mon nao,
// nen tu choi tu day thay vi de nguoi choi bam Nhan roi khong co gi vao tui.
static bool sGenreTraoDuoc(int g)
{
	return (g == 0 || g == 1 || g == 4 || g == 5 || g == 6 || g == 8 || g == 9);
}

// Kiem NGU PHAP mot chuoi `award` (dung ngu phap cot mail.award).
// Tra so muc dem duoc; -1 = hong. szLoi nhan mo ta ngan.
static int sKiemChuoiAward(const std::string& s, char* szLoi, int nLoiMax)
{
	szLoi[0] = 0;
	if (s.empty())
	{
		_snprintf(szLoi, nLoiMax - 1, "chuoi award rong");
		return -1;
	}
	if ((int)s.size() > 480)
	{
		_snprintf(szLoi, nLoiMax - 1, "award dai %d byte (tran 480)", (int)s.size());
		return -1;
	}
	int nMuc = 0;
	size_t i = 0;
	while (i < s.size())
	{
		size_t j = s.find(';', i);
		if (j == std::string::npos)
			j = s.size();
		std::string m = s.substr(i, j - i);
		i = j + 1;
		// bo khoang trang hai dau
		while (!m.empty() && (m[0] == ' ' || m[0] == '\t')) m.erase(0, 1);
		while (!m.empty() && (m[m.size() - 1] == ' ' || m[m.size() - 1] == '\t')) m.erase(m.size() - 1);
		if (m.empty())
			continue;
		size_t k = m.find(':');
		if (k == std::string::npos)
		{
			_snprintf(szLoi, nLoiMax - 1, "muc '%.40s' khong co dau hai cham", m.c_str());
			return -1;
		}
		std::string sTen = m.substr(0, k);
		std::string sGt = m.substr(k + 1);
		if (sTen == "item")
		{
			int g = atoi(sGt.c_str());
			if (!sGenreTraoDuoc(g))
			{
				_snprintf(szLoi, nLoiMax - 1, "item nhom %d khong trao duoc (chi 0,1,4,5,6,8,9)", g);
				return -1;
			}
			// so am o bat ky truong nao la dau hieu go nham: particular am lam
			// KBPT_MagicScript::GetRecord boc NGAU NHIEN mot dong trong bang.
			if (sGt.find('-') != std::string::npos)
			{
				_snprintf(szLoi, nLoiMax - 1, "item co so AM: '%.40s'", sGt.c_str());
				return -1;
			}
		}
		else if (sTen == "gold" || sTen == "aucitem" || sTen == "task" ||
			sTen == "money" || sTen == "xu" || sTen == "exp" || sTen == "repute")
		{
			if (sGt.find('-') != std::string::npos)
			{
				_snprintf(szLoi, nLoiMax - 1, "%s co so AM: '%.40s'", sTen.c_str(), sGt.c_str());
				return -1;
			}
		}
		else
		{
			_snprintf(szLoi, nLoiMax - 1, "tien to la '%.20s'", sTen.c_str());
			return -1;
		}
		nMuc++;
	}
	if (nMuc == 0)
	{
		_snprintf(szLoi, nLoiMax - 1, "khong co muc nao hop le");
		return -1;
	}
	return nMuc;
}

//////////////////////////////////////////////////////////////////////////////
// 7. NAP LAI CAU HINH
//////////////////////////////////////////////////////////////////////////////

int ChienLenh_Reload()
{
	char szLog[256];
	if (!ChienLenh_EnsureTables())
		return 1;

	int nLoi = 0;

	// ---- LUAT 1: dung MOT mua active = 1 ----
	KCLSeasonBox sb;
	if (!g_MySQLDB.Query(
		"SELECT id, name, start_time, close_time, grace_days, level_score,"
		" vip_bonus, vip_price_xu, reset_hour FROM st_season WHERE active=1",
		0, 0, _RowSeason, &sb))
	{
		ChienLenh_Log(CL_LOG_LOI, "khong doc duoc bang st_season");
		return 1;
	}
	if (sb.v.empty())
	{
		ChienLenh_Log(CL_LOG_TIN, "chua co mua nao dang chay (st_season.active=1) - Chien Lenh tam tat");
		s_bCfgOk = false;
		return 0;		// khong phai loi: chu chua mo mua
	}
	if (sb.v.size() > 1)
	{
		_snprintf(szLog, sizeof(szLog) - 1, "LOI: co %d mua cung active=1, chi duoc MOT", (int)sb.v.size());
		ChienLenh_Log(CL_LOG_LOI, szLog);
		return 1;
	}
	KCLSeason mua = sb.v[0];

	// ---- LUAT 2, 3 ----
	if (mua.nClose <= mua.nStart)
	{
		_snprintf(szLog, sizeof(szLog) - 1,
			"LOI mua %d: close_time (%d) phai LON HON start_time (%d)", mua.nId, mua.nClose, mua.nStart);
		ChienLenh_Log(CL_LOG_LOI, szLog);
		nLoi++;
	}
	if (mua.nLevelScore <= 0)
	{
		_snprintf(szLog, sizeof(szLog) - 1, "LOI mua %d: level_score = %d, phai > 0", mua.nId, mua.nLevelScore);
		ChienLenh_Log(CL_LOG_LOI, szLog);
		nLoi++;
	}
	if (mua.nResetHour < 0 || mua.nResetHour > 23)
	{
		_snprintf(szLog, sizeof(szLog) - 1, "LOI mua %d: reset_hour = %d, phai 0..23", mua.nId, mua.nResetHour);
		ChienLenh_Log(CL_LOG_LOI, szLog);
		nLoi++;
	}

	// ---- doc bang thuong ----
	KCLAwardBox ab;
	KDBParam p[1];
	p[0] = KDBParam::I(mua.nId);
	if (!g_MySQLDB.Query(
		"SELECT idx, need_score, branch, award, award_count, mail_title, mail_content"
		" FROM st_award WHERE season_id=? ORDER BY idx", p, 1, _RowAward, &ab))
	{
		ChienLenh_Log(CL_LOG_LOI, "khong doc duoc bang st_award");
		return 1;
	}

	// ---- LUAT 4: idx lien tuc 1..N, khong trung, khong trong ----
	if (ab.v.empty())
	{
		ChienLenh_Log(CL_LOG_CANHBAO, "mua dang chay chua co moc thuong nao (st_award rong)");
	}
	for (size_t i = 0; i < ab.v.size(); i++)
	{
		if (ab.v[i].nIdx != (int)i + 1)
		{
			_snprintf(szLog, sizeof(szLog) - 1,
				"LOI st_award: idx phai lien tuc 1..%d, dong thu %d co idx = %d",
				(int)ab.v.size(), (int)i + 1, ab.v[i].nIdx);
			ChienLenh_Log(CL_LOG_LOI, szLog);
			nLoi++;
			break;
		}
	}
	if ((int)ab.v.size() > CL_MAX_AWARD)
	{
		_snprintf(szLog, sizeof(szLog) - 1, "LOI st_award: %d moc, tran la %d", (int)ab.v.size(), CL_MAX_AWARD);
		ChienLenh_Log(CL_LOG_LOI, szLog);
		nLoi++;
	}

	for (size_t i = 0; i < ab.v.size() && (int)i < CL_MAX_AWARD; i++)
	{
		KCLAward& a = ab.v[i];
		// ---- LUAT 5: need_score TANG DAN NGHIEM NGAT ----
		if (i > 0 && a.nNeedScore <= ab.v[i - 1].nNeedScore)
		{
			_snprintf(szLog, sizeof(szLog) - 1,
				"LOI moc %d: need_score %d khong lon hon moc truoc (%d) - phai tang dan",
				a.nIdx, a.nNeedScore, ab.v[i - 1].nNeedScore);
			ChienLenh_Log(CL_LOG_LOI, szLog);
			nLoi++;
		}
		// ---- LUAT 6: branch 0 hoac 1 ----
		if (a.nBranch != 0 && a.nBranch != 1)
		{
			_snprintf(szLog, sizeof(szLog) - 1,
				"LOI moc %d: branch = %d, chi duoc 0 (Thuong) hoac 1 (Hao Hoa)", a.nIdx, a.nBranch);
			ChienLenh_Log(CL_LOG_LOI, szLog);
			nLoi++;
		}
		// ---- LUAT 7, 8, 9: ngu phap + nhom vat pham + so am ----
		// [CL 04/09 DOT2c] moc CHUA CO thuong (award rong) chi CANH BAO, khong lam hong ca cau hinh:
		// chu tao du 24 moc tren web truoc roi dien thuong sau (14:19 nap bi tu choi vi 24 moc rong).
		// Moc rong van hien tren cua so (khong bieu tuong), sNhanMoc tra 3 = khong cho nhan toi khi dien.
		char szLoi[128];
		int nMuc = a.sAward.empty() ? 0 : sKiemChuoiAward(a.sAward, szLoi, sizeof(szLoi));
		if (a.sAward.empty())
		{
			_snprintf(szLog, sizeof(szLog) - 1, "CANH BAO moc %d: chua co thuong (award rong) - van nap, dien tren web sau", a.nIdx);
			ChienLenh_Log(CL_LOG_CANHBAO, szLog);
		}
		else if (nMuc < 0)
		{
			_snprintf(szLog, sizeof(szLog) - 1, "LOI moc %d: %s", a.nIdx, szLoi);
			ChienLenh_Log(CL_LOG_LOI, szLog);
			nLoi++;
		}
		else
		{
			// ---- LUAT 11: award_count phai KHOP so muc dem duoc ----
			if (a.nAwardCount != nMuc)
			{
				_snprintf(szLog, sizeof(szLog) - 1,
					"LOI moc %d: award_count = %d nhung dem duoc %d muc", a.nIdx, a.nAwardCount, nMuc);
				ChienLenh_Log(CL_LOG_LOI, szLog);
				nLoi++;
			}
		}
		// ---- LUAT 13: canh bao go nham bang ma (UTF-8 lot vao cot TCVN3) ----
		if (sCoMuiUtf8(a.sTitle) || sCoMuiUtf8(a.sContent))
		{
			_snprintf(szLog, sizeof(szLog) - 1,
				"CANH BAO moc %d: tieu de/noi dung co dau hieu UTF-8 - phai la byte TCVN3,"
				" hay sua qua trang admin chu dung go thang phpMyAdmin", a.nIdx);
			ChienLenh_Log(CL_LOG_CANHBAO, szLog);
		}
	}

	// ---- doc bang nhiem vu ----
	KCLMissionBox mb;
	if (!g_MySQLDB.Query(
		"SELECT id, kind, score, target, title, tips, enabled"
		" FROM st_mission WHERE season_id=? ORDER BY id", p, 1, _RowMission, &mb))
	{
		ChienLenh_Log(CL_LOG_LOI, "khong doc duoc bang st_mission");
		return 1;
	}

	// ---- LUAT 12: id 1..20, kind 1/2, score >= 0, target >= 1 ----
	for (size_t i = 0; i < mb.v.size(); i++)
	{
		KCLMission& m = mb.v[i];
		if (m.nId < 1 || m.nId > CL_MAX_MISSION)
		{
			_snprintf(szLog, sizeof(szLog) - 1,
				"LOI st_mission: id = %d nam ngoai 1..%d. Moi id GAN CUNG voi mot cho moc"
				" trong ma may chu - them dong moi thi khong gi cong diem cho no",
				m.nId, CL_MAX_MISSION);
			ChienLenh_Log(CL_LOG_LOI, szLog);
			nLoi++;
			continue;
		}
		if (m.nKind != 1 && m.nKind != 2)
		{
			_snprintf(szLog, sizeof(szLog) - 1,
				"LOI nhiem vu %d: kind = %d, chi duoc 1 (tuan) hoac 2 (ngay)", m.nId, m.nKind);
			ChienLenh_Log(CL_LOG_LOI, szLog);
			nLoi++;
		}
		if (m.nScore < 0)
		{
			_snprintf(szLog, sizeof(szLog) - 1, "LOI nhiem vu %d: score = %d, phai >= 0", m.nId, m.nScore);
			ChienLenh_Log(CL_LOG_LOI, szLog);
			nLoi++;
		}
		if (m.nTarget < 1)
		{
			_snprintf(szLog, sizeof(szLog) - 1, "LOI nhiem vu %d: target = %d, phai >= 1", m.nId, m.nTarget);
			ChienLenh_Log(CL_LOG_LOI, szLog);
			nLoi++;
		}
		if (sCoMuiUtf8(m.sTitle) || sCoMuiUtf8(m.sTips))
		{
			_snprintf(szLog, sizeof(szLog) - 1,
				"CANH BAO nhiem vu %d: tieu de/chu giai co dau hieu UTF-8 (phai la TCVN3)", m.nId);
			ChienLenh_Log(CL_LOG_CANHBAO, szLog);
		}
	}

	// ---- LUAT 14: doi thuong cua moc DA CO NGUOI NHAN -> canh bao do ----
	for (size_t i = 0; i < ab.v.size() && (int)i < CL_MAX_AWARD; i++)
	{
		if (s_bCfgOk && (int)i < (int)s_Award.size() &&
			s_Award[i].nIdx == ab.v[i].nIdx && s_Award[i].sAward != ab.v[i].sAward)
		{
			KCLIntBox c;
			c.n = 0;
			c.bCo = false;
			KDBParam pc[2];
			pc[0] = KDBParam::I(mua.nId);
			pc[1] = KDBParam::I(ab.v[i].nIdx);
			g_MySQLDB.Query("SELECT COUNT(*) FROM st_ledger WHERE season_id=? AND idx=?",
				pc, 2, _RowInt, &c);
			if (c.n > 0)
			{
				_snprintf(szLog, sizeof(szLog) - 1,
					"CANH BAO moc %d: doi thuong trong khi DA CO %d nguoi nhan."
					" Nguoi nhan truoc giu thuong CU, nguoi sau nhan thuong MOI."
					" Trong mot mua bang thuong nen BAT BIEN - muon doi thi mo mua moi",
					ab.v[i].nIdx, (int)c.n);
				ChienLenh_Log(CL_LOG_CANHBAO, szLog);
			}
		}
	}

	// ---- hong thi GIU NGUYEN ban cu ----
	if (nLoi > 0)
	{
		_snprintf(szLog, sizeof(szLog) - 1,
			"TU CHOI nap cau hinh moi: %d loi. Van dung ban cu (%s).",
			nLoi, s_bCfgOk ? "da co ban cu" : "CHUA CO ban nao - Chien Lenh tat");
		ChienLenh_Log(CL_LOG_LOI, szLog);
		return nLoi;
	}

	// ---- hoan doi ----
	s_Season = mua;
	s_Award.clear();
	for (size_t i = 0; i < ab.v.size() && (int)i < CL_MAX_AWARD; i++)
		s_Award.push_back(ab.v[i]);
	for (int i = 0; i <= CL_MAX_MISSION; i++)
		s_Mission[i] = KCLMission();
	for (size_t i = 0; i < mb.v.size(); i++)
	{
		if (mb.v[i].nId >= 1 && mb.v[i].nId <= CL_MAX_MISSION)
			s_Mission[mb.v[i].nId] = mb.v[i];
	}
	s_bCfgOk = true;

	_snprintf(szLog, sizeof(szLog) - 1,
		"da nap cau hinh mua %d: %d moc thuong, %d nhiem vu, %d diem moi cap",
		mua.nId, (int)s_Award.size(), (int)mb.v.size(), mua.nLevelScore);
	ChienLenh_Log(CL_LOG_TIN, szLog);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// 8. LOI DUNG CHUNG
//////////////////////////////////////////////////////////////////////////////

bool ChienLenh_DangMo()
{
	if (!s_bCfgOk)
		return false;
	int nNay = (int)time(NULL);
	return (nNay >= s_Season.nStart && nNay < s_Season.nClose);
}

int ChienLenh_CapTuDiem(int nScore)
{
	if (!s_bCfgOk || s_Season.nLevelScore <= 0)
		return 0;
	if (nScore < 0)
		return 0;
	return nScore / s_Season.nLevelScore;
}

int ChienLenh_SoMoc()
{
	return s_bCfgOk ? (int)s_Award.size() : 0;
}

//////////////////////////////////////////////////////////////////////////////
// 9. HAM LUA
//////////////////////////////////////////////////////////////////////////////

// [CL 04/09 DOT2b] TU NAP cau hinh: lan dau (s_nCfgVer == -1) hoac khi web tang st_config['cfg_version'].
// Truoc day chi lenh GM "Nap lai" moi goi ChienLenh_Reload -> may chu chay ca ngay ma Chien Lenh van "tat"
// (st_cfg_log trong, khong co bieu tuong). Mot cau doc theo khoa chinh 0,08 ms nen goi o CL_Ready / CL_Load /
// CL_Tick deu re. bEp = 1: ep nap bat ke phien ban (lenh GM).
static int sTuNapLai(int bEp)
{
	if (!ChienLenh_EnsureTables())
		return 1;
	int nMoi = s_nCfgVer;
	if (!bEp)
	{
		KCLIntBox c;
		c.n = 0;
		c.bCo = false;
		g_MySQLDB.Query("SELECT v FROM st_config WHERE k='cfg_version'", 0, 0, _RowInt, &c);
		if (c.bCo)
			nMoi = c.n;
		if (nMoi == s_nCfgVer && s_nCfgVer != -1)
			return 0;		// khong doi, khong lam gi
	}
	int nLoi = ChienLenh_Reload();
	s_nCfgVer = nMoi;		// ghi ca khi loi: bao loi MOT lan moi phien ban, web sua xong tang so la nap lai
	if (s_nCfgVer == -1)
		s_nCfgVer = 0;
	return nLoi;
}

// CL_Ready() -> 1 neu bang da co VA cau hinh da nap
int LuaCL_Ready(Lua_State* L)
{
	sTuNapLai(0);
	Lua_PushNumber(L, s_bCfgOk ? 1 : 0);
	return 1;
}

// CL_Reload() -> so loi (0 = da nhan ban moi). Nap lai neu cfg_version doi,
// hoac ep nap khi truyen tham so khac 0.
int LuaCL_Reload(Lua_State* L)
{
	int bEp = sArgInt(L, 1);
	int nLoi = sTuNapLai(bEp);	// [CL 04/09 DOT2b] cung mot duong voi nap tu dong
	Lua_PushNumber(L, nLoi);
	return 1;
}

// CL_Info() -> bang trang thai (script va lenh GM dung de xem nhanh)
int LuaCL_Info(Lua_State* L)
{
	Lua_NewTable(L);
	sSetNum(L, "ready", s_bCfgOk ? 1 : 0);
	sSetNum(L, "dangmo", ChienLenh_DangMo() ? 1 : 0);
	sSetNum(L, "cfg_ver", s_nCfgVer);
	sSetNum(L, "season_id", s_Season.nId);
	sSetStr(L, "season_name", s_Season.sName);
	sSetNum(L, "start_time", s_Season.nStart);
	sSetNum(L, "close_time", s_Season.nClose);
	sSetNum(L, "grace_days", s_Season.nGraceDays);
	sSetNum(L, "level_score", s_Season.nLevelScore);
	sSetNum(L, "vip_bonus", s_Season.nVipBonus);
	sSetNum(L, "vip_price_xu", s_Season.nVipPriceXu);
	sSetNum(L, "reset_hour", s_Season.nResetHour);
	sSetNum(L, "so_moc", (int)s_Award.size());
	return 1;
}

// CL_Mission(nId) -> bang | nil
int LuaCL_Mission(Lua_State* L)
{
	int nId = sArgInt(L, 1);
	if (!s_bCfgOk || nId < 1 || nId > CL_MAX_MISSION || s_Mission[nId].nId != nId)
		return 0;
	KCLMission& m = s_Mission[nId];
	Lua_NewTable(L);
	sSetNum(L, "id", m.nId);
	sSetNum(L, "kind", m.nKind);
	sSetNum(L, "score", m.nScore);
	sSetNum(L, "target", m.nTarget);
	sSetNum(L, "enabled", m.nEnabled);
	sSetStr(L, "title", m.sTitle);
	sSetStr(L, "tips", m.sTips);
	return 1;
}

// CL_Award(nIdx) -> bang | nil
int LuaCL_Award(Lua_State* L)
{
	int nIdx = sArgInt(L, 1);
	if (!s_bCfgOk || nIdx < 1 || nIdx > (int)s_Award.size())
		return 0;
	KCLAward& a = s_Award[nIdx - 1];
	Lua_NewTable(L);
	sSetNum(L, "idx", a.nIdx);
	sSetNum(L, "need_score", a.nNeedScore);
	sSetNum(L, "cap", ChienLenh_CapTuDiem(a.nNeedScore));
	sSetNum(L, "branch", a.nBranch);
	sSetNum(L, "award_count", a.nAwardCount);
	sSetStr(L, "award", a.sAward);
	sSetStr(L, "title", a.sTitle);
	sSetStr(L, "content", a.sContent);
	return 1;
}


//////////////////////////////////////////////////////////////////////////////
// 10. [DOT 1b] TRANG THAI NGUOI CHOI - DEM TRONG RAM, XA THEO MOC
//
// LUAT 1 cua he: KHONG BAO GIO ghi CSDL theo su kien chien dau. Do that tren may
// nay: mot cau ghi cam ket 2,5 ms, luong ghi nen chi xa duoc 399 dong/giay, hang
// doi 20.000 muc day thi BO GOI IM LANG. Voi 1000 bot dang chay, ghi moi lan giet
// quai la chet. Nen: cong don trong RAM, chi ghi khi (a) vuot mot moc thuong,
// (b) dang xuat, (c) hen gio 5 phut.
//
// Cac cho MOC deu dat o LUA (mot dong CL_Cong o canh ma hoat dong) chu khong sua
// KNpc.cpp / ScriptFuns.cpp. Ly do:
//   - KNpc.cpp:1837 da co chu thich "ham nay chay MOI LAN co NPC chet - dung them
//     viec nang", va no cung la tep ma phien khac dang sua (tranh khe swap);
//   - de o Lua thi `grep CL_Cong` trong cay script ra duoc: thang sau ai doi map
//     Tong Kim hay doi so hieu task se NHIN THAY Chien Lenh, khong lam hong cam.
//////////////////////////////////////////////////////////////////////////////

struct KCLPlayer
{
	int		bLoaded;
	int		nSeasonId;
	int		nScore;
	int		nVip;
	int		nGotLow;
	int		nGotVip;
	int		nDayKey;		// yyyymmdd cua lan reset ngay gan nhat
	int		nWeekKey;		// yyyymmdd cua thu Hai tuan hien tai
	__int64	nDayState;		// 2 bit moi nhiem vu: bit chan = xong, bit le = da linh
	__int64	nWeekState;
	int		nProg[CL_MAX_MISSION + 1];
	int		bDirty;
	int		nLastFlush;
};

static KCLPlayer s_Pl[MAX_PLAYER];

// 2 bit moi nhiem vu, danh so theo CHINH id nhiem vu (1..20) -> 40 bit, vua __int64
#define CL_BIT_XONG(id)		(((__int64)1) << (((id) - 1) * 2))
#define CL_BIT_LINH(id)		(((__int64)1) << (((id) - 1) * 2 + 1))

//////////////////////////////////////////////////////////////////////////////
// 11. MOC NGAY / TUAN
//
// Dung SO NGAY TUYET DOI dang yyyymmdd va chi reset khi so MOI LON HON so da luu.
// Neu so sanh bang "khac nhau" (nhu ban 2.0) thi dong ho may chu LUI mot ngay -
// chinh tay, dong bo NTP sai, hay phuc hoi anh may ao - la nguoi choi cay lai
// duoc mot vong nhiem vu ngay, va diem thi cong don khong reset.
//////////////////////////////////////////////////////////////////////////////

static int sNgayGame(int nNow, int nResetHour)
{
	time_t t = (time_t)(nNow - nResetHour * 3600);
	struct tm* p = localtime(&t);
	if (!p)
		return 0;
	return (p->tm_year + 1900) * 10000 + (p->tm_mon + 1) * 100 + p->tm_mday;
}

static int sTuanGame(int nNow, int nResetHour)
{
	time_t t = (time_t)(nNow - nResetHour * 3600);
	struct tm* p = localtime(&t);
	if (!p)
		return 0;
	int nThu = (p->tm_wday + 6) % 7;	// 0 = thu Hai
	t -= (time_t)nThu * 86400;
	struct tm* q = localtime(&t);
	if (!q)
		return 0;
	return (q->tm_year + 1900) * 10000 + (q->tm_mon + 1) * 100 + q->tm_mday;
}

//////////////////////////////////////////////////////////////////////////////
// 12. NAP / XA TRANG THAI
//////////////////////////////////////////////////////////////////////////////

struct KCLPlBox { KCLPlayer* p; bool bCo; };

static bool _RowPlayer(const KDBRow& row, void* pv)
{
	KCLPlBox* b = (KCLPlBox*)pv;
	KCLPlayer* p = b->p;
	p->nScore		= sColInt(row, 0);
	p->nVip			= sColInt(row, 1);
	p->nGotLow		= sColInt(row, 2);
	p->nGotVip		= sColInt(row, 3);
	p->nDayKey		= sColInt(row, 4);
	p->nWeekKey		= sColInt(row, 5);
	p->nDayState	= _atoi64(sCol(row, 6).c_str());
	p->nWeekState	= _atoi64(sCol(row, 7).c_str());
	// cot prog: "id:so,id:so,..."
	std::string s = sCol(row, 8);
	size_t i = 0;
	while (i < s.size())
	{
		size_t j = s.find(',', i);
		if (j == std::string::npos)
			j = s.size();
		std::string m = s.substr(i, j - i);
		i = j + 1;
		size_t k = m.find(':');
		if (k == std::string::npos)
			continue;
		int nId = atoi(m.substr(0, k).c_str());
		int nSo = atoi(m.substr(k + 1).c_str());
		if (nId >= 1 && nId <= CL_MAX_MISSION)
			p->nProg[nId] = nSo;
	}
	b->bCo = true;
	return true;
}

// Nap trang thai cua mot nguoi choi. Goi luc dang nhap (mot cau doc 0,08 ms).
static bool sNapNguoiChoi(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER || !s_bCfgOk)
		return false;
	KCLPlayer& p = s_Pl[nIdx];
	memset(&p, 0, sizeof(p));
	p.nSeasonId = s_Season.nId;
	p.bLoaded = 1;
	p.nLastFlush = (int)time(NULL);
	if (!ChienLenh_EnsureTables())
		return false;
	KCLPlBox box;
	box.p = &p;
	box.bCo = false;
	KDBParam q[2];
	q[0] = KDBParam::I(s_Season.nId);
	q[1] = KDBParam::S(Player[nIdx].m_PlayerName);
	g_MySQLDB.Query(
		"SELECT score, vip, got_low, got_vip, day_key, week_key, day_state, week_state, prog"
		" FROM st_player WHERE season_id=? AND role_name=?", q, 2, _RowPlayer, &box);
	return true;
}

// Xa trang thai xuong CSDL. Dung Post() (bat dong bo) nen KHONG chan vong lap game.
static void sXaNguoiChoi(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return;
	KCLPlayer& p = s_Pl[nIdx];
	if (!p.bLoaded || !p.bDirty || !s_bCfgOk)
		return;
	if (!ChienLenh_EnsureTables())
		return;

	char szProg[256];
	szProg[0] = 0;
	int nLen = 0;
	for (int i = 1; i <= CL_MAX_MISSION; i++)
	{
		if (p.nProg[i] <= 0)
			continue;
		char szTmp[32];
		int n = _snprintf(szTmp, sizeof(szTmp) - 1, "%s%d:%d", nLen ? "," : "", i, p.nProg[i]);
		if (n <= 0 || nLen + n >= (int)sizeof(szProg) - 1)
			break;
		strcpy(szProg + nLen, szTmp);
		nLen += n;
	}

	KDBParam q[12];
	q[0]  = KDBParam::I(p.nSeasonId);
	q[1]  = KDBParam::S(Player[nIdx].m_PlayerName);
	q[2]  = KDBParam::I(p.nScore);
	q[3]  = KDBParam::I(p.nVip);
	q[4]  = KDBParam::I(p.nGotLow);
	q[5]  = KDBParam::I(p.nGotVip);
	q[6]  = KDBParam::I(p.nDayKey);
	q[7]  = KDBParam::I(p.nWeekKey);
	q[8]  = KDBParam::I(p.nDayState);
	q[9]  = KDBParam::I(p.nWeekState);
	q[10] = KDBParam::S(szProg);
	q[11] = KDBParam::I((__int64)time(NULL));
	g_MySQLDB.Post(
		"INSERT INTO st_player (season_id, role_name, score, vip, got_low, got_vip,"
		" day_key, week_key, day_state, week_state, prog, updated_at)"
		" VALUES (?,?,?,?,?,?,?,?,?,?,?,?)"
		" ON DUPLICATE KEY UPDATE score=VALUES(score), vip=VALUES(vip),"
		" got_low=VALUES(got_low), got_vip=VALUES(got_vip), day_key=VALUES(day_key),"
		" week_key=VALUES(week_key), day_state=VALUES(day_state),"
		" week_state=VALUES(week_state), prog=VALUES(prog), updated_at=VALUES(updated_at)",
		q, 12);
	p.bDirty = 0;
	p.nLastFlush = (int)time(NULL);
}

//////////////////////////////////////////////////////////////////////////////
// 13. RESET NGAY / TUAN
//////////////////////////////////////////////////////////////////////////////

static void sKiemReset(int nIdx)
{
	KCLPlayer& p = s_Pl[nIdx];
	if (!p.bLoaded || !s_bCfgOk)
		return;
	int nNow = (int)time(NULL);
	int nNgay = sNgayGame(nNow, s_Season.nResetHour);
	int nTuan = sTuanGame(nNow, s_Season.nResetHour);

	// CHI reset khi moc MOI LON HON moc da luu (khong dung "khac nhau")
	if (nNgay > p.nDayKey)
	{
		p.nDayKey = nNgay;
		for (int i = 1; i <= CL_MAX_MISSION; i++)
		{
			if (s_Mission[i].nId == i && s_Mission[i].nKind == 2)
			{
				p.nDayState &= ~CL_BIT_XONG(i);
				p.nDayState &= ~CL_BIT_LINH(i);
				p.nProg[i] = 0;
			}
		}
		p.bDirty = 1;
	}
	if (nTuan > p.nWeekKey)
	{
		p.nWeekKey = nTuan;
		for (int i = 1; i <= CL_MAX_MISSION; i++)
		{
			if (s_Mission[i].nId == i && s_Mission[i].nKind == 1)
			{
				p.nWeekState &= ~CL_BIT_XONG(i);
				p.nWeekState &= ~CL_BIT_LINH(i);
				p.nProg[i] = 0;
			}
		}
		p.bDirty = 1;
	}
}

//////////////////////////////////////////////////////////////////////////////
// 14. CONG TIEN DO / DANH DAU XONG
//////////////////////////////////////////////////////////////////////////////

extern int PB_IsBot(int nPlayerIdx);

// Chi so nguoi choi hien tai. Dung DUNG khuon cua KAuctionServer.cpp:491-497:
// GetPlayerIndex() nam trong ScriptFuns.cpp va KHONG duoc khai o header nao,
// nen moi tep tu doc bien toan cuc Lua SCRIPT_PLAYERINDEX.
static int sChiSoNguoiChoi(Lua_State* L)
{
	int nPlayerIdx = 0;
	lua_getglobal(L, SCRIPT_PLAYERINDEX);
	if (lua_isnumber(L, -1))
		nPlayerIdx = (int)lua_tonumber(L, -1);
	lua_settop(L, -2);
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return 0;
	return nPlayerIdx;
}

static __int64* sTruongTrangThai(KCLPlayer& p, int nId)
{
	return (s_Mission[nId].nKind == 1) ? &p.nWeekState : &p.nDayState;
}

// Cong nSo vao tien do nhiem vu nId; du muc tieu thi danh dau XONG.
// Tra 1 neu vua chuyen sang trang thai xong.
static int sCong(int nIdx, int nId, int nSo)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER || nId < 1 || nId > CL_MAX_MISSION)
		return 0;
	if (!s_bCfgOk || !ChienLenh_DangMo())
		return 0;
	// CAU DAU TIEN: bo qua bot. 1000 bot dang chay se lam phinh ban ghi va
	// chiem cho trong moi bang neu khong chan o day.
	if (PB_IsBot(nIdx))
		return 0;
	KCLPlayer& p = s_Pl[nIdx];
	if (!p.bLoaded)
		return 0;
	KCLMission& m = s_Mission[nId];
	if (m.nId != nId || !m.nEnabled)
		return 0;
	sKiemReset(nIdx);
	__int64* pTr = sTruongTrangThai(p, nId);
	if ((*pTr) & CL_BIT_XONG(nId))
		return 0;						// da xong roi
	if (nSo > 0)
	{
		p.nProg[nId] += nSo;
		p.bDirty = 1;
	}
	if (p.nProg[nId] >= m.nTarget)
	{
		*pTr |= CL_BIT_XONG(nId);
		p.bDirty = 1;
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// 15. TRAO THUONG - SO CAI TRUOC, THU SAU
//
// UNIQUE KEY (season_id, role_name, idx, branch) tren st_ledger la thu DUY NHAT
// chong nhan hai lan. Nen thu tu BAT BUOC la:
//   1. INSERT so cai  -> trung khoa = da nhan roi = tu choi (khong trao gi)
//   2. chi khi INSERT AN thi moi Mail_Send
// Neu lam nguoc (trao truoc, ghi sau) thi mot lan hong giua chung la trao hai lan.
// Neu Mail_Send hong sau khi so cai da ghi thi dong so cai con do voi mail_id = 0,
// tra ra duoc va bu tay duoc - hon han mat im lang.
//////////////////////////////////////////////////////////////////////////////

static int sNhanMoc(int nIdx, int nMoc, int nBranch)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER || !s_bCfgOk)
		return 0;
	if (PB_IsBot(nIdx))
		return 0;
	KCLPlayer& p = s_Pl[nIdx];
	if (!p.bLoaded)
		return 0;
	if (nMoc < 1 || nMoc > (int)s_Award.size())
		return 0;
	KCLAward& a = s_Award[nMoc - 1];
	if (a.sAward.empty())
		return 3;						// [CL 04/09 DOT2c] moc chua cau hinh thuong - cl_ui.lua bao rieng, khong dot moc
	if (a.nBranch != nBranch)
		return 0;						// moc nay khong thuoc nhanh do
	if (nBranch == 1 && p.nVip != 1)
		return 0;						// chua kich hoat Hao Hoa
	// MAY CHU TU TINH LAI, khong tin bat cu so nao tu client
	if (p.nScore < a.nNeedScore)
		return 0;
	int nDaNhan = (nBranch == 1) ? p.nGotVip : p.nGotLow;
	if (nMoc <= nDaNhan)
		return 0;						// da nhan roi (bo dem)
	// het mua van cho NHAN trong thoi gian an han, nhung khong cho cong diem nua
	int nNow = (int)time(NULL);
	if (nNow >= s_Season.nClose + s_Season.nGraceDays * 86400)
		return 0;

	// --- 1. gianh quyen bang so cai ---
	__int64 nAff = 0;
	KDBParam q[7];
	q[0] = KDBParam::I(s_Season.nId);
	q[1] = KDBParam::S(Player[nIdx].m_PlayerName);
	q[2] = KDBParam::I(nMoc);
	q[3] = KDBParam::I(nBranch);
	q[4] = KDBParam::S(a.sAward.c_str());
	q[5] = KDBParam::I(0);
	q[6] = KDBParam::I(nNow);
	bool bOk = g_MySQLDB.Exec(
		"INSERT IGNORE INTO st_ledger (season_id, role_name, idx, branch, award, mail_id, ts)"
		" VALUES (?,?,?,?,?,?,?)", q, 7, &nAff);
	if (!bOk || nAff != 1)
		return 0;						// trung khoa = da nhan roi

	// --- 2. gio moi gui thu ---
	const char* szTitle = a.sTitle.empty() ? "Th\255\353ng Chi\325n L\326nh" : a.sTitle.c_str();
	int nMailId = Mail_Send(Player[nIdx].m_PlayerName, "Chi\325n L\326nh", szTitle,
		a.sContent.c_str(), a.sAward.c_str(), a.nAwardCount, 90 * 86400, "chienlenh");

	char szLog[192];
	if (nMailId > 0)
	{
		KDBParam u[2];
		u[0] = KDBParam::I(nMailId);
		u[1] = KDBParam::I(0);
		// khong can biet ket qua -> Post
		KDBParam v[4];
		v[0] = KDBParam::I(nMailId);
		v[1] = KDBParam::I(s_Season.nId);
		v[2] = KDBParam::S(Player[nIdx].m_PlayerName);
		v[3] = KDBParam::I(nMoc);
		g_MySQLDB.Post("UPDATE st_ledger SET mail_id=? WHERE season_id=? AND role_name=? AND idx=?",
			v, 4);
	}
	else
	{
		_snprintf(szLog, sizeof(szLog) - 1,
			"LOI: da ghi so cai moc %d nhanh %d cho mot nguoi choi nhung Mail_Send tra 0"
			" (hom thu day toan thu con dinh kem?) - dong so cai con mail_id=0, bu tay duoc",
			nMoc, nBranch);
		ChienLenh_Log(CL_LOG_LOI, szLog);
	}

	if (nBranch == 1)
		p.nGotVip = nMoc;
	else
		p.nGotLow = nMoc;
	p.bDirty = 1;
	sXaNguoiChoi(nIdx);
	return nMailId > 0 ? 1 : 2;			// 2 = da ghi so cai nhung thu chua gui duoc
}

//////////////////////////////////////////////////////////////////////////////
// 16. HAM LUA CUA DOT 1b
//////////////////////////////////////////////////////////////////////////////

// CL_Load() -> 1/0. Goi trong playerlogin.lua.
int LuaCL_Load(Lua_State* L)
{
	int nIdx = sChiSoNguoiChoi(L);
	sTuNapLai(0);	// [CL 04/09 DOT2b] dang nhap dau tien sau boot = nap cau hinh
	Lua_PushNumber(L, sNapNguoiChoi(nIdx) ? 1 : 0);
	return 1;
}

// CL_Save() -> 1/0. Goi luc dang xuat va trong nhip 5 phut.
int LuaCL_Save(Lua_State* L)
{
	int nIdx = sChiSoNguoiChoi(L);
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sXaNguoiChoi(nIdx);
	Lua_PushNumber(L, 1);
	return 1;
}

// CL_Cong(nMissionId, nSo) -> 1 neu nhiem vu VUA xong.
// Day la ham ma cac diem moc trong script hoat dong goi (mot dong moi cho).
int LuaCL_Cong(Lua_State* L)
{
	int nIdx = sChiSoNguoiChoi(L);
	int nId = sArgInt(L, 1);
	int nSo = (Lua_GetTopIndex(L) >= 2) ? sArgInt(L, 2) : 1;
	Lua_PushNumber(L, sCong(nIdx, nId, nSo));
	return 1;
}

// CL_Xong(nMissionId) -> 1 neu vua xong. Dung cho nhiem vu KHONG dem (lam la xong).
int LuaCL_Xong(Lua_State* L)
{
	int nIdx = sChiSoNguoiChoi(L);
	int nId = sArgInt(L, 1);
	if (nId >= 1 && nId <= CL_MAX_MISSION && s_Mission[nId].nId == nId)
	{
		int nCon = s_Mission[nId].nTarget;
		if (nIdx > 0 && nIdx < MAX_PLAYER && s_Pl[nIdx].bLoaded)
			nCon = s_Mission[nId].nTarget - s_Pl[nIdx].nProg[nId];
		if (nCon < 1)
			nCon = 1;
		Lua_PushNumber(L, sCong(nIdx, nId, nCon));
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 1;
}

// CL_LinhNhiemVu(nMissionId) -> so diem duoc cong (0 = khong linh duoc).
// Nguoi choi bam nut "Nhan" o dong nhiem vu.
int LuaCL_LinhNhiemVu(Lua_State* L)
{
	int nIdx = sChiSoNguoiChoi(L);
	int nId = sArgInt(L, 1);
	int nDiem = 0;						// day DUNG MOT LAN o cuoi ham
	do
	{
		if (nIdx <= 0 || nId < 1 || nId > CL_MAX_MISSION)
			break;
		if (!s_bCfgOk || !ChienLenh_DangMo() || PB_IsBot(nIdx))
			break;
		KCLPlayer& p = s_Pl[nIdx];
		if (!p.bLoaded || s_Mission[nId].nId != nId || !s_Mission[nId].nEnabled)
			break;
		sKiemReset(nIdx);
		__int64* pTr = sTruongTrangThai(p, nId);
		if (!((*pTr) & CL_BIT_XONG(nId)))
			break;						// chua xong
		if ((*pTr) & CL_BIT_LINH(nId))
			break;						// linh roi
		*pTr |= CL_BIT_LINH(nId);
		p.nScore += s_Mission[nId].nScore;
		p.bDirty = 1;
		sXaNguoiChoi(nIdx);				// vuot moc -> xa ngay
		nDiem = s_Mission[nId].nScore;
	} while (0);
	Lua_PushNumber(L, nDiem);
	return 1;
}

// CL_Nhan(nMoc, nBranch) -> 1 nhan duoc | 2 da ghi so cai nhung thu chua gui | 0 tu choi
int LuaCL_Nhan(Lua_State* L)
{
	int nIdx = sChiSoNguoiChoi(L);
	int nMoc = sArgInt(L, 1);
	int nBranch = sArgInt(L, 2);
	Lua_PushNumber(L, sNhanMoc(nIdx, nMoc, nBranch));
	return 1;
}

// CL_MuaVip() -> 1 kich hoat duoc | 0 that bai | 2 DA co roi (KHONG tieu the)
// Script dung the goi ham nay TRUOC khi xoa vat pham; tra 2 thi dung xoa.
int LuaCL_MuaVip(Lua_State* L)
{
	int nIdx = sChiSoNguoiChoi(L);
	int nKq = 0;						// day DUNG MOT LAN o cuoi ham
	do
	{
		if (nIdx <= 0 || !s_bCfgOk || !ChienLenh_DangMo())
			break;
		KCLPlayer& p = s_Pl[nIdx];
		if (!p.bLoaded)
			break;
		if (p.nVip == 1)
		{
			nKq = 2;					// da co - nguoi goi PHAI khong tieu the
			break;
		}
		p.nVip = 1;
		p.nScore += s_Season.nVipBonus;
		p.bDirty = 1;
		sXaNguoiChoi(nIdx);
		nKq = 1;
	} while (0);
	Lua_PushNumber(L, nKq);
	return 1;
}

// CL_TrangThai() -> bang day du cho giao dien / lenh GM
int LuaCL_TrangThai(Lua_State* L)
{
	int nIdx = sChiSoNguoiChoi(L);
	if (nIdx <= 0 || nIdx >= MAX_PLAYER || !s_Pl[nIdx].bLoaded)
		return 0;
	sKiemReset(nIdx);
	KCLPlayer& p = s_Pl[nIdx];
	Lua_NewTable(L);
	sSetNum(L, "score", p.nScore);
	sSetNum(L, "cap", ChienLenh_CapTuDiem(p.nScore));
	sSetNum(L, "vip", p.nVip);
	sSetNum(L, "got_low", p.nGotLow);
	sSetNum(L, "got_vip", p.nGotVip);
	sSetNum(L, "so_moc", (int)s_Award.size());
	sSetNum(L, "dangmo", ChienLenh_DangMo() ? 1 : 0);
	sSetNum(L, "close_time", s_Season.nClose);
	// trang thai tung nhiem vu: 0 chua xong, 1 xong chua linh, 2 da linh
	for (int i = 1; i <= CL_MAX_MISSION; i++)
	{
		if (s_Mission[i].nId != i)
			continue;
		__int64* pTr = sTruongTrangThai(p, i);
		int nTt = 0;
		if ((*pTr) & CL_BIT_LINH(i))
			nTt = 2;
		else if ((*pTr) & CL_BIT_XONG(i))
			nTt = 1;
		char szKey[24];
		_snprintf(szKey, sizeof(szKey) - 1, "nv%d", i);
		sSetNum(L, szKey, nTt);
		_snprintf(szKey, sizeof(szKey) - 1, "td%d", i);
		sSetNum(L, szKey, p.nProg[i]);
	}
	return 1;
}

// CL_Tick() -> so nguoi da xa. Goi moi phut trong timerserver.lua RunTime().
// Lam ba viec: cong phut online, kiem reset ngay/tuan, xa nguoi da qua 5 phut.
int LuaCL_Tick(Lua_State* L)
{
	int nXa = 0;
	sTuNapLai(0);	// [CL 04/09 DOT2b] moi phut: web tang cfg_version la nap lai
	if (!s_bCfgOk)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nNow = (int)time(NULL);
	// [CL 04/09 DOT2b] PHUT ONLINE dem o day: Lua truyen so hieu cac nhiem vu online (CL_Tick(id1, id2, id3)),
	// C++ duyet nguoi da nap (bot bi sCong loai). Truoc day cl_def.lua duyet bang GetPlayerIdx - ham KHONG co
	// trong engine nay -> ScriptError moi phut va CL_Tick() khong bao gio duoc goi.
	int nArg = Lua_GetTopIndex(L);
	if (nArg > 8)
		nArg = 8;
	if (nArg > 0)
	{
		for (int k = 1; k < MAX_PLAYER; k++)
		{
			if (!s_Pl[k].bLoaded)
				continue;
			for (int a = 1; a <= nArg; a++)
			{
				int nId = sArgInt(L, a);
				if (nId > 0)
					sCong(k, nId, 1);
			}
		}
	}
	for (int i = 1; i < MAX_PLAYER; i++)
	{
		KCLPlayer& p = s_Pl[i];
		if (!p.bLoaded)
			continue;
		sKiemReset(i);
		if (p.bDirty && nNow - p.nLastFlush >= 300)
		{
			sXaNguoiChoi(i);
			nXa++;
		}
	}
	Lua_PushNumber(L, nXa);
	return 1;
}

// CL_Quen() -> 1. Goi luc dang xuat: xa lan cuoi roi bo khe.
int LuaCL_Quen(Lua_State* L)
{
	int nIdx = sChiSoNguoiChoi(L);
	if (nIdx > 0 && nIdx < MAX_PLAYER)
	{
		sXaNguoiChoi(nIdx);
		memset(&s_Pl[nIdx], 0, sizeof(s_Pl[nIdx]));
	}
	Lua_PushNumber(L, 1);
	return 1;
}

#endif // _SERVER
