# -*- coding: utf-8 -*-
r"""[GOP 11/09] Dot C buoc 1 (C2 + C3) - bot viec ghi lenh cho lop SDL-GPU luc dong (chu 11/09 15:35: "lam C").
Log Fold 7 15:10 (ban D1 109111459), Tong Kim: 1 818 lenh ve / khung, doi ps 837, doi texture 768, pipeline 206; ghi lenh 2,6 ms +
ve CPU 2,7 ms -> CPU 96/85 % o cua so 800 dan/tick (GPU da het nghen nho D1).

  [C3 BINDRING] Ring dinh bind MOT lan moi render pass (cung dinh gia), moi lenh ve dung first_vertex = ringOff / (stride+4)
      thay vi mot SDL_BindGPUVertexBuffers rieng -> bot 1 lenh Vulkan moi draw (1 818/khung). Ring duoc CAN len boi (stride+4)
      khi mo lenh moi (vai byte dem) de first_vertex chia het. [Client] Rep3BindRing=0 = nhu cu.
  [C2 PSBUF] Trang thai tang texture (RgPsCb 80 byte: colorop/alphaop/alphatest/loc) khong con day uniform moi lenh: gom cac
      to hop DUY NHAT cua khung vao mot storage buffer (toi da 4 096), chi so 12 bit di theo DINH trong o PALROW
      (bit 0..12 hang bang mau, 13..24 chi so ps, 25..30 danh cho lop atlas cua buoc sau). Nho vay hai quad chi khac ps
      VAN GOP duoc vao mot lenh ve. Shader bien the -DJX_PAL_BUFFER -DJX_PS_BUFFER (g_Rep3GpuFSPalPs, chi JX_ANDROID;
      mang cua PC va mang pal-buffer cu giu nguyen byte). [Client] Rep3PsBuffer=0 = nhu ban 109111459; Rep3PalBuffer=0 = ve han
      shader PC (tat ca hai).
  Log: [VE-GOP] them "ps bang N muc (tran M)", [VE] giu nguyen.

Chi JX_ANDROID / tep chi-Android; chuoi Windows khong doi mot dong. Doc/ghi latin-1, giu CRLF/LF, moc khop dung 1 cho, byte cao
khong doi; chay lai nhieu lan khong sao (kiem dau [GOP 11/09] tung tep).
Dung:  python android\va_nguon_android_gop1.py   roi  python ReverseTools\mobile_x64\dich_shader_gpu.py <goc worktree>
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
CFG = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
DICH = os.path.join(GOC, "ReverseTools", "mobile_x64", "dich_shader_gpu.py")
DAU = "[GOP 11/09]"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    crlf = s.count("\r\n")
    if crlf and crlf != s.count("\n"):
        raise SystemExit("tep co xuong dong lan lon, khong va: " + p)
    nl = "\r\n" if crlf else "\n"
    return s.replace("\r\n", "\n"), nl, sum(1 for c in s if ord(c) >= 0x80)


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
if DAU in p and DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
elif DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^extern int g_nJxSwapchainLogic;[^\n]*\n",
        "extern int g_nJxPsBuffer, g_nJxBindRing;\t// " + DAU + " [Client] Rep3PsBuffer: trang thai tang texture qua storage buffer + chi so theo dinh (gop duoc quad khac ps);\n"
        "\t\t\t\t\t\t\t\t\t\t// Rep3BindRing: bind ring dinh mot lan moi render pass, lenh ve dung first_vertex\n"
        "extern unsigned g_uJxPsBangMax, g_uJxPsTran;\t// " + DAU + " so muc bang ps lon nhat trong ky / so lan tran bang\n",
        "extern D1")
    ghi(p, s, nl, cao)

# ============================================================ KRepresentShell3.cpp
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^int g_nJxSwapchainLogic = 100;[^\n]*\n",
        "int g_nJxPsBuffer = 1, g_nJxBindRing = 1;\t// " + DAU + "\n"
        "unsigned g_uJxPsBangMax = 0, g_uJxPsTran = 0;\t// " + DAU + "\n",
        "dinh nghia D1")
    s = thay1(s, "\tg_nJxSwapchainLogic = Rep3Ini(\"Rep3SwapchainLogic\", 100);",
        "\tg_nJxPsBuffer       = Rep3Ini(\"Rep3PsBuffer\", 1) ? 1 : 0;\t// " + DAU + " 1 = trang thai tang texture qua storage buffer, chi so theo dinh (hai quad khac ps van gop duoc; bot 800 lan day uniform/khung)\n"
        "\tif (!g_nJxPalBuffer) g_nJxPsBuffer = 0;\t// shader PC khong co buffer nao\n"
        "\tg_nJxBindRing       = Rep3Ini(\"Rep3BindRing\", 1) ? 1 : 0;\t// " + DAU + " 1 = bind ring dinh mot lan moi render pass, lenh ve dung first_vertex (bot 1 lenh Vulkan moi draw)\n"
        "\tg_nJxSwapchainLogic = Rep3Ini(\"Rep3SwapchainLogic\", 100);",
        "doc ini D1")
    s = thay1(s, "\tRep3Log(\"[VE] bang mau kieu %s (Rep3PalBuffer=%d); bo khung giong khung truoc: %d (Rep3BoKhungGiong; toi da %d ms giua hai lan trinh chieu)\", g_nJxPalBuffer ? \"storage buffer\" : \"texture 256x8192\", g_nJxPalBuffer, g_nJxBoKhungGiong, g_nJxBoKhungGiongMs);\t// [PALBUF 11/09] [BKG 11/09]\n",
        "\tRep3Log(\"[VE] bang mau kieu %s (Rep3PalBuffer=%d); bo khung giong khung truoc: %d (Rep3BoKhungGiong; toi da %d ms giua hai lan trinh chieu); ps theo dinh=%d, bind ring mot lan=%d\", g_nJxPalBuffer ? \"storage buffer\" : \"texture 256x8192\", g_nJxPalBuffer, g_nJxBoKhungGiong, g_nJxBoKhungGiongMs, g_nJxPsBuffer, g_nJxBindRing);\t// [PALBUF 11/09] [BKG 11/09] " + DAU + "\n",
        "log [VE] bang mau")
    s = thay1(s, "g_nJxAtlasKe, g_nJxAtlasTrang, g_uRep3AtlasPages);\t// [VE 11/09 e]\n",
        "g_nJxAtlasKe, g_nJxAtlasTrang, g_uRep3AtlasPages, g_uJxPsBangMax, g_uJxPsTran);\t// [VE 11/09 e] " + DAU + "\n",
        "[VE-GOP] args")
    s = thay1(s, "| atlas ke=%d trang %d: %u trang\",\n",
        "| atlas ke=%d trang %d: %u trang | ps bang %u muc (tran %u)\",\n",
        "[VE-GOP] format")
    s = thay1(s, "\ts_dJxVeCpuTong = 0.0; s_dJxVeCpuMax = 0.0; s_uJxVeCpuKhung = 0;\n",
        "\ts_dJxVeCpuTong = 0.0; s_dJxVeCpuMax = 0.0; s_uJxVeCpuKhung = 0; g_uJxPsBangMax = 0; g_uJxPsTran = 0;\t// " + DAU + "\n",
        "dat lai ky")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUi.h
p = os.path.join(R3, "D3D9onGPUi.h")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "\tSDL_GPUBuffer* m_pJxPalBuf; std::vector<std::pair<UINT, UINT> > m_jxPalUploads;\t// [PALBUF 11/09] bang mau trong storage buffer; (hang, offset staging) cho tai trong khung\n",
        "\tSDL_GPUBuffer* m_pJxPalBuf; std::vector<std::pair<UINT, UINT> > m_jxPalUploads;\t// [PALBUF 11/09] bang mau trong storage buffer; (hang, offset staging) cho tai trong khung\n"
        "\tSDL_GPUBuffer* m_pJxPsBuf; std::vector<RgPsCb> m_jxPsBang; std::map<unsigned long long, UINT> m_jxPsMap; RgPsCb m_jxPsCuoi; UINT m_uJxPsCuoi;\t// " + DAU + " bang to hop trang thai tang texture cua khung (chi so di theo dinh)\n",
        "member m_pJxPalBuf")
    s = thay1(s, "#ifdef JX_ANDROID\n\tbool    JxBoKhungGiong();\t\t\t\t\t\t\t\t// [BKG 11/09] khung giong het khung vua trinh chieu -> khong SubmitFrame; tra true = da bo\n#endif\n",
        "#ifdef JX_ANDROID\n"
        "\tbool    JxBoKhungGiong();\t\t\t\t\t\t\t\t// [BKG 11/09] khung giong het khung vua trinh chieu -> khong SubmitFrame; tra true = da bo\n"
        "\tUINT    JxPsIdx(const RgPsCb& ps);\t\t\t\t\t\t// " + DAU + " chi so to hop trang thai tang texture trong bang cua khung (them neu chua co)\n"
        "#endif\n",
        "decl JxBoKhungGiong")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUDev.cpp
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    # hang so + ctor
    s = thay1(s, "#define RG_PAL_ROWS 8192\n",
        "#define RG_PAL_ROWS 8192\n"
        "#define JX_PS_MAX 4096\t// " + DAU + " so to hop trang thai tang texture toi da trong mot khung (chi so 12 bit trong o PALROW)\n",
        "RG_PAL_ROWS")
    s = thay1(s, "\tm_bJxCoKhungTruoc = false; m_bJxKhungCoFlush = false; m_uJxTrinhChieuLuc = 0; m_uJxGiongLienTiep = 0; m_pJxPalBuf = NULL;\t// [BKG 11/09] [PALBUF 11/09]\n",
        "\tm_bJxCoKhungTruoc = false; m_bJxKhungCoFlush = false; m_uJxTrinhChieuLuc = 0; m_uJxGiongLienTiep = 0; m_pJxPalBuf = NULL;\t// [BKG 11/09] [PALBUF 11/09]\n"
        "\tm_pJxPsBuf = NULL; memset(&m_jxPsCuoi, 0, sizeof(m_jxPsCuoi)); m_uJxPsCuoi = 0xFFFFFFFFu; m_uJxPsStageOff = 0xFFFFFFFFu;\t// " + DAU + "\n",
        "ctor")
    # offsetof(RgDrawState, ps) trong DrawInternal: <stddef.h> (stdio/stdlib cua NDK khong bao dam keo vao)
    s = thay1(s, "#include <stdio.h>\n",
        "#include <stddef.h>\t// " + DAU + " offsetof(RgDrawState, ps)\n#include <stdio.h>\n",
        "include stddef")
    # ham JxPsIdx (dat truoc SubmitFrame, sau RgEnsureXfer)
    s = thay1(s, "bool CDevGpu::SubmitFrame(bool bPresent)\n{\n",
        "#ifdef JX_ANDROID\n"
        "// " + DAU + " Chi so to hop trang thai tang texture (RgPsCb) trong bang cua khung: hai lenh ve chi khac ps thi VAN gop duoc vi ps\n"
        "// di theo dinh (12 bit trong o PALROW) chu khong phai uniform cua lenh. Bang gui len storage buffer mot lan moi khung.\n"
        "// Nho o cuoi (ps doi 837 lan tren 1 818 lenh -> phan lon lenh dung lai ps ngay truoc) roi moi tra bang bam; bam va cham\n"
        "// van kiem lai bang memcmp nen khong the tra nham to hop.\n"
        "UINT CDevGpu::JxPsIdx(const RgPsCb& ps)\n"
        "{\n"
        "\tif (m_uJxPsCuoi != 0xFFFFFFFFu && memcmp(&m_jxPsCuoi, &ps, sizeof(ps)) == 0) return m_uJxPsCuoi;\n"
        "\tunsigned long long h = 1469598103934665603ULL;\t// FNV-1a\n"
        "\tconst BYTE* pb = (const BYTE*)&ps;\n"
        "\tfor (size_t i = 0; i < sizeof(ps); i++) { h ^= pb[i]; h *= 1099511628211ULL; }\n"
        "\tUINT idx = 0xFFFFFFFFu;\n"
        "\tstd::map<unsigned long long, UINT>::iterator it = m_jxPsMap.find(h);\n"
        "\tif (it != m_jxPsMap.end() && it->second < m_jxPsBang.size() && memcmp(&m_jxPsBang[it->second], &ps, sizeof(ps)) == 0)\n"
        "\t\tidx = it->second;\n"
        "\tif (idx == 0xFFFFFFFFu)\n"
        "\t{\n"
        "\t\tif (m_jxPsBang.size() < JX_PS_MAX)\n"
        "\t\t{\n"
        "\t\t\tidx = (UINT)m_jxPsBang.size(); m_jxPsBang.push_back(ps);\n"
        "\t\t\tif (it == m_jxPsMap.end()) m_jxPsMap[h] = idx;\t// va cham bam: giu muc dau, muc sau van dung nhung tra bang memcmp\n"
        "\t\t\tif (m_jxPsBang.size() > g_uJxPsBangMax) g_uJxPsBangMax = (unsigned)m_jxPsBang.size();\n"
        "\t\t}\n"
        "\t\telse { idx = JX_PS_MAX - 1; m_jxPsBang[idx] = ps; g_uJxPsTran++; }\t// tran (chua gap: canh dong nhat vai chuc to hop)\n"
        "\t}\n"
        "\tm_jxPsCuoi = ps; m_uJxPsCuoi = idx;\n"
        "\treturn idx;\n"
        "}\n"
        "#endif\n"
        "\n"
        "bool CDevGpu::SubmitFrame(bool bPresent)\n{\n",
        "truoc SubmitFrame")
    # CreateShaders: bien the pal+ps
    s = thay1(s, "\t\tsi.code = g_Rep3GpuFSPalBuf; si.code_size = sizeof(g_Rep3GpuFSPalBuf); si.num_samplers = 2; si.num_storage_buffers = 1;\n",
        "\t\tsi.code = g_Rep3GpuFSPalBuf; si.code_size = sizeof(g_Rep3GpuFSPalBuf); si.num_samplers = 2; si.num_storage_buffers = 1;\n"
        "\t\tif (g_nJxPsBuffer)\n"
        "\t\t{\t// " + DAU + " them bang trang thai tang texture (set 2, binding 3); khong con uniform cua fragment\n"
        "\t\t\tsi.code = g_Rep3GpuFSPalPs; si.code_size = sizeof(g_Rep3GpuFSPalPs); si.num_storage_buffers = 2; si.num_uniform_buffers = 0;\n"
        "\t\t}\n",
        "CreateShaders bien the")
    # tao buffer ps trong Init (canh PalInit)
    s = thay1(s, "\tif (g_nJxPalBuffer && !PalInit()) return false;\t// [PALBUF 11/09] tao storage buffer bang mau ngay: shader kieu buffer can bind no truoc lenh ve dau tien\n",
        "\tif (g_nJxPalBuffer && !PalInit()) return false;\t// [PALBUF 11/09] tao storage buffer bang mau ngay: shader kieu buffer can bind no truoc lenh ve dau tien\n"
        "\tif (g_nJxPsBuffer)\n"
        "\t{\t// " + DAU + " bang to hop trang thai tang texture (80 byte moi muc)\n"
        "\t\tSDL_GPUBufferCreateInfo bi; memset(&bi, 0, sizeof(bi)); bi.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ; bi.size = (Uint32)(JX_PS_MAX * sizeof(RgPsCb));\n"
        "\t\tm_pJxPsBuf = SDL_CreateGPUBuffer(m_pGpu, &bi);\n"
        "\t\tif (!m_pJxPsBuf) { RgLog(\"bang ps: CreateGPUBuffer %u KB that bai: %s\", (unsigned)(JX_PS_MAX * sizeof(RgPsCb) >> 10), SDL_GetError()); return false; }\n"
        "\t\tRgLog(\"bang trang thai tang texture: storage buffer %d muc x %u byte (%u KB) \" \"" + DAU + "\", JX_PS_MAX, (unsigned)sizeof(RgPsCb), (unsigned)(JX_PS_MAX * sizeof(RgPsCb) >> 10));\n"
        "\t}\n",
        "Init PalInit")
    # tai bang ps trong copy pass (ngay sau khoi tai bang mau vao buffer)
    s = thay1(s, "\t\t\t\t\tm_uUploads += (unsigned)m_jxPalUploads.size();\n\t\t\t\t\tjxK.dChepPalLenh = JxVeMs(uPl0, SDL_GetPerformanceCounter());\n\t\t\t\t}\n",
        "\t\t\t\t\tm_uUploads += (unsigned)m_jxPalUploads.size();\n\t\t\t\t\tjxK.dChepPalLenh = JxVeMs(uPl0, SDL_GetPerformanceCounter());\n\t\t\t\t}\n"
        "\t\t\t\tif (m_pJxPsBuf && !m_jxPsBang.empty() && m_uJxPsStageOff != 0xFFFFFFFFu)\n"
        "\t\t\t\t{\t// " + DAU + " ca bang to hop trang thai tang texture cua khung (vai chuc muc x 80 byte)\n"
        "\t\t\t\t\tSDL_GPUTransferBufferLocation src = { m_pTexXfer, m_uJxPsStageOff }; SDL_GPUBufferRegion dst = { m_pJxPsBuf, 0, (Uint32)(m_jxPsBang.size() * sizeof(RgPsCb)) };\n"
        "\t\t\t\t\tSDL_UploadToGPUBuffer(cp, &src, &dst, false);\n"
        "\t\t\t\t\tm_uUploads++;\n"
        "\t\t\t\t}\n",
        "tai bang ps")
    # chep bang ps vao staging: ngay truoc khoi bang mau (de nam trong m_texStage cua khung)
    s = thay1(s, "\t\tsize_t uJxTexTruocPal = m_texUploads.size(); Uint64 uJxPalT0 = 0;\t// [PALBUF 11/09] muc tu day tro di trong m_texUploads la hang bang mau (kieu texture cu) -> do rieng\n",
        "\t\tm_uJxPsStageOff = 0xFFFFFFFFu;\n"
        "\t\tif (m_pJxPsBuf && !m_jxPsBang.empty())\n"
        "\t\t{\t// " + DAU + " bang ps di chung staging cua khung (nhu bang mau)\n"
        "\t\t\tconst UINT off = ((UINT)m_texStage.size() + 15) & ~15u;\n"
        "\t\t\tconst UINT bytes = (UINT)(m_jxPsBang.size() * sizeof(RgPsCb));\n"
        "\t\t\tm_texStage.resize((size_t)off + bytes);\n"
        "\t\t\tmemcpy(&m_texStage[off], &m_jxPsBang[0], bytes);\n"
        "\t\t\tm_uJxPsStageOff = off;\n"
        "\t\t}\n"
        "\t\tsize_t uJxTexTruocPal = m_texUploads.size(); Uint64 uJxPalT0 = 0;\t// [PALBUF 11/09] muc tu day tro di trong m_texUploads la hang bang mau (kieu texture cu) -> do rieng\n",
        "chep bang ps")
    # dieu kien khoi tai: co bang ps cung phai chay
    s = thay1(s, "\t\tif ((!m_texUploads.empty() || !m_jxPalUploads.empty()) && !m_texStage.empty())\t// [PALBUF 11/09] hang bang mau (storage buffer) cung tai tu staging nay\n",
        "\t\tif ((!m_texUploads.empty() || !m_jxPalUploads.empty() || m_uJxPsStageOff != 0xFFFFFFFFu) && !m_texStage.empty())\t// [PALBUF 11/09] hang bang mau (storage buffer) cung tai tu staging nay; " + DAU + " ca bang ps\n",
        "dieu kien tai")
    # bind: ring mot lan + hai storage buffer
    s = thay1(s, "\t\t\tSDL_GPUBufferBinding bd = { m_pDummy, 0 }; SDL_BindGPUVertexBuffers(pass, 1, &bd, 1);\n"
                 "#ifdef JX_ANDROID\n"
                 "\t\t\tif (g_nJxPalBuffer && m_pJxPalBuf) SDL_BindGPUFragmentStorageBuffers(pass, 0, &m_pJxPalBuf, 1);\t// [PALBUF 11/09] bang mau = storage buffer (set 2, binding 2, sau 2 sampler) - mot lan moi pass\n"
                 "#endif\n",
        "\t\t\tSDL_GPUBufferBinding bd = { m_pDummy, 0 }; SDL_BindGPUVertexBuffers(pass, 1, &bd, 1);\n"
        "#ifdef JX_ANDROID\n"
        "\t\t\tif (g_nJxBindRing && m_pRingGpu)\n"
        "\t\t\t{\t// " + DAU + " ring dinh bind MOT lan moi pass; lenh ve dung first_vertex (ringOff da can theo stride+4)\n"
        "\t\t\t\tSDL_GPUBufferBinding bdR = { m_pRingGpu, 0 }; SDL_BindGPUVertexBuffers(pass, 0, &bdR, 1);\n"
        "\t\t\t}\n"
        "\t\t\tif (g_nJxPalBuffer && m_pJxPalBuf)\n"
        "\t\t\t{\t// [PALBUF 11/09] bang mau (set 2, binding 2, sau 2 sampler); " + DAU + " bang ps (binding 3) - mot lan moi pass\n"
        "\t\t\t\tSDL_GPUBuffer* aBuf[2] = { m_pJxPalBuf, m_pJxPsBuf };\n"
        "\t\t\t\tSDL_BindGPUFragmentStorageBuffers(pass, 0, aBuf, (m_pJxPsBuf && g_nJxPsBuffer) ? 2 : 1);\n"
        "\t\t\t}\n"
        "#endif\n",
        "bind dau pass")
    # khong day uniform ps khi ps theo dinh
    s = thay1(s, "\t\tif (!bLast || memcmp(&st.ps, &last.ps, sizeof(st.ps)) != 0)\n\t\t{\n\t\t\tSDL_PushGPUFragmentUniformData(cb, 0, &st.ps, sizeof(st.ps));\n",
        "#ifdef JX_ANDROID\n"
        "\t\tif (!g_nJxPsBuffer && (!bLast || memcmp(&st.ps, &last.ps, sizeof(st.ps)) != 0))\t// " + DAU + " ps theo dinh: shader doc tu bang, khong day uniform\n"
        "#else\n"
        "\t\tif (!bLast || memcmp(&st.ps, &last.ps, sizeof(st.ps)) != 0)\n"
        "#endif\n"
        "\t\t{\n\t\t\tSDL_PushGPUFragmentUniformData(cb, 0, &st.ps, sizeof(st.ps));\n",
        "push ps uniform")
    # draw: first_vertex
    s = thay1(s, "\t\tSDL_GPUBufferBinding vb = { m_pRingGpu, c.ringOff };\n\t\tSDL_BindGPUVertexBuffers(pass, 0, &vb, 1);\n\t\tSDL_DrawGPUPrimitives(pass, c.nVerts, 1, 0, 0);\n",
        "#ifdef JX_ANDROID\n"
        "\t\tif (g_nJxBindRing) SDL_DrawGPUPrimitives(pass, c.nVerts, 1, c.ringOff / (c.stride + 4), 0);\t// " + DAU + " ring da bind dau pass\n"
        "\t\telse\n"
        "#endif\n"
        "\t\t{\n"
        "\t\t\tSDL_GPUBufferBinding vb = { m_pRingGpu, c.ringOff };\n"
        "\t\t\tSDL_BindGPUVertexBuffers(pass, 0, &vb, 1);\n"
        "\t\t\tSDL_DrawGPUPrimitives(pass, c.nVerts, 1, 0, 0);\n"
        "\t\t}\n",
        "draw first_vertex")
    # FrameReset: don bang ps
    s = thay1(s, "\tm_jxPalUploads.clear();\t// [PALBUF 11/09]\n",
        "\tm_jxPalUploads.clear();\t// [PALBUF 11/09]\n"
        "\tm_jxPsBang.clear(); m_jxPsMap.clear(); m_uJxPsCuoi = 0xFFFFFFFFu; m_uJxPsStageOff = 0xFFFFFFFFu;\t// " + DAU + " bang ps theo tung khung\n",
        "FrameReset")
    # PalRelease: tra buffer ps
    s = thay1(s, "\tif (m_pJxPalBuf && m_pGpu) SDL_ReleaseGPUBuffer(m_pGpu, m_pJxPalBuf);\n\tm_pJxPalBuf = NULL; m_jxPalUploads.clear();\t// [PALBUF 11/09]\n",
        "\tif (m_pJxPalBuf && m_pGpu) SDL_ReleaseGPUBuffer(m_pGpu, m_pJxPalBuf);\n\tm_pJxPalBuf = NULL; m_jxPalUploads.clear();\t// [PALBUF 11/09]\n"
        "\tif (m_pJxPsBuf && m_pGpu) SDL_ReleaseGPUBuffer(m_pGpu, m_pJxPsBuf);\n"
        "\tm_pJxPsBuf = NULL; m_jxPsBang.clear(); m_jxPsMap.clear(); m_uJxPsCuoi = 0xFFFFFFFFu;\t// " + DAU + "\n",
        "PalRelease")
    # DrawInternal: goi PALROW + can ring + gop bo qua ps
    s = thay1(s, "\tconst UINT s2 = stride + 4;\t// + PALROW\n\tconst UINT uPal = (m_tex[0] && m_tex[0]->m_nPalRow >= 0) ? (UINT)m_tex[0]->m_nPalRow : 0xFFFFu;\n",
        "\tconst UINT s2 = stride + 4;\t// + PALROW\n"
        "\tUINT uPal = (m_tex[0] && m_tex[0]->m_nPalRow >= 0) ? (UINT)m_tex[0]->m_nPalRow : 0xFFFFu;\n",
        "uPal")
    s = thay1(s, "\tRgDrawState st; ComputeState(st, topo);\t// (PrepareForBind o day: texture ao da co cho trong trang truoc khi doi uv)\n",
        "\tRgDrawState st; ComputeState(st, topo);\t// (PrepareForBind o day: texture ao da co cho trong trang truoc khi doi uv)\n"
        "#ifdef JX_ANDROID\n"
        "\tif (g_nJxPsBuffer)\n"
        "\t{\t// " + DAU + " o PALROW: bit 0..12 hang bang mau (0x1FFF = khong co), 13..24 chi so to hop ps, 25..30 danh cho lop atlas (buoc sau)\n"
        "\t\tconst UINT uRow = (m_tex[0] && m_tex[0]->m_nPalRow >= 0) ? ((UINT)m_tex[0]->m_nPalRow & 0x1FFFu) : 0x1FFFu;\n"
        "\t\tuPal = uRow | ((JxPsIdx(st.ps) & 0xFFFu) << 13);\n"
        "\t}\n"
        "#endif\n",
        "goi PALROW")
    s = thay1(s, "\t// dinh -> ring\n\tconst UINT ringOff = (UINT)m_ring.size();\n",
        "\t// dinh -> ring\n"
        "\tUINT ringOff = (UINT)m_ring.size();\n"
        "#ifdef JX_ANDROID\n"
        "\tif (g_nJxBindRing)\n"
        "\t{\t// " + DAU + " can len boi (stride+4) de first_vertex = ringOff / (stride+4) chia het (vai byte dem moi khi doi stride)\n"
        "\t\tconst UINT du = ringOff % s2;\n"
        "\t\tif (du) { ringOff += s2 - du; m_ring.resize(ringOff); }\n"
        "\t}\n"
        "#endif\n",
        "can ring")
    # gop: bo qua ps khi ps theo dinh
    s = thay1(s, "\t\t\tif (L.type == RGCMD_DRAW && L.stride == stride && L.ringOff + L.nVerts * s2 == ringOff && memcmp(&L.st, &st, sizeof(st)) == 0)\n\t\t\t{ L.nVerts += 6; return D3D_OK; }\n",
        "#ifdef JX_ANDROID\n"
        "\t\t\t// " + DAU + " ps theo dinh -> khong so phan ps khi gop (hai quad chi khac trang thai tang texture van gop duoc)\n"
        "\t\t\tconst size_t nSo = g_nJxPsBuffer ? offsetof(RgDrawState, ps) : sizeof(st);\n"
        "#else\n"
        "\t\t\tconst size_t nSo = sizeof(st);\n"
        "#endif\n"
        "\t\t\tif (L.type == RGCMD_DRAW && L.stride == stride && L.ringOff + L.nVerts * s2 == ringOff && memcmp(&L.st, &st, nSo) == 0)\n\t\t\t{ L.nVerts += 6; return D3D_OK; }\n",
        "gop quad")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUi.h: them m_uJxPsStageOff
p = os.path.join(R3, "D3D9onGPUi.h")
s, nl, cao = doc(p)
if "m_uJxPsStageOff" in s:
    print("da co m_uJxPsStageOff:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "\tSDL_GPUBuffer* m_pJxPsBuf; std::vector<RgPsCb> m_jxPsBang; std::map<unsigned long long, UINT> m_jxPsMap; RgPsCb m_jxPsCuoi; UINT m_uJxPsCuoi;",
        "\tSDL_GPUBuffer* m_pJxPsBuf; std::vector<RgPsCb> m_jxPsBang; std::map<unsigned long long, UINT> m_jxPsMap; RgPsCb m_jxPsCuoi; UINT m_uJxPsCuoi; UINT m_uJxPsStageOff;",
        "them m_uJxPsStageOff")
    ghi(p, s, nl, cao)

# ============================================================ Rep3ShadersGPU.frag
p = os.path.join(R3, "Rep3ShadersGPU.frag")
s, nl, cao = doc(p)
if "JX_PS_BUFFER" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "layout(set = 3, binding = 0) uniform PSCB\n{\n"
                 "    ivec4 g_st0;   // stage 0: colorOp, colorArg1, colorArg2, alphaOp   (D3DTOP_* / D3DTA_*)\n"
                 "    ivec4 g_st0b;  // stage 0: alphaArg1, alphaArg2, tex0 bound, loc tuyen tinh (1) -> tu noi suy texture bang mau\n"
                 "    ivec4 g_st1;   // stage 1: colorOp, colorArg1, colorArg2, alphaOp\n"
                 "    ivec4 g_st1b;  // stage 1: alphaArg1, alphaArg2, tex1 bound, 0\n"
                 "    vec4  g_at;    // x = alpha test bat, y = D3DCMP_*, z = alpha ref (0..255), w = 0\n"
                 "};\n",
        "#ifdef JX_PS_BUFFER\n"
        "// [GOP 11/09] (chi Android, -DJX_PS_BUFFER) trang thai tang texture cua CA KHUNG nam trong storage buffer; moi dinh mang chi so\n"
        "// trong o PALROW (bit 13..24) -> hai quad chi khac trang thai van gop chung mot lenh ve. set 2 binding 3 = sau bang mau.\n"
        "struct JxPsRec { ivec4 st0; ivec4 st0b; ivec4 st1; ivec4 st1b; vec4 at; };\n"
        "layout(std430, set = 2, binding = 3) readonly buffer PsBuf { JxPsRec g_psBuf[]; };\n"
        "#else\n"
        "layout(set = 3, binding = 0) uniform PSCB\n{\n"
        "    ivec4 g_st0;   // stage 0: colorOp, colorArg1, colorArg2, alphaOp   (D3DTOP_* / D3DTA_*)\n"
        "    ivec4 g_st0b;  // stage 0: alphaArg1, alphaArg2, tex0 bound, loc tuyen tinh (1) -> tu noi suy texture bang mau\n"
        "    ivec4 g_st1;   // stage 1: colorOp, colorArg1, colorArg2, alphaOp\n"
        "    ivec4 g_st1b;  // stage 1: alphaArg1, alphaArg2, tex1 bound, 0\n"
        "    vec4  g_at;    // x = alpha test bat, y = D3DCMP_*, z = alpha ref (0..255), w = 0\n"
        "};\n"
        "#endif\n",
        "khai bao PSCB")
    # MACRO (khong phai bien cuc bo): nhanh KHONG-JX_PS_BUFFER sinh ra DUNG chuoi token cu -> mang shader cua PC
    # (g_Rep3GpuFS) va cua ban 109111459 (g_Rep3GpuFSPalBuf) giu nguyen TUNG BYTE.
    s = thay1(s, "layout(location = 0) out vec4 outColor;\n",
        "#ifdef JX_PS_BUFFER\n"
        "#define JX_PALROW   (vPal & 0x1FFFu)   // " + DAU + " o PALROW: bit 0..12 = hang bang mau, 13..24 = chi so to hop trang thai tang texture\n"
        "#define JX_PALKHONG 0x1FFFu\n"
        "#else\n"
        "#define JX_PALROW   vPal\n"
        "#define JX_PALKHONG 0xFFFFu\n"
        "#endif\n"
        "\n"
        "layout(location = 0) out vec4 outColor;\n",
        "macro PALROW")
    s = thay1(s, "void main()\n{\n    vec4 dif = vCol;\n",
        "void main()\n{\n"
        "#ifdef JX_PS_BUFFER\n"
        "    // " + DAU + " trang thai tang texture lay tu bang cua khung theo chi so mang tren dinh (khong con uniform moi lenh ve)\n"
        "    JxPsRec jxR = g_psBuf[(vPal >> 13) & 0xFFFu];\n"
        "    ivec4 g_st0 = jxR.st0; ivec4 g_st0b = jxR.st0b; ivec4 g_st1 = jxR.st1; ivec4 g_st1b = jxR.st1b; vec4 g_at = jxR.at;\n"
        "#endif\n"
        "    vec4 dif = vCol;\n",
        "main dau")
    s = thay1(s, "        if (vPal != 0xFFFFu && g_st0b.z != 0)\n", "        if (JX_PALROW != JX_PALKHONG && g_st0b.z != 0)\n", "dieu kien pal")
    n = s.count(", vPal);\n")
    if n != 5:
        raise SystemExit("PalTex(..., vPal) khop %d cho (can 5)" % n)
    s = s.replace(", vPal);\n", ", JX_PALROW);\n")
    ghi(p, s, nl, cao)

# ============================================================ dich_shader_gpu.py: them bien the pal+ps
p = DICH
s, nl, cao = doc(p)
if "g_Rep3GpuFSPalPs" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, '("frag", "g_Rep3GpuFSPalBuf", ["-DJX_PAL_BUFFER=1"])):',
        '("frag", "g_Rep3GpuFSPalBuf", ["-DJX_PAL_BUFFER=1"]), ("frag", "g_Rep3GpuFSPalPs", ["-DJX_PAL_BUFFER=1", "-DJX_PS_BUFFER=1"])):',
        "them bien the")
    ghi(p, s, nl, cao)

# ============================================================ config.ini lop ghi de
p = CFG
s, nl, cao = doc(p)
if "Rep3PsBuffer" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "Rep3SwapchainLogic=100\n",
        "Rep3SwapchainLogic=100\n"
        "; " + DAU + " Rep3PsBuffer=1: trang thai tang texture (colorop/alphaop/alpha test) cua ca khung nam trong mot storage buffer, chi so di theo DINH\n"
        ";   -> khong con ~800 lan day uniform moi khung va hai quad chi khac trang thai van gop chung mot lenh ve. 0 = nhu ban 109111459.\n"
        "Rep3PsBuffer=1\n"
        "; " + DAU + " Rep3BindRing=1: bind bo dem dinh MOT lan moi render pass, moi lenh ve dung first_vertex -> bot 1 lenh Vulkan cho moi draw\n"
        ";   (Tong Kim 1 818 draw/khung). 0 = bind rieng tung lenh nhu cu. Xem [VE-GOP] \"ps bang N muc\" trong jx_rep3.log.\n"
        "Rep3BindRing=1\n",
        "config Rep3SwapchainLogic")
    ghi(p, s, nl, cao)

print("xong - tiep: python ReverseTools\\mobile_x64\\dich_shader_gpu.py \"%s\"" % GOC)
