# -*- coding: utf-8 -*-
"""[BANDO20 06/09] Bo va ma nguon: ban do the gioi bam dia diem -> TU CHAY BO xuyen map (nhu 2.0)
+ nhan ten Viet khi tro chuot + sua loi UiMapCave.cpp:77.
Tep dich la TCVN3 (CoreShell.cpp, UiWorldMap.cpp) -> doc/ghi latin-1, chuoi Viet sinh bang
unicode_to_tcvn3_bytes (RULE 0). Moi neo phai khop DUY NHAT. Chay lai duoc (idempotent).
Dung: python p_bando_code.py <goc worktree>
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_bando"
TAG = "[BANDO20 06/09]"

def V(s):
    """chuoi Viet -> byte TCVN3 (dang str latin-1) de nhet vao ma nguon"""
    return unicode_to_tcvn3_bytes(s).decode("latin-1")

def rd(rel):
    p = os.path.join(ROOT, rel)
    d = io.open(p, "r", encoding="latin-1", newline="").read()
    eol = "\r\n" if "\r\n" in d[:4000] else "\n"
    return p, d, eol

def wr(p, d):
    io.open(p, "w", encoding="latin-1", newline="").write(d)

def hb(d):
    return sum(1 for c in d if ord(c) >= 0x80)

def once(d, pat, repl, flags=re.M, name=""):
    """thay DUY NHAT mot cho (regex). repl la ham hoac chuoi (khong xu ly backslash)."""
    ms = list(re.finditer(pat, d, flags))
    assert len(ms) == 1, "neo %s: mong 1, thay %d" % (name, len(ms))
    m = ms[0]
    new = repl(m) if callable(repl) else repl
    return d[:m.start()] + new + d[m.end():]

def each(d, pat, repl, expect, flags=re.M, name=""):
    ms = list(re.finditer(pat, d, flags))
    assert len(ms) == expect, "neo %s: mong %d, thay %d" % (name, expect, len(ms))
    out = []; last = 0
    for m in ms:
        out.append(d[last:m.start()]); out.append(repl(m)); last = m.end()
    out.append(d[last:])
    return "".join(out)

# ============================================================================
# 1) CoreShell.h - GOI_WORLDMAP_GOTO cuoi enum
# ============================================================================
p, d, E = rd(r"Sources\Core\Src\CoreShell.h")
h0 = hb(d)
if "GOI_WORLDMAP_GOTO" not in d:
    d = once(d, r"^(\t)GOI_TASKGUIDE_GOTO_VANTIEU,(\r?\n)",
             lambda m: m.group(0) +
             "\t// %s Ban do the gioi / son dong: bam dia diem -> TU CHAY BO xuyen map toi map do (nhu 2.0,%s"
             "\t// KHONG nhay map). uParam = map id (MapList N). Tra 1 = bat dau, 2 = huy (bam lai dung map dang di),%s"
             "\t// 0 = khong duoc (da bao ly do). Dat CUOI enum (Core + S3Client cung dich).%s"
             "\tGOI_WORLDMAP_GOTO,%s" % (TAG, E, E, E, E), name="enum GOI")
    wr(p, d); print("[+] CoreShell.h: GOI_WORLDMAP_GOTO")
else:
    print("[=] CoreShell.h da co")
assert hb(d) == h0

# ============================================================================
# 2) CoreShell.cpp - mo-dun TG_BanDo*
# ============================================================================
p, d, E = rd(r"Sources\Core\Src\CoreShell.cpp")
h0 = hb(d)

MSG = {
    "HUY":        V("<color=Cyan>[Bản đồ] Đã huỷ tự chạy."),
    "DATAU":      V("<color=Yellow>[Bản đồ] Auto Dã Tẩu đang chạy - để auto tự lo việc di chuyển."),
    "DANGO":      V("<color=Yellow>[Bản đồ] Ngươi đang ở ngay bản đồ này."),
    "KHONGDUONG": V("<color=Yellow>[Bản đồ] Không tìm được đường bộ tới %s - nơi này phải đi bằng Xa Phu, thuyền hoặc phù."),
    "BATDAU":     V("<color=Cyan>[Bản đồ] Đang tự chạy tới %s (qua %d cửa map) - bấm lại địa điểm đó trên bản đồ để huỷ."),
    "DEN":        V("<color=Cyan>[Bản đồ] Đã tới %s."),
    "LAC":        V("<color=Yellow>[Bản đồ] Lạc sang map khác, không tìm được đường đi tiếp - dừng tự chạy."),
    "LAU":        V("<color=Yellow>[Bản đồ] Đi quá lâu - dừng tự chạy."),
    "KHONGQUA":   V("<color=Yellow>[Bản đồ] Không qua được cửa map (bị chặn?) - dừng tự chạy."),
}

BLOCK = r'''
//---------------------------------------------------------------------------
// [BANDO20 06/09] BAN DO THE GIOI / SON DONG: bam dia diem -> TU CHAY BO xuyen map toi map do
// (nhu 2.0, KHONG nhay map). Do thi noi map = settings\MapTraffic.ini (da nap g_MapTraffic):
// trong muc [map], k_Type=0 la CUA sang map khac, k_Point = diem tren tieu ban do (MPS = x*16,
// y*32 - cung cong thuc FlagOnTarget / ve co ScenePlaceMapC.cpp:539), k_Index = map dich.
// BFS tim chuoi cua tu map dang dung toi map dich; moi chang DT_WalkTo toi cua de dam len trap
// -> may chu doi map -> sang map ke thi chang tiep; toi map dich = xong. Dung tren cua ma map
// khong doi ~2 s -> do quanh 8 huong x 1-2 o (ky hieu co the lech o trap). Bi keo sang map
// ngoai ke hoach -> tinh lai duong tu map moi (toi da 3 lan). Client KHONG co du lieu trap
// (KRegion::GetTrap tra 0 phia client) nen chi dua vao Point cua MapTraffic.
// Tick 400 ms tu KCoreShell::Breathe nhu cac TG_ khac; map su kien chan (TG_ChanMapSuKien);
// mot nguoi mot duong: bat cai nay thi tat 3 dan duong F11 va nguoc lai. Bam lai dung map = huy.
//---------------------------------------------------------------------------
struct sBDLink { int nFrom; int nTo; int nX; int nY; };	// nX/nY = MPS cua cua
static std::vector<sBDLink> g_vBDLink;				// toan bo cua noi map (nap 1 lan)
static int  g_nBDLinkLoaded = 0;
static int  g_nBDOn = 0;
static int  g_nBDTarget = 0;							// map dich
static std::vector<sBDLink> g_vBDPlan;				// chuoi cua phai qua
static int  g_nBDStep = 0;
static int  g_nBDTry = 0;							// tick trong chang hien tai
static int  g_nBDWait = 0;							// tick dung tren cua ma map chua doi
static int  g_nBDNudge = 0;							// so lan do quanh cua
static int  g_nBDLost = 0;							// tick m_nIndex <= 0 (dang doi map)
static int  g_nBDReplan = 0;						// so lan tinh lai duong
static UINT g_uBDNext = 0;
static KIniFile* g_pBDMapList = NULL;				// \Settings\MapList.ini (ten map)
#define BD_MAX_MAP		1300
#define BD_MAX_SYMBOL	64

static void BD_LoadLinks()
{
	if (g_nBDLinkLoaded)
		return;
	g_nBDLinkLoaded = 1;
	char szSect[16], szKey[32];
	for (int nMap = 1; nMap < BD_MAX_MAP; nMap++)
	{
		sprintf(szSect, "%d", nMap);
		if (!g_MapTraffic.IsSectionExist(szSect))
			continue;
		for (int j = 1; j <= BD_MAX_SYMBOL; j++)
		{
			int nType = -1;
			sprintf(szKey, "%d_Type", j);
			g_MapTraffic.GetInteger(szSect, szKey, -1, &nType);
			if (nType != 0)
				continue;
			int nX = 0, nY = 0, nTo = 0;
			sprintf(szKey, "%d_Point", j);
			g_MapTraffic.GetInteger2(szSect, szKey, &nX, &nY);
			sprintf(szKey, "%d_Index", j);
			g_MapTraffic.GetInteger(szSect, szKey, 0, &nTo);
			if (nTo <= 0 || nTo >= BD_MAX_MAP || nTo == nMap || nX <= 0 || nY <= 0)
				continue;
			sBDLink l;
			l.nFrom = nMap;
			l.nTo = nTo;
			l.nX = nX * 16;
			l.nY = nY * 32;
			g_vBDLink.push_back(l);
		}
	}
}

// ten map (MapList N_name, bo khoang trang dau) - "" neu khong co
static const char* BD_TenMap(int nMap)
{
	static char szTen[64];
	szTen[0] = 0;
	if (!g_pBDMapList)
	{
		g_pBDMapList = new KIniFile;
		if (!g_pBDMapList->Load("\\Settings\\MapList.ini"))
		{
			delete g_pBDMapList;
			g_pBDMapList = NULL;
			return szTen;
		}
	}
	char szKey[32];
	sprintf(szKey, "%d_name", nMap);
	g_pBDMapList->GetString("List", szKey, "", szTen, sizeof(szTen));
	char* pc = szTen;
	while (*pc == ' ')
		pc++;
	if (pc != szTen)
		memmove(szTen, pc, strlen(pc) + 1);
	return szTen;
}

// BFS tren do thi cua: tra so chang (0 = dang o dich, -1 = khong co duong bo)
static int BD_TimDuong(int nFrom, int nTo, std::vector<sBDLink>& vPlan)
{
	vPlan.clear();
	if (nFrom == nTo)
		return 0;
	if (nFrom <= 0 || nFrom >= BD_MAX_MAP || nTo <= 0 || nTo >= BD_MAX_MAP)
		return -1;
	BD_LoadLinks();
	static int aPrev[BD_MAX_MAP];		// chi so cua dan toi map (-1 chua toi, -2 diem xuat phat)
	static int aQueue[BD_MAX_MAP];
	for (int i = 0; i < BD_MAX_MAP; i++)
		aPrev[i] = -1;
	int nHead = 0, nTail = 0;
	aQueue[nTail++] = nFrom;
	aPrev[nFrom] = -2;
	while (nHead < nTail)
	{
		int nCur = aQueue[nHead++];
		if (nCur == nTo)
			break;
		for (int k = 0; k < (int)g_vBDLink.size(); k++)
		{
			const sBDLink& l = g_vBDLink[k];
			if (l.nFrom != nCur || aPrev[l.nTo] != -1)
				continue;
			aPrev[l.nTo] = k;
			if (nTail < BD_MAX_MAP)
				aQueue[nTail++] = l.nTo;
		}
	}
	if (aPrev[nTo] == -1)
		return -1;
	int nMap = nTo;
	while (nMap != nFrom)
	{
		int k = aPrev[nMap];
		if (k < 0)
			return -1;
		vPlan.insert(vPlan.begin(), g_vBDLink[k]);
		nMap = g_vBDLink[k].nFrom;
		if ((int)vPlan.size() > BD_MAX_MAP)
			return -1;
	}
	return (int)vPlan.size();
}

static void TG_BanDoStop(const char* szMsg)
{
	if (!g_nBDOn)
		return;
	if (szMsg)
		DT_Msg(CLIENT_PLAYER_INDEX, szMsg);
	g_nBDOn = 0;
	g_ScenePlace.RemoveFlag();	// bo duong dang di (SubWorld[0].StopPath)
}

// bat dau tu chay toi map nMap: tra 1 = di, 2 = huy (bam lai dung map dang di), 0 = khong duoc
static int TG_BanDoStart(int nMap)
{
	int nPlayerIdx = CLIENT_PLAYER_INDEX;
	if (Player[nPlayerIdx].m_nIndex <= 0)
		return 0;
	if (g_nBDOn && nMap == g_nBDTarget)
	{
		TG_BanDoStop("$HUY$");
		return 2;
	}
	if (Player[nPlayerIdx].m_sExtAuto.nDTEngaged)
	{
		DT_Msg(nPlayerIdx, "$DATAU$");
		return 0;
	}
	if (TG_ChanMapSuKien(nPlayerIdx))
		return 0;
	int nCur = SubWorld[0].m_SubWorldID;
	if (nCur == nMap)
	{
		DT_Msg(nPlayerIdx, "$DANGO$");
		return 0;
	}
	std::vector<sBDLink> vPlan;
	int nHop = BD_TimDuong(nCur, nMap, vPlan);
	char szBuf[256];
	if (nHop <= 0)
	{
		_snprintf(szBuf, sizeof(szBuf) - 1, "$KHONGDUONG$", BD_TenMap(nMap));
		szBuf[sizeof(szBuf) - 1] = 0;
		DT_Msg(nPlayerIdx, szBuf);
		return 0;
	}
	// mot nguoi mot duong: tat 3 dan duong F11
	TG_XaFuStop(NULL);
	TG_SatThuStop(NULL);
	TG_VanTieuStop(NULL);
	g_vBDPlan = vPlan;
	g_nBDTarget = nMap;
	g_nBDStep = 0;
	g_nBDTry = 0;
	g_nBDWait = 0;
	g_nBDNudge = 0;
	g_nBDLost = 0;
	g_nBDReplan = 0;
	g_uBDNext = 0;
	g_nBDOn = 1;
	_snprintf(szBuf, sizeof(szBuf) - 1, "$BATDAU$", BD_TenMap(nMap), nHop);
	szBuf[sizeof(szBuf) - 1] = 0;
	DT_Msg(nPlayerIdx, szBuf);
	return 1;
}

static void TG_BanDoTick()
{
	if (!g_nBDOn)
		return;
	int nPlayerIdx = CLIENT_PLAYER_INDEX;
	UINT uCur = timeGetTime();
	if (uCur < g_uBDNext)
		return;
	g_uBDNext = uCur + 400;
	if (Player[nPlayerIdx].m_nIndex <= 0)
	{
		if (++g_nBDLost > 50)	// ~20 s khong co nhan vat (thoat / doi map qua lau)
			g_nBDOn = 0;
		return;
	}
	g_nBDLost = 0;
	int nCur = SubWorld[0].m_SubWorldID;
	if (nCur == g_nBDTarget)
	{
		char szBuf[256];
		_snprintf(szBuf, sizeof(szBuf) - 1, "$DEN$", BD_TenMap(nCur));
		szBuf[sizeof(szBuf) - 1] = 0;
		TG_BanDoStop(szBuf);
		return;
	}
	if (g_nBDStep < 0 || g_nBDStep >= (int)g_vBDPlan.size() || g_vBDPlan[g_nBDStep].nFrom != nCur)
	{
		// vua sang map khac: dung mot map trong ke hoach? (co the nhay qua nhieu chang)
		int nFound = -1;
		for (int k = 0; k < (int)g_vBDPlan.size(); k++)
		{
			if (g_vBDPlan[k].nFrom == nCur)
			{
				nFound = k;
				break;
			}
		}
		if (nFound < 0)
		{
			// lac sang map ngoai ke hoach -> tinh lai tu map nay
			if (++g_nBDReplan > 3 || TG_ChanMapSuKien(nPlayerIdx))
			{
				TG_BanDoStop("$LAC$");
				return;
			}
			std::vector<sBDLink> vPlan;
			if (BD_TimDuong(nCur, g_nBDTarget, vPlan) <= 0)
			{
				TG_BanDoStop("$LAC$");
				return;
			}
			g_vBDPlan = vPlan;
			nFound = 0;
		}
		g_nBDStep = nFound;
		g_nBDTry = 0;
		g_nBDWait = 0;
		g_nBDNudge = 0;
		return;		// nhip sau moi di (map vua nap, luoi A* co the chua san)
	}
	if (++g_nBDTry > 450)	// ~3 phut mot chang
	{
		TG_BanDoStop("$LAU$");
		return;
	}
	const sBDLink& l = g_vBDPlan[g_nBDStep];
	int nX = l.nX, nY = l.nY;
	if (g_nBDNudge > 0)
	{
		// do quanh cua: 8 huong x 1 o, roi 8 huong x 2 o
		static const int aDX[8] = { 1, -1, 0, 0, 1, -1, 1, -1 };
		static const int aDY[8] = { 0, 0, 1, -1, 1, -1, -1, 1 };
		int n = (g_nBDNudge - 1) % 8;
		int nBuoc = 32 * (1 + (g_nBDNudge - 1) / 8);
		nX += aDX[n] * nBuoc;
		nY += aDY[n] * nBuoc;
	}
	if (DT_WalkTo(nPlayerIdx, nX, nY, 20, uCur))
	{
		// dang dung tren cua ma map chua doi: doi ~2 s roi do sang o ben canh
		if (++g_nBDWait >= 5)
		{
			g_nBDWait = 0;
			if (++g_nBDNudge > 16)
			{
				TG_BanDoStop("$KHONGQUA$");
				return;
			}
		}
	}
	else
		g_nBDWait = 0;
}
'''
for k, v in MSG.items():
    BLOCK = BLOCK.replace("$%s$" % k, v)
assert "$" not in BLOCK.replace("$(", ""), "con placeholder chua thay"
BLOCK = BLOCK.replace("\n", E)

if "TG_BanDoTick()" not in d:
    # 2a) khoi chinh: sau dau '}' ket thuc TG_VanTieuTick, truoc chu thich DT_FindFarMob
    d = once(d, r"(\r?\n\}\r?\n)(\r?\n// \[DaTau\] tim quai con SONG da sync NGOAI tam danh de chay toi \(T4 di tim quai\)\.\r?\n)",
             lambda m: m.group(1) + BLOCK + m.group(2), name="chen khoi TG_BanDo")
    # 2b) khai bao truoc (cac Start cua F11 goi TG_BanDoStop truoc khi dinh nghia)
    d = once(d, r"^static int TG_ChanMapSuKien\(int nPlayerIdx\);\t// \[VTCN 06/09\] dinh nghia sau KMapSuKien\.h[^\r\n]*(\r?\n)",
             lambda m: m.group(0) + "static void TG_BanDoStop(const char* szMsg);\t// %s dinh nghia sau TG_VanTieu (ban do: tu chay bo)%s" % (TAG, m.group(1)),
             name="khai bao truoc TG_BanDoStop")
    # 2c) mot nguoi mot duong: 3 dan duong F11 bat len thi tat ban do
    d = each(d, r"^(\s*)g_nTGXaFuOn = 1;", lambda m: "%sTG_BanDoStop(NULL);\t// %s mot nguoi mot duong%s%sg_nTGXaFuOn = 1;" % (m.group(1), TAG, E, m.group(1)), 1, name="XaFu On")
    d = each(d, r"^(\s*)g_nTGSTOn = 1;", lambda m: "%sTG_BanDoStop(NULL);\t// %s mot nguoi mot duong%s%sg_nTGSTOn = 1;" % (m.group(1), TAG, E, m.group(1)), 2, name="SatThu On")
    d = each(d, r"^(\s*)g_nTGVTOn = 1;", lambda m: "%sTG_BanDoStop(NULL);\t// %s mot nguoi mot duong%s%sg_nTGVTOn = 1;" % (m.group(1), TAG, E, m.group(1)), 1, name="VanTieu On")
    # 2d) dispatch OperationRequest
    d = once(d, r"^(\t\t)nRet = TG_VanTieuStart\(\(int\)uParam\);(\r?\n)\t\tbreak;(\r?\n)",
             lambda m: m.group(0) + "\tcase GOI_WORLDMAP_GOTO:\t// %s ban do the gioi / son dong: bam dia diem -> tu chay bo toi map (uParam = map id)%s\t\tnRet = TG_BanDoStart((int)uParam);%s\t\tbreak;%s" % (TAG, E, E, E),
             name="dispatch GOI_WORLDMAP_GOTO")
    # 2e) tick trong Breathe (2 nhanh do/khong do)
    d = each(d, r"^(\s*)TG_VanTieuTick\(\);[^\r\n]*(\r?\n)",
             lambda m: m.group(0) + "%sTG_BanDoTick();\t// %s ban do: tu chay bo toi map da bam (chi chay khi dang bat)%s" % (m.group(1), TAG, m.group(2)),
             2, name="Breathe tick")
    wr(p, d); print("[+] CoreShell.cpp: TG_BanDo* + dispatch + tick + dung cheo")
else:
    print("[=] CoreShell.cpp da co")
h1 = hb(d)
print("    CoreShell.cpp high-byte %d -> %d (+%d = chuoi Viet moi)" % (h0, h1, h1 - h0))

# ============================================================================
# 3) UiWorldMap.h - lop dung chung KWorldMapLocs + PaintWindow
# ============================================================================
p, d, E = rd(r"Sources\S3Client\Ui\UiCase\UiWorldMap.h")
h0 = hb(d)
if "KWorldMapLocs" not in d:
    HDR = r'''
// [BANDO20 06/09] Bang dia diem tren anh ban do (settings\MapList.ini: N_MapPos / N_name / N_MapType),
// dung chung cho ban do the gioi + ban do son dong (cung anh 752x576, cung toa do): tro chuot ->
// ve ten Viet cua dia diem; bam -> GOI_WORLDMAP_GOTO (Core tu chay bo toi map do, nhu 2.0).
// Tam dia diem tren anh = N_MapPos + (30,17) - cung do lech voi ky hieu 'nguoi o day' cua KUiWorldmap.
class KIniFile;
#define WORLDMAP_MAX_LOC	320
struct KWorldMapLoc
{
	int		nMapId;
	int		nX, nY;			// tam tren anh (da cong +30,+17)
	char	szName[48];		// ten Viet (TCVN3)
	char	szType[16];		// City / Capital / Cave / Field / Battlefield / Tong / Country / Others
};
class KWorldMapLocs
{
public:
	KWorldMapLocs() : m_nCount(0), m_nHover(-1) {}
	void	Load(KIniFile* pIni);							// doc N_MapPos / N_name / N_MapType (N = 1..1200)
	int		Hit(int nLocalX, int nLocalY) const;			// dia diem gan nhat trong 14 px, -1 = khong
	int		OnClick(int nLocalX, int nLocalY);				// -1 khong trung; con lai = ket qua GOI_WORLDMAP_GOTO (1 di / 2 huy / 0 khong)
	void	PaintHover(int nAbsLeft, int nAbsTop, int nWndWidth);	// tu do vi tri chuot, ve ten dia diem dang tro
	int		GetCount() const { return m_nCount; }
private:
	KWorldMapLoc	m_aLoc[WORLDMAP_MAX_LOC];
	int				m_nCount;
	int				m_nHover;
};
'''.replace("\n", E)
    d = once(d, r"^class KUiWorldmap : protected KWndImage(\r?\n)", lambda m: HDR + E + m.group(0), name="truoc class KUiWorldmap")
    d = once(d, r"^(\tvoid\tBreathe\(\);[ \t]*\r?\n)",
             lambda m: m.group(1) + "\tvoid\tPaintWindow();\t\t// %s ve anh + ten dia diem dang tro chuot%s" % (TAG, E), name="PaintWindow decl")
    d = once(d, r"^(\tKWndPureTextBtn\t\tm_dali;[ \t]*\r?\n)",
             lambda m: m.group(1) + "\tKWorldMapLocs\t\tm_Locs;\t\t// %s bang dia diem (tro/bam)%s" % (TAG, E), name="m_Locs member")
    wr(p, d); print("[+] UiWorldMap.h: KWorldMapLocs + PaintWindow + m_Locs")
else:
    print("[=] UiWorldMap.h da co")
assert hb(d) == h0

# ============================================================================
# 4) UiWorldMap.cpp - nap bang, WndProc (tro/bam), PaintWindow, than KWorldMapLocs
# ============================================================================
p, d, E = rd(r"Sources\S3Client\Ui\UiCase\UiWorldMap.cpp")
h0 = hb(d)
if "KWorldMapLocs::Load" not in d:
    # 4a) include WndWindow.h (GetColor) + TextProcessDef (KOutputTextParam)
    d = once(d, r'^#include "\.\./\.\./\.\./core/src/GameDataDef\.h"(\r?\n)',
             lambda m: m.group(0) + '#include "../Elem/WndWindow.h"\t// %s GetColor%s#include "../../../Represent/iRepresent/Text/TextProcessDef.h"\t// %s KOutputTextParam%s' % (TAG, E, TAG, E),
             name="include")
    # 4b) nap bang trong UpdateData ngay sau Ini.Load thanh cong
    d = once(d, r"(\t\tif \(Ini\.Load\(WORLD_MAP_INFO_FILE\)\)\r?\n\t\t\{\r?\n)",
             lambda m: m.group(1) + "\t\t\tm_Locs.Load(&Ini);\t// %s bang dia diem (ten Viet + toa do) de tro chuot / bam%s" % (TAG, E),
             name="UpdateData Load")
    # 4c) WndProc: bam dia diem -> di; bam cho khac / chuot phai / phim -> dong (nhu cu)
    OLD = r"\tcase WM_LBUTTONDOWN:\r?\n\tcase WM_RBUTTONDOWN:\r?\n\tcase WM_KEYDOWN:\r?\n\t\tCloseWindow\(\);\r?\n\t\tnResult = true;\r?\n\t\tbreak;\r?\n"
    NEW = ("\tcase WM_LBUTTONDOWN:\t// %s bam dia diem -> Core tu chay bo toi map do; bam cho khac = dong (nhu cu)" % TAG + E +
           "\t\t{" + E +
           "\t\t\tint nCX = 0, nCY = 0;" + E +
           "\t\t\tWnd_GetCursorPos(&nCX, &nCY);" + E +
           "\t\t\tint nGo = m_Locs.OnClick(nCX - m_nAbsoluteLeft, nCY - m_nAbsoluteTop);" + E +
           "\t\t\tif (nGo == 0)\t// trung dia diem nhung khong di duoc (da bao ly do) -> giu ban do mo" + E +
           "\t\t\t{" + E +
           "\t\t\t\tnResult = true;" + E +
           "\t\t\t\tbreak;" + E +
           "\t\t\t}" + E +
           "\t\t\tCloseWindow();" + E +
           "\t\t\tnResult = true;" + E +
           "\t\t}" + E +
           "\t\tbreak;" + E +
           "\tcase WM_RBUTTONDOWN:" + E +
           "\tcase WM_KEYDOWN:" + E +
           "\t\tCloseWindow();" + E +
           "\t\tnResult = true;" + E +
           "\t\tbreak;" + E)
    d = once(d, OLD, NEW, name="WndProc")
    # 4d) than: PaintWindow + KWorldMapLocs (cuoi tep)
    FMT2 = V("%s (%s) - bấm để chạy tới")
    FMT1 = V("%s - bấm để chạy tới")
    LOAI = [("City", V("Thành")), ("Capital", V("Kinh đô")), ("Cave", V("Sơn động")), ("Field", V("Dã ngoại")),
            ("Battlefield", V("Chiến trường")), ("Tong", V("Bang phái")), ("Country", V("Nước")), ("Others", V("Khác"))]
    # dung "\n" o day: TAIL.replace("\n", E) ben duoi se doi EOL (dung E o day -> \r\r\n, git coi tep la binary)
    loai_tbl = "".join('\t\t{ "%s", "%s" },\n' % (a, b) for a, b in LOAI)
    TAIL = (r'''
// ===========================================================================
// [BANDO20 06/09] KWorldMapLocs - bang dia diem tren anh ban do (dung chung the gioi + son dong)
// ===========================================================================
static const char* WML_LoaiTen(const char* szType)
{
	static const char* aLoai[][2] =
	{
$LOAI$	};
	for (int i = 0; i < (int)(sizeof(aLoai) / sizeof(aLoai[0])); i++)
		if (stricmp(szType, aLoai[i][0]) == 0)
			return aLoai[i][1];
	return "";
}

void KWorldMapLocs::Load(KIniFile* pIni)
{
	m_nCount = 0;
	m_nHover = -1;
	if (!pIni)
		return;
	char szKey[32];
	for (int i = 1; i <= 1200 && m_nCount < WORLDMAP_MAX_LOC; i++)
	{
		int nX = -1, nY = -1;
		sprintf(szKey, "%d_MapPos", i);
		pIni->GetInteger2("List", szKey, &nX, &nY);
		if (nX < 0 || nY < 0)
			continue;
		KWorldMapLoc& l = m_aLoc[m_nCount];
		l.nMapId = i;
		l.nX = nX + 30;		// cung do lech voi ky hieu 'nguoi o day' (KUiWorldmap::UpdateData)
		l.nY = nY + 17;
		sprintf(szKey, "%d_name", i);
		l.szName[0] = 0;
		pIni->GetString("List", szKey, "", l.szName, sizeof(l.szName));
		char* pc = l.szName;
		while (*pc == ' ')
			pc++;
		if (pc != l.szName)
			memmove(l.szName, pc, strlen(pc) + 1);
		if (l.szName[0] == 0)
			continue;
		sprintf(szKey, "%d_MapType", i);
		l.szType[0] = 0;
		pIni->GetString("List", szKey, "", l.szType, sizeof(l.szType));
		m_nCount++;
	}
}

int KWorldMapLocs::Hit(int nLocalX, int nLocalY) const
{
	int nBest = -1, nBestD = 14 * 14 + 1;
	for (int i = 0; i < m_nCount; i++)
	{
		int dx = m_aLoc[i].nX - nLocalX;
		int dy = m_aLoc[i].nY - nLocalY;
		int dd = dx * dx + dy * dy;
		if (dd < nBestD)
		{
			nBestD = dd;
			nBest = i;
		}
	}
	return nBest;
}

int KWorldMapLocs::OnClick(int nLocalX, int nLocalY)
{
	int nLoc = Hit(nLocalX, nLocalY);
	if (nLoc < 0 || g_pCoreShell == NULL)
		return -1;
	return g_pCoreShell->OperationRequest(GOI_WORLDMAP_GOTO, (unsigned int)m_aLoc[nLoc].nMapId, 0);
}

// Tu do vi tri chuot moi khung (khong dua vao WM_MOUSEMOVE - chuot dang o tren nut con thi
// cha khong nhan WM_MOUSEMOVE, nhan se dinh lai). Ve ten dia diem canh diem, tranh trai ra ngoai cua so.
void KWorldMapLocs::PaintHover(int nAbsLeft, int nAbsTop, int nWndWidth)
{
	if (m_nCount <= 0 || g_pRepresentShell == NULL)
		return;
	int nCX = 0, nCY = 0;
	Wnd_GetCursorPos(&nCX, &nCY);
	m_nHover = Hit(nCX - nAbsLeft, nCY - nAbsTop);
	if (m_nHover < 0)
		return;
	const KWorldMapLoc& l = m_aLoc[m_nHover];
	const char* szLoai = WML_LoaiTen(l.szType);
	char szText[128];
	int nLen;
	if (szLoai[0])
		nLen = _snprintf(szText, sizeof(szText) - 1, "$FMT2$", l.szName, szLoai);
	else
		nLen = _snprintf(szText, sizeof(szText) - 1, "$FMT1$", l.szName);
	if (nLen < 0)
		nLen = sizeof(szText) - 1;
	szText[sizeof(szText) - 1] = 0;
	int nW = nLen * 6;		// font 12: ~6 px / ky tu
	int nX = l.nX + 12;
	if (nX + nW > nWndWidth - 4)
		nX = l.nX - 12 - nW;
	if (nX < 2)
		nX = 2;
	int nY = l.nY - 7;
	if (nY < 2)
		nY = 2;
	KOutputTextParam param;
	param.Color = GetColor("255,255,80");
	param.BorderColor = GetColor("0,0,0");
	param.nX = nAbsLeft + nX;
	param.nY = nAbsTop + nY;
	param.nZ = TEXT_IN_SINGLE_PLANE_COORD;
	param.nSkipLine = 0;
	param.nNumLine = 1;
	g_pRepresentShell->OutputRichText(12, &param, szText, nLen);
}

// [BANDO20 06/09] ve anh ban do roi ten dia diem dang tro chuot
void KUiWorldmap::PaintWindow()
{
	KWndImage::PaintWindow();
	m_Locs.PaintHover(m_nAbsoluteLeft, m_nAbsoluteTop, m_Width);
}
''').replace("$LOAI$", loai_tbl).replace("$FMT2$", FMT2).replace("$FMT1$", FMT1).replace("\n", E)
    if not d.endswith(E):
        d += E
    d += TAIL
    wr(p, d); print("[+] UiWorldMap.cpp: Load + WndProc + PaintWindow + KWorldMapLocs")
else:
    print("[=] UiWorldMap.cpp da co")
h1 = hb(d)
print("    UiWorldMap.cpp high-byte %d -> %d (+%d = chuoi Viet moi)" % (h0, h1, h1 - h0))

# ============================================================================
# 5) UiMapCave.h / .cpp - sua loi sprintf + cung tinh nang tro/bam
# ============================================================================
p, d, E = rd(r"Sources\S3Client\Ui\UiCase\UiMapCave.h")
h0 = hb(d)
if "KWorldMapLocs" not in d:
    d = once(d, r'^#include "\.\./Elem/UiImage\.h"(\r?\n)',
             lambda m: m.group(0) + '#include "UiWorldMap.h"\t// %s KWorldMapLocs (bang dia diem dung chung)%s' % (TAG, E), name="cave include")
    d = once(d, r"^(\tvoid\tBreathe\(\);[ \t]*\r?\n)",
             lambda m: m.group(1) + "\tvoid\tPaintWindow();\t\t// %s ve anh + ten dia diem dang tro chuot%s" % (TAG, E), name="cave PaintWindow decl")
    d = once(d, r"^(\tKWndImage\t\t\tm_Sign;[ \t]*\r?\n)",
             lambda m: m.group(1) + "\tKWorldMapLocs\t\tm_Locs;\t\t// %s bang dia diem (tro/bam)%s" % (TAG, E), name="cave m_Locs")
    wr(p, d); print("[+] UiMapCave.h")
else:
    print("[=] UiMapCave.h da co")
assert hb(d) == h0

p, d, E = rd(r"Sources\S3Client\Ui\UiCase\UiMapCave.cpp")
h0 = hb(d)
if "m_Locs.OnClick" not in d:
    # 5a) LOI: sprintf ghi de duong dan giao dien -> \UiMapCave.ini khong ton tai
    d = once(d, r'^\tsprintf\(szBuffer, "\\\\%s", SCHEME_INI_WORLD\);(\r?\n)',
             lambda m: ("\t// %s LOI CU: sprintf GHI DE duong dan giao dien vua lay -> nap \"\\UiMapCave.ini\" (khong ton tai)" % TAG + m.group(1) +
                        "\t// => cua so khong co Left/Top/Width/Height va mui ten [Sign] 'nguoi o day' khong bao gio hien." + m.group(1) +
                        "\t// Noi them nhu KUiWorldmap::Initialize." + m.group(1) +
                        "\tstrcat(szBuffer, \"\\\\\");" + m.group(1) +
                        "\tstrcat(szBuffer, SCHEME_INI_WORLD);" + m.group(1)),
             name="cave sprintf bug")
    # 5b) nap bang dia diem
    d = once(d, r"(\t\tif \(Ini\.Load\(WORLD_MAP_INFO_FILE\)\)\r?\n\t\t\{\r?\n)",
             lambda m: m.group(1) + "\t\t\tm_Locs.Load(&Ini);\t// %s bang dia diem (ten Viet + toa do) de tro chuot / bam%s" % (TAG, E),
             name="cave UpdateData Load")
    # 5c) ky hieu 'nguoi o day' cung do lech +30,+17 voi ban do the gioi (cung anh 752x576)
    d = once(d, r"^\t\t\t\t\tm_Sign\.SetPosition\(nAreaX - nWidth / 2,(\r?\n)\t\t\t\t\t\tnAreaY - nHeight / 2\);(\r?\n)",
             lambda m: "\t\t\t\t\tm_Sign.SetPosition((nAreaX - nWidth / 2) + 30,\t// %s cung do lech +30,+17 voi KUiWorldmap (cung anh)%s\t\t\t\t\t\t(nAreaY - nHeight / 2) + 17);%s" % (TAG, m.group(1), m.group(2)),
             name="cave Sign offset")
    # 5d) WndProc
    OLD = r"\tcase WM_LBUTTONDOWN:\r?\n\tcase WM_RBUTTONDOWN:\r?\n\tcase WM_KEYDOWN:\r?\n\t\tCloseWindow\(\);\r?\n\t\tnResult = true;\r?\n\t\tbreak;\r?\n"
    NEW = ("\tcase WM_LBUTTONDOWN:\t// %s bam dia diem -> Core tu chay bo toi map do; bam cho khac = dong (nhu cu)" % TAG + E +
           "\t\t{" + E +
           "\t\t\tint nCX = 0, nCY = 0;" + E +
           "\t\t\tWnd_GetCursorPos(&nCX, &nCY);" + E +
           "\t\t\tint nGo = m_Locs.OnClick(nCX - m_nAbsoluteLeft, nCY - m_nAbsoluteTop);" + E +
           "\t\t\tif (nGo == 0)\t// trung dia diem nhung khong di duoc (da bao ly do) -> giu ban do mo" + E +
           "\t\t\t{" + E +
           "\t\t\t\tnResult = true;" + E +
           "\t\t\t\tbreak;" + E +
           "\t\t\t}" + E +
           "\t\t\tCloseWindow();" + E +
           "\t\t\tnResult = true;" + E +
           "\t\t}" + E +
           "\t\tbreak;" + E +
           "\tcase WM_RBUTTONDOWN:" + E +
           "\tcase WM_KEYDOWN:" + E +
           "\t\tCloseWindow();" + E +
           "\t\tnResult = true;" + E +
           "\t\tbreak;" + E)
    d = once(d, OLD, NEW, name="cave WndProc")
    # 5e) PaintWindow cuoi tep
    TAIL = ("" + E +
            "// %s ve anh ban do son dong roi ten dia diem dang tro chuot" % TAG + E +
            "void KUiMapCave::PaintWindow()" + E +
            "{" + E +
            "\tKWndImage::PaintWindow();" + E +
            "\tm_Locs.PaintHover(m_nAbsoluteLeft, m_nAbsoluteTop, m_Width);" + E +
            "}" + E)
    if not d.endswith(E):
        d += E
    d += TAIL
    wr(p, d); print("[+] UiMapCave.cpp: sua sprintf + Load + Sign +30,+17 + WndProc + PaintWindow")
else:
    print("[=] UiMapCave.cpp da co")
assert hb(d) == h0
print("XONG.")
