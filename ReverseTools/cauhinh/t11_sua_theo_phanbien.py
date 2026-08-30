# -*- coding: utf-8 -*-
"""t11_sua_theo_phanbien.py - SUA 7 loi ma bo phan bien doi khang bat duoc.

MAY LA `ch_thuong_lib.lua` CHUA CO NOI NAO GOI (no la cong cu vua tao trong
dot nay), nen chua ai bi thiet hai. Nhung phai sua truoc khi co nguoi dung.

LOI NANG 1 - GOI SAI CHU KY AddItem (da TU DOC ScriptFuns.cpp:4922-4994 de
xac nhan, khong nghe lai):
    int LuaAddItem(...)  /* AddItem(nItemClass, nDetailType, nParticualrType,
                            nLevel, nSeries, nLuck, nItemLevel..6) */
    :4942  nLevel    = tham so 4      <- KHONG PHAI so luong!
    :4959  nStackNum chi doc khi nParamNum >= 15, o THAM SO 13
    :4946  neu duoi 15 tham so thi nStackNum = 1
  Ban cu viet `AddItem(g, d, p, nSL, 0,0,0)` => nguoi choi nhan DUNG 1 mon, va
  mon do bi gan CAP = nSL. Vi du {10, {6,1,4847}} ra 1 mon cap 10 thay vi 10 mon.
  Khuon dung cua du an: global\\vatpham.lua:38
    AddItem(g, d, p, 0,0,0,0, 0,0,0,0,0, soLuong, 0, 0)   -- 15 tham so

LOI NANG 2 - BO QUA GIA TRI TRA VE. LuaAddItem tra 0 khi that bai o BA cho:
  :4928 khong co nguoi choi | :4935 duoi 7 tham so | :4992 AddItemSet2 <= 0
  (het khe item, ma vat pham khong co trong magicscript...). Ban cu van dem la
  "da trao" va van ghi log => log noi doi, dung cai ma no sinh ra de chong.

LOI VUA 3 - `getn(x)` nem loi neu x khong phai bang. G_CFG tra cuu mot khong
  gian ten PHANG, nen go nham ten khoa co the tra ve mot con SO roi lam sap
  hoat dong. Them kiem type().

LOI VUA 4 - noi chuoi voi nil lam DUT hoat dong, dung cai ma chu thich hua la
  "khong bao gio nem loi". Chuan hoa moi tham so chuoi truoc khi noi.

LOI VUA 5 - CalcFreeItemCellCount() goi khong tham so chi DEM O TRONG 1x1 RAI
  RAC (ScriptFuns.cpp:5981 -> KInventory::FindFreeCell(1,1)); no KHONG bao dam
  mon 1x3 hay 2x3 nhet vua. Nen bo cau cam ket "thieu o thi khong trao gi ca"
  cho khoi hua sai, va noi ro trong chu thich.

LOI VUA 6 - ch_all.lua tu xung "nap CA BO" nhung thieu ch_thuong_lib.lua =>
  ai lam dung quang cao roi goi G_TraoThuong se dut ngay luc trao thuong.

LOI VUA 7 - G_CFG va HD_CFG doc KHAC THU TU tren cung khong gian khoa. Ghi ro
  luat: 5 tien to cu (TW_ BR_ BW_ TC_ YDBZ_) van thuoc tbCHD, cam khai lai.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

sys.path.insert(0, os.path.join(r"D:\GAMEDEVNEW\ReverseTools", "viemde"))
from bangtxt import uni2tcvn  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

TM = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
      r"\script\cauhinh")
NHAN = "[PHANBIEN 29/08]"

ND_THUONG = """-- ============================================================================
-- CH_THUONG_LIB.LUA - CONG TRAO THUONG dung chung
-- [CONGTHUONG 29/08] + %(nhan)s (sua 5 loi bo phan bien bat duoc)
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
-- DINH DANG MOT MUC THUONG:
--    { <so luong>, {<genre>, <detail>, <particular>}, "<ten>", <cap do> }
--    - <cap do> tuy chon, khong ghi thi 0. Chi trang bi moi can.
--    - Mon KHONG XEP CHONG DUOC (trang bi) thi phai khai TUNG DONG mot, moi
--      dong so luong 1 - vi so luong duoc truyen vao o "stack" cua engine.
--
-- VI SAO CAN CONG NAY: hien co 475 cho goi AddItem rai rac 130 tep, moi cho tu
-- kiem tui mot kieu (114 CalcFreeItemCellCount, 16 CountFreeRoomByWH, 13
-- CheckRoom, 5 CheckFreeBoxItem) va nhieu cho KHONG kiem gi.
-- ============================================================================

-- Doi mot gia tri bat ky thanh chuoi an toan de noi. %(nhan)s
-- (noi chuoi voi nil lam DUT ca hoat dong - Lua 4 nem loi o luaV_strconc)
function G_Chuoi(x, macdinh)
\tif (x == nil) then
\t\tif (macdinh == nil) then
\t\t\treturn "?"
\t\tend
\t\treturn macdinh
\tend
\tif (type(x) == "string") then
\t\treturn x
\tend
\tif (type(x) == "number") then
\t\treturn ""..x
\tend
\treturn "?"
end

-- Ghi mot dong log thuong. Dung GhiLog (ham C, ghi logs\\hethong.log) neu co;
-- khong thi in ra console de it nhat con thay duoc.
function G_LogThuong(szNoiDung)
\tif (G_CFG ~= nil and G_CFG("CH_LOG_THUONG", 1) == 0) then
\t\treturn
\tend
\tlocal sz = G_Chuoi(szNoiDung, "(rong)")
\tif (GhiLog ~= nil) then
\t\tGhiLog("THUONG", sz)
\telse
\t\tprint("[THUONG] "..sz)
\tend
end

-- Dem so mon trong mot bang thuong. Tra 0 neu khong phai bang.
function G_DemMonThuong(tbBang)
\tif (tbBang == nil or type(tbBang) ~= "table") then
\t\treturn 0
\tend
\treturn getn(tbBang)
end

-- Trao mot mon. Tra 1 neu vao duoc tui, 0 neu khong.
--   %(nhan)s AddItem cua du an co chu ky:
--     AddItem(genre, detail, particular, nLevel, nSeries, nLuck, nItemLevel0,
--             nItemLevel1..5, nStackNum, nEnChance, nPoint)
--   THAM SO 4 LA CAP DO, KHONG PHAI SO LUONG. So luong nam o THAM SO 13 va chi
--   duoc doc khi co DU 15 tham so (ScriptFuns.cpp:4959-4966). Khuon nay chep
--   theo global\\vatpham.lua:38.
function G_TraoMotMon(nGenre, nDetail, nParticular, nSoLuong, nCapDo, szTen,
\t\tszNguon)
\tlocal nSL = nSoLuong
\tif (nSL == nil or nSL < 1) then
\t\tnSL = 1
\tend
\tlocal nCap = nCapDo
\tif (nCap == nil) then
\t\tnCap = 0
\tend
\tlocal szT = G_Chuoi(szTen)
\tlocal szN = G_Chuoi(szNguon, "?")

\tlocal nIdx = AddItem(nGenre, nDetail, nParticular, nCap, 0, 0, 0,
\t\t0, 0, 0, 0, 0, nSL, 0, 0)

\t-- %(nhan)s AddItem tra 0 khi that bai (khong co nguoi choi / thieu tham so /
\t-- AddItemSet2 that bai vi het khe item hoac ma vat pham khong co trong
\t-- magicscript). Truoc day bo qua tri tra ve nen log bao "da trao" ca khi
\t-- nguoi choi khong nhan duoc gi.
\tif (nIdx == nil or nIdx <= 0) then
\t\tG_LogThuong("TRAO THAT BAI: "..szN.." | "..szT.." ("..nGenre..","
\t\t\t..nDetail..","..nParticular..") x"..nSL)
\t\treturn 0
\tend
\tG_LogThuong(szN.." | "..szT.." ("..nGenre..","..nDetail..","
\t\t..nParticular..") x"..nSL)
\treturn 1
end

-- Trao mot bang thuong cho nguoi choi HIEN TAI.
--   szKhoaBang : ten khoa trong tbCFG_THUONG (tra qua G_CFG)
--   szNguon    : ten hoat dong, de ghi log cho de tra
-- Tra ve so MUC da trao duoc (0 neu khong trao duoc gi).
function G_TraoThuong(szKhoaBang, szNguon)
\tlocal szKhoa = G_Chuoi(szKhoaBang, "(khong ten)")
\tlocal szN = G_Chuoi(szNguon, "?")
\tlocal tbBang = G_CFG(szKhoa, nil)

\tif (tbBang == nil) then
\t\tG_LogThuong("THIEU BANG THUONG: "..szKhoa.." (nguon "..szN..")")
\t\treturn 0
\tend
\t-- %(nhan)s G_CFG tra cuu MOT khong gian ten phang, nen go nham ten khoa co
\t-- the tra ve mot con SO. getn(so) nem loi lam sap ca hoat dong.
\tif (type(tbBang) ~= "table") then
\t\tG_LogThuong("KHOA KHONG PHAI BANG THUONG: "..szKhoa.." (nguon "..szN..")")
\t\treturn 0
\tend

\tlocal nSoMuc = getn(tbBang)
\tif (nSoMuc <= 0) then
\t\tG_LogThuong("BANG THUONG RONG: "..szKhoa)
\t\treturn 0
\tend

\t-- %(nhan)s Kiem so o trong TRUOC khi trao. Luu y that: goi khong tham so
\t-- thi CalcFreeItemCellCount chi DEM SO O 1x1 CON TRONG, RAI RAC
\t-- (ScriptFuns.cpp:5981 -> KInventory::FindFreeCell(1,1)). No KHONG bao dam
\t-- mon to (1x3, 2x3) nhet vua. Vi vay day chi la RAO SO BO de tranh truong
\t-- hop tui day han - khong hua chac chan.
\tlocal nOTrong = CalcFreeItemCellCount()
\tif (nOTrong ~= nil and nOTrong < nSoMuc) then
\t\tG_LogThuong("TUI DAY: "..szKhoa.." can "..nSoMuc.." o, con "..nOTrong
\t\t\t.." (nguon "..szN..")")
\t\tMsg2Player("H\u00e0nh trang c\u1ea7n tr\u1ed1ng "..nSoMuc
\t\t\t.." \u00f4 m\u1edbi nh\u1eadn \u0111\u01b0\u1ee3c ph\u1ea7n"
\t\t\t.." th\u01b0\u1edfng.")
\t\treturn 0
\tend

\tlocal nDaTrao = 0
\tfor i = 1, nSoMuc do
\t\tlocal tbMuc = tbBang[i]
\t\tif (tbMuc ~= nil and type(tbMuc) == "table") then
\t\t\tlocal tbMa = tbMuc[2]
\t\t\tif (tbMa ~= nil and type(tbMa) == "table" and tbMa[1] ~= nil
\t\t\t\tand tbMa[2] ~= nil and tbMa[3] ~= nil) then
\t\t\t\tnDaTrao = nDaTrao + G_TraoMotMon(tbMa[1], tbMa[2], tbMa[3],
\t\t\t\t\ttbMuc[1], tbMuc[4], tbMuc[3], szN.." | "..szKhoa)
\t\t\telse
\t\t\t\tG_LogThuong("MUC SAI DINH DANG: "..szKhoa.." muc "..i)
\t\t\tend
\t\telse
\t\t\tG_LogThuong("MUC SAI DINH DANG: "..szKhoa.." muc "..i)
\t\tend
\tend

\treturn nDaTrao
end

-- Trao mot mon le, van co rao so bo + ghi log. Dung cho cho chi trao 1 mon.
function G_TraoMon(nGenre, nDetail, nParticular, nSoLuong, szTen, szNguon)
\tlocal szN = G_Chuoi(szNguon, "?")
\tlocal nOTrong = CalcFreeItemCellCount()
\tif (nOTrong ~= nil and nOTrong < 1) then
\t\tG_LogThuong("TUI DAY khi trao "..G_Chuoi(szTen).." (nguon "..szN..")")
\t\tMsg2Player("H\u00e0nh trang \u0111\u00e3 \u0111\u1ea7y, kh\u00f4ng"
\t\t\t.." nh\u1eadn \u0111\u01b0\u1ee3c ph\u1ea7n th\u01b0\u1edfng.")
\t\treturn 0
\tend
\treturn G_TraoMotMon(nGenre, nDetail, nParticular, nSoLuong, 0, szTen, szN)
end
""" % dict(nhan=NHAN)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi_tep(p, nd_uni, ghi):
    nd = uni2tcvn(nd_uni)
    xau = [c for c in nd if ord(c) > 255]
    if xau:
        print("!!! LOI TO: %s con ky tu ngoai bang 1 byte: %r"
              % (os.path.basename(p), xau[:6]))
        return False
    if not ghi:
        print("  %-20s se ghi (%d dong)" % (os.path.basename(p),
                                            nd.count("\n") + 1))
        return True
    sao = p + ".truoc_phanbien"
    if os.path.isfile(p) and not os.path.isfile(sao):
        shutil.copy2(p, sao)
    with io.open(p, "wb") as f:
        f.write(nd.replace("\n", "\r\n").encode("latin-1"))
    print("  DA GHI %s" % os.path.basename(p))
    return True


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t11_sua_theo_phanbien - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))

    p_thuong = os.path.join(TM, "ch_thuong_lib.lua")
    if NHAN in doc(p_thuong):
        print("  ch_thuong_lib.lua DA VA - bo qua")
    elif not ghi_tep(p_thuong, ND_THUONG, ghi):
        return 1

    # ---- ch_all.lua: them ch_thuong_lib ----
    p_all = os.path.join(TM, "ch_all.lua")
    raw = doc(p_all)
    if NHAN in raw:
        print("  ch_all.lua DA VA - bo qua")
    else:
        moc = 'Include("\\\\script\\\\cauhinh\\\\ch_thuong.lua")'
        if raw.count(moc) != 1:
            print("!!! LOI TO: ch_all.lua khong co dung 1 dong Include ch_thuong")
            return 1
        moi = (moc + "\r\n"
               + "-- " + NHAN + " thieu dong nay thi ai lam dung quang cao"
               + " \"nap CA BO\"\r\n"
               + "-- roi goi G_TraoThuong se dut ngay luc trao thuong.\r\n"
               + 'Include("\\\\script\\\\cauhinh\\\\ch_thuong_lib.lua")')
        nd = raw.replace(moc, moi)
        if not ghi:
            print("  ch_all.lua           se them Include ch_thuong_lib")
        else:
            sao = p_all + ".truoc_phanbien"
            if not os.path.isfile(sao):
                shutil.copy2(p_all, sao)
            with io.open(p_all, "wb") as f:
                f.write(nd.encode("latin-1"))
            print("  DA GHI ch_all.lua")

    # ---- ch_lib.lua: chuan hoa chuoi + ghi ro luat khong gian khoa ----
    p_lib = os.path.join(TM, "ch_lib.lua")
    raw_l = doc(p_lib)
    if NHAN in raw_l:
        print("  ch_lib.lua DA VA - bo qua")
    else:
        cu = ('function G_CFG_BUOC(szKhoa, macdinh)\r\n'
              '\tlocal ra = G_CFG(szKhoa, nil)\r\n'
              '\tif (ra == nil) then\r\n'
              '\t\tif (GhiLog ~= nil) then\r\n'
              '\t\t\tGhiLog("CAUHINH", "thieu khoa: "..szKhoa)\r\n'
              '\t\telse\r\n'
              '\t\t\tprint("[CAUHINH] thieu khoa: "..szKhoa)\r\n'
              '\t\tend\r\n'
              '\t\treturn macdinh\r\n'
              '\tend\r\n'
              '\treturn ra\r\n'
              'end')
        if raw_l.count(cu) != 1:
            print("!!! LOI TO: ch_lib.lua khong khop khoi G_CFG_BUOC")
            return 1
        moi = ('function G_CFG_BUOC(szKhoa, macdinh)\r\n'
               '\t-- ' + NHAN + ' szKhoa co the la nil (ten khoa lay tu bien'
               ' chua gan);\r\n'
               '\t-- noi chuoi voi nil lam DUT ca hoat dong, dung cai ma ham'
               ' nay hua la\r\n'
               '\t-- khong bao gio nem loi.\r\n'
               '\tlocal szK = szKhoa\r\n'
               '\tif (szK == nil or type(szK) ~= "string") then\r\n'
               '\t\tszK = "(khong ten)"\r\n'
               '\tend\r\n'
               '\tlocal ra = G_CFG(szKhoa, nil)\r\n'
               '\tif (ra == nil) then\r\n'
               '\t\tif (GhiLog ~= nil) then\r\n'
               '\t\t\tGhiLog("CAUHINH", "thieu khoa: "..szK)\r\n'
               '\t\telse\r\n'
               '\t\t\tprint("[CAUHINH] thieu khoa: "..szK)\r\n'
               '\t\tend\r\n'
               '\t\treturn macdinh\r\n'
               '\tend\r\n'
               '\treturn ra\r\n'
               'end')
        nd_l = raw_l.replace(cu, moi)

        # them luat khong gian khoa ngay truoc G_CFG
        moc2 = "function G_CFG(szKhoa, macdinh)"
        luat = ("-- " + NHAN + " LUAT KHONG GIAN KHOA (quan trong):\r\n"
                "--   Du an da co san ham HD_CFG doc bang tbCHD"
                " (header\\cauhinh_hoatdong.lua).\r\n"
                "--   HD_CFG CHI doc tbCHD, con G_CFG doc cac bang moi TRUOC roi"
                " moi den tbCHD.\r\n"
                "--   => Neu mot khoa duoc khai o CA HAI noi thi hai ham cho HAI"
                " gia tri khac\r\n"
                "--   nhau tuy cho goi. Vi vay: 5 tien to cu TW_ BR_ BW_ TC_"
                " YDBZ_ VAN THUOC\r\n"
                "--   tbCHD - CAM khai lai chung trong cac bang tbCFG_*.\r\n"
                + moc2)
        if nd_l.count(moc2) != 1:
            print("!!! LOI TO: ch_lib.lua khong co dung 1 dong khai G_CFG")
            return 1
        nd_l = nd_l.replace(moc2, luat)

        if not ghi:
            print("  ch_lib.lua           se chuan hoa chuoi + ghi luat khoa")
        else:
            sao = p_lib + ".truoc_phanbien"
            if not os.path.isfile(sao):
                shutil.copy2(p_lib, sao)
            with io.open(p_lib, "wb") as f:
                f.write(nd_l.encode("latin-1"))
            print("  DA GHI ch_lib.lua")

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
