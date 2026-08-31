# -*- coding: utf-8 -*-
"""t40_go_he_cu_dot4.py - DOT 4: go not cac moc con lai cua 4 he cu.

Sau DOT 3, chuoi cu KHONG con ai khoi dong:
  - 4 ham sukien_* da go khoi timerserver.lua
  - grep OpenMission: khong con noi nao mo mission 2/3/4/6
  - grep StartMissionTimer(MS_ cu): chi con LOI GOI NAM TRONG CHINH chuoi chet

Con lai hai loai moc:

(1) TEP CHET HOAN TOAN -> rut ve KHUNG RONG (khong xoa tep).
    Vi sao khong xoa: engine nap missionNN.lua / taskNN.lua theo SO HIEU. Giu tep
    rong thi so hieu van hop le, khong the sinh loi 'khong mo duoc tep'.
    Khuon khung rong CHEP y ban co san trong cay: missions\\mission07.lua va
    mission09.lua (dot port JX2 da lam dung kieu nay).
      missions\\mission02.lua  Cong Thanh cu     -> ban thay: citywar_city JX2
      missions\\mission03.lua  Vuot Ai cu        -> ban thay: challengeoftime
      missions\\mission04.lua  Phong Lang Do cu  -> ban thay: fengling_ferry
      missions\\mission06.lua  Loi Dai bang hoi  -> ban thay: citywar_arena JX2
      timertask\\task04.lua    hen gio PLD cu
      timertask\\task05.lua    hen gio PLD cu
      timertask\\task06.lua    hen gio Loi Dai + Vuot Ai cu
      timertask\\task07.lua    hen gio Loi Dai cu
    Ban goc duoc chep sang _dara truoc khi rut gon.

(2) TEP CON SONG (Tong Kim dung chung) -> go RIENG nhanh chet.
      timertask\\task01.lua  giu ontime_tongkim; go ontime_congthanh + ontime_vuotai
      timertask\\task02.lua  giu ontime_tongkim; go ontime_congthanh + ontime_vuotai
      timertask\\task10.lua  giu ontime_tongkimketthuc; go 2 ham PLD/VA
      item\\longxuewan.lua    noi tuyen hang MAX_NUM_LONGHH_DAY roi bo Include

Moi tep deu duoc kiem lai bang syncheck.exe (trinh phan tich Lua 4.0.1 dung
chinh LuaLib cua engine) sau khi ghi.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402
import doi_tep as dt  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = dt.S
MOC = "[GOHECU 30/08]"
DUOI = ".truoc_gohecu"

# Dau rieng cho tung phan. KHONG dung chung MOC de kiem 'da lam chua':
# DOT 2 da dat MOC vao task07.lua, neu kiem bang MOC thi DOT 4 bo qua nham no.
MOC_RUT = "DA RUT VE KHUNG RONG"
MOC_NHANH = "da go ham ontime_"

KHUNG = """--%(moc)s Tep nay DA RUT VE KHUNG RONG.
--   He cu: %(he)s
--   Ban thay: %(thay)s
--   Ban goc %(n)d dong da chep sang _dara\\script\\%(duong)s
-- Giu tep (khong xoa) vi engine nap theo SO HIEU - tep rong thi so hieu van
-- hop le, khong sinh loi 'khong mo duoc tep'. Khuon chep tu missions\\mission07.lua.

function BeginMission()

end;

function EndMission()

end;

function OnLeave(nPlayerIndex)
\t--PlayerIndex = nPlayerIndex
end;
"""

KHUNG_TIMER = """--%(moc)s Tep nay DA RUT VE KHUNG RONG.
--   He cu: %(he)s
--   Ban thay: %(thay)s
--   Ban goc %(n)d dong da chep sang _dara\\script\\%(duong)s
-- Giu tep (khong xoa) vi settings\\TimerTask.txt tro toi day theo SO HIEU.

function OnTimer()
\tStopTimer()
end

function OnMissionTimer()

end
"""

RUT_GON = [
    ("missions/mission02.lua", "Cong Thanh Chien ban Viet",
     "Cong Thanh ban JX2 (missions\\citywar_city, BAT_CTC_JX2 = 1)", KHUNG),
    ("missions/mission03.lua", "Vuot Ai ban Viet",
     "Thach Dau Thoi Gian ban Linux (missions\\challengeoftime, BAT_HD3 = 1)",
     KHUNG),
    ("missions/mission04.lua", "Phong Lang Do ban Viet",
     "Phong Lang Do ban Linux (missions\\fengling_ferry, BAT_HD3 = 1)", KHUNG),
    ("missions/mission06.lua", "Loi Dai bang hoi ban Viet",
     "Loi Dai ban JX2 (missions\\citywar_arena, BAT_CTC_JX2 = 1)", KHUNG),
    ("timertask/task04.lua", "hen gio Phong Lang Do cu",
     "fld_landingtimer / fld_smalltimer ban Linux (TimerTask 28/29)",
     KHUNG_TIMER),
    ("timertask/task05.lua", "hen gio Phong Lang Do cu",
     "fld_landingtimer / fld_smalltimer ban Linux (TimerTask 28/29)",
     KHUNG_TIMER),
    ("timertask/task06.lua", "hen gio Loi Dai + Vuot Ai cu",
     "citywar_arena timer JX2 (TimerTask 16/17) + timer_match ban Linux (41)",
     KHUNG_TIMER),
    ("timertask/task07.lua", "hen gio Loi Dai cu",
     "citywar_arena totaltimer JX2 (TimerTask 17)", KHUNG_TIMER),
]

# (tep, [ten ham can go], [(neo dong dau khoi elseif/for, so dong)] , [Include can go])
GO_NHANH = [
    ("timertask/task01.lua",
     ["ontime_congthanh", "ontime_vuotai"],
     ['Include("\\\\script\\\\tinhnang\\\\congthanhchien\\\\lib_ctc.lua")',
      'Include("\\\\script\\\\tinhnang\\\\vuot_ai\\\\lib_vuotai.lua")']),
    ("timertask/task02.lua",
     ["ontime_congthanh", "ontime_vuotai"],
     ['Include("\\\\script\\\\tinhnang\\\\congthanhchien\\\\lib_ctc.lua")',
      'Include("\\\\script\\\\tinhnang\\\\vuot_ai\\\\lib_vuotai.lua")']),
    ("timertask/task10.lua",
     ["ontime_phonglangdoketthuc", "ontime_vuotaiketthuc"],
     ['Include("\\\\script\\\\tinhnang\\\\vuot_ai\\\\lib_vuotai.lua")',
      'Include("\\\\script\\\\tinhnang\\\\phonglangdo\\\\lib_phonglangdo.lua")']),
]

# OnTimer / OnMissionTimer viet lai (nguyen van moi) - vi chung tron nhieu he
VIET_LAI = {
    "timertask/task01.lua": [
        ("OnTimer",
         "function OnTimer()\n"
         "\t-- %s than cu chi phuc vu Vuot Ai ban Viet (MAP_VUOTAI) - da go.\n"
         "\tStopTimer()\n"
         "end" % MOC),
        ("OnMissionTimer",
         "function OnMissionTimer() --nIndex la so id nhiem vu, TongKim la mission 1\n"
         "\t-- %s da go 2 nhanh chet: ID_MAP_CTC (Cong Thanh cu) va MAP_VUOTAI\n"
         "\t-- (Vuot Ai cu). Chi con Tong Kim - hoat dong dang chay.\n"
         "\tif (SubWorld == SubWorldID2Idx(MAP_TK_TC)) then\n"
         "\t\tontime_tongkim()\n"
         "\tend\n"
         "end;" % MOC),
    ],
    "timertask/task02.lua": [
        ("OnMissionTimer",
         "function OnMissionTimer()\n"
         "\t-- %s da go 2 nhanh chet: ID_MAP_CTC (Cong Thanh cu) va MAP_VUOTAI\n"
         "\t-- (Vuot Ai cu). Chi con Tong Kim - hoat dong dang chay.\n"
         "\tif (SubWorld == SubWorldID2Idx(MAP_TK_TC)) then\n"
         "\t\tontime_tongkim()\n"
         "\tend\n"
         "end;" % MOC),
    ],
    "timertask/task10.lua": [
        ("OnMissionTimer",
         "function OnMissionTimer()\n"
         "\t-- %s da go 2 nhanh chet: MAP_DUATHUYEN_PLD (Phong Lang Do cu) va\n"
         "\t-- MAP_VUOTAI (Vuot Ai cu). Chi con Tong Kim.\n"
         "\tif (SubWorld == SubWorldID2Idx(MAP_TK_TC)) then\n"
         "\t\tontime_tongkimketthuc()\n"
         "\tend\n"
         "end;" % MOC),
    ],
}


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi_tep(p, nd, ghi):
    if not ghi:
        return True
    sao = p + DUOI
    if not os.path.isfile(sao):
        shutil.copy2(p, sao)
    with io.open(p, "wb") as f:
        f.write(nd.encode("latin-1"))
    return doc(p) == nd


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t40 DOT 4: go moc con lai - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    da_sua = []

    # ---------- (1) rut ve khung rong ----------
    print("## (1) rut ve khung rong")
    for duong, he, thay, khuon in RUT_GON:
        p = os.path.join(S, duong.replace("/", os.sep))
        raw = doc(p)
        if MOC_RUT in raw:
            print("  %-28s da rut - bo qua" % duong)
            continue
        n = raw.count("\n") + 1
        moi = (khuon % dict(moc=MOC, he=he, thay=thay, n=n,
                            duong=duong.replace("/", "\\"))).replace("\n",
                                                                     "\r\n")
        if lh.can_bang(moi) != 0:
            print("!!! LOI TO: khung rong khong can bang: %s" % duong)
            return 1
        # chep ban goc sang _dara truoc khi rut
        dich = os.path.join(dt.DARA, "script",
                            duong.replace("/", os.sep) + ".goc")
        if ghi:
            if not os.path.isdir(os.path.dirname(dich)):
                os.makedirs(os.path.dirname(dich))
            shutil.copy2(p, dich)
        if not ghi_tep(p, moi, ghi):
            print("!!! LOI TO: doc lai KHONG khop: %s" % duong)
            return 1
        da_sua.append(duong)
        print("  %-28s %4d dong -> khung rong  (%s)"
              % (duong, n, "da ghi" if ghi else "se ghi"))
    print()

    # ---------- (2) go nhanh chet trong tep con song ----------
    print("## (2) go nhanh chet trong tep con song")
    for duong, ham_go, inc_go in GO_NHANH:
        p = os.path.join(S, duong.replace("/", os.sep))
        raw = doc(p)
        if MOC_NHANH in raw:
            print("  %-28s da go - bo qua" % duong)
            continue
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                              - raw.count("\r\n")) else "\n"
        cb0 = lh.can_bang(raw)
        dong = raw.split(eol)

        # 2a. viet lai cac ham dieu phoi
        for ten, than_moi in VIET_LAI.get(duong, []):
            r = lh.tim_ham(dong, ten)
            if r is None:
                print("!!! LOI TO: %s khong do duoc ham %s" % (duong, ten))
                return 1
            a, b = r
            if lh.can_bang(eol.join(dong[a:b + 1])) != 0:
                print("!!! LOI TO: %s ham %s cat khong can bang" % (duong, ten))
                return 1
            dong[a:b + 1] = than_moi.split("\n")
            print("  %-28s viet lai %s (%d dong -> %d)"
                  % (duong, ten, b - a + 1, len(than_moi.split("\n"))))

        # 2b. xoa cac ham chet
        cat = []
        for ten in ham_go:
            r = lh.tim_ham(dong, ten)
            if r is None:
                print("!!! LOI TO: %s khong do duoc ham %s" % (duong, ten))
                return 1
            cat.append((r[0], r[1], ten))
        for a, b, ten in sorted(cat, reverse=True):
            if lh.can_bang(eol.join(dong[a:b + 1])) != 0:
                print("!!! LOI TO: %s ham %s cat khong can bang" % (duong, ten))
                return 1
            dong[a:b + 1] = ["-- %s da go ham %s (%d dong) - he cu"
                             % (MOC, ten, b - a + 1)]
            print("  %-28s xoa ham %-26s (%3d dong)" % (duong, ten, b - a + 1))

        # 2c. comment Include
        for neo in inc_go:
            chi_so = [i for i, l in enumerate(dong)
                      if neo in l and not l.lstrip().startswith("--")]
            if len(chi_so) != 1:
                print("!!! LOI TO: %s Include khop %d dong: %s"
                      % (duong, len(chi_so), neo[:50]))
                return 1
            i = chi_so[0]
            cu = dong[i]
            thut = cu[:len(cu) - len(cu.lstrip())]
            dong[i] = "%s-- %s he cu da go%s%s-- %s" % (thut, MOC, eol, thut,
                                                        cu.strip())
        nd = eol.join(dong)
        cb1 = lh.can_bang(nd)
        if cb1 != cb0:
            print("!!! LOI TO: %s can bang doi (%d -> %d)" % (duong, cb0, cb1))
            return 1
        if not ghi_tep(p, nd, ghi):
            print("!!! LOI TO: doc lai KHONG khop: %s" % duong)
            return 1
        da_sua.append(duong)
        print("  %-28s can bang %d giu nguyen  (%s)"
              % (duong, cb1, "da ghi" if ghi else "se ghi"))
    print()

    # ---------- (3) noi tuyen hang cho longxuewan.lua ----------
    print("## (3) noi tuyen hang cho item Long Huyet Hoan")
    p = os.path.join(S, "item", "longxuewan.lua")
    raw = doc(p)
    if "MAX_NUM_LONGHH_DAY =" in raw:
        print("  item/longxuewan.lua da go - bo qua")
    else:
        # lay gia tri THAT tu thu vien cu - khong tu go tay
        lib = doc(os.path.join(S, "tinhnang", "vuot_ai", "lib_vuotai.lua"))
        m = re.search(r"^\s*MAX_NUM_LONGHH_DAY\s*=\s*(\d+)", lib, re.M)
        if not m:
            print("!!! LOI TO: khong doc duoc MAX_NUM_LONGHH_DAY trong lib cu")
            return 1
        gt = m.group(1)
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                              - raw.count("\r\n")) else "\n"
        neo = 'Include("\\\\script\\\\tinhnang\\\\vuot_ai\\\\lib_vuotai.lua")'
        if raw.count(neo) != 1:
            print("!!! LOI TO: longxuewan.lua Include khop %d lan"
                  % raw.count(neo))
            return 1
        thay = ("-- %s he Vuot Ai cu da go; noi tuyen hang thay cho Include."
                % MOC + eol
                + "-- Gia tri chep tu tinhnang\\vuot_ai\\lib_vuotai.lua"
                  " truoc khi go." + eol
                + "MAX_NUM_LONGHH_DAY = " + gt)
        nd = raw.replace(neo, thay, 1)
        if lh.can_bang(nd) != lh.can_bang(raw):
            print("!!! LOI TO: longxuewan.lua can bang doi")
            return 1
        if not ghi_tep(p, nd, ghi):
            print("!!! LOI TO: doc lai KHONG khop: longxuewan.lua")
            return 1
        da_sua.append("item/longxuewan.lua")
        print("  item/longxuewan.lua  MAX_NUM_LONGHH_DAY = %s (noi tuyen)  (%s)"
              % (gt, "da ghi" if ghi else "se ghi"))

    print()
    print("=> %d tep" % len(da_sua))
    if ghi:
        print()
        print("Kiem cu phap bang syncheck.exe:")
        for d in da_sua:
            print("   %s" % d)
    else:
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
