# -*- coding: utf-8 -*-
r"""[VE 11/09 e] Android, ap sau ve1 + ve2. Muc 3 (gop lenh ve) theo [VE-GOP] Fold 7 09:31: 86 % quad khong gop la do doi texture0 =
trang atlas khac nhau, vi trang atlas dang chia theo (chieu cao bin, dinh dang) nen cac khung cua cung mot NPC nam rai rac 200+ trang 1024^2.
  1. D3D9onGPURes.cpp / D3D9onGPUi.h: atlas XEP KE theo dinh dang - mot trang chua nhieu KE (hang) cao khac nhau (bin 16..512), ke mo dan
     tu tren xuong; khung giai ma gan nhau (cung NPC, cung dam dong) nam cung trang -> lenh ve lien tiep cung texture -> gop duoc.
     Ke cuoi trang rong thi thu lai; trang rong xep lai tu dau; giu mot trang rong moi dinh dang (nhu cu). Cong tac [Client] Rep3AtlasKe
     (mac dinh 1), co trang [Client] Rep3AtlasTrang (1024 / 2048 / 4096, mac dinh 2048).
  2. D3D9onGPUDev.cpp: vung 0 tai theo tung dai <= 2 MB tu bo dem 0 co dinh 2 MB (trang 2048^2 A8L8 = 8 MB, khong de bo dem 0 > 2 MiB:
     SDL cap khoi 64 MB cho buffer lon).
  3. [VE-GOP] in them: atlas ke=%d trang %d: so trang.
Doc/ghi latin-1 (CRLF), moc khop dung 1 cho, so byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_ve3.py   (sau ve1, ve2)
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[VE 11/09 e]"
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")


def doc(p, can="[VE 11/09]"):
    s = io.open(p, encoding="latin-1", newline="").read()
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    if nl == "\r\n" and s.count("\r\n") != s.count("\n"):
        raise SystemExit("tep co xuong dong lan lon, khong va: " + p)
    if can and can not in s:
        raise SystemExit("chua ap buoc truoc (%s): %s" % (can, p))
    return s.replace("\r\n", "\n"), nl, sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, nl, cao):
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("so byte cao doi - khong ghi: " + p)
    io.open(p, "w", encoding="latin-1", newline="").write(s.replace("\n", nl))
    print("da va:", os.path.relpath(p, GOC))


def mot(s, rx, ten):
    m = list(re.finditer(rx, s, re.M))
    if len(m) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (len(m), ten))
    return m[0]


def chen_sau(s, rx, them, ten):
    m = mot(s, rx, ten)
    return s[:m.end()] + "\n" + them + s[m.end():]


def thay(s, rx, moi, ten):
    m = mot(s, rx, ten)
    return s[:m.start()] + moi + s[m.end():]


def thay_chuoi(s, cu, moi, ten):
    n = s.count(cu)
    if n != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (n, ten))
    return s.replace(cu, moi)


def A(*dong):
    for d in dong:
        for c in d:
            if ord(c) >= 0x80:
                raise SystemExit("dong moi co byte cao: " + d)
    return "\n".join(dong)


# ============================================================ 1. BaseInclude.h
p = os.path.join(R3, "BaseInclude.h")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^extern unsigned g_uJxNapKhungRong, g_uJxHoiTre;.*$', A(
        "extern int g_nJxAtlasKe, g_nJxAtlasTrang;\t// " + DAU + " [Client] Rep3AtlasKe: 1 = atlas xep ke theo dinh dang (mot trang nhieu hang bin); Rep3AtlasTrang: co trang 1024/2048/4096"), "extern e")
    ghi(p, s, nl, cao)

# ============================================================ 2. D3D9onGPUi.h: ke trong trang + ham
p = os.path.join(R3, "D3D9onGPUi.h")
s, nl, cao = doc(p, "[VE 11/09 d]")
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = thay(s, r'^\tstd::vector<std::vector<std::pair<UINT, UINT> > > m_free;\t// moi hang: cac doan trong \[x0, x1\)\n\};$', A(
        "\tstd::vector<std::vector<std::pair<UINT, UINT> > > m_free;\t// moi hang: cac doan trong [x0, x1)",
        "#ifdef JX_ANDROID",
        "\t// " + DAU + " xep KE (Rep3AtlasKe=1): trang chi theo dinh dang, cac ke cao khac nhau mo dan tu y = 0; m_binH = 0, m_rows = 0",
        "\tstruct JxKe { UINT y, h, used; std::vector<std::pair<UINT, UINT> > free; };",
        "\tstd::vector<JxKe> m_ke; UINT m_yTiep;",
        "#endif",
        "};"), "CAtlasPageGpu ke")
    s = thay(s, r'^\tCAtlasPageGpu\* NewPage\(UINT binH, SDL_GPUTextureFormat fmt\);$', A(
        "\tCAtlasPageGpu* NewPage(UINT binH, SDL_GPUTextureFormat fmt);",
        "#ifdef JX_ANDROID",
        "\tbool JxAllocKe(UINT w, UINT h, SDL_GPUTextureFormat fmt, CAtlasPageGpu** ppPage, UINT* pX, UINT* pY);\t// " + DAU,
        "\tvoid JxFreeKe(CAtlasPageGpu* pPage, UINT x, UINT y, UINT w);",
        "#endif"), "CAtlasMgrGpu ham")
    ghi(p, s, nl, cao)

# ============================================================ 3. D3D9onGPURes.cpp: atlas xep ke
p = os.path.join(R3, "D3D9onGPURes.cpp")
s, nl, cao = doc(p, None)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = thay(s, r'^CAtlasMgrGpu::CAtlasMgrGpu\(CDevGpu\* pDev\) \{ m_pDev = pDev; m_pageSize = 1024; \}$', A(
        "CAtlasMgrGpu::CAtlasMgrGpu(CDevGpu* pDev)",
        "{",
        "\tm_pDev = pDev; m_pageSize = 1024;",
        "#ifdef JX_ANDROID",
        "\tif (g_nJxAtlasTrang == 2048 || g_nJxAtlasTrang == 4096) m_pageSize = (UINT)g_nJxAtlasTrang;\t// " + DAU + " [Client] Rep3AtlasTrang",
        "#endif",
        "}"), "ctor")
    s = thay(s, r'^\tp->m_pTex = pTex; p->m_fmt = fmt; p->m_bpp = bpp; p->m_binH = binH; p->m_rows = m_pageSize / binH; p->m_used = 0;\n\tp->m_free\.resize\(p->m_rows\);\n\tfor \(UINT r = 0; r < p->m_rows; r\+\+\) p->m_free\[r\]\.push_back\(std::make_pair\(0u, m_pageSize\)\);\t// ca hang trong$', A(
        "#ifdef JX_ANDROID",
        "\tp->m_yTiep = 0;",
        "\tif (g_nJxAtlasKe) { p->m_pTex = pTex; p->m_fmt = fmt; p->m_bpp = bpp; p->m_binH = 0; p->m_rows = 0; p->m_used = 0; }\t// " + DAU + " trang xep ke: chua co hang",
        "\telse",
        "\t{",
        "#endif",
        "\tp->m_pTex = pTex; p->m_fmt = fmt; p->m_bpp = bpp; p->m_binH = binH; p->m_rows = m_pageSize / binH; p->m_used = 0;",
        "\tp->m_free.resize(p->m_rows);",
        "\tfor (UINT r = 0; r < p->m_rows; r++) p->m_free[r].push_back(std::make_pair(0u, m_pageSize));\t// ca hang trong",
        "#ifdef JX_ANDROID",
        "\t}",
        "#endif"), "NewPage")
    s = thay(s, r'^bool CAtlasMgrGpu::Alloc\(UINT w, UINT h, SDL_GPUTextureFormat fmt, CAtlasPageGpu\*\* ppPage, UINT\* pX, UINT\* pY\)\n\{\n\tUINT binH = RgAtlasBin\(h\);$', A(
        "bool CAtlasMgrGpu::Alloc(UINT w, UINT h, SDL_GPUTextureFormat fmt, CAtlasPageGpu** ppPage, UINT* pX, UINT* pY)",
        "{",
        "#ifdef JX_ANDROID",
        "\tif (g_nJxAtlasKe) return JxAllocKe(w, h, fmt, ppPage, pX, pY);\t// " + DAU,
        "#endif",
        "\tUINT binH = RgAtlasBin(h);"), "Alloc")
    s = thay(s, r'^void CAtlasMgrGpu::Free\(CAtlasPageGpu\* pPage, UINT x, UINT y, UINT w\)\n\{\n\tif \(!pPage \|\| pPage->m_binH == 0\) return;$', A(
        "void CAtlasMgrGpu::Free(CAtlasPageGpu* pPage, UINT x, UINT y, UINT w)",
        "{",
        "#ifdef JX_ANDROID",
        "\tif (g_nJxAtlasKe) { JxFreeKe(pPage, x, y, w); return; }\t// " + DAU,
        "#endif",
        "\tif (!pPage || pPage->m_binH == 0) return;"), "Free")
    s = chen_sau(s, r'^// ---------------------------------------------------------------- CSurfGpu$', A(
        "#ifdef JX_ANDROID",
        "// " + DAU + " atlas xep KE theo dinh dang: trang chi phan biet dinh dang; ke (hang) cao = bin cua khung, mo dan tu y = 0 den het trang.",
        "// Khung giai ma gan nhau (cung NPC / cung dam dong) roi vao cung trang -> lenh ve lien tiep cung texture0 -> gop duoc (Fold 7 09:31:",
        "// 86 % quad khong gop la do doi trang). Ke cuoi trang rong -> thu lai (m_yTiep lui); trang rong -> xep lai tu dau; giu mot trang rong/dinh dang.",
        "bool CAtlasMgrGpu::JxAllocKe(UINT w, UINT h, SDL_GPUTextureFormat fmt, CAtlasPageGpu** ppPage, UINT* pX, UINT* pY)",
        "{",
        "\tconst UINT binH = RgAtlasBin(h);",
        "\tif (h > binH || w > m_pageSize) return false;",
        "\tfor (int lan = 0; lan < 2; lan++)",
        "\t{",
        "\t\tfor (size_t i = 0; i < m_pages.size(); i++)",
        "\t\t{",
        "\t\t\tCAtlasPageGpu* p = m_pages[i];",
        "\t\t\tif (p->m_fmt != fmt) continue;",
        "\t\t\tfor (size_t k = 0; k < p->m_ke.size(); k++)",
        "\t\t\t{\t// ke da co cung chieu cao: first-fit",
        "\t\t\t\tCAtlasPageGpu::JxKe& ke = p->m_ke[k];",
        "\t\t\t\tif (ke.h != binH) continue;",
        "\t\t\t\tfor (size_t d = 0; d < ke.free.size(); d++)",
        "\t\t\t\t{",
        "\t\t\t\t\tif (ke.free[d].second - ke.free[d].first < w) continue;",
        "\t\t\t\t\tconst UINT x = ke.free[d].first;",
        "\t\t\t\t\tke.free[d].first += w;",
        "\t\t\t\t\tif (ke.free[d].first >= ke.free[d].second) ke.free.erase(ke.free.begin() + d);",
        "\t\t\t\t\tke.used++; p->m_used++;",
        "\t\t\t\t\t*ppPage = p; *pX = x; *pY = ke.y;",
        "\t\t\t\t\treturn true;",
        "\t\t\t\t}",
        "\t\t\t}",
        "\t\t\tif (p->m_yTiep + binH <= m_pageSize)",
        "\t\t\t{\t// mo ke moi",
        "\t\t\t\tCAtlasPageGpu::JxKe ke; ke.y = p->m_yTiep; ke.h = binH; ke.used = 1;",
        "\t\t\t\tif (w < m_pageSize) ke.free.push_back(std::make_pair(w, m_pageSize));",
        "\t\t\t\tp->m_yTiep += binH; p->m_ke.push_back(ke); p->m_used++;",
        "\t\t\t\t*ppPage = p; *pX = 0; *pY = ke.y;",
        "\t\t\t\treturn true;",
        "\t\t\t}",
        "\t\t}",
        "\t\tif (!NewPage(0, fmt)) return false;\t// lan 2: thu lai voi trang moi",
        "\t}",
        "\treturn false;",
        "}",
        "",
        "void CAtlasMgrGpu::JxFreeKe(CAtlasPageGpu* pPage, UINT x, UINT y, UINT w)",
        "{",
        "\tif (!pPage) return;",
        "\tfor (size_t k = 0; k < pPage->m_ke.size(); k++)",
        "\t{",
        "\t\tCAtlasPageGpu::JxKe& ke = pPage->m_ke[k];",
        "\t\tif (ke.y != y) continue;",
        "\t\tstd::vector<std::pair<UINT, UINT> >& fr = ke.free;",
        "\t\tconst UINT x0 = x, x1 = x + w;",
        "\t\tsize_t d = 0;",
        "\t\twhile (d < fr.size() && fr[d].first < x0) d++;",
        "\t\tfr.insert(fr.begin() + d, std::make_pair(x0, x1));",
        "\t\tif (d + 1 < fr.size() && fr[d].second == fr[d + 1].first) { fr[d].second = fr[d + 1].second; fr.erase(fr.begin() + d + 1); }",
        "\t\tif (d > 0 && fr[d - 1].second == fr[d].first) { fr[d - 1].second = fr[d].second; fr.erase(fr.begin() + d); }",
        "\t\tif (ke.used) ke.used--;",
        "\t\tbreak;",
        "\t}",
        "\twhile (!pPage->m_ke.empty() && pPage->m_ke.back().used == 0)",
        "\t{\t// ke cuoi trang rong: thu lai de mo ke cao khac",
        "\t\tpPage->m_yTiep = pPage->m_ke.back().y;",
        "\t\tpPage->m_ke.pop_back();",
        "\t}",
        "\tif (pPage->m_used) pPage->m_used--;",
        "\tif (pPage->m_used == 0)",
        "\t{",
        "\t\tpPage->m_ke.clear(); pPage->m_yTiep = 0;",
        "\t\tint nEmptySameFmt = 0;",
        "\t\tfor (size_t i = 0; i < m_pages.size(); i++)",
        "\t\t\tif (m_pages[i] != pPage && m_pages[i]->m_fmt == pPage->m_fmt && m_pages[i]->m_used == 0) nEmptySameFmt++;",
        "\t\tif (nEmptySameFmt >= 1)",
        "\t\t{\t// giu toi da MOT trang rong moi dinh dang; trang rong thu hai tra lai GPU (sau khung: DeferRelease)",
        "\t\t\tfor (size_t i = 0; i < m_pages.size(); i++)",
        "\t\t\t\tif (m_pages[i] == pPage) { m_pages.erase(m_pages.begin() + i); break; }",
        "\t\t\tif (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);",
        "\t\t\tif (g_uRep3AtlasPages) g_uRep3AtlasPages--;",
        "\t\t\tg_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * pPage->m_bpp;",
        "\t\t\tdelete pPage;",
        "\t\t}",
        "\t}",
        "}",
        "#endif",
        ""), "JxAllocKe/JxFreeKe")
    ghi(p, s, nl, cao)

# ============================================================ 4. D3D9onGPUDev.cpp: vung 0 theo dai <= 2 MB
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p, "[VE 11/09 d]")
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = thay_chuoi(s,
        "\t\t\tUINT needZ = 0;\n\t\t\tfor (size_t i = 0; i < m_jxZeroUploads.size(); i++) if (m_jxZeroUploads[i].bytes > needZ) needZ = m_jxZeroUploads[i].bytes;\n",
        "\t\t\tconst UINT needZ = 2u << 20;\t// " + DAU + " bo dem 0 co dinh 2 MiB (khong qua SMALL_ALLOCATION_THRESHOLD cua SDL); vung lon hon tai theo dai\n",
        "zero needZ")
    s = thay_chuoi(s,
        "\t\t\t\tfor (size_t i = 0; i < m_jxZeroUploads.size(); i++)\n\t\t\t\t{\n\t\t\t\t\tconst RgTexUpload& u = m_jxZeroUploads[i];\n\t\t\t\t\tSDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = m_pJxZeroXfer; src.offset = 0; src.pixels_per_row = u.w; src.rows_per_layer = u.h;\n\t\t\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.x = u.x; dst.y = u.y; dst.w = u.w; dst.h = u.h; dst.d = 1;\n\t\t\t\t\tSDL_UploadToGPUTexture(cp, &src, &dst, false);\n\t\t\t\t}\n",
        "\t\t\t\tfor (size_t i = 0; i < m_jxZeroUploads.size(); i++)\n\t\t\t\t{\n\t\t\t\t\tconst RgTexUpload& u = m_jxZeroUploads[i];\n\t\t\t\t\tconst UINT bppZ = u.bytes / (u.w * u.h); UINT hDai = needZ / (u.w * (bppZ ? bppZ : 4)); if (hDai == 0) hDai = 1;\t// " + DAU + " so hang moi dai\n\t\t\t\t\tfor (UINT y0 = 0; y0 < u.h; y0 += hDai)\n\t\t\t\t\t{\n\t\t\t\t\t\tconst UINT hh = (u.h - y0 < hDai) ? (u.h - y0) : hDai;\n\t\t\t\t\t\tSDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = m_pJxZeroXfer; src.offset = 0; src.pixels_per_row = u.w; src.rows_per_layer = hh;\n\t\t\t\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.x = u.x; dst.y = u.y + y0; dst.w = u.w; dst.h = hh; dst.d = 1;\n\t\t\t\t\t\tSDL_UploadToGPUTexture(cp, &src, &dst, false);\n\t\t\t\t\t}\n\t\t\t\t}\n",
        "zero theo dai")
    ghi(p, s, nl, cao)

# ============================================================ 5. KRepresentShell3.cpp: cong tac + [VE-GOP]
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p, "[VE 11/09 d]")
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^int g_nJxHoiKhongDe = 1; unsigned g_uJxNapKhungRong = 0, g_uJxHoiTre = 0;.*$', A(
        "int g_nJxAtlasKe = 1, g_nJxAtlasTrang = 2048;\t// " + DAU), "globals e")
    s = chen_sau(s, r'^\tg_nJxHoiKhongDe    = Rep3Ini\("NapHoiKhongDe", 1\);.*$', A(
        "\tg_nJxAtlasKe       = Rep3Ini(\"Rep3AtlasKe\", 1) ? 1 : 0;\t// " + DAU + " 1 = atlas xep ke theo dinh dang (khung cung NPC cung trang -> gop lenh), 0 = trang theo bin cao nhu cu",
        "\tg_nJxAtlasTrang    = Rep3Ini(\"Rep3AtlasTrang\", 2048);\t// co trang atlas 1024 / 2048 / 4096",
        "\tif (g_nJxAtlasTrang != 1024 && g_nJxAtlasTrang != 2048 && g_nJxAtlasTrang != 4096) g_nJxAtlasTrang = 2048;"), "ini e")
    s = thay_chuoi(s,
        "vs %u, ps %u, cat/vp %u\",",
        "vs %u, ps %u, cat/vp %u | atlas ke=%d trang %d: %u trang\",",
        "VE-GOP dinh dang")
    s = thay_chuoi(s,
        "g_uJxGopVo[4], g_uJxGopVo[5], g_uJxGopVo[6], g_uJxGopVo[7]);",
        "g_uJxGopVo[4], g_uJxGopVo[5], g_uJxGopVo[6], g_uJxGopVo[7], g_nJxAtlasKe, g_nJxAtlasTrang, g_uRep3AtlasPages);\t// " + DAU,
        "VE-GOP doi so")
    ghi(p, s, nl, cao)

# ============================================================ 6. config.ini
p = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
s, nl, cao = doc(p, "[VE 11/09 d]")
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^NapHoiKhongDe=1$', A(
        "; " + DAU + " Rep3AtlasKe=1: atlas xep ke theo dinh dang (khung cung NPC nam cung trang -> gop lenh ve; 0 = trang theo bin cao nhu cu); Rep3AtlasTrang: co trang 1024/2048/4096",
        "Rep3AtlasKe=1",
        "Rep3AtlasTrang=2048"), "config.ini e")
    ghi(p, s, nl, cao)

print("xong")
