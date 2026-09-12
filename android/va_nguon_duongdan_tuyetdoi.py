# -*- coding: utf-8 -*-
"""[MACOS 11/09 DUONGDANTUYETDOI]
Va g_GetFullPath() trong Sources/Engine/Src/KFilePath.cpp.

Loi goc: tren POSIX, duong dan TUYET DOI bat dau bang '/'. Ban Windows nhan ra duong dan
day du bang o dia ("C:\\") o nhanh phia tren, nen nhanh "duong dan mot phan" chi con
nhan '\\' hoac '/' nghia la "tuong doi theo goc" va ghep them szRootPath vao truoc.
Tren POSIX, mot duong dan tuyet doi that (vi du ket qua cua GetTempFileName = <goc>/tmp/KSG_xxxx.tmp)
cung roi vao nhanh do va bi ghep them szRootPath lan nua -> dai gap doi.
KFile::Create() giu ket qua trong char PathName[260] (MAXPATH) nen khi thu muc du lieu dai
thi tran ngan xep -> __stack_chk_fail -> SIGABRT.

Cach sua: chi tren POSIX, neu lpFileName DA bat dau bang szRootPath thi no da la duong dan
day du roi - chep thang, khong ghep goc lan nua. Duong dan trong game luon tuong doi
("\\Ui\\...") nen khong bao gio khop nhanh moi nay.

Khong dong toi ban Windows: toan bo phan them nam trong #ifdef JX_POSIX, ma Windows khong dinh nghia.
"""
import io, os, sys

TEP = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   'Sources', 'Engine', 'Src', 'KFilePath.cpp')

MOC = b'DUONGDANTUYETDOI'   # dau nhan rieng, khong dung tien to cua dong cu

CU = (b"\tif (lpFileName[0] == '\\\\' || lpFileName[0] == '/')\n"
      b"\t{\n"
      b"\t\tg_StrCpy(lpPathName, szRootPath);\n"
      b"\t\tg_StrCat(lpPathName, lpFileName);\n"
      b"\t\treturn;\n"
      b"\t}\n")

MOI = (b"\tif (lpFileName[0] == '\\\\' || lpFileName[0] == '/')\n"
       b"\t{\n"
       b"#ifdef JX_POSIX\t// [MACOS 11/09 DUONGDANTUYETDOI] tren POSIX dau '/' la duong dan TUYET DOI that.\n"
       b"\t\t// Ban Windows da bat duong dan day du o nhanh \"C:\\\\\" phia tren; POSIX khong co o dia\n"
       b"\t\t// nen duong dan day du roi vao day va bi ghep them szRootPath lan nua -> dai gap doi\n"
       b"\t\t// -> tran char PathName[260] cua KFile::Create/Open/Append -> __stack_chk_fail (SIGABRT).\n"
       b"\t\t// Duong dan trong game luon tuong doi theo goc (\"\\\\Ui\\\\...\") nen khong khop nhanh nay.\n"
       b"\t\t{\n"
       b"\t\t\tsize_t nRoot = strlen(szRootPath);\n"
       b"\t\t\tif (nRoot > 0 && strncmp(lpFileName, szRootPath, nRoot) == 0)\n"
       b"\t\t\t{\n"
       b"\t\t\t\tg_StrCpy(lpPathName, lpFileName);\n"
       b"\t\t\t\treturn;\n"
       b"\t\t\t}\n"
       b"\t\t}\n"
       b"#endif\n"
       b"\t\tg_StrCpy(lpPathName, szRootPath);\n"
       b"\t\tg_StrCat(lpPathName, lpFileName);\n"
       b"\t\treturn;\n"
       b"\t}\n")

def main():
    d = open(TEP, 'rb').read()
    if MOC in d:
        print('da va roi (thay dau nhan %s), khong lam gi' % MOC.decode())
        return 0
    n = d.count(CU)
    if n != 1:
        print('LOI: tim thay %d cho khop, can dung 1' % n); return 1
    open(TEP, 'wb').write(d.replace(CU, MOI))
    print('da va %s' % TEP)
    return 0

sys.exit(main())
