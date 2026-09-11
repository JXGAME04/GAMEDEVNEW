# -*- coding: utf-8 -*-
r"""[VE 11/09 d] Android, ap sau va_nguon_android_ve1.py. Theo log Fold 7 09:31 (BANGIAO_DONHIP_MOBILE_1209.md muc 9): 203/326 khung cham la
buoc "chep len GPU" 40-200 ms du tai rat it -> nghi do SDL cap/giai phong bo nho moi khung (bang mau tao/huy transfer buffer rieng; tao
trang atlas moi = memset 2-4 MB vao staging, staging/transfer buffer phinh 32 MB roi cycle). Sua + do:
  1. D3D9onGPUDev.cpp: bang mau di chung staging/transfer buffer co dinh; to trang atlas moi (va o chua co ban CPU) tu MOT bo dem 0
     co dinh (khong memset/memcpy, staging khong phinh); dong ho con tung buoc chep (bang mau / tex map+chep / lenh tai / zero / ring),
     dem lan phinh transfer buffer -> [VE-GIAT] va [VE].
  2. TextureResMgr.cpp: hoi kich thuoc (GetImageParam tu logic) gap sprite NPC (spr\npcres) DANG nap o luong nen -> tra "chua co"
     thay vi nap dong bo de len (KNpcRes giu m_bChange, khung sau hoi lai). Cong tac [Client] NapHoiKhongDe (mac dinh 1).
  3. TextureRes.cpp: khung rong (w/h = 0) hoac rut khung hong o luong nen -> danh dau nJxNen = 2, khong giao lai; PrepareFrameData chi
     coi nJxNen == 1 la dang cho.
Doc/ghi latin-1 (CRLF), moc khop dung 1 cho, so byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_ve2.py   (sau ve1)
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[VE 11/09 d]"
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    if nl == "\r\n" and s.count("\r\n") != s.count("\n"):
        raise SystemExit("tep co xuong dong lan lon, khong va: " + p)
    if "[VE 11/09]" not in s:
        raise SystemExit("chua ap va_nguon_android_ve1.py: " + p)
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


def chen_truoc(s, rx, them, ten):
    m = mot(s, rx, ten)
    return s[:m.start()] + them + "\n" + s[m.start():]


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
    s = thay_chuoi(s,
        "struct JxVeDo { double dCho, dChep, dGhi, dNop, dTong; unsigned uTai, uTaiKB, uRingKB, uLenh, uQuad, uDinh, uPass, uDoiPipe, uDoiTex, uDoiVs, uDoiPs, uDoiCat; };",
        "struct JxVeDo { double dCho, dChep, dGhi, dNop, dTong, dChepPal, dChepTexMap, dChepTexLenh, dChepZero, dChepRing; unsigned uTai, uTaiKB, uRingKB, uLenh, uQuad, uDinh, uPass, uDoiPipe, uDoiTex, uDoiVs, uDoiPs, uDoiCat, uPal, uZero, uXferTang, uXferKB; };\t// " + DAU + " + tung buoc chep",
        "JxVeDo")
    s = chen_sau(s, r'^extern int g_nJxAnhBoVeNen;.*$', A(
        "extern int g_nJxHoiKhongDe;\t// " + DAU + " [Client] NapHoiKhongDe: 1 = hoi kich thuoc sprite NPC dang nap o luong nen -> tra 'chua co' (khong nap dong bo de len)",
        "extern unsigned g_uJxNapKhungRong, g_uJxHoiTre;\t// " + DAU + " khung rong/hong o luong nen (khong giao lai); so lan hoi NPC dang nap tra 'chua co'"), "extern d")
    ghi(p, s, nl, cao)

# ============================================================ 2. D3D9onGPUi.h: thanh vien bo dem 0 co dinh
p = os.path.join(R3, "D3D9onGPUi.h")
s, nl, cao = doc(p) if "[VE 11/09]" in io.open(p, encoding="latin-1").read() else (None, None, None)
if s is None:
    s = io.open(p, encoding="latin-1", newline="").read().replace("\r\n", "\n"); nl = "\r\n"; cao = 0
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^\tstd::vector<RgTexUpload> m_texUploads;$', A(
        "#ifdef JX_ANDROID",
        "\tSDL_GPUTransferBuffer* m_pJxZeroXfer; UINT m_jxZeroSize, m_jxZeroDaXoa; std::vector<RgTexUpload> m_jxZeroUploads;\t// " + DAU + " bo dem 0 co dinh cho trang atlas moi / o chua co ban CPU (stageOff = 0)",
        "#endif"), "CDevGpu thanh vien")
    ghi(p, s, nl, cao)

# ============================================================ 3. D3D9onGPUDev.cpp
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    # 3a. ctor
    s = chen_sau(s, r'^\tm_pRingGpu = NULL; m_ringGpuSize = 0; m_pRingXfer = NULL; m_ringXferSize = 0; m_pTexXfer = NULL; m_texXferSize = 0;$', A(
        "#ifdef JX_ANDROID",
        "\tm_pJxZeroXfer = NULL; m_jxZeroSize = 0; m_jxZeroDaXoa = 0;\t// " + DAU,
        "#endif"), "ctor")
    # 3b. release
    s = chen_sau(s, r'^\t\tif \(m_pTexXfer\) SDL_ReleaseGPUTransferBuffer\(m_pGpu, m_pTexXfer\);$', A(
        "#ifdef JX_ANDROID",
        "\t\tif (m_pJxZeroXfer) SDL_ReleaseGPUTransferBuffer(m_pGpu, m_pJxZeroXfer);\t// " + DAU,
        "#endif"), "release")
    # 3c. QueueZeroUpload
    s = thay(s, r'^void CDevGpu::QueueZeroUpload\(SDL_GPUTexture\* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp\)\n\{\n\tif \(!pTex \|\| !w \|\| !h \|\| !bpp\) return;$', A(
        "void CDevGpu::QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp)",
        "{",
        "\tif (!pTex || !w || !h || !bpp) return;",
        "#ifdef JX_ANDROID",
        "\t{\t// " + DAU + " tai tu bo dem 0 co dinh (SubmitFrame): khong memset/memcpy vao staging, staging khong phinh 2-4 MB moi trang atlas moi",
        "\t\tRgTexUpload u = { pTex, x, y, w, h, 0, w * h * bpp };",
        "\t\tm_jxZeroUploads.push_back(u);",
        "\t\treturn;",
        "\t}",
        "#endif"), "QueueZeroUpload")
    # 3d. FrameReset
    s = chen_sau(s, r'^\tm_ring\.clear\(\); m_texStage\.clear\(\); m_texUploads\.clear\(\); m_cmds\.clear\(\);$', A(
        "#ifdef JX_ANDROID",
        "\tm_jxZeroUploads.clear();\t// " + DAU,
        "#endif"), "FrameReset")
    # 3e. copy pass: bang mau
    s = thay(s, r'^\t\tif \(!m_palPending\.empty\(\) && m_pPalTex\)\n\t\t\{\n\t\t\tconst UINT bytes = \(UINT\)m_palPending\.size\(\) \* 256 \* 4;$', A(
        "#ifdef JX_ANDROID",
        "\t\tif (!m_palPending.empty() && m_pPalTex)",
        "\t\t{\t// " + DAU + " bang mau di chung staging + transfer buffer co dinh (truoc: tao/huy mot transfer buffer rieng moi khung co bang mau moi",
        "\t\t\t// -> SDL cap/giai phong khoi bo nho 16 MB (vkAllocateMemory) -> chep 40-60 ms tren Fold 7)",
        "\t\t\tconst Uint64 uP0 = SDL_GetPerformanceCounter();",
        "\t\t\tfor (size_t i = 0; i < m_palPending.size(); i++)",
        "\t\t\t{",
        "\t\t\t\tconst UINT off = ((UINT)m_texStage.size() + 15) & ~15u;",
        "\t\t\t\tm_texStage.resize((size_t)off + 1024);",
        "\t\t\t\tmemcpy(&m_texStage[off], &m_palPending[i].second[0], 1024);",
        "\t\t\t\tRgTexUpload u = { m_pPalTex, 0, (UINT)m_palPending[i].first, 256, 1, off, 1024 };",
        "\t\t\t\tm_texUploads.push_back(u);",
        "\t\t\t}",
        "\t\t\tjxK.uPal = (unsigned)m_palPending.size(); m_palPending.clear();",
        "\t\t\tjxK.dChepPal = JxVeMs(uP0, SDL_GetPerformanceCounter());",
        "\t\t}",
        "\t\tif (0)",
        "#endif",
        "\t\tif (!m_palPending.empty() && m_pPalTex)",
        "\t\t{",
        "\t\t\tconst UINT bytes = (UINT)m_palPending.size() * 256 * 4;"), "copy pass bang mau")
    # 3f. copy pass: tex - dong ho map/chep va lenh, dem phinh
    s = thay(s, r'^\t\t\tRgEnsureXfer\(m_pGpu, &m_pTexXfer, &m_texXferSize, \(UINT\)m_texStage\.size\(\), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD\);\n\t\t\tBYTE\* p = m_pTexXfer \? \(BYTE\*\)SDL_MapGPUTransferBuffer\(m_pGpu, m_pTexXfer, true\) : NULL;$', A(
        "#ifdef JX_ANDROID",
        "\t\t\tconst Uint64 uT0 = SDL_GetPerformanceCounter(); const UINT uXferTruoc = m_texXferSize;\t// " + DAU,
        "#endif",
        "\t\t\tRgEnsureXfer(m_pGpu, &m_pTexXfer, &m_texXferSize, (UINT)m_texStage.size(), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD);",
        "#ifdef JX_ANDROID",
        "\t\t\tif (m_texXferSize != uXferTruoc) jxK.uXferTang++;",
        "#endif",
        "\t\t\tBYTE* p = m_pTexXfer ? (BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, m_pTexXfer, true) : NULL;"), "copy pass tex map")
    s = thay(s, r'^\t\t\t\tmemcpy\(p, &m_texStage\[0\], m_texStage\.size\(\)\);\n\t\t\t\tSDL_UnmapGPUTransferBuffer\(m_pGpu, m_pTexXfer\);\n\t\t\t\tfor \(size_t i = 0; i < m_texUploads\.size\(\); i\+\+\)$', A(
        "\t\t\t\tmemcpy(p, &m_texStage[0], m_texStage.size());",
        "\t\t\t\tSDL_UnmapGPUTransferBuffer(m_pGpu, m_pTexXfer);",
        "#ifdef JX_ANDROID",
        "\t\t\t\t{ const Uint64 u = SDL_GetPerformanceCounter(); jxK.dChepTexMap = JxVeMs(uT0, u); }\t// " + DAU,
        "#endif",
        "\t\t\t\tfor (size_t i = 0; i < m_texUploads.size(); i++)"), "copy pass tex memcpy")
    s = thay(s, r'^\t\t\t\tm_uUploads \+= \(unsigned\)m_texUploads\.size\(\);\n\t\t\t\}\n\t\t\telse RgLog\("map transfer texture \(%u B\) that bai: %s", \(unsigned\)m_texStage\.size\(\), SDL_GetError\(\)\);\n\t\t\}$', A(
        "\t\t\t\tm_uUploads += (unsigned)m_texUploads.size();",
        "#ifdef JX_ANDROID",
        "\t\t\t\tjxK.dChepTexLenh = JxVeMs(uT0, SDL_GetPerformanceCounter()) - jxK.dChepTexMap;\t// " + DAU + " lenh tai (SDL_UploadToGPUTexture)",
        "#endif",
        "\t\t\t}",
        "\t\t\telse RgLog(\"map transfer texture (%u B) that bai: %s\", (unsigned)m_texStage.size(), SDL_GetError());",
        "\t\t}",
        "#ifdef JX_ANDROID",
        "\t\tjxK.uXferKB = m_texXferSize >> 10;",
        "\t\tif (!m_jxZeroUploads.empty())",
        "\t\t{\t// " + DAU + " vung 0 (trang atlas moi, o chua co ban CPU): tai tu bo dem 0 co dinh, chi memset mot lan khi tao/phinh",
        "\t\t\tconst Uint64 uZ0 = SDL_GetPerformanceCounter();",
        "\t\t\tUINT needZ = 0;",
        "\t\t\tfor (size_t i = 0; i < m_jxZeroUploads.size(); i++) if (m_jxZeroUploads[i].bytes > needZ) needZ = m_jxZeroUploads[i].bytes;",
        "\t\t\tconst UINT uZeroTruoc = m_jxZeroSize;",
        "\t\t\tRgEnsureXfer(m_pGpu, &m_pJxZeroXfer, &m_jxZeroSize, needZ, SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD);",
        "\t\t\tif (m_jxZeroSize != uZeroTruoc) m_jxZeroDaXoa = 0;",
        "\t\t\tif (m_pJxZeroXfer && m_jxZeroDaXoa < m_jxZeroSize)",
        "\t\t\t{",
        "\t\t\t\tBYTE* pz = (BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, m_pJxZeroXfer, false);",
        "\t\t\t\tif (pz) { memset(pz, 0, m_jxZeroSize); SDL_UnmapGPUTransferBuffer(m_pGpu, m_pJxZeroXfer); m_jxZeroDaXoa = m_jxZeroSize; }",
        "\t\t\t}",
        "\t\t\tif (m_pJxZeroXfer && m_jxZeroDaXoa >= needZ)",
        "\t\t\t{",
        "\t\t\t\tfor (size_t i = 0; i < m_jxZeroUploads.size(); i++)",
        "\t\t\t\t{",
        "\t\t\t\t\tconst RgTexUpload& u = m_jxZeroUploads[i];",
        "\t\t\t\t\tSDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = m_pJxZeroXfer; src.offset = 0; src.pixels_per_row = u.w; src.rows_per_layer = u.h;",
        "\t\t\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.x = u.x; dst.y = u.y; dst.w = u.w; dst.h = u.h; dst.d = 1;",
        "\t\t\t\t\tSDL_UploadToGPUTexture(cp, &src, &dst, false);",
        "\t\t\t\t}",
        "\t\t\t\tm_uUploads += (unsigned)m_jxZeroUploads.size(); jxK.uZero = (unsigned)m_jxZeroUploads.size();",
        "\t\t\t}",
        "\t\t\telse RgLog(\"bo dem 0 (%u B) that bai: %s\", needZ, SDL_GetError());",
        "\t\t\tjxK.dChepZero = JxVeMs(uZ0, SDL_GetPerformanceCounter());",
        "\t\t}",
        "#endif"), "copy pass tex lenh + zero")
    # 3g. ring timer
    s = thay(s, r'^\t\tif \(!m_ring\.empty\(\)\)\n\t\t\{\n\t\t\tconst UINT need = \(UINT\)m_ring\.size\(\);$', A(
        "\t\tif (!m_ring.empty())",
        "\t\t{",
        "#ifdef JX_ANDROID",
        "\t\t\tconst Uint64 uR0 = SDL_GetPerformanceCounter();\t// " + DAU,
        "#endif",
        "\t\t\tconst UINT need = (UINT)m_ring.size();"), "copy pass ring dau")
    s = thay(s, r'^\t\t\telse RgLog\("map transfer ring \(%u B\) that bai: %s", need, SDL_GetError\(\)\);\n\t\t\}$', A(
        "\t\t\telse RgLog(\"map transfer ring (%u B) that bai: %s\", need, SDL_GetError());",
        "#ifdef JX_ANDROID",
        "\t\t\tjxK.dChepRing = JxVeMs(uR0, SDL_GetPerformanceCounter());\t// " + DAU,
        "#endif",
        "\t\t}"), "copy pass ring cuoi")
    # 3h. JxVeCong: cong don truong moi
    s = chen_sau(s, r'^\tg_jxVeTong\.uDoiPipe \+= k\.uDoiPipe; g_jxVeTong\.uDoiTex \+= k\.uDoiTex; g_jxVeTong\.uDoiVs \+= k\.uDoiVs; g_jxVeTong\.uDoiPs \+= k\.uDoiPs; g_jxVeTong\.uDoiCat \+= k\.uDoiCat;$', A(
        "\tg_jxVeTong.dChepPal += k.dChepPal; g_jxVeTong.dChepTexMap += k.dChepTexMap; g_jxVeTong.dChepTexLenh += k.dChepTexLenh; g_jxVeTong.dChepZero += k.dChepZero; g_jxVeTong.dChepRing += k.dChepRing;\t// " + DAU,
        "\tg_jxVeTong.uPal += k.uPal; g_jxVeTong.uZero += k.uZero; g_jxVeTong.uXferTang += k.uXferTang; if (k.uXferKB > g_jxVeMax.uXferKB) g_jxVeMax.uXferKB = k.uXferKB;",
        "\tif (k.dChepPal > g_jxVeMax.dChepPal) g_jxVeMax.dChepPal = k.dChepPal; if (k.dChepTexMap > g_jxVeMax.dChepTexMap) g_jxVeMax.dChepTexMap = k.dChepTexMap; if (k.dChepTexLenh > g_jxVeMax.dChepTexLenh) g_jxVeMax.dChepTexLenh = k.dChepTexLenh;",
        "\tif (k.dChepZero > g_jxVeMax.dChepZero) g_jxVeMax.dChepZero = k.dChepZero; if (k.dChepRing > g_jxVeMax.dChepRing) g_jxVeMax.dChepRing = k.dChepRing;"), "JxVeCong")
    ghi(p, s, nl, cao)

# ============================================================ 4. KRepresentShell3.cpp: cong tac + log
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^int g_nJxAnhBoVeNen = 0;$', A(
        "int g_nJxHoiKhongDe = 1; unsigned g_uJxNapKhungRong = 0, g_uJxHoiTre = 0;\t// " + DAU), "globals d")
    s = chen_sau(s, r'^\tg_nJxVeGiatMs      = Rep3Ini\("VeGiatMs", 20\);.*$', A(
        "\tg_nJxHoiKhongDe    = Rep3Ini(\"NapHoiKhongDe\", 1);\t// " + DAU + " 1 = hoi kich thuoc sprite NPC dang nap o luong nen -> tra 'chua co' (khong nap dong bo de len)"), "ini d")
    # [VE-GIAT]
    s = thay_chuoi(s,
        "| nen: bo ve %u, cho ap %u, ap %u khung %.1f ms\",",
        "| nen: bo ve %u, cho ap %u, ap %u khung %.1f ms | chep: bang mau %u/%.1f, tex map+chep %.1f, lenh tai %.1f, zero %u/%.1f, ring %.1f, xfer %u KB (phinh %u)\",",
        "VE-GIAT dinh dang")
    s = thay_chuoi(s,
        "g_pJxTexMgr ? g_pJxTexMgr->m_uJxApKhungCuoi : 0u, g_pJxTexMgr ? g_pJxTexMgr->m_dJxApCuoi : 0.0);",
        "g_pJxTexMgr ? g_pJxTexMgr->m_uJxApKhungCuoi : 0u, g_pJxTexMgr ? g_pJxTexMgr->m_dJxApCuoi : 0.0,\n\t\tk.uPal, k.dChepPal, k.dChepTexMap, k.dChepTexLenh, k.uZero, k.dChepZero, k.dChepRing, k.uXferKB, k.uXferTang);\t// " + DAU,
        "VE-GIAT doi so")
    # [VE]
    s = thay_chuoi(s,
        "| ve CPU (Begin->End) TB %.2f (max %.1f)\",",
        "| ve CPU (Begin->End) TB %.2f (max %.1f) | chep: bang mau %u hang %.1f ms (max %.2f), tex map+chep %.1f (max %.2f), lenh tai %.1f (max %.2f), zero %u vung %.1f (max %.2f), ring %.1f (max %.2f), xfer phinh %u lan, xfer max %u KB\",",
        "VE dinh dang")
    s = thay_chuoi(s,
        "s_uJxVeCpuKhung ? s_dJxVeCpuTong / s_uJxVeCpuKhung : 0.0, s_dJxVeCpuMax);",
        "s_uJxVeCpuKhung ? s_dJxVeCpuTong / s_uJxVeCpuKhung : 0.0, s_dJxVeCpuMax,\n\t\tt.uPal, t.dChepPal, m.dChepPal, t.dChepTexMap, m.dChepTexMap, t.dChepTexLenh, m.dChepTexLenh, t.uZero, t.dChepZero, m.dChepZero, t.dChepRing, m.dChepRing, t.uXferTang, m.uXferKB);\t// " + DAU,
        "VE doi so")
    # [VE-NAP]
    s = thay_chuoi(s,
        "| nap dong bo NGOAI luc ve: %u lan %.1f ms (max %.2f)\",",
        "| nap dong bo NGOAI luc ve: %u lan %.1f ms (max %.2f) | khung rong/hong khong giao lai %u | hoi NPC dang nap -> chua co %u (bat=%d)\",",
        "VE-NAP dinh dang")
    s = thay_chuoi(s,
        "g_jxNapNgoaiVe.n, g_jxNapNgoaiVe.ms, g_jxNapNgoaiVe.max);",
        "g_jxNapNgoaiVe.n, g_jxNapNgoaiVe.ms, g_jxNapNgoaiVe.max, g_uJxNapKhungRong, g_uJxHoiTre, g_nJxHoiKhongDe);\t// " + DAU,
        "VE-NAP doi so")
    s = thay_chuoi(s,
        "g_dJxNapKhungTre = g_dJxNapKhungTreMax = g_dJxNapNenBan = g_dJxNapKhungAp = g_dJxNapKhungApMax = 0.0; g_uJxNapKhungApKhung = 0; memset(&g_jxNapNgoaiVe, 0, sizeof(g_jxNapNgoaiVe));",
        "g_dJxNapKhungTre = g_dJxNapKhungTreMax = g_dJxNapNenBan = g_dJxNapKhungAp = g_dJxNapKhungApMax = 0.0; g_uJxNapKhungApKhung = 0; memset(&g_jxNapNgoaiVe, 0, sizeof(g_jxNapNgoaiVe)); g_uJxNapKhungRong = 0; g_uJxHoiTre = 0;",
        "VE-NAP dat lai")
    ghi(p, s, nl, cao)

# ============================================================ 5. TextureResMgr.cpp: hoi NPC dang nap -> chua co
p = os.path.join(R3, "TextureResMgr.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^\t\t\t\tif \(m_bVeDangDien\) \{ m_nNapNenBoVe\+\+; return NULL; \}\t// dang ve: bo anh nay khung nay, khung sau co$', A(
        "#ifdef JX_ANDROID",
        "\t\t\t\t// " + DAU + " hoi kich thuoc (GetImageParam tu KNpcRes) gap sprite NPC dang nap o luong nen: tra 'chua co' thay vi nap dong bo de len",
        "\t\t\t\t// (Fold 7: 'tep spr' dong bo toi 33 ms/lan, 282 ms/30 s luc dong); KNpcRes giu m_bChange nen khung sau hoi lai, ket qua nen ve o RepresentBegin.",
        "\t\t\t\tif (g_nJxHoiKhongDe && nType == ISI_T_SPR)",
        "\t\t\t\t{",
        "\t\t\t\t\tconst char* q = pszImage; if (*q == '\\\\' || *q == '/') q++;",
        "\t\t\t\t\tif (_strnicmp(q, \"spr\\\\npcres\", 10) == 0 || _strnicmp(q, \"spr/npcres\", 10) == 0) { g_uJxHoiTre++; return NULL; }",
        "\t\t\t\t}",
        "#endif"), "GetImage dang nap")
    ghi(p, s, nl, cao)

# ============================================================ 6. TextureRes.cpp: khung rong/hong khong giao lai
p = os.path.join(R3, "TextureRes.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = thay_chuoi(s,
        "\t\tif (g_dRep3NapKhung >= (double)g_nJxNapKhungMs && (m_pFrameInfo[nFrame].nJxNen || JxNapKhungGiao(nFrame, 0)))",
        "\t\tif (g_dRep3NapKhung >= (double)g_nJxNapKhungMs && (m_pFrameInfo[nFrame].nJxNen == 1 || JxNapKhungGiao(nFrame, 0)))\t// " + DAU + " nJxNen 2 = rong/hong: nap dong bo (re) nhu cu",
        "PrepareFrameData dieu kien")
    s = thay_chuoi(s,
        "\tif (kq.bHong || !pDiem) { if (pDiem) free(pDiem); g_uJxNapKhungHong++; return; }",
        "\tif (kq.bHong || !pDiem)\n\t{\t// " + DAU + " khung rong (w/h = 0) hoac rut khung hong: ghi kich thuoc, danh dau 2 = khong giao lai (nhanh dong bo giu raw, lan sau re)\n\t\tif (pDiem) free(pDiem);\n\t\tif (kq.nW <= 0 || kq.nH <= 0) { f.nWidth = kq.nW; f.nHeight = kq.nH; f.nOffX = kq.nOffX; f.nOffY = kq.nOffY; f.nJxNen = 2; g_uJxNapKhungRong++; }\n\t\telse g_uJxNapKhungHong++;\n\t\treturn;\n\t}",
        "JxNhanKhungNen hong")
    ghi(p, s, nl, cao)

# ============================================================ 7. config.ini
p = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^VeGiatMs=20$', A(
        "; " + DAU + " NapHoiKhongDe=1: hoi kich thuoc sprite NPC (KNpcRes) dang nap o luong nen -> tra 'chua co', khung sau hoi lai (0 = nap dong bo de len nhu cu)",
        "NapHoiKhongDe=1"), "config.ini d")
    ghi(p, s, nl, cao)

print("xong")
