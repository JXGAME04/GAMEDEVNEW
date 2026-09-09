# -*- coding: utf-8 -*-
"""goi_va_interp_log_0909.py - [INTERP 09/09] Gan LOG vao dung phan khac nhau giua PaintInterp=1 va 0.

Chu chot duoc: PaintInterp=0 HET bi am mau (nhung giat), PaintInterp=1 thi BI.
=> Goc loi bat buoc nam trong phan ma hai che do lam khac nhau. Doan mo khong ra thi DO.

Phan khac nhau, do het:
  A. POSSHIFT chay moi khung ve (PaintInterp=0 khong chay): alpha, so NPC bi doi vi tri ve,
     va DO LECH giua vi tri VE (noi suy) voi vi tri TICK - vi nut cay canh VAN o vi tri tick
     (CoreShell.cpp co ghi ro: NPC thuong chi SetDrawPos, khong MoveObject).
  B. He chieu sang: mot DIEM CO DINH giua luoi sang co bi doi gia tri khong, va doi bao nhieu
     lan moi 10 giay. Day la duong DUY NHAT trong engine nhan vao mau tung anh SPR
     (KRepresentShell3.cpp:1540 color = GetPoint3dLighting(v)), nen neu SPR bay ban doi mau
     thi so o day phai doi theo.

CHI GHI SO, KHONG DOI CACH VE. Bat bang [Client] PaintLog=1 (chu da bat san) -> jx_paint.log.
Chi sua Core -> CoreClient.dll.
"""
import io
import sys

NL = "\r\n"
FILES = {
    "cs": r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\CoreShell.cpp",
    "ipo": r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\Scene\KIpoTree.cpp",
}


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def rep(s, old, new, ten):
    c = s.count(old)
    if c != 1:
        print("FAIL neo %s: thay %d lan, can 1" % (ten, c))
        sys.exit(1)
    return s.replace(old, new)


# ---------------------------------------------------------------- A. CoreShell.cpp
p = FILES["cs"]
s = doc(p)
h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "[INTERP 09/09]" in s:
    print("CoreShell da va roi")
else:
    # A1 - bo dem + ham in, dat NGAY SAU khai bao g_nCorePaintLog
    old = "int\tg_nCorePaintLog = 0;"
    new = old + NL.join([
        "",
        "// [INTERP 09/09] Do TOAN BO phan khac nhau giua PaintInterp=1 va 0 de tim goc \"am mau khi di chuyen\".",
        "// Chu chot: PaintInterp=0 het bi (nhung giat), PaintInterp=1 thi bi => loi nam trong dung phan nay.",
        "// CHI GHI SO, khong doi cach ve. Bat bang [Client] PaintLog=1. In moi 10 giay vao jx_paint.log.",
        "static double   s_dIpAlpha = 0.0;",
        "static unsigned s_uIpFrame = 0, s_uIpKep = 0, s_uIpNpc = 0, s_uIpNpcMax = 0, s_uIpNpcKhung = 0, s_uIpVuot = 0;",
        "static int      s_nIpLechMax = 0, s_nIpLechNguoi = 0;",
        "static void IpInDong()",
        "{",
        "\tif (g_nCorePaintLog <= 0 || s_uIpFrame == 0)",
        "\t\treturn;",
        "\tstatic DWORD s_dwLan = 0;",
        "\tconst DWORD dwNow = timeGetTime();",
        "\tif (s_dwLan == 0) { s_dwLan = dwNow; return; }",
        "\tif (dwNow - s_dwLan < 10000)",
        "\t\treturn;",
        "\ts_dwLan = dwNow;",
        "\tFILE* pLog = fopen(\"jx_paint.log\", \"a\");",
        "\tif (pLog)",
        "\t{",
        "\t\tfprintf(pLog, \"[INTERP] t=%u khung=%u | alpha TB %.0f, cham tran 1000 %u lan | npc dich TB %.1f max %u\"",
        "\t\t\t\" | lech VE-TICK: npc max %d, nguoi choi max %d | vuot bien vung %u\\n\",",
        "\t\t\tdwNow, s_uIpFrame, s_dIpAlpha / s_uIpFrame, s_uIpKep,",
        "\t\t\t(double)s_uIpNpc / s_uIpFrame, s_uIpNpcMax, s_nIpLechMax, s_nIpLechNguoi, s_uIpVuot);",
        "\t\tfclose(pLog);",
        "\t}",
        "\ts_dIpAlpha = 0.0; s_uIpFrame = s_uIpKep = s_uIpNpc = s_uIpNpcMax = s_uIpVuot = 0;",
        "\ts_nIpLechMax = s_nIpLechNguoi = 0;",
        "}",
    ])
    s = rep(s, old, new, "A1 khai bao")

    # A2 - dau POSSHIFT
    old = "\t\tg_nPaintAlpha = nAlpha;\t// [MAU 08/09]"
    new = old + NL + NL.join([
        "\t\tif (g_nCorePaintLog > 0)",
        "\t\t{\t// [INTERP 09/09] moi khung ve mot lan",
        "\t\t\ts_uIpFrame++;",
        "\t\t\ts_dIpAlpha += (double)nAlpha;",
        "\t\t\tif (nAlpha >= 1000) s_uIpKep++;",
        "\t\t\ts_uIpNpcKhung = 0;",
        "\t\t}",
    ])
    s = rep(s, old, new, "A2 dau POSSHIFT")

    # A3 - trong vong lap NPC, ngay sau khi tinh nDrawX/nDrawY
    old = "\t\t\tint\tnDrawY = s_InterpFrom[nIdx].y + (s_InterpTo[nIdx].y - s_InterpFrom[nIdx].y) * nAlpha / 1000;"
    new = old + NL + NL.join([
        "\t\t\tif (g_nCorePaintLog > 0)",
        "\t\t\t{\t// [INTERP 09/09] nut cay canh VAN nam o vi tri TICK (chi SetDrawPos, khong MoveObject),",
        "\t\t\t\t// nen day chinh la do lech giua thu duoc VE va thu cay canh / he chieu sang van tuong la no o do.",
        "\t\t\t\tint nLx = nDrawX - s_InterpTo[nIdx].x; if (nLx < 0) nLx = -nLx;",
        "\t\t\t\tint nLy = nDrawY - s_InterpTo[nIdx].y; if (nLy < 0) nLy = -nLy;",
        "\t\t\t\tconst int nL = (nLx > nLy) ? nLx : nLy;",
        "\t\t\t\ts_uIpNpcKhung++;",
        "\t\t\t\tif (nL > s_nIpLechMax) s_nIpLechMax = nL;",
        "\t\t\t\tif (bIsPlayer && nL > s_nIpLechNguoi) s_nIpLechNguoi = nL;",
        "\t\t\t}",
    ])
    s = rep(s, old, new, "A3 vong lap NPC")

    # A4 - cuoi POSSHIFT (sau vong lap dan)
    old = NL.join([
        "\t\t\tMissle[nMsl].m_nDrawZ = Missle[nMsl].m_nPrevZ + nMdz * nAlpha / 1000;",
        "\t\t}",
        "\t}",
        "\tbreak;",
    ])
    new = NL.join([
        "\t\t\tMissle[nMsl].m_nDrawZ = Missle[nMsl].m_nPrevZ + nMdz * nAlpha / 1000;",
        "\t\t}",
        "\t\tif (g_nCorePaintLog > 0)",
        "\t\t{\t// [INTERP 09/09] chot ky va in moi 10 giay",
        "\t\t\ts_uIpNpc += s_uIpNpcKhung;",
        "\t\t\tif (s_uIpNpcKhung > s_uIpNpcMax) s_uIpNpcMax = s_uIpNpcKhung;",
        "\t\t\tif (nRet == 2) s_uIpVuot++;",
        "\t\t\tIpInDong();",
        "\t\t}",
        "\t}",
        "\tbreak;",
    ])
    s = rep(s, old, new, "A4 cuoi POSSHIFT")

    if sum(1 for ch in s if ord(ch) >= 0x80) != h0:
        print("FAIL byte cao CoreShell"); sys.exit(1)
    if "\ufffd" in s:
        print("FAIL FFFD CoreShell"); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK CoreShell.cpp (byte cao %d giu nguyen)" % h0)


# ---------------------------------------------------------------- B. KIpoTree.cpp
p = FILES["ipo"]
s = doc(p)
h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "[SANGDO 09/09]" in s:
    print("KIpoTree da va roi")
else:
    old = "\t\tg_pRepresent->SetLightInfo(m_nLeftTopX, m_nLeftTopY, (unsigned int*)pLightingArray);"
    new = old + NL + NL.join([
        "\t\t{\t// [SANGDO 09/09] He chieu sang la duong DUY NHAT trong engine nhan vao mau tung anh SPR",
        "\t\t\t// (KRepresentShell3.cpp:1540 color = GetPoint3dLighting(v), roi shader MODULATE voi texture).",
        "\t\t\t// Do xem MOT O CO DINH giua luoi sang co bi doi gia tri khong va doi bao nhieu lan moi 10 giay.",
        "\t\t\t// Neu SPR bay ban doi mau khi co nguoi di chuyen thi so o day phai doi theo. Chi ghi so.",
        "\t\t\textern int g_nCorePaintLog;",
        "\t\t\tif (g_nCorePaintLog > 0)",
        "\t\t\t{",
        "\t\t\t\tstatic DWORD s_dwLan = 0, s_dwTruoc = 0xFFFFFFFF, s_dwMin = 0xFFFFFFFF, s_dwMax = 0;",
        "\t\t\t\tstatic unsigned s_uKhung = 0, s_uDoi = 0, s_uGocDoi = 0, s_uDenTong = 0, s_uDenMax = 0;",
        "\t\t\t\tstatic int s_nGocX = 0x7FFFFFFF, s_nGocY = 0;",
        "\t\t\t\tconst int nO = (LIGHTING_GRID_HEIGHT / 2) * LIGHTING_GRID_WIDTH + (LIGHTING_GRID_WIDTH / 2);",
        "\t\t\t\tconst DWORD dwO = pLightingArray[nO];",
        "\t\t\t\ts_uKhung++;",
        "\t\t\t\tif (s_dwTruoc != 0xFFFFFFFF && dwO != s_dwTruoc) s_uDoi++;",
        "\t\t\t\ts_dwTruoc = dwO;",
        "\t\t\t\tif (dwO < s_dwMin) s_dwMin = dwO;",
        "\t\t\t\tif (dwO > s_dwMax) s_dwMax = dwO;",
        "\t\t\t\tif (m_nLeftTopX != s_nGocX || m_nLeftTopY != s_nGocY)",
        "\t\t\t\t{",
        "\t\t\t\t\tif (s_nGocX != 0x7FFFFFFF) s_uGocDoi++;",
        "\t\t\t\t\ts_nGocX = m_nLeftTopX; s_nGocY = m_nLeftTopY;",
        "\t\t\t\t}",
        "\t\t\t\tconst unsigned uDen = (unsigned)m_LightList.size();",
        "\t\t\t\ts_uDenTong += uDen;",
        "\t\t\t\tif (uDen > s_uDenMax) s_uDenMax = uDen;",
        "\t\t\t\tconst DWORD dwNow = GetTickCount();",
        "\t\t\t\tif (s_dwLan == 0) s_dwLan = dwNow;",
        "\t\t\t\telse if (dwNow - s_dwLan >= 10000)",
        "\t\t\t\t{",
        "\t\t\t\t\ts_dwLan = dwNow;",
        "\t\t\t\t\tFILE* pLog = fopen(\"jx_paint.log\", \"a\");",
        "\t\t\t\t\tif (pLog)",
        "\t\t\t\t\t{",
        "\t\t\t\t\t\tfprintf(pLog, \"[SANGDO] t=%u khung=%u | o giua luoi DOI %u lan | sang min %08X max %08X\"",
        "\t\t\t\t\t\t\t\" | goc cua so doi %u lan | den TB %.1f max %u | nen %08X\\n\",",
        "\t\t\t\t\t\t\tdwNow, s_uKhung, s_uDoi, s_dwMin, s_dwMax, s_uGocDoi,",
        "\t\t\t\t\t\t\ts_uKhung ? (double)s_uDenTong / s_uKhung : 0.0, s_uDenMax, m_dwAmbient);",
        "\t\t\t\t\t\tfclose(pLog);",
        "\t\t\t\t\t}",
        "\t\t\t\t\ts_uKhung = s_uDoi = s_uGocDoi = s_uDenTong = s_uDenMax = 0;",
        "\t\t\t\t\ts_dwMin = 0xFFFFFFFF; s_dwMax = 0;",
        "\t\t\t\t}",
        "\t\t\t}",
        "\t\t}",
    ])
    s = rep(s, old, new, "B SetLightInfo")

    if sum(1 for ch in s if ord(ch) >= 0x80) != h0:
        print("FAIL byte cao KIpoTree"); sys.exit(1)
    if "\ufffd" in s:
        print("FAIL FFFD KIpoTree"); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK KIpoTree.cpp (byte cao %d giu nguyen)" % h0)

for k, f in FILES.items():
    t = doc(f)
    lf = t.count("\n") - t.count("\r\n")
    print("  %-4s CRLF %5d | LF don %d | byte cao %d" % (k, t.count("\r\n"), lf, sum(1 for c in t if ord(c) >= 0x80)))
print("XONG INTERP LOG")
