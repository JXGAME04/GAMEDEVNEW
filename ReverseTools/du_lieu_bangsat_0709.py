#!/usr/bin/env python
# -*- coding: ascii -*-
r"""
[BANGSAT2 07/09] Bang Cot Tuyet Tam (114, bang binggu_xuexin) ve Linux (server + client, sao luu *.truoc_bangsat_0709):
  D1 cuiyan.lua : coldenhance_p 8->80  => 10->140 ; deadlystrikeenhance_p 45 => 60 (Conic) ;
                  fasthitrecover_v 5->25 => 5->49 (Linux khai ban Duong fasthitrecover_yan_v 5->49, du an khong dung Duong -> gap vao Am)
  D2 skills.txt dong 114: LvlSetting6 sorbdamage_p (khong co du lieu Lua => 0) => fasthitrecover_v (LvlData6 van la binggu_xuexin)
  + guong git D:\GAMEDEVNEW\serverscript_live\script\nhanvat\kynang\cuiyan.lua
"""
import os, shutil

BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
SV = os.path.join(BIN, "server"); CL = os.path.join(BIN, "client")
MIRROR = r"D:\GAMEDEVNEW\serverscript_live\script\nhanvat\kynang"
SUF = ".truoc_bangsat_0709"

def backup(p):
    b = p + SUF
    if not os.path.exists(b): shutil.copy2(p, b)
def hb(b): return sum(1 for x in b if x > 127)
def edit(p, fn, name):
    raw = open(p, "rb").read(); new = fn(raw)
    if new == raw: raise SystemExit("%s: khong doi gi (%s)" % (p, name))
    backup(p); open(p, "wb").write(new)
    print("OK", name, p[len(BIN):], "high", hb(raw), "->", hb(new))

REPL = [
    (b"coldenhance_p={{{1,8},{30,80}},{{1,-1},{2,-1}}}",
     b"coldenhance_p={{{1,10},{30,140}},{{1,-1},{2,-1}}}--[BANGSAT2 07/09] nhu Linux (thoi gian bang = khung)"),
    (b"deadlystrikeenhance_p={{{1,5},{30,45,Conic}},{{1,-1},{2,-1}}},",
     b"deadlystrikeenhance_p={{{1,5},{30,60,Conic}},{{1,-1},{2,-1}}},--[BANGSAT2 07/09] nhu Linux"),
    (b"fasthitrecover_v={{{1,5},{30,25},{31,25}},{{1,-1},{2,-1}}},",
     b"fasthitrecover_v={{{1,5},{30,49},{31,49}},{{1,-1},{2,-1}}},--[BANGSAT2 07/09] nhu Linux (fasthitrecover_yan_v gap vao Am)"),
]
def d1(raw):
    i = raw.find(b"binggu_xuexin={")
    if i < 0: raise SystemExit("cuiyan: khong thay bang")
    for old, new in REPL:
        if raw.count(old) != 1: raise SystemExit("cuiyan: neo x %d: %s" % (raw.count(old), old[:30]))
        raw = raw.replace(old, new)
    return raw
for p in (os.path.join(SV, "script", "nhanvat", "kynang", "cuiyan.lua"), os.path.join(CL, "script", "skill", "cuiyan.lua")):
    edit(p, d1, "D1")

def d2(raw):
    lines = raw.split(b"\n")
    h = [x.strip() for x in lines[0].rstrip(b"\r").split(b"\t")]
    k = h.index(b"SkillId"); i6 = h.index(b"LvlSetting6"); j6 = h.index(b"LvlData6")
    done = 0
    for n, l in enumerate(lines):
        crlf = l.endswith(b"\r"); c = l.rstrip(b"\r").split(b"\t")
        if len(c) > k and c[k].strip() == b"114":
            if c[i6].strip() != b"sorbdamage_p" or c[j6].strip() != b"binggu_xuexin":
                raise SystemExit("skills.txt 114: o 6 = %r / %r" % (c[i6], c[j6]))
            c[i6] = b"fasthitrecover_v"
            lines[n] = b"\t".join(c) + (b"\r" if crlf else b""); done += 1
    if done != 1: raise SystemExit("skills.txt: dong 114 x %d" % done)
    return b"\n".join(lines)
for p in (os.path.join(SV, "settings", "skills.txt"), os.path.join(CL, "settings", "skills.txt")):
    edit(p, d2, "D2")

shutil.copy2(os.path.join(SV, "script", "nhanvat", "kynang", "cuiyan.lua"), os.path.join(MIRROR, "cuiyan.lua"))
print("guong", os.path.join(MIRROR, "cuiyan.lua")); print("XONG")
