# -*- coding: utf-8 -*-
# [TAI 14/09] P4: tai len GPU ton CPU ~20 ms/MB tren Fold 7 (log 13/09: "zero 1/116.8" = mot trang atlas 8 MB to 0 = 117 ms ngay trong lenh
# SDL_UploadToGPUTexture, memcpy vao transfer buffer chi 3 ms; tai anh 2-3 MB = 42-68 ms) = phan lon 6 khung giat/phut con lai sau P3.
# Ba viec, chi JX_MOBILE, ban PC khong doi:
#   a. Trang atlas moi (khoi/cum/trang rieng) = 0 bang SDL_CopyGPUTextureToTexture (GPU->GPU) tu DAI NGUON 0 (w x 256, moi dinh dang, tai 0 mot lan)
#      thay vi tai 8 MB tu bo dem moi lan cap trang/lop.
#   b. Khung NAP TRUOC (nNguon != 0: chieu / NPC / nen dat) sau khi luong nen giao: tai DAN len GPU tung dai theo ngan sach [Client] NapKhungKB
#      (mac dinh 128 KB/khung) TRUOC khi khung duoc ve; luc ve chi con tai not phan du (PrepareForBind nhu cu). Khung dang ve can (nNguon 0) y nhu cu.
#   c. Anh nen vung sap Clear (m_bJxKhongGiuCpu): NewVersion(target) KHONG tai ban CPU (1 MB BGRA8 x 40 khe luc vao map), bo ban CPU (RAM).
# Do: [VE-TAI] moi ky, [VE]/[VE-GIAT] muc "zero" (gio la so lan chep). Chay lai vo hai (chay sau _h.py).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[TAI 14/09]"
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
    # chen 'them' ngay sau dong chua 'neo' (duy nhat)
    if s.count(neo) != 1:
        raise SystemExit("%s: neo '%s...' thay %d lan" % (ten, neo[:70], s.count(neo)))
    i = s.find(neo)
    j = s.find(NL, i) + len(NL)
    return s[:j] + them + s[j:]


def va(p, viec):
    s = doc(p)
    if DAU in s:
        print("da va roi:", p)
        return
    c0 = cao(s)
    s = viec(s)
    assert cao(s) == c0, p + ": byte cao doi"
    ghi(p, s)
    print("da va:", p)


# ---------------------------------------------------------------- BaseInclude.h: nNguon di theo ket qua khung nen + [Client] NapKhungKB
def v_base(s):
    cu = "struct JxKhungXong { TextureResSpr* pSpr; int nFrame; int nW, nH, nOffX, nOffY; BYTE* pDiem; int nBpp; int eFmt; int bPal; int bHong; unsigned uLuc; };"
    moi = "struct JxKhungXong { TextureResSpr* pSpr; int nFrame; int nW, nH, nOffX, nOffY; BYTE* pDiem; int nBpp; int eFmt; int bPal; int bHong; unsigned uLuc; int nNguon; };\t// " + DAU + " nNguon: 0 = dang ve can, khac 0 = nap truoc (tai dan len GPU truoc khi ve)"
    s = thay(s, cu, moi, "JxKhungXong")
    s = sau_dong(s, "extern int g_nJxNapKhungNen, g_nJxNapKhungMs, g_nJxNapKhungTruoc, g_nJxNapKhungApMs, g_nJxVeGiatMs;",
                 "extern int g_nJxNapKhungKB;\t// " + DAU + " [Client] NapKhungKB: ngan sach byte/khung tai DAN texture khung nap truoc len GPU (0 = tat)" + NL, "extern NapKhungKB")
    return s


# ---------------------------------------------------------------- TextureResMgr.cpp: chep nNguon
def v_mgr(s):
    cu = "JxKhungXong kq; memset(&kq, 0, sizeof(kq)); kq.pSpr = kv.pSpr; kq.nFrame = kv.nFrame; kq.nBpp = kv.nBpp; kq.eFmt = kv.eFmt; kq.bPal = kv.bPal; kq.uLuc = kv.uLuc;"
    return thay(s, cu, cu + " kq.nNguon = kv.nNguon;\t// " + DAU, "NapNenChay kq")


# ---------------------------------------------------------------- TextureRes.cpp: khung nap truoc -> xin tai dan
def v_res_spr(s):
    cu = "\t\tpTex->UnlockRect(0);" + NL + "\t\tti.pTexture = pTex;" + NL + "\t\tif (bPal) Rep3_D3D11TagPalette(pTex, m_nPalRow);" + NL
    moi = ("\t\tpTex->UnlockRect(0);" + NL + "\t\tti.pTexture = pTex;" + NL
           + "\t\tif (kq.nNguon != 0) { extern void Rep3Gpu_TaiTruoc(IDirect3DDevice9*, IDirect3DTexture9*); Rep3Gpu_TaiTruoc(PD3DDEVICE, pTex); }\t// " + DAU + " khung nap truoc: tai dan len GPU tung dai truoc khi ve (RepresentBegin)" + NL
           + "\t\tif (bPal) Rep3_D3D11TagPalette(pTex, m_nPalRow);" + NL)
    return thay(s, cu, moi, "JxNhanKhungNen")


# ---------------------------------------------------------------- KRepresentShell3.cpp: bien + ini + goi moi khung + log [VE-TAI]
def v_shell(s):
    cu = "int g_nJxNapKhungNen = 1, g_nJxNapKhungMs = 3, g_nJxNapKhungTruoc = 2, g_nJxNapKhungApMs = 3, g_nJxVeGiatMs = 20;" + NL
    moi = cu + "int g_nJxNapKhungKB = 128;\t// " + DAU + " [Client] NapKhungKB: ngan sach byte/khung tai dan texture khung NAP TRUOC len GPU (driver Fold 7 ton CPU ~20 ms/MB trong lenh tai)" + NL
    s = thay(s, cu, moi, "g_nJxNapKhungKB")
    cu = "\tg_nJxNapKhungApMs  = Rep3Ini(\"NapKhungApMs\", 3);\t// ngan sach tao texture tu ket qua luong nen moi khung (ms)" + NL
    moi = cu + "\tg_nJxNapKhungKB    = Rep3Ini(\"NapKhungKB\", 128);\t// " + DAU + " ngan sach tai dan khung nap truoc len GPU (KB/khung); 0 = tat (tai ca khung luc ve nhu cu)" + NL
    s = thay(s, cu, moi, "ini NapKhungKB")
    cu = "\tm_TextureResMgr.JxNenTruocXuLy();\t// [NENTRUOC 13/09] nen dat: muc cho tep -> giao khung" + NL
    moi = cu + "\t{ extern void Rep3Gpu_TaiTruocChay(IDirect3DDevice9*, unsigned); if (g_nJxNapKhungKB > 0) Rep3Gpu_TaiTruocChay(PD3DDEVICE, (unsigned)g_nJxNapKhungKB << 10); }\t// " + DAU + " tai dan khung nap truoc len GPU" + NL
    s = thay(s, cu, moi, "RepresentBegin goi")
    neo = "\tg_dJxNapKhungTre = g_dJxNapKhungTreMax = g_dJxNapNenBan = g_dJxNapKhungAp = g_dJxNapKhungApMax = 0.0;"
    if s.count(neo) != 1:
        raise SystemExit("JxVeKyIn cuoi: neo thay %d lan" % s.count(neo))
    i = s.find(neo)
    j = s.find(NL + "}" + NL, i)
    assert j > i and j - i < 400, "JxVeKyIn cuoi: khong thay dau dong ham"
    them = (NL + "\t{\t// " + DAU + " tai dan khung nap truoc + to 0 trang atlas bang chep GPU" + NL
            + "\t\textern unsigned g_uJxTaiTruocSo, g_uJxTaiTruocXong, g_uJxTaiTruocKB, g_uJxTaiTruocLuot, g_uJxTaiTruocMax, g_uJxZeroChep; extern double g_dJxTaiTruocMs, g_dJxZeroChepMs;" + NL
            + "\t\tRep3Log(\"[VE-TAI] tai dan khung nap truoc (NapKhungKB=%d): vao hang %u, xong %u, %u KB / %u luot, %.1f ms (hang cho max %u) | trang atlas moi to 0 bang chep GPU: %u trang %.1f ms\"," + NL
            + "\t\t\tg_nJxNapKhungKB, g_uJxTaiTruocSo, g_uJxTaiTruocXong, g_uJxTaiTruocKB, g_uJxTaiTruocLuot, g_dJxTaiTruocMs, g_uJxTaiTruocMax, g_uJxZeroChep, g_dJxZeroChepMs);" + NL
            + "\t\tg_uJxTaiTruocSo = g_uJxTaiTruocXong = g_uJxTaiTruocKB = g_uJxTaiTruocLuot = g_uJxTaiTruocMax = g_uJxZeroChep = 0; g_dJxTaiTruocMs = g_dJxZeroChepMs = 0.0;" + NL
            + "\t}")
    s = s[:j] + them + s[j:]
    return s


# ---------------------------------------------------------------- D3D9onGPUi.h
def v_gpui(s):
    cu = "\tvoid  QueueUpload(const RECT* prc);\t\t// chep CPU (vung prc) vao staging cua khung + ghi lenh tai" + NL
    moi = cu + "#ifdef JX_MOBILE" + NL + "\tUINT  JxTaiTruoc(UINT uMax);\t\t// " + DAU + " khung nap truoc (chua ve): tai dan ban CPU len GPU toi da uMax byte; tra byte da ghi lenh, 0 = xong/khong lam" + NL + "#endif" + NL
    s = thay(s, cu, moi, "CTexGpu QueueUpload khai bao")
    cu = "\tbool        m_bJxKhongGiuCpu;\t// [XOANEN 13/09 b] anh nen vung (dich sap Clear): khong giu ban CPU -> PrepareAsTarget/LockRect khong doc nguoc GPU" + NL
    moi = cu + "\tbool        m_bJxTaiTruoc;\t\t// " + DAU + " dang trong hang tai dan cua CDevGpu (m_jxTaiTruoc)" + NL
    s = thay(s, cu, moi, "CTexGpu m_bJxKhongGiuCpu")
    cu = "\tvoid    QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, UINT layer = 0);\t// tai vung 0 (trang moi / o chua co du lieu); [MANG 11/09] layer" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "\tvoid    QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, UINT layer = 0, SDL_GPUTextureFormat fmt = SDL_GPU_TEXTUREFORMAT_INVALID, SDL_GPUTextureType eLoai = SDL_GPU_TEXTURETYPE_2D);\t// " + DAU + " fmt hop le + ca trang -> chep GPU tu dai nguon 0 (eLoai = loai texture dich, Metal doi cung loai)" + NL
           + "\tvoid    JxTaiTruocThem(CTexGpu* p);\t\t// " + DAU + " khung nap truoc: vao hang tai dan" + NL
           + "\tvoid    JxTaiTruocBo(CTexGpu* p);\t\t// texture bi huy: rut khoi hang" + NL
           + "\tvoid    JxTaiTruocChay(UINT uNganSach);\t// moi khung (RepresentBegin): tai toi da uNganSach byte" + NL
           + "#else" + NL + cu + "#endif" + NL)
    s = thay(s, cu, moi, "QueueZeroUpload khai bao")
    cu = "\tSDL_GPUTransferBuffer* m_pJxZeroXfer; UINT m_jxZeroSize, m_jxZeroDaXoa; std::vector<RgTexUpload> m_jxZeroUploads;\t// [VE 11/09 d] bo dem 0 co dinh cho trang atlas moi / o chua co ban CPU (stageOff = 0)" + NL
    moi = (cu
           + "\tstruct JxZeroNguon { SDL_GPUTextureFormat fmt; SDL_GPUTextureType eLoai; SDL_GPUTexture* pTex; UINT w, h, bpp; };\t// " + DAU + " dai nguon 0 (w x 256) moi (dinh dang, loai), tai 0 mot lan" + NL
           + "\tJxZeroNguon m_jxZeroNguon[6]; int m_nJxZeroNguon; std::vector<RgTexUpload> m_jxZeroCopy;\t// m_jxZeroCopy: trang moi can chep 0 (stageOff = chi so dai nguon)" + NL
           + "\tstd::vector<CTexGpu*> m_jxTaiTruoc;\t// " + DAU + " texture khung nap truoc dang tai dan len GPU (JxTaiTruocChay)" + NL)
    s = thay(s, cu, moi, "m_jxZeroUploads khai bao")
    return s


# ---------------------------------------------------------------- D3D9onGPUDev.cpp
def v_dev(s):
    cu = "static double JxVeMs(Uint64 a, Uint64 b) { return (double)(b - a) * 1000.0 / (double)SDL_GetPerformanceFrequency(); }" + NL
    moi = (cu + "#ifdef JX_MOBILE" + NL
           + "unsigned g_uJxTaiTruocSo = 0, g_uJxTaiTruocXong = 0, g_uJxTaiTruocKB = 0, g_uJxTaiTruocLuot = 0, g_uJxTaiTruocMax = 0, g_uJxZeroChep = 0; double g_dJxTaiTruocMs = 0.0, g_dJxZeroChepMs = 0.0;\t// " + DAU + " -> [VE-TAI] (KRepresentShell3.cpp)" + NL
           + "#endif" + NL)
    s = thay(s, cu, moi, "bien dem")
    cu = "\tm_bJxDichSeXoa = false;\t// [XOANEN 13/09 b]" + NL
    moi = cu + "\tmemset(m_jxZeroNguon, 0, sizeof(m_jxZeroNguon)); m_nJxZeroNguon = 0;\t// " + DAU + NL
    s = thay(s, cu, moi, "khoi tao CDevGpu")
    cu = "\t\tif (m_pWhiteMang) SDL_ReleaseGPUTexture(m_pGpu, m_pWhiteMang);\t// [KHOI 11/09]" + NL
    moi = cu + "#ifdef JX_MOBILE" + NL + "\t\tfor (int q = 0; q < m_nJxZeroNguon; q++) if (m_jxZeroNguon[q].pTex) SDL_ReleaseGPUTexture(m_pGpu, m_jxZeroNguon[q].pTex);\t// " + DAU + NL + "#endif" + NL
    s = thay(s, cu, moi, "huy dai nguon 0")
    cu = "\tif (pDev && g_nRep3ApiOn == 100) ((CDevGpu*)pDev)->m_bJxDichSeXoa = (bBat != 0);" + NL + "}" + NL
    moi = (cu
           + "// " + DAU + " TextureResSpr::JxNhanKhungNen: khung NAP TRUOC vua co texture -> vao hang tai dan; KRepresentShell3 RepresentBegin: chay hang theo ngan sach" + NL
           + "void Rep3Gpu_TaiTruoc(IDirect3DDevice9* pDev, IDirect3DTexture9* pTex)" + NL
           + "{" + NL
           + "\textern int g_nJxNapKhungKB;" + NL
           + "\tif (pDev && pTex && g_nRep3ApiOn == 100 && g_nJxNapKhungKB > 0) ((CDevGpu*)pDev)->JxTaiTruocThem((CTexGpu*)pTex);" + NL
           + "}" + NL
           + "void Rep3Gpu_TaiTruocChay(IDirect3DDevice9* pDev, unsigned uNganSach)" + NL
           + "{" + NL
           + "\tif (pDev && g_nRep3ApiOn == 100) ((CDevGpu*)pDev)->JxTaiTruocChay(uNganSach);" + NL
           + "}" + NL)
    s = thay(s, cu, moi, "Rep3Gpu_DichSeXoa")
    cu = "// [GPU 11/09 ATLAS] ghi lenh tai mot vung toan 0 (trang moi, o chua co du lieu CPU)" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "// " + DAU + " hang tai dan: khung nap truoc (luong nen giao, chua ai ve) tai len GPU tung dai theo ngan sach moi khung, de luc ve khong con tai 2-3 MB mot luc." + NL
           + "// Texture bi huy thi rut khoi hang (~CTexGpu). Texture da duoc ve truoc khi toi luot (PrepareForBind tai het) -> JxTaiTruoc tra 0 -> rut." + NL
           + "void CDevGpu::JxTaiTruocThem(CTexGpu* p)" + NL
           + "{" + NL
           + "\tif (!p || p->m_bJxTaiTruoc) return;" + NL
           + "\tp->m_bJxTaiTruoc = true; m_jxTaiTruoc.push_back(p); g_uJxTaiTruocSo++;" + NL
           + "\tif ((unsigned)m_jxTaiTruoc.size() > g_uJxTaiTruocMax) g_uJxTaiTruocMax = (unsigned)m_jxTaiTruoc.size();" + NL
           + "}" + NL
           + "void CDevGpu::JxTaiTruocBo(CTexGpu* p)" + NL
           + "{" + NL
           + "\tfor (size_t i = m_jxTaiTruoc.size(); i > 0; i--)" + NL
           + "\t\tif (m_jxTaiTruoc[i - 1] == p) { m_jxTaiTruoc.erase(m_jxTaiTruoc.begin() + (i - 1)); break; }" + NL
           + "\tp->m_bJxTaiTruoc = false;" + NL
           + "}" + NL
           + "void CDevGpu::JxTaiTruocChay(UINT uNganSach)" + NL
           + "{" + NL
           + "\tif (m_jxTaiTruoc.empty() || !m_pGpu) return;" + NL
           + "\tconst Uint64 u0 = SDL_GetPerformanceCounter();" + NL
           + "\tUINT uDa = 0; unsigned uLuot = 0;" + NL
           + "\twhile (!m_jxTaiTruoc.empty() && uDa < uNganSach)" + NL
           + "\t{" + NL
           + "\t\tCTexGpu* p = m_jxTaiTruoc[0];" + NL
           + "\t\tconst UINT uB = p->JxTaiTruoc(uNganSach - uDa);" + NL
           + "\t\tuDa += uB; if (uB) uLuot++;" + NL
           + "\t\tif (uB == 0 || !p->m_bDirty) { p->m_bJxTaiTruoc = false; m_jxTaiTruoc.erase(m_jxTaiTruoc.begin()); g_uJxTaiTruocXong++; }" + NL
           + "\t}" + NL
           + "\tif (uDa) { g_uJxTaiTruocKB += uDa >> 10; g_uJxTaiTruocLuot += uLuot; g_dJxTaiTruocMs += JxVeMs(u0, SDL_GetPerformanceCounter()); }" + NL
           + "}" + NL
           + "#endif" + NL + cu)
    s = thay(s, cu, moi, "JxTaiTruocChay dinh nghia")
    cu = ("void CDevGpu::QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, UINT layer)" + NL
          + "{" + NL
          + "\tif (!pTex || !w || !h || !bpp) return;" + NL
          + "#ifdef JX_MOBILE" + NL
          + "\t{\t// [VE 11/09 d] tai tu bo dem 0 co dinh (SubmitFrame): khong memset/memcpy vao staging, staging khong phinh 2-4 MB moi trang atlas moi" + NL)
    moi = ("#ifdef JX_MOBILE" + NL
           + "void CDevGpu::QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, UINT layer, SDL_GPUTextureFormat fmt, SDL_GPUTextureType eLoai)" + NL
           + "#else" + NL
           + "void CDevGpu::QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, UINT layer)" + NL
           + "#endif" + NL
           + "{" + NL
           + "\tif (!pTex || !w || !h || !bpp) return;" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\tif (fmt != SDL_GPU_TEXTUREFORMAT_INVALID && x == 0 && y == 0 && w >= 512 && h >= 512 && m_pGpu)" + NL
           + "\t{\t// " + DAU + " trang atlas moi (8 MB): chep GPU->GPU tu DAI NGUON 0 (w x 256, tai 0 mot lan moi dinh dang/loai) thay vi tai 8 MB tu bo dem" + NL
           + "\t\t// (Fold 7 13/09: 'zero 1/116.8' = 117 ms CPU trong lenh tai buffer->anh). Dai nguon duoc tai 0 qua m_jxZeroUploads TRUOC cac lenh chep cung khung." + NL
           + "\t\tint k = -1;" + NL
           + "\t\tfor (int q = 0; q < m_nJxZeroNguon; q++) if (m_jxZeroNguon[q].pTex && m_jxZeroNguon[q].fmt == fmt && m_jxZeroNguon[q].w == w && m_jxZeroNguon[q].eLoai == eLoai) { k = q; break; }" + NL
           + "\t\tif (k < 0 && m_nJxZeroNguon < 6)" + NL
           + "\t\t{" + NL
           + "\t\t\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci)); ci.type = eLoai; ci.format = fmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;" + NL
           + "\t\t\tci.width = w; ci.height = 256; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;" + NL
           + "\t\t\tSDL_GPUTexture* pZ = SDL_CreateGPUTexture(m_pGpu, &ci);" + NL
           + "\t\t\tif (pZ)" + NL
           + "\t\t\t{" + NL
           + "\t\t\t\tk = m_nJxZeroNguon++; m_jxZeroNguon[k].fmt = fmt; m_jxZeroNguon[k].eLoai = eLoai; m_jxZeroNguon[k].pTex = pZ; m_jxZeroNguon[k].w = w; m_jxZeroNguon[k].h = 256; m_jxZeroNguon[k].bpp = bpp;" + NL
           + "\t\t\t\tRgTexUpload z = { pZ, 0, 0, w, 256, 0, w * 256 * bpp, 0 }; m_jxZeroUploads.push_back(z);" + NL
           + "\t\t\t\tRgLog(\"[TAI] dai nguon 0 fmt %d loai %d %ux256 (%u KB) de to 0 trang atlas moi bang chep GPU\", (int)fmt, (int)eLoai, w, (w * 256 * bpp) >> 10);" + NL
           + "\t\t\t}" + NL
           + "\t\t\telse RgLog(\"[TAI] dai nguon 0 fmt %d that bai: %s -> tai tu bo dem nhu cu\", (int)fmt, SDL_GetError());" + NL
           + "\t\t}" + NL
           + "\t\tif (k >= 0) { RgTexUpload u = { pTex, x, y, w, h, (UINT)k, w * h * bpp, layer }; m_jxZeroCopy.push_back(u); return; }" + NL
           + "\t}" + NL
           + "\t{\t// [VE 11/09 d] tai tu bo dem 0 co dinh (SubmitFrame): khong memset/memcpy vao staging, staging khong phinh 2-4 MB moi trang atlas moi" + NL)
    s = thay(s, cu, moi, "QueueZeroUpload than")
    cu = "\t\t\tjxK.dChepZero = JxVeMs(uZ0, SDL_GetPerformanceCounter());" + NL + "\t\t}" + NL + "#endif" + NL
    moi = ("\t\t\tjxK.dChepZero = JxVeMs(uZ0, SDL_GetPerformanceCounter());" + NL + "\t\t}" + NL
           + "\t\tif (!m_jxZeroCopy.empty())" + NL
           + "\t\t{\t// " + DAU + " trang atlas moi = 0 bang chep GPU->GPU tu dai nguon 0 (dai nguon da duoc tai 0 o khoi tren, cung copy pass, truoc noi dung texture)" + NL
           + "\t\t\tconst Uint64 uC0 = SDL_GetPerformanceCounter();" + NL
           + "\t\t\tfor (size_t i = 0; i < m_jxZeroCopy.size(); i++)" + NL
           + "\t\t\t{" + NL
           + "\t\t\t\tconst RgTexUpload& u = m_jxZeroCopy[i];" + NL
           + "\t\t\t\tif (u.stageOff >= (UINT)m_nJxZeroNguon || !m_jxZeroNguon[u.stageOff].pTex) continue;" + NL
           + "\t\t\t\tconst JxZeroNguon& ng = m_jxZeroNguon[u.stageOff];" + NL
           + "\t\t\t\tfor (UINT y0 = 0; y0 < u.h; y0 += ng.h)" + NL
           + "\t\t\t\t{" + NL
           + "\t\t\t\t\tconst UINT hh = (u.h - y0 < ng.h) ? (u.h - y0) : ng.h;" + NL
           + "\t\t\t\t\tSDL_GPUTextureLocation src; memset(&src, 0, sizeof(src)); src.texture = ng.pTex;" + NL
           + "\t\t\t\t\tSDL_GPUTextureLocation dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.layer = u.layer; dst.x = u.x; dst.y = u.y + y0;" + NL
           + "\t\t\t\t\tSDL_CopyGPUTextureToTexture(cp, &src, &dst, u.w, hh, 1, false);" + NL
           + "\t\t\t\t}" + NL
           + "\t\t\t}" + NL
           + "\t\t\tm_uUploads += (unsigned)m_jxZeroCopy.size(); jxK.uZero += (unsigned)m_jxZeroCopy.size(); g_uJxZeroChep += (unsigned)m_jxZeroCopy.size();" + NL
           + "\t\t\tconst double dC = JxVeMs(uC0, SDL_GetPerformanceCounter()); jxK.dChepZero += dC; g_dJxZeroChepMs += dC;" + NL
           + "\t\t}" + NL
           + "#endif" + NL)
    s = thay(s, cu, moi, "SubmitFrame chep 0")
    cu = "\tm_jxZeroUploads.clear();\t// [VE 11/09 d]" + NL
    moi = cu + "\tm_jxZeroCopy.clear();\t// " + DAU + NL
    s = thay(s, cu, moi, "FrameReset")
    cu = "\tconst bool bCoTai = !m_texUploads.empty() || !m_palPending.empty() || !m_jxZeroUploads.empty() || !m_jxPalUploads.empty();"
    moi = "\tconst bool bCoTai = !m_texUploads.empty() || !m_palPending.empty() || !m_jxZeroUploads.empty() || !m_jxPalUploads.empty() || !m_jxZeroCopy.empty();\t// " + DAU + " + chep 0"
    s = thay(s, cu, moi, "bCoTai")
    return s


# ---------------------------------------------------------------- D3D9onGPURes.cpp
def v_res(s):
    cu = "\tm_bJxKhongGiuCpu = false;\t// [XOANEN 13/09 b]" + NL
    moi = cu + "\tm_bJxTaiTruoc = false;\t// " + DAU + NL
    s = thay(s, cu, moi, "khoi tao CTexGpu")
    cu = "\tif (m_pDev && m_bUsedThisFrame) m_pDev->UntouchTex(this);\t// [GPU 11/09 ATLAS] dang trong m_touched cua khung -> rut ra (tranh con tro treo o FrameReset)" + NL
    moi = cu + "#ifdef JX_MOBILE" + NL + "\tif (m_pDev && m_bJxTaiTruoc) m_pDev->JxTaiTruocBo(this);\t// " + DAU + " dang trong hang tai dan -> rut ra" + NL + "#endif" + NL
    s = thay(s, cu, moi, "huy CTexGpu")
    cu = "\t\tif (m_pCpu && !bKeep) QueueUpload(NULL);" + NL + "\t\tm_bDirty = false;" + NL + "\t}" + NL + "\telse if (m_bDirty)" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "\t\tif (m_bJxKhongGiuCpu && m_pCpu) { free(m_pCpu); m_pCpu = NULL; }\t// " + DAU + " anh nen vung sap Clear: khong tai ban CPU (1 MB BGRA8 x 40 khe luc vao map, ~20 ms/MB), bo luon (LockRect cap lai neu can)" + NL
           + "\t\tif (m_pCpu && !bKeep) QueueUpload(NULL);" + NL
           + "#else" + NL
           + "\t\tif (m_pCpu && !bKeep) QueueUpload(NULL);" + NL
           + "#endif" + NL
           + "\t\tm_bDirty = false;" + NL + "\t}" + NL + "\telse if (m_bDirty)" + NL)
    s = thay(s, cu, moi, "PrepareAsTarget NewVersion")
    cu = NL + "HRESULT CTexGpu::QueryInterface(REFIID riid, void** ppvObj)" + NL
    moi = (NL + "#ifdef JX_MOBILE" + NL
           + "// " + DAU + " Khung NAP TRUOC (luong nen giao, chua ai ve): tai dan ban CPU len GPU theo tung dai (uMax byte moi lan, RepresentBegin) TRUOC khi khung" + NL
           + "// duoc ve, de luc ve khong con tai 2-3 MB mot luc (Fold 7: driver ton CPU ~20 ms/MB trong lenh chep buffer->anh)." + NL
           + "// Texture ao (atlas, <= 512x512): xin o + tai ca o (nhu PrepareForBind, KHONG danh dau da dung trong khung). Texture rieng: NewVersion + tai tung dai tu dau" + NL
           + "// m_rcDirty; phan con lai giu trong m_rcDirty -> luc ve PrepareForBind tai not (duong cu). Tra byte da ghi lenh; 0 = xong hoac khong lam duoc." + NL
           + "UINT CTexGpu::JxTaiTruoc(UINT uMax)" + NL
           + "{" + NL
           + "\tif (!m_pCpu || m_bLocked || !m_bDirty || m_bGpuTarget || !m_pDev || !m_pDev->m_pGpu) return 0;" + NL
           + "\tif (m_bVirtual)" + NL
           + "\t{" + NL
           + "\t\tif (m_pPage) return 0;" + NL
           + "\t\tCAtlasMgrGpu* pA = m_pDev->m_pAtlas;" + NL
           + "\t\tif (!pA || !pA->Alloc(m_w, m_h, m_fi.gpu, &m_pPage, &m_ax, &m_ay)) return 0;\t// khong xin duoc o: de PrepareForBind lo (texture rieng) nhu cu" + NL
           + "\t\tm_uGpuBytes = m_w * m_h * m_pPage->m_bpp; m_pDev->m_uTexBytes += m_uGpuBytes; g_uRep3GpuTexCount++; g_uRep3GpuTexBytes += m_uGpuBytes;" + NL
           + "\t\tg_uJxAtlasODat[(m_pool == D3DPOOL_MANAGED) ? 1 : 0]++;" + NL
           + "\t\tQueueUpload(NULL);" + NL
           + "\t\tm_bDirty = false; m_bGpuHasData = true;" + NL
           + "\t\treturn m_uGpuBytes;" + NL
           + "\t}" + NL
           + "\tif (!m_pGpu && !NewVersion(false)) return 0;" + NL
           + "\tRECT rc = m_rcDirty;" + NL
           + "\tif (rc.left < 0) rc.left = 0; if (rc.top < 0) rc.top = 0; if (rc.right > (int)m_w) rc.right = (int)m_w; if (rc.bottom > (int)m_h) rc.bottom = (int)m_h;" + NL
           + "\tif (rc.right <= rc.left || rc.bottom <= rc.top) { m_bDirty = false; return 0; }" + NL
           + "\tconst UINT gbpp = RgGpuBpp(m_gpuFmt); if (!gbpp) return 0;" + NL
           + "\tconst UINT uHang = (UINT)(rc.right - rc.left) * gbpp;" + NL
           + "\tUINT nDong = uMax / uHang; if (nDong == 0) nDong = 1;" + NL
           + "\tif (nDong > (UINT)(rc.bottom - rc.top)) nDong = (UINT)(rc.bottom - rc.top);" + NL
           + "\tRECT rcDai = rc; rcDai.bottom = rc.top + (int)nDong;" + NL
           + "\tQueueUpload(&rcDai);" + NL
           + "\tm_rcDirty = rc; m_rcDirty.top = rcDai.bottom;" + NL
           + "\tif (m_rcDirty.top >= m_rcDirty.bottom) m_bDirty = false;" + NL
           + "\treturn nDong * uHang;" + NL
           + "}" + NL
           + "#endif" + NL
           + NL + "HRESULT CTexGpu::QueryInterface(REFIID riid, void** ppvObj)" + NL)
    s = thay(s, cu, moi, "JxTaiTruoc dinh nghia")
    cu = "\tm_pDev->QueueZeroUpload(pTex, 0, 0, m_pageSize, m_pageSize, bpp, uLop);\t// trang moi = 0 (khong de rac; vien o khi loc tuyen tinh)" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "\tm_pDev->QueueZeroUpload(pTex, 0, 0, m_pageSize, m_pageSize, bpp, uLop, fmt, (uKhoi != 0xFFu || g_nJxAtlasMang) ? SDL_GPU_TEXTURETYPE_2D_ARRAY : SDL_GPU_TEXTURETYPE_2D);\t// " + DAU + " trang moi = 0 bang chep GPU tu dai nguon 0 (khong tai 8 MB tu bo dem)" + NL
           + "#else" + NL + cu + "#endif" + NL)
    s = thay(s, cu, moi, "NewPage QueueZeroUpload")
    return s


va("Sources/Represent/Represent3/BaseInclude.h", v_base)
va("Sources/Represent/Represent3/TextureResMgr.cpp", v_mgr)
va("Sources/Represent/Represent3/TextureRes.cpp", v_res_spr)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", v_shell)
va("Sources/Represent/Represent3/D3D9onGPUi.h", v_gpui)
va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", v_dev)
va("Sources/Represent/Represent3/D3D9onGPURes.cpp", v_res)
