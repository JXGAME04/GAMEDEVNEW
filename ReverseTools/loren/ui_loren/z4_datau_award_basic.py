# -*- coding: utf-8 -*-
"""z4_datau_award_basic.py - Thay bang thuong ngau nhien Da Tau theo ban Linux.

CHU GAME chot: thay `award_basic` nhu ban Linux (du nguyen lieu lo ren) + giu
them Huyen Thien Chuy va Que Hoa Tuu; moi ID phai DOI CHIEU voi bang du an.

CACH DOI CHIEU (dung luat "nan theo TEN", khong tru 1 mu):
  - Quality=1 (trang bi Hoang Kim An Bang/Dinh Quoc/Hiep Cot/Nhu Tinh):
      ten -> chi so dong goldequip.txt cua DU AN. Nghiem thu cheo: cung mot
      mon, chi so tra ra phai TRUNG voi award_link dang chay tot
      (An Bang Bang Tinh 163, Dinh Quoc Thanh Sa 158, An Bang Ke Huyet 166) - DA KHOP.
  - Genre 6: ten -> magicscript.txt (6,1,ptc). Ba ca dac biet:
      * "Huyen Tinh cap N": ten JX1 la "Huyen Tinh Khoang Thach" (146), cap nam
        o cot Level -> map rieng, GIU ten hien thi "Huyen Tinh cap N" cua Linux.
      * "Tien Thao Lo": doi chieu theo MA (71) + kiem ten JX1 o ma do phai chua
        "Thao Lo" (chinh ta hai ben lech nhe).
      * "Mai Khoi Hoa Vu" (6,0,20): du an doi ten thanh "Hoa Hong" (6,1,20) -
        dung ten + ma cua DU AN.
  - Genre 4 (Tinh Hong Bao Thach): ten -> questkey.txt detail cua du an.
  - MOI ca khong khop -> DUNG LAI BAO LOI, khong ghi gi (luat nan-ma).

TaskValue giu nguyen cua Linux (trong so quay thuong theo gia tri).
Them 2 mon Viet giu lai (dung nguyen dong dang chay): Huyen Thien Chuy x5
(gia 3tr) + Que Hoa Tuu x1 (gia 500k).

KHONG PHAI BUILD - chi la bang du lieu; khoi dong lai may chu de nap.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_datau lan dau).
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import tcvn2uni  # noqa: E402

B = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item"
GOC = r"D:\ServerLinux\server1\settings\task\award_basic.txt"
DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\task\award_basic.txt"
HAU_TO = ".truoc_datau"


def chuan(s):
    return re.sub(r"\s+", " ", tcvn2uni(s)).strip().lower()


def doc_bang(p):
    return [l for l in io.open(p, encoding="latin-1").read()
            .replace("\r\n", "\n").split("\n") if l.strip()]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== z4_datau_award_basic - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    # ---- bang tra cua DU AN ----
    ms_ten = {}     # chuan(ten) -> (ptc, ten_tcvn3)
    ms_ma = {}      # ptc -> ten_tcvn3
    for l in doc_bang(os.path.join(B, "magicscript.txt"))[1:]:
        c = [x.strip() for x in l.split("\t")]
        if len(c) > 3 and c[0] and c[1] == "6" and c[2] == "1" and c[3].isdigit():
            ms_ten.setdefault(chuan(c[0]), (c[3], c[0]))
            ms_ma.setdefault(int(c[3]), c[0])
    gold = {}
    for i, l in enumerate(doc_bang(os.path.join(B, "goldequip.txt"))[1:]):
        c = [x.strip() for x in l.split("\t")]
        if c and c[0]:
            gold.setdefault(chuan(c[0]), (i, c[0]))
    qk = {}
    for l in doc_bang(os.path.join(B, "questkey.txt"))[1:]:
        c = [x.strip() for x in l.split("\t")]
        if len(c) > 2 and c[0]:
            qk.setdefault(chuan(c[0]), (c[2], c[0]))

    goc = doc_bang(GOC)
    hdr_l = [h.strip() for h in goc[0].split("\t")]
    ix = {h: i for i, h in enumerate(hdr_l)}

    rows = []       # (Name_tcvn3, Quality, Genre, Detail, Particular, GoodsFive, Level, Magiclevel, TaskValue)
    loi = []
    for r, l in enumerate(goc[1:], 2):
        c = [x.strip() for x in l.split("\t")]
        while len(c) < len(hdr_l):
            c.append("")
        nm = c[ix["Name"]]
        q = c[ix["Quality"]] or "0"
        g = c[ix["Genre"]]
        pt = c[ix["Particular"]]
        se = c[ix["GoodsFive"]] or "0"
        lv = c[ix["Level"]] or "0"
        mg = c[ix["Magiclevel"]] or "0"
        tv = c[ix["TaskValue"]]
        k = chuan(nm)

        if q == "1":
            hit = gold.get(k)
            if hit is None:
                loi.append((r, tcvn2uni(nm), "goldequip khong co"))
                continue
            rows.append((nm, "1", str(hit[0]), "0", "0", se, lv, mg, tv))
        elif g == "6":
            if k.startswith("huy\u1ec1n tinh c\u1ea5p"):
                # dac biet: Huyen Tinh cap N -> (6,1,146), cap o Level, GIU ten Linux
                if 146 not in ms_ma or "huy" not in chuan(ms_ma[146]):
                    loi.append((r, tcvn2uni(nm), "ma 146 khong phai Huyen Tinh"))
                    continue
                rows.append((nm, "0", "6", "1", "146", se, lv, mg, tv))
            elif k == "ti\u00ean th\u1ea3o l\u1ed9":
                ten71 = ms_ma.get(71, "")
                if "th\u1ea3o l" not in chuan(ten71):
                    loi.append((r, tcvn2uni(nm), "ma 71 ben du an la %r" % tcvn2uni(ten71)))
                    continue
                rows.append((ten71, "0", "6", "1", "71", se, lv, mg, tv))
            elif k == "mai kh\u00f4i hoa v\u0169":
                ten20 = ms_ma.get(20, "")
                if "hoa h\u1ed3ng" not in chuan(ten20):
                    loi.append((r, tcvn2uni(nm), "ma 20 ben du an la %r" % tcvn2uni(ten20)))
                    continue
                rows.append((ten20, "0", "6", "1", "20", se, lv, mg, tv))
            else:
                hit = ms_ten.get(k)
                if hit is None:
                    loi.append((r, tcvn2uni(nm), "magicscript khong co (Linux p%s)" % pt))
                    continue
                rows.append((hit[1], "0", "6", "1", hit[0], se, lv, mg, tv))
        elif g == "4":
            hit = qk.get(k)
            if hit is None:
                loi.append((r, tcvn2uni(nm), "questkey khong co"))
                continue
            rows.append((hit[1], "0", "4", hit[0], pt or "1", se, lv, mg, tv))
        else:
            loi.append((r, tcvn2uni(nm), "genre la %s" % g))

    if loi:
        print("!!! %d dong KHONG doi chieu duoc - DUNG LAI, khong ghi gi:" % len(loi))
        for r, nm, ly in loi:
            print("    hang %-3d %-36s %s" % (r, nm[:36], ly))
        return 1
    print("  doi chieu 98/98 dong Linux -> %d dong (ten + ma theo bang DU AN)" % len(rows))

    # ---- 2 mon Viet giu lai: lay NGUYEN VAN dong dang chay ----
    cu = doc_bang(DICH)
    hdr_j = cu[0]
    hdr_jl = [h.strip() for h in hdr_j.split("\t")]
    if hdr_jl != hdr_l:
        print("!!! LOI TO: header hai bang khac nhau:\n   JX1  : %s\n   LINUX: %s"
              % (hdr_jl, hdr_l))
        return 1
    giu = []
    for l in cu[1:]:
        c = [x.strip() for x in l.split("\t")]
        u = chuan(c[0]) if c else ""
        if u in ("huy\u1ec1n thi\u00ean ch\u00f9y", "qu\u1ebf hoa t\u1eedu"):
            giu.append(l)
    print("  giu lai %d dong Viet (Huyen Thien Chuy + Que Hoa Tuu)" % len(giu))
    if not (5 <= len(giu) <= 7):
        print("!!! LOI TO: mong 6 dong (5 HTC + 1 QHT), thay %d" % len(giu))
        return 1

    # ---- ghep tep moi ----
    ra = [hdr_j]
    for x in rows:
        ra.append("\t".join(x))
    ra += giu
    nd = "\r\n".join(ra) + "\r\n"
    try:
        nd.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ngoai latin-1: %s" % e)
        return 1
    print("  tep moi: %d dong du lieu (98 Linux-nan + %d Viet)" % (len(ra) - 1, len(giu)))

    # ---- chot cuoi: moi dong genre6 phai co ten TRUNG voi ten du an tai ma do ----
    sai = 0
    for x in rows:
        if x[2] == "6" and x[4].isdigit():
            ten_da = ms_ma.get(int(x[4]), "")
            if not ten_da:
                sai += 1
            elif chuan(x[0])[:8] not in chuan(ten_da) and chuan(ten_da)[:8] not in chuan(x[0]):
                # Huyen Tinh cap N giu ten hien thi rieng - cham truoc "huyền tinh"
                if not chuan(x[0]).startswith("huy\u1ec1n tinh"):
                    print("    ?? %-30s ma %s = %r" % (tcvn2uni(x[0])[:30], x[4], tcvn2uni(ten_da)[:26]))
                    sai += 1
    print("  chot ten<->ma genre6: %d cho kha nghi (mong 0)" % sai)
    if sai:
        return 1

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. Khoi dong lai may chu de nap bang moi.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
