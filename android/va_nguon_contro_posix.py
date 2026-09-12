# -*- coding: utf-8 -*-
"""[MACOS 11/09 CONTROCHUOT]
Va KUiLoadCursor() trong Sources/S3Client/Ui/Elem/UiCursor.cpp.

Tren POSIX, LoadCursorFromFileA() trong KPosixWin32.cpp la HAM RONG: no bo qua tep va luon
tra ve con tro mac dinh cua SDL. Nghia la ca doan "chep tep .cur tu pak ra tep tam roi nap"
o duoi KHONG BAO GIO tao duoc con tro that tren Android / iOS / macOS.

Truoc ban va nay: doan do van chay, nhung that bai o KFile::Create vi g_GetFullPath ghep
szRootPath vao truoc mot duong dan VON DA TUYET DOI (ket qua cua GetTempFileName) -> chuoi
dai gap doi. Tren Android va iOS goc ngan nen chi that bai im lang; tren macOS goc dai 130
ky tu nen 130 + 147 = 277 > MAXPATH(260) -> tran char PathName[260] -> SIGABRT.

Ban va: tren POSIX thoat ngay truoc doan do. Ket qua tra ve GIU NGUYEN nhu truoc
(nResult = false, rhRetCursor = NULL) nen Android va iOS khong doi hanh vi mot chut nao,
chi bot phan viec vo ich. macOS het chet.

Khong dong toi ban Windows: phan them nam trong #ifdef JX_POSIX, Windows khong dinh nghia.
"""
import os, sys

TEP = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   'Sources', 'S3Client', 'Ui', 'Elem', 'UiCursor.cpp')

MOC = b'CONTROCHUOT'

CU = (b"    if (!pszImgFile)\n"
      b"        goto Exit0;\n"
      b"\n"
      b"    nRetCode = GetTempPath(MAX_PATH, szTempPath);\n")

MOI = (b"    if (!pszImgFile)\n"
       b"        goto Exit0;\n"
       b"\n"
       b"#ifdef JX_POSIX\t// [MACOS 11/09 CONTROCHUOT] tren POSIX LoadCursorFromFile() la ham rong\n"
       b"    // (KPosixWin32.cpp): no bo qua tep va luon tra ve con tro mac dinh cua SDL, nen ca doan\n"
       b"    // chep .cur ra tep tam o duoi khong bao gio tao duoc con tro that. Truoc day doan do van\n"
       b"    // chay nhung that bai o KFile::Create; bo han cho khoi ton cong VA khoi dua duong dan\n"
       b"    // tuyet doi vao KFile (tung lam tran PathName[260] -> SIGABRT tren macOS khi goc dai).\n"
       b"    // Ket qua tra ve giu NGUYEN nhu truoc: nResult = false, rhRetCursor = NULL.\n"
       b"    goto Exit0;\n"
       b"#endif\n"
       b"    nRetCode = GetTempPath(MAX_PATH, szTempPath);\n")

def main():
    d = open(TEP, 'rb').read()
    if MOC in d:
        print('da va roi (thay dau nhan %s), khong lam gi' % MOC.decode()); return 0
    n = d.count(CU)
    if n != 1:
        print('LOI: tim thay %d cho khop, can dung 1' % n); return 1
    open(TEP, 'wb').write(d.replace(CU, MOI))
    print('da va %s' % TEP)
    return 0

sys.exit(main())
