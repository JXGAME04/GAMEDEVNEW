# -*- coding: utf-8 -*-
# [DEM 14/09] Buoc 2 sau ban do [TAI-DO] (log Fold 7 10:22): trong SDL_UploadToGPUTexture driver Adreno ton CPU ~0,2 ms moi MB CO ANH DICH khi
# dich la BGRA8 (vung con 1 MB vao KHOI BGRA8 64 MB = 12,4-18,9 ms; vao anh rieng 16 MB = 3,0; ca anh 1 MB = 0,19; nguon cu hay vua ghi nhu nhau);
# R8G8 (bang mau, phan lon sprite) = 0,00 ms moi kieu; anh->anh (CopyGPUTextureToTexture) va buffer->buffer = 0,00 ms CPU.
# Sua (chi JX_MOBILE, ban PC khong doi):
#   a. Vung con BGRA8 vao trang atlas (khoi/cum/trang rieng 2048^2): tai vao ANH DEM 512x512 BGRA8 (1 MB, ~0,2 ms) roi chep GPU->GPU vao trang.
#      Co: 2 bit cao cua RgTexUpload.layer (0x40000000 = dem 2D, 0x80000000 = dem mang 1 lop), dat o CTexGpu::QueueUpload; SubmitFrame che bit khi dung.
#   b. O chua co ban CPU (QueueZeroUpload vung con): chep tu dai nguon 0 (co san moi dinh dang) thay vi tai tu bo dem 0 (cung bi 12+ ms khi dich BGRA8).
#   c. Tai dan (JxTaiTruoc) texture RIENG BGRA8: tai ca mot lan (moi lenh tra chi phi theo co anh dich, chia dai la ton them).
#   d. [Client] TaiDo mac dinh 0 (da co so do; van bat lai duoc). [VE-TAI] them "tai qua anh dem N o / KB".
# Chay lai vo hai (chay sau _j.py).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[DEM 14/09]"
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


def v_gpui(s):
    cu = "\tSDL_GPUTexture* m_pWhiteMang;\t\t// [KHOI 11/09] texture MANG 1x1 x 1 lop: gan vao cac khe khoi chua co khoi (SDL doi moi sampler khai bao phai duoc gan)" + NL
    moi = (cu + "#ifdef JX_MOBILE" + NL
           + "\tSDL_GPUTexture* m_pJxDem[2];\t// " + DAU + " anh dem 512x512 BGRA8 (0 = 2D, 1 = mang 1 lop): vung con BGRA8 vao trang atlas tai qua anh dem + chep GPU (driver Adreno ton CPU theo co anh dich)" + NL
           + "\tSDL_GPUTexture* JxDemLay(int nLoai);\t// tao lan dau" + NL
           + "#endif" + NL)
    return thay(s, cu, moi, "m_pJxDem khai bao")


def v_dev(s):
    cu = "unsigned g_uJxTaiTruocSo = 0, g_uJxTaiTruocXong = 0, g_uJxTaiTruocKB = 0, g_uJxTaiTruocLuot = 0, g_uJxTaiTruocMax = 0, g_uJxZeroChep = 0; double g_dJxTaiTruocMs = 0.0, g_dJxZeroChepMs = 0.0;\t// [TAI 14/09] -> [VE-TAI] (KRepresentShell3.cpp)" + NL
    moi = cu + "unsigned g_uJxDemSo = 0, g_uJxDemKB = 0;\t// " + DAU + " so o / KB tai qua anh dem -> [VE-TAI]" + NL
    s = thay(s, cu, moi, "bien dem DEM")
    cu = "int g_nJxTaiDo = 1;\t// [TAI-DO 14/09] [Client] TaiDo: 1 = do duong tai len GPU luc khoi dong thiet bi (chi log, ~1-2 s mot lan)" + NL
    moi = "int g_nJxTaiDo = 0;\t// [TAI-DO 14/09] [Client] TaiDo: 1 = do duong tai len GPU luc khoi dong thiet bi (chi log, ~0,4 s); " + DAU + " mac dinh 0 (da do 14/09 10:22)" + NL
    s = thay(s, cu, moi, "g_nJxTaiDo mac dinh")
    cu = "\tm_bJxDichSeXoa = false;\t// [XOANEN 13/09 b]" + NL
    moi = cu + "\tm_pJxDem[0] = m_pJxDem[1] = NULL;\t// " + DAU + NL
    s = thay(s, cu, moi, "khoi tao m_pJxDem")
    cu = "\t\tfor (int q = 0; q < m_nJxZeroNguon; q++) if (m_jxZeroNguon[q].pTex) SDL_ReleaseGPUTexture(m_pGpu, m_jxZeroNguon[q].pTex);\t// [TAI 14/09]" + NL
    moi = cu + "\t\tfor (int q = 0; q < 2; q++) if (m_pJxDem[q]) SDL_ReleaseGPUTexture(m_pGpu, m_pJxDem[q]);\t// " + DAU + NL
    s = thay(s, cu, moi, "huy m_pJxDem")
    cu = "#ifdef JX_MOBILE" + NL + "// [TAI-DO 14/09] Do duong tai len GPU luc khoi dong thiet bi"
    moi = ("#ifdef JX_MOBILE" + NL
           + "// " + DAU + " anh dem 512x512 BGRA8: [TAI-DO] 14/09 tren Fold 7 - SDL_UploadToGPUTexture dich BGRA8 ton CPU ~0,2 ms moi MB CO ANH DICH" + NL
           + "// (o 1 MB vao khoi 64 MB = 12-19 ms, vao anh 1 MB = 0,19 ms), chep anh->anh 0 ms CPU. Vung con BGRA8 vao trang atlas: tai vao anh dem roi chep sang." + NL
           + "SDL_GPUTexture* CDevGpu::JxDemLay(int nLoai)" + NL
           + "{" + NL
           + "\tif (nLoai < 0 || nLoai > 1 || !m_pGpu) return NULL;" + NL
           + "\tif (!m_pJxDem[nLoai])" + NL
           + "\t{" + NL
           + "\t\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci)); ci.type = nLoai ? SDL_GPU_TEXTURETYPE_2D_ARRAY : SDL_GPU_TEXTURETYPE_2D; ci.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;" + NL
           + "\t\tci.width = 512; ci.height = 512; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;" + NL
           + "\t\tm_pJxDem[nLoai] = SDL_CreateGPUTexture(m_pGpu, &ci);" + NL
           + "\t\tif (m_pJxDem[nLoai]) RgLog(\"[DEM] anh dem 512x512 BGRA8 loai %d: vung con BGRA8 vao trang atlas tai qua anh dem + chep GPU\", nLoai);" + NL
           + "\t\telse RgLog(\"[DEM] anh dem loai %d that bai: %s -> tai thang nhu cu\", nLoai, SDL_GetError());" + NL
           + "\t}" + NL
           + "\treturn m_pJxDem[nLoai];" + NL
           + "}" + NL
           + "#endif" + NL
           + cu)
    s = thay(s, cu, moi, "JxDemLay dinh nghia")
    # QueueZeroUpload: vung con cung chep tu dai nguon
    cu = "\tif (fmt != SDL_GPU_TEXTUREFORMAT_INVALID && x == 0 && y == 0 && w >= 512 && h >= 512 && m_pGpu)" + NL
    moi = "\tif (fmt != SDL_GPU_TEXTUREFORMAT_INVALID && m_pGpu)\t// " + DAU + " ca trang lan o (vung con) deu chep tu dai nguon 0 (tai tu bo dem vao dich BGRA8 = 12+ ms/o)" + NL
    s = thay(s, cu, moi, "QueueZeroUpload dieu kien")
    cu = "\t\tfor (int q = 0; q < m_nJxZeroNguon; q++) if (m_jxZeroNguon[q].pTex && m_jxZeroNguon[q].fmt == fmt && m_jxZeroNguon[q].w == w && m_jxZeroNguon[q].eLoai == eLoai) { k = q; break; }" + NL
    moi = "\t\tfor (int q = 0; q < m_nJxZeroNguon; q++) if (m_jxZeroNguon[q].pTex && m_jxZeroNguon[q].fmt == fmt && m_jxZeroNguon[q].w >= w && m_jxZeroNguon[q].eLoai == eLoai) { k = q; break; }\t// " + DAU + " dai rong >= vung" + NL
    s = thay(s, cu, moi, "QueueZeroUpload tim dai")
    cu = "\t\tif (k < 0 && m_nJxZeroNguon < 6)" + NL
    moi = "\t\tif (k < 0 && x == 0 && y == 0 && w >= 512 && h >= 512 && m_nJxZeroNguon < 6)\t// " + DAU + " chi tao dai khi cap ca trang" + NL
    s = thay(s, cu, moi, "QueueZeroUpload tao dai")
    # SubmitFrame: vong tai texture
    cu = ("\t\t\t\t\tconst RgTexUpload& u = m_texUploads[i];" + NL
          + "\t\t\t\t\tSDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = m_pTexXfer; src.offset = u.stageOff; src.pixels_per_row = u.w; src.rows_per_layer = u.h;" + NL
          + "\t\t\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.layer = u.layer; dst.x = u.x; dst.y = u.y; dst.w = u.w; dst.h = u.h; dst.d = 1;\t// [MANG 11/09] lop" + NL
          + "\t\t\t\t\tSDL_UploadToGPUTexture(cp, &src, &dst, false);" + NL
          + "\t\t\t\t}" + NL)
    moi = ("\t\t\t\t\tconst RgTexUpload& u = m_texUploads[i];" + NL
           + "\t\t\t\t\tSDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = m_pTexXfer; src.offset = u.stageOff; src.pixels_per_row = u.w; src.rows_per_layer = u.h;" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\t\t\t\tconst UINT uLopU = u.layer & 0x3FFFFFFFu;\t// " + DAU + " 2 bit cao cua layer = tai qua anh dem (CTexGpu::QueueUpload dat)" + NL
           + "\t\t\t\t\tSDL_GPUTexture* pDem = ((u.layer & 0xC0000000u) && u.w <= 512 && u.h <= 512) ? JxDemLay((u.layer & 0x80000000u) ? 1 : 0) : NULL;" + NL
           + "\t\t\t\t\tif (pDem)" + NL
           + "\t\t\t\t\t{\t// vung con BGRA8 vao trang atlas: tai vao anh dem (1 MB, ~0,2 ms) roi chep GPU->GPU sang trang (0 ms CPU) thay vi tai thang (12-19 ms/o vao khoi 64 MB)" + NL
           + "\t\t\t\t\t\tSDL_GPUTextureRegion dd; memset(&dd, 0, sizeof(dd)); dd.texture = pDem; dd.w = u.w; dd.h = u.h; dd.d = 1;" + NL
           + "\t\t\t\t\t\tSDL_UploadToGPUTexture(cp, &src, &dd, false);" + NL
           + "\t\t\t\t\t\tSDL_GPUTextureLocation sD; memset(&sD, 0, sizeof(sD)); sD.texture = pDem;" + NL
           + "\t\t\t\t\t\tSDL_GPUTextureLocation dD; memset(&dD, 0, sizeof(dD)); dD.texture = u.pTex; dD.layer = uLopU; dD.x = u.x; dD.y = u.y;" + NL
           + "\t\t\t\t\t\tSDL_CopyGPUTextureToTexture(cp, &sD, &dD, u.w, u.h, 1, false);" + NL
           + "\t\t\t\t\t\tg_uJxDemSo++; g_uJxDemKB += u.bytes >> 10;" + NL
           + "\t\t\t\t\t\tcontinue;" + NL
           + "\t\t\t\t\t}" + NL
           + "\t\t\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.layer = uLopU; dst.x = u.x; dst.y = u.y; dst.w = u.w; dst.h = u.h; dst.d = 1;" + NL
           + "#else" + NL
           + "\t\t\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.layer = u.layer; dst.x = u.x; dst.y = u.y; dst.w = u.w; dst.h = u.h; dst.d = 1;\t// [MANG 11/09] lop" + NL
           + "#endif" + NL
           + "\t\t\t\t\tSDL_UploadToGPUTexture(cp, &src, &dst, false);" + NL
           + "\t\t\t\t}" + NL)
    s = thay(s, cu, moi, "SubmitFrame vong tai")
    return s


def v_res(s):
    cu = ("\tRgTexUpload u = { pDst, (UINT)rc.left + (m_bVirtual ? m_ax : 0), (UINT)rc.top + (m_bVirtual ? m_ay : 0), rw, rh, off, bytes, JxLop() };\t// [GPU 11/09 ATLAS] [MANG 11/09] lop" + NL
          + "\tm_pDev->QueueTexUpload(u);" + NL)
    moi = ("#ifdef JX_MOBILE" + NL
           + "\t// " + DAU + " vung con BGRA8 vao trang atlas (khoi 64 MB / cum / trang 2048^2): danh dau 2 bit cao cua layer -> SubmitFrame tai qua anh dem + chep GPU" + NL
           + "\tconst UINT uJxDem = (m_bVirtual && m_pPage && gf == SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM && rw <= 512 && rh <= 512) ? ((m_pPage->m_nKhoi != 0xFFu || g_nJxAtlasMang) ? 0x80000000u : 0x40000000u) : 0u;" + NL
           + "\tRgTexUpload u = { pDst, (UINT)rc.left + (m_bVirtual ? m_ax : 0), (UINT)rc.top + (m_bVirtual ? m_ay : 0), rw, rh, off, bytes, JxLop() | uJxDem };" + NL
           + "#else" + NL
           + "\tRgTexUpload u = { pDst, (UINT)rc.left + (m_bVirtual ? m_ax : 0), (UINT)rc.top + (m_bVirtual ? m_ay : 0), rw, rh, off, bytes, JxLop() };\t// [GPU 11/09 ATLAS] [MANG 11/09] lop" + NL
           + "#endif" + NL
           + "\tm_pDev->QueueTexUpload(u);" + NL)
    s = thay(s, cu, moi, "QueueUpload co dem")
    cu = "\t\t\t\tif (m_pCpu) QueueUpload(NULL); else m_pDev->QueueZeroUpload(m_pPage->m_pTex, m_ax, m_ay, m_w, m_h, m_pPage->m_bpp, m_pPage->m_nLop);\t// [MANG 11/09] lop" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "\t\t\t\tif (m_pCpu) QueueUpload(NULL); else m_pDev->QueueZeroUpload(m_pPage->m_pTex, m_ax, m_ay, m_w, m_h, m_pPage->m_bpp, m_pPage->m_nLop, m_pPage->m_fmt, (m_pPage->m_nKhoi != 0xFFu || g_nJxAtlasMang) ? SDL_GPU_TEXTURETYPE_2D_ARRAY : SDL_GPU_TEXTURETYPE_2D);\t// " + DAU + " o rong: chep tu dai nguon 0" + NL
           + "#else" + NL + cu + "#endif" + NL)
    s = thay(s, cu, moi, "PrepareForBind o rong")
    cu = "\tUINT nDong = uMax / uHang; if (nDong == 0) nDong = 1;" + NL
    moi = cu + "\tif (m_gpuFmt == SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM) nDong = (UINT)(rc.bottom - rc.top);\t// " + DAU + " texture rieng BGRA8: moi lenh tai tra chi phi theo CO ANH dich -> tai ca mot lan, khong chia dai" + NL
    s = thay(s, cu, moi, "JxTaiTruoc BGRA8")
    return s


def v_shell(s):
    cu = "\t{ extern int g_nJxTaiDo; g_nJxTaiDo = Rep3Ini(\"TaiDo\", 1); }\t// [TAI-DO 14/09] 1 = do duong tai len GPU luc khoi dong thiet bi (chi log [TAI-DO], ~1-2 s)" + NL
    moi = "\t{ extern int g_nJxTaiDo; g_nJxTaiDo = Rep3Ini(\"TaiDo\", 0); }\t// [TAI-DO 14/09] 1 = do duong tai len GPU luc khoi dong thiet bi (chi log [TAI-DO], ~0,4 s); " + DAU + " mac dinh 0" + NL
    s = thay(s, cu, moi, "ini TaiDo mac dinh 0")
    cu = ("\t\tRep3Log(\"[VE-TAI] tai dan khung nap truoc (NapKhungKB=%d): vao hang %u, xong %u, %u KB / %u luot, %.1f ms (hang cho max %u) | trang atlas moi to 0 bang chep GPU: %u trang %.1f ms\"," + NL
          + "\t\t\tg_nJxNapKhungKB, g_uJxTaiTruocSo, g_uJxTaiTruocXong, g_uJxTaiTruocKB, g_uJxTaiTruocLuot, g_dJxTaiTruocMs, g_uJxTaiTruocMax, g_uJxZeroChep, g_dJxZeroChepMs);" + NL
          + "\t\tg_uJxTaiTruocSo = g_uJxTaiTruocXong = g_uJxTaiTruocKB = g_uJxTaiTruocLuot = g_uJxTaiTruocMax = g_uJxZeroChep = 0; g_dJxTaiTruocMs = g_dJxZeroChepMs = 0.0;" + NL)
    moi = ("\t\textern unsigned g_uJxDemSo, g_uJxDemKB;\t// " + DAU + NL
           + "\t\tRep3Log(\"[VE-TAI] tai dan khung nap truoc (NapKhungKB=%d): vao hang %u, xong %u, %u KB / %u luot, %.1f ms (hang cho max %u) | trang atlas / o rong to 0 bang chep GPU: %u lan %.1f ms | o BGRA8 tai qua anh dem: %u o %u KB\"," + NL
           + "\t\t\tg_nJxNapKhungKB, g_uJxTaiTruocSo, g_uJxTaiTruocXong, g_uJxTaiTruocKB, g_uJxTaiTruocLuot, g_dJxTaiTruocMs, g_uJxTaiTruocMax, g_uJxZeroChep, g_dJxZeroChepMs, g_uJxDemSo, g_uJxDemKB);" + NL
           + "\t\tg_uJxTaiTruocSo = g_uJxTaiTruocXong = g_uJxTaiTruocKB = g_uJxTaiTruocLuot = g_uJxTaiTruocMax = g_uJxZeroChep = 0; g_dJxTaiTruocMs = g_dJxZeroChepMs = 0.0; g_uJxDemSo = g_uJxDemKB = 0;" + NL)
    s = thay(s, cu, moi, "VE-TAI them dem")
    return s


va("Sources/Represent/Represent3/D3D9onGPUi.h", v_gpui)
va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", v_dev)
va("Sources/Represent/Represent3/D3D9onGPURes.cpp", v_res)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", v_shell)
