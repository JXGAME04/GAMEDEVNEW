# -*- coding: ascii -*-
"""vhtd_skills.py - VU HON (11) / TIEU DAO (12): du lieu KY NANG tu client VLTK Level Up (chu game 02/09: uu tien client VLTK).
  1. skills.txt server+client: +62 dong (1963-1992, 2114-2143, 1733, 1734) chep NGUYEN BYTE tu vltk_raw\\settings__skills.txt
     (header 114 cot trung JX1), ap luat "BO (Duong)" cua chu: chieu co ca hai (yan + thuong, cung gia tri) -> bo cot yan;
     chieu chi co yan -> doi ten sang thuong (khoa Lua doi theo). sorbdamage_yan_p GIU (don vi phan nghin, JX1 sorbdamage_p la %).
  2. missles.txt server+client: +27 dong dan (523,525-529,531,574-580,637-649) chep nguyen byte (header trung).
  3. script\\skill\\wuhuntang.lua, xiaoyao.lua: chep nguyen byte client VLTK (TCVN3+GBK) + doi khoa yan; advancedskill.lua: chen
     LineWith8Byte + bang xy_adskill cua VLTK vao SKILLS cua JX1 (khong dong den sl/tw/gb_adskill).
  4. settings\\npc\\player\\magic_level_exp.txt: +11 dong nguong exp (skill_skillexp_v tinh tu Lua) cho ky nang exp 2 phai.
  5. settings\\MagicDesc.ini: +15 khoa mo ta (5 dong VLTK + 10 dong tu viet TCVN3).
Doc/ghi latin-1, giu CRLF, ban luu .truoc_vhtd_0209, idempotent. DUNG: python vhtd_skills.py [--kiem]
"""
import io, os, sys, re, math, shutil
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\phai3\hoason_thicong")
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
RAW = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw"
KIEM = "--kiem" in sys.argv
BAK = ".truoc_vhtd_0209"

# TCVN3 encoder (Unicode -> byte) for our own Vietnamese description lines
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes, _UNICODE_TO_TCVN3; _TCVN3_TO_UNICODE = {ord(v): k for k, v in _UNICODE_TO_TCVN3.items()}   # bang TCVN3 da kiem chung tren byte that cua JX1 (Hoc vo cong = 48 E4 63 ...)
def V(s): return unicode_to_tcvn3_bytes(s).decode("latin-1")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    if KIEM: return
    if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def nl_of(s): return "\r\n" if "\r\n" in s else "\n"
def hib(s): return sum(1 for c in s if ord(c) >= 0x80)

IDS = list(range(1963, 1993)) + list(range(2114, 2144)) + [1733, 1734]
MSL = [523, 525, 526, 527, 528, 529, 531, 574, 575, 576, 577, 578, 579, 580] + list(range(637, 650))
# (SkillId, [(ten cu, ten moi hoac "" = bo)])  - luat BO (Duong) cua chu game
YAN = {
    1968: [("attackspeed_yan_v", ""), ("castspeed_yan_v", "")],
    1970: [("attackspeed_yan_v", ""), ("castspeed_yan_v", "")],
    1971: [("lifemax_yan_p", "")],
    1980: [("attackspeed_yan_v", ""), ("allres_yan_p", "allres_p"), ("fasthitrecover_yan_v", "fasthitrecover_v")],
    2122: [("lifemax_yan_v", "lifemax_v")],
    2123: [("lifemax_yan_p", "lifemax_p"), ("fasthitrecover_yan_v", "fasthitrecover_v"), ("allres_yan_p", "allres_p"), ("anti_lightingres_yan_p", "anti_lightingres_p")],
}
EXP_SKILLS = {11: [1967, 1969, 1970, 1983, 1984, 1985], 12: [2124, 2127, 2129, 2141, 2142]}

# ------------------------------------------------------------------ VLTK skills.txt
vl = rd(os.path.join(RAW, "settings__skills.txt"))
vl_nl = nl_of(vl); vl_lines = vl.split(vl_nl)
HDR = vl_lines[0].split("\t"); COL = {h: i for i, h in enumerate(HDR)}
def lvlcols(): return [(COL["LvlSetting%d" % k], COL["LvlData%d" % k]) for k in range(1, 30) if "LvlSetting%d" % k in COL]
vrow = {}
for l in vl_lines[1:]:
    f = l.split("\t")
    if len(f) < 100: continue
    try: vrow[int(f[2])] = f
    except: pass
lua_renames = {}   # (table, old) -> new
def prep_rows():
    out = {}
    for sid in IDS:
        f = list(vrow[sid])
        for old, new in YAN.get(sid, []):
            hit = False
            for cn, cd in lvlcols():
                if cn < len(f) and f[cn] == old:
                    tbl = f[cd]
                    if new: lua_renames[(tbl, old)] = new
                    f[cn] = new
                    if not new: f[cd] = ""
                    hit = True
            if not hit: raise SystemExit("VLTK %d khong co cot %s" % (sid, old))
        out[sid] = "\t".join(f)
    return out
NEWROWS = prep_rows()

def patch_skills(p):
    s = rd(p); nl = nl_of(s); lines = s.split(nl)
    if lines[0] != vl_lines[0]: raise SystemExit("header skills.txt khac VLTK: " + p)
    have = set()
    for l in lines[1:]:
        f = l.split("\t")
        if len(f) > 2 and f[2].isdigit(): have.add(int(f[2]))
    add = [NEWROWS[sid] for sid in IDS if sid not in have]
    if not add: print("  [=] %s: 62 dong da co" % p); return
    while lines and lines[-1] == "": lines.pop()
    out = nl.join(lines + add) + nl
    wr(p, out); print("  [+] %s: +%d dong ky nang" % (p, len(add)))

def patch_missles(p):
    s = rd(p); nl = nl_of(s); lines = s.split(nl)
    vm = rd(os.path.join(RAW, "settings__missles.txt")); vml = vm.split(nl_of(vm))
    if lines[0] != vml[0]: raise SystemExit("header missles.txt khac VLTK: " + p)
    have = set(l.split("\t")[0] for l in lines[1:])
    rows = {}
    for l in vml[1:]:
        f = l.split("\t")
        if f and f[0].isdigit() and int(f[0]) in MSL: rows[int(f[0])] = l
    miss = [m for m in MSL if m not in rows]
    if miss: raise SystemExit("VLTK missles thieu %s" % miss)
    add = [rows[m] for m in MSL if str(m) not in have]
    if not add: print("  [=] %s: dan da co" % p); return
    while lines and lines[-1] == "": lines.pop()
    wr(p, nl.join(lines + add) + nl); print("  [+] %s: +%d dong dan" % (p, len(add)))

def patch_lua(name, dst):
    src = rd(os.path.join(RAW, "script__skill__%s.lua" % name))
    nl = nl_of(src)
    s = src
    for (tbl, old), new in lua_renames.items():
        a = s.find("\t" + tbl + "={")
        if a < 0: a = s.find("\t" + tbl + " ={")
        if a < 0: continue   # bang thuoc file khac
        b = s.find(nl + "\t}", a)
        seg = s[a:b]
        pat = "\t\t" + old + "="
        if seg.count(pat) != 1:
            if ("\t\t" + new + "=") in seg: continue
            raise SystemExit("%s.%s: khoa %s xuat hien %d lan" % (name, tbl, old, seg.count(pat)))
        seg = seg.replace(pat, "\t\t" + new + "=", 1)
        s = s[:a] + seg + s[b:]
        print("  [+] %s.lua %s: %s -> %s" % (name, tbl, old, new))
    if os.path.exists(dst) and rd(dst) == s: print("  [=] %s da dung" % dst); return
    if hib(s) != hib(src): raise SystemExit("lech byte cao " + name)
    if not KIEM:
        if os.path.exists(dst) and not os.path.exists(dst + BAK): shutil.copy2(dst, dst + BAK)
        io.open(dst, "w", encoding="latin-1", newline="").write(s)
    print("  [+] ghi %s (%d byte)" % (dst, len(s)))

def patch_advanced(dst):
    s = rd(dst); nl = nl_of(s)
    if "xy_adskill" in s: print("  [=] %s da co xy_adskill" % dst); return
    src = rd(os.path.join(RAW, "script__skill__advancedskill.lua")); snl = nl_of(src)
    # LineWith8Byte
    a = src.find("function LineWith8Byte("); b = src.find(snl + "end", a) + len(snl + "end")
    fn8 = src[a:b].replace(snl, nl)
    # xy_adskill block: from '\txy_adskill' to the closing '\t},' of that table
    a = src.find("\txy_adskill"); b = src.find(snl + "\t},", a) + len(snl + "\t},")
    blk = src[a:b].replace(snl, nl)
    # insert fn8 before 'SKILLS={' and blk before the closing '}' of SKILLS
    m0 = re.search(r"(?m)^SKILLS\s*=", s)
    if not m0: raise SystemExit("advancedskill.lua JX1 khong co SKILLS=")
    line_start = m0.start()
    s2 = s[:line_start] + "-- [VHTD 02/09] noi suy tach byte cao/thap (id*256+cap) - chep tu client VLTK" + nl + fn8 + nl + nl + s[line_start:]
    m1 = re.search(r"(?m)^\}\r?$", s2[s2.find("SKILLS"):])
    if not m1: raise SystemExit("khong thay ket SKILLS")
    j = s2.find("SKILLS") + m1.start()
    if s2[j-len(nl):j] == nl: j -= len(nl)
    s2 = s2[:j] + nl + "\t-- [VHTD 02/09] Tieu Dao tien giai 2132/2133/2134 - chep tu client VLTK" + nl + blk + s2[j:]
    if hib(s2) != hib(s) + hib(fn8) + hib(blk): raise SystemExit("lech byte cao advancedskill")
    wr(dst, s2); print("  [+] %s: +LineWith8Byte +xy_adskill" % dst)

# ------------------------------------------------------------------ exp table
def SkillExpFunc(Exp0, a, Level, Time, Range): return math.floor(Exp0 * (a ** (Level - 1)) * Time * Range / 2)
def exp_table(luasrc, tbl):
    i = luasrc.find("\t" + tbl + "={")
    if i < 0: i = luasrc.find("\t" + tbl + " ={")
    if i < 0: i = luasrc.find("\t" + tbl + "=" )
    j = luasrc.find("skill_skillexp_v", i) if i >= 0 else -1
    if j < 0: raise SystemExit("khong thay bang %s / skill_skillexp_v" % tbl)
    o = luasrc.find("{", j); depth = 0; k = o
    while k < len(luasrc):
        if luasrc[k] == "{": depth += 1
        elif luasrc[k] == "}":
            depth -= 1
            if depth == 0: break
        k += 1
    blk = luasrc[o + 1:k]   # noi dung bang skill_skillexp_v (khop ngoac, chap nhan xuong dong)
    out = {}
    for lv, ex in re.findall(r"\{\s*(\d+)\s*,\s*([^{}]+?)\s*\}", blk):
        out[int(lv)] = int(eval(ex.strip(), {"SkillExpFunc": SkillExpFunc, "floor": math.floor}))
    if not out: raise SystemExit("khong doc duoc exp %s" % tbl)
    return out
def patch_exp(p):
    d = rd(p); nl = nl_of(d); lines = d.split(nl)
    ncol = len(lines[0].split("\t"))
    have = set(l.split("\t")[0] for l in lines)
    luas = {"wuhuntang": rd(os.path.join(RAW, "script__skill__wuhuntang.lua")), "xiaoyao": rd(os.path.join(RAW, "script__skill__xiaoyao.lua"))}
    add = []
    for fac, ids in EXP_SKILLS.items():
        for sid in ids:
            if str(sid) in have: continue
            f = vrow[sid]
            tbl = None
            for cn, cd in lvlcols():
                if cn < len(f) and f[cn] == "skill_skillexp_v": tbl = f[cd]
            if not tbl: raise SystemExit("%d khong co skill_skillexp_v" % sid)
            script = f[COL["LvlSetScript"]].split("\\")[-1].replace(".lua", "")
            try:
                tb = exp_table(luas[script], tbl)
            except SystemExit as ex:
                print("  [!] bo qua exp %d: %s (bang Lua VLTK khong co skill_skillexp_v -> GetSkillLevelData tra rong, khong loi)" % (sid, ex)); continue
            vals = [str(tb.get(lv, "")) for lv in range(1, ncol - 3 + 1)]
            add.append("\t".join([str(sid), f[0], str(fac)] + vals))
            print("  + exp %d %s L1=%s Lmax=%d:%s" % (sid, tbl, tb[1], max(tb), tb[max(tb)]))
    if not add: print("  [=] %s da co" % p); return
    while lines and lines[-1] == "": lines.pop()
    wr(p, nl.join(lines + add) + nl); print("  [+] %s +%d" % (p, len(add)))

# ------------------------------------------------------------------ MagicDesc.ini
DESC_VLTK = ["lightingdamage_p", "addlightingmagic_p", "special_point_base", "lifereplenish_dec_p", "unravel_effect"]
DESC_MINE = {
    "special_point_add":      u"T\u0103ng <color=orange>#d3- t\u1EA7ng<color> cho k\u1EF9 n\u0103ng #l1",
    "cost_sp":                u"\u0110i\u1EC1u ki\u1EC7n thi tri\u1EC3n: <color=yellow>#d3- t\u1EA7ng<color> c\u1EE7a #l1",
    "autocastskill":          u"M\u1ED7i <color=orange>#f6- gi\u00E2y<color> t\u1EF1 \u0111\u1ED9ng thi tri\u1EC3n <color=water>#lA-<color> c\u1EA5p #d7- (t\u1EF7 l\u1EC7 #d9-%)",
    "lock_life":              u"Sinh l\u1EF1c kh\u00F4ng th\u1EC3 gi\u1EA3m d\u01B0\u1EDBi <color=orange>#d1-<color> \u0111i\u1EC3m trong <color=orange>#f2- gi\u00E2y<color>",
    "cast_when_buff_removed": u"Khi tr\u1EA1ng th\u00E1i k\u1EBFt th\u00FAc t\u1EF1 \u0111\u1ED9ng thi tri\u1EC3n #l1",
    "reset_bufftime":         u"\u0110\u1EB7t l\u1EA1i th\u1EDDi gian tr\u1EA1ng th\u00E1i #l1 tr\u00EAn m\u1EE5c ti\u00EAu",
    "resume_life_p":          u"H\u1ED3i ph\u1EE5c <color=orange>#d1-%<color> sinh l\u1EF1c t\u1ED1i \u0111a",
    "hidebodyunlock":         u"T\u00E0ng h\u00ECnh trong <color=orange>#f2- gi\u00E2y<color>",
    "invincibility":          u"B\u1EA5t t\u1EED trong <color=orange>#f2- gi\u00E2y<color>",
    "forbit_attack":          u"Kh\u00F4ng th\u1EC3 c\u00F4ng k\u00EDch trong <color=orange>#f2- gi\u00E2y<color>",
}
def patch_desc(p):
    d = rd(p); nl = nl_of(d)
    vd = rd(os.path.join(RAW, "settings__magicdesc.ini")); vdl = vd.split(nl_of(vd))
    vmap = {}
    for l in vdl:
        if "=" in l and not l.startswith(";"):
            k, v = l.split("=", 1); vmap[k.strip()] = v
    add = []; fixed = []
    def co_gia_tri(k):   # (co dong, gia tri khong rong)
        m0 = re.search(r"(?m)^" + re.escape(k) + r"=(.*)$", d)
        return m0, (m0 is not None and m0.group(1).strip("\r ") != "")
    def dien(k, v):      # khoa co san nhung RONG (JX1 co placeholder) -> dien mo ta tai cho
        nonlocal d
        m0 = re.search(r"(?m)^" + re.escape(k) + r"=", d)
        d = d[:m0.end()] + v + d[m0.end():]; fixed.append(k)
    for k in DESC_VLTK:
        if k not in vmap or not vmap[k].strip(): raise SystemExit("VLTK magicdesc thieu " + k)
        m0, ok = co_gia_tri(k)
        if ok: continue
        if m0: dien(k, vmap[k].rstrip("\r")); continue
        add.append(k + "=" + vmap[k])
    for k, v in DESC_MINE.items():
        m0, ok = co_gia_tri(k)
        if ok: continue
        if m0: dien(k, V(v)); continue
        add.append(k + "=" + V(v))
    if fixed: print("  [+] %s: dien %d khoa rong: %s" % (p, len(fixed), ",".join(fixed)))
    if not add:
        if fixed: wr(p, d)
        else: print("  [=] %s da co" % p)
        return
    # chen sau dong autoreplyskill= (cung muc [Descript])
    m = re.search(r"(?m)^autoreplyskill=.*$", d)
    if not m: raise SystemExit("MagicDesc.ini khong co autoreplyskill=")
    e = m.end()
    d2 = d[:e] + nl + nl.join(add) + d[e:]
    wr(p, d2); print("  [+] %s: +%d mo ta" % (p, len(add)))

for side in ("server", "client"):
    B = os.path.join(BIN, side)
    patch_skills(os.path.join(B, "settings", "skills.txt"))
    patch_missles(os.path.join(B, "settings", "missles.txt"))
    for name in ("wuhuntang", "xiaoyao"):
        patch_lua(name, os.path.join(B, "script", "skill", name + ".lua"))
    patch_advanced(os.path.join(B, "script", "skill", "advancedskill.lua"))
    patch_exp(os.path.join(B, "settings", "npc", "player", "magic_level_exp.txt"))
    patch_desc(os.path.join(B, "settings", "MagicDesc.ini"))
print("XONG%s." % (" (KIEM)" if KIEM else ""))
