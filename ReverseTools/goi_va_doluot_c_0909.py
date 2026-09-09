# -*- coding: utf-8 -*-
"""goi_va_doluot_c_0909.py - [DOLUOT 09/09 c] moc VE dat dung cho + dbghelp dung ca khi da khoi tao san.

Log 15:2x: co [DOLUOT] pha TICK nhung KHONG co pha VE: KSubWorld::Paint chi la lop debug (return som), duong ve that la
KCoreShell::DrawGameSpace (CoreShell.cpp) -> g_ScenePlace.Paint(). Ten ham deu '?': SymInitialize thu 2 (CrashLog.cpp da
khoi tao o boot) tra FALSE nen chua bao gio lay SymFromAddr. Sua:
  - KDoLuot.h (moi): DoLuotBatDau/DoLuotKetThuc + RAII DoLuotPham (client) - dung duoc tu CoreShell.cpp.
  - KSubWorld.cpp: dinh nghia 2 ham, bo struct cu; moc tick giu; moc ve o KSubWorld::Paint bo.
  - CoreShell.cpp: DoLuotPham doLuotVe(2) dau KCoreShell::DrawGameSpace.
  - dbghelp: lay SymFromAddr bat ke SymInitialize tra gi (da khoi tao san van dung duoc), goi SymRefreshModuleList.
"""
import io
import sys

NL = "\r\n"
T = "\t"
CORE = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/"
F_SW = CORE + "KSubWorld.cpp"
F_CS = CORE + "CoreShell.cpp"
F_H = CORE + "KDoLuot.h"
TAG = "[DOLUOT 09/09 c]"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, s.count("\r\n"), s.count("\n") - s.count("\r\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, crlf0, lf0, h0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


def rep_khoang(s, dau, cuoi, new, ten):
    """thay tu 'dau' (duy nhat) den het lan xuat hien DAU TIEN cua 'cuoi' SAU dau"""
    if s.count(dau) != 1:
        print("FAIL neo %s: dau %d" % (ten, s.count(dau))); sys.exit(1)
    a = s.find(dau); b = s.find(cuoi, a + len(dau))
    if b < 0:
        print("FAIL neo %s: khong thay cuoi sau dau" % ten); sys.exit(1)
    b += len(cuoi)
    return s[:a] + new + s[b:]


# ---------------------------------------------------------------- KDoLuot.h (moi, CRLF, ASCII)
hdr = NL.join([
    "#ifndef KDoLuotH",
    "#define KDoLuotH",
    "// " + TAG + " Bo lay mau con tro lenh luong chinh (dinh nghia trong KSubWorld.cpp). [Client] DoLuot=1, DoLuotNguong=ms (0 = moi tick/khung).",
    "// Dung: { DoLuotPham p(1); ... } quanh tick the gioi (KSubWorld::Activate) va DoLuotPham p(2) quanh ve (KCoreShell::DrawGameSpace).",
    "#ifndef _SERVER",
    "LONG DoLuotBatDau(int nPha, LARGE_INTEGER* pLi0);" + T + "// tra so thu tu (0 = tat)",
    "void DoLuotKetThuc(int nPha, LONG lSeq, const LARGE_INTEGER& li0);",
    "struct DoLuotPham",
    "{",
    T + "LARGE_INTEGER m_li0; int m_nPha; LONG m_lSeq;",
    T + "DoLuotPham(int nPha) : m_nPha(nPha) { m_lSeq = DoLuotBatDau(nPha, &m_li0); }",
    T + "~DoLuotPham() { if (m_lSeq) DoLuotKetThuc(m_nPha, m_lSeq, m_li0); }",
    "};",
    "#endif",
    "#endif",
    "",
])
io.open(F_H, "w", encoding="latin-1", newline="").write(hdr)
print("OK KDoLuot.h (moi)")

# ---------------------------------------------------------------- KSubWorld.cpp
s, c0, l0, h0 = doc(F_SW)
if TAG not in s:
    # (a) struct RAII cu -> 2 ham
    dau = "// Luong chinh: RAII quanh tick (KSubWorld::Activate) va ve (KSubWorld::Paint)" + NL + "struct DoLuotPham" + NL + "{"
    cuoi = T + "}" + NL + "};" + NL
    new = NL.join([
        "// " + TAG + " Luong chinh: DoLuotPham (KDoLuot.h) goi 2 ham nay quanh tick (KSubWorld::Activate) va ve (KCoreShell::DrawGameSpace)",
        "LONG DoLuotBatDau(int nPha, LARGE_INTEGER* pLi0)",
        "{",
        T + "if (g_nDoLuot < 0)",
        T + "{",
        T*2 + "g_nDoLuot = GetPrivateProfileIntA(\"Client\", \"DoLuot\", 0, \".\\\\config.ini\") ? 1 : 0;",
        T*2 + "g_nDoLuotNguong = GetPrivateProfileIntA(\"Client\", \"DoLuotNguong\", 20, \".\\\\config.ini\");",
        T*2 + "if (g_nDoLuot)",
        T*2 + "{",
        T*3 + "if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &g_hDoLuotChinh, THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION, FALSE, 0))",
        T*4 + "g_nDoLuot = 0;",
        T*3 + "else",
        T*3 + "{",
        T*4 + "unsigned uTid = 0; HANDLE h = (HANDLE)_beginthreadex(NULL, 0, DoLuotLuong, NULL, 0, &uTid);",
        T*4 + "if (!h) g_nDoLuot = 0; else CloseHandle(h);",
        T*3 + "}",
        T*3 + "FILE* pB = fopen(\"jx_paint.log\", \"a\");",
        T*3 + "if (pB) { fprintf(pB, \"[DOLUOT] bat: %s, nguong %d ms (0 = gom moi tick/khung)\\n\", g_nDoLuot ? \"luong lay mau da chay\" : \"KHONG tao duoc luong\", g_nDoLuotNguong); fclose(pB); }",
        T*2 + "}",
        T + "}",
        T + "if (!g_nDoLuot || nPha < 1 || nPha > 2) return 0;",
        T + "const LONG lSeq = InterlockedIncrement(&g_lDoLuotSeq);",
        T + "QueryPerformanceCounter(pLi0);",
        T + "InterlockedExchange(&g_lDoLuotPha, (LONG)nPha);",
        T + "return lSeq;",
        "}",
        "void DoLuotKetThuc(int nPha, LONG lSeq, const LARGE_INTEGER& li0)",
        "{",
        T + "InterlockedExchange(&g_lDoLuotPha, 0);",
        T + "LARGE_INTEGER li1, f; QueryPerformanceCounter(&li1); QueryPerformanceFrequency(&f);",
        T + "const double dMs = (double)(li1.QuadPart - li0.QuadPart) * 1000.0 / (double)f.QuadPart;",
        T + "if (g_nDoLuotNguong <= 0 || dMs >= (double)g_nDoLuotNguong) { g_aDoLuotNangPha[lSeq & (DOLUOT_NANG - 1)] = nPha; InterlockedExchange(&g_aDoLuotNang[lSeq & (DOLUOT_NANG - 1)], lSeq); }",
        "}",
        "",
    ])
    s = rep_khoang(s, dau, cuoi, new, "SW struct -> ham")
    # (b) include header (sau crtdbg)
    s = rep(s, "#include \"crtdbg.h\"" + NL, "#include \"crtdbg.h\"" + NL + "#include \"KDoLuot.h\"" + T + "// " + TAG + NL, "SW include")
    # (c) bo moc ve o KSubWorld::Paint (lop debug, return som)
    s = rep(s, T + "DoLuotPham doLuotVe(2);" + T + "// [DOLUOT 09/09] ve the gioi" + NL, "", "SW bo moc ve")
    # (d) dbghelp: lay SymFromAddr ca khi SymInitialize that bai (CrashLog da khoi tao), refresh module
    old = NL.join([
        T*3 + "if (pInit && pInit(GetCurrentProcess(), NULL, TRUE))",
        T*4 + "s_pfnTu = (PFN_DoLuotSymFromAddr)GetProcAddress(h, \"SymFromAddr\");",
    ])
    new = NL.join([
        T*3 + "if (pInit) pInit(GetCurrentProcess(), NULL, TRUE);" + T + "// " + TAG + " FALSE = da khoi tao san (CrashLog.cpp) -> van dung duoc",
        T*3 + "typedef BOOL (WINAPI *PFN_DoLuotSymRefresh)(HANDLE);",
        T*3 + "PFN_DoLuotSymRefresh pRefresh = (PFN_DoLuotSymRefresh)GetProcAddress(h, \"SymRefreshModuleList\");",
        T*3 + "if (pRefresh) pRefresh(GetCurrentProcess());",
        T*3 + "s_pfnTu = (PFN_DoLuotSymFromAddr)GetProcAddress(h, \"SymFromAddr\");",
    ])
    s = rep(s, old, new, "SW dbghelp")
    ghi(F_SW, s, c0, l0, h0, "KSubWorld.cpp")
else:
    print("KSubWorld.cpp da co")

# ---------------------------------------------------------------- CoreShell.cpp: moc ve that
s, c0, l0, h0 = doc(F_CS)
if TAG not in s:
    old = "void KCoreShell::DrawGameSpace()" + NL + "{" + NL + "#ifndef _SERVER" + NL
    new = "void KCoreShell::DrawGameSpace()" + NL + "{" + NL + "#ifndef _SERVER" + NL + T + "DoLuotPham doLuotVe(2);" + T + "// " + TAG + " ve the gioi that (g_ScenePlace.Paint)" + NL
    s = rep(s, old, new, "CS moc ve")
    s = rep(s, "#include \"CoreDrawGameObj.h\"" + NL, "#include \"CoreDrawGameObj.h\"" + NL + "#include \"KDoLuot.h\"" + T + "// " + TAG + NL, "CS include")
    ghi(F_CS, s, c0, l0, h0, "CoreShell.cpp")
else:
    print("CoreShell.cpp da co")
print("XONG DOLUOT c")
