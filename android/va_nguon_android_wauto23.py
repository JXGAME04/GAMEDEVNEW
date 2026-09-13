# -*- coding: utf-8 -*-
#
# [WAUTO 12/09] Hoan thien WAuto mobile - hai cho con thieu sau wauto10:
#
#  1. Tieu de cua BANG CHON (danh sach dai) lay tu NHAN cung hang cua hop chon ("Vong sang #1", "Ruong cua"...)
#     thay vi lay chu dang hien trong hop (thuong la "Khong thiet lap" - doc khong hieu dang chon cai gi).
#     Bo sinh da noi nKheNhan cho hop chon.
#  2. "Ruong cua" cua the Tong Kim: thanh nao KHONG co huong dang luu thi tu lui ve "Gan nhat (tu chon)" (ma 5)
#     - dung nhu WA_NapRuongHuong cua ban PC lam khi doi thanh (WAuto.cpp:160-178). Khong co buoc nay thi doi thanh
#     xong o van hien "Nam" trong khi Lam An khong co cua Nam, va gia tri gui sang may auto la mot cua khong ton tai.
#
# Chi mobile (JX_MOBILE). Chay lai vo hai.

import io
import os
import re
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

DAU = "[WAUTO 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def V(s):
    return re.sub(r"@@(.*?)@@", lambda m: vn(m.group(1)), s, flags=re.S)


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(V(x) for x in cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(V(x) for x in moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("   bo qua (da va):", p)
        return
    ghi(p, ham(s))
    print("   da va:", p)


def va_trang(s):
    # 1. tieu de bang chon = nhan cung hang
    s = thay(s, [
        "\t\tm_Chon[nKhe].GetLabel(szTieuDe, sizeof(szTieuDe));",
        "\t\tKUiWAutoDsach::MoChonMuc(this, nKhe, szTieuDe, m_apDong, n, nCu);",
    ], [
        "\t\t// %s tieu de = NHAN cung hang (\"Vong sang #1\", \"Ruong cua\"...), khong phai chu dang hien trong hop" % DAU,
        "\t\tszTieuDe[0] = 0;",
        "\t\tif (p->nKheNhan < WA_TR_NHAN)",
        "\t\t\tm_Nhan[p->nKheNhan].GetText(szTieuDe, sizeof(szTieuDe));",
        "\t\tif (!szTieuDe[0])",
        "\t\t\tstrcpy(szTieuDe, \"@@Chọn một mục@@\");",
        "\t\tKUiWAutoDsach::MoChonMuc(this, nKhe, szTieuDe, m_apDong, n, nCu);",
    ], "UiWAutoTrang.cpp: tieu de bang chon")

    # 2. Ruong cua: huong khong co o thanh dang chon -> lui ve "Gan nhat"
    s = thay(s, [
        "\telse if (p->nViec == WA_V_TK_RUONG)",
        "\t{\t// %s gia tri = MA HUONG 0..4, 5 = gan nhat; danh sach tinh xep dung theo ma nen chi so = ma" % DAU,
        "\t\tv = LayInt(p);",
        "\t\tif (v < 0 || v >= (int)p->nLuaChon)",
        "\t\t\tv = (int)p->nLuaChon - 1;",
        "\t\tstrncpy(sz, p->pLuaChon[v], 31);",
        "\t\tsz[31] = 0;",
        "\t}",
    ], [
        "\telse if (p->nViec == WA_V_TK_RUONG)",
        "\t{\t// %s gia tri = MA HUONG 0..4, 5 = gan nhat; danh sach tinh xep dung theo ma nen chi so = ma." % DAU,
        "\t\t// Thanh dang chon o \"Het tran ve\" KHONG co huong do -> lui ve \"Gan nhat\" va ghi lai, y nhu",
        "\t\t// WA_NapRuongHuong cua ban PC lam khi doi thanh (WAuto.cpp:160-178).",
        "\t\tconst WAUiMuc* pVe = MucTheoIdc(\"IDC_COMBO_9_VE\");",
        "\t\tint nThanh = pVe ? LayInt(pVe) : 0;",
        "\t\tv = LayInt(p);",
        "\t\tif (nThanh < 0 || nThanh >= WA_TKR_THANH)",
        "\t\t\tnThanh = 0;",
        "\t\tif (v < 0 || v >= (int)p->nLuaChon)",
        "\t\t\tv = (int)p->nLuaChon - 1;",
        "\t\tif (v < WA_TKR_HUONG && !s_WATKRuongCo[nThanh][v])",
        "\t\t{",
        "\t\t\tv = (int)p->nLuaChon - 1;\t\t// dong cuoi = \"Gan nhat (tu chon)\" = ma 5",
        "\t\t\tif (!m_bDangDien)",
        "\t\t\t{",
        "\t\t\t\tDatInt(p, v);",
        "\t\t\t\tDaDoi();",
        "\t\t\t}",
        "\t\t\telse",
        "\t\t\t\tDatInt(p, v);",
        "\t\t}",
        "\t\tstrncpy(sz, p->pLuaChon[v], 31);",
        "\t\tsz[31] = 0;",
        "\t}",
    ], "UiWAutoTrang.cpp: Ruong cua lui ve Gan nhat")
    return s


def main():
    va("Sources/S3Client/Ui/UiCase/UiWAutoTrang.cpp", "lui ve \"Gan nhat\" va ghi lai", va_trang)
    print("xong wauto11")


if __name__ == "__main__":
    main()
