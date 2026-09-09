# -*- coding: utf-8 -*-
"""goi_va_napchieu_0909.py - [NAPCHIEU 09/09] Nap truoc anh chieu (SPR dan) o luong nen ngay khi nhan goi 95.

Van de (BANGIAO_HIEUUNG_KHAOSAT_0709.md 6.3, chua lam): goi 95 (NetCommandSkill) den truoc khi dan duoc ve
1-2 khung; luc dan ve lan dau, TextureResMgr::GetImage gap SPR chua nap -> giao luong nen, tra NULL -> dan
BO VE khung do ([REP3-NAP] bo_ve 1-33 / 30 s trong tran TK 11:40). Voi chieu moi thay lan dau, khung dau tien
cua dan mat.
Cach: ngay trong NetCommandSkill (luong chinh, truoc khi tra ve som), tra KSkill -> m_nChildSkillId (chi muc
g_MisslesLib, giong CreateMissle) -> voi moi trang thai dan co AnimFileName, goi Represent3 xep hang nap nen.
  - Represent3: TextureResMgr::NapTruoc(pszImage, nType): da co muc -> 1; giao luong nen + chen muc m_bDangNap
    (giong duong chen cua GetImage) -> 2; khong giao duoc -> 0 (KHONG nap dong bo: nap truoc chi la goi y).
    Xuat "C" Rep3_NapTruoc(psz) qua doi tuong shell duy nhat (g_pRep3ShellDuyNhat) -> Core khong can vtable moi.
  - Core (client): NapTruocAnhChieu(nSkillID, nSkillLevel) tra Rep3_NapTruoc bang GetProcAddress mot lan;
    Represent3.dll chua nap / xuat thieu -> bo qua im lang. LowMissle -> dung kieu 1 (giong KMissleRes::Draw).
Khong cham bang mau / atlas / gop lenh ve; luong nen da co [NAP 08/09 b] (Rep3NapNen=0 -> NapTruoc tra 0).
Do: [NAPCHIEU] trong jx_rep3.log (ky 10 s, PaintLog=1): goi / da co / giao nen / khong; so sanh bo_ve truoc-sau.
"""
import io
import sys

NL = "\r\n"
T = "\t"
S4 = "    "
R3 = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
CORE = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/"
F_TRM_H = R3 + "TextureResMgr.h"
F_TRM_C = R3 + "TextureResMgr.cpp"
F_SH_H = R3 + "KRepresentShell3.h"
F_SH_C = R3 + "KRepresentShell3.cpp"
F_PP = CORE + "KProtocolProcess.cpp"
TAG = "[NAPCHIEU 09/09]"


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


# ---------------------------------------------------------------- TextureResMgr.h: khai bao NapTruoc
s, c0, l0, h0 = doc(F_TRM_H)
if TAG not in s:
    old = T + "unsigned m_nNapNenGui, m_nNapNenXong, m_nNapNenHong, m_nNapNenBoVe;" + T + "// thong ke ky ([REP3-NAP])"
    new = old + NL + T + "// " + TAG + " nap truoc (goi y): 1 = da co muc, 2 = da giao luong nen + chen muc dang nap, 0 = khong giao duoc (khong nap dong bo)" + NL + \
          T + "int NapTruoc(const char* pszImage, uint32 nType);"
    s = rep(s, old, new, "TRM.h khai bao")
    ghi(F_TRM_H, s, c0, l0, h0, "TextureResMgr.h")
else:
    print("TextureResMgr.h da co")

# ---------------------------------------------------------------- TextureResMgr.cpp: dinh nghia NapTruoc
s, c0, l0, h0 = doc(F_TRM_C)
if TAG not in s:
    old = "// Luong ve, dau moi khung (RepresentBegin): gan ket qua vao muc; BMP tao texture tai day (can device)." + NL + "void TextureResMgr::NapNenNhan()"
    new = NL.join([
        "// " + TAG + " Nap truoc mot anh o luong nen (luong ve/luong chinh goi, vd ngay khi nhan goi 95 'phong chieu').",
        "// Giong duong chen muc cua GetImage khi bNapNen, nhung KHONG phu thuoc m_bVeDangDien va KHONG nap dong bo:",
        "// nap truoc chi la goi y, khong giao duoc thi thoi (GetImage se nap nhu cu). Tra 1 = da co muc, 2 = da giao, 0 = khong.",
        "int TextureResMgr::NapTruoc(const char* pszImage, uint32 nType)",
        "{",
        T + "if (!pszImage || !pszImage[0] || !g_nRep3NapNen)",
        T + T + "return 0;",
        T + "KAutoCriticalSection AutoLock(m_ImageProcessLock);",
        T + "const uint32 uImage = g_FileName2Id((LPSTR)pszImage);",
        T + "const int nIdx = FindImage(uImage, 0);" + T + "// >= 0: da co; < 0: -(vi tri chen)-1",
        T + "if (nIdx >= 0)",
        T + T + "return 1;" + T + "// da co (dang nap, da nap, hoac muc nap hong dang cho thu lai)",
        T + "if (!NapNenGiao(pszImage, uImage, nType))",
        T + T + "return 0;",
        T + "ResNode node;",
        T + "node.m_bDangNap = true;",
        T + "node.m_bCacheable = true;",
        T + "node.m_nLastUsedTime = GetTickCount();",
        T + "node.m_nRetryTime = GetTickCount();",
        T + "node.m_nLanHong = 0;",
        T + "node.m_nType = nType;",
        T + "node.m_nID = uImage;",
        T + "node.m_pTextureRes = NULL;",
        T + "m_TextureResList.insert(m_TextureResList.begin() + (-nIdx - 1), node);",
        T + "return 2;",
        "}",
        "",
        old,
    ])
    s = rep(s, old, new, "TRM.cpp dinh nghia")
    ghi(F_TRM_C, s, c0, l0, h0, "TextureResMgr.cpp")
else:
    print("TextureResMgr.cpp da co")

# ---------------------------------------------------------------- KRepresentShell3.h: phuong thuc khong ao (khong doi vtable)
s, c0, l0, h0 = doc(F_SH_H)
if TAG not in s:
    old = S4 + "virtual void SetOption(" + NL + S4 + S4 + "//## "
    new = S4 + "// " + TAG + " nap truoc anh (SPR) o luong nen; khong ao -> khong doi vtable iRepresentShell. Tra 0/1/2 (xem TextureResMgr::NapTruoc)" + NL + \
          S4 + "int NapTruoc(const char* pszImage);" + NL + NL + old
    s = rep(s, old, new, "SH.h khai bao")
    ghi(F_SH_H, s, c0, l0, h0, "KRepresentShell3.h")
else:
    print("KRepresentShell3.h da co")

# ---------------------------------------------------------------- KRepresentShell3.cpp: doi tuong duy nhat, xuat "C", dem + log
s, c0, l0, h0 = doc(F_SH_C)
if TAG not in s:
    # (a) bien dem canh cac g_nRep3*
    old = "unsigned g_uRep3ChuGiu = 0, g_uRep3ChuVe = 0;" + T + "// [LOCTG 09/09] hang so thoi gian bo loc trinh khung (ms); 0 = tat"
    new = old + NL + \
          "static KRepresentShell3* g_pRep3ShellDuyNhat = NULL;" + T + "// " + TAG + " doi tuong shell (CreateRepresentShell tao dung 1)" + NL + \
          "static unsigned g_uRep3NapTruoc[3] = { 0, 0, 0 };" + T + "// " + TAG + " ket qua NapTruoc: [0] khong, [1] da co, [2] giao nen"
    s = rep(s, old, new, "SH.cpp bien")
    # (b) ghi nhan doi tuong trong ham tao
    old = "KRepresentShell3::KRepresentShell3()" + NL + "{" + NL + T + "m_nLeft = 0;"
    new = "KRepresentShell3::KRepresentShell3()" + NL + "{" + NL + T + "g_pRep3ShellDuyNhat = this;" + T + "// " + TAG + NL + T + "m_nLeft = 0;"
    s = rep(s, old, new, "SH.cpp ctor")
    # (c) phuong thuc + xuat "C", ngay sau CreateRepresentShell
    old = "iRepresentShell* CreateRepresentShell()" + NL + "{" + NL + T + "return (new KRepresentShell3);" + NL + "}" + NL
    new = old + NL.join([
        "",
        "// " + TAG + " nap truoc anh chieu: Core goi qua GetProcAddress(\"Rep3_NapTruoc\") de khong doi vtable iRepresentShell.",
        "int KRepresentShell3::NapTruoc(const char* pszImage)",
        "{",
        T + "const int n = m_TextureResMgr.NapTruoc(pszImage, ISI_T_SPR);",
        T + "g_uRep3NapTruoc[(n >= 0 && n <= 2) ? n : 0]++;",
        T + "return n;",
        "}",
        "extern \"C\" __declspec(dllexport)",
        "int Rep3_NapTruoc(const char* pszImage)",
        "{",
        T + "if (!g_pRep3ShellDuyNhat || !pszImage)",
        T + T + "return 0;",
        T + "return g_pRep3ShellDuyNhat->NapTruoc(pszImage);",
        "}",
        "",
    ])
    s = rep(s, old, new, "SH.cpp xuat")
    # (d) log ky, canh [CHUGIU]
    old = T*3 + "g_uRep3ChuGiu = 0; g_uRep3ChuVe = 0;"
    new = old + NL + \
          T*3 + "Rep3Log(\"[NAPCHIEU] nap truoc anh chieu: goi %u | da co %u, giao nen %u, khong %u\", g_uRep3NapTruoc[0] + g_uRep3NapTruoc[1] + g_uRep3NapTruoc[2], g_uRep3NapTruoc[1], g_uRep3NapTruoc[2], g_uRep3NapTruoc[0]);" + NL + \
          T*3 + "g_uRep3NapTruoc[0] = g_uRep3NapTruoc[1] = g_uRep3NapTruoc[2] = 0;"
    s = rep(s, old, new, "SH.cpp log")
    ghi(F_SH_C, s, c0, l0, h0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")

# ---------------------------------------------------------------- KProtocolProcess.cpp: goi khi nhan goi 95 (client)
s, c0, l0, h0 = doc(F_PP)
if TAG not in s:
    # (a) ham tro giup, dat ngay truoc NetCommandSkill
    old = "void KProtocolProcess::NetCommandSkill(BYTE* pMsg)" + NL + "{"
    new = NL.join([
        "#ifndef _SERVER",
        "// " + TAG + " Nhan goi 95 (chieu bat dau) -> xep hang nap SPR dan cua chieu o luong nen Represent3 ngay, truoc khi",
        "// dan duoc ve lan dau (1-2 khung sau). Tra Rep3_NapTruoc mot lan bang GetProcAddress; thieu -> bo qua im lang.",
        "// Chi muc dan = m_nChildSkillId (giong CreateMissle); LowMissle -> kieu 1 (giong KMissleRes::Draw).",
        "static void NapTruocAnhChieu(int nSkillID, int nSkillLevel)",
        "{",
        T + "typedef int (*PFN_Rep3NapTruoc)(const char*);",
        T + "static PFN_Rep3NapTruoc s_pfn = NULL;",
        T + "static int s_nTra = 0;",
        T + "if (!s_pfn)",
        T + "{",
        T + T + "if (s_nTra >= 8) return;" + T + "// Represent3.dll chua nap luc dau: thu lai vai lan roi thoi",
        T + T + "s_nTra++;",
        T + T + "HMODULE h = GetModuleHandleA(\"Represent3.dll\");",
        T + T + "if (h) s_pfn = (PFN_Rep3NapTruoc)GetProcAddress(h, \"Rep3_NapTruoc\");",
        T + T + "if (!s_pfn) return;",
        T + "}",
        T + "if (nSkillID <= 0 || nSkillLevel <= 0) return;",
        T + "KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillID, nSkillLevel);",
        T + "if (!pSkill) return;",
        T + "int nStyle = pSkill->GetChildSkillId();",
        T + "if (Option.GetLow(LowMissle)) nStyle = 1;",
        T + "if (nStyle <= 0 || nStyle >= MAX_MISSLESTYLE) return;",
        T + "for (int s = 0; s < MAX_MISSLE_STATUS * 2; s++)",
        T + "{",
        T + T + "const char* psz = g_MisslesLib[nStyle].m_MissleRes.m_MissleRes[s].AnimFileName;",
        T + T + "if (psz[0]) s_pfn(psz);",
        T + "}",
        "}",
        "#endif",
        "",
        old,
    ])
    s = rep(s, old, new, "PP ham")
    # (b) goi ngay sau neo dem goi 95, truoc moi 'return' som
    old = T + "{ extern int g_nFX_rx95; g_nFX_rx95++; }" + T + "// [FX 07/09]"
    new = old + NL + "#ifndef _SERVER" + NL + T + "NapTruocAnhChieu(nSkillID, nSkillLevel);" + T + "// " + TAG + NL + "#endif"
    s = rep(s, old, new, "PP goi")
    ghi(F_PP, s, c0, l0, h0, "KProtocolProcess.cpp")
else:
    print("KProtocolProcess.cpp da co")
print("XONG NAPCHIEU")
