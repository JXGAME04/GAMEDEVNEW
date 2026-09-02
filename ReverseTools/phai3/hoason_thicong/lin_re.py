# -*- coding: utf-8 -*-
"""lin_re.py : tro giup dich nguoc jx_linux_y (ELF32, stripped).
  bang ten thuoc tinh: mang char* tai VA 0x830e640 (dien boi ma tai 0x80724dd..), idx = (addr-0x830e640)/4
  bang handler KNpcAttribModify: dien boi ma quanh VA 0x8099800..0x809a100
DUNG: python lin_re.py names            -> in idx/ten Linux (ghi out/linux_magic_names.txt)
      python lin_re.py handlers         -> in idx -> handler VA (ghi out/linux_handlers.txt)
      python lin_re.py dis <va> [n]     -> dich nguoc n lenh tu VA
      python lin_re.py attr <ten> [n]   -> idx Linux + dich nguoc handler
      python lin_re.py xref <va>        -> tim lenh tham chieu hang so 32 bit = va (trong .text)
"""
import struct, re, sys, os, io
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
BIN = r"D:\ServerLinux\server1\jx_linux_y"
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "out")
d = open(BIN, "rb").read()
e_phoff, = struct.unpack_from("<I", d, 0x1C); e_phentsize, e_phnum = struct.unpack_from("<HH", d, 0x2A)
segs = []
for i in range(e_phnum):
    o = e_phoff + i * e_phentsize
    ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from("<8I", d, o)
    if ptype == 1 and filesz: segs.append((va, filesz, off))
def v2o(va):
    for v, sz, o in segs:
        if v <= va < v + sz: return o + (va - v)
def o2v(off):
    for v, sz, o in segs:
        if o <= off < o + sz: return v + (off - o)
def cstr(va, n=64):
    o = v2o(va)
    if o is None: return None
    s = d[o:o + n]; return s[:s.find(b"\0")].decode("latin-1")
md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = False
def disasm(va, n=40):
    out = []
    for ins in md.disasm(d[v2o(va):v2o(va) + 16 * n], va):
        out.append("0x%08x  %-7s %s" % (ins.address, ins.mnemonic, ins.op_str))
        if len(out) >= n: break
    return out
NAME_TB = 0x830e640
def names():
    m = {}
    for ins in md.disasm(d[v2o(0x8072400):v2o(0x8072400) + 0x2000], 0x8072400):
        mm = re.match(r"mov dword ptr \[0x([0-9a-f]+)\], 0x([0-9a-f]+)", ins.mnemonic + " " + ins.op_str)
        if mm:
            a = int(mm.group(1), 16); s = int(mm.group(2), 16)
            if 0x830e000 <= a < 0x8310000 and (a - NAME_TB) % 4 == 0:
                st = cstr(s)
                if st and re.match(r"^[a-z0-9_]+$", st): m[(a - NAME_TB) // 4] = st
        if ins.mnemonic == "ret" and len(m) > 200: break
    return m
def handlers():
    m = {}; base = None
    for ins in md.disasm(d[v2o(0x8099700):v2o(0x8099700) + 0x1000], 0x8099700):
        mm = re.match(r"mov dword ptr \[0x([0-9a-f]+)\], 0x([0-9a-f]+)", ins.mnemonic + " " + ins.op_str)
        if mm:
            a = int(mm.group(1), 16); h = int(mm.group(2), 16)
            if 0x8048000 <= h < 0x82db000: m[a] = h
    if not m: return {}, None
    base = min(m)
    # stride: gcc pointer-to-member = 8 byte (ptr, adj); kiem bang do_stun_p 261 -> 0x080968F0
    for stride in (8, 4):
        cand = {(a - base) // stride: h for a, h in m.items() if (a - base) % stride == 0}
        if cand.get(261) == 0x080968F0: return cand, (base, stride)
    return {(a - base) // 8: h for a, h in m.items()}, (base, 8)
if __name__ == "__main__":
    cmd = sys.argv[1]
    if cmd == "names":
        m = names()
        open(os.path.join(OUT, "linux_magic_names.txt"), "w").write("\n".join("%d\t%s" % (k, m[k]) for k in sorted(m)))
        print("Linux ten:", len(m), "max idx", max(m))
        for k in sorted(m):
            if k >= 255 or k < 12: print(k, m[k])
    elif cmd == "handlers":
        hm, info = handlers()
        print("base/stride:", info, "so handler:", len(hm))
        nm = names()
        open(os.path.join(OUT, "linux_handlers.txt"), "w").write("\n".join("%d\t%s\t0x%08x" % (k, nm.get(k, "?"), hm[k]) for k in sorted(hm)))
        for k in sorted(hm):
            if k >= 255: print(k, nm.get(k, "?"), hex(hm[k]))
    elif cmd == "dis":
        va = int(sys.argv[2], 16); n = int(sys.argv[3]) if len(sys.argv) > 3 else 40
        print("\n".join(disasm(va, n)))
    elif cmd == "attr":
        nm = names(); hm, info = handlers()
        inv = {v: k for k, v in nm.items()}
        for name in sys.argv[2].split(","):
            k = inv.get(name)
            print("==", name, "Linux idx", k, "handler", hex(hm[k]) if k in hm else "KHONG CO handler")
            if k in hm:
                n = int(sys.argv[3]) if len(sys.argv) > 3 else 30
                print("\n".join(disasm(hm[k], n)))
    elif cmd == "xref":
        va = int(sys.argv[2], 16)
        pat = struct.pack("<I", va)
        for m in re.finditer(re.escape(pat), d):
            o = m.start(); v = o2v(o)
            if v and v < 0x82db000: print("  tai VA ~", hex(v - 4), "(lenh chua hang so)")
