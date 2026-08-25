# -*- coding: utf-8 -*-
"""Suy CHU KY ham Lua C (Lua 4.0) tu ma may i386 cua jx_linux_y.

Da xac dinh cac API Lua bang cach doc truc tiep than ham:
  0x08232490 lua_gettop(L)           -> (L->top - L->stack)/12
  0x082324B0 lua_gettop_out(L,&n)
  0x08232D40 lua_pushnumber(L,double)-> dat tag 2 (LUA_TNUMBER)
  0x082338B0 lua_tonumber(L,idx)     -> fld qword, kiem tag 2
  0x08233850 lua_tostring(L,idx)     -> kiem tag 3 (LUA_TSTRING), tra str+0x14
  0x082337A0 lua_pushstring(L,char*) -> strlen + pushlstring
"""
import io, os, re, sys, json, struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
HERE = os.path.dirname(os.path.abspath(__file__))
ELF = r"D:\ServerLinux\server1\jx_linux_y"

API = {
    0x08232490: "lua_gettop",
    0x082324B0: "lua_gettop2",
    0x08232D40: "lua_pushnumber",
    0x082338B0: "lua_tonumber",
    0x08233850: "lua_tostring",
    0x082337A0: "lua_pushstring",
    0x08233360: "lua_gettable_ish",
    0x08232E70: "lua_push?_E70",
    0x08232BE0: "lua_?_BE0",
    0x08232590: "lua_?_590",
    0x082324D0: "lua_?_4D0",
}

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
    if len(s) < 4:
        return None
    try:
        t = s.decode("ascii")
    except Exception:
        return None
    if not all(32 <= ord(c) < 127 or c in "\t" for c in t):
        return None
    return t


md = Cs(CS_ARCH_X86, CS_MODE_32)
md.detail = False

raw = json.load(io.open(os.path.join(HERE, "api_gap_raw.json"), encoding="utf-8"))
targets = [(n, v["dia_chi_linux"]) for n, v in sorted(raw.items())
           if v["nhom"] == "B" and v["dia_chi_linux"]]
if len(sys.argv) > 1:
    want = set(sys.argv[1:])
    targets = [t for t in targets if t[0] in want]

ESP4 = re.compile(r"^dword ptr \[esp \+ 4\], (0x[0-9a-f]+|\d+)$")
ESP0 = re.compile(r"^dword ptr \[esp\], (0x[0-9a-f]+|\d+)$")
ESP8 = re.compile(r"^dword ptr \[esp \+ 8\], (0x[0-9a-f]+|\d+)$")

report = {}
for name, addr in targets:
    va = int(addr, 16)
    off = v2o(va)
    ins_list = list(md.disasm(d[off:off + 260 * 16], va))[:260]
    # cat tai ret cuoi cung truoc mot 'push ebp' moi (prologue ham ke tiep)
    cut = len(ins_list)
    seen_ret = False
    for i, ins in enumerate(ins_list):
        if ins.mnemonic == "ret":
            seen_ret = True
        elif seen_ret and ins.mnemonic == "push" and ins.op_str == "ebp" and i > 8:
            # kiem tra mov ebp,esp ngay sau
            if i + 1 < len(ins_list) and ins_list[i + 1].mnemonic == "mov" \
               and ins_list[i + 1].op_str == "ebp, esp":
                cut = i
                break
        if ins.mnemonic != "ret":
            pass
    ins_list = ins_list[:cut]

    params = []       # (idx, kieu)
    pushes = []
    apicalls = []
    strs = []
    rets = set()
    last_esp4 = None
    for i, ins in enumerate(ins_list):
        if ins.mnemonic == "mov":
            m = ESP4.match(ins.op_str)
            if m:
                last_esp4 = int(m.group(1), 0)
            for rx in (ESP0, ESP4, ESP8):
                mm = rx.match(ins.op_str)
                if mm:
                    s = cstr(int(mm.group(1), 0))
                    if s:
                        strs.append(s)
        if ins.mnemonic == "push":
            if ins.op_str.startswith("0x"):
                s = cstr(int(ins.op_str, 16))
                if s:
                    strs.append(s)
        if ins.mnemonic == "call" and ins.op_str.startswith("0x"):
            t = int(ins.op_str, 16)
            if t in API:
                fn = API[t]
                apicalls.append((ins.address, fn, last_esp4))
                if fn == "lua_tonumber" and last_esp4 is not None:
                    params.append((last_esp4, "number"))
                elif fn == "lua_tostring" and last_esp4 is not None:
                    params.append((last_esp4, "string"))
                elif fn in ("lua_pushnumber", "lua_pushstring"):
                    pushes.append(fn)
        if ins.mnemonic in ("mov", "xor") and ins.op_str.startswith("eax"):
            if ins.mnemonic == "xor" and ins.op_str == "eax, eax":
                pend = 0
            else:
                mm = re.match(r"^eax, (0x[0-9a-f]+|\d+)$", ins.op_str)
                pend = int(mm.group(1), 0) if mm else None
            if pend is not None:
                # tim 'ret' trong 8 lenh ke tiep ma khong co call/jmp xa
                for j in range(i + 1, min(i + 9, len(ins_list))):
                    if ins_list[j].mnemonic == "ret":
                        rets.add(pend)
                        break
                    if ins_list[j].mnemonic in ("call",):
                        break

    # gom tham so theo chi so
    pmap = {}
    for idx, ty in params:
        pmap.setdefault(idx, set()).add(ty)
    sig = ", ".join("p%d:%s" % (k, "/".join(sorted(v))) for k, v in sorted(pmap.items()))
    report[name] = {
        "addr": addr,
        "params": {str(k): sorted(v) for k, v in sorted(pmap.items())},
        "gettop": any(a[1].startswith("lua_gettop") for a in apicalls),
        "push": {p: pushes.count(p) for p in set(pushes)},
        "retvals": sorted(rets),
        "strings": sorted(set(strs))[:8],
        "ninsn": len(ins_list),
    }
    print("%-26s %s  n=%-4d gettop=%-5s params[%s]  push=%s  ret=%s  %s"
          % (name, addr, len(ins_list), report[name]["gettop"], sig,
             report[name]["push"], sorted(rets), ("| " + " | ".join(sorted(set(strs))[:4])) if strs else ""))

json.dump(report, io.open(os.path.join(HERE, "bsig.json"), "w", encoding="utf-8"),
          ensure_ascii=False, indent=1, sort_keys=True)
