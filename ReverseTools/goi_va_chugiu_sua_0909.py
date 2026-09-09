# -*- coding: utf-8 -*-
"""goi_va_chugiu_sua_0909.py - sua loi dich cua [CHUGIU]: g_nRep3ChuGiuMs dinh nghia SAU cho doc config
(dong 555) => C2065. Dua dinh nghia len khoi bien toan cuc dau tep (canh g_nRep3LocMs), bo khoi cu."""
import io
import sys

NL = "\r\n"
P = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3\KRepresentShell3.cpp"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80)
lf0 = s.count("\n") - s.count("\r\n")


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


# bo hai dong dinh nghia trong khoi gan OutputText
s = rep(s, "int      g_nRep3ChuGiuMs = 12;" + NL, "", "bo dinh nghia 1")
s = rep(s, "unsigned g_uRep3ChuGiu = 0, g_uRep3ChuVe = 0;" + NL, "", "bo dinh nghia 2")
# dinh nghia o dau tep, canh cac g_nRep3*
s = rep(s, "int  g_nRep3LocMs     = 0;",
        "int  g_nRep3LocMs     = 0;" + NL +
        "int  g_nRep3ChuGiuMs  = 12;" + "\t" + "// [CHUGIU 09/09] giu vi tri man hinh cua chu (ms); 0 = tat" + NL +
        "unsigned g_uRep3ChuGiu = 0, g_uRep3ChuVe = 0;",
        "them dinh nghia dau tep")

if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK sua CHUGIU: dinh nghia len dau tep")
