# -*- coding: utf-8 -*-
"""goi_va_doluot_b_0909.py - [DOLUOT 09/09 b] nguong cau hinh duoc + dong 'bat' de biet luong da chay.

Sau [VUNG] khong con tick >= 20 ms, ve max 8-19 ms => [DOLUOT] khong in gi (dung, nhung khong kiem duoc bo lay mau).
Them [Client] DoLuotNguong (ms, mac dinh 20; 0 = gom MOI tick/khung -> ho so trung binh: ve 5-6 ms/khung di dau)
va dong "[DOLUOT] bat: nguong N ms" luc luong chay. Chi KSubWorld.cpp (client).
"""
import io
import sys

NL = "\r\n"
T = "\t"
P = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/KSubWorld.cpp"
TAG = "[DOLUOT 09/09 b]"


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80); lf0 = s.count("\n") - s.count("\r\n")
if TAG in s:
    print("da co"); sys.exit(0)
s = rep(s, "static int            g_nDoLuot = -1;" + T + "// -1 chua doc ini",
        "static int            g_nDoLuot = -1;" + T + "// -1 chua doc ini" + NL +
        "static int            g_nDoLuotNguong = 20;" + T + "// " + TAG + " [Client] DoLuotNguong ms; 0 = gom moi tick/khung",
        "bien nguong")
s = rep(s, T*3 + "g_nDoLuot = GetPrivateProfileIntA(\"Client\", \"DoLuot\", 0, \".\\\\config.ini\") ? 1 : 0;",
        T*3 + "g_nDoLuot = GetPrivateProfileIntA(\"Client\", \"DoLuot\", 0, \".\\\\config.ini\") ? 1 : 0;" + NL +
        T*3 + "g_nDoLuotNguong = GetPrivateProfileIntA(\"Client\", \"DoLuotNguong\", 20, \".\\\\config.ini\");" + T + "// " + TAG,
        "doc nguong")
s = rep(s, T*5 + "if (!h) g_nDoLuot = 0; else CloseHandle(h);",
        T*5 + "if (!h) g_nDoLuot = 0; else CloseHandle(h);" + NL +
        T*5 + "FILE* pB = fopen(\"jx_paint.log\", \"a\");" + T + "// " + TAG + NL +
        T*5 + "if (pB) { fprintf(pB, \"[DOLUOT] bat: %s, nguong %d ms (0 = gom moi tick/khung)\\n\", g_nDoLuot ? \"luong lay mau da chay\" : \"KHONG tao duoc luong\", g_nDoLuotNguong); fclose(pB); }",
        "dong bat")
s = rep(s, T*2 + "if (dMs >= 20.0) { g_aDoLuotNangPha[m_lSeq & (DOLUOT_NANG - 1)] = m_nPha;",
        T*2 + "if (g_nDoLuotNguong <= 0 || dMs >= (double)g_nDoLuotNguong) { g_aDoLuotNangPha[m_lSeq & (DOLUOT_NANG - 1)] = m_nPha;",
        "nguong danh dau")
s = rep(s, "InterlockedExchange(&g_aDoLuotNang[m_lSeq & (DOLUOT_NANG - 1)], m_lSeq); }" + NL,
        "InterlockedExchange(&g_aDoLuotNang[m_lSeq & (DOLUOT_NANG - 1)], m_lSeq); }" + T + "// " + TAG + " nguong 0 = danh dau moi tick/khung" + NL,
        "chu thich cuoi dong")
s = rep(s, "fprintf(pLog, \"[DOLUOT] t=%u pha %s: %u lan nang, %u mau:\", (unsigned)GetTickCount(), nPha == 1 ? \"TICK\" : \"VE\", uLan, uMau);",
        "fprintf(pLog, \"[DOLUOT] t=%u pha %s: %u lan >= %d ms, %u mau:\", (unsigned)GetTickCount(), nPha == 1 ? \"TICK\" : \"VE\", uLan, g_nDoLuotNguong, uMau);",
        "in nguong")
if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KSubWorld.cpp " + TAG)
