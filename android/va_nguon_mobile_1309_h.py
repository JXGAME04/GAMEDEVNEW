# -*- coding: utf-8 -*-
# [NENTRUOC 13/09 c] + [XOANEN 13/09 b] - sua "quay mot lan co man den" (chu 22:13, ban 109132150):
#   * Goc: NENTRUOC hoan ghep ca 8 vung KE BEN (dang tren man hinh) toi khi luong nen chuan bi xong khung; luc vao/quay lai map luong nen
#     ngap (322 tep spr) -> nen quanh nhan vat DEN toi 1,5 s ([PGND] luc vao map: "ke ben 0.0/0"). Truoc day 8 vung do ghep ngay (17 ms/vung).
#     Sua: vung ke ben ghep NGAY nhu cu, chi xin nap truoc (vung bi hoan vi ngan sach 8 ms co khung san o khung sau); chi vung XA moi hoan.
#   * Cu 156 ms khi xoa nen #38 luc vao map = PrepareAsTarget doc nguoc GPU (ThuLaiCpu = SubmitFrame + cho GPU dang ngap tai len) de giu ban CPU
#     ma khong ai doc. Sua: ClearImageData bao truoc "dich sap xoa" -> CTexGpu danh dau khong giu ban CPU -> khong doc nguoc (PrepareAsTarget,
#     LockRect). Chi JX_MOBILE, ban PC khong doi. Chay lai vo hai (chay sau _g.py).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[NENTRUOC 13/09 c]"
DAUX = "[XOANEN 13/09 b]"
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
        raise SystemExit("%s: neo '%s...' thay %d lan, can %d" % (ten, cu.strip()[:60], s.count(cu), n))
    return s.replace(cu, moi)


def va(p, viec):
    s = doc(p)
    if DAU in s or DAUX in s:
        print("da va roi:", p)
        return
    c0 = cao(s)
    s = viec(s)
    assert cao(s) == c0, p + ": byte cao doi"
    ghi(p, s)
    print("da va:", p)


def v_place(s):
    cu = ("\t\t\t{\t// [NENTRUOC 13/09] khung cac o dang duoc luong nen chuan bi -> hoan, khung sau ghep (nhanh)" + NL
          + "\t\t\t\tif (m_pInProcessAreaRegions[i]->JxNenChuaSan()) { nDeferred++; continue; }" + NL)
    moi = ("\t\t\t{\t// " + DAU + " vung KE BEN dang tren man hinh: ghep NGAY nhu cu, KHONG hoan (hoan = nen quanh nhan vat den toi 1,5 s luc vao/quay lai" + NL
           + "\t\t\t\t// map vi luong nen dang ngap - chu thay 22:13 13/09). Chi xin nap truoc: vung bi hoan vi ngan sach 8 ms se co khung san o khung sau." + NL
           + "\t\t\t\tm_pInProcessAreaRegions[i]->JxNenTruoc();" + NL)
    return thay(s, cu, moi, "nhanh 1 ke ben")


def v_gpui(s):
    a = "\tbool        m_bCpuBo;"
    i = s.find(a)
    assert i >= 0 and s.count(a) == 1, "m_bCpuBo"
    j = s.find(NL, i) + len(NL)
    s = s[:j] + "#ifdef JX_MOBILE" + NL + "\tbool        m_bJxKhongGiuCpu;\t// " + DAUX + " anh nen vung (dich sap Clear): khong giu ban CPU -> PrepareAsTarget/LockRect khong doc nguoc GPU" + NL + "#endif" + NL + s[j:]
    cu = "\tSDL_GPUTexture* m_pWhiteMang;\t\t// [KHOI 11/09] texture MANG 1x1 x 1 lop: gan vao cac khe khoi chua co khoi (SDL doi moi sampler khai bao phai duoc gan)" + NL
    moi = cu + "#ifdef JX_MOBILE" + NL + "\tbool m_bJxDichSeXoa;\t// " + DAUX + " ClearImageData bat truoc SetRenderTarget: dich sap bi Clear -> texture danh dau khong giu ban CPU" + NL + "#endif" + NL
    return thay(s, cu, moi, "m_pWhiteMang")


def v_res(s):
    cu = "\tm_bVirtual = false; m_pPage = NULL; m_ax = m_ay = 0; m_bCpuBo = false;"
    i = s.find(cu)
    assert i >= 0 and s.count(cu) == 1, "khoi tao CTexGpu"
    j = s.find(NL, i) + len(NL)
    s = s[:j] + "#ifdef JX_MOBILE" + NL + "\tm_bJxKhongGiuCpu = false;\t// " + DAUX + NL + "#endif" + NL + s[j:]
    cu = "\tif (m_bCpuBo && !m_pCpu) { m_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1); if (m_pCpu) ThuLaiCpu(); }\t// [GPU 11/09 BOCPU]" + NL + "\tif (!m_pGpu || !m_bGpuTarget)" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "\tif (m_pDev->m_bJxDichSeXoa) m_bJxKhongGiuCpu = true;\t// " + DAUX + " dich sap bi Clear (anh nen vung): ban CPU khong ai doc -> khong doc nguoc GPU (156 ms luc vao map: SubmitFrame + cho GPU dang ngap tai len)" + NL
           + "\tif (m_bCpuBo && !m_pCpu && !m_bJxKhongGiuCpu) { m_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1); if (m_pCpu) ThuLaiCpu(); }\t// [GPU 11/09 BOCPU]" + NL
           + "#else" + NL
           + "\tif (m_bCpuBo && !m_pCpu) { m_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1); if (m_pCpu) ThuLaiCpu(); }\t// [GPU 11/09 BOCPU]" + NL
           + "#endif" + NL
           + "\tif (!m_pGpu || !m_bGpuTarget)" + NL)
    s = thay(s, cu, moi, "PrepareAsTarget")
    cu = "\t\tif (m_bCpuBo) ThuLaiCpu();\t// [GPU 11/09 BOCPU] ban CPU da bo sau khi tai len: doc lai tu GPU (hiem; sprite chi ghi mot lan)" + NL + "\t}" + NL + "\tif (m_bGpuNewer && m_pGpu)" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "\t\tif (m_bCpuBo && !m_bJxKhongGiuCpu) ThuLaiCpu();\t// " + DAUX + " anh nen vung: khong doc nguoc" + NL
           + "#else" + NL
           + "\t\tif (m_bCpuBo) ThuLaiCpu();\t// [GPU 11/09 BOCPU] ban CPU da bo sau khi tai len: doc lai tu GPU (hiem; sprite chi ghi mot lan)" + NL
           + "#endif" + NL + "\t}" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\tif (m_bGpuNewer && m_pGpu && !m_bJxKhongGiuCpu)" + NL
           + "#else" + NL
           + "\tif (m_bGpuNewer && m_pGpu)" + NL
           + "#endif" + NL)
    s = thay(s, cu, moi, "LockRect")
    return s


def v_dev(s):
    cu = "\tm_pVS = NULL; m_pFS = NULL; m_pDummy = NULL; m_pWhite = NULL; m_pWhiteMang = NULL;\t// [KHOI 11/09]" + NL
    moi = cu + "#ifdef JX_MOBILE" + NL + "\tm_bJxDichSeXoa = false;\t// " + DAUX + NL + "#endif" + NL
    s = thay(s, cu, moi, "khoi tao CDevGpu")
    # ham xuat cho KRepresentShell3 (khong include D3D9onGPUi.h ben do)
    cu = "HRESULT CDevGpu::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "// " + DAUX + " KRepresentShell3::ClearImageData bao 'dich sap bi Clear' quanh SetRenderTarget: PrepareAsTarget khong doc nguoc GPU cho anh nen vung" + NL
           + "extern int g_nRep3ApiOn;" + NL
           + "void Rep3Gpu_DichSeXoa(IDirect3DDevice9* pDev, int bBat)" + NL
           + "{" + NL
           + "\tif (pDev && g_nRep3ApiOn == 100) ((CDevGpu*)pDev)->m_bJxDichSeXoa = (bBat != 0);" + NL
           + "}" + NL
           + "#endif" + NL + cu)
    return thay(s, cu, moi, "SetRenderTarget dinh nghia")


def v_shell(s):
    cu = ("\t\t\t\tif (SUCCEEDED(PD3DDEVICE->SetRenderTarget(0, pXDes)))" + NL
          + "\t\t\t\t{" + NL
          + "\t\t\t\t\tPD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0L);" + NL)
    moi = ("\t\t\t\t{ extern void Rep3Gpu_DichSeXoa(IDirect3DDevice9*, int); Rep3Gpu_DichSeXoa(PD3DDEVICE, 1); }\t// " + DAUX + " khong doc nguoc GPU khi chuan bi dich" + NL
           + "\t\t\t\tconst HRESULT hrXDes = PD3DDEVICE->SetRenderTarget(0, pXDes);" + NL
           + "\t\t\t\t{ extern void Rep3Gpu_DichSeXoa(IDirect3DDevice9*, int); Rep3Gpu_DichSeXoa(PD3DDEVICE, 0); }" + NL
           + "\t\t\t\tif (SUCCEEDED(hrXDes))" + NL
           + "\t\t\t\t{" + NL
           + "\t\t\t\t\tPD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0L);" + NL)
    return thay(s, cu, moi, "ClearImageData SetRenderTarget")


va("Sources/Core/Src/Scene/KScenePlaceC.cpp", v_place)
va("Sources/Represent/Represent3/D3D9onGPUi.h", v_gpui)
va("Sources/Represent/Represent3/D3D9onGPURes.cpp", v_res)
va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", v_dev)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", v_shell)
