# -*- coding: utf-8 -*-
"""[IOS-SCRIPT 11/09] Noi bo dem duong dan trong LoadAllScript() (Sources/Core/Src/KSortScript.cpp).

LoadAllScript() khai "char szRootPath[100]" roi goi g_GetFullPath(szRootPath, "\\script"),
tuc la ghi <thu muc goc> + "\\script" + ky tu ket vao 100 byte.

Tren Windows thu muc goc ngan ("C:\\JX1") nen khong sao. Tren iOS thu muc goc la
  /private/var/mobile/Containers/Data/Application/<UUID 36 ky tu>/Documents   = 94 ky tu
  94 + 7 ("\\script") + 1 = 102 > 100  -> tran ngan xep -> __stack_chk_fail -> SIGABRT.

Truoc khi ban iOS goi realpath() thi duong dan la /var/mobile/... = 86 ky tu, cong lai vua
94 byte nen lot. realpath() la bat buoc (iOS phan biet hoa thuong, /var la lien ket mem toi
/private/var - khong co no thi khong tim thay tep co chu hoa trong ten), nen phai noi bo dem.

Vet gay loi lay tu bao cao su co cua may that:
  __stack_chk_fail <- LoadAllScript <- g_IniScriptEngine <- g_InitCore <- CoreGetShell <- KMyApp::GameInit

Sua: tren POSIX dung MAX_PATH (260) thay vi 100. Ngay duoi da co "char szOldRootPath[MAX_PATH]"
nen day moi la kich thuoc dung. Ban Windows giu nguyen 100 de khong doi mot byte nao;
CHU Y cho phien PC: ban Windows dinh cung loi nay neu nguoi choi cai game vao thu muc sau.
"""
import os, sys

TEP = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   'Sources', 'Core', 'Src', 'KSortScript.cpp')
MOC = b'IOS-SCRIPT'

CU = (b"\tchar szRootPath[100];\n"
      b"\tchar szOldRootPath[MAX_PATH];\n")
MOI = (b"#ifdef JX_POSIX\t// [IOS-SCRIPT 11/09] thu muc goc tren iOS dai 94 ky tu, cong \"\\script\" va ky tu ket la 102 > 100 -> tran ngan xep (SIGABRT ngay khi vao game)\n"
       b"\tchar szRootPath[MAX_PATH];\n"
       b"#else\n"
       b"\tchar szRootPath[100];\n"
       b"#endif\n"
       b"\tchar szOldRootPath[MAX_PATH];\n")

def main():
    d = open(TEP, 'rb').read()
    if MOC in d:
        print('da va roi, khong lam gi'); return 0
    n = d.count(CU)
    if n != 1:
        print('LOI: tim thay %d cho khop, can 1' % n); return 1
    open(TEP, 'wb').write(d.replace(CU, MOI))
    print('da va %s' % TEP)
    return 0

sys.exit(main())
