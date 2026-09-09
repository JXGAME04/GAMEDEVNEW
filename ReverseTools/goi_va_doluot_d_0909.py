# -*- coding: utf-8 -*-
"""goi_va_doluot_d_0909.py - [DOLUOT 09/09 d] gom mau theo HAM (dia chi dau ham qua SymFromAddr) va theo MODULE.

Ho so VE 15:3x (moi khung, ~3000 khung/30 s) 'phang': gom theo EIP nen mot ham bi chia thanh nhieu muc (+59, +47...),
top-12 chi 25 %. Gom theo ham + tong theo module (CoreClient / Represent3 / Engine / driver / ntdll / vcruntime) moi tra loi
duoc 5-6 ms ve moi khung di dau. In 2 dong: '[DOLUOT] ... | module: A x% B y%' va '[DOLUOT] VE ham: | x% Module!Ham+RVA ...' (top 16).
"""
import io
import sys

NL = "\r\n"
T = "\t"
P = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/KSubWorld.cpp"
TAG = "[DOLUOT 09/09 d]"


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


def rep_khoang(s, dau, cuoi, new, ten):
    if s.count(dau) != 1:
        print("FAIL neo %s: dau %d" % (ten, s.count(dau))); sys.exit(1)
    a = s.find(dau); b = s.find(cuoi, a + len(dau))
    if b < 0:
        print("FAIL neo %s: khong thay cuoi" % ten); sys.exit(1)
    return s[:a] + new + s[b + len(cuoi):]


s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80); lf0 = s.count("\n") - s.count("\r\n")
if TAG in s:
    print("da co"); sys.exit(0)

# bang dem lon hon (ve moi khung: nhieu EIP khac nhau)
s = rep(s, T + "static DoLuotDem aDem[2][4096]; static int nDem[2] = { 0, 0 }; static unsigned uMau[2] = { 0, 0 }, uLan[2] = { 0, 0 };",
        T + "static DoLuotDem aDem[2][8192]; static int nDem[2] = { 0, 0 }; static unsigned uMau[2] = { 0, 0 }, uLan[2] = { 0, 0 };" + T + "// " + TAG + " 8192 EIP",
        "bang dem")
s = rep(s, T*3 + "if (k == nDem[p] && nDem[p] < 4096) { aDem[p][k].uEip = m.uEip; aDem[p][k].uSo = 1; nDem[p]++; }",
        T*3 + "if (k == nDem[p] && nDem[p] < 8192) { aDem[p][k].uEip = m.uEip; aDem[p][k].uSo = 1; nDem[p]++; }",
        "gioi han dem")

# thay phan in (tu dong chu thich 'sap xep giam dan' den het ham)
dau = T + "// sap xep giam dan theo so mau (chon dan 12)" + NL
cuoi = T + "fprintf(pLog, \"\\n\");" + NL + "}" + NL
new = NL.join([
    T + "// " + TAG + " gom theo HAM (dia chi dau ham) va theo MODULE; in top 16 ham + tong module",
    T + "struct DoLuotHam { DWORD_PTR uDau; HMODULE hMod; unsigned uSo; char szTen[96]; };",
    T + "struct DoLuotMod { HMODULE hMod; unsigned uSo; char szTen[48]; };",
    T + "static DoLuotHam aHam[8192]; int nHam = 0;",
    T + "static DoLuotMod aMod[48]; int nMod = 0;",
    T + "for (int k = 0; k < nDem; k++)",
    T + "{",
    T*2 + "HMODULE hMod = NULL;",
    T*2 + "GetModuleHandleExA(0x00000004 | 0x00000002, (LPCSTR)aDem[k].uEip, &hMod);" + T + "// FROM_ADDRESS | UNCHANGED_REFCOUNT",
    T*2 + "DWORD_PTR uDau = aDem[k].uEip; char szTen[96] = \"\";",
    T*2 + "if (s_pfnTu)",
    T*2 + "{",
    T*3 + "char aBuf[sizeof(SYMBOL_INFO) + 200]; SYMBOL_INFO* pSym = (SYMBOL_INFO*)aBuf; memset(aBuf, 0, sizeof(aBuf));",
    T*3 + "pSym->SizeOfStruct = sizeof(SYMBOL_INFO); pSym->MaxNameLen = 199; DWORD64 uLech = 0;",
    T*3 + "if (s_pfnTu(GetCurrentProcess(), (DWORD64)aDem[k].uEip, &uLech, pSym)) { uDau = (DWORD_PTR)pSym->Address; strncpy(szTen, pSym->Name, 95); szTen[95] = 0; }",
    T*2 + "}",
    T*2 + "int m = 0; for (; m < nMod; m++) if (aMod[m].hMod == hMod) break;",
    T*2 + "if (m == nMod && nMod < 48)",
    T*2 + "{",
    T*3 + "aMod[m].hMod = hMod; aMod[m].uSo = 0; strcpy(aMod[m].szTen, \"?\");",
    T*3 + "char szDuong[MAX_PATH]; if (hMod && GetModuleFileNameA(hMod, szDuong, MAX_PATH)) { const char* p = strrchr(szDuong, '\\\\'); strncpy(aMod[m].szTen, p ? p + 1 : szDuong, 47); aMod[m].szTen[47] = 0; }",
    T*3 + "nMod++;",
    T*2 + "}",
    T*2 + "if (m < nMod) aMod[m].uSo += aDem[k].uSo;",
    T*2 + "int j = 0; for (; j < nHam; j++) if (aHam[j].uDau == uDau && aHam[j].hMod == hMod) break;",
    T*2 + "if (j == nHam && nHam < 8192) { aHam[j].uDau = uDau; aHam[j].hMod = hMod; aHam[j].uSo = 0; strcpy(aHam[j].szTen, szTen); nHam++; }",
    T*2 + "if (j < nHam) aHam[j].uSo += aDem[k].uSo;",
    T + "}",
    T + "fprintf(pLog, \"[DOLUOT] t=%u pha %s: %u lan >= %d ms, %u mau | module:\", (unsigned)GetTickCount(), nPha == 1 ? \"TICK\" : \"VE\", uLan, g_nDoLuotNguong, uMau);",
    T + "for (int r = 0; r < nMod; r++)",
    T + "{",
    T*2 + "int nMax = r; for (int k = r + 1; k < nMod; k++) if (aMod[k].uSo > aMod[nMax].uSo) nMax = k;",
    T*2 + "if (nMax != r) { DoLuotMod t = aMod[r]; aMod[r] = aMod[nMax]; aMod[nMax] = t; }",
    T*2 + "fprintf(pLog, \" %s %.1f%%\", aMod[r].szTen, aMod[r].uSo * 100.0 / uMau);",
    T + "}",
    T + "fprintf(pLog, \"\\n[DOLUOT] %s ham:\", nPha == 1 ? \"TICK\" : \"VE\");",
    T + "for (int r = 0; r < 16 && r < nHam; r++)",
    T + "{",
    T*2 + "int nMax = r; for (int k = r + 1; k < nHam; k++) if (aHam[k].uSo > aHam[nMax].uSo) nMax = k;",
    T*2 + "if (nMax != r) { DoLuotHam t = aHam[r]; aHam[r] = aHam[nMax]; aHam[nMax] = t; }",
    T*2 + "const char* szMod = \"?\"; for (int m = 0; m < nMod; m++) if (aMod[m].hMod == aHam[r].hMod) { szMod = aMod[m].szTen; break; }",
    T*2 + "fprintf(pLog, \" | %.1f%% %s!%s+%X\", aHam[r].uSo * 100.0 / uMau, szMod, aHam[r].szTen[0] ? aHam[r].szTen : \"?\", (unsigned)(aHam[r].uDau - (DWORD_PTR)aHam[r].hMod));",
    T + "}",
    T + "fprintf(pLog, \"\\n\");",
    "}",
    "",
])
s = rep_khoang(s, dau, cuoi, new, "in ham/module")
if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KSubWorld.cpp " + TAG)
