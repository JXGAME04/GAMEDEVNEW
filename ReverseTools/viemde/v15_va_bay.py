# -*- coding: utf-8 -*-
"""VIEM DE - buoc 6b-2e: va cac BAY khac nghia giua engine Linux va engine JX1
tren nhung tep da chep co hoc o buoc 6a.

CHAY SAU v09_port_kichban.py (v09 chep de len tu ban Linux) va TRUOC/SAU v10-v13
deu duoc (v10-v13 sinh tep khac).

Moi vet va deu co nhan "[JX1 26/08]" de chay lai khong nhan ban va de doi chieu.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import uni2tcvn, tcvn2uni

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\missions\yandibaozang"
GUONG = r"D:\GAMEDEVNEW\serverscript_jx2\viemde\script\missions\yandibaozang"
NHAN = "[JX1 26/08]"

# (tep, chuoi cu, chuoi moi, ly do) - chuoi cu PHAI xuat hien DUNG MOT LAN
VA = [
    (
        "mission.lua",
        "\tif GetMSPlayerCount(YDBZ_MISSION_MATCH,0) == 0 then",
        "\t-- " + NHAN + " BAY #3: y muon hoi \"con ai trong tran khong\".\n"
        "\t-- JX1: GetMSPlayerCount(id, 0) dem RIENG NHOM 0 (LuaMissionPlayerCount:\n"
        "\t-- nGroupId >= 0 -> GetGroupPlayerCount). Nguoi choi luon o nhom 1..3 nen\n"
        "\t-- nhom 0 LUON RONG -> tran se DONG NGAY khi mot nguoi roi ban do.\n"
        "\t-- Bo tham so nhom thi engine dung GetPlayerCount() = tat ca.\n"
        "\tif GetMSPlayerCount(YDBZ_MISSION_MATCH) == 0 then",
        "bay #3: dem tat ca nguoi trong mission",
    ),
    (
        "mission.lua",
        "\tif GetMSPlayerCount(YDBZ_MISSION_MATCH,nteams) == 0 then",
        "\t-- " + NHAN + " chan nteams == 0: neu tran doanh chua duoc dat thi cau tren\n"
        "\t-- se dem nhom 0 (luon rong) va bao nham \"ca to doi da tu tran\".\n"
        "\tif nteams > 0 and GetMSPlayerCount(YDBZ_MISSION_MATCH,nteams) == 0 then",
        "chan bao nham to doi tu tran khi chua co tran doanh",
    ),
]

# Nhung cho CHU Y da soat va KHONG sua (ghi lai de khoi soat lai):
KHONG_SUA = [
    ("timer_match.lua:20", "GetMSPlayerCount(MATCH) mot tham so = dem TAT CA - dung roi"),
    ("npc_death.lua:148", "GetMSPlayerCount(MATCH, camp) voi camp 1..3 - dung y ban goc"),
    ("player_death.lua:77", "GetMSPlayerCount(MATCH, i) voi i 1..3 - dung y ban goc"),
    ("readymap/include.lua:265,309,314,357", "luong bao danh CU - CHET o ca ban Linux "
     "(chi chay khi co ai OpenMission(51), ma YDBZ_ready_missions() o trigger_include.lua:10 "
     "khong ai goi). Giu nguyen cho dung ban goc; BuildATeam thieu cung khong sao."),
    ("readymap/mission.lua", "cung luong CU da chet - giu nguyen"),
    ("saizi.lua", "he xuc xac - buoc 7"),
]


def va_tep(ten, cu, moi, ly):
    p = os.path.join(SRV, ten)
    if not os.path.isfile(p):
        return "khong thay tep"
    d = io.open(p, encoding="latin-1", newline="").read()
    if NHAN in d and moi.split("\n")[-1] in d:
        return "da va roi"
    n = d.count(cu)
    if n != 1:
        return "NEO KHONG DUY NHAT (%d lan) - BO QUA" % n
    # Cac tep .lua cua JX1 dung CRLF, ma chuoi thay the nhieu dong o bang VA viet
    # bang \n. Phai nan lai thanh CRLF, neu khong se sinh ra dong LF le loi nam
    # giua mot tep CRLF (da dinh 26/08: mission.lua co 7 dong LF le).
    moi_crlf = moi.replace("\r\n", "\n").replace("\n", "\r\n")
    d = d.replace(cu, moi_crlf)
    for goc in (SRV, GUONG):
        q = os.path.join(goc, ten)
        os.makedirs(os.path.dirname(q), exist_ok=True)
        tam = q + ".dangghi"
        with open(tam, "wb") as f:
            f.write(d.encode("latin-1"))
        os.replace(tam, q)
    return "da va (%s)" % ly


def main():
    print("=== VA BAY ===")
    for ten, cu, moi, ly in VA:
        print("  %-16s %s" % (ten, va_tep(ten, cu, moi, ly)))
    print()
    print("=== DA SOAT, CO Y KHONG SUA ===")
    for cho, ly in KHONG_SUA:
        print("  %-34s %s" % (cho, ly))
    print()
    # kiem lai
    p = os.path.join(SRV, "mission.lua")
    d = io.open(p, encoding="latin-1", newline="").read()
    print("KIEM LAI mission.lua:")
    print("   con 'GetMSPlayerCount(YDBZ_MISSION_MATCH,0)': %s"
          % ("CON - CHUA VA!" if "GetMSPlayerCount(YDBZ_MISSION_MATCH,0)" in d else "khong con (dung)"))
    print("   co 'GetMSPlayerCount(YDBZ_MISSION_MATCH)'   : %s"
          % ("co (dung)" if "GetMSPlayerCount(YDBZ_MISSION_MATCH)" in d else "KHONG CO - CHUA VA!"))
    print("   FFFD=%d, LF-don=%d" % (d.count("\ufffd"),
                                     d.count("\n") - d.count("\r\n")))


main()
