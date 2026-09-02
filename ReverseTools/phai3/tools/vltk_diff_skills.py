# -*- coding: utf-8 -*-
"""vltk_diff_skills.py - so cot skills.txt / missles.txt cua JX1 (cay chay that) voi ban VLTK Level Up (vltk_raw)
cho ky nang 3 phai (Hoa Son 1363-1384, Vu Hon 1965-1991, Tieu Dao 2114-2143). In moi khac biet theo TEN COT.
DUNG: python vltk_diff_skills.py [--all] (mac dinh chi in cot 'quan trong': pham vi / dan / thoi gian)
"""
import io, sys, os, re, importlib.util

sys.stdout.reconfigure(encoding="utf-8")
spec = importlib.util.spec_from_file_location("vn_edit", r"C:/Users/nguye/.claude/skills/swordonline-dev/scripts/vn_edit.py")
vn = importlib.util.module_from_spec(spec); spec.loader.exec_module(vn)
T2U = vn._TCVN3_TO_UNICODE
def dec(s):
    return "".join(T2U.get(c, c) for c in s)

OURS = r"E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/settings"
VLTK = r"D:/GAMEDEVNEW/ReverseTools/phai3/vltk_raw"
RANGES = [(1363, 1384), (1965, 1991), (2114, 2143)]
ALL = "--all" in sys.argv

def load(path, idcol):
    t = io.open(path, "r", encoding="latin-1", newline="").read()
    lines = [l.rstrip("\r") for l in t.split("\n")]
    hdr = lines[0].split("\t")
    rows = {}
    for l in lines[1:]:
        if not l.strip(): continue
        c = l.split("\t")
        d = dict(zip(hdr, c))
        key = d.get(idcol, "").strip()
        if key.isdigit():
            rows[int(key)] = (d, len(c))
    return hdr, rows

def in_ranges(i):
    return any(a <= i <= b for a, b in RANGES)

def diff(name, idcol, keycols):
    ho, ro = load(os.path.join(OURS, name), idcol)
    hv, rv = load(os.path.join(VLTK, "settings__" + name), idcol)
    print("=" * 20, name, "cot JX1=%d VLTK=%d" % (len(ho), len(hv)))
    only_v = [c for c in hv if c not in ho]; only_o = [c for c in ho if c not in hv]
    print("cot chi VLTK co:", only_v[:40])
    print("cot chi JX1 co:", only_o[:40])
    ids = sorted(i for i in rv if in_ranges(i))
    ndiff = 0
    for i in ids:
        dv, nv = rv[i]
        if i not in ro:
            print("[THIEU] %d %s" % (i, dec(dv.get("SkillName", dv.get("Name", "")))))
            continue
        do, no = ro[i]
        out = []
        for c in hv:
            if c not in ho: continue
            if not ALL and c not in keycols: continue
            a, b = do.get(c, "").strip(), dv.get(c, "").strip()
            if a != b:
                out.append("%s: JX1=%s VLTK=%s" % (c, a or "-", b or "-"))
        if out:
            ndiff += 1
            print("%d %s (cot JX1 %d)" % (i, dec(do.get("SkillName", do.get("Name", ""))), no))
            for o in out: print("    " + o)
    print("=> %d/%d dong khac" % (ndiff, len(ids)))

SK = {"SkillName", "SkillStyle", "MisslesForm", "MisslesCount", "MisslesId", "AttackRadius", "SkillDistance", "MaxRange",
      "MisslesRange", "MissleStyle", "CoolDownTime", "PrepareTime", "AttackAnimation", "SkillDuration", "ScriptDistance",
      "TargetEnemy", "TargetFriend", "TargetSelf", "TargetNone", "CastEffect", "StateSpecialId", "SpecialSpr", "SkillCostType",
      "CostType", "Cost", "PreCastSpr", "MisslesForm2", "AttackType", "TargetDistance", "ChildSkillId", "CoolDown", "ClientOnly"}
MS = {"Name", "MoveKind", "Speed", "LifeTime", "CollidRange", "DmgRange", "IsRangeDmg", "ColVanish", "DmgInterval", "AutoExplode",
      "Width", "Height", "MissleFlySpr", "MissleStandSpr", "MissleVanishSpr", "MissleExplodeSpr", "Scale", "SprScale"}
if __name__ == "__main__":
    diff("skills.txt", "SkillId", SK)
    diff("missles.txt", "MissleId", MS)
