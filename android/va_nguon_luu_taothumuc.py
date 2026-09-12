# -*- coding: utf-8 -*-
"""[LUU 12/09 TAOTHUMUC] Sua loi "chinh tuy chon trong game, vao lai khong luu" tren dien thoai.

CHUOI NGUYEN NHAN (da kiem tung mat xich):
  1. UiBase.cpp:65   UI_COMMON_SETTING_FILE = "\\UserData\\UiCommon.ini"
  2. UiOptions.cpp:652 / Login.cpp:860 goi CloseCommSettingFile(true) -> KIniFile::Save(...)
  3. KIniFile::Save dung KFile -> KFile::Create -> fopen(PathName, "wb+")
  4. Tren POSIX, fopen la macro -> jx_fopen -> JxPathPosix
  5. JxPathPosix HA CHU THUONG toan bo phan duoi thu muc du lieu (KPosixWin32.cpp:134)
     -> duong dan that la  <Documents>/userdata/uicommon.ini
  6. Tren iPhone chi co thu muc "UserData" (chu hoa, chep sang tu may PC), KHONG co "userdata".
     He tep iOS phan biet hoa thuong -> thu muc cha khong ton tai.
  7. fopen(..., "wb+") that bai ENOENT -> KFile::Create tra FALSE -> Save tra FALSE, KHONG BAO GI.
     Tuy chon mat, tai khoan mat, im lang.
Bang chung tren may: UserData/UiCommon.ini khong doi tu 19:05 du choi tiep toi 23:16,
trong khi apdata/ (chu thuong, do game tu tao vi cho do CO goi tao thu muc) van ghi binh thuong.

SUA o dung mot cho: jx_fopen(). Khi mo de GHI ma that bai vi thieu thu muc thi tao ca chuoi
thu muc cha roi mo lai mot lan nua. Chi chay tren nhanh DA THAT BAI nen khong the lam hong
duong di dang chay tot; ap dung cho moi cho luu (userdata, apdata, va bat ky cho nao khac).

Ban Windows KHONG lien quan: khong du an .vcxproj nao bien dich KPosixWin32.cpp.
"""
import os, sys

TEP = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   'Sources', 'Engine', 'Src', 'Platform', 'KPosixWin32.cpp')
MOC = b'TAOTHUMUC'

CU = (b"FILE* jx_fopen(const char* pszPath, const char* pszMode)\n"
      b"{\n"
      b"\tchar sz[1024]; return fopen(JxPathPosix(pszPath, sz, sizeof(sz)), pszMode);\n"
      b"}\n")

MOI = (b"/* [LUU 12/09 TAOTHUMUC] Tao ca chuoi thu muc cha cua mot duong dan tep. */\n"
       b"static void JxTaoThuMucCha(const char* pszPath)\n"
       b"{\n"
       b"\tchar sz[1024];\n"
       b"\tsize_t n = strlen(pszPath);\n"
       b"\tif (n == 0 || n >= sizeof(sz)) return;\n"
       b"\tmemcpy(sz, pszPath, n + 1);\n"
       b"\tchar* p = strrchr(sz, '/');\n"
       b"\tif (!p || p == sz) return;\t/* khong co thu muc cha, hoac cha la goc '/' */\n"
       b"\t*p = 0;\n"
       b"\tfor (char* q = sz + 1; *q; q++)\n"
       b"\t{\n"
       b"\t\tif (*q != '/') continue;\n"
       b"\t\t*q = 0; mkdir(sz, 0775); *q = '/';\n"
       b"\t}\n"
       b"\tmkdir(sz, 0775);\n"
       b"}\n"
       b"FILE* jx_fopen(const char* pszPath, const char* pszMode)\n"
       b"{\n"
       b"\tchar sz[1024];\n"
       b"\tJxPathPosix(pszPath, sz, sizeof(sz));\n"
       b"\tFILE* f = fopen(sz, pszMode);\n"
       b"\t/* [LUU 12/09 TAOTHUMUC] Mo de GHI ma thieu thu muc cha thi tao roi mo lai. Truoc day\n"
       b"\t   KIniFile::Save(\"\\\\UserData\\\\UiCommon.ini\") that bai im lang tren iPhone: JxPathPosix ha\n"
       b"\t   chu thuong thanh <goc>/userdata/..., ma tren may chi co thu muc \"UserData\" chep tu PC sang,\n"
       b"\t   he tep iOS lai phan biet hoa thuong -> mat het tuy chon va tai khoan da luu.\n"
       b"\t   Chi chay khi fopen DA that bai nen khong doi hanh vi cua duong di dang chay tot. */\n"
       b"\tif (!f && pszMode && (pszMode[0] == 'w' || pszMode[0] == 'a'))\n"
       b"\t{\n"
       b"\t\tJxTaoThuMucCha(sz);\n"
       b"\t\tf = fopen(sz, pszMode);\n"
       b"\t}\n"
       b"\treturn f;\n"
       b"}\n")

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
