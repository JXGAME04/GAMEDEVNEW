# -*- coding: utf-8 -*-
r"""[IOS-BAOLOI 11/09] Ghi ma loi + chuoi loi khoi tao vao jx_ios.log (chi iOS).

GameInit() dat Error_SetErrorCode/Error_SetErrorString truoc moi lan return FALSE, nhung tren iOS
khong co hop thoai nao hien ra nen ta khong biet no hong o buoc nao. Them mot ham ghi thang
vao jx_ios.log, goi ngay khi MyApp.Init() that bai.

Chen NGUYEN KHOI, khong sua dong cu nao. Chay lai vo hai.
  python3 ios/va_nguon_ios_baoloi.py
"""
import io, os, sys
GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
bc = lambda t: sum(1 for c in t if ord(c) >= 0x80)

def va(duong, cu, moi, ten):
    p = os.path.join(GOC, duong)
    s = io.open(p, encoding="latin-1", newline="").read(); n0 = bc(s)
    if moi.split("\n")[1] in s:
        print("   bo qua (da co): " + ten); return
    if s.count(cu) != 1: sys.exit("LOI: '%s' khop %d lan" % (ten, s.count(cu)))
    s = s.replace(cu, moi)
    if bc(s) != n0: sys.exit("LOI: byte >= 0x80 doi")
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("   da va: " + ten)

# 1. ErrorCode.cpp: ham ghi nhat ky
va("Sources/S3Client/ErrorCode.cpp",
   "void Error_SetErrorString(const char* pcszString)",
   """#ifdef JX_IOS
// [IOS-BAOLOI 11/09] Tren iOS khong co hop thoai loi nao hien ra; ghi thang ma loi + chuoi loi
// vao jx_ios.log de biet GameInit() hong o buoc nao.
extern "C" void JxIos_GhiLoiKhoiTao(void)
{
\tFILE* f = jx_fopen("jx_ios.log", "ab");
\tif (!f) return;
\tfprintf(f, "[IOS] KHOI TAO HONG: ma loi = %u, chuoi loi = \\"%s\\"\\n",
\t\ts_uErrorCode, s_szErrorString[0] ? s_szErrorString : "(rong)");
\tfclose(f);
}
#endif

void Error_SetErrorString(const char* pcszString)""",
   "ErrorCode.cpp: ham JxIos_GhiLoiKhoiTao")

# 2. S3Client.cpp JxPosixMain: goi khi Init that bai
va("Sources/S3Client/S3Client.cpp",
   "\tif (MyApp.Init(NULL))\n\t\tMyApp.Run();\n\treturn 0;\n",
   """\tif (MyApp.Init(NULL))
\t\tMyApp.Run();
#ifdef JX_IOS
\telse
\t{
\t\textern \"C\" void JxIos_GhiLoiKhoiTao(void);\t// [IOS-BAOLOI 11/09] ErrorCode.cpp
\t\tJxIos_GhiLoiKhoiTao();
\t}
#endif
\treturn 0;
""",
   "S3Client.cpp: goi JxIos_GhiLoiKhoiTao khi Init hong")
print("xong va_nguon_ios_baoloi.py")
