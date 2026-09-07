import sys, re, struct
import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elfre import D, v2o, o2v, MD, SEGS
TEXT_VA, TEXT_SZ, TEXT_OFF = [(v, sz, o) for v, sz, o, fl in SEGS if fl & 1][0]
def dis(start, maxb=8000):
    off = v2o(start); out = []
    for ins in MD.disasm(D[off:off+maxb], start):
        out.append(ins)
        if ins.mnemonic == 'ret': break
    return out
def callers(t):
    out = []; p = TEXT_OFF
    while True:
        p = D.find(b"\xe8", p + 1, TEXT_OFF + TEXT_SZ)
        if p < 0: break
        rel = struct.unpack_from("<i", D, p + 1)[0]
        if o2v(p) + 5 + rel == t: out.append(o2v(p))
    return out
def func_start(va):
    o = v2o(va); p = D.rfind(b'\x55\x89\xe5', o - 12000, o); return o2v(p)
mode = sys.argv[1]
for a in sys.argv[2:]:
    fn = int(a, 0)
    if mode == 'callers':
        cs = callers(fn)
        print("callers of 0x%08X: %d ->" % (fn, len(cs)), ["0x%08X in 0x%08X" % (c, func_start(c)) for c in cs][:40])
        continue
    ins = dis(fn)
    calls = {}
    for i in ins:
        if i.mnemonic == 'call': calls[i.op_str] = calls.get(i.op_str, 0) + 1
    print("=== func 0x%08X (%d insns) ends 0x%08X calls=%s" % (fn, len(ins), ins[-1].address, calls))
    lim = int(sys.argv[1][4:]) if mode.startswith('full') else 0
    for i in ins[:lim] if lim else []:
        print("  %08x  %s %s" % (i.address, i.mnemonic, i.op_str))
    if not lim:
        for i in ins:
            if re.search(r'byte ptr \[.*\], 0x|dword ptr \[.*\], 0x[0-9a-f]{1,4}$|word ptr \[.*\], 0x', i.op_str) and i.mnemonic == 'mov':
                print("  %08x  %s %s" % (i.address, i.mnemonic, i.op_str))
