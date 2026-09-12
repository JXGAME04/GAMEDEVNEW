# -*- coding: utf-8 -*-
r"""[KHOI 11/09] PORT buoc (f) cua loat [MANG 09/09] tu duong D3D11 (ban PC) sang duong SDL_GPU (ban mobile):
ATLAS THEO KHOI CO DINH + NHIEU MANG GAN CHET KHE SAMPLER.

Boi canh (log Fold 7 phien SM-F966U1_20260911_180949, ban [CULLCPU]):
  quad khong gop: texture0 99,9 % (pipeline da ve 0,05 % sau buoc 2). Doi texture 1 653-1 941 lan/khung (dinh 3 492).
  Atlas cuoi phien 56 trang 2048x2048, 464 MB, TAT CA cung mot ho dinh dang (2 byte/diem anh).
Moi trang atlas la mot texture RIENG -> cu nhay trang la cat lo. Ban PC (commit 5311778b) giai bang cach gom cac
trang thanh KHOI (texture mang 2D nhieu lop) roi GAN CHET tung khoi vao mot khe sampler; dinh mang chi so khoi + lop
nen khong bao gio phai doi binding.

Khac ban PC o dau (bat buoc, da do):
  - PC co 128 khe sampler (D3D11) nen dung 16 khoi MOI dinh dang (t3..t18 R8G8, t19..t34 BGRA8).
  - SDL_GPU chot cung 16 khe MOI TANG (MAX_TEXTURE_SAMPLERS_PER_STAGE, SDL_sysgpu.h:29). Dang dung 2 (g_t0, g_t1)
    -> mobile dung 8 khoi CHUNG cho moi dinh dang, o khe 2..9; hai storage buffer (bang mau, bang ps) doi ve 10, 11.
  - Trang mobile la 2048x2048 (PC 1024x1024) nen khoi 8 lop = 64 MB, y het ngan sach mot khoi cua PC.
  - Khe 0 GIU sampler2D: texture KHONG vao atlas van la texture thuong, khong ep thanh mang (C1 [MANG 11/09] ep ca
    g_t0 thanh mang - day la mot khac biet nua so voi ban da tat).
  - CHUA lam phan "khoi rong tra VRAM" cua PC: tren mobile trang atlas khong bi huy luc chay, va giu chi so khoi co
    dinh thi it rui ro hon. Ghi lai de lam sau neu do thay ton bo nho.

O PALROW (moi dinh, 32 bit): 0..12 hang bang mau | 13..24 chi so to hop ps | 25..27 lop trong khoi | 28..30 chi so khoi
| 31 = 1 nghia la texture tang 0 nam trong khoi atlas (0 = texture rieng, lay o khe g_t0 nhu cu).

Cong tac [Client] Rep3AtlasKhoi MAC DINH 0 (tat) - bat bang config cua bo tai dt_v4 de lui duoc ma khong can APK,
dung bai hoc cua C1. Can Rep3PalBuffer=1 va Rep3PsBuffer=1; thieu thi tu tat va ghi log.
Doc/ghi latin-1, giu CRLF/LF, moc khop dung 1 cho, byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_khoi.py
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
CFG = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
DAU = "[KHOI 11/09]"


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
    s = sau_dong(s, r"^extern int g_nJxCullCpu;[^\n]*\n",
        "extern int g_nJxAtlasKhoi, g_nJxAtlasKhoiLop;\t// " + DAU + " [Client] Rep3AtlasKhoi / Rep3AtlasKhoiLop: atlas theo khoi co dinh, moi khoi gan chet mot khe sampler - port buoc (f) cua [MANG 09/09]\n"
        "extern unsigned g_uJxKhoiSo, g_uJxKhoiMB, g_uJxKhoiHet;\t// " + DAU + " so khoi da cap, tong MB, so lan xin trang khi da het khoi (trang do lui ve texture rieng)\n",
        "extern cull cpu")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUi.h
p = os.path.join(R3, "D3D9onGPUi.h")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    # trang biet minh o khoi nao
    s = thay1(s, "\tCAtlasPageGpu() : m_pTex(NULL), m_fmt(SDL_GPU_TEXTUREFORMAT_INVALID), m_bpp(0), m_binH(0), m_rows(0), m_used(0), m_nLop(0) {}",
        "\tCAtlasPageGpu() : m_pTex(NULL), m_fmt(SDL_GPU_TEXTUREFORMAT_INVALID), m_bpp(0), m_binH(0), m_rows(0), m_used(0), m_nLop(0), m_nKhoi(0xFFu) {}",
        "khoi tao trang atlas")
    s = thay1(s, "\tUINT m_nLop;\t// [MANG 11/09] lop cua trang trong texture mang cua cum (m_pTex la texture DUNG CHUNG cua cum - khong duoc huy rieng)",
        "\tUINT m_nLop;\t// [MANG 11/09] lop cua trang trong texture mang cua cum (m_pTex la texture DUNG CHUNG cua cum - khong duoc huy rieng)\n"
        "\tUINT m_nKhoi;\t// " + DAU + " chi so KHOI atlas (0..7 = khe sampler 2+k); 0xFF = khong o khoi nao (texture rieng nhu cu)",
        "truong m_nLop")
    # bo quan ly khoi
    s = thay1(s, "\tstruct JxCum { SDL_GPUTexture* pTex; SDL_GPUTextureFormat fmt; UINT bpp; UINT nLop, nLopTiep; std::vector<UINT> lopTrong; };",
        "\t// " + DAU + " KHOI atlas: texture mang 2D nLop lop, GAN CHET vao khe sampler 2+nKhoi va khong bao gio doi trong ca khung\n"
        "\tstruct JxKhoi { SDL_GPUTexture* pTex; SDL_GPUTextureFormat fmt; UINT bpp; UINT nLop, nDung; std::vector<UINT> lopTrong; };\n"
        "\tstd::vector<JxKhoi> m_jxKhoiV;\n"
        "\tbool JxCapKhoi(SDL_GPUTextureFormat fmt, UINT bpp, SDL_GPUTexture** ppTex, UINT* pKhoi, UINT* pLop);\n"
        "\tSDL_GPUTexture* JxKhoiTex(UINT i) const { return (i < m_jxKhoiV.size()) ? m_jxKhoiV[i].pTex : NULL; }\n"
        "\tUINT JxKhoiSo() const { return (UINT)m_jxKhoiV.size(); }\n"
        "\tstruct JxCum { SDL_GPUTexture* pTex; SDL_GPUTextureFormat fmt; UINT bpp; UINT nLop, nLopTiep; std::vector<UINT> lopTrong; };",
        "struct JxCum")
    # texture biet khoi cua no
    s = thay1(s, "\tUINT  JxLop() const { return (m_bVirtual && m_pPage) ? m_pPage->m_nLop : 0; }\t// [MANG 11/09] lop trong texture mang (texture rieng = 0)",
        "\tUINT  JxLop() const { return (m_bVirtual && m_pPage) ? m_pPage->m_nLop : 0; }\t// [MANG 11/09] lop trong texture mang (texture rieng = 0)\n"
        "\tUINT  JxKhoi() const { return (m_bVirtual && m_pPage) ? m_pPage->m_nKhoi : 0xFFu; }\t// " + DAU + " khoi atlas (0xFF = texture rieng)",
        "JxLop")
    # texture mang 1x1 cho khe khoi chua dung
    s = thay1(s, "\tSDL_GPUTexture* m_pWhite;\t\t\t// texture 1x1 trang cho stage khong texture",
        "\tSDL_GPUTexture* m_pWhite;\t\t\t// texture 1x1 trang cho stage khong texture\n"
        "\tSDL_GPUTexture* m_pWhiteMang;\t\t// " + DAU + " texture MANG 1x1 x 1 lop: gan vao cac khe khoi chua co khoi (SDL doi moi sampler khai bao phai duoc gan)",
        "m_pWhite")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPURes.cpp
p = os.path.join(R3, "D3D9onGPURes.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    # 1. NewPage: xin lop trong khoi truoc
    s = thay1(s, "\tSDL_GPUTexture* pTex = NULL; UINT uLop = 0;\n"
        "#ifdef JX_ANDROID\n"
        "\tif (g_nJxAtlasMang)\n"
        "\t{\t// [MANG 11/09] trang = mot LOP trong texture mang cua cum (hai trang cung cum -> cung texture0 -> gop duoc lenh ve)\n"
        "\t\tif (!JxCapLop(fmt, bpp, &pTex, &uLop)) return NULL;\n"
        "\t}\n"
        "\telse\n"
        "#endif\n",
        "\tSDL_GPUTexture* pTex = NULL; UINT uLop = 0, uKhoi = 0xFFu;\n"
        "#ifdef JX_ANDROID\n"
        "\t// " + DAU + " trang = mot LOP trong KHOI atlas (khoi gan chet khe sampler) -> moi trang atlas dung chung mot bo binding.\n"
        "\t// Het khoi thi lui ve texture rieng nhu cu (khong loi), y nhu ban PC [MANG 09/09 f].\n"
        "\tif (g_nJxAtlasKhoi && JxCapKhoi(fmt, bpp, &pTex, &uKhoi, &uLop)) { }\n"
        "\telse if (g_nJxAtlasMang)\n"
        "\t{\t// [MANG 11/09] trang = mot LOP trong texture mang cua cum (hai trang cung cum -> cung texture0 -> gop duoc lenh ve)\n"
        "\t\tif (!JxCapLop(fmt, bpp, &pTex, &uLop)) return NULL;\n"
        "\t}\n"
        "\telse\n"
        "#endif\n",
        "NewPage tao texture")
    s = thay1(s, "\tp->m_nLop = uLop;\t// [MANG 11/09]\n",
        "\tp->m_nLop = uLop;\t// [MANG 11/09]\n"
        "#ifdef JX_ANDROID\n"
        "\tp->m_nKhoi = uKhoi;\t// " + DAU + "\n"
        "#endif\n",
        "gan m_nLop")
    # 2. ham cap khoi (dat ngay truoc JxCapLop)
    s = thay1(s, "bool CAtlasMgrGpu::JxCapLop(SDL_GPUTextureFormat fmt, UINT bpp, SDL_GPUTexture** ppTex, UINT* pLop)\n",
        "// " + DAU + " port buoc (f) cua [MANG 09/09] (commit 5311778b ben duong D3D11): xin mot LOP trong mot KHOI atlas.\n"
        "// Khoi = texture mang 2D co so lop CO DINH (khong bao gio lon len, khong bao gio phai CHEP lai - day la cho C1 sai:\n"
        "// C1 cho cum lon dan nen moi lan lon la mot cum MOI, cac trang cu nam rai o nhieu texture -> van doi binding).\n"
        "// Het JX_KHOI_MAX khoi -> tra false, NewPage lui ve texture rieng (khong loi).\n"
        "#define JX_KHOI_MAX 8\t// so khe sampler danh cho khoi (khe 2..9); SDL_GPU chot 16 khe moi tang\n"
        "bool CAtlasMgrGpu::JxCapKhoi(SDL_GPUTextureFormat fmt, UINT bpp, SDL_GPUTexture** ppTex, UINT* pKhoi, UINT* pLop)\n"
        "{\n"
        "\tfor (size_t i = 0; i < m_jxKhoiV.size(); i++)\n"
        "\t{\n"
        "\t\tJxKhoi& k = m_jxKhoiV[i];\n"
        "\t\tif (k.fmt != fmt) continue;\n"
        "\t\tif (!k.lopTrong.empty()) { *ppTex = k.pTex; *pKhoi = (UINT)i; *pLop = k.lopTrong.back(); k.lopTrong.pop_back(); return true; }\n"
        "\t\tif (k.nDung < k.nLop) { *ppTex = k.pTex; *pKhoi = (UINT)i; *pLop = k.nDung++; return true; }\n"
        "\t}\n"
        "\tif (m_jxKhoiV.size() >= JX_KHOI_MAX) { g_uJxKhoiHet++; return false; }\n"
        "\tUINT nLop = (UINT)g_nJxAtlasKhoiLop; if (nLop < 1) nLop = 1; if (nLop > 8) nLop = 8;\t// 8 lop = 3 bit trong o PALROW\n"
        "\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci));\n"
        "\tci.type = SDL_GPU_TEXTURETYPE_2D_ARRAY; ci.format = fmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;\n"
        "\tci.width = m_pageSize; ci.height = m_pageSize; ci.layer_count_or_depth = nLop; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;\n"
        "\tSDL_GPUTexture* pTex = SDL_CreateGPUTexture(m_pDev->m_pGpu, &ci);\n"
        "\tif (!pTex)\n"
        "\t{\n"
        "\t\tRgLog(\"[KHOI] khoi atlas #%u: CreateGPUTexture %ux%u x %u lop fmt %d that bai: %s\", (unsigned)m_jxKhoiV.size(), m_pageSize, m_pageSize, nLop, (int)fmt, SDL_GetError());\n"
        "\t\tg_uJxKhoiHet++;\n"
        "\t\treturn false;\n"
        "\t}\n"
        "\tJxKhoi k; k.pTex = pTex; k.fmt = fmt; k.bpp = bpp; k.nLop = nLop; k.nDung = 1;\n"
        "\tm_jxKhoiV.push_back(k);\n"
        "\tg_uJxKhoiSo = (unsigned)m_jxKhoiV.size();\n"
        "\tg_uJxKhoiMB += (unsigned)(((unsigned __int64)m_pageSize * m_pageSize * bpp * nLop) >> 20);\n"
        "\tRgLog(\"[KHOI] khoi atlas moi #%u: %ux%u x %u lop fmt %d (%u MB) -> khe sampler %u; tong %u khoi, %u MB\",\n"
        "\t\t(unsigned)(m_jxKhoiV.size() - 1), m_pageSize, m_pageSize, nLop, (int)fmt,\n"
        "\t\t(unsigned)(((unsigned __int64)m_pageSize * m_pageSize * bpp * nLop) >> 20), (unsigned)(2 + m_jxKhoiV.size() - 1), g_uJxKhoiSo, g_uJxKhoiMB);\n"
        "\t*ppTex = pTex; *pKhoi = (UINT)(m_jxKhoiV.size() - 1); *pLop = 0;\n"
        "\treturn true;\n"
        "}\n"
        "\n"
        "bool CAtlasMgrGpu::JxCapLop(SDL_GPUTextureFormat fmt, UINT bpp, SDL_GPUTexture** ppTex, UINT* pLop)\n",
        "dau JxCapLop")
    # 3. huy khoi luc dong thiet bi
    s = thay1(s, "\tm_jxCum.clear(); g_uJxAtlasCum = 0;\n",
        "\tm_jxCum.clear(); g_uJxAtlasCum = 0;\n"
        "\tfor (size_t i = 0; i < m_jxKhoiV.size(); i++)\t// " + DAU + " huy texture cua tung khoi\n"
        "\t\tif (m_jxKhoiV[i].pTex && m_pDev->m_pGpu) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, m_jxKhoiV[i].pTex);\n"
        "\tm_jxKhoiV.clear(); g_uJxKhoiSo = 0; g_uJxKhoiMB = 0;\n",
        "huy cum")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUDev.cpp
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    # 1. chon shader
    s = thay1(s, "\t\t\tif (g_nJxAtlasMang) { si.code = g_Rep3GpuFSPalPsMang; si.code_size = sizeof(g_Rep3GpuFSPalPsMang); }\t// [MANG 11/09] sampler2DArray, lop lay tu dinh\n",
        "\t\t\tif (g_nJxAtlasMang) { si.code = g_Rep3GpuFSPalPsMang; si.code_size = sizeof(g_Rep3GpuFSPalPsMang); }\t// [MANG 11/09] sampler2DArray, lop lay tu dinh\n"
        "\t\t\telse if (g_nJxAtlasKhoi)\n"
        "\t\t\t{\t// " + DAU + " 8 khoi atlas o khe 2..9 (sampler2DArray, gan chet ca khung); hai storage buffer doi ve 10, 11\n"
        "\t\t\t\tsi.code = g_Rep3GpuFSPalPsKhoi; si.code_size = sizeof(g_Rep3GpuFSPalPsKhoi); si.num_samplers = 2 + 8;\n"
        "\t\t\t}\n",
        "chon shader mang")
    # 2. tu tat neu thieu dieu kien
    s = thay1(s, "\tm_pFS = SDL_CreateGPUShader(m_pGpu, &si);\n",
        "#ifdef JX_ANDROID\n"
        "\tif (g_nJxAtlasKhoi && !(g_nJxPalBuffer && g_nJxPsBuffer && !g_nJxAtlasMang))\n"
        "\t{\t// " + DAU + " khoi can bang mau + bang ps o storage buffer (o PALROW moi du bit) va khong duoc bat cung C1\n"
        "\t\tRgLog(\"[KHOI] Rep3AtlasKhoi=1 nhung Rep3PalBuffer=%d Rep3PsBuffer=%d Rep3AtlasMang=%d -> TU TAT\", g_nJxPalBuffer, g_nJxPsBuffer, g_nJxAtlasMang);\n"
        "\t\tg_nJxAtlasKhoi = 0;\n"
        "\t}\n"
        "#endif\n"
        "\tm_pFS = SDL_CreateGPUShader(m_pGpu, &si);\n",
        "tao FS")
    # 3. texture mang 1x1 cho khe khoi chua dung
    s = thay1(s, "\t\tm_pWhite = SDL_CreateGPUTexture(m_pGpu, &ci);\n"
        "\t\tif (!m_pWhite || !RgUploadOnce(m_pGpu, NULL, m_pWhite, 1, 1, &white, 4)) { RgLog(\"texture trang that bai: %s\", SDL_GetError()); return false; }\n",
        "\t\tm_pWhite = SDL_CreateGPUTexture(m_pGpu, &ci);\n"
        "\t\tif (!m_pWhite || !RgUploadOnce(m_pGpu, NULL, m_pWhite, 1, 1, &white, 4)) { RgLog(\"texture trang that bai: %s\", SDL_GetError()); return false; }\n"
        "#ifdef JX_ANDROID\n"
        "\t\tif (g_nJxAtlasKhoi)\n"
        "\t\t{\t// " + DAU + " SDL doi MOI sampler da khai bao phai duoc gan: khe khoi chua co khoi thi gan texture mang 1x1 nay\n"
        "\t\t\tci.type = SDL_GPU_TEXTURETYPE_2D_ARRAY;\n"
        "\t\t\tm_pWhiteMang = SDL_CreateGPUTexture(m_pGpu, &ci);\n"
        "\t\t\tif (!m_pWhiteMang || !RgUploadOnce(m_pGpu, NULL, m_pWhiteMang, 1, 1, &white, 4)) { RgLog(\"[KHOI] texture mang trang that bai: %s\", SDL_GetError()); return false; }\n"
        "\t\t}\n"
        "#endif\n",
        "tao texture trang")
    s = thay1(s, "\tm_pVS = NULL; m_pFS = NULL; m_pDummy = NULL; m_pWhite = NULL;",
        "\tm_pVS = NULL; m_pFS = NULL; m_pDummy = NULL; m_pWhite = NULL; m_pWhiteMang = NULL;\t// " + DAU,
        "khoi tao con tro")
    s = thay1(s, "\t\tif (m_pWhite) SDL_ReleaseGPUTexture(m_pGpu, m_pWhite);\n",
        "\t\tif (m_pWhite) SDL_ReleaseGPUTexture(m_pGpu, m_pWhite);\n"
        "\t\tif (m_pWhiteMang) SDL_ReleaseGPUTexture(m_pGpu, m_pWhiteMang);\t// " + DAU + "\n",
        "huy texture trang")
    # 4. ComputeState: khe 0 GIU NGUYEN cho trang atlas trong khoi
    s = thay1(s, "\t\tif (g_nJxAtlasMang && s == 1 && m_tex[1] && m_tex[1] != m_pRtTex && m_tex[1]->m_bVirtual) m_tex[1]->BoAtlas();\n",
        "\t\tif ((g_nJxAtlasMang || g_nJxAtlasKhoi) && s == 1 && m_tex[1] && m_tex[1] != m_pRtTex && m_tex[1]->m_bVirtual) m_tex[1]->BoAtlas();\t// " + DAU + " tang 1 la sampler2D thuong\n",
        "tang 1 ra khoi atlas")
    s = thay1(s, "\t\tif (m_tex[s] && m_tex[s] != m_pRtTex) t = m_tex[s]->PrepareForBind();\n"
        "\t\tif (t) bound[s] = true;\n"
        "\t\tst.pTex[s] = t ? t : m_pWhite;\n",
        "\t\tif (m_tex[s] && m_tex[s] != m_pRtTex) t = m_tex[s]->PrepareForBind();\n"
        "\t\tif (t) bound[s] = true;\n"
        "#ifdef JX_ANDROID\n"
        "\t\t// " + DAU + " DAY LA CHO AN TIEN: trang atlas nam trong khoi thi khe 0 khong can gan texture cua no nua\n"
        "\t\t// (shader lay diem anh tu mang cua khoi theo chi so o PALROW). Khe 0 giu y nguyen giua cac lenh ve ->\n"
        "\t\t// 'texture0' thoi la ly do cat lo. bound[0] van = 1 nen tang 0 van duoc coi la CO texture.\n"
        "\t\tif (s == 0 && g_nJxAtlasKhoi && t && m_tex[0] && m_tex[0]->JxKhoi() != 0xFFu) t = NULL;\n"
        "#endif\n"
        "\t\tst.pTex[s] = t ? t : m_pWhite;\n",
        "gan texture khe")
    # 5. o PALROW mang chi so khoi + lop
    s = thay1(s, "\t\tif (g_nJxAtlasMang && m_tex[0]) uPal |= ((m_tex[0]->JxLop() & 0x3Fu) << 25);\t// [MANG 11/09] bit 25..30 = lop trong texture mang\n",
        "\t\tif (g_nJxAtlasMang && m_tex[0]) uPal |= ((m_tex[0]->JxLop() & 0x3Fu) << 25);\t// [MANG 11/09] bit 25..30 = lop trong texture mang\n"
        "\t\tif (g_nJxAtlasKhoi && m_tex[0] && m_tex[0]->JxKhoi() != 0xFFu)\n"
        "\t\t\tuPal |= 0x80000000u | ((m_tex[0]->JxKhoi() & 7u) << 28) | ((m_tex[0]->JxLop() & 7u) << 25);\t// " + DAU + " bit 31 = o khoi, 28..30 khoi, 25..27 lop\n",
        "PALROW lop")
    # 6. gan 8 khoi vao khe 2..9
    s = thay1(s, "#ifdef JX_ANDROID\n"
        "\t\t\tSDL_BindGPUFragmentSamplers(pass, 0, tb, g_nJxPalBuffer ? 2 : 3);\t// [PALBUF 11/09] kieu buffer: shader chi khai 2 sampler\n"
        "#else\n",
        "#ifdef JX_ANDROID\n"
        "\t\t\tSDL_BindGPUFragmentSamplers(pass, 0, tb, g_nJxPalBuffer ? 2 : 3);\t// [PALBUF 11/09] kieu buffer: shader chi khai 2 sampler\n"
        "\t\t\tif (g_nJxAtlasKhoi && m_pAtlas)\n"
        "\t\t\t{\t// " + DAU + " 8 khoi atlas o khe 2..9. Gan bang BO LOC cua tang 0 (lenh gop duoc da phai cung sampler[0] nen khong sai)\n"
        "\t\t\t\tSDL_GPUTextureSamplerBinding tk[8];\n"
        "\t\t\t\tfor (int q = 0; q < 8; q++)\n"
        "\t\t\t\t{\n"
        "\t\t\t\t\tSDL_GPUTexture* pk = m_pAtlas->JxKhoiTex((UINT)q);\n"
        "\t\t\t\t\ttk[q].texture = pk ? pk : m_pWhiteMang; tk[q].sampler = st.pSamp[0];\n"
        "\t\t\t\t}\n"
        "\t\t\t\tSDL_BindGPUFragmentSamplers(pass, 2, tk, 8);\n"
        "\t\t\t}\n"
        "#else\n",
        "gan sampler")
    ghi(p, s, nl, cao)

# ============================================================ KRepresentShell3.cpp
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^int g_nJxCullCpu = 1;[^\n]*\n",
        "int g_nJxAtlasKhoi = 0, g_nJxAtlasKhoiLop = 8;\t// " + DAU + "\n"
        "unsigned g_uJxKhoiSo = 0, g_uJxKhoiMB = 0, g_uJxKhoiHet = 0;\t// " + DAU + "\n",
        "dinh nghia cull cpu")
    s = thay1(s, "\tg_nJxCullCpu        = Rep3Ini(\"Rep3CullCpu\", 1) ? 1 : 0;",
        "\tg_nJxAtlasKhoi      = Rep3Ini(\"Rep3AtlasKhoi\", 0) ? 1 : 0;\t// " + DAU + " 1 = atlas theo khoi co dinh, moi khoi gan chet mot khe sampler (port buoc (f) cua [MANG 09/09]); MAC DINH TAT\n"
        "\tg_nJxAtlasKhoiLop   = Rep3Ini(\"Rep3AtlasKhoiLop\", 8);\t// " + DAU + " so trang atlas moi khoi (1..8; 8 trang 2048 x 2 byte = 64 MB/khoi)\n"
        "\tg_nJxCullCpu        = Rep3Ini(\"Rep3CullCpu\", 1) ? 1 : 0;",
        "doc ini cull cpu")
    s = thay1(s, "| cull cpu=%d: giu %u bo %u | pipeline vo: fvf %u, topo %u, blend %u, cull %u, fill %u, rt %u, stride %u, khac %u\",\n",
        "| cull cpu=%d: giu %u bo %u | pipeline vo: fvf %u, topo %u, blend %u, cull %u, fill %u, rt %u, stride %u, khac %u | atlas khoi=%d: %u khoi (%u lop/khoi, %u MB), het khoi %u\",\n",
        "[VE-GOP] format")
    s = thay1(s, "g_nJxCullCpu, g_uJxCullGiu, g_uJxCullBo, g_uJxPipeVo[0], g_uJxPipeVo[1], g_uJxPipeVo[2], g_uJxPipeVo[3], g_uJxPipeVo[4], g_uJxPipeVo[5], g_uJxPipeVo[6], g_uJxPipeVo[7]);",
        "g_nJxCullCpu, g_uJxCullGiu, g_uJxCullBo, g_uJxPipeVo[0], g_uJxPipeVo[1], g_uJxPipeVo[2], g_uJxPipeVo[3], g_uJxPipeVo[4], g_uJxPipeVo[5], g_uJxPipeVo[6], g_uJxPipeVo[7],\n"
        "\t\tg_nJxAtlasKhoi, g_uJxKhoiSo, (unsigned)g_nJxAtlasKhoiLop, g_uJxKhoiMB, g_uJxKhoiHet);",
        "[VE-GOP] args")
    ghi(p, s, nl, cao)

# ============================================================ config.ini
p = CFG
s, nl, cao = doc(p)
if "Rep3AtlasKhoi" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "Rep3CullCpu=1\n",
        "Rep3CullCpu=1\n"
        "; " + DAU + " Rep3AtlasKhoi=1: atlas theo KHOI co dinh - port buoc (f) cua loat [MANG 09/09] ben duong D3D11 cua ban PC\n"
        ";   (commit 5311778b). Moi khoi = texture mang 2D 8 trang, GAN CHET vao mot khe sampler (khe 2..9) va khong bao gio\n"
        ";   doi trong ca khung -> hai quad o hai trang atlas khac nhau van gop chung mot lenh ve. Truoc do 'texture0' chiem\n"
        ";   99,9 %% ly do khong gop (1 653-1 941 lan doi texture moi khung). Can Rep3PalBuffer=1 va Rep3PsBuffer=1.\n"
        ";   MAC DINH 0 trong ma nguon; bat o day de lui duoc bang config (khong can APK) nhu bai hoc cua Rep3AtlasMang.\n"
        ";   Doc [VE-GOP] \"atlas khoi=1: N khoi ... het khoi K\" va phan \"texture0\" cua 'quad khong gop'.\n"
        "Rep3AtlasKhoi=0\n"
        "Rep3AtlasKhoiLop=8\n",
        "config Rep3CullCpu")
    ghi(p, s, nl, cao)

print("xong")
