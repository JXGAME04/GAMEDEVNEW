# -*- coding: utf-8 -*-
"""patch_cpp2.py [VTCN 06/09 v2] - chu game: Chi nam KHONG duoc nhay map (phu ve thanh -> Xa Phu -> di bo)
va CHAN moi tu-di-chuyen cua Chi nam o MAP SU KIEN. Sua trong worktree D:\\GAMEDEVNEW_wt_vtcn:
  CoreShell.cpp : TG_ChanMapSuKien + guard o TG_XaFuStart / TG_SatThuStart; thay ca khoi TG_VanTieu* (v2)
  KProtocolProcess.cpp : bo vt_goto_canhan khoi danh sach trang
Chay: python patch_cpp2.py [--dry]
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
        sys.exit("LOI %s: byte cao doi %d -> %d" % (rel, hb(s0), hb(s1)))
    print("%-45s OK (byte cao %d giu nguyen)" % (rel, hb(s0)))
    wr(p, s1)

TG_VT2 = r'''
//---------------------------------------------------------------------------
// [VTCN 06/09 v2] Bam muc Van tieu tren bang F11 -> tu dan duong toi NPC nhan nhiem vu.
// Chu game 06/09: "KHONG DUOC NHAY MAP": dang o thanh/thon -> chay toi Xa Phu, chon
// 'thanh thi da di qua' -> ten ben cua map dich; dang o map luyen cong -> dung phu ve
// thanh truoc roi moi ra Xa Phu; toi map dich thi di bo toi NPC. Map su kien: khong
// dan duong (TG_ChanMapSuKien, ap cho ca Da Tau / Sat Thu / van tieu).
//   nhanh CA NHAN (TaskId 11): chua nhan -> Ong chu Tieu cuc (Tran Long Mon, map 121);
//     da nhan, xe chua xuat phat -> Tieu Su diem dau; xe da xuat phat -> Tieu Su diem
//     cuoi neu dang cung map (xe phai di bo).
//   nhanh BANG (TaskId 12): chua giu xe -> Tiep Dan o THANH BANG DANG CHIEM
//     (g_ClientCityInfo[].szOwner == ten bang cua minh, CITYINFO 21/08); dang giu xe ->
//     Nhan Hang diem cuoi neu cung map.
// So lieu KVanTieuPos.h (extend.lua) + ten ben Xa Phu KVanTieuMsg.h (settings\Station.txt).
// Tai dung do nghe Da Tau: DT_WalkTo / DT_FindNpcName / DT_TramGan / DT_UsePortal /
// DT_Split / DT_FindAns / DT_Answer (khuon TG_SatThu pha 2-4). Bam lai = huy.
//---------------------------------------------------------------------------
#include "KVanTieuPos.h"

static int  g_nTGVTOn = 0;
static int  g_nTGVTPhase = 0;	// 10 phu ve thanh; 20 di toi Xa Phu; 21 chon 'thanh thi da di qua'; 22 chon ten ben; 23 doi doi map; 3 di bo toi NPC
static int  g_nTGVTMap = 0;		// map dich
static int  g_nTGVTX = 0, g_nTGVTY = 0;	// o cell NPC dich
static int  g_nTGVTTpl = 0;		// template NPC dich
static const char* g_szTGVTTen = "";
static int  g_nTGVTTry = 0;
static int  g_nTGVTDlgTry = 0;
static UINT g_uTGVTNext = 0;
static UINT g_uTGVTDlgSeen = 0;
static int  g_nTGVTNpcXaPhu = 0;

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

// ten ben Xa Phu cua map (settings\Station.txt cot DESC) - NULL neu map khong co ben
static const char* TG_VanTieuTenBen(int nMap)
{
	for (int i = 0; i < VT_XAPHU_SO; i++)
		if (s_nVTXaPhuMap[i] == nMap)
			return s_szVTXaPhuTen[i];
	return NULL;
}

static void TG_VanTieuDatDich(int nMap, int nX, int nY, int nTpl, const char* szTen)
{
	g_nTGVTMap = nMap;
	g_nTGVTX = nX;
	g_nTGVTY = nY;
	g_nTGVTTpl = nTpl;
	g_szTGVTTen = szTen;
}

// chon mot muc trong thoai Xa Phu dang mo (khuon TG_SatThuPickAns):
// 1 da bam, 0 chua co thoai moi (cu ~3,2 s go lai NPC), -1 thoai co ma KHONG co muc
static int TG_VanTieuPickAns(int nPlayerIdx, const char* szMark)
{
	if (g_sDTCap.uDlgSeq == g_uTGVTDlgSeen)
	{
		if (g_nTGVTNpcXaPhu > 0 && (++g_nTGVTDlgTry % 8) == 0)
			Player[nPlayerIdx].DialogNpc(g_nTGVTNpcXaPhu);
		return 0;
	}
	g_uTGVTDlgSeen = g_sDTCap.uDlgSeq;
	g_nTGVTDlgTry = 0;
	char szBuf[2048];
	char* apAns[24];
	g_StrCpyLen(szBuf, g_sDTCap.szDlg, sizeof(szBuf));
	int nAns = DT_Split(szBuf, apAns, 24);
	int nOpt = DT_FindAns(apAns, nAns, szMark);
	if (nOpt < 0)
		return -1;
	DT_Answer(nPlayerIdx, nOpt);
	return 1;
}

// map dang dung co ben Xa Phu (bang g_MoveStation cua engine Da Tau) khong
static int TG_VanTieuCoXaPhu(int nMap)
{
	MapStation::iterator it = g_MoveStation.find(nMap);
	return (it != g_MoveStation.end() && !it->second.empty()) ? 1 : 0;
}

static void TG_VanTieuBaoXaPhu(int nPlayerIdx)
{
	const char* szBen = TG_VanTieuTenBen(g_nTGVTMap);
	char szBuf[256];
	_snprintf(szBuf, sizeof(szBuf) - 1, VT_MSG_XAPHU_DI_FMT, szBen ? szBen : "");
	szBuf[sizeof(szBuf) - 1] = 0;
	DT_Msg(nPlayerIdx, szBuf);
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
	if (TG_ChanMapSuKien(nPlayerIdx))
		return 0;
	int nMapNow = SubWorld[0].m_SubWorldID;
	int nFlag = (int)Player[nPlayerIdx].m_cTask.GetSaveVal(4179);	// TSK_LMBJTaskFlag = tuyen*10 + dao chieu
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
				TG_VanTieuDatDich(s_nVTDiemMap[nDiemDau], s_nVTDiemX[nDiemDau], s_nVTDiemY[nDiemDau], VT_TPL_TIEUSU, VT_NPC_TIEUSU);
		}
		else
			TG_VanTieuDatDich(VT_MAP_LONGMON, VT_CQ_X, VT_CQ_Y, VT_TPL_CHUONGQUY, VT_NPC_CHUONGQUY);
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
			// thanh bang minh dang chiem (ban sao 7 thanh CITYINFO tren client)
			char szTong[64];
			szTong[0] = 0;
			Player[nPlayerIdx].m_cTong.GetTongName(szTong);
			if (!szTong[0])
			{
				DT_Msg(nPlayerIdx, VT_MSG_BANG_KHONGBANG);
				return 0;
			}
			int nMapBang = 0;
			for (int c = 1; c <= 7; c++)
			{
				if (g_ClientCityInfo[c].nCityId && g_ClientCityInfo[c].szOwner[0] &&
					strcmp(g_ClientCityInfo[c].szOwner, szTong) == 0)
				{
					nMapBang = g_ClientCityInfo[c].nMapId;
					break;
				}
			}
			if (!nMapBang)
			{
				DT_Msg(nPlayerIdx, VT_MSG_BANG_KHONGTHANH);
				return 0;
			}
			int nDiem = TG_VanTieuDiemTiepDan(nMapBang);
			if (!nDiem)
			{
				DT_Msg(nPlayerIdx, VT_MSG_KHONG_NPC);
				return 0;
			}
			TG_VanTieuDatDich(nMapBang, s_nVTDiemX[nDiem], s_nVTDiemY[nDiem], VT_TPL_TIEPDAN, VT_NPC_TIEPDAN);
		}
	}
	if (g_nTGVTMap != nMapNow && !TG_VanTieuTenBen(g_nTGVTMap))
	{
		DT_Msg(nPlayerIdx, VT_MSG_XAPHU_KHONGBEN);
		return 0;
	}
	g_nTGVTTry = 0;
	g_nTGVTDlgTry = 0;
	g_uTGVTNext = 0;
	g_nTGVTNpcXaPhu = 0;
	g_uTGVTDlgSeen = g_sDTCap.uDlgSeq;
	if (nMapNow == g_nTGVTMap)
		g_nTGVTPhase = 3;
	else if (TG_VanTieuCoXaPhu(nMapNow))
		g_nTGVTPhase = 20;
	else
		g_nTGVTPhase = 10;
	g_nTGVTOn = 1;
	if (g_nTGVTPhase == 3)
	{
		char szBuf[256];
		_snprintf(szBuf, sizeof(szBuf) - 1, VT_MSG_TOI_NPC_FMT, g_szTGVTTen);
		szBuf[sizeof(szBuf) - 1] = 0;
		DT_Msg(nPlayerIdx, szBuf);
	}
	else if (g_nTGVTPhase == 20)
		TG_VanTieuBaoXaPhu(nPlayerIdx);
	else
		DT_Msg(nPlayerIdx, VT_MSG_PHU);
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
	if (++g_nTGVTTry > 900)	// ~6 phut ca chuoi
	{
		TG_VanTieuStop(VT_MSG_LAU);
		return;
	}
	int nMapNow = SubWorld[0].m_SubWorldID;
	// bat ky pha nao: da dung map dich thi chuyen sang di bo
	if (g_nTGVTPhase != 3 && nMapNow == g_nTGVTMap)
	{
		g_nTGVTPhase = 3;
		g_nTGVTTry = 0;
		return;
	}
	// --- pha 10: map luyen cong -> dung phu ve thanh, toi map co Xa Phu thi sang pha 20 ---
	if (g_nTGVTPhase == 10)
	{
		if (TG_VanTieuCoXaPhu(nMapNow))
		{
			g_nTGVTPhase = 20;
			g_nTGVTTry = 0;
			TG_VanTieuBaoXaPhu(nPlayerIdx);
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
	// --- pha 20: chay toi Xa Phu gan nhat roi mo thoai ---
	if (g_nTGVTPhase == 20)
	{
		MapStation::iterator it = g_MoveStation.find(nMapNow);
		if (it == g_MoveStation.end() || it->second.empty())
		{
			g_nTGVTPhase = 10;	// bi keo ra map khong co ben
			return;
		}
		sStation& s = DT_TramGan(it->second, nPlayerIdx);
		int nIdx = DT_FindNpcName(nPlayerIdx, "xa phu", s.x, s.y, 400);
		if (nIdx)
		{
			int nX, nY, dX, dY;
			Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
			Npc[nIdx].GetMpsPos(&dX, &dY);
			if (g_GetDistance(nX, nY, dX, dY) <= 160)
			{
				g_nTGVTNpcXaPhu = nIdx;
				g_nTGVTTry = 0;
				g_nTGVTDlgTry = 0;
				g_uTGVTDlgSeen = g_sDTCap.uDlgSeq;
				Player[nPlayerIdx].DialogNpc(nIdx);
				g_nTGVTPhase = 21;
				return;
			}
			DT_WalkTo(nPlayerIdx, dX, dY, 128, uCur);
			return;
		}
		DT_WalkTo(nPlayerIdx, s.x, s.y, 200, uCur);
		return;
	}
	// --- pha 21/22: bam 2 cap menu cua Xa Phu: 'thanh thi da di qua' -> ten ben map dich ---
	if (g_nTGVTPhase == 21 || g_nTGVTPhase == 22)
	{
		const char* szMark = (g_nTGVTPhase == 21) ? VT_MENU_THANHTHI : TG_VanTieuTenBen(g_nTGVTMap);
		int nRet = TG_VanTieuPickAns(nPlayerIdx, szMark ? szMark : "");
		if (nRet == 1)
		{
			g_nTGVTTry = 0;
			g_nTGVTPhase++;	// 21 -> 22 -> 23 (da bam ten ben, cho doi map)
		}
		else if (nRet < 0)
		{
			char szBuf[256];
			_snprintf(szBuf, sizeof(szBuf) - 1, VT_MSG_XAPHU_KHONGCO_FMT, szMark ? szMark : "");
			szBuf[sizeof(szBuf) - 1] = 0;
			TG_VanTieuStop(szBuf);
		}
		return;
	}
	// --- pha 23: doi Xa Phu cho di (map doi -> bat o dau ham) ---
	if (g_nTGVTPhase == 23)
	{
		if (g_nTGVTTry > 25)	// ~10 giay khong doi map: thieu tien xe / tu choi
			TG_VanTieuStop(VT_MSG_XAPHU_CHUADI);
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
    e = eol_of(s)
    # (1) include + forward decl ngay sau forward decl DT_Answer
    old = "static void DT_Answer(int nPlayerIdx, int nIdx);\t// dinh nghia o duoi (dung chung voi engine)\n"
    new = old + ('#include "KVanTieuMsg.h"\t// [VTCN 06/09] thong bao [Chi nam] + ten ben Xa Phu (sinh tu settings\\Station.txt)\n'
                 "static int TG_ChanMapSuKien(int nPlayerIdx);\t// [VTCN 06/09] dinh nghia sau KMapSuKien.h: 1 = dang o map su kien (da bao)\n")
    s = rep(s, old, new, 1, "fwd decl")
    # (2) dinh nghia TG_ChanMapSuKien sau include KMapSuKien.h
    old = '#include "KMapSuKien.h"\t// bang MAP SU KIEN (sinh tu map_type.txt cua may chu)\n'
    new = old + ("\n// [VTCN 06/09] Chu game: MOI tu-di-chuyen cua bang Chi nam (Xa Phu Da Tau, boss Sat Thu,\n"
                 "// van tieu) bi CHAN khi dang o MAP SU KIEN (KMapSuKien.h, nChan = 1). Tra 1 va bao nguoi choi.\n"
                 "static int WA_MapSuKien(int nPlayerIdx);\t// dinh nghia duoi (bang KMapSuKien.h)\n"
                 "static int TG_ChanMapSuKien(int nPlayerIdx)\n"
                 "{\n"
                 "\tint nSK = WA_MapSuKien(nPlayerIdx);\n"
                 "\tif (nSK <= 0)\n"
                 "\t\treturn 0;\n"
                 "\tchar szBuf[256];\n"
                 "\t_snprintf(szBuf, sizeof(szBuf) - 1, VT_MSG_SUKIEN_FMT, s_aTenSuKien[nSK - 1]);\n"
                 "\tszBuf[sizeof(szBuf) - 1] = 0;\n"
                 "\tDT_Msg(nPlayerIdx, szBuf);\n"
                 "\treturn 1;\n"
                 "}\n")
    s = rep(s, old, new, 1, "TG_ChanMapSuKien def")
    # (3) guard TG_XaFuStart
    old = "\tint nMap = SubWorld[0].m_SubWorldID;\n\tMapStation::iterator it = g_MoveStation.find(nMap);\n"
    new = "\tif (TG_ChanMapSuKien(nPlayerIdx))\t// [VTCN 06/09] map su kien: Chi nam khong tu di chuyen\n\t\treturn 0;\n" + old
    s = rep(s, old, new, 1, "guard XaFu")
    # (4) guard TG_SatThuStart
    old = "\tint nBoss = (int)Player[nPlayerIdx].m_cTask.GetSaveVal(1082);\n\tif (nBoss < 1 || nBoss > ST3_POS_MAX)\n"
    new = "\tif (TG_ChanMapSuKien(nPlayerIdx))\t// [VTCN 06/09] map su kien: Chi nam khong tu di chuyen\n\t\treturn 0;\n" + old
    s = rep(s, old, new, 1, "guard SatThu")
    # (5) thay ca khoi TG_VanTieu (v1 -> v2)
    a = s.find(e + "//---------------------------------------------------------------------------" + e + "// [VTCN 06/09] Bam muc Van tieu tren bang F11")
    b = s.find(e + "// [DaTau] tim quai con SONG da sync NGOAI tam danh de chay toi (T4 di tim quai).")
    if a < 0 or b < 0 or b <= a:
        sys.exit("khong thay khoi TG_VanTieu v1 (%d, %d)" % (a, b))
    s = s[:a] + TG_VT2.replace("\n", e) + s[b:]
    return s

def f_kpp(s):
    old = ("\t\t\t\t// [VTCN 06/09] bang F11 muc Van tieu: dan duong (vt_goto_canhan) / nut Bo nhiem vu\n"
           "\t\t\t\t// (vt_quit_canhan, vt_quit_bang) -> script\\event\\lmbiaoche\\vt_chinam.lua. Chep ra bo dem\n")
    new = ("\t\t\t\t// [VTCN 06/09 v2] bang F11 muc Van tieu: nut Bo nhiem vu (vt_quit_canhan, vt_quit_bang)\n"
           "\t\t\t\t// -> script\\event\\lmbiaoche\\vt_chinam.lua. (Khong con vt_goto_canhan: chu game cam nhay map,\n"
           "\t\t\t\t// client tu di bang phu ve thanh + Xa Phu.) Chep ra bo dem\n")
    s = rep(s, old, new, 1, "kpp comment")
    old = '\t\t\t\tif (!strcmp(szFunVT, "vt_goto_canhan") || !strcmp(szFunVT, "vt_quit_canhan") || !strcmp(szFunVT, "vt_quit_bang"))\n'
    new = '\t\t\t\tif (!strcmp(szFunVT, "vt_quit_canhan") || !strcmp(szFunVT, "vt_quit_bang"))\n'
    s = rep(s, old, new, 1, "kpp whitelist")
    return s

patch(r"Core\Src\CoreShell.cpp", f_coreshell_cpp)
patch(r"Core\Src\KProtocolProcess.cpp", f_kpp)
print("XONG" + (" (dry)" if DRY else ""))
