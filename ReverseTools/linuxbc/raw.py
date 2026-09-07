import sys
import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elfre import D, v2o, MD
start = int(sys.argv[1], 0); n = int(sys.argv[2])
off = v2o(start); k = 0
for ins in MD.disasm(D[off:off+4000], start):
    print("  %08x  %s %s" % (ins.address, ins.mnemonic, ins.op_str)); k += 1
    if k >= n: break
