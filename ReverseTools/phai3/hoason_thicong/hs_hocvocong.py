# -*- coding: utf-8 -*-
"""hs_hocvocong.py [HOASON 01/09c] - global\\hocvocong.lua (NPC Ho Tro Test / quanly / hotrotanthu):
  1. SKILLNORMAL[11] Hoa Son (bang RIENG cua tep nay, khac factionhead.lua) -> het loi getn(nil) o hockynang:774
  2. hvccl() tra ve hockynang(10) = Con Lon (bi doi thanh 11); them hvchs() = hockynang(11) + muc menu 'Hoc vo cong mon phai Hoa Son'
  3. tbAllSkill2.huashan (HoTroSkill)
  4. menu test 90 (hocskill902): 'Hoa Son 90/skillhoason' -> add_hs(90)
"""
import io, os, sys
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
MARK = "[HOASON 01/09c]"
def V(s): return unicode_to_tcvn3_bytes(s).decode("latin-1")
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\global\hocvocong.lua"
d = io.open(p, "r", encoding="latin-1", newline="").read()
nl = "\r\n" if "\r\n" in d else "\n"
if MARK in d:
    print("da va roi"); sys.exit(0)
def thay(old, new, so=1):
    global d
    c = d.count(old); assert c == so, "neo %d lan (can %d): %r" % (c, so, old[:70])
    d = d.replace(old, new)

# 1. SKILLNORMAL[11]
sk = ["{1347,0},---Bach Hong Quan Nhat", "{1372,0},---Thanh Phong Tong Sang", "{1349,0},---Kiem Tong Tong Quyet", "{1374,0},---Long Nhieu Than",
      "{1350,0},---Duong Ngo Kiem Phap", "{1375,0},---Hai Nap Bach Xuyen", "{1351,0},---Kim Nhan Hoanh Khong", "{1376,0},---Long Huyen Kiem Khi",
      "{1354,0},---Hi Di Kiem Phap", "{1378,0},---Khi Chan Son Ha", "{1355,0},---Thien Than Dao Huyen", "{1379,0},---Khi Quan Truong Hong",
      "{1358,0},---Huyen Nhan Van Yen - tran phai", "{1360,0},---Thuong Tung Nghenh Khach", "{1380,0}---Ma Van Kiem Khi"]
old = "\t\t{630,0}---Huyen Thien Vo cuc" + nl + "\t}" + nl + "}" + nl
new = ("\t\t{630,0}---Huyen Thien Vo cuc" + nl + "\t}," + nl + "\t[11]={--hoa son " + MARK + " (Linux add_hs 10..70; bang RIENG cua hocvocong.lua)" + nl
       + nl.join("\t\t" + s for s in sk) + nl + "\t}" + nl + "}" + nl)
thay(old, new)
# 2. hvccl -> 10 ; them hvchs ; menu hocvocong2
old = "function hvccl()" + nl + "\t--HoTroSkill(\"kunlun\")" + nl + "\thockynang(11)" + nl + "end" + nl
new = ("function hvccl()" + nl + "\t--HoTroSkill(\"kunlun\")" + nl + "\thockynang(10)\t-- " + MARK + " Con Lon = SKILLNORMAL[10] (ban .truoc_hoason_0109 la 10; 11 nay la Hoa Son)" + nl + "end" + nl
       + "function hvchs()\t-- " + MARK + " Hoa Son = SKILLNORMAL[11]" + nl + "\t--HoTroSkill(\"huashan\")" + nl + "\thockynang(11)" + nl + "end" + nl)
thay(old, new)
old = "\tSay(\"" + V("Chào") + ": \",6," + nl + "\t\"Trang 1/hocvocong\","
assert d.count(old) == 1, "neo menu hocvocong2"
thay(old, "\tSay(\"" + V("Chào") + ": \",7," + nl + "\t\"Trang 1/hocvocong\",")
old = "\t\"" + V("Học võ công môn phái Côn Luân") + "/hvccl\"," + nl
thay(old, old + "\t\"" + V("Học võ công môn phái Hoa Sơn") + "/hvchs\"," + nl)
# 3. tbAllSkill2.huashan
old = "\tkunlun = {" + nl + "\t\t[1] = {250,252},"
assert d.count(old) == 1, "neo kunlun tbAllSkill2"
i = d.find(old); j = d.find(nl + "\t}," + nl, i)
assert j > 0
hs = ("\t}," + nl + "\thuashan = {\t-- " + MARK + nl + "\t\t[1] = {1347,1372}," + nl + "\t\t[2] = {1349,1374}," + nl + "\t\t[3] = {1350,1375}," + nl
      + "\t\t[4] = {1351,1376}," + nl + "\t\t[5] = {1354,1378}," + nl + "\t\t[6] = {1355,1379}," + nl + "\t\t[7] = {1358,1360,1380}," + nl
      + "\t\t[9] = {1364,1382,1365}," + nl)
d = d[:j] + nl + hs + d[j + len(nl + "\t}," + nl):]
# 4. menu test 90: hocskill902 (6 -> 7) + skillhoason
old = "\tSay(\"" + V("Ngươi muốn học kỹ năng môn phái nào?") + "\",6," + nl + "\t\"Trang 1/hocskill90\","
assert d.count(old) == 1, "neo hocskill902"
thay(old, "\tSay(\"" + V("Ngươi muốn học kỹ năng môn phái nào?") + "\",7," + nl + "\t\"Trang 1/hocskill90\",")
old = " 90/skillconluan\"," + nl
assert d.count(old) == 1, "neo dong Con luan 90"
thay(old, " 90/skillconluan\",\"" + V("Hoa Sơn") + " 90/skillhoason\"," + nl)
old = "function skillconluan()" + nl + "\tadd_kl(90);" + nl
thay(old, "function skillhoason()\t-- " + MARK + nl + "\tadd_hs(90);" + nl + "\tSay(\"" + V("Ngươi đã học được skill 90") + "\",0)" + nl + "end" + nl + nl + old)
io.open(p, "w", encoding="latin-1", newline="").write(d)
print("ghi", p)
