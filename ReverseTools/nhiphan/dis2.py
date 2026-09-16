# -*- coding: utf-8 -*-
import io, struct, re, sys
sys.path.insert(0,".")
from elfsym import ELF32
from capstone import *
class Lib:
    def __init__(s,p):
        s.e=ELF32(p); s.d=s.e.d; s.path=p
        s.n=(s.e.strtab-s.e.symtab)//s.e.syment if s.e.strtab>s.e.symtab else 0
        s.by_name={}; s.by_addr={}
        for i in range(s.n):
            try: nm,val,sz,info,shndx=s.e.sym(i)
            except Exception: continue
            if nm and shndx!=0 and (info&0xf) in (1,2):
                if (info&0xf)==2: s.by_name[nm]=(val,sz); s.by_addr.setdefault(val,nm)
        s.gotmap={}
        T=s.e.T
        for tr,ts in ((23,2),(17,18)):
            if tr in T:
                base=s.e.v2o(T[tr][0]); size=T.get(ts,[0])[0]
                for k in range(size//8):
                    r_off,r_info=struct.unpack_from("<II",s.d,base+k*8)
                    try: nm,val,sz,info,shndx=s.e.sym(r_info>>8)
                    except Exception: continue
                    if nm: s.gotmap[r_off]=nm
        s.pltgot=T.get(3,[0])[0]
        s.md=Cs(CS_ARCH_X86,CS_MODE_32)
        s.stub={}
    def stubname(s,va):
        if va in s.stub: return s.stub[va]
        o=s.e.v2o(va); r=None
        if o is not None:
            b=s.d[o:o+8]
            if b[:2]==b"\xff\xa3":                 # jmp [ebx+off]  (PIC PLT)
                off,=struct.unpack_from("<i",b,2); r=s.gotmap.get((s.pltgot+off)&0xffffffff)
            elif b[:2]==b"\xff\x25":
                a,=struct.unpack_from("<I",b,2); r=s.gotmap.get(a)
        s.stub[va]=r; return r
    def name(s,va):
        return s.by_addr.get(va) or s.stubname(va)
    def dis(s,fn,maxi=900):
        if fn not in s.by_name: return []
        va,sz=s.by_name[fn]; o=s.e.v2o(va)
        return list(s.md.disasm(s.d[o:o+(sz or 600)],va))[:maxi]
    def rodata(s,va,maxlen=120):
        o=s.e.v2o(va)
        if o is None or o>=len(s.d): return None
        e=o
        while e<len(s.d) and s.d[e] not in (0,) and e-o<maxlen: e+=1
        t=s.d[o:e]
        if len(t)>=3 and all(9<=c<127 for c in t): return t.decode("latin-1")
        return None
def pretty(lib, fn, keep=None, note=""):
    ins=lib.dis(fn)
    if not ins: print("  !! khong co",fn); return
    print("\n--- %s %s ---" % (fn,note))
    consts=[]
    for i in ins:
        t="%s %s"%(i.mnemonic,i.op_str); ann=""
        if i.mnemonic=="call":
            try:
                tgt=int(i.op_str,16); nm=lib.name(tgt)
                if nm: ann="   ; -> %s"%nm
            except ValueError: pass
        if keep and not (ann or any(k in t for k in keep)): continue
        print("   %06x  %-46s%s" % (i.address,t,ann))
