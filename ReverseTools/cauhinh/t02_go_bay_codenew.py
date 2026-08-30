# -*- coding: utf-8 -*-
"""t02_go_bay_codenew.py - THI CONG DOT 1/b: go BAY ghi hong kho 8.000 ma.

BAY (da tu kiem chung tan tep):
  script\\codenew.lua        khai  Code_New = { [1]=... }        8.000 muc, khoa SO
  script\\giftcode_new.lua   khai  Code_New = { ["3UG4..."]=... } 3.000 muc, khoa CHU
  => TRUNG TEN BIEN TOAN CUC, hai luoc do khoa KHONG tuong thich.

  hotrotanthu.lua:12    Include codenew.lua                <- dang BAT
  hotrotanthu.lua:13-14 Include giftcode_*                 <- da comment
        (HAI DONG COMMENT NAY LA THU DUY NHAT dang chan bay)
  hotrotanthu.lua:218   SaveData("script/codenew.lua", ...) <- ghi NGUOC xuong dia

  Ai bo comment o :13-14 (khong biet ly do) => Code_New trong state do thanh
  bang khoa-CHU 3.000 muc => SaveData ghi de len tep 8.000 muc => MAT VINH VIEN.

MIENG VA (that su 1 dong): doi ten bang trong giftcode_new.lua
      Code_New  ->  Code_GiftNew
va doi theo o MOI cho dung no (lequan.lua). Sau do hai tep khong con dung ten,
bay bien mat du ai co bo comment hay khong.

An toan: giftcode_new.lua la tep DU LIEU (3.000 dong khoa) + 1 dong ten bang.
Doi ten bang KHONG dung toi du lieu. lequan.lua la noi duy nhat doc no.

KHONG build. Can KHOI DONG LAI GameServer (bao chu).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_gobay lan dau).
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dem_ma import dem_ma  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
GIFT = os.path.join(S, "giftcode_new.lua")
LEQUAN = os.path.join(S, "global", "npcchucnang", "lequan.lua")
CODENEW = os.path.join(S, "codenew.lua")
HOTRO = os.path.join(S, "npcthon", "balanghuyen", "hotrotanthu.lua")
HAU_TO = ".truoc_gobay"
NHAN = "[GOBAY 29/08]"
TEN_CU = "Code_New"
TEN_MOI = "Code_GiftNew"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t02_go_bay_codenew - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    for p in (GIFT, LEQUAN, CODENEW, HOTRO):
        if not os.path.isfile(p):
            print("!!! LOI TO: thieu %s" % p)
            return 1

    # ---- chot bang chung: bay co that ----
    d_code = doc(CODENEW)[:200]
    d_gift = doc(GIFT)[:200]
    if TEN_CU not in d_code or TEN_CU not in d_gift:
        print("!!! LOI TO: khong thay ca hai tep cung khai %s - kiem lai tay" % TEN_CU)
        return 1
    khoa_so = bool(re.search(r"%s\s*=\s*\{\s*\r?\n?\s*\[\d+\]" % TEN_CU, d_code))
    khoa_chu = bool(re.search(r'%s\s*=\s*\{\s*\r?\n?\s*\["' % TEN_CU, d_gift))
    print("  bang chung: codenew.lua khoa SO = %s | giftcode_new.lua khoa CHU = %s"
          % (khoa_so, khoa_chu))
    if not (khoa_so and khoa_chu):
        print("!!! LOI TO: luoc do khoa khong nhu mo ta - DUNG LAI")
        return 1
    if 'SaveData("script/codenew.lua"' not in doc(HOTRO):
        print("!!! LOI TO: khong thay duong ghi nguoc trong hotrotanthu.lua")
        return 1
    print("  bang chung: hotrotanthu.lua CO ghi nguoc xuong codenew.lua")

    viec = []

    # ---- 1) giftcode_new.lua: doi ten bang (chi dong khai bao) ----
    raw = doc(GIFT)
    if NHAN in raw:
        print("  giftcode_new.lua: DA VA - bo qua")
    else:
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
        dg = raw.split(eol)
        vt = [i for i, l in enumerate(dg) if re.match(r"^\s*%s\s*=" % TEN_CU, l)]
        if len(vt) != 1:
            print("!!! LOI TO: giftcode_new.lua co %d dong khai %s (can 1)"
                  % (len(vt), TEN_CU))
            return 1
        dg[vt[0]] = re.sub(r"^(\s*)%s(\s*=)" % TEN_CU,
                           r"\g<1>%s\g<2>" % TEN_MOI, dg[vt[0]])
        dg.insert(0, "-- " + NHAN + " doi ten bang %s -> %s de khong trung ten voi"
                  % (TEN_CU, TEN_MOI))
        dg.insert(1, "-- codenew.lua (8.000 ma, khoa SO). Trung ten + SaveData o"
                  " hotrotanthu.lua:218")
        dg.insert(2, "-- => nguy co ghi de MAT VINH VIEN kho 8.000 ma."
                  " Xem ReverseTools/cauhinh/t02_go_bay_codenew.py")
        nd = eol.join(dg)
        if dem_ma(nd, TEN_MOI) != 1:
            print("!!! LOI TO: ten moi khong duy nhat trong giftcode_new.lua")
            return 1
        con = dem_ma(nd, TEN_CU)
        if con:
            print("!!! LOI TO: giftcode_new.lua van con %d cho dung %s" % (con, TEN_CU))
            return 1
        print("  giftcode_new.lua: doi ten bang o dong %d" % (vt[0] + 1))
        viec.append((GIFT, nd))

    # ---- 2) lequan.lua: doi moi cho dung ----
    raw2 = doc(LEQUAN)
    if NHAN in raw2:
        print("  lequan.lua: DA VA - bo qua")
    else:
        eol2 = "\r\n" if raw2.count("\r\n") >= (raw2.count("\n") - raw2.count("\r\n")) else "\n"
        # DO THAT: lequan.lua chi con MOT DONG CHU THICH (:114) nhac toi bang nay -
        # kho 3.000 ma qua tang hien KHONG duoc ma song nao nap. Van doi ten trong
        # chu thich, de hom sau ai bo chu thich ra thi dung ngay ten moi, khong bi nil.
        n_ma = dem_ma(raw2, TEN_CU)
        n_cu = len(re.findall(r"\b%s\b" % TEN_CU, raw2))
        print("  lequan.lua: %d cho trong MA THAT, %d cho ke ca chu thich"
              % (n_ma, n_cu))
        if n_cu == 0:
            print("!!! LOI TO: lequan.lua khong nhac toi %s - kiem lai" % TEN_CU)
            return 1
        nd2 = re.sub(r"\b%s\b" % TEN_CU, TEN_MOI, raw2)
        hi0 = sum(1 for c in raw2 if ord(c) > 127)
        if sum(1 for c in nd2 if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi (lequan.lua)")
            return 1
        if dem_ma(nd2, TEN_CU) != 0:
            print("!!! LOI TO: lequan.lua van con %s" % TEN_CU)
            return 1
        d2 = nd2.split(eol2)
        d2.insert(0, "-- " + NHAN + " %s -> %s (%d cho trong ma) -"
                  " xem ReverseTools/cauhinh/t02_go_bay_codenew.py"
                  % (TEN_CU, TEN_MOI, n_cu))
        nd2 = eol2.join(d2)
        print("  lequan.lua: doi %d cho dung %s -> %s" % (n_cu, TEN_CU, TEN_MOI))
        viec.append((LEQUAN, nd2))

    if not viec:
        print("Khong co gi de lam.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, nd in viec:
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(p) != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    print("\nBay da go. Can KHOI DONG LAI GameServer (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
