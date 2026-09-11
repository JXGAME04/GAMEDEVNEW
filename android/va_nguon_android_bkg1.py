# -*- coding: utf-8 -*-
r"""[BKG 11/09] + [PALBUF 11/09] + [FPSNGOAI 11/09] + [MAU 11/09] - sua tan goc theo PHUONGAN_FPS_NHIET_PIN_MOBILE_1109.md muc 6 (chu 11/09 12:40:
"thuc hien cac buoc ... fix o ban mobile khong anh huong ban pc"). Chi JX_ANDROID / tep chi-Android; chuoi Windows khong doi.

  [BKG]     Khung ve (lenh + dinh) giong HET khung vua trinh chieu, khong co texture / bang mau / vung 0 cho tai, khong bi flush giua khung
            -> KHONG SubmitFrame (man hinh dang hien dung khung do, pixel y het). Cua so / be mat doi (xoay, gap-mo, quay lai app, vung an toan)
            -> KSdlApp goi Rep3_JxEpTrinhChieu: khung ke tiep phai trinh chieu; toi da Rep3BoKhungGiongMs giua hai lan trinh chieu.
            Dem [VE-BKG] moi ky: trinh chieu / bo / giong-co-tai / ep / chuoi dai nhat. [Client] Rep3BoKhungGiong 1 bat, 0 chi dem, -1 tat.
  [PALBUF]  Bang mau: storage buffer 8192 hang x 1 KB thay cho texture 256x8192 (log Fold 7 11:48: 260/260 khung chep >= 10 ms la khung tai
            hang bang mau vao texture, ~20 ms co dinh trong driver). Hang moi tai bang SDL_UploadToGPUBuffer tu staging cua khung; shader doc
            g_palBuf[row*256+idx] (bien the -DJX_PAL_BUFFER, g_Rep3GpuFSPalBuf trong Rep3ShadersGPU_spv.h). [Client] Rep3PalBuffer=0 = texture cu.
            Do rieng "lenh tai bang mau" ([VE] bang mau lenh, [VE-GIAT] pal lenh) o CA hai kieu.
  [FPSNGOAI] Chua vao the gioi (dang nhap / chon nhan vat): ve FpsNgoaiTheGioi (30) + xin man 60 Hz; vao the gioi ap lai nac nguoi choi.
  [MAU]     JxDoNhip.java [MAU] them gpu=% (kgsl gpu_busy_percentage...), gpu_mhz, cpu_mhz tung nhan (sysfs); [GPU-SYS] duong doc duoc.

Doc/ghi latin-1, giu CRLF/LF cua tung tep, moc khop dung 1 cho, so byte cao khong doi; chay lai nhieu lan khong sao (kiem DAU tung tep).
Dung:  python android\va_nguon_android_bkg1.py     roi  python ReverseTools\mobile_x64\dich_shader_gpu.py <goc worktree>
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
PL = os.path.join(GOC, "Sources", "S3Client", "Platform")
JAVA = os.path.join(GOC, "android", "gradle-project", "app", "src", "main", "java", "vn", "jx1", "mobile", "JxDoNhip.java")
CFG = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
DICH = os.path.join(GOC, "ReverseTools", "mobile_x64", "dich_shader_gpu.py")


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    crlf = s.count("\r\n")
    if crlf and crlf != s.count("\n"):
        raise SystemExit("tep co xuong dong lan lon, khong va: " + p)
    nl = "\r\n" if crlf else "\n"
    cao = sum(1 for c in s if ord(c) >= 0x80)
    return s.replace("\r\n", "\n"), nl, cao


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
    e = m[0].end()
    return s[:e] + them + s[e:]


# ============================================================ BaseInclude.h
p = os.path.join(R3, "BaseInclude.h")
s, nl, cao = doc(p)
if "[BKG 11/09]" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "uXferTang, uXferKB; };", "uXferTang, uXferKB; double dChepPalLenh; };", "JxVeDo")
    s = sau_dong(s, r"^extern int g_nJxAtlasKe, g_nJxAtlasTrang;[^\n]*\n",
        "extern int g_nJxPalBuffer;\t// [PALBUF 11/09] [Client] Rep3PalBuffer: 1 = bang mau trong storage buffer (tai hang bang lenh copy buffer, khong copy tung hang vao texture 256x8192)\n"
        "extern int g_nJxBoKhungGiong, g_nJxBoKhungGiongMs;\t// [BKG 11/09] [Client] Rep3BoKhungGiong: 1 = khung giong het khung vua trinh chieu -> khong trinh chieu; 0 = chi dem; -1 = tat. Rep3BoKhungGiongMs = toi da ms giua hai lan trinh chieu\n"
        "extern unsigned g_uJxKhungGiongBo, g_uJxKhungGiongCoTai, g_uJxKhungGiongEp, g_uJxKhungGiongDem, g_uJxKhungGiongChuoiMax, g_uJxKhungTrinhChieu;\t// [BKG 11/09] dem trong ky ([VE-BKG])\n",
        "extern atlas")
    ghi(p, s, nl, cao)

# ============================================================ KRepresentShell3.cpp
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p)
if "[BKG 11/09]" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^int g_nJxAtlasKe = 1, g_nJxAtlasTrang = 2048;[^\n]*\n",
        "int g_nJxPalBuffer = 1;\t// [PALBUF 11/09]\n"
        "int g_nJxBoKhungGiong = 1, g_nJxBoKhungGiongMs = 250;\t// [BKG 11/09]\n"
        "unsigned g_uJxKhungGiongBo = 0, g_uJxKhungGiongCoTai = 0, g_uJxKhungGiongEp = 0, g_uJxKhungGiongDem = 0, g_uJxKhungGiongChuoiMax = 0, g_uJxKhungTrinhChieu = 0;\t// [BKG 11/09]\n",
        "dinh nghia g_nJxAtlasKe")
    s = thay1(s, "\tif (g_nJxAtlasTrang != 1024 && g_nJxAtlasTrang != 2048 && g_nJxAtlasTrang != 4096) g_nJxAtlasTrang = 2048;\n",
        "\tif (g_nJxAtlasTrang != 1024 && g_nJxAtlasTrang != 2048 && g_nJxAtlasTrang != 4096) g_nJxAtlasTrang = 2048;\n"
        "\tg_nJxPalBuffer      = Rep3Ini(\"Rep3PalBuffer\", 1) ? 1 : 0;\t// [PALBUF 11/09] 1 = bang mau trong storage buffer (het khung chep 17-100 ms khi tai hang bang mau vao texture 256x8192); 0 = texture nhu cu\n"
        "\tg_nJxBoKhungGiong   = Rep3Ini(\"Rep3BoKhungGiong\", 1);\t// [BKG 11/09] 1 = khung giong het khung vua trinh chieu -> khong trinh chieu; 0 = chi dem [VE-BKG]; -1 = tat han (khong so sanh)\n"
        "\tg_nJxBoKhungGiongMs = Rep3Ini(\"Rep3BoKhungGiongMs\", 250);\t// toi da ms giua hai lan trinh chieu khi khung giong (0 = khong gioi han)\n"
        "\tif (g_nJxBoKhungGiong > 1) g_nJxBoKhungGiong = 1; if (g_nJxBoKhungGiong < -1) g_nJxBoKhungGiong = -1;\n",
        "doc ini atlas")
    s = sau_dong(s, r"^\tRep3Log\(\"\[VE\] nap khung nen=%d[^\n]*\n",
        "\tRep3Log(\"[VE] bang mau kieu %s (Rep3PalBuffer=%d); bo khung giong khung truoc: %d (Rep3BoKhungGiong; toi da %d ms giua hai lan trinh chieu)\", g_nJxPalBuffer ? \"storage buffer\" : \"texture 256x8192\", g_nJxPalBuffer, g_nJxBoKhungGiong, g_nJxBoKhungGiongMs);\t// [PALBUF 11/09] [BKG 11/09]\n",
        "log [VE] nap khung nen")
    s = thay1(s, "\t\tt.uPal, t.dChepPal, m.dChepPal, t.dChepTexMap, m.dChepTexMap, t.dChepTexLenh, m.dChepTexLenh, t.uZero, t.dChepZero, m.dChepZero, t.dChepRing, m.dChepRing, t.uXferTang, m.uXferKB);\t// [VE 11/09 d]\n",
        "\t\tt.uPal, t.dChepPal, m.dChepPal, t.dChepTexMap, m.dChepTexMap, t.dChepTexLenh, m.dChepTexLenh, t.uZero, t.dChepZero, m.dChepZero, t.dChepRing, m.dChepRing, t.uXferTang, m.uXferKB);\t// [VE 11/09 d]\n"
        "\tRep3Log(\"[VE-BKG] trinh chieu %u khung, bo vi giong khung truoc %u, giong nhung co tai %u, giong nhung ep %u (cua so doi / qua %d ms), chi dem %u, chuoi bo dai nhat %u | bat=%d | bang mau kieu %s: lenh tai TB %.3f ms/khung (max %.2f)\",\n"
        "\t\tg_uJxKhungTrinhChieu, g_uJxKhungGiongBo, g_uJxKhungGiongCoTai, g_uJxKhungGiongEp, g_nJxBoKhungGiongMs, g_uJxKhungGiongDem, g_uJxKhungGiongChuoiMax, g_nJxBoKhungGiong, g_nJxPalBuffer ? \"buffer\" : \"texture\", t.dChepPalLenh / n, m.dChepPalLenh);\t// [BKG 11/09] [PALBUF 11/09]\n"
        "\tg_uJxKhungTrinhChieu = g_uJxKhungGiongBo = g_uJxKhungGiongCoTai = g_uJxKhungGiongEp = g_uJxKhungGiongDem = g_uJxKhungGiongChuoiMax = 0;\n",
        "cuoi Rep3Log [VE]")
    s = thay1(s, "xfer %u KB (phinh %u)\"", "xfer %u KB (phinh %u), pal lenh %.1f\"", "[VE-GIAT] format")
    s = thay1(s, "k.uXferKB, k.uXferTang);\t// [VE 11/09 d]", "k.uXferKB, k.uXferTang, k.dChepPalLenh);\t// [VE 11/09 d] [PALBUF 11/09]", "[VE-GIAT] args")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUi.h
p = os.path.join(R3, "D3D9onGPUi.h")
s, nl, cao = doc(p)
if "[BKG 11/09]" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^\tSDL_GPUTransferBuffer\* m_pJxZeroXfer; UINT m_jxZeroSize, m_jxZeroDaXoa; std::vector<RgTexUpload> m_jxZeroUploads;[^\n]*\n",
        "\tstd::vector<RgCmd> m_jxCmdsTruoc; std::vector<BYTE> m_jxRingTruoc; bool m_bJxCoKhungTruoc, m_bJxKhungCoFlush; Uint64 m_uJxTrinhChieuLuc; unsigned m_uJxGiongLienTiep;\t// [BKG 11/09] lenh + dinh cua khung vua trinh chieu de so voi khung sau\n"
        "\tSDL_GPUBuffer* m_pJxPalBuf; std::vector<std::pair<UINT, UINT> > m_jxPalUploads;\t// [PALBUF 11/09] bang mau trong storage buffer; (hang, offset staging) cho tai trong khung\n",
        "member m_pJxZeroXfer")
    s = thay1(s, "\tbool    SubmitFrame(bool bPresent);\t\t\t\t\t\t// phat lai lenh cua khung (Present hoac can doc lai)\n",
        "\tbool    SubmitFrame(bool bPresent);\t\t\t\t\t\t// phat lai lenh cua khung (Present hoac can doc lai)\n"
        "#ifdef JX_ANDROID\n"
        "\tbool    JxBoKhungGiong();\t\t\t\t\t\t\t\t// [BKG 11/09] khung giong het khung vua trinh chieu -> khong SubmitFrame; tra true = da bo\n"
        "#endif\n",
        "decl SubmitFrame")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUDev.cpp
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p)
if "[BKG 11/09]" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    # ctor
    s = thay1(s, "\tm_pJxZeroXfer = NULL; m_jxZeroSize = 0; m_jxZeroDaXoa = 0;\t// [VE 11/09 d]\n",
        "\tm_pJxZeroXfer = NULL; m_jxZeroSize = 0; m_jxZeroDaXoa = 0;\t// [VE 11/09 d]\n"
        "\tm_bJxCoKhungTruoc = false; m_bJxKhungCoFlush = false; m_uJxTrinhChieuLuc = 0; m_uJxGiongLienTiep = 0; m_pJxPalBuf = NULL;\t// [BKG 11/09] [PALBUF 11/09]\n",
        "ctor")
    # static ep trinh chieu (sau JxGopVo)
    s = thay1(s, "\tg_uJxGopVo[k]++;\n}\n#endif\n",
        "\tg_uJxGopVo[k]++;\n}\n"
        "static int s_nJxEpTrinhChieu = 0;\t// [BKG 11/09] 1 = khung ke tiep PHAI trinh chieu (be mat / cua so doi) du giong khung truoc; Present dat lai 0\n"
        "#endif\n",
        "cuoi JxGopVo")
    # JxVeCong
    s = thay1(s, "\tg_jxVeTong.uPal += k.uPal; g_jxVeTong.uZero += k.uZero; g_jxVeTong.uXferTang += k.uXferTang; if (k.uXferKB > g_jxVeMax.uXferKB) g_jxVeMax.uXferKB = k.uXferKB;\n",
        "\tg_jxVeTong.uPal += k.uPal; g_jxVeTong.uZero += k.uZero; g_jxVeTong.uXferTang += k.uXferTang; if (k.uXferKB > g_jxVeMax.uXferKB) g_jxVeMax.uXferKB = k.uXferKB;\n"
        "\tg_jxVeTong.dChepPalLenh += k.dChepPalLenh; if (k.dChepPalLenh > g_jxVeMax.dChepPalLenh) g_jxVeMax.dChepPalLenh = k.dChepPalLenh;\t// [PALBUF 11/09]\n",
        "JxVeCong")
    # CreateShaders
    s = thay1(s, "\tsi.code = g_Rep3GpuFS; si.code_size = sizeof(g_Rep3GpuFS); si.entrypoint = \"main\"; si.format = SDL_GPU_SHADERFORMAT_SPIRV; si.stage = SDL_GPU_SHADERSTAGE_FRAGMENT; si.num_samplers = 3; si.num_uniform_buffers = 1;\n\tm_pFS = SDL_CreateGPUShader(m_pGpu, &si);\n",
        "\tsi.code = g_Rep3GpuFS; si.code_size = sizeof(g_Rep3GpuFS); si.entrypoint = \"main\"; si.format = SDL_GPU_SHADERFORMAT_SPIRV; si.stage = SDL_GPU_SHADERSTAGE_FRAGMENT; si.num_samplers = 3; si.num_uniform_buffers = 1;\n"
        "#ifdef JX_ANDROID\n"
        "\tif (g_nJxPalBuffer)\n"
        "\t{\t// [PALBUF 11/09] bang mau = storage buffer: shader chi co 2 sampler (t0, t1), buffer o set 2 binding 2 (SDL: sau cac sampler)\n"
        "\t\tsi.code = g_Rep3GpuFSPalBuf; si.code_size = sizeof(g_Rep3GpuFSPalBuf); si.num_samplers = 2; si.num_storage_buffers = 1;\n"
        "\t}\n"
        "#endif\n"
        "\tm_pFS = SDL_CreateGPUShader(m_pGpu, &si);\n",
        "CreateShaders FS")
    # Init: tao buffer bang mau ngay
    s = thay1(s, "\tif (!CreateShaders()) return false;\n",
        "\tif (!CreateShaders()) return false;\n"
        "#ifdef JX_ANDROID\n"
        "\tif (g_nJxPalBuffer && !PalInit()) return false;\t// [PALBUF 11/09] tao storage buffer bang mau ngay: shader kieu buffer can bind no truoc lenh ve dau tien\n"
        "#endif\n",
        "Init CreateShaders")
    # Reset
    s = thay1(s, "\tif (m_bFrameOpen || !m_cmds.empty()) SubmitFrame(false);\n\tm_pp = *pp;\n",
        "\tif (m_bFrameOpen || !m_cmds.empty()) SubmitFrame(false);\n"
        "#ifdef JX_ANDROID\n"
        "\tm_bJxCoKhungTruoc = false; s_nJxEpTrinhChieu = 1;\t// [BKG 11/09] backbuffer / swapchain doi -> khung ke tiep phai trinh chieu\n"
        "#endif\n"
        "\tm_pp = *pp;\n",
        "Reset")
    # SubmitFrame dau
    s = thay1(s, "bool CDevGpu::SubmitFrame(bool bPresent)\n{\n#ifdef JX_ANDROID\n",
        "bool CDevGpu::SubmitFrame(bool bPresent)\n{\n#ifdef JX_ANDROID\n"
        "\tif (!bPresent) m_bJxKhungCoFlush = true;\t// [BKG 11/09] flush giua khung (doc lai / doi khung bay): khung nay khong dung lam \"khung truoc\" de so\n",
        "SubmitFrame dau")
    # khoi bang mau (Android)
    s = thay1(s, "\t\tif (!m_palPending.empty() && m_pPalTex)\n\t\t{\t// [VE 11/09 d] bang mau di chung staging",
        "\t\tsize_t uJxTexTruocPal = m_texUploads.size(); Uint64 uJxPalT0 = 0;\t// [PALBUF 11/09] muc tu day tro di trong m_texUploads la hang bang mau (kieu texture cu) -> do rieng\n"
        "\t\tif (!m_palPending.empty() && (m_pPalTex || m_pJxPalBuf))\n\t\t{\t// [VE 11/09 d] bang mau di chung staging",
        "khoi bang mau")
    s = thay1(s, "\t\t\t\tRgTexUpload u = { m_pPalTex, 0, (UINT)m_palPending[i].first, 256, 1, off, 1024 };\n\t\t\t\tm_texUploads.push_back(u);\n",
        "\t\t\t\tif (m_pJxPalBuf) m_jxPalUploads.push_back(std::make_pair((UINT)m_palPending[i].first, off));\t// [PALBUF 11/09] -> storage buffer (tai o duoi)\n"
        "\t\t\t\telse { RgTexUpload u = { m_pPalTex, 0, (UINT)m_palPending[i].first, 256, 1, off, 1024 }; m_texUploads.push_back(u); }\n",
        "push bang mau")
    # dieu kien khoi tai texture
    s = thay1(s, "\t\tif (!m_texUploads.empty() && !m_texStage.empty())\n\t\t{\n#ifdef JX_ANDROID\n\t\t\tconst Uint64 uT0 = SDL_GetPerformanceCounter(); const UINT uXferTruoc = m_texXferSize;\t// [VE 11/09 d]\n",
        "#ifdef JX_ANDROID\n"
        "\t\tif ((!m_texUploads.empty() || !m_jxPalUploads.empty()) && !m_texStage.empty())\t// [PALBUF 11/09] hang bang mau (storage buffer) cung tai tu staging nay\n"
        "#else\n"
        "\t\tif (!m_texUploads.empty() && !m_texStage.empty())\n"
        "#endif\n"
        "\t\t{\n#ifdef JX_ANDROID\n\t\t\tconst Uint64 uT0 = SDL_GetPerformanceCounter(); const UINT uXferTruoc = m_texXferSize;\t// [VE 11/09 d]\n",
        "dieu kien tai texture")
    s = thay1(s, "\t\t\t\tfor (size_t i = 0; i < m_texUploads.size(); i++)\n\t\t\t\t{\n\t\t\t\t\tconst RgTexUpload& u = m_texUploads[i];\n",
        "\t\t\t\tfor (size_t i = 0; i < m_texUploads.size(); i++)\n\t\t\t\t{\n"
        "#ifdef JX_ANDROID\n"
        "\t\t\t\t\tif (i == uJxTexTruocPal) uJxPalT0 = SDL_GetPerformanceCounter();\t// [PALBUF 11/09] tu day la hang bang mau kieu texture cu\n"
        "#endif\n"
        "\t\t\t\t\tconst RgTexUpload& u = m_texUploads[i];\n",
        "vong tai texture")
    s = thay1(s, "\t\t\t\tm_uUploads += (unsigned)m_texUploads.size();\n#ifdef JX_ANDROID\n\t\t\t\tjxK.dChepTexLenh = JxVeMs(uT0, SDL_GetPerformanceCounter()) - jxK.dChepTexMap;\t// [VE 11/09 d] lenh tai (SDL_UploadToGPUTexture)\n#endif\n",
        "\t\t\t\tm_uUploads += (unsigned)m_texUploads.size();\n#ifdef JX_ANDROID\n\t\t\t\tjxK.dChepTexLenh = JxVeMs(uT0, SDL_GetPerformanceCounter()) - jxK.dChepTexMap;\t// [VE 11/09 d] lenh tai (SDL_UploadToGPUTexture)\n"
        "\t\t\t\tif (uJxPalT0) jxK.dChepPalLenh = JxVeMs(uJxPalT0, SDL_GetPerformanceCounter());\t// [PALBUF 11/09] phan hang bang mau (kieu texture cu)\n"
        "\t\t\t\tif (!m_jxPalUploads.empty() && m_pJxPalBuf)\n"
        "\t\t\t\t{\t// [PALBUF 11/09] hang bang mau -> storage buffer bang lenh copy buffer (khong dinh layout/tile cua anh 256x8192: het chep 17-100 ms moi khi co sprite moi)\n"
        "\t\t\t\t\tconst Uint64 uPl0 = SDL_GetPerformanceCounter();\n"
        "\t\t\t\t\tfor (size_t i = 0; i < m_jxPalUploads.size(); i++)\n"
        "\t\t\t\t\t{\n"
        "\t\t\t\t\t\tSDL_GPUTransferBufferLocation src = { m_pTexXfer, m_jxPalUploads[i].second }; SDL_GPUBufferRegion dst = { m_pJxPalBuf, m_jxPalUploads[i].first * 1024u, 1024u };\n"
        "\t\t\t\t\t\tSDL_UploadToGPUBuffer(cp, &src, &dst, false);\n"
        "\t\t\t\t\t}\n"
        "\t\t\t\t\tm_uUploads += (unsigned)m_jxPalUploads.size();\n"
        "\t\t\t\t\tjxK.dChepPalLenh = JxVeMs(uPl0, SDL_GetPerformanceCounter());\n"
        "\t\t\t\t}\n"
        "#endif\n",
        "sau vong tai texture")
    # render pass: bind storage buffer + so sampler
    s = thay1(s, "\t\t\tSDL_GPUBufferBinding bd = { m_pDummy, 0 }; SDL_BindGPUVertexBuffers(pass, 1, &bd, 1);\n",
        "\t\t\tSDL_GPUBufferBinding bd = { m_pDummy, 0 }; SDL_BindGPUVertexBuffers(pass, 1, &bd, 1);\n"
        "#ifdef JX_ANDROID\n"
        "\t\t\tif (g_nJxPalBuffer && m_pJxPalBuf) SDL_BindGPUFragmentStorageBuffers(pass, 0, &m_pJxPalBuf, 1);\t// [PALBUF 11/09] bang mau = storage buffer (set 2, binding 2, sau 2 sampler) - mot lan moi pass\n"
        "#endif\n",
        "bind dummy")
    s = thay1(s, "\t\t\tSDL_BindGPUFragmentSamplers(pass, 0, tb, 3);\n",
        "#ifdef JX_ANDROID\n"
        "\t\t\tSDL_BindGPUFragmentSamplers(pass, 0, tb, g_nJxPalBuffer ? 2 : 3);\t// [PALBUF 11/09] kieu buffer: shader chi khai 2 sampler\n"
        "#else\n"
        "\t\t\tSDL_BindGPUFragmentSamplers(pass, 0, tb, 3);\n"
        "#endif\n",
        "bind samplers")
    # FrameReset
    s = thay1(s, "\tm_jxZeroUploads.clear();\t// [VE 11/09 d]\n",
        "\tm_jxZeroUploads.clear();\t// [VE 11/09 d]\n\tm_jxPalUploads.clear();\t// [PALBUF 11/09]\n",
        "FrameReset")
    # Present + ham bo khung giong
    s = thay1(s, "HRESULT CDevGpu::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)\n{\n\tLock();\n\tSubmitFrame(true);\n\tm_uFrames++;\n",
        "#ifdef JX_ANDROID\n"
        "// [BKG 11/09] Khung nay (danh sach lenh + ring dinh) giong HET khung vua trinh chieu, khong co texture / bang mau / vung 0 cho tai, khong bi flush\n"
        "// giua khung, chua qua Rep3BoKhungGiongMs va khong bi ep (cua so / be mat doi) -> khong SubmitFrame: man hinh dang hien dung khung do (pixel y het),\n"
        "// bot ghi lenh + nop + toan bo viec GPU cua khung. Canh yen (logic 18 tick/s, chi noi suy vat dang di chuyen) phan lon khung giong nhau.\n"
        "// RgCmd/RgDrawState duoc memset 0 khi tao nen so bang memcmp la dung. Tra true = da bo (chi don khung).\n"
        "bool CDevGpu::JxBoKhungGiong()\n"
        "{\n"
        "\tif (g_nJxBoKhungGiong < 0) return false;\n"
        "\tconst Uint64 uNay = SDL_GetPerformanceCounter();\n"
        "\tconst bool bCoTai = !m_texUploads.empty() || !m_palPending.empty() || !m_jxZeroUploads.empty() || !m_jxPalUploads.empty();\n"
        "\tbool bGiong = m_bJxCoKhungTruoc && !m_bJxKhungCoFlush && m_cmds.size() == m_jxCmdsTruoc.size() && m_ring.size() == m_jxRingTruoc.size();\n"
        "\tif (bGiong && !m_cmds.empty() && memcmp(&m_cmds[0], &m_jxCmdsTruoc[0], m_cmds.size() * sizeof(RgCmd)) != 0) bGiong = false;\n"
        "\tif (bGiong && !m_ring.empty() && memcmp(&m_ring[0], &m_jxRingTruoc[0], m_ring.size()) != 0) bGiong = false;\n"
        "\tbool bBo = false;\n"
        "\tif (bGiong)\n"
        "\t{\n"
        "\t\tif (bCoTai) g_uJxKhungGiongCoTai++;\n"
        "\t\telse if (s_nJxEpTrinhChieu) g_uJxKhungGiongEp++;\n"
        "\t\telse if (g_nJxBoKhungGiongMs > 0 && JxVeMs(m_uJxTrinhChieuLuc, uNay) >= (double)g_nJxBoKhungGiongMs) g_uJxKhungGiongEp++;\n"
        "\t\telse if (g_nJxBoKhungGiong > 0) bBo = true;\n"
        "\t\telse g_uJxKhungGiongDem++;\n"
        "\t}\n"
        "\tif (bBo)\n"
        "\t{\n"
        "\t\tg_uJxKhungGiongBo++; m_uJxGiongLienTiep++; if (m_uJxGiongLienTiep > g_uJxKhungGiongChuoiMax) g_uJxKhungGiongChuoiMax = m_uJxGiongLienTiep;\n"
        "\t\tFrameReset();\t// bo lenh + ring cua khung nay (y het khung dang hien); texture cham / cho trong atlas / bang mau don nhu thuong (khong co gi cho tai)\n"
        "\t\treturn true;\n"
        "\t}\n"
        "\t// se trinh chieu: luu lenh + dinh lam \"khung truoc\" (truoc khi SubmitFrame -> FrameReset xoa); khung co flush giua chung thi khong luu\n"
        "\tif (m_bJxKhungCoFlush) { m_bJxCoKhungTruoc = false; m_bJxKhungCoFlush = false; }\n"
        "\telse { m_jxCmdsTruoc = m_cmds; m_jxRingTruoc = m_ring; m_bJxCoKhungTruoc = true; }\n"
        "\tm_uJxTrinhChieuLuc = uNay; m_uJxGiongLienTiep = 0; s_nJxEpTrinhChieu = 0; g_uJxKhungTrinhChieu++;\n"
        "\treturn false;\n"
        "}\n"
        "#endif\n"
        "\n"
        "HRESULT CDevGpu::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)\n{\n\tLock();\n"
        "#ifdef JX_ANDROID\n"
        "\tif (!JxBoKhungGiong()) SubmitFrame(true);\t// [BKG 11/09] khung giong het khung vua trinh chieu -> khong trinh chieu (man hinh giu nguyen)\n"
        "#else\n"
        "\tSubmitFrame(true);\n"
        "#endif\n"
        "\tm_uFrames++;\n",
        "Present")
    # PalInit / PalRelease
    s = thay1(s, "bool CDevGpu::PalInit()\n{\n\tif (m_pPalTex) return true;\n",
        "bool CDevGpu::PalInit()\n{\n"
        "#ifdef JX_ANDROID\n"
        "\tif (m_pJxPalBuf) return true;\n"
        "\tif (g_nJxPalBuffer && !m_pPalTex)\n"
        "\t{\t// [PALBUF 11/09] bang mau = storage buffer RG_PAL_ROWS hang x 1 KB: hang tai bang SDL_UploadToGPUBuffer (copy buffer thuong), shader doc g_palBuf[row*256+idx]\n"
        "\t\tSDL_GPUBufferCreateInfo bi; memset(&bi, 0, sizeof(bi)); bi.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ; bi.size = (Uint32)RG_PAL_ROWS * 1024u;\n"
        "\t\tm_pJxPalBuf = SDL_CreateGPUBuffer(m_pGpu, &bi);\n"
        "\t\tif (!m_pJxPalBuf) { RgLog(\"bang mau: CreateGPUBuffer %d KB that bai: %s\", RG_PAL_ROWS, SDL_GetError()); return false; }\n"
        "\t\tm_palFree.reserve(RG_PAL_ROWS);\n"
        "\t\tfor (int i = RG_PAL_ROWS - 1; i >= 0; i--) m_palFree.push_back(i);\n"
        "\t\tRgLog(\"bang mau: storage buffer %d hang x 256 mau BGRA8 (%d MB) [PALBUF 11/09]\", RG_PAL_ROWS, RG_PAL_ROWS / 1024);\n"
        "\t\treturn true;\n"
        "\t}\n"
        "#endif\n"
        "\tif (m_pPalTex) return true;\n",
        "PalInit")
    s = thay1(s, "\tif (m_pPalTex && m_pGpu) SDL_ReleaseGPUTexture(m_pGpu, m_pPalTex);\n\tm_pPalTex = NULL; m_palFree.clear(); m_palDeferred.clear(); m_palPending.clear();\n",
        "\tif (m_pPalTex && m_pGpu) SDL_ReleaseGPUTexture(m_pGpu, m_pPalTex);\n\tm_pPalTex = NULL; m_palFree.clear(); m_palDeferred.clear(); m_palPending.clear();\n"
        "#ifdef JX_ANDROID\n"
        "\tif (m_pJxPalBuf && m_pGpu) SDL_ReleaseGPUBuffer(m_pGpu, m_pJxPalBuf);\n"
        "\tm_pJxPalBuf = NULL; m_jxPalUploads.clear();\t// [PALBUF 11/09]\n"
        "#endif\n",
        "PalRelease")
    # hook ep trinh chieu (canh Rep3_DoNhipDat)
    s = thay1(s, "// [DONHIP 12/09] ban do nhip: bat/tat chep swapchain moi khung + so khung bay (SDL_SetGPUAllowedFramesInFlight 1..3). -1 = giu nguyen.\n",
        "// [BKG 11/09] cua so / be mat doi (xoay, gap-mo, quay lai app, vung an toan, tieu diem): khung ke tiep PHAI trinh chieu du giong khung truoc.\n"
        "// KSdlApp::TranslateEvent goi qua GetModuleHandle/GetProcAddress (lop tuong thich) nhu Rep3_DoNhipDat.\n"
        "extern \"C\" void Rep3_JxEpTrinhChieu() { s_nJxEpTrinhChieu = 1; }\n"
        "\n"
        "// [DONHIP 12/09] ban do nhip: bat/tat chep swapchain moi khung + so khung bay (SDL_SetGPUAllowedFramesInFlight 1..3). -1 = giu nguyen.\n",
        "hook ep trinh chieu")
    ghi(p, s, nl, cao)

# ============================================================ Rep3ShadersGPU.frag
p = os.path.join(R3, "Rep3ShadersGPU.frag")
s, nl, cao = doc(p)
if "JX_PAL_BUFFER" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "layout(set = 2, binding = 2) uniform sampler2D g_pal;   // atlas bang mau 256 x N (BGRA8), hang = vPal\n",
        "#ifdef JX_PAL_BUFFER\n"
        "// [PALBUF 11/09] (chi Android, -DJX_PAL_BUFFER) bang mau = storage buffer 8192 hang x 256 mau BGRA8 (uint), hang = vPal; set 2 binding 2 = ngay sau 2 sampler\n"
        "layout(std430, set = 2, binding = 2) readonly buffer PalBuf { uint g_palBuf[]; };\n"
        "#else\n"
        "layout(set = 2, binding = 2) uniform sampler2D g_pal;   // atlas bang mau 256 x N (BGRA8), hang = vPal\n"
        "#endif\n",
        "khai bao g_pal")
    s = thay1(s, "    int idx = int(ia.r * 255.0 + 0.5);\n    vec4 c = texelFetch(g_pal, ivec2(idx, int(row)), 0);\n    return vec4(c.rgb, ia.g);\n",
        "    int idx = int(ia.r * 255.0 + 0.5);\n"
        "#ifdef JX_PAL_BUFFER\n"
        "    vec4 c = unpackUnorm4x8(g_palBuf[row * 256u + uint(idx)]);   // byte 0..3 = B, G, R, A (DWORD 0xAARRGGBB little-endian)\n"
        "    return vec4(c.z, c.y, c.x, ia.g);\n"
        "#else\n"
        "    vec4 c = texelFetch(g_pal, ivec2(idx, int(row)), 0);\n"
        "    return vec4(c.rgb, ia.g);\n"
        "#endif\n",
        "PalTex")
    ghi(p, s, nl, cao)

# ============================================================ dich_shader_gpu.py
p = DICH
s, nl, cao = doc(p)
if "g_Rep3GpuFSPalBuf" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, 'for stage, name in (("vert", "g_Rep3GpuVS"), ("frag", "g_Rep3GpuFS")):\n    src = os.path.join(D, "Rep3ShadersGPU." + stage); spv = src + ".spv"\n    r = subprocess.run([GLSLC, "-fshader-stage=" + stage, "-O", "--target-env=vulkan1.0", src, "-o", spv], capture_output=True, text=True)\n',
        '# [PALBUF 11/09] bien the thu ba: frag voi -DJX_PAL_BUFFER=1 (bang mau = storage buffer), chi Android (#ifdef JX_ANDROID) - g_Rep3GpuFS cua PC giu nguyen byte\n'
        'for stage, name, opts in (("vert", "g_Rep3GpuVS", []), ("frag", "g_Rep3GpuFS", []), ("frag", "g_Rep3GpuFSPalBuf", ["-DJX_PAL_BUFFER=1"])):\n    src = os.path.join(D, "Rep3ShadersGPU." + stage); spv = src + "." + name + ".spv"\n    r = subprocess.run([GLSLC, "-fshader-stage=" + stage, "-O", "--target-env=vulkan1.0"] + opts + [src, "-o", spv], capture_output=True, text=True)\n',
        "vong dich")
    s = thay1(s, '    out.append("// %s: %d byte" % (os.path.basename(src), len(b)))\n',
        '    if opts: out.append("#ifdef JX_ANDROID\\t// [PALBUF 11/09] bien the bang mau = storage buffer (%s), chi Android" % " ".join(opts))\n'
        '    out.append("// %s: %d byte" % (os.path.basename(src), len(b)))\n',
        "dau mang")
    s = thay1(s, '    out.append("};"); out.append("")\n',
        '    out.append("};")\n    if opts: out.append("#endif")\n    out.append("")\n    try: os.remove(spv)\n    except OSError: pass\n',
        "cuoi mang")
    ghi(p, s, nl, cao)

# ============================================================ KSdlApp.cpp
p = os.path.join(PL, "KSdlApp.cpp")
s, nl, cao = doc(p)
if "[BKG 11/09]" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "bool KSdlApp::TranslateEvent(const SDL_Event& ev)\n{\n",
        "#ifdef JX_ANDROID\n"
        "// [BKG 11/09] Represent3 khong trinh chieu khung giong het khung truoc; be mat / cua so doi (xoay, gap-mo, quay lai app, vung an toan, tieu diem)\n"
        "// thi khung ke tiep PHAI trinh chieu. Goi Rep3_JxEpTrinhChieu cua libRepresent3.so qua GetModuleHandle/GetProcAddress (lop tuong thich) nhu JxPerfHudAndroid.\n"
        "typedef void (*PFN_Rep3JxEpTrinhChieu)();\n"
        "static void JxSdl_EpTrinhChieu()\n"
        "{\n"
        "\tstatic PFN_Rep3JxEpTrinhChieu s_pfn = NULL; static int s_nThu = 0;\n"
        "\tif (!s_pfn && s_nThu < 8) { s_nThu++; HMODULE h = GetModuleHandleA(\"Represent3.dll\"); if (h) s_pfn = (PFN_Rep3JxEpTrinhChieu)GetProcAddress(h, \"Rep3_JxEpTrinhChieu\"); }\n"
        "\tif (s_pfn) s_pfn();\n"
        "}\n"
        "#endif\n"
        "\n"
        "bool KSdlApp::TranslateEvent(const SDL_Event& ev)\n{\n",
        "TranslateEvent dau")
    s = thay1(s, "\tif (ChamSuKien(ev))\t\t// [ANDROID 09/09 CHAM] su kien chuot do NGON TAY sinh ra di duong rieng\n\t\treturn true;\n#endif\n\tswitch (ev.type)\n",
        "\tif (ChamSuKien(ev))\t\t// [ANDROID 09/09 CHAM] su kien chuot do NGON TAY sinh ra di duong rieng\n\t\treturn true;\n"
        "\tif (ev.type == SDL_EVENT_WINDOW_SHOWN || ev.type == SDL_EVENT_WINDOW_EXPOSED || ev.type == SDL_EVENT_WINDOW_RESIZED || ev.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED\n"
        "\t\t|| ev.type == SDL_EVENT_WINDOW_RESTORED || ev.type == SDL_EVENT_WINDOW_DISPLAY_CHANGED || ev.type == SDL_EVENT_WINDOW_SAFE_AREA_CHANGED\n"
        "\t\t|| ev.type == SDL_EVENT_WINDOW_FOCUS_GAINED || ev.type == SDL_EVENT_DID_ENTER_FOREGROUND)\n"
        "\t\tJxSdl_EpTrinhChieu();\t// [BKG 11/09] be mat / cua so doi -> khung ke tiep phai trinh chieu (khong doi luong su kien ben duoi)\n"
        "#endif\n\tswitch (ev.type)\n",
        "truoc switch")
    ghi(p, s, nl, cao)

# ============================================================ JxPerfHudAndroid.cpp
p = os.path.join(PL, "JxPerfHudAndroid.cpp")
s, nl, cao = doc(p)
if "[FPSNGOAI 11/09]" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "static int s_nFpsMuc = 0;\n",
        "static int s_nFpsMuc = 0;\n"
        "static int s_nFpsNgoai = -2;\t// [FPSNGOAI 11/09] [Client] FpsNgoaiTheGioi (mac dinh 30, 0 = tat): nhip ve khi CHUA vao the gioi (dang nhap, chon nhan vat); -2 = chua doc ini\n"
        "static int s_nNgoaiDangAp = 0;\t// 1 = dang ve theo nhip ngoai the gioi\n",
        "s_nFpsMuc")
    s = thay1(s, "void PerfHud_Draw(int nPaintFps, int nLogicFps, unsigned int dwPing)\n{\n",
        "// [FPSNGOAI 11/09] Ngoai the gioi (dang nhap, chon nhan vat) khong co gi de xem o 90-120 khung/giay (log Fold 7 11:48: ~90 fps, 2,3-4,7 W luc do):\n"
        "// ve FpsNgoaiTheGioi + xin man 60 Hz; vao the gioi (co thanh cong cu) thi ap lai nac nguoi choi chon. Khong lam gi khi ban do nhip [DoNhip] Bat=1 (no tu dat nhip).\n"
        "static void FpsNgoai_Nhip()\n"
        "{\n"
        "\tif (s_nFpsNgoai == -2)\n"
        "\t{\n"
        "\t\ts_nFpsNgoai = GetPrivateProfileInt(\"Client\", \"FpsNgoaiTheGioi\", 30, \".\\\\config.ini\");\n"
        "\t\tif (s_nFpsNgoai < 0) s_nFpsNgoai = 0;\n"
        "\t\tif (s_nFpsNgoai > 0 && s_nFpsNgoai < 15) s_nFpsNgoai = 15;\n"
        "\t}\n"
        "\tif (s_nFpsNgoai <= 0 || s_nDnBat > 0) return;\n"
        "\tconst bool bTrong = KUiToolsControlBar::GetSelf() != NULL;\n"
        "\tif (!bTrong)\n"
        "\t{\t// LoadSetting (UiInit + UiShell) ap lai nac nguoi choi SAU moc nay (thu may ao: 2 lan setFrameRate(0) ngay sau) -> he thay nhip khac la dat lai\n"
        "\t\tint nFps = 0; JxDoNhip_LayNhip(&nFps, NULL, NULL);\n"
        "\t\tif (nFps != s_nFpsNgoai)\n"
        "\t\t{\n"
        "\t\t\ts_nNgoaiDangAp = 1;\n"
        "\t\t\tJxDoNhip_DatNhip(s_nFpsNgoai, -1, -1);\n"
        "\t\t\tDnXinHz(60);\n"
        "\t\t\tSDL_Log(\"[FPSNGOAI] chua vao the gioi (nhip dang %d) -> %d khung/giay, xin man 60 Hz\", nFps, s_nFpsNgoai);\n"
        "\t\t}\n"
        "\t}\n"
        "\telse if (s_nNgoaiDangAp)\n"
        "\t{\n"
        "\t\ts_nNgoaiDangAp = 0;\n"
        "\t\tJxNhip_DatMuc(s_nFpsMuc);\t// ap lai nac nguoi choi (PaintFps + xin man hinh)\n"
        "\t\tSDL_Log(\"[FPSNGOAI] da vao the gioi -> ap lai nac %d\", s_nFpsMuc);\n"
        "\t}\n"
        "}\n"
        "\n"
        "void PerfHud_Draw(int nPaintFps, int nLogicFps, unsigned int dwPing)\n{\n",
        "PerfHud_Draw dau")
    s = thay1(s, "\tDoNhip_VeNhan();\t// [DONHIP 12/09] ten pha dang do (ca khi tat bang do)\n",
        "\tFpsNgoai_Nhip();\t// [FPSNGOAI 11/09] nhip ve ngoai / trong the gioi\n"
        "\tDoNhip_VeNhan();\t// [DONHIP 12/09] ten pha dang do (ca khi tat bang do)\n",
        "goi FpsNgoai")
    ghi(p, s, nl, cao)

# ============================================================ JxDoNhip.java
p = JAVA
s, nl, cao = doc(p)
if "[MAU 11/09]" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "    private boolean mDung = false;\n",
        "    private boolean mDung = false;\n"
        "    private boolean mSysDaTim = false; private String mGpuBanDuong = null, mGpuXungDuong = null;   // [MAU 11/09] duong sysfs GPU doc duoc\n",
        "mDung")
    s = thay1(s, "            ghi(sb.toString());\n        }\n        catch (Throwable t) { ghi(\"[LOI] ghiMau \" + t); }\n",
        "            sb.append(mauSys());   // [MAU 11/09] gpu=% gpu_mhz cpu_mhz\n"
        "            ghi(sb.toString());\n        }\n        catch (Throwable t) { ghi(\"[LOI] ghiMau \" + t); }\n",
        "ghiMau cuoi")
    s = thay1(s, "    private Display manHinh()\n",
        "    // [MAU 11/09] GPU ban (%), xung GPU (MHz), xung CPU tung nhan (MHz) tu sysfs - de biet phan GPU trong dien/nhiet (phuong an D0).\n"
        "    // May khong cho doc thi ghi '-' ; tim duong mot lan, ghi [GPU-SYS]. Loi gi cung nuot.\n"
        "    private static final String[] GPU_BAN = { \"/sys/class/kgsl/kgsl-3d0/gpu_busy_percentage\", \"/sys/kernel/gpu/gpu_busy\", \"/sys/class/misc/mali0/device/utilization\", \"/sys/devices/platform/mali.0/utilization\" };\n"
        "    private static final String[] GPU_XUNG = { \"/sys/class/kgsl/kgsl-3d0/gpuclk\", \"/sys/class/kgsl/kgsl-3d0/devfreq/cur_freq\", \"/sys/kernel/gpu/gpu_clock\", \"/sys/class/misc/mali0/device/clock\" };\n"
        "    private static String docSys(String p)\n"
        "    {\n"
        "        try (BufferedReader r = new BufferedReader(new InputStreamReader(new FileInputStream(p), StandardCharsets.US_ASCII)))\n"
        "        { String l = r.readLine(); return l == null ? null : l.trim(); }\n"
        "        catch (Throwable t) { return null; }\n"
        "    }\n"
        "    private static long soDau(String s) { try { String d = s.replaceAll(\"[^0-9].*$\", \"\"); return d.isEmpty() ? -1 : Long.parseLong(d); } catch (Throwable t) { return -1; } }\n"
        "    private static long raMhz(long v) { return v >= 100000000L ? v / 1000000L : (v >= 100000L ? v / 1000L : v); }   // Hz / kHz / MHz -> MHz\n"
        "    private String mauSys()\n"
        "    {\n"
        "        StringBuilder sb = new StringBuilder();\n"
        "        try\n"
        "        {\n"
        "            if (!mSysDaTim)\n"
        "            {\n"
        "                mSysDaTim = true;\n"
        "                for (String p : GPU_BAN) if (docSys(p) != null) { mGpuBanDuong = p; break; }\n"
        "                for (String p : GPU_XUNG) if (docSys(p) != null) { mGpuXungDuong = p; break; }\n"
        "                ghi(\"[GPU-SYS] \" + gio() + \" ban=\" + (mGpuBanDuong == null ? \"khong doc duoc\" : mGpuBanDuong) + \" xung=\" + (mGpuXungDuong == null ? \"khong doc duoc\" : mGpuXungDuong));\n"
        "            }\n"
        "            String ban = mGpuBanDuong == null ? null : docSys(mGpuBanDuong);\n"
        "            String xung = mGpuXungDuong == null ? null : docSys(mGpuXungDuong);\n"
        "            long nBan = ban == null ? -1 : soDau(ban), nXung = xung == null ? -1 : soDau(xung);\n"
        "            sb.append(\" | gpu=\").append(nBan < 0 ? \"-\" : (nBan + \"%\"));\n"
        "            if (nXung >= 0) sb.append(\" gpu_mhz=\").append(raMhz(nXung));\n"
        "            StringBuilder c = new StringBuilder();\n"
        "            for (int i = 0; i < 12; i++)\n"
        "            {\n"
        "                String f = docSys(\"/sys/devices/system/cpu/cpu\" + i + \"/cpufreq/scaling_cur_freq\");\n"
        "                if (f == null) { if (i == 0) continue; break; }\n"
        "                if (c.length() > 0) c.append('/');\n"
        "                c.append(raMhz(soDau(f)));\n"
        "            }\n"
        "            if (c.length() > 0) sb.append(\" cpu_mhz=\").append(c);\n"
        "        }\n"
        "        catch (Throwable ignored) {}\n"
        "        return sb.toString();\n"
        "    }\n"
        "\n"
        "    private Display manHinh()\n",
        "manHinh")
    ghi(p, s, nl, cao)

# ============================================================ config.ini lop ghi de
p = CFG
s, nl, cao = doc(p)
if "Rep3PalBuffer" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "Rep3AtlasTrang=2048\n",
        "Rep3AtlasTrang=2048\n"
        "; [PALBUF 11/09] Rep3PalBuffer=1: bang mau trong storage buffer 8 MB, hang moi tai bang lenh copy buffer -> het khung \"chep\" 17-100 ms moi khi co sprite moi\n"
        ";   (log Fold 7 11:48: 260/260 khung chep cham la khung tai hang bang mau vao texture 256x8192). 0 = texture nhu cu. [VE] ghi \"bang mau kieu ...\", [VE-GIAT] \"pal lenh\".\n"
        "Rep3PalBuffer=1\n"
        "; [BKG 11/09] Rep3BoKhungGiong=1: khung ve giong HET khung vua trinh chieu (lenh + dinh, khong texture cho tai) thi KHONG trinh chieu - man hinh giu nguyen, pixel y het,\n"
        ";   bot ghi lenh + nop + viec GPU (canh yen phan lon khung giong nhau). 0 = chi dem (xem [VE-BKG] trong jx_rep3.log), -1 = tat han.\n"
        ";   Rep3BoKhungGiongMs = toi da ms giua hai lan trinh chieu khi khung giong (0 = khong gioi han); cua so/be mat doi thi trinh chieu ngay.\n"
        "Rep3BoKhungGiong=1\n"
        "Rep3BoKhungGiongMs=250\n"
        "; [FPSNGOAI 11/09] FpsNgoaiTheGioi=30: nhip ve khi chua vao the gioi (dang nhap, chon nhan vat) + xin man 60 Hz; vao the gioi ap lai nac nguoi choi chon (0 = tat)\n"
        "FpsNgoaiTheGioi=30\n",
        "config Rep3AtlasTrang")
    ghi(p, s, nl, cao)

print("xong - tiep: python ReverseTools\\mobile_x64\\dich_shader_gpu.py \"%s\"" % GOC)
