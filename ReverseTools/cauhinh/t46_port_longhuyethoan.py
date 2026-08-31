# -*- coding: utf-8 -*-
"""t46_port_longhuyethoan.py - LAM CHO LONG HUYET HOAN DUNG DUOC TRO LAI.

VAN DE: script\\item\\longxuewan.lua (ban Fong Kieu 2021) dat SetTask(43, 0) va
dem task 65. He Vuot Ai / Thach Dau Thoi Gian MOI (ban Linux, dang chay) KHONG
doc hai task do -> vat pham hien VO DUNG.

DIEU BAT NGO TOT: ban Linux DA CO SAN co che Long Huyet Hoan, va ma task cua no
TRUNG Y HET voi cay ta:
  script\\missions\\challengeoftime\\include.lua:19-21   TSK_JOIN_DATE=1551,
                                                        TSK_REMAIN_COUNT=1550
  script\\missions\\challengeoftime\\include.lua:126-128 TSK_Longxuewan_Date=2641,
                                                        TSK_Longxuewan_Use=2642,
                                                        TSK_Longxuewan_avail=4018
  challengeoftime\\npc\\dragonboat_main.lua:154-160  DOC TSK_Longxuewan_Date +
                                                    TSK_Longxuewan_avail
  task\\metempsychosis\\translife_5.lua:105-109      DOC TSK_Longxuewan_avail
Tuc la ben nhan DA SAN SANG, chi thieu ben phat. Chi can port dung tep item.

Ban goc: D:\\ServerLinux\\server1\\script\\item\\longxuewan.lua (29 dong).

BA CHO PHAI SUA KHI PORT (khong chep nguyen xi duoc):
  1. Vo ham: Linux dung "function main()" khong tham so; JX1 dung
     "function main(nItemIndex)" va phai tu goi RemoveItemByIndex de tieu thu.
     Lay khuon tu mot tep DA PORT trong cay: script\\item\\ruong_datau_tasklink.lua
     (return 1 = giu vat pham, return 0 = da tieu thu).
  2. Linux ghi cung SetTask(1550, 1) luc sang ngay moi. Cay ta cho chinh so nay
     bang cau hinh: challengeoftime\\include.lua:39
     COUNT_LIMIT = HD_CFG("HD3_VA_LUOT_NGAY", COUNT_LIMIT). Dung HD_CFG de ton
     trong cau hinh cua chu game.
  3. Linux chan "GetExtPoint(0) == 0". GetExtPoint(0) la SO XU cua nguoi choi
     (ScriptFuns.cpp:218-231 -> Player.GetExtPoint(); doi chieu
     global\\admin\\quanly.lua:182 "local TienXu = GetExtPoint()").
     Chan nhu vay la CHAN NGUOI CHOI KHONG CO XU - day la quyet dinh kinh te,
     KHONG phai loi ky thuat. Ban JX1 hien KHONG chan. => GIU NGUYEN khong chan,
     de san mot dong da chu thich de chu game tu bat neu muon.

KHONG Include challengeoftime\\include.lua (406 dong + keo theo 5 tep: bigboss,
activitysys\\functionlib, tong_award_head, cauhinh_hoatdong, IncludeLib RELAYLADDER)
- nap ngan ay chi de lay 5 con so, moi lan bam chuot phai, la phi. Chep thang 5
ma task vao day kem chu thich nguon. Chi Include header\\cauhinh_hoatdong.lua
(tep LA, khong Include gi) de lay HD_CFG.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
LINUX = r"D:\ServerLinux\server1\script\item\longxuewan.lua"
P = os.path.join(S, "item", "longxuewan.lua")
INC = os.path.join(S, "missions", "challengeoftime", "include.lua")
DUOI = ".truoc_portlxw"

# Cau tieng Viet: lay NGUYEN BYTE tu ban goc Linux, khong tu go lai.
# (TCVN3 khong ma hoa duoc nguyen am HOA co dau; tu go la sai chinh ta hoac hong byte.)
CAU_LINUX = {
    "cap90": None,      # dong 4 ban Linux
    "du2": None,        # dong 20
    "thanhcong": None,  # dong 28
}


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def lay_hang_so():
    """Doc 5 ma task tu chinh challengeoftime\\include.lua - khong go tay."""
    d = doc(INC)
    ra = {}
    for ten in ("TSK_JOIN_DATE", "TSK_REMAIN_COUNT", "TSK_Longxuewan_Date",
                "TSK_Longxuewan_Use", "TSK_Longxuewan_avail"):
        m = re.search(r"^\s*%s\s*=\s*(\d+)" % re.escape(ten), d, re.M)
        if not m:
            return None, "khong doc duoc %s trong include.lua" % ten
        ra[ten] = m.group(1)
    return ra, None


def lay_cau():
    """Lay 3 cau tieng Viet NGUYEN BYTE tu ban goc Linux."""
    d = doc(LINUX).replace("\r\n", "\n").split("\n")
    ra = {}
    for l in d:
        m = re.search(r'Say\("([^"]*)"', l)
        if not m:
            continue
        s = m.group(1)
        if "90" in s:
            ra["cap90"] = s
        elif "2" in s and "Long" in s:
            ra["du2"] = s
        elif "1" in s:
            ra["thanhcong"] = s
    if len(ra) != 3:
        return None, "chi lay duoc %d/3 cau tu ban Linux" % len(ra)
    return ra, None


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t46 PORT Long Huyet Hoan tu ban Linux - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    if not os.path.isfile(LINUX):
        print("!!! LOI TO: khong thay ban goc Linux: %s" % LINUX)
        return 1

    hs, loi = lay_hang_so()
    if loi:
        print("!!! LOI TO: %s" % loi)
        return 1
    print("  Ma task doc TU CHINH challengeoftime\\include.lua:")
    for k, v in hs.items():
        print("     %-22s = %s" % (k, v))

    cau, loi = lay_cau()
    if loi:
        print("!!! LOI TO: %s" % loi)
        return 1
    print("  Ba cau tieng Viet lay NGUYEN BYTE tu ban goc Linux (%d/%d/%d byte)"
          % (len(cau["cap90"]), len(cau["du2"]), len(cau["thanhcong"])))

    cu = doc(P)
    eol = "\r\n"
    than = [
        "-- longxuewan.lua - Long Huyet Hoan (vat pham 6/1/2126).",
        "-- [PORT LINUX 30/08] PORT tu D:\\ServerLinux\\server1\\script\\item\\longxuewan.lua",
        "--",
        "-- VI SAO PHAI PORT: ban cu (Fong Kieu 2021) dat SetTask(43,0) va dem task 65.",
        "-- He Vuot Ai / Thach Dau Thoi Gian MOI (ban Linux, dang chay) KHONG doc hai",
        "-- task do -> vat pham VO DUNG. Ben NHAN da san sang tu truoc:",
        "--   challengeoftime\\npc\\dragonboat_main.lua:154-160  doc TSK_Longxuewan_Date",
        "--                                                     + TSK_Longxuewan_avail",
        "--   task\\metempsychosis\\translife_5.lua:105-109      doc TSK_Longxuewan_avail",
        "-- chi thieu ben PHAT - la tep nay.",
        "--",
        "-- Ma task CHEP TU script\\missions\\challengeoftime\\include.lua",
        "-- (dong 19-21 va 126-128). KHONG Include tep do vi no 406 dong va keo theo",
        "-- 5 tep nua (bigboss / activitysys\\functionlib / tong_award_head /",
        "-- cauhinh_hoatdong / IncludeLib RELAYLADDER) - nap ngan ay moi lan bam",
        "-- chuot phai chi de lay 5 con so la phi.",
        "--",
        "-- KHAC BAN LINUX MOT CHO, CO Y: ban Linux con chan",
        "--   GetExtPoint(0) == 0",
        "-- GetExtPoint(0) la SO XU cua nguoi choi (ScriptFuns.cpp:218-231), tuc ban",
        "-- Linux chan nguoi choi khong co Xu. Do la quyet dinh KINH TE chu khong",
        "-- phai loi ky thuat, va ban JX1 truoc gio khong chan. De chu game tu quyet:",
        "-- muon chan thi bo dau -- o dong LXW_CAN_XU duoi day.",
        "",
        "Include(\"\\\\script\\\\header\\\\cauhinh_hoatdong.lua\")\t-- HD_CFG (tep LA, khong Include gi)",
        "",
        "-- chep tu challengeoftime\\include.lua:19-21",
        "LXW_TSK_JOIN_DATE        = %s" % hs["TSK_JOIN_DATE"],
        "LXW_TSK_REMAIN_COUNT     = %s" % hs["TSK_REMAIN_COUNT"],
        "-- chep tu challengeoftime\\include.lua:126-128",
        "LXW_TSK_DATE             = %s" % hs["TSK_Longxuewan_Date"],
        "LXW_TSK_USE              = %s" % hs["TSK_Longxuewan_Use"],
        "LXW_TSK_AVAIL            = %s" % hs["TSK_Longxuewan_avail"],
        "-- ban Linux gioi han 2 vien/ngay (longxuewan.lua:19)",
        "LXW_MAX_NGAY             = 2",
        "-- LXW_CAN_XU = 1\t-- bo dau -- o dau dong nay de chan nguoi choi 0 Xu (nhu ban Linux)",
        "",
        "function LXW_LuotNgay()",
        "\t-- ban Linux ghi cung 1; cay ta cho chinh bang cau hinh",
        "\t-- (challengeoftime\\include.lua:39 COUNT_LIMIT = HD_CFG(\"HD3_VA_LUOT_NGAY\", ...))",
        "\tif (HD_CFG ~= nil) then",
        "\t\treturn HD_CFG(\"HD3_VA_LUOT_NGAY\", 1)",
        "\tend",
        "\treturn 1",
        "end",
        "",
        "function main(nItemIndex)",
        "\tif (GetLevel() < 90) then",
        "\t\tSay(\"%s\", 0)" % cau["cap90"],
        "\t\treturn 1",
        "\tend",
        "\tif (LXW_CAN_XU ~= nil and GetExtPoint(0) == 0) then",
        "\t\tSay(\"%s\", 0)" % cau["cap90"],
        "\t\treturn 1",
        "\tend",
        "",
        "\tlocal ndate = tonumber(GetLocalDate(\"%y%m%d\"))",
        "\tif (ndate ~= GetTask(LXW_TSK_DATE)) then",
        "\t\tSetTask(LXW_TSK_DATE, ndate)",
        "\t\tSetTask(LXW_TSK_USE, 0)",
        "\t\tSetTask(LXW_TSK_AVAIL, 0)",
        "\t\tif (ndate ~= GetTask(LXW_TSK_JOIN_DATE)) then",
        "\t\t\tSetTask(LXW_TSK_JOIN_DATE, ndate)",
        "\t\t\tSetTask(LXW_TSK_REMAIN_COUNT, LXW_LuotNgay())",
        "\t\tend",
        "\tend",
        "",
        "\tif (GetTask(LXW_TSK_USE) >= LXW_MAX_NGAY) then",
        "\t\tSay(\"%s\", 0)" % cau["du2"],
        "\t\treturn 1",
        "\tend",
        "",
        "\tSetTask(LXW_TSK_REMAIN_COUNT, GetTask(LXW_TSK_REMAIN_COUNT) + 1)",
        "\tSetTask(LXW_TSK_USE, GetTask(LXW_TSK_USE) + 1)",
        "\tSetTask(LXW_TSK_AVAIL, GetTask(LXW_TSK_AVAIL) + 1)",
        "\tSay(\"%s\", 0)" % cau["thanhcong"],
        "\tRemoveItemByIndex(nItemIndex)",
        "\treturn 0",
        "end",
        "",
    ]
    moi = eol.join(than)

    if lh.can_bang(moi) != 0:
        print("!!! LOI TO: tep moi khong can bang tu khoa (%d)"
              % lh.can_bang(moi))
        return 1
    print("  Tep moi: %d dong, can bang tu khoa 0" % len(than))
    print("  Tep cu : %d dong (task 43/65 - khong ai doc)"
          % (cu.count("\n") + 1))

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = P + DUOI
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(moi.encode("latin-1"))
    if doc(P) != moi:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI %s (ban cu o %s)" % (P, os.path.basename(sao)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
