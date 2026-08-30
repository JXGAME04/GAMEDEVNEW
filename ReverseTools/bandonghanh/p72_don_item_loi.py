# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Chu bi item LOI dinh tren chuot -> khong bam duoc NPC.
Nguyen nhan: cac item cap ra TRUOC khi sua bang (ma khong khop dong ->
ban ghi rong) nen client khong ve duoc, ke o con tro.

Cach go:
 A. THOAT GAME roi vao lai: item dang cam se ve tui (hoac mat) - con tro
    duoc giai phong.
 B. Menu admin them muc "Dọn item pet lỗi trong túi": tru sach moi item
    thuoc dai pet 4874..4932 dang co trong hanh trang (dung
    ConsumeEquiproomItem - CHI dai pet, khong dung do khac).
"""
import io
import re
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CR = chr(13)
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\bdh_admin.lua"
lf = io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")

if "BDH_P_DonLoi" in lf:
    print("da co")
else:
    # them muc menu ngay truoc "Đồng bộ xuống client"
    neo = '\t"' + vn("Đồng bộ xuống client") + '/BDH_P_DongBo",\n'
    assert lf.count(neo) == 1, "khong thay muc dong bo"
    lf = lf.replace(neo, '\t"' + vn("Dọn item pet lỗi trong túi") + '/BDH_P_DonLoi",\n' + neo, 1)

    them = '''function BDH_P_DonLoi()
	local i
	local nDem = 0
	for i = 4874, 4932 do
		local nCo = CalcEquiproomItemCount(6, 1, i, -1)
		if nCo > 0 then
			ConsumeEquiproomItem(nCo, 6, 1, i)
			nDem = nDem + nCo
		end
	end
	Msg2Player(format("''' + vn("Đã dọn %d vật phẩm hệ Bạn Đồng Hành khỏi hành trang") + '''", nDem))
end

'''
    neo2 = "function BDH_P_DongBo()"
    assert lf.count(neo2) == 1
    lf = lf.replace(neo2, them + neo2, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("da them muc Don item pet loi")

kh = lf[lf.find("function BDH_P_Root()"):lf.find("\nend", lf.find("function BDH_P_Root()"))]
nhan = re.findall(r'"([^"]*)"', kh)
print("menu: %d muc, ~%d byte (tran 512)" % (len(nhan), sum(len(x) + 1 for x in nhan) + 55))
dinh = set(re.findall(r"function\s+([\w:.]+)\s*\(", lf))
goi = set(re.findall(r'/(BDH_[\w]+)"', lf))
print("ham thieu:", sorted(goi - dinh) or "KHONG")
