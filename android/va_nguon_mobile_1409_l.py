# -*- coding: utf-8 -*-
# [NAPTO 14/09] + [LOGIC-PHA 14/09] - sau log Fold 7 10:49 (ban 109141044): tai len GPU da xong (0 khung giat do tai), con 24 khung giat/10 phut:
#   * 10 khung = nap DONG BO tren luong ve: rut MOT khung to 24 / 26 / 72 ms, mo mot tep spr lanh 37 ms ngoai luc ve. Ngan sach NapKhungMs (3 ms)
#     chi kiem TRUOC khi rut nen khong chan duoc mot lan rut 72 ms; log chua ghi ten tep.
#     -> NAPTO: khung chua rut co co NEN (m_pOffset[n].Length) >= [Client] NapKhungToKB (128) thi giao luong nen ngay (bo ve khung nay 1-3 khung,
#        nhu 'bo ve' khi het ngan sach) thay vi rut dong bo; ghi [NAP-CHAM] ten tep + khung + co + ms khi mot lan rut / mo tep >= 10 ms (rep3.log).
#   * 13 khung = LOGIC game (SPIKE logic 89-199 ms) ma [LOGIC] (Breathe + UiHeartBeat) KHONG ghi -> thoi gian nam NGOAI hai ham do:
#     NetConnectAgent.Breathe (mang), JxWAuto_NhipVongLap (WAuto), ProcIpcCommand, khoi sau UiHeartBeat, OperationRequest PROCFRAME, SendAllCommand.
#     -> LOGIC-PHA: do tung pha bang QPC, ghi [LOGIC-PHA] vao jx_paint.log khi phan logic cua vong lap >= 30 ms (chi khi PaintLog > 0).
# Chi JX_MOBILE, ban PC khong doi. Chay lai vo hai (chay sau _k.py).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[NAPTO 14/09]"
DAUL = "[LOGIC-PHA 14/09]"
NL = "\r\n"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    assert NL in s, p
    return s


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def cao(s):
    return sum(1 for c in s if ord(c) >= 128)


def thay(s, cu, moi, ten, n=1):
    if s.count(cu) != n:
        raise SystemExit("%s: neo '%s...' thay %d lan, can %d" % (ten, cu.strip()[:70], s.count(cu), n))
    return s.replace(cu, moi)


def va(p, viec):
    s = doc(p)
    if DAU in s or DAUL in s:
        print("da va roi:", p)
        return
    c0 = cao(s)
    s = viec(s)
    assert cao(s) == c0, p + ": byte cao doi"
    ghi(p, s)
    print("da va:", p)


# ---------------------------------------------------------------- Represent3
def v_base(s):
    cu = "extern int g_nJxNapKhungKB;\t// [TAI 14/09] [Client] NapKhungKB: ngan sach byte/khung tai DAN texture khung nap truoc len GPU (0 = tat)" + NL
    moi = cu + "extern int g_nJxNapKhungToKB; extern unsigned g_uJxNapKhungTo;\t// " + DAU + " [Client] NapKhungToKB: khung co nen >= nguong (KB) thi giao luong nen ngay ca khi con ngan sach dong bo (0 = tat); so lan" + NL
    return thay(s, cu, moi, "extern NapKhungToKB")


def v_shell(s):
    cu = "int g_nJxNapKhungKB = 128;\t// [TAI 14/09] [Client] NapKhungKB: ngan sach byte/khung tai dan texture khung NAP TRUOC len GPU (driver Fold 7 ton CPU ~20 ms/MB trong lenh tai)" + NL
    moi = cu + "int g_nJxNapKhungToKB = 128; unsigned g_uJxNapKhungTo = 0;\t// " + DAU + " [Client] NapKhungToKB: khung nen >= nguong giao luong nen ngay (Fold 7: rut dong bo 1 khung to = 24-72 ms)" + NL
    s = thay(s, cu, moi, "g_nJxNapKhungToKB")
    cu = "\tg_nJxNapKhungKB    = Rep3Ini(\"NapKhungKB\", 128);\t// [TAI 14/09] ngan sach tai dan khung nap truoc len GPU (KB/khung); 0 = tat (tai ca khung luc ve nhu cu)" + NL
    moi = cu + "\tg_nJxNapKhungToKB  = Rep3Ini(\"NapKhungToKB\", 128);\t// " + DAU + " khung chua rut co co nen >= KB nay -> giao luong nen ngay, bo ve 1-3 khung; 0 = tat (rut dong bo khi con ngan sach nhu cu)" + NL
    s = thay(s, cu, moi, "ini NapKhungToKB")
    cu = "\t\tRep3Log(\"[VE-TAI] tai dan khung nap truoc (NapKhungKB=%d): vao hang %u, xong %u, %u KB / %u luot, %.1f ms (hang cho max %u) | trang atlas / o rong to 0 bang chep GPU: %u lan %.1f ms | o BGRA8 tai qua anh dem: %u o %u KB\"," + NL
    moi = "\t\tRep3Log(\"[VE-TAI] tai dan khung nap truoc (NapKhungKB=%d): vao hang %u, xong %u, %u KB / %u luot, %.1f ms (hang cho max %u) | trang atlas / o rong to 0 bang chep GPU: %u lan %.1f ms | o BGRA8 tai qua anh dem: %u o %u KB | khung to (>= %d KB) giao nen thay vi rut dong bo: %u\"," + NL
    s = thay(s, cu, moi, "VE-TAI format")
    cu = "\t\t\tg_nJxNapKhungKB, g_uJxTaiTruocSo, g_uJxTaiTruocXong, g_uJxTaiTruocKB, g_uJxTaiTruocLuot, g_dJxTaiTruocMs, g_uJxTaiTruocMax, g_uJxZeroChep, g_dJxZeroChepMs, g_uJxDemSo, g_uJxDemKB);" + NL
    moi = "\t\t\tg_nJxNapKhungKB, g_uJxTaiTruocSo, g_uJxTaiTruocXong, g_uJxTaiTruocKB, g_uJxTaiTruocLuot, g_dJxTaiTruocMs, g_uJxTaiTruocMax, g_uJxZeroChep, g_dJxZeroChepMs, g_uJxDemSo, g_uJxDemKB, g_nJxNapKhungToKB, g_uJxNapKhungTo);" + NL
    s = thay(s, cu, moi, "VE-TAI args")
    cu = "\t\tg_uJxTaiTruocSo = g_uJxTaiTruocXong = g_uJxTaiTruocKB = g_uJxTaiTruocLuot = g_uJxTaiTruocMax = g_uJxZeroChep = 0; g_dJxTaiTruocMs = g_dJxZeroChepMs = 0.0; g_uJxDemSo = g_uJxDemKB = 0;" + NL
    moi = "\t\tg_uJxTaiTruocSo = g_uJxTaiTruocXong = g_uJxTaiTruocKB = g_uJxTaiTruocLuot = g_uJxTaiTruocMax = g_uJxZeroChep = 0; g_dJxTaiTruocMs = g_dJxZeroChepMs = 0.0; g_uJxDemSo = g_uJxDemKB = 0; g_uJxNapKhungTo = 0;" + NL
    s = thay(s, cu, moi, "VE-TAI reset")
    return s


def v_res(s):
    cu = ("\tif (bPrepareTex && g_nJxNapKhungNen > 0 && g_pJxTexMgr && g_pJxTexMgr->m_bVeDangDien)" + NL
          + "\t{" + NL
          + "\t\tif (g_dRep3NapKhung >= (double)g_nJxNapKhungMs && (m_pFrameInfo[nFrame].nJxNen == 1 || JxNapKhungGiao(nFrame, 0)))\t// [VE 11/09 d] nJxNen 2 = rong/hong: nap dong bo (re) nhu cu" + NL)
    moi = ("\tif (bPrepareTex && g_nJxNapKhungNen > 0 && g_pJxTexMgr && g_pJxTexMgr->m_bVeDangDien)" + NL
           + "\t{" + NL
           + "\t\tif (g_nJxNapKhungToKB > 0 && !m_pFrameInfo[nFrame].pRawData && m_pHeader && m_pOffset && m_pFrameInfo[nFrame].nJxNen != 2)" + NL
           + "\t\t{\t// " + DAU + " khung TO chua rut (co nen >= NapKhungToKB): giao luong nen NGAY du con ngan sach - rut dong bo mot khung to = 24-72 ms tren Fold 7" + NL
           + "\t\t\t// (ngan sach NapKhungMs chi kiem TRUOC khi rut nen khong chan duoc). Bo ve khung nay 1-3 khung nhu 'bo ve' khi het ngan sach." + NL
           + "\t\t\tint nLTo = (int)m_pOffset[nFrame].Length; if (nLTo < 0) nLTo = -nLTo;" + NL
           + "\t\t\tif (nLTo >= g_nJxNapKhungToKB * 1024 && (m_pFrameInfo[nFrame].nJxNen == 1 || JxNapKhungGiao(nFrame, 0)))" + NL
           + "\t\t\t{" + NL
           + "\t\t\t\tJxNapKhungTruoc(nFrame);" + NL
           + "\t\t\t\tg_uJxNapKhungBoVe++; g_uJxNapKhungBoVeKhung++; g_uJxNapKhungTo++; g_nJxAnhBoVeNen = 1; return false;" + NL
           + "\t\t\t}" + NL
           + "\t\t}" + NL
           + "\t\tif (g_dRep3NapKhung >= (double)g_nJxNapKhungMs && (m_pFrameInfo[nFrame].nJxNen == 1 || JxNapKhungGiao(nFrame, 0)))\t// [VE 11/09 d] nJxNen 2 = rong/hong: nap dong bo (re) nhu cu" + NL)
    s = thay(s, cu, moi, "PrepareFrameData khung to")
    cu = "\t\tm_pFrameInfo[nFrame].pRawData = pFrame->Sprite;" + NL + "\t\tm_pFrameInfo[nFrame].pFrame = pFrame;" + NL + "\t}" + NL
    moi = ("\t\tm_pFrameInfo[nFrame].pRawData = pFrame->Sprite;" + NL + "\t\tm_pFrameInfo[nFrame].pFrame = pFrame;" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\t{\t// " + DAU + " mot lan rut khung dong bo >= 10 ms: ghi ten de biet tep/khung nao (Fold 7: 24-72 ms/lan chua ro cua ai)" + NL
           + "\t\t\tconst double dRut = Rep3NapMs(liK0, liK1);" + NL
           + "\t\t\tif (dRut >= 10.0) Rep3Log(\"[NAP-CHAM] rut khung %s k%d (%d KB nen, %dx%d): %.1f ms (%s)\", szImage, nFrame, nRawLen >> 10, (int)pFrame->Width, (int)pFrame->Height, dRut, (g_pJxTexMgr && g_pJxTexMgr->m_bVeDangDien) ? \"luc ve\" : \"ngoai luc ve\");" + NL
           + "\t\t}" + NL
           + "#endif" + NL
           + "\t}" + NL)
    s = thay(s, cu, moi, "NAP-CHAM rut khung")
    return s


def v_mgr(s):
    cu = ("\t\t{\tLARGE_INTEGER liA, liB; QueryPerformanceCounter(&liA);\t// [NAP 08/09 a] tep spr: pak + giai nen" + NL
          + "\t\t\tbool bNap = pRet->LoadImage((LPSTR)pszImageFile, nType);" + NL
          + "\t\t\tQueryPerformanceCounter(&liB); Rep3NapCong(g_napSpr, Rep3NapMs(liA, liB));" + NL)
    moi = (cu
           + "#ifdef JX_MOBILE" + NL
           + "\t\t\tif (Rep3NapMs(liA, liB) >= 10.0) Rep3Log(\"[NAP-CHAM] mo tep spr %s: %.1f ms (%s)\", pszImageFile, Rep3NapMs(liA, liB), m_bVeDangDien ? \"luc ve\" : \"ngoai luc ve\");\t// " + DAU + " tep lanh tren flash 37 ms (Fold 7)" + NL
           + "#endif" + NL)
    return thay(s, cu, moi, "NAP-CHAM mo tep")


# ---------------------------------------------------------------- S3Client: pha logic
def v_s3(s):
    cu = "static inline unsigned JxHudUs(const LARGE_INTEGER& a) { LARGE_INTEGER b, f; QueryPerformanceCounter(&b); QueryPerformanceFrequency(&f); return f.QuadPart ? (unsigned)((b.QuadPart - a.QuadPart) * 1000000 / f.QuadPart) : 0; }" + NL
    moi = (cu
           + "// " + DAUL + " [SPIKE] logic=89-199 ms tren Fold 7 ma [LOGIC] (Breathe + UiHeartBeat) khong ghi -> thoi gian nam ngoai hai ham do; do tung pha cua phan logic" + NL
           + "// trong GameLoop (mang, WAuto, IPC, Breathe, UiHeartBeat, khoi sau, PROCFRAME, gui lenh) va ghi [LOGIC-PHA] vao jx_paint.log khi phan logic >= 30 ms (PaintLog > 0)." + NL
           + "static inline double JxLgMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); return f.QuadPart ? (double)(b.QuadPart - a.QuadPart) * 1000.0 / (double)f.QuadPart : 0.0; }" + NL
           + "#define JX_LG_MOC(v) QueryPerformanceCounter(&(v))" + NL)
    s = thay(s, cu, moi, "JxLgMs")
    cu = "\tDWORD\tnLogCntBefore = m_GameCounter;" + NL + "\tg_NetConnectAgent.Breathe();" + NL
    moi = ("\tDWORD\tnLogCntBefore = m_GameCounter;" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\tLARGE_INTEGER liLg0, liLgNet, liLgWa, liLgIpc, liLgBre, liLgUi, liLgSau, liLgPf, liLgSend; JX_LG_MOC(liLg0);\t// " + DAUL + NL
           + "#endif" + NL
           + "\tg_NetConnectAgent.Breathe();" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\tJX_LG_MOC(liLgNet); liLgWa = liLgIpc = liLgBre = liLgUi = liLgSau = liLgPf = liLgSend = liLgNet;" + NL
           + "#endif" + NL)
    s = thay(s, cu, moi, "moc mang")
    cu = "\tJxDoNhip_Vong();\t// [DONHIP 12/09] ban do nhip: doi pha / ghi jx_nhip.log (khong lam gi khi [DoNhip] Bat=0)" + NL + "#endif" + NL + "\tProcIpcCommand();" + NL
    moi = ("\tJxDoNhip_Vong();\t// [DONHIP 12/09] ban do nhip: doi pha / ghi jx_nhip.log (khong lam gi khi [DoNhip] Bat=0)" + NL
           + "\tJX_LG_MOC(liLgWa);\t// " + DAUL + NL
           + "#endif" + NL + "\tProcIpcCommand();" + NL
           + "#ifdef JX_MOBILE" + NL + "\tJX_LG_MOC(liLgIpc); liLgBre = liLgUi = liLgSau = liLgPf = liLgSend = liLgIpc;" + NL + "#endif" + NL)
    s = thay(s, cu, moi, "moc wauto/ipc")
    cu = "\t\tBOOL\tbLgBre = g_pCoreShell->Breathe();" + NL + "\t\tDWORD\tdwLgT1 = g_nPaintLog > 0 ? timeGetTime() : 0;" + NL + "\t\tBOOL\tbLgUi  = bLgBre ? UiHeartBeat() : FALSE;" + NL + "\t\tDWORD\tdwLgT2 = g_nPaintLog > 0 ? timeGetTime() : 0;" + NL
    moi = ("\t\tBOOL\tbLgBre = g_pCoreShell->Breathe();" + NL
           + "#ifdef JX_MOBILE" + NL + "\t\tJX_LG_MOC(liLgBre); liLgUi = liLgSau = liLgPf = liLgSend = liLgBre;\t// " + DAUL + NL + "#endif" + NL
           + "\t\tDWORD\tdwLgT1 = g_nPaintLog > 0 ? timeGetTime() : 0;" + NL + "\t\tBOOL\tbLgUi  = bLgBre ? UiHeartBeat() : FALSE;" + NL
           + "#ifdef JX_MOBILE" + NL + "\t\tJX_LG_MOC(liLgUi); liLgSau = liLgPf = liLgSend = liLgUi;" + NL + "#endif" + NL
           + "\t\tDWORD\tdwLgT2 = g_nPaintLog > 0 ? timeGetTime() : 0;" + NL)
    s = thay(s, cu, moi, "moc breathe/uihb")
    cu = "\t\t\tm_GameCounter++;" + NL + "\t\t\tint\tnElapse = m_Timer.GetElapse();" + NL
    moi = "#ifdef JX_MOBILE" + NL + "\t\t\tJX_LG_MOC(liLgSau); liLgPf = liLgSend = liLgSau;\t// " + DAUL + " khoi sau UiHeartBeat (replay, TeamManager/TargetInfo UpdateData)" + NL + "#endif" + NL + cu
    s = thay(s, cu, moi, "moc sau")
    cu = "\tSendAllCommand();" + NL + "\tif (MyApp.NotifiIconState())" + NL
    moi = ("#ifdef JX_MOBILE" + NL + "\tJX_LG_MOC(liLgPf);\t// " + DAUL + " sau PROCFRAME/UiPaint" + NL + "#endif" + NL
           + "\tSendAllCommand();" + NL
           + "#ifdef JX_MOBILE" + NL + "\tJX_LG_MOC(liLgSend);" + NL + "#endif" + NL
           + "\tif (MyApp.NotifiIconState())" + NL)
    s = thay(s, cu, moi, "moc send")
    cu = "\tif (g_nPaintLog > 0)" + NL + "\t\tnLogTick = timeGetTime() - nLogT0;" + NL
    moi = (cu
           + "#ifdef JX_MOBILE" + NL
           + "\tif (g_nPaintLog > 0 && nLogTick >= 30)" + NL
           + "\t{\t// " + DAUL + " phan logic cua vong lap >= 30 ms: chia pha (ms)" + NL
           + "\t\tconst double dNet = JxLgMs(liLg0, liLgNet), dWa = JxLgMs(liLgNet, liLgWa), dIpc = JxLgMs(liLgWa, liLgIpc), dBre = JxLgMs(liLgIpc, liLgBre), dUi = JxLgMs(liLgBre, liLgUi);" + NL
           + "\t\tconst double dSau = JxLgMs(liLgUi, liLgSau), dPf = JxLgMs(liLgSau, liLgPf), dSend = JxLgMs(liLgPf, liLgSend);" + NL
           + "\t\tFILE* pLgP = fopen(\"jx_paint.log\", \"a\");" + NL
           + "\t\tif (pLgP)" + NL
           + "\t\t{" + NL
           + "\t\t\tfprintf(pLgP, \"[LOGIC-PHA] t=%u logic=%u ms: mang %.1f, wauto %.1f, ipc %.1f, breathe %.1f, uihb %.1f, sau %.1f, procframe %.1f, gui %.1f, khac %.1f | tick=%d\\n\"," + NL
           + "\t\t\t\tnLogT0, nLogTick, dNet, dWa, dIpc, dBre, dUi, dSau, dPf, dSend, (double)nLogTick - (dNet + dWa + dIpc + dBre + dUi + dSau + dPf + dSend), (int)(m_GameCounter != nLogCntBefore));" + NL
           + "\t\t\tfclose(pLgP);" + NL
           + "\t\t}" + NL
           + "\t}" + NL
           + "#endif" + NL)
    s = thay(s, cu, moi, "LOGIC-PHA ghi")
    return s


va("Sources/Represent/Represent3/BaseInclude.h", v_base)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", v_shell)
va("Sources/Represent/Represent3/TextureRes.cpp", v_res)
va("Sources/Represent/Represent3/TextureResMgr.cpp", v_mgr)
va("Sources/S3Client/S3Client.cpp", v_s3)
