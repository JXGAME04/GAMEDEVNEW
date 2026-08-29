# -*- coding: utf-8 -*-
"""v21_timertask.py - VA "test 1 minh vao map khong NPC/khong thong bao".

GOC (da do tan ma):
  - InitMission (mission.lua:36) chi dat bay + StartMissionTimer(50, 87, 5*18);
    TOAN BO khoi dong tran (thong bao, spawn quai 3 duong, dem gio) nam o
    OnTimer cua timer_match.lua, chay khi TIMER 87 no.
  - Timer no -> KTimerTaskFun::Activate -> GetTimerTaskScript tra bang
    settings\\TimerTask.txt THEO KHOA id (KTaskFuns.cpp:185 GetString(szTaskId,
    "SCRIPT")) -> gọi OnMissionTimer/OnTimer (KMission.cpp:338).
  - TimerTask.txt JX1 43 dong, KHONG co hang 87/88/89 => szScript rong =>
    khong goi gi => im lang tuyet doi. Ban Linux (settings/TimerTask.txt
    :88-90) co du 3 hang. Id 87/88/89 ben JX1 con TRONG (da kiem: id dang
    dung toi 77).

MIENG VA: them 3 hang y nguyen ban Linux (duong dan script JX1 da ton tai,
syncheck 3 file OK, khong unpack/lib):
    87  \\script\\missions\\yandibaozang\\timer_match.lua
    88  \\script\\missions\\yandibaozang\\readymap\\timer_match.lua
    89  \\script\\missions\\yandibaozang\\timer_setfightstate.lua

KHONG build. m_TimerTaskTab nap MOT LAN luc boot (KTimerTaskFun::Init) =>
can KHOI DONG LAI GameServer de an (CAM tu restart - bao chu).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_ydbztimer lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\TimerTask.txt"
HAU_TO = ".truoc_ydbztimer"
THEM = [
    "87\t\\script\\missions\\yandibaozang\\timer_match.lua",
    "88\t\\script\\missions\\yandibaozang\\readymap\\timer_match.lua",
    "89\t\\script\\missions\\yandibaozang\\timer_setfightstate.lua",
]
GOC_SCRIPT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== v21_timertask - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    raw = io.open(DICH, "rb").read().decode("latin-1")
    if "yandibaozang" in raw:
        print("  DA CO hang yandibaozang - bo qua (idempotent)")
        return 0

    dong = [l for l in raw.replace("\r\n", "\n").split("\n")]
    ids = set()
    for l in dong[1:]:
        c = l.split("\t")
        if c and c[0].strip().isdigit():
            ids.add(int(c[0]))
    for h in THEM:
        nid = int(h.split("\t")[0])
        if nid in ids:
            print("!!! LOI TO: id %d da co trong bang - dung lai" % nid)
            return 1
    # 3 script dich phai ton tai
    for h in THEM:
        duong = GOC_SCRIPT + h.split("\t")[1].replace("\\\\", "\\")
        if not os.path.isfile(duong):
            print("!!! LOI TO: thieu tep %s" % duong)
            return 1
    print("  id 87/88/89 trong, 3 tep script co that")

    nd = raw
    if not nd.endswith("\r\n") and not nd.endswith("\n"):
        nd += "\r\n"
    nd += "\r\n".join(THEM) + "\r\n"
    try:
        nd.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ngoai latin-1: %s" % e)
        return 1
    print("  them 3 hang (87/88/89) vao cuoi bang")

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. Can KHOI DONG LAI GameServer de nap bang (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
