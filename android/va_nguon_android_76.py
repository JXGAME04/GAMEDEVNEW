# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 c] Khoi dong lai ngay sau khi app bi dong -> "GameInit that bai" 1-2 lan roi Android tu mo lai moi vao duoc.
# Do that (logcat 06:27:42, 06:27:45, 06:28:30, 06:28:33): jx_rep3.log cua lan hong chi co
#   "[REP3] Rep3Api=11 nhung khong tao duoc D3D11 -> lui ve D3D9"
# tuc Rep3Ini("Rep3Api", 11) KHONG doc duoc config.ini (mac dinh 11) trong khi config.ini co Rep3Api=100; lan mo sau doc duoc.
# Tren Android chi co MOT bo ve chay duoc (SDL_GPU = Rep3Api 100): doc tu config.ini la vo nghia va mong manh.
# 1. KRepresentShell3.cpp: JX_ANDROID -> g_nRep3Api = 100 luon (khong doc ini).
# 2. Lop JX_POSIX: jx_ini_get mo tep hong -> ghi mot dong nhat ky [INI] (ten tep + errno), toi da 20 dong, de lan sau co so lieu.
# Windows khong doi.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 c]"


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


def va_shell3(s):
    return thay(s, ['\tg_nRep3Api       = Rep3Ini("Rep3Api", 11);'],
                ['\tg_nRep3Api       = Rep3Ini("Rep3Api", 11);',
                 '#ifdef JX_ANDROID',
                 '\tg_nRep3Api = 100;\t// %s Android chi co SDL_GPU; doc ini co luc hong (khoi dong lai ngay sau khi dong app) -> "Rep3Api=11 ... lui ve D3D9" -> GameInit that bai' % DAU,
                 '#endif'], "KRepresentShell3.cpp: doc Rep3Api")


def va_shim(s):
    return thay(s, ['\tFILE* f = jx_fopen(file, "rb"); if (!f) return FALSE;',
                    '\tchar line[2048]; int in = 0; BOOL found = FALSE;'],
                   ['\tFILE* f = jx_fopen(file, "rb");',
                    '\tif (!f)',
                    '\t{\t/* %s mo ini hong -> ghi errno (toi da 20 dong) de biet vi sao config.ini co luc khong doc duoc */' % DAU,
                    '\t\tstatic int s_nIniLoi = 0;',
                    '\t\tif (s_nIniLoi < 20) { s_nIniLoi++; jx_log("[INI] khong mo duoc %s: errno %d (%s)", file, errno, strerror(errno)); }',
                    '\t\treturn FALSE;',
                    '\t}',
                    '\tchar line[2048]; int in = 0; BOOL found = FALSE;'], "KPosixWin32.cpp: jx_ini_get fopen")


va("Sources/Represent/Represent3/KRepresentShell3.cpp", va_shell3)
va("Sources/Engine/Src/Platform/KPosixWin32.cpp", va_shim)
print("xong")
