# -*- coding: utf-8 -*-
"""t09_dongbo_repo.py - Chep NGUOC cac tep Lua/settings DA VA hom nay tu cay
VAN HANH (E:) ve cay REPO (D:\\GAMEDEVNEW\\serverscript_jx2) de commit git.

LUAT DU AN: hai cay D (repo) va E (van hanh) LECH NHAU; sua tep nao thi up
DUNG tep do. Ban va Lua ap thang tren E (script nap luc chay), nay dua ve repo.

DANH SACH DUNG BANG CAC BO VA HOM NAY:
  t01  5 tep bang thuong (nan ma vat pham)
  t02  giftcode_new.lua, lequan.lua (go bay ghi de kho ma)
  t04  timerserver.lua (nhip nap lai)
  t05b 7 tep MOI script\\cauhinh\\ch_*.lua
  t07  ch_thuong_lib.lua (cong trao thuong)
  v30  yandibaozang\\npc.lua (rao chan toa do)
  + settings\\gamesetting.ini (nhom [Exp] va [Log])

Nghiem thu tung tep: so byte cao (dau tieng Viet TCVN3) phai giu nguyen sau khi
chep, va doc lai phai khop 100%.

Mac dinh DIEN TAP; --ghi moi chep that.
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
D = r"D:\GAMEDEVNEW\serverscript_jx2\taithiet_2908"

TEP = [
    # (duong tuong doi tu thu muc server)
    "script/missions/boss/bigboss.lua",
    "script/task/tollgate/killer/kill_level.lua",
    "script/task/tollgate/killer/mibao_head.lua",
    "script/missions/challengeoftime/chuangguang30.lua",
    "script/vng_event/thapnienlenhbai/lenhbai_def.lua",
    "script/giftcode_new.lua",
    "script/global/npcchucnang/lequan.lua",
    "script/timerserver.lua",
    "script/cauhinh/ch_lib.lua",
    "script/cauhinh/ch_chung.lua",
    "script/cauhinh/ch_lich.lua",
    "script/cauhinh/ch_thuong.lua",
    "script/cauhinh/ch_exp.lua",
    "script/cauhinh/ch_drop.lua",
    "script/cauhinh/ch_all.lua",
    "script/cauhinh/ch_thuong_lib.lua",
    "script/missions/yandibaozang/npc.lua",
    "script/lib/lib_sukien.lua",
    "script/global/LuaNpcMonsters/Droprate_normal.lua",
    "script/vng_event/change_request_baoruong/exp_award.lua",
    "script/event/event_vantieu/tieudau.lua",
    "script/tinhnang/tong_kim_tcap/lib_tktc.lua",
    "script/task/metempsychosis/task_head.lua",
    "script/lib/lib_server.lua",
    "script/missions/citywar_city/head.lua",
    "script/tinhnang/congthanhchien/lib_ctc.lua",
    "script/missions/citywar_arena/head.lua",
    "script/missions/citywar_global/head.lua",
    "script/missions/citywar_global/mission.lua",
    "script/tinhnang/tong_kim_tcap/quanquan.lua",
    "script/tinhnang/tong_kim_tcap/trinhsat.lua",
    "script/tinhnang/congthanhchien/congthanhquan.lua",
    "script/missions/citywar_global/citywar_function.lua",
    "script/tinhnang/boss_hoangkim/lib_bosshk.lua",
    "script/tinhnang/boss_hoangkim/deathhktieu.lua",
    "script/tinhnang/boss_hoangkim/deathhkdai.lua",
    "script/tinhnang/boss_hoangkim/deathbosspld.lua",
    "script/petsys/common.lua",
    "script/petsys/xiuzhen.lua",
    "script/petsys/petequip.lua",
    "script/task/partner/reward/partner_reward.lua",
    "script/task/newtask/tasklink/tasklink_head.lua",
    "script/event/trongbanghoi/lib.lua",
    "script/tinhnang/loidaihonchien/mainloidai.lua",
    "settings/gamesetting.ini",
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t09_dongbo_repo - %s ===" % ("CHEP THAT" if ghi else "DIEN TAP"))

    viec = []
    for rel in TEP:
        nguon = os.path.join(E, rel.replace("/", os.sep))
        dich = os.path.join(D, rel.replace("/", os.sep))
        if not os.path.isfile(nguon):
            print("!!! LOI TO: thieu nguon %s" % rel)
            return 1
        if os.path.isfile(dich) and doc(dich) == doc(nguon):
            print("  %-52s giong nhau - bo qua" % rel)
            continue
        n_cao = sum(1 for c in doc(nguon) if ord(c) > 127)
        print("  %-52s se chep (%d byte tieng Viet)" % (rel, n_cao))
        viec.append((nguon, dich, rel, n_cao))

    if not viec:
        print("\nKhong co gi de chep.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua chep. Chay lai voi --ghi de chep that.")
        return 0

    for nguon, dich, rel, n_cao in viec:
        tm = os.path.dirname(dich)
        if not os.path.isdir(tm):
            os.makedirs(tm)
        shutil.copy2(nguon, dich)
        if doc(dich) != doc(nguon):
            print("!!! LOI TO: doc lai KHONG khop: %s" % rel)
            return 1
        if sum(1 for c in doc(dich) if ord(c) > 127) != n_cao:
            print("!!! LOI TO: byte cao doi sau khi chep: %s" % rel)
            return 1
        print("  DA CHEP %s" % rel)
    print("\nXong. Nho `git add` thu muc %s roi commit." % D)
    return 0


if __name__ == "__main__":
    sys.exit(main())
