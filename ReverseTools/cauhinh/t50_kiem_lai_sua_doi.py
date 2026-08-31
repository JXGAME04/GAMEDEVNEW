# -*- coding: utf-8 -*-
"""t50_kiem_lai_sua_doi.py - KIEM LAI moi sua doi hom nay o muc DONG LOGIC.

VI SAO: mot so tep tron kieu xuong dong (co ca CRLF va LF le):
    station.lua     CRLF=615 LF=637   -> 22 dong dung LF le
    timerserver.lua CRLF=822 LF=827   -> 5 dong
    startgame.lua   CRLF=219 LF=224   -> 5 dong
    task01.lua      CRLF=76  LF=77    -> 1 dong
Cac cong cu t38..t45 tach tep bang raw.split("\\r\\n"). Voi tep tron, mot phan
tu co the CHUA NHIEU DONG LOGIC. Khi phan tu do bi thay/xoa la keo theo ca dong
logic khong lien quan.

Cong cu nay so ban sao (.truoc_*) voi ban hien tai theo DONG LOGIC (tach "\\n"),
in ra MOI dong bi mat va moi dong them, de doc lai bang mat xem co dong nao bi
xoa oan khong.

CHI DOC.
"""
import io
import os
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
DUOI = (".truoc_gohecu", ".truoc_boldhc", ".truoc_cfgldhc", ".truoc_portlxw",
        ".truoc_dondatau")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def dong_logic(s):
    return [l.rstrip("\r") for l in s.split("\n")]


def main():
    cap = []
    for goc, _, tep in os.walk(S):
        for t in tep:
            for d in DUOI:
                if t.endswith(d):
                    goc_tep = os.path.join(goc, t[:-len(d)])
                    if os.path.isfile(goc_tep):
                        cap.append((goc_tep, os.path.join(goc, t)))
    print("=== KIEM LAI %d TEP DA SUA (so theo DONG LOGIC) ===" % len(cap))
    print()
    tong_mat = 0
    for moi_p, cu_p in sorted(cap):
        rel = os.path.relpath(moi_p, S)
        cu = dong_logic(doc(cu_p))
        moi = dong_logic(doc(moi_p))
        moi_set = {}
        for l in moi:
            moi_set[l.strip()] = moi_set.get(l.strip(), 0) + 1
        mat = []
        for i, l in enumerate(cu, 1):
            k = l.strip()
            if moi_set.get(k, 0) > 0:
                moi_set[k] = moi_set[k] - 1
            else:
                mat.append((i, l))
        # dong bi mat ma KHONG phai dong da bi comment lai
        cu_txt = "\n".join(moi)
        that_su_mat = [(i, l) for i, l in mat
                       if ("-- " + l.strip()) not in cu_txt
                       and ("--" + l.strip()) not in cu_txt]
        print("## %-46s  %d -> %d dong logic; mat %d (chua bi comment lai: %d)"
              % (rel, len(cu), len(moi), len(mat), len(that_su_mat)))
        for i, l in that_su_mat:
            print("   -%4d| %s" % (i, l.strip()[:104]))
        tong_mat = tong_mat + len(that_su_mat)
        print()
    print("=" * 74)
    print("TONG: %d dong logic bien mat han (khong con o dang comment)."
          % tong_mat)
    print("Doc tung dong tren: dong nao KHONG thuoc khoi dinh go la BAO DONG.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
