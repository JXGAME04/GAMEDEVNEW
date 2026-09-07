import sys, re
import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elfre import D, v2o, o2v, MD
def dis(start, maxb=4000):
    off = v2o(start); out = []
    for ins in MD.disasm(D[off:off+maxb], start):
        out.append(ins)
        if ins.mnemonic == 'ret': break
    return out
for fn in (0x80e0ef0, 0x80e1810, 0x80e19d0, 0x80e1b80):
    ins = dis(fn)
    print("=== callee 0x%08X (%d insns) ends 0x%08X" % (fn, len(ins), ins[-1].address))
    for i in ins:
        print("  %08x  %s %s" % (i.address, i.mnemonic, i.op_str))
