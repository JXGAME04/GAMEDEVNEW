# -*- coding: utf-8 -*-
# Dot 14/09 chieu (chu: 'Tiep tuc' sau khi chon lam ca bon) - theo log Fold 7 11:12 + 12:41 (ban 109141108/109141129):
#   A. [KHOITRUOC] cap san khoi atlas (mac dinh 4 khoi R8G8 + 1 khoi BGRA8 = 320 MB) o khung ve dau tien + to 0 het cac lop bang chep GPU
#      -> driver cam ket bo nho ngay luc do (man dang nhap/nap) thay vi 2-3 khung giat 33-113 ms giua tran moi khi can khoi moi (4 khoi / 5 phut).
#      [Client] Rep3KhoiTruocPal / Rep3KhoiTruoc32 (0 = tat).
#   B. [MANG] KNetConnectAgent::Breathe: ngan sach xu ly goi moi vong lap [Client] MangMs (10 ms; 0 = khong gioi han nhu cu); het thi de phan con
#      lai sang vong sau (thu tu giu nguyen, tre <= 1 vong); ghi [MANG-CAT] (jx_paint.log, <= 1 dong/giay). Go cu mang 45-236 ms ([LOGIC-PHA]).
#   C. [PAKBAN] co g_nJxNenDocPak: luong nen bat khi dang nap tep spr / rut khung (giu khoa pak cua tep); luong ve thay co thi giao khung cho luong nen
#      (bo ve 1-3 khung nhu NAPTO) thay vi doi khoa 15-97 ms cho mot khung 1 KB ([NAP-CHAM]). Dem 'pak ban giao nen' vao [VE-TAI].
#   D. [PDET-UI] cua so GOC (con truc tiep cua goc lop) ve lau nhat trong khung: ghi vi tri/co/lop phu vao dong [PDET-UI] khi ba lop UI >= 20 ms.
# Chi JX_MOBILE, ban PC khong doi. Chay lai vo hai (chay sau _l.py).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[KHOITRUOC 14/09]"
DAUM = "[MANG 14/09]"
DAUP = "[PAKBAN 14/09]"
DAUU = "[PDET-UI 14/09]"
NL = "\r\n"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    assert NL in s, p
    return s


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def cao(s):
    return sum(1 for c in s if ord(c) >= 128)


def thay(s, cu, moi, ten, n=1):
    if s.count(cu) != n:
        raise SystemExit("%s: neo '%s...' thay %d lan, can %d" % (ten, cu.strip()[:70], s.count(cu), n))
    return s.replace(cu, moi)


def sau_dong(s, neo, them, ten):
    if s.count(neo) != 1:
        raise SystemExit("%s: neo '%s...' thay %d lan" % (ten, neo[:70], s.count(neo)))
    i = s.find(neo)
    j = s.find(NL, i) + len(NL)
    return s[:j] + them + s[j:]


def va(p, viec):
    s = doc(p)
    if DAU in s or DAUM in s or DAUP in s or DAUU in s:
        print("da va roi:", p)
        return
    c0 = cao(s)
    s = viec(s)
    assert cao(s) == c0, p + ": byte cao doi"
    ghi(p, s)
    print("da va:", p)


# ================================================================ A. KHOITRUOC
def v_gpui(s):
    cu = "\tSDL_GPUTexture* JxKhoiTex(UINT i) const { return (i < m_jxKhoiV.size()) ? m_jxKhoiV[i].pTex : NULL; }" + NL
    moi = cu + "\tint JxKhoiCapTruoc(SDL_GPUTextureFormat fmt, UINT bpp, int nSo);\t// " + DAU + " cap san nSo khoi (chua dung lop nao) + to 0 het cac lop bang chep GPU; tra so khoi da tao" + NL
    return thay(s, cu, moi, "JxKhoiCapTruoc khai bao")


def v_res_gpu(s):
    cu = "\t*ppTex = pTex; *pKhoi = (UINT)(m_jxKhoiV.size() - 1); *pLop = 0;" + NL + "\treturn true;" + NL + "}" + NL
    moi = (cu + NL
           + "// " + DAU + " Cap san nSo khoi (fmt) chua dung lop nao va to 0 het cac lop bang chep GPU ngay: driver cam ket bo nho trong man dang nhap/nap thay vi" + NL
           + "// 2-3 khung giat 33-113 ms giua tran moi khi can khoi moi (Fold 7 12:41: tao 64 MB ~54 ms + 2-3 lan nop dau 21-43 ms). JxCapKhoi lay dan cac lop." + NL
           + "int CAtlasMgrGpu::JxKhoiCapTruoc(SDL_GPUTextureFormat fmt, UINT bpp, int nSo)" + NL
           + "{" + NL
           + "\tint nTao = 0;" + NL
           + "\tif (!g_nJxAtlasKhoi || !m_pDev || !m_pDev->m_pGpu || bpp == 0) return 0;" + NL
           + "\tfor (int q = 0; q < nSo; q++)" + NL
           + "\t{" + NL
           + "\t\tif (m_jxKhoiV.size() >= JX_KHOI_MAX) break;" + NL
           + "\t\tUINT nLop = (UINT)g_nJxAtlasKhoiLop; if (nLop < 1) nLop = 1; if (nLop > 8) nLop = 8;" + NL
           + "\t\tif (bpp >= 4 && nLop > 4) nLop = 4;" + NL
           + "\t\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci));" + NL
           + "\t\tci.type = SDL_GPU_TEXTURETYPE_2D_ARRAY; ci.format = fmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;" + NL
           + "\t\tci.width = m_pageSize; ci.height = m_pageSize; ci.layer_count_or_depth = nLop; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;" + NL
           + "\t\tconst Uint64 u0 = SDL_GetPerformanceCounter();" + NL
           + "\t\tSDL_GPUTexture* pTex = SDL_CreateGPUTexture(m_pDev->m_pGpu, &ci);" + NL
           + "\t\tif (!pTex) { RgLog(\"[KHOITRUOC] cap san khoi fmt %d that bai: %s\", (int)fmt, SDL_GetError()); break; }" + NL
           + "\t\tconst double dTao = (double)(SDL_GetPerformanceCounter() - u0) * 1000.0 / (double)SDL_GetPerformanceFrequency();" + NL
           + "\t\tJxKhoi k; k.pTex = pTex; k.fmt = fmt; k.bpp = bpp; k.nLop = nLop; k.nDung = 0;" + NL
           + "\t\tm_jxKhoiV.push_back(k);" + NL
           + "\t\tg_uJxKhoiSo = (unsigned)m_jxKhoiV.size();" + NL
           + "\t\tg_uJxKhoiMB += (unsigned)(((unsigned __int64)m_pageSize * m_pageSize * bpp * nLop) >> 20);" + NL
           + "\t\tfor (UINT l = 0; l < nLop; l++) m_pDev->QueueZeroUpload(pTex, 0, 0, m_pageSize, m_pageSize, bpp, l, fmt, SDL_GPU_TEXTURETYPE_2D_ARRAY);\t// to 0 = chep GPU tu dai nguon 0, cham het bo nho" + NL
           + "\t\tRgLog(\"[KHOITRUOC] cap san khoi atlas #%u: %ux%u x %u lop fmt %d (%u MB, tao %.1f ms) -> khe sampler %u; to 0 ca %u lop; tong %u khoi, %u MB\"," + NL
           + "\t\t\t(unsigned)(m_jxKhoiV.size() - 1), m_pageSize, m_pageSize, nLop, (int)fmt, (unsigned)(((unsigned __int64)m_pageSize * m_pageSize * bpp * nLop) >> 20), dTao," + NL
           + "\t\t\t(unsigned)(2 + m_jxKhoiV.size() - 1), nLop, g_uJxKhoiSo, g_uJxKhoiMB);" + NL
           + "\t\tnTao++;" + NL
           + "\t}" + NL
           + "\treturn nTao;" + NL
           + "}" + NL)
    return thay(s, cu, moi, "JxKhoiCapTruoc dinh nghia")


def v_dev(s):
    cu = "\tif (pDev && g_nRep3ApiOn == 100) ((CDevGpu*)pDev)->JxTaiTruocChay(uNganSach);" + NL + "}" + NL
    moi = (cu
           + "// " + DAU + " KRepresentShell3::RepresentBegin (khung dau): cap san khoi atlas + to 0 (chep GPU) de khong giat giua tran khi can khoi moi" + NL
           + "void Rep3Gpu_KhoiCapTruoc(IDirect3DDevice9* pDev, int nPal, int n32)" + NL
           + "{" + NL
           + "\tif (!pDev || g_nRep3ApiOn != 100) return;" + NL
           + "\tCDevGpu* d = (CDevGpu*)pDev;" + NL
           + "\tif (!d->m_pAtlas || !g_nJxAtlasKhoi) return;" + NL
           + "\tconst int a = (nPal > 0) ? d->m_pAtlas->JxKhoiCapTruoc(SDL_GPU_TEXTUREFORMAT_R8G8_UNORM, 2, nPal) : 0;" + NL
           + "\tconst int b = (n32 > 0) ? d->m_pAtlas->JxKhoiCapTruoc(SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, 4, n32) : 0;" + NL
           + "\tRgLog(\"[KHOITRUOC] da cap san %d khoi R8G8 (bang mau) + %d khoi BGRA8 (Rep3KhoiTruocPal=%d, Rep3KhoiTruoc32=%d)\", a, b, nPal, n32);" + NL
           + "}" + NL)
    return thay(s, cu, moi, "Rep3Gpu_KhoiCapTruoc")


# ================================================================ Represent3 shell: bien, ini, goi, [VE-TAI]; C: PAKBAN
def v_base(s):
    cu = "extern int g_nJxNapKhungToKB; extern unsigned g_uJxNapKhungTo;\t// [NAPTO 14/09] [Client] NapKhungToKB: khung co nen >= nguong (KB) thi giao luong nen ngay ca khi con ngan sach dong bo (0 = tat); so lan" + NL
    moi = cu + "extern volatile int g_nJxNenDocPak; extern unsigned g_uJxNapKhungPakBan;\t// " + DAUP + " luong nen dang doc pak (giu khoa tep) -> luong ve giao khung thay vi doi khoa; so lan" + NL
    return thay(s, cu, moi, "extern PAKBAN")


def v_shell(s):
    cu = "int g_nJxNapKhungToKB = 128; unsigned g_uJxNapKhungTo = 0;\t// [NAPTO 14/09] [Client] NapKhungToKB: khung nen >= nguong giao luong nen ngay (Fold 7: rut dong bo 1 khung to = 24-72 ms)" + NL
    moi = (cu + "volatile int g_nJxNenDocPak = 0; unsigned g_uJxNapKhungPakBan = 0;\t// " + DAUP + NL
           + "int g_nJxKhoiTruocPal = 4, g_nJxKhoiTruoc32 = 1;\t// " + DAU + " [Client] Rep3KhoiTruocPal / Rep3KhoiTruoc32: so khoi atlas cap san o khung dau (R8G8 64 MB / BGRA8 64 MB moi khoi)" + NL)
    s = thay(s, cu, moi, "bien PAKBAN/KHOITRUOC")
    cu = "\tg_nJxNapKhungToKB  = Rep3Ini(\"NapKhungToKB\", 128);\t// [NAPTO 14/09] khung chua rut co co nen >= KB nay -> giao luong nen ngay, bo ve 1-3 khung; 0 = tat (rut dong bo khi con ngan sach nhu cu)" + NL
    moi = (cu + "\tg_nJxKhoiTruocPal  = Rep3Ini(\"Rep3KhoiTruocPal\", 4);\t// " + DAU + " so khoi R8G8 cap san (0 = tat)" + NL
           + "\tg_nJxKhoiTruoc32   = Rep3Ini(\"Rep3KhoiTruoc32\", 1);\t// so khoi BGRA8 cap san" + NL)
    s = thay(s, cu, moi, "ini KHOITRUOC")
    cu = "\t{ extern void Rep3Gpu_TaiTruocChay(IDirect3DDevice9*, unsigned); if (g_nJxNapKhungKB > 0) Rep3Gpu_TaiTruocChay(PD3DDEVICE, (unsigned)g_nJxNapKhungKB << 10); }\t// [TAI 14/09] tai dan khung nap truoc len GPU" + NL
    moi = (cu + "\t{ static int s_nJxKhoiTruoc = 0; if (!s_nJxKhoiTruoc) { s_nJxKhoiTruoc = 1; extern void Rep3Gpu_KhoiCapTruoc(IDirect3DDevice9*, int, int); Rep3Gpu_KhoiCapTruoc(PD3DDEVICE, g_nJxKhoiTruocPal, g_nJxKhoiTruoc32); } }\t// " + DAU + " khung dau: cap san khoi atlas + to 0" + NL)
    s = thay(s, cu, moi, "RepresentBegin KHOITRUOC")
    cu = "| khung to (>= %d KB) giao nen thay vi rut dong bo: %u\","
    moi = "| khung to (>= %d KB) giao nen thay vi rut dong bo: %u | pak ban giao nen: %u\","
    s = thay(s, cu, moi, "VE-TAI format")
    cu = "g_uJxDemSo, g_uJxDemKB, g_nJxNapKhungToKB, g_uJxNapKhungTo);" + NL
    moi = "g_uJxDemSo, g_uJxDemKB, g_nJxNapKhungToKB, g_uJxNapKhungTo, g_uJxNapKhungPakBan);" + NL
    s = thay(s, cu, moi, "VE-TAI args")
    cu = "g_uJxDemSo = g_uJxDemKB = 0; g_uJxNapKhungTo = 0;" + NL
    moi = "g_uJxDemSo = g_uJxDemKB = 0; g_uJxNapKhungTo = 0; g_uJxNapKhungPakBan = 0;" + NL
    s = thay(s, cu, moi, "VE-TAI reset")
    return s


def v_tres(s):
    cu = ("\t\tif (g_nJxNapKhungToKB > 0 && !m_pFrameInfo[nFrame].pRawData && m_pHeader && m_pOffset && m_pFrameInfo[nFrame].nJxNen != 2)" + NL
          + "\t\t{\t// [NAPTO 14/09] khung TO chua rut (co nen >= NapKhungToKB): giao luong nen NGAY du con ngan sach - rut dong bo mot khung to = 24-72 ms tren Fold 7" + NL
          + "\t\t\t// (ngan sach NapKhungMs chi kiem TRUOC khi rut nen khong chan duoc). Bo ve khung nay 1-3 khung nhu 'bo ve' khi het ngan sach." + NL
          + "\t\t\tint nLTo = (int)m_pOffset[nFrame].Length; if (nLTo < 0) nLTo = -nLTo;" + NL
          + "\t\t\tif (nLTo >= g_nJxNapKhungToKB * 1024 && (m_pFrameInfo[nFrame].nJxNen == 1 || JxNapKhungGiao(nFrame, 0)))" + NL
          + "\t\t\t{" + NL
          + "\t\t\t\tJxNapKhungTruoc(nFrame);" + NL
          + "\t\t\t\tg_uJxNapKhungBoVe++; g_uJxNapKhungBoVeKhung++; g_uJxNapKhungTo++; g_nJxAnhBoVeNen = 1; return false;" + NL
          + "\t\t\t}" + NL
          + "\t\t}" + NL)
    moi = ("\t\tif (!m_pFrameInfo[nFrame].pRawData && m_pHeader && m_pOffset && m_pFrameInfo[nFrame].nJxNen != 2)" + NL
           + "\t\t{\t// [NAPTO 14/09] khung TO chua rut (co nen >= NapKhungToKB): giao luong nen NGAY du con ngan sach - rut dong bo mot khung to = 24-72 ms tren Fold 7" + NL
           + "\t\t\t// (ngan sach NapKhungMs chi kiem TRUOC khi rut nen khong chan duoc). Bo ve khung nay 1-3 khung nhu 'bo ve' khi het ngan sach." + NL
           + "\t\t\t// " + DAUP + " luong nen dang doc pak (giu khoa tep): rut dong bo se doi khoa 15-97 ms cho mot khung 1 KB ([NAP-CHAM] Fold 7) -> cung giao." + NL
           + "\t\t\tint nLTo = (int)m_pOffset[nFrame].Length; if (nLTo < 0) nLTo = -nLTo;" + NL
           + "\t\t\tconst bool bPakBan = (g_nJxNenDocPak != 0);" + NL
           + "\t\t\tif ((bPakBan || (g_nJxNapKhungToKB > 0 && nLTo >= g_nJxNapKhungToKB * 1024)) && (m_pFrameInfo[nFrame].nJxNen == 1 || JxNapKhungGiao(nFrame, 0)))" + NL
           + "\t\t\t{" + NL
           + "\t\t\t\tJxNapKhungTruoc(nFrame);" + NL
           + "\t\t\t\tg_uJxNapKhungBoVe++; g_uJxNapKhungBoVeKhung++; if (bPakBan) g_uJxNapKhungPakBan++; else g_uJxNapKhungTo++; g_nJxAnhBoVeNen = 1; return false;" + NL
           + "\t\t\t}" + NL
           + "\t\t}" + NL)
    s = thay(s, cu, moi, "PrepareFrameData PAKBAN")
    cu = ("\t\tpFrame = (SPRFRAME*)SprGetFrame((SPRHEAD*)m_pHeader, nFrame);" + NL + "\t\tif (!pFrame)" + NL + "\t\t\treturn false;" + NL
          + "\t\tint nL = (int)m_pOffset[nFrame].Length;")
    moi = ("\t\tg_nJxNenDocPak = 1;\t// " + DAUP + " dang giu khoa pak cua tep nay" + NL
           + "\t\tpFrame = (SPRFRAME*)SprGetFrame((SPRHEAD*)m_pHeader, nFrame);" + NL
           + "\t\tg_nJxNenDocPak = 0;" + NL
           + "\t\tif (!pFrame)" + NL + "\t\t\treturn false;" + NL
           + "\t\tint nL = (int)m_pOffset[nFrame].Length;")
    s = thay(s, cu, moi, "JxGiaiMaNen PAKBAN")
    return s


def v_tmgr(s):
    cu = "\t\t\tkq.pRes = NapNenTai(v.szTen, v.nType);" + NL
    moi = ("#ifdef JX_MOBILE" + NL + "\t\t\tg_nJxNenDocPak = 1;\t// " + DAUP + " nap ca tep spr o luong nen: giu khoa pak" + NL + "#endif" + NL
           + cu
           + "#ifdef JX_MOBILE" + NL + "\t\t\tg_nJxNenDocPak = 0;" + NL + "#endif" + NL)
    return thay(s, cu, moi, "NapNenChay PAKBAN")


# ================================================================ B. MANG
def v_net(s):
    s = sau_dong(s, "g_bDisconnect;", "#ifdef JX_MOBILE" + NL + "extern int g_nJxMangMs; extern int g_nPaintLog; static unsigned s_uJxMangCat = 0; static DWORD s_dwJxMangLog = 0;\t// " + DAUM + " ngan sach xu ly goi moi vong lap (S3Client.cpp doc [Client] MangMs)" + NL + "#endif" + NL, "extern MangMs")
    cu = "\tconst char* pBuffer = NULL;" + NL + NL + "\tif (m_bIsClientConnecting)" + NL
    moi = ("\tconst char* pBuffer = NULL;" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\tLARGE_INTEGER liJxM0, liJxMF; QueryPerformanceCounter(&liJxM0); QueryPerformanceFrequency(&liJxMF); int nJxGoi = 0; bool bJxCat = false;\t// " + DAUM + NL
           + "#endif" + NL + NL
           + "\tif (m_bIsClientConnecting)" + NL)
    s = thay(s, cu, moi, "moc mang")
    kiem = ("#ifdef JX_MOBILE" + NL
            + "\t\t\tnJxGoi++;\t// " + DAUM + " het ngan sach MangMs: de goi con lai sang vong lap sau (thu tu giu nguyen, tre <= 1 vong)" + NL
            + "\t\t\tif (g_nJxMangMs > 0) { LARGE_INTEGER liJxT; QueryPerformanceCounter(&liJxT); if (liJxMF.QuadPart && (double)(liJxT.QuadPart - liJxM0.QuadPart) * 1000.0 / (double)liJxMF.QuadPart >= (double)g_nJxMangMs) { bJxCat = true; break; } }" + NL
            + "#endif" + NL)
    cu = "\t\t\t\t(m_MsgTargetObjs[Msg])->AcceptNetMsg(pMsg);" + NL + "\t\t}" + NL + "\t}" + NL
    moi = "\t\t\t\t(m_MsgTargetObjs[Msg])->AcceptNetMsg(pMsg);" + NL + kiem + "\t\t}" + NL + "\t}" + NL
    s = thay(s, cu, moi, "vong 1")
    cu = ("PROTOCOL_MSG_SIZE)));" + NL + "\t\t\t\t}" + NL + NL + "\t\t\t}" + NL + "\t\t}" + NL + "\t}" + NL + "}" + NL + NL
          + "void KNetConnectAgent::RegisterMsgTargetObject(")
    moi = ("PROTOCOL_MSG_SIZE)));" + NL + "\t\t\t\t}" + NL + NL + "\t\t\t}" + NL
           + kiem
           + "\t\t}" + NL + "\t}" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\tif (bJxCat)" + NL
           + "\t{\t// " + DAUM + " ghi toi da 1 dong/giay khi cat" + NL
           + "\t\ts_uJxMangCat++;" + NL
           + "\t\tconst DWORD dwJxNow = GetTickCount();" + NL
           + "\t\tif (g_nPaintLog > 0 && dwJxNow - s_dwJxMangLog >= 1000)" + NL
           + "\t\t{" + NL
           + "\t\t\ts_dwJxMangLog = dwJxNow;" + NL
           + "\t\t\tLARGE_INTEGER liJxT; QueryPerformanceCounter(&liJxT);" + NL
           + "\t\t\tFILE* pJxLog = fopen(\"jx_paint.log\", \"a\");" + NL
           + "\t\t\tif (pJxLog) { fprintf(pJxLog, \"[MANG-CAT] t=%u xu ly %d goi trong %.1f ms (MangMs=%d), con lai de vong sau; tong cat %u\\n\", dwJxNow, nJxGoi, liJxMF.QuadPart ? (double)(liJxT.QuadPart - liJxM0.QuadPart) * 1000.0 / (double)liJxMF.QuadPart : 0.0, g_nJxMangMs, s_uJxMangCat); fclose(pJxLog); }" + NL
           + "\t\t}" + NL
           + "\t}" + NL
           + "#endif" + NL
           + "}" + NL + NL
           + "void KNetConnectAgent::RegisterMsgTargetObject(")
    s = thay(s, cu, moi, "vong 2 + log")
    return s


def v_s3(s):
    cu = "int\tg_nPaintLog = 0;\t\t// config.ini [Client] PaintLog; 1 = write jx_paint.log frame-time probe" + NL
    moi = cu + "#ifdef JX_MOBILE" + NL + "int g_nJxMangMs = 10;\t// " + DAUM + " config.ini [Client] MangMs: ngan sach xu ly goi mang moi vong lap (ms); 0 = xu ly het nhu cu (NetConnectAgent.cpp)" + NL + "#endif" + NL
    s = thay(s, cu, moi, "g_nJxMangMs")
    cu = "\tIniFile.GetInteger(\"Client\", \"PaintLog\", 0, &g_nPaintLog);" + NL
    moi = cu + "#ifdef JX_MOBILE" + NL + "\tIniFile.GetInteger(\"Client\", \"MangMs\", 10, &g_nJxMangMs);\t// " + DAUM + NL + "#endif" + NL
    s = thay(s, cu, moi, "ini MangMs")
    return s


# ================================================================ D. PDET-UI
def v_wnds(s):
    cu = "double g_dJxPhaVe[5] = { 0, 0, 0, 0, 0 };" + NL
    moi = cu + "double g_dJxUiMaxMs = 0.0; int g_nJxUiMaxX = 0, g_nJxUiMaxY = 0, g_nJxUiMaxW = 0, g_nJxUiMaxH = 0; const char* g_pszJxUiMaxLop = NULL; const void* g_pJxUiMax = NULL;\t// " + DAUU + " cua so goc ve lau nhat trong khung (WndWindow.cpp ghi, UiShell.cpp in [PDET-UI])" + NL
    s = thay(s, cu, moi, "bien PDET-UI")
    cu = "\tif (bJxDo) QueryPerformanceCounter(&jxT[0]);" + NL + "#endif" + NL
    moi = "\tif (bJxDo) QueryPerformanceCounter(&jxT[0]);" + NL + "\tg_dJxUiMaxMs = 0.0; g_pJxUiMax = NULL; g_pszJxUiMaxLop = NULL;\t// " + DAUU + NL + "#endif" + NL
    s = thay(s, cu, moi, "dat lai PDET-UI")
    return s


def v_wndwindow(s):
    cu = ("\t{" + NL + "\t\tPaintWindow();" + NL + "\t\tif (m_pFirstChild)" + NL + "\t\t\tm_pFirstChild->Paint();" + NL + "\t}" + NL
          + "\tif (m_pNextWnd)" + NL + "\t\tm_pNextWnd->Paint();" + NL + "}" + NL)
    moi = ("\t{" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\t// " + DAUU + " cua so GOC (con truc tiep cua goc lop): do rieng (ban than + con, khong ke anh em) -> giu cua so lau nhat cua khung" + NL
           + "\t\textern int g_nPaintLog; extern double g_dJxUiMaxMs; extern int g_nJxUiMaxX, g_nJxUiMaxY, g_nJxUiMaxW, g_nJxUiMaxH; extern const char* g_pszJxUiMaxLop; extern const void* g_pJxUiMax;" + NL
           + "\t\tconst bool bJxDoUi = (g_nPaintLog > 0 && m_pParentWnd && !m_pParentWnd->m_pParentWnd);" + NL
           + "\t\tLARGE_INTEGER jxU0; if (bJxDoUi) QueryPerformanceCounter(&jxU0);" + NL
           + "#endif" + NL
           + "\t\tPaintWindow();" + NL + "\t\tif (m_pFirstChild)" + NL + "\t\t\tm_pFirstChild->Paint();" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\tif (bJxDoUi)" + NL
           + "\t\t{" + NL
           + "\t\t\tLARGE_INTEGER jxU1, jxUF; QueryPerformanceCounter(&jxU1); QueryPerformanceFrequency(&jxUF);" + NL
           + "\t\t\tconst double dJx = jxUF.QuadPart ? (double)(jxU1.QuadPart - jxU0.QuadPart) * 1000.0 / (double)jxUF.QuadPart : 0.0;" + NL
           + "\t\t\tif (dJx > g_dJxUiMaxMs && (m_Width > 0 || m_Height > 0)) { g_dJxUiMaxMs = dJx; g_pJxUiMax = this; g_nJxUiMaxX = m_nAbsoluteLeft; g_nJxUiMaxY = m_nAbsoluteTop; g_nJxUiMaxW = m_Width; g_nJxUiMaxH = m_Height; g_pszJxUiMaxLop = UiTenLopPhu(); }" + NL
           + "\t\t}" + NL
           + "#endif" + NL
           + "\t}" + NL
           + "\tif (m_pNextWnd)" + NL + "\t\tm_pNextWnd->Paint();" + NL + "}" + NL)
    return thay(s, cu, moi, "KWndWindow::Paint")


def v_uishell(s):
    cu = "\t\t\t\t\tg_dJxPhaCanh[7], g_dJxPhaCanh[8], g_dJxPhaCanh[9], g_dJxPhaCanh[10]);" + NL + "#else" + NL
    moi = ("\t\t\t\t\tg_dJxPhaCanh[7], g_dJxPhaCanh[8], g_dJxPhaCanh[9], g_dJxPhaCanh[10]);" + NL
           + "\t\t\t\tif (g_dJxPhaVe[2] + g_dJxPhaVe[3] + g_dJxPhaVe[4] >= 20.0)" + NL
           + "\t\t\t\t{\t// " + DAUU + " ba lop UI >= 20 ms: cua so goc ve lau nhat (vi tri/co theo man hinh, lop phu neu co) de biet cua so nao nang" + NL
           + "\t\t\t\t\textern double g_dJxUiMaxMs; extern int g_nJxUiMaxX, g_nJxUiMaxY, g_nJxUiMaxW, g_nJxUiMaxH; extern const char* g_pszJxUiMaxLop; extern const void* g_pJxUiMax;" + NL
           + "\t\t\t\t\tfprintf(pLog, \"[PDET-UI] cua so goc nang nhat %.1f ms: tai (%d,%d) %dx%d lop phu %s con tro %p\\n\", g_dJxUiMaxMs, g_nJxUiMaxX, g_nJxUiMaxY, g_nJxUiMaxW, g_nJxUiMaxH, g_pszJxUiMaxLop ? g_pszJxUiMaxLop : \"-\", g_pJxUiMax);" + NL
           + "\t\t\t\t}" + NL
           + "#else" + NL)
    return thay(s, cu, moi, "PDET-UI in")


va("Sources/Represent/Represent3/D3D9onGPUi.h", v_gpui)
va("Sources/Represent/Represent3/D3D9onGPURes.cpp", v_res_gpu)
va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", v_dev)
va("Sources/Represent/Represent3/BaseInclude.h", v_base)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", v_shell)
va("Sources/Represent/Represent3/TextureRes.cpp", v_tres)
va("Sources/Represent/Represent3/TextureResMgr.cpp", v_tmgr)
va("Sources/S3Client/NetConnect/NetConnectAgent.cpp", v_net)
va("Sources/S3Client/S3Client.cpp", v_s3)
va("Sources/S3Client/Ui/Elem/Wnds.cpp", v_wnds)
va("Sources/S3Client/Ui/Elem/WndWindow.cpp", v_wndwindow)
va("Sources/S3Client/Ui/UiShell.cpp", v_uishell)
