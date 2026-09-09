# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 KHINHCONG c] Chu: "khinh cong duoc roi nhung chua chon huong nhay duoc, no mac dinh theo huong
# nhin cua nhan vat".
#
# Ly do: JxKyNang_Nhip ban PHAT DAU NGAY khi dat ngon (s_uKNDanhLuc = 0 -> danh ngay o nhip ke tiep) - hop voi
# ky nang danh (giu la danh lien tuc), nhung ky nang CAN DIEM (khinh cong) thi nhay luon theo huong nhin roi
# thoi (nhay mot lan), keo ngam sau do khong con tac dung. Ban tham khao: nut phu la CCMenuItem, callback chay
# luc NHA ngon (tap) - keo can thi lay huong can.
# Nay: ky nang can diem -> KHONG ban luc dat ngon; NHA ngon moi ban: co keo ngam thi nhay theo diem ngam
# (vach ngam / mui ten van hien khi keo), khong keo thi theo huong nhin. Cho xuong ngua van giu lenh nhu 68.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
DAU = "[ANDROID 10/09 KHINHCONG c]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten, so=1):
    for nl in ("\r\n", "\n"):
        c = nl.join(cu)
        if s.count(c) == so:
            return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho: %s" % (so, ten))


P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# 1. trang thai: nut dang giu la ky nang can diem?
s = thay(s, ["static int\t\t\ts_nKNNgonGiu = 0;\t\t// ngon tay dang that su de tren nut"], [
    "static int\t\t\ts_nKNNgonGiu = 0;\t\t// ngon tay dang that su de tren nut",
    "static int\t\t\ts_nKNGiuCanDiem = 0;\t// %s nut dang giu la ky nang CAN DIEM -> chi ban luc nha ngon" % DAU,
], "khai bao s_nKNNgonGiu")

# 2. BatDau: hoi truoc (chi hoi) xem nut nay co can diem khong
s = thay(s, [
    "\ts_nKNDangCam = nNut - 1;\t// 0 = nut chinh, 1..8 = o phu",
    "\ts_nKNNgonGiu = 1;\t\t\t// [ANDROID 10/09 KHINHCONG b]",
    "\ts_uKNDoiNguaDen = 0;",
], [
    "\ts_nKNDangCam = nNut - 1;\t// 0 = nut chinh, 1..8 = o phu",
    "\ts_nKNNgonGiu = 1;\t\t\t// [ANDROID 10/09 KHINHCONG b]",
    "\ts_uKNDoiNguaDen = 0;",
    "\t// %s ky nang can diem (khinh cong): khong ban luc dat ngon, doi nha ngon de con chon huong" % DAU,
    "\ts_nKNGiuCanDiem = 0;",
    "\t{",
    "\t\tKUiGameObject oCD;",
    "\t\tint nCD = 0;",
    "",
    "\t\tif (KyNang_CuaNut(nNut - 1, &oCD))",
    "\t\t\tKyNang_HoiCore((int)oCD.uId, NULL, NULL, NULL, NULL, &nCD, 1);",
    "\t\ts_nKNGiuCanDiem = nCD;",
    "\t}",
], "BatDau can diem")

# 3. Nhip: dang giu ky nang can diem thi chua ban (tru khi dang cho xuong ngua = ngon da nha)
s = thay(s, [
    "\tKyNang_LuanChuyen();\t// [ANDROID 10/09 LUAN] chay moi khung, khong phu thuoc dang giu nut hay khong",
    "\tif (s_nKNDangCam < 0)",
    "\t\treturn;",
], [
    "\tKyNang_LuanChuyen();\t// [ANDROID 10/09 LUAN] chay moi khung, khong phu thuoc dang giu nut hay khong",
    "\tif (s_nKNDangCam < 0)",
    "\t\treturn;",
    "\tif (s_nKNGiuCanDiem && s_nKNNgonGiu)",
    "\t\treturn;\t\t// %s con de ngon: chua nhay, doi nha ngon (JxKyNang_Nha ban)" % DAU,
], "Nhip can diem")

# 4. Nha: ky nang can diem -> ban luc nha
s = thay(s, [
    "\ts_nKNNgonGiu = 0;",
    "\t// [ANDROID 10/09 KHINHCONG b] dang cho xuong/len ngua: giu lenh, JxKyNang_Nhip thu lai roi tu nha",
], [
    "\ts_nKNNgonGiu = 0;",
    "\t// %s ky nang can diem: NHA ngon moi nhay - co keo ngam thi theo diem ngam, khong thi theo huong nhin" % DAU,
    "\tif (bCo && s_nKNGiuCanDiem && s_uKNDoiNguaDen == 0)",
    "\t{",
    "\t\ts_uKNDanhLuc = (unsigned int)GetTickCount();",
    "\t\tKyNang_DanhMotPhat();\t// ben trong tu dat s_nKNDangCam = -1 khi nhay xong",
    "\t}",
    "\t// [ANDROID 10/09 KHINHCONG b] dang cho xuong/len ngua: giu lenh, JxKyNang_Nhip thu lai roi tu nha",
], "Nha can diem")
ghi(P, s)
print("da va:", P)
