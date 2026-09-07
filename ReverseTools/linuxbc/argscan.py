import sys, re, struct
import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elfre import D, v2o, o2v, MD, SEGS
TEXT_VA, TEXT_SZ, TEXT_OFF = [(v, sz, o) for v, sz, o, fl in SEGS if fl & 1][0]
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
for a in sys.argv[1:]:
    t = int(a, 0)
    for c in callers(t):
        fs = func_start(c); off = v2o(fs); pre = []
        for ins in MD.disasm(D[off:off + (c - fs) + 5], fs): pre.append(ins)
        win = pre[-40:]
        slots = {}; bytes_ = []
        for ins in win:
            m = re.match(r'dword ptr \[esp(?: \+ (0x[0-9a-f]+|\d+))?\], (0x[0-9a-f]+|\d+)$', ins.op_str)
            if ins.mnemonic == 'mov' and m: slots['esp+%x' % (int(m.group(1), 0) if m.group(1) else 0)] = int(m.group(2), 0)
            m2 = re.match(r'(e[a-d]x|e[sd]i|ebp), (0x[0-9a-f]+|\d+)$', ins.op_str)
            if ins.mnemonic == 'mov' and m2: slots[m2.group(1)] = int(m2.group(2), 0)
            m3 = re.match(r'byte ptr \[ebp - (0x[0-9a-f]+|\d+)\], (0x[0-9a-f]+|\d+)$', ins.op_str)
            if ins.mnemonic == 'mov' and m3: bytes_.append((int(m3.group(1), 0), int(m3.group(2), 0)))
        # packet type = byte store with the largest ebp offset (start of struct) typically
        pt = max(bytes_)[1] if bytes_ else None
        print("0x%08X -> call@0x%08X func 0x%08X  type=%s  args=%s" % (t, c, fs, pt, slots))
