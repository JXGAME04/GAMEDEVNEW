# -*- coding: utf-8 -*-
"""v29_dongbo_repo.py - Chep NGUOC cac tep DA VA tu cay VAN HANH (E:) ve cay
REPO (D:\\GAMEDEVNEW\\serverscript_jx2) de commit git.

LUAT DU AN: sua tep nao thi up ĐÚNG tep do; hai cay D (repo) va E (van hanh)
LECH NHAU nen phai chep tay. Cac ban va hom nay deu duoc ap thang tren E
(script Lua nap luc chay), nay dua ve repo de git giu lich su.

CHI chep tep DA CO san trong repo (khong tu tao duong moi) va CHI khi noi dung
khac nhau. Nghiem thu tung tep: so byte cao (dau tieng Viet TCVN3) phai giu
nguyen sau khi chep, va doc lai phai khop 100%.

Cac tep settings (TimerTask.txt, settings\\maps\\yandibaozang\\**) chep sang
    D:\\GAMEDEVNEW\\serverscript_jx2\\viemde\\settings\\...
(tu tao thu muc - day la du lieu MOI cua dot nay, chua tung co trong repo).

Mac dinh DIEN TAP; --ghi moi chep that.
"""
import io
import os
import shutil
import sys

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
D = r"D:\GAMEDEVNEW\serverscript_jx2\viemde"

# (duong tuong doi trong E, duong tuong doi trong D)
SCRIPT = [
    ("script\\missions\\yandibaozang\\npc.lua", "script\\missions\\yandibaozang\\npc.lua"),
    ("script\\missions\\yandibaozang\\head.lua", "script\\missions\\yandibaozang\\head.lua"),
    ("script\\missions\\yandibaozang\\saizi.lua", "script\\missions\\yandibaozang\\saizi.lua"),
    ("script\\missions\\yandibaozang\\doubleexp.lua", "script\\missions\\yandibaozang\\doubleexp.lua"),
    ("script\\missions\\yandibaozang\\item\\yandimibao.lua", "script\\missions\\yandibaozang\\item\\yandimibao.lua"),
    ("script\\missions\\yandibaozang\\npc\\yandituteng.lua", "script\\missions\\yandibaozang\\npc\\yandituteng.lua"),
    ("script\\missions\\yandibaozang\\readymap\\ready.lua", "script\\missions\\yandibaozang\\readymap\\ready.lua"),
    ("script\\item\\test_hoatdong_admin.lua", "script\\item\\test_hoatdong_admin.lua"),
]
# tep chua co trong repo nhung thuoc dot nay -> them moi
THEM = [
    ("script\\activitysys\\functionlib.lua", "script\\activitysys\\functionlib.lua"),
    ("settings\\TimerTask.txt", "settings\\TimerTask.txt"),
]
# ca cay bang toa do (94 tep)
CAY = [("settings\\maps\\yandibaozang", "settings\\maps\\yandibaozang")]


def chep(nguon, dich, ghi, ketqua):
    if not os.path.isfile(nguon):
        print("!!! LOI TO: thieu nguon %s" % nguon)
        return False
    a = io.open(nguon, "rb").read()
    if os.path.isfile(dich):
        b = io.open(dich, "rb").read()
        if a == b:
            return True   # da giong, bo qua
    hi = sum(1 for c in a if c > 127)
    ketqua.append((nguon, dich, len(a), hi))
    if ghi:
        thu = os.path.dirname(dich)
        if thu and not os.path.isdir(thu):
            os.makedirs(thu)
        shutil.copy2(nguon, dich)
        c = io.open(dich, "rb").read()
        if c != a:
            print("!!! LOI TO: doc lai KHONG khop: %s" % dich)
            return False
        if sum(1 for x in c if x > 127) != hi:
            print("!!! LOI TO: byte cao doi khi chep: %s" % dich)
            return False
    return True


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== v29_dongbo_repo - %s ===" % ("CHEP THAT" if ghi else "DIEN TAP"))
    ketqua = []

    for a, b in SCRIPT + THEM:
        if not chep(os.path.join(E, a), os.path.join(D, b), ghi, ketqua):
            return 1
    for a, b in CAY:
        goc = os.path.join(E, a)
        if not os.path.isdir(goc):
            print("!!! LOI TO: thieu thu muc %s" % goc)
            return 1
        for thumuc, _, teps in os.walk(goc):
            for ten in teps:
                n = os.path.join(thumuc, ten)
                d = os.path.join(D, b, os.path.relpath(n, goc))
                if not chep(n, d, ghi, ketqua):
                    return 1

    if not ketqua:
        print("  Repo da dong bo - khong co gi de chep.")
        return 0
    print("  %d tep khac nhau:" % len(ketqua))
    for n, d, kt, hi in ketqua[:14]:
        print("    %-46s %7d byte, %4d byte co dau" % (os.path.basename(n), kt, hi))
    if len(ketqua) > 14:
        print("    ... con %d tep nua" % (len(ketqua) - 14))
    if not ghi:
        print("\nDIEN TAP - chua chep. Chay lai voi --ghi de chep that.")
        return 0
    print("  DA CHEP %d tep, doc lai khop + byte cao giu nguyen." % len(ketqua))
    return 0


if __name__ == "__main__":
    sys.exit(main())
