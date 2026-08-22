#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Doi chieu id NPC template / skill / item (6,1,N) / mat na particular giua Linux va du an THEO TEN."""
import io, sys, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
LNX = r"D:\ServerLinux\server1\settings"
PRJ = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings"
def vn(s):
    try: return s.encode("latin-1").decode("gbk", "replace")
    except Exception: return s
def rows(p):
    return [ln.rstrip("\r\n").split("\t") for ln in io.open(p, encoding="latin-1")]

print("=== NPC template (id = dong-2) ===")
ln, pr = rows(LNX + r"\npcs.txt"), rows(PRJ + r"\npcs.txt")
pbyname = collections.defaultdict(list)
for i, r in enumerate(pr[1:]):
    if r and r[0].strip(): pbyname[r[0].strip()].append(i)
for nid in [393, 625, 389, 55, 49, 629, 630, 626, 627, 628, 53]:
    r = ln[nid + 1] if nid + 1 < len(ln) else None
    name = r[0].strip() if r else "?"
    cand = pbyname.get(name, [])
    pname = pr[nid + 1][0].strip() if nid + 1 < len(pr) else "?"
    st = "TRUNG ID" if nid in cand else ("-> %s" % cand if cand else ">>> THIEU")
    print("  %-5d Linux=%-28s du an cung id=%-28s %s" % (nid, vn(name), vn(pname), st))

print("\n=== SKILL 661 cac cap (skills.txt) ===")
def skl(p):
    d = {}
    for r in rows(p)[1:]:
        if len(r) > 2 and r[2].strip().isdigit(): d[int(r[2])] = r
    return d
ls, ps = skl(LNX + r"\skills.txt"), skl(PRJ + r"\skills.txt")
for sid in (661, 1500, 1485, 990, 991, 976, 977):
    a, b = ls.get(sid), ps.get(sid)
    print("  %-5d Linux=%-40s du an=%-40s" % (sid, vn(a[0]) if a else "-", vn(b[0]) if b else "-"))
# cot MaxLevel cua skills.txt?
h = rows(PRJ + r"\skills.txt")[0]
for key in ("MaxLevel", "SkillType", "SkillStyle", "MaxSkillLevel"):
    if key in h:
        k = h.index(key)
        print("  661 %s: Linux=%s du an=%s" % (key, ls[661][k] if 661 in ls and k < len(ls[661]) else "?", ps[661][k] if 661 in ps and k < len(ps[661]) else "?"))
print("  header du an:", [vn(x) for x in h[:12]])

print("\n=== Mat na cam (particular, magicscript.txt) ===")
def ms(p):
    d = {}
    for r in rows(p):
        if len(r) > 4 and r[3].strip().isdigit() and r[1].strip() == "6" and r[2].strip() == "1": d[int(r[3])] = r
    return d
lm, pm = ms(LNX + r"\item\004\magicscript.txt"), ms(PRJ + r"\item\magicscript.txt")
def equips(p):
    """bang trang bi (mat na) Linux settings/item/...? dung other.txt / 'equip' tables: tim theo particular trong cac bang co cot Particular"""
    return None
for pid in (482, 447, 450, 446, 647, 806):
    print("  particular %d: magicscript Linux=%s" % (pid, vn(lm[pid][0]) if pid in lm else "(khong trong magicscript)"))

print("\n=== Item thuong ===")
pbyn = collections.defaultdict(list)
for k, r in pm.items(): pbyn[r[0].strip()].append(k)
for iid in (30438, 2264, 30301):
    r = lm.get(iid)
    print("  %-6d %-32s -> du an %s" % (iid, vn(r[0]) if r else "?", pbyn.get(r[0].strip()) if r else "?"))
