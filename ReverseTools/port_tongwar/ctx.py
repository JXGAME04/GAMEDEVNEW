#!/usr/bin/env python
# -*- coding: ascii -*-
"""In ngu canh N lenh truoc/sau mot dia chi (dich nguoc lui bang cach thu nhieu diem bat dau).
   python ctx.py <elf> <addr> [before=30] [after=15]"""
import sys, struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
path = sys.argv[1]; target = int(sys.argv[2], 16)
nb = int(sys.argv[3]) if len(sys.argv) > 3 else 30
na = int(sys.argv[4]) if len(sys.argv) > 4 else 15
d = open(path, 'rb').read()
e_phoff, = struct.unpack_from('<I', d, 0x1C)
e_phentsize, e_phnum = struct.unpack_from('<HH', d, 0x2A)
segs = []
for i in range(e_phnum):
    o = e_phoff + i * e_phentsize
    ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from('<8I', d, o)
    if ptype == 1 and filesz: segs.append((va, filesz, off))
def v2o(va):
    for v, sz, off in segs:
        if v <= va < v + sz: return off + (va - v)
md = Cs(CS_ARCH_X86, CS_MODE_32)
# thu cac diem bat dau lui dan cho toi khi chuoi lenh di qua dung target
best = None
for back in range(nb * 8, 0, -1):
    st = target - back
    insns = list(md.disasm(d[v2o(st):v2o(st) + back + 200], st))
    if any(i.address == target for i in insns):
        best = insns; break
if not best:
    print("khong dong bo duoc"); sys.exit()
idx = [i.address for i in best].index(target)
for i in best[max(0, idx - nb): idx + na]:
    print("%s0x%08X  %-22s %s %s" % (">> " if i.address == target else "   ", i.address, i.bytes.hex(), i.mnemonic, i.op_str))
