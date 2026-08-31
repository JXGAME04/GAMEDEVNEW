# -*- coding: utf-8 -*-
"""t60_doi_ungvien_chet.py - doi 9 ung vien DA KIEM KY (du 8 duong nap).

Khac voi y dinh ban dau la "quet toan cay roi don hang loat": t57 do duoc
1205/3065 tep khong ai nhac ten, nhung con so do KHONG dung de xoa hang loat
(xem chu thich trong t59). Nen chi dong den nhung ung vien DA DOC TAY, biet ro
la tinh nang gi, va da kiem du 8 duong nap.

Ket qua kiem (t59 + kiem tay):

  header\\loidai.lua                161 dong. He "Loi Dai CBT" cu - KHAC Loi Dai
                                   Hon Chien va KHAC Loi Dai bang hoi. Dong 9
                                   tro toi \\script\\feature\\loidaicbt\\xaphu.lua
                                   ma thu muc script\\feature KHONG TON TAI.
                                   Ban sao song cua BW_COMPETEMAP dang dung nam
                                   o missions\\bw\\bwhead.lua:9.
  startgame\\khuvucbang\\            17 tep, 834 dong. Lanh dia/khu vuc bang hoi
                                   ban cu; ban thay = scriptjx2\\tong_vn (200 tep)
                                   + startgame\\tongjx2npc.lua. Chua
                                   bossbanghoi1-5.lua ban cu (Boss bang hoi da
                                   port lai 21/08, ban cu o _backup_bossbanghoi_2108).
  event\\demhuyhoang\\               3 tep, 208 dong  ) hai ban Dem Huy Hoang cu;
  event\\event_demhuyhoang\\         6 tep, 248 dong  ) ban thu ba dang chay la
                                   event\\event_huyhoang_dungdb (timerserver.lua:26).
  global\\thanh\\npc\\add_npc.lua      244 dong. Bang tbNpcSatThu (boss sat thu
                                   "nua port" map 995). Hai ham add_npc_thanh /
                                   add_boss KHONG noi nao goi (do that: 0 cho).
  item\\ib\\{hoangkim,bachngan,      4 tep, 68 dong. The Lien Dau CU. Bang vat pham
   thanhdong,hanthiet}.lua         co 13 dong tro vao item\\ib\\ nhung KHONG dong
                                   nao tro 4 tep nay (WLLS port 20/08 da doi sang
                                   leaguematch\\item\\honour.lua).

Moi ung vien: 0 duong dan trong script/settings, 0 cho trong C++ (cac cho C++
bat duoc deu la COMMENT va khop nham chuoi con), 0 ten trong du lieu ban do.

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
    ("header/loidai.lua",
     "he 'Loi Dai CBT' cu - mo coi; :9 tro toi script\\feature khong ton tai"),
    ("startgame/khuvucbang",
     "lanh dia/khu vuc bang hoi ban cu - ban thay: scriptjx2\\tong_vn (JX2)"),
    ("event/demhuyhoang",
     "Dem Huy Hoang ban 1 - ban dang chay: event\\event_huyhoang_dungdb"),
    ("event/event_demhuyhoang",
     "Dem Huy Hoang ban 2 - ban dang chay: event\\event_huyhoang_dungdb"),
    ("global/thanh/npc/add_npc.lua",
     "bang tbNpcSatThu boss sat thu 'nua port' - 2 ham khong ai goi"),
    ("item/ib/hoangkim.lua",
     "the Lien Dau CU - bang vat pham da doi sang leaguematch\\item\\honour.lua"),
    ("item/ib/bachngan.lua",
     "the Lien Dau CU - bang vat pham da doi sang leaguematch\\item\\honour.lua"),
    ("item/ib/thanhdong.lua",
     "the Lien Dau CU - bang vat pham da doi sang leaguematch\\item\\honour.lua"),
    ("item/ib/hanthiet.lua",
     "the Lien Dau CU - bang vat pham da doi sang leaguematch\\item\\honour.lua"),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t60 doi 9 ung vien da kiem ky - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    tong_tep = tong_dong = 0
    chan = 0
    for duong, ly_do in DS:
        p = os.path.join(dt.S, duong.replace("/", os.sep))
        if not os.path.exists(p):
            print("  %-32s khong ton tai - bo qua" % duong)
            continue
        n_tep, n_dong = dt.dem(duong)
        goi = dt.ai_goi(duong)
        if goi:
            print("  [CHAN] %-26s CON %d CHO GOI:" % (duong, len(goi)))
            for g in goi[:4]:
                print("         %s:%d | %s" % g)
            chan += 1
            continue
        ok, msg = dt.doi(duong, ly_do, ghi=ghi)
        print("  %-32s %2d tep %5d dong  %s" % (duong, n_tep, n_dong, msg))
        if ok:
            tong_tep += n_tep
            tong_dong += n_dong
    print()
    print("=> %d tep, %d dong" % (tong_tep, tong_dong))
    if chan:
        print("=> %d muc BI CHAN" % chan)
    if not ghi:
        print()
        print("DIEN TAP - chua doi gi. Chay lai voi --ghi de lam that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
