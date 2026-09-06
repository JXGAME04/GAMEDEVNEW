# -*- coding: utf-8 -*-
# Replica of pb_CoChieuNoiTayKhong (KPlayerBot.cpp) on real server data, level-110 bot.
import io, re, sys
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
rows = [l.rstrip("\r\n").split("\t") for l in io.open(SRV + r"\settings\skills.txt", "r", encoding="latin-1")]
h = rows[0]; ix = {n: i for i, n in enumerate(h)}
sk = {}
for r in rows[1:]:
    if len(r) < len(h) - 5:
        continue
    try:
        sid = int(r[ix["SkillId"]])
    except Exception:
        continue
    attrs = set()
    for k in range(1, 21):
        a = r[ix["LvlSetting%d" % k]].strip()
        if a:
            attrs.add(a)
    def gi(n, d=0):
        try:
            return int(r[ix[n]])
        except Exception:
            return d
    sk[sid] = dict(name=r[ix["SkillName"]], style=gi("SkillStyle"), aura=gi("IsAura"), rad=gi("AttackRadius"),
                   phys=gi("IsPhysical"), tE=gi("TargetEnemy"), tS=gi("TargetSelf"), rq=gi("ReqLevel"),
                   eqt=gi("EqtLimit", -2), series=gi("Series", -1), attrs=attrs)
s = io.open(SRV + r"\script\header\factionhead.lua", "r", encoding="latin-1").read()

def block(name):
    i = s.find(name + " = {")
    if i < 0:
        i = s.find(name + "={")
    if i < 0:
        i = re.search(name + r"\s*=\s*\{", s).start()
    j = i
    depth = 0
    while j < len(s):
        c = s[j]
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return s[i:j + 1]
        j += 1
    return s[i:]

def ids(name):
    out = {}
    cur = None
    for line in block(name).splitlines():
        m = re.match(r"\s*\[(\d+)\]\s*=\s*\{", line)
        if m:
            cur = int(m.group(1))
            out[cur] = []
        if cur is None:
            continue
        for m2 in re.finditer(r"\{\s*\"?([^\",{}]*)\"?\s*,\s*(\d+)\s*(?:,\s*(\d+))?\s*\}", line):
            a, b, c = m2.groups()
            a = a.strip()
            out[cur].append(int(a) if a.isdigit() else int(b))
    return out

normal = ids("SKILLNORMAL")
s90 = ids("SKILL90_ARRAY")
FAC = ["Thieu Lam", "Thien Vuong", "Duong Mon", "Ngu Doc", "Nga Mi", "Thuy Yen", "Cai Bang",
       "Thien Nhan", "Vo Dang", "Con Lon", "Hoa Son", "Vu Hon", "Tieu Dao"]
SER = [0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 2, 3, 4]
PHEP = {"magicdamage_v", "colddamage_v", "firedamage_v", "lightingdamage_v"}
LEVEL = int(sys.argv[1]) if len(sys.argv) > 1 else 110

def loc(p, bot_series, want):
    if p["series"] >= 0 and p["series"] < 5 and p["series"] != bot_series:
        return "HE"
    if p["rad"] <= 0:
        return "TAM0"
    if p["aura"]:
        return "AURA"
    if p["tS"]:
        return "SELF"
    if not p["tE"]:
        return "KDICH"
    if p["style"] not in (0, 1):
        return "KIEU"
    if want == -1 and p["eqt"] not in (-1, -2):
        return "EQT%d" % p["eqt"]
    rq = min(p["rq"], 80)
    if LEVEL < rq:
        return "CAP"
    return "OK"

print("cap bot = %d" % LEVEL)
for f in range(13):
    lst = normal.get(f + 1, []) + s90.get(f + 1, [])
    noi_tk = []
    ok_tk = []
    phep_vk = []
    for sid in lst:
        p = sk.get(sid)
        if not p:
            continue
        phep = bool(p["attrs"] & PHEP)
        docthuan = (not p["phys"]) and ("poisondamage_v" in p["attrs"]) and not phep \
            and not ({"physicsdamage_v", "physicsenhance_p"} & p["attrs"])
        # castable WITH a specific weapon (eqt >= 0) and has magic damage: what odd-ID bots use today
        r_any = loc(p, SER[f], 0)
        if r_any == "OK" and phep and not docthuan and p["eqt"] >= 0:
            phep_vk.append("%d(eqt%d)" % (sid, p["eqt"]))
        r = loc(p, SER[f], -1)
        if r != "OK":
            continue
        if docthuan:
            continue
        ok_tk.append(sid)
        if phep:
            noi_tk.append("%d(eqt%d%s,rq%d)" % (sid, p["eqt"], ",phys" if p["phys"] else "", p["rq"]))
    print("%2d %-11s he%d  tay-khong dung duoc: %s" % (f, FAC[f], SER[f], ok_tk))
    print("      NOI tay-khong (pb_DonPhepThat): %s" % (noi_tk if noi_tk else "KHONG -> giu vu khi"))
    print("      phep can vu khi rieng (eqt>=0): %s" % (phep_vk if phep_vk else "-"))
