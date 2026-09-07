#!/usr/bin/env python3
# -*- coding: ascii -*-
"""cl.py - do dac game_y_unpacked.bin (client, ma tho, VA = 0x401000 + offset)"""
import struct, sys
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
PATH=r"D:\ServerLinux\Patch\game_y_unpacked.bin"
D=open(PATH,'rb').read()
BASE=0x401000
def v2o(va): return va-BASE
def o2v(o): return o+BASE
MD=Cs(CS_ARCH_X86, CS_MODE_32)
def find_bytes(pat):
    out,p=[],-1
    while True:
        p=D.find(pat,p+1)
        if p<0: break
        out.append(p)
    return out
def xref(va, limit=40): return [o2v(p) for p in find_bytes(struct.pack('<I',va))[:limit]]
def dis(va,n=60,stop_ret=False):
    off=v2o(va)
    for ins in MD.disasm(D[off:off+n*16],va):
        print("0x%08X  %-20s %s %s"%(ins.address,ins.bytes.hex(),ins.mnemonic,ins.op_str))
        n-=1
        if n<=0: break
        if stop_ret and ins.mnemonic=='ret': break
def strsearch(s):
    raw=s.encode('latin-1'); hits=find_bytes(raw)
    print("## '%s' : %d noi"%(s,len(hits)))
    for h in hits[:20]:
        va=o2v(h); print("  off=0x%X va=0x%08X pre=%r term=%r"%(h,va,D[h-1:h],D[h+len(raw):h+len(raw)+1]))
        for r in xref(va,12): print("      <- nap tai 0x%08X"%r)
if __name__=='__main__':
    c=sys.argv[1]
    if c=='s': strsearch(sys.argv[2])
    elif c=='f': dis(int(sys.argv[2],0), int(sys.argv[3]) if len(sys.argv)>3 else 400, True)
    elif c=='r': dis(int(sys.argv[2],0), int(sys.argv[3]))
    elif c=='x':
        for r in xref(int(sys.argv[2],0)): print("0x%08X"%r)
    elif c=='b':
        for h in find_bytes(bytes.fromhex(sys.argv[2])): print("off=0x%X va=0x%08X"%(h,o2v(h)))
