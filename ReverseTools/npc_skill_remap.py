#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Doi chieu NPC template id + skill id mot cay script Linux sang du an THEO TEN.

  npc_skill_remap.py <thu-muc-Linux-tuong-doi> [thu-muc2 ...]
NPC: bat AddNpc(  AddNpcEx(  AddNpcNew( ... tham so 1 la template; GetNpcSettingIdx(..) == N ;
     bang npcs.txt: cot 0 = id? -> dung cot 'Name'/ten tieng Viet de doi chieu (id = dong-2 o du an).
Skill: AddNpcSkillState(idx, SKILL, ...), AddSkillState(SKILL, ...), CastSkill(SKILL ...) -> skills.txt.
"""
import io, os, re, sys, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
LNX = r"D:\ServerLinux\server1"
PRJ = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"


def vn(s):
    try:
        return s.encode("latin-1").decode("gbk", "replace")
    except Exception:
        return s


def load_tab(p, idcol, namecol):
    d = {}
    for i, ln in enumerate(io.open(p, encoding="latin-1")):
        c = ln.rstrip("\r\n").split("\t")
        if i == 0 or len(c) <= max(idcol, namecol):
            continue
        try:
            d[int(c[idcol])] = c[namecol].strip()
        except ValueError:
            pass
    return d


def hdr(p):
    return io.open(p, encoding="latin-1").readline().rstrip("\r\n").split("\t")


def scan(roots, pats):
    hits = collections.defaultdict(list)
    for R in roots:
        base = os.path.join(LNX, "script", R.replace("/", "\\"))
        walk = [(os.path.dirname(base), None, [os.path.basename(base)])] if os.path.isfile(base) else os.walk(base)
        for dp, _, fs in walk:
            for f in fs:
                if not f.lower().endswith(".lua"):
                    continue
                p = os.path.join(dp, f)
                for i, line in enumerate(io.open(p, encoding="latin-1").read().split("\n"), 1):
                    if line.lstrip().startswith("--"):
                        continue
                    for pat in pats:
                        for m in pat.finditer(line):
                            hits[int(m.group(1))].append("%s:%d" % (os.path.relpath(p, os.path.join(LNX, "script")).replace("\\", "/"), i))
    return hits


def report(title, hits, ltab, ptab):
    byname = collections.defaultdict(list)
    for i, n in ptab.items():
        byname[n].append(i)
    print("\n=== %s: %d id ===" % (title, len(hits)))
    print("%-7s %-34s %-14s %s" % ("Linux", "TEN", "DU AN", "dung o"))
    for i in sorted(hits):
        n = ltab.get(i)
        if n is None:
            st, name = "(khong co trong bang Linux)", ""
        else:
            name = n
            cand = byname.get(n, [])
            if not cand:
                st = ">>> THIEU <<<"
            elif i in cand:
                st = "TRUNG ID" + ("" if cand == [i] else " (+%s)" % ",".join(str(x) for x in cand if x != i))
            else:
                st = "-> " + ",".join(str(x) for x in cand)
        print("%-7d %-34s %-14s %s (x%d)" % (i, vn(name)[:34], st, hits[i][0], len(hits[i])))


def main():
    roots = sys.argv[1:]
    # npcs.txt
    lp, pp = os.path.join(LNX, r"settings\npcs.txt"), os.path.join(PRJ, r"settings\npcs.txt")
    lh, ph = hdr(lp), hdr(pp)
    print("npcs.txt header Linux:", [vn(x) for x in lh[:4]], "| du an:", [vn(x) for x in ph[:4]])
    npc_pats = [re.compile(r"AddNpc(?:Ex|New)?\s*\(\s*(\d+)"), re.compile(r"GetNpcSettingIdx\s*\([^)]*\)\s*==\s*(\d+)"),
                re.compile(r"nNpcId\s*==\s*(\d+)"), re.compile(r"AddNpc_\w+\s*\(\s*(\d+)")]
    hits = scan(roots, npc_pats)
    # Linux npcs.txt: cot 0 = Name? cot 1 = id? -> tu phat hien: tim cot so nguyen tang dan
    def detect(p):
        rows = [ln.rstrip("\r\n").split("\t") for ln in io.open(p, encoding="latin-1")][1:6]
        for c in range(0, 4):
            try:
                v = [int(r[c]) for r in rows]
                if v == sorted(v):
                    return c
            except Exception:
                pass
        return None
    lid, pid = None, None   # ca hai cay: id = dong-2 (da xac minh 21/08)
    print("cot id Linux=%s du an=%s (None = id theo dong-2)" % (lid, pid))
    def load_npc(p, idc):
        d = {}
        for i, ln in enumerate(io.open(p, encoding="latin-1")):
            c = ln.rstrip("\r\n").split("\t")
            if i == 0 or len(c) < 2:
                continue
            key = int(c[idc]) if idc is not None and c[idc].strip().lstrip("-").isdigit() else i - 1
            name = c[0].strip() if idc != 0 else c[1].strip()
            d[key] = name
        return d
    report("NPC template", hits, load_npc(lp, lid), load_npc(pp, pid))

    # skills
    ls, ps = os.path.join(LNX, r"settings\skills.txt"), os.path.join(PRJ, r"settings\skills.txt")
    sk_pats = [re.compile(r"AddNpcSkillState\s*\(\s*[^,]+,\s*(\d+)"), re.compile(r"AddSkillState\s*\(\s*(\d+)"),
               re.compile(r"CastSkill\s*\(\s*(\d+)"), re.compile(r"RemoveSkillState\s*\(\s*(\d+)"),
               re.compile(r"SetNpcSkill\w*\s*\(\s*[^,]+,\s*(\d+)")]
    hits = scan(roots, sk_pats)
    def load_sk(p):
        d = {}
        h = hdr(p)
        for i, ln in enumerate(io.open(p, encoding="latin-1")):
            c = ln.rstrip("\r\n").split("\t")
            if i == 0 or len(c) < 2:
                continue
            if len(c) > 2 and c[2].strip().isdigit():   # skills.txt: SkillName | Property | SkillId
                d[int(c[2])] = c[0].strip()
        return d
    report("SKILL", hits, load_sk(ls), load_sk(ps))


main()
