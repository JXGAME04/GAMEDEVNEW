# -*- coding: utf-8 -*-
"""t52_va_knpc_onkill.py - CHANG B: sua KNpc.cpp tro moc giet quai sang nha moi.

Sources\\Core\\Src\\KNpc.cpp:1698 dang goi CUNG
  ExecuteScript2("\\script\\tinhnang\\datau\\danhquai.lua", "OnDeathMonsterDaTau", ...)
MOI LAN nguoi choi giet mot NPC. Do la ly do DUY NHAT thu muc tinhnang\\datau
con phai ton tai.

Chang A da: dua ma that sang script\\global\\onkillnpc.lua va rut danhquai.lua
thanh bo chuyen tiep 11 dong. Nen DLL cu van chay dung.

Chang nay: tro thang sang nha moi. Sau khi chu game swap DLL, bo chuyen tiep het
viec va thu muc tinhnang\\datau doi di duoc han.

LUU Y KNpc.cpp la tep DUNG CHUNG client + server (Core\\Src). Dong nay nam
trong #ifdef _SERVER nhung VAN PHAI dung ca hai cau hinh de chac.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp"
DUOI = ".truoc_dondatau"
NEO = ('ExecuteScript2("\\\\script\\\\tinhnang\\\\datau\\\\danhquai.lua",'
       ' "OnDeathMonsterDaTau"')


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t52 sua KNpc.cpp - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    raw = doc(P)
    if "onkillnpc.lua" in raw:
        print("  da sua - bo qua")
        return 0
    dong = raw.split("\n")
    ci = [i for i, l in enumerate(dong) if NEO in l]
    if len(ci) != 1:
        print("!!! LOI TO: moc khop %d dong (can 1)" % len(ci))
        return 1
    i = ci[0]
    cu = dong[i]
    co_cr = cu.endswith("\r")
    thut = cu[:len(cu) - len(cu.lstrip())]
    print("  dong %d, hien tai:" % (i + 1))
    print("    %s" % cu.strip()[:110])

    moi_dong = (
        thut + 'ExecuteScript2("\\\\script\\\\global\\\\onkillnpc.lua",'
        ' "OnPlayerKillNpc", m_Index, m_nLastDamageIdx);'
        '\t// [DONDATAU 30/08] moc "nguoi choi giet NPC".'
    )
    ghi_chu = [
        thut + '// Truoc day moc nay goi thang vao'
        ' \\script\\tinhnang\\datau\\danhquai.lua cua he Da Tau CU.',
        thut + '// He do da go (ban thay = script\\task\\newtask\\tasklink,'
        ' port 15-16/08); phan con',
        thut + '// phai chay moi lan giet quai chi la moc dem cua Ban Dong Hanh,'
        ' nay o onkillnpc.lua.',
        thut + '// CHU Y: ham nay chay MOI LAN co NPC chet vi tay nguoi choi -'
        ' dung them viec nang.',
    ]
    cr = "\r" if co_cr else ""
    dong[i:i + 1] = [x + cr for x in ghi_chu] + [moi_dong + cr]
    nd = "\n".join(dong)

    print("  thay bang:")
    print("    %s" % moi_dong.strip()[:110])
    print("  byte tieng Viet: %d -> %d (giam %d - chu thich cu co dau, moi la"
          " ASCII)" % (hi(raw), hi(nd), hi(raw) - hi(nd)))
    print("  ngoac nhon {} : %d -> %d (phai bang nhau)"
          % (raw.count("{") - raw.count("}"), nd.count("{") - nd.count("}")))
    if (raw.count("{") - raw.count("}")) != (nd.count("{") - nd.count("}")):
        print("!!! LOI TO: can bang ngoac doi")
        return 1

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + DUOI
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI KNpc.cpp (ban cu o %s)" % os.path.basename(sao))
    return 0


if __name__ == "__main__":
    sys.exit(main())
