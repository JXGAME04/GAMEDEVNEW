# -*- coding: utf-8 -*-
"""t38_go_he_cu_dot2.py - DOT 2: go cac dong Include/hang CHET cua he cu.

Chi go nhung dong da xac minh: tep goi KHONG dung MOT TEN NAO chi rieng he cu
(do bang t36 sau khi loc cac ten chung 'main'/'no'/'OnTimer'/'NOW_END_SAY'...,
va kiem lai tay bang danh sach 41 ham chi co o 3 thu vien cu).

Cach go: COMMENT lai kem dau moc - dung loi nha (timerserver.lua:18
'[WLLS port 20/08/2026] da go he lien_dau cu'). Giu duoc vet, hoan tac de.

KHONG dung toi cac tep DU LIEU nam nho trong thu muc he cu:
    congthanhchien\\danhsach_bang.lua   (danh sach bang hoi - playerlogin doc)
    congthanhchien\\mapbanghoi.lua      (lenhbaitanthu doc)
    congthanhchien\\log_tax.lua         (mgs2player_from_c doc)
    congthanhchien\\quany.lua           (yaoshang ban JX2 doc)
Bon tep nay la DU LIEU dung chung, khong phai logic he cu.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
MOC = "[GOHECU 30/08]"
DUOI = ".truoc_gohecu"

# (tep, doan neo nguyen van trong dong, ly do)
VA = [
    ("startgame.lua",
     'Include("\\\\script\\\\tinhnang\\\\boss_satthu\\\\lib_boss_st.lua")',
     "Boss Sat Thu ban Viet - cum 3HD ban Linux thay the (BAT_HD3=1);"
     " startgame khong goi ham nao cua thu vien nay"),
    ("startgame.lua",
     'Include("\\\\script\\\\tinhnang\\\\phonglangdo\\\\lib_phonglangdo.lua")',
     "Phong Lang Do ban Viet - da TAT tu 25/08 (startgame.lua:102);"
     " startgame khong goi ham nao cua thu vien nay"),
    ("item/lenhbaiadmin.lua",
     'Include("\\\\script\\\\tinhnang\\\\boss_satthu\\\\lib_boss_st.lua")',
     "menu admin khong goi ham nao cua thu vien nay"),
    ("item/lenhbaiadmin.lua",
     'Include("\\\\script\\\\tinhnang\\\\vuot_ai\\\\lib_vuotai.lua")',
     "menu admin khong goi ham nao cua thu vien nay"),
    ("item/lenhbaiadmin.lua",
     'Include("\\\\script\\\\tinhnang\\\\phonglangdo\\\\lib_phonglangdo.lua")',
     "menu admin khong goi ham nao cua thu vien nay"),
    ("timertask/task07.lua",
     'Include("\\\\script\\\\tinhnang\\\\congthanhchien\\\\lib_ctc.lua")',
     "task07 khong goi ham nao cua lib_ctc (chi dung GameOverLDBH cua loidai)"),
    ("lib/lib_map.lua",
     'OTHER_SGSTHU = "\\\\script\\\\tinhnang\\\\vuot_ai\\\\sugiasatthu.lua"',
     "hang CHET - 7 cho dung deu da comment (startgame\\thanh\\*.lua)"),
    ("lib/lib_map.lua",
     'OTHER_DATAU = "\\\\script\\\\tinhnang\\\\datau\\\\datau.lua"',
     "hang CHET - khong noi nao doc"),
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def can_bang(s):
    t = re.sub(r"--[^\n]*", "", s)
    t = re.sub(r'"[^"]*"', '""', t)
    t = re.sub(r"'[^']*'", "''", t)

    def d(w):
        return len(re.findall(r"\b%s\b" % w, t))
    return (d("function") + d("then") + d("do") - d("elseif")) - d("end")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t38 DOT 2: go Include/hang chet - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    theo_tep = {}
    for tep, neo, ly_do in VA:
        theo_tep.setdefault(tep, []).append((neo, ly_do))

    tong = 0
    for tep, ds in theo_tep.items():
        p = os.path.join(S, tep.replace("/", os.sep))
        raw = doc(p)
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                              - raw.count("\r\n")) else "\n"
        hi0 = sum(1 for c in raw if ord(c) > 127)
        cb0 = can_bang(raw)
        nd = raw
        n_tep = 0
        for neo, ly_do in ds:
            # tim dong CHUA neo va CHUA bi comment
            dong = nd.split(eol)
            chi_so = [i for i, l in enumerate(dong)
                      if neo in l and not l.lstrip().startswith("--")]
            if len(chi_so) != 1:
                print("!!! LOI TO: %s khop %d dong (can 1): %s"
                      % (tep, len(chi_so), neo[:60]))
                return 1
            i = chi_so[0]
            cu = dong[i]
            thut = cu[:len(cu) - len(cu.lstrip())]
            dong[i] = ("%s-- %s %s" % (thut, MOC, ly_do) + eol
                       + "%s-- %s" % (thut, cu.strip()))
            nd = eol.join(dong)
            n_tep = n_tep + 1
            print("  %-26s %s" % (tep, neo[:64]))
            print("  %-26s    -> %s" % ("", ly_do[:80]))
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte tieng Viet doi o %s" % tep)
            return 1
        cb1 = can_bang(nd)
        if cb1 != cb0:
            print("!!! LOI TO: can bang tu khoa Lua doi o %s (%d -> %d)"
                  % (tep, cb0, cb1))
            return 1
        tong = tong + n_tep
        if ghi:
            sao = p + DUOI
            if not os.path.isfile(sao):
                shutil.copy2(p, sao)
            with io.open(p, "wb") as f:
                f.write(nd.encode("latin-1"))
            if doc(p) != nd:
                print("!!! LOI TO: doc lai KHONG khop: %s" % p)
                return 1
            print("  DA GHI %s (can bang %d giu nguyen)" % (tep, cb1))
        print()

    print("=> %d dong" % tong)
    if not ghi:
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
