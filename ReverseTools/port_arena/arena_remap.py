# -*- coding: utf-8 -*-
# Doi chieu NPC template (108/625/203) + 94 skill-state id trong vnforbidstate.lua
# + shop Sale(53) cua cay missions/arena sang bang du an THEO TEN.
import io, os, re, sys, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools")
from gbktool import decline
LNX = r"D:\ServerLinux\server1"
PRJ = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

def load_rows(p):
    return [ln.rstrip(b"\r\n").split(b"\t") for ln in open(p, "rb")]

def npc_tab(p):
    rows = load_rows(p)
    return {i - 1: decline(r[0]).strip() for i, r in enumerate(rows) if i >= 1 and len(r) > 1}

def skill_tab(p):
    rows = load_rows(p)
    hdr = [decline(x) for x in rows[0]]
    # tim cot id va cot ten
    idc = next((i for i, h in enumerate(hdr) if h.strip().lower() in ("skillid", "id", "magicid")), 0)
    namec = next((i for i, h in enumerate(hdr) if "name" in h.lower() or h.strip() in ("技能名称", "Name")), 1)
    d = {}
    for r in rows[1:]:
        if len(r) <= max(idc, namec): continue
        try: d[int(r[idc])] = decline(r[namec]).strip()
        except ValueError: pass
    return d, hdr[idc], hdr[namec]

def report(title, ids, ltab, ptab, note=None):
    byname = collections.defaultdict(list)
    for i, n in ptab.items():
        byname[n].append(i)
    print("\n=== %s: %d id ===" % (title, len(ids)))
    print("%-7s %-34s %-22s %s" % ("Linux", "TEN Linux", "DU AN", "ghi chu"))
    for i, tag in ids:
        n = ltab.get(i)
        if n is None:
            st = "(khong co trong bang Linux)"; n = ""
        else:
            cand = byname.get(n, [])
            if not cand: st = ">>> THIEU <<<"
            elif i in cand: st = "TRUNG ID" + ("" if cand == [i] else " (+%s)" % ",".join(str(x) for x in cand if x != i))
            else: st = "-> " + ",".join(str(x) for x in cand)
        print("%-7d %-34s %-22s %s | du an[%d]=%s" % (i, n[:34], st, tag, i, ptab.get(i, "")[:30]))

# NPC
ln, pn = npc_tab(os.path.join(LNX, r"settings\npcs.txt")), npc_tab(os.path.join(PRJ, r"settings\npcs.txt"))
report("NPC template", [(108, "officer: autoexec_npc.lua:221"), (625, "rule.lua:25 Ruong chua do"), (203, "rule.lua:26 Chu duoc diem")], ln, pn)

# SKILL (vnforbidstate.lua)
ls, lh1, lh2 = skill_tab(os.path.join(LNX, r"settings\skills.txt"))
ps, ph1, ph2 = skill_tab(os.path.join(PRJ, r"settings\skills.txt"))
print("\nskills.txt cot id/ten: Linux=(%s,%s) du an=(%s,%s)" % (lh1, lh2, ph1, ph2))
src = open(os.path.join(LNX, r"script\missions\arena\vnforbidstate.lua"), "rb").read()
ids = []
for i, line in enumerate(src.split(b"\n"), 1):
    m = re.search(rb'\{\s*"[^"]*"\s*,\s*(\d+)\s*\}', line)
    if m: ids.append((int(m.group(1)), "vnforbidstate.lua:%d %s" % (i, decline(re.search(rb'"([^"]*)"', line).group(1)))))
report("SKILL state id (RemoveSkillState)", ids, ls, ps)

# SHOP
def shop_rows(p):
    return [decline(r[0]) if r else "" for r in load_rows(p)]
lr, pr = load_rows(os.path.join(LNX, r"settings\buysell.txt")), load_rows(os.path.join(PRJ, r"settings\buysell.txt"))
print("\n=== SHOP Sale(53) -> BuySell.OpenSale(id-1=52) : Linux dong %d (%d dong), du an dong %d (%d dong) ===" % (54, len(lr), 54, len(pr)))
print("Linux dong 54 (10 item dau):", [decline(x) for x in lr[53][:10]] if len(lr) > 53 else None)
print("du an dong 54:", [decline(x) for x in pr[53][:12]] if len(pr) > 53 else None)
print("Sale(175,16): Linux buysell co %d dong -> shop 175 KHONG TON TAI ca tren Linux" % len(lr))
