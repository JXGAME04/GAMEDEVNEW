#!/usr/bin/env python
# -*- coding: ascii -*-
"""Tim cac lenh truy cap [reg+dispA] ma trong +-WIN byte co lenh truy cap [reg+dispB] (cung thanh ghi base).
   python find_pair.py <elf> <dispA> <dispB> [win=600] [start] [end]"""
import sys, struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86 import X86_OP_MEM
path = sys.argv[1]; dA = int(sys.argv[2], 16); dB = int(sys.argv[3], 16)
win = int(sys.argv[4]) if len(sys.argv) > 4 else 600
start = int(sys.argv[5], 16) if len(sys.argv) > 5 else 0x8048000
end = int(sys.argv[6], 16) if len(sys.argv) > 6 else 0x82da000
d = open(path, 'rb').read()
e_phoff, = struct.unpack_from('<I', d, 0x1C)
e_phentsize, e_phnum = struct.unpack_from('<HH', d, 0x2A)
segs = []
for i in range(e_phnum):
    o = e_phoff + i * e_phentsize
    ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from('<8I', d, o)
    if ptype == 1 and filesz and (flags & 1): segs.append((va, filesz, off))
md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = True
A, B = [], []
for va, sz, off in segs:
    code = d[off:off + sz]
    lo = max(start, va); hi = min(end, va + sz)
    if lo >= hi: continue
    pos = lo - va
    while pos < hi - va:
        n = 0
        for ins in md.disasm(code[pos:pos + 0x10000], va + pos):
            n += 1
            pos = ins.address - va + ins.size
            if ins.address > hi: break
            for op in ins.operands:
                if op.type == X86_OP_MEM and op.mem.base != 0:
                    if op.mem.disp == dA: A.append((ins.address, op.mem.base, ins.mnemonic + " " + ins.op_str))
                    if op.mem.disp == dB: B.append((ins.address, op.mem.base, ins.mnemonic + " " + ins.op_str))
        else:
            pos += 1; continue
        if n == 0: pos += 1
Bs = sorted(B)
import bisect
keys = [x[0] for x in Bs]
for a, base, txt in A:
    i = bisect.bisect_left(keys, a - win)
    hits = []
    while i < len(Bs) and Bs[i][0] <= a + win:
        if Bs[i][1] == base: hits.append(Bs[i])
        i += 1
    if hits:
        print("0x%08X  %-34s  | %s" % (a, txt, "; ".join("0x%08X %s" % (h[0], h[2]) for h in hits[:3])))
