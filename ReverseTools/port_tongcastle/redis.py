#!/usr/bin/env python
# -*- coding: ascii -*-
"""dis.py - phu tro dich nguoc jx_linux_y (ELF32) cho dot tongcastle.

  python dis.py fn   0x080FD920 [max_insn]   dich nguoc tu dia chi, dung sau 'ret' dau tien
                                             (co dem call/jmp de doc het than ham don gian)
  python dis.py raw  0x080FD920 N            dich nguoc N lenh
  python dis.py xref 0x080E1260              tim moi 'call rel32' toi dia chi nay trong .text
  python dis.py str  0x08240000              in chuoi C tai dia chi ao
  python dis.py find "chuoi"                 tim chuoi + noi tham chieu (push imm32)
"""
import struct, sys
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

ELF = r"D:\ServerLinux\server1\jx_linux_y"
import os
if os.environ.get("RELAY"):
    ELF = r"D:\ServerLinux\gateway\s3relay\s3relay_y"


def map_file(path):
    d = open(path, 'rb').read()
    e_phoff, = struct.unpack_from('<I', d, 0x1C)
    e_phentsize, e_phnum = struct.unpack_from('<HH', d, 0x2A)
    segs = []
    for i in range(e_phnum):
        o = e_phoff + i * e_phentsize
        ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from('<8I', d, o)
        if ptype == 1 and filesz:
            segs.append((va, filesz, off, flags))

    def v2o(va):
        for v, sz, off, fl in segs:
            if v <= va < v + sz:
                return off + (va - v)
        return None

    def o2v(off):
        for v, sz, o, fl in segs:
            if o <= off < o + sz:
                return v + (off - o)
        return None
    return d, v2o, o2v, segs


D, V2O, O2V, SEGS = map_file(ELF)
MD = Cs(CS_ARCH_X86, CS_MODE_32)


def cstr(va, n=120):
    o = V2O(va)
    if o is None:
        return None
    e = D.find(b"\0", o, o + n)
    return D[o:e if e >= 0 else o + n].decode("latin-1")


def dis(va, maxn=400, stop_ret=True):
    o = V2O(va)
    code = D[o:o + maxn * 8]
    n = 0
    for ins in MD.disasm(code, va):
        s = "%s %s" % (ins.mnemonic, ins.op_str)
        extra = ""
        # chu thich chuoi cho push imm / mov imm
        for tok in ins.op_str.replace(",", " ").split():
            if tok.startswith("0x8") and len(tok) >= 9:
                try:
                    a = int(tok, 16)
                except Exception:
                    continue
                c = cstr(a, 60)
                if c and all(32 <= ord(ch) < 127 for ch in c[:8]) and len(c) >= 3:
                    extra = '  ; "%s"' % c
        print("0x%08X  %-40s%s" % (ins.address, s, extra))
        n += 1
        if stop_ret and ins.mnemonic == "ret":
            break
        if n >= maxn:
            break


def xref(target):
    out = []
    for v, sz, off, fl in SEGS:
        if not (fl & 1):
            continue
        seg = D[off:off + sz]
        i = 0
        while True:
            i = seg.find(b"\xe8", i)
            if i < 0 or i + 5 > len(seg):
                break
            rel, = struct.unpack_from("<i", seg, i + 1)
            if (v + i + 5 + rel) & 0xFFFFFFFF == target:
                out.append(v + i)
            i += 1
    for a in out:
        print("call tai 0x%08X" % a)
    print("%d xref" % len(out))


if __name__ == "__main__":
    cmd = sys.argv[1]
    if cmd == "fn":
        dis(int(sys.argv[2], 0), int(sys.argv[3]) if len(sys.argv) > 3 else 400, True)
    elif cmd == "raw":
        dis(int(sys.argv[2], 0), int(sys.argv[3]), False)
    elif cmd == "xref":
        xref(int(sys.argv[2], 0))
    elif cmd == "str":
        print(repr(cstr(int(sys.argv[2], 0))))
    elif cmd == "find":
        raw = sys.argv[2].encode("latin-1")
        p = -1
        while True:
            p = D.find(raw, p + 1)
            if p < 0:
                break
            va = O2V(p)
            print("chuoi 0x%08X" % (va or 0))
            if va:
                pat = struct.pack("<I", va)
                q = -1
                while True:
                    q = D.find(pat, q + 1)
                    if q < 0:
                        break
                    r = O2V(q)
                    if r:
                        print("   ref 0x%08X" % r)
