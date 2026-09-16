# -*- coding: utf-8 -*-
# Thao ma ham Go (ELF64), chu thich chuoi literal (lea rip-rel) va loi goi
import sys, re
sys.path.insert(0, ".")
from gopcln import GoBin
from capstone import *

class G(GoBin):
    def init_cs(s):
        s.md = Cs(CS_ARCH_X86, CS_MODE_64)

    def strat(s, va, n):
        o = s.v2o(va)
        if o is None or n <= 0 or n > 400: return None
        t = s.d[o:o + n]
        try:
            u = t.decode("utf-8")
        except Exception:
            return None
        if all(ord(c) >= 32 or c in "\n\t" for c in u): return u
        return None

def dump(g, fname, maxn=4000, quiet=False):
    hits = [x for x in g.addr_of(fname) if x[1].endswith(fname) or fname in x[1]]
    if not hits:
        print("!! khong thay", fname); return
    for va, nm in hits:
        a, e = g.frange(va)
        print("\n===== %s  [%x-%x] =====" % (nm, a, e))
        o = g.v2o(a)
        code = g.d[o:o + min(e - a, maxn)]
        ins = list(g.md.disasm(code, a))
        # ghi nho lea gan nhat vao moi thanh ghi de ghep (lea reg,[rip+x]) + (mov $len)
        lastlea = {}
        for i, I in enumerate(ins):
            ann = ""
            if I.mnemonic == "lea" and "rip" in I.op_str:
                m = re.match(r"(\w+), \[rip \+ (0x[0-9a-f]+)\]", I.op_str) or re.match(r"(\w+), \[rip \- (0x[0-9a-f]+)\]", I.op_str)
                if m:
                    sign = 1 if "+" in I.op_str else -1
                    tgt = I.address + I.size + sign * int(m.group(2), 16)
                    lastlea[m.group(1)] = tgt
                    fn = g.name_of(tgt)
                    # thu doc do dai tu lenh ke tiep
                    for j in range(i + 1, min(i + 4, len(ins))):
                        J = ins[j]
                        mm = re.match(r".*, (0x[0-9a-f]+)$", J.op_str)
                        if J.mnemonic in ("mov",) and mm:
                            sv = g.strat(tgt, int(mm.group(1), 16))
                            if sv: ann = '  ; "%s"' % sv.replace("\n", "\\n")
                            break
                    if not ann:
                        for L in (8, 12, 16, 20, 24, 30, 40, 60):
                            sv = g.strat(tgt, L)
                            if sv and sv.isprintable():
                                ann = '  ; str@%x ~"%s"' % (tgt, sv); break
                    if not ann and fn and fn[0] == tgt:
                        ann = "  ; &%s" % fn[1]
            if I.mnemonic == "call":
                try:
                    t = int(I.op_str, 16)
                    fn = g.name_of(t)
                    if fn: ann = "  ; -> %s" % fn[1]
                except ValueError:
                    pass
            if quiet and not ann: continue
            print("  %08x  %-42s%s" % (I.address, I.mnemonic + " " + I.op_str, ann))

if __name__ == "__main__":
    g = G(sys.argv[1]); g.init_cs()
    for f in sys.argv[2:]:
        dump(g, f)
