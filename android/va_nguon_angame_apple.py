# -*- coding: utf-8 -*-
"""[IOS-AN 11/09] Nut "an game" tren Apple: day ung dung xuong nen.

Nut da co san trong thanh nhan vat (KUiPlayerBar|HideWindow). Tren Windows no goi
TrayMode::HideNotify(): an cua so + bo bieu tuong xuong khay he thong. Tren POSIX,
TrayMode la HAM RONG (JxAndroidStubs.cpp) nen bam vao khong xay ra gi.

Ban va: tren Apple goi JxIosAnGame() (ios/JxIosAnGame.mm) thay cho TrayMode:
  - iOS  : -[UIApplication suspend] - tac dung y het bam nut Home. Khong co ham chinh thuc
           nao cho viec nay; day la ham noi bo, du phong la mo mot dia chi ngoai.
  - macOS: [NSApp hide:] - ham chinh thuc, y nhu Cmd+H.

Ban Windows KHONG doi mot byte nao: dong goi TrayMode giu nguyen y het trong nhanh #else.
"""
import os, sys

TEP = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   'Sources', 'S3Client', 'Ui', 'UiCase', 'UiPlayerBar.cpp')
MOC = b'IOS-AN'

CU1 = b"\t\t{\n\t\t\tgTrayMode.HideNotify(hInst);\t\t\t\n\t\t}\n"
MOI1 = (b"\t\t{\n"
        b"#ifdef JX_APPLE\t// [IOS-AN 11/09] TrayMode la ham rong tren POSIX; Apple day han ung dung xuong nen (ios/JxIosAnGame.mm)\n"
        b"\t\t\tJxIosAnGame(NULL);\n"
        b"#else\n"
        b"\t\t\tgTrayMode.HideNotify(hInst);\t\t\t\n"
        b"#endif\n"
        b"\t\t}\n")

CU2 = b"#define\tSCHEME_INI_MINI\t\t\"UiPlayerBarMini.ini\"\t\t//file ini quy dinh Ui toi uu hoa game mini\n"
MOI2 = (CU2 +
        b"#ifdef JX_APPLE\t// [IOS-AN 11/09] day ung dung xuong nen (ios/JxIosAnGame.mm)\n"
        b"extern \"C\" int JxIosAnGame(const char* pszDiaChiDuPhong);\n"
        b"#endif\n")

def main():
    d = open(TEP, 'rb').read()
    if MOC in d:
        print('da va roi, khong lam gi'); return 0
    for cu, moi, ten in ((CU1, MOI1, 'noi nut'), (CU2, MOI2, 'khai bao ham')):
        n = d.count(cu)
        if n != 1:
            print('LOI: %s - tim thay %d cho khop, can 1' % (ten, n)); return 1
        d = d.replace(cu, moi)
    open(TEP, 'wb').write(d)
    print('da va %s' % TEP)
    return 0

sys.exit(main())
