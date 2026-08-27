# -*- coding: utf-8 -*-
"""VIEM DE - buoc 6b-2d: khai 2 mission cua tinh nang vao settings\\task\\missions.txt.

Ban Linux (settings\\task\\missions.txt dong 51-52):
    50  \\script\\missions\\yandibaozang\\mission.lua              <- mission TRAN DANH (SONG)
    51  \\script\\missions\\yandibaozang\\readymap\\mission.lua    <- mission phong cho (CHET, xem duoi)

JX1 chi khai den id 45 nen PHAI keo dai bang, neu khong thi:
  ready.lua:318 lib:DoFunInWorld(..., OpenMission, YDBZ_MISSION_MATCH)  ==> OpenMission(50)
  van tao duoc o chua nguoi (LuaInitMission ScriptFuns.cpp) NHUNG
  g_MissionTabFile.GetString(51, 2, ...) tra rong ==> InitMission() cua mission.lua
  KHONG BAO GIO CHAY ==> quai khong sinh, dong ho tran khong chay.

Vi sao noi mission 51 la CHET o ban Linux (van khai cho dung ban goc):
  - Luong SONG: yandibaozang_trigger.lua:14 -> tbReady:Init() (ready.lua) -> pho ban.
  - Luong CU: readymap\\mission.lua (InitMission/EndMission) chi chay khi co ai goi
    OpenMission(51), ma ham lam viec do la YDBZ_ready_missions() o trigger_include.lua:10
    - da soat ca cay: KHONG AI GOI. readymap\\timer_match.lua cung chi chay khi
    mission 51 da mo. => nhanh do khong bao gio khoi dong.

Bang co 2 cot MISSION/SCRIPT, id N nam o DONG N+1 (engine doc GetString(id+1, 2)).
Cac khe 46-49 chua ai dung -> dien script rong san co cua du an de khong lech dong.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import Bang, so_sanh_byte

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
BANG = SRV + r"\settings\task\missions.txt"
BAK = ".truoc_viemde_2608"
RONG = r"\script\missions\mission_trong.lua"

CAN_KHAI = {
    50: r"\script\missions\yandibaozang\mission.lua",
    51: r"\script\missions\yandibaozang\readymap\mission.lua",
}


def main():
    b = Bang(BANG)
    print("truoc: %d dong (id 1..%s)" % (len(b.rows), b.rows[-1][0]))

    # kiem tep script rong co that khong
    if not os.path.isfile(SRV + RONG):
        print("!! khong thay %s - dung lai" % RONG)
        return

    hien = {}
    for c in b.rows[1:]:
        if len(c) >= 2:
            try:
                hien[int(c[0])] = c[1]
            except ValueError:
                pass

    for i, duong in CAN_KHAI.items():
        if i in hien and hien[i].strip() and hien[i] != duong:
            print("!! khe %d DA CO '%s' - DUNG LAI, khong ghi de" % (i, hien[i]))
            return
        q = SRV + duong
        if not os.path.isfile(q):
            print("!! khe %d tro toi tep KHONG TON TAI: %s - dung lai" % (i, duong))
            return

    maxid = max(hien) if hien else 0
    can = max(CAN_KHAI)
    if maxid >= can and all(hien.get(i) == d for i, d in CAN_KHAI.items()):
        print("da khai roi - bo qua.")
        return

    for i in range(maxid + 1, can + 1):
        b.rows.append([str(i), CAN_KHAI.get(i, RONG)])

    n = b.ghi(BAK)
    print("sau  : %d dong, %d byte" % (len(b.rows), n))

    # ---- kiem lai ----
    b2 = Bang(BANG)
    ok = True
    for i, duong in CAN_KHAI.items():
        got = b2.rows[i][1] if i < len(b2.rows) and len(b2.rows[i]) > 1 else "?"
        # engine doc GetString(id+1, 2) -> dong (id+1) cua tep, tuc rows[id] khi rows[0] la tieu de
        khop = (b2.rows[i][0] == str(i)) and (got == duong)
        print("   khe %-3d dong %-3d id-ghi='%s' script='%s'  %s"
              % (i, i + 1, b2.rows[i][0], got, "OK" if khop else "<< LECH"))
        ok = ok and khop
    kq = so_sanh_byte(BANG, BAK, len(CAN_KHAI))
    print("   KIEM BYTE CRLF %d->%d  LFdon %d->%d  FFFD=%d"
          % (kq["crlf_truoc"], kq["crlf_sau"], kq["lf_don_truoc"], kq["lf_don_sau"], kq["fffd"]))
    assert ok and kq["eol_giu_nguyen"] and kq["fffd"] == 0
    print("*** PHAI KHOI DONG LAI MAY CHU (doi settings\\task\\missions.txt) ***")


main()
