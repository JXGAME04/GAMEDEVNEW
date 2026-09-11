# -*- coding: utf-8 -*-
r"""[D1 11/09] Phia game cho SWAPCHAIN THEO KHUNG LOGIC (chu 11/09 14:55: "lam d1 ngay"; PHUONGAN_FPS_NHIET_PIN_MOBILE_1109.md D1):
  - BaseInclude.h / KRepresentShell3.cpp: [Client] Rep3SwapchainLogic (phan tram, mac dinh 100 = swapchain = backbuffer; 150 = 1,5x; 0 = tat = cua so nhu cu).
  - D3D9onGPUDev.cpp: JxDatHintSwapchain() dat hint JX_SWAPCHAIN_W/H cho SDL (android/va_sdl3_d1.py) TRUOC SDL_ClaimWindowForGPUDevice (Init) va trong
    Reset (gap/mo: backbuffer doi -> SDL_SetGPUSwapchainParameters dung lai swapchain voi hint moi); ghi [D1] vao jx_rep3.log moi khi kich thuoc
    swapchain nhan duoc tu acquire doi (kem JX_SWAPCHAIN_THAT / JX_SWAPCHAIN_EXTENT cua SDL). Letterbox() doc kich thuoc that -> ti le tu ve 1.
  - android/CMakeLists.txt: goi va_sdl3_d1.py sau va_sdl3_donhip.py luc configure.
  - android/du_lieu_ghi_de/config.ini: khoa Rep3SwapchainLogic.
Chi JX_ANDROID; doc/ghi latin-1, giu CRLF/LF, moc khop dung 1 cho, byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_d1.py
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
CFG = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
CMK = os.path.join(GOC, "android", "CMakeLists.txt")


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
if "[D1 11/09]" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^extern int g_nJxBoKhungGiong, g_nJxBoKhungGiongMs;[^\n]*\n",
        "extern int g_nJxSwapchainLogic;\t// [D1 11/09] [Client] Rep3SwapchainLogic: swapchain = backbuffer x %/100 (100 = khung logic, HWC phong len man; 0 = cua so nhu cu)\n",
        "extern BKG")
    ghi(p, s, nl, cao)

# ============================================================ KRepresentShell3.cpp
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p)
if "[D1 11/09]" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^int g_nJxBoKhungGiong = 1, g_nJxBoKhungGiongMs = 250;[^\n]*\n",
        "int g_nJxSwapchainLogic = 100;\t// [D1 11/09]\n", "dinh nghia BKG")
    s = thay1(s, "\tif (g_nJxBoKhungGiong > 1) g_nJxBoKhungGiong = 1; if (g_nJxBoKhungGiong < -1) g_nJxBoKhungGiong = -1;\n",
        "\tif (g_nJxBoKhungGiong > 1) g_nJxBoKhungGiong = 1; if (g_nJxBoKhungGiong < -1) g_nJxBoKhungGiong = -1;\n"
        "\tg_nJxSwapchainLogic = Rep3Ini(\"Rep3SwapchainLogic\", 100);\t// [D1 11/09] swapchain = backbuffer x %/100 (100 = khung logic 1040x936/1436x616, GPU to it diem hon 3-4,4 lan; 150 = 1,5x; 0 = cua so nhu cu)\n"
        "\tif (g_nJxSwapchainLogic < 0) g_nJxSwapchainLogic = 0; if (g_nJxSwapchainLogic > 0 && g_nJxSwapchainLogic < 50) g_nJxSwapchainLogic = 50; if (g_nJxSwapchainLogic > 400) g_nJxSwapchainLogic = 400;\n",
        "doc ini BKG")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUDev.cpp
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p)
if "[D1 11/09]" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "static int s_nJxEpTrinhChieu = 0;\t// [BKG 11/09] 1 = khung ke tiep PHAI trinh chieu (be mat / cua so doi) du giong khung truoc; Present dat lai 0\n",
        "static int s_nJxEpTrinhChieu = 0;\t// [BKG 11/09] 1 = khung ke tiep PHAI trinh chieu (be mat / cua so doi) du giong khung truoc; Present dat lai 0\n"
        "// [D1 11/09] swapchain theo KHUNG LOGIC: hint JX_SWAPCHAIN_W/H cho SDL (android/va_sdl3_d1.py) = backbuffer x Rep3SwapchainLogic/100, khong vuot cua so.\n"
        "// HWC/DPU phong len man (khong them pass GPU, GPU to it diem hon 3-4,4 lan); Letterbox() doc kich thuoc that tu acquire nen ti le tu ve 1.\n"
        "// Bang cua so (may ao 1040x604) hoac Rep3SwapchainLogic=0 -> hint 0 = SDL nhu cu. Goi TRUOC SDL_ClaimWindowForGPUDevice va trong Reset.\n"
        "static UINT s_uJxD1W = 0, s_uJxD1H = 0;\t// kich thuoc swapchain nhan duoc lan gan nhat (ghi [D1] khi doi)\n"
        "static void JxDatHintSwapchain(SDL_Window* pWin, UINT bbW, UINT bbH)\n"
        "{\n"
        "\tchar szW[16], szH[16];\n"
        "\tint pw = 0, ph = 0; if (pWin) SDL_GetWindowSizeInPixels(pWin, &pw, &ph);\n"
        "\tUINT w = 0, h = 0;\n"
        "\tif (g_nJxSwapchainLogic > 0 && bbW && bbH)\n"
        "\t{\n"
        "\t\tw = (UINT)((unsigned long long)bbW * (unsigned)g_nJxSwapchainLogic / 100u); h = (UINT)((unsigned long long)bbH * (unsigned)g_nJxSwapchainLogic / 100u);\n"
        "\t\tif (pw > 0 && w > (UINT)pw) w = (UINT)pw; if (ph > 0 && h > (UINT)ph) h = (UINT)ph;\n"
        "\t\tif (pw > 0 && ph > 0 && w == (UINT)pw && h == (UINT)ph) w = h = 0;\t// bang cua so: khong can hint\n"
        "\t}\n"
        "\tsnprintf(szW, sizeof(szW), \"%u\", w); snprintf(szH, sizeof(szH), \"%u\", h);\n"
        "\tSDL_SetHint(\"JX_SWAPCHAIN_W\", szW); SDL_SetHint(\"JX_SWAPCHAIN_H\", szH);\n"
        "\tRgLog(\"[D1] hint swapchain %ux%u (backbuffer %ux%u, cua so %dx%d px, Rep3SwapchainLogic=%d)\", w, h, bbW, bbH, pw, ph, g_nJxSwapchainLogic);\n"
        "}\n",
        "sau s_nJxEpTrinhChieu")
    s = thay1(s, "\tif (!SDL_ClaimWindowForGPUDevice(m_pGpu, m_pWin)) { RgLog(\"ClaimWindowForGPUDevice that bai: %s\", SDL_GetError()); return false; }\n",
        "#ifdef JX_ANDROID\n"
        "\tJxDatHintSwapchain(m_pWin, m_bbW, m_bbH);\t// [D1 11/09] truoc khi SDL tao swapchain lan dau\n"
        "#endif\n"
        "\tif (!SDL_ClaimWindowForGPUDevice(m_pGpu, m_pWin)) { RgLog(\"ClaimWindowForGPUDevice that bai: %s\", SDL_GetError()); return false; }\n",
        "Init ClaimWindow")
    s = thay1(s, "\tif (m_pBackSurf) { m_pBackSurf->m_w = m_bbW; m_pBackSurf->m_h = m_bbH; }\n\tApplyWindowMode();\n",
        "\tif (m_pBackSurf) { m_pBackSurf->m_w = m_bbW; m_pBackSurf->m_h = m_bbH; }\n\tApplyWindowMode();\n"
        "#ifdef JX_ANDROID\n"
        "\tJxDatHintSwapchain(m_pWin, m_bbW, m_bbH);\t// [D1 11/09] backbuffer doi (gap/mo) -> SDL_SetGPUSwapchainParameters ben duoi dung lai swapchain voi hint moi\n"
        "#endif\n",
        "Reset ApplyWindowMode")
    s = thay1(s, "\t\tJxNhipGhiCho(uJxT0, pSwap != NULL, swW, swH);\t// [DONHIP 12/09]\n",
        "\t\tJxNhipGhiCho(uJxT0, pSwap != NULL, swW, swH);\t// [DONHIP 12/09]\n"
        "\t\tif (pSwap && (swW != s_uJxD1W || swH != s_uJxD1H))\n"
        "\t\t{\t// [D1 11/09] swapchain doi kich thuoc (lan dau / gap-mo / doi hint): ghi de doi chieu backbuffer, cua so, extent SDL bao\n"
        "\t\t\tint pw = 0, ph = 0; SDL_GetWindowSizeInPixels(m_pWin, &pw, &ph);\n"
        "\t\t\tconst char* t = SDL_GetHint(\"JX_SWAPCHAIN_THAT\"); const char* e = SDL_GetHint(\"JX_SWAPCHAIN_EXTENT\");\n"
        "\t\t\tRgLog(\"[D1] swapchain %ux%u | backbuffer %ux%u | cua so %dx%d px | Rep3SwapchainLogic=%d | SDL: tao %s; extent %s\", swW, swH, m_bbW, m_bbH, pw, ph, g_nJxSwapchainLogic, t ? t : \"-\", e ? e : \"-\");\n"
        "\t\t\ts_uJxD1W = swW; s_uJxD1H = swH;\n"
        "\t\t}\n",
        "SubmitFrame acquire")
    ghi(p, s, nl, cao)

# ============================================================ android/CMakeLists.txt (xuong dong lan lon CRLF/LF -> va tren tep tho, giu nguyen)
p = CMK
raw = io.open(p, encoding="latin-1", newline="").read()
if "va_sdl3_d1.py" in raw:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    moc = "  message(STATUS \"va_sdl3_donhip.py: ${JX_VA_SDL_RA}\")"
    if raw.count(moc) != 1:
        raise SystemExit("moc CMake khop %d cho (can 1)" % raw.count(moc))
    i = raw.index(moc) + len(moc)
    nlc = "\r\n" if raw[i:i + 2] == "\r\n" else "\n"
    them = nlc.join([
        "  # [D1 11/09] swapchain theo khung logic (hint JX_SWAPCHAIN_W/H) - SAU va_sdl3_donhip.py (dung ham JX_DemSdl)",
        "  execute_process(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/va_sdl3_d1.py ${JX_SDL3_SRC}",
        "                  RESULT_VARIABLE JX_VA_D1_KQ OUTPUT_VARIABLE JX_VA_D1_RA ERROR_VARIABLE JX_VA_D1_LOI OUTPUT_STRIP_TRAILING_WHITESPACE)",
        "  if(NOT JX_VA_D1_KQ EQUAL 0)",
        "    message(FATAL_ERROR \"va_sdl3_d1.py loi (${JX_VA_D1_KQ}): ${JX_VA_D1_RA} ${JX_VA_D1_LOI}\")",
        "  endif()",
        "  message(STATUS \"va_sdl3_d1.py: ${JX_VA_D1_RA}\")"])
    raw = raw[:i] + nlc + them + raw[i:]
    io.open(p, "w", encoding="latin-1", newline="").write(raw)
    print("da va:", os.path.relpath(p, GOC))

# ============================================================ config.ini lop ghi de
p = CFG
s, nl, cao = doc(p)
if "Rep3SwapchainLogic" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "FpsNgoaiTheGioi=30\n",
        "FpsNgoaiTheGioi=30\n"
        "; [D1 11/09] Rep3SwapchainLogic=100: swapchain (khung GPU to) = khung logic 1040x936 / 1436x616 thay vi cua so 2184x1968 / 2520x1080, man hinh (HWC) phong len\n"
        ";   -> GPU to it diem hon 3-4,4 lan (Fold 7 14:16 Tong Kim: GPU 82-99 % ban). 150 = 1,5x (net hon, ton hon), 0 = cua so nhu cu. [D1] trong jx_rep3.log.\n"
        "Rep3SwapchainLogic=100\n",
        "config FpsNgoaiTheGioi")
    ghi(p, s, nl, cao)
print("xong")
