#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Doi chieu NPC template / skill / obj cua tongcastle giua 2 cay (theo TEN, id = dong-2)."""
import io, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import _UNICODE_TO_TCVN3
T2U = {ord(v): k for k, v in _UNICODE_TO_TCVN3.items()}
def tc(s): return "".join(T2U.get(ord(c), c) for c in s)
BS = chr(92)
LNX = "D:" + BS + "ServerLinux" + BS + "server1" + BS + "settings"
PRJ = "E:" + BS + "SourceTuanLe" + BS + "SourceVs22" + BS + "TESTLOFFF_ONLINE" + BS + "bin" + BS + "server" + BS + "settings"
def rows(p):
    ls = io.open(p, encoding="latin-1").read().split("\n")
    hdr = ls[0].rstrip("\r").split("\t")
    return hdr, [l.rstrip("\r").split("\t") for l in ls[1:]]
KEEP = ("Life", "Level", "LifeMax", "Kind", "Camp", "AiType", "AIType", "ReviveTime", "Exp", "DeathScript", "Script", "ScriptFile", "Series", "Name", "Attack", "Defend", "Attack1", "AttackRate", "MoveSpeed")
hL, rL = rows(os.path.join(LNX, "npcs.txt")); hP, rP = rows(os.path.join(PRJ, "npcs.txt"))
print("npcs.txt: Linux %d dong, du an %d dong" % (len(rL), len(rP)))
print("cols Linux:", hL[:14]); print("cols du an:", hP[:14])
ids = [108, 469, 1908, 1909, 1910, 1911, 1912, 1913, 1914]
for nid in ids:
    a = rL[nid] if nid < len(rL) else None; b = rP[nid] if nid < len(rP) else None
    print("id", nid)
    if a: print("   L:", tc(a[0]), [(hL[i], a[i]) for i in range(min(len(hL), len(a))) if hL[i] in KEEP][:12])
    if b: print("   P:", tc(b[0]), [(hP[i], b[i]) for i in range(min(len(hP), len(b))) if hP[i] in KEEP][:12])
    if a and b:
        print("   TRUNG BYTE DONG:", a == b, " TRUNG TEN:", a[0] == b[0], " TRUNG TRU TEN:", a[1:] == b[1:])
        if a[1:] != b[1:]:
            print("   KHAC o cot:", [(hL[i] if i < len(hL) else i, a[i], b[i]) for i in range(1, min(len(a), len(b))) if a[i] != b[i]][:20])
names = {rL[n][0]: n for n in ids if n < len(rL)}
for i, r in enumerate(rP):
    if r and r[0] in names and i != names[r[0]]: print("du an co ten", tc(r[0]), "o id", i, "(Linux", names[r[0]], ")")
# skill 93
for root in (LNX, PRJ):
    for cand in ("skills.txt", "skill" + BS + "skills.txt", "skills" + BS + "skills.txt"):
        p = os.path.join(root, cand)
        if os.path.isfile(p):
            for ln in io.open(p, encoding="latin-1"):
                c = ln.split("\t")
                if c[0].strip() == "93": print(p, "skill 93:", tc(c[1]), c[2:5]); break
            break
# obj 469 (AddObstacleObj(469,...)) -> settings\obj\objdata.txt ?
for p in (os.path.join(LNX, "obj" + BS + "objdata.txt"), os.path.join(PRJ, "obj" + BS + "ObjData.txt")):
    ls = io.open(p, encoding="latin-1").read().split("\n")
    print(p, "rows", len(ls), "hdr", ls[0][:160])
    for idx in (469, 470, 471):
        if idx - 1 < len(ls): print("   dong", idx, tc(ls[idx - 1][:160]))
