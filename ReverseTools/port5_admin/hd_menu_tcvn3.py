# -*- coding: utf-8 -*-
r"""[24/08] Doi menu Lenh Bai Admin sang tieng Viet co dau (TCVN3) theo yeu cau chu game.
Chi doi CHUOI hien thi; logic giu nguyen. Tu kiem mapping bang chuoi that trong du an.
"""
import io, shutil

M = {
 u"á":0xB8,u"à":0xB5,u"ả":0xB6,u"ã":0xB7,u"ạ":0xB9,
 u"ă":0xA8,u"ắ":0xBE,u"ằ":0xBB,u"ẳ":0xBC,u"ẵ":0xBD,u"ặ":0xC6,
 u"â":0xA9,u"ấ":0xCA,u"ầ":0xC7,u"ẩ":0xC8,u"ẫ":0xC9,u"ậ":0xCB,
 u"đ":0xAE,u"Đ":0xAE,
 u"é":0xD0,u"è":0xCC,u"ẻ":0xCE,u"ẽ":0xCF,u"ẹ":0xD1,
 u"ê":0xAA,u"ế":0xD5,u"ề":0xD2,u"ể":0xD3,u"ễ":0xD4,u"ệ":0xD6,
 u"í":0xDD,u"ì":0xD7,u"ỉ":0xD8,u"ĩ":0xDC,u"ị":0xDE,
 u"ó":0xE3,u"ò":0xDF,u"ỏ":0xE1,u"õ":0xE2,u"ọ":0xE4,
 u"ô":0xAB,u"ố":0xE8,u"ồ":0xE5,u"ổ":0xE6,u"ỗ":0xE7,u"ộ":0xE9,
 u"ơ":0xAC,u"ớ":0xED,u"ờ":0xEA,u"ở":0xEB,u"ỡ":0xEC,u"ợ":0xEE,
 u"ú":0xF3,u"ù":0xEF,u"ủ":0xF1,u"ũ":0xF2,u"ụ":0xF4,
 u"ư":0xAD,u"ứ":0xF8,u"ừ":0xF5,u"ử":0xF6,u"ữ":0xF7,u"ự":0xF9,
 u"ý":0xFD,u"ỳ":0xFA,u"ỷ":0xFB,u"ỹ":0xFC,u"ỵ":0xFE,
 u"ấ":0xCA,
}
# chu HOA co dau -> dung byte chu thuong (nhu file goc: 'Lien Dau' viet la 'Li^en %Dau' voi 0xAE)
_UP = {}
for _k, _v in list(M.items()):
    _KU = _k.upper()
    if _KU != _k and _KU not in M:
        _UP[_KU] = _v
M.update(_UP)

def tcvn3(u):
    out = []
    for ch in u:
        o = ord(ch)
        if o < 128:
            out.append(chr(o))
        else:
            assert ch in M, u"thieu map: %r" % ch
            out.append(chr(M[ch]))
    return "".join(out)

# ---- TU KIEM: "Phải đạt cấp" phai ra chuoi dang co trong tongwar_signup.lua ----
chk = tcvn3(u"Phải đạt cấp ")
s0 = io.open(r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\event\tongwar\tongwar_signup.lua",
             "r", encoding="latin-1", newline="").read()
assert chk in s0, "mapping TCVN3 lech voi du an!"
chk2 = tcvn3(u"trở lên")
assert chk2 in s0, "mapping tro len lech!"
print("mapping TCVN3 khop du an OK")

V = tcvn3  # viet tat

p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\hoatdong_admin.lua"
s = io.open(p, "r", encoding="latin-1", newline="").read()

REP = [
 # ---- menu chinh ----
 (u'"<color=yellow>Hoat dong 23-24/08<color>: chon muc test (cau hinh: script\\\\header\\\\cauhinh_hoatdong.lua)"',
  u'"<color=yellow>Hoạt động 23-24/08<color>: chọn mục test (cấu hình: script\\\\header\\\\cauhinh_hoatdong.lua)"'),
 (u'"1. Bang Chien - Vo Lam De Nhat Bang/HD_TW_Menu"', u'"1. Bang Chiến - Võ Lâm Đệ Nhất Bang/HD_TW_Menu"'),
 (u'"2. Bach Nhan Loi Dai/HD_BR_Menu"', u'"2. Bách Nhân Lôi Đài/HD_BR_Menu"'),
 (u'"3. Ty Vo Dai/HD_BW_Menu"', u'"3. Tỷ Võ Đài/HD_BW_Menu"'),
 (u'"4. Bang Hoi Thanh Bao/HD_TC_Menu"', u'"4. Bang Hội Thành Bảo/HD_TC_Menu"'),
 (u'"5. Nhan item thuong (test)/HD_Item_Menu"', u'"5. Nhận item thưởng (test)/HD_Item_Menu"'),
 (u'"6. BOT Tong Kim: BAT/HD_TK_Bat"', u'"6. BOT Tống Kim: bật/HD_TK_Bat"'),
 (u'"7. BOT Tong Kim: TAT/HD_TK_Tat"', u'"7. BOT Tống Kim: tắt/HD_TK_Tat"'),
 (u'"8. NAP LAI CONFIG (khi khong co tran chay)/HD_ReloadCfg"', u'"8. Nạp lại CONFIG (khi không có trận chạy)/HD_ReloadCfg"'),
 # ket thuc chung (2 cho dung chung chuoi nay)
 (u'"Ket thuc doi thoai/no"', u'"Kết thúc đối thoại/no"'),
 # ---- TW ----
 (u'"<color=yellow>Bang Chien<color> - mua 9: 29/10-05/11/2026, tran "',
  u'"<color=yellow>Bang Chiến<color> - mùa 9: 29/10-05/11/2026, trận "'),
 (u'" (can >=2 bang chiem thanh de khoi tao)"', u'" (cần >=2 bang chiếm thành để khởi tạo)"'),
 (u'"Xem trang thai (co tran? file mua?)/HD_TW_TrangThai"', u'"Xem trạng thái (cờ trận? file mùa?)/HD_TW_TrangThai"'),
 (u'"B1. Khoi tao mua 9 NGAY (tongwar_initmatch)/HD_TW_InitMatch"', u'"B1. Khởi tạo mùa 9 NGAY (tongwar_initmatch)/HD_TW_InitMatch"'),
 (u'"B2. Ep tick lich NGAY (TaskContent)/HD_TW_Tick"', u'"B2. Ép tick lịch NGAY (TaskContent)/HD_TW_Tick"'),
 (u'"B3. Ep mo pha 2 - bao danh 3 cap test/HD_TW_Redo"', u'"B3. Ép mở pha 2 - báo danh 3 cặp test/HD_TW_Redo"'),
 (u'"Dich chuyen: NPC bao danh Ba Lang Huyen/HD_TW_Tele53"', u'"Dịch chuyển: NPC báo danh Ba Lăng Huyện/HD_TW_Tele53"'),
 (u'"Dich chuyen: map bao danh 608/HD_TW_Tele608"', u'"Dịch chuyển: map báo danh 608/HD_TW_Tele608"'),
 (u'"Dich chuyen: map bao danh 610/HD_TW_Tele610"', u'"Dịch chuyển: map báo danh 610/HD_TW_Tele610"'),
 (u'"Dich chuyen: map bao danh 612/HD_TW_Tele612"', u'"Dịch chuyển: map báo danh 612/HD_TW_Tele612"'),
 (u'"Quay lai/HD_AdminMenu"', u'"Quay lại/HD_AdminMenu"'),
 (u'"Co van dang mo tran (GlbValue 850) = "', u'"Cờ đang mở trận (GlbValue 850) = "'),
 (u'" (1 = dang co tran)"', u'" (1 = đang có trận)"'),
 (u'" dong (7 dong G = da khoi tao mua)"', u'" dòng (7 dòng G = đã khởi tạo mùa)"'),
 (u'"CHUA co jx2league.txt (mua chua khoi tao - bam B1)"', u'"CHƯA có jx2league.txt (mùa chưa khởi tạo - bấm B1)"'),
 (u'"Da goi tongwar_initmatch(9) - xem jx2league.txt (can >=2 bang dang chiem thanh)"',
  u'"Đã gọi tongwar_initmatch(9) - xem jx2league.txt (cần >=2 bang đang chiếm thành)"'),
 (u'"Da ep TaskContent (tick lich Bang Chien)"', u'"Đã ép TaskContent (tick lịch Bang Chiến)"'),
 (u'"Da ep pha 2 (bao danh) cho 3 cap test - id bang sua o TW_TEST_REDO trong config"',
  u'"Đã ép pha 2 (báo danh) cho 3 cặp test - id bang sửa ở TW_TEST_REDO trong config"'),
 # ---- BR ----
 (u'"<color=yellow>Bach Nhan Loi Dai<color> - mo "', u'"<color=yellow>Bách Nhân Lôi Đài<color> - mở "'),
 (u'"-24:00 hang ngay, map 960, cap >="', u'"-24:00 hằng ngày, map 960, cấp >="'),
 (u'"Xem cau hinh dang chay/HD_BR_TrangThai"', u'"Xem cấu hình đang chạy/HD_BR_TrangThai"'),
 (u'"Dich chuyen: NPC loi vao (Lam An)/HD_BR_TeleNpc"', u'"Dịch chuyển: NPC lối vào (Lâm An)/HD_BR_TeleNpc"'),
 (u'"Dich chuyen: vao map 960 (dai 1)/HD_BR_Tele960"', u'"Dịch chuyển: vào map 960 (đài 1)/HD_BR_Tele960"'),
 (u'"Goi lai Init (CHI khi boot loi)/HD_BR_Init"', u'"Gọi lại Init (CHỈ khi boot lỗi)/HD_BR_Init"'),
 (u'"Gio mo %s | exp %d/%d phut (Loi Chu +%d) | tran %d luot/ngay | buff x2: %d%% nguoi, moi %d phut"',
  u'"Giờ mở %s | exp %d/%d phút (Lôi Chủ +%d) | trần %d lượt/ngày | buff x2: %d%% người, mỗi %d phút"'),
 (u'"Da dung hom nay (task 2709): %d luot"', u'"Đã dùng hôm nay (task 2709): %d lượt"'),
 (u'"Da goi BairenLeitai_Adm_Init (CHU Y: goi lap se tao them timer)"',
  u'"Đã gọi BairenLeitai_Adm_Init (CHÚ Ý: gọi lặp sẽ tạo thêm timer)"'),
 # ---- BW ----
 (u'"<color=yellow>Ty Vo Dai<color> - khong co lich, 2 doi truong (moi doi du 2 nguoi) bao danh o Cong Binh Tu; tran "',
  u'"<color=yellow>Tỷ Võ Đài<color> - không có lịch, 2 đội trưởng (mỗi đội đủ 2 người) báo danh ở Công Bình Tử; trận "'),
 (u'" phut (cho "', u'" phút (chờ "'),
 (u'" phut)"', u'" phút)"'),
 (u'"Dich chuyen: Cong Binh Tu DUONG CHAU/HD_BW_Tele80"', u'"Dịch chuyển: Công Bình Tử DƯƠNG CHÂU/HD_BW_Tele80"'),
 (u'"Dich chuyen: Cong Binh Tu TUONG DUONG/HD_BW_Tele78"', u'"Dịch chuyển: Công Bình Tử TƯƠNG DƯƠNG/HD_BW_Tele78"'),
 (u'"Dich chuyen: Cong Binh Tu THANH DO/HD_BW_Tele11"', u'"Dịch chuyển: Công Bình Tử THÀNH ĐÔ/HD_BW_Tele11"'),
 (u'"Vao dau truong 209 (cho khan gia)/HD_BW_Tele209"', u'"Vào đấu trường 209 (chỗ khán giả)/HD_BW_Tele209"'),
 (u'"Go ket tran treo cua TOI (task 200/2340-2342)/HD_BW_GoKet"', u'"Gỡ kẹt trận treo của TÔI (task 200/2340-2342)/HD_BW_GoKet"'),
 (u'"Da xoa co trong-tran (TaskTemp 200) + vi tri luu (task 2340-2342) cua nhan vat nay"',
  u'"Đã xóa cờ trong-trận (TaskTemp 200) + vị trí lưu (task 2340-2342) của nhân vật này"'),
 # ---- TC ----
 (u'"<color=yellow>Bang Hoi Thanh Bao<color> - thu "', u'"<color=yellow>Bang Hội Thành Bảo<color> - thứ "'),
 (u'" (0=CN), map 984 (CLIENT CHUA CO DATA - vao se den man)"',
  u'" (0=CN), map 984 (CLIENT CHƯA CÓ DATA - vào sẽ đen màn)"'),
 (u'"Hoi sinh cay 1 (Thanh Dong) NGAY/HD_TC_Cay1"', u'"Hồi sinh cây 1 (Thanh Đồng) NGAY/HD_TC_Cay1"'),
 (u'"Hoi sinh cay 2 (Bach Ngan) NGAY/HD_TC_Cay2"', u'"Hồi sinh cây 2 (Bạch Ngân) NGAY/HD_TC_Cay2"'),
 (u'"Hoi sinh cay 3 (Hoang Kim) NGAY/HD_TC_Cay3"', u'"Hồi sinh cây 3 (Hoàng Kim) NGAY/HD_TC_Cay3"'),
 (u'"Xoa het cay + record (don test)/HD_TC_XoaCay"', u'"Xóa hết cây + record (dọn test)/HD_TC_XoaCay"'),
 (u'"Xem diem Than Moc cua toi/HD_TC_XemDiem"', u'"Xem điểm Thần Mộc của tôi/HD_TC_XemDiem"'),
 (u'"Cong 500 diem Than Moc (test doi lenh bai)/HD_TC_CongDiem"', u'"Cộng 500 điểm Thần Mộc (test đổi lệnh bài)/HD_TC_CongDiem"'),
 (u'"Dich chuyen: NPC Thanh Bao (Lam An)/HD_TC_TeleNpc"', u'"Dịch chuyển: NPC Thành Bảo (Lâm An)/HD_TC_TeleNpc"'),
 (u'"Vao map 984 (CANH BAO: client den man)/HD_TC_Tele984"', u'"Vào map 984 (CẢNH BÁO: client đen màn)/HD_TC_Tele984"'),
 (u'"Da ep hoi sinh cay loai 1 o map 984"', u'"Đã ép hồi sinh cây loại 1 ở map 984"'),
 (u'"Da ep hoi sinh cay loai 2 o map 984"', u'"Đã ép hồi sinh cây loại 2 ở map 984"'),
 (u'"Da ep hoi sinh cay loai 3 o map 984"', u'"Đã ép hồi sinh cây loại 3 ở map 984"'),
 (u'"Da xoa record cay ca 3 loai o relay (cay dang dung trong map se bi don o tick sau)"',
  u'"Đã xóa record cây cả 3 loại ở relay (cây đang đứng trong map sẽ bị dọn ở tick sau)"'),
 (u'"Diem Than Moc (task 3399) = "', u'"Điểm Thần Mộc (task 3399) = "'),
 (u'"Da cong 500 diem Than Moc -> hien co "', u'"Đã cộng 500 điểm Thần Mộc -> hiện có "'),
 # ---- ITEM ----
 (u'"Nhan item thuong de test (genre 6/detail 1)"', u'"Nhận item thưởng để test (genre 6/detail 1)"'),
 (u'"Qua Dai Hoang Kim 4864 ("', u'"Quả Đại Hoàng Kim 4864 ("'),
 (u'"tr exp) x1/HD_It4864"', u'"tr exp) x1/HD_It4864"'),
 (u'"Hoang Chan Don 2273 ("', u'"Hoàng Chân Đơn 2273 ("'),
 (u'"tr exp co tran) x1/HD_It2273"', u'"tr exp có trần) x1/HD_It2273"'),
 (u'"Bua trieu Thu Ve 3204 x5/HD_It3204"', u'"Bùa triệu Thủ Vệ 3204 x5/HD_It3204"'),
 (u'"Thanh Dong Than Moc Lenh 3205 x1/HD_It3205"', u'"Thanh Đồng Thần Mộc Lệnh 3205 x1/HD_It3205"'),
 (u'"Bach Ngan Than Moc Lenh 3206 x1/HD_It3206"', u'"Bạch Ngân Thần Mộc Lệnh 3206 x1/HD_It3206"'),
 (u'"Hoang Kim Than Moc Lenh 3207 x1/HD_It3207"', u'"Hoàng Kim Thần Mộc Lệnh 3207 x1/HD_It3207"'),
 (u'"Han nguyen chan dan 4857 x1/HD_It4857"', u'"Hàn nguyên chân đan 4857 x1/HD_It4857"'),
 (u'"Da nhan Qua Dai Hoang Kim"', u'"Đã nhận Quả Đại Hoàng Kim"'),
 (u'"Da nhan Hoang Chan Don"', u'"Đã nhận Hoàng Chân Đơn"'),
 (u'"Da nhan 5 Bua trieu Thu Ve"', u'"Đã nhận 5 Bùa triệu Thủ Vệ"'),
 (u'"Da nhan Thanh Dong Than Moc Lenh"', u'"Đã nhận Thanh Đồng Thần Mộc Lệnh"'),
 (u'"Da nhan Bach Ngan Than Moc Lenh"', u'"Đã nhận Bạch Ngân Thần Mộc Lệnh"'),
 (u'"Da nhan Hoang Kim Than Moc Lenh"', u'"Đã nhận Hoàng Kim Thần Mộc Lệnh"'),
 (u'"Da nhan Han nguyen chan dan"', u'"Đã nhận Hàn nguyên chân đan"'),
 # ---- TK + reload ----
 (u'"Da BAT bot tu tham gia Tong Kim (PB_SetTongKim 1)"', u'"Đã BẬT bot tự tham gia Tống Kim (PB_SetTongKim 1)"'),
 (u'"Da TAT bot tu tham gia Tong Kim (PB_SetTongKim 0)"', u'"Đã TẮT bot tự tham gia Tống Kim (PB_SetTongKim 0)"'),
 (u'"Da nap lai CONFIG + "', u'"Đã nạp lại CONFIG + "'),
 (u'" file tinh nang. Cac muc [RESTART] trong config van can restart GS."',
  u'" file tính năng. Các mục [RESTART] trong config vẫn cần restart GS."'),
]

n_ok = 0
for old_u, new_u in REP:
    old = old_u.encode("latin-1", "strict").decode("latin-1") if max(ord(c) for c in old_u) < 128 else old_u
    old = str(old_u)  # ASCII
    new = V(new_u)
    c = s.count(old)
    assert c >= 1, "khong thay: %r (%d)" % (old_u[:50], c)
    s = s.replace(old, new)
    n_ok += 1
print("da thay", n_ok, "chuoi")

io.open(p, "w", encoding="latin-1", newline="").write(s)
shutil.copyfile(p, r"D:\GAMEDEVNEW\serverscript_jx2\port5_admin\hoatdong_admin.lua")

# ---- dong menu trong lenhbaiadmin.lua ----
p2 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\lenhbaiadmin.lua"
s2 = io.open(p2, "r", encoding="latin-1", newline="").read()
old = '"Hoat dong 23-24/08 (BC-BN-TV-TB): bo test/HD_AdminMenu",'
if old in s2:
    s2 = s2.replace(old, V(u'"Hoạt động 23-24/08 (BC-BN-TV-TB): bộ test/HD_AdminMenu",'), 1)
    io.open(p2, "w", encoding="latin-1", newline="").write(s2)
    print("lenhbaiadmin: dong menu da co dau")
else:
    print("lenhbaiadmin: da doi truoc do / khong thay anchor")
shutil.copyfile(p2, r"D:\GAMEDEVNEW\serverscript_jx2\port5_admin\lenhbaiadmin.lua")
print("XONG")
