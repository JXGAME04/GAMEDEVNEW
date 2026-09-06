#!/usr/bin/env python3
# -*- coding: utf-8 -*-
r"""va_lmbj.py - [VANTIEU 06/09] BO VA nhanh CA NHAN Long Mon Tieu Cuc.

Ba cho sua, deu nam trong cay Lua 5.4 da chuyen:
    <goc>\script\event\longmenbiaoju\head.lua
    <goc>\script\event\longmenbiaoju\taskclass.lua

  1) head.lua:252  "LongMenBiaoJu:Load()"  (than chunk)
       -> Include("\\script\\event\\longmenbiaoju\\lmbj_config.lua")
     Load() goi OB_LoadShareData - ham CHI CO ben Relay. Tren GameServer no la nil nen
     MOI tep Include head.lua deu chet day trang (luaerror_20260806.txt:201-290).
     Sau khi va, viec nap so lieu chuyen sang LongMenBiaoJu:EnsureLoaded() cua
     lmbj_config.lua: chi nap khi that su co OB_LoadShareData, va chi nap mot lan.
     Dong Include nay CUNG chinh la cho nap tep cau hinh moi.

  2) taskclass.lua:639  "TaskClass:Load()"  (than chunk)
       -> chu thich lai
     Load() goi TabFile_Load - khong co ben Relay -> route.txt KHONG BAO GIO duoc nap
     (luaerror_20260806.txt:281-284). Sau khi va, bang tuyen duong duoc nap TRE trong
     LongMenBiaoJu:GetTaskClass() cua lmbj_config.lua.

  3) head.lua GenerateTask - lo thung 1%
     Tong nRate cua RandomTable = 0,9900 (head.lua:18-27) nhung ham boc so bang
     random(1, 10000). Roi vao 9901..10000 (1%) thi khong nhanh nao khop, tbData van la
     nil va dong "getn(tbData.tbRoute)" nem loi -> nguoi choi bam "nhan nhiem vu ngau
     nhien" thinh thoang khong ra gi. Them mot dong lui ve nhiem vu 1 sao
     (RandomTable[1], dung y nType = 1 ma chinh ham da dat san o tren).

Chay:
    set PYTHONIOENCODING=utf-8
    python va_lmbj.py                 # xem truoc, KHONG ghi
    python va_lmbj.py --ghi           # ghi that (tu tao ban luu .truoc_lmbj)
    python va_lmbj.py --goc <duong dan thu muc script>   # doi cay dich

Mac dinh cay dich:
    D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\lua54\script

LUAT: tep nguon JX1 la ANSI/TCVN3 - chi doc/ghi bang encoding="latin-1", newline="".
"""
import os
import sys

GOC = r"D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\lua54\script"
HAU_TO_LUU = ".truoc_lmbj"

if "--goc" in sys.argv:
    GOC = sys.argv[sys.argv.index("--goc") + 1]
GHI = "--ghi" in sys.argv


def doc(p):
    with open(p, "r", encoding="latin-1", newline="") as f:
        return f.read()


def ghi(p, s):
    if os.path.exists(p) and not os.path.exists(p + HAU_TO_LUU):
        with open(p + HAU_TO_LUU, "w", encoding="latin-1", newline="") as f:
            f.write(doc(p))
    with open(p, "w", encoding="latin-1", newline="") as f:
        f.write(s)


DAU = "[VANTIEU 06/09]"        # dau nhan "da va roi"

# (ten, duong dan tuong doi, chuoi cu, chuoi moi)
VA = [
    (
        "1. head.lua: bo LongMenBiaoJu:Load() o than chunk, nap lmbj_config.lua",
        r"event\longmenbiaoju\head.lua",
        "\nLongMenBiaoJu:Load()",
        (
            "\n"
            "-- [VANTIEU 06/09] BO loi goi Load() o than chunk: Load() dung OB_LoadShareData,\n"
            "-- ham CHI CO ben Relay -> tren GameServer no la nil va lam chet ca chuoi Include\n"
            "-- (luaerror_20260806.txt:201-290). Nap so lieu doi sang LongMenBiaoJu:EnsureLoaded()\n"
            "-- trong lmbj_config.lua (goi tre, chi khi that su dang o Relay).\n"
            "-- Dong duoi day cung chinh la cho nap phan cau hinh TU VIET cua nhanh ca nhan.\n"
            'Include("\\\\script\\\\event\\\\longmenbiaoju\\\\lmbj_config.lua")'
        ),
    ),
    (
        "2. taskclass.lua: bo TaskClass:Load() o than chunk (doi sang nap tre)",
        r"event\longmenbiaoju\taskclass.lua",
        "\nTaskClass:Load()",
        (
            "\n"
            "-- [VANTIEU 06/09] BO loi goi Load() o than chunk: Load() dung TabFile_Load, khong\n"
            "-- co ben Relay -> route.txt KHONG BAO GIO duoc nap (luaerror_20260806.txt:281-284).\n"
            "-- Bang tuyen duong nay duoc nap TRE trong LongMenBiaoJu:GetTaskClass()\n"
            "-- (lmbj_config.lua muc 11), dung o lan dau co ai can den no.\n"
            "-- TaskClass:Load()"
        ),
    ),
    (
        "3. head.lua GenerateTask: bit lo thung 1% lam tbData = nil",
        r"event\longmenbiaoju\head.lua",
        "\tlocal idx = random(1, getn(tbData.tbRoute))",
        (
            "\t-- [VANTIEU 06/09] Tong nRate cua RandomTable chi la 0,9900 (head.lua:18-27) ma\n"
            "\t-- so boc la random(1, 10000): roi vao 9901..10000 thi khong nhanh nao khop,\n"
            "\t-- tbData con nil va dong duoi nem loi. Lui ve nhiem vu 1 sao - dung y nType = 1\n"
            "\t-- ma chinh ham da dat san.\n"
            "\ttbData = tbData or self.RandomTable[1]\n"
            "\tlocal idx = random(1, getn(tbData.tbRoute))"
        ),
    ),
]


def main():
    print("cay dich :", GOC)
    print("che do   :", "GHI THAT" if GHI else "xem truoc (them --ghi de ghi)")
    print()
    nOk = 0
    nBo = 0
    nLoi = 0
    for ten, rel, cu, moi in VA:
        p = os.path.join(GOC, rel)
        print("---", ten)
        print("    tep:", p)
        if not os.path.exists(p):
            print("    LOI: khong thay tep")
            nLoi += 1
            continue
        d = doc(p)
        # dau nhan rieng cua tung cho va = 40 ky tu dau cua khoi chu thich moi
        dau = moi.strip().split("\n")[0].strip()
        if dau.find(DAU) >= 0 and d.find(dau) >= 0:
            print("    da va tu truoc -> bo qua")
            nBo += 1
            continue
        n = d.count(cu)
        if n != 1:
            print("    LOI: tim thay %d cho (can dung 1)" % n)
            nLoi += 1
            continue
        d2 = d.replace(cu, moi, 1)
        if GHI:
            ghi(p, d2)
            print("    DA GHI (ban luu: %s)" % (p + HAU_TO_LUU))
        else:
            print("    SE THAY:")
            for dong in cu.split("\n"):
                print("      - " + dong)
            for dong in moi.split("\n"):
                print("      + " + dong)
        nOk += 1
        print()
    print("ket qua: %d cho va, %d bo qua, %d loi" % (nOk, nBo, nLoi))
    return 1 if nLoi else 0


if __name__ == "__main__":
    sys.exit(main())
