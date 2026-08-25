# -*- coding: utf-8 -*-
"""Dich nguoc cac ham NHOM B (JX1 chua co) tu jx_linux_y, kem:
   - tan suat dia chi duoc CALL (de nhan dien API Lua)
   - chuoi ASCII ma ham tham chieu
Ket qua: port_3hd/bdump.txt  +  port_3hd/callstat.txt
"""
import io, os, re, sys, json, struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
HERE = os.path.dirname(os.path.abspath(__file__))
ELF = r"D:\ServerLinux\server1\jx_linux_y"

d = open(ELF, "rb").read()
e_phoff, = struct.unpack_from("<I", d, 0x1C)
e_phentsize, e_phnum = struct.unpack_from("<HH", d, 0x2A)
segs = []
for i in range(e_phnum):
    o = e_phoff + i * e_phentsize
    ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from("<8I", d, o)
    if ptype == 1 and filesz:
        segs.append((va, filesz, off))


def v2o(va):
    for v, sz, off in segs:
        if v <= va < v + sz:
            return off + (va - v)
    return None


def cstr(va, maxlen=90):
    o = v2o(va)
    if o is None:
        return None
    end = d.find(b"\x00", o, o + maxlen)
    if end < 0:
        return None
    s = d[o:end]
    if len(s) < 3:
        return None
    try:
        t = s.decode("ascii")
    except Exception:
        try:
            t = s.decode("gbk")
        except Exception:
            return None
    if sum(1 for c in t if 32 <= ord(c) < 127 or ord(c) > 127) < len(t) * 0.9:
        return None
    return t


md = Cs(CS_ARCH_X86, CS_MODE_32)
md.detail = False

raw = json.load(io.open(os.path.join(HERE, "api_gap_raw.json"), encoding="utf-8"))
targets = [(n, v["dia_chi_linux"]) for n, v in sorted(raw.items())
           if v["nhom"] == "B" and v["dia_chi_linux"]]
if len(sys.argv) > 1:                       # cho phep chi dinh ten cu the
    want = set(sys.argv[1:])
    targets = [t for t in targets if t[0] in want]

callstat = {}
out = io.open(os.path.join(HERE, "bdump.txt"), "w", encoding="utf-8")
for name, addr in targets:
    va = int(addr, 16)
    off = v2o(va)
    out.write("\n" + "=" * 78 + "\n== %s @ %s\n" % (name, addr))
    if off is None:
        out.write("   (dia chi ngoai tep)\n")
        continue
    n = 0
    for ins in md.disasm(d[off:off + 200 * 16], va):
        n += 1
        extra = ""
        if ins.mnemonic == "call" and ins.op_str.startswith("0x"):
            t = int(ins.op_str, 16)
            callstat[t] = callstat.get(t, 0) + 1
        for m in re.finditer(r"0x[0-9a-f]{6,8}", ins.op_str):
            s = cstr(int(m.group(0), 16))
            if s:
                extra += '   ; "%s"' % s
        out.write("0x%08X  %-20s %s %s%s\n" % (ins.address, ins.bytes.hex(), ins.mnemonic, ins.op_str, extra))
        if ins.mnemonic == "ret" and n > 6:
            # tiep tuc mot chut de bat cac nhanh sau
            if n > 150:
                break
        if n >= 200:
            break
out.close()

with io.open(os.path.join(HERE, "callstat.txt"), "w", encoding="utf-8") as f:
    for t, c in sorted(callstat.items(), key=lambda x: -x[1]):
        f.write("0x%08X  x%d\n" % (t, c))
print("bdump.txt + callstat.txt xong;", len(targets), "ham,", len(callstat), "dich call")
for t, c in sorted(callstat.items(), key=lambda x: -x[1])[:25]:
    print("  0x%08X  x%d" % (t, c))
