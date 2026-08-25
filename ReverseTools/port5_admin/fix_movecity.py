# -*- coding: utf-8 -*-
r"""[25/08] Bit lo cuoi cua Tin Su: tu roi map bang Dich Quan khi dang lam do (1203=20).

DOI CHIEU LINUX (script 2 ban GIONG HET tung dong - da diff):
 1. Linux KHONG BAO GIO tu huy nhiem vu khi roi map. 1203 chi ve 0 khi: tu xoa
    (losetask) hoac HET GIO (middletime). Da liet ke 100% diem doi 1203.
 2. Duong roi DUOC THIET KE = Tieu Tran > "Ta chua hoan thanh khao nghiem, ta muon
    di ra mot chut" (messenger_icantdo, turenpc.lua:168-176): don trang thai +
    nt_setTask(1203,21) TAM NGAT + SetPos ra cong. Sau do roi map thi 1203 da la 21,
    vao lai bam "Tiep tuc" -> nhanh 21 -> ture_real_starttask bat lai DU trang thai.
 3. Duong tat = Dich Quan > "Roi khoi khu vuc" (ture_movecity) nhanh 1203==20:
    Linux GIU 20. Tren Linux van lam tiep duoc vi engine Linux khong khoa danh-quai
    theo rut-vu-khi; engine JX1 CO khoa (KNpcSet.cpp:1553-1558 + KNpc.cpp:1477)
    => tren JX1 duong tat nay lam ket vinh vien.

VA (1 dong, dung DUNG pattern Kingsoft cua messenger_icantdo trong CUNG cum file):
    trong ture_movecity, truoc khi NewWorld o nhanh (10/0/20/21): neu 1203==20 thi
    dat 21 - moi duong-roi-map hoi tu ve 21, "Tiep tuc" luon bat du trang thai.
    Hanh vi cuoi = giong Linux (vao lai lam tiep binh thuong).
"""
import io, os, shutil

P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\task\tollgate\messenger\messenger_turerukou.lua"
ENC = "latin-1"
s = io.open(P, "r", encoding=ENC, newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"

if "FIX 25/08 roi map" in s:
    print("da va truoc do"); raise SystemExit

cu = ("\telseif ( nt_getTask(1203) == 10 ) or ( nt_getTask(1203) == 0 ) or ( nt_getTask(1203) == 20) or ( nt_getTask(1203) == 21 ) then" + NL +
      "\t\tSetLogoutRV(0);" + NL)
assert s.count(cu) == 1, "anchor movecity = %d" % s.count(cu)
moi = ("\telseif ( nt_getTask(1203) == 10 ) or ( nt_getTask(1203) == 0 ) or ( nt_getTask(1203) == 20) or ( nt_getTask(1203) == 21 ) then" + NL +
       "\t\t-- [FIX 25/08 roi map] dang lam do (20) ma roi bang duong tat nay thi dat TAM NGAT (21)" + NL +
       "\t\t-- - dung pattern messenger_icantdo (turenpc.lua:169) cua chinh Kingsoft. Linux giu 20" + NL +
       "\t\t-- van lam tiep duoc vi engine Linux khong khoa danh-quai theo rut-vu-khi; engine JX1" + NL +
       "\t\t-- co khoa (KNpcSet.cpp:1553) nen phai hoi tu ve 21 de 'Tiep tuc' bat lai du trang thai." + NL +
       "\t\tif ( nt_getTask(1203) == 20 ) then" + NL +
       "\t\t\tnt_setTask(1203,21)" + NL +
       "\t\tend" + NL +
       "\t\tSetLogoutRV(0);" + NL)
s = s.replace(cu, moi, 1)

q = P + ".truoc_movecity_2508"
if not os.path.isfile(q): shutil.copyfile(P, q)
io.open(P, "w", encoding=ENC, newline="").write(s)
print("DA VA ture_movecity (nhanh 20 -> dat 21 truoc khi roi)")
