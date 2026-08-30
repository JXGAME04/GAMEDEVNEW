# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Menu admin thieu muc cap TRANG BI (chu: "chua co cho
nhan trang bi moi"). Them 3 muc + rut ngan nhan de khong cham tran 512:
 - Cap bo Bich Huyet (4907..4916)
 - Cap bo Kim Lan   (4917..4926)
 - Cap ruong/chia/ket tinh (4929..4932 + 4927)
Tat ca AddItem deu 7 tham so (loi chi tu da va o p67).
"""
import io
import re

CR = chr(13)
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\bdh_admin.lua"
lf = io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")

i = lf.find("function BDH_P_Root()")
j = lf.find("\nend", i)
moi = '''function BDH_P_Root()
	SayEx({format("Pet cap %d - diem %d/%d/%d/%d - xu %d",
		PET_GetLevel(), PET_GetUpgradePoint(), PET_GetGrownPoint(),
		PET_GetTamePoint(), PET_GetXiuzhenPoint(), GetTask(251)),
	"Cap thiep/thuoc/trai/BDH_P_CapDo",
	"Cap 5 Bi kiep/BDH_P_CapKN",
	"Cap bo Bich Huyet/BDH_P_CapBo1",
	"Cap bo Kim Lan/BDH_P_CapBo2",
	"Cap ruong+chia+ket tinh/BDH_P_CapRuong",
	"Diem+chan nguyen+xu/BDH_P_Diem",
	"Tao pet nhanh/BDH_P_TaoNhanh",
	"Xoa ky nang/BDH_P_XoaKN",
	"Thao trang bi/BDH_P_XoaTB",
	"Dong bo client/BDH_P_DongBo",
	"Ket thuc doi thoai./no"})'''
lf = lf[:i] + moi + lf[j:]

them = '''function BDH_P_CapBo1()
	local i
	for i = 4907, 4916 do
		AddItem(6, 1, i, 1, 0, 0, 0)
	end
	Msg2Player("Da cap bo Bich Huyet 10 mon (4907..4916)")
end

function BDH_P_CapBo2()
	local i
	for i = 4917, 4926 do
		AddItem(6, 1, i, 1, 0, 0, 0)
	end
	Msg2Player("Da cap bo Kim Lan 10 mon (4917..4926)")
end

function BDH_P_CapRuong()
	local i
	for i = 1, 5 do
		AddItem(6, 1, 4929, 1, 0, 0, 0)
		AddItem(6, 1, 4930, 1, 0, 0, 0)
		AddItem(6, 1, 4931, 1, 0, 0, 0)
		AddItem(6, 1, 4932, 1, 0, 0, 0)
		AddItem(6, 1, 4927, 1, 0, 0, 0)
	end
	Msg2Player("Da cap 5 ruong 1/2/3 + 5 chia khoa + 5 ket tinh")
end

'''
if "function BDH_P_CapBo1" not in lf:
    neo = "function BDH_P_CapKN()"
    assert lf.count(neo) == 1
    lf = lf.replace(neo, them + neo, 1)

# CapKN chi con bi kiep (bo phan trang bi da tach ra)
i2 = lf.find("function BDH_P_CapKN()")
j2 = lf.find("\nend", i2)
moi2 = '''function BDH_P_CapKN()
	local i
	for i = 1, 5 do
		AddItem(6, 1, 4880, 1, 0, 0, 0)
	end
	Msg2Player("Da cap 5 Bi kiep ban dong hanh")'''
lf = lf[:i2] + moi2 + lf[j2:]

io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))

# kiem
kh = lf[lf.find("function BDH_P_Root()"):lf.find("\nend", lf.find("function BDH_P_Root()"))]
nhan = re.findall(r'"([^"]*)"', kh)
byte = sum(len(x) + 1 for x in nhan) + 50
dinh = set(re.findall(r"function\s+([\w:.]+)\s*\(", lf))
goi = set(re.findall(r'/(BDH_[\w]+)"', lf))
print("menu: %d muc, ~%d byte (tran 512)" % (len(nhan), byte))
print("ham thieu:", sorted(goi - dinh) or "KHONG")
print("so AddItem 6 tham so con lai:",
      len(re.findall(r"AddItem\(\s*\d+\s*,\s*\d+\s*,\s*[\w\d]+\s*,\s*\d+\s*,\s*\d+\s*,\s*\d+\s*\)", lf)))
