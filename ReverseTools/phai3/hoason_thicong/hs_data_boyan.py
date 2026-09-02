# -*- coding: ascii -*-
"""hs_data_boyan.py - HOA SON: BO nhanh "(Duong)" (chu game quyet 02/09: "bo (duong) lam 1 di").
Linux giu 2 nhanh thuong/yan va lay MAX; JX1 cong don. Cach lam theo chu: dua thuoc tinh yan cua Hoa Son ve thuoc tinh thuong,
o chieu co CA HAI (1349 attackspeed_v + attackspeed_yan_v cung gia tri; 1376/1381 lifemax_p >= lifemax_yan_p moi cap) thi BO nhanh yan
=> gia tri hieu dung bang MAX cua Linux. sorbdamage_yan_p (1376/1381) GIU NGUYEN: JX1 da max() + don vi phan nghin dung Linux,
con sorbdamage_p cua JX1 la PHAN TRAM (x10) -> doi ten se manh gap 10.
Sua: server|client settings\\skills.txt (cot LvlSetting theo ten) + server|client script\\skill\\huashan.lua (khoa bang).
Doc/ghi latin-1, giu CRLF. Idempotent. DUNG: python hs_data_boyan.py [--kiem]
"""
import io, os, sys, shutil

BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
KIEM = "--kiem" in sys.argv
BAK = ".truoc_hoason_0209"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    if KIEM: return
    if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def hib(s): return sum(1 for c in s if ord(c) >= 0x80)

# ---- skills.txt -------------------------------------------------------------------------------
# (SkillId, ten Lua bang, [(ten cu, ten moi hoac "" = bo cot), ...])
ROWS = [
    (1349, "jianzong_zongjue", [("attackspeed_yan_v", ""), ("lifemax_yan_p", "lifemax_p")]),
    (1358, "huanyan_yunyan",   [("lifemax_yan_p", "lifemax_p")]),
    (1374, "longraoshen",      [("manamax_yan_p", "manamax_p")]),
    (1376, "longxuan_jianqi1", [("lifemax_yan_p", "")]),
    (1381, "new_wuji",         [("lifemax_yan_p", "")]),
]
def patch_skills(p):
    s = rd(p); nl = "\r\n" if "\r\n" in s else "\n"
    lines = s.split(nl)
    hdr = lines[0].split("\t")
    col = {h: i for i, h in enumerate(hdr)}
    n = 0
    for sid, tbl, changes in ROWS:
        idx = [i for i, l in enumerate(lines) if i > 0 and l.split("\t")[2:3] == [str(sid)]]
        if len(idx) != 1: raise SystemExit("%s: khong tim thay duy nhat dong id %d (%d)" % (p, sid, len(idx)))
        f = lines[idx[0]].split("\t")
        for k in range(1, 30):
            cn, cd = col.get("LvlSetting%d" % k), col.get("LvlData%d" % k)
            if cn is None or cn >= len(f): break
            if f[cn] and f[cd] and f[cd] != "0" and f[cd] != tbl:
                raise SystemExit("%s: dong %d cot LvlData%d = %r, mong %r" % (p, sid, k, f[cd], tbl))
        done_any = False
        for old, new in changes:
            hit = False
            for k in range(1, 30):
                cn, cd = col.get("LvlSetting%d" % k), col.get("LvlData%d" % k)
                if cn is None or cn >= len(f): break
                if f[cn] == old:
                    f[cn] = new
                    if new == "": f[cd] = ""
                    hit = True; done_any = True
            if not hit:
                # da ap? (ten moi da co / cot da trong)
                names = [f[col["LvlSetting%d" % k]] for k in range(1, 30) if col.get("LvlSetting%d" % k) is not None and col["LvlSetting%d" % k] < len(f)]
                if (new and new in names) or (new == "" and old not in names):
                    print("  [=] %s: %d %s da ap" % (os.path.basename(p), sid, old)); continue
                raise SystemExit("%s: dong %d khong thay cot %s" % (p, sid, old))
        if done_any:
            lines[idx[0]] = "\t".join(f); n += 1
            print("  [+] %s: %d %s" % (os.path.basename(p), sid, ", ".join("%s->%s" % (o, nw or "(bo)") for o, nw in changes)))
    out = nl.join(lines)
    if out != s:
        if hib(out) != hib(s): raise SystemExit("lech byte cao " + p)
        wr(p, out); print("  => ghi %s (%d dong)%s" % (p, n, " KIEM" if KIEM else ""))
    return n

# ---- huashan.lua ------------------------------------------------------------------------------
LUA = [
    ("jianzong_zongjue", [("del", "\t\tattackspeed_yan_v={{{1,6},{26,32},{31,32},{32,32}},{{1,-1},{2,-1}}},"),
                          ("ren", "\t\tlifemax_yan_p=", "\t\tlifemax_p=")]),
    ("huanyan_yunyan",   [("ren", "\t\tlifemax_yan_p=", "\t\tlifemax_p=")]),
    ("longraoshen",      [("ren", "\t\tmanamax_yan_p=", "\t\tmanamax_p=")]),
    ("longxuan_jianqi1", [("del", "\t\tlifemax_yan_p={{{1,5},{15,10},{25,15},{31,20},{32,20}},{{1,10},{31,18},{32,18}}},")]),
    ("new_wuji",         [("del", "\t\tlifemax_yan_p={{{1,5},{15,20},{25,60},{31,70},{32,70}},{{1,10},{31,18},{32,18}}},")]),
]
def patch_lua(p):
    s = rd(p); nl = "\r\n" if "\r\n" in s else "\n"
    n = 0
    for tbl, ops in LUA:
        a = s.find("\t" + tbl + "={")
        if a < 0: raise SystemExit("%s: khong thay bang %s" % (p, tbl))
        b = s.find(nl + "\t}," , a)
        if b < 0: raise SystemExit("%s: khong thay ket bang %s" % (p, tbl))
        seg = s[a:b]
        for op in ops:
            if op[0] == "del":
                line = op[1] + nl
                if line in seg:
                    seg = seg.replace(line, "", 1); n += 1; print("  [+] %s: %s bo %s" % (os.path.basename(p), tbl, op[1].strip()[:24]))
                elif op[1].split("=")[0].strip() not in seg:
                    print("  [=] %s: %s da bo %s" % (os.path.basename(p), tbl, op[1].strip()[:24]))
                else: raise SystemExit("%s: %s dong khac mong doi: %s" % (p, tbl, op[1].strip()[:40]))
            else:
                old, new = op[1], op[2]
                if old in seg:
                    if seg.count(old) != 1: raise SystemExit("%s: %s %s khong duy nhat" % (p, tbl, old.strip()))
                    seg = seg.replace(old, new, 1); n += 1; print("  [+] %s: %s %s -> %s" % (os.path.basename(p), tbl, old.strip(), new.strip()))
                elif new in seg:
                    print("  [=] %s: %s da doi %s" % (os.path.basename(p), tbl, new.strip()))
                else: raise SystemExit("%s: %s khong thay %s" % (p, tbl, old.strip()))
        s = s[:a] + seg + s[b:]
    if n:
        orig = rd(p)
        if hib(s) != hib(orig): raise SystemExit("lech byte cao " + p)
        wr(p, s); print("  => ghi %s (%d cho)%s" % (p, n, " KIEM" if KIEM else ""))
    return n

for side in ("server", "client"):
    patch_skills(os.path.join(BIN, side, "settings", "skills.txt"))
    patch_lua(os.path.join(BIN, side, "script", "skill", "huashan.lua"))
print("XONG%s." % (" (KIEM)" if KIEM else ""))
