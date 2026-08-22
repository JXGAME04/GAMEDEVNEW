#!/usr/bin/env python
"""Tim moi vi tri xuat hien mot hang 32-bit (vd offset struct) trong doan ma ELF, in dia chi ao.
Dung: python find_imm.py <elf> <hex-imm> [max]
"""
import struct, sys
path = sys.argv[1]
imm = int(sys.argv[2], 16)
mx = int(sys.argv[3]) if len(sys.argv) > 3 else 60
d = open(path, 'rb').read()
e_phoff, = struct.unpack_from('<I', d, 0x1C)
e_phentsize, e_phnum = struct.unpack_from('<HH', d, 0x2A)
segs = []
for i in range(e_phnum):
    o = e_phoff + i * e_phentsize
    ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from('<8I', d, o)
    if ptype == 1 and filesz and (flags & 1):
        segs.append((va, filesz, off))
pat = struct.pack('<I', imm)
n = 0
for va, sz, off in segs:
    seg = d[off:off + sz]
    i = seg.find(pat)
    while i >= 0 and n < mx:
        print("0x%08X" % (va + i))
        n += 1
        i = seg.find(pat, i + 1)
