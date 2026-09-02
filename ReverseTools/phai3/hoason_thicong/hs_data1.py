# -*- coding: utf-8 -*-
"""hs_data1.py [HOASON 01/09] - du lieu + script JX1 cho mon phai Hoa Son (id 10), nguon = Linux.
Ghi vao cay chay that E:\...\bin\server va bin\client (latin-1, giu byte). Idempotent theo marker.
  python hs_data1.py          ap dung
  python hs_data1.py --kiem   chi in ke hoach / kiem neo
"""
import io, os, re, sys, shutil
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
KIEM = "--kiem" in sys.argv
MARK = "[HOASON 01/09]"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
LIN = r"D:\ServerLinux\server1"
VLTK = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw"
SCR = os.path.dirname(os.path.abspath(__file__))
def V(s): return unicode_to_tcvn3_bytes(s).decode("latin-1")
def G(s): return s.encode("gbk").decode("latin-1")
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, d, backup=True):
    if KIEM:
        print("  (kiem) se ghi", p); return
    if backup and os.path.exists(p) and not os.path.exists(p + ".truoc_hoason_0109"):
        shutil.copyfile(p, p + ".truoc_hoason_0109")
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    print("  ghi", p)
def nl_of(d): return "\r\n" if "\r\n" in d else "\n"

def thay(d, old, new, so=1, ten=""):
    c = d.count(old)
    assert c == so, "%s: neo %d lan (can %d): %r" % (ten, c, so, old[:80])
    return d.replace(old, new)

# ============================================================ 1. FactionInfo.ini (4 tep)
def faction_ini():
    for root in (SRV, CLI):
        for fn in ("FactionInfo.ini", G("门派设定.ini")):
            p = os.path.join(root, "settings", "faction", fn)
            if not os.path.exists(p): print("  bo qua (khong co)", p); continue
            d = rd(p)
            if "[10]" in d: print("  da co [10]:", p); continue
            nl = nl_of(d)
            blk = nl.join(["", "[10]", "Name=" + G("华山派"), "Series=" + G("水"), "Camp=" + G("中立"), "ValueName=huashan", "ShowName=" + V("Hoa Sơn"), ""])
            d = d.rstrip("\r\n") + nl + blk
            wr(p, d)

# ============================================================ 2. skills.txt: 38 dong 1347-1384 tu LINUX (server + client)
def skills_txt():
    lin = rd(os.path.join(LIN, "settings", "skills.txt")).split("\n")
    rows = {}
    for l in lin[1:]:
        c = l.rstrip("\r").split("\t")
        if len(c) > 100:
            try: rows[int(c[2])] = l.rstrip("\r")
            except: pass
    for p in (os.path.join(SRV, "settings", "skills.txt"), os.path.join(CLI, "settings", "skills.txt")):
        d = rd(p); nl = nl_of(d)
        lines = d.split(nl)
        n = 0
        for i, l in enumerate(lines):
            c = l.split("\t")
            if len(c) > 2 and c[2].isdigit() and 1347 <= int(c[2]) <= 1384:
                if lines[i] != rows[int(c[2])]:
                    lines[i] = rows[int(c[2])]; n += 1
        print("  skills.txt %s: thay %d dong Hoa Son theo Linux" % (p, n))
        wr(p, nl.join(lines))

# ============================================================ 3. RankSetting.txt 82-105 tu VLTK (server + client)
def rank():
    v = rd(os.path.join(VLTK, "settings__ranksetting.txt")).replace("\r\n", "\n").split("\n")
    add = [l for l in v if l.split("\t")[0].isdigit() and int(l.split("\t")[0]) >= 82]
    for p in (os.path.join(SRV, "settings", "RankSetting.txt"), os.path.join(CLI, "settings", "RankSetting.txt")):
        d = rd(p); nl = nl_of(d)
        if "\n89\t" in d.replace("\r\n", "\n") or d.replace("\r\n", "\n").endswith("\n89\t"): print("  da co rank 89:", p); continue
        d = d.rstrip("\r\n") + nl + nl.join(add) + nl
        wr(p, d)

# ============================================================ 4. npcs.txt: NPC Hoa Son id 2087..2119 tu LINUX (dong = id+1)
HS_NPC_IDS = list(range(2087, 2120))
def npcs():
    lin = rd(os.path.join(LIN, "settings", "npcs.txt")).replace("\r\n", "\n").split("\n")
    hl = lin[0].split("\t")
    for p in (os.path.join(SRV, "settings", "npcs.txt"), os.path.join(CLI, "settings", "npcs.txt")):
        d = rd(p); nl = nl_of(d); lines = d.split(nl)
        hj = lines[0].split("\t")
        assert all(h in hl for h in hj), "npcs: cot JX1 khong co o Linux: %r" % [h for h in hj if h not in hl]
        idx = [hl.index(h) for h in hj]   # anh xa theo TEN cot (Linux 103 cot -> JX1 87 cot)
        n = 0
        for i in HS_NPC_IDS:
            li = i + 1
            cur = lines[li].split("\t")[0].strip()
            lc = lin[li].split("\t")
            assert lc[0].strip() not in ("", "0"), "Linux npcs dong %d rong" % li
            src = "\t".join(lc[k] if k < len(lc) else "" for k in idx)
            if cur in ("", "0"):
                lines[li] = src; n += 1
            elif lines[li] != src:
                print("   CANH BAO: dong %d JX1 da co '%s' - giu nguyen" % (li, cur[:30]))
        print("  npcs.txt %s: dien %d dong (%d cot)" % (p, n, len(hj)))
        wr(p, nl.join(lines))
    # NpcName=id.txt (bang tra cuu)
    p = os.path.join(SRV, "settings", "NpcName=id.txt")
    d = rd(p); nl = nl_of(d); lines = d.split(nl)
    have = set(l.split("\t")[0] for l in lines)
    add = []
    for i in HS_NPC_IDS:
        if str(i) not in have:
            add.append("%d\t%s" % (i, lin[i + 1].split("\t")[0]))
    if add:
        d = d.rstrip("\r\n") + nl + nl.join(add) + nl
        wr(p, d)

# ============================================================ 5. WorldSet + package.ini + pak ban do
def worldset():
    p = os.path.join(SRV, "Maps", "WorldSet_GameServer.ini")
    d = rd(p); nl = nl_of(d)
    if "=987" not in d:
        m = re.search(r"Count=(\d+)", d); cnt = int(m.group(1))
        d = d.replace("Count=%d" % cnt, "Count=%d" % (cnt + 1), 1)
        d = d.rstrip("\r\n") + nl + "World%03d=987 --- Hoa Son phai 2013 %s" % (cnt, MARK) + nl
        wr(p, d)
    else: print("  WorldSet da co 987")
    p = os.path.join(SRV, "package.ini")
    d = rd(p); nl = nl_of(d)
    if "maps_hoason2013.pak" not in d:
        keys = [int(x) for x in re.findall(r"^(\d+)=", d, re.M)]
        d = d.rstrip("\r\n") + nl + "%d=maps_hoason2013.pak" % (max(keys) + 1) + nl
        wr(p, d)
    else: print("  package.ini da co")
    src = os.path.join(SCR, "..", "out", "pak", "maps_hoason2013.pak")
    dst = os.path.join(SRV, "Pak", "maps_hoason2013.pak")
    if not KIEM:
        shutil.copyfile(src, dst); print("  chep", dst)

# ============================================================ 6. huashan.lua (Linux) -> server + client
def huashan_lua():
    src = os.path.join(LIN, "script", "skill", "huashan.lua")
    d = rd(src)
    for p in (os.path.join(SRV, "script", "skill", "huashan.lua"), os.path.join(CLI, "script", "skill", "huashan.lua")):
        if rd(p) == d: print("  huashan.lua da giong Linux:", p); continue
        wr(p, d)

# ============================================================ 7. skills_table.lua: add_hs (Linux 1033-1093)
def skills_table():
    p = os.path.join(SRV, "script", "global", "skills_table.lua")
    d = rd(p)
    if "function add_hs(" in d: print("  add_hs da co"); return
    lin = rd(os.path.join(LIN, "script", "global", "skills_table.lua")).replace("\r\n", "\n")
    m = re.search(r"function add_hs\(lvl\).*?\nend\n", lin, re.S)
    assert m, "Linux skills_table: khong thay add_hs"
    blk = m.group(0).replace("\r\n", "\n")
    nl = nl_of(d)
    blk = "-- %s add_hs: chep nguyen ban Linux script\\global\\skills_table.lua:1033-1093" % MARK + nl + blk.replace("\n", nl)
    d = thay(d, "function del_all_skill()", blk + nl + "function del_all_skill()", 1, "skills_table")
    wr(p, d)

# ============================================================ 8. factionhead.lua + hocvocong.lua + lib_faction.lua
def factionhead():
    p = os.path.join(SRV, "script", "header", "factionhead.lua")
    d = rd(p); nl = nl_of(d)
    if MARK in d: print("  factionhead da va"); return
    # 8a FACTION_INFO[11]
    old = "\t--{2, \"huashan\", 3, 89,"
    i = d.find(old); assert i > 0, "factionhead: khong thay dong huashan comment"
    j = d.find(nl, i)
    line_hs = "\t{2, \"%s\", 3, 89, \"%s\", \"%s\"},\t-- %s Hoa Son id 10" % (
        G("华山派"),
        V("Hoan nghênh bạn gia nhập Hoa Sơn phái trở thành Thư Đồng! Học được võ công Bạch Hồng Quán Nhật, Thanh Phong Tống Sảng"),
        V("Gia nhập Hoa Sơn phái, trở thành Thư Đồng"), MARK)
    d = d[:i] + line_hs + d[j:]
    # 8b SKILLNORMAL[11]
    old = "\t{630,0}---Huyen Thien Vo cuc" + nl + "}" + nl + "};"
    assert d.count(old) == 1, "factionhead: neo SKILLNORMAL cuoi"
    sk = ["{1347,0},---Bach Hong Quan Nhat", "{1372,0},---Thanh Phong Tong Sang", "{1349,0},---Kiem Tong Tong Quyet", "{1374,0},---Long Nhieu Than",
          "{1350,0},---Duong Ngo Kiem Phap", "{1375,0},---Hai Nap Bach Xuyen", "{1351,0},---Kim Nhan Hoanh Khong", "{1376,0},---Long Huyen Kiem Khi",
          "{1354,0},---Hi Di Kiem Phap", "{1378,0},---Khi Chan Son Ha", "{1355,0},---Thien Than Dao Huyen", "{1379,0},---Khi Quan Truong Hong",
          "{1358,0},---Huyen Nhan Van Yen - tran phai", "{1360,0},---Thuong Tung Nghenh Khach", "{1380,0}---Ma Van Kiem Khi"]
    new = "\t{630,0}---Huyen Thien Vo cuc" + nl + "}," + nl + "[11]={--hoa son " + MARK + " (Linux add_hs 10..70)" + nl + nl.join("\t" + s for s in sk) + nl + "}" + nl + "};"
    d = d.replace(old, new)
    # 8c SKILL90_ARRAY[11]
    old = "\t{\"" + V("Túy Tiên Tá Cốt") + "\",394,0}" + nl + "}" + nl + "};"
    assert d.count(old) == 1, "factionhead: neo SKILL90 cuoi"
    new = ("\t{\"" + V("Túy Tiên Tá Cốt") + "\",394,0}" + nl + "}," + nl + "[11]={" + nl +
           "\t{\"" + V("Đoạt Mệnh Liên Hoàn Tam Tiên Kiếm") + "\",1364,1}," + nl + "\t{\"" + V("Phách Thạch Phá Ngọc") + "\",1382,1}" + nl + "}" + nl + "};")
    d = d.replace(old, new)
    # 8d SKILL120AR
    old = "\t709,708,710,711,712,713,714,715,716,717" + nl + "};"
    assert d.count(old) == 1, "factionhead: neo SKILL120AR"
    d = d.replace(old, "\t709,708,710,711,712,713,714,715,716,717,1365 -- 1365 = Tu Ha Kiem Khi (Hoa Son)" + nl + "};")
    # 8e SKILL150_ARRAY[11]
    old = "\t{\"" + V("Thiên Lôi Chấn Nhạc") + "\",1081,1}," + nl + "}" + nl + "};"
    assert d.count(old) == 1, "factionhead: neo SKILL150 cuoi"
    new = ("\t{\"" + V("Thiên Lôi Chấn Nhạc") + "\",1081,1}," + nl + "}," + nl + "[11]={" + nl +
           "\t{\"" + V("Cửu Kiếm Hợp Nhất") + "\",1369,1}," + nl + "\t{\"" + V("Thần Quang Toàn Nhiễu") + "\",1384,1}" + nl + "}" + nl + "};")
    d = d.replace(old, new)
    wr(p, d)

def hocvocong():
    p = os.path.join(SRV, "script", "global", "hocvocong.lua")
    d = rd(p); nl = nl_of(d)
    if MARK in d: print("  hocvocong da va"); return
    old = "\t\t{\"" + V("Túy Tiên Tá Cốt") + "\",394,0}" + nl + "\t}" + nl + "};"
    assert d.count(old) == 1, "hocvocong: neo SKILL90 cuoi (%d)" % d.count(old)
    new = ("\t\t{\"" + V("Túy Tiên Tá Cốt") + "\",394,0}" + nl + "\t}," + nl + "\t[11]={ -- " + MARK + nl +
           "\t\t{\"" + V("Đoạt Mệnh Liên Hoàn Tam Tiên Kiếm") + "\",1364,1}," + nl + "\t\t{\"" + V("Phách Thạch Phá Ngọc") + "\",1382,1}" + nl + "\t}" + nl + "};")
    d = d.replace(old, new)
    old = "\t709,708,710,711,712,713,714,715,716,717" + nl + "}"
    assert d.count(old) == 1, "hocvocong: neo SKILL120AR"
    d = d.replace(old, "\t709,708,710,711,712,713,714,715,716,717,1365" + nl + "}")
    old = "\t\t{\"" + V("Thiên Lôi Chấn Nhạc") + "\",1081,1}," + nl + "\t}" + nl + "}"
    assert d.count(old) == 1, "hocvocong: neo SKILL150 cuoi (%d)" % d.count(old)
    new = ("\t\t{\"" + V("Thiên Lôi Chấn Nhạc") + "\",1081,1}," + nl + "\t}," + nl + "\t[11]={" + nl +
           "\t\t{\"" + V("Cửu Kiếm Hợp Nhất") + "\",1369,1}," + nl + "\t\t{\"" + V("Thần Quang Toàn Nhiễu") + "\",1384,1}" + nl + "\t}" + nl + "}")
    d = d.replace(old, new)
    old = "\t[10] = 10, " + nl + "}"
    assert d.count(old) == 1, "hocvocong: neo FACTION_TO_SKILL150"
    d = d.replace(old, "\t[10] = 10, " + nl + "\t[11] = 11, " + nl + "}")
    wr(p, d)

def lib_faction():
    p = os.path.join(SRV, "script", "lib", "lib_faction.lua")
    d = rd(p); nl = nl_of(d)
    if MARK in d: print("  lib_faction da va"); return
    old = "\tif nFaction == \"" + G("昆仑派") + "\" then F = \"" + V("Côn Lôn") + "\"\treturn F end"
    assert d.count(old) == 1, "lib_faction: neo GetFactionEx"
    d = d.replace(old, old + nl + "\tif nFaction == \"" + G("华山派") + "\" then F = \"" + V("Hoa Sơn") + "\"\treturn F end -- " + MARK)
    old = "\tif nFaction == \"kunlun\" then F = \"" + V("Côn Lôn") + "\"\treturn F end"
    assert d.count(old) == 1, "lib_faction: neo GetFactionEx2"
    d = d.replace(old, old + nl + "\tif nFaction == \"huashan\" then F = \"" + V("Hoa Sơn") + "\"\treturn F end")
    wr(p, d)

# ============================================================ 9. skillbook.lua / lvl120skillbook.lua (sach 90/120)
def skillbook():
    p = os.path.join(SRV, "script", "item", "skillbook.lua")
    d = rd(p); nl = nl_of(d)
    if MARK in d: print("  skillbook da va"); return
    old = "\t[9] = {[90] = {372, 375, 394}, [120] = {717}, [150] = {1080, 1081},}," + nl + "}"
    assert d.count(old) == 1, "skillbook: neo faction_skill_list"
    d = d.replace(old, "\t[9] = {[90] = {372, 375, 394}, [120] = {717}, [150] = {1080, 1081},}," + nl +
                  "\t[10] = {[90] = {1364, 1382}, [120] = {1365}, [150] = {1369, 1384},},\t-- " + MARK + " Hoa Son" + nl + "}")
    old = "\t[59] = {332, 0, 4, 80, "
    i = d.find(old); assert i > 0
    j = d.find(nl, i)
    add = (nl + "\t[4938] = {1364, 1, 10, 80, \"" + V("Hoa Sơn Kiếm Tông-Đoạt mệnh liên hoàn tam tiên kiếm") + "\",\t\"" + V("Đoạt Mệnh Liên Hoàn Tam Tiên Kiếm") + "\"}," +
           nl + "\t[4939] = {1382, 1, 10, 80, \"" + V("Hoa Sơn Khí Tông-Bích Thạch Phá Ngọc") + "\",\t\"" + V("Phách Thạch Phá Ngọc") + "\"},")
    d = d[:j] + add + d[j:]
    old = "\tif nFact == nil or nFact < 0 or  nFact > 9 then"
    assert d.count(old) == 1, "skillbook: neo nFact > 9"
    d = d.replace(old, "\tif nFact == nil or nFact < 0 or  nFact > 12 or %faction_skill_list[nFact] == nil then")
    wr(p, d)
    p = os.path.join(SRV, "script", "item", "lvl120skillbook.lua")
    d = rd(p); nl = nl_of(d)
    if "[10] = {1365" in d: print("  lvl120skillbook da va"); return
    old = "\t[9] = {717, 1, 9, 120, \"" + V("Kỹ năng cấp 120") + "\",\t\"" + V("Lưỡng Nghi Chân Khí") + " \"},"
    assert d.count(old) == 1, "lvl120skillbook: neo [9]"
    d = d.replace(old, old + nl + "\t[10] = {1365, 1, 10, 120, \"" + V("Kỹ năng cấp 120") + "\",\t\"" + V("Tử Hà Kiếm Khí") + "\"},\t-- " + MARK)
    wr(p, d)

# ============================================================ 10. lenhbaitanthu.lua: chuyen phai them Hoa Son
def lenhbai():
    p = os.path.join(SRV, "script", "item", "lenhbaitanthu.lua")
    d = rd(p); nl = nl_of(d)
    if MARK in d: print("  lenhbaitanthu da va"); return
    old = "\"" + V("Côn Lôn") + " /doiphai1\")"
    assert d.count(old) == 1, "lenhbai: neo Con Lon /doiphai1"
    d = d.replace(old, "\"" + V("Côn Lôn") + " /doiphai1\"," + nl + "\"" + V("Hoa Sơn") + " /doiphai1\") -- " + MARK)
    # so muc 10 -> 11 trong Say cua chuyenphai: dong 'Say("Vui long chon mon phai can chuyen:...",10,'
    m = re.search(r'(function chuyenphai\(\)\s*' + re.escape(nl) + r'\s*' + re.escape(nl) + r'?Say\(".*?",)10,', d, re.S)
    assert m, "lenhbai: neo Say(...,10,"
    d = d[:m.start()] + m.group(1) + "11," + d[m.end():]
    old = ("elseif nCurFac == 10 then" + nl + "\tSetSeries(4)" + nl + "\tSetFaction(FACTION_INFO[nCurFac][2])" + nl + "\tSetCamp(FACTION_INFO[nCurFac][3])" + nl +
           "\tSetCurCamp(FACTION_INFO[nCurFac][3])" + nl + "\tSetRank(FACTION_INFO[nCurFac][4])" + nl + "\thockynang(nCurFac)" + nl + "end\t")
    assert d.count(old) == 1, "lenhbai: neo elseif nCurFac == 10 block (%d)" % d.count(old)
    new = old[:-len("end\t")] + ("elseif nCurFac == 11 then -- Hoa Son (he Thuy)" + nl + "\tSetSeries(2)" + nl + "\tSetFaction(FACTION_INFO[nCurFac][2])" + nl +
                                 "\tSetCamp(FACTION_INFO[nCurFac][3])" + nl + "\tSetCurCamp(FACTION_INFO[nCurFac][3])" + nl + "\tSetRank(FACTION_INFO[nCurFac][4])" + nl +
                                 "\thockynang(nCurFac)" + nl + "end\t")
    d = d.replace(old, new)
    wr(p, d)

# ============================================================ 11. NPC gia nhap trong thon + spawn
def npc_monphai():
    p = os.path.join(SRV, "script", "npcthon", "npcmonphai", "hoason.lua")
    nl = "\r\n"
    d = nl.join([
        "--" + MARK + " De Tu Hoa Son phai o thon lang - khuon theo conlon.lua (he Thuy, id 10)",
        "Include(\"\\\\script\\\\header\\\\factionhead.lua\")",
        "Include(\"\\\\script\\\\lib\\\\lib_task.lua\")",
        "Include(\"\\\\script\\\\npcthon\\\\npcmonphai\\FactionHelper.lua\")",
        "",
        "FactionName = \"" + G("华山派") + "\"",
        "",
        "function main(nNpcIndex)",
        "local player_Faction = GetFaction();",
        "\tif (player_Faction == \"\") then",
        "\t\tTalk(1,\"enroll_select\",\"" + V("Phái Hoa Sơn ta đã không tham gia giang hồ hơn 10 năm nay, lần này tái xuất ắt sẽ khiến bọn giang hồ tà phái phải khiếp sợ! Võ công bổn phái chia hai nhánh: Kiếm Tông và Khí Tông.") + "\")",
        "\telseif (player_Faction == \"" + G("华山派") + "\") or (player_Faction == \"huashan\") then",
        "\t\t\tif (GetTask(TASK_DUNGCHUNG2) == 0) then",
        "\t\t\t\tif GetLevel() >= 60 then",
        "\t\t\t\tSay(\"<npc>: " + V("Nghe nói sau khi ngươi xuống núi đã lập chút công danh, có nhớ đến sư đệ sư muội chúng ta không?") + "\",4,\"" + V("Xuất sư xuống núi") + "/xuatsu\",\"" + V("Ta muốn nhận kỹ năng 9x") + "/hotrokn\",\"" + V("Tìm hiểu võ nghệ bổn môn") + "/skill_help\",\"" + V("Xin chuyển dùm lời hỏi thăm sư phụ") + " /no\")",
        "\t\t\t\telse",
        "\t\t\t\tSay(\"<npc>: " + V("Nghe nói sau khi ngươi xuống núi đã lập chút công danh, có nhớ đến sư đệ sư muội chúng ta không?") + "\",3,\"" + V("Tìm hiểu khu vực luyện công") + "/map_help\",\"" + V("Tìm hiểu võ nghệ bổn môn") + "/skill_help\",\"" + V("Xin chuyển dùm lời hỏi thăm sư phụ") + " /no\")",
        "\t\t\t\tend",
        "\t\t\telse",
        "\t\t\t\tSay(\"<npc>: " + V("Nghe nói sau khi ngươi xuống núi đã lập chút công danh, có nhớ đến sư đệ sư muội chúng ta không?") + "\",4,\"" + V("Trùng phản môn phái") + "/trungphansumon\",\"" + V("Tìm hiểu khu vực luyện công") + "/map_help\",\"" + V("Tìm hiểu võ nghệ bổn môn") + "/skill_help\",\"" + V("Xin chuyển dùm lời hỏi thăm sư phụ") + " /no\")",
        "\t\t\tend",
        "\tend",
        "end",
        "",
        "function enroll_select()",
        "if (GetSeries() == 2) and (GetCamp() == 0) then",
        "\t\tif (GetLevel() >= 10) then",
        "\t\t\tSay(\"" + V("Bổn môn Kiếm Tông tu thân, Khí Tông tu tâm, dung hợp trong võ công. Có muốn gia nhập phái Hoa Sơn không?") + "\", 2, \"" + V("Gia nhập Hoa Sơn") + "/go\", \"" + V("Để ta suy nghĩ kỹ lại xem") + "/thing\")",
        "\t\telse",
        "\t\t\tTalk(1,\"\",\"" + V("Căn bản của ngươi còn kém lắm! Hãy đi luyện tập thêm, bao giờ đến cấp 10 lại đến tìm ta!") + "\")",
        "\t\tend",
        "\tend",
        "end;",
        "",
        "function go()",
        "\tgianhapmonphai(10)",
        "\tSetLastFactionNumber(10)",
        "\tSetTask(3481, 10*256)\t-- bien nhiem vu mon phai Hoa Son (Linux nTaskId_Fact)",
        "\tSetRevPos(987,1)",
        "end;",
        "",
        "function thing()",
        "\tTalk(1,\"\",10239)",
        "end;",
        ""])
    if os.path.exists(p) and rd(p) == d: print("  hoason.lua da co");
    else: wr(p, d, backup=False)
    # spawn NPC trong 3 thon (id 2096 = Lan Hao Thien, ten 'Hoa Son Kiem Khach' theo Linux hoason_parserby)
    spawns = {"balanghuyen.lua": (53, 1632, 3191), "giangtanthon.lua": (20, 3567, 6190), "longmontran.lua": (99, 1641, 3189)}
    for fn, (mapid, x, y) in spawns.items():
        p = os.path.join(SRV, "script", "startgame", "thon", fn)
        d = rd(p); nl = nl_of(d)
        if MARK in d: print("  spawn da co:", fn); continue
        ms = [m for m in re.finditer(r"^[^\n]*npcmonphai\\\\conlon\.lua[^\n]*$", d, re.M)]
        assert len(ms) >= 1, "%s: khong thay dong spawn Con Lon" % fn
        j = ms[-1].end()
        if d[j-1:j] == "\r": j -= 1
        line = ("\tnNpcIdx = AddNpcNew(2096,1,%d,%d*32,%d*32,\"\\\\script\\\\npcthon\\\\npcmonphai\\\\hoason.lua\",6,\"%s\") SetNpcValue(nNpcIdx, 10); -- %s"
                % (mapid, x, y, V("Hoa Sơn Kiếm Khách"), MARK))
        d = d[:j] + nl + line + d[j:]
        wr(p, d)

# ============================================================ 12. Client UI ini: UiSkillNew / UiSkillFly / UiSkillFlySub
def ui_ini():
    nl = "\r\n"
    p = os.path.join(CLI, "Ui", "Ui3", "UiSkillNew.ini")
    d = rd(p)
    if "[Main10]" not in d:
        add = nl.join(["", "#hoa son " + MARK, "[Main10]", "Left=220", "Top=20", "Width=366", "Height=500", "Moveable=1", "Trans=0", "StartPos=220,-676",
                       "Image=\\Spr\\Ui3\\UiSkills\\khung_hs.spr", "", "[Main101024]", "Left=400", "Top=80", "Width=366", "Height=500", "Moveable=1", "Trans=0",
                       "StartPos=220,-676", "Image=\\Spr\\Ui3\\UiSkills\\khung_hs.spr", ""])
        wr(p, d.rstrip("\r\n") + nl + add)
    else: print("  UiSkillNew [Main10] da co")
    p = os.path.join(CLI, "Ui", "Ui3", "UiSkillFly.ini")
    d = rd(p)
    if "[RemainPoint_10]" not in d:
        add = nl.join(["", "[RemainPoint_10]", "Left=310", "Top=390", "Width=36", "Height=14", "Color=255,253,122", "BorderColor=255,0,0", "Font=14", ""])
        wr(p, d.rstrip("\r\n") + nl + add)
    else: print("  UiSkillFly [RemainPoint_10] da co")
    p = os.path.join(CLI, "Ui", "Ui3", "UiSkillFlySub.ini")
    d = rd(p)
    if "[Skill_10_0]" not in d:
        m = re.search(r"\[SkillBtn_9_0\]\r?\n(.*?)(?=\r?\n\r?\n|\r?\n\[)", d, re.S)
        assert m, "UiSkillFlySub: khong thay [SkillBtn_9_0]"
        btn_body = m.group(1).replace("\r\n", "\n").split("\n")
        img = [l for l in btn_body if l.startswith("Image=")][0]
        xs = [18, 88, 158, 230, 299]; ys = [15, 81, 148, 216, 284, 350]
        # index -> (col,row): kiem tong col0 (0..5), khi tong col1 (6..10 + 11 tran phai), ho tro col2 (12..17), col3 (18,19,20), khinh cong col4 (30)
        pos = {}
        for k in range(6): pos[k] = (0, k)
        for k in range(6, 12): pos[k] = (1, k - 6)
        for k in range(12, 18): pos[k] = (2, k - 12)
        pos[18] = (3, 0); pos[19] = (3, 1); pos[20] = (3, 2); pos[30] = (4, 0)
        out = ["", "#======================HOA SON " + MARK + " =============="]
        for k, (c, r) in pos.items():
            x, y = xs[c], ys[r]
            out += ["[Skill_10_%d]" % k, "Left=%d" % x, "Top=%d" % y, "Width=36", "Height=36", "HaveBgColor=0", "",
                    "[SkillBtn_10_%d]" % k, "Left=%d" % (x + 24), "Top=%d" % (y + 39), "Width=14", "Height=14", "Trans=0", "Up=1", "Down=0", "DisableFrame=2", img, ""]
        wr(p, d.rstrip("\r\n") + nl + nl.join(out))
    else: print("  UiSkillFlySub [Skill_10_*] da co")

if __name__ == "__main__":
    loi = []
    for f in (faction_ini, skills_txt, rank, npcs, worldset, huashan_lua, skills_table, factionhead, hocvocong, lib_faction, skillbook, lenhbai, npc_monphai, ui_ini):
        print("==", f.__name__)
        if KIEM:
            try: f()
            except Exception as e:
                print("   LOI:", e); loi.append(f.__name__)
        else:
            f()
    if loi: print("HAM LOI:", loi)
    print("XONG" + (" (kiem)" if KIEM else ""))
