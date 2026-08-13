# -*- coding: utf-8 -*-
"""dis.py <TenHam|0xADDR> [so_dong]
Dich nguoc mot ham engine trong jx_linux_y, giai ra:
  - chuoi ky tu duoc tham chieu  -> ";; \"...\""
  - dia chi ham duoc goi         -> ";; -> TenHam"  neu nam trong bang API
Dung: python dis.py TONG_GetMoney
      python dis.py TONG_ApplyAddMoney 200
"""
import struct, json, sys, os, re

BASE = os.path.dirname(os.path.abspath(__file__))
PATH = r"D:\ServerLinux\server1\jx_linux_y"
data = open(PATH, "rb").read()

_ph = struct.unpack_from("<I", data, 0x1C)[0]
_pe = struct.unpack_from("<H", data, 0x2A)[0]
_pn = struct.unpack_from("<H", data, 0x2C)[0]
loads = []
for i in range(_pn):
    t, o, va, pa, fsz, msz, fl, al = struct.unpack_from("<8I", data, _ph + i*_pe)
    if t == 1 and fsz > 0:
        loads.append((va, o, fsz, fl))

def v2o(v):
    for va, o, fsz, fl in loads:
        if va <= v < va + fsz:
            return o + (v - va)
    return None

api = json.load(open(os.path.join(BASE, "engine_api_full.json")))
addr2name = {}
for k, v in api.items():
    addr2name.setdefault(v, k)

PRINTABLE = re.compile(rb'^[\x20-\x7e\xa1-\xfe]{3,}$')
def try_string(v):
    o = v2o(v)
    if o is None:
        return None
    end = data.find(b"\0", o, o + 160)
    if end < 0 or end <= o:
        return None
    s = data[o:end]
    if PRINTABLE.match(s):
        try:
            return s.decode("gbk")
        except Exception:
            return s.decode("latin-1")
    return None

def main():
    if len(sys.argv) < 2:
        print(__doc__); return
    tgt = sys.argv[1]
    limit = int(sys.argv[2]) if len(sys.argv) > 2 else 120
    if tgt.startswith("0x"):
        va = int(tgt, 16); nm = addr2name.get(va, tgt)
    else:
        if tgt not in api:
            cand = [k for k in api if tgt.lower() in k.lower()]
            print("Khong co ham '%s'. Gan giong: %s" % (tgt, cand[:20])); return
        va = api[tgt]; nm = tgt

    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = False
    off = v2o(va)
    print("=== %s @ 0x%08x (file 0x%x) ===" % (nm, va, off))
    n = 0
    for ins in md.disasm(data[off:off + limit*16], va):
        line = "  0x%08x  %-8s %s" % (ins.address, ins.mnemonic, ins.op_str)
        ann = []
        for m in re.finditer(r'0x[0-9a-f]+', ins.op_str):
            v = int(m.group(0), 16)
            if v in addr2name and ins.mnemonic in ("call", "jmp"):
                ann.append("-> %s" % addr2name[v])
            else:
                s = try_string(v)
                if s and len(s) >= 3:
                    ann.append('"%s"' % s.replace("\n", "\\n")[:90])
        if ins.mnemonic == "call" and not ann:
            t = ins.op_str
            if t.startswith("0x"):
                ann.append("sub_%s" % t)
        print(line + ("   ;; " + " | ".join(ann) if ann else ""))
        n += 1
        if n >= limit:
            print("  ... (cat bot, tang so_dong de xem tiep)"); break
        if ins.mnemonic == "ret":
            break

main()
