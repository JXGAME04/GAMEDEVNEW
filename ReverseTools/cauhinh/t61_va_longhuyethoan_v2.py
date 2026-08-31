# -*- coding: utf-8 -*-
"""t61_va_longhuyethoan_v2.py - sua 6 diem bo phan bien bat duoc trong ban
longxuewan.lua toi vua port.

1. [VUA] LOI BIEN MINH SAI SU THAT viet ngay trong tep: toi ghi la khong Include
   challengeoftime\\include.lua "vi no 406 dong va keo theo 5 tep". Do that:
       challengeoftime\\include.lua  11.452 byte / 406 dong
       header\\cauhinh_hoatdong.lua  24.504 byte / 533 dong  <- tep toi DA Include
   Tuc tep toi ne NHO HON tep toi nap. Va script chi nap MOT LAN luc boot chu
   khong phai moi lan bam chuot phai. Ly do THAT de khong Include no la: no keo
   IncludeLib("RELAYLADDER") + bigboss.lua + activitysys\\functionlib.lua +
   tong_award_head.lua vao Lua_State cua mot script vat pham - la be mat tac dung
   phu, khong phai so dong. Viet lai cho dung.

2. [NHE] Bo chot GetFightState() cua ban JX1 cu ma khong khai bao. Khoi phuc -
   chuoi lay NGUYEN BYTE tu ban sao .truoc_portlxw.

3. [NHE] Khoi reset ngay chay TRUOC chot tru vat pham: tru that bai van ghi 4
   task, dua task 4018 tu -1 ve 0 (dragonboat_main.lua:160 dat -1). Sap xep lai:
   TINH truoc, chi GHI sau khi tru thanh cong.

4. [NHE] Nhanh chan "khong co Xu" in NHAM thong bao cap 90. Gop lai mot chot nhu
   ban Linux de cau thong bao khop.

5. [NHE] Tran 2 vien/ngay ghi cung o CA hang so lan chuoi thong bao. Dung lai
   khuon cua ban JX1 cu: noi so vao giua hai manh chuoi.

6. [NHE] Cau "nap the" bi doi thanh "nop the" (chep tu ban Linux). Nguoi choi
   quen cach viet cu -> lay lai chuoi JX1 cu, NGUYEN BYTE.

MOI CHUOI TIENG VIET DEU TRICH TU TEP CO SAN, KHONG GO TAY BYTE NAO.

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
P = os.path.join(S, "item", "longxuewan.lua")
BAK = P + ".truoc_portlxw"
INC = os.path.join(S, "missions", "challengeoftime", "include.lua")
DUOI = ".truoc_v2"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def lay_hang_so():
    d = doc(INC)
    ra = {}
    for ten in ("TSK_JOIN_DATE", "TSK_REMAIN_COUNT", "TSK_Longxuewan_Date",
                "TSK_Longxuewan_Use", "TSK_Longxuewan_avail"):
        m = re.search(r"^\s*%s\s*=\s*(\d+)" % re.escape(ten), d, re.M)
        if not m:
            return None, "khong doc duoc %s" % ten
        ra[ten] = m.group(1)
    return ra, None


def lay_chuoi_cu():
    """Trich NGUYEN BYTE cac cau tieng Viet tu ban JX1 cu."""
    d = doc(BAK).replace("\r\n", "\n").split("\n")
    ra = {}
    for l in d:
        m = re.search(r'Talk\(1,"","([^"]*)"\)', l)
        if m:
            s = m.group(1)
            if "90" in s:
                ra["cap90"] = s
            continue
        # dong tran/ngay: Talk(1,"","..."..MAX.." ...")
        m = re.search(r'Talk\(1,"","([^"]*)"\.\.\w+\.\."([^"]*)"\)', l)
        if m:
            ra["tran_dau"] = m.group(1)
            ra["tran_cuoi"] = m.group(2)
    # cau chan chien dau: dong dau tien co Talk sau GetFightState
    for i, l in enumerate(d):
        if "GetFightState()" in l:
            for j in range(i, min(i + 4, len(d))):
                m = re.search(r'Talk\(1,"","([^"]*)"\)', d[j])
                if m:
                    ra["chiendau"] = m.group(1)
                    break
            break
    can = ("cap90", "tran_dau", "tran_cuoi", "chiendau")
    thieu = [x for x in can if x not in ra]
    if thieu:
        return None, "thieu chuoi: %s" % thieu
    return ra, None


def lay_cau_thanhcong():
    """Cau thanh cong dang co trong ban hien tai - giu nguyen byte."""
    for l in doc(P).replace("\r\n", "\n").split("\n"):
        m = re.search(r'Say\("([^"]*)"\s*,\s*0\)', l)
        if m and "1" in m.group(1):
            return m.group(1)
    return None


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t61 sua 6 diem trong longxuewan.lua - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    raw = doc(P)
    if "[LXW-V2 30/08]" in raw:
        print("  da sua - bo qua")
        return 0

    hs, loi = lay_hang_so()
    if loi:
        print("!!! LOI TO: %s" % loi)
        return 1
    cu, loi = lay_chuoi_cu()
    if loi:
        print("!!! LOI TO: %s" % loi)
        return 1
    thanh_cong = lay_cau_thanhcong()
    if not thanh_cong:
        print("!!! LOI TO: khong lay duoc cau thanh cong")
        return 1
    print("  Chuoi tieng Viet trich NGUYEN BYTE:")
    for k in ("chiendau", "cap90", "tran_dau", "tran_cuoi"):
        print("     %-10s %3d byte (tu ban JX1 cu)" % (k, len(cu[k])))
    print("     %-10s %3d byte (tu ban Linux, giu nguyen)"
          % ("thanhcong", len(thanh_cong)))

    eol = "\r\n"
    than = [
        "-- longxuewan.lua - Long Huyet Hoan (vat pham 6/1/2126).",
        "-- [PORT LINUX 30/08] PORT tu D:\\ServerLinux\\server1\\script\\item\\longxuewan.lua",
        "-- [LXW-V2 30/08] sua 6 diem bo phan bien bat duoc - xem cuoi tep.",
        "--",
        "-- VI SAO PHAI PORT: ban cu (Fong Kieu 2021) dat SetTask(43,0) va dem task 65.",
        "-- He Vuot Ai / Thach Dau Thoi Gian MOI (ban Linux, dang chay) KHONG doc hai",
        "-- task do -> vat pham VO DUNG. Ben NHAN da san sang tu truoc:",
        "--   challengeoftime\\npc\\dragonboat_main.lua:154-160  doc TSK_Longxuewan_Date",
        "--                                                     + TSK_Longxuewan_avail",
        "--   task\\metempsychosis\\translife_5.lua:105-109      doc TSK_Longxuewan_avail",
        "-- chi thieu ben PHAT - la tep nay.",
        "--",
        "-- Ma task CHEP TU script\\missions\\challengeoftime\\include.lua (dong 19-21",
        "-- va 126-128). KHONG Include tep do - ly do THAT: no goi",
        "-- IncludeLib(\"RELAYLADDER\") + Include bigboss.lua + activitysys\\functionlib.lua",
        "-- + tong_award_head.lua, tuc keo ca mot manh he thong vao Lua_State cua mot",
        "-- script vat pham. Do la be mat tac dung phu, KHONG phai chuyen so dong:",
        "-- tep do 11.452 byte, con header\\cauhinh_hoatdong.lua ma tep nay CO Include",
        "-- lai 24.504 byte. (Ban dau toi ghi ly do sai o cho nay - da sua.)",
        "-- !! Doi lai: 5 hang so duoi day bi NHAN DOI. Neu sua include.lua thi phai",
        "-- !! sua ca day.",
        "",
        "Include(\"\\\\script\\\\header\\\\cauhinh_hoatdong.lua\")\t-- HD_CFG (tep LA)",
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
        "-- LXW_CAN_XU = 1\t-- bo dau -- de chan nguoi choi 0 Xu (nhu ban Linux)",
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
        "\t-- [LXW-V2] khoi phuc chot cua ban JX1 cu: khong dung trong khu chien dau.",
        "\t-- Ban Linux khong co chot nay, nhung bo di la doi hanh vi nguoi choi thay.",
        "\tif (GetFightState() ~= 0) then",
        "\t\tSay(\"%s\", 0)" % cu["chiendau"],
        "\t\treturn 1",
        "\tend",
        "\t-- [LXW-V2] gop mot chot nhu ban Linux de cau thong bao khop ca hai ve.",
        "\tif (GetLevel() < 90 or (LXW_CAN_XU ~= nil and GetExtPoint(0) == 0)) then",
        "\t\tSay(\"%s\", 0)" % cu["cap90"],
        "\t\treturn 1",
        "\tend",
        "",
        "\t-- [LXW-V2] TINH truoc, chi GHI sau khi tru duoc vat pham. Ban truoc ghi",
        "\t-- khoi reset ngay TRUOC chot tru, nen tru that bai van dua task",
        "\t-- LXW_TSK_AVAIL tu -1 (dragonboat_main.lua:160 dat) ve 0.",
        "\tlocal ndate = tonumber(GetLocalDate(\"%y%m%d\"))",
        "\tlocal bNgayMoi = 0",
        "\tif (ndate ~= GetTask(LXW_TSK_DATE)) then",
        "\t\tbNgayMoi = 1",
        "\tend",
        "\tlocal nDaDung = 0",
        "\tif (bNgayMoi == 0) then",
        "\t\tnDaDung = GetTask(LXW_TSK_USE)",
        "\tend",
        "\tif (nDaDung >= LXW_MAX_NGAY) then",
        "\t\tSay(\"%s\"..LXW_MAX_NGAY..\"%s\", 0)" % (cu["tran_dau"],
                                                      cu["tran_cuoi"]),
        "\t\treturn 1",
        "\tend",
        "",
        "\tif (RemoveItemByIndex(nItemIndex) ~= 1) then",
        "\t\treturn 1",
        "\tend",
        "",
        "\tif (bNgayMoi == 1) then",
        "\t\tSetTask(LXW_TSK_DATE, ndate)",
        "\t\tSetTask(LXW_TSK_USE, 0)",
        "\t\tSetTask(LXW_TSK_AVAIL, 0)",
        "\t\tif (ndate ~= GetTask(LXW_TSK_JOIN_DATE)) then",
        "\t\t\tSetTask(LXW_TSK_JOIN_DATE, ndate)",
        "\t\t\tSetTask(LXW_TSK_REMAIN_COUNT, LXW_LuotNgay())",
        "\t\tend",
        "\tend",
        "\tSetTask(LXW_TSK_REMAIN_COUNT, GetTask(LXW_TSK_REMAIN_COUNT) + 1)",
        "\tSetTask(LXW_TSK_USE, nDaDung + 1)",
        "\tSetTask(LXW_TSK_AVAIL, GetTask(LXW_TSK_AVAIL) + 1)",
        "\tSay(\"%s\", 0)" % thanh_cong,
        "\treturn 0",
        "end",
        "",
    ]
    moi = eol.join(than)
    if lh.can_bang(moi) != 0:
        print("!!! LOI TO: tep moi khong can bang (%d)" % lh.can_bang(moi))
        return 1
    print("  Tep moi %d dong, can bang 0" % len(than))
    print("  byte tieng Viet: %d -> %d" % (hi(raw), hi(moi)))

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
    print("  DA GHI longxuewan.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
