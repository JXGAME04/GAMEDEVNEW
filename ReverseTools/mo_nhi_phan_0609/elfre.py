#!/usr/bin/env python3
# -*- coding: ascii -*-
"""elfre.py - do dac ELF32 khong section header (jx_linux_y)
   lenh:
     s  <chuoi>            : tim chuoi + moi noi tham chieu dia chi cua no
     f  <va> [n]           : dis mot ham (dung o ret / n lenh)
     r  <va> <n>           : dis n lenh tho
     x  <va>               : tim moi noi nap hang so = va
     b  <hex bytes>        : tim day byte
"""
import struct, sys, re
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

PATH = r"D:\ServerLinux\server1\jx_linux_y"

def load(path=PATH):
    d = open(path, 'rb').read()
    e_phoff, = struct.unpack_from('<I', d, 0x1C)
    e_phentsize, e_phnum = struct.unpack_from('<HH', d, 0x2A)
    segs = []
    for i in range(e_phnum):
        o = e_phoff + i * e_phentsize
        ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from('<8I', d, o)
        if ptype == 1 and filesz:
            segs.append((va, filesz, off, flags))
    return d, segs

D, SEGS = load()

def v2o(va):
    for v, sz, off, fl in SEGS:
        if v <= va < v + sz:
            return off + (va - v)
    return None

def o2v(off):
    for v, sz, o, fl in SEGS:
        if o <= off < o + sz:
            return v + (off - o)
    return None

MD = Cs(CS_ARCH_X86, CS_MODE_32)

def find_bytes(pat):
    out, p = [], -1
    while True:
        p = D.find(pat, p + 1)
        if p < 0: break
        out.append(p)
    return out

def xref(va, limit=40):
    return [o2v(p) for p in find_bytes(struct.pack('<I', va))[:limit]]

def dis(va, n=60, stop_ret=False):
    off = v2o(va)
    if off is None:
        print("!! 0x%08X ngoai tep" % va); return
    lines = []
    for ins in MD.disasm(D[off:off + n * 16], va):
        lines.append("0x%08X  %-20s %s %s" % (ins.address, ins.bytes.hex(), ins.mnemonic, ins.op_str))
        n -= 1
        if n <= 0: break
        if stop_ret and ins.mnemonic in ('ret',):
            break
    print("\n".join(lines))

def strsearch(s):
    raw = s.encode('latin-1') if isinstance(s, str) else s
    hits = find_bytes(raw)
    print("## '%s' : %d noi" % (s, len(hits)))
    for h in hits[:20]:
        va = o2v(h)
        # chi nhan chuoi ket thuc \0 va bat dau sau \0
        pre = D[h-1:h]
        term = D[h+len(raw):h+len(raw)+1]
        print("  off=0x%X va=%s  pre=%r term=%r" % (h, ("0x%08X" % va) if va else "?", pre, term))
        if va:
            for r in xref(va, 12):
                if r: print("      <- nap tai 0x%08X" % r)

if __name__ == '__main__':
    c = sys.argv[1]
    if c == 's': strsearch(sys.argv[2])
    elif c == 'f': dis(int(sys.argv[2],0), int(sys.argv[3]) if len(sys.argv)>3 else 400, True)
    elif c == 'r': dis(int(sys.argv[2],0), int(sys.argv[3]))
    elif c == 'x':
        for r in xref(int(sys.argv[2],0)): print("0x%08X" % r if r else "?")
    elif c == 'b':
        for h in find_bytes(bytes.fromhex(sys.argv[2])): print("off=0x%X va=0x%08X" % (h, o2v(h) or 0))
