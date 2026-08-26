# -*- coding: utf-8 -*-
"""C49 - NOI he BAC DAU LENH BAI (sinh boi gen_beidou.py) vao may chu:
  1) 4 moc phat lenh bai tu hoat dong (dung y ban Linux beidouactivity.lua)
       Phong Lang Do  <- mission.lua       (OnLanding, doi MANG Truy Cong Lenh)
       Vuot ai        <- award.lua         (OnPassChuanGuan: ai thu 10 va 28)
       Tin Su         <- posthouse.lua     (OnFinishMessenger)
       Boss sat thu   <- lib_killlevel.lua (OnFinishKillerBoss: chi nhom cap 90)
     (Tong Kim: ham HD3_BD_TongKim(nDiem) DA CO san nhung CHUA noi - can tim cho
      ket thuc tran cua JX1; de lai, khong doan bua.)
  2) 2 item script: Bac Dau Chi Bao 6,1,4140 va Huyet Linh Don 6,1,4141
     (bang item JX1 dang de cot Script = 0)
  3) NPC "Bac Dau lao nhan" o 7 thanh, sinh trong hd3_driver
  4) khoa cau hinh trong cauhinh_hoatdong.lua
"""
import io, os, re, sys, shutil

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
INC = 'Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\beidou\\\\bd_activity.lua")'


def sync(rel, goc=SRV):
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(os.path.join(goc, rel), dst)


def doc(rel, goc=SRV):
    return io.open(os.path.join(goc, rel), encoding="latin-1", newline="").read()


def viet(rel, d, goc=SRV):
    io.open(os.path.join(goc, rel), "w", encoding="latin-1", newline="").write(d)
    if goc == SRV:
        sync(rel)


def them_include(d, nl):
    """chen dong Include bd_activity ngay TRUOC dong Include dau tien (hoac dau tep)"""
    if INC in d:
        return d
    m = re.search(r"^Include\(", d, re.M)
    if m:
        return d[:m.start()] + INC + nl + d[m.start():]
    return INC + nl + d


# ============================================================ 1) 4 moc hoat dong
MOC = [
    # (tep, anchor, dong them, ghi chu)
    (r"script\missions\fengling_ferry\mission.lua",
     "\t \tPlayerIndex = tbPlayer[i]",
     ["\t \tPlayerIndex = tbPlayer[i]",
      "\t\t-- [BAC DAU 25/08] Linux beidouactivity.lua:404-420 OnLanding: nguoi choi",
      "\t\t-- PHAI DANG MANG Truy Cong Lenh moi duoc lenh bai Bac Dau (KHONG bi tru).",
      "\t\t-- bShuizeiTask = 1 (khung gio ton phi) -> lenh bai Phong Lang Do cap 2.",
      "\t\tif (HD3_BD_PhongLangDo ~= nil) then",
      "\t\t\tHD3_BD_PhongLangDo(bShuizeiTask)",
      "\t\tend"],
     "Phong Lang Do"),
    (r"script\missions\challengeoftime\award.lua",
     "\t\tCallPlayerFunction(nPlayerIndex, award_batch_contribution, batch)",
     ["\t\tCallPlayerFunction(nPlayerIndex, award_batch_contribution, batch)",
      "\t\t-- [BAC DAU 25/08] Linux beidouactivity.lua:429-448 OnPassChuanGuan:",
      "\t\t-- qua ai thu 10 -> lenh bai Vuot ai cap 1; ai thu 28 -> cap 2.",
      "\t\tif (HD3_BD_VuotAi ~= nil) then",
      "\t\t\tCallPlayerFunction(nPlayerIndex, HD3_BD_VuotAi, batch)",
      "\t\tend"],
     "Vuot ai"),
    (r"script\task\tollgate\killer\lib_killlevel.lua",
     "function HD3_ST_ThuongBoss(nCapNhom, nExpGoc, nSeries)",
     ["function HD3_ST_ThuongBoss(nCapNhom, nExpGoc, nSeries)",
      "\t-- [BAC DAU 25/08] Linux beidouactivity.lua:460-472 OnFinishKillerBoss:",
      "\t-- CHI nhom boss cap 90 moi duoc lenh bai (ham tu loc nCapNhom ~= 90).",
      "\tif (HD3_BD_SatThu ~= nil) then",
      "\t\tHD3_BD_SatThu(nCapNhom)",
      "\tend"],
     "Boss sat thu"),
    (r"script\task\tollgate\messenger\posthouse.lua",
     "function messenger_treasureprize()",
     ["function messenger_treasureprize()",
      "\t-- [BAC DAU 25/08] Linux beidouactivity.lua:450-458 OnFinishMessenger.",
      "\tif (HD3_BD_TinSu ~= nil) then",
      "\t\tHD3_BD_TinSu()",
      "\tend"],
     "Tin Su"),
]
for rel, neo, moi, ten in MOC:
    d = doc(rel)
    if "[BAC DAU 25/08]" in d:
        print("  (da noi) %-14s %s" % (ten, os.path.basename(rel)))
        continue
    if d.count(neo) != 1:
        print("  !! %-14s neo khong khop duy nhat (%d): %s" % (ten, d.count(neo), os.path.basename(rel)))
        continue
    nl = "\r\n" if "\r\n" in d else "\n"
    d = d.replace(neo, nl.join(moi))
    d = them_include(d, nl)
    viet(rel, d)
    print("  OK moc %-14s -> %s" % (ten, rel))

# ================================================== 2) 2 item script vao bang item
DOI = [
    (4140, "\\script\\item\\bd_chibao.lua"),
    (4141, "\\script\\item\\bd_linhdan.lua"),
]
for goc in (SRV, CLI):
    rel = r"settings\item\magicscript.txt"
    p = os.path.join(goc, rel)
    if not os.path.exists(p + ".truoc_beidou_2508"):
        shutil.copyfile(p, p + ".truoc_beidou_2508")
    d = io.open(p, encoding="latin-1", newline="").read()
    dong = d.split("\n")
    n = 0
    for i, l in enumerate(dong):
        c = l.split("\t")
        if len(c) > 10 and c[1] == "6" and c[2] == "1":
            for pid, sc in DOI:
                if c[3] == str(pid) and c[9] != sc:
                    c[9] = sc
                    dong[i] = "\t".join(c)
                    n += 1
    if n:
        io.open(p, "w", encoding="latin-1", newline="").write("\n".join(dong))
    print("  bang item %s: doi %d cot Script" % ("server" if goc == SRV else "client", n))
sync(r"settings\item\magicscript.txt")

# ====================================================== 3) NPC + boot trong driver
rel = r"script\tinhnang\3hoatdong\hd3_driver.lua"
d = doc(rel)
nl = "\r\n" if "\r\n" in d else "\n"
if "HD3_BD_AddNpc" in d:
    print("  (da co) NPC Bac Dau trong driver")
else:
    neo = "\tHD3_PLD_AddBoatNpc()"
    assert d.count(neo) == 1, d.count(neo)
    d = d.replace(neo, nl.join([
        neo,
        "\t-- [BAC DAU 25/08] NPC Bac Dau lao nhan o 7 thanh (he Bac Dau lenh bai).",
        "\tHD3_BD_AddNpc()",
    ]))
    ham = nl.join([
        "",
        "-- ============================================================================",
        "-- BAC DAU LENH BAI - NPC 'Bac Dau lao nhan'. Ban Linux dat NPC qua activitysys",
        "-- config 1046 (DA BI CHU THICH, tuc ban Linux cung khong chay) nen o day tu dat.",
        "-- Moi NPC mang so thu tu thanh (SetNpcValue) de lam nhiem vu di duong.",
        "-- ============================================================================",
        "function HD3_BD_AddNpc()",
        '\tif (HD_CFG("HD3_BD_BAT", 1) ~= 1) then',
        "\t\treturn",
        "\tend",
        '\tInclude("\\\\script\\\\tinhnang\\\\3hoatdong\\\\beidou\\\\bd_activity.lua")',
        "\tif (tbBeidou == nil or tbBeidou.THANH == nil) then",
        "\t\treturn",
        "\tend",
        "\tif (HD3_DelNpcByScript ~= nil) then",
        '\t\tHD3_DelNpcByScript("bd_npc")',
        "\tend",
        "\tlocal n = 0",
        "\tfor i = 1, getn(tbBeidou.THANH) do",
        "\t\tlocal t = tbBeidou.THANH[i]",
        "\t\tlocal nIdx = SubWorldID2Idx(t[2])",
        "\t\tif (nIdx >= 0) then",
        '\t\t\tlocal npc = HD3_AddNpc(108, 1, nIdx, t[3]*32, t[4]*32, 0, "' + V("Bắc Đẩu lão nhân") + '")',
        "\t\t\tif (npc ~= nil and npc > 0) then",
        '\t\t\t\tSetNpcScript(npc, "\\\\script\\\\tinhnang\\\\3hoatdong\\\\beidou\\\\bd_npc.lua")',
        "\t\t\t\tSetNpcValue(npc, i)",
        "\t\t\t\tn = n + 1",
        "\t\t\tend",
        "\t\tend",
        "\tend",
        '\tprint("[3HD] Bac Dau: sinh "..n.." NPC Bac Dau lao nhan.")',
        "end",
        "",
    ])
    neo2 = "-- moi thuyen phu: template 240, script wrapper dat BOATID roi goi fld_wanttakeboat"
    assert d.count(neo2) == 1, d.count(neo2)
    d = d.replace(neo2, ham + neo2)
    viet(rel, d)
    print("  OK them HD3_BD_AddNpc vao hd3_driver.lua")

# ================================================================ 4) cau hinh
rel = r"script\header\cauhinh_hoatdong.lua"
d = doc(rel)
if "HD3_BD_BAT" in d:
    print("  (da co) khoa cau hinh Bac Dau")
else:
    nl = "\r\n" if "\r\n" in d else "\n"
    neo = "HD3_PLD_CAP_TOITHIEU = 1,"
    assert d.count(neo) == 1, d.count(neo)
    d = d.replace(neo, nl.join([
        "-- ==========================================================================",
        "-- BAC DAU LENH BAI (he gom lenh bai tu cac hoat dong roi doi thuong)",
        "-- ==========================================================================",
        "-- 1 = bat he Bac Dau, 0 = tat han (khong sinh NPC, khong phat lenh bai).",
        "HD3_BD_BAT = 1,",
        "",
        "-- Khung gio NPC Bac Dau lam viec (gio may chu). Ban Linux: 8 den 22.",
        "HD3_BD_GIO_MO = 8,",
        "HD3_BD_GIO_DONG = 22,",
        "",
        "-- Cap toi thieu (hoac da trung sinh). Ban Linux: 150.",
        "HD3_BD_CAP_TOITHIEU = 150,",
        "",
        "-- So lan nhan nhiem vu Bac Dau moi ngay. Ban Linux: 30.",
        "HD3_BD_SO_LAN_NGAY = 30,",
        "",
        "-- Kinh nghiem thuong moi nhiem vu di duong. Ban Linux: 8.000.000.",
        "HD3_BD_EXP_NHIEMVU = 8000000,",
        "",
        "-- Han dung cua lenh bai (phut). Ban Linux: 1440 = 1 ngay.",
        "HD3_BD_HAN_LENHBAI = 1440,",
        "",
        "-- So lan dung Bac Dau Huyet Linh Don moi ngay. Ban Linux: 15.",
        "HD3_BD_LINHDAN_NGAY = 15,",
        "",
        "-- 1 = hoat dong phat KEM 1 'Lenh bai Bac Dau' (ma dung de doi thuong 15/20)",
        "--     ben canh lenh bai rieng cua hoat dong.",
        "-- 0 = y het ban Linux: hoat dong CHI phat lenh bai rieng - ma 13 lenh bai rieng",
        "--     do KHONG CHO NAO TIEU nen chi de suu tap.",
        "HD3_BD_QUYDOI_LENHBAI = 1,",
        "",
        neo,
    ]))
    viet(rel, d)
    print("  OK them 9 khoa cau hinh Bac Dau")
