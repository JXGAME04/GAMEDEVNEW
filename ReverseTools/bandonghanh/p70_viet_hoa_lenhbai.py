# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Viet hoa CO DAU toan bo menu + thong bao lenh bai PET
(chu yeu cau). Dung vn_edit.vn() -> byte TCVN3 chuan; TRANH chu HOA co dau
(TCVN3 khong ma hoa duoc) - vi du dung "Cấp", "Điểm", "Đồng bộ" (chu hoa
dau tu la ky tu KHONG dau hoac Đ/Ă/Â/Ê/Ô/Ơ/Ư).
"""
import io
import re
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CR = chr(13)
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\bdh_admin.lua"
lf = io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")

# ---- menu goc ----
i = lf.find("function BDH_P_Root()")
j = lf.find("\nend", i)
moi = ('function BDH_P_Root()\n'
       '\tSayEx({format("' + vn("Pet cấp %d - điểm %d/%d/%d/%d - xu %d") + '",\n'
       '\t\tPET_GetLevel(), PET_GetUpgradePoint(), PET_GetGrownPoint(),\n'
       '\t\tPET_GetTamePoint(), PET_GetXiuzhenPoint(), GetTask(251)),\n'
       '\t"' + vn("Cấp thiệp + thuốc + trái") + '/BDH_P_CapDo",\n'
       '\t"' + vn("Cấp 5 Bí kiếp kỹ năng") + '/BDH_P_CapKN",\n'
       '\t"' + vn("Cấp bộ Bích Huyết (10 món)") + '/BDH_P_CapBo1",\n'
       '\t"' + vn("Cấp bộ Kim Lân (10 món)") + '/BDH_P_CapBo2",\n'
       '\t"' + vn("Cấp rương + chìa khóa + kết tinh") + '/BDH_P_CapRuong",\n'
       '\t"' + vn("Bốn điểm + chân nguyên + xu") + '/BDH_P_Diem",\n'
       '\t"' + vn("Tạo pet nhanh") + '/BDH_P_TaoNhanh",\n'
       '\t"' + vn("Xóa kỹ năng đã học") + '/BDH_P_XoaKN",\n'
       '\t"' + vn("Tháo hết trang bị") + '/BDH_P_XoaTB",\n'
       '\t"' + vn("Đồng bộ dữ liệu xuống client") + '/BDH_P_DongBo",\n'
       '\t"' + vn("Kết thúc đối thoại.") + '/no"})')
lf = lf[:i] + moi + lf[j:]

# ---- cac thong bao ----
THAY = [
    ("Da cap 5 Thiep + 200 Thuoc + 20 moi loai trai",
     vn("Đã cấp 5 Thiệp + 200 Thuốc tăng trưởng + 20 mỗi loại trái")),
    ("Da cap 5 Bi kiep ban dong hanh", vn("Đã cấp 5 Bí kiếp bạn đồng hành")),
    ("Da cap bo Bich Huyet 10 mon (4907..4916)", vn("Đã cấp bộ Bích Huyết - 10 món")),
    ("Da cap bo Kim Lan 10 mon (4917..4926)", vn("Đã cấp bộ Kim Lân - 10 món")),
    ("Da cap 5 ruong 1/2/3 + 5 chia khoa + 5 ket tinh",
     vn("Đã cấp 5 rương 1/2/3 + 5 chìa khóa + 5 kết tinh")),
    ("Da cap 4 diem 9999 + 2000 chan nguyen + 500 xu",
     vn("Đã cấp 4 điểm 9999 + 2000 chân nguyên + 500 xu")),
    ("Da xoa het ky nang pet", vn("Đã xóa hết kỹ năng của bạn đồng hành")),
    ("Da thao het trang bi pet (don ca o rac he cu)",
     vn("Đã tháo hết trang bị (dọn cả ô dữ liệu cũ)")),
    ("Da ban lai 29 o diem pet - mo cua so Ban Dong Hanh xem",
     vn("Đã gửi lại dữ liệu xuống client - mở cửa sổ Bạn Đồng Hành xem")),
    ("Da cap 2000 diem chan nguyen (task 362) - tong: ",
     vn("Đã cấp 2000 điểm chân nguyên - tổng: ")),
]
n = 0
for cu, mo in THAY:
    if cu in lf:
        lf = lf.replace(cu, mo)
        n += 1
io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))

kh = lf[lf.find("function BDH_P_Root()"):lf.find("\nend", lf.find("function BDH_P_Root()"))]
nhan = re.findall(r'"([^"]*)"', kh)
print("menu: %d muc, ~%d byte (tran 512) | thong bao doi: %d"
      % (len(nhan), sum(len(x) + 1 for x in nhan) + 50, n))
dinh = set(re.findall(r"function\s+([\w:.]+)\s*\(", lf))
goi = set(re.findall(r'/(BDH_[\w]+)"', lf))
print("ham thieu:", sorted(goi - dinh) or "KHONG")
