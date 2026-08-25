# -*- coding: latin-1 -*-
# gen_liendau_tables.py - sinh Sources/Core/Src/KLienDauTables.h cho AUTO LIEN DAU (WAuto).
#
# Cung khuon voi gen_tongkim_tables.py / gen_datau_tables.py: MOI chuoi marker duoc
# TRICH THANG tu script Lua song cua may chu (doc/ghi latin-1, byte-for-byte) nen
# chuoi trong .h la RAW TCVN3. KHONG go tay tieng Viet vao .h, KHONG go tay \xNN.
#
# Chay:  python ReverseTools/gen_liendau_tables.py [duong-dan-thu-muc-script-server]
import io
import os
import re
import sys

SRV = sys.argv[1] if len(sys.argv) > 1 else \
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                   "..", "Sources", "Core", "Src", "KLienDauTables.h")

LM = os.path.join(SRV, "missions", "leaguematch")
NL = chr(10)
BS = chr(92)


def rd(*parts):
    with io.open(os.path.join(*parts), "r", encoding="latin-1", newline="") as f:
        return f.read()


def cstr(s):
    """Chuoi C giu nguyen byte (latin-1); chi thoat nhung ky tu bat buoc."""
    out = []
    for ch in s:
        if ch == BS:
            out.append(BS + BS)
        elif ch == '"':
            out.append(BS + '"')
        elif ch == chr(9):
            out.append(BS + 't')
        elif ord(ch) < 0x20:
            raise SystemExit("marker co ky tu dieu khien: %r" % s)
        else:
            out.append(ch)
    return '"%s"' % ''.join(out)


def pick(text, pat, what):
    m = re.search(pat, text, re.S)
    if not m:
        raise SystemExit("KHONG trich duoc marker: %s" % what)
    return m.group(1)


def num(text, name):
    return int(pick(text, re.escape(name) + r"\s*=\s*(-?\d+)", name))


def func(text, name):
    """Than cua "function <name>(" toi "function" ke tiep - de neo marker dung cho."""
    m = re.search(r"function\s+" + re.escape(name) + r"\s*\(", text)
    if not m:
        raise SystemExit("KHONG thay ham %s" % name)
    rest = text[m.end():]
    m2 = re.search(NL + r"function\s", rest)
    return rest[:m2.start()] if m2 else rest


def lowascii(s):
    """Ha thuong CHI ASCII - dung y het g_StrLower (KStrBase.cpp:362); byte TCVN3 giu nguyen."""
    return "".join(chr(ord(c) + 32) if "A" <= c <= "Z" else c for c in s)


def prefix(a, b):
    n = 0
    while n < len(a) and n < len(b) and a[n] == b[n]:
        n += 1
    return a[:n]


# ---------------------------------------------------------------- nguon
head = rd(LM, "head.lua")
officer = rd(LM, "npc", "officer.lua")
signup = rd(LM, "npc", "signup.lua")
chefu = rd(LM, "npc", "chefu.lua")
autoexec = rd(LM, "wlls_autoexec.lua")
gmscript = rd(LM, "wlls_gmscript.lua")
sched = rd(LM, "glbmission", "schedule.lua")

# ---------------------------------------------------------------- hang so
LEVEL_JUNIOR = num(head, "WLLS_LEVEL_JUNIOR")
LEVEL_SENIOR = num(head, "WLLS_LEVEL_SENIOR")
PREP_FREQ = num(head, "WLLS_TIMER_PREP_FREQ")
PREP_TOTAL = num(head, "WLLS_TIMER_PREP_TOTAL")
FIGHT_FREQ = num(head, "WLLS_TIMER_FIGHT_FREQ")
FIGHT_TOTAL = num(head, "WLLS_TIMER_FIGHT_TOTAL")
MATCHTIME = num(rd(SRV, "leaguematch", "head.lua"), "WLLS_MATCHTIME")

m = re.search(r"WLLS_MAPPOS_SIGN\s*=\s*\{\s*(\d+)\s*,\s*(\d+)\s*\}", head)
SIGN = (int(m.group(1)), int(m.group(2)))
m = re.search(r"WLLS_MAPPOS_PRE\s*=\s*\{\s*(\d+)\s*,\s*(\d+)\s*\}", head)
PREP = (int(m.group(1)), int(m.group(2)))

# ---------------------------------------------------------------- bang map
halls, preps, arenas = set(), set(), set()
mt = os.path.join(LM, "macthtype")
for f in sorted(os.listdir(mt)):
    if not f.endswith(".lua"):
        continue
    s = rd(mt, f)
    for a, b, c in re.findall(r"\{\s*(\d{3,4})\s*,\s*(\d{3,4})\s*,\s*(\d{3,4})\s*,\s*\"", s):
        halls.add(int(a))
        preps.add(int(b))
        arenas.add(int(c))
if not halls or (halls & arenas) or (halls & preps):
    raise SystemExit("bang map hoi truong/chuan bi/dau truong khong hop le")

# ---------------------------------------------------------------- thanh + Su gia
cityhang = {}
m = re.search(r"WLLS_CITY_INDEX\s*=\s*\{(.*?)" + NL + r"\}", head, re.S)
for mid, hang in re.findall(r"\[\s*(\d+)\s*\]\s*=\s*\{\s*(\d+)\s*,", m.group(1)):
    cityhang[int(mid)] = int(hang)

citypos = {}
m = re.search(r"WLLS_NPC_CITY\s*=\s*\{(.*?)" + NL + r"\}", autoexec, re.S)
for line in m.group(1).split(NL):
    if "officer.lua" not in line:
        continue
    for mid, x, y in re.findall(r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\}", line):
        citypos[int(mid)] = (int(x), int(y))
if len(citypos) != len(cityhang):
    raise SystemExit("so thanh co Su gia (%d) khac WLLS_CITY_INDEX (%d)"
                     % (len(citypos), len(cityhang)))

# ---------------------------------------------------------------- NPC hoi truong


def npcpos(block, script):
    mm = re.search(block + r"\s*=\s*\{(.*?)" + NL + r"\}", autoexec, re.S)
    out = []
    for line in mm.group(1).split(NL):
        if script not in line:
            continue
        m3 = re.search(r"\{\s*\d+\s*,\s*(\d+)\s*,\s*(\d+)\s*,", line)
        if m3:
            out.append((int(m3.group(1)), int(m3.group(2))))
    return out


def npcname(script):
    out = []
    for line in autoexec.split(NL):
        if script not in line:
            continue
        for nm in re.findall('"([^"]+)"', line):
            if nm.strip() and BS not in nm:
                out.append(nm)
    if not out:
        raise SystemExit("khong doc duoc ten NPC cua %s" % script)
    return out


sign_npc = npcpos("WLLS_NPC_SIGNUP", "signup.lua")
xaphu_npc = npcpos("WLLS_NPC_SIGNUP", "chefu.lua")
ruong_npc = npcpos("WLLS_NPC_SIGNUP", "chuwuxiang.lua")
thuoc_npc = npcpos("WLLS_NPC_SIGNUP", "yaoshang.lua")
thive_npc = npcpos("WLLS_NPC_PREP", "housecarl.lua")
for nm, v in (("signup", sign_npc), ("chefu", xaphu_npc),
              ("chuwuxiang", ruong_npc), ("housecarl", thive_npc)):
    if not v:
        raise SystemExit("khong doc duoc toa do NPC %s" % nm)

_off = npcname("officer.lua")
NPCNAME = [
    ("LDM_NPC_SUGIA", prefix(_off[0], _off[1]).strip()),
    ("LDM_NPC_QUANVIEN", npcname("signup.lua")[0].strip()),
    ("LDM_NPC_XAPHU", npcname("chefu.lua")[0].strip()),
    ("LDM_NPC_RUONG", npcname("chuwuxiang.lua")[0].strip()),
    ("LDM_NPC_THIVE", npcname("housecarl.lua")[0].strip()),
]
for nm, v in NPCNAME:
    if len(v) < 4:
        raise SystemExit("ten NPC qua ngan: %s = %r" % (nm, v))

# ---------------------------------------------------------------- Xa phu hoi truong
ve = re.findall(r'"([^"/]+)/#wlls_want2go\((\d+)\)"', chefu)
if len(ve) < 7:
    raise SystemExit("khong doc du 7 dong Xa phu hoi truong")

# ---------------------------------------------------------------- marker thoai
MARK = [
    ("LDM_OPT_KHUTHIDAU", pick(officer, r'"(Ta[^"]*?khu thi [^"]*?h[^"]*?ng )"\.\.str_des',
                               "Ta muon den khu thi dau hang")),
    ("LDM_OPT_CHIENDOI", pick(officer, r'"(Chi[^"]*?i h[^"]*?ng )"\.\.str_des\.\."[^"]*?/wlls_mylg"',
                              "Chien doi hang ... cua ta")),
    ("LDM_OPT_THUONGLD", pick(func(officer, "main"),
                              r'tb_option\[getn\(tb_option\) \+ 1\] = "([^"/]*)/wlls_wantaward"',
                              "lanh phan thuong lien dau")),
    ("LDM_OPT_LAPDOI", pick(officer, r'"([^"/]*)/#wlls_createleague\(\)"', "Ta muon lap chien doi!")),
    ("LDM_OPT_LAPNHOM", pick(officer, r'"([^"/]*)/wlls_inputstr_lgname"', "Ta muon lap nhom!")),
    ("LDM_OPT_XEPHANG", pick(officer, r'"([^"/]*)/wlls_wantaward_rank"', "lanh phan thuong xep hang")),
    ("LDM_OPT_LANHGIAI", pick(officer, r'"([^"/]*)/wlls_getaward_rank"', "Ta muon lanh giai thuong!")),
    ("LDM_OPT_DANHHIEU", pick(officer, r'"([^"/]*)/wlls_wantaward_title"', "lanh giai thuong danh hieu")),
    ("LDM_OPT_THAMCHIEN", pick(signup, r'"([^"/]*)/wlls_en1"', "Ta muon tham chien!")),
    ("LDM_OPT_SANSANG", pick(signup, r'"([^"/]*)/wlls_en2"', "Ta da san sang!")),
    ("LDM_OPT_XACNHAN", pick(signup, r'"([^"/]*)/wlls_en3"', "Xac nhan")),
    ("LDM_SAY_DOCAM", pick(func(head, "wlls_en_check"), r'Say\("([^"]+)", 0\)', "loi mang do cam")),
    ("LDM_SAY_DONGCUA", pick(func(officer, "wlls_want2signmap"),
                             r'Say\(wlls_npcname\(\)\.\."([^"]+)", 0\)', "hoi truong dong cua")),
    ("LDM_SAY_TAMNGHI", pick(func(signup, "wlls_ready2join"),
                             r'n_matchphase == 3.*?Say\("([^"!]+)!', "pha 3 tam nghi")),
    ("LDM_SAY_DANGDAU", pick(func(signup, "wlls_ready2join"),
                             r'n_matchphase == 5.*?Say\("([^"!]+)!', "pha 5 dang thi dau")),
    ("LDM_SAY_CHUATHIDAU", pick(func(signup, "wlls_ready2join"),
                                r'n_matchphase < 3.*?Say\("([^",?]+)', "pha <3 chua thi dau")),
    ("LDM_MSG_BAODANH", pick(gmscript,
                             r'TB_WLLS_PHASE_MSG\s*=\s*\{[^{}]*?"[^"]*?%d ([^"]*?)!"',
                             "tin toan may chu mo bao danh")),
    ("LDM_MSG_BATDAU", pick(sched,
                            r'Msg2SubWorld\("[^"]*?, (thi [^"]*?u ch[^"]*?nh th[^"]*?c b[^"]*?t [^"]*?u)!"',
                            "tin bat dau thi dau")),
]
# "Ta muon lap" - tien to CHUNG cua "Ta muon lap nhom!" va "Ta muon lap chien doi!";
# dong o menu chien doi la "Ta muon lap"..str_des.."nhom" (KHONG co dau cach sau "lap")
# nen phai cat sach dau cach cuoi.
_dmark = dict(MARK)
MARK.append(("LDM_OPT_LAP", prefix(_dmark["LDM_OPT_LAPNHOM"], _dmark["LDM_OPT_LAPDOI"]).rstrip()))
for nm, v in MARK:
    if len(v) < 6:
        raise SystemExit("marker qua ngan: %s = %r" % (nm, v))

# ---------------------------------------------------------------- do cam
m = re.search(r"WLLS_FORBID_ITEM\s*=\s*\{(.*?)" + NL + r"\}", head, re.S)
forbid = []
for g, d, p in re.findall(r"\{\s*(\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*-?\d+\s*,",
                          re.sub(r'"[^"]*"', '""', m.group(1))):
    t = (int(g), int(d), int(p))
    if t not in forbid:
        forbid.append(t)
if len(forbid) < 40:
    raise SystemExit("bang do cam chi doc duoc %d muc - sai regex?" % len(forbid))

# ---------------------------------------------------------------- xuat
L = []
A = L.append
A("// KLienDauTables.h - SINH TU DONG boi ReverseTools/gen_liendau_tables.py - DUNG SUA TAY")
A("// Nguon: script song cua may chu (%s)" % SRV.replace(BS, "/"))
A("// Marker la RAW TCVN3 trich thang tu Lua (byte-for-byte) - giong KTongKimTables.h.")
A("#ifndef KLIENDAUTABLES_H")
A("#define KLIENDAUTABLES_H")
A("")
A("// ===== hang so he Lien dau (missions/leaguematch/head.lua) =====")
A("#define LD_LEVEL_JUNIOR\t%d\t// cap toi thieu hang Kiet xuat" % LEVEL_JUNIOR)
A("#define LD_LEVEL_SENIOR\t%d\t// cap toi thieu hang Vo lam" % LEVEL_SENIOR)
A("#define LD_PREP_SEC\t%d\t// giay o khu chuan bi truoc khi vao san (PREP_FREQ*PREP_TOTAL)"
  % (PREP_FREQ * PREP_TOTAL))
A("#define LD_FIGHT_SEC\t%d\t// giay toi da mot tran (FIGHT_FREQ*FIGHT_TOTAL)"
  % (FIGHT_FREQ * FIGHT_TOTAL))
A("#define LD_PHUT_LUOT\t%d\t// so phut mot luot (WLLS_MATCHTIME - doi duoc o wlls_config.lua)"
  % MATCHTIME)
A("")
A("struct LDPoint { short x, y; };")
A("")
A("// ===== toa do chuan trong hoi truong / khu chuan bi (don vi O) =====")
A("static const LDPoint g_LDPosHall = { %d, %d };\t// diem dap khi vao hoi truong" % SIGN)
A("static const LDPoint g_LDPosPrep = { %d, %d };\t// diem dap khu chuan bi" % PREP)
A("")
A("// ===== bang map (hoi truong / khu chuan bi / dau truong) =====")


def arr(name, vals):
    A("#define %s_COUNT\t%d" % (name.upper(), len(vals)))
    A("static const short %s[%s_COUNT] = {" % (name, name.upper()))
    for i in range(0, len(vals), 10):
        A("\t" + ", ".join(str(v) for v in vals[i:i + 10]) + ",")
    A("};")


arr("g_LDHall", sorted(halls))
arr("g_LDPrep", sorted(preps))
arr("g_LDArena", sorted(arenas))
A("")
A("// ===== 7 thanh co Su gia lien dau: { mapid, hang(1 Kiet xuat/2 Vo lam), x, y } =====")
A("#define LD_CITY_COUNT\t%d" % len(citypos))
A("static const short g_LDCity[LD_CITY_COUNT][4] = {")
for mid in sorted(citypos, key=lambda k: (cityhang[k], k)):
    x, y = citypos[mid]
    A("\t{ %d, %d, %d, %d }," % (mid, cityhang[mid], x, y))
A("};")
A("")
A("// ===== NPC trong hoi truong / khu chuan bi (don vi O) =====")
A("#define LD_SIGNUP_COUNT\t%d" % len(sign_npc))
A("static const LDPoint g_LDNpcSignup[LD_SIGNUP_COUNT] = {\t// Quan vien hoi truong")
for x, y in sign_npc:
    A("\t{ %d, %d }," % (x, y))
A("};")
A("static const LDPoint g_LDNpcXaPhu = { %d, %d };\t// Xa phu hoi truong" % xaphu_npc[0])
A("static const LDPoint g_LDNpcRuong = { %d, %d };\t// Ruong chua do (OpenBox)" % ruong_npc[0])
if thuoc_npc:
    A("static const LDPoint g_LDNpcThuoc = { %d, %d };\t// Chu duoc diem" % thuoc_npc[0])
A("static const LDPoint g_LDNpcThiVe = { %d, %d };\t// Thi ve hoi truong (khu chuan bi)" % thive_npc[0])
A("")
A("// ===== Xa phu hoi truong: dong thoai -> map thanh =====")
A("#define LD_VE_COUNT\t%d" % len(ve))
A("static const short g_LDVeMap[LD_VE_COUNT] = { %s };" % ", ".join(mid for _, mid in ve))
A("static const char* const g_LDVeOpt[LD_VE_COUNT] = {")
for opt, _ in ve:
    A("\t%s," % cstr(opt.rstrip()))
A("};")
A("")
A("// ===== marker hoi thoai / thong bao (RAW TCVN3) =====")
for nm, v in MARK:
    A("static const char %s[] = %s;" % (nm, cstr(v)))
A("")
A("// ten NPC de DT_FindNpcName (ha thuong CHI ASCII nhu g_StrLower - byte co dau giu nguyen)")
for nm, v in NPCNAME:
    A("static const char %s[] = %s;" % (nm, cstr(lowascii(v))))
A("")
A("// ===== do CAM mang vao san (WLLS_FORBID_ITEM) - auto phai cat vao ruong truoc =====")
A("// { genre, detail, particular }")
A("#define LD_FORBID_COUNT\t%d" % len(forbid))
A("static const short g_LDForbid[LD_FORBID_COUNT][3] = {")
for g, d, p in forbid:
    A("\t{ %d, %d, %d }," % (g, d, p))
A("};")
A("")
A("#endif // KLIENDAUTABLES_H")

with io.open(os.path.abspath(OUT), "w", encoding="latin-1", newline=chr(13) + NL) as f:
    f.write(NL.join(L) + NL)

print("da ghi %s" % os.path.abspath(OUT))
print("  hoi truong %d map / chuan bi %d / dau truong %d" % (len(halls), len(preps), len(arenas)))
print("  thanh co Su gia: %d ; do cam: %d muc" % (len(citypos), len(forbid)))
for nm, v in MARK + NPCNAME:
    print("  %-20s %s" % (nm, v))
