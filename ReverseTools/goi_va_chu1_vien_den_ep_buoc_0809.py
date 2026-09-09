# -*- coding: utf-8 -*-
"""goi_va_chu1_vien_den_ep_buoc_0809.py - [CHU 08/09] Chu bao "mau ten nhan vat / danh hieu dam toi hon".
Tim ra: KFont3::SetBorderColor LAT NGUOC y nghia cua alpha 0. Chu thich ngay tren ham noi "alpha = 0 nghia la
KHONG ve vien chu", nhung than ham lai `if (a == 0) a = 0xFF;` -> ep vien DEN DAC. Ma KNpc::PaintInfo khai bao
`DWORD dwBorderColor = 0` (KNpc.h:922) va MOI nhan vat khong duoc chon deu goi PaintInfo(...) khong truyen mau vien
=> moi ten nhan vat, ten bang, danh hieu deu bi ve THEM mot lop vien den dac (KFont3 ve chu hai luot: vien roi than chu).
Khong bo cung, de chu tu so sanh: [Client] VienChu trong config.ini - 1 = nhu hien nay (ep vien den, mac dinh),
0 = ton trong ben goi (alpha 0 = khong vien). Chi sua Represent3 (KFont3.cpp bien dich vao Represent3.dll).
LUU Y: KFont3.cpp xuong dong bang LF, KRepresentShell3.cpp bang CRLF - giu nguyen tung tep."""
import io, re, sys
def load(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, ("\r\n" if "\r\n" in s else "\n")
def save(p, s, h0, nl):
    if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    if nl == "\r\n" and re.search(r"[^\r]\n", s): print("FAIL LF lan vao tep CRLF", p); sys.exit(1)
    if nl == "\n" and "\r" in s: print("FAIL CR lan vao tep LF", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)
def hi(s): return sum(1 for ch in s if ord(ch) >= 0x80)

p = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3\KRepresentShell3.cpp"
s, nl = load(p); h0 = hi(s)
if "g_nRep3VienChu" not in s:
    s = rep(s, "int  g_nRep3Vsync     = 0;" + nl,
        "int  g_nRep3Vsync     = 0;" + nl +
        "int  g_nRep3VienChu   = 1;\t// [CHU 08/09] 1 = ep vien den cho moi chuoi chu (nhu tu truoc); 0 = ton trong ben goi (alpha 0 = khong vien)" + nl)
    s = rep(s, "\tg_nRep3Vsync     = Rep3Ini(\"Rep3Vsync\", 0);" + nl,
        "\tg_nRep3Vsync     = Rep3Ini(\"Rep3Vsync\", 0);" + nl +
        "\tg_nRep3VienChu   = Rep3Ini(\"VienChu\", 1);\t// [CHU 08/09]" + nl)
save(p, s, h0, nl); print("OK KRepresentShell3.cpp (%s)" % ("CRLF" if nl == "\r\n" else "LF"))

p = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\iRepresent\Font\KFont3.cpp"
s, nl = load(p); h0 = hi(s)
if "g_nRep3VienChu" not in s:
    s = rep(s, "\tif (a == 0) // " + nl + "\t\ta = 0xFF; //" + nl,
        "\t// [CHU 08/09] Chu thich cua ham noi \"alpha = 0 nghia la KHONG ve vien\", nhung doan nay lai ep vien DEN DAC." + nl +
        "\t// KNpc::PaintInfo mac dinh dwBorderColor = 0 cho MOI nhan vat khong duoc chon => moi ten/danh hieu deu co" + nl +
        "\t// them mot lop vien den (chu duoc ve hai luot) => nhin dam/toi hon. [Client] VienChu = 0 de tat." + nl +
        "\textern int g_nRep3VienChu;" + nl +
        "\tif (a == 0 && g_nRep3VienChu)" + nl +
        "\t\ta = 0xFF;" + nl)
save(p, s, h0, nl); print("OK KFont3.cpp (%s)" % ("CRLF" if nl == "\r\n" else "LF"))
print("XONG CHU")
