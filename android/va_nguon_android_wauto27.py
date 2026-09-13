# -*- coding: utf-8 -*-
#
# [WAUTO 12/09] Cham vao NHAN cua hop chon thi mo luon hop do (nhan "Vong sang #1", "Ruong cua"... la chu, ngon tay
# hay cham vao do hon la cham dung o hop ben canh). Bo sung cho wauto14 (cham vao nhan cua o TICK).
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
    return thay(s, [
        "\t\t\t\t\tif (m.nLoai == WA_MUC_TICK && m.nKheNhan == i && m.nKhe < WA_TR_TICK && !m_Tick[m.nKhe].IsDisable())",
        "\t\t\t\t\t{",
        "\t\t\t\t\t\tWndProc(WND_N_BUTTON_CLICK, (KUPARAM)(KWndWindow*)&m_Tick[m.nKhe], 0);",
        "\t\t\t\t\t\tbreak;",
        "\t\t\t\t\t}",
    ], [
        "\t\t\t\t\tif (m.nKheNhan != i)",
        "\t\t\t\t\t\tcontinue;",
        "\t\t\t\t\tif (m.nLoai == WA_MUC_TICK && m.nKhe < WA_TR_TICK && !m_Tick[m.nKhe].IsDisable())",
        "\t\t\t\t\t{",
        "\t\t\t\t\t\tWndProc(WND_N_BUTTON_CLICK, (KUPARAM)(KWndWindow*)&m_Tick[m.nKhe], 0);",
        "\t\t\t\t\t\tbreak;",
        "\t\t\t\t\t}",
        "\t\t\t\t\tif (m.nLoai == WA_MUC_CHON && m.nKhe < WA_TR_CHON && !m_Chon[m.nKhe].IsDisable())",
        "\t\t\t\t\t{\t// %s cham vao nhan cua hop chon (\"Vong sang #1\", \"Ruong cua\"...) = mo hop do" % DAU,
        "\t\t\t\t\t\tMoMenuChon(m.nKhe);",
        "\t\t\t\t\t\tbreak;",
        "\t\t\t\t\t}",
    ], "UiWAutoTrang.cpp: cham nhan mo hop chon")


def main():
    va("Sources/S3Client/Ui/UiCase/UiWAutoTrang.cpp", "cham vao nhan cua hop chon", va_trang)
    print("xong wauto15")


if __name__ == "__main__":
    main()
