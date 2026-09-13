# -*- coding: utf-8 -*-
# [KHOI 13/09 SUA] Trang atlas RONG nam trong KHOI bi huy CA TEXTURE KHOI dung chung -> SIGSEGV o SDL_BindGPUFragmentSamplers.
#
# Phien WAuto do 12/09 12:15-12:26 (BANGIAO_WAUTO_MOBILE_HOANTHIEN_1209.md 6.2): Rep3AtlasKhoi=1, mo the Co ban (7 o chu doi
# moi 500 ms) la sap sau ~145 ms, signal 11 dia chi 0x?00000029 tai D3D9onGPUDev.cpp "SDL_BindGPUFragmentSamplers(pass, 2, tk, 12)";
# "mot o doi thi chua sao, hai o tro len la sap". Phien do phai dat Rep3AtlasKhoi=0 tren may ao.
#
# GOC (D3D9onGPURes.cpp, CAtlasMgrGpu::Free va JxFreeKe): trang rong THU HAI cung lop/dinh dang thi tra GPU:
#     if (g_nJxAtlasMang) JxTraLop(pPage);            // cum: chi tra lop
#     else if (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);   // texture rieng: huy
# KHOI bat buoc AtlasMang=0 (KRepresentShell3 tu tat khi bat cung C1) nen trang trong KHOI roi vao nhanh DeferRelease,
# ma pPage->m_pTex chinh la texture MANG cua CA KHOI: 12 khe sampler gan chet + moi trang khac cung khoi con dang ve tu no.
# Khung sau, tk[q].texture = m_jxKhoiV[k].pTex tro vao texture da huy -> SDL doc rac -> SIGSEGV. Chu doi lien tuc = trang
# chu day roi rong nhanh -> lo ra som; trong Tong Kim (ten/so sat thuong/chat doi lien tuc) cung co the dinh.
# ReleaseAll cung huy texture khoi HAI LAN (moi trang + vong khoi) khi huy thiet bi.
#
# SUA (chi trong khoi #ifdef JX_MOBILE, ban PC khong doi): trang trong khoi -> JxTraLopKhoi (tra LOP ve khoi, giu texture;
# NewPage cap lai lop do va QueueZeroUpload nhu duong cum); ReleaseAll bo qua trang trong khoi (vong khoi huy mot lan).
# Chay lai vo hai (neo bang su vang mat cua dau "[KHOI 13/09 SUA]").
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
CPP = "Sources/Represent/Represent3/D3D9onGPURes.cpp"
H = "Sources/Represent/Represent3/D3D9onGPUi.h"
DAU = "[KHOI 13/09 SUA]"
NL = "\r\n"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, n, ten):
    if s.count(cu) != n:
        raise SystemExit("%s: neo '%s...' thay %d lan, can %d" % (ten, cu.strip()[:50], s.count(cu), n))
    return s.replace(cu, moi)


# ---- D3D9onGPUi.h: khai bao
h = doc(H)
if DAU in h:
    print("da va roi, bo qua:", H)
else:
    assert NL in h, H + " khong phai CRLF"
    cu = "\tvoid JxTraLop(CAtlasPageGpu* pPage);\t// tra lop ve cum (KHONG huy texture cum)" + NL
    moi = cu + "\tvoid JxTraLopKhoi(CAtlasPageGpu* pPage);\t// " + DAU + " tra lop ve KHOI (KHONG huy texture khoi: 12 khe sampler + cac trang khac dang dung chung)" + NL
    h = thay(h, cu, moi, 1, H)
    ghi(H, h)
    print("da va:", H)

# ---- D3D9onGPURes.cpp
c = doc(CPP)
if DAU in c:
    print("da va roi, bo qua:", CPP)
else:
    assert NL in c, CPP + " khong phai CRLF"
    # 1. hai duong tra trang rong (Free + JxFreeKe)
    cu = ("\t\t\tif (g_nJxAtlasMang) JxTraLop(pPage);\t// [MANG 11/09] texture la cua CUM (dung chung): chi tra lop, khong huy" + NL
          + "\t\t\telse if (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);" + NL)
    moi = ("\t\t\tif (pPage->m_nKhoi != 0xFFu) JxTraLopKhoi(pPage);\t// " + DAU + " texture la cua KHOI (dung chung, gan chet khe sampler 2+k): chi tra lop, KHONG huy"
           " - huy la 12 khe + moi trang khac trong khoi tro vao texture chet -> SIGSEGV o SDL_BindGPUFragmentSamplers (the Co ban WAuto, chu doi 500 ms)" + NL
           + "\t\t\telse if (g_nJxAtlasMang) JxTraLop(pPage);\t// [MANG 11/09] texture la cua CUM (dung chung): chi tra lop, khong huy" + NL
           + "\t\t\telse if (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);" + NL)
    c = thay(c, cu, moi, 2, CPP + " (tra trang rong)")
    # 2. ReleaseAll: khong huy texture khoi theo tung trang
    cu = ("\t\tif (p->m_pTex && m_pDev->m_pGpu && !g_nJxAtlasMang) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, p->m_pTex);"
          "\t// [MANG 11/09] texture cum huy o duoi, khong huy theo tung trang" + NL)
    moi = ("\t\tif (p->m_pTex && m_pDev->m_pGpu && !g_nJxAtlasMang && p->m_nKhoi == 0xFFu) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, p->m_pTex);"
           "\t// [MANG 11/09] texture cum huy o duoi, khong huy theo tung trang; " + DAU + " trang trong KHOI cung vay (vong khoi duoi huy mot lan, khong huy hai lan)" + NL)
    c = thay(c, cu, moi, 1, CPP + " (ReleaseAll)")
    # 3. ham moi, ngay sau JxTraLop (trong #ifdef JX_MOBILE)
    cu = ("void CAtlasMgrGpu::JxTraLop(CAtlasPageGpu* pPage)" + NL + "{" + NL
          + "\tif (!pPage || !pPage->m_pTex) return;" + NL
          + "\tfor (size_t i = 0; i < m_jxCum.size(); i++)" + NL
          + "\t\tif (m_jxCum[i].pTex == pPage->m_pTex) { m_jxCum[i].lopTrong.push_back(pPage->m_nLop); return; }" + NL
          + "}" + NL)
    moi = cu + NL.join([
        "",
        "// " + DAU + " trang nam trong KHOI: tra LOP ve khoi de JxCapKhoi cap lai (NewPage se QueueZeroUpload lop do), texture khoi GIU NGUYEN",
        "// vi gan chet khe sampler 2+k va cac trang khac trong khoi van dang ve tu no. Truoc day roi vao DeferRelease(pPage->m_pTex) cua duong",
        "// \"texture rieng\" (KHOI bat buoc AtlasMang=0) -> huy ca khoi -> khung sau SDL_BindGPUFragmentSamplers doc texture da chet (SIGSEGV 0x..29).",
        "void CAtlasMgrGpu::JxTraLopKhoi(CAtlasPageGpu* pPage)",
        "{",
        "\tif (!pPage || !pPage->m_pTex || pPage->m_nKhoi >= (UINT)m_jxKhoiV.size()) return;",
        "\tJxKhoi& k = m_jxKhoiV[pPage->m_nKhoi];",
        "\tif (k.pTex != pPage->m_pTex) return;",
        "\tk.lopTrong.push_back(pPage->m_nLop);",
        "}",
        "",
    ])
    c = thay(c, cu, moi, 1, CPP + " (JxTraLop)")
    ghi(CPP, c)
    print("da va:", CPP)
