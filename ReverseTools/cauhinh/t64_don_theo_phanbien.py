# -*- coding: utf-8 -*-
"""t64_don_theo_phanbien.py - don 3 diem bo phan bien bat duoc (da tu kiem lai).

1. congthanhquan.lua - phan bien noi "bi thao Include lib_loidai nhung than tep
   van con 2 loi goi vao thu vien do". TU KIEM LAI: sai mot nua -
   IsArenaBegin va GetArenaBothSides la HAM ENGINE (ScriptFuns.cpp:15553-15554),
   khong phai ham cua lib_loidai. NHUNG EnterBattle thi KHONG co trong
   ScriptFuns.cpp -> dung la mat that.
   Du sao tep nay cung MO COI: ca hai dong tao NPC "Cong thanh quan" deu da
   comment (lib_ctc.lua:243-244), chi con hang FILE_CTHANHQUAN tro toi. Doi han
   di la het chuyen.

2. GLB_MANH_BOSS_SATTHU (ch_chung.lua:120) -> STRONGBOSS_ST (lib_server.lua:23).
   Do that: STRONGBOSS_ST duoc GAN nhung KHONG NOI NAO DOC (grep script/ +
   scriptjx2/ chi ra dung dong gan). Ben tieu thu la he Boss Sat Thu ban Viet,
   da doi sang _dara sang nay. Cong tac khong dieu khien gi -> bo ca hai.

3. Ghi bo sung vao _dara\\NHATKY_DOI.txt 8 tep bi RUT VE KHUNG RONG. Chung nam
   trong _dara duoi duoi .goc nhung khong co muc nhat ky nao - nguoi hoan tac
   sau nay se khong biet.

Mac dinh DIEN TAP; --ghi moi lam that.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402
import doi_tep as dt  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = dt.S
MOC = "[PB 30/08]"
RUT_GON = ["missions/mission02.lua", "missions/mission03.lua",
           "missions/mission04.lua", "missions/mission06.lua",
           "timertask/task04.lua", "timertask/task05.lua",
           "timertask/task06.lua", "timertask/task07.lua"]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def bo_dong(duong, neo, ly_do, ghi):
    p = os.path.join(S, duong.replace("/", os.sep))
    raw = doc(p)
    dong = raw.split("\n")
    ci = [i for i, l in enumerate(dong)
          if re.match(r"\s*%s\s*=" % re.escape(neo), l)]
    if not ci:
        return True, "khong con"
    if len(ci) != 1:
        return False, "khop %d dong" % len(ci)
    i = ci[0]
    cr = "\r" if dong[i].endswith("\r") else ""
    hi0, cb0 = hi(raw), lh.can_bang(raw)
    dong[i:i + 1] = ["-- %s bo %s: %s" % (MOC, neo, ly_do) + cr]
    nd = "\n".join(dong)
    if lh.can_bang(nd) != cb0:
        return False, "can bang tu khoa doi"
    if ghi:
        sao = p + ".truoc_donpb"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(p) != nd:
            return False, "doc lai KHONG khop"
        return True, "da bo dong %d" % (i + 1)
    return True, "se bo dong %d" % (i + 1)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t64 don theo phan bien - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    # --- 1. doi congthanhquan.lua ---
    print("## 1. tinhnang/congthanhchien/congthanhquan.lua")
    duong = "tinhnang/congthanhchien/congthanhquan.lua"
    if not os.path.isfile(os.path.join(S, duong.replace("/", os.sep))):
        print("   khong con - bo qua")
    else:
        goi = [g for g in dt.ai_goi(duong)
               if "lib_ctc.lua" not in g[0]]
        if goi:
            print("   [CHAN] con %d cho goi ngoai lib_ctc:" % len(goi))
            for g in goi[:3]:
                print("          %s:%d | %s" % g)
        else:
            n_t, n_d = dt.dem(duong)
            ok, msg = dt.doi(duong,
                             "NPC 'Cong thanh quan' he CU - ca hai dong tao NPC"
                             " deu da comment (lib_ctc.lua:243-244); than tep"
                             " con goi EnterBattle cua thu vien loi dai da go",
                             ghi=ghi, du_biet_con_goi=True)
            print("   %d dong  %s (chi con hang FILE_CTHANHQUAN o lib_ctc.lua:187"
                  " tro toi - hang do nay tro vao khoang khong, vo hai vi ca hai"
                  " cho dung deu da comment)" % (n_d, msg))
    print()

    # --- 2. bo cong tac chet ---
    print("## 2. cong tac chet GLB_MANH_BOSS_SATTHU / STRONGBOSS_ST")
    ok, msg = bo_dong("cauhinh/ch_chung.lua", "GLB_MANH_BOSS_SATTHU",
                      "he Boss Sat Thu ban Viet da go; STRONGBOSS_ST khong noi"
                      " nao doc", ghi)
    print("   ch_chung.lua   : %s" % msg)
    if not ok:
        return 1
    ok, msg = bo_dong("lib/lib_server.lua", "STRONGBOSS_ST",
                      "chi duoc GAN, khong noi nao DOC (he tieu thu da go)", ghi)
    print("   lib_server.lua : %s" % msg)
    if not ok:
        return 1
    print()

    # --- 3. bo sung nhat ky ---
    print("## 3. ghi bo sung nhat ky cho 8 tep rut ve khung rong")
    thieu = []
    nk = ""
    if os.path.isfile(dt.NHATKY):
        nk = io.open(dt.NHATKY, encoding="utf-8").read()
    for x in RUT_GON:
        if x.replace("/", "\\") not in nk and x not in nk:
            thieu.append(x)
    print("   %d/%d tep chua co muc nhat ky" % (len(thieu), len(RUT_GON)))
    if ghi and thieu:
        for x in thieu:
            dt.ghi_nhat_ky(
                "[2026-08-30] %s  <- RUT VE KHUNG RONG (khong doi tep). Ban goc"
                " o _dara\\script\\%s.goc" % (x, x.replace("/", "\\")))
        print("   DA GHI %d muc" % len(thieu))
    print()
    if not ghi:
        print("DIEN TAP - chua lam gi. Chay lai voi --ghi de lam that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
