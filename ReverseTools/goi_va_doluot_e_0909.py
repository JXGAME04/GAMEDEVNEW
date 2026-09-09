# -*- coding: utf-8 -*-
"""goi_va_doluot_e_0909.py - [DOLUOT 09/09 e] BO dbghelp trong game (giu Core.pdb -> LNK1201 khong build duoc khi game chay);
in tong theo module (GetModuleHandleEx, khong can pdb) + dong [DOLUOT-EIP] top 240 dia chi (mod+rva:so) de tra OFFLINE bang
CoreClient.map (doluot_tra_map.py gom theo ham). Dong "bat" giu nguyen.
"""
import io
import sys

NL = "\r\n"
T = "\t"
P = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/KSubWorld.cpp"
TAG = "[DOLUOT 09/09 e]"


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
dau = "static void DoLuotIn(FILE* pLog, int nPha, DoLuotDem* aDem, int nDem, unsigned uMau, unsigned uLan)" + NL + "{" + NL
cuoi = T + "fprintf(pLog, \"\\n\");" + NL + "}" + NL
new = NL.join([
    "static void DoLuotIn(FILE* pLog, int nPha, DoLuotDem* aDem, int nDem, unsigned uMau, unsigned uLan)",
    "{",
    T + "// " + TAG + " KHONG dung dbghelp trong game (giu Core.pdb cua thu muc build -> linker LNK1201 khi game dang chay).",
    T + "// Tong theo module (GetModuleHandleEx) + top 240 dia chi 'mod+rva:so' -> tra ten offline bang CoreClient.map (doluot_tra_map.py).",
    T + "struct DoLuotMod { HMODULE hMod; unsigned uSo; char szTen[48]; };",
    T + "static DoLuotMod aMod[48]; int nMod = 0;",
    T + "static HMODULE aEipMod[8192];",
    T + "for (int k = 0; k < nDem; k++)",
    T + "{",
    T*2 + "HMODULE hMod = NULL;",
    T*2 + "GetModuleHandleExA(0x00000004 | 0x00000002, (LPCSTR)aDem[k].uEip, &hMod);" + T + "// FROM_ADDRESS | UNCHANGED_REFCOUNT",
    T*2 + "aEipMod[k] = hMod;",
    T*2 + "int m = 0; for (; m < nMod; m++) if (aMod[m].hMod == hMod) break;",
    T*2 + "if (m == nMod && nMod < 48)",
    T*2 + "{",
    T*3 + "aMod[m].hMod = hMod; aMod[m].uSo = 0; strcpy(aMod[m].szTen, \"?\");",
    T*3 + "char szDuong[MAX_PATH]; if (hMod && GetModuleFileNameA(hMod, szDuong, MAX_PATH)) { const char* p = strrchr(szDuong, '\\\\'); strncpy(aMod[m].szTen, p ? p + 1 : szDuong, 47); aMod[m].szTen[47] = 0; }",
    T*3 + "nMod++;",
    T*2 + "}",
    T*2 + "if (m < nMod) aMod[m].uSo += aDem[k].uSo;",
    T + "}",
    T + "fprintf(pLog, \"[DOLUOT] t=%u pha %s: %u lan >= %d ms, %u mau | module:\", (unsigned)GetTickCount(), nPha == 1 ? \"TICK\" : \"VE\", uLan, g_nDoLuotNguong, uMau);",
    T + "for (int r = 0; r < nMod; r++)",
    T + "{",
    T*2 + "int nMax = r; for (int k = r + 1; k < nMod; k++) if (aMod[k].uSo > aMod[nMax].uSo) nMax = k;",
    T*2 + "if (nMax != r) { DoLuotMod t = aMod[r]; aMod[r] = aMod[nMax]; aMod[nMax] = t; }",
    T*2 + "fprintf(pLog, \" %s %.1f%%\", aMod[r].szTen, aMod[r].uSo * 100.0 / uMau);",
    T + "}",
    T + "fprintf(pLog, \"\\n[DOLUOT-EIP] pha %s mau %u:\", nPha == 1 ? \"TICK\" : \"VE\", uMau);",
    T + "for (int r = 0; r < 240 && r < nDem; r++)",
    T + "{",
    T*2 + "int nMax = r; for (int k = r + 1; k < nDem; k++) if (aDem[k].uSo > aDem[nMax].uSo) nMax = k;",
    T*2 + "if (nMax != r) { DoLuotDem t = aDem[r]; aDem[r] = aDem[nMax]; aDem[nMax] = t; HMODULE hm = aEipMod[r]; aEipMod[r] = aEipMod[nMax]; aEipMod[nMax] = hm; }",
    T*2 + "const char* szMod = \"?\"; for (int m = 0; m < nMod; m++) if (aMod[m].hMod == aEipMod[r]) { szMod = aMod[m].szTen; break; }",
    T*2 + "fprintf(pLog, \" %s+%X:%u\", szMod, (unsigned)(aDem[r].uEip - (DWORD_PTR)aEipMod[r]), aDem[r].uSo);",
    T + "}",
    T + "fprintf(pLog, \"\\n\");",
    "}",
    "",
])
s = rep_khoang(s, dau, cuoi, new, "DoLuotIn")
# bo cac typedef dbghelp khong dung nua? giu (vo hai). Bo #include <dbghelp.h> de khoi phu thuoc SDK header.
if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KSubWorld.cpp " + TAG)
