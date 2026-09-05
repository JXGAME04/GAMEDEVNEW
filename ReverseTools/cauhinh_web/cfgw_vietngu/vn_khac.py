# -*- coding: utf-8 -*-
"""Nhom DATAU (DT_*), BANDONGHANH (BDH_*), BOSS (BHK_*), TRONGBANG (TBH_*), VANTIEU (VT_*)."""

VN = {}

_RESTART = "Chốt khi nạp script, cần khởi động lại máy chủ."
_LIVE = "Có hiệu lực trong vòng 1,5 phút."

# --- DA TAU -------------------------------------------------------------------------------------
VN["DT_SO_NV_MOI_VONG"] = (
    "Dã Tẩu: số nhiệm vụ mỗi vòng",
    "Phải hoàn thành bấy nhiêu nhiệm vụ Dã Tẩu mới qua một vòng mới (thưởng vòng). Mặc định 20.\n" + _RESTART,
    "Hạ thấp là thưởng vòng phát dày hơn cho cả người lẫn bot Dã Tẩu. Cần khởi động lại máy chủ.",
    1, {"dv": "nhiem vu"})
VN["DT_SO_VONG_MOI_CHU_KY"] = (
    "Dã Tẩu: số vòng mỗi chu kỳ",
    "Một chu kỳ Dã Tẩu gồm bấy nhiêu vòng. Mặc định 20.\n" + _RESTART,
    "Đổi giữa chừng làm bộ đếm vòng của người đang làm dở lệch chu kỳ. Cần khởi động lại máy chủ.",
    1, {"dv": "vong"})
VN["DT_SO_CHU_KY_TOI_DA"] = (
    "Dã Tẩu: số chu kỳ tối đa trước khi đếm về 0",
    "Đếm chu kỳ tới số này rồi quay về 0. Mặc định 20.\n" + _RESTART,
    "Không ảnh hưởng nhiều; đặt 0 có thể làm phép chia dư lỗi. Cần khởi động lại máy chủ.",
    0, {"dv": "chu ky"})

# --- BAN DONG HANH ------------------------------------------------------------------------------
VN["BDH_SO_O_TRANGBI"] = (
    "Bạn đồng hành: số ô trang bị",
    "Số ô trang bị của Bạn Đồng Hành. Mặc định 10.\n" + _LIVE,
    "Giảm khi người chơi đã mặc đủ ô là trang bị ở ô bị bỏ có thể mất hoặc không tháo được. Không giảm khi server đang có người chơi.",
    2)
VN["BDH_CHANNGUYEN_DOI_1_TUCHAN"] = (
    "Bạn đồng hành: số chân nguyên đổi 1 điểm Tu Chân",
    "Bỏ ra bấy nhiêu điểm chân nguyên để đổi lấy 1 điểm Tu Chân. Mặc định 200.\n" + _RESTART,
    "Hạ thấp là Tu Chân rẻ đi, kỹ năng bí kíp lên tối đa nhanh. Cần khởi động lại máy chủ.",
    1, {"dv": "diem"})
VN["BDH_TUCHAN_TOI_DA"] = (
    "Bạn đồng hành: trần điểm Tu Chân",
    "Số điểm Tu Chân tối đa một Bạn Đồng Hành tích được. Mặc định 20.000.\n" + _RESTART,
    "Đặt thấp hơn số điểm người chơi đang có là điểm dư bị kẹt. Cần khởi động lại máy chủ.",
    1, {"dv": "diem"})
VN["BDH_SO_LAN_CHO_AN_MOI_NGAY"] = (
    "Bạn đồng hành: số lần cho ăn mỗi loại quả mỗi ngày",
    "Mỗi ngày được cho ăn tối đa bấy nhiêu lần mỗi loại quả (đếm riêng từng loại). Mặc định 4.\n" + _RESTART,
    "Tăng là Bạn Đồng Hành lên cấp nhanh hơn tương ứng. Cần khởi động lại máy chủ.",
    1)
VN["BDH_GIA_DOI_NGOAI_QUAN"] = (
    "Bạn đồng hành: giá đổi ngoại quan",
    "Số Xu phải trả để đổi ngoại quan Bạn Đồng Hành. Mặc định 5.\n" + _RESTART,
    "Đặt 0 là miễn phí. Cần khởi động lại máy chủ.",
    0, {"dv": "xu"})
VN["BDH_GIA_DOI_TEN"] = (
    "Bạn đồng hành: giá đổi tên",
    "Số Xu phải trả để đổi tên Bạn Đồng Hành. Mặc định 5.\n" + _RESTART,
    "Đặt 0 là miễn phí. Cần khởi động lại máy chủ.",
    0, {"dv": "xu"})
VN["BDH_CAP_TOI_DA"] = (
    "Bạn đồng hành: cấp tối đa",
    "Cấp tối đa của Bạn Đồng Hành. Mặc định 130. Tên biến này rất chung, chỉ đổi trong tệp petsys\\common.lua.\n" + _RESTART,
    "Tăng cấp tối đa mà bảng exp theo cấp không có dòng tương ứng là lỗi khi lên cấp. Cần khởi động lại máy chủ.",
    2)
VN["BDH_BUOC_CAP_NGOAI_QUAN"] = (
    "Bạn đồng hành: mỗi bao nhiêu cấp mở thêm nhóm ngoại quan",
    "Cứ mỗi bấy nhiêu cấp thì mở thêm một nhóm ngoại quan. Mặc định 10.\n" + _RESTART,
    "Đặt 0 làm phép chia lỗi. Cần khởi động lại máy chủ.",
    0, {"mn": 1, "mx": 200})
VN["BDH_CAP_NHANVAT_TOI_THIEU"] = (
    "Bạn đồng hành: cấp nhân vật tối thiểu để tạo",
    "Nhân vật phải đạt cấp này mới tạo được Bạn Đồng Hành. Mặc định 150.\n" + _RESTART,
    "Hạ thấp là tân thủ có pet sớm, đổi cân bằng đầu game. Cần khởi động lại máy chủ.",
    1)
VN["BDH_CAP_MO_KYNANG_BIKIP"] = (
    "Bạn đồng hành: cấp bắt đầu học kỹ năng bí kíp",
    "Bạn Đồng Hành phải đạt cấp này mới học được kỹ năng từ Bí kíp. Mặc định 21.\n" + _RESTART,
    "Cần khởi động lại máy chủ.",
    0)
VN["BDH_SO_O_KYNANG_TOI_DA"] = (
    "Bạn đồng hành: số ô kỹ năng bị động tối đa",
    "Số ô kỹ năng bị động tối đa. Mặc định 4.\n" + _RESTART,
    "Giảm khi pet đã học đủ ô là kỹ năng ở ô bị bỏ mất tác dụng. Cần khởi động lại máy chủ.",
    1)
VN["BDH_BUOC_CAP_MO_O_KYNANG"] = (
    "Bạn đồng hành: mỗi bao nhiêu cấp mở thêm ô kỹ năng",
    "Cứ mỗi bấy nhiêu cấp mở thêm một ô kỹ năng, tới trần BDH_SO_O_KYNANG_TOI_DA. Mặc định 5.\n" + _RESTART,
    "Đặt 0 làm phép chia lỗi. Cần khởi động lại máy chủ.",
    0, {"mn": 1, "mx": 200})
VN["BDH_KYNANG_BIKIP_CAP_TOI_DA"] = (
    "Bạn đồng hành: cấp tối đa mỗi kỹ năng bí kíp",
    "Mỗi kỹ năng bí kíp nâng tối đa tới cấp này (bằng điểm Tu Chân). Mặc định 30.\n" + _RESTART,
    "Tăng là kỹ năng pet mạnh hơn trần cũ. Cần khởi động lại máy chủ.",
    1)
VN["BDH_TUCHAN_MOI_CAP_KYNANG"] = (
    "Bạn đồng hành: điểm Tu Chân mỗi cấp kỹ năng",
    "Nâng kỹ năng từ cấp N lên N+1 tốn N nhân số này điểm Tu Chân. Mặc định 10.\n" + _RESTART,
    "Hạ thấp là kỹ năng lên tối đa rất nhanh. Cần khởi động lại máy chủ.",
    1, {"dv": "diem"})
VN["BDH_THOIHAN_DANHHIEU_NGAY"] = (
    "Bạn đồng hành: số ngày danh hiệu tồn tại",
    "Danh hiệu Bạn Đồng Hành tồn tại bấy nhiêu ngày. Mặc định 30. Khai lại y hệt ở partner_reward2.lua.\n" + _RESTART,
    "Cần khởi động lại máy chủ.",
    0)

# --- BOSS HOANG KIM -----------------------------------------------------------------------------
VN["BHK_PHUT_BOSS_TON_TAI"] = (
    "Boss: số phút tồn tại nếu không ai giết",
    "Boss Hoàng Kim sống bấy nhiêu phút rồi tự biến mất nếu không ai giết. Mặc định 120.\n" + _RESTART,
    "Đặt ngắn là boss biến mất khi đang đánh dở. Cần khởi động lại máy chủ.",
    1)
VN["BHK_MAU_BOSS_TIEU"] = (
    "Boss: máu Boss Tiểu Hoàng Kim",
    "Máu của Boss Tiểu Hoàng Kim (ghi chú trong mã: mặc định cũ 8.000.000). Đang 18.000.000.\n" + _RESTART,
    "Máu quá thấp là boss chết một đòn; quá cao là không ai hạ nổi trong 120 phút. Cần khởi động lại máy chủ.",
    1)
VN["BHK_MAU_BOSS_DAI"] = (
    "Boss: máu Boss Đại Hoàng Kim (đang 1!)",
    "Máu của boss đại, boss máy chủ và boss Phong Lăng Độ. hiện đang là 1 (chết một đòn) trong khi ghi chú nói mặc định 18.000.000 - rất có thể là giá trị thử nghiệm bỏ quên.\n" + _RESTART,
    "Đang 1 máu: boss đại chết ngay đòn đầu, ai chạm trước ăn 25 triệu exp. Nếu không cố ý thì đặt lại 18.000.000. Cần khởi động lại máy chủ.",
    2)
VN["BHK_NETRANH_BOSS"] = (
    "Boss: chỉ số né tránh",
    "Chỉ số né tránh của boss; càng cao người chơi càng hay đánh trượt. Mặc định 5000.\n" + _RESTART,
    "Quá cao là boss không thể đánh trúng. Cần khởi động lại máy chủ.",
    1, {"dv": ""})
VN["BHK_HOIMAU_BOSS"] = (
    "Boss: lượng máu tự hồi mỗi nhịp",
    "Boss tự hồi bấy nhiêu máu mỗi nhịp. Mặc định 300.\n" + _RESTART,
    "Đặt lớn hơn sát thương của cả nhóm là boss bất tử. Cần khởi động lại máy chủ.",
    1)
VN["BHK_EXP_NPC_BOSS_DAI"] = (
    "Boss: exp gắn thẳng vào NPC boss đại",
    "Exp gắn thẳng vào con NPC boss đại (đang 0; exp thưởng thật phát ở tệp death*). Mặc định 0.\n" + _RESTART,
    "Đặt khác 0 là cộng thêm exp ngoài phần thưởng death*, dễ thành hai lần thưởng. Cần khởi động lại máy chủ.",
    1)
VN["BHK_EXP_NPC_BOSS_TIEU"] = (
    "Boss: exp gắn thẳng vào NPC boss tiểu",
    "Exp gắn thẳng vào con NPC boss tiểu (đang 0; exp thưởng thật phát ở tệp death*). Mặc định 0.\n" + _RESTART,
    "Đặt khác 0 dễ thành hai lần thưởng. Cần khởi động lại máy chủ.",
    1)
for _lo, _ten, _giet, _lan in (("PLD", "Boss Hoàng Kim Phong Lăng Độ", "50.000.000", "20.000.000"),
                                ("DAI", "Boss Đại Hoàng Kim", "25.000.000", "10.000.000"),
                                ("TIEU", "Boss Tiểu Hoàng Kim", "20.000.000", "10.000.000")):
    VN["BHK_EXP_GIET_BOSS_%s" % _lo] = (
        "Boss: exp cho người hạ %s" % _ten,
        "Exp cho người hạ %s và tổ đội của họ. Mặc định %s.\n%s" % (_ten, _giet, _RESTART),
        "Exp phát thẳng, nhân với số boss mỗi ngày; thêm một số 0 là bơm hàng trăm triệu exp. Cần khởi động lại máy chủ.",
        2)
    VN["BHK_EXP_LANCAN_BOSS_%s" % _lo] = (
        "Boss: exp cho người đứng gần khi hạ %s" % _ten,
        "Exp cho người đứng trong bán kính chia exp quanh xác %s nhưng khác tổ đội với người hạ. Mặc định %s.\n%s" % (_ten, _lan, _RESTART),
        "Đặt cao là đứng xem boss cũng có exp lớn, bot đứng gần boss hưởng ké. Cần khởi động lại máy chủ.",
        2, {"dv": "exp"})
    VN["BHK_PHAMVI_HUONG_EXP_%s" % _lo] = (
        "Boss: bán kính chia exp quanh xác %s" % _ten,
        "Bán kính (ô) quanh xác %s mà người chơi phải đứng trong đó mới được chia exp lân cận. Mặc định 200.\n%s" % (_ten, _RESTART),
        "Đặt rất lớn là cả bản đồ hưởng exp lân cận. Cần khởi động lại máy chủ.",
        1, {"dv": "o"})

# --- TRONG BANG HOI -----------------------------------------------------------------------------
VN["TBH_PHUT_TRONG_TON_TAI"] = (
    "Trống bang: số phút trống tồn tại",
    "Trống Khải Hoàn do bang chủ đánh ra tồn tại bấy nhiêu phút. Mặc định 30.\n" + _RESTART,
    "Đặt dài là trống đứng chiếm chỗ lâu. Cần khởi động lại máy chủ.",
    0)
VN["TBH_GIO_MO"] = (
    "Trống bang: giờ sớm nhất được đánh trống",
    "Giờ sớm nhất trong ngày được dùng Trống Khải Hoàn, dạng HH:MM giờ máy chủ. Mặc định 19:00.\n" + _RESTART,
    "Đặt sau giờ đóng là không bao giờ đánh được. Cần khởi động lại máy chủ.",
    1)
VN["TBH_GIO_DONG"] = (
    "Trống bang: giờ muộn nhất được đánh trống",
    "Giờ muộn nhất trong ngày được dùng Trống Khải Hoàn, dạng HH:MM giờ máy chủ. Mặc định 23:59.\n" + _RESTART,
    "Phải sau giờ mở. Cần khởi động lại máy chủ.",
    1)
VN["TBH_EXP_TRAN_NGAY"] = (
    "Trống bang: trần exp mỗi người mỗi ngày",
    "Mỗi người nhận tối đa bấy nhiêu exp mỗi ngày từ đánh trống bang. Mặc định 200.000.000.\n" + _RESTART,
    "Đây là trần kinh tế; bỏ trần là exp trống bang không giới hạn. Cần khởi động lại máy chủ.",
    2)
VN["TBH_EXP_MIN"] = (
    "Trống bang: exp thấp nhất mỗi nhịp",
    "Exp thấp nhất cộng cho mỗi nhịp (một lần đánh có 10 nhịp). Mặc định 1.000.000.\n" + _RESTART,
    "Nhân 10 nhịp mỗi lần đánh; phải nhỏ hơn hoặc bằng TBH_EXP_MAX. Cần khởi động lại máy chủ.",
    2)
VN["TBH_EXP_MAX"] = (
    "Trống bang: exp cao nhất mỗi nhịp",
    "Exp cao nhất cộng cho mỗi nhịp (một lần đánh có 10 nhịp). Mặc định 2.000.000.\n" + _RESTART,
    "Nhân 10 nhịp mỗi lần đánh; phải lớn hơn hoặc bằng TBH_EXP_MIN. Cần khởi động lại máy chủ.",
    2)

# --- VAN TIEU -----------------------------------------------------------------------------------
VN["VT_EXP_NEN_LAN"] = (
    "Vận tiêu: số lần cộng exp nền",
    "Exp nền được cộng thành nhiều lần: số lần nhân với VT_EXP_NEN_MOI. Mặc định 1000 (1000 x 50.000 = 50 triệu).\n" + _LIVE,
    "Nhân với exp mỗi lần; tăng là tăng exp nền mọi chuyến tiêu.",
    1)
VN["VT_EXP_NEN_MOI"] = (
    "Vận tiêu: exp mỗi lần cộng nền",
    "Exp mỗi lần cộng nền, nhân với VT_EXP_NEN_LAN. Mặc định 50.000.\n" + _LIVE,
    "Nhân với 1000 lần; thêm một số 0 là 500 triệu mỗi chuyến.",
    2)
VN["VT_EXP_NEN_SUM"] = (
    "Vận tiêu: exp nền cộng thêm",
    "Exp nền cộng thêm một lần cho mỗi chuyến (thông báo trong game chỉ ghi 50.000.000). Mặc định 500.000.000.\n" + _LIVE,
    "Số này đã gấp 10 lần thông báo hiển thị; tăng nữa là lệch xa lời hứa với người chơi.",
    2)
for _lo, _ten, _exp, _tb in (("DONG", "tiêu đồng", "100.000.000", "10.000.000"), ("BAC", "tiêu bạc", "300.000.000", "30.000.000"), ("VANG", "tiêu vàng", "500.000.000", "50.000.000")):
    VN["VT_EXP_%s" % _lo] = (
        "Vận tiêu: exp cộng thêm khi giao %s" % _ten,
        "Exp cộng thêm khi giao %s thành công (thông báo trong game ghi %s). Mặc định %s.\n%s" % (_ten, _tb, _exp, _LIVE),
        "Đang gấp 10 lần số thông báo; tăng nữa là bơm exp lớn mỗi chuyến.",
        2)
    VN["VT_HOMACH_%s" % _lo] = (
        "Vận tiêu: số Hộ Mạch Đơn khi giao %s" % _ten,
        "Số Hộ Mạch Đơn thưởng khi giao %s. Mặc định %s.\n%s" % (_ten, {"DONG": "300", "BAC": "400", "VANG": "500"}[_lo], _LIVE),
        "Tăng là bơm Hộ Mạch Đơn vào server mỗi chuyến.",
        1, {"dv": "cai"})
    VN["VT_CHANNGUYEN_%s" % _lo] = (
        "Vận tiêu: số Chân Nguyên Đơn khi giao %s" % _ten,
        "Số Chân Nguyên Đơn thưởng khi giao %s. Mặc định %s.\n%s" % (_ten, {"DONG": "50", "BAC": "70", "VANG": "100"}[_lo], _LIVE),
        "Tăng là bơm Chân Nguyên Đơn vào server mỗi chuyến.",
        1, {"dv": "cai"})
    VN["VT_RUONG_%s" % _lo] = (
        "Vận tiêu: số rương trang bị xanh khi giao %s" % _ten,
        "Số rương trang bị xanh thưởng khi giao %s. Mặc định %s.\n%s" % (_ten, {"DONG": "2", "BAC": "5", "VANG": "10"}[_lo], _LIVE),
        "Tăng là trang bị xanh tràn chợ.",
        1, {"dv": "cai"})
    VN["VT_DIEM_TONG_%s" % _lo] = (
        "Vận tiêu: điểm bang hội khi giao %s" % _ten,
        "Điểm cộng cho bang hội khi giao %s. Mặc định %s.\n%s" % (_ten, {"DONG": "100", "BAC": "200", "VANG": "300"}[_lo], _LIVE),
        "Tăng là bang có nhiều người vận tiêu lên cấp bang rất nhanh.",
        1)
VN["VT_LENHBAI_BOSS"] = (
    "Vận tiêu: số Lệnh Bài Boss mỗi chuyến",
    "Số Lệnh Bài Boss thưởng mỗi chuyến (mọi loại tiêu). Mặc định 10.\n" + _LIVE,
    "Tăng là lệnh bài boss tràn server.",
    1, {"dv": "cai"})
