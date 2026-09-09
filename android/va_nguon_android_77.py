# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 HUD] Bang do hieu nang trong game cho Android (chu: "them hien thi FPS o trong game - Pin - GPU nhu ban
# du an USVOLAM de de biet"). Tep moi Sources/S3Client/Platform/JxPerfHudAndroid.cpp (chi JX_ANDROID) thay 3 stub PerfHud_*.
#  1. D3D9onGPUDev.cpp: giu ten driver / trinh chieu / lenh ve khung truoc vao bien tinh; xuat "C" Rep3_ThongKeGpu(sz, n)
#     (chi JX_ANDROID) cho HUD goi qua GetProcAddress (nhu Rep3_NapTruoc2).
#  2. JxAndroidStubs.cpp: stub PerfHud chi con cho POSIX khong phai Android.
#  3. android/CMakeLists.txt: them JxPerfHudAndroid.cpp vao target main.
#  4. config lop ghi de: PerfHud=1 (bat mac dinh tren Android), PerfHudX/PerfHudY.
# Windows: khong doi (moi thu rao JX_ANDROID).

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 HUD]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


def va_gpudev(s):
    nl = nl_cua(s)
    s = thay(s, ["#include <string.h>"], [
        "#include <string.h>",
        "#ifdef JX_ANDROID",
        "// %s so lieu cho bang do hieu nang trong game (JxPerfHudAndroid.cpp goi Rep3_ThongKeGpu qua GetProcAddress)" % DAU,
        "static char        s_szRep3GpuDriver[32] = \"\";",
        "static const char* s_szRep3GpuTrinhChieu = \"?\";",
        "static unsigned    s_uRep3GpuLenhVe = 0, s_uRep3GpuQuad = 0;",
        "#endif",
    ], "D3D9onGPUDev.cpp: include string.h")
    # ghi ten driver + trinh chieu sau dong log 'thiet bi' (trong Create)
    s = thay(s, ['\tRgLog("thiet bi: driver %s, backbuffer %ux%u, swapchain fmt %d, trinh chieu %s, windowed=%d", SDL_GetGPUDeviceDriver(m_pGpu), m_bbW, m_bbH, (int)m_swapFmt,'],
             ["#ifdef JX_ANDROID",
              "\t{ const char* d = SDL_GetGPUDeviceDriver(m_pGpu); strncpy(s_szRep3GpuDriver, d ? d : \"?\", sizeof(s_szRep3GpuDriver) - 1); }\t// %s" % DAU,
              "\ts_szRep3GpuTrinhChieu = pm == SDL_GPU_PRESENTMODE_IMMEDIATE ? \"ngay\" : (pm == SDL_GPU_PRESENTMODE_MAILBOX ? \"mailbox\" : \"vsync\");",
              "#endif",
              '\tRgLog("thiet bi: driver %s, backbuffer %ux%u, swapchain fmt %d, trinh chieu %s, windowed=%d", SDL_GetGPUDeviceDriver(m_pGpu), m_bbW, m_bbH, (int)m_swapFmt,'],
             "D3D9onGPUDev.cpp: log thiet bi")
    # Reset: cap nhat trinh chieu
    s = thay(s, ['\tRgLog("Reset: %ux%u windowed=%d vsync=%d", m_bbW, m_bbH, (int)(m_pp.Windowed != FALSE), (int)(pm == SDL_GPU_PRESENTMODE_VSYNC));'],
             ["#ifdef JX_ANDROID",
              "\ts_szRep3GpuTrinhChieu = pm == SDL_GPU_PRESENTMODE_IMMEDIATE ? \"ngay\" : (pm == SDL_GPU_PRESENTMODE_MAILBOX ? \"mailbox\" : \"vsync\");\t// %s" % DAU,
              "#endif",
              '\tRgLog("Reset: %ux%u windowed=%d vsync=%d", m_bbW, m_bbH, (int)(m_pp.Windowed != FALSE), (int)(pm == SDL_GPU_PRESENTMODE_VSYNC));'],
             "D3D9onGPUDev.cpp: log Reset")
    # Present: giu so lenh ve cua khung vua xong
    s = thay(s, ["\tm_uDrawCmds = m_uQuads = m_uUploads = 0;"],
             ["#ifdef JX_ANDROID",
              "\ts_uRep3GpuLenhVe = m_uDrawCmds; s_uRep3GpuQuad = m_uQuads;\t// %s" % DAU,
              "#endif",
              "\tm_uDrawCmds = m_uQuads = m_uUploads = 0;"], "D3D9onGPUDev.cpp: Present dat lai dem")
    s = s.rstrip("\r\n") + nl + nl.join([
        "",
        "#ifdef JX_ANDROID",
        "// %s chuoi cho bang do hieu nang: driver | texture GPU | atlas | trinh chieu | lenh ve khung truoc. Tra do dai chuoi." % DAU,
        "extern \"C\" int Rep3_ThongKeGpu(char* sz, int n)",
        "{",
        "\tif (!sz || n < 8) return 0;",
        "\tsnprintf(sz, (size_t)n, \"GPU %s | texture %u MB (%u) | atlas %u trang | %s | lenh ve %u, quad %u\",",
        "\t\ts_szRep3GpuDriver[0] ? s_szRep3GpuDriver : \"?\", (unsigned)(g_uRep3GpuTexBytes >> 20), g_uRep3GpuTexCount, g_uRep3AtlasPages,",
        "\t\ts_szRep3GpuTrinhChieu, s_uRep3GpuLenhVe, s_uRep3GpuQuad);",
        "\tsz[n - 1] = 0;",
        "\treturn (int)strlen(sz);",
        "}",
        "#endif",
        "",
    ])
    return s


def va_stubs(s):
    return thay(s, ["// ---- PerfHud ----",
                    "void PerfHud_SetEnable(int nOn) { (void)nOn; }",
                    "int  PerfHud_IsEnable() { return 0; }",
                    "void PerfHud_Draw(int nPaintFps, int nLogicFps, unsigned int dwPing) { (void)nPaintFps; (void)nLogicFps; (void)dwPing; }"],
                   ["// ---- PerfHud ---- %s Android co bang do rieng (Platform/JxPerfHudAndroid.cpp); stub chi cho POSIX khac" % DAU,
                    "#ifndef JX_ANDROID",
                    "void PerfHud_SetEnable(int nOn) { (void)nOn; }",
                    "int  PerfHud_IsEnable() { return 0; }",
                    "void PerfHud_Draw(int nPaintFps, int nLogicFps, unsigned int dwPing) { (void)nPaintFps; (void)nLogicFps; (void)dwPing; }",
                    "#endif"], "JxAndroidStubs.cpp: stub PerfHud")


def va_cmake(s):
    return thay(s, ["    ${JX_SRC}/S3Client/Platform/JxAndroidStubs.cpp)   # JxReplay / PerfHud / TrayMode (chi Windows)"],
                   ["    ${JX_SRC}/S3Client/Platform/JxAndroidStubs.cpp    # JxReplay / TrayMode (chi Windows)",
                    "    ${JX_SRC}/S3Client/Platform/JxPerfHudAndroid.cpp)  # %s bang do FPS / CPU / RAM / pin / GPU" % DAU],
                   "CMakeLists.txt: nguon main")


def va_config(s):
    return thay(s, ["PerfHud=0"], [
        "; %s 1 = hien bang do FPS / khung ve / CPU / RAM / pin / GPU o giua-tren man hinh (3 dong). 0 = tat. PerfHudX/PerfHudY = vi tri (px), -1 = tu tinh." % DAU,
        "PerfHud=1",
        "PerfHudX=-1",
        "PerfHudY=4"], "config.ini PerfHud")


va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", va_gpudev)
va("Sources/S3Client/Platform/JxAndroidStubs.cpp", va_stubs)
va("android/CMakeLists.txt", va_cmake)
va("android/du_lieu_ghi_de/config.ini", va_config)
print("xong")
