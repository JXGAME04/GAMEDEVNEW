# -*- coding: utf-8 -*-
"""Nhom LICH (TK_*), HOATDONG cong tac (BAT_*), TONGKIM (TKT_*), CONGTHANH (CTC_*), LOIDAI (CTLD_*)."""

VN = {}

_RESTART = "Chốt khi nạp script nên cần khởi động lại máy chủ mới có hiệu lực."

# --- LICH: Tong Kim (lib_tktc.lua) ------------------------------------------------------------
VN["TK_PHUT_BAODANH"] = (
    "Tống Kim: số phút báo danh",
    "Độ dài pha báo danh trước mỗi trận Tống Kim (đồng hồ 1 của nhiệm vụ). Trong pha này người chơi và bot chỉ được đứng trong doanh trại, chưa được ra trận. "
    "Mặc định gốc 10, đang chạy 1 (chú thích trong mã vẫn ghi 10).\n" + _RESTART,
    "Đặt 0 là không có pha báo danh, người đến sớm ra trận trước người đến sau. Đặt quá dài (30) là người chơi chờ chán. Cần khởi động lại máy chủ.",
    1)
VN["TK_PHUT_TRAN"] = (
    "Tống Kim: số phút cả trận",
    "Độ dài một trận Tống Kim từ lúc bắt đầu tới lúc tính kết quả. Mặc định gốc 70, đang chạy 30. Phải khớp với khung giờ trong bảng TK_LICH của script (giờ kết thúc).\n" + _RESTART,
    "Trận dài hơn khoảng cách giữa hai khung giờ trong TK_LICH là trận sau mở đè trận trước. Đặt quá ngắn (5) thì Nguyên Soái chưa kịp ra đã hết trận. Cần khởi động lại máy chủ.",
    1)
VN["TK_PHUT_NGUYENSOAI"] = (
    "Tống Kim: phút Nguyên Soái xuất hiện",
    "Sau khi trận bắt đầu bao nhiêu phút thì Nguyên Soái hai phe xuất hiện. Mặc định gốc 30, đang chạy 1.\n" + _RESTART,
    "Phải nhỏ hơn TK_PHUT_TRAN, nếu không Nguyên Soái không bao giờ ra và không ai nhận điểm giết Nguyên Soái. Cần khởi động lại máy chủ.",
    1)
VN["TK_PHUT_XOANPC"] = (
    "Tống Kim: phút dọn NPC sau trận",
    "Sau khi trận kết thúc bao nhiêu phút thì xoá toàn bộ NPC của trận (Nguyên Soái, cờ, trap). Mặc định 1.\n" + _RESTART,
    "Đặt 0 có thể xoá NPC ngay lúc đang tổng kết. Đặt dài mà trận kế mở sớm là NPC cũ còn đứng trong trận mới. Cần khởi động lại máy chủ.",
    0)
VN["TK_NGUOI_MOI_PHE"] = (
    "Tống Kim: số người tối đa mỗi phe",
    "Trần số người (kể cả bot) mỗi phe trong một trận. Mặc định 2000.\n" + _RESTART,
    "Số này cao cùng với nhiều bot làm một bản đồ quá đông, máy chủ phát gói tới hàng trăm người mỗi hành động, gây 'không thấy địch'. Muốn hạn chế bot thì dùng BOT_TK_TRAN. Cần khởi động lại máy chủ.",
    1)

# --- HOATDONG: cong tac BAT_* -----------------------------------------------------------------
_BAT = {
    "BAT_TONGKIM": ("Tống Kim", "chiến trường Tống Kim theo lịch TK_LICH"),
    "BAT_HATHUYHOANG": ("Hạ Thuỷ Hoàng", "hoạt động Hạ Thuỷ Hoàng"),
    "BAT_BOSS_HOANGKIM": ("Boss Hoàng Kim", "lịch xuất hiện Boss Hoàng Kim (tiểu và đại)"),
    "BAT_HOADANG": ("Hoa Đăng / Trạng Nguyên", "sự kiện Hoa Đăng và Trạng Nguyên (câu hỏi)"),
    "BAT_TRONG_BANGHOI": ("Trống bang hội", "Trống Khải Hoàn của bang hội (xem nhóm Trống bang hội)"),
    "BAT_KIEMMONQUAN": ("Kiếm Môn Quan", "hoạt động Kiếm Môn Quan"),
    "BAT_VANTIEU_LOA": ("Loa Vận Tiêu", "loa thông báo của Vận Tiêu (NPC vận tiêu vẫn chạy dù tắt khoá này)"),
    "BAT_DUATOP": ("Đua Top", "sự kiện Đua Top"),
    "BAT_SONGBAC": ("Sòng Bạc", "hoạt động Sòng Bạc"),
    "BAT_PUBG": ("Sinh Tồn (PUBG)", "hoạt động Sinh Tồn"),
    "BAT_HD3": ("Cụm 3 hoạt động bản Linux", "cụm Sát Thủ / Phong Lăng Độ / Vượt ải bản Linux (nhóm HD3_*)"),
    "BAT_VIEMDE": ("Viêm Đế Bảo Tàng", "Viêm Đế Bảo Tàng (nhóm Viêm Đế; YDBZ_BAT cũng phải bật)"),
    "BAT_CTC_JX2": ("Công Thành + Lôi Đài bang hội (JX2)", "Công Thành Chiến và Lôi Đài bang hội bản JX2"),
}
for _k, (_ten, _mt) in _BAT.items():
    VN[_k] = (
        "Bật hoạt động: " + _ten,
        "1 = bật, 0 = tắt %s. timerserver.lua đọc công tắc này mỗi phút khi tới giờ trong lịch.\nCó hiệu lực trong vòng 1,5 phút, không cắt trận đang chạy." % _mt,
        "Hoạt động đã tắt lâu ngày có thể chưa được nạp thư viện: bật lên mà không thấy chạy thì xem logs\\hethong.log, sẽ có dòng '<khoá> bật nhưng hàm <tên> chưa nạp'. "
        "Tắt giữa chừng không dừng trận đang diễn ra, chỉ không mở trận kế.",
        1)
VN["BAT_CHECK_KICK"] = (
    "Bật tự kích tài khoản",
    "1 = bật cơ chế tự kiểm tra và kích (đá) tài khoản vi phạm khỏi máy chủ theo nhịp timerserver. 0 = tắt (đang tắt).\nCó hiệu lực trong vòng 1,5 phút.",
    "Bật mà luật kiểm tra chưa đúng là đá nhầm người chơi thật hàng loạt. Chỉ bật sau khi đã thử trên máy chủ thử nghiệm.",
    2)

# --- TONGKIM: TKT_* (lib_tktc.lua, chot luc nap) ----------------------------------------------
VN["TKT_GIA_DOI_NHACVUONGKIEM"] = (
    "Tống Kim: tiền đổi Nhạc Vương Kiếm",
    "Số tiền (lượng) phải trả khi đổi Nhạc Vương Kiếm tại NPC Tống Kim, cùng với TKT_SL_HONTHACH_DOI_KIEM Hồn Thạch. Mặc định 10.000.000 lượng.\n" + _RESTART,
    "Hạ giá là Nhạc Vương Kiếm rẻ đi cho toàn server. Cần khởi động lại máy chủ.",
    1)
VN["TKT_PHI_TRINHSAT"] = (
    "Tống Kim: phí mỗi lần trinh sát",
    "Số tiền (lượng) trừ mỗi lần dùng chức năng trinh sát (xem quân số hai phe). Mặc định 500.\n" + _RESTART,
    "Đặt 0 là trinh sát miễn phí, không hại gì nhiều. Cần khởi động lại máy chủ.",
    0)
VN["TKT_SL_HONTHACH_DOI_KIEM"] = (
    "Tống Kim: số Hồn Thạch đổi một Nhạc Vương Kiếm",
    "Số Nhạc Vương Hồn Thạch cần nộp để đổi một Nhạc Vương Kiếm. Mặc định 100.\n" + _RESTART,
    "Hạ thấp là kiếm tràn server. Cần khởi động lại máy chủ.",
    1, {"dv": "cai"})
VN["TKT_CAP_TOI_THIEU"] = (
    "Tống Kim: cấp tối thiểu báo danh",
    "Nhân vật phải đạt cấp này mới được báo danh Tống Kim. Mặc định 80.\n" + _RESTART,
    "Hạ thấp là người cấp thấp vào trận bị giết liên tục và có thể bị lợi dụng để farm điểm. Cần khởi động lại máy chủ.",
    1)
VN["TKT_LECH_QUANSO_TOIDA"] = (
    "Tống Kim: chênh lệch quân số tối đa hai phe",
    "Khi phe đông hơn phe kia quá số người này thì phe đông bị chặn báo danh cho tới khi cân lại. Mặc định 20.\n" + _RESTART,
    "Đặt 0 là hai phe phải đúng bằng nhau, nhiều người không báo danh được. Đặt rất lớn là một phe có thể đông áp đảo. Cần khởi động lại máy chủ.",
    1, {"dv": "nguoi"})
VN["TKT_PHI_BAO_DANH"] = (
    "Tống Kim: phí báo danh (chỉ hiển thị)",
    "Số tiền ghi trong lời thoại báo danh. Hiện chỉ hiện chữ, không trừ tiền vì mọi lệnh Pay trong script đã bị vô hiệu. Mặc định 20.000.\n" + _RESTART,
    "Đổi chỉ đổi con số hiển thị, không đổi tiền thật. Muốn thu phí thật phải sửa script. Cần khởi động lại máy chủ.",
    0)
for _k, _tt, _md in (("TKT_THUONG_TICHLUY_THANG", "phe thắng", "1200"), ("TKT_THUONG_TICHLUY_THUA", "phe thua", "300"), ("TKT_THUONG_TICHLUY_HOA", "khi hoà", "600")):
    VN[_k] = (
        "Tống Kim: điểm tích luỹ thưởng %s" % _tt,
        "Điểm tích luỹ cộng thêm cho mỗi người %s khi hết trận, nhân với cấp quân hàm (1 đến 6) của người đó. Mặc định %s.\n%s" % (_tt, _md, _RESTART),
        "Đây là nguồn điểm tích luỹ (đổi thưởng) chính của Tống Kim; tăng gấp đôi là thưởng cả server gấp đôi mỗi trận. Giữ thắng > hoà > thua. Cần khởi động lại máy chủ.",
        1, {"dv": "diem"})
VN["TKT_DIEM_TOITHIEU_NHAN_THUONG"] = (
    "Tống Kim: điểm tối thiểu để nhận thưởng",
    "Người có điểm trong trận dưới ngưỡng này thì hết trận không được nhận thưởng (chống vào trận đứng không). Mặc định 1000.\n" + _RESTART,
    "Đặt 0 là ai vào cũng có thưởng, bot và người treo máy nhận thưởng miễn phí. Đặt quá cao là người chơi bình thường không đạt. Cần khởi động lại máy chủ.",
    1)
VN["TKT_SO_CO_TOIDA"] = (
    "Tống Kim: số cờ tối đa mỗi phe cắm được",
    "Mỗi phe chỉ được cắm tối đa bấy nhiêu lá cờ trong một trận. Mặc định 30.\n" + _RESTART,
    "Đặt 0 là không cắm được cờ. Cần khởi động lại máy chủ.",
    0, {"dv": "la"})
VN["TKT_DIEM_CAM_CO"] = (
    "Tống Kim: điểm mỗi lần cắm cờ thành công",
    "Điểm cộng cho cá nhân và cho phe mỗi lần mang cờ về cắm thành công. Mặc định 300.\n" + _RESTART,
    "Tăng cao là cắm cờ thành cách farm điểm chính, lấn át giết người. Cần khởi động lại máy chủ.",
    1)
VN["TKT_MUC_GIAM_TOCDO_GIU_CO"] = (
    "Tống Kim: mức giảm tốc độ người vác cờ",
    "Người đang vác cờ bị giảm tốc độ chạy theo mức này (đơn vị tốc độ của engine). Mặc định 60.\n" + _RESTART,
    "Đặt 0 là vác cờ chạy như thường, cờ về quá dễ. Đặt quá cao là người vác cờ đứng yên. Cần khởi động lại máy chủ.",
    0, {"dv": ""})
VN["TKT_GIAY_TRONG_DOANHTRAI"] = (
    "Tống Kim: số giây tối đa trong doanh trại",
    "Người chơi đứng trong doanh trại (hậu doanh) quá số giây này sẽ bị đẩy ra ngoài. Mặc định 90.\n" + _RESTART,
    "Đặt quá ngắn là người vừa hồi sinh chưa kịp mua máu đã bị đẩy ra. Đặt 0 có thể làm cơ chế đẩy hoạt động lạ. Cần khởi động lại máy chủ.",
    1)
VN["TKT_GIAY_DELAY_RA_TRAI"] = (
    "Tống Kim: số giây bảo hộ khi ra khỏi trại",
    "Số giây còn được giữ trạng thái bảo hộ khi bước ra khỏi doanh trại. Mặc định 10.\n" + _RESTART,
    "Đặt quá dài là người ra trại bất tử lâu, lợi dụng đánh trước cửa trại. Cần khởi động lại máy chủ.",
    1)
VN["TKT_MAU_NGUYENSOAI"] = (
    "Tống Kim: máu Nguyên Soái",
    "Máu của Nguyên Soái hai phe khi xuất hiện, còn được nhân với GLB_MANH_NGUYENSOAI_TK. Mặc định 5.000.000.\n" + _RESTART,
    "Máu quá thấp là Nguyên Soái chết ngay khi ra, trận kết thúc sớm; quá cao là không ai hạ nổi. Đổi phải tính cả hệ số GLB_MANH_NGUYENSOAI_TK. Cần khởi động lại máy chủ.",
    1)
VN["TKT_DIEM_GIET_NGUYENSOAI"] = (
    "Tống Kim: điểm cho người hạ Nguyên Soái",
    "Điểm cộng cho người đánh đòn cuối hạ Nguyên Soái (người đó ăn trọn). Mặc định 5000.\n" + _RESTART,
    "Số này lớn hơn cả trận giết người nên rất dễ gây tranh chấp đòn cuối. Cần khởi động lại máy chủ.",
    1)

# --- CONGTHANH: CTC_* ----------------------------------------------------------------------------
VN["CTC_CHUKY_QUET_MO_TRAN_PHUT"] = (
    "Công thành: chu kỳ quét mở trận",
    "Cứ bao nhiêu phút script Công Thành quét lịch một lần để xem có tới giờ mở trận không. Mặc định 5.\n" + _RESTART,
    "Đặt lớn là trận mở trễ tới bấy nhiêu phút so với lịch. Cần khởi động lại máy chủ.",
    0)
VN["CTC_DIEM_PHA_LONGTRU"] = (
    "Công thành: điểm công trạng phá một Long Trụ",
    "Điểm công trạng cộng cho người phá được một Long Trụ. Mặc định 300.\n" + _RESTART,
    "Tăng cao là phá trụ lấn át mọi cách ghi điểm khác. Cần khởi động lại máy chủ.",
    1)
VN["CTC_EXP_TOP10_BANSAO"] = (
    "Công thành: exp thưởng top 10 (bản trong lib_ctc)",
    "Exp thưởng cho 10 người dẫn đầu công trạng, bản sao đọc tại lib_ctc.lua. Có hai khoá cùng nghĩa (CTC_EXP_TOP10 ở missions\\citywar_global\\head.lua) - hãy đặt cả hai bằng nhau. Mặc định 3.000.000.\n" + _RESTART,
    "Đặt lệch với CTC_EXP_TOP10 là hai chỗ phát exp khác nhau. Cần khởi động lại máy chủ.",
    1)
VN["CTC_EXP_TOP10"] = (
    "Công thành: exp thưởng top 10 công trạng",
    "Exp thưởng cho 10 người dẫn đầu bảng công trạng sau trận (đọc tại missions\\citywar_global\\head.lua). Có một bản sao CTC_EXP_TOP10_BANSAO ở lib_ctc.lua, nên đặt bằng nhau. Mặc định 3.000.000.\n" + _RESTART,
    "Đây là exp phát trực tiếp, tăng gấp 10 là bơm 30 triệu exp cho 10 người mỗi trận. Cần khởi động lại máy chủ.",
    1)
VN["CTC_GIA_LENHBAI"] = (
    "Công thành: giá mua lệnh bài đăng ký",
    "Số tiền (lượng) bang phải trả để mua lệnh bài đăng ký Công Thành. Mặc định 200.000.\n" + _RESTART,
    "Đặt 0 là bang nào cũng đăng ký được không tốn gì. Cần khởi động lại máy chủ.",
    1)
VN["CTC_GIA_TRA_LAI_LENHBAI"] = (
    "Công thành: tiền hoàn khi trả lại lệnh bài",
    "Số tiền (lượng) hoàn lại khi bang trả lệnh bài. Mặc định 10.000.\n" + _RESTART,
    "Đặt lớn hơn CTC_GIA_LENHBAI là mua rồi trả để lấy lời, nhân tiền vô hạn. Cần khởi động lại máy chủ.",
    2)
for _k, _tt in (("CTC_MAX_LIENMINH_CONG_VEBINH", "phe công (số NPC vệ binh dùng)"), ("CTC_MAX_LIENMINH_THU_VEBINH", "phe thủ (số NPC vệ binh dùng)")):
    VN[_k] = (
        "Công thành: số người liên minh %s" % _tt.split(" (")[0],
        "Số người ngoài bang được trợ giúp %s, bản do NPC vệ binh đọc (lib_ctc.lua). Bản 'cửa trận' là CTC_MAX_LIENMINH_CONG/THU. Mặc định 5.\n%s" % (_tt, _RESTART),
        "Hai bản (vệ binh và cửa trận) đang khác nhau (5 và 50); đổi nên đổi cả hai cho khớp. Cần khởi động lại máy chủ.",
        1, {"dv": "nguoi"})
for _k, _tt in (("CTC_MAX_NGUOI_PHE_CONG_VEBINH", "phe công"), ("CTC_MAX_NGUOI_PHE_THU_VEBINH", "phe thủ")):
    VN[_k] = (
        "Công thành: số người tối đa %s (NPC vệ binh dùng)" % _tt,
        "Trần số người %s mà NPC vệ binh kiểm khi cho vào. Bản cửa trận là CTC_MAX_NGUOI_PHE_CONG/THU (200). Mặc định 50.\n%s" % (_tt, _RESTART),
        "Đang lệch với bản cửa trận (50 và 200): người thứ 51 qua cửa được nhưng vệ binh chặn. Nên đặt hai bản bằng nhau. Cần khởi động lại máy chủ.",
        1)
VN["CTC_CHUKY_LOA_GIAY"] = (
    "Công thành: chu kỳ loa tình hình",
    "Trong trận, cứ bao nhiêu giây loa thông báo tình hình (trụ, cổng, điểm) một lần. Mặc định 60.\n" + _RESTART,
    "Đặt nhỏ (5) là loa dồn dập che khung chat. Cần khởi động lại máy chủ.",
    0)
VN["CTC_DODAI_TRAN_PHUT"] = (
    "Công thành: độ dài một trận",
    "Số phút một trận Công Thành Chiến kéo dài trước khi tính kết quả. Mặc định 90.\n" + _RESTART,
    "Đặt ngắn là phe công không kịp phá đủ cổng và trụ, phe thủ luôn thắng. Cần khởi động lại máy chủ.",
    1)
VN["CTC_SO_LONGTRU"] = (
    "Công thành: số Long Trụ trong trận",
    "Số Long Trụ sinh ra trong trận; phá đủ toàn bộ là phe công thắng ngay. Mặc định 3.\n" + _RESTART,
    "Số trụ phải có đủ toạ độ đặt trong bảng của script; đặt nhiều hơn số toạ độ là trụ không sinh được. Cần khởi động lại máy chủ.",
    2, {"dv": "cai"})
VN["CTC_SO_CONG_THANH"] = (
    "Công thành: số cổng thành phải phá",
    "Số cổng thành phe công phải phá. phải khớp số dòng trong bảng DoorPos của script. Mặc định 3.\n" + _RESTART,
    "Đặt khác số dòng bảng DoorPos là script lỗi khi sinh cổng hoặc trận không thể kết thúc. Đừng đổi nếu chưa sửa bảng DoorPos. Cần khởi động lại máy chủ.",
    2, {"dv": "cai"})
for _k, _tt in (("CTC_MAX_NGUOI_PHE_THU", "phe thủ"), ("CTC_MAX_NGUOI_PHE_CONG", "phe công")):
    VN[_k] = (
        "Công thành: số người tối đa %s (cửa trận)" % _tt,
        "Trần số người %s được vào trận, bản do cửa trận kiểm. Mặc định 200. Bản NPC vệ binh (CTC_MAX_NGUOI_PHE_*_VEBINH) đang là 50.\n%s" % (_tt, _RESTART),
        "Quá đông trong một bản đồ là tải gói tin cao, dễ 'không thấy địch'. Nên khớp với bản vệ binh. Cần khởi động lại máy chủ.",
        1)
for _k, _tt in (("CTC_MAX_LIENMINH_THU", "phe thủ"), ("CTC_MAX_LIENMINH_CONG", "phe công")):
    VN[_k] = (
        "Công thành: số người liên minh %s (cửa trận)" % _tt,
        "Số người ngoài bang được vào trợ giúp %s, bản do cửa trận kiểm. Mặc định 50. Bản NPC vệ binh đang là 5.\n%s" % (_tt, _RESTART),
        "Nên khớp với bản vệ binh, nếu không người liên minh qua cửa rồi bị vệ binh chặn. Cần khởi động lại máy chủ.",
        1, {"dv": "nguoi"})
VN["CTC_CAP_LONGTRU"] = (
    "Công thành: cấp NPC Long Trụ",
    "Cấp của NPC Long Trụ khi sinh ra, quyết định máu của trụ. Mặc định 10.\n" + _RESTART,
    "Tăng cấp là trụ trâu hơn nhiều, phe công khó thắng. Cần khởi động lại máy chủ.",
    1)
VN["CTC_CAP_CONG_THANH"] = (
    "Công thành: cấp NPC cổng thành",
    "Cấp của NPC cổng thành, quyết định cổng khó phá tới đâu. Mặc định 60.\n" + _RESTART,
    "Cấp quá cao là không phá nổi cổng trong 90 phút. Cần khởi động lại máy chủ.",
    1)
VN["CTC_DIEM_GIET_NGUOI"] = (
    "Công thành: điểm công trạng mỗi lần hạ gục đối phương",
    "Điểm công trạng cơ bản mỗi lần hạ gục người phe kia. Mặc định 75. Có hiệu lực trong vòng 1,5 phút.",
    "Tăng cao là giết người lấn át phá trụ/cổng.",
    1)
VN["CTC_DIEM_LIEN_TRAM"] = (
    "Công thành: điểm thưởng mỗi mốc liên trảm",
    "Điểm thưởng thêm mỗi lần đạt mốc liên trảm (giết liên tiếp không chết). Mặc định 150. Có hiệu lực trong vòng 1,5 phút.",
    "Tăng cao khuyến khích cao thủ farm người yếu.",
    1)

# --- LOIDAI: CTLD_* --------------------------------------------------------------------------
VN["CTLD_MAX_NGUOI_MOI_PHE"] = (
    "Lôi đài: số người tối đa mỗi bang",
    "Mỗi bang được đưa tối đa bấy nhiêu người vào đấu trường Lôi Đài. Mặc định 16.\n" + _RESTART,
    "Đặt lớn là đấu trường quá đông cho bản đồ nhỏ. Cần khởi động lại máy chủ.",
    1)
VN["CTLD_CHUKY_LOA_GIAY"] = (
    "Lôi đài: chu kỳ loa",
    "Cứ bao nhiêu giây loa tình hình trận Lôi Đài một lần. Mặc định 20.\n" + _RESTART,
    "Đặt nhỏ là loa dồn dập. Cần khởi động lại máy chủ.",
    0)
VN["CTLD_DODAI_TRAN_PHUT"] = (
    "Lôi đài: độ dài một trận",
    "Số phút một trận Lôi Đài kéo dài (chú thích trong mã ghi nhầm là một tiếng). Mặc định 25.\n" + _RESTART,
    "Đặt ngắn là trận kết thúc hoà nhiều. Cần khởi động lại máy chủ.",
    1)
VN["CTLD_GIAY_CHO_VAO_DAUTRUONG"] = (
    "Lôi đài: thời gian chờ hai bên vào đấu trường",
    "Số giây chờ hai bang vào đấu trường trước khi bắt đầu (chú thích trong mã ghi nhầm là 10 phút). Mặc định 480 = 8 phút.\n" + _RESTART,
    "Đặt quá ngắn là bang vào chậm bị xử thua. Cần khởi động lại máy chủ.",
    1)
VN["CTLD_EXP_BANG_THANG"] = (
    "Lôi đài: exp bang hội cho bên thắng",
    "Kinh nghiệm bang hội (không phải exp nhân vật) cộng cho bang thắng một trận Lôi Đài. Mặc định 1200.\n" + _RESTART,
    "Tăng cao là bang mạnh lên cấp bang rất nhanh. Cần khởi động lại máy chủ.",
    1)
