# -*- coding: ascii -*-
import re, struct, sys
sys.path.insert(0,'.')
from elfre import D, v2o, o2v, MD, find_bytes
from loader import cstr
from findfn import funcs
# 1) KMagicDesc ctor: ham chua 0x08072560
fs=funcs(0x08071000,0x08072560); start=fs[-1]; print("ctor KMagicDesc bat dau 0x%08X"%start)
off=v2o(start); names={}
for ins in MD.disasm(D[off:off+0x2400], start):
    r=re.match(r'dword ptr \[(0x[0-9a-f]+)\], (0x[0-9a-f]+)$', ins.op_str)
    if ins.mnemonic=='mov' and r:
        a=int(r.group(1),0); v=int(r.group(2),0); s=cstr(v)
        if s and re.match(r'^[a-z][a-z0-9_]+$', s): names[a]=s
    if ins.mnemonic=='ret': break
addrs=sorted(names); base=addrs[0]; stride=addrs[1]-addrs[0]
print("bang ten: %d muc, base=0x%08X stride=%d"%(len(names),base,stride))
idx2name={ (a-base)//stride: names[a] for a in addrs }
open('linux_magicnames.txt','w').write("\n".join("%d %s"%(k,idx2name[k]) for k in sorted(idx2name)))
for k in sorted(idx2name):
    if idx2name[k] in ('randmove','fastwalkrun_p','autodeathskill','autoattackskill','skill_eventskilllevel','hide','missle_missrate','missle_hitcount'): print("  idx %d = %s"%(k,idx2name[k]))
# 2) handler table: xref cac handler
for h in (0x08096150,0x08096170,0x08096190,0x08097860):
    xs=[o2v(p) for p in find_bytes(struct.pack('<I',h))]
    print("handler 0x%08X xref: %s"%(h, ["0x%08X"%x for x in xs if x]))
# 3) parse handler ctor around first xref of 0x08096170
xs=[o2v(p) for p in find_bytes(struct.pack('<I',0x08096170)) if o2v(p)]
x=xs[0]
fs2=funcs(x-0x3000,x); cstart=fs2[-1]; print("ctor handler bat dau 0x%08X"%cstart)
off=v2o(cstart); hand={}
for ins in MD.disasm(D[off:off+0x3000], cstart):
    r=re.match(r'dword ptr \[(\w+)(?: \+ (0x[0-9a-f]+|\d+))?\], (0x[0-9a-f]+)$', ins.op_str)
    if ins.mnemonic=='mov' and r and r.group(1)!='esp':
        k=int(r.group(2),0) if r.group(2) else 0; v=int(r.group(3),0)
        if 0x08048000<=v<0x082DA338: hand[k]=v
    if ins.mnemonic=='ret': break
print("handler tbl %d muc"%len(hand))
ks=sorted(hand); 
for k in ks:
    i=k//4
    nm=idx2name.get(i,'?')
    if hand[k] in (0x08096150,0x08096170,0x08096190,0x08097860) or nm in ('randmove','fastwalkrun_p','autodeathskill','autoattackskill','hide'):
        print("  [+0x%X] idx=%d handler=0x%08X ten=%s"%(k,i,hand[k],nm))
print("chuoi 0x82555d2:", cstr(0x082555d2))
