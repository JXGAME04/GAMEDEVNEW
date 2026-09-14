# -*- coding: utf-8 -*-
# [TAI-DO 14/09] Ban DO (chi log, chi JX_MOBILE): log Fold 7 14/09 sau P4 cho thay cung lenh SDL_UploadToGPUTexture, cung transfer buffer:
#   tai CA anh rieng nho (dai nguon 0, 2 MB) = 1,3 ms (0,6 ms/MB) nhung tai VUNG CON 512x512 BGRA8 vao KHOI (2D array 64 MB) = 45-50 ms/o,
#   vao map 24 MB = 518 ms. Chi phi nam o DICH (vung con cua anh lon / dinh dang / nguon vua ghi?) - chua biet co che trong driver Adreno.
# Do 10 kieu tai 0,5-1 MB luc khoi dong thiet bi (truoc dang nhap), moi kieu 3 lan, + anh->anh + buffer->buffer + 3 lan lien tiep khong cho fence
# (map cycle=true nhu duong khung). In [TAI-DO] vao jx_rep3.log. [Client] TaiDo=0 tat. Chu mo app mot lan la du. Chay lai vo hai.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[TAI-DO 14/09]"
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


HAM = [
    "#ifdef JX_MOBILE",
    "// " + DAU + " Do duong tai len GPU luc khoi dong thiet bi (chi log; [Client] TaiDo=0 tat). Log Fold 7 14/09: cung lenh SDL_UploadToGPUTexture",
    "// nhung tai CA anh rieng nho = 0,6 ms/MB, tai VUNG CON 512x512 BGRA8 vao KHOI = 45-50 ms/o -> chi phi o DICH, chua ro co che driver Adreno.",
    "// Moi kieu 3 lan: 'ghi lenh' = CPU trong SDL_UploadToGPUTexture (dung muc do cua [VE-GIAT] lenh tai), 'tong' = tu map toi GPU xong (fence).",
    "struct JxTaiDoKieu { const char* ten; SDL_GPUTextureFormat fmt; UINT bpp; SDL_GPUTextureType loai; UINT nLop; UINT tw, th; UINT lop, x, y, w, h; int nNguon; };",
    "void CDevGpu::JxTaiDo()",
    "{",
    "\textern int g_nJxTaiDo;",
    "\tif (!m_pGpu || !g_nJxTaiDo) return;",
    "\tstatic const JxTaiDoKieu k[] = {",
    "\t\t{ \"vung con 512x512 -> KHOI BGRA8 2048^2 x4 lop (lop 2), nguon vua ghi\", SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, 4, SDL_GPU_TEXTURETYPE_2D_ARRAY, 4, 2048, 2048, 2, 512, 512, 512, 512, 1 },",
    "\t\t{ \"vung con 512x512 -> KHOI BGRA8 (lop 2), nguon CU (ghi mot lan)\",       SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, 4, SDL_GPU_TEXTURETYPE_2D_ARRAY, 4, 2048, 2048, 2, 512, 512, 512, 512, 0 },",
    "\t\t{ \"vung con 512x512 -> anh RIENG BGRA8 2048^2, nguon vua ghi\",             SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, 4, SDL_GPU_TEXTURETYPE_2D, 1, 2048, 2048, 0, 512, 512, 512, 512, 1 },",
    "\t\t{ \"CA ANH 512x512 rieng BGRA8, nguon vua ghi\",                             SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, 4, SDL_GPU_TEXTURETYPE_2D, 1, 512, 512, 0, 0, 0, 512, 512, 1 },",
    "\t\t{ \"CA ANH 512x512 rieng BGRA8, nguon CU\",                                  SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, 4, SDL_GPU_TEXTURETYPE_2D, 1, 512, 512, 0, 0, 0, 512, 512, 0 },",
    "\t\t{ \"CA ANH 2048x128 rieng BGRA8 mang 1 lop (nhu dai nguon 0), nguon vua ghi\", SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, 4, SDL_GPU_TEXTURETYPE_2D_ARRAY, 1, 2048, 128, 0, 0, 0, 2048, 128, 1 },",
    "\t\t{ \"dai 2048x128 -> KHOI BGRA8 (lop 2, y 512), nguon vua ghi\",              SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, 4, SDL_GPU_TEXTURETYPE_2D_ARRAY, 4, 2048, 2048, 2, 0, 512, 2048, 128, 1 },",
    "\t\t{ \"vung con 1024x512 -> KHOI R8G8 2048^2 x8 lop (lop 5), nguon vua ghi\",  SDL_GPU_TEXTUREFORMAT_R8G8_UNORM, 2, SDL_GPU_TEXTURETYPE_2D_ARRAY, 8, 2048, 2048, 5, 512, 512, 1024, 512, 1 },",
    "\t\t{ \"CA ANH 1024x512 rieng R8G8, nguon vua ghi\",                             SDL_GPU_TEXTUREFORMAT_R8G8_UNORM, 2, SDL_GPU_TEXTURETYPE_2D, 1, 1024, 512, 0, 0, 0, 1024, 512, 1 },",
    "\t\t{ \"vung con 512x512 -> KHOI R8G8 (lop 5) 512 KB, nguon vua ghi\",          SDL_GPU_TEXTUREFORMAT_R8G8_UNORM, 2, SDL_GPU_TEXTURETYPE_2D_ARRAY, 8, 2048, 2048, 5, 512, 512, 512, 512, 1 },",
    "\t};",
    "\tconst UINT uBuf = 1u << 20;",
    "\tSDL_GPUTransferBufferCreateInfo ti; memset(&ti, 0, sizeof(ti)); ti.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD; ti.size = uBuf;",
    "\tSDL_GPUTransferBuffer* pX = SDL_CreateGPUTransferBuffer(m_pGpu, &ti);",
    "\tif (!pX) { RgLog(\"[TAI-DO] khong tao duoc transfer buffer: %s\", SDL_GetError()); return; }",
    "\tstd::vector<BYTE> mau(uBuf); for (UINT i = 0; i < uBuf; i++) mau[i] = (BYTE)(i * 7u);",
    "\t{ BYTE* p = (BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, pX, false); if (p) { memcpy(p, &mau[0], uBuf); SDL_UnmapGPUTransferBuffer(m_pGpu, pX); } }",
    "\tconst Uint64 uBatDau = SDL_GetPerformanceCounter();",
    "\tRgLog(\"[TAI-DO] do duong tai len GPU (%s): moi kieu 3 lan; 'ghi lenh' = CPU trong SDL_UploadToGPUTexture, 'tong' = tu map toi GPU xong (fence)\", SDL_GetGPUDeviceDriver(m_pGpu));",
    "\tSDL_GPUTexture* pKhoiBgra = NULL; SDL_GPUTexture* pAnh512 = NULL;",
    "\tfor (size_t i = 0; i < sizeof(k) / sizeof(k[0]); i++)",
    "\t{",
    "\t\tconst JxTaiDoKieu& t = k[i];",
    "\t\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci)); ci.type = t.loai; ci.format = t.fmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;",
    "\t\tci.width = t.tw; ci.height = t.th; ci.layer_count_or_depth = t.nLop; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;",
    "\t\tconst Uint64 uT0 = SDL_GetPerformanceCounter();",
    "\t\tSDL_GPUTexture* pT = SDL_CreateGPUTexture(m_pGpu, &ci);",
    "\t\tconst double dTao = JxVeMs(uT0, SDL_GetPerformanceCounter());",
    "\t\tif (!pT) { RgLog(\"[TAI-DO] %s: tao texture that bai: %s\", t.ten, SDL_GetError()); continue; }",
    "\t\tconst UINT bytes = t.w * t.h * t.bpp;",
    "\t\tdouble dMapMin = 1e9, dLenhMin = 1e9, dLenhMax = 0.0, dTongMin = 1e9, dTongMax = 0.0;",
    "\t\tfor (int r = 0; r < 3; r++)",
    "\t\t{",
    "\t\t\tconst Uint64 u0 = SDL_GetPerformanceCounter();",
    "\t\t\tif (t.nNguon) { BYTE* p = (BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, pX, false); if (p) { memcpy(p, &mau[0], bytes); SDL_UnmapGPUTransferBuffer(m_pGpu, pX); } }",
    "\t\t\tconst Uint64 u1 = SDL_GetPerformanceCounter();",
    "\t\t\tSDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(m_pGpu);",
    "\t\t\tif (!cb) break;",
    "\t\t\tSDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);",
    "\t\t\tconst Uint64 u2 = SDL_GetPerformanceCounter();",
    "\t\t\tSDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = pX; src.pixels_per_row = t.w; src.rows_per_layer = t.h;",
    "\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = pT; dst.layer = t.lop; dst.x = t.x; dst.y = t.y; dst.w = t.w; dst.h = t.h; dst.d = 1;",
    "\t\t\tSDL_UploadToGPUTexture(cp, &src, &dst, false);",
    "\t\t\tconst Uint64 u3 = SDL_GetPerformanceCounter();",
    "\t\t\tSDL_EndGPUCopyPass(cp);",
    "\t\t\tSDL_GPUFence* f = SDL_SubmitGPUCommandBufferAndAcquireFence(cb);",
    "\t\t\tif (f) { SDL_WaitForGPUFences(m_pGpu, true, &f, 1); SDL_ReleaseGPUFence(m_pGpu, f); }",
    "\t\t\tconst Uint64 u4 = SDL_GetPerformanceCounter();",
    "\t\t\tconst double dMap = JxVeMs(u0, u1), dLenh = JxVeMs(u2, u3), dTong = JxVeMs(u1, u4);",
    "\t\t\tif (dMap < dMapMin) dMapMin = dMap; if (dLenh < dLenhMin) dLenhMin = dLenh; if (dLenh > dLenhMax) dLenhMax = dLenh; if (dTong < dTongMin) dTongMin = dTong; if (dTong > dTongMax) dTongMax = dTong;",
    "\t\t}",
    "\t\tRgLog(\"[TAI-DO] %-76s | %4u KB | ghi lenh %6.2f..%6.2f ms (%5.1f ms/MB) | tong %6.2f..%6.2f ms | map+memcpy %.2f | tao texture %.1f ms\", t.ten, bytes >> 10, dLenhMin, dLenhMax, dLenhMin * 1024.0 / (double)(bytes >> 10), dTongMin, dTongMax, dMapMin, dTao);",
    "\t\tif (i == 0) pKhoiBgra = pT; else if (i == 3) pAnh512 = pT; else SDL_ReleaseGPUTexture(m_pGpu, pT);",
    "\t}",
    "\tif (pKhoiBgra && pAnh512)",
    "\t{\t// anh -> anh (GPU): 512x512 tu anh rieng vao khoi lop 2",
    "\t\tdouble dLenhMin = 1e9, dLenhMax = 0.0, dTongMin = 1e9, dTongMax = 0.0;",
    "\t\tfor (int r = 0; r < 3; r++)",
    "\t\t{",
    "\t\t\tconst Uint64 u1 = SDL_GetPerformanceCounter();",
    "\t\t\tSDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(m_pGpu);",
    "\t\t\tif (!cb) break;",
    "\t\t\tSDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);",
    "\t\t\tconst Uint64 u2 = SDL_GetPerformanceCounter();",
    "\t\t\tSDL_GPUTextureLocation s; memset(&s, 0, sizeof(s)); s.texture = pAnh512;",
    "\t\t\tSDL_GPUTextureLocation d; memset(&d, 0, sizeof(d)); d.texture = pKhoiBgra; d.layer = 2; d.x = 1024; d.y = 1024;",
    "\t\t\tSDL_CopyGPUTextureToTexture(cp, &s, &d, 512, 512, 1, false);",
    "\t\t\tconst Uint64 u3 = SDL_GetPerformanceCounter();",
    "\t\t\tSDL_EndGPUCopyPass(cp);",
    "\t\t\tSDL_GPUFence* f = SDL_SubmitGPUCommandBufferAndAcquireFence(cb);",
    "\t\t\tif (f) { SDL_WaitForGPUFences(m_pGpu, true, &f, 1); SDL_ReleaseGPUFence(m_pGpu, f); }",
    "\t\t\tconst double dLenh = JxVeMs(u2, u3), dTong = JxVeMs(u1, SDL_GetPerformanceCounter());",
    "\t\t\tif (dLenh < dLenhMin) dLenhMin = dLenh; if (dLenh > dLenhMax) dLenhMax = dLenh; if (dTong < dTongMin) dTongMin = dTong; if (dTong > dTongMax) dTongMax = dTong;",
    "\t\t}",
    "\t\tRgLog(\"[TAI-DO] %-76s | %4u KB | ghi lenh %6.2f..%6.2f ms | tong %6.2f..%6.2f ms\", \"ANH -> ANH: 512x512 tu anh rieng BGRA8 vao KHOI lop 2 (CopyGPUTextureToTexture)\", 1024u, dLenhMin, dLenhMax, dTongMin, dTongMax);",
    "\t}",
    "\t{\t// buffer -> buffer 1 MB (nhu PALBUF)",
    "\t\tSDL_GPUBufferCreateInfo bi; memset(&bi, 0, sizeof(bi)); bi.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ; bi.size = uBuf;",
    "\t\tSDL_GPUBuffer* pB = SDL_CreateGPUBuffer(m_pGpu, &bi);",
    "\t\tif (pB)",
    "\t\t{",
    "\t\t\tdouble dLenhMin = 1e9, dLenhMax = 0.0, dTongMin = 1e9, dTongMax = 0.0;",
    "\t\t\tfor (int r = 0; r < 3; r++)",
    "\t\t\t{",
    "\t\t\t\t{ BYTE* p = (BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, pX, false); if (p) { memcpy(p, &mau[0], uBuf); SDL_UnmapGPUTransferBuffer(m_pGpu, pX); } }",
    "\t\t\t\tconst Uint64 u1 = SDL_GetPerformanceCounter();",
    "\t\t\t\tSDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(m_pGpu);",
    "\t\t\t\tif (!cb) break;",
    "\t\t\t\tSDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);",
    "\t\t\t\tconst Uint64 u2 = SDL_GetPerformanceCounter();",
    "\t\t\t\tSDL_GPUTransferBufferLocation src = { pX, 0 }; SDL_GPUBufferRegion dst = { pB, 0, uBuf };",
    "\t\t\t\tSDL_UploadToGPUBuffer(cp, &src, &dst, false);",
    "\t\t\t\tconst Uint64 u3 = SDL_GetPerformanceCounter();",
    "\t\t\t\tSDL_EndGPUCopyPass(cp);",
    "\t\t\t\tSDL_GPUFence* f = SDL_SubmitGPUCommandBufferAndAcquireFence(cb);",
    "\t\t\t\tif (f) { SDL_WaitForGPUFences(m_pGpu, true, &f, 1); SDL_ReleaseGPUFence(m_pGpu, f); }",
    "\t\t\t\tconst double dLenh = JxVeMs(u2, u3), dTong = JxVeMs(u1, SDL_GetPerformanceCounter());",
    "\t\t\t\tif (dLenh < dLenhMin) dLenhMin = dLenh; if (dLenh > dLenhMax) dLenhMax = dLenh; if (dTong < dTongMin) dTongMin = dTong; if (dTong > dTongMax) dTongMax = dTong;",
    "\t\t\t}",
    "\t\t\tRgLog(\"[TAI-DO] %-76s | %4u KB | ghi lenh %6.2f..%6.2f ms | tong %6.2f..%6.2f ms\", \"BUFFER -> BUFFER 1 MB (UploadToGPUBuffer, nhu PALBUF)\", 1024u, dLenhMin, dLenhMax, dTongMin, dTongMax);",
    "\t\t\tSDL_ReleaseGPUBuffer(m_pGpu, pB);",
    "\t\t}",
    "\t}",
    "\tif (pKhoiBgra)",
    "\t{\t// 3 lan LIEN TIEP khong cho fence, map cycle=true (nhu duong khung: transfer buffer cua khung truoc con dang chay)",
    "\t\tSDL_GPUFence* fs[3] = { NULL, NULL, NULL };",
    "\t\tfor (int r = 0; r < 3; r++)",
    "\t\t{",
    "\t\t\tconst Uint64 u0 = SDL_GetPerformanceCounter();",
    "\t\t\t{ BYTE* p = (BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, pX, true); if (p) { memcpy(p, &mau[0], uBuf); SDL_UnmapGPUTransferBuffer(m_pGpu, pX); } }",
    "\t\t\tconst Uint64 u1 = SDL_GetPerformanceCounter();",
    "\t\t\tSDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(m_pGpu);",
    "\t\t\tif (!cb) break;",
    "\t\t\tSDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);",
    "\t\t\tconst Uint64 u2 = SDL_GetPerformanceCounter();",
    "\t\t\tSDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = pX; src.pixels_per_row = 512; src.rows_per_layer = 512;",
    "\t\t\tSDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = pKhoiBgra; dst.layer = 2; dst.x = 0; dst.y = (Uint32)r * 512u; dst.w = 512; dst.h = 512; dst.d = 1;",
    "\t\t\tSDL_UploadToGPUTexture(cp, &src, &dst, false);",
    "\t\t\tconst Uint64 u3 = SDL_GetPerformanceCounter();",
    "\t\t\tSDL_EndGPUCopyPass(cp);",
    "\t\t\tfs[r] = SDL_SubmitGPUCommandBufferAndAcquireFence(cb);",
    "\t\t\tRgLog(\"[TAI-DO] LIEN TIEP lan %d (khong cho fence, map cycle=true): vung con 512x512 -> KHOI BGRA8 | map+memcpy %.2f ms | ghi lenh %.2f ms | nop %.2f ms\", r + 1, JxVeMs(u0, u1), JxVeMs(u2, u3), JxVeMs(u3, SDL_GetPerformanceCounter()));",
    "\t\t}",
    "\t\tfor (int r = 0; r < 3; r++) if (fs[r]) { SDL_WaitForGPUFences(m_pGpu, true, &fs[r], 1); SDL_ReleaseGPUFence(m_pGpu, fs[r]); }",
    "\t}",
    "\tSDL_WaitForGPUIdle(m_pGpu);",
    "\tif (pKhoiBgra) SDL_ReleaseGPUTexture(m_pGpu, pKhoiBgra);",
    "\tif (pAnh512) SDL_ReleaseGPUTexture(m_pGpu, pAnh512);",
    "\tSDL_ReleaseGPUTransferBuffer(m_pGpu, pX);",
    "\tRgLog(\"[TAI-DO] xong, tong %.0f ms\", JxVeMs(uBatDau, SDL_GetPerformanceCounter()));",
    "}",
    "#endif",
]


def v_dev(s):
    cu = "unsigned g_uJxTaiTruocSo = 0, g_uJxTaiTruocXong = 0, g_uJxTaiTruocKB = 0, g_uJxTaiTruocLuot = 0, g_uJxTaiTruocMax = 0, g_uJxZeroChep = 0; double g_dJxTaiTruocMs = 0.0, g_dJxZeroChepMs = 0.0;\t// [TAI 14/09] -> [VE-TAI] (KRepresentShell3.cpp)" + NL
    moi = cu + "int g_nJxTaiDo = 1;\t// " + DAU + " [Client] TaiDo: 1 = do duong tai len GPU luc khoi dong thiet bi (chi log, ~1-2 s mot lan)" + NL
    s = thay(s, cu, moi, "g_nJxTaiDo")
    cu = "bool CDevGpu::Init()" + NL + "{" + NL
    s = thay(s, cu, NL.join(HAM) + NL + NL + cu, "JxTaiDo dinh nghia")
    cu = ("\t\tpm == SDL_GPU_PRESENTMODE_IMMEDIATE ? \"ngay\" : (pm == SDL_GPU_PRESENTMODE_MAILBOX ? \"mailbox\" : \"vsync\"), (int)(m_pp.Windowed != FALSE));" + NL
          + "\treturn true;" + NL + "}" + NL)
    moi = ("\t\tpm == SDL_GPU_PRESENTMODE_IMMEDIATE ? \"ngay\" : (pm == SDL_GPU_PRESENTMODE_MAILBOX ? \"mailbox\" : \"vsync\"), (int)(m_pp.Windowed != FALSE));" + NL
           + "#ifdef JX_MOBILE" + NL + "\tJxTaiDo();\t// " + DAU + " do duong tai len GPU (chi log)" + NL + "#endif" + NL
           + "\treturn true;" + NL + "}" + NL)
    s = thay(s, cu, moi, "Init goi JxTaiDo")
    return s


def v_gpui(s):
    cu = "\tvoid    JxTaiTruocChay(UINT uNganSach);\t// moi khung (RepresentBegin): tai toi da uNganSach byte" + NL
    moi = cu + "\tvoid    JxTaiDo();\t\t\t\t\t\t// " + DAU + " do duong tai len GPU luc khoi dong (chi log)" + NL
    return thay(s, cu, moi, "JxTaiDo khai bao")


def v_shell(s):
    cu = "\tg_nJxNapKhungKB    = Rep3Ini(\"NapKhungKB\", 128);\t// [TAI 14/09] ngan sach tai dan khung nap truoc len GPU (KB/khung); 0 = tat (tai ca khung luc ve nhu cu)" + NL
    moi = cu + "\t{ extern int g_nJxTaiDo; g_nJxTaiDo = Rep3Ini(\"TaiDo\", 1); }\t// " + DAU + " 1 = do duong tai len GPU luc khoi dong thiet bi (chi log [TAI-DO], ~1-2 s)" + NL
    return thay(s, cu, moi, "ini TaiDo")


va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", v_dev)
va("Sources/Represent/Represent3/D3D9onGPUi.h", v_gpui)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", v_shell)
