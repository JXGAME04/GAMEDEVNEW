# -*- coding: utf-8 -*-
"""goi_va_napnpc_0909.py - [NAPNPC 09/09] Nap truoc anh THAN NPC (bo phan, bong, hieu ung trang thai) o luong nen
ngay khi ten anh duoc gan (KSprControl::SetSprFile) - tuc la luc NPC duoc them / doi do / doi dong tac, TRUOC lan ve dau.

Do [NAPCHIEU b]: bo_ve con lai toan anh than NPC (429/1294 luc vao map, 38-47 luc dong). Cho gan ten duy nhat cua
moi anh NPC la KSprControl::SetSprFile (Init/SetHelm/SetArmor/SetWeapon/SetHorse/SetAction/SetRideHorse deu qua day,
da bo qua khi ten khong doi) -> goi Rep3_NapTruoc2(ten, 2).
Kem theo (Represent3):
  - HANG DOI 2 MUC: anh dang ve can (GetImage -> NapNenGiao) vao hang TRUOC; nap truoc vao hang SAU; luong nen lay hang
    truoc het roi moi den hang sau => nap truoc khong bao gio lam anh dang can doi lau hon.
  - Dem kip/tre THEO NGUON: 1 = anh chieu (goi 95), 2 = anh than NPC. Log [NAPCHIEU] va [NAPNPC] rieng.
  - Cong tac [Client] Rep3NapNpc (mac dinh 1; 0 = tat A/B), doc lap voi Rep3NapChieu.
  - Xuat them Rep3_NapTruoc2(psz, nguon); Rep3_NapTruoc(psz) giu nguyen = nguon 1.
Core (client): ham dung chung Rep3NapTruocAnh(psz, nguon) trong KNpcRes.cpp (tra GetProcAddress "Rep3_NapTruoc2" 1 lan);
KProtocolProcess::NapTruocAnhChieu dung lai ham nay; KSprControl::SetSprFile goi nguon 2. May chu khong doi (#ifndef _SERVER).
"""
import io
import sys

NL = "\r\n"
T = "\t"
R3 = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
CORE = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/"
F_TRM_H = R3 + "TextureResMgr.h"
F_TRM_C = R3 + "TextureResMgr.cpp"
F_SH_H = R3 + "KRepresentShell3.h"
F_SH_C = R3 + "KRepresentShell3.cpp"
F_SPR = CORE + "KSprControl.cpp"
F_NRES = CORE + "KNpcRes.cpp"
F_PP = CORE + "KProtocolProcess.cpp"
TAG = "[NAPNPC 09/09]"


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
    """thay ca doan tu 'dau' (ke ca) den het 'cuoi' (ke ca); moi neo phai xuat hien dung 1 lan, cuoi sau dau"""
    if s.count(dau) != 1 or s.count(cuoi) != 1:
        print("FAIL neo %s: dau %d cuoi %d" % (ten, s.count(dau), s.count(cuoi))); sys.exit(1)
    a = s.find(dau); b = s.find(cuoi) + len(cuoi)
    if b <= a:
        print("FAIL neo %s: cuoi truoc dau" % ten); sys.exit(1)
    return s[:a] + new + s[b:]


# ================================================================ TextureResMgr.h
s, c0, l0, h0 = doc(F_TRM_H)
if TAG not in s:
    s = rep(s, T + "ResNode() : m_bDangNap(false), m_nLanHong(0), m_bNapTruoc(false) {}",
            T + "ResNode() : m_bDangNap(false), m_nLanHong(0), m_nNapTruoc(0) {}", "TRM.h ctor")
    s = rep(s, T + "bool" + T*2 + "m_bNapTruoc;" + T*4 + "// [NAPCHIEU 09/09 b] muc do NapTruoc chen, chua duoc hoi lan nao (lan hoi dau: kip/tre)",
            T + "unsigned char" + T + "m_nNapTruoc;" + T*4 + "// [NAPCHIEU 09/09 b] " + TAG + " muc do NapTruoc chen, chua duoc hoi lan nao: 0 = khong, 1 = anh chieu, 2 = anh than NPC (lan hoi dau: kip/tre theo nguon)",
            "TRM.h co")
    s = rep(s, T + "unsigned m_nNapTruocKip, m_nNapTruocTre;" + T + "// [NAPCHIEU 09/09 b] lan hoi dau cua muc nap truoc: da nap xong / con dang nap",
            T + "unsigned m_nNapTruocKip[3], m_nNapTruocTre[3];" + T + "// [NAPCHIEU 09/09 b] " + TAG + " lan hoi dau cua muc nap truoc theo nguon [1] chieu [2] NPC: da nap xong / con dang nap",
            "TRM.h bien dem")
    s = rep(s, T + "int NapTruoc(const char* pszImage, uint32 nType);",
            T + "int NapTruoc(const char* pszImage, uint32 nType, int nNguon);" + T + "// " + TAG + " nNguon: 1 = anh chieu, 2 = anh than NPC",
            "TRM.h NapTruoc")
    s = rep(s, T + "bool NapNenGiao(const char* pszImage, uint32 uId, uint32 nType);" + T + "// false = khong tao duoc luong -> nap ngay",
            T + "bool NapNenGiao(const char* pszImage, uint32 uId, uint32 nType, bool bSau = false);" + T + "// false = khong tao duoc luong -> nap ngay; " + TAG + " bSau: hang SAU (nap truoc), mac dinh hang TRUOC (anh dang ve can)",
            "TRM.h NapNenGiao")
    s = rep(s, T + "vector<NapViec> m_napViec; vector<NapKetQua> m_napXong;",
            T + "vector<NapViec> m_napViec, m_napViecSau; vector<NapKetQua> m_napXong;",
            "TRM.h hang doi")
    ghi(F_TRM_H, s, c0, l0, h0, "TextureResMgr.h")
else:
    print("TextureResMgr.h da co")

# ================================================================ TextureResMgr.cpp
s, c0, l0, h0 = doc(F_TRM_C)
if TAG not in s:
    s = rep(s, T + "m_nNapTruocKip = 0; m_nNapTruocTre = 0;" + T + "// [NAPCHIEU 09/09 b]",
            T + "m_nNapTruocKip[0] = m_nNapTruocKip[1] = m_nNapTruocKip[2] = 0; m_nNapTruocTre[0] = m_nNapTruocTre[1] = m_nNapTruocTre[2] = 0;" + T + "// [NAPCHIEU 09/09 b] " + TAG,
            "TRM.cpp ctor")
    # dem theo nguon trong GetImage
    old = NL.join([
        T*3 + "if (m_TextureResList[nImagePosition].m_bNapTruoc)" + T + "// [NAPCHIEU 09/09 b] lan hoi dau tien cua muc nap truoc",
        T*3 + "{",
        T*4 + "m_TextureResList[nImagePosition].m_bNapTruoc = false;",
        T*4 + "if (m_TextureResList[nImagePosition].m_bDangNap) m_nNapTruocTre++;",
        T*4 + "else if (m_TextureResList[nImagePosition].m_pTextureRes) m_nNapTruocKip++;",
        T*3 + "}",
    ])
    new = NL.join([
        T*3 + "if (m_TextureResList[nImagePosition].m_nNapTruoc)" + T + "// [NAPCHIEU 09/09 b] lan hoi dau tien cua muc nap truoc; " + TAG + " dem theo nguon",
        T*3 + "{",
        T*4 + "const int nNg = (m_TextureResList[nImagePosition].m_nNapTruoc < 3) ? (int)m_TextureResList[nImagePosition].m_nNapTruoc : 0;",
        T*4 + "m_TextureResList[nImagePosition].m_nNapTruoc = 0;",
        T*4 + "if (m_TextureResList[nImagePosition].m_bDangNap) m_nNapTruocTre[nNg]++;",
        T*4 + "else if (m_TextureResList[nImagePosition].m_pTextureRes) m_nNapTruocKip[nNg]++;",
        T*3 + "}",
    ])
    s = rep(s, old, new, "TRM.cpp GetImage dem")
    # NapTruoc: nguon + hang sau
    s = rep(s, "int TextureResMgr::NapTruoc(const char* pszImage, uint32 nType)" + NL + "{",
            "int TextureResMgr::NapTruoc(const char* pszImage, uint32 nType, int nNguon)" + T + "// " + TAG + " nNguon 1 = chieu, 2 = NPC; vao hang SAU" + NL + "{",
            "TRM.cpp NapTruoc dau")
    s = rep(s, T + "if (!NapNenGiao(pszImage, uImage, nType))" + NL + T*2 + "return 0;" + NL + T + "ResNode node;",
            T + "if (!NapNenGiao(pszImage, uImage, nType, true))" + NL + T*2 + "return 0;" + NL + T + "ResNode node;",
            "TRM.cpp NapTruoc giao")
    s = rep(s, T + "node.m_bNapTruoc = true;" + T + "// [NAPCHIEU 09/09 b]",
            T + "node.m_nNapTruoc = (unsigned char)((nNguon >= 1 && nNguon <= 2) ? nNguon : 0);" + T + "// [NAPCHIEU 09/09 b] " + TAG,
            "TRM.cpp NapTruoc co")
    # NapNenGiao: tham so bSau + 2 hang
    s = rep(s, "bool TextureResMgr::NapNenGiao(const char* pszImage, uint32 uId, uint32 nType)" + NL + "{",
            "bool TextureResMgr::NapNenGiao(const char* pszImage, uint32 uId, uint32 nType, bool bSau)" + T + "// " + TAG + " bSau = hang sau (nap truoc)" + NL + "{",
            "TRM.cpp NapNenGiao dau")
    s = rep(s, T*2 + "m_napViec.push_back(v);",
            T*2 + "if (bSau) m_napViecSau.push_back(v); else m_napViec.push_back(v);" + T + "// " + TAG,
            "TRM.cpp NapNenGiao push")
    # NapNenChay: lay hang truoc roi hang sau
    old = NL.join([
        T*4 + "if (m_napViec.empty())",
        T*5 + "break;",
        T*4 + "v = m_napViec.front();",
        T*4 + "m_napViec.erase(m_napViec.begin());",
    ])
    new = NL.join([
        T*4 + "vector<NapViec>& q = m_napViec.empty() ? m_napViecSau : m_napViec;" + T + "// " + TAG + " hang TRUOC (anh dang ve can) het roi moi den hang SAU (nap truoc)",
        T*4 + "if (q.empty())",
        T*5 + "break;",
        T*4 + "v = q.front();",
        T*4 + "q.erase(q.begin());",
    ])
    s = rep(s, old, new, "TRM.cpp NapNenChay")
    s = rep(s, T + "m_napXong.clear(); m_napViec.clear();",
            T + "m_napXong.clear(); m_napViec.clear(); m_napViecSau.clear();" + T + "// " + TAG,
            "TRM.cpp NapNenDung")
    ghi(F_TRM_C, s, c0, l0, h0, "TextureResMgr.cpp")
else:
    print("TextureResMgr.cpp da co")

# ================================================================ KRepresentShell3.h
s, c0, l0, h0 = doc(F_SH_H)
if TAG not in s:
    s = rep(s, "    int NapTruoc(const char* pszImage);",
            "    int NapTruoc(const char* pszImage, int nNguon);" + T + "// " + TAG + " nNguon: 1 = anh chieu (Rep3NapChieu), 2 = anh than NPC (Rep3NapNpc)",
            "SH.h NapTruoc")
    ghi(F_SH_H, s, c0, l0, h0, "KRepresentShell3.h")
else:
    print("KRepresentShell3.h da co")

# ================================================================ KRepresentShell3.cpp
s, c0, l0, h0 = doc(F_SH_C)
if TAG not in s:
    s = rep(s, "static unsigned g_uRep3NapTruoc[3] = { 0, 0, 0 };" + T + "// [NAPCHIEU 09/09] ket qua NapTruoc: [0] khong, [1] da co, [2] giao nen",
            "static unsigned g_uRep3NapTruoc[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };" + T + "// [NAPCHIEU 09/09] " + TAG + " [nguon 1 chieu / 2 NPC][ket qua: 0 khong, 1 da co, 2 giao nen]",
            "SH.cpp bien dem")
    s = rep(s, "int g_nRep3NapChieu = 1;" + T + "// [NAPCHIEU 09/09 b] cong tac [Client] Rep3NapChieu: 1 = nap truoc anh chieu khi nhan goi 95 (mac dinh), 0 = tat (A/B)",
            "int g_nRep3NapChieu = 1;" + T + "// [NAPCHIEU 09/09 b] cong tac [Client] Rep3NapChieu: 1 = nap truoc anh chieu khi nhan goi 95 (mac dinh), 0 = tat (A/B)" + NL +
            "int g_nRep3NapNpc = 1;" + T + "// " + TAG + " cong tac [Client] Rep3NapNpc: 1 = nap truoc anh than NPC khi gan ten (mac dinh), 0 = tat (A/B)",
            "SH.cpp cong tac")
    s = rep(s, T + 'g_nRep3NapChieu  = Rep3Ini("Rep3NapChieu", 1) ? 1 : 0;' + T + "// [NAPCHIEU 09/09 b]",
            T + 'g_nRep3NapChieu  = Rep3Ini("Rep3NapChieu", 1) ? 1 : 0;' + T + "// [NAPCHIEU 09/09 b]" + NL +
            T + 'g_nRep3NapNpc    = Rep3Ini("Rep3NapNpc", 1) ? 1 : 0;' + T + "// " + TAG,
            "SH.cpp doc ini")
    # phuong thuc + 2 ham xuat: thay ca khoi cu
    dau = '// [NAPCHIEU 09/09] nap truoc anh chieu: Core goi qua GetProcAddress("Rep3_NapTruoc") de khong doi vtable iRepresentShell.'
    cuoi = NL.join([
        "int Rep3_NapTruoc(const char* pszImage)",
        "{",
        T + "if (!g_pRep3ShellDuyNhat || !pszImage)",
        T*2 + "return 0;",
        T + "return g_pRep3ShellDuyNhat->NapTruoc(pszImage);",
        "}",
    ])
    new = NL.join([
        '// [NAPCHIEU 09/09] nap truoc anh: Core goi qua GetProcAddress("Rep3_NapTruoc2") de khong doi vtable iRepresentShell.',
        "// " + TAG + " nNguon 1 = anh chieu (goi 95, cong tac Rep3NapChieu), 2 = anh than NPC (SetSprFile, cong tac Rep3NapNpc).",
        "int KRepresentShell3::NapTruoc(const char* pszImage, int nNguon)",
        "{",
        T + "if (nNguon < 1 || nNguon > 2) return 0;",
        T + "if (nNguon == 1 && !g_nRep3NapChieu) return 0;" + T + "// cong tac tat: khong nap truoc, khong dem",
        T + "if (nNguon == 2 && !g_nRep3NapNpc) return 0;",
        T + "const int n = m_TextureResMgr.NapTruoc(pszImage, ISI_T_SPR, nNguon);",
        T + "g_uRep3NapTruoc[nNguon][(n >= 0 && n <= 2) ? n : 0]++;",
        T + "return n;",
        "}",
        'extern "C" __declspec(dllexport)',
        "int Rep3_NapTruoc(const char* pszImage)",
        "{",
        T + "if (!g_pRep3ShellDuyNhat || !pszImage)",
        T*2 + "return 0;",
        T + "return g_pRep3ShellDuyNhat->NapTruoc(pszImage, 1);",
        "}",
        'extern "C" __declspec(dllexport)',
        "int Rep3_NapTruoc2(const char* pszImage, int nNguon)" + T + "// " + TAG,
        "{",
        T + "if (!g_pRep3ShellDuyNhat || !pszImage)",
        T*2 + "return 0;",
        T + "return g_pRep3ShellDuyNhat->NapTruoc(pszImage, nNguon);",
        "}",
    ])
    s = rep_khoang(s, dau, cuoi, new, "SH.cpp khoi xuat")
    # log: thay 3 dong cu bang vong 2 nguon
    dau = T*3 + 'Rep3Log("[NAPCHIEU] nap truoc anh chieu (Rep3NapChieu=%d):'
    cuoi = T*3 + "g_uRep3NapTruoc[0] = g_uRep3NapTruoc[1] = g_uRep3NapTruoc[2] = 0;"
    new = NL.join([
        T*3 + "for (int ng = 1; ng <= 2; ng++)" + T + "// [NAPCHIEU 09/09 b] " + TAG + " 1 = anh chieu (goi 95), 2 = anh than NPC (SetSprFile)",
        T*3 + "{",
        T*4 + 'Rep3Log("%s nap truoc (bat=%d): goi %u | da co %u, giao nen %u, khong %u | lan dung dau: kip %u, tre %u", ng == 1 ? "[NAPCHIEU]" : "[NAPNPC]", ng == 1 ? g_nRep3NapChieu : g_nRep3NapNpc,',
        T*5 + "g_uRep3NapTruoc[ng][0] + g_uRep3NapTruoc[ng][1] + g_uRep3NapTruoc[ng][2], g_uRep3NapTruoc[ng][1], g_uRep3NapTruoc[ng][2], g_uRep3NapTruoc[ng][0],",
        T*5 + "m_TextureResMgr.m_nNapTruocKip[ng], m_TextureResMgr.m_nNapTruocTre[ng]);",
        T*4 + "g_uRep3NapTruoc[ng][0] = g_uRep3NapTruoc[ng][1] = g_uRep3NapTruoc[ng][2] = 0; m_TextureResMgr.m_nNapTruocKip[ng] = 0; m_TextureResMgr.m_nNapTruocTre[ng] = 0;",
        T*3 + "}",
    ])
    s = rep_khoang(s, dau, cuoi, new, "SH.cpp log")
    ghi(F_SH_C, s, c0, l0, h0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")

# ================================================================ Core: KNpcRes.cpp - ham dung chung (client)
s, c0, l0, h0 = doc(F_NRES)
if TAG not in s:
    old = "BOOL" + T + "KNpcRes::Init(char *lpszNpcName, KNpcResList *pNpcResList)" + NL + "{"
    new = NL.join([
        "#ifndef _SERVER",
        "// " + TAG + " Goi Represent3 xep hang nap nen mot anh, TRUOC lan ve dau (nguon 1 = anh chieu tu goi 95, 2 = anh than NPC tu",
        "// KSprControl::SetSprFile). Tra GetProcAddress \"Rep3_NapTruoc2\" mot lan (thu toi da 8 lan neu DLL chua nap); thieu -> 0, im lang.",
        "// Represent3 khong bao gio nap dong bo vi loi goi nay; nap truoc chi la goi y (xem TextureResMgr::NapTruoc).",
        "int Rep3NapTruocAnh(const char* psz, int nNguon)",
        "{",
        T + "typedef int (*PFN_Rep3NapTruoc2)(const char*, int);",
        T + "static PFN_Rep3NapTruoc2 s_pfn = NULL;",
        T + "static int s_nTra = 0;",
        T + "if (!psz || !psz[0]) return 0;",
        T + "if (!s_pfn)",
        T + "{",
        T*2 + "if (s_nTra >= 8) return 0;",
        T*2 + "s_nTra++;",
        T*2 + "HMODULE h = GetModuleHandleA(\"Represent3.dll\");",
        T*2 + "if (h) s_pfn = (PFN_Rep3NapTruoc2)GetProcAddress(h, \"Rep3_NapTruoc2\");",
        T*2 + "if (!s_pfn) return 0;",
        T + "}",
        T + "return s_pfn(psz, nNguon);",
        "}",
        "#endif",
        "",
        old,
    ])
    s = rep(s, old, new, "NRES ham chung")
    ghi(F_NRES, s, c0, l0, h0, "KNpcRes.cpp")
else:
    print("KNpcRes.cpp da co")

# ================================================================ Core: KSprControl.cpp - goi khi gan ten anh
s, c0, l0, h0 = doc(F_SPR)
if TAG not in s:
    old = T + "m_dwNameID = g_FileName2Id(m_szName);"
    new = old + NL + T + "{ extern int Rep3NapTruocAnh(const char*, int); Rep3NapTruocAnh(m_szName, 2); }" + T + "// " + TAG + " xep hang nap nen ngay khi doi anh bo phan (them NPC / doi do / doi dong tac), truoc lan ve dau"
    s = rep(s, old, new, "SPR SetSprFile")
    ghi(F_SPR, s, c0, l0, h0, "KSprControl.cpp")
else:
    print("KSprControl.cpp da co")

# ================================================================ Core: KProtocolProcess.cpp - dung ham chung
s, c0, l0, h0 = doc(F_PP)
if TAG not in s:
    old = NL.join([
        T + "typedef int (*PFN_Rep3NapTruoc)(const char*);",
        T + "static PFN_Rep3NapTruoc s_pfn = NULL;",
        T + "static int s_nTra = 0;",
        T + "if (!s_pfn)",
        T + "{",
        T*2 + "if (s_nTra >= 8) return;" + T + "// Represent3.dll chua nap luc dau: thu lai vai lan roi thoi",
        T*2 + "s_nTra++;",
        T*2 + "HMODULE h = GetModuleHandleA(\"Represent3.dll\");",
        T*2 + "if (h) s_pfn = (PFN_Rep3NapTruoc)GetProcAddress(h, \"Rep3_NapTruoc\");",
        T*2 + "if (!s_pfn) return;",
        T + "}",
    ])
    new = T + "extern int Rep3NapTruocAnh(const char*, int);" + T + "// " + TAG + " dung chung (KNpcRes.cpp), nguon 1 = anh chieu"
    s = rep(s, old, new, "PP pfn")
    s = rep(s, T*2 + "if (psz[0]) s_pfn(psz);", T*2 + "if (psz[0]) Rep3NapTruocAnh(psz, 1);", "PP goi")
    ghi(F_PP, s, c0, l0, h0, "KProtocolProcess.cpp")
else:
    print("KProtocolProcess.cpp da co")
print("XONG NAPNPC")
