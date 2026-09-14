# -*- coding: utf-8 -*-
# [NENTRUOC 13/09] P3 nap truoc NEN DAT + [CACHE 13/09] ngan sach texture dien thoai. Chi JX_MOBILE, ban PC khong doi.
#
# DO 13/09 (script _d, [PGND-V] tren may ao; Fold 7 [PGND] 314-398 lan x 18 ms): 'ghep nen' mot vung ~66 o mat 11-32 ms,
# trong do GetImage 95-99 % (PrepareFrameData DONG BO: rut khung + giai ma + tao texture cho tung o), ghi lenh ve (RIO) 0-0,5 ms,
# doi dich ve 0,2-0,7 ms. Moi vung moi = mot khung roi (18 ms > 16,67).
#
# SUA: vung KE BEN / XA (khong phai vung chua nguoi choi) truoc khi ghep thi xin Represent3 chuan bi KHUNG cua tung o o luong nen
# (bo nap khung nen [VE 11/09] san co: JxNapKhungGiao nguon 3); moi khung Core hoi lai, khung nao con 'dang chuan bi' thi hoan;
# tat ca san (hoac qua 1,5 s) thi ghep - luc do GetImage chi tim, ghep het < 1 ms. Vung chua nguoi choi giu nguyen: ghep ngay.
# Core goi Represent3 qua GetProcAddress("Rep3_NenTruocKhung") nhu Rep3NapTruocAnh (libCoreClient.so khong lien ket libRepresent3.so).
#
# [CACHE 13/09]: Fold 7 'cache 508 MB, ngan sach 393' = kep theo GetAvailableTextureMem cua SDL_GPU = 1024 MB GIA (dien thoai RAM chung)
# -> CheckBalance moi 24 ms bo mot khung ('bo 101 632' / 50 phut), giai ma lai 20 khung/s, tai len GPU 4,6 MB/s. Sua: SDL_GPU khong kep
# theo VRAM (RAM/8 kep [128, 768], RAM/3 van kep tren), PressureByVram cung bo qua.
# Chay lai vo hai. iOS: dang ky ky hieu trong ios/JxIosMain.cpp.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[NENTRUOC 13/09]"
DAUC = "[CACHE 13/09]"
NL = "\r\n"


def doc(p, nl=NL):
    s = io.open(p, encoding="latin-1", newline="").read()
    assert (nl in s) if nl == NL else (NL not in s), p + " xuong dong khac mong doi"
    return s


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def cao(s):
    return sum(1 for c in s if ord(c) >= 128)


def thay(s, cu, moi, ten, n=1):
    if s.count(cu) != n:
        raise SystemExit("%s: neo '%s...' thay %d lan, can %d" % (ten, cu.strip()[:60], s.count(cu), n))
    return s.replace(cu, moi)


def va(p, viec, nl=NL):
    s = doc(p, nl)
    if DAU in s or DAUC in s:
        print("da va roi:", p)
        return
    c0 = cao(s)
    s = viec(s)
    assert cao(s) == c0, p + ": byte cao doi"
    ghi(p, s)
    print("da va:", p)


# ---------------------------------------------------------------- Represent3: TextureResMgr.h
def v_mgr_h(s):
    cu = "\tint NapTruoc(const char* pszImage, uint32 nType, int nNguon);\t// [NAPNPC 09/09] nNguon: 1 = anh chieu, 2 = anh than NPC" + NL
    moi = cu + "#ifdef JX_MOBILE" + NL + \
        "\t// " + DAU + " nen dat: chuan bi truoc KHUNG (rut + giai ma + tao texture) o luong nen. 1 = da san, 2 = dang chuan bi, 0 = khong (nguoi goi ghep dong bo)" + NL + \
        "\tint  JxNenTruocKhung(const char* pszImage, int nFrame);" + NL + \
        "\tvoid JxNenTruocXuLy();\t// dau khung (RepresentBegin): (id, khung) cho tep nap xong -> giao khung cho luong nen" + NL + \
        "\tstruct JxNenCho { uint32 uId; int nFrame; unsigned uLuc; };" + NL + \
        "\tvector<JxNenCho> m_jxNenCho;" + NL + \
        "#endif" + NL
    return thay(s, cu, moi, "TextureResMgr.h NapTruoc")


# ---------------------------------------------------------------- Represent3: TextureResMgr.cpp
def v_mgr_cpp(s):
    # a. ham moi sau NapTruoc
    cu = "\tm_TextureResList.insert(m_TextureResList.begin() + (-nIdx - 1), node);" + NL + "\treturn 2;" + NL + "}" + NL
    moi = cu + NL.join([
        "#ifdef JX_MOBILE",
        "// " + DAU + " Nen dat: chuan bi truoc KHUNG o luong nen cho vung ke ben / xa, TRUOC khi KScenePlaceRegionC::PrerenderGround ghep.",
        "// Do 13/09 (may ao [PGND-V], Fold 7 [PGND] 18 ms/vung): 'ghep nen' nam gan het trong GetImage -> PrepareFrameData DONG BO (rut khung +",
        "// giai ma + tao texture) cho ~66 o; ghi lenh ve chi 0,5 ms. Tra 1 = khung da san, 2 = dang chuan bi (tep hoac khung o luong nen),",
        "// 0 = khong duoc (khong co luong nen / muc hong): nguoi goi ghep dong bo nhu cu. KHONG BAO GIO nap dong bo trong ham nay.",
        "int TextureResMgr::JxNenTruocKhung(const char* pszImage, int nFrame)",
        "{",
        "\tif (!pszImage || !pszImage[0] || !g_nRep3NapNen || g_nJxNapKhungNen <= 0 || nFrame < 0)",
        "\t\treturn 0;",
        "\tconst uint32 uId = g_FileName2Id((LPSTR)pszImage);",
        "\tbool bChoTep = false;",
        "\t{",
        "\t\tKAutoCriticalSection AutoLock(m_ImageProcessLock);",
        "\t\tconst int nIdx = FindImage(uId, 0);",
        "\t\tif (nIdx >= 0)",
        "\t\t{",
        "\t\t\tResNode& node = m_TextureResList[nIdx];",
        "\t\t\tif (node.m_nType != ISI_T_SPR)",
        "\t\t\t\treturn 0;",
        "\t\t\tnode.m_nLastUsedTime = GetTickCount();\t// giu khoi bi don trong luc cho",
        "\t\t\tif (node.m_bDangNap)",
        "\t\t\t\tbChoTep = true;\t// tep dang nap o luong nen: nho (id, khung), JxNenTruocXuLy giao khung khi tep xong",
        "\t\t\telse",
        "\t\t\t{",
        "\t\t\t\tTextureResSpr* p = (TextureResSpr*)node.m_pTextureRes;",
        "\t\t\t\tif (!p || !p->m_pFrameInfo || nFrame >= p->m_nFrameNum)",
        "\t\t\t\t\treturn 0;\t// muc nap hong (GetImage tu thu lai theo lich rieng) / khung ngoai tam",
        "\t\t\t\tif (p->m_pFrameInfo[nFrame].texInfo[0].pTexture)",
        "\t\t\t\t\treturn 1;",
        "\t\t\t\tif (p->m_pFrameInfo[nFrame].nJxNen == 1)",
        "\t\t\t\t\treturn 2;",
        "\t\t\t\treturn p->JxNapKhungGiao(nFrame, 3) ? 2 : 0;",
        "\t\t\t}",
        "\t\t}",
        "\t}",
        "\tif (!bChoTep && NapTruoc(pszImage, ISI_T_SPR, 3) == 0)\t// chua co muc: nap tep o luong nen (hang SAU), khong nap dong bo",
        "\t\treturn 0;",
        "\t{",
        "\t\tKAutoCriticalSection AutoLock(m_ImageProcessLock);",
        "\t\tif (m_jxNenCho.size() < 4096)",
        "\t\t{",
        "\t\t\tJxNenCho c; c.uId = uId; c.nFrame = nFrame; c.uLuc = (unsigned)timeGetTime();",
        "\t\t\tm_jxNenCho.push_back(c);",
        "\t\t}",
        "\t}",
        "\treturn 2;",
        "}",
        "",
        "// " + DAU + " Luong ve, dau khung (sau JxNapKhungNhan): muc cho tep -> tep da xong thi giao khung; qua 3 s thi bo (Core tu ghep dong bo).",
        "void TextureResMgr::JxNenTruocXuLy()",
        "{",
        "\tif (m_jxNenCho.empty())",
        "\t\treturn;",
        "\tKAutoCriticalSection AutoLock(m_ImageProcessLock);",
        "\tconst unsigned uNow = (unsigned)timeGetTime();",
        "\tsize_t nGiu = 0; int nLam = 0;",
        "\tfor (size_t i = 0; i < m_jxNenCho.size(); i++)",
        "\t{",
        "\t\tJxNenCho c = m_jxNenCho[i];",
        "\t\tbool bGiu = false;",
        "\t\tif (uNow - c.uLuc > 3000)",
        "\t\t\tbGiu = false;",
        "\t\telse if (nLam >= 256)",
        "\t\t\tbGiu = true;\t// du viec khung nay: khung sau",
        "\t\telse",
        "\t\t{",
        "\t\t\tconst int nIdx = FindImage(c.uId, 0);",
        "\t\t\tif (nIdx >= 0 && m_TextureResList[nIdx].m_nType == ISI_T_SPR)",
        "\t\t\t{",
        "\t\t\t\tif (m_TextureResList[nIdx].m_bDangNap)",
        "\t\t\t\t\tbGiu = true;",
        "\t\t\t\telse",
        "\t\t\t\t{",
        "\t\t\t\t\tTextureResSpr* p = (TextureResSpr*)m_TextureResList[nIdx].m_pTextureRes;",
        "\t\t\t\t\tif (p && p->m_pFrameInfo && c.nFrame >= 0 && c.nFrame < p->m_nFrameNum && !p->m_pFrameInfo[c.nFrame].texInfo[0].pTexture && p->m_pFrameInfo[c.nFrame].nJxNen == 0)",
        "\t\t\t\t\t\tp->JxNapKhungGiao(c.nFrame, 3);",
        "\t\t\t\t\tnLam++;",
        "\t\t\t\t}",
        "\t\t\t}",
        "\t\t}",
        "\t\tif (bGiu)",
        "\t\t\tm_jxNenCho[nGiu++] = c;",
        "\t}",
        "\tm_jxNenCho.resize(nGiu);",
        "}",
        "#endif",
        "",
    ])
    s = thay(s, cu, moi, "TextureResMgr.cpp sau NapTruoc")
    # b. CapBudgetByVram: SDL_GPU dien thoai khong co VRAM rieng
    cu = ("\tif (g_nRep3CacheMB > 0 || uVramFreeMB == 0)" + NL + "\t\treturn;" + NL
          + "\tunsigned __int64 uBudgetMB = ((unsigned __int64)(uint32)m_nBalanceNum) >> 20;" + NL
          + "\t// [REP3 08/09 q] chia cho so client dang mo va he so trang atlas (1,3); san 256 MB (khung TK nang ~200 MB) hoac VRAM/2" + NL)
    moi = ("#ifdef JX_MOBILE" + NL
           + "\tif (g_nRep3ApiOn == 100)" + NL
           + "\t{\t// " + DAUC + " dien thoai: SDL_GPU khong co VRAM rieng, GetAvailableTextureMem = 1024 MB GIA -> kep 512 -> 393 MB trong khi Tong Kim" + NL
           + "\t\t// dung 508 MB (Fold 7 13/09): CheckBalance moi 24 ms bo mot khung ('bo 101 632' / 50 phut), giai ma lai 20 khung/s, tai len GPU 4,6 MB/s." + NL
           + "\t\t// Ngan sach RAM/8 kep [128, 768] va tran RAM/3 (SetBudget) la du; khong kep theo con so gia nay." + NL
           + "\t\tRep3Log(\"[REP3] cache texture: SDL_GPU dien thoai -> khong kep theo VRAM (VRAM %llu MB la so gia), giu ngan sach %llu MB\", uVramFreeMB, ((unsigned __int64)(uint32)m_nBalanceNum) >> 20);" + NL
           + "\t\treturn;" + NL
           + "\t}" + NL
           + "#endif" + NL + cu)
    s = thay(s, cu, moi, "CapBudgetByVram")
    # c. PressureByVram: bo qua tren dien thoai
    cu = "\tif (g_nRep3CacheMB > 0 || uVramFreeMB == 0 || uVramFreeMB >= 128)" + NL + "\t\treturn;" + NL
    moi = "#ifdef JX_MOBILE" + NL + "\tif (g_nRep3ApiOn == 100)" + NL + "\t\treturn;\t// " + DAUC + " VRAM cua SDL_GPU la so gia (1024 - texture da tao), khong phai ap luc bo nho that" + NL + "#endif" + NL + cu
    s = thay(s, cu, moi, "PressureByVram")
    # d. SetBudget: tran 512 -> 768 (khoi JX_POSIX)
    cu = "\t\t\tif (uBudgetMB > 512) uBudgetMB = 512;" + NL
    moi = "\t\t\tif (uBudgetMB > 768) uBudgetMB = 768;\t// " + DAUC + " 512 -> 768: Fold 7 Tong Kim dung 508-512 MB (8 khoi atlas); may 4-5 GB van la RAM/8 = 512; tran RAM/3 van kep" + NL
    s = thay(s, cu, moi, "SetBudget 512")
    return s


# ---------------------------------------------------------------- Represent3: KRepresentShell3.h/.cpp
def v_shell_h(s):
    cu = "    int NapTruoc(const char* pszImage, int nNguon);\t// [NAPNPC 09/09] nNguon: 1 = anh chieu (Rep3NapChieu), 2 = anh than NPC (Rep3NapNpc)" + NL
    moi = cu + "#ifdef JX_MOBILE" + NL + "    int JxNenTruocKhung(const char* pszImage, int nFrame);\t// " + DAU + " nen dat: chuan bi khung o luong nen (Core goi qua Rep3_NenTruocKhung)" + NL + "#endif" + NL
    return thay(s, cu, moi, "KRepresentShell3.h NapTruoc")


def v_shell_cpp(s):
    cu = ("extern \"C\" __declspec(dllexport)" + NL + "int Rep3_NapTruoc2(const char* pszImage, int nNguon)\t// [NAPNPC 09/09]" + NL + "{" + NL
          + "\tif (!g_pRep3ShellDuyNhat || !pszImage)" + NL + "\t\treturn 0;" + NL + "\treturn g_pRep3ShellDuyNhat->NapTruoc(pszImage, nNguon);" + NL + "}" + NL)
    moi = cu + NL.join([
        "#ifdef JX_MOBILE",
        "// " + DAU + " nen dat: Core (KScenePlaceRegionC) goi qua GetProcAddress(\"Rep3_NenTruocKhung\") - chuan bi khung o nen o luong nen truoc khi ghep vung.",
        "int KRepresentShell3::JxNenTruocKhung(const char* pszImage, int nFrame)",
        "{",
        "\treturn m_TextureResMgr.JxNenTruocKhung(pszImage, nFrame);",
        "}",
        "extern \"C\" __declspec(dllexport)",
        "int Rep3_NenTruocKhung(const char* pszImage, int nFrame)\t// " + DAU,
        "{",
        "\tif (!g_pRep3ShellDuyNhat || !pszImage)",
        "\t\treturn 0;",
        "\treturn g_pRep3ShellDuyNhat->JxNenTruocKhung(pszImage, nFrame);",
        "}",
        "#endif",
        "",
    ])
    s = thay(s, cu, moi, "Rep3_NapTruoc2")
    cu = "\tm_TextureResMgr.JxNapKhungNhan();\t// [VE 11/09] tao texture tu khung da giai ma o luong nen (theo ngan sach NapKhungApMs)" + NL
    moi = cu + "\tm_TextureResMgr.JxNenTruocXuLy();\t// " + DAU + " nen dat: muc cho tep -> giao khung" + NL
    s = thay(s, cu, moi, "RepresentBegin JxNapKhungNhan")
    return s


# ---------------------------------------------------------------- Core: KScenePlaceRegionC.h/.cpp
def v_region_h(s):
    cu = "\tbool PrerenderGround(bool bForce);" + NL
    moi = cu + "#ifdef JX_MOBILE" + NL + \
        "\tbool JxNenChuaSan();\t// " + DAU + " vung ke ben/xa: xin luong nen chuan bi khung cac o; true = con dang chuan bi (hoan ghep sang khung sau), qua 1,5 s -> false" + NL + \
        "\tint  JxNenTruoc();\t\t// xin/hoi tung o (grunode + object); tra so o con 'dang chuan bi'" + NL + \
        "\tunsigned m_uJxNenXinLuc;\t// timeGetTime luc xin lan dau (0 = chua)" + NL + \
        "#endif" + NL
    return thay(s, cu, moi, "KScenePlaceRegionC.h PrerenderGround")


def v_region_cpp(s):
    # khoi tao
    cu = "\tm_pPrerenderGroundImg = NULL;" + NL + NL + "\tmemset(m_TrapInfo, 0, sizeof(m_TrapInfo));" + NL
    moi = "\tm_pPrerenderGroundImg = NULL;" + NL + "#ifdef JX_MOBILE" + NL + "\tm_uJxNenXinLuc = 0;\t// " + DAU + NL + "#endif" + NL + NL + "\tmemset(m_TrapInfo, 0, sizeof(m_TrapInfo));" + NL
    s = thay(s, cu, moi, "khoi tao")
    # dat lai khi anh nen bi bo / can ve lai (2 cho)
    cu = "\t\tm_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = false;" + NL + "\t\tm_pPrerenderGroundImg = NULL;" + NL
    moi = "\t\tm_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = false;" + NL + "#ifdef JX_MOBILE" + NL + "\t\tm_uJxNenXinLuc = 0;\t// " + DAU + " bo anh nen -> xin lai" + NL + "#endif" + NL + "\t\tm_pPrerenderGroundImg = NULL;" + NL
    s = thay(s, cu, moi, "bo anh nen")
    cu = "\tif (m_pPrerenderGroundImg)" + NL + "\t\tm_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = false;" + NL + "}" + NL
    moi = "\tif (m_pPrerenderGroundImg)" + NL + "\t\tm_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = false;" + NL + "#ifdef JX_MOBILE" + NL + "\tm_uJxNenXinLuc = 0;\t// " + DAU + " ve lai -> xin lai" + NL + "#endif" + NL + "}" + NL
    s = thay(s, cu, moi, "ve lai")
    cu = "\tmemset(&m_BiosData, 0, sizeof(KBiosData));" + NL + "\tm_Status = REGION_S_STANDBY;" + NL
    moi = "\tmemset(&m_BiosData, 0, sizeof(KBiosData));" + NL + "#ifdef JX_MOBILE" + NL + "\tm_uJxNenXinLuc = 0;\t// " + DAU + NL + "#endif" + NL + "\tm_Status = REGION_S_STANDBY;" + NL
    s = thay(s, cu, moi, "STANDBY")
    # ham moi sau PrerenderGround
    cu = "\t\tJX_NEN_VE(nNum);\t// [GOMNEN 12/09]" + NL + "\t}" + NL + "\treturn true;" + NL + "}" + NL
    moi = cu + NL.join([
        "#ifdef JX_MOBILE",
        "// " + DAU + " Core goi Represent3 (libRepresent3.so nap bang dlopen) qua GetProcAddress nhu Rep3NapTruocAnh (KNpcRes.cpp): thieu -> 0, im lang.",
        "// (tep nay chi co trong ban client; rao #ifdef don de ios/kiem_android_tuongduong.py so duoc tung dong)",
        "static int Rep3NenTruocKhung(const char* psz, int nFrame)",
        "{",
        "\ttypedef int (*PFN_NenTruocKhung)(const char*, int);",
        "\tstatic PFN_NenTruocKhung s_pfn = NULL;",
        "\tstatic int s_nTra = 0;",
        "\tif (!psz || !psz[0]) return 0;",
        "\tif (!s_pfn)",
        "\t{",
        "\t\tif (s_nTra >= 8) return 0;",
        "\t\ts_nTra++;",
        "\t\tHMODULE h = GetModuleHandleA(\"Represent3.dll\");",
        "\t\tif (h) s_pfn = (PFN_NenTruocKhung)GetProcAddress(h, \"Rep3_NenTruocKhung\");",
        "\t\tif (!s_pfn) return 0;",
        "\t}",
        "\treturn s_pfn(psz, nFrame);",
        "}",
        "",
        "// " + DAU + " Xin / hoi tung o cua vung (cung danh sach ma PrerenderGround ghep). Tra so o con 'dang chuan bi' (2); 0 (khong duoc) coi nhu san.",
        "int KScenePlaceRegionC::JxNenTruoc()",
        "{",
        "\tint nCho = 0;",
        "\tchar szTen[MAX_PATH];",
        "\tKSPRCrunode* pGrunode = m_GroundLayerData.pGrunodes;",
        "\tfor (unsigned int nIndex = 0; pGrunode && nIndex < m_GroundLayerData.uNumGrunode; nIndex++)",
        "\t{",
        "\t\tint nLen = (int)pGrunode->Param.nFileNameLen;",
        "\t\tif (nLen >= MAX_PATH) nLen = MAX_PATH - 1;",
        "\t\tif (nLen > 0) { memcpy(szTen, pGrunode->szImgName, nLen); szTen[nLen] = 0; if (Rep3NenTruocKhung(szTen, pGrunode->Param.nFrame) == 2) nCho++; }",
        "\t\tpGrunode = (KSPRCrunode*)(((char*)pGrunode) + sizeof(KSPRCrunode::KSPRCrunodeParam) + pGrunode->Param.nFileNameLen);",
        "\t}",
        "\tKSPRCoverGroundObj* pObj = m_GroundLayerData.pObjects;",
        "\tfor (unsigned int nIndex = 0; pObj && nIndex < m_GroundLayerData.uNumObject; nIndex++, pObj++)",
        "\t\tif (Rep3NenTruocKhung(pObj->szImage, pObj->nFrame) == 2) nCho++;",
        "\treturn nCho;",
        "}",
        "",
        "// " + DAU + " Vung ke ben / xa: lan dau xin luong nen chuan bi khung cac o; con o dang chuan bi -> true (hoan ghep, khung sau hoi lai);",
        "// tat ca san -> false (ghep, < 1 ms); qua 1,5 s ke tu lan xin dau -> false (ghep dong bo nhu cu, khong bao gio de vung trong qua lau).",
        "bool KScenePlaceRegionC::JxNenChuaSan()",
        "{",
        "\tif (g_pRepresent == NULL || m_pPrerenderGroundImg == NULL || m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG)",
        "\t\treturn false;",
        "\tconst unsigned uNow = (unsigned)timeGetTime();",
        "\tif (m_uJxNenXinLuc == 0)",
        "\t\tm_uJxNenXinLuc = uNow ? uNow : 1;",
        "\telse if (uNow - m_uJxNenXinLuc > 1500)",
        "\t\treturn false;",
        "\treturn JxNenTruoc() > 0;",
        "}",
        "#endif",
        "",
    ])
    s = thay(s, cu, moi, "sau PrerenderGround")
    return s


# ---------------------------------------------------------------- Core: KScenePlaceC.cpp (bo lich ba nhanh)
def v_place_cpp(s):
    cu = ("\t\t\tif (timeGetTime() - dwPgT0 < dwPgBudgetMs)" + NL
          + "\t\t\t\tJX_NEN_DO(1, m_pInProcessAreaRegions[i]->PrerenderGround(false));\t// [NENDAT 11/09]" + NL
          + "\t\t\telse" + NL + "\t\t\t\tnDeferred++;" + NL)
    moi = ("\t\t\tif (timeGetTime() - dwPgT0 < dwPgBudgetMs)" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\t\t{\t// " + DAU + " khung cac o dang duoc luong nen chuan bi -> hoan, khung sau ghep (nhanh)" + NL
           + "\t\t\t\tif (m_pInProcessAreaRegions[i]->JxNenChuaSan()) { nDeferred++; continue; }" + NL
           + "\t\t\t\tJX_NEN_DO(1, m_pInProcessAreaRegions[i]->PrerenderGround(false));\t// [NENDAT 11/09]" + NL
           + "\t\t\t}" + NL
           + "#else" + NL
           + "\t\t\t\tJX_NEN_DO(1, m_pInProcessAreaRegions[i]->PrerenderGround(false));\t// [NENDAT 11/09]" + NL
           + "#endif" + NL
           + "\t\t\telse" + NL + "\t\t\t\tnDeferred++;" + NL)
    s = thay(s, cu, moi, "nhanh 1")
    cu = "\t\t\t\tbool bJxOk = false;\t// [NENDAT 11/09]" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "\t\t\t\tif (m_pInProcessAreaRegions[i]->JxNenChuaSan()) { nDeferred++; continue; }\t// " + DAU + NL
           + "#endif" + NL + cu)
    s = thay(s, cu, moi, "nhanh 2")
    return s


# ---------------------------------------------------------------- iOS: bang ky hieu tinh
def v_ios(s):
    cu = "extern \"C\" int             Rep3_JxTheGioi(int, int);          // [TG 13/09] Represent3/KRepresentShell3.cpp (Wnds.cpp goi qua GetProcAddress)\n"
    moi = cu + "extern \"C\" int             Rep3_NenTruocKhung(const char*, int); // " + DAU + " Represent3/KRepresentShell3.cpp (KScenePlaceRegionC goi qua GetProcAddress)\n"
    s = thay(s, cu, moi, "ios extern")
    cu = "\tJxPosix_DangKyKyHieu(\"Represent3.dll\", \"Rep3_JxTheGioi\",       (void*)&Rep3_JxTheGioi);       // [TG 13/09]\n"
    moi = cu + "\tJxPosix_DangKyKyHieu(\"Represent3.dll\", \"Rep3_NenTruocKhung\",   (void*)&Rep3_NenTruocKhung);   // " + DAU + "\n"
    s = thay(s, cu, moi, "ios dang ky")
    return s


va("Sources/Represent/Represent3/TextureResMgr.h", v_mgr_h)
va("Sources/Represent/Represent3/TextureResMgr.cpp", v_mgr_cpp)
va("Sources/Represent/Represent3/KRepresentShell3.h", v_shell_h)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", v_shell_cpp)
va("Sources/Core/Src/Scene/KScenePlaceRegionC.h", v_region_h)
va("Sources/Core/Src/Scene/KScenePlaceRegionC.cpp", v_region_cpp)
va("Sources/Core/Src/Scene/KScenePlaceC.cpp", v_place_cpp)
va("ios/JxIosMain.cpp", v_ios, nl="\n")
