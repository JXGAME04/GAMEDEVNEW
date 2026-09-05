//////////////////////////////////////////////////////////////////////////////
// KCauHinhWeb.cpp - [CFGW 04/09] CAU HINH GAME chinh tu WEB ADMIN qua MySQL.
//
// Chu game 04/09: "config toan bo len web de chinh cho de" - exp, exp ky nang,
// ti le rot do, tien, thoi gian hoat dong.
//
// MOT bang gcfg do MAY CHU tao (CREATE TABLE IF NOT EXISTS, khuon KChienLenh.cpp).
// Web chi SELECT/UPDATE noi dung. Sau MOI lan luu web tang gcfg_config['cfg_version'];
// may chu do khoa do moi 30 giay (CFGW_Tick tu script\cauhinh_web\cfgw_driver.lua)
// -> khac thi nap lai TOAN BO -> ap:
//   - khoa C++ (ServerConfig.* / Exp.*): gan thang bien toan cuc, an NGAY;
//   - khoa script: giu trong s_Val, Lua doc bang CFGW_Get(k) - G_CFG (ch_lib.lua)
//     va HD_CFG (cauhinh_hoatdong.lua) hoi ban web TRUOC roi moi den bang tep.
// Gia tri hong (khong phai so, ngoai khoang) -> GIU gia tri cu + ghi gcfg_log (co
// cot k de web to do dung dong) + ghi v_ap = gia tri DANG THUC DUNG (web hien
// "may chu tu choi, dang dung X"). Sai du lieu trong MySQL im lang, khong co
// ScriptError - nen phai ghi nguoc.
//
// Giao keo voi phien web (webver5-eb) 04/09: cot thu_tu, don_vi, min_v/max_v DOUBLE,
// v_ap, gcfg_log.k; kieu 5 = khoa cu khong con trong script (web an mac dinh).
// BA LUAT HIEU NANG (KMySQLDB.h): Exec/Query dong bo chi luc boot (khai khoa) va
// khi cfg_version doi (mot SELECT 0,08 ms moi 30 s); moi thu khac dung Post.
// Xem BANGIAO_CAUHINH_WEB_0409.md.
//////////////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KCauHinhWeb.h"

#ifdef _SERVER
#include "KMySQLDB.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////////////
// 1. BA BANG - GAME tao, WEB chi doc/ghi noi dung
//////////////////////////////////////////////////////////////////////////////

static bool s_bTableOk = false;
static bool s_bIniDaDoc = false;	// [05/09] KCore.cpp InitGameSetting() da doc gamesetting.ini chua (truoc do bien toan cuc = gia tri tinh)
static bool s_bCppDaKhai = false;	// [05/09] 27 khoa C++ da khai trong tien trinh nay (chi lam SAU khi ini da doc)

static const char* s_szDDL[] =
{
	"CREATE TABLE IF NOT EXISTS gcfg ("
	" k VARBINARY(64) NOT NULL PRIMARY KEY,"
	" v VARBINARY(255) NOT NULL DEFAULT '',"
	" v_ap VARBINARY(255) NOT NULL DEFAULT '',"
	" v_macdinh VARBINARY(255) NOT NULL DEFAULT '',"
	" nhom VARBINARY(32) NOT NULL DEFAULT '',"
	" kieu TINYINT NOT NULL DEFAULT 0,"
	" min_v DOUBLE NOT NULL DEFAULT 0,"
	" max_v DOUBLE NOT NULL DEFAULT 0,"
	" don_vi VARBINARY(16) NOT NULL DEFAULT '',"
	" mota VARBINARY(255) NOT NULL DEFAULT '',"
	" nguon VARBINARY(64) NOT NULL DEFAULT '',"
	" ap_dung TINYINT NOT NULL DEFAULT 0,"
	" thu_tu INT NOT NULL DEFAULT 0,"
	" updated_at INT NOT NULL DEFAULT 0,"
	" updated_by VARBINARY(32) NOT NULL DEFAULT '',"
	" ten VARBINARY(96) NOT NULL DEFAULT '',"			// [05/09] ten tieng Viet (TCVN3)
	" giai_thich VARBINARY(1500) NOT NULL DEFAULT '',"	// [05/09] giai thich chi tiet (TCVN3, xuong dong tach doan)
	" canh_bao VARBINARY(800) NOT NULL DEFAULT '',"		// [05/09] canh bao neu chinh sai (TCVN3)
	" nguy_co TINYINT NOT NULL DEFAULT 0,"				// [05/09] 0 an toan | 1 can than | 2 nguy hiem
	" KEY idx_nhom (nhom, thu_tu)"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1",

	"CREATE TABLE IF NOT EXISTS gcfg_config ("
	" k VARBINARY(32) NOT NULL PRIMARY KEY,"
	" v VARBINARY(255) NOT NULL DEFAULT '',"
	" updated_at INT NOT NULL DEFAULT 0"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1",

	"CREATE TABLE IF NOT EXISTS gcfg_log ("
	" id INT AUTO_INCREMENT PRIMARY KEY,"
	" ts INT NOT NULL DEFAULT 0,"
	" level TINYINT NOT NULL DEFAULT 0,"
	" k VARBINARY(64) NOT NULL DEFAULT '',"
	" msg VARBINARY(255) NOT NULL DEFAULT ''"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1",
};

//////////////////////////////////////////////////////////////////////////////
// 2. TRANG THAI TRONG BO NHO
//////////////////////////////////////////////////////////////////////////////

#define CFGW_KIEU_INT		0
#define CFGW_KIEU_DOUBLE	1
#define CFGW_KIEU_CHUOI		2
#define CFGW_KIEU_BATTAT	3
#define CFGW_KIEU_HHMM		4
#define CFGW_KIEU_CU		5	// khoa khong con trong script (web an mac dinh)

struct KCFGWVal
{
	std::string	v;
	int			nKieu;
	double		dMin, dMax;		// 0/0 = khong kiem khoang
	KCFGWVal() : nKieu(0), dMin(0), dMax(0) {}
};

static std::map<std::string, KCFGWVal>	s_Val;		// gia tri dang dung (da qua kiem)
static std::set<std::string>			s_Khai;		// khoa da khai trong tien trinh nay
static int		s_nVer = -1;		// cfg_version da nap; -1 = chua nap lan nao
static int		s_nSoKhoa = 0;
static int		s_nSoLoi = 0;
static int		s_nThuTu = 0;
static time_t	s_tNapCuoi = 0;

// Khoa C++: gan thang bien toan cuc (KCore.cpp) - AN NGAY, khong restart.
// Khoang kep lay dung bang aKep cua KCore.cpp:844 (nhom [Exp]).
struct KCFGWCpp
{
	const char*	k;
	int*		p;
	int			nMin, nMax;
	const char*	nhom;
	const char*	donvi;
	const char*	mota;
};

static KCFGWCpp s_aCpp[] =
{
	{ "ServerConfig.ExpRate",      &g_ExpRate,          1, 100000, "EXP",        "x",   "He so NHAN kinh nghiem toan server (gamesetting.ini [ServerConfig] ExpRate). Nhan them tren HeSo1..4" },
	{ "ServerConfig.MoneyRate",    &g_MoneyRate,        1, 100000, "TIEN",       "x",   "He so NHAN tien quai roi ra (MoneyRate)" },
	{ "ServerConfig.Skill90Rate",  &g_Skill90ExpRate,   0, 100000, "EXP_KYNANG", "x",   "Toc do tang exp KY NANG 90 (Skill90Rate; 0 = tat)" },
	{ "ServerConfig.Skill120Rate", &g_Skill120ExpRate,  0, 100000, "EXP_KYNANG", "x",   "Toc do tang exp KY NANG 120 (Skill120Rate; 0 = tat)" },
	{ "Exp.MocCap1",       &g_nExpMocCap1,       1, 200,    "EXP", "cap", "Duoi cap nay dung HeSo1" },
	{ "Exp.HeSo1",         &g_nExpHeSo1,         1, 100000, "EXP", "x",   "He so exp cho cap < MocCap1" },
	{ "Exp.MocCap2",       &g_nExpMocCap2,       1, 200,    "EXP", "cap", "Duoi cap nay dung HeSo2" },
	{ "Exp.HeSo2",         &g_nExpHeSo2,         1, 100000, "EXP", "x",   "He so exp cho cap < MocCap2" },
	{ "Exp.MocCap3",       &g_nExpMocCap3,       1, 200,    "EXP", "cap", "Duoi cap nay dung HeSo3" },
	{ "Exp.HeSo3",         &g_nExpHeSo3,         1, 100000, "EXP", "x",   "He so exp cho cap < MocCap3 (dang chi phoi 8x-13x)" },
	{ "Exp.HeSo4",         &g_nExpHeSo4,         1, 100000, "EXP", "x",   "He so exp cho cap >= MocCap3" },
	{ "Exp.VipCong",       &g_nExpVipCong,       0, 100000, "EXP", "x",   "VIP cong them vao he so exp" },
	{ "Exp.CsMap",         &g_nExpCsMap,         0, 100000, "EXP", "map", "Map ap dung luat chuyen sinh" },
	{ "Exp.CsLanToiThieu", &g_nExpCsLanToiThieu, 0, 100,    "EXP", "lan", "So lan chuyen sinh toi thieu de an he so chuyen sinh" },
	{ "Exp.CsDuoi",        &g_nExpCsDuoi,        1, 100000, "EXP", "x",   "He so chuyen sinh khi cap < MocCap3" },
	{ "Exp.CsTren",        &g_nExpCsTren,        1, 100000, "EXP", "x",   "He so chuyen sinh khi cap >= MocCap3" },
	{ "Exp.ChenhCapMax",   &g_nExpChenhCapMax,   0, 200,    "EXP", "cap", "Chenh cap toi da (nguoi - quai) con an du exp" },
	{ "Exp.ChiaKhiChenh",  &g_nExpChiaKhiChenh,  1, 100000, "EXP", "x",   "Chenh qua muc thi exp chia cho so nay" },
	{ "Exp.MienTruCap",    &g_nExpMienTruCap,    1, 200,    "EXP", "cap", "Tu cap nay tro len bo moi phat chenh cap" },
	{ "Exp.ToDoi2",        &g_nExpToDoi2,        1, 100,    "EXP", "%",   "% exp moi nguoi khi to doi 2 nguoi" },
	{ "Exp.ToDoi3",        &g_nExpToDoi3,        1, 100,    "EXP", "%",   "% exp moi nguoi khi to doi 3 nguoi" },
	{ "Exp.ToDoi4",        &g_nExpToDoi4,        1, 100,    "EXP", "%",   "% exp moi nguoi khi to doi 4 nguoi" },
	{ "Exp.ToDoi5",        &g_nExpToDoi5,        1, 100,    "EXP", "%",   "% exp moi nguoi khi to doi 5 nguoi" },
	{ "Exp.ToDoi6",        &g_nExpToDoi6,        1, 100,    "EXP", "%",   "% exp moi nguoi khi to doi 6 nguoi" },
	{ "Exp.ToDoi7",        &g_nExpToDoi7,        1, 100,    "EXP", "%",   "% exp moi nguoi khi to doi 7 nguoi" },
	{ "Exp.ToDoi8",        &g_nExpToDoi8,        1, 100,    "EXP", "%",   "% exp moi nguoi khi to doi 8 nguoi" },
	{ "Exp.ToDoiKhac",     &g_nExpToDoiKhac,     1, 100,    "EXP", "%",   "% exp khi so nguoi to doi ngoai 2..8" },
};

static bool sLaKhoaCpp(const std::string& k)
{
	const int nSo = (int)(sizeof(s_aCpp) / sizeof(s_aCpp[0]));
	for (int i = 0; i < nSo; i++)
		if (k == s_aCpp[i].k)
			return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// 3. HAM PHU
//////////////////////////////////////////////////////////////////////////////

static std::string sCol(const KDBRow& row, int c)
{
	if (c < 0 || c >= row.nCol || !row.pVal[c] || row.pLen[c] <= 0)
		return std::string();
	return std::string(row.pVal[c], row.pLen[c]);
}

static double sColDbl(const KDBRow& row, int c)
{
	std::string s = sCol(row, c);
	return s.empty() ? 0.0 : atof(s.c_str());
}

static const char* sArgStr(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsString(L, n)) ? Lua_ValueToString(L, n) : "";
}

static int sArgInt(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsNumber(L, n)) ? (int)Lua_ValueToNumber(L, n) : 0;
}

static double sArgDbl(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsNumber(L, n)) ? (double)Lua_ValueToNumber(L, n) : 0.0;
}

static void sSetNum(Lua_State* L, const char* szKey, double v)
{
	Lua_PushString(L, (char*)szKey);
	Lua_PushNumber(L, v);
	Lua_SetTable(L, -3);
}

// Chuoi co phai SO nguyen / so thuc tron ven khong (khong ky tu thua)
static bool sLaSoNguyen(const std::string& s, __int64* pn)
{
	if (s.empty() || s.size() > 24)
		return false;
	char* e = 0;
	__int64 n = _strtoi64(s.c_str(), &e, 10);
	if (!e || *e != 0)
		return false;
	if (pn) *pn = n;
	return true;
}

static bool sLaSoThuc(const std::string& s, double* pd)
{
	if (s.empty() || s.size() > 32)
		return false;
	char* e = 0;
	double d = strtod(s.c_str(), &e);
	if (!e || *e != 0)
		return false;
	if (pd) *pd = d;
	return true;
}

// Ghi nguoc nhat ky: web hien ra, to do dung khoa k (rong = loi chung)
void CauHinhWeb_Log(int nLevel, const char* k, const char* szMsg)
{
	if (!szMsg || !szMsg[0])
		return;
	g_DebugLog((LPSTR)"[CFGW] %s %s", (k && k[0]) ? k : "-", szMsg);
	if (!g_MySQLDB.IsReady() || !s_bTableOk)
		return;
	KDBParam p[4];
	p[0] = KDBParam::I((__int64)time(NULL));
	p[1] = KDBParam::I(nLevel);
	p[2] = KDBParam::S(k ? k : "");
	p[3] = KDBParam::S(szMsg);
	g_MySQLDB.Post("INSERT INTO gcfg_log (ts, level, k, msg) VALUES (?, ?, ?, ?)", p, 4);
}

// v_ap = gia tri may chu DANG THUC DUNG cho khoa k (web doi chieu voi v)
static void sGhiVAp(const std::string& k, const std::string& v)
{
	KDBParam p[2];
	p[0] = KDBParam::S(v.c_str());
	p[1] = KDBParam::S(k.c_str());
	g_MySQLDB.Post("UPDATE gcfg SET v_ap=? WHERE k=?", p, 2);
}

//////////////////////////////////////////////////////////////////////////////
// 4. KHAI KHOA (INSERT IGNORE: web da sua thi giu; metadata lam moi moi lan boot)
//////////////////////////////////////////////////////////////////////////////

// [05/09] ten / giaithich / canhbao / nNguyCo: tieng Viet TCVN3 tu cfgw_meta.lua (chu game: "giai thich ro
// hon bang tieng Viet co dau va canh bao neu chinh sai"). NULL = khong dong vao (khoa C++ khai tu sKhaiCpp,
// tieng Viet den sau qua CFGW_MoTa).
static bool sKhai(const char* k, const char* v, const char* nhom, int nKieu, double dMin, double dMax,
                  const char* mota, const char* nguon, int nApDung, const char* donvi,
                  const char* ten = 0, const char* giaithich = 0, const char* canhbao = 0, int nNguyCo = 0)
{
	if (!s_bTableOk || !k || !k[0])
		return false;
	if (strlen(k) > 64)
		return false;
	if (s_Khai.count(k))
		return true;						// da khai trong tien trinh nay
	const int nThuTu = ++s_nThuTu;
	{
		KDBParam p[17];
		p[0]  = KDBParam::S(k);
		p[1]  = KDBParam::S(v ? v : "");	// v
		p[2]  = KDBParam::S(v ? v : "");	// v_ap: luc khai, gia tri dang chay = gia tri tep
		p[3]  = KDBParam::S(v ? v : "");	// v_macdinh
		p[4]  = KDBParam::S(nhom ? nhom : "");
		p[5]  = KDBParam::I(nKieu);
		p[6]  = KDBParam::D(dMin);
		p[7]  = KDBParam::D(dMax);
		p[8]  = KDBParam::S(donvi ? donvi : "");
		p[9]  = KDBParam::S(mota ? mota : "");
		p[10] = KDBParam::S(nguon ? nguon : "");
		p[11] = KDBParam::I(nApDung);
		p[12] = KDBParam::I(nThuTu);
		p[13] = KDBParam::S(ten ? ten : "");
		p[14] = KDBParam::S(giaithich ? giaithich : "");
		p[15] = KDBParam::S(canhbao ? canhbao : "");
		p[16] = KDBParam::I(nNguyCo);
		if (!g_MySQLDB.Exec("INSERT IGNORE INTO gcfg (k, v, v_ap, v_macdinh, nhom, kieu, min_v, max_v, don_vi, mota, nguon, ap_dung, thu_tu, updated_at, updated_by, ten, giai_thich, canh_bao, nguy_co)"
		                    " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0, 'server', ?, ?, ?, ?)", p, 17, 0, 0))
			return false;					// MySQL loi -> lan sau khai lai
	}
	{
		// [05/09] LUAT NGUON GIA TRI: khoa web CHUA sua (updated_by='server') thi gia tri TEP/INI hien tai
		// la gia tri chay (v, v_ap theo tep); web da sua (updated_by = ten dang nhap) thi web thang.
		// Dong bo (Exec, chi luc boot, mot lan moi khoa) de SELECT nap lai ngay sau do thay gia tri moi.
		// Bai hoc 05/09: Skill90/120Rate bi khai bang gia tri tinh 1 (truoc khi doc ini = 10) roi
		// ban 1 do de len ini o lan nap ke tiep du web khong he sua.
		KDBParam q[4];
		q[0] = KDBParam::S(v ? v : "");
		q[1] = KDBParam::S(v ? v : "");
		q[2] = KDBParam::S(k);
		q[3] = KDBParam::S(v ? v : "");
		g_MySQLDB.Exec("UPDATE gcfg SET v=?, v_ap=? WHERE k=? AND updated_by='server' AND v<>?", q, 4, 0, 0);
	}
	{
		// lam moi metadata (mac dinh dang chay, mo ta, nhom, thu tu, tieng Viet...) - KHONG dong vao v cua web
		KDBParam p[14];
		p[0] = KDBParam::S(v ? v : "");
		p[1] = KDBParam::S(nhom ? nhom : "");
		p[2] = KDBParam::I(nKieu);
		p[3] = KDBParam::D(dMin);
		p[4] = KDBParam::D(dMax);
		p[5] = KDBParam::S(donvi ? donvi : "");
		p[6] = KDBParam::S(mota ? mota : "");
		p[7] = KDBParam::S(nguon ? nguon : "");
		p[8] = KDBParam::I(nApDung);
		if (ten)
		{
			p[9]  = KDBParam::S(ten);
			p[10] = KDBParam::S(giaithich ? giaithich : "");
			p[11] = KDBParam::S(canhbao ? canhbao : "");
			p[12] = KDBParam::I(nNguyCo);
			p[13] = KDBParam::S(k);
			g_MySQLDB.Post("UPDATE gcfg SET v_macdinh=?, nhom=?, kieu=?, min_v=?, max_v=?, don_vi=?, mota=?, nguon=?, ap_dung=?, ten=?, giai_thich=?, canh_bao=?, nguy_co=? WHERE k=?", p, 14);
		}
		else
		{
			p[9] = KDBParam::S(k);
			g_MySQLDB.Post("UPDATE gcfg SET v_macdinh=?, nhom=?, kieu=?, min_v=?, max_v=?, don_vi=?, mota=?, nguon=?, ap_dung=? WHERE k=?", p, 10);
		}
		{
			KDBParam q[2];
			q[0] = KDBParam::I(nThuTu);
			q[1] = KDBParam::S(k);
			g_MySQLDB.Post("UPDATE gcfg SET thu_tu=? WHERE k=?", q, 2);
		}
	}
	s_Khai.insert(k);
	return true;
}

static void sKhaiCpp()
{
	const int nSo = (int)(sizeof(s_aCpp) / sizeof(s_aCpp[0]));
	for (int i = 0; i < nSo; i++)
	{
		char szV[32];
		_snprintf(szV, sizeof(szV) - 1, "%d", *s_aCpp[i].p);
		szV[sizeof(szV) - 1] = 0;
		sKhai(s_aCpp[i].k, szV, s_aCpp[i].nhom, CFGW_KIEU_INT, (double)s_aCpp[i].nMin, (double)s_aCpp[i].nMax,
		      s_aCpp[i].mota, "gamesetting.ini", 0, s_aCpp[i].donvi);
	}
}

static void sDamBaoCotMoi();		// [05/09] dinh nghia o muc 5 (can _RowInt)

// [05/09] KCore.cpp InitGameSetting() goi sau khi doc xong [ServerConfig] + [Exp]. Tu day moi duoc
// khai / ap khoa C++; nhip CFGW_Tick ke tiep (<= 30 s) se khai va nap lai.
void CauHinhWeb_IniDaDoc()
{
	s_bIniDaDoc = true;
}

bool CauHinhWeb_EnsureTables()
{
	if (!g_MySQLDB.IsReady())
		return false;
	if (s_bTableOk)
		return true;
	const int nSo = (int)(sizeof(s_szDDL) / sizeof(s_szDDL[0]));
	for (int i = 0; i < nSo; i++)
	{
		if (!g_MySQLDB.Exec(s_szDDL[i], 0, 0))
		{
			g_DebugLog((LPSTR)"[CFGW] khong tao duoc bang gcfg thu %d", i + 1);
			return false;
		}
	}
	sDamBaoCotMoi();		// bang da co tu ban truoc -> them cot tieng Viet neu thieu
	// khoa cfg_version phai co san de web INCREMENT duoc ngay lan luu dau
	g_MySQLDB.Exec("INSERT IGNORE INTO gcfg_config (k, v, updated_at) VALUES ('cfg_version', '0', 0)", 0, 0);
	s_bTableOk = true;
	// [05/09] KHONG sKhaiCpp() o day nua: luc nay (script nap trong g_InitCore) gamesetting.ini CHUA doc,
	// bien toan cuc con gia tri tinh -> khai sai. Khai C++ o CauHinhWeb_TuNapLai() sau khi co co s_bIniDaDoc.
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// 5. NAP LAI + AP
//////////////////////////////////////////////////////////////////////////////

struct KCFGWIntBox { int n; bool bCo; };

static bool _RowInt(const KDBRow& row, void* p)
{
	std::string s = sCol(row, 0);
	((KCFGWIntBox*)p)->n = s.empty() ? 0 : atoi(s.c_str());
	((KCFGWIntBox*)p)->bCo = true;
	return true;
}

// [05/09] Cot tieng Viet them sau khi bang gcfg co the da ton tai (may chu tung chay ban 04/09):
// CREATE TABLE IF NOT EXISTS khong them cot -> hoi information_schema, thieu thi ALTER (mot lan).
struct KCFGWCotMoi { const char* ten; const char* ddl; };
static const KCFGWCotMoi s_aCotMoi[] =
{
	{ "ten",        "ALTER TABLE gcfg ADD COLUMN ten VARBINARY(96) NOT NULL DEFAULT ''" },
	{ "giai_thich", "ALTER TABLE gcfg ADD COLUMN giai_thich VARBINARY(1500) NOT NULL DEFAULT ''" },
	{ "canh_bao",   "ALTER TABLE gcfg ADD COLUMN canh_bao VARBINARY(800) NOT NULL DEFAULT ''" },
	{ "nguy_co",    "ALTER TABLE gcfg ADD COLUMN nguy_co TINYINT NOT NULL DEFAULT 0" },
};

static void sDamBaoCotMoi()
{
	const int nSo = (int)(sizeof(s_aCotMoi) / sizeof(s_aCotMoi[0]));
	for (int i = 0; i < nSo; i++)
	{
		KCFGWIntBox c;
		c.n = 0;
		c.bCo = false;
		KDBParam p[1];
		p[0] = KDBParam::S(s_aCotMoi[i].ten);
		if (!g_MySQLDB.Query("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA=DATABASE() AND TABLE_NAME='gcfg' AND COLUMN_NAME=?", p, 1, _RowInt, &c))
			continue;
		if (c.bCo && c.n == 0)
		{
			if (g_MySQLDB.Exec(s_aCotMoi[i].ddl, 0, 0))
				g_DebugLog((LPSTR)"[CFGW] da them cot gcfg.%s", s_aCotMoi[i].ten);
			else
				g_DebugLog((LPSTR)"[CFGW] KHONG them duoc cot gcfg.%s", s_aCotMoi[i].ten);
		}
	}
}

struct KCFGWRow { std::string k; std::string vAp; std::string vMacDinh; KCFGWVal x; };
struct KCFGWRowBox { std::vector<KCFGWRow> v; };

static bool _RowCfg(const KDBRow& row, void* p)
{
	KCFGWRow r;
	r.k        = sCol(row, 0);
	r.x.v      = sCol(row, 1);
	r.vAp      = sCol(row, 2);
	r.vMacDinh = sCol(row, 3);
	r.x.nKieu  = (int)sColDbl(row, 4);
	r.x.dMin   = sColDbl(row, 5);
	r.x.dMax   = sColDbl(row, 6);
	((KCFGWRowBox*)p)->v.push_back(r);
	return true;
}

// Gia tri co hop kieu + khoang khong. Loi -> szLoi mo ta ngan (ASCII).
static bool sHopLe(const KCFGWVal& x, char* szLoi, int nLoi)
{
	szLoi[0] = 0;
	if (x.nKieu == CFGW_KIEU_CHUOI)
		return x.v.size() <= 255;
	const bool bKiemKhoang = (x.dMin != 0 || x.dMax != 0);
	if (x.nKieu == CFGW_KIEU_DOUBLE)
	{
		double d = 0;
		if (!sLaSoThuc(x.v, &d))
		{
			_snprintf(szLoi, nLoi - 1, "khong phai so thuc");
			return false;
		}
		if (bKiemKhoang && (d < x.dMin || d > x.dMax))
		{
			_snprintf(szLoi, nLoi - 1, "ngoai khoang %g..%g", x.dMin, x.dMax);
			return false;
		}
		return true;
	}
	__int64 n = 0;
	if (!sLaSoNguyen(x.v, &n))
	{
		_snprintf(szLoi, nLoi - 1, "khong phai so nguyen");
		return false;
	}
	if (bKiemKhoang && ((double)n < x.dMin || (double)n > x.dMax))
	{
		_snprintf(szLoi, nLoi - 1, "ngoai khoang %g..%g", x.dMin, x.dMax);
		return false;
	}
	if (x.nKieu == CFGW_KIEU_BATTAT && n != 0 && n != 1)
	{
		_snprintf(szLoi, nLoi - 1, "chi nhan 0 hoac 1");
		return false;
	}
	if (x.nKieu == CFGW_KIEU_HHMM && (n < 0 || n > 2359 || (n % 100) > 59))
	{
		_snprintf(szLoi, nLoi - 1, "khong phai gio HHMM");
		return false;
	}
	return true;
}

// Ap khoa C++: doc tu s_Val, kep khoang, gan bien toan cuc. Tra so khoa da doi.
static int sApCpp()
{
	if (!s_bIniDaDoc)
		return 0;		// [05/09] ini chua doc: khong ap (InitGameSetting se ghi de ngay sau, va DB co the con gia tri tinh)
	int nDoi = 0;
	const int nSo = (int)(sizeof(s_aCpp) / sizeof(s_aCpp[0]));
	for (int i = 0; i < nSo; i++)
	{
		std::map<std::string, KCFGWVal>::iterator it = s_Val.find(s_aCpp[i].k);
		if (it == s_Val.end())
			continue;
		__int64 n = 0;
		if (!sLaSoNguyen(it->second.v, &n))
			continue;						// da bao o sHopLe
		char szCu[32];
		_snprintf(szCu, sizeof(szCu) - 1, "%d", *s_aCpp[i].p);
		szCu[sizeof(szCu) - 1] = 0;
		if (n < s_aCpp[i].nMin || n > s_aCpp[i].nMax)
		{
			char szMsg[200];
			_snprintf(szMsg, sizeof(szMsg) - 1, "gia tri %I64d ngoai khoang %d..%d - giu %s", n, s_aCpp[i].nMin, s_aCpp[i].nMax, szCu);
			szMsg[sizeof(szMsg) - 1] = 0;
			CauHinhWeb_Log(2, s_aCpp[i].k, szMsg);
			it->second.v = szCu;			// s_Val cung giu gia tri dang dung
			sGhiVAp(s_aCpp[i].k, szCu);
			continue;
		}
		if (*s_aCpp[i].p != (int)n)
		{
			char szMsg[200];
			_snprintf(szMsg, sizeof(szMsg) - 1, "%s -> %I64d (ap ngay)", szCu, n);
			szMsg[sizeof(szMsg) - 1] = 0;
			CauHinhWeb_Log(0, s_aCpp[i].k, szMsg);
			*s_aCpp[i].p = (int)n;
			nDoi++;
		}
	}
	return nDoi;
}

// Tra: -1 chua san sang | 0 khong doi | 1 da nap lai (so loi xem s_nSoLoi)
int CauHinhWeb_TuNapLai(int bEp)
{
	if (!CauHinhWeb_EnsureTables())
		return -1;
	if (s_bIniDaDoc && !s_bCppDaKhai)
	{
		sKhaiCpp();		// [05/09] khai 27 khoa C++ bang gia tri INI that (sau InitGameSetting), v theo ini neu web chua sua
		s_bCppDaKhai = true;
		bEp = 1;		// nap lai ngay de s_Val + bien toan cuc theo gia tri vua khai
	}
	int nMoi = s_nVer;
	{
		KCFGWIntBox c;
		c.n = 0;
		c.bCo = false;
		g_MySQLDB.Query("SELECT v FROM gcfg_config WHERE k='cfg_version'", 0, 0, _RowInt, &c);
		if (c.bCo)
			nMoi = c.n;
	}
	if (!bEp && nMoi == s_nVer && s_nVer != -1)
		return 0;							// khong doi, khong lam gi (mot SELECT 0,08 ms)

	KCFGWRowBox box;
	if (!g_MySQLDB.Query("SELECT k, v, v_ap, v_macdinh, kieu, min_v, max_v FROM gcfg", 0, 0, _RowCfg, &box))
	{
		CauHinhWeb_Log(2, "", "khong doc duoc bang gcfg - giu cau hinh dang chay");
		return -1;
	}
	int nLoi = 0;
	const bool bDaKhaiScript = (s_Khai.size() > 100);	// driver Lua da khai xong -> moi dam danh dau khoa cu
	for (size_t i = 0; i < box.v.size(); i++)
	{
		KCFGWRow& r = box.v[i];
		if (r.x.nKieu == CFGW_KIEU_CU)
			continue;						// khoa cu, khong ai doc
		if (bDaKhaiScript && !s_Khai.count(r.k) && !sLaKhoaCpp(r.k))
		{
			// khoa co trong bang ma script khong con khai -> danh dau kieu 5 de web an di
			KDBParam p[1];
			p[0] = KDBParam::S(r.k.c_str());
			g_MySQLDB.Post("UPDATE gcfg SET kieu=5 WHERE k=?", p, 1);
			continue;
		}
		char szLoi[96];
		if (!sHopLe(r.x, szLoi, sizeof(szLoi)))
		{
			nLoi++;
			char szMsg[240];
			_snprintf(szMsg, sizeof(szMsg) - 1, "gia tri '%.60s' %s - giu gia tri cu", r.x.v.c_str(), szLoi);
			szMsg[sizeof(szMsg) - 1] = 0;
			CauHinhWeb_Log(2, r.k.c_str(), szMsg);
			// v_ap = gia tri DANG DUNG: ban da nap truoc do, khong co thi mac dinh cua tep
			std::map<std::string, KCFGWVal>::iterator it = s_Val.find(r.k);
			const std::string sDung = (it != s_Val.end()) ? it->second.v : r.vMacDinh;
			if (sDung != r.vAp)
				sGhiVAp(r.k, sDung);
			continue;
		}
		s_Val[r.k] = r.x;
		if (r.x.v != r.vAp)
			sGhiVAp(r.k, r.x.v);
	}
	const int nDoiCpp = sApCpp();
	s_nVer = (nMoi < 0) ? 0 : nMoi;
	s_nSoKhoa = (int)box.v.size();
	s_nSoLoi = nLoi;
	s_tNapCuoi = time(NULL);
	{
		char szMsg[200];
		_snprintf(szMsg, sizeof(szMsg) - 1, "da nap cfg_version=%d: %d khoa, %d loi, %d khoa C++ doi", s_nVer, s_nSoKhoa, nLoi, nDoiCpp);
		szMsg[sizeof(szMsg) - 1] = 0;
		CauHinhWeb_Log(nLoi ? 1 : 0, "", szMsg);
	}
	return 1;
}

const char* CauHinhWeb_Get(const char* k)
{
	if (!k || !k[0])
		return 0;
	std::map<std::string, KCFGWVal>::iterator it = s_Val.find(k);
	return (it == s_Val.end()) ? 0 : it->second.v.c_str();
}

//////////////////////////////////////////////////////////////////////////////
// 6. HAM LUA (dang ky o ScriptFuns.cpp, phia may chu)
//////////////////////////////////////////////////////////////////////////////

// CFGW_Get(k) -> so | chuoi | nil (chua co ban web cho khoa nay -> G_CFG dung bang tep)
int LuaCFGW_Get(Lua_State* L)
{
	const char* k = sArgStr(L, 1);
	if (!k[0])
		return 0;
	std::map<std::string, KCFGWVal>::iterator it = s_Val.find(k);
	if (it == s_Val.end())
		return 0;
	const KCFGWVal& x = it->second;
	if (x.nKieu != CFGW_KIEU_CHUOI)
	{
		double d = 0;
		if (sLaSoThuc(x.v, &d))
		{
			Lua_PushNumber(L, d);
			return 1;
		}
	}
	Lua_PushString(L, (char*)x.v.c_str());
	return 1;
}

#define CFGW_MAX_TEN		96
#define CFGW_MAX_GIAITHICH	1500
#define CFGW_MAX_CANHBAO	800

// CFGW_Khai(k, v, nhom, kieu, min, max, mota, nguon, ap_dung, don_vi [, ten, giai_thich, canh_bao, nguy_co])
//   -> 1 da khai | 0 chua san sang. [05/09] 4 doi so cuoi = tieng Viet TCVN3 (tu cfgw_meta.lua), bo trong = khong dong.
int LuaCFGW_Khai(Lua_State* L)
{
	if (!CauHinhWeb_EnsureTables())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	std::string k     = sArgStr(L, 1);
	std::string v     = sArgStr(L, 2);
	std::string nhom  = sArgStr(L, 3);
	const int nKieu   = sArgInt(L, 4);
	const double dMin = sArgDbl(L, 5);
	const double dMax = sArgDbl(L, 6);
	std::string mota  = sArgStr(L, 7);
	std::string nguon = sArgStr(L, 8);
	const int nAp     = sArgInt(L, 9);
	std::string donvi = sArgStr(L, 10);
	std::string ten   = sArgStr(L, 11);
	std::string gt    = sArgStr(L, 12);
	std::string cb    = sArgStr(L, 13);
	int nNguyCo       = sArgInt(L, 14);
	if (mota.size() > 250)  mota.resize(250);
	if (v.size() > 250)     v.resize(250);
	if (donvi.size() > 16)  donvi.resize(16);
	if (ten.size() > CFGW_MAX_TEN)      ten.resize(CFGW_MAX_TEN);
	if (gt.size() > CFGW_MAX_GIAITHICH) gt.resize(CFGW_MAX_GIAITHICH);
	if (cb.size() > CFGW_MAX_CANHBAO)   cb.resize(CFGW_MAX_CANHBAO);
	if (nNguyCo < 0 || nNguyCo > 2)     nNguyCo = 1;
	const bool bCoVn = !ten.empty();
	Lua_PushNumber(L, sKhai(k.c_str(), v.c_str(), nhom.c_str(), nKieu, dMin, dMax, mota.c_str(), nguon.c_str(), nAp, donvi.c_str(),
	                        bCoVn ? ten.c_str() : 0, bCoVn ? gt.c_str() : 0, bCoVn ? cb.c_str() : 0, bCoVn ? nNguyCo : 0) ? 1 : 0);
	return 1;
}

// CFGW_MoTa(k, ten, giai_thich, canh_bao, nguy_co) -> 1 da ghi | 0 chua san sang / khoa rong.
// [05/09] Tieng Viet cho khoa C++ (ServerConfig.* / Exp.*): C++ tu khai metadata (sKhaiCpp, chu ASCII),
// script gui tieng Viet TCVN3 qua day de C++ khong phai chua literal TCVN3. Post (khong cho), mot lan moi boot.
int LuaCFGW_MoTa(Lua_State* L)
{
	if (!CauHinhWeb_EnsureTables())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	std::string k   = sArgStr(L, 1);
	std::string ten = sArgStr(L, 2);
	std::string gt  = sArgStr(L, 3);
	std::string cb  = sArgStr(L, 4);
	int nNguyCo     = sArgInt(L, 5);
	if (k.empty() || k.size() > 64)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (ten.size() > CFGW_MAX_TEN)      ten.resize(CFGW_MAX_TEN);
	if (gt.size() > CFGW_MAX_GIAITHICH) gt.resize(CFGW_MAX_GIAITHICH);
	if (cb.size() > CFGW_MAX_CANHBAO)   cb.resize(CFGW_MAX_CANHBAO);
	if (nNguyCo < 0 || nNguyCo > 2)     nNguyCo = 1;
	KDBParam p[5];
	p[0] = KDBParam::S(ten.c_str());
	p[1] = KDBParam::S(gt.c_str());
	p[2] = KDBParam::S(cb.c_str());
	p[3] = KDBParam::I(nNguyCo);
	p[4] = KDBParam::S(k.c_str());
	g_MySQLDB.Post("UPDATE gcfg SET ten=?, giai_thich=?, canh_bao=?, nguy_co=? WHERE k=?", p, 5);
	Lua_PushNumber(L, 1);
	return 1;
}

// CFGW_Tick() -> 1 vua nap lai | 0 khong doi | -1 chua san sang
int LuaCFGW_Tick(Lua_State* L)
{
	Lua_PushNumber(L, CauHinhWeb_TuNapLai(0));
	return 1;
}

// CFGW_Reload() -> ep nap lai bat ke cfg_version (lenh GM); tra so loi (-1 chua san sang)
int LuaCFGW_Reload(Lua_State* L)
{
	const int r = CauHinhWeb_TuNapLai(1);
	Lua_PushNumber(L, (r < 0) ? -1 : s_nSoLoi);
	return 1;
}

// CFGW_Info() -> bang {ready, ver, so_khoa, so_loi, nap_cuoi, so_khai}
int LuaCFGW_Info(Lua_State* L)
{
	Lua_NewTable(L);
	sSetNum(L, "ready", s_bTableOk ? 1 : 0);
	sSetNum(L, "ver", s_nVer);
	sSetNum(L, "so_khoa", s_nSoKhoa);
	sSetNum(L, "so_loi", s_nSoLoi);
	sSetNum(L, "nap_cuoi", (double)s_tNapCuoi);
	sSetNum(L, "so_khai", (double)s_Khai.size());
	return 1;
}

#endif // _SERVER
