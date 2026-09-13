# -*- coding: utf-8 -*-
#
# [WAUTO 12/09] Hoan thien WAuto mobile - hai viec "cam giac dung" con treo tu B1/B2:
#
#  1. LOP VE RIENG CUA ANDROID DE LEN KHUNG AUTO (bay 3 cua ban giao B1): cum nut ky nang goc phai duoi va icon NPC
#     doi thoai ve SAU UiPaint nen nam tren khung. Nay: khung Auto dang mo thi khong ve hai thu do.
#     Can dieu khien (JxCan_Ve) VAN VE: nguoi choi van di duoc trong luc chinh, va may auto da nhuong khi dang cam can
#     (WA_TAY_CAM tu ban B2 g).
#  2. CHAM VAO CHU canh o tick cung bat/tat (truoc day chi trung dung o 24 px - kho cham). Nhan cua o tick doi thanh
#     KWndNhanWA, cham vao thi bao len trang y nhu cham vao o tick.
#
# Chi mobile (JX_MOBILE); ban PC khong doi. Chay lai vo hai.

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


# ---------------------------------------------------------------- 1. khong ve lop rieng khi khung Auto dang mo
def va_uishell(s):
    return thay(s, [
        "\tJxIconNpc_Ve();\t// [ANDROID 09/09 ICON] icon \"noi chuyen\" tren dau NPC doi thoai gan nhat",
        "\tJxHuongDi_Ve();\t// [ANDROID 09/09 HUONGDI] mui ten nho duoi chan theo huong di chuyen",
        "\tJxKyNang_Ve();\t// [ANDROID 09/09 KYNANG] bang nut ky nang + vach ngam + vong duoi chan dich",
    ], [
        "\t// %s Khung Auto dang mo thi KHONG ve icon NPC va cum nut ky nang: hai thu nay ve SAU UiPaint nen nam" % DAU,
        "\t// TREN khung (bay 3 cua ban giao B1 - \"cum ky nang che goc phai duoi\"). Can dieu khien van ve de con di duoc.",
        "\tif (!KUiWAuto::GetIfVisible())",
        "\t\tJxIconNpc_Ve();\t// [ANDROID 09/09 ICON] icon \"noi chuyen\" tren dau NPC doi thoai gan nhat",
        "\tJxHuongDi_Ve();\t// [ANDROID 09/09 HUONGDI] mui ten nho duoi chan theo huong di chuyen",
        "\tif (!KUiWAuto::GetIfVisible())",
        "\t\tJxKyNang_Ve();\t// [ANDROID 09/09 KYNANG] bang nut ky nang + vach ngam + vong duoi chan dich",
    ], "UiShell.cpp: khong ve lop rieng khi khung Auto mo")


def va_uishell_inc(s):
    # KUiWAuto phai duoc khai bao TRUOC khoi ve (dong ~375). Tep da co #include "UiCase/UiWAuto.h" nhung o tan
    # dong 1074 (khoi Player_WAuto cua B1) nen khong dung duoc - them mot dong nua o dau tep (co rao dau vao).
    if "WAUTO 12/09] khung Auto" in s:
        return s
    return thay(s, [
        '#include "UiShell.h"',
    ], [
        '#include "UiShell.h"',
        '#ifdef JX_MOBILE',
        '#include "UiCase/UiWAuto.h"\t// %s khung Auto: dang mo thi khong ve cum ky nang / icon NPC de len' % DAU,
        '#endif',
    ], "UiShell.cpp: include UiWAuto.h")


# ---------------------------------------------------------------- 2. cham vao chu canh o tick
def va_trang_h(s):
    s = thay(s, [
        "// O NHOM (chu 11/09: \"phai ke o de phan biet tung nhom\", \"co mau phan biet cac nhom chuc nang\"): nen toi mo + vien + tieu de",
    ], [
        "// %s Nhan chu di kem o tick: cham vao CHU cung bat/tat (truoc day chi trung dung o 24 px moi an - kho cham)." % DAU,
        "class KWndNhanWA : public KWndText80",
        "{",
        "public:",
        "\tint\t\tWndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);",
        "};",
        "",
        "// O NHOM (chu 11/09: \"phai ke o de phan biet tung nhom\", \"co mau phan biet cac nhom chuc nang\"): nen toi mo + vien + tieu de",
    ], "UiWAutoTrang.h: lop KWndNhanWA")
    s = thay(s, [
        "\tKWndText80\t\t\tm_Nhan[WA_TR_NHAN];",
    ], [
        "\tKWndNhanWA\t\t\tm_Nhan[WA_TR_NHAN];\t// %s cham vao chu canh o tick cung bat/tat" % DAU,
    ], "UiWAutoTrang.h: doi kieu m_Nhan")
    return s


def va_trang_cpp(s):
    s = thay(s, [
        "// ---------------------------------------------------------------- o nhom",
    ], [
        "// ---------------------------------------------------------------- nhan chu cham duoc",
        "// %s Cham vao CHU canh o tick = cham vao o tick. Bao len cha bang WND_N_BUTTON_CLICK cua chinh nhan;" % DAU,
        "// KUiWAutoTrang::WndProc tim o tick nao nhan nhan do lam nhan cua no (nKheNhan) roi bat/tat.",
        "int KWndNhanWA::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)",
        "{",
        "\tif (uMsg == WM_LBUTTONDOWN && m_pParentWnd && !IsDisable())",
        "\t{",
        "\t\tm_pParentWnd->WndProc(WND_N_BUTTON_CLICK, (KUPARAM)(KWndWindow*)this, 0);",
        "\t\treturn 1;",
        "\t}",
        "\treturn KWndText80::WndProc(uMsg, uParam, nParam);",
        "}",
        "",
        "// ---------------------------------------------------------------- o nhom",
    ], "UiWAutoTrang.cpp: KWndNhanWA::WndProc")
    s = thay(s, [
        "\t\tfor (i = 0; i < WA_TR_CHON; i++)",
        "\t\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Chon[i])",
        "\t\t\t{",
        "\t\t\t\tMoMenuChon(i);",
        "\t\t\t\treturn 1;",
        "\t\t\t}",
    ], [
        "\t\tfor (i = 0; i < WA_TR_CHON; i++)",
        "\t\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Chon[i])",
        "\t\t\t{",
        "\t\t\t\tMoMenuChon(i);",
        "\t\t\t\treturn 1;",
        "\t\t\t}",
        "\t\tfor (i = 0; i < WA_TR_NHAN; i++)",
        "\t\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Nhan[i])",
        "\t\t\t{\t// %s cham vao CHU canh o tick: tim o tick nhan nhan nay lam nhan cua no" % DAU,
        "\t\t\t\tif (!m_pTab)",
        "\t\t\t\t\treturn 1;",
        "\t\t\t\tfor (int k = 0; k < m_pTab->nMuc; k++)",
        "\t\t\t\t{",
        "\t\t\t\t\tconst WAUiMuc& m = m_pTab->pMuc[k];",
        "\t\t\t\t\tif (m.nLoai == WA_MUC_TICK && m.nKheNhan == i && m.nKhe < WA_TR_TICK && !m_Tick[m.nKhe].IsDisable())",
        "\t\t\t\t\t{",
        "\t\t\t\t\t\tWndProc(WND_N_BUTTON_CLICK, (KUPARAM)(KWndWindow*)&m_Tick[m.nKhe], 0);",
        "\t\t\t\t\t\tbreak;",
        "\t\t\t\t\t}",
        "\t\t\t\t}",
        "\t\t\t\treturn 1;",
        "\t\t\t}",
    ], "UiWAutoTrang.cpp: WndProc nhan cham duoc")
    return s


def main():
    va("Sources/S3Client/Ui/UiShell.cpp", "khung Auto: dang mo thi khong ve", lambda s: va_uishell_inc(s) if "KHONG ve icon NPC" in s else va_uishell(va_uishell_inc(s)))
    va("Sources/S3Client/Ui/UiCase/UiWAutoTrang.h", "KWndNhanWA", va_trang_h)
    va("Sources/S3Client/Ui/UiCase/UiWAutoTrang.cpp", "KWndNhanWA::WndProc", va_trang_cpp)
    print("xong wauto14")


if __name__ == "__main__":
    main()
