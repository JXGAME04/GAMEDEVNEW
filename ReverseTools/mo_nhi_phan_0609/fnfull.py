# -*- coding: ascii -*-
"""fnfull.py <va> [maxbytes] : dis tu va den prologue ke tiep (55 89 e5 sau mot ret/jmp/padding)"""
import sys
from elfre import D, v2o, o2v, MD
def nxt(va, maxb):
    o=v2o(va); p=o+8
    while True:
        p=D.find(b'\x55\x89\xe5', p)
        if p<0 or p-o>maxb: return va+maxb
        # prologue that (thuong sau c3 / 90 / 8d ... padding) - chap nhan neu byte truoc la c3,90,00 hoac lea padding
        prev=D[p-1]
        if prev in (0xc3,0x90,0x00,0xcc) or D[p-4:p]==b'\x00\x00\x00\x00' or D[p-2:p]==b'\x00\x00':
            return o2v(p)
        p+=1
va=int(sys.argv[1],0); maxb=int(sys.argv[2],0) if len(sys.argv)>2 else 0x1000
end=nxt(va,maxb); off=v2o(va)
for ins in MD.disasm(D[off:off+(end-va)], va):
    print("0x%08X  %-14s %s %s"%(ins.address, ins.bytes.hex()[:14], ins.mnemonic, ins.op_str))
print("-- het tai 0x%08X"%end)
