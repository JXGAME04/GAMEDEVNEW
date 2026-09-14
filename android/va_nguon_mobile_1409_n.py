# -*- coding: utf-8 -*-
# Dot 14/09 16:xx (chu 'oke tiep tuc' sau log 14:47, ban 109141405):
#   1. [PAKTHU] XPackFile::GetSprFrame: khoa CHUNG ms_ReadCritical (mot khoa cho moi pak) - luong ve rut khung o che do THU (thread_local):
#      khoa dang bi luong nen giu (nap ca tep / rut khung) thi tra NULL + bao 'ban' -> PrepareFrameData giao khung cho luong nen (bo ve 1-3 khung);
#      khoa ranh thi rut ngay nhu cu. Thay co tho [PAKBAN] (hoan thua 558-1448 khung/30 s, 'bo ve' 700-1000/30 s). Co [PAKBAN] van dat nhung khong dung.
#   2. [MANG-CHAM] NetConnectAgent::Breathe: mot goi xu ly >= 8 ms -> [MANG-CHAM] t, msg id, ms (jx_paint.log, <= 4 dong/giay) de biet handler nao nang.
#   3. [PDET-UI b] cua so goc = ANH EM cua goc lop (Wnd_AddWindow -> AddBrother, parent NULL): sua dieu kien do.
#   4. [KHOIDUTRU] doi map (GameSpaceChangedNotify GDCNI_SWITCHING_MAPMODE) -> Rep3_KhoiDuTru(8): lop trong R8G8 < 8 (hoac BGRA8 < 2) thi cap them mot khoi
#      + to 0 ngay trong man nap (khong giat giua tran); dang ky ky hieu iOS. Config dt_v4: Rep3KhoiTruocPal=6.
# Chi JX_MOBILE, ban PC khong doi. Chay lai vo hai (chay sau _m.py).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[PAKTHU 14/09]"
DAUM = "[MANG-CHAM 14/09]"
DAUU = "[PDET-UI 14/09 b]"
DAUK = "[KHOIDUTRU 14/09]"
NL = "\r\n"


def doc(p, nl=NL):
    s = io.open(p, encoding="latin-1", newline="").read()
    assert nl in s, p
    return s


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def cao(s):
    return sum(1 for c in s if ord(c) >= 128)


def thay(s, cu, moi, ten, n=1):
    if s.count(cu) != n:
        raise SystemExit("%s: neo '%s...' thay %d lan, can %d" % (ten, cu.strip()[:70], s.count(cu), n))
    return s.replace(cu, moi)


def sau_dong(s, neo, them, ten, nl=NL):
    if s.count(neo) != 1:
        raise SystemExit("%s: neo '%s...' thay %d lan" % (ten, neo[:70], s.count(neo)))
    i = s.find(neo)
    j = s.find(nl, i) + len(nl)
    return s[:j] + them + s[j:]


def va(p, viec, nl=NL):
    s = doc(p, nl)
    if DAU in s or DAUM in s or DAUU in s or DAUK in s:
        print("da va roi:", p)
        return
    c0 = cao(s)
    s = viec(s)
    assert cao(s) == c0, p + ": byte cao doi"
    ghi(p, s)
    print("da va:", p)


# ================================================================ 1. PAKTHU (Engine)
def v_xpack(s):
    cu = "#define LeaveCriticalSection(p)       SDL_UnlockMutex(*(SDL_Mutex**)(p))" + NL
    moi = (cu + "#ifdef JX_MOBILE" + NL
           + "// " + DAU + " luong ve rut khung dong bo o che do THU: khoa CHUNG ms_ReadCritical (mot khoa cho MOI pak) dang bi luong nen giu (nap ca tep / rut khung)" + NL
           + "// thi khong doi (Fold 7: khung 1 KB doi 15-97 ms) ma tra NULL + bao 'ban' de PrepareFrameData giao khung cho luong nen. Co theo luong." + NL
           + "static thread_local int t_nJxPakThu = 0, t_nJxPakThuBan = 0;" + NL
           + "void XPack_JxThu(int bBat) { t_nJxPakThu = bBat; if (bBat) t_nJxPakThuBan = 0; }" + NL
           + "int XPack_JxThuBan() { return t_nJxPakThuBan; }" + NL
           + "#endif" + NL)
    s = thay(s, cu, moi, "XPack_JxThu")
    cu = "\t\tEnterCriticalSection(&ms_ReadCritical);" + NL + "\t\t// [PAK 12/09 CHISOMUC]"
    moi = ("#ifdef JX_MOBILE" + NL
           + "\t\tif (t_nJxPakThu) { if (!SDL_TryLockMutex(*(SDL_Mutex**)&ms_ReadCritical)) { t_nJxPakThuBan = 1; return NULL; } }\t// " + DAU + " khoa ban -> khong doi" + NL
           + "\t\telse EnterCriticalSection(&ms_ReadCritical);" + NL
           + "#else" + NL
           + "\t\tEnterCriticalSection(&ms_ReadCritical);" + NL
           + "#endif" + NL
           + "\t\t// [PAK 12/09 CHISOMUC]")
    s = thay(s, cu, moi, "GetSprFrame thu khoa")
    return s


def v_pakh(s):
    cu = "\tENGINE_API void\t\t\tSprReleaseFrame(SPRFRAME* pFrame);" + NL
    moi = (cu + "#ifdef JX_MOBILE" + NL
           + "\tENGINE_API void\t\t\tXPack_JxThu(int bBat);\t\t// " + DAU + " 1 = SprGetFrame o luong nay chi THU khoa pak (ban -> NULL), 0 = doi nhu cu" + NL
           + "\tENGINE_API int\t\t\tXPack_JxThuBan();\t\t\t// 1 = lan SprGetFrame vua roi tra NULL vi khoa ban" + NL
           + "#endif" + NL)
    return thay(s, cu, moi, "KPakFile.h khai bao")


def v_tres(s):
    cu = "\t\t\tconst bool bPakBan = (g_nJxNenDocPak != 0);" + NL
    moi = "\t\t\tconst bool bPakBan = false;\t// " + DAU + " co tho [PAKBAN] khong dung nua (hoan thua): thay bang thu khoa dung mutex o SprGetFrame ben duoi" + NL
    s = thay(s, cu, moi, "bo co tho PAKBAN")
    cu = ("\t\tLARGE_INTEGER liK0, liK1; QueryPerformanceCounter(&liK0);\t// [NAP 08/09 a] rut khung tu pak" + NL
          + "\t\tSPRFRAME *pFrame = (SPRFRAME *)SprGetFrame((SPRHEAD*)m_pHeader, nFrame);" + NL
          + "\t\tQueryPerformanceCounter(&liK1); Rep3NapCong(g_napKhung, Rep3NapMs(liK0, liK1));" + NL + NL
          + "\t\tif(!pFrame)" + NL + "\t\t\treturn false;" + NL)
    moi = ("\t\tLARGE_INTEGER liK0, liK1; QueryPerformanceCounter(&liK0);\t// [NAP 08/09 a] rut khung tu pak" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\t// " + DAU + " dang ve + co luong nen: rut o che do THU - khoa chung cua pak dang bi luong nen giu thi giao khung cho luong nen (bo ve 1-3 khung)" + NL
           + "\t\t// thay vi doi 15-97 ms (Fold 7 [NAP-CHAM]); khoa ranh thi rut ngay nhu cu (khong hoan thua nhu co tho [PAKBAN])." + NL
           + "\t\tconst bool bJxThu = (bPrepareTex && g_nJxNapKhungNen > 0 && g_pJxTexMgr && g_pJxTexMgr->m_bVeDangDien && m_pFrameInfo[nFrame].nJxNen != 2);" + NL
           + "\t\tif (bJxThu) XPack_JxThu(1);" + NL
           + "\t\tSPRFRAME *pFrame = (SPRFRAME *)SprGetFrame((SPRHEAD*)m_pHeader, nFrame);" + NL
           + "\t\tif (bJxThu)" + NL
           + "\t\t{" + NL
           + "\t\t\tXPack_JxThu(0);" + NL
           + "\t\t\tif (!pFrame && XPack_JxThuBan())" + NL
           + "\t\t\t{" + NL
           + "\t\t\t\tif (m_pFrameInfo[nFrame].nJxNen == 1 || JxNapKhungGiao(nFrame, 0))" + NL
           + "\t\t\t\t{" + NL
           + "\t\t\t\t\tQueryPerformanceCounter(&liK1); Rep3NapCong(g_napKhung, Rep3NapMs(liK0, liK1));" + NL
           + "\t\t\t\t\tJxNapKhungTruoc(nFrame);" + NL
           + "\t\t\t\t\tg_uJxNapKhungBoVe++; g_uJxNapKhungBoVeKhung++; g_uJxNapKhungPakBan++; g_nJxAnhBoVeNen = 1; return false;" + NL
           + "\t\t\t\t}" + NL
           + "\t\t\t\tpFrame = (SPRFRAME *)SprGetFrame((SPRHEAD*)m_pHeader, nFrame);\t// khong giao duoc (hang day / hong): doi khoa nhu cu" + NL
           + "\t\t\t}" + NL
           + "\t\t}" + NL
           + "#else" + NL
           + "\t\tSPRFRAME *pFrame = (SPRFRAME *)SprGetFrame((SPRHEAD*)m_pHeader, nFrame);" + NL
           + "#endif" + NL
           + "\t\tQueryPerformanceCounter(&liK1); Rep3NapCong(g_napKhung, Rep3NapMs(liK0, liK1));" + NL + NL
           + "\t\tif(!pFrame)" + NL + "\t\t\treturn false;" + NL)
    s = thay(s, cu, moi, "PrepareFrameData thu khoa")
    return s


# ================================================================ 2. MANG-CHAM
def v_net(s):
    cu = "extern int g_nJxMangMs; extern int g_nPaintLog; static unsigned s_uJxMangCat = 0; static DWORD s_dwJxMangLog = 0;\t// [MANG 14/09] ngan sach xu ly goi moi vong lap (S3Client.cpp doc [Client] MangMs)" + NL
    moi = (cu
           + "// " + DAUM + " mot goi xu ly >= 8 ms (handler nap tep / dong bo dong bo...) -> ghi id goi + ms, toi da 4 dong/giay, de biet handler nao nang" + NL
           + "static void JxMangCham(int nMsg, const LARGE_INTEGER& a, const LARGE_INTEGER& f)" + NL
           + "{" + NL
           + "\tif (g_nPaintLog <= 0 || !f.QuadPart) return;" + NL
           + "\tLARGE_INTEGER b; QueryPerformanceCounter(&b);" + NL
           + "\tconst double d = (double)(b.QuadPart - a.QuadPart) * 1000.0 / (double)f.QuadPart;" + NL
           + "\tif (d < 8.0) return;" + NL
           + "\tstatic DWORD s_dwGiay = 0; static int s_nTrongGiay = 0;" + NL
           + "\tconst DWORD dwNow = GetTickCount();" + NL
           + "\tif (dwNow - s_dwGiay >= 1000) { s_dwGiay = dwNow; s_nTrongGiay = 0; }" + NL
           + "\tif (++s_nTrongGiay > 4) return;" + NL
           + "\tFILE* p = fopen(\"jx_paint.log\", \"a\");" + NL
           + "\tif (p) { fprintf(p, \"[MANG-CHAM] t=%u msg=%d %.1f ms\\n\", dwNow, nMsg, d); fclose(p); }" + NL
           + "}" + NL)
    s = thay(s, cu, moi, "JxMangCham")
    cu = "\t\t\tif (m_MsgTargetObjs[Msg])" + NL + "\t\t\t\t(m_MsgTargetObjs[Msg])->AcceptNetMsg(pMsg);" + NL
    moi = ("#ifdef JX_MOBILE" + NL + "\t\t\tLARGE_INTEGER liJxG0; QueryPerformanceCounter(&liJxG0);\t// " + DAUM + NL + "#endif" + NL
           + cu
           + "#ifdef JX_MOBILE" + NL + "\t\t\tJxMangCham((int)Msg, liJxG0, liJxMF);" + NL + "#endif" + NL)
    s = thay(s, cu, moi, "vong 1 cham")
    cu = "\t\t\t\t\tg_pCoreShell->NetMsgCallbackFunc(pMsg);" + NL
    moi = ("#ifdef JX_MOBILE" + NL + "\t\t\t\t\tLARGE_INTEGER liJxG0; QueryPerformanceCounter(&liJxG0);\t// " + DAUM + NL + "#endif" + NL
           + cu
           + "#ifdef JX_MOBILE" + NL + "\t\t\t\t\tJxMangCham((int)Msg, liJxG0, liJxMF);" + NL + "#endif" + NL)
    s = thay(s, cu, moi, "vong 2 cham")
    return s


# ================================================================ 3. PDET-UI b
def v_wndwindow(s):
    cu = "\t\tconst bool bJxDoUi = (g_nPaintLog > 0 && m_pParentWnd && !m_pParentWnd->m_pParentWnd);" + NL
    moi = "\t\tconst bool bJxDoUi = (g_nPaintLog > 0 && (!m_pParentWnd || !m_pParentWnd->m_pParentWnd));\t// " + DAUU + " cua so goc = ANH EM cua goc lop (Wnd_AddWindow -> AddBrother, parent NULL); cap con truc tiep van do, cha thang vi gom con" + NL
    return thay(s, cu, moi, "PDET-UI dieu kien")


# ================================================================ 4. KHOIDUTRU
def v_gpui(s):
    cu = "\tint JxKhoiCapTruoc(SDL_GPUTextureFormat fmt, UINT bpp, int nSo);\t// [KHOITRUOC 14/09] cap san nSo khoi (chua dung lop nao) + to 0 het cac lop bang chep GPU; tra so khoi da tao" + NL
    moi = cu + "\tint JxKhoiLopTrong(SDL_GPUTextureFormat fmt) const;\t// " + DAUK + " so lop con trong (chua cap + da tra) cua cac khoi cung dinh dang" + NL
    return thay(s, cu, moi, "JxKhoiLopTrong khai bao")


def v_res_gpu(s):
    cu = "int CAtlasMgrGpu::JxKhoiCapTruoc(SDL_GPUTextureFormat fmt, UINT bpp, int nSo)" + NL
    moi = ("// " + DAUK + " so lop con trong cua cac khoi cung dinh dang (chua cap + da tra ve)" + NL
           + "int CAtlasMgrGpu::JxKhoiLopTrong(SDL_GPUTextureFormat fmt) const" + NL
           + "{" + NL
           + "\tint n = 0;" + NL
           + "\tfor (size_t i = 0; i < m_jxKhoiV.size(); i++) { const JxKhoi& k = m_jxKhoiV[i]; if (k.fmt == fmt) n += (int)(k.nLop - k.nDung) + (int)k.lopTrong.size(); }" + NL
           + "\treturn n;" + NL
           + "}" + NL + NL + cu)
    return thay(s, cu, moi, "JxKhoiLopTrong dinh nghia")


def v_dev(s):
    cu = "\tRgLog(\"[KHOITRUOC] da cap san %d khoi R8G8 (bang mau) + %d khoi BGRA8 (Rep3KhoiTruocPal=%d, Rep3KhoiTruoc32=%d)\", a, b, nPal, n32);" + NL + "}" + NL
    moi = (cu
           + "// " + DAUK + " doi map (GameSpaceChangedNotify -> Rep3_KhoiDuTru): lop trong R8G8 < nLopToiThieu hoac BGRA8 < 2 -> cap them MOT khoi + to 0 ngay trong man nap" + NL
           + "void Rep3Gpu_KhoiDuTru(IDirect3DDevice9* pDev, int nLopToiThieu)" + NL
           + "{" + NL
           + "\tif (!pDev || g_nRep3ApiOn != 100) return;" + NL
           + "\tCDevGpu* d = (CDevGpu*)pDev;" + NL
           + "\tif (!d->m_pAtlas || !g_nJxAtlasKhoi) return;" + NL
           + "\tconst int nPal = d->m_pAtlas->JxKhoiLopTrong(SDL_GPU_TEXTUREFORMAT_R8G8_UNORM), n32 = d->m_pAtlas->JxKhoiLopTrong(SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM);" + NL
           + "\tint a = 0, b = 0;" + NL
           + "\tif (nPal < nLopToiThieu) a = d->m_pAtlas->JxKhoiCapTruoc(SDL_GPU_TEXTUREFORMAT_R8G8_UNORM, 2, 1);" + NL
           + "\tif (n32 < 2) b = d->m_pAtlas->JxKhoiCapTruoc(SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, 4, 1);" + NL
           + "\tif (a || b) RgLog(\"[KHOIDUTRU] doi map: lop trong R8G8 %d (< %d) / BGRA8 %d -> cap them %d khoi R8G8 + %d khoi BGRA8\", nPal, nLopToiThieu, n32, a, b);" + NL
           + "}" + NL)
    return thay(s, cu, moi, "Rep3Gpu_KhoiDuTru")


def v_shell(s):
    cu = ("extern \"C\" __declspec(dllexport)" + NL + "int Rep3_JxTheGioi(int nLenh, int nThamSo)" + NL + "{" + NL
          + "\tif (!g_pRep3ShellDuyNhat) return 0;" + NL + "\treturn g_pRep3ShellDuyNhat->JxTheGioi(nLenh, nThamSo);" + NL + "}" + NL)
    moi = (cu
           + "// " + DAUK + " S3Client GameSpaceChangedNotify (doi map that) goi qua GetProcAddress; iOS dang ky trong ios/JxIosMain.cpp" + NL
           + "void Rep3Gpu_KhoiDuTru(IDirect3DDevice9*, int);\t// D3D9onGPUDev.cpp - khai bao NGOAI ham extern \"C\" (khai bao trong than ham extern \"C\" bi lien ket C -> undefined symbol)" + NL
           + "extern \"C\" __declspec(dllexport)" + NL
           + "int Rep3_KhoiDuTru(int nLopToiThieu)" + NL
           + "{" + NL
           + "\tif (!g_pRep3ShellDuyNhat || !PD3DDEVICE) return 0;" + NL
           + "\tRep3Gpu_KhoiDuTru(PD3DDEVICE, nLopToiThieu);" + NL
           + "\treturn 1;" + NL
           + "}" + NL)
    return thay(s, cu, moi, "Rep3_KhoiDuTru")


def v_gscn(s):
    cu = "\t\tJxLia_DatLai();\t// [LIA 13/09] doi map that (KScenePlaceC::OpenPlace) -> bo do lech lia canh, tat co FollowWithMap" + NL
    moi = (cu
           + "\t\t{\t// " + DAUK + " doi map: cap them khoi atlas neu lop trong < 8 (trong man nap, khong giat giua tran); Represent3 qua GetProcAddress nhu Rep3_JxTheGioi" + NL
           + "\t\t\ttypedef int (*PFN_JxKhoiDuTru)(int); static PFN_JxKhoiDuTru s_pfnKhoiDuTru = NULL; static int s_nKhoiDuTruThu = 0;" + NL
           + "\t\t\tif (!s_pfnKhoiDuTru && s_nKhoiDuTruThu < 8) { s_nKhoiDuTruThu++; HMODULE h = GetModuleHandleA(\"Represent3.dll\"); if (h) s_pfnKhoiDuTru = (PFN_JxKhoiDuTru)GetProcAddress(h, \"Rep3_KhoiDuTru\"); }" + NL
           + "\t\t\tif (s_pfnKhoiDuTru) s_pfnKhoiDuTru(8);" + NL
           + "\t\t}" + NL)
    return thay(s, cu, moi, "GameSpaceChangedNotify goi")


def v_ios(s):
    lf = "\n"
    cu = 'extern "C" int             Rep3_NenTruocKhung(const char*, int); // [NENTRUOC 13/09] Represent3/KRepresentShell3.cpp (KScenePlaceRegionC goi qua GetProcAddress)' + lf
    moi = cu + 'extern "C" int             Rep3_KhoiDuTru(int);              // ' + DAUK + ' Represent3/KRepresentShell3.cpp (GameSpaceChangedNotify goi qua GetProcAddress)' + lf
    s = thay(s, cu, moi, "ios extern")
    cu = '\tJxPosix_DangKyKyHieu("Represent3.dll", "Rep3_NenTruocKhung",   (void*)&Rep3_NenTruocKhung);   // [NENTRUOC 13/09]' + lf
    moi = cu + '\tJxPosix_DangKyKyHieu("Represent3.dll", "Rep3_KhoiDuTru",       (void*)&Rep3_KhoiDuTru);       // ' + DAUK + lf
    s = thay(s, cu, moi, "ios dang ky")
    return s


def v_cfg_ghi_de(s):
    lf = chr(10)  # dong nay trong tep ghi de ket thuc bang LF (tep tron CRLF/LF)
    cu = "Rep3DoVeChiTiet=0" + lf
    moi = cu + "; [KHOITRUOC 14/09] so khoi atlas R8G8 / BGRA8 cap san o khung dau (64 MB moi khoi) - Fold 7 9 phut dung 7 khoi; doi map cap them khi lop trong < 8" + lf + "Rep3KhoiTruocPal=6" + lf + "Rep3KhoiTruoc32=1" + lf
    return thay(s, cu, moi, "config ghi de")


va("Sources/Engine/Src/XPackFile.cpp", v_xpack)
va("Sources/Engine/Src/KPakFile.h", v_pakh)
va("Sources/Represent/Represent3/TextureRes.cpp", v_tres)
va("Sources/S3Client/NetConnect/NetConnectAgent.cpp", v_net)
va("Sources/S3Client/Ui/Elem/WndWindow.cpp", v_wndwindow)
va("Sources/Represent/Represent3/D3D9onGPUi.h", v_gpui)
va("Sources/Represent/Represent3/D3D9onGPURes.cpp", v_res_gpu)
va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", v_dev)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", v_shell)
va("Sources/S3Client/Ui/GameSpaceChangedNotify.cpp", v_gscn)
va("ios/JxIosMain.cpp", v_ios, "\n")
va("android/du_lieu_ghi_de/config.ini", v_cfg_ghi_de)
