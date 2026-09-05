# -*- coding: utf-8 -*-
"""Nhom BANGCHIEN (TW_*), THANHBAO (TS_* tin su + TC_* thanh bao), BACHNHAN (BR_*), TYVO (BW_*)."""

VN = {}

_LIVE = "Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy)."
_RESTART = "Chốt khi nạp script, cần khởi động lại máy chủ."

# --- BANG CHIEN (tongwar) ----------------------------------------------------------------------
VN["TW_CAP_TOITHIEU"] = (
    "Bang chiến: cấp tối thiểu",
    "Cấp tối thiểu để báo danh và vào đấu trường Bang Chiến (Võ Lâm Đệ Nhất Bang). Mặc định 90.\n" + _LIVE,
    "Hạ thấp là người cấp thấp vào trận làm mồi. Không đặt cao hơn cấp phổ biến của server.",
    1)
VN["TW_GIO_KHAICHIEN"] = (
    "Bang chiến: giờ khai chiến",
    "Giờ khai chiến trong ngày thi đấu, dạng HH:MM giờ máy chủ. Driver tick 15 phút một lần với cửa sổ khớp cộng trừ 5 phút quanh mốc. Mặc định 20:00.\n" + _LIVE,
    "Đổi giờ mà quên đổi TW_GIO_KETTHUC và TW_PHUT_TRAN là trận kết thúc sai lúc. Lời thoại NPC (head.lua:492) vẫn ghi giờ cũ, chỉ là chữ.",
    1)
VN["TW_GIO_KETTHUC"] = (
    "Bang chiến: giờ đóng trận",
    "Giờ đóng trận trong ngày thi đấu, dạng HH:MM giờ máy chủ. Mặc định 21:30. Nên bằng giờ khai chiến cộng TW_PHUT_TRAN.\n" + _LIVE,
    "Đặt trước giờ khai chiến là trận vừa mở đã đóng.",
    1)
VN["TW_PHUT_TRAN"] = (
    "Bang chiến: độ dài trận",
    "Số phút một trận Bang Chiến. Nên bằng (TW_GIO_KETTHUC trừ TW_GIO_KHAICHIEN). Mặc định 90.\n" + _RESTART,
    "Lệch với hai mốc giờ là trận bị cắt ngang hoặc kéo quá giờ đóng. Cần khởi động lại máy chủ.",
    1)
VN["TW_PHUT_CHUANBI"] = (
    "Bang chiến: phút chuẩn bị trước khai chiến",
    "Số phút mở cửa cho vào chuẩn bị trước giờ khai chiến. Mặc định 30.\n" + _RESTART,
    "Đặt 0 là không có thời gian vào sân. Cần khởi động lại máy chủ.",
    1)
VN["TW_NGUOI_TOIDA"] = (
    "Bang chiến: số người tối đa mỗi phe",
    "Trần số người mỗi phe được vào trận. Mặc định 150.\n" + _RESTART,
    "Quá đông trong bản đồ 605-607 là tải gói tin cao. Cần khởi động lại máy chủ.",
    1)
VN["TW_NGUOI_TOITHIEU"] = (
    "Bang chiến: số người tối thiểu mỗi phe",
    "Phe có ít hơn số người này khi vào trận bị xử thua. Mặc định 5.\n" + _RESTART,
    "Đặt cao là bang nhỏ luôn bị xử thua. Cần khởi động lại máy chủ.",
    1)
VN["TW_GIAY_HAUDOANH"] = (
    "Bang chiến: số giây tối đa ở hậu doanh",
    "Đứng trong hậu doanh quá số giây này thì bị đẩy ra ngoài. Mặc định 120.\n" + _RESTART,
    "Quá ngắn là chưa kịp hồi máu đã bị đẩy. Cần khởi động lại máy chủ.",
    1)
VN["TW_SO_MANG"] = (
    "Bang chiến: số mạng tối đa một trận",
    "Chết đủ số mạng này là bị loại khỏi trận. Mặc định 10.\n" + _LIVE,
    "Đặt 1 là chết một lần bị loại, trận kết thúc rất nhanh. Đặt rất lớn là không ai bị loại.",
    1)
VN["TW_DIEM_KILL"] = (
    "Bang chiến: điểm mỗi lần giết địch",
    "Điểm cộng khi giết một địch, nhân với hệ số quân hàm RANK_PKBONUS (missions\\tongwar\\head.lua:44). Mặc định 75.\n" + _RESTART,
    "Tăng cao là giết người lấn át chiến thuật. Cần khởi động lại máy chủ.",
    1)
VN["TW_DIEM_LIENTRAM"] = (
    "Bang chiến: điểm thưởng liên trảm",
    "Điểm thưởng mỗi 3 mạng giết liên tiếp không chết. Mặc định 150.\n" + _RESTART,
    "Tăng cao khuyến khích cao thủ farm. Cần khởi động lại máy chủ.",
    1)

# --- TIN SU (Thien Bao Kho) -------------------------------------------------------------------
VN["TS_CAP_TOITHIEU"] = (
    "Tín sứ: cấp tối thiểu nhận nhiệm vụ",
    "Cấp tối thiểu để nhận nhiệm vụ Tín Sứ Thiên Bảo Khố tại Dịch Quan Ba Lăng Huyện (bản đồ 11) hoặc Đại Lý (162). Gốc Linux 120, chủ game đã hạ 90.\n" + _RESTART,
    "Hạ thấp là người yếu vào ải chết liên tục. Cần khởi động lại máy chủ.",
    1)
VN["TS_LUOT_THUONG_NGAY"] = (
    "Tín sứ: số lượt thường mỗi ngày",
    "Số lượt nhận nhiệm vụ Tín Sứ miễn phí mỗi ngày (đếm ở task 4128). Mặc định 2.\n" + _RESTART,
    "Tăng là tăng exp và điểm Tín Sứ toàn server mỗi ngày. Cần khởi động lại máy chủ.",
    1, {"dv": "lan"})
VN["TS_LUOT_ITEM_NGAY"] = (
    "Tín sứ: số lượt mua thêm bằng Thiên Khố Bảo Lệnh",
    "Số lượt nhận thêm mỗi ngày bằng vật phẩm Thiên Khố Bảo Lệnh (6,1,3431). Mặc định 1.\n" + _RESTART,
    "Tăng là người có nhiều lệnh làm được nhiều lượt hơn hẳn. Cần khởi động lại máy chủ.",
    1, {"dv": "lan"})
VN["TS_TRA_HANHHIEP"] = (
    "Tín sứ: số Hành Hiệp Lệnh khi trả nhiệm vụ lần đầu trong ngày",
    "Lần trả nhiệm vụ đầu tiên trong ngày tại trạm dịch nhận bấy nhiêu Hành Hiệp Lệnh (6,1,2575). Mặc định 3.\n" + _RESTART,
    "Tăng là bơm Hành Hiệp Lệnh vào server. Cần khởi động lại máy chủ.",
    1, {"dv": "cai"})
VN["TS_TRA_BAORUONG"] = (
    "Tín sứ: số Tín Sứ Bảo Rương mỗi lần trả nhiệm vụ",
    "mọi lần trả nhiệm vụ nhận bấy nhiêu Tín Sứ Bảo Rương (6,1,3430). Mặc định 2.\n" + _RESTART,
    "Rương có exp và vật phẩm, tăng là tăng thưởng toàn server. Cần khởi động lại máy chủ.",
    1, {"dv": "cai"})

# --- BACH NHAN LOI DAI -----------------------------------------------------------------------
VN["BR_CAP_TOITHIEU"] = (
    "Bách nhân: cấp tối thiểu",
    "Cấp tối thiểu để vào Bách Nhân Lôi Đài (bản đồ 960, vào từ NPC ở Lâm An). Mặc định 90.\n" + _LIVE,
    "Hạ thấp là người yếu vào bị NPC cao thủ hạ ngay.",
    1)
VN["BR_GIO_MO"] = (
    "Bách nhân: giờ mở trong ngày",
    "Giờ mở cửa trong ngày, dạng HH:MM giờ máy chủ. Mặc định 12:00.\n" + _LIVE,
    "Giờ mở sau giờ đóng là không bao giờ mở.",
    1)
VN["BR_GIO_DONG_H"] = (
    "Bách nhân: giờ đóng (giờ chẵn 0 đến 23)",
    "Giờ đóng cửa trong ngày, chỉ ghi giờ (0 đến 23), 0 = 0 giờ đêm, tới giờ này mọi người được trả về. Mặc định 0.\n" + _LIVE,
    "Chỉ nhận số giờ 0..23, không phải HHMM: gõ 2300 là sai. Đặt trùng giờ mở là đóng ngay khi mở.",
    1, {"k": 0, "dv": "gio", "mn": 0, "mx": 23})
VN["BR_PHUT_CHUKY_EXP"] = (
    "Bách nhân: chu kỳ phát exp",
    "Cứ bao nhiêu phút phát một nhịp exp cho người đang trong bản đồ. Mặc định 5.\n" + _LIVE,
    "Đặt 1 là exp phát gấp 5 lần. Đặt 0 có thể làm nhịp chạy mỗi tick.",
    1)
VN["BR_EXP_TICK"] = (
    "Bách nhân: exp mỗi nhịp cho người trong bản đồ",
    "Exp cộng mỗi nhịp cho mỗi người đang trong bản đồ (có buff Cổ Thủ thì gấp đôi). Mặc định 1.000.000.\n" + _LIVE,
    "Đây là exp treo máy: người đứng trong bản đồ cũng nhận. Nhân với 50 nhịp mỗi ngày (BR_TRAN_LUOT_NGAY) là 50 triệu exp mỗi người mỗi ngày.",
    2)
VN["BR_EXP_LOICHU"] = (
    "Bách nhân: exp cộng thêm cho Lôi Chủ",
    "Exp cộng thêm mỗi nhịp cho người đang giữ đài (Lôi Chủ). Mặc định 2.000.000.\n" + _LIVE,
    "Giữ đài lâu là exp rất lớn, nhân với số nhịp mỗi ngày.",
    2)
VN["BR_TRAN_LUOT_NGAY"] = (
    "Bách nhân: trần số nhịp exp mỗi người mỗi ngày",
    "Mỗi người chỉ được nhận tối đa bấy nhiêu nhịp exp mỗi ngày (task daily 2709). Mặc định 50.\n" + _LIVE,
    "Đặt 0 có thể thành không giới hạn tuỳ script; đặt cao là exp treo máy không trần.",
    2, {"dv": "lan"})
VN["BR_PHUT_COTHU"] = (
    "Bách nhân: chu kỳ NPC Cổ Thủ xuất hiện",
    "Cứ bao nhiêu phút NPC Cổ Thủ (phát buff exp x2) xuất hiện một lần. Mặc định 30.\n" + _LIVE,
    "Đặt nhỏ là buff x2 gần như liên tục.",
    1)
VN["BR_TILE_BUFFX2"] = (
    "Bách nhân: tỉ lệ người được buff x2 mỗi đợt",
    "Tỉ lệ số người trong bản đồ được Cổ Thủ buff mỗi đợt, ghi dạng số thập phân 0 đến 1 (0.2 = 20%). Mặc định 0.2.\n" + _LIVE,
    "Ghi 20 thay vì 0.2 là 2000%, ai cũng được buff. Chỉ nhận 0..1.",
    1, {"dv": "", "mn": 0, "mx": 1})
VN["BR_GIAY_CHO_KHIEUCHIEN"] = (
    "Bách nhân: số giây chờ người khiêu chiến",
    "Hết số giây này không ai khiêu chiến thì gọi NPC cao thủ lên đài. Mặc định 30.\n" + _RESTART,
    "Đặt 0 là NPC lên đài ngay, người chơi không kịp khiêu chiến. Cần khởi động lại máy chủ.",
    0)
VN["BR_GIAY_DEM_NGUOC"] = (
    "Bách nhân: số giây đếm ngược trước khi đánh",
    "Đếm ngược bấy nhiêu giây trước khi lượt đấu bắt đầu. Mặc định 3.\n" + _RESTART,
    "Đặt quá dài là nhịp đấu chậm. Cần khởi động lại máy chủ.",
    0)
VN["BR_PHUT_MOI_LUOT"] = (
    "Bách nhân: số phút mỗi lượt đấu",
    "Hết số phút này chưa ai chết thì so sát thương: Lôi Chủ thắng nếu chịu ít sát thương hơn. Mặc định 3.\n" + _RESTART,
    "Đặt quá ngắn là lượt đấu luôn phân định bằng so sát thương. Cần khởi động lại máy chủ.",
    0)
VN["BR_PHUT_DUNG_YEN"] = (
    "Bách nhân: số phút đứng yên tối đa",
    "Người đứng yên trong bản đồ quá số phút này bị đá ra. Mặc định 90.\n" + _RESTART,
    "Đặt quá ngắn là người đang xem bị đá; quá dài là treo máy ăn exp thoải mái. Cần khởi động lại máy chủ.",
    1)
VN["BR_PHUT_BUFF_X2"] = (
    "Bách nhân: số phút hiệu lực buff x2",
    "Buff exp x2 của Cổ Thủ kéo dài bấy nhiêu phút. Mặc định 30.\n" + _RESTART,
    "Đặt bằng hoặc dài hơn chu kỳ Cổ Thủ là buff liên tục. Cần khởi động lại máy chủ.",
    1)
VN["BR_TRAN_CHUOI"] = (
    "Bách nhân: chuỗi thắng tối đa của một người",
    "Chuỗi thắng đạt tới số này thì loa 'truyền thuyết' và đặt lại chuỗi. Mỗi 10 trận thắng ở đài 1 loa toàn server. Mặc định 100.\n" + _LIVE,
    "Đặt nhỏ là loa liên tục.",
    0)
VN["BR_CAP_NPC_GOC"] = (
    "Bách nhân: cấp gốc NPC cao thủ",
    "Cấp gốc của NPC cao thủ; cấp thật = gốc + số lượt thắng chia 10, trần gốc + 9. Mặc định 90.\n" + _LIVE,
    "Đặt cao là NPC quá mạnh, không ai giữ đài nổi.",
    1)
VN["BR_NPC_MAP"] = (
    "Bách nhân: bản đồ đặt NPC lối vào",
    "Mã bản đồ đặt NPC lối vào Bách Nhân Lôi Đài (đặt lúc máy chủ khởi động). Mặc định 176 (Lâm An).\n" + _RESTART,
    "Sai mã bản đồ là NPC lối vào không xuất hiện, không ai vào được. Cần khởi động lại máy chủ.",
    2)
VN["BR_NPC_ID"] = (
    "Bách nhân: mẫu NPC lối vào",
    "Mã mẫu (template) trong npcs.txt của NPC lối vào. Mặc định 1747.\n" + _RESTART,
    "Sai mẫu là sinh NPC khác hoặc không sinh được. Cần khởi động lại máy chủ.",
    2, {"dv": ""})
VN["BR_NPC_LEVEL"] = (
    "Bách nhân: cấp NPC lối vào",
    "Cấp của NPC lối vào khi sinh. Mặc định 95.\n" + _RESTART,
    "Chỉ ảnh hưởng hiển thị / máu NPC lối vào. Cần khởi động lại máy chủ.",
    0, {"dv": "cap"})

# --- TY VO DAI --------------------------------------------------------------------------------
VN["BW_CAP_TOITHIEU"] = (
    "Tỷ võ: cấp tối thiểu",
    "Cấp tối thiểu để đăng ký Tỷ Võ Đài qua NPC Công Bình Tử (khán giả không bị chặn). Mặc định 90.\n" + _LIVE,
    "Hạ thấp không hại nhiều vì là đấu tự nguyện.",
    0)
VN["BW_DOI_TOIDA"] = (
    "Tỷ võ: số người tối đa mỗi đội",
    "Số người tối đa mỗi đội. Menu NPC hiện '1 vs 1' tới '8 vs 8'; đặt quá 8 thì menu vẫn chỉ hiện 8 lựa chọn (sửa thêm chữ ở bwmanager.lua:55). Mặc định 8.\n" + _LIVE,
    "Đặt trên 8 không có tác dụng trên menu. Đặt 0 là không lập được đội.",
    0, {"dv": "nguoi", "mn": 1, "mx": 8})
VN["BW_PHUT_CHO"] = (
    "Tỷ võ: phút chờ trước khi đánh",
    "Số phút chờ hai đội vào đài trước khi đánh. Mặc định 2.\n" + _RESTART,
    "Phải nhỏ hơn BW_PHUT_TRAN. Cần khởi động lại máy chủ.",
    0)
VN["BW_PHUT_TRAN"] = (
    "Tỷ võ: tổng phút một trận kể cả chờ",
    "Tổng số phút một trận kể cả thời gian chờ (mặc định 12 = 2 chờ + 10 đánh).\n" + _RESTART,
    "Đặt nhỏ hơn hoặc bằng BW_PHUT_CHO là không có thời gian đánh. Cần khởi động lại máy chủ.",
    0)

# --- THANH bao (tongcastle) --------------------------------------------------------------------
_TC_CHUA = "Chú ý: client hiện chưa có dữ liệu bản đồ 984 nên người chơi chưa vào được cho tới khi bổ sung pak client (máy chủ đã sẵn sàng)."
VN["TC_CAP_TOITHIEU"] = (
    "Thành bảo: cấp tối thiểu",
    "Cấp tối thiểu để tham gia Bang Hội Thành Bảo (bản đồ 984), áp dụng ở cả ba chỗ kiểm. Mặc định 90.\n" + _TC_CHUA + "\n" + _LIVE,
    "Hạ thấp là người yếu vào bị Thủ Vệ hạ.",
    1)
VN["TC_VAO_BANG_PHUT"] = (
    "Thành bảo: số phút phải vào bang trước khi tham gia",
    "Phải là thành viên bang ít nhất bấy nhiêu phút mới được tham gia (1440 = 1 ngày, chống nhảy bang). Mặc định 1440.\n" + _LIVE,
    "Đặt 0 là vừa vào bang đã đánh được, bang lớn kéo người vào tạm.",
    1)
VN["TC_THU"] = (
    "Thành bảo: thứ mở cửa trong tuần",
    "Ngày trong tuần mở Thành Bảo: 0 = Chủ nhật, 1 = Thứ hai ... 6 = Thứ bảy. Mặc định 0.\n" + _RESTART,
    "Chỉ nhận 0..6. Cần khởi động lại máy chủ. Lời thoại NPC ghi cứng giờ cũ, chỉ là chữ.",
    1, {"dv": "", "mn": 0, "mx": 6})
VN["TC_GIO_NHANDIEM_TU"] = (
    "Thành bảo: giờ bắt đầu nhận điểm Thần Mộc",
    "Đầu khung giờ được nhận điểm Thần Mộc, dạng HH:MM giờ máy chủ. Mặc định 19:00.\n" + _LIVE,
    "Đặt sau giờ kết thúc là không ai nhận được điểm.",
    1)
VN["TC_GIO_NHANDIEM_DEN"] = (
    "Thành bảo: giờ kết thúc nhận điểm Thần Mộc (HHMM)",
    "Cuối khung giờ nhận điểm, ghi dạng số HHMM, 2400 = hết ngày. Mặc định 2400.\n" + _LIVE,
    "Ghi dạng số HHMM (1930 = 19:30), tối đa 2400. Nhỏ hơn giờ bắt đầu là khung rỗng.",
    1, {"k": 0, "dv": "HHMM", "mn": 0, "mx": 2400})
VN["TC_PHUT_TOITHIEU"] = (
    "Thành bảo: số phút lưu trú tối thiểu",
    "Phải ở trong bản đồ ít nhất bấy nhiêu phút mới được nhận điểm. Mặc định 45.\n" + _LIVE,
    "Đặt 0 là vào ra nhận điểm ngay.",
    1)
for _i, _ten, _md in ((1, "Thanh Đồng (3205)", "10"), (2, "Bạch Ngân (3206)", "20"), (3, "Hoàng Kim (3207)", "120")):
    VN["TC_GIA_LENH_%d" % _i] = (
        "Thành bảo: giá đổi Thần Mộc Lệnh %s" % _ten.split(" (")[0],
        "Số điểm Thần Mộc để đổi một Thần Mộc Lệnh %s. Mặc định %s.\n%s" % (_ten, _md, _LIVE),
        "Đặt 0 là đổi miễn phí. Ba mức nên tăng dần.",
        1, {"dv": "diem"})
VN["TC_DOI_LENH_TOIDA"] = (
    "Thành bảo: số lệnh đổi tối đa một lần",
    "Mỗi lần đổi tối đa bấy nhiêu Thần Mộc Lệnh. Mặc định 100.\n" + _LIVE,
    "Chỉ là tiện ích, không đổi tổng số được đổi.",
    0, {"dv": "cai"})
VN["TC_LENH_TUAN"] = (
    "Thành bảo: trần mỗi loại lệnh mỗi tuần",
    "Mỗi người mỗi tuần đổi tối đa bấy nhiêu lệnh mỗi loại. Mặc định 5.\n" + _LIVE,
    "Đặt cao là thưởng Thành Bảo (exp, chân nguyên) không trần.",
    2, {"dv": "cai"})
VN["TC_GIA_BUA"] = (
    "Thành bảo: giá Bùa triệu Thủ Vệ",
    "Số điểm Thần Mộc để mua một Bùa triệu Thủ Vệ (3204). Mặc định 200.\n" + _LIVE,
    "Đặt 0 là Thủ Vệ miễn phí, bang lớn triệu kín bản đồ.",
    1, {"dv": "diem"})
VN["TC_MUA_BUA_TOIDA"] = (
    "Thành bảo: số bùa mua tối đa một lần",
    "Mỗi lần mua tối đa bấy nhiêu bùa. Mặc định 100.\n" + _LIVE,
    "Chỉ là tiện ích.",
    0, {"dv": "cai"})
VN["TC_TRAN_THUVE"] = (
    "Thành bảo: trần tổng số Thủ Vệ trong bản đồ",
    "Tổng số NPC Thủ Vệ được đứng trong bản đồ cùng lúc. Mặc định 100.\n" + _LIVE,
    "Đặt cao là bản đồ đầy NPC, tải cao và khó di chuyển.",
    1, {"dv": "con"})
VN["TC_BANKINH_BUA"] = (
    "Thành bảo: bán kính dùng bùa quanh cây",
    "Phải đứng cách cây Thần Mộc trong bán kính này (mét) mới dùng được bùa. Mặc định 15.\n" + _LIVE,
    "Đặt quá nhỏ là không ai dùng được bùa.",
    0, {"dv": "met"})
