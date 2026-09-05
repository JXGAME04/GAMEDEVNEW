# -*- coding: utf-8 -*-
"""Nhom HOATDONG: 3 hoat dong ban Linux (HD3_ST_ Sat Thu, HD3_BD_ Bac Dau, HD3_PLD_ Phong Lang Do,
HD3_VA_ Vuot ai), HCD_/QDHK_ item dung chung; nhom VIEMDE (YDBZ_*)."""

VN = {}

_LIVE = "Có hiệu lực trong vòng 1,5 phút (đọc qua HD_CFG lúc chạy)."
_RESTART = "Chốt khi nạp script, cần khởi động lại máy chủ."

# --- (A) SAT THU ------------------------------------------------------------------------------
VN["HD3_ST_CAP_TOITHIEU"] = (
    "Sát thủ: cấp tối thiểu (chỉ hiển thị)",
    "Cấp tối thiểu hiện trên menu admin của hoạt động Săn Boss Sát Thủ. Bản Linux không chặn cấp ở bước nhận nhiệm vụ (nhóm boss chia theo cấp 20..90 sẵn trong killbosshead). Mặc định 90.",
    "Đổi chỉ đổi chữ trên menu, không chặn ai.",
    0)
VN["HD3_ST_MAX_NGAY"] = (
    "Sát thủ: số lần giết boss tối đa mỗi ngày",
    "Trần số lần giết boss Sát Thủ mỗi người mỗi ngày (gốc Linux KILLER_MAXCOUNT = 8). Mặc định 8.\n" + _RESTART,
    "Tăng là Sát Thủ lệnh và exp Sát Thủ vào server nhiều hơn tương ứng. Cần khởi động lại máy chủ.",
    1, {"dv": "lan"})
VN["HD3_ST_HESO_EXP"] = (
    "Sát thủ: phần trăm exp mỗi lần giết boss",
    "Hệ số phần trăm exp thưởng mỗi lần giết một boss Sát Thủ: 100 = giữ nguyên bản Linux (15.000 exp ở nhóm cấp 20, tăng dần tới 200.000 ở nhóm cấp 90); 200 = gấp đôi; 50 = một nửa.\n" + _RESTART,
    "Là phần trăm, 100 là giữ nguyên: gõ 1 là exp còn 1%, gõ 1000 là gấp 10. Cần khởi động lại máy chủ.",
    1, {"dv": "%"})
VN["HD3_ST_EXP_QUAY"] = (
    "Sát thủ: exp ô quay thay Túi được phẩm (nhóm cấp 90)",
    "Số exp của ô quay thay cho Túi được phẩm trong bảng thưởng boss Sát Thủ cấp 90 (ô này chiếm khoảng 68% lượt quay). Mức 10 triệu cố định vẫn giữ. Mặc định 5.000.000.\n" + _RESTART,
    "Ô này trúng gần 7/10 lượt nên tăng là tăng exp trung bình mỗi lần giết boss rất mạnh. Cần khởi động lại máy chủ.",
    1)
VN["HD3_ST_SO_LENH"] = (
    "Sát thủ: số Sát Thủ lệnh mỗi lần giết boss",
    "Số Sát Thủ lệnh phát mỗi lần giết boss (bản Linux 1). Lệnh cùng cấp và cùng ngũ hành với boss; gộp 5 cái cùng cấp thành Sát Thủ Giản (vé vào Vượt ải).\n" + _LIVE,
    "Tăng là vé Vượt ải rẻ đi tương ứng.",
    1, {"dv": "cai"})
VN["HD3_ST_TIEN_XE"] = (
    "Sát thủ: giá thuê xe tới chỗ boss",
    "Số tiền (lượng) trừ khi bấm dẫn đường tới boss trên F11 (khuôn cũ MONEY_GO_BOSS = 1000). Mặc định 1000.\n" + _LIVE,
    "Đặt 0 là dẫn đường miễn phí, không hại nhiều.",
    0)

# --- BAC DAU LENH BAI -------------------------------------------------------------------------
VN["HD3_BD_BAT"] = (
    "Bắc Đẩu: bật hệ lệnh bài",
    "1 = bật hệ Bắc Đẩu (gom lệnh bài từ các hoạt động rồi đổi thưởng; sinh NPC Bắc Đẩu). 0 = tắt hẳn: không sinh NPC, không phát lệnh bài.\n" + _LIVE,
    "Tắt giữa chừng là lệnh bài người chơi đang giữ không đổi được nữa cho tới khi bật lại.",
    1)
VN["HD3_BD_GIO_MO"] = (
    "Bắc Đẩu: giờ NPC bắt đầu làm việc (giờ chẵn)",
    "Giờ trong ngày NPC Bắc Đẩu bắt đầu làm việc, chỉ ghi giờ 0..23 (giờ máy chủ). Bản Linux 8.\n" + _LIVE,
    "Chỉ nhận số giờ 0..23, không phải HHMM. Đặt sau giờ đóng là NPC không bao giờ làm việc.",
    0, {"k": 0, "dv": "gio", "mn": 0, "mx": 23})
VN["HD3_BD_GIO_DONG"] = (
    "Bắc Đẩu: giờ NPC nghỉ (giờ chẵn)",
    "Giờ trong ngày NPC Bắc Đẩu nghỉ, chỉ ghi giờ 0..23. Bản Linux 22.\n" + _LIVE,
    "Chỉ nhận 0..23. Phải lớn hơn giờ mở.",
    0, {"k": 0, "dv": "gio", "mn": 0, "mx": 23})
VN["HD3_BD_CAP_TOITHIEU"] = (
    "Bắc Đẩu: cấp tối thiểu nhận nhiệm vụ",
    "Cấp tối thiểu (hoặc đã trùng sinh) để nhận nhiệm vụ Bắc Đẩu. Bản Linux 150.\n" + _LIVE,
    "Hạ thấp là nhiều người hơn nhận 8 triệu exp mỗi nhiệm vụ (HD3_BD_EXP_NHIEMVU).",
    1)
VN["HD3_BD_SO_LAN_NGAY"] = (
    "Bắc Đẩu: số lần nhận nhiệm vụ mỗi ngày",
    "Số nhiệm vụ Bắc Đẩu mỗi người nhận được mỗi ngày. Bản Linux 30.\n" + _LIVE,
    "Nhân với HD3_BD_EXP_NHIEMVU là exp tối đa mỗi ngày từ Bắc Đẩu (30 x 8 triệu = 240 triệu). Tăng là tăng trần đó.",
    1)
VN["HD3_BD_EXP_NHIEMVU"] = (
    "Bắc Đẩu: exp thưởng mỗi nhiệm vụ đi đường",
    "Exp thưởng mỗi nhiệm vụ đi đường hoàn thành. Bản Linux 8.000.000.\n" + _LIVE,
    "Nhân với 30 nhiệm vụ mỗi ngày; tăng gấp đôi là thêm 240 triệu exp mỗi người mỗi ngày.",
    2)
VN["HD3_BD_HAN_LENHBAI"] = (
    "Bắc Đẩu: hạn dùng lệnh bài",
    "Số phút lệnh bài tồn tại trước khi hết hạn. Bản Linux 1440 = 1 ngày.\n" + _LIVE,
    "Đặt ngắn là người chơi chưa kịp đổi đã mất lệnh bài, khiếu nại.",
    1, {"dv": "phut"})
VN["HD3_BD_LINHDAN_NGAY"] = (
    "Bắc Đẩu: số lần dùng Huyết Linh Đơn mỗi ngày",
    "Số lần dùng Bắc Đẩu Huyết Linh Đơn mỗi người mỗi ngày. Bản Linux 15.\n" + _LIVE,
    "Tăng là thưởng của đơn này không trần.",
    1, {"dv": "lan"})
VN["HD3_BD_QUYDOI_LENHBAI"] = (
    "Bắc Đẩu: phát kèm Lệnh bài Bắc Đẩu chung",
    "1 = mỗi hoạt động phát kèm 1 'Lệnh bài Bắc Đẩu' (mã dùng để đổi thưởng 15/20) bên cạnh lệnh bài riêng. 0 = y hệt bản Linux: chỉ phát lệnh bài riêng, mà 13 lệnh bài riêng đó không có chỗ tiêu, chỉ để sưu tập.\n" + _LIVE,
    "Đặt 0 là người chơi không đổi được thưởng Bắc Đẩu nữa (không có lệnh bài chung).",
    1, {"k": 3})
VN["HD3_BD_LENHBAI_RIENG"] = (
    "Bắc Đẩu: phát thêm lệnh bài riêng của từng hoạt động",
    "0 = không (mặc định): mỗi hoạt động chỉ cho 1 'Lệnh bài Bắc Đẩu' chung (xếp chồng 50/ô, mã duy nhất đổi thưởng). 1 = có, đúng bản Linux: phát thêm lệnh bài riêng (chỉ để sưu tập), người chơi nhận 2 vật phẩm mỗi lần.\n" + _LIVE,
    "Bật là túi người chơi thêm vật phẩm vô dụng mỗi lần hoàn thành hoạt động.",
    0, {"k": 3})

# --- (B) PHONG LANG DO ------------------------------------------------------------------------
VN["HD3_PLD_CAP_TOITHIEU"] = (
    "Phong Lăng Độ: cấp tối thiểu (chỉ hiển thị)",
    "Cấp hiện trên menu admin. Bản Linux chỉ đòi có môn phái (fld_head.lua:40), không chặn cấp. Mặc định 1.",
    "Đổi chỉ đổi chữ trên menu.",
    0)
VN["HD3_PLD_SUC_CHUA"] = (
    "Phong Lăng Độ: sức chứa mỗi thuyền",
    "Số người tối đa mỗi chuyến thuyền (fld_haveroom, đã nối HD_CFG đọc lúc chạy). Mặc định 100.\n" + _RESTART,
    "Đặt nhỏ là người đến sau không lên thuyền được. Cần khởi động lại máy chủ.",
    1, {"dv": "nguoi"})
VN["HD3_PLD_CO_THUYENPHU_BAC"] = (
    "Phong Lăng Độ: có thuyền phu ở bờ Bắc",
    "1 = giữ như dự án cũ: có NPC thuyền phu ở bờ Bắc đưa về bờ Nam. 0 = đúng bản Linux: chỉ 3 thuyền phu bờ Nam.\n" + _RESTART,
    "Đặt 0 là người ở bờ Bắc phải tự về bằng cách khác. Cần khởi động lại máy chủ.",
    0, {"k": 3})
VN["HD3_PLD_PHI_VENAM"] = (
    "Phong Lăng Độ: phí qua sông từ bờ Bắc",
    "Số tiền (lượng) thuyền phu bờ Bắc thu để đưa về bờ Nam. Dự án cũ 1000.\n" + _LIVE,
    "Đặt 0 là miễn phí, không hại nhiều.",
    0)
VN["HD3_PLD_SO_AN_BOSS"] = (
    "Phong Lăng Độ: số Thi Gia Chi ấn rơi từ boss Đầu Lĩnh",
    "Số Thi Gia Chi ấn (6,1,1095) rơi từ boss Thuỷ Tặc Đầu Lĩnh 725 (gốc 2; bang có Đảo Chủ tiền đại thì gấp đôi theo getSignetDropRate).\n" + _RESTART,
    "Tăng là bơm ấn vào server. Cần khởi động lại máy chủ.",
    1, {"dv": "cai"})
VN["HD3_PLD_TILE_HAILONG"] = (
    "Phong Lăng Độ: tỉ lệ rơi Hải Long Châu từ boss",
    "Tỉ lệ rơi Hải Long Châu (6,1,2124) từ boss, ghi dạng số thập phân 0 đến 1 (0.005 = 0,5%). Gốc 0.005.\n" + _RESTART,
    "Ghi 5 thay vì 0.005 là 500%, boss nào cũng rơi. Chỉ nhận 0..1. Cần khởi động lại máy chủ.",
    2, {"dv": "", "mn": 0, "mx": 1})
VN["HD3_PLD_TILE_TRUYCONG"] = (
    "Phong Lăng Độ: phần trăm rơi Truy Công Lệnh từ Thuỷ Tặc thường",
    "Tỉ lệ phần trăm rơi Truy Công Lệnh (6,1,2024) từ Thuỷ Tặc thường trong giờ sự kiện (gốc: số bốc nhỏ hơn 50 trên 100 = 49%). Gốc 50.\n" + _RESTART,
    "Là phần trăm 0..100. Đặt 100 là quái nào cũng rơi. Cần khởi động lại máy chủ.",
    1, {"dv": "%", "mn": 0, "mx": 100})
VN["HD3_PLD_TILE_TRUYCONG_BOSS"] = (
    "Phong Lăng Độ: phần trăm rơi Truy Công Lệnh từ boss Đầu Lĩnh",
    "Tỉ lệ phần trăm rơi Truy Công Lệnh từ mỗi boss Đầu Lĩnh trong giờ hoạt động. 100 = luôn rơi (đúng bản Linux). Hạ xuống nếu thấy ra quá nhiều.\n" + _LIVE,
    "Là phần trăm 0..100.",
    1, {"dv": "%", "mn": 0, "mx": 100})
VN["HD3_PLD_THUONG_CAPBEN"] = (
    "Phong Lăng Độ: số Bảo Rương Thuỷ Tặc khi cập bến",
    "Số Bảo Rương Thuỷ Tặc (6,1,3361) phát khi cập bến thành công. Gốc 2.\n" + _RESTART,
    "Tăng là tăng thưởng mỗi chuyến, nhân với 24 chuyến mỗi ngày. Cần khởi động lại máy chủ.",
    1, {"dv": "cai"})

# --- (C) VUOT AI ------------------------------------------------------------------------------
VN["HD3_VA_CAP_TOITHIEU"] = (
    "Vượt ải: cấp tối thiểu (chỉ hiển thị)",
    "Cấp hiện trên menu admin. Giới hạn thật nằm ở bảng tbLevels trong include.lua (sơ cấp 50-89, cao cấp 90+); đổi ở đây không có tác dụng. Mặc định 50.",
    "Đổi chỉ đổi chữ trên menu.",
    0)
VN["HD3_VA_PHUT_BAODANH"] = (
    "Vượt ải: phút báo danh",
    "Số phút báo danh mỗi lượt (đã nối vào challengeoftime include.lua). Mặc định 10.\n" + _RESTART,
    "Cộng với phút làm nhiệm vụ không được vượt 60 vì lượt mở mỗi giờ. Cần khởi động lại máy chủ.",
    1)
VN["HD3_VA_PHUT_NHIEMVU"] = (
    "Vượt ải: phút làm nhiệm vụ",
    "Số phút tổ đội được ở trong ải. Mặc định 30.\n" + _RESTART,
    "Đặt ngắn là không kịp qua 28 ải. Báo danh + nhiệm vụ không vượt 60 phút. Cần khởi động lại máy chủ.",
    1)
VN["HD3_VA_NGUOI_TOIDA"] = (
    "Vượt ải: số người tối đa một đội",
    "Số người tối đa một tổ đội vào ải (LIMIT_PLAYER_COUNT = 8). Mặc định 8.\n" + _RESTART,
    "Đặt trên 8 vượt cỡ tổ đội của engine, không có tác dụng. Cần khởi động lại máy chủ.",
    0, {"mn": 1, "mx": 8})
VN["HD3_VA_GIO_XEPHANG"] = (
    "Vượt ải: giờ chốt bảng xếp hạng ngày",
    "Giờ chốt bảng xếp hạng ngày, dạng HH:MM giờ máy chủ (00:00 = nửa đêm). Mặc định 00:00.\n" + _RESTART,
    "Đổi giờ chốt giữa ngày là bảng xếp hạng chốt lệch. Cần khởi động lại máy chủ.",
    0)
VN["HD3_VA_LUOT_NGAY"] = (
    "Vượt ải: số lượt mỗi người mỗi ngày",
    "Số lượt vào Vượt ải mỗi người mỗi ngày (COUNT_LIMIT gốc Linux = 1). Mặc định 1.\n" + _RESTART,
    "Tăng là thưởng Vượt ải (Thiên Niên Linh Dược, rương) nhân lên. Cần khởi động lại máy chủ.",
    1, {"dv": "lan"})

# --- item dung chung --------------------------------------------------------------------------
VN["HCD_EXP"] = (
    "Exp của Hoàng Chân Đơn (2273)",
    "Exp cộng khi dùng Hoàng Chân Đơn, đi theo đường exp có trần cấp 200. Mặc định 2.000.000.000 (2 tỷ).\n" + _LIVE,
    "Đây là vật phẩm thưởng của nhiều hoạt động (Bang Chiến, Thành Bảo). Tăng là mọi nguồn phát đơn này cùng tăng exp. Số đã rất lớn, đừng thêm số 0.",
    2)
VN["QDHK_EXP"] = (
    "Exp của Quả Đại Hoàng Kim (4864)",
    "Exp cộng khi dùng Quả Đại Hoàng Kim, đi theo đường exp thường. Mặc định 200.000.000.\n" + _LIVE,
    "Vật phẩm thưởng của nhiều hoạt động; tăng là mọi nguồn phát cùng tăng.",
    2)

# --- VIEM DE bao TANG -------------------------------------------------------------------------
VN["YDBZ_BAT"] = (
    "Viêm Đế: bật toàn bộ tính năng",
    "1 = bật Viêm Đế Bảo Tàng. 0 = tắt hẳn (driver không làm gì). Công tắc BAT_VIEMDE ở nhóm Hoạt động cũng phải bật.\n" + _RESTART,
    "Tắt giữa chừng cần khởi động lại máy chủ mới có hiệu lực; trận đang chạy không bị cắt.",
    1)
VN["YDBZ_GIO"] = (
    "Viêm Đế: các giờ mở báo danh",
    "Danh sách giờ mở báo danh trong ngày, dạng HHMM cách nhau bằng dấu phẩy, giờ máy chủ. Gốc Linux: 0825,1025,1425,1625,1825,2025,2225.\n" + _LIVE,
    "Sai định dạng (thiếu số 0 đầu, dấu chấm phẩy, khoảng trắng lạ) là script không nhận giờ nào, hoạt động không mở. Mỗi mốc cách nhau ít nhất báo danh + trận (5 + 30 phút).",
    2)
VN["YDBZ_CAP_TOITHIEU"] = (
    "Viêm Đế: cấp tối thiểu báo danh",
    "Cấp tối thiểu để báo danh. Gốc head.lua:48 = 120.\n" + _LIVE,
    "Hạ thấp là đội yếu không qua nổi ải, tốn lượt.",
    1)
VN["YDBZ_LAN_TUAN"] = (
    "Viêm Đế: số lần tham gia tối đa mỗi tuần",
    "Trần số lần mỗi người tham gia trong tuần (mang Viêm Đế Lệnh thì được miễn trần, đúng bản gốc). Gốc 10.\n" + _LIVE,
    "Tăng là thưởng Viêm Đế nhân lên. Nên lớn hơn hoặc bằng trần ngày.",
    1)
VN["YDBZ_LAN_NGAY"] = (
    "Viêm Đế: số lần tham gia tối đa mỗi ngày",
    "Trần số lần mỗi người tham gia trong ngày. Gốc 4.\n" + _LIVE,
    "Đặt lớn hơn trần tuần là vô nghĩa.",
    1)
VN["YDBZ_DOI_MIN"] = (
    "Viêm Đế: số người tối thiểu một tổ đội",
    "Tổ đội phải có ít nhất bấy nhiêu người mới được vào. Gốc 4.\n" + _LIVE,
    "Đặt 1 là đi một mình được, ải thiết kế cho 4-6 người.",
    1, {"dv": "nguoi"})
VN["YDBZ_DOI_MAX"] = (
    "Viêm Đế: số người tối đa một tổ đội",
    "Tổ đội tối đa bấy nhiêu người. Gốc 6.\n" + _LIVE,
    "Phải lớn hơn hoặc bằng YDBZ_DOI_MIN và không quá 8 (cỡ tổ đội engine).",
    1, {"dv": "nguoi", "mn": 1, "mx": 8})
VN["YDBZ_PHONG_TOIDA"] = (
    "Viêm Đế: số phòng (tổ đội) tối đa mỗi đợt",
    "Số tổ đội (= số phòng chờ) tối đa mỗi đợt. Gốc 15. Mỗi phòng là một bản đồ riêng (hiện 1003-1017).\n" + _RESTART,
    "Đổi số này phải khai thêm bản đồ vào MapList.ini và WorldSet_GameServer.ini (xem ReverseTools\\viemde\\v02_khai_bando.py), nếu không phòng thứ 16 trở đi không có bản đồ và lỗi. Cần khởi động lại máy chủ.",
    2, {"dv": "phong"})
VN["YDBZ_PHUT_BAODANH"] = (
    "Viêm Đế: thời gian báo danh",
    "Số phút báo danh mỗi đợt. Gốc ready.lua:38 = 5.\n" + _RESTART,
    "Đặt quá dài là chồng lên mốc giờ kế trong YDBZ_GIO. Cần khởi động lại máy chủ.",
    1)
VN["YDBZ_PHUT_TRAN"] = (
    "Viêm Đế: thời hạn một trận",
    "Số phút tối đa một trận. Gốc head.lua:42 = 30.\n" + _LIVE,
    "Đặt ngắn là không kịp qua 3 đường x 10 ải và boss cuối.",
    1)
VN["YDBZ_NPC_MAP"] = (
    "Viêm Đế: bản đồ đặt NPC Bình Bình cô nương",
    "Mã bản đồ đặt 4 NPC 'Bình Bình cô nương' (mẫu 1320). Gốc 37 = Biện Kinh.\n" + _RESTART,
    "Sai mã bản đồ là NPC không xuất hiện, không ai báo danh được. Toạ độ NPC gắn với bản đồ 37. Cần khởi động lại máy chủ.",
    2)
