# -*- coding: utf-8 -*-
"""t35_quet_mocoi_he_cu.py - quet TEP MO COI trong cac he da bi thay bang ban port.

CAN CU (ghi chu do CHINH CAY SCRIPT de lai - nguon chac chan nhat):
  timerserver.lua:18        "[WLLS port 20/08/2026] da go he lien_dau cu"
  balanghuyen.lua:97-98     "[WLLS port 20/08/2026 go NPC he cu]"
  balanghuyen.lua:79        "[DOT-E1 bo he CTC cu]"
  playerlogin.lua:24        "[DOT-E1 bo he CTC cu ...]"
  startgame.lua:102         "[3HD 25/08] TAT ban VN Phong Lang Do (thay bang ban Linux)"
  dichquan.lua:24           "[TIN SU 21/08] ... Thien Bao Kho tu che cu, KHONG con trong menu"
  item\\lbhtdatau.lua:3-4    "he do da MO COI sau dot port 15-16/08"
  item\\bdh_admin.lua:6      "[29/08] menu partner cu DA GO theo yeu cau chu"
Va git log:
  21e570bf  "bo di tinh nang lien dau co san o du an" (yeu cau cua chu game)
  THICONG_LIENDAU_PORT.md  "D8. Go he cu: xoa hook khoi startgame.lua +
                            timerserver.lua; DOI TEN thu muc tinhnang\\lien_dau"

VIEC: voi moi thu muc/tep thuoc he CU, dem xem con BAO NHIEU CHO GOI. Chia lam:
  (A) 0 cho goi        -> MO COI, doi duoc ngay
  (B) chi con cho goi DA BI COMMENT -> gan mo coi, doi duoc sau khi xac nhan
  (C) con cho goi SONG -> phai go loi goi truoc, hoac ban cu van dang dung

CHI DOC - khong doi gi. Dung --doi-mocoi de doi rieng nhom (A).
"""
import io
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import doi_tep as dt  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

RA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "mocoi_he_cu.txt")

# (duong dan, ten he, can cu)
UNG_VIEN = [
    ("header/liendau.lua", "Lien Dau CU",
     "he lien_dau cu da go (timerserver.lua:18); thu muc tinhnang\\lien_dau"
     " da bi xoa; tep nay sot lai"),
    ("global/npcchucnang/sgliendau.lua", "Lien Dau CU", "NPC su gia lien dau cu"),
    ("global/npcchucnang/sugialiendau.lua", "Lien Dau CU", "NPC su gia lien dau cu"),
    ("global/npcchucnang/sugiakietxuat.lua", "Lien Dau CU", "NPC su gia kiet xuat cu"),

    ("tinhnang/datau", "Da Tau CU",
     "item\\lbhtdatau.lua:3-4 ghi 'he do da MO COI sau dot port 15-16/08'"),
    ("global/npcchucnang/datau.lua", "Da Tau CU", "NPC Da Tau ban cu"),
    ("item/lbhtdatau_cu_1908.lua", "Da Tau CU", "ban cu cua lenh bai Da Tau"),

    ("tinhnang/congthanhchien", "Cong Thanh CU",
     "balanghuyen.lua:79 + playerlogin.lua:24 '[DOT-E1 bo he CTC cu]'"),

    ("tinhnang/phonglangdo", "Phong Lang Do ban VN",
     "startgame.lua:102 '[3HD 25/08] TAT ban VN Phong Lang Do"
     " (thay bang ban Linux)'"),
    ("tinhnang/vuot_ai", "Vuot Ai ban VN",
     "cum 3 hoat dong Linux thay the (BAT_HD3 = 1)"),
    ("tinhnang/boss_satthu", "Boss Sat Thu ban VN",
     "cum 3 hoat dong Linux thay the"),

    ("tinhnang/loidai", "Loi Dai CU", "kiem xem ban JX2 citywar_arena co thay khong"),
    ("tinhnang/loidaihonchien", "Loi Dai Hon Chien", "kiem xem co ban thay khong"),
]


def main():
    doi_mocoi = "--doi-mocoi" in sys.argv[1:]
    ghi_that = "--ghi" in sys.argv[1:]
    print("=== t35_quet_mocoi_he_cu ===")
    print()

    nhom_a, nhom_b, nhom_c = [], [], []
    for duong, he, can_cu in UNG_VIEN:
        p = os.path.join(dt.S, duong.replace("/", os.sep))
        if not os.path.exists(p):
            print("  %-40s (khong ton tai - da go tu truoc)" % duong)
            continue
        n_tep, n_dong = dt.dem(duong)
        goi = dt.ai_goi(duong)
        if not goi:
            nhom_a.append((duong, he, can_cu, n_tep, n_dong, goi))
        else:
            # kiem xem moi cho goi co phai deu da comment khong
            song = [g for g in goi if not g[2].lstrip().startswith("--")]
            if not song:
                nhom_b.append((duong, he, can_cu, n_tep, n_dong, goi))
            else:
                nhom_c.append((duong, he, can_cu, n_tep, n_dong, song))

    def in_nhom(ten, ds, chu_thich):
        print("## %s: %d muc  (%d tep, %d dong)"
              % (ten, len(ds), sum(x[3] for x in ds), sum(x[4] for x in ds)))
        print("   %s" % chu_thich)
        for duong, he, can_cu, n_tep, n_dong, goi in ds:
            print("   %-40s %-24s %3d tep %5d dong  (%d cho goi)"
                  % (duong, he, n_tep, n_dong, len(goi)))
        print()

    in_nhom("A - MO COI (0 cho goi)", nhom_a, "doi duoc ngay")
    in_nhom("B - chi con cho goi DA COMMENT", nhom_b,
            "gan mo coi; doi duoc sau khi xac nhan")
    in_nhom("C - con cho goi SONG", nhom_c,
            "phai go loi goi truoc, hoac ban cu van dang dung")

    with io.open(RA, "w", encoding="utf-8", newline="") as f:
        f.write("QUET TEP MO COI TRONG CAC HE DA BI THAY BANG BAN PORT\n")
        f.write("Sinh boi ReverseTools/cauhinh/t35_quet_mocoi_he_cu.py\n")
        f.write("=" * 78 + "\n\n")
        for ten, ds in (("A - MO COI (0 cho goi)", nhom_a),
                        ("B - chi con cho goi DA COMMENT", nhom_b),
                        ("C - con cho goi SONG", nhom_c)):
            f.write("## %s\n\n" % ten)
            for duong, he, can_cu, n_tep, n_dong, goi in ds:
                f.write("### %s   [%s]\n" % (duong, he))
                f.write("    quy mo : %d tep, %d dong\n" % (n_tep, n_dong))
                f.write("    can cu : %s\n" % can_cu)
                if goi:
                    f.write("    con %d cho goi:\n" % len(goi))
                    for g in goi[:10]:
                        f.write("        %s:%d | %s\n" % g)
                f.write("\n")
    print("=> da ghi %s" % RA)

    if doi_mocoi and nhom_a:
        print()
        print("--- DOI NHOM A ---")
        for duong, he, can_cu, _t, _d, _g in nhom_a:
            ok, msg = dt.doi(duong, "%s - %s" % (he, can_cu), ghi=ghi_that)
            print("  %-40s %s" % (duong, msg))
        if not ghi_that:
            print("  (dien tap - them --ghi de doi that)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
