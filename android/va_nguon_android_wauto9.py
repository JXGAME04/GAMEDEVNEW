# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 WAUTO B2 i e] Nut ban tay "nhat ngay": chay toi nhat ca khi nhan vat KHONG o the chien dau.
#
# Do 00:15 (thu_nhat.sh): dot nhat gui dung (fight=0 pick=1 fpick=1 pvis=800 city=1) nhung ATYPE_PICKUP (CoreShell.cpp) chi CHAY TOI
# vat khi Npc.m_FightMode (cong "PICK-FOLLOW-GATE": bFollowPick && !bOnPK && m_FightMode && !bLBObjDown). Dung trong thanh / vua dung
# chien dau (F) thi cham ban tay khong chay. Sua CHI Android: chap nhan them bCityPick (dot nhat ngay ep bCityPick = 1; nguoi choi bat
# "Nhat trong thanh" thi auto cung chay toi nhat ngoai the chien dau - hop y mobile). Ban PC: nhanh #else giu nguyen dong cu.
# Nguon CoreShell.cpp co byte cao -> sua bang latin-1. Chay lai vo hai.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 WAUTO B2 i e]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


def va_coreshell(s):
    return thay(s, ["\t\t\t\t\tif(pApData->bFollowPick && !pApData->bOnPK && Npc[nNpcIdx].m_FightMode",
                    "\t\t\t\t\t&& !Player[nPlayerIdx].m_sExtAuto.bLBObjDown)"],
                   ["#ifdef JX_ANDROID",
                    "\t\t\t\t\t// %s nut ban tay (JxWAuto_NhatNgay ep bCityPick = 1): chay toi nhat ca khi khong o the chien dau" % DAU,
                    "\t\t\t\t\tif(pApData->bFollowPick && !pApData->bOnPK && (Npc[nNpcIdx].m_FightMode || pApData->bCityPick)",
                    "\t\t\t\t\t&& !Player[nPlayerIdx].m_sExtAuto.bLBObjDown)",
                    "#else",
                    "\t\t\t\t\tif(pApData->bFollowPick && !pApData->bOnPK && Npc[nNpcIdx].m_FightMode",
                    "\t\t\t\t\t&& !Player[nPlayerIdx].m_sExtAuto.bLBObjDown)",
                    "#endif"], "CoreShell.cpp: PICK-FOLLOW-GATE")


va("Sources/Core/Src/CoreShell.cpp", DAU, va_coreshell)
print("xong")
