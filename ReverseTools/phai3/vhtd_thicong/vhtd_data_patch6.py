# -*- coding: utf-8 -*-
"""vhtd_data_patch6.py [VHTD 02/09i] - dot 6: DONG BO thuoc tinh ky nang 3 phai theo CLIENT VLTK LEVEL UP
(C:\\Users\\nguye\\Level Up Games\\Vo Lam Truyen Ky, pak dau tien trong package.ini co tep = slistcache.pak 19/08 15:49 = vltk_raw/*).
Chu 02/09 ~11:40: "kiem tra thuoc tinh cac phai moi xem dung nhu ban VLTK khong? toi thay sai hoi nhieu", "pham vi ky nang nho hon ban goc".
 D1 skills.txt (server+client): 13 dong khac VLTK -> lay DUNG gia tri VLTK theo TEN COT (1369 1374 1376 1378 1381 1382 1384 | 1968 1970 1971 1980 | 2122 2123).
    GIU 2119 (chieu con Tap Dap Luu Tinh, gia dinh dot 5) va moi dong khac (da trung 100%).
 D2 huashan.lua (server+client): Hoa Son dot 1 lay tu Linux huashan2013 (52 muc lech VLTK: physicsenhance_p, skill_attackradius 384->512,
    autoattackskill 15s/5% -> 5s/10%, thieu lifemax_yan_p/anti_sorbdamage_yan_p/enhancehit*_rate/skill_misslenum_v...) -> thay bang ban VLTK,
    TACH bang (SKILLS.x=) nhu wuhuntang/xiaoyao (tranh tran stack Lua 4 client), kiem lua4 -s100 dofile + so GetSkillLevelData.
 D3 MagicDesc.ini (server+client): bo hau to "(Duong)" / "(am duong)" / "(duong)" (chu quyet dot g: BO (Duong)); cac thuoc tinh VLTK KHONG mo ta
    (cost_sp special_point_add reset_bufftime hidebodyunlock invincibility forbit_attack autocastskill lock_life cast_when_buff_removed resume_life_p)
    -> de trong nhu VLTK (VLTK dua vao skill_desc cua tung ky nang; het "thuoc tinh la").
Doc/ghi latin-1, giu NL tung tep, idempotent. DUNG: python vhtd_data_patch6.py [--kiem]
"""
import io, os, sys, re, shutil, subprocess, importlib.util

KIEM = "--kiem" in sys.argv
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
VLTK = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw"
LUA4 = r"D:\GAMEDEVNEW\ReverseTools\lua4\lua4.exe"
BAK = ".truoc_vhtd_patch6_0209"
SIDES = ("server", "client")
IDS_SKILL = [1369, 1374, 1376, 1378, 1381, 1382, 1384, 1968, 1970, 1971, 1980, 2122, 2123]
NO_DESC = ["cost_sp", "special_point_add", "reset_bufftime", "hidebodyunlock", "invincibility", "forbit_attack",
           "autocastskill", "lock_life", "cast_when_buff_removed", "resume_life_p"]

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import vhtd_lua_split as VS
spec = importlib.util.spec_from_file_location("vn_edit", r"C:/Users/nguye/.claude/skills/swordonline-dev/scripts/vn_edit.py")
vn = importlib.util.module_from_spec(spec); spec.loader.exec_module(vn)
def tcvn(u):
    return vn.unicode_to_tcvn3_bytes(u).decode("latin-1")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s, tag):
    if "\xef\xbf\xbd" in s: raise SystemExit("EF BF BD " + p)
    if not KIEM:
        if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
        io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  => %s %s (%s)" % ("KIEM" if KIEM else "ghi", p, tag))

# ---------------------------------------------------------------- D1 skills.txt
def load_tab(s):
    nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
    lines = s.split(nl)
    hdr = lines[0].split("\t")
    return nl, lines, hdr

def d1_skills():
    vs = rd(os.path.join(VLTK, "settings__skills.txt"))
    _, vlines, vhdr = load_tab(vs)
    vid = vhdr.index("SkillId")
    vrows = {}
    for l in vlines[1:]:
        c = l.split("\t")
        if len(c) > vid and c[vid].strip().isdigit(): vrows[int(c[vid])] = dict(zip(vhdr, c))
    for side in SIDES:
        p = os.path.join(BIN, side, "settings", "skills.txt")
        s = rd(p); nl, lines, hdr = load_tab(s)
        sid = hdr.index("SkillId"); n = 0
        for i in range(1, len(lines)):
            c = lines[i].split("\t")
            if len(c) <= sid or not c[sid].strip().isdigit(): continue
            k = int(c[sid])
            if k not in IDS_SKILL: continue
            v = vrows[k]; ch = []
            for j, h in enumerate(hdr):
                if h in v and c[j] != v[h]:
                    ch.append("%s %s->%s" % (h, c[j] or "-", v[h] or "-")); c[j] = v[h]
            if ch:
                n += 1; lines[i] = "\t".join(c)
                print("  [+] skills %d: %s" % (k, "; ".join(ch)[:230]))
        if n: wr(p, nl.join(lines), "skills.txt %d dong" % n)
        else: print("  [=] %s skills.txt da khop" % side)

# ---------------------------------------------------------------- D2 huashan.lua
def d2_huashan():
    v = rd(os.path.join(VLTK, "script__skill__huashan.lua"))
    res = VS.split_skills(v)
    if res is None: raise SystemExit("VLTK huashan da tach?")
    s2, ne = res
    if sum(1 for c in s2 if ord(c) >= 0x80) != sum(1 for c in v if ord(c) >= 0x80): raise SystemExit("lech byte cao huashan")
    # kiem lua4: (a) ban tach nap qua dofile voi stack 100 (client); (b) GetSkillLevelData giong ban goc (goc chay stack lon)
    d = os.path.dirname(os.path.abspath(__file__))
    t_old = os.path.join(d, "_t_hs_old.lua"); t_new = os.path.join(d, "_t_hs_new.lua")
    io.open(t_old, "w", encoding="latin-1", newline="").write(v); io.open(t_new, "w", encoding="latin-1", newline="").write(s2)
    try:
        # head.lua cua cay chay that cung cap Link/Param2String/GetSkillLevelData
        head = os.path.join(BIN, "server", "script", "skill", "head.lua").replace("\\", "/")
        def run(luaf, stack):
            test = ('dofile("%s")\ndofile("%s")\nfor tb, v in SKILLS do if type(v) == "table" then for k, _ in v do '
                    'for lv = 1, 20, 9 do local r = GetSkillLevelData(k, tb, lv) print(tb, k, lv, tostring(r)) end end end end\n') % (head, luaf.replace("\\", "/"))
            tp = os.path.join(d, "_t_hs_run.lua"); io.open(tp, "w", encoding="latin-1", newline="").write(test)
            r = subprocess.run([LUA4, "-s%d" % stack, tp], capture_output=True, timeout=60, cwd=d); os.remove(tp)
            return r.returncode, r.stdout, r.stderr
        rc0, o0, e0 = run(t_old, 16384); rc1, o1, e1 = run(t_new, 100)
    finally:
        os.remove(t_old); os.remove(t_new)
    # ket qua co dia chi ham (skill_desc = function) -> bo dong 'function:'
    f = lambda o: b"\n".join(sorted(l for l in o.splitlines() if b"function:" not in l))   # thu tu duyet bang phu thuoc cach dung -> sap xep roi so
    print("  lua4 huashan: goc(-s16384) rc=%d %r | tach(-s100 dofile) rc=%d %r | dong %d/%d | giong: %s" % (rc0, e0[:50], rc1, e1[:50], o0.count(b"\n"), o1.count(b"\n"), "CO" if f(o0) == f(o1) and o1 else "KHAC"))
    if rc1 != 0 or f(o0) != f(o1) or not o1: raise SystemExit("huashan.lua tach khong dat")
    for side in SIDES:
        p = os.path.join(BIN, side, "script", "skill", "huashan.lua")
        cur = rd(p)
        if cur == s2: print("  [=] %s huashan.lua da la ban VLTK tach" % side); continue
        wr(p, s2, "huashan.lua VLTK + tach %d bang" % ne)

# ---------------------------------------------------------------- D3 MagicDesc.ini
def d3_magicdesc():
    suffixes = [tcvn(" (D\u01b0\u01a1ng)"), tcvn("(D\u01b0\u01a1ng)"), tcvn(" (\u00e2m d\u01b0\u01a1ng)"), tcvn("(\u00e2m d\u01b0\u01a1ng)"), tcvn(" (d\u01b0\u01a1ng)"), tcvn("(d\u01b0\u01a1ng)")]
    for side in SIDES:
        p = os.path.join(BIN, side, "settings", "MagicDesc.ini")
        s = rd(p); nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
        lines = s.split(nl); n = 0
        for i, l in enumerate(lines):
            m = re.match(r"^(\s*)([A-Za-z0-9_]+)(\s*=)(.*)$", l)
            if not m: continue
            key, val = m.group(2), m.group(4)
            new = val
            for suf in suffixes:
                new = new.replace(suf, "")
            if key in NO_DESC and new.strip():
                new = ""
            if new != val:
                n += 1; lines[i] = m.group(1) + key + m.group(3) + new
                print("  [+] MagicDesc %s: %r -> %r" % (key, val[:60], new[:60]))
        if n: wr(p, nl.join(lines), "MagicDesc %d dong" % n)
        else: print("  [=] %s MagicDesc da khop" % side)

if __name__ == "__main__":
    print("vhtd_data_patch6 [VHTD 02/09i]%s" % (" (KIEM)" if KIEM else ""))
    d1_skills(); d2_huashan(); d3_magicdesc()
    print("XONG.")
