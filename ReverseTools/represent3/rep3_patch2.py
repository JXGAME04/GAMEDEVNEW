# -*- coding: utf-8 -*-
"""[REP3 03/09 RAM] Represent3: RAM gap doi Represent2 vi texture sprite o D3DPOOL_MANAGED (D3D giu them ban sao day du
trong RAM tien trinh, = VRAM dang dung) + ngan sach cache 480 MB. Vá:
 1. Texture sprite -> D3DPOOL_DEFAULT (chi VRAM): do qua texture tam SYSTEMMEM + UpdateTexture; mat device -> bo, tao lai luc ve.
 2. GetPixelAlpha (hit-test chuot) doc tu RLE goc (pRawData) thay vi LockRect texture (DEFAULT khong lock duoc).
 3. Ngan sach cache: RAM/16 kep 60..384 MB (la VRAM), doc Rep3CacheMB SAU khi doc ini (truoc day doc trong ctor -> khong tac dung);
    vuot ngan sach -> bo toi da 8 khung/luot, nghi > 1 s (binh thuong 1 khung, nghi > 10 s).
 4. Thong ke dinh ky vao jx_rep3.log (Rep3StatSec, mac dinh 30 s): RAM rieng, VRAM con, cache texture/raw, nap/bo, fps.
Cong tac [Client]: Rep3Pool=1 (0 = MANAGED cu), Rep3StatSec=30 (0 = tat). Doc/ghi latin-1, giu CRLF."""
import io, os, re

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def nl(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
def rep(text, old, new, name, count=1):
    n = text.count(old)
    assert n == count, "%s: found %d (expected %d)" % (name, n, count)
    return text.replace(old, new)

MARK = "[REP3 03/09 RAM]"
ROOT = r"D:\GAMEDEVNEW_wt_rep3\Sources\Represent\Represent3"

# ---------------- BaseInclude.h
p = os.path.join(ROOT, "BaseInclude.h"); s = rd(p); N = nl(s)
if MARK not in s:
    old = "extern int  g_nRep3Log;"
    i = s.index(old); j = s.index(N, i) + len(N)
    add = ("extern int  g_nRep3Pool;       // %s 1 = texture sprite o D3DPOOL_DEFAULT (chi VRAM, khong ban sao RAM), 0 = MANAGED cu" % MARK + N +
           "extern int  g_nRep3StatSec;    // %s chu ky ghi thong ke RAM/VRAM/cache vao jx_rep3.log (giay), 0 = tat" % MARK + N)
    s = s[:j] + add + s[j:]
    wr(p, s); print("BaseInclude.h OK")
else:
    print("BaseInclude.h da va")

# ---------------- KRepresentShell3.cpp
p = os.path.join(ROOT, "KRepresentShell3.cpp"); s = rd(p); N = nl(s)
if MARK not in s:
    s = rep(s, "int  g_nRep3Log       = 1;", "int  g_nRep3Log       = 1;" + N + "int  g_nRep3Pool      = 1;\t// %s" % MARK + N + "int  g_nRep3StatSec   = 30;", "globals")
    s = rep(s, '#include "TextureRes.h"' + N, '#include "TextureRes.h"' + N + "#include <psapi.h>\t// %s GetProcessMemoryInfo" % MARK + N + '#pragma comment(lib, "psapi.lib")' + N, "include")
    s = rep(s, '\tg_nRep3Log       = Rep3Ini("Rep3Log", 1);' + N,
            '\tg_nRep3Log       = Rep3Ini("Rep3Log", 1);' + N +
            '\tg_nRep3Pool      = Rep3Ini("Rep3Pool", 1);\t\t// %s' % MARK + N +
            '\tg_nRep3StatSec   = Rep3Ini("Rep3StatSec", 30);' + N +
            '\tm_TextureResMgr.SetBudget();\t// %s doc Rep3CacheMB SAU khi doc ini (ctor chay truoc Create)' % MARK + N, "ini")
    old_log = ('\tRep3Log("[REP3] Create %dx%d full=%d flat=%d composite=%d tex32=%d npot=%d vsync=%d cacheMB=%d",' + N +
               '\t\tnWidth, nHeight, (int)bFullScreen, g_nRep3Flat, g_nRep3Composite, g_nRep3Tex32, g_nRep3Npot, g_nRep3Vsync, g_nRep3CacheMB);' + N)
    new_log = ('\tRep3Log("[REP3] Create %dx%d full=%d flat=%d composite=%d tex32=%d npot=%d vsync=%d cacheMB=%d pool=%s statSec=%d",' + N +
               '\t\tnWidth, nHeight, (int)bFullScreen, g_nRep3Flat, g_nRep3Composite, g_nRep3Tex32, g_nRep3Npot, g_nRep3Vsync, g_nRep3CacheMB,' + N +
               '\t\tg_nRep3Pool ? "DEFAULT(VRAM)" : "MANAGED(RAM+VRAM)", g_nRep3StatSec);\t// %s' % MARK + N)
    s = rep(s, old_log, new_log, "create log")
    old_end = "\tif (m_fFpsAvg >= 25.0f)" + N + "\t\tm_TextureResMgr.CheckBalanceFrame();" + N
    new_end = old_end + N.join([
        "",
        "\t// %s thong ke dinh ky: RAM rieng tien trinh, VRAM con trong, cache texture (VRAM) + raw spr (RAM), so nap/bo, fps" % MARK,
        "\tif (g_nRep3StatSec > 0)",
        "\t{",
        "\t\tstatic DWORD s_dwLastStat = 0;",
        "\t\tif (s_dwLastStat == 0 || (dwNow - s_dwLastStat) >= (DWORD)g_nRep3StatSec * 1000)",
        "\t\t{",
        "\t\t\ts_dwLastStat = dwNow;",
        "\t\t\tPROCESS_MEMORY_COUNTERS_EX pmc; memset(&pmc, 0, sizeof(pmc)); pmc.cb = sizeof(pmc);",
        "\t\t\tGetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));",
        "\t\t\tuint32 uNodes = 0, uTexMB = 0, uRawMB = 0, uDrawMB = 0, uBudgetMB = 0;",
        "\t\t\tm_TextureResMgr.GetStat(uNodes, uTexMB, uRawMB, uDrawMB, uBudgetMB);",
        '\t\t\tRep3Log("[REP3] RAM rieng %u MB, WS %u MB | VRAM con %u MB | cache %u muc: texture %u MB (ve khung nay %u MB, ngan sach %u MB), raw spr %u MB | nap %u, bo %u | fps TB %.0f",',
        "\t\t\t\t(unsigned)(pmc.PrivateUsage >> 20), (unsigned)(pmc.WorkingSetSize >> 20), (unsigned)(PD3DDEVICE->GetAvailableTextureMem() >> 20),",
        "\t\t\t\tuNodes, uTexMB, uDrawMB, uBudgetMB, uRawMB, (unsigned)m_TextureResMgr.m_nLoadCount, (unsigned)m_TextureResMgr.m_nReleaseCount, m_fFpsAvg);",
        "\t\t}",
        "\t}",
        "",
    ])
    s = rep(s, old_end, new_end, "represent end")
    wr(p, s); print("KRepresentShell3.cpp OK")
else:
    print("KRepresentShell3.cpp da va")

# ---------------- TextureResMgr.h
p = os.path.join(ROOT, "TextureResMgr.h"); s = rd(p); N = nl(s)
if MARK not in s:
    s = rep(s, "    void CheckBalanceFrame();" + N,
            "    void CheckBalanceFrame();" + N +
            "    // %s ngan sach cache (goi sau khi doc ini) + thong ke cho jx_rep3.log" % MARK + N +
            "    void SetBudget();" + N +
            "    void GetStat(uint32& uNodes, uint32& uTexMB, uint32& uRawMB, uint32& uDrawMB, uint32& uBudgetMB);" + N, "mgr.h")
    wr(p, s); print("TextureResMgr.h OK")
else:
    print("TextureResMgr.h da va")

# ---------------- TextureResMgr.cpp
p = os.path.join(ROOT, "TextureResMgr.cpp"); s = rd(p); N = nl(s)
if MARK not in s:
    # ctor: bo khoi tinh ngan sach (chay truoc khi doc ini) -> mac dinh 256 MB, SetBudget() tinh lai trong Create
    i0 = s.index("\tMEMORYSTATUSEX stat;")
    i1 = s.index('\tRep3Log("[REP3] cache texture: RAM %I64u MB -> ngan sach %I64u MB", uPhysMB, uBudgetMB);')
    i1 = s.index(N, i1) + len(N)
    ctor_new = ("\t// %s ctor chay TRUOC khi Create() doc config.ini -> chi dat mac dinh, SetBudget() tinh lai sau" % MARK + N +
                "\tm_nBalanceNum = 256 * 1024 * 1024;" + N +
                "\tm_nMaxReleaseCount = 64;" + N +
                "\tm_uCheckPoint = 25;" + N)
    s = s[:i0] + ctor_new + s[i1:]
    # CheckBalance moi
    j0 = s.index("void TextureResMgr::CheckBalance()")
    j1 = s.index(N + "}" + N, j0) + len(N) + 1 + len(N)
    cb_new = N.join([
        "void TextureResMgr::CheckBalance()",
        "{",
        "\t// [REP3 03/09] theo represent3free.dll 2.0 (0x10024F80): duyet tu cuoi, bo qua tai nguyen vua ve khung truoc,",
        "\t// chi xet tai nguyen nghi > 10 s; moi luot BO DUNG MOT KHUNG texture (ReleaseAFrameData) - het khung",
        "\t// moi xoa ca tai nguyen. Khong con cu xoa hang loat 16/32/64 cai gay khung.",
        "\t// %s vuot ngan sach VRAM: bo toi da 8 khung/luot, chi can nghi > 1 s (Release() texture re, ton khi tao lai thoi)" % MARK,
        "    KAutoCriticalSection AutoLock(m_ImageProcessLock);",
        "",
        "\tbool bOver = (m_uTexCacheMemUsed > (uint32)m_nBalanceNum);",
        "\tint nMax = bOver ? 8 : 1;",
        "\tuint32 uIdle = bOver ? 1000 : 10000;",
        "\tuint32 nTickCount = GetTickCount();",
        "\tint nDone = 0;",
        "\tfor (int i = (int)m_TextureResList.size() - 1; i >= 0 && nDone < nMax; i--)",
        "\t{",
        "\t\tResNode& node = m_TextureResList[i];",
        "\t\tif (!node.m_bCacheable || !node.m_pTextureRes)",
        "\t\t\tcontinue;",
        "\t\tif (node.m_pTextureRes->m_bLastFrameUsed)",
        "\t\t\tcontinue;",
        "\t\tif ((nTickCount - node.m_nLastUsedTime) <= uIdle)",
        "\t\t\tcontinue;",
        "\t\tm_nReleaseCount++;",
        "\t\tnDone++;",
        "\t\tif (node.m_pTextureRes->ReleaseAFrameData())",
        "\t\t\tcontinue;",
        "\t\tif (m_uTexCacheMemUsed >= node.m_pTextureRes->m_nTexMemUsed)",
        "\t\t\tm_uTexCacheMemUsed -= node.m_pTextureRes->m_nTexMemUsed;",
        "\t\tnode.m_pTextureRes->Release();",
        "\t\tSAFE_DELETE(node.m_pTextureRes);",
        "\t\tm_TextureResList.erase(m_TextureResList.begin() + i);",
        "\t}",
        "}",
        "",
        "void TextureResMgr::SetBudget()",
        "{",
        "\t// %s ngan sach cache texture = VRAM (texture o POOL_DEFAULT, khong chiem RAM tien trinh): RAM/16 kep 60..384 MB;" % MARK,
        "\t// 2.0 dung toi da 120 MB 4444 (= 240 MB 8888). [Client] Rep3CacheMB ghi de.",
        "\tMEMORYSTATUSEX stat;",
        "\tstat.dwLength = sizeof(stat);",
        "\tGlobalMemoryStatusEx(&stat);",
        "\tunsigned __int64 uPhysMB = stat.ullTotalPhys / (1024 * 1024);",
        "\tunsigned __int64 uBudgetMB = uPhysMB / 16;",
        "\tif (uBudgetMB < 60)  uBudgetMB = 60;",
        "\tif (uBudgetMB > 384) uBudgetMB = 384;",
        "\tif (g_nRep3CacheMB > 0)",
        "\t\tuBudgetMB = (unsigned __int64)g_nRep3CacheMB;",
        "\tm_nBalanceNum = (int32)(uBudgetMB * 1024 * 1024);",
        '\tRep3Log("[REP3] cache texture: RAM %I64u MB -> ngan sach %I64u MB (%s)", uPhysMB, uBudgetMB, g_nRep3Pool ? "VRAM, POOL_DEFAULT" : "RAM+VRAM, POOL_MANAGED");',
        "}",
        "",
        "void TextureResMgr::GetStat(uint32& uNodes, uint32& uTexMB, uint32& uRawMB, uint32& uDrawMB, uint32& uBudgetMB)",
        "{",
        "\t// %s cho dong thong ke trong jx_rep3.log" % MARK,
        "    KAutoCriticalSection AutoLock(m_ImageProcessLock);",
        "\tunsigned __int64 uRaw = 0;",
        "\tfor (int i = 0; i < (int)m_TextureResList.size(); i++)",
        "\t\tif (m_TextureResList[i].m_pTextureRes)",
        "\t\t\tuRaw += m_TextureResList[i].m_pTextureRes->m_nSprMemUsed;",
        "\tuNodes = (uint32)m_TextureResList.size();",
        "\tuTexMB = m_uTexCacheMemUsed >> 20;",
        "\tuDrawMB = m_uMemDrawingUsed >> 20;",
        "\tuRawMB = (uint32)(uRaw >> 20);",
        "\tuBudgetMB = ((uint32)m_nBalanceNum) >> 20;",
        "}",
        "",
    ])
    s = s[:j0] + cb_new + s[j1:]
    wr(p, s); print("TextureResMgr.cpp OK")
else:
    print("TextureResMgr.cpp da va")

# ---------------- TextureRes.h
p = os.path.join(ROOT, "TextureRes.h"); s = rd(p); N = nl(s)
if MARK not in s:
    s = rep(s, "\tvirtual bool ReleaseAFrameData();" + N + N + "\tbool PrepareFrameData(",
            "\tvirtual bool ReleaseAFrameData();" + N +
            "\tvirtual bool InvalidateDeviceObjects();\t// %s texture POOL_DEFAULT mat khi reset device -> bo het, tao lai luc ve" % MARK + N + N +
            "\tbool PrepareFrameData(", "spr class")
    wr(p, s); print("TextureRes.h OK")
else:
    print("TextureRes.h da va")

# ---------------- TextureRes.cpp
p = os.path.join(ROOT, "TextureRes.cpp"); s = rd(p); N = nl(s)
if MARK not in s:
    # (a) dem raw spr
    s = rep(s, "\t\t\tmemcpy(m_pFrameInfo[i].pRawData, pFrame->Sprite, m_pFrameInfo[i].nRawDataLen);" + N,
            "\t\t\tmemcpy(m_pFrameInfo[i].pRawData, pFrame->Sprite, m_pFrameInfo[i].nRawDataLen);" + N +
            "\t\t\tm_nSprMemUsed += m_pFrameInfo[i].nRawDataLen;\t// %s" % MARK + N, "raw count")
    s = rep(s, "\tm_nTexMemUsed = 0;" + N + "\tm_bLastFrameUsed = false;" + N + "\tm_bNew = false;" + N,
            "\tm_nTexMemUsed = 0;" + N + "\tm_nSprMemUsed = 0;\t// %s" % MARK + N + "\tm_bLastFrameUsed = false;" + N + "\tm_bNew = false;" + N, "spr resetvar")
    # (b) CreateTexture16Bit: than vong lap tao texture
    k0 = s.index("\tfor(i=0; i<m_pFrameInfo[nFrame].nTexNum; i++)" + N + "\t{" + N + "\t\tm_nTexMemUsed += m_pFrameInfo[nFrame].texInfo[i].nWidth")
    k1 = s.index("\tSAFE_DELETE_ARRAY(pTempData);" + N + "\tif(m_pHeader)", k0)
    loop_new = N.join([
        "\tfor(i=0; i<m_pFrameInfo[nFrame].nTexNum; i++)",
        "\t{",
        "\t\tTextureInfo& ti = m_pFrameInfo[nFrame].texInfo[i];",
        "\t\tm_nTexMemUsed += ti.nWidth * ti.nHeight * nBpp;",
        "",
        "\t\tSAFE_RELEASE(ti.pTexture);",
        "\t\t// %s Rep3Pool=1: do vao texture tam SYSTEMMEM roi UpdateTexture sang texture POOL_DEFAULT (chi o VRAM)." % MARK,
        "\t\t// MANAGED cu giu them mot ban sao day du trong RAM tien trinh (= VRAM dang dung) -> RAM gap doi Represent2.",
        "\t\tLPDIRECT3DTEXTURE9 pFill = NULL;",
        "\t\tif (FAILED(PD3DDEVICE->CreateTexture(ti.nWidth, ti.nHeight, 1, 0, eFmt,",
        "\t\t\t\t\t\t\t\tg_nRep3Pool ? D3DPOOL_SYSTEMMEM : D3DPOOL_MANAGED, &pFill, NULL)))",
        "\t\t\tgoto error;",
        "",
        "\t\tD3DLOCKED_RECT LockedRect;",
        "\t\tif (FAILED(pFill->LockRect(0, &LockedRect, NULL, 0)))",
        "\t\t{",
        "\t\t\tpFill->Release();",
        "\t\t\tgoto error;",
        "\t\t}",
        "",
        "\t\tBYTE *pTexData = (BYTE*)LockedRect.pBits;",
        "\t\tBYTE *pTp = pTempData + (ti.nFrameY * nW + ti.nFrameX) * nBpp;",
        "\t\tfor(j=0; j<ti.nFrameHeight; j++)",
        "\t\t{",
        "\t\t\tmemcpy(pTexData, pTp, ti.nFrameWidth * nBpp);",
        "\t\t\tpTexData += LockedRect.Pitch;",
        "\t\t\tpTp += nW * nBpp;",
        "\t\t}",
        "\t\tpFill->UnlockRect(0);",
        "",
        "\t\tif (g_nRep3Pool)",
        "\t\t{",
        "\t\t\tLPDIRECT3DTEXTURE9 pVram = NULL;",
        "\t\t\tif (FAILED(PD3DDEVICE->CreateTexture(ti.nWidth, ti.nHeight, 1, 0, eFmt, D3DPOOL_DEFAULT, &pVram, NULL)) ||",
        "\t\t\t\tFAILED(PD3DDEVICE->UpdateTexture(pFill, pVram)))",
        "\t\t\t{",
        "\t\t\t\tSAFE_RELEASE(pVram);",
        "\t\t\t\tpFill->Release();",
        "\t\t\t\tgoto error;",
        "\t\t\t}",
        "\t\t\tpFill->Release();",
        "\t\t\tti.pTexture = pVram;",
        "\t\t}",
        "\t\telse",
        "\t\t\tti.pTexture = pFill;",
        "\t}",
        "",
    ])
    s = s[:k0] + loop_new + s[k1:]
    # (c) GetPixelAlpha doc tu RLE goc
    g0 = s.index("int32 TextureResSpr::GetPixelAlpha(int32 nFrame, int32 x, int32 y)")
    g1 = s.index(N + "}" + N, g0) + len(N) + 1 + len(N)
    gpa_new = N.join([
        "int32 TextureResSpr::GetPixelAlpha(int32 nFrame, int32 x, int32 y)",
        "{",
        "\t// %s doc alpha tu du lieu RLE goc (pRawData) thay vi LockRect texture: texture POOL_DEFAULT khong lock duoc," % MARK,
        "\t// va cach cu ep dong bo GPU moi lan ro chuot. RLE: [n][a] roi n byte chi so mau neu a != 0 (nhu RenderToA8R8G8B8).",
        "\tif (nFrame < 0 || nFrame >= m_nFrameNum || !m_pFrameInfo)",
        "\t\treturn 0;",
        "\tx -= m_pFrameInfo[nFrame].nOffX;",
        "\ty -= m_pFrameInfo[nFrame].nOffY;",
        "\tif (x < 0 || y < 0 || x >= m_pFrameInfo[nFrame].nWidth || y >= m_pFrameInfo[nFrame].nHeight)",
        "\t\treturn 0;",
        "",
        "\tBYTE* pSrc = m_pFrameInfo[nFrame].pRawData;",
        "\tint nLen = m_pFrameInfo[nFrame].nRawDataLen;",
        "\tSPRFRAME* pTmp = NULL;",
        "\tif (!pSrc && m_pHeader)\t// spr nen theo khung (raw da tra sau khi tao texture): lay lai khung tu pak",
        "\t{",
        "\t\tpTmp = (SPRFRAME*)SprGetFrame((SPRHEAD*)m_pHeader, nFrame);",
        "\t\tif (!pTmp)",
        "\t\t\treturn 0;",
        "\t\tint nL = (int)m_pOffset[nFrame].Length;",
        "\t\tif (nL < 0) nL = -nL;",
        "\t\tpSrc = pTmp->Sprite;",
        "\t\tnLen = nL - 8;",
        "\t}",
        "\tif (!pSrc || nLen <= 0)",
        "\t{",
        "\t\tif (pTmp) SprReleaseFrame(pTmp);",
        "\t\treturn 0;",
        "\t}",
        "",
        "\tint nRet = 0;",
        "\tint nTarget = y * m_pFrameInfo[nFrame].nWidth + x;",
        "\tint nPos = 0;",
        "\tBYTE* p = pSrc;",
        "\tBYTE* pEnd = pSrc + nLen;",
        "\twhile (p + 2 <= pEnd)",
        "\t{",
        "\t\tint n = *p++;",
        "\t\tint a = *p++;",
        "\t\tif (nTarget < nPos + n)",
        "\t\t{",
        "\t\t\tnRet = a;",
        "\t\t\tbreak;",
        "\t\t}",
        "\t\tnPos += n;",
        "\t\tif (a != 0)",
        "\t\t\tp += n;",
        "\t}",
        "\tif (pTmp)",
        "\t\tSprReleaseFrame(pTmp);",
        "\treturn nRet;",
        "}",
        "",
    ])
    s = s[:g0] + gpa_new + s[g1:]
    # (d) InvalidateDeviceObjects cho spr: sau ReleaseAFrameData
    r0 = s.index("bool TextureResSpr::ReleaseAFrameData()")
    r1 = s.index(N + "}" + N, r0) + len(N) + 1 + len(N)
    inv_new = N.join([
        "",
        "bool TextureResSpr::InvalidateDeviceObjects()",
        "{",
        "\t// %s texture POOL_DEFAULT phai bo truoc khi Reset device; PrepareFrameData tao lai lan ve sau" % MARK,
        "\tif (g_nRep3Pool)",
        "\t\twhile (ReleaseAFrameData()) {}",
        "\treturn true;",
        "}",
        "",
    ])
    s = s[:r1] + inv_new + s[r1:]
    wr(p, s); print("TextureRes.cpp OK")
else:
    print("TextureRes.cpp da va")
print("DONE")
