# -*- coding: utf-8 -*-
"""bo_bom_tcvn3.py [VHTD 02/09i] - tep nguon .cpp/.h co BOM UTF-8 -> MSVC (/source-charset:windows-1258 nhung BOM thang) doc UTF-8:
  * literal TCVN3 THO (byte 0x80-0xFF le) = UTF-8 hong -> C4828 + chuoi bi nuot (do 02/09: bom3.obj mat han chuoi)
  * literal "boc UTF-8" (byte b -> C2/C3 xx) -> ra byte b nhung chu co ma U+00C3/CC/D2/D5/DD/DE/E3/EC/F2/F5/FD/FE (e, o, i, u co dau)
    KHONG co trong cp1258 -> C4566 -> '?' (do: bom.obj 'A?B<ab>C?D').
=> Cach duy nhat dung cho MOI chu: BO BOM + literal TCVN3 tho (nhu 87 tep khac, CoreUseNameDef.h). Tool: giai boc cac literal boc
   (chi khi moi ky tu giai ra la ASCII hoac byte TCVN3; literal GBK giu nguyen), roi cat BOM. Kiem: so byte cao literal, khong EF BF BD.
DUNG: python bo_bom_tcvn3.py [--kiem] <tep...>
"""
import io, os, re, sys, importlib.util

KIEM = "--kiem" in sys.argv
FILES = [a for a in sys.argv[1:] if not a.startswith("--")]
spec = importlib.util.spec_from_file_location("vn_edit", r"C:/Users/nguye/.claude/skills/swordonline-dev/scripts/vn_edit.py")
vn = importlib.util.module_from_spec(spec); spec.loader.exec_module(vn)
TCVN = set((k if isinstance(k, int) else ord(k)) for k in vn._TCVN3_TO_UNICODE.keys())
LIT = re.compile(rb'"((?:[^"\\\n]|\\.)*)"')
PAIR = re.compile(rb"[\xc2\xc3][\x80-\xbf]")

def unwrap(body):
    """giai boc neu MOI cap C2/C3+xx giai ra byte TCVN3 va khong con byte cao nao khac; tra (moi, so cap) hoac (body, 0)"""
    if not PAIR.search(body): return body, 0
    out = bytearray(); i = 0; n = 0
    while i < len(body):
        c = body[i]
        if c in (0xC2, 0xC3) and i + 1 < len(body) and 0x80 <= body[i+1] <= 0xBF:
            v = ((c & 0x1F) << 6) | (body[i+1] & 0x3F)
            if v not in TCVN: return body, 0
            out.append(v); i += 2; n += 1
        elif c >= 0x80:
            return body, 0            # tron byte cao khac (GBK / TCVN3 tho) -> khong dong
        else:
            out.append(c); i += 1
    return bytes(out), n

def main():
    for p in FILES:
        b = io.open(p, "rb").read()
        if b[:3] != b"\xef\xbb\xbf":
            print("  [=] %s khong co BOM" % p); continue
        body_all = b[3:]
        tot = 0; lits = 0
        def rep(m):
            nonlocal tot, lits
            s, n = unwrap(m.group(1))
            if n:
                tot += n; lits += 1
                print("  [+] giai boc: %r -> %r" % (m.group(1)[:50], s[:50]))
            return b'"' + s + b'"'
        nb = LIT.sub(rep, body_all)
        assert b"\xef\xbf\xbd" not in nb
        # kiem: so byte cao giam dung = so cap giai boc
        hi0 = sum(1 for c in body_all if c >= 0x80); hi1 = sum(1 for c in nb if c >= 0x80)
        assert hi1 == hi0 - tot, (hi0, hi1, tot)
        print("  => %s %s: bo BOM, giai boc %d literal (%d cap)" % ("KIEM" if KIEM else "ghi", p, lits, tot))
        if not KIEM: io.open(p, "wb").write(nb)

if __name__ == "__main__":
    main()
