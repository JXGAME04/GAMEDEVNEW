import sys, re
import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elfre import D, v2o, o2v, MD
refs = [0x80e101d, 0x80e198d, 0x80e1b3d, 0x80e1ced]
def func_start(va):
    o = v2o(va)
    p = D.rfind(b'\x55\x89\xe5', o - 8000, o)
    return o2v(p)
def dis(start, maxb=6000):
    off = v2o(start); out = []
    for ins in MD.disasm(D[off:off+maxb], start):
        out.append(ins)
        if ins.mnemonic == 'ret': break
    return out
seen = set()
for r in refs:
    fs = func_start(r)
    if fs in seen: continue
    seen.add(fs)
    ins = dis(fs)
    calls = {}; imms = {}
    for i in ins:
        if i.mnemonic == 'call':
            calls[i.op_str] = calls.get(i.op_str, 0) + 1
        for tok in re.findall(r'0x[0-9a-f]+', i.op_str):
            v = int(tok, 16)
            if 2 <= v < 0x10000: imms[v] = imms.get(v, 0) + 1
    print("=== func 0x%08X (%d insns, ref 0x%X) ends 0x%08X" % (fs, len(ins), r, ins[-1].address))
    print("  calls:", {k: v for k, v in sorted(calls.items(), key=lambda kv: -kv[1])})
    print("  imms:", {hex(k): v for k, v in sorted(imms.items(), key=lambda kv: -kv[1]) if k in (99, 100, 500, 32, 5, 25, 30, 40, 0x4d, 0x4b, 0x63, 0xe5, 0xea, 8, 9, 1000, 1500)})
    # print 30 insns before the ref (to see nMaxCount init + first call)
    idx = next(k for k, i in enumerate(ins) if i.address <= r < i.address + i.size)
    for i in ins[max(0, idx-30):idx+6]:
        print("    %08x  %s %s" % (i.address, i.mnemonic, i.op_str))
