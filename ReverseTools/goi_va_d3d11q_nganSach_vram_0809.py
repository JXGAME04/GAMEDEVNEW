# -*- coding: ascii -*-
"""goi_va_d3d11q_nganSach_vram_0809.py - [REP3 08/09 q] Ngan sach cache texture TU DONG theo VRAM va so client dang mo
(cho may yeu / 4 tab), chi khi KHONG co [Client] Rep3CacheMB:
  cap = VRAM_con / so_client / 2 / he_so_atlas(1,3 khi D3D11+atlas), san 256 MB (khung TK nang can ~200 MB @32 bit) hoac VRAM_con/2 neu nho hon.
Kem: ap luc VRAM luc chay - moi 5 s, neu VRAM con < 128 MB va ngan sach > san -> ha 25 % (chi che do tu dong), ghi log."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
def load(name):
    p = ROOT + "\\" + name
    return p, io.open(p, "r", encoding="latin-1", newline="").read()
def save(p, s, h0):
    if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    if re.search(r"[^\r]\n", s): print("FAIL LF", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:70], c, n)); sys.exit(1)
    return s.replace(old, new)

p, s = load("TextureResMgr.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "PressureByVram" not in s:
    s = rep(s, "    void CapBudgetByVram(unsigned __int64 uVramFreeMB);\t// [FX 08/09] kep them theo VRAM con, goi sau khi tao device\r\n",
        "    void CapBudgetByVram(unsigned __int64 uVramFreeMB);\t// [FX 08/09] kep them theo VRAM con, goi sau khi tao device\r\n"
        "    void PressureByVram(unsigned __int64 uVramFreeMB);\t// [REP3 08/09 q] luc chay: VRAM con thap -> ha ngan sach (che do tu dong)\r\n"
        "    unsigned __int64 m_uBudgetFloorMB;\t// [REP3 08/09 q] san ngan sach tu dong\r\n")
save(p, s, h0); print("OK TextureResMgr.h")

p, s = load("TextureResMgr.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "PressureByVram" not in s:
    s = rep(s, '#include "TextureResMgr.h"\r\n', '#include "TextureResMgr.h"\r\n#include <tlhelp32.h>\t// [REP3 08/09 q] dem so client dang mo\r\n', 1)
    s = rep(s, "    m_nMaxReleaseCount = 0;\r\n", "    m_nMaxReleaseCount = 0;\r\n\tm_uBudgetFloorMB = 60;\t// [REP3 08/09 q]\r\n", 1)
    old = ("void TextureResMgr::CapBudgetByVram(unsigned __int64 uVramFreeMB)\r\n{\r\n"
           "\tif (g_nRep3CacheMB > 0 || uVramFreeMB == 0)\r\n\t\treturn;\r\n"
           "\tunsigned __int64 uBudgetMB = ((unsigned __int64)(uint32)m_nBalanceNum) >> 20;\r\n"
           "\tunsigned __int64 uCapMB = uVramFreeMB / 2;\r\n"
           "\tif (uCapMB < 60)\r\n\t\tuCapMB = 60;\r\n")
    new = ("// [REP3 08/09 q] dem tien trinh cung ten exe (4 tab game): moi client chia nhau VRAM\r\n"
           "static int Rep3DemClient()\r\n{\r\n"
           "\tchar szMe[MAX_PATH] = \"\"; GetModuleFileNameA(NULL, szMe, MAX_PATH);\r\n"
           "\tconst char* pMe = strrchr(szMe, '\\\\'); pMe = pMe ? pMe + 1 : szMe;\r\n"
           "\tint n = 0;\r\n"
           "\tHANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);\r\n"
           "\tif (hSnap == INVALID_HANDLE_VALUE) return 1;\r\n"
           "\tPROCESSENTRY32 pe; pe.dwSize = sizeof(pe);\r\n"
           "\tif (Process32First(hSnap, &pe)) { do { if (_stricmp(pe.szExeFile, pMe) == 0) n++; } while (Process32Next(hSnap, &pe)); }\r\n"
           "\tCloseHandle(hSnap);\r\n"
           "\treturn n < 1 ? 1 : n;\r\n"
           "}\r\n"
           "void TextureResMgr::CapBudgetByVram(unsigned __int64 uVramFreeMB)\r\n{\r\n"
           "\tif (g_nRep3CacheMB > 0 || uVramFreeMB == 0)\r\n\t\treturn;\r\n"
           "\tunsigned __int64 uBudgetMB = ((unsigned __int64)(uint32)m_nBalanceNum) >> 20;\r\n"
           "\t// [REP3 08/09 q] chia cho so client dang mo va he so trang atlas (1,3); san 256 MB (khung TK nang ~200 MB) hoac VRAM/2\r\n"
           "\tint nClients = Rep3DemClient();\r\n"
           "\tunsigned __int64 uHeSo10 = (g_nRep3ApiOn == 11 && g_nRep3Atlas) ? 13 : 10;\r\n"
           "\tunsigned __int64 uCapMB = uVramFreeMB * 10 / (unsigned __int64)nClients / 2 / uHeSo10;\r\n"
           "\tunsigned __int64 uFloorMB = 256;\r\n"
           "\tif (uFloorMB > uVramFreeMB / 2) uFloorMB = uVramFreeMB / 2;\r\n"
           "\tif (uFloorMB < 60) uFloorMB = 60;\r\n"
           "\tm_uBudgetFloorMB = uFloorMB;\r\n"
           "\tif (uCapMB < uFloorMB)\r\n\t\tuCapMB = uFloorMB;\r\n"
           "\tRep3Log(\"[REP3] cache texture: VRAM con %I64u MB, %d client dang mo, he so atlas %I64u/10 -> tran %I64u MB (san %I64u)\", uVramFreeMB, nClients, uHeSo10, uCapMB, uFloorMB);\r\n")
    s = rep(s, old, new)
    s = rep(s, "void TextureResMgr::GetStat(uint32& uNodes, uint32& uTexMB, uint32& uRawMB, uint32& uDrawMB, uint32& uBudgetMB)\r\n{\r\n",
        "// [REP3 08/09 q] luc chay: VRAM con < 128 MB (may yeu, nhieu tab) -> ha ngan sach 25 % toi san, dep bot khung ngay. Chi che do tu dong.\r\n"
        "void TextureResMgr::PressureByVram(unsigned __int64 uVramFreeMB)\r\n{\r\n"
        "\tif (g_nRep3CacheMB > 0 || uVramFreeMB == 0 || uVramFreeMB >= 128)\r\n\t\treturn;\r\n"
        "\tunsigned __int64 uBudgetMB = ((unsigned __int64)(uint32)m_nBalanceNum) >> 20;\r\n"
        "\tif (uBudgetMB <= m_uBudgetFloorMB)\r\n\t\treturn;\r\n"
        "\tunsigned __int64 uNewMB = uBudgetMB * 3 / 4;\r\n"
        "\tif (uNewMB < m_uBudgetFloorMB) uNewMB = m_uBudgetFloorMB;\r\n"
        "\tm_nBalanceNum = (int32)(uNewMB * 1024 * 1024);\r\n"
        "\tRep3Log(\"[REP3] cache texture: VRAM con %I64u MB thap -> ha ngan sach %I64u -> %I64u MB\", uVramFreeMB, uBudgetMB, uNewMB);\r\n"
        "\tm_tmLastCheckBalance = 0;\r\n"
        "\tCheckBalance();\r\n"
        "}\r\n"
        "void TextureResMgr::GetStat(uint32& uNodes, uint32& uTexMB, uint32& uRawMB, uint32& uDrawMB, uint32& uBudgetMB)\r\n{\r\n")
save(p, s, h0); print("OK TextureResMgr.cpp")

p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "PressureByVram" not in s:
    s = rep(s, "\tif (m_fFpsAvg >= 25.0f)\r\n\t\tm_TextureResMgr.CheckBalanceFrame();\r\n",
        "\tif (m_fFpsAvg >= 25.0f)\r\n\t\tm_TextureResMgr.CheckBalanceFrame();\r\n"
        "\t{\t// [REP3 08/09 q] moi 5 s: VRAM con thap -> ha ngan sach (che do tu dong)\r\n"
        "\t\tstatic DWORD s_dwLastVram = 0;\r\n"
        "\t\tif (s_dwLastVram == 0 || (dwNow - s_dwLastVram) >= 5000)\r\n"
        "\t\t{\r\n\t\t\ts_dwLastVram = dwNow;\r\n"
        "\t\t\tm_TextureResMgr.PressureByVram((unsigned __int64)(PD3DDEVICE->GetAvailableTextureMem() >> 20));\r\n"
        "\t\t}\r\n\t}\r\n")
save(p, s, h0); print("OK KRepresentShell3.cpp")
print("XONG")
