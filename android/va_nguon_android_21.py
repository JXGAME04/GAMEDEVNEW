# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 21: DONG TRONG KHUNG THOAI / DANH SACH TIN CAO HON CHO DE CHAM.
#
# Chu: "kich vao npc phai hien cac dong chat voi npc de DE KICH VAO - vuot len xuong duoc".
# Da do tan mat: cham DUNG dong thi thoai chay tiep, nhung cham lech 8 diem anh la truot. Dong thoai
# khong phai KWndButton (nen khong huong duoc phan noi vung cham cua dot va 19) ma la muc cua
# KWndMessageListBox - khung thoai NPC la KUiMsgSayNew voi KScrollMessageListBox.
#
# Buoc dong cua lop do tinh bang "m_nFontSize + 1" o 10 cho (ve, do trung, cuon). Gom het ve mot ham
# CaoDong() roi tren Android cong them mot chut -> dong cao hon, de cham hon, va vi DUNG CHUNG mot
# ham nen ve va do trung khong bao gio lech nhau.
#
# Hai cho "nWidth/nHeight = m_nFontSize + 1" (chan co nho nhat) KHONG phai buoc dong nen giu nguyen -
# vi vay chi thay dang CO NGOAC "(m_nFontSize + 1)".
#
# Ban Windows khong doi hanh vi: CaoDong() tra ve dung m_nFontSize + 1 khi khong co JX_ANDROID.
import io, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
H = "Sources/S3Client/Ui/Elem/WndMessageListBox.h"
C = "Sources/S3Client/Ui/Elem/WndMessageListBox.cpp"


def doc(p):
    return io.open(os.path.join(ROOT, p), encoding="latin-1", newline="").read()

def ghi(p, s):
    io.open(os.path.join(ROOT, p), "w", encoding="latin-1", newline="").write(s)


# --- 1. khai bao CaoDong() trong header ------------------------------------
s = doc(H)
if "CaoDong" in s:
    print("  bo qua (da co): khai bao CaoDong")
else:
    neo = "\tint GetFontSize()\t{return m_nFontSize;}"
    if s.count(neo) != 1:
        print("  !! khong tim thay GetFontSize (%d cho)" % s.count(neo))
        sys.exit(1)
    them = neo + "\r\n" + (
        "\t// [ANDROID 09/09 CHAM] BUOC DONG cua danh sach: ve, do trung va cuon deu dung ham nay nen\r\n"
        "\t// khong bao gio lech nhau. Tren dien thoai cong them cho dong cao hon, ngon tay de cham.\r\n"
        "#ifdef JX_ANDROID\r\n"
        "\tint CaoDong() const\t{ return m_nFontSize + 1 + 5; }\r\n"
        "#else\r\n"
        "\tint CaoDong() const\t{ return m_nFontSize + 1; }\r\n"
        "#endif")
    ghi(H, s.replace(neo, them))
    print("  va xong: khai bao CaoDong")

# --- 2. thay buoc dong trong .cpp ------------------------------------------
s = doc(C)
if "CaoDong()" in s:
    print("  bo qua (da co): thay buoc dong")
else:
    n1 = s.count("(m_nFontSize + 1)")
    n2 = s.count("max(m_nFontSize + 1, 0)")
    s = s.replace("max(m_nFontSize + 1, 0)", "max(CaoDong(), 0)")
    s = s.replace("(m_nFontSize + 1)", "(CaoDong())")
    ghi(C, s)
    print("  va xong: thay %d cho '(m_nFontSize + 1)' va %d cho 'max(m_nFontSize + 1, 0)'" % (n1, n2))
    con = doc(C).count("m_nFontSize + 1")
    print("  con lai %d cho 'm_nFontSize + 1' (phai la 2 cho chan co nho nhat)" % con)

print("")
print("XONG dot va 21.")
