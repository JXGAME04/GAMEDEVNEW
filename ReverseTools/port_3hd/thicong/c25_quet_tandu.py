# -*- coding: utf-8 -*-
"""C25 - QUET TAN DU du an cu theo TUNG LOI GOI HAM (bai hoc tu C24: spawner cu
nam o startgame\\khac\\satthu.lua chu khong phai tinhnang\\boss_satthu\\ => grep
theo TEN HE khong bat duoc).

Cach lam:
  1) Lay MOI loi goi ham dang SONG trong startgame.lua (OnGame) + timerserver.lua
     (RunTime) - bo dong comment.
  2) Truy dinh nghia tung ham trong toan cay script.
  3) Trong than ham, tim moi AddNpc*/NewWorld/JoinMission/... co MAP thuoc 3 hoat
     dong: PLD 336-339, Vuot Ai 464-495 + 957, boss sat thu (40 map trong bang
     addkillertasknpc cua killbosshead).
  4) In bao cao: ham nao con dung toi map cua 3 hoat dong.
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
SCRIPT = SRV + r"\script"

# ---------- map cua 3 hoat dong ----------
kb = io.open(SRV + r"\script\task\tollgate\killbosshead.lua", encoding="latin-1", newline="").read().split("\n")
st = next(i for i, l in enumerate(kb) if l.startswith("addkillertasknpc"))
bossmaps = set()
for l in kb[st:st + 400]:
    m = re.match(r"\s*\{(\d+),(\d+),(\d+),(\d+),(\d+),(\d+),\"", l)
    if m:
        bossmaps.add(int(m.group(3)))
    if l.startswith("}"):
        break
PLD = set(range(336, 340))
VA = set(range(464, 496)) | {957}
HD3MAP = PLD | VA | bossmaps
print("map 3 hoat dong: PLD %d + VuotAi %d + boss %d = %d map"
      % (len(PLD), len(VA), len(bossmaps), len(HD3MAP)))

# ---------- gom toan bo dinh nghia ham trong cay ----------
defs = {}       # ten ham -> [(duong dan, dong bat dau)]
files = {}
for dp, dn, fs in os.walk(SCRIPT):
    for f in fs:
        if not f.endswith(".lua"):
            continue
        p = os.path.join(dp, f)
        try:
            txt = io.open(p, encoding="latin-1", newline="").read()
        except Exception:
            continue
        files[p] = txt.split("\n")
        for i, l in enumerate(files[p]):
            m = re.match(r"\s*function\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(", l)
            if m:
                defs.setdefault(m.group(1), []).append((p, i))
print("tong ham dinh nghia trong cay:", len(defs))


def than_ham(p, i0):
    """tra ve cac dong tu 'function' den 'end' cung cap (xap xi: den dong 'end' o
    cot 0 dau tien)"""
    out = []
    for l in files[p][i0:i0 + 800]:
        out.append(l)
        if l.rstrip("\r").rstrip() == "end" or l.startswith("end"):
            break
    return out


def quet_than(lines):
    """tim map cua 3 hoat dong trong than ham"""
    hit = set()
    for l in lines:
        s = l.lstrip()
        if s.startswith("--"):
            continue
        # AddNpcNew(id, lv, MAP, ...) / AddNpcEx1({id},lv,{s},MAP,...) / NewWorld(MAP,..)
        for m in re.finditer(r"(AddNpcNew|AddNpc|AddNpcEx\d?|HD3_AddNpc\w*|NewWorld|AddTrap\w*)\s*\(([^)]*)", s):
            args = m.group(2)
            for num in re.findall(r"(?<![\w.])(\d{1,4})(?![\w.])", args):
                n = int(num)
                if n in HD3MAP:
                    hit.add((m.group(1), n))
    return hit


# ---------- loi goi SONG trong 2 tep boot ----------
bao_cao = []
for ten_tep in ("startgame.lua", "timerserver.lua"):
    p = os.path.join(SCRIPT, ten_tep)
    lines = files[p]
    goi = []
    for i, l in enumerate(lines):
        s = l.lstrip()
        if s.startswith("--"):
            continue
        m = re.match(r"([A-Za-z_][A-Za-z0-9_]*)\s*\(", s)
        if m and m.group(1) not in ("if", "for", "while", "function", "return", "print",
                                    "Include", "IncludeLib", "format", "tonumber", "getn"):
            goi.append((i + 1, m.group(1)))
    print("\n=== %s: %d loi goi ham SONG ===" % (ten_tep, len(goi)))
    for dong, ten in goi:
        if ten not in defs:
            continue
        for (fp, i0) in defs[ten]:
            hit = quet_than(than_ham(fp, i0))
            if hit:
                rel = os.path.relpath(fp, SRV)
                bao_cao.append((ten_tep, dong, ten, rel, sorted(hit)))
                print("  ⚠ dong %-4d %-28s -> %s" % (dong, ten + "()", rel))
                for h in sorted(hit)[:6]:
                    print("        %s(... %d ...)" % h)

print("\n================ KET LUAN ================")
if not bao_cao:
    print("KHONG con loi goi SONG nao cham toi map cua 3 hoat dong.")
else:
    print("Co %d ham can xem lai (o tren)." % len(bao_cao))
