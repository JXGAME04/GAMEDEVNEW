#!/usr/bin/env python
# -*- coding: ascii -*-
"""Tim moi lenh doc/ghi [reg+OFFSET] (disp32) trong doan .text cua ELF32.
   python find_off.py <elf> <offset-hex> [maxhits]
"""
import struct, sys
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools")
from re_disasm import map_file
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

path, off = sys.argv[1], int(sys.argv[2], 0)
mx = int(sys.argv[3]) if len(sys.argv) > 3 else 60
d, v2o, o2v, kind = map_file(path)
pat = struct.pack('<i', off)
md = Cs(CS_ARCH_X86, CS_MODE_32)
p = -1; n = 0
while n < mx:
    p = d.find(pat, p + 1)
    if p < 0: break
    va = o2v(p)
    if va is None or va < 0x08048000 or va > 0x0830a000: continue
    # disasm tu p-6 de bat lenh
    for back in (2, 3, 4, 5, 6, 7):
        s = p - back
        ins = list(md.disasm(d[s:s + 12], o2v(s)))
        if ins and ins[0].address + ins[0].size > o2v(p) and ('0x%x' % off) in ins[0].op_str:
            print("0x%08X  %s %s" % (ins[0].address, ins[0].mnemonic, ins[0].op_str)); n += 1
            break
