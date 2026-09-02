# -*- coding: utf-8 -*-
"""oct2raw_tcvn3.py [VHTD 02/09i] - doi chuoi thong bao ESCAPE bat phan ("Kh\\253ng...") trong cac hunk [VHTD]/[HOASON]/[HS ...]
thanh TCVN3 THO (doc duoc, quy tac RULE 0 cua chu 20/08: "trong script hien ca chu ca so, rat kho doc").
Byte trong binary KHONG doi (/source-charset = /execution-charset = windows-1258) -> khong can build lai.
Chi doi literal nam trong 12 dong sau 1 dong co marker, va MOI byte giai ra phai thuoc bang TCVN3 (tranh GBK).
DUNG: python oct2raw_tcvn3.py [--kiem]
"""
import io, os, re, sys, importlib.util

KIEM = "--kiem" in sys.argv
ROOTS = [r"D:\GAMEDEVNEW\Sources\Core\Src", r"D:\GAMEDEVNEW\Sources\S3Client"]
spec = importlib.util.spec_from_file_location("vn_edit", r"C:/Users/nguye/.claude/skills/swordonline-dev/scripts/vn_edit.py")
vn = importlib.util.module_from_spec(spec); spec.loader.exec_module(vn)
TCVN = set((k if isinstance(k, int) else ord(k)) for k in vn._TCVN3_TO_UNICODE.keys())
OCT = re.compile(rb"\\([0-3][0-7][0-7])")
MARK = re.compile(rb"\[(VHTD|HOASON|HS|HOTHAN|PF|PFCHAT|VD|DL|PETKN|TKKET|TKCHET|CHITU|WASH)[^\]\n]{0,24}\]")
LIT = re.compile(rb'"((?:[^"\\\n]|\\.)*)"')

def conv_literal(m):
    body = m.group(1)
    if not OCT.search(body) or b"\\\\" in body:
        return m.group(0), 0
    vals = [int(x, 8) for x in OCT.findall(body)]
    if any(v < 0x80 or v not in TCVN for v in vals):
        return m.group(0), 0
    new = OCT.sub(lambda o: bytes([int(o.group(1), 8)]), body)
    return b'"' + new + b'"', len(vals)

def main():
    tot = 0
    for root in ROOTS:
        for dp, dn, fn in os.walk(root):
            for f in fn:
                if not f.lower().endswith((".cpp", ".h")): continue
                p = os.path.join(dp, f)
                b = io.open(p, "rb").read()
                lines = b.split(b"\n")
                changed = 0; last_mark = -100
                for i, l in enumerate(lines):
                    if MARK.search(l): last_mark = i
                    if i - last_mark > 12 or b'"' not in l or not OCT.search(l): continue
                    n = [0]
                    def rep(m):
                        s, k = conv_literal(m); n[0] += k; return s
                    nl = LIT.sub(rep, l)
                    if n[0]:
                        print("  [+] %s:%d (%d escape) %s" % (f, i + 1, n[0], nl.strip().decode("latin-1")[:110]))
                        lines[i] = nl; changed += n[0]
                if changed:
                    nb = b"\n".join(lines)
                    hi_old = sum(1 for c in b if c >= 0x80); hi_new = sum(1 for c in nb if c >= 0x80)
                    assert hi_new == hi_old + changed, (f, hi_old, hi_new, changed)
                    assert b"\xef\xbf\xbd" not in nb
                    if not KIEM:
                        io.open(p, "wb").write(nb)
                    print("  => %s %s: +%d byte cao" % ("KIEM" if KIEM else "ghi", p, changed))
                    tot += changed
    print("TONG %d escape%s" % (tot, " (KIEM)" if KIEM else ""))

if __name__ == "__main__":
    main()
