# -*- coding: utf-8 -*-
"""t37_go_he_cu_dot1.py - DOT 1: doi cac tep MO COI HOAN TOAN cua he cu.

Chi doi nhung tep da xac minh KHONG con cho nao goi (khong NPC nao bind, khong
tep nao Include, khong bang nao tro toi). Doi = chuyen sang _dara, hoan tac duoc.

XAC MINH DA LAM (t35 + t36 + doc tay):
  header\\liendau.lua              0 cho goi. He lien_dau cu da go 20/08
                                  (timerserver.lua:18; THICONG_LIENDAU_PORT.md
                                  muc D8 'DOI TEN thu muc tinhnang\\lien_dau').
                                  Tep nay sot lai.
  global\\npcchucnang\\datau.lua     0 cho goi. Khong AddNpc* nao tro toi
                                  (grep 'AddNpc.*datau' = 0). He Da Tau moi =
                                  script\\task\\newtask\\tasklink (port 15-16/08);
                                  item\\lbhtdatau.lua:3-4 ghi he cu 'da MO COI'.
  item\\lbhtdatau_cu_1908.lua       0 cho goi. Ban cu cua lbhtdatau.lua.
  global\\npcchucnang\\nhieptran.lua 0 cho goi. NPC 'Nhiep Thi Tran' ban Viet bi
                                  hd3_driver.lua:52-67 XOA moi phut. Ban thay =
                                  task\\tollgate\\killer\\nieshichen.lua (Linux).

Mac dinh DIEN TAP; --ghi moi doi that.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import doi_tep as dt  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

DS = [
    ("header/liendau.lua",
     "Lien Dau CU - he da go 20/08 (THICONG_LIENDAU_PORT.md D8), tep sot lai"),
    ("global/npcchucnang/datau.lua",
     "Da Tau CU - khong NPC nao bind; ban thay = task\\newtask\\tasklink"),
    ("item/lbhtdatau_cu_1908.lua",
     "Da Tau CU - ban cu cua item\\lbhtdatau.lua"),
    ("global/npcchucnang/nhieptran.lua",
     "Nhiep Thi Tran ban Viet - hd3_driver xoa NPC moi phut;"
     " ban thay = task\\tollgate\\killer\\nieshichen.lua"),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t37 DOT 1: doi tep mo coi - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    tong_tep = tong_dong = 0
    loi = 0
    for duong, ly_do in DS:
        n_tep, n_dong = dt.dem(duong)
        goi = dt.ai_goi(duong)
        if goi:
            print("  [BO QUA] %-36s CON %d CHO GOI:" % (duong, len(goi)))
            for g in goi[:5]:
                print("           %s:%d | %s" % g)
            loi = loi + 1
            continue
        ok, msg = dt.doi(duong, ly_do, ghi=ghi)
        print("  %-36s %5d dong  %s" % (duong, n_dong, msg))
        if ok:
            tong_tep = tong_tep + n_tep
            tong_dong = tong_dong + n_dong
    print()
    print("  => %d tep, %d dong" % (tong_tep, tong_dong))
    if loi:
        print("  => %d muc BI CHAN vi con cho goi - phai xu ly truoc" % loi)
    if not ghi:
        print()
        print("DIEN TAP - chua doi gi. Chay lai voi --ghi de doi that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
