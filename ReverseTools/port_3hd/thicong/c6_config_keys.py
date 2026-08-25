# -*- coding: ascii -*-
"""C6 - cap nhat khoi [6] cua cauhinh_hoatdong.lua:
   - them cac khoa PHAN THUONG (nil = bang goc) + khoa da noi o C5
   - sua nhan sai (A7-N5 [LIVE] -> [RESTART] cho khoa lich; A7-L6 ten ham;
     A7-L7 ghi chu HHMM; 3 khoa cap -> [HIEN THI])
"""
import io, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
rel = os.path.join("script", "header", "cauhinh_hoatdong.lua")
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
nl = "\r\n" if "\r\n" in d else "\n"

if "HD3_ST_THUONG" in d:
    print("da cap nhat roi")
    raise SystemExit

# ---- 1) sua nhan / ghi chu cu ----
fixes = [
    # A7-N5: khoa lich chi nap luc boot vao state driver -> RESTART
    ("-- Gio mo trong ngay (dang HHMM, moi so = 1 luot dua). Ban Linux relay chay\n-- moi gio dung phut :00; day la danh sach cac gio bat. [LIVE]",
     "-- Gio mo trong ngay (dang HHMM, moi so = 1 luot dua). Ban Linux relay chay\n-- MOI GIO dung phut :00 (24 luot/ngay); mac dinh o day 12 gio chan = LECH CO\n-- CHU DICH de nhe server - muon dung 100% Linux thi liet ke du 24 gio. [RESTART]"),
    ("-- Gio bao danh (HHMM) - Linux relay chay moi gio phut :00. [LIVE]",
     "-- Gio bao danh (HHMM) - Linux relay chay moi gio phut :00. [RESTART]"),
    ("-- Gio bang xep hang ngay (trao \"Thien Nien Linh Duoc\", HHMM = 0 -> 00:00). [LIVE]",
     "-- Gio bang xep hang ngay, so sanh dang HHMM (0 = 00:00, 130 = 01:30). [RESTART]"),
    # A7-L6: ten ham sai
    ("hd3_driver HD3_Boot", "hd3_driver HD3_DriverInit"),
    # 3 khoa cap -> HIEN THI (khong co hang Linux tuong ung)
    ("-- Cap toi thieu de nhan nhiem vu (NPC Nhiep Thi Tran o 7 thanh). [LIVE]\nHD3_ST_CAP_TOITHIEU = 90,",
     "-- Cap toi thieu - CHI HIEN THI tren menu admin (ban Linux khong chan cap o\n-- buoc nhan nhiem vu; nhom boss chia theo cap 20..90 san trong killbosshead). [HIEN THI]\nHD3_ST_CAP_TOITHIEU = 90,"),
    ("-- Cap toi thieu len thuyen. [LIVE]\nHD3_PLD_CAP_TOITHIEU = 1,",
     "-- Cap toi thieu - CHI HIEN THI (ban Linux chi doi co mon phai, fld_head.lua:40). [HIEN THI]\nHD3_PLD_CAP_TOITHIEU = 1,"),
    ("-- Cap toi thieu (khong chan khan gia). [LIVE]\nHD3_VA_CAP_TOITHIEU = 50,",
     "-- Cap toi thieu - CHI HIEN THI (gioi han that = tbLevels trong include.lua:\n-- so cap 50-89, cao cap 90+; doi o day KHONG co tac dung). [HIEN THI]\nHD3_VA_CAP_TOITHIEU = 50,"),
    # cac khoa da noi that o C5 -> ghi ro noi noi
    ("-- Tran so lan giet boss / ngay (KILLER_MAXCOUNT ban Linux = 8). [LIVE]",
     "-- Tran so lan giet boss / ngay (goc Linux KILLER_MAXCOUNT = 8; da noi vao\n-- nieshichen.lua). [RESTART]"),
    ("-- Suc chua moi thuyen (GetMSPlayerCount >= n thi day). Ban Linux = 100. [RESTART]",
     "-- Suc chua moi thuyen (fld_haveroom - da noi HD_CFG doc luc chay). [RESTART]"),
    ("-- Cac gio \"ton phi\" (dung Lenh Bai Thuy Tac) dang HHMM - Linux: 10/14/16/18/20h.",
     "-- Cac gio \"ton phi\" (dung Lenh Bai Thuy Tac 6,1,3363) dang HHMM - goc Linux\n-- 10/14/16/18/20h; da noi vao fld_head check_new_shuizeitask."),
    ("-- Phut bao danh (LIMIT_SIGNUP = 10) va phut lam nhiem vu (LIMIT_FINISH = 30). [RESTART]",
     "-- Phut bao danh + phut lam nhiem vu (da noi vao challengeoftime include.lua). [RESTART]"),
]
n = 0
for a, b in fixes:
    a2 = a.replace("\n", nl)
    b2 = b.replace("\n", nl)
    if a2 in d:
        d = d.replace(a2, b2)
        n += 1
    else:
        print("  !! khong khop:", a[:60])
print("sua nhan:", n, "/", len(fixes))

# ---- 2) them khoa moi (truoc phan [ENGINE] cua muc C) ----
anchor = "-- [ENGINE] Khong chinh o day:" + nl + "--   * 2 cap do + ban do: challengeoftime"
assert d.count(anchor) == 1
block = nl.join([
    "-- So luot vao Vuot ai / nguoi / ngay (COUNT_LIMIT goc Linux = 1). [RESTART]",
    "HD3_VA_LUOT_NGAY = 1,",
    "",
    "-- ============ PHAN THUONG (nil = dung bang goc ban Linux) ============",
    "-- Muon doi: chep nguyen bang goc tu tep script neu duoi day vao thay cho nil",
    "-- roi sua so; bang co the chua ca function (giu nguyen cau truc goc).",
    "",
    "-- (A) SAT THU - bang thuong hoan thanh nhom cap 90 (25 dong vat pham +",
    "--     10.000.000 exp). Goc: kill_level.lua:89-118 OnFinishKillerTask. [RESTART]",
    "HD3_ST_THUONG = nil,",
    "",
    "-- (B) PLD - so Thi Gia Chi An (6,1,1095) roi tu boss dau linh 725 (goc 2;",
    "--     bang co Dao Chu tien dai thi tu x2 theo getSignetDropRate). [RESTART]",
    "HD3_PLD_SO_AN_BOSS = 2,",
    "-- (B) PLD - ti le roi Hai Long Chau 6,1,2124 tu boss (goc 0.005 = 0,5%). [RESTART]",
    "HD3_PLD_TILE_HAILONG = 0.005,",
    "-- (B) PLD - ti le roi Truy Cong Lenh 6,1,2024 tu Thuy tac thuong trong gio",
    "--     su kien (goc: nCurRate < 50 tren random(1,100) = 49%). [RESTART]",
    "HD3_PLD_TILE_TRUYCONG = 50,",
    "-- (B) PLD - so Bao Ruong Thuy Tac 6,1,3361 khi cap ben thanh cong. [RESTART]",
    "HD3_PLD_THUONG_CAPBEN = 2,",
    "",
    "-- (C) VUOT AI - bang exp hoan thanh 28 ai (2 cap do, co function tinh theo",
    "--     thoi gian). Goc: award.lua:58-83 tbAward_Success. [RESTART]",
    "HD3_VA_THUONG_HOANTHANH = nil,",
    "-- (C) VUOT AI - thuong hang 1 bang xep hang ngay (goc: 1 Thien Nien Linh Duoc",
    "--     6,1,2125, han 24h). Goc: rank_perday.lua:13. [RESTART]",
    "HD3_VA_THUONG_HANG_NGAY = nil,",
    "-- (C) VUOT AI - bang do trong Bao Ruong Vuot ai (theo loai chia khoa).",
    "--     Goc: chuangguanbaoxiang.lua:21-90 tbCOT_Box_Award. [RESTART]",
    "HD3_VA_THUONG_RUONG = nil,",
    "",
]) + nl
d = d.replace(anchor, block + anchor)
print("da them 8 khoa thuong + 1 khoa luot")

io.open(p, "w", encoding="latin-1", newline="").write(d)
m = os.path.join(MIRROR, rel)
os.makedirs(os.path.dirname(m), exist_ok=True)
io.open(m, "w", encoding="latin-1", newline="").write(d)
print("xong C6")
