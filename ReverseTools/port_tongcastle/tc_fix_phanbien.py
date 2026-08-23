# -*- coding: utf-8 -*-
r"""[TONGCASTLE 23/08 - hau phan bien] F13, F15, F16, F18, F20, F22, F23 (F14 = F10 da va o tongwar_fix):
F13 (CAO):  KItemList sIsJx2ItemScript + shenmuling.lua (main tra nil khi thanh cong -> engine
            tru stack-aware nhu Linux; thieu thi Than Moc Lenh dung khong mat).  [ENGINE - build lai]
F15 (TRUNG):CheckAllMaps (header\forbidmap.lua) + 984 - MOT diem chan phu thodiafu/shenxingfu/
            thodiaphuvh... (moi item dich chuyen deu goi CheckAllMaps).
F16 (TRUNG):TONGCASTLE_Adm_ReviveTree xoa record cay loai n o relay truoc khi AddTreeInMap
            (relay SaveNpcData APPEND -> goi lap = x2 diem thu + cay chong).
F18 (THAP): npcs.txt 1912-1914 (cay): 5 cot *ResistMax 25 -> 95 theo Linux (server + client).
            NpcResType giu enemy197 (boss099 chua chac co trong pak client) - lech ghi nhan.
F20 (THAP): tc_port.py buoc 10: guard idempotent Include zhenyuan/exp.
F22 (THAP): comment nhanh huoyuedu trong NotifyPlayTime (he huoyuedu khong co tren JX1 ->
            spam log moi phut moi nguoi du dieu kien Chu nhat).
F23 (THAP): treedeath adapter dat PlayerIndex = ke giet truoc khi tbLog (log dung nguoi).
"""
import io, os, re, shutil

SRC  = r"D:\GAMEDEVNEW\Sources\Core\Src"
E    = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI  = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIRC = r"D:\GAMEDEVNEW\serverscript_jx2\tongcastle"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    d = os.path.dirname(p)
    if d and not os.path.isdir(d): os.makedirs(d)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)
def mir(p_abs, rel):
    d = os.path.dirname(os.path.join(MIRC, rel))
    if not os.path.isdir(d): os.makedirs(d)
    shutil.copyfile(p_abs, os.path.join(MIRC, rel))

# ---- F13: engine KItemList ----
p = SRC + r"\KItemList.cpp"; s = rd(p); h = hi(s)
if "shenmuling" not in s:
    old = "\t\t\"\\\\script\\\\vng_event\\\\item\\\\biggoldenseed.lua\","
    assert s.count(old) == 1
    s = s.replace(old, old + "\t\t\t// [PORT5 23/08]\n\t\t\"\\\\script\\\\missions\\\\tongcastle\\\\shenmuling.lua\",\t\t// [TONGCASTLE 23/08 phan bien F13] Than Moc Lenh 3205-3207 (thanh cong tra nil -> engine tru)", 1)
    assert hi(s) == h
    wr(p, s)
    print("F13 ok (can BUILD LAI CoreServer)")
else:
    print("F13 da co")

# ---- F15: CheckAllMaps + 984 ----
p = os.path.join(E, r"script\header\forbidmap.lua"); s = rd(p)
if "984" not in s:
    NL = "\r\n" if "\r\n" in s else "\n"
    old = "function CheckAllMaps(mapid)"
    assert s.count(old) == 1
    s = s.replace(old, old + NL +
        "\t-- [TONGCASTLE 23/08 phan bien F15] cam moi item dich chuyen trong Thanh Bao 984" + NL +
        "\t-- (DisabledUseTownP la no-op tren JX1; Linux cam qua forbiditem TRANSFER)" + NL +
        "\tif mapid == 984 then" + NL +
        "\t\treturn 1" + NL +
        "\tend", 1)
    wr(p, s)
    print("F15 ok")
else:
    print("F15 da co")
d = os.path.join(MIRC, r"jx1_edits\script\header")
if not os.path.isdir(d): os.makedirs(d)
shutil.copyfile(p, os.path.join(d, "forbidmap.lua"))

# ---- F16: Adm_ReviveTree don record truoc ----
p = os.path.join(E, r"script\missions\tongcastle\tongcastle_driver.lua"); s = rd(p)
if "DelAllTreeData" not in s:
    NL = "\r\n" if "\r\n" in s else "\n"
    old = 'function TONGCASTLE_Adm_ReviveTree(n)'
    assert s.count(old) == 1
    s = s.replace(old, old + NL +
        '\t-- [phan bien F16] relay SaveNpcData APPEND record - phai xoa record loai n truoc, khong thi x2 diem thu/cay chong' + NL +
        '\tDynamicExecute(TC_RELAY, "tbS3TongCastle:DelAllTreeData", n or 1)', 1)
    wr(p, s)
    print("F16 ok")
else:
    print("F16 da co")
mir(p, r"script\missions\tongcastle\tongcastle_driver.lua")

# ---- F18: npcs.txt cay 1912-1914 ResistMax 95 ----
for root, tag in ((E, "server"), (CLI, "client")):
    p = os.path.join(root, r"settings\npcs.txt"); s = rd(p)
    NL = "\r\n" if "\r\n" in s else "\n"
    ls = s.split(NL)
    hdr = ls[0].split("\t")
    idxs = [hdr.index(c) for c in ("FireResistMax", "ColdResistMax", "LightResistMax", "PoisonResistMax", "PhysicsResistMax")]
    fixed = 0
    for tid in (1912, 1913, 1914):
        row = ls[tid + 1].split("\t")
        for ci in idxs:
            if row[ci] == "25":
                row[ci] = "95"
                fixed += 1
        ls[tid + 1] = "\t".join(row)
    wr(p, NL.join(ls))
    print("F18 %s: %d o 25->95" % (tag, fixed))
mir(os.path.join(E, r"settings\npcs.txt"), r"jx1_edits\settings\npcs.txt")
shutil.copyfile(os.path.join(CLI, r"settings\npcs.txt"), os.path.join(MIRC, "jx1_edits", "client_npcs.txt"))

# ---- F20: tc_port.py guard idempotent buoc 10 ----
p = r"D:\GAMEDEVNEW\ReverseTools\port_tongcastle\tc_port.py"
s = io.open(p, "r", encoding="utf-8").read()
if 'if "zhenyuan_jx1" not in s:' not in s:
    old = 's = s[:m.end()] + NLa + \'Include("\\\\\\\\script\\\\\\\\lib\\\\\\\\awardtype\\\\\\\\zhenyuan_jx1.lua")\\t\' + MARK + \'diem Chan Nguyen (shenmuling)\' + s[m.end():]'
    assert s.count(old) == 1, "tc_port buoc 10"
    s = s.replace(old, 'if "zhenyuan_jx1" not in s:\n    ' + old.replace("s = s[", "s = s["), 1)
    io.open(p, "w", encoding="utf-8").write(s)
    print("F20 ok")
else:
    print("F20 da co")

# ---- F22: comment huoyuedu ----
p = os.path.join(E, r"script\missions\tongcastle\tongcastle.lua"); s = rd(p)
old = '\t\tDynamicExecuteByPlayer(PlayerIndex, "\\\\script\\\\huoyuedu\\\\huoyuedu.lua", "tbHuoYueDu:AddHuoYueDu", "tongcastle")'
if s.count(old) == 1:
    NL = "\r\n" if "\r\n" in s else "\n"
    s = s.replace(old,
        "\t\t-- [TONGCASTLE 23/08 phan bien F22] he huoyuedu (diem hoat bat) KHONG co tren JX1 -" + NL +
        "\t\t-- de nguyen se spam log 'script chua nap' moi phut moi nguoi du dieu kien Chu nhat" + NL +
        "\t\t--" + old.lstrip("\t"), 1)
    wr(p, s)
    print("F22 ok")
else:
    print("F22 da xu ly (%d)" % s.count(old))
mir(p, r"script\missions\tongcastle\tongcastle.lua")

# ---- F23: treedeath adapter dat PlayerIndex ----
p = os.path.join(E, r"script\missions\tongcastle\treedeath.lua"); s = rd(p)
old = "\tlocal nAtk = NpcIdx2PIdx(nDamageIdx)\n\tif nAtk and nAtk > 0 then\n"
oldr = old.replace("\n", "\r\n")
target = old if old in s else (oldr if oldr in s else None)
if target and "PlayerIndex = nAtk" not in s:
    NL = "\r\n" if "\r\n" in s else "\n"
    new = ("\tlocal nAtk = NpcIdx2PIdx(nDamageIdx)" + NL +
           "\tif nAtk and nAtk > 0 then" + NL +
           "\t\tPlayerIndex = nAtk\t-- [phan bien F23] engine khong dat boi canh khi goi OnDeath cua NPC - de tbLog ghi dung ke giet" + NL)
    s = s.replace(target, new, 1)
    wr(p, s)
    print("F23 ok")
else:
    print("F23 da co / khong thay anchor" if not target else "F23 da co")
mir(p, r"script\missions\tongcastle\treedeath.lua")
print("XONG")
