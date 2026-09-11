# -*- coding: utf-8 -*-
r"""[MANG 11/09] Dot C buoc 2 (C1) - ATLAS THANH TEXTURE MANG 2D, chi JX_ANDROID.
Log Fold 7 15:53 (ban C buoc 1 109111545): trang thai tang texture da het la ly do khong gop (doi ps 0/khung, bang ps 5 muc),
NHUNG doi texture van 952/khung (max 1523) va texture0 chiem gan het "quad khong gop" -> moi trang atlas la MOT SDL_GPUTexture rieng,
hai sprite lien tiep khac trang thi khong gop duoc (thu tu ve theo Y nen khong sap lai duoc).

Cach sua: nhieu trang atlas nam trong MOT texture mang 2D (cum), moi trang la mot LOP. Hai quad o hai trang cung cum -> cung
texture0 -> GOP duoc. Chi so lop di theo DINH trong o PALROW (bit 25..30, 6 bit) nhu chi so ps cua buoc 1.
  - Cum cap theo NGAN SACH BYTE va tang dan (2, 4, 8... lop, toi da Rep3AtlasLop) de khong cap 64 MB ngay khi vao map.
  - Trang rong tra LOP ve cum (khong huy texture cum - day la texture dung chung; huy nham = mat het sprite).
  - Texture rieng (map, UI lon, render target, texture trang 1x1) cung tao kieu mang 1 lop de dung chung shader; lop cua chung = 0.
  - Texture o TANG 1 (g_t1, hiem) bi ep ra khoi atlas de lop luon = 0.
  - Shader bien the thu tu g_Rep3GpuFSPalPsMang (-DJX_PAL_BUFFER -DJX_PS_BUFFER -DJX_TEX_ARRAY): sampler2DArray + macro
    JX_TEX0/JX_TEX1/JX_FETCH0/JX_DIM0 -> nhanh khong-JX_TEX_ARRAY sinh DUNG chuoi token cu, ba mang shader cu (PC, PalBuf, PalPs)
    GIU NGUYEN TUNG BYTE (bai hoc 15:45).
[Client] Rep3AtlasMang=1 bat (0 = tung trang mot texture nhu ban 109111545), Rep3AtlasLop = so lop toi da moi cum (8),
Rep3AtlasCumMB = ngan sach byte moi cum (64 MB) -> so lop = min(Rep3AtlasLop, CumMB / (trang^2 x bpp)). Can Rep3PsBuffer=1.
Log: [VE-GOP] them "cum N (L lop)"; [GPU] ghi moi lan tao cum.

Doc/ghi latin-1, giu CRLF/LF, moc khop dung 1 cho, byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_c1.py   roi  python ReverseTools\mobile_x64\dich_shader_gpu.py <goc worktree>
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
CFG = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
DICH = os.path.join(GOC, "ReverseTools", "mobile_x64", "dich_shader_gpu.py")
DAU = "[MANG 11/09]"


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
    s = sau_dong(s, r"^extern unsigned g_uJxPsBangMax, g_uJxPsTran;[^\n]*\n",
        "extern int g_nJxAtlasMang, g_nJxAtlasLop, g_nJxAtlasCumMB;\t// " + DAU + " [Client] Rep3AtlasMang: nhieu trang atlas trong MOT texture mang 2D (lop di theo dinh) -> gop duoc quad khac trang\n"
        "extern unsigned g_uJxAtlasCum;\t// " + DAU + " so cum dang song\n",
        "extern ps bang")
    ghi(p, s, nl, cao)

# ============================================================ KRepresentShell3.cpp
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^unsigned g_uJxPsBangMax = 0, g_uJxPsTran = 0;[^\n]*\n",
        "int g_nJxAtlasMang = 1, g_nJxAtlasLop = 8, g_nJxAtlasCumMB = 64;\t// " + DAU + "\n"
        "unsigned g_uJxAtlasCum = 0;\t// " + DAU + "\n",
        "dinh nghia ps bang")
    s = thay1(s, "\tg_nJxBindRing       = Rep3Ini(\"Rep3BindRing\", 1) ? 1 : 0;",
        "\tg_nJxAtlasMang      = Rep3Ini(\"Rep3AtlasMang\", 1) ? 1 : 0;\t// " + DAU + " 1 = nhieu trang atlas trong mot texture mang 2D (hai quad khac trang van gop duoc); 0 = tung trang mot texture nhu ban 109111545\n"
        "\tif (!g_nJxPsBuffer) g_nJxAtlasMang = 0;\t// lop di chung o PALROW voi chi so ps: can shader bien the pal+ps\n"
        "\t{ int n = Rep3Ini(\"Rep3AtlasLop\", 8); if (n < 2) n = 2; if (n > 32) n = 32; g_nJxAtlasLop = n; }\t// so lop toi da moi cum\n"
        "\t{ int n = Rep3Ini(\"Rep3AtlasCumMB\", 64); if (n < 8) n = 8; if (n > 256) n = 256; g_nJxAtlasCumMB = n; }\t// ngan sach byte moi cum\n"
        "\tg_nJxBindRing       = Rep3Ini(\"Rep3BindRing\", 1) ? 1 : 0;",
        "doc ini bind ring")
    s = thay1(s, ", g_nJxPsBuffer, g_nJxBindRing);\t// [PALBUF 11/09] [BKG 11/09] [GOP 11/09]\n",
        ", g_nJxPsBuffer, g_nJxBindRing);\t// [PALBUF 11/09] [BKG 11/09] [GOP 11/09]\n"
        "\tRep3Log(\"[VE] atlas mang 2D=%d (Rep3AtlasMang; toi da %d lop/cum, ngan sach %d MB/cum)\", g_nJxAtlasMang, g_nJxAtlasLop, g_nJxAtlasCumMB);\t// " + DAU + "\n",
        "log [VE] ps")
    s = thay1(s, "| ps bang %u muc (tran %u)\",\n", "| ps bang %u muc (tran %u) | atlas mang=%d: %u cum\",\n", "[VE-GOP] format")
    s = thay1(s, "g_uJxPsBangMax, g_uJxPsTran);\t// [VE 11/09 e] [GOP 11/09]\n",
        "g_uJxPsBangMax, g_uJxPsTran, g_nJxAtlasMang, g_uJxAtlasCum);\t// [VE 11/09 e] [GOP 11/09] " + DAU + "\n",
        "[VE-GOP] args")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUi.h
p = os.path.join(R3, "D3D9onGPUi.h")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    # RgTexUpload: them lop o CUOI (cac khoi tao cu thieu phan tu -> 0)
    s = thay1(s, "struct RgTexUpload { SDL_GPUTexture* pTex; UINT x, y, w, h; UINT stageOff; UINT bytes; };\t// tai tu staging texture cua khung\n",
        "struct RgTexUpload { SDL_GPUTexture* pTex; UINT x, y, w, h; UINT stageOff; UINT bytes; UINT layer; };\t// tai tu staging texture cua khung (" + DAU + " layer = lop trong texture mang; 0 = texture thuong)\n",
        "RgTexUpload")
    # CAtlasPageGpu: them m_nLop
    s = thay1(s, "\tCAtlasPageGpu() : m_pTex(NULL), m_fmt(SDL_GPU_TEXTUREFORMAT_INVALID), m_bpp(0), m_binH(0), m_rows(0), m_used(0) {}\n",
        "\tCAtlasPageGpu() : m_pTex(NULL), m_fmt(SDL_GPU_TEXTUREFORMAT_INVALID), m_bpp(0), m_binH(0), m_rows(0), m_used(0), m_nLop(0) {}\n",
        "ctor trang")
    s = thay1(s, "\tSDL_GPUTexture* m_pTex; SDL_GPUTextureFormat m_fmt; UINT m_bpp; UINT m_binH, m_rows, m_used;\n",
        "\tSDL_GPUTexture* m_pTex; SDL_GPUTextureFormat m_fmt; UINT m_bpp; UINT m_binH, m_rows, m_used;\n"
        "\tUINT m_nLop;\t// " + DAU + " lop cua trang trong texture mang cua cum (m_pTex la texture DUNG CHUNG cua cum - khong duoc huy rieng)\n",
        "member trang")
    # CAtlasMgrGpu: cum
    s = thay1(s, "\tCDevGpu* m_pDev; std::vector<CAtlasPageGpu*> m_pages; UINT m_pageSize;\n",
        "#ifdef JX_ANDROID\n"
        "\t// " + DAU + " cum = mot texture mang 2D chua nhieu trang (moi trang mot lop)\n"
        "\tstruct JxCum { SDL_GPUTexture* pTex; SDL_GPUTextureFormat fmt; UINT bpp; UINT nLop, nLopTiep; std::vector<UINT> lopTrong; };\n"
        "\tstd::vector<JxCum> m_jxCum;\n"
        "\tbool JxCapLop(SDL_GPUTextureFormat fmt, UINT bpp, SDL_GPUTexture** ppTex, UINT* pLop);\t// cap mot lop (tao cum moi neu het)\n"
        "\tvoid JxTraLop(CAtlasPageGpu* pPage);\t// tra lop ve cum (KHONG huy texture cum)\n"
        "#endif\n"
        "\tCDevGpu* m_pDev; std::vector<CAtlasPageGpu*> m_pages; UINT m_pageSize;\n",
        "member quan ly")
    # CTexGpu: lop
    s = thay1(s, "\tSDL_GPUTexture* GpuTex() const { return m_bVirtual ? (m_pPage ? m_pPage->m_pTex : NULL) : m_pGpu; }\n",
        "\tSDL_GPUTexture* GpuTex() const { return m_bVirtual ? (m_pPage ? m_pPage->m_pTex : NULL) : m_pGpu; }\n"
        "\tUINT  JxLop() const { return (m_bVirtual && m_pPage) ? m_pPage->m_nLop : 0; }\t// " + DAU + " lop trong texture mang (texture rieng = 0)\n",
        "GpuTex")
    # QueueZeroUpload + ReadbackRegion: them lop
    s = thay1(s, "\tvoid    QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp);\t// tai vung 0 (trang moi / o chua co du lieu)\n",
        "\tvoid    QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, UINT layer = 0);\t// tai vung 0 (trang moi / o chua co du lieu); " + DAU + " layer\n",
        "decl QueueZeroUpload")
    s = thay1(s, "\tbool    ReadbackRegion(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, BYTE* pDst, UINT dstPitch);\t// [GPU 11/09 BOCPU]\n",
        "\tbool    ReadbackRegion(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, BYTE* pDst, UINT dstPitch, UINT layer = 0);\t// [GPU 11/09 BOCPU] " + DAU + " layer\n",
        "decl ReadbackRegion")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPURes.cpp
p = os.path.join(R3, "D3D9onGPURes.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    # NewVersion: texture rieng cung kieu mang 1 lop (dung chung shader sampler2DArray)
    s = thay1(s, "\tci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = gf;\n",
        "#ifdef JX_ANDROID\n"
        "\tci.type = g_nJxAtlasMang ? SDL_GPU_TEXTURETYPE_2D_ARRAY : SDL_GPU_TEXTURETYPE_2D; ci.format = gf;\t// " + DAU + " shader dung sampler2DArray -> MOI texture phai la mang (rieng = 1 lop)\n"
        "#else\n"
        "\tci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = gf;\n"
        "#endif\n",
        "NewVersion type")
    # QueueUpload: lop
    s = thay1(s, "\tRgTexUpload u = { pDst, (UINT)rc.left + (m_bVirtual ? m_ax : 0), (UINT)rc.top + (m_bVirtual ? m_ay : 0), rw, rh, off, bytes };\t// [GPU 11/09 ATLAS]\n",
        "\tRgTexUpload u = { pDst, (UINT)rc.left + (m_bVirtual ? m_ax : 0), (UINT)rc.top + (m_bVirtual ? m_ay : 0), rw, rh, off, bytes, JxLop() };\t// [GPU 11/09 ATLAS] " + DAU + " lop\n",
        "QueueUpload")
    # PrepareForBind: vung 0 cua o moi trong trang -> co lop
    s = thay1(s, "\t\t\t\tif (m_pCpu) QueueUpload(NULL); else m_pDev->QueueZeroUpload(m_pPage->m_pTex, m_ax, m_ay, m_w, m_h, m_pPage->m_bpp);\n",
        "\t\t\t\tif (m_pCpu) QueueUpload(NULL); else m_pDev->QueueZeroUpload(m_pPage->m_pTex, m_ax, m_ay, m_w, m_h, m_pPage->m_bpp, m_pPage->m_nLop);\t// " + DAU + " lop\n",
        "PrepareForBind zero")
    # ThuLaiCpu: doc lai DUNG LOP cua trang (khong thi lay nham lop 0 cua cum -> anh sai khi cache bo ban CPU roi doc lai)
    s = thay1(s, "\tif (!m_pDev->ReadbackRegion(pSrc, ox, oy, m_w, m_h, gbpp, &tmp[0], m_w * gbpp)) { m_bCpuBo = false; return false; }\n",
        "\tif (!m_pDev->ReadbackRegion(pSrc, ox, oy, m_w, m_h, gbpp, &tmp[0], m_w * gbpp, JxLop())) { m_bCpuBo = false; return false; }\t// " + DAU + " dung lop cua trang\n",
        "ThuLaiCpu readback")
    # NewPage: cap lop trong cum khi bat mang
    s = thay1(s, "\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci));\n"
                 "\tci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = fmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;\n"
                 "\tci.width = m_pageSize; ci.height = m_pageSize; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;\n"
                 "\tSDL_GPUTexture* pTex = SDL_CreateGPUTexture(m_pDev->m_pGpu, &ci);\n"
                 "\tif (!pTex) { RgLog(\"atlas: CreateGPUTexture trang %ux%u fmt %d that bai: %s\", m_pageSize, m_pageSize, (int)fmt, SDL_GetError()); return NULL; }\n"
                 "\tm_pDev->QueueZeroUpload(pTex, 0, 0, m_pageSize, m_pageSize, bpp);\t// trang moi = 0 (khong de rac; vien o khi loc tuyen tinh)\n",
        "\tSDL_GPUTexture* pTex = NULL; UINT uLop = 0;\n"
        "#ifdef JX_ANDROID\n"
        "\tif (g_nJxAtlasMang)\n"
        "\t{\t// " + DAU + " trang = mot LOP trong texture mang cua cum (hai trang cung cum -> cung texture0 -> gop duoc lenh ve)\n"
        "\t\tif (!JxCapLop(fmt, bpp, &pTex, &uLop)) return NULL;\n"
        "\t}\n"
        "\telse\n"
        "#endif\n"
        "\t{\n"
        "\t\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci));\n"
        "\t\tci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = fmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;\n"
        "\t\tci.width = m_pageSize; ci.height = m_pageSize; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;\n"
        "\t\tpTex = SDL_CreateGPUTexture(m_pDev->m_pGpu, &ci);\n"
        "\t\tif (!pTex) { RgLog(\"atlas: CreateGPUTexture trang %ux%u fmt %d that bai: %s\", m_pageSize, m_pageSize, (int)fmt, SDL_GetError()); return NULL; }\n"
        "\t}\n"
        "\tm_pDev->QueueZeroUpload(pTex, 0, 0, m_pageSize, m_pageSize, bpp, uLop);\t// trang moi = 0 (khong de rac; vien o khi loc tuyen tinh)\n",
        "NewPage tao texture")
    s = thay1(s, "\tCAtlasPageGpu* p = new CAtlasPageGpu();\n", "\tCAtlasPageGpu* p = new CAtlasPageGpu();\n\tp->m_nLop = uLop;\t// " + DAU + "\n", "NewPage gan lop")
    # ReleaseAll: khong huy texture cua trang khi dung cum (huy theo cum)
    s = thay1(s, "\t\tCAtlasPageGpu* p = m_pages[i];\n\t\tif (p->m_pTex && m_pDev->m_pGpu) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, p->m_pTex);\n\t\tdelete p;\n",
        "\t\tCAtlasPageGpu* p = m_pages[i];\n"
        "#ifdef JX_ANDROID\n"
        "\t\tif (p->m_pTex && m_pDev->m_pGpu && !g_nJxAtlasMang) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, p->m_pTex);\t// " + DAU + " texture cum huy o duoi, khong huy theo tung trang\n"
        "#else\n"
        "\t\tif (p->m_pTex && m_pDev->m_pGpu) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, p->m_pTex);\n"
        "#endif\n"
        "\t\tdelete p;\n",
        "ReleaseAll trang")
    s = thay1(s, "\tm_pages.clear();\n\tg_uRep3AtlasPages = 0; g_uRep3AtlasBytes = 0;\n",
        "\tm_pages.clear();\n"
        "#ifdef JX_ANDROID\n"
        "\tfor (size_t i = 0; i < m_jxCum.size(); i++)\t// " + DAU + " huy texture cua tung cum\n"
        "\t\tif (m_jxCum[i].pTex && m_pDev->m_pGpu) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, m_jxCum[i].pTex);\n"
        "\tm_jxCum.clear(); g_uJxAtlasCum = 0;\n"
        "#endif\n"
        "\tg_uRep3AtlasPages = 0; g_uRep3AtlasBytes = 0;\n",
        "ReleaseAll cuoi")
    # Free (duong bin cu) va JxFreeKe: tra lop thay vi huy texture
    s = s.replace("\t\t\tif (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);\n",
                  "#ifdef JX_ANDROID\n"
                  "\t\t\tif (g_nJxAtlasMang) JxTraLop(pPage);\t// " + DAU + " texture la cua CUM (dung chung): chi tra lop, khong huy\n"
                  "\t\t\telse if (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);\n"
                  "#else\n"
                  "\t\t\tif (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);\n"
                  "#endif\n")
    s = s.replace("\t\t\tif (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);\n\t\t\tif (g_uRep3AtlasPages) g_uRep3AtlasPages--;\n",
                  "\t\t\tif (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);\n\t\t\tif (g_uRep3AtlasPages) g_uRep3AtlasPages--;\n")
    if s.count(DAU + " texture la cua CUM") != 2:
        raise SystemExit("tra lop: khop %d cho (can 2 - Free va JxFreeKe)" % s.count(DAU + " texture la cua CUM"))
    # hai ham cum, dat truoc CSurfGpu
    s = thay1(s, "CSurfGpu::CSurfGpu(CDevGpu* pDev, RgSurfKind kind, CTexGpu* pTex, UINT w, UINT h, D3DFORMAT fmt)\n",
        "#ifdef JX_ANDROID\n"
        "// " + DAU + " Cap mot LOP cho trang atlas: tim cum cung dinh dang con lop (lop da tra truoc, roi lop chua dung), het thi tao cum moi.\n"
        "// So lop moi cum tang dan (2, 4, 8...) va khong vuot ngan sach byte Rep3AtlasCumMB -> khong cap 64 MB ngay khi vao map.\n"
        "bool CAtlasMgrGpu::JxCapLop(SDL_GPUTextureFormat fmt, UINT bpp, SDL_GPUTexture** ppTex, UINT* pLop)\n"
        "{\n"
        "\tfor (size_t i = 0; i < m_jxCum.size(); i++)\n"
        "\t{\n"
        "\t\tJxCum& c = m_jxCum[i];\n"
        "\t\tif (c.fmt != fmt) continue;\n"
        "\t\tif (!c.lopTrong.empty()) { *ppTex = c.pTex; *pLop = c.lopTrong.back(); c.lopTrong.pop_back(); return true; }\n"
        "\t\tif (c.nLopTiep < c.nLop) { *ppTex = c.pTex; *pLop = c.nLopTiep++; return true; }\n"
        "\t}\n"
        "\tUINT nLop = 2;\n"
        "\tfor (size_t i = 0; i < m_jxCum.size(); i++) if (m_jxCum[i].fmt == fmt) nLop *= 2;\t// cum sau nhieu lop hon cum truoc\n"
        "\tif ((int)nLop > g_nJxAtlasLop) nLop = (UINT)g_nJxAtlasLop;\n"
        "\t{\n"
        "\t\tconst unsigned __int64 uMotLop = (unsigned __int64)m_pageSize * m_pageSize * bpp;\n"
        "\t\tconst unsigned __int64 uNganSach = (unsigned __int64)g_nJxAtlasCumMB << 20;\n"
        "\t\tUINT nToiDa = (UINT)(uMotLop ? (uNganSach / uMotLop) : 1); if (nToiDa < 1) nToiDa = 1;\n"
        "\t\tif (nLop > nToiDa) nLop = nToiDa;\n"
        "\t}\n"
        "\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci));\n"
        "\tci.type = SDL_GPU_TEXTURETYPE_2D_ARRAY; ci.format = fmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;\n"
        "\tci.width = m_pageSize; ci.height = m_pageSize; ci.layer_count_or_depth = nLop; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;\n"
        "\tSDL_GPUTexture* pTex = SDL_CreateGPUTexture(m_pDev->m_pGpu, &ci);\n"
        "\tif (!pTex)\n"
        "\t{\n"
        "\t\tRgLog(\"atlas mang: CreateGPUTexture %ux%u x %u lop fmt %d that bai: %s\", m_pageSize, m_pageSize, nLop, (int)fmt, SDL_GetError());\n"
        "\t\treturn false;\n"
        "\t}\n"
        "\tJxCum c; c.pTex = pTex; c.fmt = fmt; c.bpp = bpp; c.nLop = nLop; c.nLopTiep = 1;\n"
        "\tm_jxCum.push_back(c);\n"
        "\tg_uJxAtlasCum = (unsigned)m_jxCum.size();\n"
        "\tRgLog(\"[MANG] cum atlas moi: %ux%u x %u lop fmt %d (%u MB), tong %u cum\", m_pageSize, m_pageSize, nLop, (int)fmt,\n"
        "\t\t(unsigned)(((unsigned __int64)m_pageSize * m_pageSize * bpp * nLop) >> 20), (unsigned)m_jxCum.size());\n"
        "\t*ppTex = pTex; *pLop = 0;\n"
        "\treturn true;\n"
        "}\n"
        "\n"
        "void CAtlasMgrGpu::JxTraLop(CAtlasPageGpu* pPage)\n"
        "{\n"
        "\tif (!pPage || !pPage->m_pTex) return;\n"
        "\tfor (size_t i = 0; i < m_jxCum.size(); i++)\n"
        "\t\tif (m_jxCum[i].pTex == pPage->m_pTex) { m_jxCum[i].lopTrong.push_back(pPage->m_nLop); return; }\n"
        "}\n"
        "#endif\n"
        "\n"
        "CSurfGpu::CSurfGpu(CDevGpu* pDev, RgSurfKind kind, CTexGpu* pTex, UINT w, UINT h, D3DFORMAT fmt)\n",
        "truoc CSurfGpu")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUDev.cpp
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    # texture trang 1x1 cung kieu mang
    s = thay1(s, "\t\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci)); ci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;\n"
                 "\t\tci.width = 1; ci.height = 1; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;\n"
                 "\t\tm_pWhite = SDL_CreateGPUTexture(m_pGpu, &ci);\n",
        "\t\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci)); ci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;\n"
        "#ifdef JX_ANDROID\n"
        "\t\tif (g_nJxAtlasMang) ci.type = SDL_GPU_TEXTURETYPE_2D_ARRAY;\t// " + DAU + " shader dung sampler2DArray\n"
        "#endif\n"
        "\t\tci.width = 1; ci.height = 1; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;\n"
        "\t\tm_pWhite = SDL_CreateGPUTexture(m_pGpu, &ci);\n",
        "m_pWhite")
    # CreateShaders: bien the thu tu
    s = thay1(s, "\t\t\tsi.code = g_Rep3GpuFSPalPs; si.code_size = sizeof(g_Rep3GpuFSPalPs); si.num_storage_buffers = 2; si.num_uniform_buffers = 0;\n",
        "\t\t\tsi.code = g_Rep3GpuFSPalPs; si.code_size = sizeof(g_Rep3GpuFSPalPs); si.num_storage_buffers = 2; si.num_uniform_buffers = 0;\n"
        "\t\t\tif (g_nJxAtlasMang) { si.code = g_Rep3GpuFSPalPsMang; si.code_size = sizeof(g_Rep3GpuFSPalPsMang); }\t// " + DAU + " sampler2DArray, lop lay tu dinh\n",
        "CreateShaders")
    # QueueZeroUpload: nhan lop
    s = thay1(s, "void CDevGpu::QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp)\n{\n\tif (!pTex || !w || !h || !bpp) return;\n",
        "void CDevGpu::QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, UINT layer)\n{\n\tif (!pTex || !w || !h || !bpp) return;\n",
        "QueueZeroUpload dinh nghia")
    s = thay1(s, "\t\tRgTexUpload u = { pTex, x, y, w, h, 0, w * h * bpp };\n\t\tm_jxZeroUploads.push_back(u);\n",
        "\t\tRgTexUpload u = { pTex, x, y, w, h, 0, w * h * bpp, layer };\t// " + DAU + "\n\t\tm_jxZeroUploads.push_back(u);\n",
        "QueueZeroUpload android")
    s = thay1(s, "\tRgTexUpload u = { pTex, x, y, w, h, off, bytes };\n\tm_texUploads.push_back(u);\n}\n",
        "\tRgTexUpload u = { pTex, x, y, w, h, off, bytes, layer };\n\tm_texUploads.push_back(u);\n}\n",
        "QueueZeroUpload chung")
    # ReadbackRegion: nhan lop
    s = thay1(s, "bool CDevGpu::ReadbackRegion(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, BYTE* pDst, UINT dstPitch)\n",
        "bool CDevGpu::ReadbackRegion(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, BYTE* pDst, UINT dstPitch, UINT layer)\n",
        "ReadbackRegion dinh nghia")
    s = thay1(s, "\tSDL_GPUTextureRegion src; memset(&src, 0, sizeof(src)); src.texture = pTex; src.x = x; src.y = y; src.w = w; src.h = h; src.d = 1;\n\tSDL_GPUTextureTransferInfo dst; memset(&dst, 0, sizeof(dst)); dst.transfer_buffer = pX; dst.pixels_per_row = w; dst.rows_per_layer = h;\n\tSDL_DownloadFromGPUTexture(cp, &src, &dst);\n\tSDL_EndGPUCopyPass(cp);\n\tSDL_GPUFence* f = SDL_SubmitGPUCommandBufferAndAcquireFence(cb);\n\tif (f) { SDL_WaitForGPUFences(m_pGpu, true, &f, 1); SDL_ReleaseGPUFence(m_pGpu, f); }\n\tbool ok = false;\n\tconst BYTE* p = (const BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, pX, false);\n\tif (p) { for (UINT r = 0; r < h; r++)",
        "\tSDL_GPUTextureRegion src; memset(&src, 0, sizeof(src)); src.texture = pTex; src.layer = layer; src.x = x; src.y = y; src.w = w; src.h = h; src.d = 1;\t// " + DAU + " lop\n\tSDL_GPUTextureTransferInfo dst; memset(&dst, 0, sizeof(dst)); dst.transfer_buffer = pX; dst.pixels_per_row = w; dst.rows_per_layer = h;\n\tSDL_DownloadFromGPUTexture(cp, &src, &dst);\n\tSDL_EndGPUCopyPass(cp);\n\tSDL_GPUFence* f = SDL_SubmitGPUCommandBufferAndAcquireFence(cb);\n\tif (f) { SDL_WaitForGPUFences(m_pGpu, true, &f, 1); SDL_ReleaseGPUFence(m_pGpu, f); }\n\tbool ok = false;\n\tconst BYTE* p = (const BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, pX, false);\n\tif (p) { for (UINT r = 0; r < h; r++)",
        "ReadbackRegion vung")
    # SubmitFrame: dat lop khi tai (vung 0 va texture)
    s = thay1(s, "\t\t\t\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.x = u.x; dst.y = u.y + y0; dst.w = u.w; dst.h = hh; dst.d = 1;\n",
        "\t\t\t\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.layer = u.layer; dst.x = u.x; dst.y = u.y + y0; dst.w = u.w; dst.h = hh; dst.d = 1;\t// " + DAU + " lop\n",
        "SubmitFrame vung 0")
    s = thay1(s, "\t\t\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.x = u.x; dst.y = u.y; dst.w = u.w; dst.h = u.h; dst.d = 1;\n",
        "\t\t\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.layer = u.layer; dst.x = u.x; dst.y = u.y; dst.w = u.w; dst.h = u.h; dst.d = 1;\t// " + DAU + " lop\n",
        "SubmitFrame texture")
    # ComputeState: tang 1 khong nam trong atlas (lop cua t1 luon 0)
    s = thay1(s, "\t\tSDL_GPUTexture* t = NULL;\n\t\tif (m_tex[s] && m_tex[s] != m_pRtTex) t = m_tex[s]->PrepareForBind();\n",
        "\t\tSDL_GPUTexture* t = NULL;\n"
        "#ifdef JX_ANDROID\n"
        "\t\t// " + DAU + " o PALROW chi cho MOT chi so lop (dung cho tang 0): texture cua tang 1 phai ra khoi atlas de lop cua no luon = 0\n"
        "\t\tif (g_nJxAtlasMang && s == 1 && m_tex[1] && m_tex[1] != m_pRtTex && m_tex[1]->m_bVirtual) m_tex[1]->BoAtlas();\n"
        "#endif\n"
        "\t\tif (m_tex[s] && m_tex[s] != m_pRtTex) t = m_tex[s]->PrepareForBind();\n",
        "ComputeState tang 1")
    # DrawInternal: lop vao o PALROW
    s = thay1(s, "\t\tuPal = uRow | ((JxPsIdx(st.ps) & 0xFFFu) << 13);\n",
        "\t\tuPal = uRow | ((JxPsIdx(st.ps) & 0xFFFu) << 13);\n"
        "\t\tif (g_nJxAtlasMang && m_tex[0]) uPal |= ((m_tex[0]->JxLop() & 0x3Fu) << 25);\t// " + DAU + " bit 25..30 = lop trong texture mang\n",
        "DrawInternal lop")
    ghi(p, s, nl, cao)

# ============================================================ Rep3ShadersGPU.frag
p = os.path.join(R3, "Rep3ShadersGPU.frag")
s, nl, cao = doc(p)
if "JX_TEX_ARRAY" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "layout(set = 2, binding = 0) uniform sampler2D g_t0;\nlayout(set = 2, binding = 1) uniform sampler2D g_t1;\n",
        "#ifdef JX_TEX_ARRAY\n"
        "// " + DAU + " (chi Android) nhieu trang atlas trong MOT texture mang 2D; lop lay tu o PALROW bit 25..30 -> hai quad o hai trang cung cum GOP duoc\n"
        "layout(set = 2, binding = 0) uniform sampler2DArray g_t0;\n"
        "layout(set = 2, binding = 1) uniform sampler2DArray g_t1;\n"
        "#define JX_LOP0      float((vPal >> 25) & 0x3Fu)\n"
        "#define JX_TEX0(uv)  texture(g_t0, vec3(uv, JX_LOP0))\n"
        "#define JX_TEX1(uv)  texture(g_t1, vec3(uv, 0.0))\n"
        "#define JX_FETCH0(p) texelFetch(g_t0, ivec3(p, int((vPal >> 25) & 0x3Fu)), 0)\n"
        "#define JX_DIM0      textureSize(g_t0, 0).xy\n"
        "#else\n"
        "layout(set = 2, binding = 0) uniform sampler2D g_t0;\n"
        "layout(set = 2, binding = 1) uniform sampler2D g_t1;\n"
        "#define JX_TEX0(uv)  texture(g_t0, uv)\n"
        "#define JX_TEX1(uv)  texture(g_t1, uv)\n"
        "#define JX_FETCH0(p) texelFetch(g_t0, p, 0)\n"
        "#define JX_DIM0      textureSize(g_t0, 0)\n"
        "#endif\n",
        "khai bao sampler")
    s = thay1(s, "        vec4 tex0 = (g_st0b.z != 0) ? texture(g_t0, vUv) : vec4(1.0);\n",
        "        vec4 tex0 = (g_st0b.z != 0) ? JX_TEX0(vUv) : vec4(1.0);\n", "tex0")
    s = thay1(s, "            vec4 tex1 = (g_st1b.z != 0) ? texture(g_t1, vUv) : vec4(1.0);\n",
        "            vec4 tex1 = (g_st1b.z != 0) ? JX_TEX1(vUv) : vec4(1.0);\n", "tex1")
    s = thay1(s, "                ivec2 dim = textureSize(g_t0, 0);\n", "                ivec2 dim = JX_DIM0;\n", "dim")
    n = s.count("PalTex(texelFetch(g_t0, ")   # (hai dong #define JX_FETCH0 cung chua texelFetch(g_t0 -> dem theo PalTex(...))
    if n != 4:
        raise SystemExit("PalTex(texelFetch(g_t0 khop %d cho (can 4)" % n)
    s = s.replace("PalTex(texelFetch(g_t0, clamp(p0, ivec2(0), mx), 0), JX_PALROW)", "PalTex(JX_FETCH0(clamp(p0, ivec2(0), mx)), JX_PALROW)")
    s = s.replace("PalTex(texelFetch(g_t0, clamp(p0 + ivec2(1, 0), ivec2(0), mx), 0), JX_PALROW)", "PalTex(JX_FETCH0(clamp(p0 + ivec2(1, 0), ivec2(0), mx)), JX_PALROW)")
    s = s.replace("PalTex(texelFetch(g_t0, clamp(p0 + ivec2(0, 1), ivec2(0), mx), 0), JX_PALROW)", "PalTex(JX_FETCH0(clamp(p0 + ivec2(0, 1), ivec2(0), mx)), JX_PALROW)")
    s = s.replace("PalTex(texelFetch(g_t0, clamp(p0 + ivec2(1, 1), ivec2(0), mx), 0), JX_PALROW)", "PalTex(JX_FETCH0(clamp(p0 + ivec2(1, 1), ivec2(0), mx)), JX_PALROW)")
    if s.count("PalTex(texelFetch(g_t0, ") != 0:
        raise SystemExit("con PalTex(texelFetch(g_t0 chua doi: %d" % s.count("PalTex(texelFetch(g_t0, "))
    ghi(p, s, nl, cao)

# ============================================================ dich_shader_gpu.py
p = DICH
s, nl, cao = doc(p)
if "g_Rep3GpuFSPalPsMang" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, '("frag", "g_Rep3GpuFSPalPs", ["-DJX_PAL_BUFFER=1", "-DJX_PS_BUFFER=1"])):',
        '("frag", "g_Rep3GpuFSPalPs", ["-DJX_PAL_BUFFER=1", "-DJX_PS_BUFFER=1"]), ("frag", "g_Rep3GpuFSPalPsMang", ["-DJX_PAL_BUFFER=1", "-DJX_PS_BUFFER=1", "-DJX_TEX_ARRAY=1"])):',
        "them bien the mang")
    ghi(p, s, nl, cao)

# ============================================================ config.ini
p = CFG
s, nl, cao = doc(p)
if "Rep3AtlasMang" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "Rep3BindRing=1\n",
        "Rep3BindRing=1\n"
        "; " + DAU + " Rep3AtlasMang=1: nhieu trang atlas nam trong MOT texture mang 2D (moi trang mot lop, chi so lop di theo dinh)\n"
        ";   -> hai sprite o hai trang van gop chung mot lenh ve (truoc: doi texture 950-1200 lan moi khung luc dong). 0 = tung trang mot texture.\n"
        ";   Rep3AtlasLop = so lop toi da moi cum (cum dau 2 lop, cum sau gap doi); Rep3AtlasCumMB = ngan sach byte moi cum.\n"
        "Rep3AtlasMang=1\n"
        "Rep3AtlasLop=8\n"
        "Rep3AtlasCumMB=64\n",
        "config Rep3BindRing")
    ghi(p, s, nl, cao)

print("xong - tiep: python ReverseTools\\mobile_x64\\dich_shader_gpu.py \"%s\"" % GOC)
