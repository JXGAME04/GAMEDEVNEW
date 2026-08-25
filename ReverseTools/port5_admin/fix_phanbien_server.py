# -*- coding: utf-8 -*-
r"""[24/08 hau phan bien] Va phia MAY CHU (3 loi da qua kiem chung doi khang).

E. Bang Chien: task 2378 (tong diem ca mua) khong duoc dong bo lai sau khi dang nhap
   => bang F11 luon hien "Tong diem ca mua: 0". Them dong bo khi bao danh.
F. Bach Nhan: bo dem 2709 chi tu reset khi nguoi choi DA vao map va an tick dau tien
   => sang hom sau bang F11 con doc so cua ngay cu -> bao "het luot" oan.
   Goi PlayerFunLib:GetTaskDailyCount(2709) ngay khi NPC kiem dieu kien vao: ham nay
   TU ghi lai ngay moi va day gia tri da reset xuong client.
G. Lenh Bai Admin in "-1 luot": goi GetTaskDailyCount nhu bien toan cuc, trong khi no la
   PHUONG THUC cua bang PlayerFunLib => luon ra -1. Giai ma tai cho (khong Include
   playerfunlib.lua vi se tao lai vong Include da gay tran stack dem 23/08).
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def bak(p):
    q = p + ".truoc_phanbien_2408"
    if not os.path.isfile(q): shutil.copyfile(p, q)

# ---------------- E. dong bo 2378 ----------------
p = os.path.join(E, r"missions\tongwar\trap\tongwar_trap.lua")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
if "TONGWAR_RLTASK_TOTALPOINT)" in s and "hau phan bien" in s:
    print("E: da va")
else:
    anchor = "\ttongwar_setdata(TONGWAR_RLTASK_MAXDEATH, HD_CFG(\"TW_SO_MANG\", 10))"
    assert s.count(anchor) == 1, "anchor E = %d" % s.count(anchor)
    s = s.replace(anchor, anchor + NL +
        "\t-- [24/08 hau phan bien] 2378 (tong diem ca mua) khong duoc dong bo lai sau khi dang" + NL +
        "\t-- nhap (client bi xoa sach bang task luc vao game) nen bang chi nam luon hien 0." + NL +
        "\tSyncTaskValue(TONGWAR_RLTASK_TOTALPOINT)", 1)
    bak(p); wr(p, s)
    print("E: da them SyncTaskValue(2378) khi bao danh")

# ---------------- F. reset bo dem ngay Bach Nhan ----------------
p = os.path.join(E, r"missions\bairenleitai\hundred_arena.lua")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
if "hau phan bien" in s:
    print("F: da va")
else:
    anchor = "\t\tif (GetLevel() >= HD_CFG(\"BR_CAP_TOITHIEU\", 90)) then"
    assert s.count(anchor) == 1, "anchor F = %d" % s.count(anchor)
    s = s.replace(anchor, anchor + NL +
        "\t\t\t-- [24/08 hau phan bien] doc mot lan de ham tu ghi lai NGAY MOI va day gia tri da" + NL +
        "\t\t\t-- reset xuong client; khong the bang chi nam (F11) con doc so luot cua ngay cu" + NL +
        "\t\t\t-- va bao \"het luot\" oan." + NL +
        "\t\t\tPlayerFunLib:GetTaskDailyCount(self.nTaskExpLimit)", 1)
    bak(p); wr(p, s)
    print("F: da them reset bo dem ngay khi kiem dieu kien vao")

# ---------------- G. lenh GM in -1 ----------------
p = os.path.join(E, r"item\hoatdong_admin.lua")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
old = '\tMsg2Player(format("'
i = s.find("GetTaskDailyCount and GetTaskDailyCount(2709)")
if i < 0:
    print("G: da va / khong thay anchor")
else:
    # thay ca bieu thuc bang bien da giai ma
    line_start = s.rfind(NL, 0, i) + len(NL)
    line_end = s.find(NL, i)
    old_line = s[line_start:line_end]
    new_block = NL.join([
        "\t-- [24/08 hau phan bien] GetTaskDailyCount la PHUONG THUC cua bang PlayerFunLib, goi",
        "\t-- nhu bien toan cuc se luon ra -1. Giai ma tai cho (yymmdd*256 + so_lan); KHONG",
        "\t-- Include playerfunlib.lua vi se tao lai vong Include gay tran stack dem 23/08.",
        "\tlocal nRaw2709 = GetTask(2709)",
        "\tlocal nNgayNay = tonumber(GetLocalDate(\"%y%m%d\"))",
        "\tlocal nDaDung = 0",
        "\tif (floor(nRaw2709 / 256) == nNgayNay) then",
        "\t\tnDaDung = mod(nRaw2709, 256)",
        "\tend",
    ])
    new_line = old_line.replace("GetTaskDailyCount and GetTaskDailyCount(2709) or -1", "nDaDung")
    s = s[:line_start] + new_block + NL + new_line + s[line_end:]
    bak(p); wr(p, s)
    print("G: da sua lenh GM (giai ma tai cho thay vi goi phuong thuc)")

print("XONG PHIA MAY CHU")
