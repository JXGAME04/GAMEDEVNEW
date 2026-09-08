# -*- coding: utf-8 -*-
"""goi_va_nap2_luong_nen_0809.py - [NAP 08/09 b] NAP TAI NGUYEN O LUONG NEN (viec #1 KEHOACH_D3D11_TIEP_0809.md).
Do [REP3-NAP] 16:3x: khung nang nhat 38-75 ms = mot khung phai doc/giai nen hang chuc tep sprite (0,3-3,7 ms/tep) + jpeg nen 10 ms/anh.
Thiet ke: GetImage gap anh CHUA NAP khi dang ve (giua RepresentBegin/End) -> chen muc 'dang nap' + giao viec cho luong nen, tra NULL
(anh do khong ve khung nay, hien o khung sau); luong nen doc pak + giai ma (SPR: LoadSprFile - ZCache/XPackFile co khoa san; JPEG:
get_jpg_image vao m_pJpgCho), KHONG dung device; luong ve nhan ket qua o RepresentBegin (NapNenNhan): SPR gan vao muc, BMP tao texture
(LoadJpegFinish). Hoi dong bo (GetImageParam/FrameParam/PixelAlpha, ngoai luc ve) van nap ngay nhu cu. Cong lui [Client] Rep3NapNen=0.
Tep: TextureRes.h/.cpp, TextureResMgr.h/.cpp, KRepresentShell3.cpp, BaseInclude.h (latin-1 CRLF, giu nguyen chu thich GBK)."""
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
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:90], c, n)); sys.exit(1)
    return s.replace(old, new)
def hi(s): return sum(1 for ch in s if ord(ch) >= 0x80)

# ============ BaseInclude.h: ini
p, s = load("BaseInclude.h"); h0 = hi(s)
if "g_nRep3NapNen" not in s:
    s = rep(s, "extern int  g_nRep3StatSec;", "extern int  g_nRep3NapNen;\t// [NAP 08/09 b] [Client] Rep3NapNen: 1 = nap sprite/jpeg o luong nen (mac dinh), 0 = nap ngay tren luong ve nhu cu\r\nextern int  g_nRep3StatSec;")
save(p, s, h0); print("OK BaseInclude.h")

# ============ TextureRes.h
p, s = load("TextureRes.h"); h0 = hi(s)
if "NapNenHoanTat" not in s:
    s = rep(s, "class TextureRes\r\n{\r\n", "struct KSGImageContent;\t// [NAP 08/09 b] KPakFile.h (anh JPEG da giai ma)\r\n\r\nclass TextureRes\r\n{\r\n")
    s = rep(s, "\tvirtual bool ReleaseAFrameData() = 0;\r\n\r\n\tint32 GetWidth(){ return (int32)m_nWidth; }\r\n",
        "\tvirtual bool ReleaseAFrameData() = 0;\r\n\r\n"
        "\t// [NAP 08/09 b] luong ve goi sau khi luong nen nap xong (BMP: tao texture tu anh da giai ma); false = hong\r\n"
        "\tvirtual bool NapNenHoanTat() { return true; }\r\n\r\n"
        "\tint32 GetWidth(){ return (int32)m_nWidth; }\r\n")
    s = rep(s, "\tLPDIRECT3DTEXTURE9\tm_pSysMemTexture;", "\tKSGImageContent*\tm_pJpgCho;\t\t// [NAP 08/09 b] JPEG da giai ma o luong nen, cho luong ve tao texture (LoadJpegFinish)\r\n\tLPDIRECT3DTEXTURE9\tm_pSysMemTexture;")
    s = rep(s, "private:\r\n\tbool LoadJpegFile(char* szImage);\r\n",
        "private:\r\n\tbool LoadJpegFile(char* szImage);\r\n"
        "public:\r\n"
        "\tbool LoadJpegDecode(char* szImage);\t// [NAP 08/09 b] luong nen: chi giai ma JPEG vao m_pJpgCho (khong dung device)\r\n"
        "\tbool LoadJpegFinish();\t\t\t\t// luong ve: tao texture tu m_pJpgCho\r\n"
        "\tvirtual bool NapNenHoanTat() { return LoadJpegFinish(); }\r\n")
save(p, s, h0); print("OK TextureRes.h")

# ============ TextureRes.cpp
p, s = load("TextureRes.cpp"); h0 = hi(s)
if "LoadJpegDecode" not in s:
    s = rep(s, "\tm_FrameInfo.texInfo[0].pTexture = NULL;\r\n\tm_pSysMemTexture = NULL;\r\n\tResetVar();\r\n}\r\n",
        "\tm_FrameInfo.texInfo[0].pTexture = NULL;\r\n\tm_pSysMemTexture = NULL;\r\n\tm_pJpgCho = NULL;\t// [NAP 08/09 b]\r\n\tResetVar();\r\n}\r\n")
    s = rep(s, "void TextureResBmp::Release()\r\n{\r\n\tSAFE_RELEASE(m_FrameInfo.texInfo[0].pTexture);\r\n\tSAFE_RELEASE(m_pSysMemTexture);\r\n",
        "void TextureResBmp::Release()\r\n{\r\n\tSAFE_RELEASE(m_FrameInfo.texInfo[0].pTexture);\r\n\tSAFE_RELEASE(m_pSysMemTexture);\r\n"
        "\tif (m_pJpgCho) { release_image(m_pJpgCho); m_pJpgCho = NULL; }\t// [NAP 08/09 b]\r\n")
    a = s.find("bool TextureResBmp::LoadJpegFile(char* szImage)\r\n{\r\n")
    end_marker = "error:\r\n\trelease_image(pImageContent);\r\n\tRelease();\r\n\treturn false;\r\n}\r\n"
    b = s.find(end_marker, a)
    if a < 0 or b < 0: print("FAIL: khong thay LoadJpegFile"); sys.exit(1)
    b += len(end_marker)
    new_fn = (
        "// [NAP 08/09 b] LoadJpegFile tach lam hai: LoadJpegDecode (luong nen, chi giai ma - get_jpg_image doc pak co khoa) + LoadJpegFinish\r\n"
        "// (luong ve, tao texture). Giai ma JPEG khoa rieng vi bo giai ma co the khong reentrant khi luong ve hoi dong bo cung luc.\r\n"
        "static CRITICAL_SECTION* Rep3JpgKhoa()\r\n{\r\n"
        "\tstatic CRITICAL_SECTION s_cs; static bool s_bInit = false;\r\n"
        "\tif (!s_bInit) { InitializeCriticalSection(&s_cs); s_bInit = true; }\r\n"
        "\treturn &s_cs;\r\n}\r\n"
        "bool TextureResBmp::LoadJpegDecode(char* szImage)\r\n{\r\n"
        "\tif (m_pJpgCho) { release_image(m_pJpgCho); m_pJpgCho = NULL; }\r\n"
        "\tEnterCriticalSection(Rep3JpgKhoa());\r\n"
        "\tif(g_16BitFormat == D3DFMT_R5G6B5)\r\n\t\tm_pJpgCho = get_jpg_image(szImage, RGB_565);\r\n"
        "\telse\r\n\t\tm_pJpgCho = get_jpg_image(szImage, RGB_555);\r\n"
        "\tLeaveCriticalSection(Rep3JpgKhoa());\r\n"
        "\tif(!m_pJpgCho)\r\n\t\treturn false;\r\n"
        "\tm_nWidth = m_pJpgCho->nWidth;\r\n\tm_nHeight = m_pJpgCho->nHeight;\r\n"
        "\tm_FrameInfo.nWidth = m_pJpgCho->nWidth;\r\n\tm_FrameInfo.nHeight = m_pJpgCho->nHeight;\r\n\tm_FrameInfo.nTexNum = 1;\r\n"
        "\tm_FrameInfo.texInfo[0].nWidth = FitTextureSize(m_nWidth);\r\n\tm_FrameInfo.texInfo[0].nHeight = FitTextureSize(m_nHeight);\r\n"
        "\tif(m_FrameInfo.texInfo[0].nWidth == 0 || m_FrameInfo.texInfo[0].nHeight == 0)\r\n"
        "\t{\r\n\t\trelease_image(m_pJpgCho); m_pJpgCho = NULL;\r\n\t\treturn false;\r\n\t}\r\n"
        "\treturn true;\r\n}\r\n\r\n"
        "bool TextureResBmp::LoadJpegFinish()\r\n{\r\n"
        "\tKSGImageContent *pImageContent;\r\n\tint i;\r\n\tBYTE *pDes;\r\n\tBYTE *pSrc;\r\n\tD3DLOCKED_RECT LockedRect;\r\n"
        "\tif (!m_pJpgCho)\r\n\t\treturn m_FrameInfo.texInfo[0].pTexture != NULL;\r\n"
        "\tpImageContent = m_pJpgCho;\r\n\tm_pJpgCho = NULL;\r\n"
        "\tif (FAILED(Rep3CreateTex(PD3DDEVICE, m_FrameInfo.texInfo[0].nWidth, m_FrameInfo.texInfo[0].nHeight, 1,\r\n"
        "\t\t\t\t\t\t\t\tD3DUSAGE_RENDERTARGET, g_16BitFormat, D3DPOOL_DEFAULT, &m_FrameInfo.texInfo[0].pTexture)))\r\n\t\tgoto error;\r\n"
        "\tif (FAILED(Rep3CreateTex(PD3DDEVICE, m_FrameInfo.texInfo[0].nWidth, m_FrameInfo.texInfo[0].nHeight, 1,\r\n"
        "\t\t\t\t\t\t\t\t0, g_16BitFormat, D3DPOOL_SYSTEMMEM, &m_pSysMemTexture)))\r\n\t\tgoto error;\r\n"
        "\tif (FAILED(m_pSysMemTexture->LockRect(0, &LockedRect, NULL, 0)))\r\n\t\tgoto error;\r\n"
        "\tpDes = (BYTE*)LockedRect.pBits;\r\n\tpSrc = (BYTE*)pImageContent->Data;\r\n"
        "\tfor(i=0; i<m_nHeight; i++)\r\n\t{\r\n\t\tmemcpy(pDes, pSrc, m_nWidth * 2);\r\n\t\tpSrc += m_nWidth * 2;\r\n\t\tpDes += LockedRect.Pitch;\r\n\t}\r\n"
        "\trelease_image(pImageContent);\r\n\tm_pSysMemTexture->UnlockRect(0);\r\n"
        "\tPD3DDEVICE->UpdateTexture(m_pSysMemTexture, m_FrameInfo.texInfo[0].pTexture);\r\n"
        "\tm_nTexMemUsed = m_FrameInfo.texInfo[0].nWidth * m_FrameInfo.texInfo[0].nHeight * 2;\r\n\treturn true;\r\n\r\n"
        "error:\r\n\trelease_image(pImageContent);\r\n\tRelease();\r\n\treturn false;\r\n}\r\n\r\n"
        "bool TextureResBmp::LoadJpegFile(char* szImage)\r\n{\r\n\treturn LoadJpegDecode(szImage) && LoadJpegFinish();\r\n}\r\n")
    h0 = h0 - hi(s[a:b]) + hi(new_fn)   # ham cu co chu thich GBK bi thay ca ham -> dieu chinh so byte cao ky vong
    s = s[:a] + new_fn + s[b:]
save(p, s, h0); print("OK TextureRes.cpp")

# ============ TextureResMgr.h
p, s = load("TextureResMgr.h"); h0 = hi(s)
if "NapNenNhan" not in s:
    s = rep(s, "struct ResNode\r\n{\r\n", "struct ResNode\r\n{\r\n\tResNode() : m_bDangNap(false) {}\t// [NAP 08/09 b]\r\n\tbool\t\tm_bDangNap;\t\t\t\t// [NAP 08/09 b] dang nap o luong nen (m_pTextureRes NULL tam thoi)\r\n")
    s = rep(s, "    void GetStat(uint32& uNodes, uint32& uTexMB, uint32& uRawMB, uint32& uDrawMB, uint32& uBudgetMB);\r\n",
        "    void GetStat(uint32& uNodes, uint32& uTexMB, uint32& uRawMB, uint32& uDrawMB, uint32& uBudgetMB);\r\n"
        "\t// [NAP 08/09 b] NAP O LUONG NEN: GetImage gap anh chua nap khi dang ve -> giao viec, tra NULL (anh do bo ve khung nay, hien o khung sau);\r\n"
        "\t// luong nen doc pak + giai ma (SPR: LoadSprFile; JPEG: LoadJpegDecode), khong dung device; luong ve nhan ket qua o RepresentBegin.\r\n"
        "\tstruct NapViec   { char szTen[MAX_PATH]; uint32 uId; uint32 nType; };\r\n"
        "\tstruct NapKetQua { char szTen[MAX_PATH]; uint32 uId; uint32 nType; TextureRes* pRes; };\r\n"
        "\tvoid NapNenNhan();\t\t\t// luong ve, dau moi khung: nhan ket qua luong nen\r\n"
        "\tvoid NapNenDung();\t\t\t// dung luong nen, bo viec/ket qua con lai (Free)\r\n"
        "\tbool m_bVeDangDien;\t\t\t// true giua RepresentBegin/End: cho phep giao viec cho luong nen\r\n"
        "\tunsigned m_nNapNenGui, m_nNapNenXong, m_nNapNenHong, m_nNapNenBoVe;\t// thong ke ky ([REP3-NAP])\r\n"
        "private:\r\n"
        "\tbool NapNenGiao(const char* pszImage, uint32 uId, uint32 nType);\t// false = khong tao duoc luong -> nap ngay\r\n"
        "\tstatic unsigned __stdcall NapNenLuong(void* p);\r\n"
        "\tvoid NapNenChay();\r\n"
        "\tvector<NapViec> m_napViec; vector<NapKetQua> m_napXong; KCriticalSection m_napKhoa; HANDLE m_hNapLuong; HANDLE m_hNapCo; volatile long m_lNapDung; bool m_bNapNenLoi;\r\n"
        "public:\r\n")
save(p, s, h0); print("OK TextureResMgr.h")

# ============ TextureResMgr.cpp
p, s = load("TextureResMgr.cpp"); h0 = hi(s)
if "NapNenNhan" not in s:
    s = rep(s, "#include <tlhelp32.h>\t// [REP3 08/09 q] dem so client dang mo\r\n", "#include <tlhelp32.h>\t// [REP3 08/09 q] dem so client dang mo\r\n#include <process.h>\t// [NAP 08/09 b] _beginthreadex\r\n")
    s = rep(s, "\tm_uBudgetFloorMB = 60;\t// [REP3 08/09 q]\r\n",
        "\tm_uBudgetFloorMB = 60;\t// [REP3 08/09 q]\r\n"
        "\tm_bVeDangDien = false; m_nNapNenGui = 0; m_nNapNenXong = 0; m_nNapNenHong = 0; m_nNapNenBoVe = 0;\t// [NAP 08/09 b]\r\n"
        "\tm_hNapLuong = NULL; m_hNapCo = NULL; m_lNapDung = 0; m_bNapNenLoi = false;\r\n")
    s = rep(s, "void TextureResMgr::Free()\r\n{\r\n    KAutoCriticalSection AutoLock(m_ImageProcessLock);\r\n",
        "void TextureResMgr::Free()\r\n{\r\n    KAutoCriticalSection AutoLock(m_ImageProcessLock);\r\n\tNapNenDung();\t// [NAP 08/09 b]\r\n")
    # GetImage: muc dang nap
    s = rep(s, "\t\tif (m_TextureResList[nImagePosition].m_nType == nType)\r\n\t\t{\r\n\t\t\tm_TextureResList[nImagePosition].m_nLastUsedTime = GetTickCount();\r\n\t\t\tpObject = m_TextureResList[nImagePosition].m_pTextureRes;\r\n",
        "\t\tif (m_TextureResList[nImagePosition].m_nType == nType)\r\n\t\t{\r\n\t\t\tm_TextureResList[nImagePosition].m_nLastUsedTime = GetTickCount();\r\n"
        "\t\t\tif (m_TextureResList[nImagePosition].m_bDangNap)\t// [NAP 08/09 b] dang nap o luong nen\r\n\t\t\t{\r\n"
        "\t\t\t\tif (m_bVeDangDien) { m_nNapNenBoVe++; return NULL; }\t// dang ve: bo anh nay khung nay, khung sau co\r\n"
        "\t\t\t\t// hoi dong bo (kich thuoc / alpha): nap ngay; ket qua luong nen ve sau se bi bo (NapNenNhan thay muc het 'dang nap')\r\n"
        "\t\t\t\tm_TextureResList[nImagePosition].m_bDangNap = false;\r\n"
        "\t\t\t\tm_TextureResList[nImagePosition].m_pTextureRes = LoadImage(pszImage, nType);\r\n"
        "\t\t\t\tif (m_TextureResList[nImagePosition].m_pTextureRes) { m_nLoadCount++; m_uTexCacheMemUsed += m_TextureResList[nImagePosition].m_pTextureRes->m_nTexMemUsed; }\r\n"
        "\t\t\t\telse Rep3LogLoadFail(pszImage, nType);\r\n\t\t\t}\r\n"
        "\t\t\tpObject = m_TextureResList[nImagePosition].m_pTextureRes;\r\n")
    # nap lai sau that bai: cung giao luong nen
    s = rep(s, "\t\t\t\t\tm_TextureResList[nImagePosition].m_nRetryTime = tmNow;\r\n\t\t\t\t\tpObject = LoadImage(pszImage, nType);\r\n",
        "\t\t\t\t\tm_TextureResList[nImagePosition].m_nRetryTime = tmNow;\r\n"
        "\t\t\t\t\tif (g_nRep3NapNen && m_bVeDangDien && NapNenGiao(pszImage, uImage, nType))\t// [NAP 08/09 b]\r\n"
        "\t\t\t\t\t{\r\n\t\t\t\t\t\tm_TextureResList[nImagePosition].m_bDangNap = true;\r\n\t\t\t\t\t\treturn NULL;\r\n\t\t\t\t\t}\r\n"
        "\t\t\t\t\tpObject = LoadImage(pszImage, nType);\r\n")
    # miss lan dau
    s = rep(s, "\telse\r\n\t{\r\n\t\tpObject = LoadImage(pszImage, nType);\r\n\t\tm_nLoadCount++;\r\n\t\tif (!pObject)\t// [REP3 03/09 LAG] VAN chen muc NULL: lan sau FindImage thay ngay, khoi quet lai 40 pak.\r\n\t\t\tRep3LogLoadFail(pszImage, nType);\t// van nap lai duoc, nhung theo nhip REP3_RELOAD_COOLDOWN\r\n\r\n\t\tResNode node;\r\n\t\tnode.m_bCacheable = true;\r\n",
        "\telse\r\n\t{\r\n"
        "\t\tconst bool bNapNen = (g_nRep3NapNen && m_bVeDangDien && NapNenGiao(pszImage, uImage, nType));\t// [NAP 08/09 b] dang ve: giao luong nen\r\n"
        "\t\tif (!bNapNen)\r\n\t\t{\r\n"
        "\t\tpObject = LoadImage(pszImage, nType);\r\n\t\tm_nLoadCount++;\r\n\t\tif (!pObject)\t// [REP3 03/09 LAG] VAN chen muc NULL: lan sau FindImage thay ngay, khoi quet lai 40 pak.\r\n\t\t\tRep3LogLoadFail(pszImage, nType);\t// van nap lai duoc, nhung theo nhip REP3_RELOAD_COOLDOWN\r\n"
        "\t\t}\r\n\r\n\t\tResNode node;\r\n\t\tnode.m_bDangNap = bNapNen;\r\n\t\tnode.m_bCacheable = true;\r\n")
    s = rep(s, "\t\tm_TextureResList.insert(m_TextureResList.begin() + nImagePosition, node);\r\n\r\n        \r\n\t\tif(pObject && m_bDoProfile)\r\n",
        "\t\tm_TextureResList.insert(m_TextureResList.begin() + nImagePosition, node);\r\n"
        "\t\tif (bNapNen)\r\n\t\t\treturn NULL;\t// [NAP 08/09 b] khung sau NapNenNhan gan ket qua vao muc\r\n\r\n        \r\n\t\tif(pObject && m_bDoProfile)\r\n")
    # hoi dong bo: tat co dang ve quanh GetImage
    s = rep(s, "\tvoid* pTemp = GetImage(pszImage, uImage, nPos, 0, nType, false);\r\n\tif (!pTemp)\r\n\t\treturn false;\r\n",
        "\tconst bool bVeLuu = m_bVeDangDien; m_bVeDangDien = false;\t// [NAP 08/09 b] hoi dong bo: nap ngay\r\n"
        "\tvoid* pTemp = GetImage(pszImage, uImage, nPos, 0, nType, false);\r\n\tm_bVeDangDien = bVeLuu;\r\n\tif (!pTemp)\r\n\t\treturn false;\r\n", 2)
    s = rep(s, "\t\tpTemp = GetImage(pszImage, uImage, nPos, nFrame, ISI_T_SPR);\r\n",
        "\t\t{ const bool bVeLuu = m_bVeDangDien; m_bVeDangDien = false; pTemp = GetImage(pszImage, uImage, nPos, nFrame, ISI_T_SPR); m_bVeDangDien = bVeLuu; }\t// [NAP 08/09 b]\r\n")
    s = rep(s, "\t\tpTemp = GetImage(pszImage, uImage, nPos, nFrame, ISI_T_BITMAP16);\r\n",
        "\t\t{ const bool bVeLuu = m_bVeDangDien; m_bVeDangDien = false; pTemp = GetImage(pszImage, uImage, nPos, nFrame, ISI_T_BITMAP16); m_bVeDangDien = bVeLuu; }\t// [NAP 08/09 b]\r\n")
    s += (
        "\r\n// ============================ [NAP 08/09 b] nap tai nguyen o luong nen ============================\r\n"
        "// Chay o luong nen: chi doc pak + giai ma, KHONG dung device (ZCache/XPackFile co khoa san; JPEG khoa rieng trong LoadJpegDecode).\r\n"
        "static TextureRes* NapNenTai(const char* pszTen, uint32 nType)\r\n{\r\n"
        "\tif (nType == ISI_T_SPR)\r\n\t{\r\n"
        "\t\tTextureResSpr* p = new TextureResSpr;\r\n"
        "\t\tif (!p->LoadImage((LPSTR)pszTen, nType)) { delete p; return NULL; }\r\n"
        "\t\treturn p;\r\n\t}\r\n"
        "\tif (nType == ISI_T_BITMAP16)\r\n\t{\r\n"
        "\t\tTextureResBmp* p = new TextureResBmp;\r\n"
        "\t\tp->Release();\r\n\t\tp->m_nType = nType;\r\n"
        "\t\tif (!p->LoadJpegDecode((LPSTR)pszTen)) { delete p; return NULL; }\r\n"
        "\t\treturn p;\r\n\t}\r\n"
        "\treturn NULL;\r\n}\r\n\r\n"
        "unsigned __stdcall TextureResMgr::NapNenLuong(void* p)\r\n{\r\n\t((TextureResMgr*)p)->NapNenChay();\r\n\treturn 0;\r\n}\r\n\r\n"
        "void TextureResMgr::NapNenChay()\r\n{\r\n"
        "\tfor (;;)\r\n\t{\r\n"
        "\t\tWaitForSingleObject(m_hNapCo, INFINITE);\r\n"
        "\t\tif (m_lNapDung)\r\n\t\t\tbreak;\r\n"
        "\t\tfor (;;)\r\n\t\t{\r\n"
        "\t\t\tNapViec v;\r\n"
        "\t\t\t{\r\n\t\t\t\tKAutoCriticalSection k(m_napKhoa);\r\n"
        "\t\t\t\tif (m_napViec.empty())\r\n\t\t\t\t\tbreak;\r\n"
        "\t\t\t\tv = m_napViec.front();\r\n\t\t\t\tm_napViec.erase(m_napViec.begin());\r\n\t\t\t}\r\n"
        "\t\t\tNapKetQua kq;\r\n\t\t\tmemcpy(kq.szTen, v.szTen, sizeof(kq.szTen)); kq.uId = v.uId; kq.nType = v.nType;\r\n"
        "\t\t\tkq.pRes = NapNenTai(v.szTen, v.nType);\r\n"
        "\t\t\t{\r\n\t\t\t\tKAutoCriticalSection k(m_napKhoa);\r\n\t\t\t\tm_napXong.push_back(kq);\r\n\t\t\t}\r\n"
        "\t\t\tif (m_lNapDung)\r\n\t\t\t\tbreak;\r\n"
        "\t\t}\r\n\t}\r\n}\r\n\r\n"
        "bool TextureResMgr::NapNenGiao(const char* pszImage, uint32 uId, uint32 nType)\r\n{\r\n"
        "\tif (m_bNapNenLoi || !pszImage)\r\n\t\treturn false;\r\n"
        "\tif (!m_hNapLuong)\r\n\t{\r\n"
        "\t\tm_hNapCo = CreateEventA(NULL, FALSE, FALSE, NULL);\r\n"
        "\t\tm_lNapDung = 0;\r\n"
        "\t\tunsigned uTid = 0;\r\n"
        "\t\tm_hNapLuong = m_hNapCo ? (HANDLE)_beginthreadex(NULL, 0, NapNenLuong, this, 0, &uTid) : NULL;\r\n"
        "\t\tif (!m_hNapLuong)\r\n\t\t{\r\n"
        "\t\t\tm_bNapNenLoi = true;\r\n"
        "\t\t\tif (m_hNapCo) { CloseHandle(m_hNapCo); m_hNapCo = NULL; }\r\n"
        "\t\t\tRep3Log(\"[REP3] nap nen: khong tao duoc luong -> nap ngay tren luong ve\");\r\n"
        "\t\t\treturn false;\r\n\t\t}\r\n"
        "\t\tSetThreadPriority(m_hNapLuong, THREAD_PRIORITY_BELOW_NORMAL);\r\n"
        "\t\tRep3Log(\"[REP3] nap nen: luong nen da chay (Rep3NapNen=1)\");\r\n"
        "\t}\r\n"
        "\tNapViec v;\r\n\tstrncpy(v.szTen, pszImage, MAX_PATH - 1); v.szTen[MAX_PATH - 1] = 0; v.uId = uId; v.nType = nType;\r\n"
        "\t{\r\n\t\tKAutoCriticalSection k(m_napKhoa);\r\n\t\tm_napViec.push_back(v);\r\n\t}\r\n"
        "\tm_nNapNenGui++;\r\n"
        "\tSetEvent(m_hNapCo);\r\n"
        "\treturn true;\r\n}\r\n\r\n"
        "// Luong ve, dau moi khung (RepresentBegin): gan ket qua vao muc; BMP tao texture tai day (can device).\r\n"
        "void TextureResMgr::NapNenNhan()\r\n{\r\n"
        "\tvector<NapKetQua> xong;\r\n"
        "\t{\r\n\t\tKAutoCriticalSection k(m_napKhoa);\r\n\t\tif (m_napXong.empty())\r\n\t\t\treturn;\r\n\t\txong.swap(m_napXong);\r\n\t}\r\n"
        "\tKAutoCriticalSection AutoLock(m_ImageProcessLock);\r\n"
        "\tfor (size_t i = 0; i < xong.size(); i++)\r\n\t{\r\n"
        "\t\tNapKetQua& kq = xong[i];\r\n"
        "\t\tint nIdx = FindImage(kq.uId, 0);\r\n"
        "\t\tif (nIdx < 0 || m_TextureResList[nIdx].m_nType != kq.nType || !m_TextureResList[nIdx].m_bDangNap)\r\n"
        "\t\t{\t// muc da bi bo (CheckBalance/Free) hoac da nap dong bo trong luc cho -> bo ket qua\r\n"
        "\t\t\tif (kq.pRes) delete kq.pRes;\r\n\t\t\tcontinue;\r\n\t\t}\r\n"
        "\t\tResNode& node = m_TextureResList[nIdx];\r\n"
        "\t\tnode.m_bDangNap = false;\r\n"
        "\t\tTextureRes* pRes = kq.pRes;\r\n"
        "\t\tif (pRes && !pRes->NapNenHoanTat()) { delete pRes; pRes = NULL; }\r\n"
        "\t\tif (pRes)\r\n\t\t{\r\n"
        "\t\t\tnode.m_pTextureRes = pRes;\r\n\t\t\tnode.m_nLastUsedTime = GetTickCount();\r\n"
        "\t\t\tm_uTexCacheMemUsed += pRes->m_nTexMemUsed;\r\n\t\t\tm_nLoadCount++;\r\n\t\t\tm_nNapNenXong++;\r\n\t\t}\r\n"
        "\t\telse\r\n\t\t{\r\n"
        "\t\t\tnode.m_pTextureRes = NULL;\r\n\t\t\tnode.m_nRetryTime = GetTickCount();\r\n\t\t\tm_nNapNenHong++;\r\n"
        "\t\t\tRep3LogLoadFail(kq.szTen, kq.nType);\r\n\t\t}\r\n"
        "\t}\r\n}\r\n\r\n"
        "void TextureResMgr::NapNenDung()\r\n{\r\n"
        "\tif (m_hNapLuong)\r\n\t{\r\n"
        "\t\tInterlockedExchange(&m_lNapDung, 1);\r\n"
        "\t\tif (m_hNapCo) SetEvent(m_hNapCo);\r\n"
        "\t\tWaitForSingleObject(m_hNapLuong, 5000);\r\n"
        "\t\tCloseHandle(m_hNapLuong); m_hNapLuong = NULL;\r\n\t}\r\n"
        "\tif (m_hNapCo) { CloseHandle(m_hNapCo); m_hNapCo = NULL; }\r\n"
        "\tKAutoCriticalSection k(m_napKhoa);\r\n"
        "\tfor (size_t i = 0; i < m_napXong.size(); i++)\r\n\t\tif (m_napXong[i].pRes) delete m_napXong[i].pRes;\r\n"
        "\tm_napXong.clear(); m_napViec.clear();\r\n"
        "\tm_lNapDung = 0;\r\n}\r\n")
save(p, s, h0); print("OK TextureResMgr.cpp")

# ============ KRepresentShell3.cpp: ini, RepresentBegin/End, thong ke
p, s = load("KRepresentShell3.cpp"); h0 = hi(s)
if "g_nRep3NapNen" not in s:
    s = rep(s, "int  g_nRep3StatSec   = 30;", "int  g_nRep3NapNen    = 1;\t// [NAP 08/09 b] nap sprite/jpeg o luong nen\r\nint  g_nRep3StatSec   = 30;")
    s = rep(s, '\tg_nRep3StatSec   = Rep3Ini("Rep3StatSec", 30);', '\tg_nRep3NapNen    = Rep3Ini("Rep3NapNen", 1);\t// [NAP 08/09 b]\r\n\tg_nRep3StatSec   = Rep3Ini("Rep3StatSec", 30);')
    s = rep(s, "\tm_TextureResMgr.StartProfile();\r\n", "\tm_TextureResMgr.NapNenNhan();\t// [NAP 08/09 b] nhan ket qua luong nen truoc khi ve\r\n\tm_TextureResMgr.m_bVeDangDien = true;\r\n\tm_TextureResMgr.StartProfile();\r\n")
    s = rep(s, "\t{\t// [NAP 08/09 a] tong ms nap trong khung nay -> max / dem khung nang\r\n",
        "\tm_TextureResMgr.m_bVeDangDien = false;\t// [NAP 08/09 b] ngoai luc ve: hoi anh nap ngay\r\n\t{\t// [NAP 08/09 a] tong ms nap trong khung nay -> max / dem khung nang\r\n")
    s = rep(s, "khung co nap >5 ms: %u, >16 ms: %u, max %.1f ms/khung\",\t// [NAP 08/09 a]\r\n",
        "khung co nap >5 ms: %u, >16 ms: %u, max %.1f ms/khung | nen: giao %u xong %u hong %u bo_ve %u\",\t// [NAP 08/09 a/b]\r\n")
    s = rep(s, "g_napGiaiMa.n, g_napGiaiMa.ms, g_napGiaiMa.max, g_napGpu.n, g_napGpu.ms, g_napGpu.max, g_uRep3NapKhung5, g_uRep3NapKhung16, g_dRep3NapKhungMax);\r\n",
        "g_napGiaiMa.n, g_napGiaiMa.ms, g_napGiaiMa.max, g_napGpu.n, g_napGpu.ms, g_napGpu.max, g_uRep3NapKhung5, g_uRep3NapKhung16, g_dRep3NapKhungMax,\r\n"
        "\t\t\t\tm_TextureResMgr.m_nNapNenGui, m_TextureResMgr.m_nNapNenXong, m_TextureResMgr.m_nNapNenHong, m_TextureResMgr.m_nNapNenBoVe);\r\n"
        "\t\t\tm_TextureResMgr.m_nNapNenGui = 0; m_TextureResMgr.m_nNapNenXong = 0; m_TextureResMgr.m_nNapNenHong = 0; m_TextureResMgr.m_nNapNenBoVe = 0;\r\n")
save(p, s, h0); print("OK KRepresentShell3.cpp")
print("XONG NAP b")
