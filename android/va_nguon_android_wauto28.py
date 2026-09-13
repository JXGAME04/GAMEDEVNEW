# -*- coding: utf-8 -*-
#
# [WAUTO 12/09] SUA HAI CHUOI DUNG WINDOWS - loi KHONG phai cua dot WAuto nay.
#
# Commit 1a400b7e "[BAOMAT 12/09] Ba viec bao mat cho iOS" them khoi doc [Client] NhatKyChanDoan vao
# Sources/S3Client/S3Client.cpp. Khoi do lam GAY CA HAI chuoi Windows (ban Android van dung duoc vi moi thu
# la .so dung chung, khong co bien gioi DLL va co san SDL):
#
#  1. Release|x64 (client PC phat hanh, KHONG co SDL): goi SDL_Log
#     -> error C3861: 'SDL_Log': identifier not found
#     Sua: rao dong do bang #ifdef JX_PLATFORM_SDL. Ban PC truoc do khong dich duoc nen khong mat hanh vi nao.
#
#  2. ReleaseSDL|x64 (GameSDL.exe): "extern int g_nJxNhatKyChanDoan;" tro toi bien nam trong Engine.dll nhung
#     bien do KHONG co ENGINE_API (khong xuat khoi DLL)
#     -> error LNK2001: unresolved external symbol "int g_nJxNhatKyChanDoan"
#     Sua: them ham XUAT g_SetNhatKyChanDoan(int) trong KDebug (cung kieu voi g_DebugLog san co) va goi ham do.
#     Khong xuat thang bien: xuat du lieu qua bien gioi DLL de hong hon ham.
#
# Gui lai phien [BAOMAT] soi cheo theo luat "soi cheo ma dung chung" (12/09). Chay lai vo hai.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[WAUTO 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay1(p, cu, moi, ten):
    s = doc(p)
    if s.count(cu) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(cu), ten))
    ghi(p, s.replace(cu, moi))
    print("   da va:", ten)


def main():
    # 1. SDL_Log trong cau hinh khong co SDL
    P = "Sources/S3Client/S3Client.cpp"
    if "SDL_Log chi co trong ban SDL" in doc(P):
        print("   bo qua (da va): S3Client.cpp SDL_Log")
    else:
        thay1(P,
              '\t\tif (!nNk) SDL_Log("[BAOMAT] nhat ky chan doan: TAT ([Client] NhatKyChanDoan=0)");',
              '\t\t// %s SDL_Log chi co trong ban SDL; cau hinh Release|x64 (client PC phat hanh) khong co SDL\r\n' % DAU +
              '\t\t// -> error C3861. Xem android/va_nguon_android_wauto28.py.\r\n'
              '#ifdef JX_PLATFORM_SDL\r\n'
              '\t\tif (!nNk) SDL_Log("[BAOMAT] nhat ky chan doan: TAT ([Client] NhatKyChanDoan=0)");\r\n'
              '#endif',
              "S3Client.cpp: rao SDL_Log")

    # 2. bien cua Engine.dll khong xuat -> them ham xuat
    P = "Sources/Engine/Src/KDebug.h"
    if "g_SetNhatKyChanDoan" in doc(P):
        print("   bo qua (da va): KDebug.h")
    else:
        thay1(P,
              "ENGINE_API void g_DebugLog(LPSTR Fmt, ...);",
              "ENGINE_API void g_DebugLog(LPSTR Fmt, ...);\r\n"
              "// %s [BAOMAT 12/09] cong tac tat nhat ky chan doan. Phai la HAM XUAT: bien g_nJxNhatKyChanDoan nam\r\n" % DAU +
              "// trong Engine.dll, S3Client.exe khong thay duoc neu chi khai \"extern int\" (LNK2001).\r\n"
              "ENGINE_API void g_SetNhatKyChanDoan(int bBat);",
              "KDebug.h: khai g_SetNhatKyChanDoan")

    P = "Sources/Engine/Src/KDebug.cpp"
    if "g_SetNhatKyChanDoan" in doc(P):
        print("   bo qua (da va): KDebug.cpp")
    else:
        thay1(P,
              "int g_nJxNhatKyChanDoan = 1;",
              "int g_nJxNhatKyChanDoan = 1;\r\n"
              "void g_SetNhatKyChanDoan(int bBat)\t// %s ham XUAT cho ben ngoai Engine.dll (S3Client)\r\n" % DAU +
              "{\r\n\tg_nJxNhatKyChanDoan = bBat ? 1 : 0;\r\n}",
              "KDebug.cpp: g_SetNhatKyChanDoan")

    P = "Sources/S3Client/S3Client.cpp"
    if "g_SetNhatKyChanDoan" in doc(P):
        print("   bo qua (da va): S3Client.cpp goi ham")
    else:
        thay1(P,
              "\t\textern int g_nJxNhatKyChanDoan;\r\n"
              "\t\tint nNk = 1; IniFile.GetInteger(\"Client\", \"NhatKyChanDoan\", 1, &nNk);\r\n"
              "\t\tg_nJxNhatKyChanDoan = nNk ? 1 : 0;",
              "\t\tint nNk = 1; IniFile.GetInteger(\"Client\", \"NhatKyChanDoan\", 1, &nNk);\r\n"
              "\t\tg_SetNhatKyChanDoan(nNk);\t// %s qua ham XUAT cua Engine (bien khong xuat khoi DLL)" % DAU,
              "S3Client.cpp: goi g_SetNhatKyChanDoan")
    print("xong wauto28")


if __name__ == "__main__":
    main()
