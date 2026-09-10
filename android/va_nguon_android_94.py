# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 MANG] Gop main (CHUGIU b, CAY c, GOP do, MANG a-e) - phien PC bao truoc hai cho Android se vo, va dung:
#   1. [CPU 09/09 do] trong KRepresentShell3.cpp goi GetProcessTimes / GetThreadTimes - lop tuong thich JX_POSIX chua co.
#      Bo sung vao lop tuong thich (KHONG sua nguon game) de lan gop sau khong phai xu ly lai:
#        GetProcessTimes  -> /proc/self/stat (utime + stime), don vi 100 ns nhu Windows.
#        GetThreadTimes   -> clock_gettime(CLOCK_THREAD_CPUTIME_ID).
#   2. Cac bo dem thong ke moi dinh nghia trong D3D9on11Dev.cpp (tep chi Windows, bi loai khoi ban Android):
#        g_uRep3GopVo[12], g_uRep3VeNgay[4], g_uRep3CullGiu, g_uRep3CullBo, g_uRep3RingVong, g_uRep3TexRiengTao,
#        g_dRep3RingMapMax
#      -> them dinh nghia rong (0) vao D3D9on11Stub.cpp, dung cho da co san cac bo dem cu.
# Lop D3D9onGPU (SDL_GPU) cua ban mobile khong bi dung toi.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 MANG]"


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


# ---------------------------------------------------------------- 1. lop tuong thich: hai ham thoi gian CPU
def va_shim_h(s):
    return thay(s, ["BOOL    SystemTimeToFileTime(const SYSTEMTIME* lpSystemTime, LPFILETIME lpFileTime);"],
                ["BOOL    SystemTimeToFileTime(const SYSTEMTIME* lpSystemTime, LPFILETIME lpFileTime);",
                 "BOOL    GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreation, LPFILETIME lpExit, LPFILETIME lpKernel, LPFILETIME lpUser);\t// %s" % DAU,
                 "BOOL    GetThreadTimes(HANDLE hThread, LPFILETIME lpCreation, LPFILETIME lpExit, LPFILETIME lpKernel, LPFILETIME lpUser);"],
                "KPosixWin32.h: khai bao")


def va_shim_cpp(s):
    return thay(s, ["BOOL QueryPerformanceCounter(LARGE_INTEGER* p) { if (p) p->QuadPart = (LONGLONG)SDL_GetPerformanceCounter(); return TRUE; }"],
                ["BOOL QueryPerformanceCounter(LARGE_INTEGER* p) { if (p) p->QuadPart = (LONGLONG)SDL_GetPerformanceCounter(); return TRUE; }",
                 "/* %s thoi gian CPU: Windows tra don vi 100 ns trong FILETIME (kernel = he thong, user = nguoi dung). */" % DAU,
                 "static void jx_dat_ft(LPFILETIME p, unsigned long long u100ns)",
                 "{",
                 "\tif (!p) return;",
                 "\tp->dwLowDateTime  = (DWORD)(u100ns & 0xFFFFFFFFull);",
                 "\tp->dwHighDateTime = (DWORD)(u100ns >> 32);",
                 "}",
                 "BOOL GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreation, LPFILETIME lpExit, LPFILETIME lpKernel, LPFILETIME lpUser)",
                 "{",
                 "\t/* /proc/self/stat: truong 14 utime, 15 stime (don vi tick = 1/CLK_TCK giay) */",
                 "\tunsigned long long ut = 0, st = 0;",
                 "\tlong clk = sysconf(_SC_CLK_TCK);",
                 "\tFILE* f;",
                 "",
                 "\t(void)hProcess;",
                 "\tjx_dat_ft(lpCreation, 0); jx_dat_ft(lpExit, 0);",
                 "\tif (clk <= 0) clk = 100;",
                 "\tf = fopen(\"/proc/self/stat\", \"r\");",
                 "\tif (f)",
                 "\t{",
                 "\t\tchar sz[1024] = { 0 };",
                 "\t\tif (fgets(sz, sizeof(sz) - 1, f))",
                 "\t\t{",
                 "\t\t\tchar* p = strrchr(sz, ')');\t/* bo qua ten tien trinh trong ngoac */",
                 "\t\t\tif (p)",
                 "\t\t\t{",
                 "\t\t\t\tchar* tok = strtok(p + 1, \" \");",
                 "\t\t\t\tint i;",
                 "\t\t\t\tfor (i = 3; tok && i <= 15; i++)",
                 "\t\t\t\t{",
                 "\t\t\t\t\tif (i == 14) ut = strtoull(tok, NULL, 10);",
                 "\t\t\t\t\tif (i == 15) st = strtoull(tok, NULL, 10);",
                 "\t\t\t\t\ttok = strtok(NULL, \" \");",
                 "\t\t\t\t}",
                 "\t\t\t}",
                 "\t\t}",
                 "\t\tfclose(f);",
                 "\t}",
                 "\tjx_dat_ft(lpKernel, st * 10000000ull / (unsigned long long)clk);",
                 "\tjx_dat_ft(lpUser,   ut * 10000000ull / (unsigned long long)clk);",
                 "\treturn TRUE;",
                 "}",
                 "BOOL GetThreadTimes(HANDLE hThread, LPFILETIME lpCreation, LPFILETIME lpExit, LPFILETIME lpKernel, LPFILETIME lpUser)",
                 "{",
                 "\tstruct timespec ts;",
                 "",
                 "\t(void)hThread;",
                 "\tjx_dat_ft(lpCreation, 0); jx_dat_ft(lpExit, 0); jx_dat_ft(lpKernel, 0);",
                 "\tif (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts) != 0)",
                 "\t{",
                 "\t\tjx_dat_ft(lpUser, 0);",
                 "\t\treturn FALSE;",
                 "\t}",
                 "\tjx_dat_ft(lpUser, (unsigned long long)ts.tv_sec * 10000000ull + (unsigned long long)ts.tv_nsec / 100ull);",
                 "\treturn TRUE;",
                 "}"], "KPosixWin32.cpp: hai ham thoi gian CPU")


# ---------------------------------------------------------------- 2. bo dem thong ke moi
def va_stub(s):
    return thay(s, ["unsigned         g_uRep3PalRows = 0;"],
                ["unsigned         g_uRep3PalRows = 0;",
                 "/* %s bo dem moi cua [GOP do] / [MANG a-e] - dinh nghia trong D3D9on11Dev.cpp (chi Windows) */" % DAU,
                 "unsigned         g_uRep3GopVo[12] = { 0 };",
                 "unsigned         g_uRep3VeNgay[4] = { 0 };",
                 "unsigned         g_uRep3CullGiu = 0;",
                 "unsigned         g_uRep3CullBo = 0;",
                 "unsigned         g_uRep3RingVong = 0;",
                 "unsigned         g_uRep3TexRiengTao = 0;",
                 "double           g_dRep3RingMapMax = 0.0;"], "D3D9on11Stub.cpp: bo dem moi")


va("Sources/Engine/Src/Platform/KPosixWin32.h", va_shim_h)
va("Sources/Engine/Src/Platform/KPosixWin32.cpp", va_shim_cpp)
va("Sources/Represent/Represent3/D3D9on11Stub.cpp", va_stub)
print("xong")
