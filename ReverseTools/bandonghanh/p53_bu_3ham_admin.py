# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Bu 3 ham menu admin bi thieu (goc "kich lenh bai admin
phan dong hanh bao loi"): p51 kiem 'if "BDH_P_CapKN" not in lf' nhung ten
do DA nam trong chuoi MENU -> dieu kien sai -> khong chen ham.
Kiem lai bang: moi muc menu /BDH_* phai co 'function <ten>('.
"""
import io
import re

CR = chr(13)
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\bdh_admin.lua"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")

HAM = {
    "BDH_P_CapKN": '''function BDH_P_CapKN()
	local i
	for i = 1, 5 do
		AddItem(6, 1, 4880, 1, 0, 0)
	end
	for i = 4881, 4886 do
		AddItem(6, 1, i, 1, 0, 0)
	end
	Msg2Player("Da cap 5 Bi kip + 6 trang bi")
end''',
    "BDH_P_XoaKN": '''function BDH_P_XoaKN()
	local i
	for i = 0, 3 do
		SetTask(5139 + i, 0)
	end
	Msg2Player("Da xoa het ky nang pet")
end''',
    "BDH_P_XoaTB": '''function BDH_P_XoaTB()
	local i
	for i = 0, 5 do
		SetTask(5143 + i, 0)
		SetTask(5150 + i, 0)
	end
	SetTask(5157, 0)
	SetTask(5158, 0)
	Msg2Player("Da thao het trang bi pet")
end''',
}
them = []
for ten, ma in HAM.items():
    if re.search(r"function\s+" + ten + r"\s*\(", lf):
        print("da co:", ten)
    else:
        them.append(ma)
if them:
    neo = "function BDH_P_CapDo()"
    assert lf.count(neo) == 1
    lf = lf.replace(neo, "\n\n".join(them) + "\n\n" + neo, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("VA: bu", len(them), "ham")

# kiem lai toan bo
s2 = io.open(p, "r", encoding="latin-1").read().replace(CR, "")
dinh = set(re.findall(r"function\s+([\w:.]+)\s*\(", s2))
goi = set(re.findall(r'/(BDH_[\w]+)"', s2))
thieu = sorted(goi - dinh)
print("KIEM: muc menu", len(goi), "| thieu ham:", thieu if thieu else "KHONG")
