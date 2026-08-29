# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Menu admin PET chi hien 1 muc "Ket thuc doi thoai" =
Say/SayEx TRAN 512 BYTE (tieu de dong + 7 nhan dai ~484 + so 5 chu so ->
vuot, engine cat im lang cac muc). Rut gon con < 300 byte.
"""
import io
import re

CR = chr(13)
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\bdh_admin.lua"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
i = lf.find("function BDH_P_Root()")
j = lf.find("\nend", i)
assert i > 0 and j > i
moi = '''function BDH_P_Root()
	SayEx({format("Pet cap %d - diem %d/%d/%d/%d - xu %d",
		PET_GetLevel(), PET_GetUpgradePoint(), PET_GetGrownPoint(),
		PET_GetTamePoint(), PET_GetXiuzhenPoint(), GetTask(251)),
	"Cap thiep + thuoc + trai/BDH_P_CapDo",
	"Cap bi kip + trang bi/BDH_P_CapKN",
	"Diem + chan nguyen + xu/BDH_P_Diem",
	"Tao pet nhanh/BDH_P_TaoNhanh",
	"Xoa ky nang/BDH_P_XoaKN",
	"Thao trang bi/BDH_P_XoaTB",
	"Dong bo client/BDH_P_DongBo",
	"Ket thuc doi thoai./no"})'''
lf = lf[:i] + moi + lf[j:]
io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))

kh = lf[lf.find("function BDH_P_Root()"):lf.find("\nend", lf.find("function BDH_P_Root()"))]
nhan = re.findall(r'"([^"]*)"', kh)
byte = sum(len(x) + 1 for x in nhan) + 40   # +40: so lieu format thay %d
print("menu moi: %d muc, ~%d byte (tran 512)" % (len(nhan), byte))
dinh = set(re.findall(r"function\s+([\w:.]+)\s*\(", lf))
goi = set(re.findall(r'/(BDH_[\w]+)"', lf))
print("kiem ham thieu:", sorted(goi - dinh) or "KHONG")
