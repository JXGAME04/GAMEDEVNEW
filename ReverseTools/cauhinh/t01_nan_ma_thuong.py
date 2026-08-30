# -*- coding: utf-8 -*-
"""t01_nan_ma_thuong.py - THI CONG DOT 1/a: nan ma vat pham SAI trong bang thuong.

PHAT HIEN (bo kiem ktr_cauhinh.py, da tu kiem chung tan ma):
  45 tham chieu ma vat pham trong bang thuong KHONG co trong magicscript.txt
  cua du an, o 6 tep. Hai dang, deu la ma ban LINUX chua nan:
    - {6, 0, x} : magicscript JX1 CHI co detail = 1 (4.916 muc, 0 muc detail 0)
    - {6, 1, 30xxx} : dai 30000+ la ma Linux

  Bang thuong SAI dang duoc dung that: kill_level.lua:129
     tbAward = HD_CFG("HD3_ST_THUONG", nil) or tbAward
  ma cauhinh_hoatdong.lua:390 khai HD3_ST_THUONG = nil
  => nil or tbAward = bang goc co ma sai. Nguoi choi khong nhan duoc mon.

CHIA HAI PHAN (luat "nan ma phai BAO LOI khi tra ten that bai"):
  * 5 mon TRA DUOC ten trong bang du an  -> tep nay nan.
  * 19 mon KHONG CO vat pham trong du an -> KHONG dung toi, liet ke ra tep
    `thieu_vatpham.txt` de chu game quyet (tao vat pham moi / thay mon khac /
    bo khoi bang thuong). Tu y thay mon la DOI CAN BANG - cam.

CACH NAN: chi doi tren dong CO ten mon khop (szName), khong doi mu theo so.
Moi lan doi deu doi chieu ten trong magicscript truoc khi ghi.

KHONG build. Script Lua nap luc chay => can KHOI DONG LAI GameServer (bao chu).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_nanthuong lan dau).
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

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
S = os.path.join(SV, "script")
MS = os.path.join(SV, "settings", "item", "magicscript.txt")
HAU_TO = ".truoc_nanthuong"
NHAN = "[MATHUONG 29/08]"

# (ma_cu_regex, ma_moi, ten_mon_de_doi_chieu)
NAN = [
    (r"\{\s*6\s*,\s*0\s*,\s*3\b", "{6,1,3", "Đại Lực hoàn"),
    (r"\{\s*6\s*,\s*0\s*,\s*6\b", "{6,1,6", "Phi Tốc hoàn"),
    (r"\{\s*6\s*,\s*1\s*,\s*30449\b", "{6,1,3211", "Thiên Sơn Thánh Thủy (đại)"),
    (r"\{\s*6\s*,\s*1\s*,\s*30534\b", "{6,1,4869", "Đồ Phổ Đằng Long Bội"),
    (r"\{\s*6\s*,\s*1\s*,\s*30538\b", "{6,1,4866", "Đằng Long Thạch - Hạ"),
]

TEP = [
    "missions/boss/bigboss.lua",
    "task/tollgate/killer/kill_level.lua",
    "task/tollgate/killer/mibao_head.lua",
    "missions/challengeoftime/chuangguang30.lua",
    "vng_event/thapnienlenhbai/lenhbai_def.lua",
    "task/metempsychosis/task_head.lua",
]

RA_THIEU = os.path.join(os.path.dirname(os.path.abspath(__file__)), "thieu_vatpham.txt")


def _gon(s):
    """chuan hoa CHUOI DA LA UNICODE."""
    return re.sub(r"[\s\(\)\-]+", " ", s).strip().lower()


def chuan(s):
    """chuoi lay TU TEP (TCVN3) -> dich roi chuan hoa."""
    return _gon(tcvn2uni(s))


def chuan_ten(s):
    """ten viet san trong tep .py nay - DA la Unicode, KHONG duoc dich lai.
    (Loi da mac: goi tcvn2uni cho chuoi Unicode lam hong dau, khien mon
     'Thien Son Thanh Thuy (dai)' khong khop du ten giong het.)"""
    return _gon(s)


def bang_vatpham():
    co = {}
    ten_theo_ma = {}
    for l in io.open(MS, "rb").read().decode("latin-1").replace("\r\n", "\n").split("\n")[1:]:
        c = [x.strip() for x in l.split("\t")]
        if len(c) > 3 and c[0] and c[3].isdigit():
            co.setdefault(chuan(c[0]), (c[1], c[2], c[3]))
            ten_theo_ma.setdefault((c[1], c[2], c[3]), tcvn2uni(c[0]))
    return co, ten_theo_ma


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t01_nan_ma_thuong - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    co, ten_ma = bang_vatpham()

    # chot truoc: 5 ma moi PHAI ton tai va dung ten
    for _, moi, ten in NAN:
        s = re.findall(r"\d+", moi)
        k = (s[0], s[1], s[2])
        that = ten_ma.get(k, "")
        if _gon(that) != chuan_ten(ten):
            print("!!! LOI TO: ma moi (%s) ben du an la %r, mong %r"
                  % (",".join(k), that, ten))
            return 1
    print("  chot: 5 ma dich deu ton tai va DUNG TEN trong magicscript")

    ketqua = []
    tong = 0
    for rel in TEP:
        p = os.path.join(S, rel.replace("/", os.sep))
        if not os.path.isfile(p):
            print("!!! LOI TO: thieu %s" % rel)
            return 1
        raw = io.open(p, "rb").read().decode("latin-1")
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
        hi0 = sum(1 for c in raw if ord(c) > 127)
        if NHAN in raw:
            print("  %-46s DA VA - bo qua" % os.path.basename(rel))
            continue
        d = raw.split(eol)
        n_tep = 0
        for i, l in enumerate(d):
            if l.strip().startswith("--"):
                continue
            for cu, moi, ten in NAN:
                if not re.search(cu, l):
                    continue
                # NGU CANH: dong phai co ten mon khop, hoac la bang thuong co szName
                m = re.search(r'szName\s*=\s*"([^"]*)"', l)
                if m and chuan(m.group(1)) != chuan_ten(ten):
                    continue     # ten khac -> khong dong toi
                if not m and "tbProp" not in l and "nCount" not in l:
                    continue     # khong phai dong bang thuong
                d[i] = re.sub(cu, moi, d[i])
                n_tep += 1
                tong += 1
                print("     %-40s d%-5d %s" % (os.path.basename(rel), i + 1, ten))
        if n_tep == 0:
            continue
        nd = eol.join(d)
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi (%s)" % rel)
            return 1
        nd = eol.join([("-- " + NHAN + " nan ma vat pham bang thuong theo TEN"
                        " (xem ReverseTools\\cauhinh\\t01_nan_ma_thuong.py)")] + nd.split(eol))
        ketqua.append((p, nd, n_tep, rel))

    print("  => nan %d cho o %d tep" % (tong, len(ketqua)))

    # ---- liet ke mon THIEU vat pham ----
    thieu = {}
    mau = re.compile(r"\{\s*6\s*,\s*([01])\s*,\s*(\d+)")
    for rel in TEP:
        p = os.path.join(S, rel.replace("/", os.sep))
        for i, l in enumerate(io.open(p, "rb").read().decode("latin-1")
                              .replace("\r\n", "\n").split("\n"), 1):
            if l.strip().startswith("--") or "szName" not in l:
                continue
            for m in mau.finditer(l):
                k = (m.group(1), m.group(2))
                if ("6", k[0], k[1]) in ten_ma:
                    continue
                # bo qua 5 ma sap duoc nan
                if any(re.search(cu, "{6,%s,%s" % k) for cu, _, _ in NAN):
                    continue
                tm = re.search(r'szName\s*=\s*"([^"]*)"', l)
                ten = tcvn2uni(tm.group(1)) if tm else "?"
                thieu.setdefault((k[0], k[1], ten), []).append("%s:%d" % (rel, i))
    print("  => con %d mon KHONG co vat pham trong du an (khong dong toi)" % len(thieu))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, nd, n, rel in ketqua:
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if io.open(p, "rb").read().decode("latin-1") != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %-46s (%d cho)" % (rel, n))

    with io.open(RA_THIEU, "w", encoding="utf-8", newline="") as f:
        f.write("DANH SACH VAT PHAM CHUA CO TRONG DU AN (bang thuong dang tro toi)\n")
        f.write("Sinh boi t01_nan_ma_thuong.py - CHU GAME quyet: tao moi / thay the / bo\n")
        f.write("=" * 78 + "\n\n")
        for (dt, pt, ten), noi in sorted(thieu.items(), key=lambda x: x[0][2]):
            f.write("(6,%s,%s)  %s\n" % (dt, pt, ten))
            for x in noi:
                f.write("        %s\n" % x)
            f.write("\n")
    print("  => da ghi danh sach mon thieu: %s" % RA_THIEU)
    print("\nCan KHOI DONG LAI GameServer de nap (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
