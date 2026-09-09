# -*- coding: utf-8 -*-
"""Mo PE x64 KHONG CAN PDB: dung bang .pdata (RUNTIME_FUNCTION) de biet bien ham chinh xac,
roi capstone de giai ma lenh. Dung khi PDB da bi build sau ghi de.
  python mo_x64.py <pe> fn <rva>          -> ham chua rva: bien + giai ma quanh diem loi
  python mo_x64.py <pe> chain <rva> ...   -> moi rva -> ham chua no (dung cho ngan xep)
  python mo_x64.py <pe> dis <rva> [n]     -> giai ma n byte tu rva
"""
import struct, sys, bisect
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

class PE:
    def __init__(self, path):
        self.d = open(path, "rb").read()
        e = struct.unpack_from("<I", self.d, 0x3C)[0]
        nsec, = struct.unpack_from("<H", self.d, e + 6)
        optsz, = struct.unpack_from("<H", self.d, e + 20)
        opt = e + 24
        magic, = struct.unpack_from("<H", self.d, opt)
        self.base, = struct.unpack_from("<Q", self.d, opt + 24)
        ddoff = opt + (112 if magic == 0x20B else 96)
        self.dirs = [struct.unpack_from("<II", self.d, ddoff + 8 * i) for i in range(16)]
        self.secs = []
        so = opt + optsz
        for i in range(nsec):
            s = so + 40 * i
            name = self.d[s:s + 8].rstrip(b"\0").decode("latin-1")
            vsz, va, rsz, praw = struct.unpack_from("<IIII", self.d, s + 8)
            self.secs.append((name, va, vsz, praw, rsz))
    def off(self, rva):
        for name, va, vsz, praw, rsz in self.secs:
            if va <= rva < va + max(vsz, rsz):
                o = praw + (rva - va)
                return o if o < len(self.d) else None
        return None
    def read(self, rva, n):
        o = self.off(rva)
        return self.d[o:o + n] if o is not None else b""
    def pdata(self):
        rva, sz = self.dirs[3]
        o = self.off(rva)
        out = []
        for i in range(sz // 12):
            b, e, u = struct.unpack_from("<III", self.d, o + 12 * i)
            if b == 0 and e == 0:
                break
            out.append((b, e))
        out.sort()
        return out

def func_of(fns, starts, rva):
    i = bisect.bisect_right(starts, rva) - 1
    if i < 0:
        return None
    b, e = fns[i]
    return (b, e) if b <= rva < e else None

pe = PE(sys.argv[1]); cmd = sys.argv[2]
fns = pe.pdata(); starts = [f[0] for f in fns]
md = Cs(CS_ARCH_X86, CS_MODE_64); md.detail = False
print("PE base 0x%X  .pdata: %d ham" % (pe.base, len(fns)))

if cmd == "chain":
    for a in sys.argv[3:]:
        rva = int(a, 16)
        f = func_of(fns, starts, rva)
        print("  rva %-9s -> %s" % (hex(rva), ("ham %X..%X  (offset trong ham +0x%X, co %d byte)" % (f[0], f[1], rva - f[0], f[1] - f[0])) if f else "KHONG nam trong .pdata (co the la du lieu)"))
elif cmd == "fn":
    rva = int(sys.argv[3], 16)
    f = func_of(fns, starts, rva)
    if not f:
        print("rva khong nam trong ham nao"); raise SystemExit
    b, e = f
    print("HAM %X .. %X  (%d byte), diem loi +0x%X\n" % (b, e, e - b, rva - b))
    code = pe.read(b, e - b)
    for ins in md.disasm(code, pe.base + b):
        r = ins.address - pe.base
        mark = "  <<< LOI" if r == rva else ""
        print("  %08X  %-22s %s %s%s" % (r, ins.bytes.hex(), ins.mnemonic, ins.op_str, mark))
elif cmd == "dis":
    rva = int(sys.argv[3], 16); n = int(sys.argv[4], 0) if len(sys.argv) > 4 else 0x80
    for ins in md.disasm(pe.read(rva, n), pe.base + rva):
        print("  %08X  %-22s %s %s" % (ins.address - pe.base, ins.bytes.hex(), ins.mnemonic, ins.op_str))
