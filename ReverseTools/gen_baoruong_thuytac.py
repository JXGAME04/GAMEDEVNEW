# -*- coding: utf-8 -*-
"""Sinh script\\missions\\fengling_ferry\\hd3_baoruongthuytac.lua - BANG THUONG BAN
LINUX cua Bao Ruong Thuy Tac (chu game yeu cau thay ban Viet).

NGUON: D:\\ServerLinux\\server1\\script\\activitysys\\config\\17\\vnshuizeibaoxiang.lua
(ban VNG dang duoc dung that: extend.lua:59 goi VnPirateBox_main).
Khuon giong het chuangguanbaoxiang.lua (Bao ruong Vuot ai) DA PORT va chay duoc.

REMAP ID (tra theo TEN trong settings/item/magicscript.txt cua JX1):
  ruong  6,1,2743 -> 6,1,3361      chia nhu y 6,1,2744  -> 6,1,3362
  chia vang 6,1,30191 -> 6,1,2953  (dung cung bo voi ruong Vuot ai da port)
  Chan Nguyen Dan      4134  -> 3926      Chan Nguyen Don (trung) 30228 -> 4846
  Chan Nguyen Don (dai) 30229 -> 4847     Hon nguyen chan don     30301 -> 4857
  Ho Mach Don          3203  -> 3821      Huyet Long Dang         30289 -> 3051
  Tinh Tinh Khoang     3811  -> 4429      Tinh Thiet Khoang       3810  -> 4428
  Do Pho Dang Long Y   30529 -> 4858      Do Pho Dang Long Khi Gioi 30537 -> 4859
  Tinh Suong Lenh      30506 -> 4860      Huyen Thiet             30507 -> 4861
  Do Pho Tinh Suong Y  30006 -> 4862      Do Pho Tinh Suong Khi Gioi 30505 -> 4863
"""
import io, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
REL = r"script\missions\fengling_ferry\hd3_baoruongthuytac.lua"

# nhanh chia NHU Y: chi diem kinh nghiem (giong het ban Linux)
NHUY_EXP = [(1000000, 52), (2000000, 30), (3000000, 10), (4000000, 5), (5000000, 3)]
# nhanh chia VANG: vat pham + diem kinh nghiem
VANG_ITEM = [
    # (ten, bo JX1, cap, so luong, ti le, bind)
    ("Chân Nguyên Đan",            3926, 1, 10,   5,    None),
    ("Chân Nguyên Đơn (Trung)",    4846, 1, 7,    10,   -2),
    ("Chân Nguyên Đơn (Đại)",      4847, 1, 7,    5,    -2),
    ("Hỗn nguyên chân đơn",        4857, 1, 1,    0.01, None),
    ("Hộ Mạch Đơn",                3821, 1, 50,   10,   None),
    ("Huyết Long Đằng Cấp 9",      3051, 9, 5,    1.2,  None),
    ("Huyết Long Đằng Cấp 11",     3051, 11, 5,   1.1,  None),
    ("Huyết Long Đằng Cấp 12",     3051, 12, 5,   0.5,  None),
    ("Tinh Tinh Khoáng",           4429, 1, 1,    1,    None),
    ("Tinh Thiết Khoáng",          4428, 1, 1,    0.5,  None),
    ("Đồ Phổ Đằng Long Y",         4858, 1, 1,    0.002, None),
    ("Đồ Phổ Đằng Long Khí Giới",  4859, 1, 1,    0.001, None),
    ("Tinh Sương Lệnh",            4860, 1, 1,    0.007, None),
    ("Huyền Thiết",                4861, 1, 1,    0.03, None),
    ("Đồ Phổ Tinh Sương Y",        4862, 1, 1,    0.007, None),
    ("Đồ Phổ Tinh Sương Khí Giới", 4863, 1, 1,    0.006, None),
]
VANG_EXP = [(2000000, 29.637), (4000000, 15), (5000000, 10), (6000000, 5), (8000000, 4), (10000000, 2)]

TEN_RUONG = "Bảo Rương Thủy Tặc"
L = []
A = L.append
A("-- ============================================================================")
A("-- HD3_BAORUONGTHUYTAC.LUA - SINH TU DONG boi ReverseTools/gen_baoruong_thuytac.py")
A("-- DUNG SUA TAY. Bang thuong ban LINUX (activitysys/config/17/vnshuizeibaoxiang.lua)")
A("-- thay cho ban Viet cu (script/item/baoruongthuytac.lua chi cho phao hoa).")
A("-- Khuon giong chuangguanbaoxiang.lua (Bao ruong Vuot ai) da port va chay duoc.")
A("-- Doi bang thuong o cauhinh_hoatdong.lua khoa HD3_PLD_THUONG_RUONG (nil = bang nay).")
A("-- ============================================================================")
A('Include("\\\\script\\\\lib\\\\awardtemplet.lua")')
A('Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")')
A('Include("\\\\script\\\\vng_event\\\\change_request_baoruong\\\\exp_award.lua")')
A('Include("\\\\script\\\\lib\\\\objbuffer_head.lua")')
A('Include("\\\\script\\\\misc\\\\eventsys\\\\type\\\\func.lua")')
A("")
A("tbPirate_Box_Key_Require = {")
A('\t["chiakhoanhuy"] = {6, 1, 3362},')
A('\t["chiakhoavang"] = {6, 1, 2953},')
A("}")
A("")
A("tbPirateBoxNewAward =")
A("{")
A('\t["chiakhoanhuy"] =')
A("\t{")
for i, (exp, rate) in enumerate(NHUY_EXP, 1):
    A('\t\t{szName="' + V("Điểm kinh nghiệm ") + str(i) + '",')
    A("\t\t\tpFun = function (tbItem, nItemCount, szLogTitle)")
    A("				" + chr(37) + 'tbvng_ChestExpAward:ExpAward(' + str(exp) + ', "' + V(TEN_RUONG) + '")')
    A("\t\t\tend,")
    A("\t\t\tnRate = %s," % rate)
    A("\t\t},")
A("\t},")
A('\t["chiakhoavang"] =')
A("\t{")
for (ten, pid, cap, cnt, rate, bind) in VANG_ITEM:
    s = '\t\t{szName="%s",tbProp={6,1,%d,%d,0,0},nCount=%d,nRate=%s' % (V(ten), pid, cap, cnt, rate)
    if bind is not None:
        s += ",nBindState=%d" % bind
    A(s + "},")
for i, (exp, rate) in enumerate(VANG_EXP, 1):
    A('\t\t{szName="' + V("Điểm kinh nghiệm ") + str(i) + '",')
    A("\t\t\tpFun = function (tbItem, nItemCount, szLogTitle)")
    A("				" + chr(37) + 'tbvng_ChestExpAward:ExpAward(' + str(exp) + ', "' + V(TEN_RUONG) + '")')
    A("\t\t\tend,")
    A("\t\t\tnRate = %s," % rate)
    A("\t\t},")
A("\t},")
A("}")
A("")
A("nWidth = 1")
A("nHeight = 1")
A("nFreeItemCellLimit = 1")
A("")
A("function main(nIndexItem)")
A('\tlocal tbKey1 = tbPirate_Box_Key_Require["chiakhoanhuy"]')
A('\tlocal tbKey2 = tbPirate_Box_Key_Require["chiakhoavang"]')
A("\tlocal nCount1 = CalcItemCount(3, tbKey1[1], tbKey1[2], tbKey1[3], -1)")
A("\tlocal nCount2 = CalcItemCount(3, tbKey2[1], tbKey2[2], tbKey2[3], -1)")
A("\tif (nCount1 == 0 and nCount2 == 0) then")
A('\t\tSay("' + V("Cần có Chìa Khóa Vàng hoặc Chìa Khóa Nhũ ý mới mở được Bảo Rương Thủy Tặc.") + '", 1, "' + V("Đóng") + '/no")')
A("\t\treturn 1")
A("\tend")
A("\tif (CountFreeRoomByWH(nWidth, nHeight, nFreeItemCellLimit) < nFreeItemCellLimit) then")
A('\t\tSay("' + V("Hãy chừa trống ít nhất 1 ô hành trang trước khi mở.") + '", 0)')
A("\t\treturn 1")
A("\tend")
A("\tlocal tbOpt = {}")
A("\tif (nCount1 ~= 0) then")
A('\t\ttinsert(tbOpt, format("' + V("Dùng Chìa khóa nhũ ý") + '/#HD3_PLD_MoRuong(%d, ' + "'%s'" + ')", nIndexItem, "chiakhoanhuy"))')
A("\tend")
A("\tif (nCount2 ~= 0) then")
A('\t\ttinsert(tbOpt, format("' + V("Dùng Chìa khóa vàng") + '/#HD3_PLD_MoRuong(%d, ' + "'%s'" + ')", nIndexItem, "chiakhoavang"))')
A("\tend")
A('\ttinsert(tbOpt, "' + V("Đóng") + '/no")')
A('\tSay("' + V("Chọn chìa khóa để mở rương:") + '", getn(tbOpt), tbOpt)')
A("\treturn 1")
A("end")
A("")
A("function no()")
A("end")
A("")
A("function HD3_PLD_MoRuong(nItemIdx, strKeyType)")
A('\ttbPirateBoxNewAward = HD_CFG("HD3_PLD_THUONG_RUONG", nil) or tbPirateBoxNewAward')
A("\tlocal tbKey = tbPirate_Box_Key_Require[strKeyType]")
A("\tlocal tbAward = tbPirateBoxNewAward[strKeyType]")
A("\tif (not tbKey or not tbAward) then")
A("\t\treturn")
A("\tend")
A("\tif (ConsumeItem(3, 1, tbKey[1], tbKey[2], tbKey[3], -1) ~= 1) then")
A('\t\tSay("' + V("Không tìm thấy chìa khóa.") + '", 1, "' + V("Đóng") + '/no")')
A("\t\treturn")
A("\tend")
A("\tif (ConsumeItem(3, 1, 6, 1, 3361, -1) ~= 1) then")
A('\t\tSay("' + V("Không tìm thấy Bảo Rương Thủy Tặc.") + '", 1, "' + V("Đóng") + '/no")')
A("\t\treturn")
A("\tend")
A('\ttbAwardTemplet:Give(tbAward, 1, {"PhongLangDo", "use_shuizeibaoxiang"})')
A('\tEventSys:GetType("OpenFuncAwardBox"):OnPlayerEvent("OpenAwardBoxEvent", PlayerIndex)')
A("end")
A("")
body = "\r\n".join(L)
p = os.path.join(SRV, REL)
io.open(p, "w", encoding="latin-1", newline="").write(body)
dst = os.path.join(MIR, REL)
os.makedirs(os.path.dirname(dst), exist_ok=True)
io.open(dst, "w", encoding="latin-1", newline="").write(body)
print("da sinh", REL, "-", len(L), "dong")
