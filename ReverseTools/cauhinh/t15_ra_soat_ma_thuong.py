# -*- coding: utf-8 -*-
"""t15_ra_soat_ma_thuong.py - RA SOAT LAI toan bo ma vat pham trong bang thuong,
tra bang DUNG CACH MAY TRA (theo chi so), va doi khop ten GAN DUNG.

VI SAO PHAI LAM LAI: bo phan bien chi ra hai loi trong bo kiem cu -
 1) tra bang theo COT ParticularType, trong khi may tra theo CHI SO DONG
    (KItemGenerator.CPP:1660 -> KBasPropTbl.cpp:1058). Hai thu nay LECH nhau o
    35 dong ke tu chi so 4881.
 2) doi khop ten qua chat: bao "Dai Thanh Bi Kip 150" la KHONG CO trong du an,
    trong khi bang co "Dai Thanh Bi Kip cap 150" (chi so 3208) - khac moi chu
    "cap", va script con go sai dau (Thanh sac vs Thanh huyen). Suyt nua chu
    game them mot mon TRUNG TEN vao bang.

CHI DOC - khong sua tep nao. Sinh ra `ra_soat_ma_thuong.txt`.
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import bang_vatpham as bv  # noqa: E402

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
S = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script")
RA = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                  "ra_soat_ma_thuong.txt")

TEP = [
    "missions/boss/bigboss.lua",
    "task/tollgate/killer/kill_level.lua",
    "task/tollgate/killer/mibao_head.lua",
    "missions/challengeoftime/chuangguang30.lua",
    "vng_event/thapnienlenhbai/lenhbai_def.lua",
    "task/metempsychosis/task_head.lua",
]

MAU = re.compile(r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)")
MAU_TEN = re.compile(r'szName\s*=\s*"([^"]*)"')


def main():
    cs, tt, ttm, lech = bv.nap()
    bv.bao_cao_lech(lech, cs)
    print()

    sys.path.insert(0, os.path.join(r"D:\GAMEDEVNEW\ReverseTools", "viemde"))
    from bangtxt import tcvn2uni

    khop = []
    gan = []
    thieu = []
    for rel in TEP:
        p = os.path.join(S, rel.replace("/", os.sep))
        if not os.path.isfile(p):
            print("  bo qua (khong co): %s" % rel)
            continue
        raw = io.open(p, "rb").read().decode("latin-1").replace("\r\n", "\n")
        for i, l in enumerate(raw.split("\n"), 1):
            if l.strip().startswith("--") or "szName" not in l:
                continue
            mt = MAU_TEN.search(l)
            if not mt:
                continue
            ten = tcvn2uni(mt.group(1)).strip()
            for m in MAU.finditer(l):
                g, d, p_ = int(m.group(1)), int(m.group(2)), int(m.group(3))
                if g != 6:
                    continue
                that = cs.get(p_)
                k, cach = bv.tim_theo_ten(ten, tt, ttm)
                muc = (rel, i, g, d, p_, ten,
                       that[0] if that else None, k, cach)
                if that and k == p_:
                    khop.append(muc)
                elif k is not None:
                    gan.append(muc)
                else:
                    thieu.append(muc)

    print("  ma DUNG (chi so tra ra dung mon co ten do) : %d" % len(khop))
    print("  ma SAI nhung TIM DUOC mon dung theo ten    : %d" % len(gan))
    print("  ma KHONG tra duoc ten trong du an          : %d" % len(thieu))

    with io.open(RA, "w", encoding="utf-8", newline="") as f:
        f.write("RA SOAT MA VAT PHAM TRONG BANG THUONG\n")
        f.write("Sinh boi ReverseTools/cauhinh/t15_ra_soat_ma_thuong.py"
                " - CHI DOC\n")
        f.write("Tra bang theo CHI SO DONG (dung cach may tra:"
                " KItemGenerator.CPP:1660)\n")
        f.write("=" * 78 + "\n\n")

        f.write("## A) MA SAI - DA TIM DUOC MON DUNG THEO TEN (%d cho)\n"
                % len(gan))
        f.write("Doi ma theo cot 'nen doi thanh' la nguoi choi nhan dung mon"
                " ghi trong bang.\n")
        f.write("!! Luu y: nhung o nay TRUOC DAY khong ra gi (ma sai) - doi"
                " xong la chung\n")
        f.write("   BAT DAU RA DO THAT. Do la thay doi ve kinh te, can chu game"
                " duyet.\n\n")
        for rel, i, g, d, p_, ten, that, k, cach in gan:
            f.write("  %s:%d\n" % (rel, i))
            f.write("      bang ghi   : {%d,%d,%d}  \"%s\"\n" % (g, d, p_, ten))
            f.write("      may tra ra : %s\n" % (that or "(khong co dong nay)"))
            f.write("      nen doi thanh: {6,1,%d}   (%s, khop %s)\n\n"
                    % (k, cs[k][0], cach))

        f.write("\n## B) KHONG TRA DUOC TEN TRONG DU AN (%d cho)\n" % len(thieu))
        f.write("Chu game quyet: tao vat pham moi / thay mon khac / bo khoi"
                " bang thuong.\n")
        f.write("Cot 'ung vien' la nhung mon co ten gan giong, de doi chieu"
                " cho nhanh.\n\n")
        for rel, i, g, d, p_, ten, that, k, cach in thieu:
            uv = [cs[x][0] for x in (cach or [])][:4]
            f.write("  %s:%d\n" % (rel, i))
            f.write("      bang ghi   : {%d,%d,%d}  \"%s\"\n" % (g, d, p_, ten))
            f.write("      may tra ra : %s\n" % (that or "(khong co dong nay)"))
            f.write("      ung vien   : %s\n\n" % (uv if uv else "(khong co)"))

        f.write("\n## C) MA DUNG (%d cho) - khong phai lam gi\n" % len(khop))

    print()
    print("  => da ghi %s" % RA)


if __name__ == "__main__":
    main()
