# -*- coding: utf-8 -*-
"""[BAOMAT 12/09 KHOMAT] Tren Apple, dua tai khoan / mat khau vao Keychain thay vi tep ini.

HIEN TRANG: KLogin ghi hai muc nay bang KIniFile vao <du lieu>/userdata/uicommon.ini muc [Login].
Tep do nam trong thu muc du lieu nguoi dung mo ra doc duoc, va di theo moi ban sao luu.
Noi dung co qua EDOneTimePad nhung do la CHE MAT chu khong phai ma hoa co khoa: ai co tep la doc lai duoc.

SUA: tren iOS va macOS, bon cho doc/ghi di qua Keychain cua he dieu hanh (ios/JxIosKhoaMat.mm).
Khoa nam trong phan cung may, app khac khong doc duoc, va dat co ThisDeviceOnly nen KHONG
di theo ban sao luu iCloud.

Ban Windows va Android KHONG doi mot dong nao: moi thay doi trong #ifdef JX_APPLE, nhanh #else
giu nguyen tung dong goc.
"""
import os, sys

TEP = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   'Sources', 'S3Client', 'Login', 'Login.cpp')
MOC = b'BAOMAT 12/09 KHOMAT'

KHAI = (b'#define\t$LAST_PASSWORD\t"LastPassword"\n')
KHAI_MOI = (KHAI +
  b'#ifdef JX_APPLE\t// [BAOMAT 12/09 KHOMAT] tren Apple hai muc nay nam trong Keychain, khong nam trong tep ini\n'
  b'extern "C" int  JxKhoMat_Luu(const char* pszKhoa, const void* pDuLieu, int nDai);\n'
  b'extern "C" int  JxKhoMat_Doc(const char* pszKhoa, void* pRa, int nRa);\n'
  b'extern "C" void JxKhoMat_Xoa(const char* pszKhoa);\n'
  b'#endif\n')

def rao(cu, moi_apple, thut):
    return (b'#ifdef JX_APPLE\t// [BAOMAT 12/09 KHOMAT]\n' + thut + moi_apple + b'\n#else\n' + cu + b'#endif\n')

CAP = [
 (b'\t\tpSetting->GetStruct($LOGIN, $LAST_ACCOUNT, szAccount, sizeof(szAccount));\n',
  b'JxKhoMat_Doc($LAST_ACCOUNT, szAccount, sizeof(szAccount));', b'\t\t'),
 (b'\t\t\tpSetting->GetStruct($LOGIN, $LAST_PASSWORD, Password.szPassword, sizeof(Password.szPassword));\n',
  b'JxKhoMat_Doc($LAST_PASSWORD, Password.szPassword, sizeof(Password.szPassword));', b'\t\t\t'),
 (b'\t\tpSetting->EraseKey($LOGIN, $LAST_ACCOUNT);\n',
  b'JxKhoMat_Xoa($LAST_ACCOUNT);', b'\t\t'),
 (b'\t\t\tpSetting->WriteStruct($LOGIN, $LAST_ACCOUNT, szBuffer, sizeof(szBuffer));\n',
  b'JxKhoMat_Luu($LAST_ACCOUNT, szBuffer, sizeof(szBuffer));', b'\t\t\t'),
 (b'\t\t\t\tpSetting->WriteStruct($LOGIN, $LAST_PASSWORD, Password.szPassword, sizeof(Password.szPassword));\n',
  b'JxKhoMat_Luu($LAST_PASSWORD, Password.szPassword, sizeof(Password.szPassword));', b'\t\t\t\t'),
]

def main():
    d = open(TEP, 'rb').read()
    if MOC in d:
        print('da va roi'); return 0
    if d.count(KHAI) != 1:
        print('LOI: khong thay khai bao $LAST_PASSWORD'); return 1
    d = d.replace(KHAI, KHAI_MOI, 1)
    for cu, moi, thut in CAP:
        n = d.count(cu)
        if n != 1:
            print('LOI: %d cho khop cho %r' % (n, cu[:60])); return 1
        d = d.replace(cu, rao(cu, moi, thut), 1)
    open(TEP, 'wb').write(d)
    print('da va %s (5 cho)' % TEP)
    return 0

sys.exit(main())
