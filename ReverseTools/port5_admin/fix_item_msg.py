# -*- coding: utf-8 -*-
r"""[24/08] Va NOT 2 loi con lai cua item hoat dong.

B6 [quan trong] Dung item xong KHONG CO THONG BAO NAO -> nguoi test tuong item hong.
   Ban Linux moi lan trao thuong deu ban 1 dong cho nguoi choi:
     script\lib\awardtype\exp.lua:7   "Dat duoc kinh nghiem khong the cong don %s"
     script\lib\awardtype\exp.lua:13  "Nhan duoc diem kinh nghiem cong don %s."
     script\lib\awardtype\zhenyuanpoint.lua:13 "Nhan duoc %d diem chan nguyen"
   Ban JX1 chi ghi log, khong Msg2Player. Them vao.
   DUNG %s cho exp (KHONG dung %d) - dung nhu ban Linux ghi chu: 2 ty TRAN dinh dang so nguyen.

B5 Hoang Chan Don: thoai ghi "60 ngay" nhung khoa THAT la 30 ngay
   (BeiDouChuanGong\head.lua:10 TSKV_..._UseLimit = 30*60*24*60 = 30 ngay; chu thich "60 ngay"
   la chu cu chua sua). Doi chu trong thoai cho khop hang so.
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
ENC = "latin-1"
def rd(p): return io.open(p, "r", encoding=ENC, newline="").read()
def wr(p, s): io.open(p, "w", encoding=ENC, newline="").write(s)
def bak(p, t):
    q = p + ".truoc_" + t
    if not os.path.isfile(q): shutil.copyfile(p, q)

# chuoi TCVN3
S_EXP_KHONG_CONG_DON = "\xae\xb9t \xae\xdaîc kinh nghi\xd6m kh\xf4ng th\xd3 c\xe9ng d\xabn %s"
S_EXP_CONG_DON       = "Nh\xadn \xae\xdaîc \xaei\xd3m kinh nghi\xd6m c\xe9ng d\xabn %s."
S_ZHENYUAN           = "Nh\xadn \xae\xdaîc %d \xaei\xd3m ch\xa9n nguy\xaan"

# ---------------- B6a: exp_jx1.lua ----------------
p = os.path.join(E, r"lib\awardtype\exp_jx1.lua")
s = rd(p)
if "Msg2Player" in s:
    print("B6a: da co thong bao")
else:
    NL = "\r\n" if "\r\n" in s else "\n"
    a1 = '\t\tWriteLog(format("[awardtype exp_jx1] %s +%d exp", GetName() or "", nExp))'
    assert s.count(a1) == 1, "anchor exp thuong = %d" % s.count(a1)
    s = s.replace(a1, a1 + NL +
        '\t\t-- [FIX 24/08] ban Linux (lib\\awardtype\\exp.lua:7) BAN thong bao cho nguoi choi;' + NL +
        '\t\t-- thieu no thi an item xong im lang, nguoi choi tuong item hong.' + NL +
        '\t\t-- Dung %s (KHONG phai %d): 2 ty TRAN dinh dang so nguyen.' + NL +
        '\t\tMsg2Player(format("' + S_EXP_KHONG_CONG_DON + '", nExp))', 1)
    a2 = '\t\tWriteLog(format("[awardtype exp_jx1] %s +%d exp (tl)", GetName() or "", nExp))'
    assert s.count(a2) == 1, "anchor exp tl = %d" % s.count(a2)
    s = s.replace(a2, a2 + NL +
        '\t\t-- [FIX 24/08] khuon ban Linux lib\\awardtype\\exp.lua:13' + NL +
        '\t\tMsg2Player(format("' + S_EXP_CONG_DON + '", nExp))', 1)
    bak(p, "msg_2408"); wr(p, s)
    print("B6a: exp_jx1.lua da them 2 thong bao")

# ---------------- B6b: zhenyuan_jx1.lua ----------------
p = os.path.join(E, r"lib\awardtype\zhenyuan_jx1.lua")
s = rd(p)
if "Msg2Player" in s:
    print("B6b: da co thong bao")
else:
    NL = "\r\n" if "\r\n" in s else "\n"
    a = '\tWriteLog(format("[TongCastle awardtype zhenyuan] %s +%d diem Chan Nguyen", GetName() or "", nPoint))'
    assert s.count(a) == 1, "anchor zhenyuan = %d" % s.count(a)
    s = s.replace(a, a + NL +
        '\t-- [FIX 24/08] ban Linux (lib\\awardtype\\zhenyuanpoint.lua:13) co thong bao nay;' + NL +
        '\t-- thieu no thi dung Than Moc Lenh xong khong biet co nhan duoc gi khong.' + NL +
        '\tMsg2Player(format("' + S_ZHENYUAN + '", nPoint))', 1)
    bak(p, "msg_2408"); wr(p, s)
    print("B6b: zhenyuan_jx1.lua da them thong bao")

# ---------------- B5: 60 ngay -> 30 ngay ----------------
p = os.path.join(E, r"item\huangzhendan.lua")
s = rd(p)
b = s.encode(ENC)
old = "60 ng\xb5y".encode(ENC)
new = "30 ng\xb5y".encode(ENC)
c = b.count(old)
if c == 0:
    print("B5: da sua / khong thay chuoi '60 ngay'")
else:
    bak(p, "ngay_2408")
    b = b.replace(old, new)
    wr(p, b.decode(ENC))
    print("B5: doi %d cho '60 ngay' -> '30 ngay' (hang so that la 30 ngay)" % c)

print("XONG")
