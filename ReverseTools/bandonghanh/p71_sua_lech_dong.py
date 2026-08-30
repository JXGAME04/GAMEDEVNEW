# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] GOC "item moi khong nhan duoc / vo hinh / dinh chuot":
KItem.cpp:3259 ghi ro - bang magicscript cua JX1 co RECORD INDEX = chinh
ma particular (KItemGenerator::Gen_MagicScript goi GetMagicScript(nParti)).
Nghia la: MOI particular PHAI nam DUNG dong cua no.
Toi da xoa 17 dong (4890..4906) roi them moi o cuoi -> 35 dong lech ->
engine tra ra ban ghi SAI (hoac rong) -> item vo hinh, khong dung duoc.

VA: dung lai bang cho particular == chi so dong:
 - giu nguyen moi dong dang co
 - CHEN dong giu cho cho 17 particular bi thieu (4890..4906) - dat ten
   "Vi tri trong", Script rong, de khong ai nhat duoc
 - sap xep tang dan theo particular
Ap cho CA HAI bang (server + client) va kiem lai 0 dong lech.
"""
import io
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CR = chr(13)
P = [r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt",
     r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\settings\item\magicscript.txt"]

for p in P:
    raw = io.open(p, "r", encoding="latin-1", newline="").read()
    lf = raw.replace(CR + "\n", "\n")
    rows = [d.split("\t") for d in lf.split("\n") if d.strip()]
    header = rows[0]
    data = {}
    for c in rows[1:]:
        if len(c) > 3 and c[3].isdigit():
            data[int(c[3])] = c
    mx = max(data)
    # khuon dong giu cho: lay dong 4874 lam mau
    mau = data.get(4874) or rows[1]
    ra = [header]
    them = 0
    for i in range(0, mx + 1):
        c = data.get(i)
        if c is None:
            c = mau[:]
            c[0] = vn("Vị trí trống")
            c[3] = str(i)
            c[4] = mau[4]
            if len(c) > 8:
                c[8] = vn("Vị trí trống")
            if len(c) > 9:
                c[9] = "0"
            if len(c) > 10:
                c[10] = "0"
            them += 1
        ra.append(c)
    io.open(p, "w", encoding="latin-1", newline="").write(
        ("\n".join("\t".join(x) for x in ra) + "\n").replace("\n", CR + "\n"))
    # kiem lai
    n2 = [d.split("\t") for d in io.open(p, "r", encoding="latin-1").read().replace(CR, "").split("\n") if d.strip()]
    lech = [(i, c[3]) for i, c in enumerate(n2) if len(c) > 3 and c[3].isdigit() and int(c[3]) != i - 1]
    print("%s: %d dong | them giu cho %d | LECH sau khi sua: %d"
          % (p[:2], len(n2), them, len(lech)))
    if lech:
        print("   con lech:", lech[:5])
