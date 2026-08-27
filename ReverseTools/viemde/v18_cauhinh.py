# -*- coding: utf-8 -*-
"""VIEM DE - buoc 8a: them khoi cau hinh [7] vao script\\header\\cauhinh_hoatdong.lua.

Moi so mac dinh = DUNG SO CUA BAN LINUX (luat muc 4 cua HUONGDAN_DICHNGUOC):
  gio chay      : gateway\\s3relay\\relaysetting\\task\\baotangviemde\\*.lua
                  -> TaskTime(8,25) (10,25) (14,25) (16,25) (18,25) (20,25) (22,25)
  cap toi thieu : head.lua:48  YDBZ_LIMIT_PLAYER_LEVEL = 120
  lan / tuan    : head.lua:49  YDBZ_LIMIT_WEEK_COUNT   = 10
  lan / ngay    : head.lua:50  YDBZ_LIMIT_DAY_COUNT    = 4
  so nguoi doi  : readymap\\include.lua:25,26  4 .. 6
  so doi toi da : head.lua:47  YDBZ_LIMIT_TEAMS_COUNT  = 15
  phut bao danh : ready.lua:38 nReadyTime = 5*60
  phut mot tran : head.lua:42  YDBZ_LIMIT_FINISH = 30*60

LUAT CUA TEP CAU HINH: "FILE NAY LA LA - CAM Include bat cu gi vao day"
(bai hoc vong Include 23/08 lam GameServer boot stack overflow). Bo va nay
CHI them cac dong 'KHOA = so,' vao trong bang tbCHD.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\header\cauhinh_hoatdong.lua"
BAK = ".truoc_viemde_2608"
NHAN = "[7] VIEM DE BAO TANG"

KHOI = """
-- ===========================================================================
-- [7] VIEM DE BAO TANG (yandibaozang) - tien to YDBZ_        (port 26/08)
-- Vuot ai theo to doi, 3 duong x 10 ai, roi tranh doat, boss cuoi Luong Mi Nhi.
-- Moi so duoi = DUNG SO CUA BAN LINUX (nguon ghi ben canh tung dong).
-- ===========================================================================

-- Bat/tat toan bo tinh nang. 0 = tat han (driver khong lam gi). [RESTART]
YDBZ_BAT = 1,

-- Gio mo bao danh, dang HHMM cach nhau bang dau phay.
-- Goc Linux: gateway\\s3relay\\relaysetting\\task\\baotangviemde\\*.lua
--   TaskTime(8,25) (10,25) (14,25) (16,25) (18,25) (20,25) (22,25). [LIVE]
YDBZ_GIO = "0825,1025,1425,1625,1825,2025,2225",

-- Cap toi thieu de bao danh. Goc: head.lua:48 = 120. [LIVE]
YDBZ_CAP_TOITHIEU = 120,

-- Tran so lan tham gia trong TUAN / trong NGAY. Goc: head.lua:49 = 10, :50 = 4.
-- Mang Viem De Lenh thi duoc mien tran (dung y ban goc). [LIVE]
YDBZ_LAN_TUAN = 10,
YDBZ_LAN_NGAY = 4,

-- So nguoi trong to doi: it nhat / nhieu nhat. Goc: readymap\\include.lua:25,26. [LIVE]
YDBZ_DOI_MIN = 4,
YDBZ_DOI_MAX = 6,

-- So to doi (= so phong cho) toi da moi dot. Goc: head.lua:47 = 15.
-- !! Doi so nay PHAI khai them ban do vao MapList.ini + WorldSet_GameServer.ini
--    (be hien tai: 1003-1017). Xem ReverseTools\\viemde\\v02_khai_bando.py. [RESTART]
YDBZ_PHONG_TOIDA = 15,

-- Thoi gian bao danh (phut). Goc: ready.lua:38 nReadyTime = 5*60. [RESTART]
YDBZ_PHUT_BAODANH = 5,

-- Thoi han mot tran (phut). Goc: head.lua:42 YDBZ_LIMIT_FINISH = 30*60. [LIVE]
YDBZ_PHUT_TRAN = 30,

-- Ban do dat 4 NPC "Binh Binh co nuong" va toa do.
-- Goc: server1\\script\\global\\autoexec_npc.lua:141-144 (map 37 = Bien Kinh,
-- NPC template 1320 - JX1 DA CO san trong npcs.txt). [RESTART]
YDBZ_NPC_MAP = 37,
"""


def main():
    d = io.open(P, encoding="latin-1", newline="").read()
    if NHAN in d:
        print("da them roi - bo qua.")
        return
    # TU DO kieu xuong dong cua tep (tep nay dung LF, khac cac bang .txt dung CRLF).
    # Luat: GIU NGUYEN quy uoc cua tep goc.
    raw = open(P, "rb").read()
    nl = "\r\n" if raw.count(b"\r\n") > 0 else "\n"
    neo = nl + "}" + nl
    n = d.count(neo)
    if n < 1:
        print("!! khong tim thay dau dong bang tbCHD - DUNG LAI")
        return
    vt = d.index(neo)          # dau dong '}' DAU TIEN o cot 0 = ket bang tbCHD
    khoi = KHOI.replace("\r\n", "\n")
    if nl == "\r\n":
        khoi = khoi.replace("\n", "\r\n")
    d2 = d[:vt] + nl + khoi + d[vt:]
    if not os.path.isfile(P + BAK):
        shutil.copyfile(P, P + BAK)
    tam = P + ".dangghi"
    with open(tam, "wb") as f:
        f.write(d2.encode("latin-1"))
    os.replace(tam, P)

    a = open(P + BAK, "rb").read()
    b = open(P, "rb").read()
    print("da them khoi [7]: %d -> %d byte" % (len(a), len(b)))
    print("KIEM BYTE: CRLF %d->%d  LFdon %d->%d  FFFD=%d"
          % (a.count(b"\r\n"), b.count(b"\r\n"),
             a.count(b"\n") - a.count(b"\r\n"), b.count(b"\n") - b.count(b"\r\n"),
             b.count(b"\xef\xbf\xbd")))
    # Tep nay dung LF. Luat: giu nguyen quy uoc cua tep goc, KHONG ep ve CRLF.
    crlf_goc = a.count(b"\r\n") > 0
    assert (b.count(b"\r\n") > 0) == crlf_goc, "kieu xuong dong bi doi"
    print("   kieu xuong dong: %s (giu nguyen)" % ("CRLF" if crlf_goc else "LF"))
    # kiem: khong duoc co Include moi
    assert d2.count("Include(") == d.count("Include("), "da lo them Include vao tep LA!"
    print("   so lenh Include: %d (khong doi) - dung luat 'tep la'" % d2.count("Include("))


main()
