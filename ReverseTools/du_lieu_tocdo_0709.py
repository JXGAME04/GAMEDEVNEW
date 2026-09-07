#!/usr/bin/env python
# -*- coding: ascii -*-
r"""
[TOCDO 07/09] Du lieu toc do danh nhu Linux (cay chay that server + client), sao luu *.truoc_tocdo_0709:
  D1 BaseValue.ini x4        : AttackFrame 17 -> 18, CastFrame 17 -> 18 (Linux basevalue.ini)
  D2 skills.txt x2 dong 75   : LvlSetting5 = castspeed_v, LvlSetting6 = attackspeed_v (Ngu Doc Ky Kinh)
  D3 wudu.lua x2 wudu_qijing : castspeed_v = max(1->80, Duong 32) gap vao Am; attackspeed_v = 32 (Duong Linux gap vao Am)
  D4 kunlun.lua x2 qihan_aoxue: thoi gian giam toc 18*45..18*120 -> 18*20..18*60 nhu Linux (giu gia tri Am -6..-50)
  + guong git D:\GAMEDEVNEW\serverscript_live\script\nhanvat\kynang\{wudu,kunlun}.lua
"""
import os, shutil, sys

BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
SV = os.path.join(BIN, "server"); CL = os.path.join(BIN, "client")
MIRROR = r"D:\GAMEDEVNEW\serverscript_live\script\nhanvat\kynang"
SUF = ".truoc_tocdo_0709"

def backup(p):
    b = p + SUF
    if not os.path.exists(b):
        shutil.copy2(p, b)

def hb(b):
    return sum(1 for x in b if x > 127)

def edit(p, fn, name):
    raw = open(p, "rb").read()
    new = fn(raw)
    if new == raw:
        raise SystemExit("%s: khong doi gi (%s)" % (p, name))
    backup(p)
    open(p, "wb").write(new)
    print("OK", name, p[len(BIN):], "high", hb(raw), "->", hb(new))

# D1 ini
def d1(raw):
    if raw.count(b"AttackFrame=17") != 1 or raw.count(b"CastFrame=17") != 1:
        raise SystemExit("ini: neo lech")
    return raw.replace(b"AttackFrame=17", b"AttackFrame=18").replace(b"CastFrame=17", b"CastFrame=18")
for p in (os.path.join(SV, "settings", "npc", "player", "BaseValue.ini"),
          os.path.join(SV, "settings", "player", "BaseValue.ini"),
          os.path.join(CL, "settings", "npc", "player", "BaseValue.ini"),
          os.path.join(CL, "settings", "player", "BaseValue.ini")):
    edit(p, d1, "D1")

# D2 skills.txt
def d2(raw):
    lines = raw.split(b"\n")
    h = [x.strip() for x in lines[0].rstrip(b"\r").split(b"\t")]
    k = h.index(b"SkillId"); i5 = h.index(b"LvlSetting5"); i6 = h.index(b"LvlSetting6")
    done = 0
    for n, l in enumerate(lines):
        crlf = l.endswith(b"\r")
        c = l.rstrip(b"\r").split(b"\t")
        if len(c) > k and c[k].strip() == b"75":
            if c[i5].strip() != b"" or c[i6].strip() != b"":
                raise SystemExit("skills.txt dong 75: o 5/6 khong trong")
            c[i5] = b"castspeed_v"; c[i6] = b"attackspeed_v"
            lines[n] = b"\t".join(c) + (b"\r" if crlf else b"")
            done += 1
    if done != 1: raise SystemExit("skills.txt: dong 75 x %d" % done)
    return b"\n".join(lines)
for p in (os.path.join(SV, "settings", "skills.txt"), os.path.join(CL, "settings", "skills.txt")):
    edit(p, d2, "D2")

# D3 wudu.lua
A3 = b"\t\tdeadlystrikeenhance_p={{{1,4},{30,45}},{{1,-1},{2,-1}}},\r\n"
N3 = A3 + (b"\t\tcastspeed_v={{{1,32},{16,32},{17,33.41},{40,80}},{{1,-1},{30,-1}}},--[TOCDO 07/09] nhu Linux: max(castspeed_v 1->80, Duong 32) gap vao Am\r\n"
           b"\t\tattackspeed_v={{{1,32},{40,32}},{{1,-1},{30,-1}}},--[TOCDO 07/09] nhu Linux: attackspeed Duong 32 gap vao Am\r\n")
def d3(raw):
    i = raw.find(b"wudu_qijing={")
    if i < 0: raise SystemExit("wudu: khong thay bang")
    j = raw.find(A3, i)
    if j < 0 or raw.count(A3) != 1: raise SystemExit("wudu: neo x %d" % raw.count(A3))
    return raw[:j] + N3 + raw[j + len(A3):]
for p in (os.path.join(SV, "script", "nhanvat", "kynang", "wudu.lua"), os.path.join(CL, "script", "skill", "wudu.lua")):
    edit(p, d3, "D3")

# D4 kunlun.lua
A4 = b"castspeed_v={{{1,-6},{20,-39},{30,-50},{31,-50}},{{1,18*45},{20,18*120}}},"
N4 = b"castspeed_v={{{1,-6},{20,-39},{30,-50},{31,-50}},{{1,18*20},{20,18*60}}},--[TOCDO 07/09] thoi gian nhu Linux"
def d4(raw):
    if raw.count(A4) != 1: raise SystemExit("kunlun: neo x %d" % raw.count(A4))
    return raw.replace(A4, N4)
for p in (os.path.join(SV, "script", "nhanvat", "kynang", "kunlun.lua"), os.path.join(CL, "script", "skill", "kunlun.lua")):
    edit(p, d4, "D4")

# guong git
for name in ("wudu.lua", "kunlun.lua"):
    src = os.path.join(SV, "script", "nhanvat", "kynang", name)
    dst = os.path.join(MIRROR, name)
    shutil.copy2(src, dst)
    print("guong", dst)
print("XONG")
