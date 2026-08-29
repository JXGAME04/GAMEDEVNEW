# -*- coding: utf-8 -*-
"""v26_ma_consot.py - Nan NOT 8 tham chieu ma vat pham con sot ma cua ban LINUX.

Phat hien khi doc TRON tinh nang (yeu cau chu game 29/08: "doc het tinh nang
roi moi viet ban test"). Ca 8 cho deu nam trong nhanh Calc/Consume/so sanh nen
KHONG lam sap - chung AM THAM vo hieu hoa chuc nang, rat kho thay khi test.

Da doi chieu TEN bang magicscript.txt cua CHINH du an:

  A) item\\yandimibao.lua:159   ConsumeItem(3,1,6,1,2805,-1)
     2805 ben JX1 = "Banh Tet"  ->  Viem De Bi Bao = 3423
     (chinh head.lua:181 cap rương 3423; dong 160 ngay duoi in "Khong tim thay
      Viem De Bi Bao") => mo rương LUON that bai, ma CHIA KHOA da bi tieu o :154.

  B) npc\\yandituteng.lua:11,29,34   (6,1,1685)
     1685 ben JX1 = "Bach Chan Don"  ->  "Manh Do Dang Viem De" = 1694
     => ghep 9 manh thanh Do Dang khong the chay.

  C) npc\\yandituteng.lua:104,106   (6,1,2532)
     2532 ben JX1 = "Hoa phong hoan le bao" -> "Viem De Lenh Ky" = 2541
     (chinh tep nay dong 54/61 TAO ra 2541) => gom 100 Lenh Ky doi Huyen Vien
     Lenh khong bao gio du.

  D) saizi.lua:46,58 (nPart == 1605) va :55 (nPart == 1606)
     1605 = "Thiep chuc su de", 1606 = "Thiep chuc su huynh" ben JX1;
     mon that dua vao xuc xac la Hinh nhan = 1614 (head.lua:139/145), va
     Do Dang = 1615 => nhanh NHAN DOI Hinh nhan cho nguoi mang Viem De Lenh
     + 2 dong ghi log KHONG BAO GIO chay.

KHONG build. Can KHOI DONG LAI GameServer de nap (bao chu).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_masot lan dau).
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import tcvn2uni  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

B = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MS = os.path.join(B, "settings", "item", "magicscript.txt")
YD = os.path.join(B, "script", "missions", "yandibaozang")
HAU_TO = ".truoc_masot"
NHAN = "[MA2 29/08]"

# (tep, [(chuoi_cu, chuoi_moi, so_lan_mong, ten_mon_de_nghiem_thu)])
VIEC = [
    (os.path.join(YD, "item", "yandimibao.lua"), [
        ("6, 1, 2805, -1", "6, 1, 3423, -1", 1, "Viêm Đế Bí Bảo"),
    ]),
    (os.path.join(YD, "npc", "yandituteng.lua"), [
        ("6,1,1685,-1", "6,1,1694,-1", 3, "Mảnh Đồ Đằng Viêm Đế"),
        ("6, 1, 2532, -1", "6, 1, 2541, -1", 1, "Viêm Đế Lệnh Kỳ"),
        ("6,1,2532, -1", "6,1,2541, -1", 1, "Viêm Đế Lệnh Kỳ"),
    ]),
    (os.path.join(YD, "saizi.lua"), [
        ("nPart == 1605", "nPart == 1614", 2, "Hình nhân"),
        ("nPart == 1606", "nPart == 1615", 1, "Viêm Đế Đồ Đằng"),
    ]),
]


def bang_ten():
    d = {}
    for l in io.open(MS, "rb").read().decode("latin-1").replace("\r\n", "\n").split("\n")[1:]:
        c = [x.strip() for x in l.split("\t")]
        if len(c) > 3 and c[3].isdigit():
            d.setdefault((c[1], c[2], c[3]), tcvn2uni(c[0]))
    return d


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== v26_ma_consot - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    tra = bang_ten()

    ketqua = []
    for duong, thaythe in VIEC:
        if not os.path.isfile(duong):
            print("!!! LOI TO: khong thay %s" % duong)
            return 1
        raw = io.open(duong, "rb").read().decode("latin-1")
        hi0 = sum(1 for c in raw if ord(c) > 127)
        if NHAN in raw:
            print("  %-20s DA VA - bo qua" % os.path.basename(duong))
            continue
        nd = raw
        tong = 0
        for cu, moi, mong, ten_mong in thaythe:
            n = nd.count(cu)
            if n != mong:
                print("!!! LOI TO: %s - chuoi %r khop %d lan (mong %d)"
                      % (os.path.basename(duong), cu, n, mong))
                return 1
            # nghiem thu: ma MOI phai dung ten mong doi
            m = re.search(r"(\d+)\s*,\s*(\d+)\s*,\s*(\d+)", moi) or re.search(r"(\d+)$", moi)
            if "nPart" in cu:
                pnew = moi.split("==")[1].strip()
                gdp = ("6", "1", pnew)
            else:
                s = re.findall(r"\d+", moi)
                gdp = (s[0], s[1], s[2])
            ten_that = tra.get(gdp, "")
            if ten_that.strip().lower() != ten_mong.strip().lower():
                print("!!! LOI TO: ma moi %s ben du an la %r, mong %r"
                      % (str(gdp), ten_that, ten_mong))
                return 1
            nd = nd.replace(cu, moi)
            tong += n
            print("  %-20s %-18s -> %-18s x%d  (%s)"
                  % (os.path.basename(duong), cu, moi, n, ten_mong))
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi (%s)" % os.path.basename(duong))
            return 1
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
        d = nd.split(eol)
        d.insert(0, "-- " + NHAN + " nan ma vat pham con sot cua ban Linux (doi chieu TEN"
                    " bang magicscript cua du an) - xem ReverseTools\\viemde\\v26_ma_consot.py")
        nd = eol.join(d)
        ketqua.append((duong, nd, tong))

    if not ketqua:
        print("Khong co gi de lam.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for duong, nd, tong in ketqua:
        sao = duong + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(duong, sao)
        with io.open(duong, "wb") as f:
            f.write(nd.encode("latin-1"))
        if io.open(duong, "rb").read().decode("latin-1") != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % duong)
            return 1
        print("  DA GHI %-22s (%d cho)" % (os.path.basename(duong), tong))
    print("\nCan KHOI DONG LAI GameServer de nap (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
