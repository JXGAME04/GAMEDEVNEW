# -*- coding: utf-8 -*-
r"""[25/08 v2] Them BLOCK CONFIG TIN SU (TS_) vao cauhinh_hoatdong.lua + noi diem doc.
NL do theo TUNG FILE (cauhinh = LF, cac file khac tu xet). Chu thich tieng Viet
co dau (TCVN3 qua tcvn3.py - bang ma tu kiem 4/4 voi chuoi that trong repo).
Nguon doi chieu ghi trong commit r8.
"""
import io, os, sys, shutil
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from tcvn3 import tcvn3 as C

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"

def rd(p): return io.open(p, "rb").read()
def wr(p, b): io.open(p, "wb").write(b)
def nlof(b): return b"\r\n" if b"\r\n" in b else b"\n"
def bak(p):
    q = p + ".truoc_cfgts_2508"
    if not os.path.isfile(q): shutil.copyfile(p, q)

# ================= 1) BLOCK TS_ =================
p = os.path.join(E, r"header\cauhinh_hoatdong.lua")
s = rd(p); NL = nlof(s)
if b"TS_CAP_TOITHIEU" in s:
    print("1. block TS_: da co")
else:
    neo = b"-- ===========================================================================" + NL + b"-- [2] BACH NHAN LOI DAI"
    assert s.count(neo) == 1, "khong thay neo [2]"
    B = []
    A = B.append
    A(b"-- ===========================================================================")
    A(b"-- [1b] " + C("NHI\u1ec6M V\u1ee4 T\u00cdN S\u1ee8 (Thi\u00ean B\u1ea3o Kh\u1ed1 \u0111\u01b0a tin) - ti\u1ec1n t\u1ed1 TS_"))
    A(b"-- ===========================================================================")
    A(b"-- " + C("Nh\u1eadn \u1edf D\u1ecbch Quan Ba L\u0103ng Huy\u1ec7n (map 11) / \u0110\u1ea1i L\u00fd (162); Xa phu \u0111\u01b0a v\u00e0o \u1ea3i map 395."))
    A(b"-- " + C("Chu\u1ed7i l\u00e0m: gi\u1ebft Th\u1ee7 H\u1ed9 Gi\u1ea3 -> m\u1edf 5/9 B\u1ea3o R\u01b0\u01a1ng \u0111\u00fang th\u1ee9 t\u1ef1 -> Ti\u00eau Tr\u1ea5n ra -> tr\u1ea3 nhi\u1ec7m v\u1ee5."))
    A(b"")
    A(b"-- " + C("C\u1ea5p t\u1ed1i thi\u1ec3u nh\u1eadn nhi\u1ec7m v\u1ee5 (g\u1ed1c Linux 120, ch\u1ee7 game \u0111\u00e3 h\u1ea1 90). [RESTART]"))
    A(b"TS_CAP_TOITHIEU = 90,")
    A(b"")
    A(b"-- " + C("S\u1ed1 l\u01b0\u1ee3t th\u01b0\u1eddng/ng\u00e0y + s\u1ed1 l\u01b0\u1ee3t mua th\u00eam b\u1eb1ng item Thi\u00ean Kh\u1ed1 B\u1ea3o L\u1ec7nh (6,1,3431)."))
    A(b"-- " + C("\u0110\u1ebfm theo ng\u00e0y \u1edf task 4128. [RESTART]"))
    A(b"TS_LUOT_THUONG_NGAY = 2,")
    A(b"TS_LUOT_ITEM_NGAY = 1,")
    A(b"")
    A(b"-- --- " + C("PH\u1ea6N TH\u01af\u1edeNG (nil = d\u00f9ng \u0111\u00fang b\u1ea3ng g\u1ed1c Linux) ---------------------------"))
    A(b"-- " + C("B\u1ea3ng g\u1ed1c: settings\\task\\tollgate\\messenger\\messenger_tollprize.txt, h\u00e0ng 10 (qian120):"))
    A(b"-- " + C("   m\u1edf 1 B\u1ea3o R\u01b0\u01a1ng    = 10000 exp + 9 \u0111i\u1ec3m T\u00edn S\u1ee9 (c\u1ed9ng v\u00e0o task 1205)"))
    A(b"-- " + C("   gi\u1ebft 1 Th\u1ee7 H\u1ed9 Gi\u1ea3 = 40000 exp + 30 \u0111i\u1ec3m T\u00edn S\u1ee9"))
    A(b"-- " + C("\u0110i\u1ec1n s\u1ed1 v\u00e0o \u0111\u00e2y l\u00e0 GHI \u0110\u00c8 b\u1ea3ng; \u0111\u1ec3 nil l\u00e0 gi\u1eef nguy\u00ean b\u1ea3ng (100% Linux). [RESTART]"))
    A(b"TS_EXP_MO_RUONG = nil,")
    A(b"TS_DIEM_MO_RUONG = nil,")
    A(b"TS_EXP_GIET_THUHO = nil,")
    A(b"TS_DIEM_GIET_THUHO = nil,")
    A(b"")
    A(b"-- " + C("Th\u01b0\u1edfng khi TR\u1ea2 nhi\u1ec7m v\u1ee5 \u1edf tr\u1ea1m d\u1ecbch (posthouse.lua, h\u00e0m messenger_treasureprize):"))
    A(b"-- " + C("   l\u1ea7n \u0110\u1ea6U trong ng\u00e0y: TS_TRA_HANHHIEP c\u00e1i H\u00e0nh Hi\u1ec7p L\u1ec7nh (6,1,2575)"))
    A(b"-- " + C("   M\u1eccI l\u1ea7n tr\u1ea3: TS_TRA_BAORUONG c\u00e1i T\u00edn S\u1ee9 B\u1ea3o R\u01b0\u01a1ng (6,1,3430)"))
    A(b"-- " + C("   (ri\u00eang nguy\u00ean li\u1ec7u Kinh M\u1ea1ch 4847 t\u1ed1i \u0111a 2 l\u1ea7n/ng\u00e0y theo task 3073 - gi\u1eef nguy\u00ean)"))
    A(b"-- [RESTART]")
    A(b"TS_TRA_HANHHIEP = 3,")
    A(b"TS_TRA_BAORUONG = 2,")
    A(b"")
    A(b"-- [ENGINE/" + C("B\u1ea2NG") + b"] " + C("Kh\u00f4ng ch\u1ec9nh \u1edf \u0111\u00e2y:"))
    A(b"--   * " + C("S\u1ed1 r\u01b0\u01a1ng ph\u1ea3i m\u1edf (5) / t\u1ed5ng r\u01b0\u01a1ng (9): messenger_baoxiangtask.lua d\u00f2ng 10-11,"))
    A(b"--     " + C("g\u1eafn c\u1ee9ng v\u1edbi 9 v\u1ecb tr\u00ed NPC trong killbosshead.lua - \u0111\u1eebng \u0111\u1ed5i."))
    A(b"--   * " + C("Gi\u1edbi h\u1ea1n 2 gi\u1edd trong \u1ea3i (task 1222): B\u1eca COMMENT T\u1eea G\u1ed0C \u1edf c\u1ea3 Linux"))
    A(b"--     " + C("(messenger_timeer.lua) - kh\u00f4ng gi\u1edbi h\u1ea1n th\u1eddi gian l\u00e0 \u0111\u00fang b\u1ea3n g\u1ed1c."))
    A(b"--   * " + C("Danh s\u00e1ch tr\u1ea1m tr\u1ea3 nhi\u1ec7m v\u1ee5: b\u1ea3ng citygo trong posthouse.lua."))
    A(b"")
    s = s.replace(neo, NL.join(B) + NL + neo, 1)
    bak(p); wr(p, s)
    print("1. cauhinh_hoatdong.lua: DA THEM block TS_ (co dau)")

# ================= 2) posthouse.lua =================
p = os.path.join(E, r"task\tollgate\messenger\posthouse.lua")
s = rd(p); NL = nlof(s)
if b"HD_CFG" in s:
    print("2. posthouse: da noi")
else:
    i = s.find(b"Include(")
    j = s.find(NL, i)
    inc = b'Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")\t-- [CFG TS 25/08] doc TS_* (file LA, khong Include gi)'
    s = s[:j+len(NL)] + inc + NL + s[j+len(NL):]

    cu = b"if ( GetLevel() < 90 ) then"
    assert s.count(cu) == 1, "anchor cap = %d" % s.count(cu)
    s = s.replace(cu, b'if ( GetLevel() < HD_CFG("TS_CAP_TOITHIEU", 90) ) then', 1)

    cu = b"local nNormalTaskLimit = 2"
    assert s.count(cu) == 1
    s = s.replace(cu, b'local nNormalTaskLimit = HD_CFG("TS_LUOT_THUONG_NGAY", 2)', 1)
    cu = b"local nIBTaskLimit = 1"
    assert s.count(cu) == 1
    s = s.replace(cu, b'local nIBTaskLimit = HD_CFG("TS_LUOT_ITEM_NGAY", 1)', 1)

    cu = b"for i = 1, 3 do"
    assert s.count(cu) == 1, "anchor HanhHiep = %d" % s.count(cu)
    s = s.replace(cu, b'for i = 1, HD_CFG("TS_TRA_HANHHIEP", 3) do', 1)
    # Msg "3 Hanh Hiep Lenh" -> so dong
    i = s.find(b"AddItem(6,1,2575,1,0,0)")
    m = s.find(b"Msg2Player(", i); e = s.find(NL, m)
    line = s[m:e]
    k = line.find(b"3")
    assert k > 0, "khong thay so 3 trong msg HanhHiep"
    s = s[:m] + line[:k] + b'"..HD_CFG("TS_TRA_HANHHIEP", 3).."' + line[k+1:] + s[e:]

    cu = b"for i = 1, 2 do" + NL + b"\t\tAddItem(6,1,3430,0,0,0)"
    assert s.count(cu) == 1, "anchor BaoRuong = %d" % s.count(cu)
    s = s.replace(cu, b'for i = 1, HD_CFG("TS_TRA_BAORUONG", 2) do' + NL + b"\t\tAddItem(6,1,3430,0,0,0)", 1)
    i = s.find(b"AddItem(6,1,3430,0,0,0)")
    m = s.find(b"Msg2Player(", i); e = s.find(NL, m)
    line = s[m:e]
    k = line.find(b"2")
    assert k > 0, "khong thay so 2 trong msg BaoRuong"
    s = s[:m] + line[:k] + b'"..HD_CFG("TS_TRA_BAORUONG", 2).."' + line[k+1:] + s[e:]

    bak(p); wr(p, s)
    print("2. posthouse.lua: DA NOI cap/luot/thuong-tra")

# ================= 3) tureboss90 + turebug90 =================
for ten, v_exp, v_diem, k_exp, k_diem in (
    ("tureboss90.lua", b"TUREBOSS_EXP", b"TUREBOSS_MESSENGEREXP", b"TS_EXP_MO_RUONG", b"TS_DIEM_MO_RUONG"),
    ("turebug90.lua", b"TUREBUGBEAR_EXP", b"TUREBUGBEAR_MESSENGEREXP", b"TS_EXP_GIET_THUHO", b"TS_DIEM_GIET_THUHO")):
    p = os.path.join(E, r"task\tollgate\messenger\qianbaoku\90", ten)
    s = rd(p); NL = nlof(s)
    if b"HD_CFG" in s:
        print("3. %s: da noi" % ten); continue
    i = s.find(b"Include(")
    j = s.find(NL, i)
    s = s[:j+len(NL)] + b'Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")\t-- [CFG TS 25/08]' + NL + s[j+len(NL):]
    i2 = s.find(v_diem)
    i2 = s.find(v_diem, i2 + 1) if s[i2-1:i2] == b"\n" else i2   # lay dong khai bao
    # don gian: tim dong "v_diem=" khai bao (co dau =)
    i2 = s.find(v_diem + b"=")
    if i2 < 0: i2 = s.find(v_diem + b" =")
    assert i2 > 0, "khong thay khai bao " + v_diem.decode()
    j2 = s.find(NL, i2)
    ov = (b"-- [CFG TS 25/08] " + C("cho ph\u00e9p ghi \u0111\u00e8 th\u01b0\u1edfng t\u1eeb config; \u0111\u1ec3 nil = gi\u1eef b\u1ea3ng g\u1ed1c Linux") + NL +
          v_exp + b' = HD_CFG("' + k_exp + b'", ' + v_exp + b")" + NL +
          v_diem + b' = HD_CFG("' + k_diem + b'", ' + v_diem + b")" + NL)
    s = s[:j2+len(NL)] + ov + s[j2+len(NL):]
    bak(p); wr(p, s)
    print("3. %s: DA NOI %s/%s" % (ten, k_exp.decode(), k_diem.decode()))

# ================= 4) HD_RELOAD_LIST =================
p = os.path.join(E, r"item\hoatdong_admin.lua")
s = rd(p); NL = nlof(s)
if b"posthouse.lua" in s:
    print("4. HD_RELOAD_LIST: da co")
else:
    cu = b'"\\\\script\\\\vng_event\\\\item\\\\biggoldenseed.lua",'
    assert s.count(cu) == 1, "anchor RELOAD_LIST = %d" % s.count(cu)
    them = (cu + NL +
        b"\t-- [CFG TS 25/08] file doc TS_*; posthouse con BAN SAO trong state wagoner/turenpc/" + NL +
        b"\t-- turerukou nen nap lai o day chi cap nhat state goc - muon an moi cua van can" + NL +
        b"\t-- restart GS (vi vay config danh dau [RESTART])" + NL +
        b'\t"\\\\script\\\\task\\\\tollgate\\\\messenger\\\\posthouse.lua",' + NL +
        b'\t"\\\\script\\\\task\\\\tollgate\\\\messenger\\\\qianbaoku\\\\90\\\\tureboss90.lua",' + NL +
        b'\t"\\\\script\\\\task\\\\tollgate\\\\messenger\\\\qianbaoku\\\\90\\\\turebug90.lua",')
    s = s.replace(cu, them, 1)
    bak(p); wr(p, s)
    print("4. hoatdong_admin.lua: DA THEM 3 file vao HD_RELOAD_LIST")
print("XONG")
