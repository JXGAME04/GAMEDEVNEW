# -*- coding: utf-8 -*-
"""t45_bo_loidaihonchien.py - BO HAN hoat dong LOI DAI HON CHIEN.

Chu game quyet 30/08: "Loi Dai Hon Chien bo luon".

Hoat dong nay KHONG co ban Linux thay the - day la BO HAN mot tinh nang, khong
phai thay bang ban khac. Van DOI chu khong xoa, hoan tac duoc.

Da tu kiem truoc khi lam:
  - Ban do 210 = "Dien vo truong" (settings\\MapList.ini:2285-2286). Grep toan
    cay: KHONG hoat dong nao khac dung ban do nay. Cac cho khac co so 210 la ma
    ky nang / ma qua / gia tien, khong lien quan.
  - mainloidai.lua:58 ghi "210 cung map data, Tho Dia Phu da chan" -> ngoai NPC
    bao danh ra khong co duong nao vao 210.
  - TASK_DSK (dong 460 cong 20 diem) la task DUNG CHUNG toan game (tho ren, le
    quan, tien trang, lenh bai tan thu...) -> CHI go cho cong diem, TUYET DOI
    khong dung toi task.
  - Vat pham 4844 (Ho Mach Don) va manh hoang kim 753-770 / 903-942 deu dung
    chung voi hoat dong khac -> khong dung toi bang vat pham.

Nam viec:
  1. timerserver.lua: xoa ham LoiDaiHonChien (201 dong), nhanh goi
     BAT_LOIDAI_HONCHIEN, va ham phu LDHC_CFG (chi ham tren dung).
  2. cauhinh\\ch_lich.lua: bo khoa BAT_LOIDAI_HONCHIEN + LDHC_PHI_BAODANH.
  3. cauhinh\\ch_thuong.lua: bo 4 khoa LDHC_*.
  4. startgame\\thon\\balanghuyen.lua: comment dong AddNpcNew tao NPC bao danh.
  5. Doi thu muc script\\tinhnang\\loidaihonchien\\ sang _dara.

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
MOC = "[BOLDHC 30/08]"
DUOI = ".truoc_boldhc"

KHOA_LICH = ["BAT_LOIDAI_HONCHIEN", "LDHC_PHI_BAODANH"]
KHOA_THUONG = ["LDHC_EXP_COMAT", "LDHC_SL_HOMACH_COMAT", "LDHC_EXP_QUANQUAN",
               "LDHC_SL_MANH_HOANGKIM"]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def ghi_tep(p, nd, ghi):
    if not ghi:
        return True
    sao = p + DUOI
    if not os.path.isfile(sao):
        shutil.copy2(p, sao)
    with io.open(p, "wb") as f:
        f.write(nd.encode("latin-1"))
    return doc(p) == nd


def bo_khoa(duong, khoa, ghi):
    p = os.path.join(S, duong.replace("/", os.sep))
    raw = doc(p)
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                          - raw.count("\r\n")) else "\n"
    dong = raw.split(eol)
    cat = []
    for k in khoa:
        ci = [i for i, l in enumerate(dong)
              if re.match(r"\s*%s\s*=" % re.escape(k), l)]
        if not ci:
            print("  %-22s khong con - bo qua" % k)
            continue
        if len(ci) != 1:
            print("!!! LOI TO: %s khop %d dong" % (k, len(ci)))
            return None
        cat.append((ci[0], k))
    for i, k in sorted(cat, reverse=True):
        dong[i:i + 1] = ["-- %s bo khoa %s: hoat dong Loi Dai Hon Chien da bo han"
                         % (MOC, k)]
        print("  %-22s bo dong %d" % (k, i + 1))
    nd = eol.join(dong)
    if not ghi_tep(p, nd, ghi):
        print("!!! LOI TO: doc lai KHONG khop %s" % duong)
        return None
    return nd


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t45 BO HAN Loi Dai Hon Chien - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    # ---- 1. timerserver.lua ----
    print("## 1. timerserver.lua")
    p = os.path.join(S, "timerserver.lua")
    raw = doc(p)
    if MOC in raw:
        print("  da bo - bo qua")
    else:
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                              - raw.count("\r\n")) else "\n"
        cb0 = lh.can_bang(raw)
        dong = raw.split(eol)
        viec = []
        for ten in ("LoiDaiHonChien", "LDHC_CFG"):
            r = lh.tim_ham(dong, ten)
            if r is None:
                print("!!! LOI TO: khong do duoc ham %s" % ten)
                return 1
            viec.append((r[0], r[1], "ham " + ten))
        k = lh.tim_khoi(dong, 'G_CFG("BAT_LOIDAI_HONCHIEN"')
        if k is None:
            print("!!! LOI TO: khong do duoc nhanh goi")
            return 1
        viec.append((k[0], k[1], "nhanh goi BAT_LOIDAI_HONCHIEN"))
        for a, b, ten in sorted(viec, reverse=True):
            if lh.can_bang(eol.join(dong[a:b + 1])) != 0:
                print("!!! LOI TO: %s cat khong can bang" % ten)
                return 1
            thut = dong[a][:len(dong[a]) - len(dong[a].lstrip())]
            dong[a:b + 1] = [
                "%s-- %s da bo %s (%d dong)." % (thut, MOC, ten, b - a + 1),
                "%s-- Hoat dong Loi Dai Hon Chien bo han theo quyet dinh chu game"
                " 30/08." % thut,
                "%s-- Khong co ban Linux thay the. Ban goc o"
                " _dara\\script\\tinhnang\\loidaihonchien." % thut,
            ]
            print("  bo %-32s dong %4d..%-4d (%3d dong)"
                  % (ten, a + 1, b + 1, b - a + 1))
        nd = eol.join(dong)
        if lh.can_bang(nd) != cb0:
            print("!!! LOI TO: can bang tu khoa doi (%d -> %d)"
                  % (cb0, lh.can_bang(nd)))
            return 1
        ma = lh.sach(nd)
        for t in ("LoiDaiHonChien", "LDHC_CFG", "LDHC_EXP_COMAT",
                  "LDHC_SL_HOMACH_COMAT", "LDHC_EXP_QUANQUAN",
                  "LDHC_SL_MANH_HOANGKIM", "BAT_LOIDAI_HONCHIEN"):
            if re.search(r"\b%s\b" % re.escape(t), ma):
                print("!!! LOI TO: con tham chieu %s" % t)
                return 1
        print("  khong con tham chieu LoiDaiHonChien / LDHC_* /"
              " BAT_LOIDAI_HONCHIEN")
        # TASK_DSK phai VAN CON (dung chung toan game)
        if not re.search(r"\bTASK_DSK\b", ma):
            print("  chu y: timerserver.lua khong con nhac TASK_DSK"
                  " (dung - chi Loi Dai Hon Chien dung o tep nay)")
        if not ghi_tep(p, nd, ghi):
            print("!!! LOI TO: doc lai KHONG khop timerserver.lua")
            return 1
        print("  can bang %d giu nguyen  (%s)"
              % (cb0, "da ghi" if ghi else "se ghi"))
    print()

    # ---- 2 + 3. bo khoa cau hinh ----
    print("## 2. cauhinh/ch_lich.lua")
    if bo_khoa("cauhinh/ch_lich.lua", KHOA_LICH, ghi) is None:
        return 1
    print()
    print("## 3. cauhinh/ch_thuong.lua")
    if bo_khoa("cauhinh/ch_thuong.lua", KHOA_THUONG, ghi) is None:
        return 1
    print()

    # ---- 4. NPC bao danh ----
    print("## 4. startgame/thon/balanghuyen.lua - NPC bao danh")
    p = os.path.join(S, "startgame", "thon", "balanghuyen.lua")
    raw = doc(p)
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                          - raw.count("\r\n")) else "\n"
    dong = raw.split(eol)
    neo = "loidaihonchien\\\\mainloidai.lua"
    ci = [i for i, l in enumerate(dong)
          if neo in l and not l.lstrip().startswith("--")]
    if not ci:
        print("  da comment - bo qua")
    elif len(ci) != 1:
        print("!!! LOI TO: khop %d dong" % len(ci))
        return 1
    else:
        hi0, cb0 = hi(raw), lh.can_bang(raw)
        i = ci[0]
        cu = dong[i]
        thut = cu[:len(cu) - len(cu.lstrip())]
        dong[i] = ("%s-- %s NPC bao danh Loi Dai Hon Chien - hoat dong da bo han"
                   " (quyet dinh chu game 30/08)" % (thut, MOC)
                   + eol + "%s-- %s" % (thut, cu.strip()))
        nd = eol.join(dong)
        if lh.can_bang(nd) != cb0 or hi(nd) != hi0:
            print("!!! LOI TO: can bang / byte tieng Viet doi")
            return 1
        if not ghi_tep(p, nd, ghi):
            print("!!! LOI TO: doc lai KHONG khop balanghuyen.lua")
            return 1
        print("  dong %d: da comment AddNpcNew  (%s)"
              % (i + 1, "da ghi" if ghi else "se ghi"))
    print()

    # ---- 5. doi thu muc ----
    print("## 5. doi thu muc tinhnang/loidaihonchien")
    n_tep, n_dong = dt.dem("tinhnang/loidaihonchien")
    goi = [g for g in dt.ai_goi("tinhnang/loidaihonchien")
           if "balanghuyen.lua" not in g[0]]
    if goi:
        print("  [CHAN] con %d cho goi:" % len(goi))
        for g in goi[:6]:
            print("         %s:%d | %s" % g)
        return 1
    ok, msg = dt.doi("tinhnang/loidaihonchien",
                     "Loi Dai Hon Chien - BO HAN theo quyet dinh chu game 30/08."
                     " Khong co ban Linux thay the.",
                     ghi=ghi, du_biet_con_goi=True)
    print("  tinhnang/loidaihonchien  %d tep %d dong  %s"
          % (n_tep, n_dong, msg))
    print()
    if not ghi:
        print("DIEN TAP - chua lam gi. Chay lai voi --ghi de lam that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
