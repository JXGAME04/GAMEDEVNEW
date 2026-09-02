# -*- coding: utf-8 -*-
"""Dump cac dong skills.txt (Linux hoac JX1) theo danh sach id -> TSV utf-8 + tom tat.
usage: lin_skills_dump.py <skills.txt> <out.tsv> <mode: lin|jx1> id1 id2 ... (hoac a-b)
"""
import sys, io, importlib.util
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec); spec.loader.exec_module(dec2)
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
try:
    from vn_to_octal import unicode_to_tcvn3_bytes
    T2U = {}
    for cp in range(0x20, 0x1EFF):
        ch = chr(cp)
        try:
            b = unicode_to_tcvn3_bytes(ch)
        except Exception:
            continue
        if len(b) == 1 and b[0] >= 0x80:
            T2U.setdefault(b[0], ch)
except Exception:
    T2U = {}

def tcvn3(bs):
    return "".join(T2U.get(c, chr(c)) if c >= 0x80 else chr(c) for c in bs)

def ids_from(args):
    s = set()
    for a in args:
        if "-" in a:
            x, y = a.split("-"); s.update(range(int(x), int(y) + 1))
        else:
            s.add(int(a))
    return s

def main():
    src, out, mode = sys.argv[1], sys.argv[2], sys.argv[3]
    want = ids_from(sys.argv[4:])
    lines = open(src, "rb").read().split(b"\n")
    hdr = lines[0].rstrip(b"\r").split(b"\t")
    hdr = [h.decode("latin-1") for h in hdr]
    dec = (lambda b: dec2.decline2(b)) if mode == "lin" else tcvn3
    rows = []
    for l in lines[1:]:
        c = l.rstrip(b"\r").split(b"\t")
        if len(c) < 3: continue
        try: sid = int(c[2])
        except: continue
        if sid in want:
            rows.append((sid, c))
    rows.sort()
    with io.open(out, "w", encoding="utf-8", newline="") as f:
        f.write("\t".join(hdr) + "\n")
        for sid, c in rows:
            cells = []
            for i, x in enumerate(c):
                if i in (0, 1, 60, 62, 113):
                    cells.append(dec(x))
                else:
                    cells.append(x.decode("gbk", "replace"))
            f.write("\t".join(cells) + "\n")
    print("ghi %d dong -> %s" % (len(rows), out))
    have = {sid for sid, _ in rows}
    print("thieu:", sorted(want - have))
    ix = {h: i for i, h in enumerate(hdr)}
    for sid, c in rows:
        g = lambda k: c[ix[k]].decode("gbk", "replace") if k in ix and ix[k] < len(c) else ""
        print("%5d | %-32s | %-26s | sty %-3s ser %-2s req %-3s max %-3s cls %-3s cost %s/%s | icon %s | lvscript %s" % (
            sid, dec(c[0]).strip(), dec(c[1]).strip(), g("SkillStyle"), g("Series"), g("ReqLevel"), g("MaxLevel"), g("CharClass"),
            g("SkillCostType"), g("CostValue"), g("SkillIcon").split("\\")[-1], g("LvlSetScript")))

main()
