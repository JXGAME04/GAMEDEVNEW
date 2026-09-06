# -*- coding: utf-8 -*-
"""patch_cpp.py [VTCN 06/09] - va C++ trong worktree D:\GAMEDEVNEW_wt_vtcn cho muc VAN TIEU tren bang
Chi nam nhiem vu (F11): enum GOI_TASKGUIDE_GOTO_VANTIEU, bo dan duong TG_VanTieu* (CoreShell.cpp),
danh sach trang UI_CMD case 6 (KProtocolProcess.cpp), 2 muc moi trong KUiTaskGuide.
Moi sua qua latin-1 (byte-safe), chi chen/thay ASCII, kiem so lan khop. Chay: python patch_cpp.py [--dry]
"""
import io, os, sys

WT = r"D:\GAMEDEVNEW_wt_vtcn\Sources"
DRY = "--dry" in sys.argv

def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()

def wr(p, s):
    if DRY:
        print("  (dry) ghi", p)
        return
    io.open(p, "w", encoding="latin-1", newline="").write(s)

def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)

def eol_of(s):
    return "\r\n" if "\r\n" in s else "\n"

def rep(s, old, new, count=1, tag=""):
    e = eol_of(s)
    old = old.replace("\n", e)
    new = new.replace("\n", e)
    n = s.count(old)
    if n != count:
        sys.exit("LOI %s: mong %d khop, thay %d: %r" % (tag, count, n, old[:80]))
    return s.replace(old, new)

def patch(rel, fn):
    p = os.path.join(WT, rel)
    s0 = rd(p)
    s1 = fn(s0)
    if s1 == s0:
        sys.exit("LOI %s khong doi" % rel)
    assert "\ufffd" not in s1
    if hb(s1) != hb(s0):
        sys.exit("LOI %s: byte cao doi %d -> %d (chi duoc chen ASCII)" % (rel, hb(s0), hb(s1)))
    print("%-45s OK (byte cao %d giu nguyen)" % (rel, hb(s0)))
    wr(p, s1)

# ============================================================ CoreShell.h
def f_coreshell_h(s):
    old = "\tGOI_CHIENLENH_UI,\t\t\t// [CL 04/09 DOT2] cua so Chien Lenh -> ChienLenhUi_OnRequest\n"
    new = old + ("\n\t// [VTCN 06/09] bang F11 muc Van tieu: uParam = TaskId (11 ca nhan / 12 ap Tieu Bang) ->\n"
                 "\t// tu dan duong toi NPC nhan nhiem vu (bam lai = huy). Dat CUOI enum (Core + S3Client cung dich).\n"
                 "\tGOI_TASKGUIDE_GOTO_VANTIEU,\n")
    return rep(s, old, new, 1, "CoreShell.h enum")

# ============================================================ CoreShell.cpp
TG_VT = r'''
//---------------------------------------------------------------------------
// [VTCN 06/09] Bam muc Van tieu tren bang F11 -> tu dan duong toi NPC nhan nhiem vu:
//   nhanh CA NHAN (TaskId 11): chua nhan -> Ong chu Tieu cuc (Long Mon tran; khac map thi
//     nho may chu thue xe qua UI_CMD 'vt_goto_canhan'); da nhan, xe chua xuat phat -> Tieu Su
//     diem dau (may chu dua di, mien phi); xe da xuat phat -> Tieu Su diem cuoi neu cung map.
//   nhanh BANG (TaskId 12): chua giu xe -> Tiep Dan ap Tieu Bang o thanh dang dung (ngoai
//     thanh thi dung phu ve thanh nhu TG_SatThu pha 10); dang giu xe -> Nhan Hang diem cuoi
//     neu cung map (xe phai di bo, khong dan qua ban do).
// So lieu KVanTieuPos.h sinh tu extend.lua cua may chu (task 4179 = tuyen*10 + dao chieu,
// 4180 = moc xuat phat). Tai dung DT_WalkTo / TG_SatThuTimNpc / DT_UsePortal. Bam lai = huy.
//---------------------------------------------------------------------------
#include "KVanTieuPos.h"
#include "KVanTieuMsg.h"

static int  g_nTGVTOn = 0;
static int  g_nTGVTPhase = 0;		// 1 doi may chu doi map, 2 doi phu ve thanh, 3 di bo toi NPC
static int  g_nTGVTMap = 0;			// map dich
static int  g_nTGVTX = 0, g_nTGVTY = 0;	// o cell NPC dich
static int  g_nTGVTTpl = 0;			// template NPC dich
static const char* g_szTGVTTen = "";
static int  g_nTGVTTry = 0;
static UINT g_uTGVTNext = 0;

static void TG_VanTieuStop(const char* szMsg)
{
	if (g_nTGVTOn && szMsg)
		DT_Msg(CLIENT_PLAYER_INDEX, szMsg);
	g_nTGVTOn = 0;
}

// map nay co Tiep Dan ap Tieu Bang khong -> chi so diem trong tbBJPoints, 0 = khong
static int TG_VanTieuDiemTiepDan(int nMap)
{
	for (int i = 0; i < VT_TIEPDAN_SO; i++)
		if (s_nVTDiemMap[s_nVTTiepDanDiem[i]] == nMap)
			return s_nVTTiepDanDiem[i];
	return 0;
}

static void TG_VanTieuDatDich(int nMap, int nX, int nY, int nTpl, const char* szTen)
{
	g_nTGVTMap = nMap;
	g_nTGVTX = nX;
	g_nTGVTY = nY;
	g_nTGVTTpl = nTpl;
	g_szTGVTTen = szTen;
}

// nTaskId = 11 (ca nhan) / 12 (bang). Tra 1 neu vua bat.
static int TG_VanTieuStart(int nTaskId)
{
	int nPlayerIdx = CLIENT_PLAYER_INDEX;
	if (g_nTGVTOn)
	{
		TG_VanTieuStop(VT_MSG_HUY);
		return 0;
	}
	if (Player[nPlayerIdx].m_nIndex <= 0)
		return 0;
	if (Player[nPlayerIdx].m_sExtAuto.nDTEngaged)
	{
		DT_Msg(nPlayerIdx, VT_MSG_DT);
		return 0;
	}
	int nMapNow = SubWorld[0].m_SubWorldID;
	int nFlag = (int)Player[nPlayerIdx].m_cTask.GetSaveVal(4179);	// TSK_LMBJTaskFlag
	int nTuyen = nFlag / 10;
	int nDao = nFlag % 10;
	int nSao = 0, nDiemDau = 0, nDiemCuoi = 0;
	if (nTuyen >= 1 && nTuyen <= VT_TUYEN_SO)
	{
		nSao = s_nVTTuyenSao[nTuyen];
		nDiemDau = nDao ? s_nVTTuyenCuoi[nTuyen] : s_nVTTuyenDau[nTuyen];
		nDiemCuoi = nDao ? s_nVTTuyenDau[nTuyen] : s_nVTTuyenCuoi[nTuyen];
	}
	int nXuatPhat = (int)Player[nPlayerIdx].m_cTask.GetSaveVal(4180);	// TSK_LMBJTaskTime
	g_nTGVTTry = 0;
	g_uTGVTNext = 0;
	g_nTGVTPhase = 3;

	if (nTaskId == 11)
	{
		if (nSao >= 1 && nSao <= 9)
		{
			if (nXuatPhat != 0)
			{
				if (s_nVTDiemMap[nDiemCuoi] != nMapNow)
				{
					DT_Msg(nPlayerIdx, VT_MSG_CN_XUATPHAT);
					return 0;
				}
				TG_VanTieuDatDich(nMapNow, s_nVTDiemX[nDiemCuoi], s_nVTDiemY[nDiemCuoi], VT_TPL_TIEUSU, VT_NPC_TIEUSU);
			}
			else
			{
				TG_VanTieuDatDich(s_nVTDiemMap[nDiemDau], s_nVTDiemX[nDiemDau], s_nVTDiemY[nDiemDau], VT_TPL_TIEUSU, VT_NPC_TIEUSU);
				if (g_nTGVTMap != nMapNow)
				{
					DT_Msg(nPlayerIdx, VT_MSG_CN_DIEMDAU);
					SendUiCmdScript(6, (char*)"vt_goto_canhan");	// may chu dua toi Tieu Su diem dau (mien phi)
					g_nTGVTPhase = 1;
				}
			}
		}
		else
		{
			TG_VanTieuDatDich(VT_MAP_LONGMON, VT_CQ_X, VT_CQ_Y, VT_TPL_CHUONGQUY, VT_NPC_CHUONGQUY);
			if (nMapNow != VT_MAP_LONGMON)
			{
				DT_Msg(nPlayerIdx, VT_MSG_XE_THUE);
				SendUiCmdScript(6, (char*)"vt_goto_canhan");	// may chu thue xe (tru tien) toi Long Mon tran
				g_nTGVTPhase = 1;
			}
		}
	}
	else
	{
		if (nSao >= 10)
		{
			if (s_nVTDiemMap[nDiemCuoi] != nMapNow)
			{
				DT_Msg(nPlayerIdx, VT_MSG_BH_XE);
				return 0;
			}
			TG_VanTieuDatDich(nMapNow, s_nVTDiemX[nDiemCuoi], s_nVTDiemY[nDiemCuoi], VT_TPL_NHANHANG, VT_NPC_NHANHANG);
		}
		else
		{
			int nDiem = TG_VanTieuDiemTiepDan(nMapNow);
			if (nDiem)
				TG_VanTieuDatDich(nMapNow, s_nVTDiemX[nDiem], s_nVTDiemY[nDiem], VT_TPL_TIEPDAN, VT_NPC_TIEPDAN);
			else
			{
				TG_VanTieuDatDich(0, 0, 0, VT_TPL_TIEPDAN, VT_NPC_TIEPDAN);	// chot map sau khi ve thanh
				DT_Msg(nPlayerIdx, VT_MSG_PHU);
				g_nTGVTPhase = 2;
			}
		}
	}
	g_nTGVTOn = 1;
	if (g_nTGVTPhase == 3)
	{
		char szBuf[256];
		_snprintf(szBuf, sizeof(szBuf) - 1, VT_MSG_TOI_NPC_FMT, g_szTGVTTen);
		szBuf[sizeof(szBuf) - 1] = 0;
		DT_Msg(nPlayerIdx, szBuf);
	}
	return 1;
}

static void TG_VanTieuTick()
{
	if (!g_nTGVTOn)
		return;
	int nPlayerIdx = CLIENT_PLAYER_INDEX;
	if (Player[nPlayerIdx].m_nIndex <= 0)
	{
		g_nTGVTOn = 0;
		return;
	}
	UINT uCur = timeGetTime();
	if (uCur < g_uTGVTNext)
		return;
	g_uTGVTNext = uCur + 400;
	if (++g_nTGVTTry > 450)	// ~3 phut
	{
		TG_VanTieuStop(VT_MSG_LAU);
		return;
	}
	int nMapNow = SubWorld[0].m_SubWorldID;
	// --- pha 1: doi may chu doi map (thue xe / dua toi Tieu Su) ---
	if (g_nTGVTPhase == 1)
	{
		if (nMapNow == g_nTGVTMap)
		{
			g_nTGVTPhase = 3;
			g_nTGVTTry = 0;
			return;
		}
		if (g_nTGVTTry > 25)	// ~10 giay khong doi map: thieu tien / may chu tu choi
			TG_VanTieuStop(VT_MSG_CHUYENMAP);
		return;
	}
	// --- pha 2: ngoai thanh -> dung phu ve thanh roi chot Tiep Dan cua thanh do ---
	if (g_nTGVTPhase == 2)
	{
		int nDiem = TG_VanTieuDiemTiepDan(nMapNow);
		if (nDiem)
		{
			TG_VanTieuDatDich(nMapNow, s_nVTDiemX[nDiem], s_nVTDiemY[nDiem], VT_TPL_TIEPDAN, VT_NPC_TIEPDAN);
			g_nTGVTPhase = 3;
			g_nTGVTTry = 0;
			return;
		}
		if ((g_nTGVTTry % 12) == 1)	// ~5 giay thu dung phu mot lan
		{
			if (!DT_UsePortal(nPlayerIdx))
			{
				TG_VanTieuStop(VT_MSG_KHONGPHU);
				return;
			}
		}
		return;
	}
	// --- pha 3: di bo toi NPC dich (cung map) ---
	if (nMapNow != g_nTGVTMap)
	{
		g_nTGVTOn = 0;	// bi keo sang map khac - tat im lang
		return;
	}
	int nDX = g_nTGVTX * 32;
	int nDY = g_nTGVTY * 32;
	int nNpc = TG_SatThuTimNpc(g_nTGVTTpl, nDX, nDY, 600);
	if (nNpc)
	{
		int nX, nY, dX, dY;
		Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
		Npc[nNpc].GetMpsPos(&dX, &dY);
		if (g_GetDistance(nX, nY, dX, dY) <= 160)
		{
			Player[nPlayerIdx].DialogNpc(nNpc);
			char szBuf[256];
			_snprintf(szBuf, sizeof(szBuf) - 1, VT_MSG_DEN_NPC_FMT, g_szTGVTTen);
			szBuf[sizeof(szBuf) - 1] = 0;
			TG_VanTieuStop(szBuf);
			return;
		}
		DT_WalkTo(nPlayerIdx, dX, dY, 128, uCur);
		return;
	}
	if (DT_WalkTo(nPlayerIdx, nDX, nDY, 200, uCur))
		TG_VanTieuStop(VT_MSG_KHONG_NPC);	// da toi o quy dinh ma khong thay NPC
}
'''

def f_coreshell_cpp(s):
    # (b) khoi TG_VanTieu chen truoc phan DT_FindFarMob
    old = "\n// [DaTau] tim quai con SONG da sync NGOAI tam danh de chay toi (T4 di tim quai).\n"
    new = TG_VT + old
    s = rep(s, old, new, 1, "CoreShell TG block")
    # (c) OperationRequest
    old = ("\tcase GOI_TASKGUIDE_GOTO_XAFU:\t// [TaskGuide] bang F11: nhiem vu loai 4 -> tu chay den Xa Phu\n"
           "\t\tnRet = TG_XaFuStart();\n"
           "\t\tbreak;\n")
    new = old + ("\tcase GOI_TASKGUIDE_GOTO_VANTIEU:\t// [VTCN 06/09] bang F11 muc Van tieu: uParam = TaskId 11/12 -> dan duong toi NPC\n"
                 "\t\tnRet = TG_VanTieuStart((int)uParam);\n"
                 "\t\tbreak;\n")
    s = rep(s, old, new, 1, "CoreShell case")
    # (d) 2 cho tick
    old = "\t\tTG_SatThuTick();\t// [3HD C20] dan duong toi boss Sat Thu\n"
    new = old + "\t\tTG_VanTieuTick();\t// [VTCN 06/09] dan duong van tieu (chi chay khi dang bat)\n"
    s = rep(s, old, new, 1, "CoreShell tick 2tab")
    old = "\n\tTG_SatThuTick();\t// [3HD C20] dan duong toi boss Sat Thu\n"
    new = old + "\tTG_VanTieuTick();\t// [VTCN 06/09] dan duong van tieu (chi chay khi dang bat)\n"
    s = rep(s, old, new, 1, "CoreShell tick 1tab")
    return s

# ============================================================ KProtocolProcess.cpp
def f_kpp(s):
    old = ('\t\t\t\tPlayer[nIndex].ExecuteScript("\\\\script\\\\task\\\\tollgate\\\\killer\\\\nieshichen.lua", "cancel", 0);\n'
           "\t\t\t}\n"
           "\t\t\tbreak;\n")
    new = ('\t\t\t\tPlayer[nIndex].ExecuteScript("\\\\script\\\\task\\\\tollgate\\\\killer\\\\nieshichen.lua", "cancel", 0);\n'
           "\t\t\t}\n"
           "\t\t\telse\n"
           "\t\t\t{\n"
           "\t\t\t\t// [VTCN 06/09] bang F11 muc Van tieu: dan duong (vt_goto_canhan) / nut Bo nhiem vu\n"
           "\t\t\t\t// (vt_quit_canhan, vt_quit_bang) -> script\\event\\lmbiaoche\\vt_chinam.lua. Chep ra bo dem\n"
           "\t\t\t\t// co ket thuc vi szFunc[32] tu client khong chac co NUL (khuon case 7).\n"
           "\t\t\t\tchar szFunVT[sizeof(pUiCmd->szFunc) + 1];\n"
           "\t\t\t\tmemcpy(szFunVT, pUiCmd->szFunc, sizeof(pUiCmd->szFunc));\n"
           "\t\t\t\tszFunVT[sizeof(pUiCmd->szFunc)] = 0;\n"
           '\t\t\t\tif (!strcmp(szFunVT, "vt_goto_canhan") || !strcmp(szFunVT, "vt_quit_canhan") || !strcmp(szFunVT, "vt_quit_bang"))\n'
           '\t\t\t\t\tPlayer[nIndex].ExecuteScript("\\\\script\\\\event\\\\lmbiaoche\\\\vt_chinam.lua", szFunVT, 0);\n'
           "\t\t\t}\n"
           "\t\t\tbreak;\n")
    return rep(s, old, new, 1, "KProtocolProcess case 6")

# ============================================================ UiTaskGuide.h
def f_ui_h(s):
    old = "\tvoid\t\t\t\t\tBuildSatThuText();\t// [3HD 25/08] san boss sat thu (TaskId 10)\n"
    new = old + ("\tvoid\t\t\t\t\tBuildVanTieuCaNhanText();\t// [VTCN 06/09] van tieu ca nhan (TaskId 11)\n"
                 "\tvoid\t\t\t\t\tBuildVanTieuBangText();\t\t// [VTCN 06/09] ap Tieu Bang (TaskId 12)\n")
    return rep(s, old, new, 1, "UiTaskGuide.h decl")

# ============================================================ UiTaskGuide.cpp
VT_FUNCS = r'''
// ============================================================================
// [VTCN 06/09] Van tieu Long Mon Tieu Cuc (2 muc: ca nhan TaskId 11, ap Tieu Bang TaskId 12).
// So lieu diem/tuyen: KVanTieuPos.h (sinh tu extend.lua may chu); chuoi: UiTaskGuideVanTieu.h.
// Task: 4179 = tuyen*10 + dao chieu (0 = chua nhan), 4180 = giay epoch may chu luc xe xuat phat,
// 4178 / 4183 = dem ngay (yymmdd*256 + n), 3542..3545 = diem bam xe bang (tuan%W*1000 + n).
// ============================================================================
static void VT_TenDiem(int nDiem, char* pOut, int nSize)
{
	pOut[0] = 0;
	if (nDiem >= 1 && nDiem <= VT_DIEM_SO)
		strncpy(pOut, s_szVTDiemTen[nDiem], nSize - 1);
	pOut[nSize - 1] = 0;
}

// 4180 la giay epoch cua MAY CHU; client cung mui gio nen tru thang time(NULL) (lech vai giay khong sao)
static void VT_TrangThaiXe(int nXuatPhat, char* pOut, int nSize)
{
	if (nXuatPhat == 0)
		strncpy(pOut, VT_CN_XE_CHUA, nSize - 1);
	else
	{
		int nCon = 1800 - (int)((long)time(NULL) - (long)nXuatPhat);
		if (nCon <= 0)
			strncpy(pOut, VT_CN_XE_HET, nSize - 1);
		else
			_snprintf(pOut, nSize - 1, VT_CN_XE_CON_FMT, (nCon + 59) / 60);
	}
	pOut[nSize - 1] = 0;
}

// tra nFlag (4179); *pSao = 0 khi chua nhan / tuyen la
static int VT_Tuyen(int* pSao, int* pDau, int* pCuoi)
{
	int nFlag = DTG_TaskVal(4179);
	int nTuyen = nFlag / 10;
	int nDao = nFlag % 10;
	*pSao = 0;
	*pDau = 0;
	*pCuoi = 0;
	if (nTuyen >= 1 && nTuyen <= VT_TUYEN_SO)
	{
		*pSao = s_nVTTuyenSao[nTuyen];
		*pDau = nDao ? s_nVTTuyenCuoi[nTuyen] : s_nVTTuyenDau[nTuyen];
		*pCuoi = nDao ? s_nVTTuyenDau[nTuyen] : s_nVTTuyenCuoi[nTuyen];
	}
	return nFlag;
}

// 3542..3545: tuan (%W) * 1000 + so diem; khac tuan hien tai coi nhu 0 (extend.lua GetFollowAwardCount)
static int VT_DiemBamXe(int nTaskId)
{
	int v = DTG_TaskVal(nTaskId);
	if (v <= 0)
		return 0;
	time_t t = time(NULL);
	struct tm* p = localtime(&t);
	char szTuan[8] = "0";
	if (p)
		strftime(szTuan, sizeof(szTuan), "%W", p);
	if (v / 1000 != atoi(szTuan))
		return 0;
	return v % 1000;
}

void KUiTaskGuide::BuildVanTieuCaNhanText()
{
	m_Content.Clear();
	AddLine(VT_CN_TIEU);
	AddLine(VT_CN_DK);
	AddLine(VT_CN_B1);
	AddLine(VT_CN_B2);
	AddLine(VT_CN_B3);
	AddLine(VT_CN_B3B);
	AddLine(VT_CN_B4);
	AddLine(VT_CN_THUONG);
	AddLine(VT_CN_SHOP);
	AddLine(VT_CN_LUUY);
	int nSao, nDau, nCuoi;
	VT_Tuyen(&nSao, &nDau, &nCuoi);
	char szLine[1024];
	if (nSao >= 1 && nSao <= 9)
	{
		char szD[96], szC[96], szXe[96];
		VT_TenDiem(nDau, szD, sizeof(szD));
		VT_TenDiem(nCuoi, szC, sizeof(szC));
		VT_TrangThaiXe(DTG_TaskVal(4180), szXe, sizeof(szXe));
		_snprintf(szLine, sizeof(szLine) - 1, VT_CN_TT_FMT, nSao, szD, szC, szXe);
		szLine[sizeof(szLine) - 1] = 0;
		AddLine(szLine);
	}
	else
		AddLine(VT_CN_TT_NONE);
	_snprintf(szLine, sizeof(szLine) - 1, VT_CN_NGAY_FMT, DTG_DailyCount(DTG_TaskVal(4178)), DTG_DailyCount(DTG_TaskVal(4183)));
	szLine[sizeof(szLine) - 1] = 0;
	AddLine(szLine);
	AddLine(VT_CN_HINT);
}

void KUiTaskGuide::BuildVanTieuBangText()
{
	m_Content.Clear();
	AddLine(VT_BH_TIEU);
	AddLine(VT_BH_DK);
	AddLine(VT_BH_B1);
	AddLine(VT_BH_B2);
	AddLine(VT_BH_B3);
	AddLine(VT_BH_B3B);
	AddLine(VT_BH_B4);
	AddLine(VT_BH_LUUY);
	int nSao, nDau, nCuoi;
	VT_Tuyen(&nSao, &nDau, &nCuoi);
	char szLine[1024];
	if (nSao >= 10)
	{
		char szD[96], szC[96], szXe[96];
		VT_TenDiem(nDau, szD, sizeof(szD));
		VT_TenDiem(nCuoi, szC, sizeof(szC));
		VT_TrangThaiXe(DTG_TaskVal(4180), szXe, sizeof(szXe));
		_snprintf(szLine, sizeof(szLine) - 1, VT_BH_TT_FMT, szD, szC, szXe);
		szLine[sizeof(szLine) - 1] = 0;
		AddLine(szLine);
	}
	else
		AddLine(VT_BH_TT_NONE);
	_snprintf(szLine, sizeof(szLine) - 1, VT_BH_DIEM_FMT, VT_DiemBamXe(3542), VT_DiemBamXe(3543), VT_DiemBamXe(3544), VT_DiemBamXe(3545));
	szLine[sizeof(szLine) - 1] = 0;
	AddLine(szLine);
	AddLine(VT_BH_HINT);
}

// ban rut gon cho khung Theo doi nhiem vu
static void VT_BuildBrief(int nTaskId, char* pOut, int nSize)
{
	int nSao, nDau, nCuoi;
	VT_Tuyen(&nSao, &nDau, &nCuoi);
	char szD[96], szC[96], szXe[96];
	VT_TenDiem(nDau, szD, sizeof(szD));
	VT_TenDiem(nCuoi, szC, sizeof(szC));
	VT_TrangThaiXe(DTG_TaskVal(4180), szXe, sizeof(szXe));
	pOut[0] = 0;
	if (nTaskId == TASKGUIDE_VT_CANHAN_TASKID)
	{
		if (nSao >= 1 && nSao <= 9)
			_snprintf(pOut, nSize - 1, VT_CN_BRIEF_FMT, nSao, szD, szC, szXe);
		else
			strncpy(pOut, VT_CN_BRIEF_NONE, nSize - 1);
	}
	else
	{
		if (nSao >= 10)
			_snprintf(pOut, nSize - 1, VT_BH_BRIEF_FMT, szD, szC, szXe);
		else
			strncpy(pOut, VT_BH_BRIEF_NONE, nSize - 1);
	}
	pOut[nSize - 1] = 0;
}

'''

def f_ui_cpp(s):
    # includes
    old = '#include "UiTaskGuideSatThuBoss.h"\t// [C18] SINH TU DONG tu killer.txt\n'
    new = old + ('#include "UiTaskGuideVanTieu.h"\t// [VTCN 06/09] SINH TU DONG (ReverseTools/vantieu/gen_vantieu_chinam.py)\n'
                 '#include "../../../core/src/KVanTieuPos.h"\t// [VTCN 06/09] diem/tuyen van tieu (sinh tu extend.lua)\n')
    s = rep(s, old, new, 1, "ui include")
    old = "#include <string.h>\n"
    new = old + "#include <time.h>\t// [VTCN 06/09] time/strftime cho trang thai van tieu\n"
    s = rep(s, old, new, 1, "ui time.h")
    # defines
    old = "#define TASKGUIDE_SATTHU_TASKID\t10\t// [3HD 25/08] san boss sat thu: task 1082/1192/1193/1217 (nt_setTask co SyncTaskValue)\n"
    new = old + ("#define TASKGUIDE_VT_CANHAN_TASKID\t11\t// [VTCN 06/09] van tieu ca nhan: task 4179/4180/4178/4183\n"
                 "#define TASKGUIDE_VT_BANG_TASKID\t12\t// [VTCN 06/09] ap Tieu Bang: task 4179/4180/3542..3545\n")
    s = rep(s, old, new, 1, "ui defines")
    # OnTaskValueChanged
    old = ("\t\tif (nTaskId == 1082 || nTaskId == 1192 || nTaskId == 1193 || nTaskId == 1217)\n"
           "\t\t\tm_pSelf->BuildSatThuText();\n"
           "\t}\n")
    new = old + ("\telse if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_VT_CANHAN_TASKID)\n"
                 "\t{\n"
                 "\t\tif ((nTaskId >= 4178 && nTaskId <= 4187) || nTaskId == 4169)\t// [VTCN 06/09]\n"
                 "\t\t\tm_pSelf->BuildVanTieuCaNhanText();\n"
                 "\t}\n"
                 "\telse if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_VT_BANG_TASKID)\n"
                 "\t{\n"
                 "\t\tif ((nTaskId >= 4178 && nTaskId <= 4187) || (nTaskId >= 3540 && nTaskId <= 3545))\t// [VTCN 06/09]\n"
                 "\t\t\tm_pSelf->BuildVanTieuBangText();\n"
                 "\t}\n")
    s = rep(s, old, new, 1, "ui OnTaskValueChanged")
    # AutoTraceOnTask
    old = ("\t\tnHe = TASKGUIDE_DATAU_TASKID;\n"
           "\t\tbCo = (DTG_TaskVal(1021) != 0);\n"
           "\t}\n")
    new = old + ("\telse if (nTaskId == 4179)\t// [VTCN 06/09] van tieu: tuyen*10+dao chieu; sao >= 10 la xe bang\n"
                 "\t{\n"
                 "\t\tint nF = DTG_TaskVal(4179);\n"
                 "\t\tint nT = nF / 10;\n"
                 "\t\tint nS = (nT >= 1 && nT <= VT_TUYEN_SO) ? s_nVTTuyenSao[nT] : 0;\n"
                 "\t\tif (nS <= 0)\n"
                 "\t\t{\n"
                 "\t\t\ts_abAutoDone[TASKGUIDE_VT_CANHAN_TASKID] = false;\n"
                 "\t\t\ts_abAutoDone[TASKGUIDE_VT_BANG_TASKID] = false;\n"
                 "\t\t\treturn;\n"
                 "\t\t}\n"
                 "\t\tnHe = (nS >= 10) ? TASKGUIDE_VT_BANG_TASKID : TASKGUIDE_VT_CANHAN_TASKID;\n"
                 "\t\tbCo = true;\n"
                 "\t}\n")
    s = rep(s, old, new, 1, "ui AutoTrace")
    # ShowTask
    old = ("\telse if (pEntry->nTaskId == TASKGUIDE_SATTHU_TASKID)\n"
           "\t{\n"
           "\t\tBuildSatThuText();\n"
           "\t}\n")
    new = old + ("\telse if (pEntry->nTaskId == TASKGUIDE_VT_CANHAN_TASKID)\t// [VTCN 06/09]\n"
                 "\t{\n"
                 "\t\tBuildVanTieuCaNhanText();\n"
                 "\t}\n"
                 "\telse if (pEntry->nTaskId == TASKGUIDE_VT_BANG_TASKID)\n"
                 "\t{\n"
                 "\t\tBuildVanTieuBangText();\n"
                 "\t}\n")
    s = rep(s, old, new, 1, "ui ShowTask")
    # ham moi: chen truoc BuildTinSuText
    old = "void KUiTaskGuide::BuildTinSuText()\n{\n"
    s = rep(s, old, VT_FUNCS + old, 1, "ui funcs")
    # GetTaskTitle
    old = ("\tif (nTaskId == TASKGUIDE_SATTHU_TASKID)\n"
           "\t\treturn ST3_TRACE_TITLE;\n")
    new = old + ("\tif (nTaskId == TASKGUIDE_VT_CANHAN_TASKID)\t// [VTCN 06/09]\n"
                 "\t\treturn VT_CN_TRACE_TITLE;\n"
                 "\tif (nTaskId == TASKGUIDE_VT_BANG_TASKID)\n"
                 "\t\treturn VT_BH_TRACE_TITLE;\n")
    s = rep(s, old, new, 1, "ui GetTaskTitle")
    # BuildTraceLineOf
    old = "void KUiTaskGuide::BuildTraceLineOf(int nTaskId, char* pOut, int nSize)\n{\n"
    new = old + ("\tif (nTaskId == TASKGUIDE_VT_CANHAN_TASKID || nTaskId == TASKGUIDE_VT_BANG_TASKID)\t// [VTCN 06/09]\n"
                 "\t{\n"
                 "\t\tVT_BuildBrief(nTaskId, pOut, nSize);\n"
                 "\t\treturn;\n"
                 "\t}\n")
    s = rep(s, old, new, 1, "ui BuildTraceLineOf")
    # UpdateButtons
    old = ("\tint nCourse = bDT ? DTG_TaskVal(1028) : 0;\n"
           "\tint nBossST = bST ? DTG_TaskVal(1082) : 0;\n"
           "\tm_BtnQuit.Enable((bDT && nCourse == 1) || (bST && nBossST >= 1 && nBossST <= ST3_BOSS_MAX));\n")
    new = ("\t// [VTCN 06/09] 2 tab van tieu: nut Bo nhiem vu = huy o may chu (du luat: ca nhan chi khi xe\n"
           "\t// chua xuat phat va duoi 7 sao; bang = CancelTongTask), Theo doi dung ban rut gon VT_BuildBrief\n"
           "\tint nCurVT = (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount) ? m_Entries[m_nCurEntry].nTaskId : -1;\n"
           "\tbool bVT = (nCurVT == TASKGUIDE_VT_CANHAN_TASKID || nCurVT == TASKGUIDE_VT_BANG_TASKID);\n"
           "\tint nSaoVT = 0, nDauVT = 0, nCuoiVT = 0;\n"
           "\tif (bVT)\n"
           "\t\tVT_Tuyen(&nSaoVT, &nDauVT, &nCuoiVT);\n"
           "\tbool bQuitVT = (nCurVT == TASKGUIDE_VT_CANHAN_TASKID && nSaoVT >= 1 && nSaoVT <= 6 && DTG_TaskVal(4180) == 0) ||\n"
           "\t\t(nCurVT == TASKGUIDE_VT_BANG_TASKID && nSaoVT >= 10);\n"
           "\tint nCourse = bDT ? DTG_TaskVal(1028) : 0;\n"
           "\tint nBossST = bST ? DTG_TaskVal(1082) : 0;\n"
           "\tm_BtnQuit.Enable((bDT && nCourse == 1) || (bST && nBossST >= 1 && nBossST <= ST3_BOSS_MAX) || bQuitVT);\n")
    s = rep(s, old, new, 1, "ui UpdateButtons")
    old = "\tbool bCoBan = (bDT || bST);\t// he da co ban rut gon de hien trong khung\n"
    new = "\tbool bCoBan = (bDT || bST || bVT);\t// he da co ban rut gon de hien trong khung ([VTCN] + 2 tab van tieu)\n"
    s = rep(s, old, new, 1, "ui bCoBan")
    # nut Quit
    old = ("\t\t\t\t\tpCmd = \"st3_quit\";\t// [C18] -> nieshichen.lua cancel()\n")
    new = old + ("\t\t\t\tif (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount &&\n"
                 "\t\t\t\t\tm_Entries[m_nCurEntry].nTaskId == TASKGUIDE_VT_CANHAN_TASKID)\n"
                 "\t\t\t\t\tpCmd = \"vt_quit_canhan\";\t// [VTCN 06/09] -> vt_chinam.lua AbandonTask (hop xac nhan)\n"
                 "\t\t\t\tif (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount &&\n"
                 "\t\t\t\t\tm_Entries[m_nCurEntry].nTaskId == TASKGUIDE_VT_BANG_TASKID)\n"
                 "\t\t\t\t\tpCmd = \"vt_quit_bang\";\t// [VTCN 06/09] -> CancelTongTask\n")
    s = rep(s, old, new, 1, "ui quit cmd")
    # TryGoXaFu
    old = ("\t\t\tg_pCoreShell->OperationRequest(GOI_TASKGUIDE_GOTO_SATTHU, 0, 0);\n"
           "\t\treturn;\n"
           "\t}\n")
    new = old + ("\t// [VTCN 06/09] 2 tab van tieu: dan duong toi NPC nhan nhiem vu (uParam = TaskId)\n"
                 "\tif (m_Entries[m_nCurEntry].nTaskId == TASKGUIDE_VT_CANHAN_TASKID ||\n"
                 "\t\tm_Entries[m_nCurEntry].nTaskId == TASKGUIDE_VT_BANG_TASKID)\n"
                 "\t{\n"
                 "\t\tif (g_pCoreShell)\n"
                 "\t\t\tg_pCoreShell->OperationRequest(GOI_TASKGUIDE_GOTO_VANTIEU, (unsigned int)m_Entries[m_nCurEntry].nTaskId, 0);\n"
                 "\t\treturn;\n"
                 "\t}\n")
    s = rep(s, old, new, 1, "ui TryGo")
    return s

patch(r"Core\Src\CoreShell.h", f_coreshell_h)
patch(r"Core\Src\CoreShell.cpp", f_coreshell_cpp)
patch(r"Core\Src\KProtocolProcess.cpp", f_kpp)
patch(r"S3Client\Ui\UiCase\UiTaskGuide.h", f_ui_h)
patch(r"S3Client\Ui\UiCase\UiTaskGuide.cpp", f_ui_cpp)
print("XONG" + (" (dry)" if DRY else ""))
