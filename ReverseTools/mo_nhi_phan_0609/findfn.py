# -*- coding: ascii -*-
import sys, re, struct
sys.path.insert(0,".")
from elfre import D, v2o, o2v, MD, find_bytes

def funcs(lo, hi):
    """tim moi vi tri co prologue 55 89 e5 trong khoang"""
    out=[]
    o1, o2 = v2o(lo), v2o(hi)
    p = o1-1
    while True:
        p = D.find(b'\x55\x89\xe5', p+1)
        if p < 0 or p > o2: break
        out.append(o2v(p))
    return out

def body(va, maxb=4096):
    off=v2o(va); txt=[]
    for ins in MD.disasm(D[off:off+maxb], va):
        txt.append("%s %s"%(ins.mnemonic, ins.op_str))
        if ins.mnemonic=='ret': break
    return txt

if __name__=='__main__':
    lo,hi = int(sys.argv[1],0), int(sys.argv[2],0)
    need = sys.argv[3:]
    for f in funcs(lo,hi):
        b = body(f)
        s = " | ".join(b)
        if all(n in s for n in need):
            print("0x%08X  (%d lenh)" % (f, len(b)))
