# -*- coding: utf-8 -*-
"""t79_disasm_linux_dropitem_team.py - mo 4 ham Lua cua jx_linux_y de chot "dung chuan Linux":

    DropItem      0x081200B0   (nhom A - bi COI la trung JX1 ma chua ai kiem chu ky;
                                yandibaozang\\npc_death.lua goi kieu 16 tham so toa do)
    GetTeamSize   0x08115480   (saizi.lua dung de moi nguoi gieo xuc xac)
    GetTeamMember 0x08115530
    LeaveTeam     (tra dia chi tu bang dang ky - ready.lua:243 goi truoc tran)

Kem DropItemEx 0x0811FD70 lam "hon da Rosetta": chu ky da CHAC (19 tham so, doc tu
droptemplet.lua) nen cac dia chi callee lap lai cua no = lua_gettop / lua_isnumber /
lua_tonumber / lua_pushnumber..., dung de dan nhan cho 4 ham kia.

Chi DOC file. Ket qua ghi: port_3hd/t79_dump.txt
"""
import io
import os
import struct
import sys

from capstone import Cs, CS_ARCH_X86, CS_MODE_32

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ELF = r"D:\ServerLinux\server1\jx_linux_y"
OUT = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\t79_dump.txt"

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


def cstr(va, maxlen=120):
    o = v2o(va)
    if o is None:
        return None
    end = d.find(b"\x00", o, o + maxlen)
    if end < 0:
        return None
    try:
        return d[o:end].decode("ascii")
    except UnicodeDecodeError:
        return None


def tim_dangky(ten):
    """Quet moi cap {char* ten, void* fn} trong cac doan LOAD."""
    hits = []
    for v, sz, off in segs:
        end = off + sz - 8
        for o in range(off, end, 4):
            pname, pfn = struct.unpack_from("<II", d, o)
            if not (0x8048000 <= pname < 0x8400000):
                continue
            if not (0x8048000 <= pfn < 0x82da338):
                continue
            s = cstr(pname, 64)
            if s == ten:
                hits.append((v + (o - off), pfn))
    return hits


md = Cs(CS_ARCH_X86, CS_MODE_32)
md.detail = False


def disasm(va, ten, maxbytes=2048):
    o = v2o(va)
    ra = []
    ra.append("=" * 70)
    ra.append("%s @ 0x%08X (file 0x%X)" % (ten, va, o))
    ra.append("=" * 70)
    calls = {}
    n = 0
    for ins in md.disasm(d[o:o + maxbytes], va):
        line = "0x%08X  %-8s %s" % (ins.address, ins.mnemonic, ins.op_str)
        # chu thich: hang so tro vao vung doc duoc -> thu doc chuoi
        if ins.mnemonic == "push" and ins.op_str.startswith("0x"):
            s = cstr(int(ins.op_str, 16))
            if s:
                line += '    ; "%s"' % s
        if ins.mnemonic == "call" and ins.op_str.startswith("0x"):
            t = int(ins.op_str, 16)
            calls[t] = calls.get(t, 0) + 1
        ra.append(line)
        n += 1
        # dung o ret dau tien sau >= 10 lenh (ham nho); van in du neu chua gap
        if ins.mnemonic == "ret" and n >= 10:
            break
    ra.append("")
    ra.append("-- thong ke call cua %s:" % ten)
    for t in sorted(calls):
        ra.append("   call 0x%08X  x%d" % (t, calls[t]))
    ra.append("")
    return "\n".join(ra), calls


def main():
    out = []
    # 1) tra dia chi tu bang dang ky
    out.append("### Tra bang dang ky (ten -> dia chi ham):")
    diachi = {}
    for ten in ["DropItem", "DropItemEx", "GetTeamSize", "GetTeamMember", "LeaveTeam",
                "ApplyItemDice", "RollItem", "GetNpcPos"]:
        hits = tim_dangky(ten)
        for vaddr, fn in hits:
            out.append("  %-14s bang@0x%08X -> fn 0x%08X" % (ten, vaddr, fn))
            diachi.setdefault(ten, fn)
        if not hits:
            out.append("  %-14s KHONG THAY trong bang dang ky" % ten)
    out.append("")

    # 2) disasm
    for ten, fallback in [("DropItemEx", 0x0811FD70), ("DropItem", 0x081200B0),
                          ("GetTeamSize", 0x08115480), ("GetTeamMember", 0x08115530),
                          ("LeaveTeam", None), ("GetNpcPos", None)]:
        va = diachi.get(ten, fallback)
        if va is None:
            out.append("!!! %s: khong co dia chi, bo qua" % ten)
            continue
        if fallback is not None and va != fallback:
            out.append("!!! CHU Y: %s bang dang ky = 0x%08X khac ghi chep cu 0x%08X"
                       % (ten, va, fallback))
        text, calls = disasm(va, ten)
        out.append(text)

    io.open(OUT, "w", encoding="utf-8", newline="\n").write("\n".join(out))
    print("da ghi %s (%d dong)" % (OUT, len(out)))
    # in phan tra bang ra man hinh cho nhanh
    for l in out[:20]:
        print(l)
    return 0


if __name__ == "__main__":
    sys.exit(main())
