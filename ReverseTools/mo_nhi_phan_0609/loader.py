# -*- coding: ascii -*-
import os
"""Quet mot doan ma, ghep cap  GetInteger/GetString(cot, mac dinh, &truong)  -> bang offset."""
import sys, re, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elfre import D, v2o, o2v, MD

def cstr(va):
    o = v2o(va)
    if o is None: return None
    e = D.find(b'\0', o, o + 200)
    if e < 0: return None
    s = D[o:e]
    try: return s.decode('gbk')
    except Exception: return s.decode('latin-1')

def scan(start, end):
    off = v2o(start)
    n = (end - start)
    slots = {}     # esp+k -> (kind, value)
    lastlea = {}   # reg -> (base, off)
    rows = []
    for ins in MD.disasm(D[off:off + n], start):
        m = ins.op_str
        # mov dword ptr [esp + K], IMM
        r = re.match(r'dword ptr \[esp(?: \+ (0x[0-9a-f]+|\d+))?\], (0x[0-9a-f]+|\d+)$', m)
        if ins.mnemonic == 'mov' and r:
            k = int(r.group(1), 0) if r.group(1) else 0
            v = int(r.group(2), 0)
            slots[k] = ('imm', v)
            continue
        # lea REG, [BASE + OFF]
        r = re.match(r'(\w+), \[(\w+)(?: \+ (0x[0-9a-f]+|\d+))?\]$', m)
        if ins.mnemonic == 'lea' and r:
            lastlea[r.group(1)] = (r.group(2), int(r.group(3), 0) if r.group(3) else 0)
            continue
        # mov dword ptr [esp+K], REG
        r = re.match(r'dword ptr \[esp(?: \+ (0x[0-9a-f]+|\d+))?\], (\w+)$', m)
        if ins.mnemonic == 'mov' and r:
            k = int(r.group(1), 0) if r.group(1) else 0
            reg = r.group(2)
            slots[k] = ('reg', reg, lastlea.get(reg))
            continue
        if ins.mnemonic == 'call':
            col = slots.get(8)
            colname = cstr(col[1]) if col and col[0] == 'imm' else None
            if colname and re.match(r'^[A-Za-z][A-Za-z0-9_]{1,30}$', colname):
                dflt = slots.get(0xc)
                fld  = slots.get(0x10)
                rows.append((ins.address, colname,
                             dflt[1] if dflt and dflt[0]=='imm' else dflt,
                             fld[2] if fld and fld[0]=='reg' else fld,
                             ins.op_str))
            slots = {}
    return rows

if __name__ == '__main__':
    a = int(sys.argv[1], 0); b = int(sys.argv[2], 0)
    for va, col, dflt, fld, tgt in scan(a, b):
        f = ("%s+0x%X" % fld) if isinstance(fld, tuple) else str(fld)
        d = ("0x%X" % dflt) if isinstance(dflt, int) else str(dflt)
        print("0x%08X  %-22s mac dinh=%-8s -> %-14s call %s" % (va, col, d, f, tgt))
