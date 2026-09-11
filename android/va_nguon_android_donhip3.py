# -*- coding: utf-8 -*-
r"""[DONHIP 12/09 b] Nang SDL Android len 3.2.30 + bang pha moi cho ban do nhip (sau ket qua Fold 7 10/09, BANGIAO_DONHIP_MOBILE_1209.md muc 8).

Chi Android (luat chu: fix mobile khong dong den ban PC): JxPerfHudAndroid.cpp chi dich khi JX_ANDROID va khong nam trong vcxproj nao;
android/CMakeLists.txt, tai_sdl3_src.ps1, du_lieu_ghi_de/config.ini chi dung cho APK. Goi VC ThirdParty\SDL3 3.2.14 cua Windows giu nguyen.

Bon tep, moi moc khop dung 1 cho; doc/ghi latin-1 (nguon TCVN3, CRLF), kiem so byte cao khong doi; chay lai nhieu lan khong sao:
  1. Sources/S3Client/Platform/JxPerfHudAndroid.cpp
     - bang pha moi (SDL 3.2.30 mac dinh khong dung lai swapchain): 0 SDL moi nhip cu | 1 +nhip PC | 2 +bo chep khung (2 bay)
       | 3 +xin 60 Hz PaintFps 60 | 4 1 khung bay (doi chung) | 5 SDL cu (doi chung, hint JX_DUNG_LAI_SUBOPTIMAL=1, khong trong mac dinh)
     - hint JX_BO_QUA_SUBOPTIMAL -> JX_DUNG_LAI_SUBOPTIMAL (dao chieu: 1 = hanh vi 3.2.14)
     - dong ho pha dung khi app ra nen (cach hai vong bom > 2 s -> doi moc, ghi [NHIP-NGHI]); Fold 7 10/09: buoc 1 keo dai 412 s
  2. android/CMakeLists.txt: JX_SDL3_SRC -> SDL3-3.2.30 (khong CACHE), bao loi ro khi chua tai, tu chay va_sdl3_donhip.py truoc add_subdirectory
  3. android/tai_sdl3_src.ps1: -Ver (mac dinh 3.2.30) thay vi lay phien ban tu goi VC Windows
  4. android/du_lieu_ghi_de/config.ini: chu thich bang pha moi ([DoNhip] Bat van 0)

Dung:  python android\va_nguon_android_donhip3.py     (tu cay nay)
"""
import io
import os
import sys

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[DONHIP 12/09 b]"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    return s, nl


def ghi(p, s, cao):
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("so byte cao doi - khong ghi: " + p)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("da va:", os.path.relpath(p, GOC))


def thay(s, nl, cu, moi, ten):
    c = nl.join(cu)
    n = s.count(c)
    if n != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (n, ten))
    return s.replace(c, nl.join(moi))


# ---------------------------------------------------------------- 1. JxPerfHudAndroid.cpp
p = os.path.join(GOC, "Sources", "S3Client", "Platform", "JxPerfHudAndroid.cpp")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    s = thay(s, nl,
        ["struct DnPha { const char* szTen; int nSuaSdl, nFps, nVsync, nSmooth, nChep, nBay, nXinHz; };\t// -2 = giu cau hinh luc mo app",
         "static const DnPha s_aDnPha[] = {",
         "\t{ \"hien tai\",              0,  -2, -2, -2, 1, 2,   0 },",
         "\t{ \"+sua SDL\",              1,  -2, -2, -2, 1, 2,   0 },",
         "\t{ \"+nhip PC\",              1,  -2,  1,  2, 1, 2,   0 },",
         "\t{ \"+bo chep, 1 khung bay\", 1,  -2,  1,  2, 0, 1,   0 },",
         "\t{ \"+xin 120 Hz\",           1, 120,  1,  2, 0, 1, 120 },",
         "\t{ \"+xin 60 Hz\",            1,  60,  1,  2, 0, 1,  60 },",
         "};"],
        ["// " + DAU + " SDL Android = 3.2.30 (co bca30aa: KHONG dung lai swapchain vi VK_SUBOPTIMAL_KHR). nSdlCu = 1 -> hint",
         "// JX_DUNG_LAI_SUBOPTIMAL=1 = hanh vi 3.2.14 (chi de doi chung). Ket qua Fold 7 10/09 (BANGIAO muc 8): bo qua SUBOPTIMAL 45 -> 120 fps",
         "// cung muc dien; nhip PC deu hon chut; 1 khung bay KHONG on dinh (82 -> 55 fps khi CPU nang) -> mac dinh 2 khung bay.",
         "struct DnPha { const char* szTen; int nSdlCu, nFps, nVsync, nSmooth, nChep, nBay, nXinHz; };\t// -2 = giu cau hinh luc mo app",
         "static const DnPha s_aDnPha[] = {",
         "\t{ \"SDL moi, nhip cu\",         0,  -2,  0,  1, 1, 2,   0 },\t// 0: nhu ban phat hanh sau khi nang SDL",
         "\t{ \"+nhip PC\",                 0,  -2,  1,  2, 1, 2,   0 },\t// 1: + PaintVsync=1, PaintSmooth=2 ([NHIP a-e] cua PC)",
         "\t{ \"+bo chep khung, 2 bay\",    0,  -2,  1,  2, 0, 2,   0 },\t// 2: + khong chep swapchain moi khung (M2), van 2 khung bay",
         "\t{ \"+xin 60 Hz, PaintFps 60\",  0,  60,  1,  2, 0, 2,  60 },\t// 3: nac tiet kiem pin (man 120 Hz ha 60)",
         "\t{ \"1 khung bay (doi chung)\",  0,  -2,  1,  2, 0, 1,   0 },\t// 4: nhu 2 nhung 1 khung bay",
         "\t{ \"SDL cu (doi chung)\",       1,  -2,  0,  1, 1, 2,   0 },\t// 5: dung lai swapchain moi khung nhu 3.2.14 (khong trong danh sach mac dinh)",
         "};"], "bang pha")
    s = thay(s, nl,
        ["\tSDL_SetHint(\"JX_BO_QUA_SUBOPTIMAL\", p.nSuaSdl ? \"1\" : \"0\");"],
        ["\tSDL_SetHint(\"JX_DUNG_LAI_SUBOPTIMAL\", p.nSdlCu ? \"1\" : \"0\");\t// " + DAU + " 1 = dung lai swapchain nhu 3.2.14 (doi chung)"],
        "DnApPha hint")
    s = thay(s, nl,
        ["\tSDL_SetHint(\"JX_BO_QUA_SUBOPTIMAL\", \"0\");"],
        ["\tSDL_SetHint(\"JX_DUNG_LAI_SUBOPTIMAL\", \"0\");\t// " + DAU + " tra ve hanh vi SDL 3.2.30"],
        "DnKetThuc hint")
    s = thay(s, nl,
        ["\tGetPrivateProfileString(\"DoNhip\", \"Pha\", \"0,1,2,3,4,5\", sz, sizeof(sz), \".\\\\config.ini\");"],
        ["\tGetPrivateProfileString(\"DoNhip\", \"Pha\", \"0,1,2,3,4\", sz, sizeof(sz), \".\\\\config.ini\");\t// " + DAU + " pha 5 (SDL cu) chi khi ghi ro"],
        "Pha mac dinh")
    s = thay(s, nl,
        ["static Uint64\ts_uDnVeTruoc = 0;"],
        ["static Uint64\ts_uDnVeTruoc = 0;",
         "static unsigned\ts_uDnVongTruoc = 0;\t\t// " + DAU + " lan goi JxDoNhip_Vong truoc: cach > 2 s = app ra nen -> khong tinh vao pha"],
        "khai bao s_uDnVongTruoc")
    s = thay(s, nl,
        ["\tif (s_nDnBat <= 0 || s_nDnXong)",
         "\t\treturn;",
         "\tconst unsigned uNay = (unsigned)SDL_GetTicks();",
         "\tif (s_nDnBuoc < 0)"],
        ["\tif (s_nDnBat <= 0 || s_nDnXong)",
         "\t\treturn;",
         "\tconst unsigned uNay = (unsigned)SDL_GetTicks();",
         "\tif (s_nDnBuoc >= 0 && s_uDnVongTruoc && uNay - s_uDnVongTruoc > 2000)",
         "\t{\t// " + DAU + " app ra nen (SDL chan vong lap) -> doi moc thoi gian, khong tinh vao pha (Fold 7 10/09: buoc 1 keo dai 412 s)",
         "\t\tconst unsigned uNghi = uNay - s_uDnVongTruoc;",
         "\t\ts_uDnPhaLuc += uNghi; s_uDnCuaLuc += uNghi; s_DnC.uLuc += uNghi; s_DnP.uLuc += uNghi;",
         "\t\ts_uDnVeTruoc = 0;",
         "\t\tDnGhi(\"[NHIP-NGHI]\", \"app ra nen %u ms - khong tinh vao pha\", uNghi);",
         "\t}",
         "\ts_uDnVongTruoc = uNay;",
         "\tif (s_nDnBuoc < 0)"],
        "dong ho pha dung khi ra nen")
    ghi(p, s, cao)

# ---------------------------------------------------------------- 2. android/CMakeLists.txt
p = os.path.join(GOC, "android", "CMakeLists.txt")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    s = thay(s, nl,
        ["set(JX_SDL3_SRC ${JX_ROOT}/ThirdParty/SDL3-src/SDL3-3.2.14 CACHE PATH \"nguon SDL3 (tai bang android/tai_sdl3_src.ps1)\")"],
        ["# " + DAU + " SDL Android = 3.2.30 (ban release-3.2.x cuoi, 01/01/2026): co bca30aa (khong dung lai swapchain vi VK_SUBOPTIMAL_KHR",
         "# khi man xoay - Fold 7 45 -> 120 fps, BANGIAO_DONHIP_MOBILE_1209.md muc 8) + hai sua Android khi quay lai tu nen. CHI Android: goi VC",
         "# ThirdParty/SDL3 3.2.14 cua Windows giu nguyen. Khong CACHE de cay cu khong giu duong 3.2.14. Tai bang android/tai_sdl3_src.ps1.",
         "set(JX_SDL3_VER 3.2.30)",
         "set(JX_SDL3_SRC ${JX_ROOT}/ThirdParty/SDL3-src/SDL3-${JX_SDL3_VER})",
         "if(NOT EXISTS ${JX_SDL3_SRC}/CMakeLists.txt)",
         "  message(FATAL_ERROR \"Chua co nguon SDL3 ${JX_SDL3_VER} o ${JX_SDL3_SRC} - chay: powershell -File android/tai_sdl3_src.ps1\")",
         "endif()"], "JX_SDL3_SRC")
    s = thay(s, nl,
        ["  set(SDL_TEST_LIBRARY OFF CACHE BOOL \"\" FORCE)",
         "  add_subdirectory(${JX_SDL3_SRC} SDL3 EXCLUDE_FROM_ALL)"],
        ["  set(SDL_TEST_LIBRARY OFF CACHE BOOL \"\" FORCE)",
         "  # " + DAU + " va bo dem swapchain (hint JX_DEM_SUBOPTIMAL / JX_DEM_DUNG_LAI_SWAPCHAIN / JX_DUNG_LAI_SUBOPTIMAL) vao nguon SDL",
         "  # truoc khi dich: idempotent, chay o MOI cay -> moi APK cung mot libSDL3.so (10/09: APK -m dung o cay chua va, pha 1 vo hieu).",
         "  find_package(Python3 COMPONENTS Interpreter)",
         "  if(NOT Python3_Interpreter_FOUND)",
         "    message(FATAL_ERROR \"Can python de chay android/va_sdl3_donhip.py (va nguon SDL3)\")",
         "  endif()",
         "  execute_process(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/va_sdl3_donhip.py ${JX_SDL3_SRC}",
         "                  RESULT_VARIABLE JX_VA_SDL_KQ OUTPUT_VARIABLE JX_VA_SDL_RA ERROR_VARIABLE JX_VA_SDL_LOI OUTPUT_STRIP_TRAILING_WHITESPACE)",
         "  if(NOT JX_VA_SDL_KQ EQUAL 0)",
         "    message(FATAL_ERROR \"va_sdl3_donhip.py loi (${JX_VA_SDL_KQ}): ${JX_VA_SDL_RA} ${JX_VA_SDL_LOI}\")",
         "  endif()",
         "  message(STATUS \"va_sdl3_donhip.py: ${JX_VA_SDL_RA}\")",
         "  add_subdirectory(${JX_SDL3_SRC} SDL3 EXCLUDE_FROM_ALL)"], "va SDL luc configure")
    ghi(p, s, cao)

# ---------------------------------------------------------------- 3. android/tai_sdl3_src.ps1
p = os.path.join(GOC, "android", "tai_sdl3_src.ps1")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    s = thay(s, nl,
        ["# [ANDROID 08/09] Tai nguon SDL3 (cung phien ban voi goi VC dung san o ThirdParty\\SDL3) vao ThirdParty\\SDL3-src\\SDL3-<ver>"],
        ["# [ANDROID 08/09] Tai nguon SDL3 vao ThirdParty\\SDL3-src\\SDL3-<ver>. " + DAU + " Android dung 3.2.30 (bca30aa + sua quay lai tu nen),",
         "# KHONG con lay phien ban tu goi VC Windows ThirdParty\\SDL3 (van 3.2.14). Phai khop JX_SDL3_VER trong android/CMakeLists.txt."], "chu thich dau")
    s = thay(s, nl,
        ["$root = Split-Path -Parent $PSScriptRoot",
         "$verH = Get-Content \"$root\\ThirdParty\\SDL3\\include\\SDL3\\SDL_version.h\"",
         "$maj = ($verH | Select-String 'define SDL_MAJOR_VERSION\\s+(\\d+)').Matches[0].Groups[1].Value",
         "$min = ($verH | Select-String 'define SDL_MINOR_VERSION\\s+(\\d+)').Matches[0].Groups[1].Value",
         "$mic = ($verH | Select-String 'define SDL_MICRO_VERSION\\s+(\\d+)').Matches[0].Groups[1].Value",
         "$ver = \"$maj.$min.$mic\""],
        ["param([string]$Ver = \"3.2.30\")   # " + DAU + " phien ban SDL cho Android (= JX_SDL3_VER trong android/CMakeLists.txt)",
         "$root = Split-Path -Parent $PSScriptRoot",
         "$ver = $Ver"], "phien ban")
    ghi(p, s, cao)

# ---------------------------------------------------------------- 4. android/du_lieu_ghi_de/config.ini
p = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    s = thay(s, nl,
        [";   Moi pha THEM mot thay doi so voi pha truoc:",
         ";     0 hien tai | 1 +sua SDL (bo qua VK_SUBOPTIMAL_KHR, can android\\va_sdl3_donhip.py) | 2 +nhip PC (PaintVsync=1, PaintSmooth=2)",
         ";     3 +bo chep khung moi khung, 1 khung bay | 4 +xin man 120 Hz, PaintFps=120 | 5 +xin man 60 Hz, PaintFps=60"],
        [";   " + DAU + " SDL Android 3.2.30 (khong dung lai swapchain vi SUBOPTIMAL - Fold 7 45 -> 120 fps). Bang pha moi (Pha=0,1,2,3,4 mac dinh):",
         ";     0 SDL moi, nhip cu (PaintVsync=0, PaintSmooth=1) | 1 +nhip PC (PaintVsync=1, PaintSmooth=2) | 2 +bo chep khung moi khung, van 2 khung bay",
         ";     3 +xin man 60 Hz, PaintFps=60 (nac tiet kiem pin) | 4 1 khung bay (doi chung) | 5 SDL cu: dung lai swapchain moi khung (doi chung, phai ghi ro)",
         ";   Dong ho pha dung khi app ra nen ([NHIP-NGHI]); GiayMoiPha=60, LanLap=2 -> 10 phut."], "chu thich bang pha")
    s = thay(s, nl, ["Pha=0,1,2,3,4,5"], ["Pha=0,1,2,3,4"], "Pha mac dinh config")
    ghi(p, s, cao)

print("xong")
