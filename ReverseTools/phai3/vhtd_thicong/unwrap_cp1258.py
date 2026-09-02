# -*- coding: utf-8 -*-
"""unwrap_cp1258.py [VHTD 02/09p] - KNpc.cpp tung duoc luu kieu "cp1258 -> UTF-8" (moi byte TCVN3/GBK trong literal bien thanh 1-3 byte UTF-8
cua ky tu cp1258 tuong ung; co BOM). MSVC (BOM) giai nguoc dung: U+00B2 -> 0xB2, U+0300 -> 0xCC, U+01A1 -> 0xF5, U+0110 -> 0xD0...
bo_bom_tcvn3.py (dot 6) cat BOM nhung chi giai boc literal chu Viet (C2/C3 -> byte trong bang TCVN3) -> cac literal GBK boc
(\\spr\\skill\\...\\mag_spe_*.spr, \\Spr\\Ui3\\摆摊\\... = sap hang "bay ban") con nguyen 2 byte -> duong dan hong (chu 02/09 15:20).
Tool nay: moi literal la UTF-8 hop le co ky tu ngoai ASCII -> decode utf-8 -> encode cp1258 (= dung byte MSVC tung sinh). Literal tho
(khong phai UTF-8 hop le) giu nguyen. DUNG: python unwrap_cp1258.py [--kiem] <tep...>
"""
import io, re, sys

KIEM = "--kiem" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
FILES = [a for a in sys.argv[1:] if not a.startswith("--")] or [r"D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp"]
LIT = re.compile(rb'"((?:[^"\\\r\n]|\\.)*)"')

def to_bytes(u):
    """ky tu -> byte goc: uu tien cp1258 (BOM+MSVC tung sinh dung), ky tu U+0080..U+00FF khong co trong cp1258 (boc kieu Latin-1) -> ord."""
    out = bytearray()
    for ch in u:
        try:
            out += ch.encode("cp1258")
        except UnicodeEncodeError:
            if ord(ch) < 0x100: out.append(ord(ch))
            else: raise
    return bytes(out)

def main():
    for p in FILES:
        b = io.open(p, "rb").read()
        assert b[:3] != b"\xef\xbb\xbf", "con BOM: " + p
        n = 0; ex = []
        def rep(m):
            nonlocal n
            body = m.group(1)
            if not any(c >= 0x80 for c in body): return m.group(0)
            try:
                u = body.decode("utf-8")
            except UnicodeDecodeError:
                return m.group(0)                     # tho (TCVN3/GBK) -> giu
            try:
                raw = to_bytes(u)
            except UnicodeEncodeError:
                ex.append(("KHONG MAP cp1258", body[:60])); return m.group(0)
            n += 1
            if len(ex) < 40: ex.append((raw.decode("gbk", "replace")[:50], body[:50]))
            return b'"' + raw + b'"'
        nb = LIT.sub(rep, b)
        for e in ex: print("   ", e)
        print("  => %s %s: giai boc %d literal (byte cao %d -> %d)" % ("KIEM" if KIEM else "ghi", p, n, sum(1 for c in b if c >= 0x80), sum(1 for c in nb if c >= 0x80)))
        if not KIEM and n: io.open(p, "wb").write(nb)

if __name__ == "__main__":
    main()
