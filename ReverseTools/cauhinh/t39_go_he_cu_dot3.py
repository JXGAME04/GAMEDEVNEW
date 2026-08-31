# -*- coding: utf-8 -*-
"""t39_go_he_cu_dot3.py - DOT 3: go LICH cua 4 hoat dong cu TRUNG voi ban Linux.

Chi go 4 hoat dong DA CO BAN THAY, khong mat tinh nang nao:

  sukien_vuotai        -> cum 3HD ban Linux (BAT_HD3 = 1, HD3_Tick)
  sukien_phonglangdo   -> cum 3HD ban Linux (BAT_HD3 = 1, HD3_Tick)
  sukien_congthanh     -> Cong Thanh ban JX2 (BAT_CTC_JX2 = 1, CTC_JX2_Tick)
  sukien_loidaibanghoi -> Loi Dai bang hoi ban JX2 (CTC_JX2_Tick, TimerTask 16/17
                          citywar_arena)

Ca 4 hien DEU DANG TAT (ch_lich.lua: BAT_VUOTAI_VIET / BAT_PHONGLANGDO_VIET /
BAT_CONGTHANH_VIET / BAT_LOIDAI_BANGHOI = 0), nen go la go MA CHET.

KHONG dung toi 4 hoat dong cu KHONG co ban thay (sukien_bosshk,
sukien_hathuyhoang, sukien_trangnguyen, sukien_trongbanghoi) - bao chu quyet.

Lam 4 viec, tren cung mot tep timerserver.lua:
  1. Xoa 4 than ham (ranh gioi do bang can bang tu khoa, khong dem tay).
  2. Xoa 4 nhanh goi `if (G_CFG("BAT_...", 0) == 1) then ... end`, thay bang
     mot dong ghi chu.
  3. Comment 4 dong Include thu vien cu (sau buoc 1+2 la khong con ai dung).
  4. Xoa 4 khoa BAT_* chet khoi cauhinh\\ch_lich.lua (cong tac khong con dieu
     khien gi thi te hon la khong co).

CHOT AN TOAN:
  - can bang tu khoa Lua giu nguyen (xoa ham/khoi can bang thi delta = 0)
  - so byte tieng Viet: KHONG doi hoi bang nhau (dang xoa cau tieng Viet) ma
    doi hoi dung bang so byte cua phan bi xoa
  - sau khi xoa, KHONG con tham chieu nao toi cac ten chi rieng 4 thu vien cu
  - doc lai tep sau khi ghi

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
P = os.path.join(S, "timerserver.lua")
P_LICH = os.path.join(S, "cauhinh", "ch_lich.lua")
MOC = "[GOHECU 30/08]"
DUOI = ".truoc_gohecu"

# (ten ham, khoa cong tac, ban thay the)
HAM = [
    ("sukien_congthanh", "BAT_CONGTHANH_VIET",
     "Cong Thanh ban JX2 (BAT_CTC_JX2 = 1, CTC_JX2_Tick)"),
    ("sukien_loidaibanghoi", "BAT_LOIDAI_BANGHOI",
     "Loi Dai bang hoi ban JX2 (CTC_JX2_Tick + TimerTask 16/17 citywar_arena)"),
    ("sukien_vuotai", "BAT_VUOTAI_VIET",
     "Vuot Ai trong cum 3 hoat dong ban Linux (BAT_HD3 = 1, HD3_Tick)"),
    ("sukien_phonglangdo", "BAT_PHONGLANGDO_VIET",
     "Phong Lang Do trong cum 3 hoat dong ban Linux (BAT_HD3 = 1, HD3_Tick)"),
]

INCLUDE = [
    ('Include("\\\\script\\\\tinhnang\\\\congthanhchien\\\\lib_ctc.lua")',
     "khong con ai dung sau khi go sukien_congthanh"),
    ('Include("\\\\script\\\\tinhnang\\\\loidai\\\\lib_loidai.lua")',
     "khong con ai dung sau khi go sukien_loidaibanghoi"),
    ('Include("\\\\script\\\\tinhnang\\\\vuot_ai\\\\lib_vuotai.lua")',
     "khong con ai dung sau khi go sukien_vuotai"),
    ('Include("\\\\script\\\\tinhnang\\\\phonglangdo\\\\lib_phonglangdo.lua")',
     "khong con ai dung sau khi go sukien_phonglangdo"),
]

# thu vien cu: sau khi go thi khong duoc con tham chieu nao toi ten RIENG cua no
THU_VIEN = ["tinhnang/congthanhchien", "tinhnang/loidai", "tinhnang/vuot_ai",
            "tinhnang/phonglangdo"]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def _chuan(s):
    return re.sub(r"\\+", "\\\\", s.replace("/", "\\").lower())


def ten_rieng(thu_muc):
    """Ten toan cuc CHI thu muc nay dinh nghia (loai ten noi khac cung co)."""
    can = _chuan(thu_muc)
    trong, ngoai = set(), set()
    for goc, _, tep in os.walk(S):
        for t in tep:
            if not t.lower().endswith(".lua"):
                continue
            p = os.path.join(goc, t)
            try:
                d = lh.sach(doc(p))
            except OSError:
                continue
            tap = trong if _chuan(os.path.relpath(p, S)).startswith(can) \
                else ngoai
            for m in re.finditer(r"^\s*function\s+([A-Za-z_]\w*)", d, re.M):
                tap.add(m.group(1))
            for m in re.finditer(r"^\s*([A-Za-z_]\w*)\s*=\s*[{\d\"']", d, re.M):
                tap.add(m.group(1))
    return trong - ngoai


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t39 DOT 3: go lich 4 hoat dong cu - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    raw = doc(P)
    if MOC in raw:
        print("  timerserver.lua DA GO - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                          - raw.count("\r\n")) else "\n"
    cb0 = lh.can_bang(raw)
    hi0 = hi(raw)
    dong = raw.split(eol)
    hi_xoa = 0

    # ---- buoc 1+2: go nhanh goi TRUOC (o tren), roi than ham (o duoi) -------
    # Lam tu DUOI len TREN de chi so dong khong bi xe dich.
    viec = []
    for ten, khoa, thay in HAM:
        r = lh.tim_ham(dong, ten)
        if r is None:
            print("!!! LOI TO: khong do duoc ranh gioi ham %s" % ten)
            return 1
        viec.append((r[0], r[1], "ham", ten, khoa, thay))
        neo = 'G_CFG("%s"' % khoa
        k = lh.tim_khoi(dong, neo)
        if k is None:
            print("!!! LOI TO: khong do duoc nhanh goi %s" % khoa)
            return 1
        viec.append((k[0], k[1], "nhanh", ten, khoa, thay))

    for a, b, loai, ten, khoa, thay in sorted(viec, reverse=True):
        cat = eol.join(dong[a:b + 1])
        if lh.can_bang(cat) != 0:
            print("!!! LOI TO: doan cat KHONG can bang (%s %s dong %d..%d)"
                  % (loai, ten, a + 1, b + 1))
            return 1
        hi_xoa = hi_xoa + hi(cat) + hi(eol) * (b - a)
        thut = dong[a][:len(dong[a]) - len(dong[a].lstrip())]
        if loai == "nhanh":
            thay_the = ["%s-- %s da go lich %s (khoa %s). Ban thay: %s."
                        % (thut, MOC, ten, khoa, thay)]
        else:
            thay_the = ["%s-- %s da go ham %s (%d dong). Ban thay: %s."
                        % (thut, MOC, ten, b - a + 1, thay)]
        dong[a:b + 1] = thay_the
        hi_xoa = hi_xoa - hi(thay_the[0])
        print("  go %-6s %-22s dong %4d..%-4d (%3d dong)"
              % (loai, ten if loai == "ham" else khoa, a + 1, b + 1, b - a + 1))

    # ---- buoc 3: comment 4 dong Include ------------------------------------
    for neo, ly_do in INCLUDE:
        chi_so = [i for i, l in enumerate(dong)
                  if neo in l and not l.lstrip().startswith("--")]
        if len(chi_so) != 1:
            print("!!! LOI TO: Include khop %d dong (can 1): %s"
                  % (len(chi_so), neo[:60]))
            return 1
        i = chi_so[0]
        cu = dong[i]
        thut = cu[:len(cu) - len(cu.lstrip())]
        dong[i] = "%s-- %s %s%s%s-- %s" % (thut, MOC, ly_do, eol, thut,
                                           cu.strip())
        print("  comment Include: %s" % neo[:64])

    nd = eol.join(dong)

    # ---- chot an toan ------------------------------------------------------
    cb1 = lh.can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return 1
    hi1 = hi(nd)
    if hi1 > hi0:
        print("!!! LOI TO: byte tieng Viet TANG (%d -> %d)" % (hi0, hi1))
        return 1
    print()
    print("  can bang tu khoa: %d (giu nguyen)" % cb1)
    print("  byte tieng Viet : %d -> %d (giam %d, deu nam trong doan da go)"
          % (hi0, hi1, hi0 - hi1))

    ma = lh.sach(nd)
    for tm in THU_VIEN:
        con = sorted(t for t in ten_rieng(tm)
                     if re.search(r"\b%s\b" % re.escape(t), ma))
        if con:
            print("!!! LOI TO: con tham chieu toi %s: %s" % (tm, con[:8]))
            return 1
    print("  tham chieu    : khong con ten nao cua 4 thu vien cu")

    # ---- buoc 4: xoa 4 khoa BAT_* chet khoi ch_lich.lua ---------------------
    raw_l = doc(P_LICH)
    eol_l = "\r\n" if raw_l.count("\r\n") >= (raw_l.count("\n")
                                              - raw_l.count("\r\n")) else "\n"
    dong_l = raw_l.split(eol_l)
    xoa_l = []
    for _ten, khoa, thay in HAM:
        chi_so = [i for i, l in enumerate(dong_l)
                  if re.match(r"\s*%s\s*=" % re.escape(khoa), l)]
        if len(chi_so) != 1:
            print("!!! LOI TO: ch_lich.lua khoa %s khop %d dong"
                  % (khoa, len(chi_so)))
            return 1
        xoa_l.append((chi_so[0], khoa, thay))
    for i, khoa, thay in sorted(xoa_l, reverse=True):
        dong_l[i:i + 1] = ["-- %s bo khoa %s: he cu da go, ban thay = %s"
                           % (MOC, khoa, thay)]
        print("  ch_lich.lua: bo khoa %s" % khoa)
    nd_l = eol_l.join(dong_l)

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, moi in ((P, nd), (P_LICH, nd_l)):
        sao = p + DUOI
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(moi.encode("latin-1"))
        if doc(p) != moi:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    return 0


if __name__ == "__main__":
    sys.exit(main())
