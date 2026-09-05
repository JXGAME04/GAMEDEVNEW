# -*- coding: utf-8 -*-
"""cfgw_vietngu - [CFGW 05/09] TU DIEN TIENG VIET CO DAU cho tung khoa cau hinh game.

Chu game 04/09 toi: "toi can chi tiet hon, phai giai thich ro hon bang tieng Viet co dau
va canh bao neu chinh sai".

Moi khoa = mot muc trong VN:
    "KHOA": ("Ten tieng Viet", "Giai thich chi tiet", "Canh bao neu chinh sai", nguy_co)
    hoac them phan tu thu 5 la dict ghi de metadata cua bo sinh:
        {"k": kieu, "dv": "don vi", "mn": min, "mx": max}
    nguy_co: 0 = an toan (sua thoai mai) | 1 = can than (anh huong can bang / hoat dong)
             2 = nguy hiem (kinh te toan server, co the lam hong hoat dong, can restart hay
                 khai them du lieu)

Tep nay va cac tep vn_*.py la UTF-8 (co dau day du). sinh_cfgw_meta.py doc VN roi doi sang
TCVN3 bang vn_to_octal.unicode_to_tcvn3_bytes truoc khi ghi cfgw_meta.lua. TCVN3 KHONG co
nguyen am hoa co dau (chi co A A D E O O U hoa co mu/moc) -> khong duoc viet "Ở", "Ứng",
"Ải"... o dau cau; bo sinh se BAO LOI ten khoa neu vi pham.

Trong giai thich, "\\n" tach doan - web hien thanh xuong dong.
"""
from . import vn_exp_tien, vn_rotdo, vn_hethong_bot, vn_tongkim_ctc, vn_hoatdong1, vn_hoatdong2, vn_khac

VN = {}
for _m in (vn_exp_tien, vn_rotdo, vn_hethong_bot, vn_tongkim_ctc, vn_hoatdong1, vn_hoatdong2, vn_khac):
    for _k, _v in _m.VN.items():
        if _k in VN:
            raise SystemExit("cfgw_vietngu: khoa %s khai hai lan (%s)" % (_k, _m.__name__))
        VN[_k] = _v

# Mo ta 22 nhom (ma nhom -> (ten, mo ta ngan)) - gui cho web hien dau moi nhom.
NHOM_VN = {
    "EXP":         ("Kinh nghiệm", "Mọi hệ số quyết định tốc độ lên cấp: hệ số nhân của máy chủ (C++), hệ số theo mốc cấp, VIP, chuyển sinh, phạt chênh cấp, chia exp tổ đội và trần exp bảo rương. Đây là nhóm ảnh hưởng mạnh nhất tới nhịp phát triển của cả server."),
    "EXP_KYNANG":  ("Exp kỹ năng", "Tốc độ luyện kỹ năng 90 và 120 khi đánh quái. Đặt 0 là tắt hẳn việc luyện."),
    "TIEN":        ("Tiền (vạn / lượng)", "Hệ số nhân tiền quái rơi và tiền thưởng sự kiện. Đổi ở đây là đổi lạm phát của cả server, hãy tăng từng bước nhỏ."),
    "ROTDO":       ("Rớt đồ", "Xác suất và số lượng vật phẩm rơi từ quái thường, boss xanh và các nhánh sự kiện (Hiệp Cốt Nhu Tình, Huy Hoàng Đơn, Tiền Đồng, trang bị HKMP...). Danh mục vật phẩm nằm ở settings\\droprate\\*.ini, không nằm ở đây."),
    "LICH":        ("Lịch hoạt động", "Thời lượng các pha của Tống Kim (báo danh, trận, Nguyên Soái, dọn NPC) và số người mỗi phe. Khung giờ trận Tống Kim vẫn nằm trong bảng TK_LICH của script."),
    "HOATDONG":    ("Hoạt động", "Công tắc bật/tắt từng hoạt động và tham số ba hoạt động bản Linux (Sát Thủ, Phong Lăng Độ, Vượt ải), hệ Bắc Đẩu lệnh bài, exp hai vật phẩm dùng chung."),
    "TONGKIM":     ("Tống Kim", "Cấp tối thiểu, phí, điểm tích luỹ, cờ, doanh trại và Nguyên Soái của chiến trường Tống Kim. Phần lớn chỉ đổi sau khi khởi động lại máy chủ vì được chốt lúc nạp script."),
    "CONGTHANH":   ("Công thành chiến", "Độ dài trận, số cổng, Long Trụ, số người mỗi phe, điểm công trạng và exp thưởng của Công Thành Chiến bản JX2."),
    "LOIDAI":      ("Lôi đài", "Lôi đài bang hội (đấu trường JX2): số người mỗi bang, thời gian chờ, độ dài trận, exp bang thắng."),
    "BANGCHIEN":   ("Bang chiến", "Võ Lâm Đệ Nhất Bang (tongwar): cấp tối thiểu, giờ khai chiến / kết thúc, độ dài trận, số người, số mạng, điểm giết và liên trảm."),
    "BACHNHAN":    ("Bách nhân lôi đài", "Bách Nhân Lôi Đài (map 960): giờ mở, exp mỗi nhịp, Lôi Chủ, NPC Cổ Thủ buff x2, nhịp đấu và NPC lối vào."),
    "TYVO":        ("Tỷ võ", "Tỷ Võ Đài (đăng ký qua Công Bình Tử): cấp tối thiểu, số người mỗi đội, phút chờ và tổng phút một trận."),
    "THANHBAO":    ("Thành bảo / Tín sứ", "Bang hội Thành Bảo (thủ cây Thần Mộc, map 984) và nhiệm vụ Tín Sứ Thiên Bảo Khố: cấp, lượt, giờ nhận điểm, giá đổi lệnh bài, bùa Thủ Vệ."),
    "VIEMDE":      ("Viêm Đế", "Viêm Đế Bảo Tàng (vượt ải tổ đội): công tắc, giờ mở báo danh, cấp, số lần, cỡ tổ đội, số phòng, thời gian báo danh và thời hạn trận."),
    "BANDONGHANH": ("Bạn đồng hành", "Hệ Bạn Đồng Hành (pet): số ô trang bị, Tu Chân, cho ăn, giá đổi tên / ngoại quan, cấp và kỹ năng bí kíp."),
    "DATAU":       ("Dã Tẩu", "Nhiệm vụ Dã Tẩu: số nhiệm vụ mỗi vòng, số vòng mỗi chu kỳ, số chu kỳ tối đa."),
    "BOSS":        ("Boss Hoàng Kim", "Boss Hoàng Kim (tiểu, đại, Phong Lăng Độ): thời gian tồn tại, máu, né tránh, hồi máu, exp giết và exp lân cận, bán kính chia exp."),
    "VANTIEU":     ("Vận tiêu", "Thưởng vận tiêu ba loại tiêu (đồng / bạc / vàng): exp, Hộ Mạch Đơn, Chân Nguyên Đơn, rương, Lệnh Bài Boss, điểm bang hội."),
    "TRONGBANG":   ("Trống bang hội", "Trống Khải Hoàn của bang: thời gian tồn tại, khung giờ được đánh, exp mỗi nhịp và trần exp mỗi người mỗi ngày."),
    "THUONG":      ("Thưởng hoạt động", "Các mức thưởng chưa xếp vào nhóm riêng."),
    "CHUNG":       ("Chung", "Hệ số sức mạnh boss / quái của các hoạt động bản Việt cũ, Nguyên Soái Tống Kim, và toàn bộ hệ BOT tự động (số bot, Dã Tẩu, bày sạp, Tống Kim)."),
    "HETHONG":     ("Hệ thống", "Mốc giờ mở server, chế độ thử nghiệm, nhịp nạp lại script và ghi log. Nhóm này ảnh hưởng toàn máy chủ - web luôn hỏi lại trước khi lưu."),
}

# Nhan muc nguy co (web hien)
NGUY_CO_VN = {0: "An toàn", 1: "Cẩn thận", 2: "Nguy hiểm"}
