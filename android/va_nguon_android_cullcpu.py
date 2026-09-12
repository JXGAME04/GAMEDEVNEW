# -*- coding: utf-8 -*-
r"""[CULLCPU 11/09] PORT buoc (e) cua loat [MANG 09/09] tu duong D3D11 (ban PC) sang duong SDL_GPU (ban mobile):
CULL TREN CPU CHO LENH 2D, de chu (CULLMODE=CCW) gop chung lo voi sprite (CULLMODE=NONE).

Boi canh (log Fold 7 phien SM-F966U1_20260911_173416, ban buoc 1 [CHUATLAS]):
  quad khong gop: texture0 3 687 840 (88,4 %), pipeline 470 686 (11,3 %), khong lien tiep 3 631, con lai 0.
Khoa pipeline cua duong SDL_GPU co CULLMODE o bit 34..35 (D3D9onGPUDev.cpp GetPipeline), y het nguyen nhan (e)
ben PC: KFont3 dat CULLMODE=CCW bang state block cho chu, sprite dung NONE -> moi nhan ten / dong chat / so sat
thuong cat lo quad. Ban PC sua bang cach tinh chieu quay tam giac tren CPU roi lo dung raster CULL_NONE.

Sua (chi JX_ANDROID, co cong tac [Client] Rep3CullCpu, mac dinh 1):
  1. GetPipeline -> vo boc quanh GetPipelineCull(..., dwCull): cho phep ep CULL_NONE cho lenh 2D.
  2. ComputeState: dinh XYZRHW + cull CW/CCW -> ghi nho che do cull vao s_nJxCullCpuCur, lay pipeline CULL_NONE.
  3. DrawInternal: ba duong ghi dinh (quad 4 dinh, fan, list) bo tam giac sai chieu ngay tren CPU truoc khi chep
     vao ring; ring co lai dung so dinh giu.
  4. DO: g_uJxCullGiu / g_uJxCullBo va g_uJxPipeVo[8] (tach ly do 'pipeline' theo tung truong cua khoa) in trong [VE-GOP].
Doc/ghi latin-1, giu CRLF/LF, moc khop dung 1 cho, byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_cullcpu.py
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
CFG = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
DAU = "[CULLCPU 11/09]"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    crlf = s.count("\r\n")
    if crlf and crlf != s.count("\n"):
        raise SystemExit("tep co xuong dong lan lon, khong va: " + p)
    return s.replace("\r\n", "\n"), ("\r\n" if crlf else "\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, nl, cao):
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("so byte cao doi - khong ghi: " + p)
    io.open(p, "w", encoding="latin-1", newline="").write(s.replace("\n", nl))
    print("da va:", os.path.relpath(p, GOC))


def thay1(s, cu, moi, ten):
    n = s.count(cu)
    if n != 1:
        raise SystemExit("moc '%s' khop %d cho (can 1)" % (ten, n))
    return s.replace(cu, moi)


def sau_dong(s, rx, them, ten):
    m = list(re.finditer(rx, s, re.M))
    if len(m) != 1:
        raise SystemExit("moc dong '%s' khop %d cho (can 1)" % (ten, len(m)))
    return s[:m[0].end()] + them + s[m[0].end():]


# ============================================================ BaseInclude.h
p = os.path.join(R3, "BaseInclude.h")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^extern int g_nJxAtlasManaged;[^\n]*\n",
        "extern int g_nJxCullCpu;\t// " + DAU + " [Client] Rep3CullCpu: cull tam giac 2D tren CPU de chu (CULL_CCW) gop chung lo voi sprite (CULL_NONE) - port buoc (e) cua [MANG 09/09]\n"
        "extern unsigned g_uJxCullGiu, g_uJxCullBo;\t// " + DAU + " tam giac 2D giu / bo khi cull tren CPU\n"
        "extern unsigned g_uJxPipeVo[8];\t// " + DAU + " quad vo lo vi 'pipeline': 0 fvf, 1 topo, 2 blend, 3 cull, 4 fill, 5 dinh dang target, 6 stride, 7 cung khoa (tao pipeline hong)\n",
        "extern atlas managed")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUi.h
p = os.path.join(R3, "D3D9onGPUi.h")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "\tSDL_GPUGraphicsPipeline* GetPipeline(DWORD fvf, SDL_GPUPrimitiveType topo, SDL_GPUTextureFormat rtFmt);",
        "\tSDL_GPUGraphicsPipeline* GetPipeline(DWORD fvf, SDL_GPUPrimitiveType topo, SDL_GPUTextureFormat rtFmt);\n"
        "\tSDL_GPUGraphicsPipeline* GetPipelineCull(DWORD fvf, SDL_GPUPrimitiveType topo, SDL_GPUTextureFormat rtFmt, DWORD dwCull);\t// " + DAU,
        "khai bao GetPipeline")
    s = thay1(s, "\tSDL_GPUTexture* pTarget;\t\t// RGCMD_TARGET: NULL = backbuffer",
        "\tSDL_GPUTexture* pTarget;\t\t// RGCMD_TARGET: NULL = backbuffer\n"
        "\tunsigned long long ullPipeKey;\t// " + DAU + " khoa pipeline luc ghi lenh (chi de DO: tach ly do 'pipeline' khi quad vo lo)",
        "RgCmd pTarget")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUDev.cpp
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    # 1. bien theo doi (phai nam TRUOC JxGopVo)
    s = thay1(s, "unsigned g_uJxVeKhungSo = 0, g_uJxVe8 = 0, g_uJxVe16 = 0, g_uJxGopVo[8];\n",
        "unsigned g_uJxVeKhungSo = 0, g_uJxVe8 = 0, g_uJxVe16 = 0, g_uJxGopVo[8];\n"
        "static int s_nJxCullCpuCur = 0;\t\t\t\t// " + DAU + " lenh ve hien tai: 0 = khong cull tren CPU, khac 0 = che do cull cua D3D9 (D3DCULL_CW / D3DCULL_CCW)\n"
        "static unsigned long long s_ullJxPipeKeyCur = 0;\t// " + DAU + " khoa pipeline cua lenh ve hien tai (chi de DO)\n",
        "bien ve khung so")
    # 2. JxGopVo: tach ly do 'pipeline'
    s = thay1(s, "\telse if (L.st.pPipe != st.pPipe) k = 2;\n",
        "\telse if (L.st.pPipe != st.pPipe)\n"
        "\t{\n"
        "\t\tk = 2;\n"
        "#ifdef JX_ANDROID\n"
        "\t\tconst unsigned long long x = L.ullPipeKey ^ s_ullJxPipeKeyCur;\t// " + DAU + " pipeline khac nhau o truong nao cua khoa\n"
        "\t\tif (x & 0xFFFull) g_uJxPipeVo[0]++;\t\t\t\t// fvf\n"
        "\t\tif (x & (7ull << 12)) g_uJxPipeVo[1]++;\t\t\t// topo\n"
        "\t\tif (x & (0x3FFFFull << 16)) g_uJxPipeVo[2]++;\t// blend (src/dst/op/bat/mat na ghi mau)\n"
        "\t\tif (x & (3ull << 34)) g_uJxPipeVo[3]++;\t\t\t// cull\n"
        "\t\tif (x & (3ull << 36)) g_uJxPipeVo[4]++;\t\t\t// fill\n"
        "\t\tif (x & (0xFFull << 40)) g_uJxPipeVo[5]++;\t\t// dinh dang target\n"
        "\t\tif (x & (0xFFull << 48)) g_uJxPipeVo[6]++;\t\t// stride\n"
        "\t\tif (x == 0) g_uJxPipeVo[7]++;\t\t\t\t\t// cung khoa ma khac con tro = tao pipeline hong (luu NULL)\n"
        "#endif\n"
        "\t}\n",
        "JxGopVo pipeline")
    # 3. GetPipeline -> vo boc GetPipelineCull
    s = thay1(s, "// pipeline theo (fvf, topo, blend, cull, fill, dinh dang target)\n"
        "SDL_GPUGraphicsPipeline* CDevGpu::GetPipeline(DWORD fvf, SDL_GPUPrimitiveType topo, SDL_GPUTextureFormat rtFmt)\n"
        "{\n",
        "// pipeline theo (fvf, topo, blend, cull, fill, dinh dang target)\n"
        "SDL_GPUGraphicsPipeline* CDevGpu::GetPipeline(DWORD fvf, SDL_GPUPrimitiveType topo, SDL_GPUTextureFormat rtFmt)\n"
        "{\treturn GetPipelineCull(fvf, topo, rtFmt, m_rs[D3DRS_CULLMODE] & 3);\t// " + DAU + " (nhu cu: cull lay tu trang thai hien tai)\n"
        "}\n"
        "SDL_GPUGraphicsPipeline* CDevGpu::GetPipelineCull(DWORD fvf, SDL_GPUPrimitiveType topo, SDL_GPUTextureFormat rtFmt, DWORD dwCull)\n"
        "{\n",
        "GetPipeline dau ham")
    s = thay1(s, "\t\t| ((unsigned long long)(m_rs[D3DRS_CULLMODE] & 3) << 34) | ((unsigned long long)(m_rs[D3DRS_FILLMODE] & 3) << 36)\n"
        "\t\t| ((unsigned long long)(rtFmt & 0xFF) << 40) | ((unsigned long long)(stride & 0xFF) << 48);\n",
        "\t\t| ((unsigned long long)(dwCull & 3) << 34) | ((unsigned long long)(m_rs[D3DRS_FILLMODE] & 3) << 36)\n"
        "\t\t| ((unsigned long long)(rtFmt & 0xFF) << 40) | ((unsigned long long)(stride & 0xFF) << 48);\n"
        "\ts_ullJxPipeKeyCur = key;\t// " + DAU + " chi de DO\n",
        "khoa pipeline")
    s = thay1(s, "\tswitch (m_rs[D3DRS_CULLMODE])\n", "\tswitch (dwCull)\t// " + DAU + "\n", "switch cull")
    s = thay1(s, "(unsigned)blendOn, (unsigned)m_rs[D3DRS_CULLMODE], (int)rtFmt, SDL_GetError());",
        "(unsigned)blendOn, (unsigned)dwCull, (int)rtFmt, SDL_GetError());", "log tao pipeline hong")
    # 4. ComputeState: quyet dinh cull tren CPU
    s = thay1(s, "\tmemset(&st, 0, sizeof(st));\n\tst.pPipe = GetPipeline(m_fvf, topo, CurrentTargetFmt());\n",
        "\tmemset(&st, 0, sizeof(st));\n"
        "#ifdef JX_ANDROID\n"
        "\t{\t// " + DAU + " port buoc (e) cua [MANG 09/09] (commit ac7d255b ben duong D3D11): chu dat CULLMODE=CCW, sprite dung NONE ->\n"
        "\t\t// khoa pipeline khac nhau o bit 34 -> cat lo quad. Voi lenh 2D (dinh XYZRHW) ta tu bo tam giac sai chieu tren CPU va\n"
        "\t\t// lay pipeline CULL_NONE, nen ket qua tren man hinh y het ma chu gop chung lo voi sprite.\n"
        "\t\tconst DWORD dwCull = m_rs[D3DRS_CULLMODE] & 3;\n"
        "\t\tconst bool bRhwCull = ((m_fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW);\n"
        "\t\ts_nJxCullCpuCur = (g_nJxCullCpu && bRhwCull && (dwCull == D3DCULL_CW || dwCull == D3DCULL_CCW)) ? (int)dwCull : 0;\n"
        "\t\tst.pPipe = GetPipelineCull(m_fvf, topo, CurrentTargetFmt(), s_nJxCullCpuCur ? (DWORD)D3DCULL_NONE : dwCull);\n"
        "\t}\n"
        "#else\n"
        "\tst.pPipe = GetPipeline(m_fvf, topo, CurrentTargetFmt());\n"
        "#endif\n",
        "ComputeState pipeline")
    # 5. ham thu chieu quay tam giac
    s = thay1(s, "// ---------------------------------------------------------------- ve\n",
        "// ---------------------------------------------------------------- ve\n"
        "#ifdef JX_ANDROID\n"
        "// " + DAU + " chieu quay tam giac 2D tren man hinh (y huong xuong): cr > 0 = thuan chieu kim dong ho = mat truoc cua D3D9.\n"
        "// D3DCULL_CCW bo cr < 0, D3DCULL_CW bo cr > 0, cr == 0 (tam giac det) bo. Giong het ban PC [MANG 09/09 e].\n"
        "static bool JxGiuTamGiac(const BYTE* pV, UINT stride, UINT i0, UINT i1, UINT i2, int nCull)\n"
        "{\n"
        "\tconst float* p0 = (const float*)(pV + i0 * stride);\n"
        "\tconst float* p1 = (const float*)(pV + i1 * stride);\n"
        "\tconst float* p2 = (const float*)(pV + i2 * stride);\n"
        "\tconst float cr = (p1[0] - p0[0]) * (p2[1] - p0[1]) - (p1[1] - p0[1]) * (p2[0] - p0[0]);\n"
        "\tif (cr == 0.0f || (nCull == D3DCULL_CCW ? (cr < 0.0f) : (cr > 0.0f))) { g_uJxCullBo++; return false; }\n"
        "\tg_uJxCullGiu++;\n"
        "\treturn true;\n"
        "}\n"
        "#endif\n",
        "dau khu vuc ve")
    # 6. duong quad (strip 4 dinh)
    s = thay1(s, "\t\tstatic const int s_idx[6] = { 0, 1, 2, 2, 1, 3 };\n"
        "\t\tm_ring.resize(ringOff + 6 * s2);\n"
        "\t\tBYTE* d = &m_ring[ringOff];\n"
        "\t\tfor (int i = 0; i < 6; i++) { memcpy(d + i * s2, pVerts + s_idx[i] * stride, stride); *(UINT*)(d + i * s2 + stride) = uPal; }\n"
        "\t\tRgAtlasUv(d, 6, s2, m_fvf, m_tex[0], fPage);\t// [GPU 11/09 ATLAS]\n"
        "\t\tnOut = 6;\n"
        "\t\tm_uQuads++;\n",
        "\t\tstatic const int s_idx[6] = { 0, 1, 2, 2, 1, 3 };\n"
        "\t\tm_ring.resize(ringOff + 6 * s2);\n"
        "\t\tBYTE* d = &m_ring[ringOff];\n"
        "\t\tnOut = 0;\n"
        "\t\tfor (int t = 0; t < 2; t++)\n"
        "\t\t{\n"
        "#ifdef JX_ANDROID\n"
        "\t\t\tif (s_nJxCullCpuCur && !JxGiuTamGiac(pVerts, stride, s_idx[t * 3], s_idx[t * 3 + 1], s_idx[t * 3 + 2], s_nJxCullCpuCur)) continue;\t// " + DAU + "\n"
        "#endif\n"
        "\t\t\tfor (int k = 0; k < 3; k++) { BYTE* q = d + (nOut + k) * s2; memcpy(q, pVerts + s_idx[t * 3 + k] * stride, stride); *(UINT*)(q + stride) = uPal; }\n"
        "\t\t\tnOut += 3;\n"
        "\t\t}\n"
        "\t\tm_uQuads++;\n"
        "\t\tif (nOut != 6) m_ring.resize(ringOff + (size_t)nOut * s2);\t// " + DAU + " thu hep khong bao gio cap phat lai -> con tro d van dung\n"
        "\t\tif (nOut == 0) return D3D_OK;\t// " + DAU + " ca quad bi cull\n"
        "\t\tRgAtlasUv(d, nOut, s2, m_fvf, m_tex[0], fPage);\t// [GPU 11/09 ATLAS]\n",
        "duong quad")
    s = thay1(s, "\t\t\t{ L.nVerts += 6; return D3D_OK; }\n",
        "\t\t\t{ L.nVerts += nOut; return D3D_OK; }\t// " + DAU + " nOut = 3 hoac 6 (co the da cull bot)\n", "gop lenh truoc")
    # 7. duong fan
    s = thay1(s, "\t\tfor (UINT i = 0; i < nTri; i++)\n"
        "\t\t{\n"
        "\t\t\tconst UINT src[3] = { 0, i + 1, i + 2 };\n"
        "\t\t\tfor (int k = 0; k < 3; k++) { memcpy(d + (i * 3 + k) * s2, pVerts + src[k] * stride, stride); *(UINT*)(d + (i * 3 + k) * s2 + stride) = uPal; }\n"
        "\t\t}\n"
        "\t\tnOut = nTri * 3;\n"
        "\t\tRgAtlasUv(d, nOut, s2, m_fvf, m_tex[0], fPage);\t// [GPU 11/09 ATLAS]\n",
        "\t\tnOut = 0;\n"
        "\t\tfor (UINT i = 0; i < nTri; i++)\n"
        "\t\t{\n"
        "\t\t\tconst UINT src[3] = { 0, i + 1, i + 2 };\n"
        "#ifdef JX_ANDROID\n"
        "\t\t\tif (s_nJxCullCpuCur && !JxGiuTamGiac(pVerts, stride, src[0], src[1], src[2], s_nJxCullCpuCur)) continue;\t// " + DAU + "\n"
        "#endif\n"
        "\t\t\tfor (int k = 0; k < 3; k++) { BYTE* q = d + (nOut + k) * s2; memcpy(q, pVerts + src[k] * stride, stride); *(UINT*)(q + stride) = uPal; }\n"
        "\t\t\tnOut += 3;\n"
        "\t\t}\n"
        "\t\tif (nOut != nTri * 3) m_ring.resize(ringOff + (size_t)nOut * s2);\t// " + DAU + "\n"
        "\t\tif (nOut == 0) return D3D_OK;\t// " + DAU + "\n"
        "\t\tRgAtlasUv(d, nOut, s2, m_fvf, m_tex[0], fPage);\t// [GPU 11/09 ATLAS]\n",
        "duong fan")
    # 8. duong chung (danh sach tam giac)
    s = thay1(s, "\t\tm_ring.resize(ringOff + (size_t)nVerts * s2);\n"
        "\t\tBYTE* d = &m_ring[ringOff];\n"
        "\t\tfor (UINT i = 0; i < nVerts; i++) { memcpy(d + i * s2, pVerts + i * stride, stride); *(UINT*)(d + i * s2 + stride) = uPal; }\n"
        "\t\tRgAtlasUv(d, nVerts, s2, m_fvf, m_tex[0], fPage);\t// [GPU 11/09 ATLAS]\n",
        "\t\tm_ring.resize(ringOff + (size_t)nVerts * s2);\n"
        "\t\tBYTE* d = &m_ring[ringOff];\n"
        "#ifdef JX_ANDROID\n"
        "\t\tif (s_nJxCullCpuCur && type == D3DPT_TRIANGLELIST && (nVerts % 3) == 0)\n"
        "\t\t{\t// " + DAU + " danh sach tam giac 2D: bo tam giac sai chieu ngay tren CPU\n"
        "\t\t\tnOut = 0;\n"
        "\t\t\tfor (UINT i = 0; i + 2 < nVerts; i += 3)\n"
        "\t\t\t{\n"
        "\t\t\t\tif (!JxGiuTamGiac(pVerts, stride, i, i + 1, i + 2, s_nJxCullCpuCur)) continue;\n"
        "\t\t\t\tfor (int k = 0; k < 3; k++) { BYTE* q = d + (nOut + k) * s2; memcpy(q, pVerts + (i + k) * stride, stride); *(UINT*)(q + stride) = uPal; }\n"
        "\t\t\t\tnOut += 3;\n"
        "\t\t\t}\n"
        "\t\t\tif (nOut != nVerts) m_ring.resize(ringOff + (size_t)nOut * s2);\n"
        "\t\t\tif (nOut == 0) return D3D_OK;\n"
        "\t\t}\n"
        "\t\telse\n"
        "#endif\n"
        "\t\tfor (UINT i = 0; i < nVerts; i++) { memcpy(d + i * s2, pVerts + i * stride, stride); *(UINT*)(d + i * s2 + stride) = uPal; }\n"
        "\t\tRgAtlasUv(d, nOut, s2, m_fvf, m_tex[0], fPage);\t// [GPU 11/09 ATLAS]\n",
        "duong chung")
    # 9. ghi khoa pipeline vao lenh (chi de DO)
    s = thay1(s, "\tRgCmd c; memset(&c, 0, sizeof(c)); c.type = RGCMD_DRAW; c.st = st; c.ringOff = ringOff; c.nVerts = nOut; c.stride = stride;\n",
        "\tRgCmd c; memset(&c, 0, sizeof(c)); c.type = RGCMD_DRAW; c.st = st; c.ringOff = ringOff; c.nVerts = nOut; c.stride = stride;\n"
        "\tc.ullPipeKey = s_ullJxPipeKeyCur;\t// " + DAU + " chi de DO\n",
        "ghi lenh")
    ghi(p, s, nl, cao)

# ============================================================ KRepresentShell3.cpp
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^unsigned g_uJxAtlasODat\[2\] = \{ 0, 0 \}, g_uJxAtlasOMoi = 0;[^\n]*\n",
        "int g_nJxCullCpu = 1;\t// " + DAU + "\n"
        "unsigned g_uJxCullGiu = 0, g_uJxCullBo = 0, g_uJxPipeVo[8] = { 0 };\t// " + DAU + "\n",
        "dinh nghia atlas o dat")
    s = thay1(s, "\tg_nJxAtlasManaged   = Rep3Ini(\"Rep3AtlasManaged\", 1) ? 1 : 0;",
        "\tg_nJxCullCpu        = Rep3Ini(\"Rep3CullCpu\", 1) ? 1 : 0;\t// " + DAU + " 1 = cull tam giac 2D tren CPU de chu gop chung lo voi sprite (port buoc (e) cua [MANG 09/09]); 0 = nhu cu\n"
        "\tg_nJxAtlasManaged   = Rep3Ini(\"Rep3AtlasManaged\", 1) ? 1 : 0;",
        "doc ini atlas managed")
    s = thay1(s, "| o atlas: DEFAULT %u, MANAGED %u (managed=%d), xin o moi %u\",\n",
        "| o atlas: DEFAULT %u, MANAGED %u (managed=%d), xin o moi %u | cull cpu=%d: giu %u bo %u | pipeline vo: fvf %u, topo %u, blend %u, cull %u, fill %u, rt %u, stride %u, khac %u\",\n",
        "[VE-GOP] format")
    s = thay1(s, "g_uJxAtlasODat[0], g_uJxAtlasODat[1], g_nJxAtlasManaged, g_uJxAtlasOMoi);",
        "g_uJxAtlasODat[0], g_uJxAtlasODat[1], g_nJxAtlasManaged, g_uJxAtlasOMoi,\n"
        "\t\tg_nJxCullCpu, g_uJxCullGiu, g_uJxCullBo, g_uJxPipeVo[0], g_uJxPipeVo[1], g_uJxPipeVo[2], g_uJxPipeVo[3], g_uJxPipeVo[4], g_uJxPipeVo[5], g_uJxPipeVo[6], g_uJxPipeVo[7]);",
        "[VE-GOP] args")
    s = thay1(s, "g_uJxAtlasODat[0] = g_uJxAtlasODat[1] = 0; g_uJxAtlasOMoi = 0;\t// [CHUATLAS 11/09]",
        "g_uJxAtlasODat[0] = g_uJxAtlasODat[1] = 0; g_uJxAtlasOMoi = 0;\t// [CHUATLAS 11/09]\n"
        "\tg_uJxCullGiu = g_uJxCullBo = 0; memset(g_uJxPipeVo, 0, sizeof(g_uJxPipeVo));\t// " + DAU,
        "dat lai ky")
    ghi(p, s, nl, cao)

# ============================================================ config.ini
p = CFG
s, nl, cao = doc(p)
if "Rep3CullCpu" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "Rep3AtlasManaged=1\n",
        "Rep3AtlasManaged=1\n"
        "; " + DAU + " Rep3CullCpu=1: cull tam giac 2D tren CPU roi lo dung raster CULL_NONE - port buoc (e) cua loat [MANG 09/09]\n"
        ";   ben duong D3D11 cua ban PC (commit ac7d255b). Chu dat CULLMODE=CCW con sprite dung NONE -> khoa pipeline khac nhau\n"
        ";   -> cat lo quad (mobile: 'pipeline' chiem 11,3 %% ly do khong gop). Ket qua tren man hinh y het. 0 = nhu cu.\n"
        ";   Doc [VE-GOP] \"cull cpu=1: giu N bo M\" va \"pipeline vo: ... cull K\": K phai tut ve gan 0.\n"
        "Rep3CullCpu=1\n",
        "config Rep3AtlasManaged")
    ghi(p, s, nl, cao)

print("xong")
