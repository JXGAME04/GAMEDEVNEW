# -*- coding: utf-8 -*-
"""t23_nan_5_ma_con_lai.py - nan 5 ma vat pham con sai trong bang thuong.

CAN CU: bo ra soat `t15_ra_soat_ma_thuong.py` tra bang DUNG CACH MAY TRA (theo
CHI SO DONG - KItemGenerator.CPP:1660 -> KBasPropTbl.cpp:1058), doi khop ten co
bo dau. Sau hai vong loc duong tinh gia, con dung 5 cho:

  bigboss.lua:68        {6,1,30446} "Dai Thanh Bi Kip 150"
        may tra ra: (KHONG CO dong nay)          -> {6,1,3208}
  lenhbai_def.lua:433   {6,1,30446} "Dai thanh bi kip(150)"
        may tra ra: (KHONG CO dong nay)          -> {6,1,3208}
  mibao_head.lua:19     {6,1,2024}  "Khieu chien le bao"
        may tra ra: "Truy cong lenh"  <- NHAM MON -> {6,1,2015}
  task_head.lua:56      {6,1,2973}  "Tich Lich don"
        may tra ra: ten tieng Trung rac          -> {6,1,3591}
  task_head.lua:57      {6,1,2974}  "<Bac Dau Truong Sinh Thuat...>"
        may tra ra: ten tieng Trung rac          -> {6,1,3592}

HAI DUONG TINH GIA DA LOC RA (ghi lai de khong ai nan nham ve sau):
  - {6,1,3440} "Qua Huy Hoang (cao)": may tra ra DUNG ten do. Bang co HAI mon
    trung ten (chi so 907 va 3440). Doi 3440 -> 907 la doi sang mon KHAC.
  - {6,1,2433} "Dai Thanh Bi Kip cap 90": may tra ra "Dai Thanh Bi Kip 90" -
    dung mon, chi khac chu "cap" trong ten script.

⚠️ BAO CHU: ba cho dau (30446 x2, 2024) truoc day KHONG ra dung mon, nan xong
   la chung BAT DAU ra do that / ra dung mon. Do la thay doi ve kinh te.
   Hoan tac duoc bang ban sao .truoc_nan5ma.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import bang_vatpham as bv  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
NHAN = "[NAN5MA 30/08]"

# (tep, so_dong_mong, ma_cu, ma_moi, ten_mong)
NAN = [
    ("missions/boss/bigboss.lua", 68, 30446, 3208, "Đại Thành Bí Kíp cấp 150"),
    ("vng_event/thapnienlenhbai/lenhbai_def.lua", 433, 30446, 3208,
     "Đại Thành Bí Kíp cấp 150"),
    ("task/tollgate/killer/mibao_head.lua", 19, 2024, 2015,
     "Khiêu chiến Lễ bao"),
    ("task/metempsychosis/task_head.lua", 56, 2973, 3591, "Tích Lịch đơn"),
    ("task/metempsychosis/task_head.lua", 57, 2974, 3592,
     "<Bắc Đẩu Trường Sinh ThuậtĐại Thừa Tâm Pháp>"),
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t23_nan_5_ma_con_lai - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    cs, tt, ttm, lech = bv.nap()
    bv.bao_cao_lech(lech, cs)

    # chot truoc: moi ma MOI phai ton tai va dung ten
    for _tep, _dg, _cu, moi, ten in NAN:
        that = cs.get(moi)
        if not that:
            print("!!! LOI TO: ma moi %d khong co trong bang" % moi)
            return 1
        if bv._gon_manh(that[0]) != bv._gon_manh(ten):
            print("!!! LOI TO: ma moi %d ben du an la %r, mong %r"
                  % (moi, that[0], ten))
            return 1
        if moi in lech:
            print("!!! LOI TO: ma moi %d NAM TRONG VUNG LECH - khong an toan"
                  % moi)
            return 1
    print("  chot: 5 ma dich deu ton tai, dung ten, va NGOAI vung lech")

    theo_tep = {}
    for tep, dg, cu, moi, ten in NAN:
        theo_tep.setdefault(tep, []).append((dg, cu, moi, ten))

    viec = []
    for tep, ds in theo_tep.items():
        p = os.path.join(S, tep.replace("/", os.sep))
        if not os.path.isfile(p):
            print("!!! LOI TO: thieu %s" % tep)
            return 1
        raw = doc(p)
        if NHAN in raw:
            print("  %-46s DA VA - bo qua" % tep)
            continue
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                             - raw.count("\r\n")) else "\n"
        hi0 = sum(1 for c in raw if ord(c) > 127)
        d = raw.split(eol)
        n_tep = 0
        for dg, cu, moi, ten in ds:
            i = dg - 1
            if i < 0 or i >= len(d):
                print("!!! LOI TO: %s khong co dong %d" % (tep, dg))
                return 1
            l = d[i]
            mau = re.compile(r"\{\s*6\s*,\s*1\s*,\s*%d\b" % cu)
            if not mau.search(l):
                print("!!! LOI TO: %s:%d khong chua ma %d (tep da doi?)"
                      % (tep, dg, cu))
                print("    dong that: %r" % l[:100])
                return 1
            if len(mau.findall(l)) != 1:
                print("!!! LOI TO: %s:%d co %d lan ma %d"
                      % (tep, dg, len(mau.findall(l)), cu))
                return 1
            d[i] = mau.sub("{6,1,%d" % moi, l)
            n_tep += 1
            print("  %-46s d%-5d %d -> %d  (%s)"
                  % (tep, dg, cu, moi, ten[:30]))
        if n_tep == 0:
            continue
        nd = eol.join(d)
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte tieng Viet doi (%s)" % tep)
            return 1
        nd = eol.join([
            "-- " + NHAN + " nan ma vat pham theo TEN, tra bang DUNG cach may"
            " tra (theo chi so dong).",
            "-- Xem ReverseTools/cauhinh/t23_nan_5_ma_con_lai.py va"
            " ra_soat_ma_thuong.txt",
        ] + nd.split(eol))
        viec.append((p, nd, raw, tep, n_tep))

    if not viec:
        print("Khong co gi de lam.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, nd, raw, tep, n in viec:
        sao = p + ".truoc_nan5ma"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(p) != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %-46s (%d cho)" % (tep, n))
    print("\nCan KHOI DONG LAI GameServer (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
