# -*- coding: utf-8 -*-
"""vhtd_faction_script.py - VU HON (faction id 11, FACTION_INFO[12]) / TIEU DAO (id 12, FACTION_INFO[13]): script mon phai may chu.
Khuon theo dot Hoa Son (marker [HOASON 01/09]) -> marker [VHTD 02/09]. Nguon du lieu: client VLTK Level Up (Linux KHONG co 2 phai).
  header\\factionhead.lua      FACTION_INFO +2, SKILLNORMAL [12]/[13], SKILL90_ARRAY [12]/[13], SKILL120AR +1984,2127, SKILL150_ARRAY (neu co)
  global\\hocvocong.lua        bang rieng: SKILLNORMAL/SKILL90/SKILL120/SKILL150/FACTION_TO_SKILL150 + menu hvcwh/hvcxy + skillvuhon/skilltieudao
  lib\\lib_faction.lua         tbSkillBase/tbSkill120/tbSkill150 + dieu kien nFactionID 12/13 + GetFactionEx
  item\\skillbook.lua          [11]/[12] + sach 90: 4968/4969 (Vu Hon), 4975/4976 (Tieu Dao)
  item\\lvl120skillbook.lua    [11] 1984, [12] 2127
  item\\lenhbaitanthu.lua      menu doi phai + nhanh 12 (SetSeries 3) / 13 (SetSeries 4)
  skill\\skillfaction.lua      ten GBK -> 12/13; mainskill 1982/2123
  skill\\nangskillkieumoi5x/9x/tp.lua, item\\daithanhbk90/120.lua, npcthon\\balanghuyen\\hotrotest.lua
  global\\skills_table.lua     add_wh / add_xy (khuon add_hs)
  MOI: npcthon\\npcmonphai\\vuhon.lua, tieudao.lua (khuon hoason.lua), global\\vhtd\\npc_vhtd.lua (spawn) + startgame.lua Include/goi
Doc/ghi latin-1, giu CRLF, ban luu .truoc_vhtd_0209, idempotent. DUNG: python vhtd_faction_script.py [--kiem] [--bo-kiem-npc]
"""
import io, os, sys, re, shutil
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
SRV = os.path.join(BIN, "server")
SCR = os.path.join(SRV, "script")
RAW = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw"
KIEM = "--kiem" in sys.argv
BAK = ".truoc_vhtd_0209"
M = "[VHTD 02/09]"
BS = chr(92)

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes, _UNICODE_TO_TCVN3; _TCVN3_TO_UNICODE = {ord(v): k for k, v in _UNICODE_TO_TCVN3.items()}   # bang TCVN3 da kiem chung tren byte that cua JX1 (Hoc vo cong = 48 E4 63 ...)
def V(s): return unicode_to_tcvn3_bytes(s).decode("latin-1")
def G(s): return s.encode("gbk").decode("latin-1")
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def hib(s): return sum(1 for c in s if ord(c) >= 0x80)

WH_GBK, XY_GBK, HS_GBK = G(u"\u6b66\u9b42\u5802"), G(u"\u900d\u9065\u6d3e"), G(u"\u534e\u5c71\u6d3e")   # 武魂堂 / 逍遥派 / 华山派

# ------------------------------------------------------------------ ten ky nang (TCVN3) tu skills.txt may chu (da chep tu VLTK)
NAME = {}
_sk = rd(os.path.join(SRV, r"settings\skills.txt")).split("\r\n")
for _l in _sk[1:]:
    _f = _l.split("\t")
    if len(_f) > 2 and _f[2].isdigit(): NAME[int(_f[2])] = _f[0].strip()   # cot 0 = SkillName (cot 1 la Property)
def nm(i):
    if i not in NAME: raise SystemExit("skills.txt may chu chua co ky nang %d (chay vhtd_skills.py truoc)" % i)
    return NAME[i]
# ten vat pham VLTK (TCVN3) cho sach 90
ITEM = {}
for _l in rd(os.path.join(RAW, "settings__item__004__magicscript.txt")).replace("\r\n", "\n").split("\n")[1:]:
    _f = _l.split("\t")
    if len(_f) > 13 and _f[3].isdigit(): ITEM[int(_f[3])] = _f[0]

# bo ky nang (client VLTK wuhuntang.lua / xiaoyao.lua tbSkillID + skills.txt) --------------------------------------------
WH_LV = [(10, [1972, 1974]), (20, [1964, 1975]), (30, [1976]), (40, [1965, 1977]), (50, [1963, 1979]), (60, [1980]), (70, [1982, 1971, 1981])]
XY_LV = [(10, [2114, 2136]), (20, [2115, 2137]), (30, [2116]), (40, [2118, 2138]), (50, [2120, 2140]), (60, [2121]), (70, [2123, 2122])]
WH_NORMAL = [i for _, l in WH_LV for i in l]
XY_NORMAL = [i for _, l in XY_LV for i in l]
WH90, WH120, WH150, WH_MAIN = [1967, 1983], 1984, [1969, 1985], 1982
XY90, XY120, XY150, XY_MAIN = [2124, 2141], 2127, [2129, 2142], 2123

class F:
    def __init__(self, rel):
        self.p = os.path.join(SCR, rel); self.rel = rel
        self.s = rd(self.p); self.orig = self.s; self.n = 0
        self.nl = "\r\n" if "\r\n" in self.s else "\n"
    def has(self, t): return t in self.s
    def rep(self, old, new, tag, count=1):
        if new in self.s: print("  [=] %s: da ap (%s)" % (self.rel, tag)); return False
        them = new.replace(old, "", 1) if old in new else ""
        if them.strip() and them in self.s: print("  [=] %s: da ap (%s) - phan them da co" % (self.rel, tag)); return False
        c = self.s.count(old)
        if c == 0:
            if M in self.s and tag in self.s: print("  [=] %s: da ap (%s)" % (self.rel, tag)); return False
            raise SystemExit("KHONG THAY anchor %s (%s):\n%r" % (self.rel, tag, old[:160]))
        if count and c != count: raise SystemExit("anchor %s (%s) xuat hien %d lan (cho %d)" % (self.rel, tag, c, count))
        self.s = self.s.replace(old, new); self.n += 1; print("  [+] %s: %s" % (self.rel, tag)); return True
    def rep_re(self, pat, fn, tag, flags=re.M):
        m = re.search(pat, self.s, flags)
        if not m:
            if M in self.s: print("  [=] %s: da ap (%s) - regex khong khop, file da co marker" % (self.rel, tag)); return False
            raise SystemExit("KHONG THAY regex %s (%s): %s" % (self.rel, tag, pat))
        new = fn(m)
        if new == m.group(0): print("  [=] %s: da ap (%s)" % (self.rel, tag)); return False
        them = new.replace(m.group(0), "", 1) if m.group(0) in new else ""
        if them.strip() and them in self.s: print("  [=] %s: da ap (%s) - phan them da co" % (self.rel, tag)); return False
        self.s = self.s[:m.start()] + new + self.s[m.end():]; self.n += 1; print("  [+] %s: %s" % (self.rel, tag)); return True
    def save(self):
        if self.s == self.orig: print("  (khong doi) %s" % self.rel); return
        if "\xef\xbf\xbd" in self.s: raise SystemExit("EF BF BD trong " + self.rel)
        if not KIEM:
            if not os.path.exists(self.p + BAK): shutil.copy2(self.p, self.p + BAK)
            io.open(self.p, "w", encoding="latin-1", newline="").write(self.s)
        print("  => ghi %s (%d cho)%s" % (self.rel, self.n, " KIEM" if KIEM else ""))

def lv_block(lst, indent, nl, tag):
    """{id,0},---ten  (ten ASCII hoa: bo dau bang cach giu nguyen TCVN3 -> dung comment ASCII id)"""
    out = []
    for k, i in enumerate(lst):
        out.append("%s{%d,0}%s---%s" % (indent, i, "," if k < len(lst) - 1 else "", asc(nm(i))))
    return nl.join(out)
def asc(t):  # TCVN3 -> ASCII khong dau cho comment (bang nguoc vn_to_octal + NFD)
    import unicodedata
    out = []
    for c in t:
        if ord(c) < 0x80: out.append(c); continue
        u = _TCVN3_TO_UNICODE.get(ord(c))
        if u is None: out.append('?'); continue
        u = u.replace(u'\u0111', 'd').replace(u'\u0110', 'D')
        out.append(''.join(ch for ch in unicodedata.normalize('NFD', u) if not unicodedata.combining(ch)))
    return ''.join(out)
def arr90(lst, indent, nl):
    return nl.join('%s{"%s",%d,1}%s' % (indent, nm(i), i, "," if k < len(lst) - 1 else "") for k, i in enumerate(lst))

# ================================================================== 1. factionhead.lua
def factionhead():
    f = F(r"header\factionhead.lua"); nl = f.nl
    # FACTION_INFO: sau dong Hoa Son  {2, "华山派", 3, 89, ...},
    def fi(m):
        line = m.group(0)
        add = (nl + '\t{3, "%s", 1, 90, "%s", "%s"},\t-- %s Vu Hon id 11 (he Hoa, chinh phai, rank 90 Hau Quan - client VLTK faction_settings)' % (WH_GBK,
               V(u"Hoan ngh\u00eanh b\u1ea1n gia nh\u1ead p V\u0169 H\u1ed3n \u0110\u01b0\u1eddng, tr\u1edf th\u00e0nh H\u1ead u Qu\u00e2n! H\u1ecdc \u0111\u01b0\u1ee3c v\u00f5 c\u00f4ng V\u0169 H\u1ed3n Thu\u1eabn Ph\u00e1p, V\u0169 H\u1ed3n \u0110ao Ph\u00e1p".replace(u"nh\u1ead p", u"nh\u1eadp").replace(u"H\u1ead u", u"H\u1eadu")),
               V(u"Gia nh\u1eadp V\u0169 H\u1ed3n \u0110\u01b0\u1eddng"), M)
             + nl + '\t{4, "%s", 3, 98, "%s", "%s"},\t-- %s Tieu Dao id 12 (he Tho, trung lap, rank 98 Tieu Dao Cu Si)' % (XY_GBK,
               V(u"Hoan ngh\u00eanh b\u1ea1n gia nh\u1eadp Ti\u00eau Dao ph\u00e1i, tr\u1edf th\u00e0nh Ti\u00eau Dao C\u01b0 S\u0129! H\u1ecdc \u0111\u01b0\u1ee3c v\u00f5 c\u00f4ng Ti\u00eau Dao Ki\u1ebfm Ph\u00e1p, Ti\u00eau Dao C\u1ea7m Ph\u00e1p"),
               V(u"Gia nh\u1eadp Ti\u00eau Dao ph\u00e1i"), M))
        return line + add
    if WH_GBK not in f.s.split("SKILLNORMAL")[0]:
        f.rep_re(r'^\t\{2, "%s", 3, 89, [^\r\n]*' % re.escape(HS_GBK), fi, "FACTION_INFO +2")
    else: print("  [=] factionhead FACTION_INFO da co")
    # SKILLNORMAL
    old = "\t{1380,0}---Ma Van Kiem Khi" + nl + "}" + nl + "};"
    new = ("\t{1380,0}---Ma Van Kiem Khi" + nl + "}," + nl + "[12]={--vu hon %s (client VLTK wuhuntang.lua tbSkillID 10..70)" % M + nl + lv_block(WH_NORMAL, "\t", nl, M)
           + nl + "}," + nl + "[13]={--tieu dao %s (client VLTK xiaoyao.lua tbSkillID 10..70)" % M + nl + lv_block(XY_NORMAL, "\t", nl, M) + nl + "}" + nl + "};")
    f.rep(old, new, "SKILLNORMAL [12]/[13]")
    # SKILL90_ARRAY
    old = ",1382,1}" + nl + "}" + nl + "};"
    new = (",1382,1}" + nl + "}," + nl + "[12]={ -- %s" % M + nl + arr90(WH90, "\t", nl) + nl + "}," + nl + "[13]={ -- %s" % M + nl + arr90(XY90, "\t", nl) + nl + "}" + nl + "};")
    f.rep(old, new, "SKILL90_ARRAY [12]/[13]")
    # SKILL120AR
    f.rep("717,1365 -- 1365 = Tu Ha Kiem Khi (Hoa Son)", "717,1365,1984,2127 -- 1365 = Tu Ha Kiem Khi (Hoa Son); 1984 Vu Hon, 2127 Tieu Dao %s" % M, "SKILL120AR")
    # SKILL150_ARRAY (neu factionhead co)
    if "SKILL150_ARRAY" in f.s and (",1384,1}" + nl + "}" + nl + "};") in f.s:
        f.rep(",1384,1}" + nl + "}" + nl + "};", ",1384,1}" + nl + "}," + nl + "[12]={ -- %s" % M + nl + arr90(WH150, "\t", nl) + nl + "}," + nl + "[13]={ -- %s" % M + nl + arr90(XY150, "\t", nl) + nl + "}" + nl + "};", "SKILL150_ARRAY [12]/[13]")
    f.save()

# ================================================================== 2. hocvocong.lua
def hocvocong():
    f = F(r"global\hocvocong.lua"); nl = f.nl
    old = "\t\t{1380,0}---Ma Van Kiem Khi" + nl + "\t}" + nl + "}"
    new = ("\t\t{1380,0}---Ma Van Kiem Khi" + nl + "\t}," + nl + "\t[12]={--vu hon %s" % M + nl + lv_block(WH_NORMAL, "\t\t", nl, M) + nl + "\t},"
           + nl + "\t[13]={--tieu dao %s" % M + nl + lv_block(XY_NORMAL, "\t\t", nl, M) + nl + "\t}" + nl + "}")
    f.rep(old, new, "SKILLNORMAL [12]/[13]")
    old = ",1382,1}" + nl + "\t}" + nl + "};"
    new = (",1382,1}" + nl + "\t}," + nl + "\t[12]={ -- %s" % M + nl + arr90(WH90, "\t\t", nl) + nl + "\t}," + nl + "\t[13]={ -- %s" % M + nl + arr90(XY90, "\t\t", nl) + nl + "\t}" + nl + "};")
    f.rep(old, new, "SKILL90_ARRAY [12]/[13]")
    f.rep("\t709,708,710,711,712,713,714,715,716,717,1365" + nl + "}", "\t709,708,710,711,712,713,714,715,716,717,1365,1984,2127 -- %s" % M + nl + "}", "SKILL120AR")
    old = ",1384,1}" + nl + "\t}" + nl + "}"
    new = (",1384,1}" + nl + "\t}," + nl + "\t[12]={ -- %s" % M + nl + arr90(WH150, "\t\t", nl) + nl + "\t}," + nl + "\t[13]={ -- %s" % M + nl + arr90(XY150, "\t\t", nl) + nl + "\t}" + nl + "}")
    f.rep(old, new, "SKILL150_ARRAY [12]/[13]")
    f.rep_re(r"^(\t\[11\] = 11,[ \t]*)(?=\r?\n)", lambda m: m.group(1) + nl + "\t[12] = 12, -- %s" % M + nl + "\t[13] = 13,", "FACTION_TO_SKILL150")
    # menu hoc vo cong (hocvocong2)
    hs_menu = '"%s/hvchs",' % V(u"H\u1ecdc v\u00f5 c\u00f4ng m\u00f4n ph\u00e1i Hoa S\u01a1n")
    if '/hvcwh"' not in f.s:
        i0 = f.s.find("function hocvocong2()"); i1 = f.s.find(nl + "end", i0)
        body = f.s[i0:i1]
        if hs_menu not in body: raise SystemExit("hocvocong2 khong co menu Hoa Son")
        body2 = body.replace(hs_menu, hs_menu + nl + '\t"%s/hvcwh",' % V(u"H\u1ecdc v\u00f5 c\u00f4ng m\u00f4n ph\u00e1i V\u0169 H\u1ed3n") + nl + '\t"%s/hvcxy",' % V(u"H\u1ecdc v\u00f5 c\u00f4ng m\u00f4n ph\u00e1i Ti\u00eau Dao"), 1)
        body2 = re.sub(r'(Say\("[^"]*",)7,', r'\g<1>9,', body2, count=1)
        if body2 == body or ",9," not in body2: raise SystemExit("hocvocong2: khong doi duoc so muc Say")
        f.s = f.s[:i0] + body2 + f.s[i1:]; f.n += 1; print("  [+] hocvocong2 menu +2")
    else: print("  [=] hocvocong2 menu da co")
    old = "function hvchs()\t-- [HOASON 01/09c] Hoa Son = SKILLNORMAL[11]" + nl + '\t--HoTroSkill("huashan")' + nl + "\thockynang(11)" + nl + "end"
    f.rep(old, old + nl + "function hvcwh()\t-- %s Vu Hon = SKILLNORMAL[12]" % M + nl + "\thockynang(12)" + nl + "end" + nl + "function hvcxy()\t-- %s Tieu Dao = SKILLNORMAL[13]" % M + nl + "\thockynang(13)" + nl + "end", "hvcwh/hvcxy")
    # test skill 90 (hocskill902)
    hs90 = '"%s/skillhoason",' % V(u"Hoa S\u01a1n 90")
    if "/skillvuhon" not in f.s:
        i0 = f.s.find("function hocskill902()"); i1 = f.s.find(nl + "end", i0)
        body = f.s[i0:i1]
        if hs90 not in body: raise SystemExit("hocskill902 khong co Hoa Son 90")
        body2 = body.replace(hs90, hs90 + nl + '\t"%s/skillvuhon","%s/skilltieudao",' % (V(u"V\u0169 H\u1ed3n 90"), V(u"Ti\u00eau Dao 90")), 1)
        body2 = re.sub(r'(Say\("[^"]*",)7,', r'\g<1>9,', body2, count=1)
        if ",9," not in body2: raise SystemExit("hocskill902: khong doi duoc so muc Say")
        f.s = f.s[:i0] + body2 + f.s[i1:]; f.n += 1; print("  [+] hocskill902 menu +2")
    else: print("  [=] hocskill902 da co")
    old = "function skillhoason()\t-- [HOASON 01/09c]" + nl + "\tadd_hs(90);" + nl
    i = f.s.find(old)
    if i < 0: raise SystemExit("khong thay skillhoason")
    j = f.s.find(nl + "end", i) + len(nl) + 3
    blk = f.s[i:j]
    new_fn = (nl + nl + "function skillvuhon()\t-- %s" % M + nl + "\tadd_wh(90);" + nl + '\tSay("%s",0)' % V(u"Ng\u01b0\u01a1i \u0111\u00e3 h\u1ecdc \u0111\u01b0\u1ee3c skill 90") + nl + "end"
              + nl + nl + "function skilltieudao()\t-- %s" % M + nl + "\tadd_xy(90);" + nl + '\tSay("%s",0)' % V(u"Ng\u01b0\u01a1i \u0111\u00e3 h\u1ecdc \u0111\u01b0\u1ee3c skill 90") + nl + "end")
    if "function skillvuhon()" not in f.s:
        f.s = f.s[:j] + new_fn + f.s[j:]; f.n += 1; print("  [+] skillvuhon/skilltieudao")
    f.save()

# ================================================================== 3. lib_faction.lua
def lib_faction():
    f = F(r"lib\lib_faction.lua"); nl = f.nl
    old = "\t{1347,1372,1349,1374,1350,1375,1351,1376,1354,1378,1355,1379,1358,1360,1380}, -- [HOASON 01/09c] Hoa Son"
    f.rep(old, old + nl + "\t{%s}, -- %s Vu Hon" % (",".join(str(i) for i in WH_NORMAL), M) + nl + "\t{%s}, -- %s Tieu Dao" % (",".join(str(i) for i in XY_NORMAL), M), "tbSkillBase +2")
    old = '\t{"%s",1365}, -- [HOASON 01/09c]' % nm(1365)
    if old not in f.s:
        m = re.search(r'^\t\{"[^"\r\n]*",1365\}, -- \[HOASON 01/09c\](?=\r?\n)', f.s, re.M)
        if not m: raise SystemExit("lib_faction tbSkill120 khong thay dong 1365")
        old = m.group(0)
    f.rep(old, old + nl + '\t{"%s",1984}, -- %s Vu Hon' % (nm(1984), M) + nl + '\t{"%s",2127}, -- %s Tieu Dao' % (nm(2127), M), "tbSkill120 +2")
    # tbSkill150: sau khoi Hoa Son
    i = f.s.find("\t{ -- [HOASON 01/09c] Hoa Son")
    if i < 0: raise SystemExit("lib_faction tbSkill150 khong thay khoi Hoa Son")
    j = f.s.find(nl + "\t},", i) + len(nl) + 3
    if "-- %s Vu Hon 150" % M not in f.s:
        add = (nl + "\t{ -- %s Vu Hon 150" % M + nl + '\t\t{"%s", 1969},' % nm(1969) + nl + '\t\t{"%s", 1985},' % nm(1985) + nl + "\t},"
               + nl + "\t{ -- %s Tieu Dao 150" % M + nl + '\t\t{"%s", 2129},' % nm(2129) + nl + '\t\t{"%s", 2142},' % nm(2142) + nl + "\t},")
        f.s = f.s[:j] + add + f.s[j:]; f.n += 1; print("  [+] tbSkill150 +2")
    c = f.s.count("or nFactionID == 11)")
    if c: f.s = f.s.replace("or nFactionID == 11)", "or nFactionID == 11 or nFactionID == 12 or nFactionID == 13)"); f.n += 1; print("  [+] dieu kien nFactionID 12/13 (%d cho)" % c)
    elif "nFactionID == 13)" in f.s: print("  [=] dieu kien da co")
    else: raise SystemExit("lib_faction: khong thay 'or nFactionID == 11)'")
    pat = re.compile(r'^(\tif nFaction == "%s" then F = "%s"\treturn F end -- \[HOASON 01/09\])(?=\r?\n)' % (re.escape(HS_GBK), re.escape(V(u"Hoa S\u01a1n"))), re.M)
    ms = list(pat.finditer(f.s))
    if not ms: raise SystemExit("lib_faction GetFactionEx khong thay dong Hoa Son")
    if WH_GBK not in f.s:
        for m in reversed(ms):
            add = (nl + '\tif nFaction == "%s" then F = "%s"\treturn F end -- %s' % (WH_GBK, V(u"V\u0169 H\u1ed3n"), M)
                   + nl + '\tif nFaction == "%s" then F = "%s"\treturn F end -- %s' % (XY_GBK, V(u"Ti\u00eau Dao"), M))
            f.s = f.s[:m.end()] + add + f.s[m.end():]
        f.n += 1; print("  [+] GetFactionEx (%d ham)" % len(ms))
    f.save()

# ================================================================== 4. skillbook.lua / lvl120skillbook.lua
def skillbook():
    f = F(r"item\skillbook.lua"); nl = f.nl
    old = "\t[10] = {[90] = {1364, 1382}, [120] = {1365}, [150] = {1369, 1384},},\t-- [HOASON 01/09] Hoa Son"
    f.rep(old, old + nl + "\t[11] = {[90] = {1967, 1983}, [120] = {1984}, [150] = {1969, 1985},},\t-- %s Vu Hon" % M
               + nl + "\t[12] = {[90] = {2124, 2141}, [120] = {2127}, [150] = {2129, 2142},},\t-- %s Tieu Dao" % M, "bang phai 11/12")
    m = re.search(r"^\t\[4939\] = \{1382, 1, 10, 80, [^\r\n]*", f.s, re.M)
    if not m: raise SystemExit("skillbook khong thay [4939]")
    if "[4968]" not in f.s:
        add = "".join(nl + '\t[%d] = {%d, 1, %d, 80, "%s",\t"%s"},\t-- %s' % (nid, sk, fac, ITEM[vid], nm(sk), M)
                      for nid, vid, sk, fac in ((4968, 5106, 1967, 11), (4969, 5107, 1983, 11), (4975, 5231, 2124, 12), (4976, 5232, 2141, 12)))
        f.s = f.s[:m.end()] + add + f.s[m.end():]; f.n += 1; print("  [+] sach 90: 4968/4969/4975/4976")
    f.save()
    f = F(r"item\lvl120skillbook.lua"); nl = f.nl
    m = re.search(r"^\t\[10\] = \{1365, 1, 10, 120, [^\r\n]*", f.s, re.M)
    if not m: raise SystemExit("lvl120skillbook khong thay [10]")
    if "[11] = {1984" not in f.s:
        add = (nl + '\t[11] = {1984, 1, 11, 120, "%s",\t"%s"},\t-- %s' % (V(u"K\u1ef9 n\u0103ng c\u1ea5p 120"), nm(1984), M)
               + nl + '\t[12] = {2127, 1, 12, 120, "%s",\t"%s"},\t-- %s' % (V(u"K\u1ef9 n\u0103ng c\u1ea5p 120"), nm(2127), M))
        f.s = f.s[:m.end()] + add + f.s[m.end():]; f.n += 1; print("  [+] [11]/[12]")
    f.save()

# ================================================================== 5. lenhbaitanthu.lua
def lenhbai():
    f = F(r"item\lenhbaitanthu.lua"); nl = f.nl
    old = '"%s /doiphai1") -- [HOASON 01/09]' % V(u"Hoa S\u01a1n")
    if old in f.s:
        i = f.s.find(old)
        head = f.s[:i]
        k = head.rfind(",11,")
        if k < 0 or i - k > 1500: raise SystemExit("lenhbaitanthu: khong thay so muc Say 11 truoc menu")
        f.s = head[:k] + ",13," + head[k + 4:] + '"%s /doiphai1",' % V(u"Hoa S\u01a1n") + nl + '"%s /doiphai1",' % V(u"V\u0169 H\u1ed3n") + nl + '"%s /doiphai1") -- [HOASON 01/09] %s 13 phai' % (V(u"Ti\u00eau Dao"), M) + f.s[i + len(old):]
        f.n += 1; print("  [+] menu doi phai 13 muc")
    else: print("  [=] menu doi phai da co")
    pat = re.compile(r"(\thockynang\(nCurFac\)\r?\n)(end[ \t]*\r?\n\r?\nlocal tbSkillNew = SKILL90_ARRAY\[nCurFac\])")
    def fn(m):
        return (m.group(1) + "elseif nCurFac == 12 then -- Vu Hon (he Hoa) %s" % M + nl + "\tSetSeries(3)" + nl + "\tSetFaction(FACTION_INFO[nCurFac][2])" + nl + "\tSetCamp(FACTION_INFO[nCurFac][3])" + nl
                + "\tSetCurCamp(FACTION_INFO[nCurFac][3])" + nl + "\tSetRank(FACTION_INFO[nCurFac][4])" + nl + "\thockynang(nCurFac)" + nl
                + "elseif nCurFac == 13 then -- Tieu Dao (he Tho) %s" % M + nl + "\tSetSeries(4)" + nl + "\tSetFaction(FACTION_INFO[nCurFac][2])" + nl + "\tSetCamp(FACTION_INFO[nCurFac][3])" + nl
                + "\tSetCurCamp(FACTION_INFO[nCurFac][3])" + nl + "\tSetRank(FACTION_INFO[nCurFac][4])" + nl + "\thockynang(nCurFac)" + nl + m.group(2))
    if "nCurFac == 12 then -- Vu Hon" not in f.s: f.rep_re(pat.pattern, fn, "doiphai1 nhanh 12/13", flags=0)
    else: print("  [=] doiphai1 da co")
    f.save()

# ================================================================== 6. skillfaction.lua + nang skill + dai thanh bi kip + hotrotest
def skill_misc():
    f = F(r"skill\skillfaction.lua"); nl = f.nl
    old = 'elseif (player_Faction == "%s") then -- [HOASON 01/09c]' % HS_GBK + nl + "return 11" + nl
    f.rep(old, old + 'elseif (player_Faction == "%s") then -- %s' % (WH_GBK, M) + nl + "return 12" + nl + 'elseif (player_Faction == "%s") then -- %s' % (XY_GBK, M) + nl + "return 13" + nl, "phai -> 12/13")
    old = '{1358,"Huyen Nhan Van Yen"} -- [HOASON 01/09c]' + nl + "}"
    f.rep(old, '{1358,"Huyen Nhan Van Yen"}, -- [HOASON 01/09c]' + nl + '{1982,"Vu Muc Di Thu"}, -- %s' % M + nl + '{2123,"Thien Ly Doc Hanh"}' + nl + "}", "mainskill 1982/2123")
    f.save()
    for rel, tag in ((r"skill\nangskillkieumoi5x.lua", "5x"), (r"skill\nangskillkieumoi9x.lua", "9x"), (r"skill\nangskillkieumoitp.lua", "tp")):
        f = F(rel); nl = f.nl
        old = 'elseif (player_Faction == "%s") then -- [HOASON 01/09c]' % HS_GBK + nl + "mp = 11" + nl
        f.rep(old, old + 'elseif (player_Faction == "%s") then -- %s' % (WH_GBK, M) + nl + "mp = 12" + nl + 'elseif (player_Faction == "%s") then -- %s' % (XY_GBK, M) + nl + "mp = 13" + nl, "mp 12/13")
        if tag == "5x":
            f.rep_re(r"^\{\{1349,1374\},\{1355,1379\}\} -- \[HOASON 01/09c\][^\r\n]*", lambda m: m.group(0).replace("}} --", "}}, --", 1) + nl + "{{1964,1975},{1963,1979}}, -- %s Vu Hon: ho tro 20 = 1964/1975, 5x = 1963/1979" % M + nl + "{{2115,2137},{2120,2140}} -- %s Tieu Dao: 20 = 2115/2137, 5x = 2120/2140" % M, "bang 5x")
        elif tag == "9x":
            f.rep("{{1349,1374},{1355,1379}}, -- [HOASON 01/09c]" + nl + "},", "{{1349,1374},{1355,1379}}, -- [HOASON 01/09c]" + nl + "{{1964,1975},{1963,1979}}, -- %s Vu Hon" % M + nl + "{{2115,2137},{2120,2140}}, -- %s Tieu Dao" % M + nl + "},", "bang 9x ho tro")
            f.rep("{1364,1382} -- [HOASON 01/09c]" + nl + "}", "{1364,1382}, -- [HOASON 01/09c]" + nl + "{1967,1983}, -- %s Vu Hon 90" % M + nl + "{2124,2141} -- %s Tieu Dao 90" % M + nl + "}", "bang 9x skill 90")
        else:
            f.rep("{167,168},{1347,1372}}, -- [HOASON 01/09c] Hoa Son nhap mon", "{167,168},{1347,1372},{1972,1974},{2114,2136}}, -- [HOASON 01/09c] Hoa Son nhap mon; %s Vu Hon 1972/1974, Tieu Dao 2114/2136" % M, "tp nhap mon")
            f.rep("166,275,1358}, -- [HOASON 01/09c] 1358 Huyen Nhan Van Yen", "166,275,1358,1982,2123}, -- [HOASON 01/09c] 1358 Huyen Nhan Van Yen; %s 1982 Vu Muc Di Thu, 2123 Thien Ly Doc Hanh" % M, "tp tran phai")
        f.save()
    f = F(r"item\daithanhbk90.lua"); nl = f.nl
    f.rep("\t\t[10] = {1364, 1382}, -- [HOASON 01/09c]", "\t\t[10] = {1364, 1382}, -- [HOASON 01/09c]" + nl + "\t\t[11] = {1967, 1983}, -- %s Vu Hon" % M + nl + "\t\t[12] = {2124, 2141}, -- %s Tieu Dao" % M, "[11]/[12]"); f.save()
    f = F(r"item\daithanhbk120.lua"); nl = f.nl
    f.rep("\t\t[10] = {1365}, -- [HOASON 01/09c]", "\t\t[10] = {1365}, -- [HOASON 01/09c]" + nl + "\t\t[11] = {1984}, -- %s Vu Hon" % M + nl + "\t\t[12] = {2127}, -- %s Tieu Dao" % M, "[11]/[12]"); f.save()
    f = F(r"npcthon\balanghuyen\hotrotest.lua"); nl = f.nl
    old = "\t\t\tif (series == 2) then tinsert(tbPh, 10) end"
    f.rep(old, old + nl + "\t\t\tif (series == 3) then tinsert(tbPh, 11) end\t-- %s he Hoa co them Vu Hon (11)" % M + nl + "\t\t\tif (series == 4) then tinsert(tbPh, 12) end\t-- %s he Tho co them Tieu Dao (12)" % M, "series 3/4"); f.save()

# ================================================================== 7. skills_table.lua add_wh / add_xy
def skills_table():
    f = F(r"global\skills_table.lua"); nl = f.nl
    def mk(fn, lv, s90, s120, s150):
        out = ["-- %s %s: khuon add_hs (Linux) - du lieu client VLTK" % (M, fn), "function %s(lvl)" % fn]
        for lv_, ids in lv:
            out.append("\tif (lvl >= %d) then" % lv_)
            for i in ids:
                out.append("\t\tif (HaveMagic(%d) == -1) then" % i); out.append("\t\t\tAddMagic(%d)\t\t-- %s" % (i, asc(nm(i)))); out.append("\t\tend")
            out.append("\tend")
        for lv_, ids in ((90, s90), (120, [s120]), (150, s150)):
            out.append("\tif (lvl >= %d) then" % lv_)
            for i in ids:
                out.append("\t\tif (HaveMagic(%d) == -1) then" % i); out.append("\t\t\tAddMagic(%d,20)\t\t-- %s" % (i, asc(nm(i)))); out.append("\t\tend")
            out.append("\tend")
        out.append("end")
        return nl.join(out)
    if "function add_wh(" not in f.s:
        old = nl + "function del_all_skill()"
        if f.s.count(old) != 1: raise SystemExit("skills_table: del_all_skill anchor")
        f.s = f.s.replace(old, nl + mk("add_wh", WH_LV, WH90, WH120, WH150) + nl + nl + mk("add_xy", XY_LV, XY90, XY120, XY150) + nl + old); f.n += 1; print("  [+] add_wh/add_xy")
    else: print("  [=] add_wh da co")
    f.save()

# ================================================================== 8. NPC mon phai o thon (khuon hoason.lua) + spawn
def npc_scripts():
    src = rd(os.path.join(SCR, r"npcthon\npcmonphai\hoason.lua")); nl = "\r\n" if "\r\n" in src else "\n"
    lines = src.split(nl)
    def make(fname, gbk, romaji, fac_id, series, ten_phai, ten_dv, mota, kn1, kn2, go_fn, go_label, go_world):
        out = []
        for l in lines:
            if l.startswith("--[HOASON 01/09]"):
                out.append("--%s %s o thon lang - khuon theo hoason.lua (he %s, id %d); nguon: client VLTK (Linux khong co phai nay)" % (M, romaji, {3: "Hoa", 4: "Tho"}[series], fac_id)); continue
            if l.startswith("FactionName = "):
                out.append('FactionName = "%s"' % gbk); continue
            if 'Talk(1,"enroll_select",' in l:
                ind = l[:len(l) - len(l.lstrip())]
                out.append(ind + 'Talk(1,"enroll_select","%s")' % V(mota)); continue
            if 'player_Faction == "%s"' % HS_GBK in l:
                out.append(l.replace('"%s"' % HS_GBK, '"%s"' % gbk).replace('"huashan"', '"%s"' % romaji.lower().replace(" ", ""))); continue
            if "GetSeries() == 2" in l:
                out.append(l.replace("GetSeries() == 2", "GetSeries() == %d" % series)); continue
            if V(u"Gia nh\u1eadp Hoa S\u01a1n/go") in l:
                ind = l[:len(l) - len(l.lstrip())]
                out.append(ind + 'Say("%s", 3, "%s/go", "%s/%s", "%s/nothing")' % (V(u"B\u1ee9c m\u00f4n ch\u00fang ta l\u1ea5y %s l\u00e0m g\u1ed1c, %s. Ng\u01b0\u01a1i c\u00f3 mu\u1ed1n gia nh\u1eadp %s kh\u00f4ng?".replace(u"B\u1ee9c", u"B\u1ea3n") % (kn1, kn2, ten_phai)), V(u"Gia nh\u1eadp " + ten_phai), V(go_label), go_fn, V(u"\u0110\u1ec3 ta suy ngh\u0129 k\u1ef9 l\u1ea1i xem"))); continue
            if "gianhapmonphai(10)" in l:
                out.append("\tgianhapmonphai(%d)\t-- %s nhu hoason.lua: SetFaction/Camp/Rank + hockynang (SKILLNORMAL[%d])" % (fac_id, M, fac_id + 1)); continue
            out.append(l)
        # [Linux hauquan_vuhon.lua / detu_xiaoyao.lua] lua chon 'Ve phai' (NewWorld) cho thanh vien: them vao 3 menu Say cua main() (dong co /no"))
        out2 = []
        for l in out:
            if 'Say("<npc>' in l and '/no")' in l:
                l = re.sub(r'(Say\("[^"]*",)(\d+),', lambda m: m.group(1) + str(int(m.group(2)) + 1) + ',"' + V(go_label) + '/' + go_fn + '",', l, count=1)
            out2.append(l)
        out = out2
        out.append("")
        out.append("function %s()\t-- [VHTD 02/09d] Linux: NewWorld toa do mon phai" % go_fn)
        out.append('\tMsg2Player("%s")' % V(u"Ng\u1ed3i y\u00ean, ch\u00fang ta \u0111\u1ebfn " + ten_phai))
        out.append("\tNewWorld(%d, %d, %d)" % go_world)
        out.append("end")
        s = nl.join(out)
        if hib(s) == 0: raise SystemExit("npc script rong?")
        p = os.path.join(SCR, r"npcthon\npcmonphai", fname)
        if os.path.exists(p) and rd(p) == s: print("  [=] %s da dung" % fname); return
        if not KIEM: io.open(p, "w", encoding="latin-1", newline="").write(s)
        print("  [+] ghi npcthon\\npcmonphai\\%s (%d dong)" % (fname, len(out)))
    make("vuhon.lua", WH_GBK, "Wuhuntang", 11, 3, u"V\u0169 H\u1ed3n \u0110\u01b0\u1eddng", u"H\u1ead u Qu\u00e2n",
         u"V\u0169 H\u1ed3n \u0110\u01b0\u1eddng do Nh\u1ea1c Phi l\u1ead p n\u00ean, quy t\u1ee5 tinh binh Nh\u1ea1c gia qu\u00e2n, l\u1ea5y \u0111ao thu\u1eabn l\u00e0m v\u0169 kh\u00ed, l\u1ea5y ch\u1eef trung ngh\u0129a l\u00e0m g\u1ed1c. Ng\u01b0\u01a1i c\u00f3 mu\u1ed1n c\u00f9ng ta b\u1ea3o v\u1ec7 bi\u00ean c\u01b0\u01a1ng kh\u00f4ng?".replace(u"l\u1ead p", u"l\u1eadp"),
         u"V\u0169 H\u1ed3n Thu\u1eabn Ph\u00e1p", u"V\u0169 H\u1ed3n \u0110ao Ph\u00e1p c\u00f4ng th\u1ee7 to\u00e0n di\u1ec7n", "go_vuhon", u"\u0110\u1ebfn V\u0169 H\u1ed3n \u0110\u01b0\u1eddng", (1042, 1625, 3130))
    make("tieudao.lua", XY_GBK, "Xiaoyao", 12, 4, u"Ti\u00eau Dao ph\u00e1i", u"Ti\u00eau Dao C\u01b0 S\u0129",
         u"Ti\u00eau Dao ph\u00e1i ta \u1ea9n c\u01b0 gi\u1ee5a n\u00fai r\u1eebng, l\u1ea5y ki\u1ebfm v\u00e0 c\u1ea7m l\u00e0m b\u1ea1n, ti\u00eau dao t\u1ef1 t\u1ea1i ngo\u00e0i v\u00f2ng danh l\u1ee3i. Ng\u01b0\u01a1i c\u00f3 mu\u1ed1n c\u00f9ng ta ng\u1eafm c\u1ea3nh th\u01b0\u1edfng nh\u1ea1c kh\u00f4ng?".replace(u"gi\u1ee5a", u"gi\u1eefa"),
         u"Ti\u00eau Dao Ki\u1ebfm Ph\u00e1p", u"Ti\u00eau Dao C\u1ea7m Ph\u00e1p \u00e2m lu\u1ead t s\u00e1t \u0111\u1ecbch".replace(u"lu\u1ead t", u"lu\u1eadt"), "go_tieudao", u"\u0110\u1ebfn Ti\u00eau Dao ph\u00e1i", (XY_MAP, 1641, 3288))

# spawn theo LINUX (D:\ServerLinux\server1\script\wumumenpai\npc_wumumenpai.lua:110-167, xiaoyao\npc\npc_xiaoyao.lua:6-23) - lenh chu 02/09.
# id NPC Linux == id JX1 (npcs.txt VLTK, id = dong-2) cho ca khoi 2467..2501 va 2607..2614 (kiem ten khi chay); Linux 2476/2477 (Cat Khang/Thu Nhue)
# o JX1 la Nam/Nu De Tu Vu Hon -> BO. Linux 1046 (xiaoyao) = cung .wor voi map JX1 tro map_publish\xiaoyao (md5 khop) -> toa do dung nguyen.
NPC_WH, NPC_XY = 2467, 2607          # npcs.txt may chu (id = dong-2): 2467 Hau Quan Vu Hon (kind 3), 2607 Tieu Dao Cu Si (kind 3) - ten VLTK
def _map_xiaoyao():
    ml = rd(os.path.join(SRV, r"settings\MapList.ini"))
    m = re.search(r"(?m)^(\d+)=map_publish" + re.escape(BS) + r"xiaoyao\s*$", ml)
    if not m: raise SystemExit("MapList.ini may chu khong co map map_publish\\xiaoyao")
    return int(m.group(1))
XY_MAP = _map_xiaoyao()
SC = lambda name: BS + BS + "script" + BS + BS + name.replace("/", BS + BS)
SC_WH, SC_XY = SC("npcthon/npcmonphai/vuhon.lua"), SC("npcthon/npcmonphai/tieudao.lua")
SC_CHAO, SC_TP_WH, SC_TP_XY = SC("global/vhtd/npc_chao.lua"), SC("global/vhtd/thuyenphu_vuhon.lua"), SC("global/vhtd/thuyenphu_tieudao.lua")
SC_RUONG = SC("global/npcchucnang/ruongchua.lua")
TOWNS_WH_LINUX = [(53,1624,3197),(20,3556,6187),(99,1654,3196),(100,1616,3172),(101,1690,3114),(121,1962,4511)]
TOWNS_XY_LINUX = [(53,1609,3215),(20,3575,6222),(99,1691,3193),(100,1641,3209),(101,1703,3125),(121,1966,4480),(153,1650,3201),(174,1579,3267)]
# (id, map, x, y, script, ten) - map 1042 Vu Hon theo Linux npc_wumumenpai.lua (chuong mon Nhac Loi dung menu phai; NPC khac: chao; Thuyen Phu: ve thanh)
MAP_WH = [(2468,1042,1693,3046,SC_WH,u"Nh\u1ea1c L\u00f4i"), (2469,1042,1683,3048,SC_CHAO,u"Ng\u01b0u Th\u00f4ng"), (2470,1042,1693,3059,SC_CHAO,u"H\u00e0n Th\u01b0\u1ee3ng \u0110\u1ee9c"),
          (2471,1042,1749,3254,SC_CHAO,u"Thi Phong"), (2472,1042,1775,3197,SC_CHAO,u"Thang Anh"), (2473,1042,1601,3020,SC_CHAO,u"H\u00e0n Kh\u1edfi Ph\u01b0\u1ee3ng"),
          (2474,1042,1591,2999,SC_CHAO,u"Nh\u1ea1c \u0110\u1ecbnh"), (2475,1042,1625,3130,SC_CHAO,u"Ng\u0169 Li\u00ean"), (2480,1042,1787,3205,SC_CHAO,u"Y S\u01b0"),
          (2481,1042,1806,3237,SC_CHAO,u"D\u01b0 \u0110\u1ea1i Ch\u00f9y"), (2482,1042,1673,3304,SC_CHAO,u"Nh\u1ea1c Nh\u1ecb N\u01b0\u01a1ng"), (2483,1042,1649,3023,SC_CHAO,u"Nh\u1ea1c Phi Y Quan Gia"),
          (2490,1042,1552,3067,SC_CHAO,u"Luy\u1ec7n V\u00f5 Tr\u00e0ng Gi\u00e1o Quan"), (2494,1042,1574,3210,SC_CHAO,u"Manh M\u1ed1i"),
          (1846,1042,1773,3268,SC_TP_WH,u"Thuy\u1ec1n Phu"), (1846,1042,1688,3314,SC_TP_WH,u"Thuy\u1ec1n Phu"), (625,1042,1558,3195,SC_RUONG,u"R\u01b0\u01a1ng Ch\u1ee9a \u0110\u1ed3")]
# map Tieu Dao theo Linux npc_xiaoyao.lua (Linux 1046 -> XY_MAP): Van Ban Son = chuong mon (menu phai), Ly Sau Ngoc/... chao
MAP_XY = [(2608,None,1629,3216,SC_XY,u"V\u0103n B\u00e1n S\u01a1n"), (2611,None,1750,3225,SC_CHAO,u"Ch\u01b0\u1edfng M\u00f4n L\u00fd S\u1ea5u Ng\u1ecdc"), (2609,None,1645,3281,SC_CHAO,u"H\u01b0\u1edbng Nam Chi"),
          (2610,None,1666,3112,SC_CHAO,u"Li\u1ec5u Tam Bi\u1ebfn"), (2612,None,1578,3251,SC_CHAO,u"B\u00f9i Linh Lung"), (2613,None,1697,3151,SC_CHAO,u"C\u1ea7m \u0110\u1ed3ng"),
          (2614,None,1696,3168,SC_CHAO,u"Ki\u1ebfm \u0110\u1ed3ng"), (1846,None,1726,3426,SC_TP_XY,u"Thuy\u1ec1n Phu"), (625,None,1674,3323,SC_RUONG,u"R\u01b0\u01a1ng Ch\u1ee9a \u0110\u1ed3")]

def _ghi(rel, body):
    p = os.path.join(SCR, rel)
    if os.path.exists(p) and rd(p) == body: print("  [=] %s da dung" % rel); return
    if not KIEM:
        os.makedirs(os.path.dirname(p), exist_ok=True); io.open(p, "w", encoding="latin-1", newline="").write(body)
    print("  [+] ghi %s" % rel)

def npc_spawn():
    npcs = rd(os.path.join(SRV, r"settings\npcs.txt")).split("\r\n")
    def ten(n):
        if n + 1 >= len(npcs) or not npcs[n + 1].strip(): return None
        return npcs[n + 1].split("\t")[0]
    for n, t in ((NPC_WH, "Hau Quan Vu Hon"), (NPC_XY, "Tieu Dao Cu Si")):
        if ten(n) is None:
            msg = "npcs.txt may chu CHUA co dong NPC id %d (%s)" % (n, t)
            if "--bo-kiem-npc" in sys.argv: print("  [!] " + msg)
            else: raise SystemExit(msg)
        else: print("  NPC mau %d: %s - mong doi %s" % (n, asc(ten(n)), t))
    nl = "\r\n"
    rows = []
    for (mp, x, y) in TOWNS_WH_LINUX: rows.append((NPC_WH, mp, x, y, SC_WH, V(u"V\u0169 H\u1ed3n \u0110\u01b0\u1eddng H\u1eadu Qu\u00e2n")))
    for (mp, x, y) in TOWNS_XY_LINUX: rows.append((NPC_XY, mp, x, y, SC_XY, V(u"Ti\u00eau Dao M\u1eadt S\u1ee9")))
    for (i, mp, x, y, sc, name) in MAP_WH + MAP_XY:
        t = ten(i)
        print("  NPC %d '%s' (npcs.txt: %s)" % (i, asc(V(name)), asc(t) if t else "!!! KHONG CO"))
        if t is None: raise SystemExit("npcs.txt thieu id %d" % i)
        rows.append((i, mp if mp else XY_MAP, x, y, sc, V(name)))
    body = nl.join([
        "-- [VHTD 02/09d] NPC Vu Hon (11) / Tieu Dao (12) - TOA DO LINUX: script\\wumumenpai\\npc_wumumenpai.lua (6 thon + map 1042),",
        "-- script\\xiaoyao\\npc\\npc_xiaoyao.lua (8 thon + map xiaoyao; Linux 1046 = map JX1 %d, cung .wor). id NPC Linux = id JX1 (VLTK)." % XY_MAP,
        "-- Bo: Linux 2476 Cat Khang / 2477 Thu Nhue (JX1 la Nam/Nu De Tu Vu Hon), De Tu Kha Nghi 2501, Thi The 2495 (NPC nhiem vu Linux).",
        "vhtd_parserby = {"] + ['{%d,%d,%d,%d,"%s","%s"},' % r for r in rows] + ["}", "",
        "function add_npc_vhtd()", "\tvhtd_bynpc(vhtd_parserby)", "end", "",
        "function vhtd_bynpc(Tab)", "\tfor i = 1 , getn(Tab) do", "\t\tlocal SId = SubWorldID2Idx(Tab[i][2]);", "\t\tif (SId >= 0) then",
        "\t\t\tlocal npcindex = AddNpc(Tab[i][1],1,SId,Tab[i][3]*32,Tab[i][4]*32,1,Tab[i][6]);", "\t\t\tif (npcindex > 0) then",
        "\t\t\t\tSetNpcScript(npcindex, Tab[i][5]);", "\t\t\t\tSetNpcValue(npcindex, 10);", "\t\t\tend", "\t\tend;", "\tend", "end;", ""])
    _ghi(r"global\vhtd\npc_vhtd.lua", body)
    print("  npc_vhtd.lua: %d NPC (6 thon VH + 8 thon TD + %d map 1042 + %d map %d)" % (len(rows), len(MAP_WH), len(MAP_XY), XY_MAP))
    # script chao (NPC trang tri Linux la NPC nhiem vu wuhun2020 - JX1 khong co nhiem vu -> chao)
    _ghi(r"global\vhtd\npc_chao.lua", nl.join([
        "-- [VHTD 02/09d] NPC mon phai Vu Hon/Tieu Dao khong co nhiem vu o JX1 (Linux: script\\wumumenpai\\*.lua nhiem vu wuhun2020) - chi chao.",
        "function main()", '\tTalk(1, "", "%s")' % V(u"Hoan ngh\u00eanh ng\u01b0\u01a1i \u0111\u1ebfn th\u0103m b\u1ed5n m\u00f4n. Ch\u01b0\u1edfng m\u00f4n \u0111ang \u0111\u1ee3i ng\u01b0\u01a1i \u1edf \u0111\u1ea1i \u0111i\u1ec7n."), "end", ""]))
    # Thuyen Phu: port Linux wumumenpai\thuyenphu.lua (Ba Lang Huyen 53 / Lam An 176) va xiaoyao\npc\thuyenphu.lua (Tay Son Thon 175 / Lam An 176)
    def tp(rel, o1, w1):
        _ghi(rel, nl.join([
            "-- [VHTD 02/09d] port Linux %s (Describe -> Say, cung API JX1: NewWorld, SetFightState)" % rel.split(BS)[-1],
            "function main()",
            '\tSay("%s", 3, "%s/di1", "%s/di2", "%s/no")' % (V(u"Thuy\u1ec1n Phu: Ng\u01b0\u01a1i mu\u1ed1n \u0111i \u0111\u00e2u?"), V(o1), V(u"\u0110\u1ebfn L\u00e2m An"), V(u"Ta kh\u00f4ng \u0111i \u0111\u00e2u c\u1ea3")),
            "end", "", "function di1()", "\tNewWorld(%d, %d, %d)" % w1, "\tSetFightState(1)", "end", "",
            "function di2()", "\tNewWorld(176, 1607, 2553)", "\tSetFightState(1)", "end", "", "function no()", "end", ""]))
    tp(r"global\vhtd\thuyenphu_vuhon.lua", u"\u0110\u1ebfn Ba L\u0103ng Huy\u1ec7n", (53, 1794, 3157))
    tp(r"global\vhtd\thuyenphu_tieudao.lua", u"\u0110\u1ebfn T\u00e2y S\u01a1n Th\u00f4n", (175, 1712, 3125))
    f = F("startgame.lua"); nl = f.nl
    old = 'Include("' + BS + BS + "script" + BS + BS + "global" + BS + BS + "huashan2013" + BS + BS + 'npc_hoason.lua");\t-- [HOASON 01/09] NPC/quai Hoa Son 2013 (Linux)'
    f.rep(old, old + nl + 'Include("' + BS + BS + "script" + BS + BS + "global" + BS + BS + "vhtd" + BS + BS + 'npc_vhtd.lua");\t-- %s NPC nhap mon Vu Hon / Tieu Dao' % M, "Include npc_vhtd")
    old = "\tadd_npc_hoason()\t-- [HOASON 01/09] Linux npc_hoason.lua: quai + NPC map 987 + Hoa Son Kiem Khach 8 thon"
    f.rep(old, old + nl + "\tadd_npc_vhtd()\t-- %s NPC nhap mon Vu Hon / Tieu Dao 8 thon" % M, "goi add_npc_vhtd")
    f.save()

if __name__ == "__main__":
    if "--chi-npc" in sys.argv:
        print("== npc mon phai (chi NPC)"); npc_scripts()
        print("== spawn"); npc_spawn()
        print("XONG%s." % (" (KIEM)" if KIEM else "")); raise SystemExit
    print("== factionhead"); factionhead()
    print("== hocvocong"); hocvocong()
    print("== lib_faction"); lib_faction()
    print("== skillbook / lvl120"); skillbook()
    print("== lenhbaitanthu"); lenhbai()
    print("== skillfaction / nang skill / dai thanh / hotrotest"); skill_misc()
    print("== skills_table"); skills_table()
    print("== npc mon phai"); npc_scripts()
    print("== spawn + startgame"); npc_spawn()
    print("XONG%s." % (" (KIEM)" if KIEM else ""))
