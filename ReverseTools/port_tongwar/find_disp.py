#!/usr/bin/env python
# -*- coding: ascii -*-
"""Quet tuyen tinh .text cua ELF tim moi lenh co toan hang bo nho [reg+disp] voi disp cho truoc.
   python find_disp.py <elf> <disp-hex> [start] [end]
   In ra dia chi + lenh + ten ham Lua gan nhat phia tren (theo luamap.full.txt) de doan ngu canh."""
import sys, struct, bisect, re
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86 import X86_OP_MEM
path = sys.argv[1]; disp = int(sys.argv[2], 16)
d = open(path, 'rb').read()
e_phoff, = struct.unpack_from('<I', d, 0x1C)
e_phentsize, e_phnum = struct.unpack_from('<HH', d, 0x2A)
segs = []
for i in range(e_phnum):
    o = e_phoff + i * e_phentsize
    ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from('<8I', d, o)
    if ptype == 1 and filesz and (flags & 1):
        segs.append((va, filesz, off))
names = []
for ln in open(r"D:\GAMEDEVNEW\ReverseTools\jx_linux_y.luamap.full.txt"):
    m = re.match(r'(0x[0-9A-Fa-f]+)\s+(\S+)', ln)
    if m: names.append((int(m.group(1), 16), m.group(2)))
names.sort()
keys = [a for a, n in names]
def near(va):
    i = bisect.bisect_right(keys, va) - 1
    return "%s+0x%x" % (names[i][1], va - names[i][0]) if i >= 0 else "?"
ONLY2 = '--two' in sys.argv
sys.argv = [a for a in sys.argv if a != '--two']
start = int(sys.argv[3], 16) if len(sys.argv) > 3 else 0
end = int(sys.argv[4], 16) if len(sys.argv) > 4 else 0xffffffff
md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = True
for va, sz, off in segs:
    code = d[off:off + sz]
    pos = 0
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
                if op.type == X86_OP_MEM and op.mem.disp == disp and op.mem.base != 0 and (op.mem.index != 0 or not ONLY2):
                    print("0x%08X  %-30s %s" % (ins.address, ins.mnemonic + " " + ins.op_str, near(ins.address)))
        else:
            pos += 1
            continue
        if n == 0: pos += 1
