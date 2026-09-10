# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 WAUTO B2 g] Chu (11/09, sau B2): "nut di chuyen phai uu tien - dang danh di chuyen cung phai uu tien".
#
# Tren PC, WAuto NHUONG quyen cho nguoi choi khi GIU CHUOT TRAI: ExtAutoLoop (S3Client.cpp) rao cac may PK / danh / di / ve thanh
# bang Wnd_IsLButtonDown(), va ATYPE_PICKUPSET nhan co "dang giu chuot" de khong nhat do. Tren Android nguoi choi di bang CAN
# DIEU KHIEN (JxCanDieuKhien.cpp), khong phai chuot trai, nen may auto cu danh tiep de len lenh di -> nhan vat dung tai cho.
# Cung vay, giu nut ky nang (JxKyNang_Nhip) cu danh lap lai ke ca khi dang keo can.
#
#  1. VA  Sources/S3Client/S3Client.cpp: macro WA_TAY_CAM() = Wnd_IsLButtonDown() || JxCan_DangCam() (chi JX_ANDROID; ban Windows
#         macro mo ra DUNG Wnd_IsLButtonDown() nhu cu -> ma may khong doi). Thay 8 cho Wnd_IsLButtonDown() TRONG ExtAutoLoop.
#  2. VA  Sources/S3Client/Platform/JxCanDieuKhien.cpp: JxKyNang_Nhip() khong danh lap lai khi dang cam can (Android-only).
# Chay lai vo hai. Nguon TCVN3 chi sua bang latin-1.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 WAUTO B2 g]"


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


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


# ---------------------------------------------------------------- 1. S3Client.cpp
def va_s3client(s):
    nl = nl_cua(s)
    s = thay(s, ["#ifdef JX_ANDROID",
                 "#include \"Platform/JxWAutoNoiBo.h\"\t// [ANDROID 11/09 WAUTO B0] ben gui WAuto trong tien trinh (thay WAuto.exe)",
                 "#endif"],
                ["#ifdef JX_ANDROID",
                 "#include \"Platform/JxWAutoNoiBo.h\"\t// [ANDROID 11/09 WAUTO B0] ben gui WAuto trong tien trinh (thay WAuto.exe)",
                 "#include \"Platform/JxCanDieuKhien.h\"\t// %s JxCan_DangCam(): dang cam can = nguoi choi dang cam, may auto nhuong" % DAU,
                 "// %s Chu: \"nut di chuyen phai uu tien - dang danh di chuyen cung phai uu tien\". Tren PC WAuto nhuong quyen khi" % DAU,
                 "// nguoi choi GIU CHUOT TRAI (Wnd_IsLButtonDown) - may PK / danh / di / ve thanh dung, khong nhat. Tren Android can dieu",
                 "// khien khong phai chuot trai nen dang cam can cung tinh la \"tay dang cam\". Ban Windows: macro mo ra dung Wnd_IsLButtonDown().",
                 "#define WA_TAY_CAM()\t(Wnd_IsLButtonDown() || JxCan_DangCam())",
                 "#else",
                 "#define WA_TAY_CAM()\tWnd_IsLButtonDown()",
                 "#endif"], "S3Client.cpp: include + macro")
    # chi thay trong ExtAutoLoop
    a = s.index("void KMyApp::ExtAutoLoop(const autoData* pApData)")
    b = s.index(nl + "}" + nl, a) + 1
    than = s[a:b]
    n = than.count("Wnd_IsLButtonDown()")
    if n != 8:
        raise SystemExit("ExtAutoLoop: mong 8 cho Wnd_IsLButtonDown(), thay %d" % n)
    than = than.replace("Wnd_IsLButtonDown()", "WA_TAY_CAM()")
    return s[:a] + than + s[b:]


# ---------------------------------------------------------------- 2. JxCanDieuKhien.cpp
def va_can(s):
    return thay(s, ["void JxKyNang_Nhip()",
                    "{",
                    "\tunsigned int uNay;",
                    "",
                    "\tKyNang_LuanChuyen();\t// [ANDROID 10/09 LUAN] chay moi khung, khong phu thuoc dang giu nut hay khong",
                    "\tif (s_nKNDangCam < 0)",
                    "\t\treturn;"],
                   ["void JxKyNang_Nhip()",
                    "{",
                    "\tunsigned int uNay;",
                    "",
                    "\tKyNang_LuanChuyen();\t// [ANDROID 10/09 LUAN] chay moi khung, khong phu thuoc dang giu nut hay khong",
                    "\tif (s_nKNDangCam < 0)",
                    "\t\treturn;",
                    "\tif (JxCan_DangCam())\t// %s dang cam can: di chuyen uu tien, khong danh lap lai (nha can thi danh tiep)" % DAU,
                    "\t\treturn;"], "JxCanDieuKhien.cpp: JxKyNang_Nhip nhuong can")


va("Sources/S3Client/S3Client.cpp", DAU, va_s3client)
va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", DAU, va_can)
print("xong")
