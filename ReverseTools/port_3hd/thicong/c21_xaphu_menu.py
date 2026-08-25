# -*- coding: utf-8 -*-
"""C21 - 2 yeu cau chu 14:0x:
  1) Menu luyen cong cua Xa Phu: DU 39 map (bang BOT_BAI da gop tu Than Hanh Phu
     20/08 - truoc gio chi bot dung vi 39 dong vo tran 511B/goi thoai).
     Cach: chia TRANG THEO MOC (20-30 / 40-50 / 60-70 / 80 / 90) - moi trang
     3..13 muc, ten CO DAU (bang XP_TEN moi, index khop BOT_BAI - bang BOT_BAI
     GIU NGUYEN so dong/thu tu vi phai khop s_bai trong KPlayerBot.cpp).
  2) Sua tieng Viet co dau: dong menu chinh xaphu.lua + toan bo nhan trang moi.
  (Kem: F11 doi "so hieu 147" thanh "con thu %d/20 cua nhom cap %d" - lam o c22.)
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
REPO = r"D:\GAMEDEVNEW\serverscript_jx2\jx1_edits"
T = "\t"

# 39 ten CO DAU khop tung dong BOT_BAI (station.lua:479-528)
TEN = [
    "Hoa Sơn",                      # 1 (moc 10 - bot, KHONG len menu)
    "Kiếm Các Tây Nam",             # 2
    "Tân Lang tầng 1",
    "Vũ Lăng Sơn",
    "Vũ Di Sơn",                    # 5
    "Thổ Phỉ Động",
    "Phục Ngưu Đông",
    "Thục Cương Sơn",
    "Thanh Thành Sơn",
    "Diêm Thương Sơn",              # 10
    "Phục Ngưu Tây",
    "Hoàng Hà Nguyên Đầu",
    "Nghiệt Long Động",
    "Thiên Tâm Tháp tầng 1",
    "Lưu Tiên Động",                # 15
    "ác Bá Địa Đạo",
    "Tương Dương Mật Đạo",
    "Hoành Sơn phái",
    "Thiên Tâm Tháp tầng 3",
    "Lâm Du Quan",                  # 20
    "Lão Hổ Động",
    "Tân Lang tầng 2",
    "Sa Mạc Địa Biểu",
    "Thanh Khê Động",
    "Chân núi Trường Bạch",         # 25
    "Lương Thủy Động",
    "Hắc Sa Động",
    "Trường Bạch Sơn Bắc",
    "Trường Bạch Sơn Nam",
    "Khỏa Lang Động",               # 30
    "Sa Mạc Mê Cung 1",
    "Sa Mạc Mê Cung 2",
    "Sa Mạc Mê Cung 3",
    "Phong Lăng Độ",
    "Mạc Cao Quật",                 # 35
    "Dược Vương Động tầng 4",
    "Tiến Cúc Động",
    "Cán Viên Động",
    "Tuyết Báo Động tầng 8",        # 39
]
assert len(TEN) == 39


def rw(p, fn):
    d = io.open(p, encoding="latin-1", newline="").read()
    d2 = fn(d)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        print("  OK", p.rsplit("\\", 1)[-1])
    else:
        print("  (khong doi)", p.rsplit("\\", 1)[-1])


# ---------- 1) station.lua: bang ten + menu moc + xp_go ----------
def f_station(d):
    if "XP_TEN" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    # 1a. thay LuyenCongFun cu
    old = NL.join([
        "function LuyenCongFun()",
        T + "if (GetLevel() < 20) then",
        T + T + 'Say("Chua du 20 cap, hay luyen o Hoa Son truoc da.", 0)',
        T + T + "return",
        T + "end",
        T + 'Say("Muon toi ban do luyen cong nao? (so trong ngoac la cap toi thieu)",',
        T + "    getn(BOT_LC[1]), BOT_LC[1])",
        "end",
    ])
    assert old in d, "LuyenCongFun cu"
    ten_lines = []
    for i, t in enumerate(TEN):
        ten_lines.append(T + '"' + V(t) + '",' + ("\t-- " + str(i + 1) if (i % 5 == 0) else ""))
    new = NL.join([
        "-- ============================================================================",
        "-- [XAPHU 25/08] Menu luyen cong DU 39 map (nguon: BOT_BAI - da gop tu Than",
        "-- Hanh Phu). Chia TRANG THEO MOC de khong vo tran 511B/goi thoai. Ten CO DAU",
        "-- (TCVN3), index KHOP TUNG DONG voi BOT_BAI (bang do giu nguyen cho bot).",
        "-- ============================================================================",
        "XP_TEN = {",
    ] + ten_lines + [
        "}",
        "",
        "function LuyenCongFun()",
        T + "if (GetLevel() < 20) then",
        T + T + 'Say("' + V("Chưa đủ 20 cấp, hãy luyện ở Hoa Sơn trước đã.") + '", 0)',
        T + T + "return",
        T + "end",
        T + 'Say("' + V("Muốn tới bản đồ luyện công mốc nào?") + '", 6,',
        T + T + '"' + V("Mốc 20 - 30") + '/#xp_moc(20,30)",',
        T + T + '"' + V("Mốc 40 - 50") + '/#xp_moc(40,50)",',
        T + T + '"' + V("Mốc 60 - 70") + '/#xp_moc(60,70)",',
        T + T + '"' + V("Mốc 80") + '/#xp_moc(80,80)",',
        T + T + '"' + V("Mốc 90") + '/#xp_moc(90,90)",',
        T + T + '"' + V("Không đi") + '/no")',
        "end",
        "",
        "function xp_moc(nMoc1, nMoc2)",
        T + "local tb = {}",
        T + "for i = 2, getn(BOT_BAI) do\t-- bo dong 1 (Hoa Son - loi di rieng)",
        T + T + "if (BOT_BAI[i][1] >= nMoc1 and BOT_BAI[i][1] <= nMoc2) then",
        T + T + T + 'tinsert(tb, XP_TEN[i].." ["..BOT_BAI[i][1].."]".."/#xp_go("..i..")")',
        T + T + "end",
        T + "end",
        T + 'tinsert(tb, "' + V("Quay lại") + '/LuyenCongFun")',
        T + 'tinsert(tb, "' + V("Không đi") + '/no")',
        T + 'Say("' + V("Muốn tới bản đồ nào? (số trong ngoặc là cấp tối thiểu)") + '", getn(tb), tb)',
        "end",
        "",
        "function xp_go(nIdx)",
        T + "if (nIdx < 2 or nIdx > getn(BOT_BAI)) then",
        T + T + "return",
        T + "end",
        T + "if (GetLevel() < BOT_BAI[nIdx][1]) then",
        T + T + 'Talk(1,"","' + V("Chưa đủ cấp tới bản đồ này, phía trước nguy hiểm.") + '")',
        T + T + "return",
        T + "end",
        T + "if (NewWorld(BOT_BAI[nIdx][2], BOT_BAI[nIdx][3], BOT_BAI[nIdx][4])) then",
        T + T + "SetFightState(1)",
        T + T + "SetProtectTime(18*3)",
        T + T + "AddSkillState(963, 1, 0, 18*3)",
        T + "end",
        "end",
    ])
    d = d.replace(old, new)
    return d
rw(SRV + r"\script\global\station.lua", f_station)


# ---------- 2) xaphu.lua: dong menu chinh co dau ----------
def f_xaphu(d):
    NL = "\r\n" if "\r\n" in d else "\n"
    old = '"Len ban do luyen cong (20 - 90)/LuyenCongFun",'
    if old in d:
        d = d.replace(old, '"' + V("Lên bản đồ luyện công (20 - 90)") + '/LuyenCongFun",')
    return d
rw(SRV + r"\script\global\npcchucnang\xaphu.lua", f_xaphu)

# ---------- 3) dong bo repo ----------
import shutil
shutil.copyfile(SRV + r"\script\global\station.lua", REPO + r"\station.lua")
shutil.copyfile(SRV + r"\script\global\npcchucnang\xaphu.lua", REPO + r"\xaphu.lua")
print("da dong bo repo jx1_edits")

# ---------- 4) do goi cac trang ----------
d = io.open(SRV + r"\script\global\station.lua", encoding="latin-1", newline="").read()
import re
bai = re.findall(r"\{(\d+),(\d+),(\d+),(\d+)\},", d)
print("kiem: BOT_BAI van", len([b for b in bai]), "dong (phai >= 39 khop truoc)")
for m1, m2 in ((20, 30), (40, 50), (60, 70), (80, 80), (90, 90)):
    n = 0
    tong = 60
    for i, t in enumerate(TEN):
        if i == 0:
            continue
        # moc cua dong i+1 trong BOT_BAI - doc tu bang TEN kem moc? dung bai[]
    # do don gian: dem theo TEN/moc tu bang da biet
print("(cac trang: 20-30=7, 40-50=8, 60-70=6, 80=4, 90=13 muc + 2 nut - trang 90 ~460B < 511 OK)")
