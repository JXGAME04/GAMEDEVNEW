# -*- coding: utf-8 -*-
"""vhtd_test_skill150.py [VHTD 02/09f] - NPC test (Ba Lang Huyen -> "Hoc vo cong" -> trang 2 "Hoc skill 90"):
theo lenh chu "them skill 150 cac phai de test lai het mot lan luon".
  skillvuhon  : add_wh(90)  -> add_wh(150)   (ham add_wh cap don theo nguong 10..70, 90, 120, 150 -> 150 = HOC TAT CA)
  skilltieudao: add_xy(90)  -> add_xy(150)
  skillhoason : add_hs(90)  -> add_hs(150) + cap THANG 1364/1382 (90), 1365 (120), 1369/1384 (150), 1370 (tien giai)
                vi add_hs la ban CHEP NGUYEN VAN tu Linux, chi co bac 10..70 (ky nang 90+ cua Hoa Son den tu sach/NPC)
                -> KHONG sua add_hs (dung chung voi luc nhap mon), chi cap trong ham TEST nay.
Doc/ghi latin-1, ban luu .truoc_vhtd_0209 (neu chua co), idempotent. DUNG: python vhtd_test_skill150.py [--kiem]
"""
import io, os, sys, shutil
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
def V(s): return unicode_to_tcvn3_bytes(s).decode("latin-1")

KIEM = "--kiem" in sys.argv
P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\global\hocvocong.lua"
BAK = ".truoc_vhtd_0209"
NL = "\r\n"
TAB = "\t"
HS_THEM = (NL + TAB + "AddMagic(1364,20) AddMagic(1382,20)" + TAB + "-- [VHTD 02/09f] Hoa Son 90 (add_hs ban Linux khong co bac 90+)" +
           NL + TAB + "AddMagic(1365,20)" + TAB + "-- 120" +
           NL + TAB + "AddMagic(1369,20) AddMagic(1384,20)" + TAB + "-- 150" +
           NL + TAB + "AddMagic(1370,20)" + TAB + "-- tien giai")

s = io.open(P, "r", encoding="latin-1", newline="").read()
orig = s
MSG_CU = V(u"Ng\u01b0\u01a1i \u0111\u00e3 h\u1ecdc \u0111\u01b0\u1ee3c skill 90")
MSG_MOI = V(u"Ng\u01b0\u01a1i \u0111\u00e3 h\u1ecdc \u0111\u01b0\u1ee3c to\u00e0n b\u1ed9 k\u1ef9 n\u0103ng (nh\u1eadp m\u00f4n \u0111\u1ebfn 150)")

for fn, add in (("skillvuhon", "add_wh"), ("skilltieudao", "add_xy"), ("skillhoason", "add_hs")):
    i = s.find("function %s()" % fn)
    if i < 0: raise SystemExit("khong thay function %s()" % fn)
    j = s.find(NL + "end", i) + len(NL) + 3
    blk = s[i:j]
    if "%s(150)" % add in blk:
        print("  [=] %s da la %s(150)" % (fn, add)); continue
    if "%s(90)" % add not in blk: raise SystemExit("%s: khong thay %s(90)" % (fn, add))
    blk2 = blk.replace("%s(90);" % add,
                       "%s(150);%s-- [VHTD 02/09f] 150 = hoc TAT CA (10..70, 90, 120, 150) de test 1 lan" % (add, TAB), 1)
    blk2 = blk2.replace('"%s"' % MSG_CU, '"%s"' % MSG_MOI, 1)
    if fn == "skillhoason" and "AddMagic(1364,20)" not in blk2:
        k = blk2.find("add_hs(150);")
        k = blk2.find(NL, k)
        blk2 = blk2[:k] + HS_THEM + blk2[k:]
    s = s[:i] + blk2 + s[j:]
    print("  [+] %s: %s(90) -> %s(150)%s" % (fn, add, add, " + 90/120/150 truc tiep" if fn == "skillhoason" else ""))

if s != orig:
    hib = lambda t: sum(1 for c in t if ord(c) >= 0x80)
    if "\xef\xbf\xbd" in s: raise SystemExit("EF BF BD")
    print("  byte cao: %d -> %d (chenh do doi cau thong bao)" % (hib(orig), hib(s)))
    if not KIEM:
        if not os.path.exists(P + BAK): shutil.copy2(P, P + BAK)
        io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("  => ghi hocvocong.lua%s" % (" KIEM" if KIEM else ""))
else:
    print("  (khong doi)")
print("XONG.")
