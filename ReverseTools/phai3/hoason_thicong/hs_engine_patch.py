# -*- coding: utf-8 -*-
"""hs_engine_patch.py [HOASON 01/09]: engine JX1 len 13 mon phai (them Hoa Son id 10, cho san 11 Vu Hon, 12 Tieu Dao).
Sua bang latin-1, giu nguyen byte TCVN3/GBK, neo phai DUY NHAT, idempotent theo marker [HOASON 01/09].
Neo viet bang LF; tep CRLF duoc doi tu dong.
Chay:  python hs_engine_patch.py            (ap dung)
       python hs_engine_patch.py --kiem     (chi kiem neo, khong ghi)
"""
import io, os, re, sys
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
KIEM = "--kiem" in sys.argv
SRC = r"D:\GAMEDEVNEW\Sources"
MARK = "[HOASON 01/09]"
LF = chr(10); CR = chr(13); CRLF = CR + LF

def V(s):  # unicode -> chuoi latin-1 mang byte TCVN3
    return unicode_to_tcvn3_bytes(s).decode("latin-1")
def G(s):  # unicode -> chuoi latin-1 mang byte GBK
    return s.encode("gbk").decode("latin-1")

class Tep:
    def __init__(self, rel):
        self.p = os.path.join(SRC, rel)
        self.d = io.open(self.p, "r", encoding="latin-1", newline="").read()
        self.nl = CRLF if CRLF in self.d else LF
        self.n = 0
        self.rel = rel
    def nx(self, s):
        if self.nl == CRLF:
            s = s.replace(CRLF, LF).replace(LF, CRLF)
        return s
    def da(self, marker):
        return marker in self.d
    def co(self, s):
        return self.nx(s) in self.d
    def thay(self, old, new, so=1):
        old = self.nx(old); new = self.nx(new)
        c = self.d.count(old)
        assert c == so, "%s: neo xuat hien %d lan (can %d): %r" % (self.rel, c, so, old[:90])
        self.d = self.d.replace(old, new); self.n += 1
    def ghi(self):
        if KIEM:
            print("  (kiem) %s: %d cho" % (self.rel, self.n)); return
        io.open(self.p, "w", encoding="latin-1", newline="").write(self.d)
        print("  ghi %s: %d cho" % (self.rel, self.n))

# ---------------------------------------------------------------- GameDataDef.h
t = Tep(r"Core\Src\GameDataDef.h")
if not t.da(MARK):
    t.thay("\tseries_nil,\n\tseries_num = series_nil,",
           "\tseries_nil,\n\tseries_num = series_nil,\n"
           "\t// %s so mon phai TOI DA (13 = 10 goc + Hoa Son 10 + Vu Hon 11 + Tieu Dao 12);\n"
           "\t// truoc day MAX_FACTION = 2 phai x 5 he (KFaction.h). Dung o KProtocol.h TGAME_STAT_DATA,\n"
           "\t// KLadder.h, KNpc.cpp - CORE va GODDESS (DBBackup.h) phai cung con so nay.\n"
           "\tMAX_FACTION_NUM = 13,\n" % MARK)
    t.ghi()

# ---------------------------------------------------------------- KFaction.h
t = Tep(r"Core\Src\KFaction.h")
if not t.da(MARK):
    t.thay("#define\t\tMAX_FACTION\t\t\t\t\t\t(FACTIONS_PRR_SERIES * series_num)\t",
           "#define\t\tMAX_FACTION\t\t\t\t\t\t(MAX_FACTION_NUM)\t// %s 13 phai; FACTIONS_PRR_SERIES chi con la 'so phai goc moi he' cho code cu (bot)" % MARK)
    t.thay("\tint\t\t\t\tGetCamp(int nFactionID);\t\t\t// ",
           "\tint\t\t\t\tGetIDByValueName(const char* lpszValueName);\t// %s huashan/wuhun/xiaoyao -> id\n"
           "\tint\t\t\t\tGetCamp(int nFactionID);\t\t\t// " % MARK)
    t.ghi()

# ---------------------------------------------------------------- KFaction.cpp
t = Tep(r"Core\Src\KFaction.cpp")
if not t.da(MARK):
    old = ("\t\tfor (j = 0; j < series_num; j++)\n\t\t{\n\t\t\tif (strcmp(szBuffer, szSeries[j]) != 0)\n\t\t\t\tcontinue;\n"
           "\t\t\tfor (k = 0; k < FACTIONS_PRR_SERIES; k++)\n\t\t\t{\n"
           "\t\t\t\tif (m_sAttribute[j * FACTIONS_PRR_SERIES + k].m_szName[0] == 0)\n\t\t\t\t{\n"
           "\t\t\t\t\tnArrayPos = j * FACTIONS_PRR_SERIES + k;\n\t\t\t\t\tm_sAttribute[nArrayPos].m_nSeries = j;\n"
           "\t\t\t\t\tbreak;\n\t\t\t\t}\n\t\t\t}\n\t\t\tbreak;\n\t\t}\n\t\t_ASSERT(j < series_num);\n")
    new = ("\t\t// %s id phai = SO MUC [i] trong FactionInfo.ini (khong con 'o trong dau tien cua he'\n"
           "\t\t// nua - cong thuc he*2+k sup do khi 13 phai chia 5 he khong deu). 10 muc goc giu nguyen id 0..9.\n"
           "\t\tnArrayPos = i;\n"
           "\t\tfor (j = 0; j < series_num; j++)\n\t\t{\n\t\t\tif (strcmp(szBuffer, szSeries[j]) == 0)\n\t\t\t{\n"
           "\t\t\t\tm_sAttribute[nArrayPos].m_nSeries = j;\n\t\t\t\tbreak;\n\t\t\t}\n\t\t}\n"
           "\t\tif (j >= series_num)\n\t\t\tcontinue;\t\t\t\t\t\t// muc khong co / he la -> bo qua, ten de trong\n") % MARK
    t.thay(old, new)
    old = ("int\t\tKFaction::GetID(int nSeries, int nNo)\n{\n"
           "\tif (nSeries < series_metal || nSeries >= series_num || nNo < 0 || nNo >= FACTIONS_PRR_SERIES)\n\t\treturn -1;\n"
           "\treturn nSeries * FACTIONS_PRR_SERIES + nNo;\n}\n")
    new = ("int\t\tKFaction::GetID(int nSeries, int nNo)\n{\n"
           "\t// %s phai thu nNo (0..) trong so cac phai cung he, duyet bang thay vi he*2+nNo\n"
           "\tif (nSeries < series_metal || nSeries >= series_num || nNo < 0)\n\t\treturn -1;\n"
           "\tint nDem = 0;\n\tfor (int i = 0; i < MAX_FACTION; i++)\n\t{\n"
           "\t\tif (m_sAttribute[i].m_szName[0] == 0 || m_sAttribute[i].m_nSeries != nSeries)\n\t\t\tcontinue;\n"
           "\t\tif (nDem == nNo)\n\t\t\treturn i;\n\t\tnDem++;\n\t}\n\treturn -1;\n}\n\n"
           "int\t\tKFaction::GetIDByValueName(const char* lpszValueName)\n{\n"
           "\tif (!lpszValueName || !lpszValueName[0])\n\t\treturn -1;\n"
           "\tfor (int i = 0; i < MAX_FACTION; i++)\n\t{\n"
           "\t\tif (m_sAttribute[i].m_szName[0] && strcmp(lpszValueName, m_sAttribute[i].m_szValueName) == 0)\n\t\t\treturn i;\n"
           "\t}\n\treturn -1;\n}\n") % MARK
    t.thay(old, new)
    old = ("\tfor (int i = nSeries * FACTIONS_PRR_SERIES; i < (nSeries + 1) * FACTIONS_PRR_SERIES; i++)\n\t{\n"
           "\t\tif (strcmp(lpszName, m_sAttribute[i].m_szName) == 0)\n\t\t\treturn i;\n\t}\n\treturn -1;\n")
    new = ("\t// %s duyet toan bang, chi lay phai cung he; nhan CA ten GBK (Name=) LAN ValueName (huashan...)\n"
           "\tfor (int i = 0; i < MAX_FACTION; i++)\n\t{\n"
           "\t\tif (m_sAttribute[i].m_szName[0] == 0 || m_sAttribute[i].m_nSeries != nSeries)\n\t\t\tcontinue;\n"
           "\t\tif (strcmp(lpszName, m_sAttribute[i].m_szName) == 0 || strcmp(lpszName, m_sAttribute[i].m_szValueName) == 0)\n\t\t\treturn i;\n"
           "\t}\n\treturn -1;\n") % MARK
    t.thay(old, new)
    t.ghi()

# ---------------------------------------------------------------- KPlayerFaction.cpp
t = Tep(r"Core\Src\KPlayerFaction.cpp")
if not t.da(MARK):
    old = ("\tif (nFactionID < nSeries * FACTIONS_PRR_SERIES || nFactionID >= (nSeries + 1) * FACTIONS_PRR_SERIES)\n\t\treturn FALSE;\n")
    new = ("\t// %s cua vao duy nhat cua AddFaction: kiem theo bang (13 phai) thay vi khoang [he*2, he*2+2)\n"
           "\tif (nFactionID < 0 || nFactionID >= MAX_FACTION)\n\t\treturn FALSE;\n"
           "\tif (g_Faction.m_sAttribute[nFactionID].m_szName[0] == 0 || g_Faction.m_sAttribute[nFactionID].m_nSeries != nSeries)\n\t\treturn FALSE;\n") % MARK
    t.thay(old, new)
    t.ghi()

# ---------------------------------------------------------------- KLadder.h / .cpp
t = Tep(r"Core\Src\KLadder.h")
if not t.da(MARK):
    t.thay("#define\tMAX_FAC series_num * FACTIONS_PRR_SERIES + 1",
           "#define\tMAX_FAC (MAX_FACTION_NUM + 1)\t// %s 13 phai + 1 (o [0] = chua vao phai)" % MARK)
    t.ghi()
t = Tep(r"Core\Src\KLadder.cpp")
if not t.da(MARK):
    t.thay("nFac >= series_num * FACTIONS_PRR_SERIES", "nFac >= MAX_FACTION_NUM", 5)
    t.thay("#include \"KLadder.h\"\n", "#include \"KLadder.h\"\n// %s bien nFac 0..12 (MAX_FACTION_NUM), mang TGAME_STAT_DATA [MAX_FACTION_NUM+1]\n" % MARK)
    t.ghi()

# ---------------------------------------------------------------- KProtocol.h (TGAME_STAT_DATA)
t = Tep(r"Core\Src\KProtocol.h")
if not t.da(MARK):
    t.thay("\tTRoleList MoneyStatBySect[11][10];\t//",
           "\t// %s [11] -> [MAX_FACTION_NUM + 1] (14): 13 phai + o [0] chua vao phai. Goddess DBBackup.h TStatData PHAI cung co (sizeof kiem o KLadder::Init).\n\tTRoleList MoneyStatBySect[MAX_FACTION_NUM + 1][10];\t//" % MARK)
    t.thay("\tTRoleList LevelStatBySect[11][10];\t//", "\tTRoleList LevelStatBySect[MAX_FACTION_NUM + 1][10];\t//")
    t.thay("\tint SectPlayerNum[11];\t\t\t\t//", "\tint SectPlayerNum[MAX_FACTION_NUM + 1];\t\t\t\t//")
    t.thay("\tint SectMoneyMost[11];\t\t\t\t//", "\tint SectMoneyMost[MAX_FACTION_NUM + 1];\t\t\t\t//")
    t.thay("\tint SectLevelMost[11];\t\t\t\t//", "\tint SectLevelMost[MAX_FACTION_NUM + 1];\t\t\t\t//")
    t.ghi()

# ---------------------------------------------------------------- KNpc.cpp (ten phai tren dau)
t = Tep(r"Core\Src\KNpc.cpp")
if not t.da(MARK):
    # bang cu bi hong ma hoa (mot so ten la TCVN3 bi UTF-8 hoa boi tool sua) -> viet lai ca bang bang TCVN3 chuan, 13 ten
    m = re.search(r"\tstatic const char\* const FactionName\[\] = \{.*?\r?\n\t\};", t.d, re.S)
    assert m, "KNpc: khong thay bang FactionName"
    ten = ["Thiếu Lâm", "Thiên Vương Bang", "Đường Môn", "Ngũ Độc", "Nga My", "Thúy Yên", "Cái Bang", "Thiên Nhẫn", "Võ Đang", "Côn Lôn", "Hoa Sơn", "Vũ Hồn", "Tiêu Dao"]
    new = ("\tstatic const char* const FactionName[MAX_FACTION_NUM] = {\t// %s 13 phai, TCVN3 (bang cu 10 ten bi hong ma hoa)\n" % MARK
           + "".join("\t\t\"%s\"%s\t// %d\n" % (V(x), "," if i < 12 else "", i) for i, x in enumerate(ten)) + "\t};")
    t.d = t.d[:m.start()] + t.nx(new) + t.d[m.end():]; t.n += 1
    t.thay("\t\tnFaction = (nFirstFaction <= 9) ? (int)nFirstFaction : -1;",
           "\t\tnFaction = (nFirstFaction < MAX_FACTION_NUM) ? (int)nFirstFaction : -1;\t// %s" % MARK)
    t.thay("\t\tif (nFaction >= 0 && nFaction <= 9)\n\t\t{\n\t\t\tszFactionName = FactionName[nFaction];",
           "\t\tif (nFaction >= 0 && nFaction < MAX_FACTION_NUM)\n\t\t{\n\t\t\tszFactionName = FactionName[nFaction];")
    t.ghi()

# ---------------------------------------------------------------- ScriptFuns.cpp
t = Tep(r"Core\Src\ScriptFuns.cpp")
if not t.da(MARK):
    t.thay("\tstatic char* s_szFaction[11] = {\n", "\tstatic char* s_szFaction[MAX_FACTION_NUM] = {\t// %s 13 phai\n" % MARK)
    old = "\t\t(char*)\"C«n L«n\",\n\t\t(char*)\"Hoa S¬n\"\n\t};"
    new = ("\t\t(char*)\"C«n L«n\",\n\t\t(char*)\"%s\",\n\t\t(char*)\"%s\",\n\t\t(char*)\"%s\"\n\t};"
           % (V("Hoa Sơn"), V("Vũ Hồn"), V("Tiêu Dao")))
    t.thay(old, new)
    t.thay("\tif (nNo >= 0 && nNo < 11)\n\t\tLua_PushString(L, s_szFaction[nNo]);",
           "\tif (nNo >= 0 && nNo < MAX_FACTION_NUM)\n\t\tLua_PushString(L, s_szFaction[nNo]);")
    old = "// GetLastFactionNumber() - JX2: so hieu mon phai 0..9 (Thieu Lam=0 ... Con\n"
    assert t.co(old), "ScriptFuns: khong thay comment GetLastFactionNumber"
    new = ("// %s SetLastFactionNumber(n) - Linux: ghi 'last' cua faction record. JX1 chi co\n"
           "// cur/first/addTimes va GetLastFactionNumber() = cur, nen o day = dat cur (va first neu\n"
           "// chua co) roi dong bo client + game title. Script Hoa Son goi SAU SetFaction(\"huashan\")\n"
           "// (idempotent). Tra 1 khi hop le.\n"
           "int LuaSetLastFactionNumber(Lua_State* L)\n{\n"
           "\tint nPlayerIndex = GetPlayerIndex(L);\n"
           "\tif (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1 || !Lua_IsNumber(L, 1))\n\t{\n\t\tLua_PushNumber(L, 0);\n\t\treturn 1;\n\t}\n"
           "\tint nNo = (int)Lua_ValueToNumber(L, 1);\n"
           "\tif (nNo < 0 || nNo >= MAX_FACTION_NUM)\n\t{\n\t\tLua_PushNumber(L, 0);\n\t\treturn 1;\n\t}\n"
           "\tKPlayerFaction& cF = Player[nPlayerIndex].m_cFaction;\n"
           "\tcF.m_nCurFaction = nNo;\n"
           "\tif (cF.m_nFirstAddFaction < 0)\n\t\tcF.m_nFirstAddFaction = nNo;\n"
           "#ifdef _SERVER\n"
           "\tif (Player[nPlayerIndex].m_nIndex > 0)\n\t\tNpc[Player[nPlayerIndex].m_nIndex].UpdateGameTitle();\n"
           "\tPlayer[nPlayerIndex].SendFactionData();\n"
           "#endif\n"
           "\tLua_PushNumber(L, 1);\n\treturn 1;\n}\n\n"
           "// %s GetFactionNumber() - Linux: so hieu phai hien tai (= GetFactionNo)\n"
           "int LuaGetFactionNumber(Lua_State* L)\n{\n"
           "\tint nPlayerIndex = GetPlayerIndex(L);\n"
           "\tif (nPlayerIndex > 0)\n\t\tLua_PushNumber(L, Player[nPlayerIndex].GetFactionNo());\n"
           "\telse\n\t\tLua_PushNumber(L, -1);\n\treturn 1;\n}\n\n" % (MARK, MARK)) + old
    t.thay(old, new)
    old = "\t{\"GetLastFactionNumber\",LuaGetLastFactionNumber}, \n"
    if not t.co(old):
        old = "\t{\"GetLastFactionNumber\",LuaGetLastFactionNumber},\n"
    new = old + ("\t{\"SetLastFactionNumber\",LuaSetLastFactionNumber},\t// %s\n"
                 "\t{\"GetFactionNumber\",LuaGetFactionNumber},\n"
                 "\t{\"ClearFactionRecord\",LuaClearFactionIfnfo},\t// Linux ClearFactionRecord = JX1 ClearFactionIfnfo\n" % MARK)
    t.thay(old, new)
    t.ghi()

# ---------------------------------------------------------------- UiTongJX2.cpp (client)
t = Tep(r"S3Client\Ui\UiCase\UiTongJX2.cpp")
if not t.da(MARK):
    old = ("static const char* s_szFaction[11] =\n{\n\t\"-\", \"Thi\xd5u L\xa9m\", \"Thi\xaan V\xad\xacng\", \"\xa7\xad\xeang M\xabn\",\n"
           "\t\"Ng\xf2 \xa7\xe9c\", \"Nga My\", \"Th\xf3y Y\xaan\", \"C\xb8i Bang\",\n\t\"Thi\xaan Nh\xc9n\", \"V\xe2 \xa7ang\", \"C\xabn L\xabn\",\n};")
    assert t.co(old), "UiTongJX2: khong khop bang ten"
    new = ("static const char* s_szFaction[MAX_FACTION_NUM + 1] =\t// %s 13 phai (id 1..13)\n{\n\t\"-\", \"Thi\xd5u L\xa9m\", \"Thi\xaan V\xad\xacng\", \"\xa7\xad\xeang M\xabn\",\n"
           "\t\"Ng\xf2 \xa7\xe9c\", \"Nga My\", \"Th\xf3y Y\xaan\", \"C\xb8i Bang\",\n\t\"Thi\xaan Nh\xc9n\", \"V\xe2 \xa7ang\", \"C\xabn L\xabn\",\n"
           "\t\"%s\", \"%s\", \"%s\",\n};" % (MARK, V("Hoa Sơn"), V("Vũ Hồn"), V("Tiêu Dao")))
    t.thay(old, new)
    t.ghi()

# ---------------------------------------------------------------- KPlayerBot.cpp
t = Tep(r"Core\Src\KPlayerBot.cpp")
if not t.da(MARK):
    old = ("\t{ 4, \"\\\\script\\\\npcthon\\\\npcmonphai\\\\conlon.lua\",     \"Con Lon\"     },  // 9\n};")
    new = ("\t{ 4, \"\\\\script\\\\npcthon\\\\npcmonphai\\\\conlon.lua\",     \"Con Lon\"     },  // 9\n"
           "\t{ 2, \"\\\\script\\\\npcthon\\\\npcmonphai\\\\hoason.lua\",     \"Hoa Son\"     },  // 10 %s\n"
           "\t{ 3, \"\\\\script\\\\npcthon\\\\npcmonphai\\\\vuhon.lua\",      \"Vu Hon\"      },  // 11 (chua co script)\n"
           "\t{ 4, \"\\\\script\\\\npcthon\\\\npcmonphai\\\\tieudao.lua\",    \"Tieu Dao\"    },  // 12 (chua co script)\n};" % MARK)
    t.thay(old, new)
    t.thay("\tcase 4: case 8: pPool = s_wKiem; nPool = 2; break;",
           "\tcase 4: case 8: case 10: pPool = s_wKiem; nPool = 2; break;\t// %s Hoa Son: kiem hoac tay khong (khi tong)" % MARK)
    t.ghi()

# ---------------------------------------------------------------- UiTeamManager2.cpp (client icon phai)
t = Tep(r"S3Client\Ui\UiCase\UiTeamManager2.cpp")
if not t.da(MARK):
    m = re.search(r'\t\tcase 10:\r?\n\t\t\tbtn\.SetImage\(ISI_T_SPR, "(\\\\spr\\\\Ui4\\\\[^"]*?)icon_zd_hsp\.spr",true\); \r?\n\t\t\tbreak;\r?\n', t.d)
    assert m, "UiTeamManager2: khong thay case 10"
    dirp = m.group(1)
    add = ("\t\tcase 11:\t// %s Vu Hon (icon co san updatejx15.pak)\n\t\t\tbtn.SetImage(ISI_T_SPR, \"%sicon_zd_wht.spr\",true); \n\t\t\tbreak;\n"
           "\t\tcase 12:\t// Tieu Dao (icon co san updatejx16.pak)\n\t\t\tbtn.SetImage(ISI_T_SPR, \"%sicon_zd_xy.spr\",true); \n\t\t\tbreak;\n" % (MARK, dirp, dirp))
    t.d = t.d.replace(m.group(0), m.group(0) + t.nx(add), 1); t.n += 1
    t.ghi()

# ---------------------------------------------------------------- UiSkillsNew.cpp (client bang ky nang)
t = Tep(r"S3Client\Ui\UiCase\UiSkillsNew.cpp")
if not t.da(MARK):
    old = "\t{ 273, 21 }  // Nhu Lai Thien Diep - Tran phai\n"
    new = ("\t{ 273, 21 },  // Nhu Lai Thien Diep - Tran phai\n"
           "\t//{ 210,   22 }, // Khinh cong\n\n"
           "\t// Faction 10 Hoa Son %s - o theo [Skill_10_i] trong UiSkillFlySub.ini\n"
           "\t{ 1347, 0 },   // Bach Hong Quan Nhat (kiem tong, nhap mon)\n"
           "\t{ 1351, 1 },   // Kim Nhan Hoanh Khong (30)\n"
           "\t{ 1355, 2 },   // Thien Than Dao Huyen (50)\n"
           "\t{ 1360, 3 },   // Thuong Tung Nghenh Khach (60)\n"
           "\t{ 1364, 4 },   // Doat Menh Lien Hoan Tam Tien Kiem (90 kiem)\n"
           "\t{ 1369, 5 },   // Cuu Kiem Hop Nhat (150 kiem)\n"
           "\t{ 1372, 6 },   // Thanh Phong Tong Sang (khi tong, nhap mon)\n"
           "\t{ 1376, 7 },   // Long Huyen Kiem Khi (30)\n"
           "\t{ 1380, 8 },   // Ma Van Kiem Khi (60)\n"
           "\t{ 1382, 9 },   // Phach Thach Pha Ngoc (90 khi)\n"
           "\t{ 1384, 10 },  // Than Quang Toan Nhieu (150 khi)\n"
           "\t{ 1358, 11 },  // Huyen Nhan Van Yen - tran phai\n"
           "\t{ 1349, 12 },  // Kiem Tong Tong Quyet (10)\n"
           "\t{ 1350, 13 },  // Duong Ngo Kiem Phap (20)\n"
           "\t{ 1354, 14 },  // Hi Di Kiem Phap (40)\n"
           "\t{ 1374, 15 },  // Long Nhieu Than (10)\n"
           "\t{ 1375, 16 },  // Hai Nap Bach Xuyen (20)\n"
           "\t{ 1378, 17 },  // Khi Chan Son Ha (40)\n"
           "\t{ 1379, 18 },  // Khi Quan Truong Hong (50)\n"
           "\t{ 1365, 19 },  // Tu Ha Kiem Khi (120)\n"
           "\t{ 1370, 20 }   // Hao Nhien Chi Khi (tien giai)\n" % MARK)
    t.thay(old, new)
    old = "\t\t\tif (Info.nFirstAddFaction == 8 || Info.nFirstAddFaction == 4\t\n\t\t\t\t|| Info.nFirstAddFaction == 5 || Info.nFirstAddFaction == 6) {"
    if not t.co(old):
        old = "\t\t\tif (Info.nFirstAddFaction == 8 || Info.nFirstAddFaction == 4\n\t\t\t\t|| Info.nFirstAddFaction == 5 || Info.nFirstAddFaction == 6) {"
    assert t.co(old), "UiSkillsNew: khong thay dieu kien nut dong"
    t.thay(old, old.replace("Info.nFirstAddFaction == 6) {", "Info.nFirstAddFaction == 6 || Info.nFirstAddFaction == 10) {\t// %s" % MARK))
    t.ghi()

# ---------------------------------------------------------------- Goddess (MultiServer\Goddess = ban build; Goddess2 = ban sao, sua cung)
for rel in (r"MultiServer\Goddess\DBBackup.h", r"MultiServer\Goddess2\src\DBBackup.h"):
    t = Tep(rel)
    if not t.da(MARK):
        t.thay("\t\tTRoleList MoneyStatBySect[11][SECTMAXSTATNUM];",
               "\t\t// %s [11] -> [14] = 13 phai + o [0] chua vao phai; PHAI = Core KProtocol.h TGAME_STAT_DATA (MAX_FACTION_NUM+1)\n\t\tTRoleList MoneyStatBySect[14][SECTMAXSTATNUM];" % MARK)
        t.thay("\t\tTRoleList LevelStatBySect[11][SECTMAXSTATNUM];", "\t\tTRoleList LevelStatBySect[14][SECTMAXSTATNUM];")
        t.thay("\t\tint SectPlayerNum[11];", "\t\tint SectPlayerNum[14];")
        t.thay("\t\tint SectMoneyMost[11];", "\t\tint SectMoneyMost[14];")
        t.thay("\t\tint SectLevelMost[11];", "\t\tint SectLevelMost[14];")
        t.ghi()
t = Tep(r"MultiServer\Goddess\GameStatistic.h")
if not t.da(MARK):
    t.thay("\t\tTRoleList MoneyStatBySect[11][10];", "\t\tTRoleList MoneyStatBySect[14][10];\t// %s" % MARK)
    t.thay("\t\tTRoleList LevelStatBySect[11][10];", "\t\tTRoleList LevelStatBySect[14][10];")
    t.thay("\t\tint SectPlayerNum[11];", "\t\tint SectPlayerNum[14];")
    t.thay("\t\tint SectMoneyMost[11];", "\t\tint SectMoneyMost[14];")
    t.thay("\t\tint SectLevelMost[11];", "\t\tint SectLevelMost[14];")
    t.ghi()
for rel in (r"MultiServer\Goddess\DBBackup.cpp", r"MultiServer\Goddess2\src\DBBackup.cpp"):
    t = Tep(rel)
    if not t.da(MARK):
        t.thay("\tint aDBSSectPlayerCount[12] = {0};", "\tint aDBSSectPlayerCount[15] = {0};\t// %s 13 phai + [13] chua nhap + [14] xuat su" % MARK)
        t.thay("\tdouble aDBSSectMoneyCount[12] = {0};", "\tdouble aDBSSectMoneyCount[15] = {0};")
        old9 = "\t\t\tcase 9:strcpy(aDBSSect,\"%s\");break;\n" % G("昆仑派")
        new9 = old9 + ("\t\t\tcase 10:strcpy(aDBSSect,\"%s\");break;\t// %s\n\t\t\tcase 11:strcpy(aDBSSect,\"%s\");break;\n\t\t\tcase 12:strcpy(aDBSSect,\"%s\");break;\n"
                       % (G("华山派"), MARK, G("武魂"), G("逍遥派")))
        t.thay("\t\t\tcase 10:strcpy(aDBSSect,\"%s\");break;\n\t\t\tcase 11:strcpy(aDBSSect,\"%s\");break;\n" % (G("新手"), G("出师")),
               "\t\t\tcase 13:strcpy(aDBSSect,\"%s\");break;\n\t\t\tcase 14:strcpy(aDBSSect,\"%s\");break;\n" % (G("新手"), G("出师")), 2)
        t.thay(old9, new9, 3)
        t.thay("\t\t\t\t++aDBSSectPlayerCount[10];", "\t\t\t\t++aDBSSectPlayerCount[13];")
        t.thay("\t\t\taDBSSectMoneyCount[10] += ", "\t\t\taDBSSectMoneyCount[13] += ")
        t.thay("\t\t\t\t++aDBSSectPlayerCount[11];", "\t\t\t\t++aDBSSectPlayerCount[14];")
        t.thay("\t\t\t\taDBSSectMoneyCount[11] += ", "\t\t\t\taDBSSectMoneyCount[14] += ")
        t.thay("\tfor(i=0;i<12;++i)\n", "\tfor(i=0;i<15;++i)\n", 2)
        t.thay("(pRoleData->BaseInfo.nSect <=10) && (pRoleData->BaseInfo.nSect >= 1)",
               "(pRoleData->BaseInfo.nSect <= 12) && (pRoleData->BaseInfo.nSect >= 0)", 3)
        t.ghi()

print("XONG" + (" (chi kiem)" if KIEM else ""))
