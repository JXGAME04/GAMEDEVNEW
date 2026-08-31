# -*- coding: utf-8 -*-
"""t42_go_he_cu_dot5.py - DOT 5: ba tan du CUOI CUNG cua he cu con dang song.

(1) BA BAY CONG THANH CU CON SONG TREN BAN DO 221 - LOI DANG XAY RA
    startgame.lua:117 goi addtrapcongthanh() moi lan khoi dong. Trong ham do
    (lib_ctc.lua:265-285) 9/12 dong AddTrap da comment tu truoc, con lai 3 dong
    278-280 dat bay tai (1472,3085) (1535,3029) (1409,3149) ban kinh 30 tren
    ID_MAP_CTC = 221.
    Ban do 221 = "Cong thanh chien truong" - DUNG ban do ma Cong Thanh JX2 dang
    chay (citywar_city\\head.lua:83-84 CS_CampPos1/2 = {221,...}).
    Ba bay chay chancong_1/2/3.lua, doc GetMissionV(M_CTHANHVO_1..3) cua mission
    CU (mission 2 - khong bao gio mo nua) roi SetPos day nguoi choi lui.
    => Giua tran Cong Thanh JX2, ai di qua 3 diem do bi bat lui.
    Cach vá: comment 3 dong AddTrapEx1 - dung kieu 9 dong da comment san.
    KHONG dung toi 2 dong rương o ban do 222/223 (lib_ctc.lua:229-230): he JX2
    co xa phu/quan nhu/ve binh/duoc thuong cho hau phuong nhung KHONG co rương,
    bo di la mat cho gui do. De chu game quyet.

(2) MIN LIEN DAU trong dong ho may chu
    timerserver.lua:808 sukien_liendau Include
    "\\script\\tinhnang\\lien_dau\\data\\..." - thu muc nay DA BI DOI di tu
    20/08 (THICONG_LIENDAU_PORT.md D8). Ham cung dung TIME_LIEN_DAU /
    TAB_DANHSACHTRUNG / TAB_DANHSACHCAO - khong con noi nao khai.
    Hien tat boi BAT_LIENDAU = 0 nen chua no. Nhung cong tac ten "BAT_LIENDAU"
    de khien nguoi ta bat len tuong la bat Lien Dau - trong khi Lien Dau THAT
    (WLLS) chay khong qua cong tac nay.
    => Go ca ham + nhanh goi + khoa BAT_LIENDAU.

(3) MUC MENU CHET
    item\\lenhbaiadmin.lua:512 "Test Phong Lang Do/admintestPLD" - ham
    admintestPLD chi co trong lib_phonglangdo.lua, ma lenhbaiadmin da bo Include
    tu DOT 2. Bam vao la goi ham nil.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
MOC = "[GOHECU 30/08]"
DUOI = ".truoc_gohecu"

BAY = [
    ('AddTrapEx1(ID_MAP_CTC,1472,3085,30,'
     '"\\\\script\\\\maps\\\\congthanhchien\\\\trap\\\\chancong_1.lua")'),
    ('AddTrapEx1(ID_MAP_CTC,1535,3029,30,'
     '"\\\\script\\\\maps\\\\congthanhchien\\\\trap\\\\chancong_2.lua")'),
    ('AddTrapEx1(ID_MAP_CTC,1409,3149,30,'
     '"\\\\script\\\\maps\\\\congthanhchien\\\\trap\\\\chancong_3.lua")'),
]
LY_DO_BAY = ("bay he Cong Thanh CU tren ban do 221 - dung ban do Cong Thanh JX2"
             " dang chay; doc GetMissionV cua mission 2 (khong con mo) nen luon"
             " = 0 va SetPos day nguoi choi lui giua tran")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def ghi_tep(p, nd, ghi):
    if not ghi:
        return True
    sao = p + DUOI
    if not os.path.isfile(sao):
        shutil.copy2(p, sao)
    with io.open(p, "wb") as f:
        f.write(nd.encode("latin-1"))
    return doc(p) == nd


def comment_dong(dong, neo, ly_do, eol):
    chi_so = [i for i, l in enumerate(dong)
              if neo in l and not l.lstrip().startswith("--")]
    if len(chi_so) != 1:
        return None, "khop %d dong (can 1)" % len(chi_so)
    i = chi_so[0]
    cu = dong[i]
    thut = cu[:len(cu) - len(cu.lstrip())]
    dong[i] = "%s-- %s %s%s%s-- %s" % (thut, MOC, ly_do, eol, thut, cu.strip())
    return i, None


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t42 DOT 5: ba tan du cuoi - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    # ---------- (1) ba bay tren ban do 221 ----------
    print("## (1) ba bay Cong Thanh cu tren ban do 221 (LOI DANG XAY RA)")
    p = os.path.join(S, "tinhnang", "congthanhchien", "lib_ctc.lua")
    raw = doc(p)
    if MOC in raw:
        print("  lib_ctc.lua da va - bo qua")
        nd1 = raw
    else:
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                              - raw.count("\r\n")) else "\n"
        cb0, hi0 = lh.can_bang(raw), hi(raw)
        dong = raw.split(eol)
        for neo in BAY:
            i, loi = comment_dong(dong, neo, LY_DO_BAY, eol)
            if loi:
                print("!!! LOI TO: bay %s: %s" % (neo[:44], loi))
                return 1
            print("  dong %4d  %s" % (i + 1, neo[:74]))
        nd1 = eol.join(dong)
        if lh.can_bang(nd1) != cb0:
            print("!!! LOI TO: lib_ctc.lua can bang doi")
            return 1
        if hi(nd1) != hi0:
            print("!!! LOI TO: lib_ctc.lua byte tieng Viet doi")
            return 1
        if not ghi_tep(p, nd1, ghi):
            print("!!! LOI TO: doc lai KHONG khop lib_ctc.lua")
            return 1
        print("  lib_ctc.lua: can bang %d giu nguyen  (%s)"
              % (cb0, "da ghi" if ghi else "se ghi"))
    print()

    # ---------- (2) min Lien Dau ----------
    print("## (2) min Lien Dau trong dong ho may chu")
    p = os.path.join(S, "timerserver.lua")
    raw = doc(p)
    if "da go ham sukien_liendau" in raw:
        print("  timerserver.lua da go - bo qua")
    else:
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                              - raw.count("\r\n")) else "\n"
        cb0 = lh.can_bang(raw)
        dong = raw.split(eol)
        viec = []
        r = lh.tim_ham(dong, "sukien_liendau")
        if r is None:
            print("!!! LOI TO: khong do duoc ham sukien_liendau")
            return 1
        viec.append((r[0], r[1], "ham sukien_liendau"))
        k = lh.tim_khoi(dong, 'G_CFG("BAT_LIENDAU"')
        if k is None:
            print("!!! LOI TO: khong do duoc nhanh goi BAT_LIENDAU")
            return 1
        viec.append((k[0], k[1], "nhanh goi BAT_LIENDAU"))
        for a, b, ten in sorted(viec, reverse=True):
            if lh.can_bang(eol.join(dong[a:b + 1])) != 0:
                print("!!! LOI TO: %s cat khong can bang" % ten)
                return 1
            thut = dong[a][:len(dong[a]) - len(dong[a].lstrip())]
            dong[a:b + 1] = [
                "%s-- %s da go %s (%d dong). He lien_dau cu da doi di tu 20/08;"
                % (thut, MOC, ten, b - a + 1),
                "%s-- ham nay Include thu muc KHONG CON TON TAI. Lien Dau that"
                " = ban WLLS," % thut,
                "%s-- chay doc lap, khong qua cong tac BAT_LIENDAU." % thut,
            ]
            print("  go %-22s dong %4d..%-4d (%3d dong)"
                  % (ten, a + 1, b + 1, b - a + 1))
        nd2 = eol.join(dong)
        if lh.can_bang(nd2) != cb0:
            print("!!! LOI TO: timerserver.lua can bang doi")
            return 1
        ma = lh.sach(nd2)
        for t in ("TIME_LIEN_DAU", "TAB_DANHSACHTRUNG", "TAB_DANHSACHCAO",
                  "sukien_liendau", "lien_dau"):
            if re.search(r"\b%s\b" % re.escape(t), ma):
                print("!!! LOI TO: con tham chieu %s" % t)
                return 1
        print("  khong con tham chieu TIME_LIEN_DAU / TAB_DANHSACH* / lien_dau")
        if not ghi_tep(p, nd2, ghi):
            print("!!! LOI TO: doc lai KHONG khop timerserver.lua")
            return 1
        print("  timerserver.lua: can bang %d giu nguyen  (%s)"
              % (cb0, "da ghi" if ghi else "se ghi"))

        # bo khoa BAT_LIENDAU khoi ch_lich.lua
        pl = os.path.join(S, "cauhinh", "ch_lich.lua")
        rawl = doc(pl)
        eoll = "\r\n" if rawl.count("\r\n") >= (rawl.count("\n")
                                                - rawl.count("\r\n")) else "\n"
        dongl = rawl.split(eoll)
        ci = [i for i, l in enumerate(dongl)
              if re.match(r"\s*BAT_LIENDAU\s*=", l)]
        if len(ci) != 1:
            print("!!! LOI TO: ch_lich.lua BAT_LIENDAU khop %d dong" % len(ci))
            return 1
        dongl[ci[0]:ci[0] + 1] = [
            "-- %s bo khoa BAT_LIENDAU: he Lien Dau cu da go han." % MOC,
            "-- Lien Dau dang chay = ban WLLS (missions\\leaguematch +"
            " leaguematch\\gsdriver.lua),",
            "-- chay theo lich rieng, KHONG doc khoa nay.",
        ]
        ndl = eoll.join(dongl)
        if not ghi_tep(pl, ndl, ghi):
            print("!!! LOI TO: doc lai KHONG khop ch_lich.lua")
            return 1
        print("  ch_lich.lua: bo khoa BAT_LIENDAU  (%s)"
              % ("da ghi" if ghi else "se ghi"))
    print()

    # ---------- (3) muc menu chet ----------
    print("## (3) muc menu chet trong lenh bai admin")
    p = os.path.join(S, "item", "lenhbaiadmin.lua")
    raw = doc(p)
    if "/admintestPLD" not in raw or ("%s he cu da go" % MOC) in raw.split(
            "/admintestPLD")[0][-200:]:
        pass
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                          - raw.count("\r\n")) else "\n"
    dong = raw.split(eol)
    ci = [i for i, l in enumerate(dong)
          if "/admintestPLD" in l and not l.lstrip().startswith("--")]
    if not ci:
        print("  muc menu da go - bo qua")
    elif len(ci) != 1:
        print("!!! LOI TO: muc menu khop %d dong" % len(ci))
        return 1
    else:
        hi0, cb0 = hi(raw), lh.can_bang(raw)
        i = ci[0]
        cu = dong[i]
        thut = cu[:len(cu) - len(cu.lstrip())]
        dong[i] = ("%s-- %s ham admintestPLD chi co trong he Phong Lang Do CU"
                   " (da go) - bam vao la goi ham nil" % (thut, MOC)
                   + eol + "%s-- %s" % (thut, cu.strip()))
        nd3 = eol.join(dong)
        if lh.can_bang(nd3) != cb0 or hi(nd3) != hi0:
            print("!!! LOI TO: lenhbaiadmin.lua can bang / byte tieng Viet doi")
            return 1
        if not ghi_tep(p, nd3, ghi):
            print("!!! LOI TO: doc lai KHONG khop lenhbaiadmin.lua")
            return 1
        print("  dong %d: da comment muc 'Test Phong Lang Do/admintestPLD'"
              "  (%s)" % (i + 1, "da ghi" if ghi else "se ghi"))

    print()
    if not ghi:
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
