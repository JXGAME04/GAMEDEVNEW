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
		char szLoi[128];
		int nMuc = sKiemChuoiAward(a.sAward, szLoi, sizeof(szLoi));
		if (nMuc < 0)
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

// CL_Ready() -> 1 neu bang da co VA cau hinh da nap
int LuaCL_Ready(Lua_State* L)
{
	Lua_PushNumber(L, (ChienLenh_EnsureTables() && s_bCfgOk) ? 1 : 0);
	return 1;
}

// CL_Reload() -> so loi (0 = da nhan ban moi). Nap lai neu cfg_version doi,
// hoac ep nap khi truyen tham so khac 0.
int LuaCL_Reload(Lua_State* L)
{
	int bEp = sArgInt(L, 1);
	if (!ChienLenh_EnsureTables())
	{
		Lua_PushNumber(L, 1);
		return 1;
	}
	if (!bEp)
	{
		KCLIntBox c;
		c.n = 0;
		c.bCo = false;
		// mot cau doc theo khoa chinh: 0,08 ms - re, goi moi 30 giay duoc
		g_MySQLDB.Query("SELECT v FROM st_config WHERE k='cfg_version'", 0, 0, _RowInt, &c);
		if (c.bCo && c.n == s_nCfgVer)
		{
			Lua_PushNumber(L, 0);	// khong doi, khong lam gi
			return 1;
		}
		s_nCfgVer = c.bCo ? c.n : s_nCfgVer;
	}
	int nLoi = ChienLenh_Reload();
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

#endif // _SERVER
