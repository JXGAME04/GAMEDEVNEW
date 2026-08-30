# -*- coding: utf-8 -*-
"""t07_cong_thuong.py - THI CONG DOT 6: CONG TRAO THUONG dung chung.

VAN DE HIEN NAY (do that tren cay script):
  475 cho goi AddItem/AddItemEx rai rac khap 130 tep, moi cho tu kiem tui theo
  kieu rieng: 114 cho dung CalcFreeItemCellCount, 16 cho CountFreeRoomByWH,
  13 cho CheckRoom, 5 cho CheckFreeBoxItem - va RAT NHIEU cho KHONG kiem gi.
  Khong kiem tui = mon thuong roi vao hu khong khi tui day, nguoi choi mat do
  ma khong ai biet vi khong co log.

CONG THUONG (script\\cauhinh\\ch_thuong_lib.lua) lam DUNG BON viec:
  1) Kiem du o trong TRUOC khi trao (CalcFreeItemCellCount - ham 114 cho trong
     du an dang dung; khong de ra ham moi).
  2) Trao tung mon, dem so mon trao duoc.
  3) GHI LOG moi lan trao: hoat dong nao - mon gi - co trao duoc khong.
     Nho do khi nguoi choi bao "khong nhan duoc thuong" thi tra log ra la biet.
  4) Tra ve so mon da trao, de noi goi tu xu ly phan con lai.

!! TEP NAY CHI THEM CONG CU. KHONG sua bat ky cho trao thuong nao dang chay -
   doi cach trao cua 475 cho la viec LON, phai lam tung hoat dong mot va co
   nghiem thu rieng. Chu game quyet dinh chuyen dan tung cho.

!! MA HOA: tieng Viet trong tep .py nay la UNICODE; khi ghi ra .lua thi doi
   sang TCVN3 bang bangtxt.uni2tcvn. TUYET DOI khong go thang byte TCVN3 vao
   tep .py (bay da mac nhieu lan).

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import sys

sys.path.insert(0, os.path.join(r"D:\GAMEDEVNEW\ReverseTools", "viemde"))
from bangtxt import uni2tcvn  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
TM = os.path.join(S, "cauhinh")
TEN = "ch_thuong_lib.lua"
NHAN = "[CONGTHUONG 29/08]"

ND = """-- ============================================================================
-- CH_THUONG_LIB.LUA - CONG TRAO THUONG dung chung
-- %(nhan)s
--
-- !! TEP NAY LA "LA": CAM Include bat cu gi vao day.
--
-- CACH DUNG trong mot hoat dong:
--    Include("\\\\script\\\\cauhinh\\\\ch_lib.lua")
--    Include("\\\\script\\\\cauhinh\\\\ch_thuong.lua")
--    Include("\\\\script\\\\cauhinh\\\\ch_thuong_lib.lua")
--    ...
--    local nDaTrao = G_TraoThuong("TK_THUONG_QUANQUAN", "TONGKIM")
--
-- VI SAO CAN: hien co 475 cho goi AddItem rai rac 130 tep, moi cho tu kiem tui
-- mot kieu (114 CalcFreeItemCellCount, 16 CountFreeRoomByWH, 13 CheckRoom,
-- 5 CheckFreeBoxItem) va nhieu cho KHONG kiem gi - tui day la mat mon thuong
-- ma khong co dau vet nao.
-- ============================================================================

-- Ghi mot dong log thuong. Dung GhiLog (ham C, ghi logs\\hethong.log) neu co;
-- khong thi in ra console de it nhat con thay duoc.
function G_LogThuong(szNoiDung)
\tif (G_CFG ~= nil and G_CFG("CH_LOG_THUONG", 1) == 0) then
\t\treturn
\tend
\tif (GhiLog ~= nil) then
\t\tGhiLog("THUONG", szNoiDung)
\telse
\t\tprint("[THUONG] "..szNoiDung)
\tend
end

-- Dem so mon trong mot bang thuong (moi muc: {soluong, {g,d,p}, "ten"}).
function G_DemMonThuong(tbBang)
\tif (tbBang == nil) then
\t\treturn 0
\tend
\treturn getn(tbBang)
end

-- Trao mot bang thuong cho nguoi choi HIEN TAI.
--   szKhoaBang : ten khoa trong tbCFG_THUONG (tra qua G_CFG)
--   szNguon    : ten hoat dong, chi de ghi log cho de tra
-- Tra ve so mon da trao duoc (0 neu khong trao duoc mon nao).
function G_TraoThuong(szKhoaBang, szNguon)
\tlocal tbBang = G_CFG(szKhoaBang, nil)
\tif (tbBang == nil) then
\t\tG_LogThuong("THIEU BANG THUONG: "..szKhoaBang.." (nguon "..szNguon..")")
\t\treturn 0
\tend

\tlocal nSoMon = getn(tbBang)
\tif (nSoMon <= 0) then
\t\tG_LogThuong("BANG THUONG RONG: "..szKhoaBang)
\t\treturn 0
\tend

\t-- Kiem du o TRUOC khi trao mon dau tien. Thieu o thi bao nguoi choi va
\t-- KHONG trao gi ca - tot hon la trao nua voi roi mat phan con lai.
\tlocal nOTrong = CalcFreeItemCellCount()
\tif (nOTrong < nSoMon) then
\t\tG_LogThuong("TUI DAY: "..szKhoaBang.." can "..nSoMon
\t\t\t.." o, con "..nOTrong.." (nguon "..szNguon..")")
\t\tMsg2Player("H\u00e0nh trang c\u1ea7n tr\u1ed1ng "..nSoMon
\t\t\t.." \u00f4 m\u1edbi nh\u1eadn \u0111\u01b0\u1ee3c ph\u1ea7n"
\t\t\t.." th\u01b0\u1edfng.")
\t\treturn 0
\tend

\tlocal nDaTrao = 0
\tfor i = 1, nSoMon do
\t\tlocal tbMuc = tbBang[i]
\t\tif (tbMuc ~= nil and tbMuc[1] ~= nil and tbMuc[2] ~= nil) then
\t\t\tlocal nSL = tbMuc[1]
\t\t\tlocal tbMa = tbMuc[2]
\t\t\tlocal szTen = tbMuc[3]
\t\t\tif (szTen == nil) then
\t\t\t\tszTen = "?"
\t\t\tend
\t\t\tif (tbMa[1] ~= nil and tbMa[2] ~= nil and tbMa[3] ~= nil) then
\t\t\t\tAddItem(tbMa[1], tbMa[2], tbMa[3], nSL, 0, 0, 0)
\t\t\t\tnDaTrao = nDaTrao + 1
\t\t\t\tG_LogThuong(szNguon.." | "..szKhoaBang.." | "..szTen
\t\t\t\t\t.." ("..tbMa[1]..","..tbMa[2]..","..tbMa[3]..") x"..nSL)
\t\t\telse
\t\t\t\tG_LogThuong("MUC SAI DINH DANG: "..szKhoaBang.." muc "..i)
\t\t\tend
\t\tend
\tend

\treturn nDaTrao
end

-- Trao mot mon le, van co kiem o trong + ghi log. Dung cho cho chi trao 1 mon.
function G_TraoMon(nGenre, nDetail, nParticular, nSoLuong, szTen, szNguon)
\tif (CalcFreeItemCellCount() < 1) then
\t\tG_LogThuong("TUI DAY khi trao "..szTen.." (nguon "..szNguon..")")
\t\tMsg2Player("H\u00e0nh trang \u0111\u00e3 \u0111\u1ea7y, kh\u00f4ng"
\t\t\t.." nh\u1eadn \u0111\u01b0\u1ee3c ph\u1ea7n th\u01b0\u1edfng.")
\t\treturn 0
\tend
\tAddItem(nGenre, nDetail, nParticular, nSoLuong, 0, 0, 0)
\tG_LogThuong(szNguon.." | "..szTen.." ("..nGenre..","..nDetail..","
\t\t..nParticular..") x"..nSoLuong)
\treturn 1
end
""" % dict(nhan=NHAN)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t07_cong_thuong - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    p = os.path.join(TM, TEN)
    if not os.path.isdir(TM):
        print("!!! LOI TO: chua co thu muc %s (chay t05b truoc)" % TM)
        return 1
    if os.path.isfile(p):
        print("  %s DA CO - bo qua (khong ghi de)" % TEN)
        return 0

    nd = uni2tcvn(ND)
    xau = [c for c in nd if ord(c) > 255]
    if xau:
        print("!!! LOI TO: con %d ky tu ngoai bang 1 byte sau khi doi ma: %r"
              % (len(xau), xau[:8]))
        return 1
    n_cao = sum(1 for c in nd if ord(c) > 127)
    print("  %s se tao (%d dong, %d byte tieng Viet TCVN3)"
          % (TEN, nd.count("\n") + 1, n_cao))
    if n_cao == 0:
        print("!!! LOI TO: khong co byte tieng Viet nao - doi ma hong?")
        return 1

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    with io.open(p, "wb") as f:
        f.write(nd.replace("\n", "\r\n").encode("latin-1"))
    print("  DA GHI %s" % p)
    return 0


if __name__ == "__main__":
    sys.exit(main())
