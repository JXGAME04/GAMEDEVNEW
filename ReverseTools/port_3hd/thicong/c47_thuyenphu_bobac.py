# -*- coding: utf-8 -*-
"""C47 - chu game: "thuyen phu o bo bac hien tai dang loi no nhu thuyen phu o bo
nam (can fix lai)".

DUNG. LOI CUA TOI o hd3_driver.lua: bang HD3_PLD_BOAT chep DU 6 toa do thuyen phu
tu du an cu roi gan CUNG MOT script len thuyen (hd3_thuyenphu.lua) cho ca 6.

BANG CHUNG 6 toa do do la 3 NAM + 3 BAC:
  script\\tinhnang\\phonglangdo\\lib_phonglangdo.lua:79-85  TAB_NPCCHUCNANG
  script\\tinhnang\\phonglangdo\\lib_phonglangdo.lua:150-156 addnpcphonglangdo():
      muc 1,2,3 -> TAB_NPCCHUCNANG_SCRIPT[1] = thuyenphu.lua      (BO NAM, len thuyen)
      muc 4,5,6 -> TAB_NPCCHUCNANG_SCRIPT[2] = thuyenphubac.lua   (BO BAC, ve nam)
BAN LINUX xac nhan y het: script\\item\\ib\\huichengfu.lua:12-14 chi liet ke BA ben
      {336,1149,3020,"Ben 1"} {336,1280,2909,"Ben 2"} {336,1538,2808,"Ben 3"}
      = dung 3 toa do dau. Ba toa do sau la CHO CAP BEN bo Bac:
      fld_head.lua:15 northMAP_POS = {1158,2964} {1343,2868} {1482,2796} - moi cai
      nam sat mot NPC muc 6 / 4 / 5 (cach 17..26 o).

VA:
 (1) HD3_PLD_BOAT tach thanh 2 bang ro rang: HD3_PLD_BEN_NAM (3 muc, BOATID 1/2/3)
     va HD3_PLD_BEN_BAC (3 muc, khong BOATID).
 (2) Them script rieng hd3_thuyenphubac.lua cho bo Bac: dua nguoi choi TRO VE BO
     NAM (ben 1/2/3 gan nhat theo dung so hieu ben), phi 1000 quan tien - giu
     nguyen MONEY_VETHANH cua du an cu de khong doi kinh te. KHONG kem phan tra
     nhiem vu "Tieu diet thuy tac" doi Truy Cong Lenh cua ban Viet: he Linux dung
     Truy Cong Lenh lam DIEU KIEN MANG THEO cua Bac Dau lenh bai (khong tru), tru
     mat se hong he do.
 (3) hd3_driver.lua don NPC cu / sinh NPC moi theo ca hai bang.

GHI CHU: ban LINUX KHONG he co NPC nao o bo Bac (chi 3 ben). Neu chu game muon
100% Linux thi dat HD3_PLD_BEN_BAC = {} trong cauhinh (khoa HD3_PLD_CO_THUYENPHU_BAC).
"""
import io, os, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"


def ghi(rel, body):
    p = os.path.join(JX1, rel)
    os.makedirs(os.path.dirname(p), exist_ok=True)
    io.open(p, "w", encoding="latin-1", newline="").write(body)
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(p, dst)


def sync(rel):
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(os.path.join(JX1, rel), dst)


# ---------------------------------------------------------------- 1) script bo Bac
L = []
A = L.append
A("-- ============================================================================")
A("-- HD3_THUYENPHUBAC.LUA - thuyen phu BO BAC Phong Lang Do.")
A("-- SINH boi ReverseTools/port_3hd/thicong/c47_thuyenphu_bobac.py - DUNG SUA TAY.")
A("--")
A("-- Ban LINUX KHONG co NPC o bo Bac: huichengfu.lua:12-14 chi biet BA ben")
A("-- (1149,3020) (1280,2909) (1538,2808) = bo NAM, con fld_head.lua:15")
A("-- northMAP_POS chi la CHO CAP BEN khi het gio / chet. Du an cu co NPC bo Bac")
A("-- (thuyenphubac.lua) lam nhiem vu dua khach ve bo Nam - giu lai dung phan do.")
A("-- KHONG giu phan tra nhiem vu doi Truy Cong Lenh cua ban Viet: he Bac Dau cua")
A("-- Linux can nguoi choi MANG THEO Truy Cong Lenh (khong tru) nen tru la hong.")
A("-- ============================================================================")
A('Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")')
A("")
A("-- ben Nam tuong ung (dung so hieu ben) - lay tu huichengfu.lua ban Linux")
A("HD3_PLD_BENNAM_POS = { {1149, 3020}, {1280, 2909}, {1538, 2808} }")
A("HD3_PLD_PHI_VENAM = 1000	-- MONEY_VETHANH cua du an cu, giu nguyen")
A("")
A("function main(NpcIndex)")
A("\tHD3_TPB_BEN = GetNpcValue(NpcIndex)")
A("\tif (HD3_TPB_BEN == nil or HD3_TPB_BEN < 1 or HD3_TPB_BEN > 3) then")
A("\t\tHD3_TPB_BEN = 1")
A("\tend")
A("\tlocal nPhi = HD_CFG(\"HD3_PLD_PHI_VENAM\", HD3_PLD_PHI_VENAM)")
A("\tlocal tb = {")
A('\t\tformat("' + V("Đưa ta trở về bờ Nam (%d quan tiền)") + '/hd3_tpb_venam", nPhi),')
A('\t\t"' + V("Kết thúc đối thoại") + '/hd3_tpb_no",')
A("\t}")
A('\tSay("<color=green>' + V("Thuyền phu") + ':<color> ' + V("đây là bờ Bắc Phong Lăng Độ. Thuyền của ta chỉ đưa khách trở lại bờ Nam thôi.") + '", getn(tb), tb)')
A("end")
A("")
A("function hd3_tpb_venam()")
A("\tlocal nPhi = HD_CFG(\"HD3_PLD_PHI_VENAM\", HD3_PLD_PHI_VENAM)")
A("\tif (GetCash() < nPhi) then")
A('\t\tTalk(1, "", format("' + V("Cần %d quan tiền mới qua sông được, hãy quay lại khi đủ tiền.") + '", nPhi))')
A("\t\treturn")
A("\tend")
A("\tlocal nBen = HD3_TPB_BEN")
A("\tif (nBen == nil or nBen < 1 or nBen > 3) then nBen = 1 end")
A("\tPay(nPhi)")
A("\tNewWorld(336, HD3_PLD_BENNAM_POS[nBen][1], HD3_PLD_BENNAM_POS[nBen][2])")
A("\tSetFightState(1)")
A('\tSetDeathScript("")')
A("\tSetLogoutRV(0)")
A("end")
A("")
A("function hd3_tpb_no()")
A("end")
A("")
ghi(r"script\missions\fengling_ferry\hd3_thuyenphubac.lua", "\r\n".join(L))
print("1) da sinh hd3_thuyenphubac.lua")

# ---------------------------------------------------------------- 2) driver
rel = r"script\tinhnang\3hoatdong\hd3_driver.lua"
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
nl = "\r\n" if "\r\n" in d else "\n"

if "HD3_PLD_BEN_BAC" in d:
    print("2) driver: da va roi")
else:
    cu_bang = nl.join([
        "HD3_PLD_BOAT = {",
        "\t{1147, 3018, 336, 1}, {1280, 2907, 336, 2}, {1535, 2808, 336, 3},",
        "\t{1324, 2886, 336, 1}, {1493, 2809, 336, 2}, {1173, 2981, 336, 3},",
        "}",
    ])
    assert d.count(cu_bang) == 1, "bang HD3_PLD_BOAT khong khop: %d" % d.count(cu_bang)
    moi_bang = nl.join([
        "-- [3HD 25/08 C47] TACH bo NAM / bo BAC. Truoc day ca 6 NPC deu mang script len",
        "-- thuyen nen thuyen phu bo Bac hanh xu y het bo Nam (chu game bao loi).",
        "-- Bo NAM = 3 BEN cua ban Linux (huichengfu.lua:12-14 'Ben 1/2/3').",
        "-- Bo BAC = 3 cho cap ben (fld_head.lua:15 northMAP_POS nam sat 3 diem nay).",
        "HD3_PLD_BEN_NAM = {",
        "\t{1147, 3018, 336, 1}, {1280, 2907, 336, 2}, {1535, 2808, 336, 3},",
        "}",
        "-- Ban Linux KHONG co NPC o bo Bac. Giu lai theo du an cu cho tien duong ve;",
        "-- muon dung 100% Linux thi dat HD3_PLD_CO_THUYENPHU_BAC = 0 trong cauhinh.",
        "HD3_PLD_BEN_BAC = {",
        "\t{1324, 2886, 336, 2}, {1493, 2809, 336, 3}, {1173, 2981, 336, 1},",
        "}",
    ])
    d = d.replace(cu_bang, moi_bang)

    cu_ham = nl.join([
        "function HD3_PLD_AddBoatNpc()",
        "\tfor i = 1, getn(HD3_PLD_BOAT) do",
        "\t\tlocal t = HD3_PLD_BOAT[i]",
        "\t\tlocal nIdx = SubWorldID2Idx(t[3])",
        "\t\tif (nIdx >= 0) then",
        '\t\t\tlocal npc = HD3_AddNpc(240, 1, nIdx, t[1]*32, t[2]*32, 1, "' + V("Thuyền phu") + '")',
        "\t\t\tif (npc ~= nil and npc > 0) then",
        '\t\t\t\tSetNpcScript(npc, "\\\\script\\\\missions\\\\fengling_ferry\\\\hd3_thuyenphu.lua")',
        "\t\t\t\tSetNpcValue(npc, t[4])   -- BOATID 1/2/3",
        "\t\t\tend",
        "\t\tend",
        "\tend",
        "end",
    ])
    assert d.count(cu_ham) == 1, "ham HD3_PLD_AddBoatNpc khong khop: %d" % d.count(cu_ham)
    moi_ham = nl.join([
        "-- [3HD 25/08 C47] sinh rieng tung bo: bo Nam len thuyen, bo Bac ve Nam.",
        "function HD3_PLD_AddBoatNpcTab(tb, szScript)",
        "\tfor i = 1, getn(tb) do",
        "\t\tlocal t = tb[i]",
        "\t\tlocal nIdx = SubWorldID2Idx(t[3])",
        "\t\tif (nIdx >= 0) then",
        '\t\t\tlocal npc = HD3_AddNpc(240, 1, nIdx, t[1]*32, t[2]*32, 1, "' + V("Thuyền phu") + '")',
        "\t\t\tif (npc ~= nil and npc > 0) then",
        "\t\t\t\tSetNpcScript(npc, szScript)",
        "\t\t\t\tSetNpcValue(npc, t[4])   -- bo Nam: BOATID 1/2/3; bo Bac: so hieu ben ve",
        "\t\t\tend",
        "\t\tend",
        "\tend",
        "end",
        "",
        "function HD3_PLD_AddBoatNpc()",
        '\tHD3_PLD_AddBoatNpcTab(HD3_PLD_BEN_NAM, "\\\\script\\\\missions\\\\fengling_ferry\\\\hd3_thuyenphu.lua")',
        '\tif (HD_CFG("HD3_PLD_CO_THUYENPHU_BAC", 1) == 1) then',
        '\t\tHD3_PLD_AddBoatNpcTab(HD3_PLD_BEN_BAC, "\\\\script\\\\missions\\\\fengling_ferry\\\\hd3_thuyenphubac.lua")',
        "\tend",
        "end",
    ])
    d = d.replace(cu_ham, moi_ham)

    # KHONG can them dong xoa NPC: HD3_DelNpcByScript dung strstr (KJx2WarInfra.cpp)
    # nen chuoi "hd3_thuyenphu" DA khop ca "hd3_thuyenphubac.lua"; tham so loai tru
    # cua HD3_DelNpcByNameEx cung la strstr. Hai dong don NPC san co van dung.

    io.open(p, "w", encoding="latin-1", newline="").write(d)
    sync(rel)
    print("2) da tach bang + ham sinh NPC trong hd3_driver.lua")

# ---------------------------------------------------------------- 3) cauhinh
rel = r"script\header\cauhinh_hoatdong.lua"
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
if "HD3_PLD_CO_THUYENPHU_BAC" in d:
    print("3) cauhinh: da co khoa")
else:
    nl = "\r\n" if "\r\n" in d else "\n"
    neo = "HD3_PLD_SO_AN_BOSS = 2,"
    assert d.count(neo) == 1, d.count(neo)
    d = d.replace(neo, nl.join([
        "-- Co sinh thuyen phu o BO BAC khong. Ban Linux KHONG co (chi 3 ben bo Nam);",
        "-- 1 = giu nhu du an cu (co NPC dua ve bo Nam), 0 = dung y ban Linux.",
        "HD3_PLD_CO_THUYENPHU_BAC = 1,",
        "",
        "-- Phi qua song cua thuyen phu bo Bac (quan tien). Du an cu: 1000.",
        "HD3_PLD_PHI_VENAM = 1000,",
        "",
        neo,
    ]))
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    sync(rel)
    print("3) da them 2 khoa cauhinh (HD3_PLD_CO_THUYENPHU_BAC / HD3_PLD_PHI_VENAM)")
