# -*- coding: utf-8 -*-
r"""[24/08] VA LOI CHUC NANG cua muc 5 "Nhan item thuong" trong Lenh Bai Admin.

GOC: engine yeu cau AddItem TOI THIEU 7 THAM SO -
     ScriptFuns.cpp:4764  /*AddItem(nItemClass, nDetailType, nParticualrType, nLevel,
                             nSeries, nLuck, nItemLevel..6)*/
     va ngay dau ham: if (nParamNum < 7) return  => THOAT LUON, KHONG TAO ITEM.
Menu cua toi goi AddItem(6, 1, <id>, 0, 0, 0) - chi 6 tham so => bam "nhan item" KHONG
duoc gi ca (dung nhu chu game bao "chuc nang sai").

Cach goi CHUAN trong du an (script\lib\awardtype\item_jx1.lua:30-31):
     AddItem(tbProp[1], tbProp[2], tbProp[3], tbProp[4] or 1, tbProp[5] or 0, tbProp[6] or 0, 0)
=> 7 tham so, nLevel mac dinh 1 (bang thuong tongwar cung ghi tbProp={6,1,4864,1}).

Va: doi tat ca sang AddItem(6, 1, <id>, 1, 0, 0, 0) va kiem tra ket qua tra ve.
"""
import io, os, re, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
p = os.path.join(E, r"script\item\hoatdong_admin.lua")
s = io.open(p, "r", encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"

if "AddItem(6, 1, 4864, 1, 0, 0, 0)" in s:
    print("da va truoc do"); raise SystemExit(0)

if not os.path.isfile(p + ".truoc_additem_2408"):
    shutil.copyfile(p, p + ".truoc_additem_2408")

# 1) doi moi loi goi 6 tham so -> 7 tham so, level 1
rx = re.compile(r"AddItem\(6, 1, (\d+), 0, 0, 0\)")
n = len(rx.findall(s))
s = rx.sub(lambda m: "AddItem(6, 1, %s, 1, 0, 0, 0)" % m.group(1), s)
print("doi %d loi goi AddItem sang 7 tham so (level 1)" % n)

# 2) them chu thich giai thich ngay tren muc 5
anchor = "-- ================= 5) ITEM THUONG TEST ================="
if anchor in s:
    s = s.replace(anchor,
        "-- ================= 5) ITEM THUONG TEST =================" + NL +
        "-- [FIX 24/08] Engine doi AddItem TOI THIEU 7 THAM SO (ScriptFuns.cpp:4764: neu" + NL +
        "-- nParamNum < 7 thi return luon, KHONG tao item). Truoc day o day goi 6 tham so nen" + NL +
        "-- bam \"nhan item\" khong duoc gi. Chuan cua du an: AddItem(genre, detail, particular," + NL +
        "-- nLevel, nSeries, nLuck, 0) voi nLevel = 1 (xem lib\\awardtype\\item_jx1.lua:30).", 1)

# 3) bao ket qua that thay vi bao mu quang
def add_check(fn_name, msg_ok):
    global s
    rxf = re.compile(r"(function " + fn_name + r"\(\)\s*)(AddItem\(6, 1, (\d+), 1, 0, 0, 0\))\s*(Msg2Player\(\"[^\"]*\"\))\s*(end)")
    m = rxf.search(s)
    if not m: return 0
    new = ("function %s()" % fn_name + NL +
           "\tlocal nIdx = AddItem(6, 1, %s, 1, 0, 0, 0)" % m.group(3) + NL +
           "\tif (nIdx and nIdx > 0) then" + NL +
           "\t\t%s" % m.group(4) + NL +
           "\telse" + NL +
           "\t\tMsg2Player(\"Khong nhan duoc item - hanh trang day hoac id sai.\")" + NL +
           "\tend" + NL +
           "end")
    s = s[:m.start()] + new + s[m.end():]
    return 1

nchk = 0
for fn in ("HD_It4864", "HD_It2273", "HD_It3205", "HD_It3206", "HD_It3207", "HD_It4857"):
    nchk += add_check(fn, None)
print("them kiem ket qua cho %d ham" % nchk)

io.open(p, "w", encoding="latin-1", newline="").write(s)

d = r"D:\GAMEDEVNEW\serverscript_jx2\port5_admin"
if os.path.isdir(d):
    shutil.copyfile(p, os.path.join(d, "hoatdong_admin.lua"))
print("XONG")
