import sys, re, struct
import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elfre import D, v2o, o2v, MD, SEGS
TEXT_VA, TEXT_SZ, TEXT_OFF = [(v, sz, o) for v, sz, o, fl in SEGS if fl & 1][0]
targets = {0x80e0fc0: "W5", 0x80e1920: "W7a", 0x80e1ad0: "W7b", 0x80e1c80: "W7c",
           0x80e0ef0: "B5", 0x80e1810: "B7a", 0x80e19d0: "B7b", 0x80e1b80: "B7c"}
# find E8 rel32 calls to targets
sites = []
for t, name in targets.items():
    p = TEXT_OFF
    while True:
        p = D.find(b"\xe8", p + 1, TEXT_OFF + TEXT_SZ)
        if p < 0: break
        rel = struct.unpack_from("<i", D, p + 1)[0]
        if o2v(p) + 5 + rel == t:
            sites.append((o2v(p), name))
sites.sort()
print("call sites:", len(sites))
def func_start(va):
    o = v2o(va); p = D.rfind(b'\x55\x89\xe5', o - 12000, o); return o2v(p)
for va, name in sites:
    # disassemble ~40 insns before the call site within the function
    fs = func_start(va)
    off = v2o(fs); pre = []
    for ins in MD.disasm(D[off:off + (va - fs) + 5], fs):
        pre.append(ins)
    pre = pre[-28:]
    slots = {}
    for ins in pre:
        m = re.match(r'dword ptr \[esp(?: \+ (0x[0-9a-f]+|\d+))?\], (0x[0-9a-f]+|\d+)$', ins.op_str)
        if ins.mnemonic == 'mov' and m:
            slots[int(m.group(1), 0) if m.group(1) else 0] = int(m.group(2), 0)
        m2 = re.match(r'(e[a-d]x|e[sd]i|ebp), (0x[0-9a-f]+|\d+)$', ins.op_str)
        if ins.mnemonic == 'mov' and m2:
            slots['reg_' + m2.group(1)] = int(m2.group(2), 0)
    print("%s at 0x%08X in func 0x%08X  imm-args: %s" % (name, va, fs, {k: v for k, v in sorted(slots.items(), key=lambda kv: str(kv[0]))}))
