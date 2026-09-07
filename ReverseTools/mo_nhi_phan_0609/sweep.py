# -*- coding: ascii -*-
"""Quet tuyen tinh toan bo doan ma thuc thi -> ghi dis_all.txt (addr mnemonic op)"""
import sys, time
from elfre import D, SEGS, MD, o2v
t=time.time()
out=open('dis_all.txt','w')
n=0
for va, sz, off, fl in SEGS:
    if not (fl & 1): continue   # PF_X
    print("seg va=0x%08X sz=0x%X" % (va, sz))
    MD.skipdata = True
    for ins in MD.disasm(D[off:off+sz], va):
        out.write("%08X %s %s\n" % (ins.address, ins.mnemonic, ins.op_str)); n+=1
out.close()
print("xong", n, "lenh", round(time.time()-t,1), "s")
