# -*- coding: utf-8 -*-
"""C52 - tiep C51: chi dan cho 2 vat pham roi MOI LAN giet boss sat thu ma bam
chuot phai khong lam gi (dung thiet ke, ca ban Linux cung the):

  6,1,398 Sat Thu lenh  -> mang toi NPC Nhiep Thi Tran, muc "hop thanh":
                          bo dung 5 cai CUNG CAP -> 1 Sat thu gian (ngu hanh random)
                          (nieshichen.lua:175-226 compose/exchange_token/givesword)
  6,1,399 Sat thu gian  -> VE VAO VUOT AI: dragonboat_main.lua:136-142 tru 1 cai
                          khi ghi danh (CalcEquiproomItemCount 6,1,399,<cap>).

Script CHI HIEN THONG TIN, khong tru item, khong nam trong sIsJx2ItemScript nen
engine cung khong tru (KItemList.cpp:1809 - chi nhanh danh sach trang moi tru).
"""
import io, os, sys, shutil

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

L = [
    "-- ============================================================================",
    "-- ST_LENHBAI_INFO.LUA - SINH boi ReverseTools/port_3hd/thicong/c52_chidan_satthulenh.py",
    "-- Chi dan khi bam chuot phai Sat Thu lenh (398) / Sat thu gian (399).",
    "-- KHONG tru item. Hai ma nay von de cot Script = 0 o ca ban Linux lan du an,",
    "-- nguoi choi bam khong thay gi nen tuong item hong.",
    "-- ============================================================================",
    "",
    "function main(nItemIdx)",
    "\tlocal g, d, p, lv = GetItemProp(nItemIdx)",
    "\tif (p == 399) then",
    '\t\tSay("' + V("Sát thủ giản không dùng trực tiếp. Đây là ")
    + '<color=yellow>' + V("vé vào Vượt ải") + '<color> - '
    + V("mang theo rồi ghi danh ở Dịch Quan, mỗi lượt trừ 1 cái.") + '", 0)',
    "\t\treturn",
    "\tend",
    '\tlocal nSo = CalcItemCount(3, 6, 1, 398, -1)',
    '\tSay("' + V("Sát Thủ lệnh không dùng trực tiếp. Mang tới ")
    + '<color=yellow>' + V("Nhiếp Thí Trần") + '<color> '
    + V("chọn mục hợp thành, bỏ đúng ") + '<color=yellow>5<color> '
    + V("Sát Thủ lệnh cùng cấp sẽ được 1 Sát thủ giản (ngũ hành ngẫu nhiên).") + '", 0)',
    '\tSay("' + V("Đang giữ tất cả ") + '<color=yellow>"..nSo.."<color> '
    + V("Sát Thủ lệnh. Sát thủ giản dùng làm vé vào Vượt ải.") + '", 0)',
    "end",
    "",
]
rel = r"script\item\st_lenhbai_info.lua"
p = os.path.join(SRV, rel)
io.open(p, "w", encoding="latin-1", newline="").write("\r\n".join(L))
dst = os.path.join(MIR, rel)
os.makedirs(os.path.dirname(dst), exist_ok=True)
shutil.copyfile(p, dst)
print("da sinh %s (%d dong)" % (rel, len(L)))

SC = "\\script\\item\\st_lenhbai_info.lua"
for goc in (SRV, CLI):
    q = os.path.join(goc, r"settings\item\magicscript.txt")
    dd = io.open(q, encoding="latin-1", newline="").read().split("\n")
    n = 0
    for i, l in enumerate(dd):
        c = l.split("\t")
        if len(c) > 10 and c[1] == "6" and c[2] == "1" and c[3] in ("398", "399") and c[9] != SC:
            c[9] = SC
            dd[i] = "\t".join(c)
            n += 1
    if n:
        io.open(q, "w", encoding="latin-1", newline="").write("\n".join(dd))
    print("  bang item %s: tro %d dong (398/399) sang script chi dan"
          % ("server" if goc == SRV else "client", n))
shutil.copyfile(os.path.join(SRV, r"settings\item\magicscript.txt"),
                os.path.join(MIR, r"settings\item\magicscript.txt"))
