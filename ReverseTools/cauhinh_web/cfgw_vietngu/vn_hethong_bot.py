# -*- coding: utf-8 -*-
"""Nhom HETHONG (GLB_GIO_MO_SERVER, GLB_CHE_DO_TEST, CH_*) va CHUNG (he so suc manh GLB_*, BOT_*)."""

VN = {}

# --- HE THONG ---------------------------------------------------------------------------------
VN["GLB_GIO_MO_SERVER"] = (
    "Mốc giờ mở cửa máy chủ (yymmddHHMM)",
    "Mốc thời gian mở cửa, viết liền dạng năm-tháng-ngày-giờ-phút hai chữ số mỗi phần: 2506251900 = 19:00 ngày 25/06/2025. "
    "Bảy chỗ trong script đọc mốc này để chặn người chơi làm một số việc trước giờ mở (giờ máy chủ).\n"
    "Chú ý: script\\lib\\worldlibrary.lua còn một bản trùng tên (giá trị cũ 2104011900) không bao giờ thắng bản này. Chốt khi nạp script, cần khởi động lại máy chủ.",
    "Đặt mốc ở tương lai là các chức năng phụ thuộc giờ mở bị khoá cho mọi người chơi tới lúc đó. Gõ thiếu một chữ số là mốc thành ngày vô nghĩa. Nhóm Hệ thống, cần khởi động lại máy chủ.",
    2, {"k": 0, "dv": ""})
VN["GLB_CHE_DO_TEST"] = (
    "Chế độ thử nghiệm (NPC Hỗ Trợ Test)",
    "1 = mở chế độ thử nghiệm: hai NPC 'Hỗ Trợ Test' tại làng tân thủ (bản đồ 53) mở menu GM đầy đủ cho mọi người chơi (1 tỷ lượng, 100.000 Xu, lên thẳng cấp 200, không kiểm tài khoản GM, không giới hạn số lần). "
    "0 = tắt, đổi về NPC tân thủ thật.\nChốt khi nạp script, cần khởi động lại máy chủ. Xem BAOCAO_LOHONG_2908.md mục 1.",
    "Đang mở trên máy chủ thật là lỗ hổng lớn nhất của server: ai cũng tự phát đồ GM. Trước khi mở cửa cho người chơi thật phải đặt 0 và khởi động lại máy chủ.",
    2)
VN["CH_NAPLAI_PHUT"] = (
    "Nhịp tự nạp lại script (phút)",
    "timerserver.lua tự nạp lại chính nó (33 Include, khoảng 103 tệp, gần 1 MB) theo nhịp này. 1 = mỗi phút (mặc định, sửa script ăn ngay). 5 = năm phút một lần, nhẹ máy chủ hơn năm lần. "
    "0 = tắt hẳn: sửa script phải khởi động lại máy chủ mới có hiệu lực.\nKhoá này được đọc lúc tick nên đổi có hiệu lực ở nhịp kế tiếp; đã đặt 0 thì chỉ khởi động lại mới bật lại được.",
    "Đặt 0 là khoá luôn cơ chế nạp lại: mọi khoá script trên web (kể cả chính khoá này) chỉ còn đổi được khi khởi động lại máy chủ. Đặt quá lớn làm sửa script chờ lâu mới ăn. Khoảng hợp lý 0..60.",
    2, {"k": 0, "dv": "phut", "mn": 0, "mx": 60})
VN["CH_LOG_CAUHINH"] = (
    "Ghi log hệ cấu hình",
    "1 = hệ cấu hình ghi nhật ký vào logs\\hethong.log (hàm GhiLog của script). 0 = im lặng. Cần khởi động lại máy chủ.",
    "Tắt thì khi cấu hình không ăn sẽ không còn dấu vết để tra. Chỉ tắt khi log quá nhiều.",
    0)
VN["CH_LOG_THUONG"] = (
    "Ghi log hệ thưởng",
    "1 = hệ phát thưởng (ch_thuong_lib.lua) ghi mỗi lần phát thưởng vào logs\\hethong.log. 0 = im lặng. Có hiệu lực trong vòng 1,5 phút.",
    "Tắt thì mất dấu vết ai nhận thưởng gì, khó điều tra khiếu nại hay nhân bản đồ. Nên để 1.",
    0)

# --- CHUNG: he so suc manh --------------------------------------------------------------------
_HESO = {
    "GLB_SATTHUONG_BOSS_HK": ("Hệ số sát thương Boss Hoàng Kim tiểu", "sát thương của Boss Hoàng Kim tiểu bản Việt cũ"),
    "GLB_SATTHUONG_QUAI_PLD": ("Hệ số sát thương Thuỷ Tặc Phong Lăng Độ (bản Việt cũ)", "sát thương quái Thuỷ Tặc trên thuyền Phong Lăng Độ của bản Việt cũ"),
    "GLB_SATTHUONG_BOSS_TTDL": ("Hệ số sát thương boss Thuỷ Tặc Đầu Lĩnh (bản Việt cũ)", "sát thương boss Thuỷ Tặc Đầu Lĩnh của bản Việt cũ"),
    "GLB_MANH_BOSS_VUOTAI": ("Hệ số máu và sát thương quái Vượt ải (bản Việt cũ)", "máu và sát thương của quái và boss Vượt ải bản Việt cũ"),
}
for _k, (_ten, _mt) in _HESO.items():
    VN[_k] = (
        _ten,
        "Hệ số nhân vào %s. 1 = giữ nguyên, 2 = gấp đôi. Hoạt động bản Việt cũ này hiện đang tắt (thay bằng bản Linux) nên khoá gần như không có tác dụng; giữ lại để tương thích.\n"
        "Chốt khi nạp lib_server.lua, cần khởi động lại máy chủ." % _mt,
        "Không phải công tắc bật/tắt: đặt 0 là quái không gây sát thương (hoặc máu 0) chứ không phải tắt hoạt động. Chỉ có ý nghĩa khi bật lại hoạt động bản Việt cũ.",
        0, {"k": 0, "dv": "x"})
VN["GLB_MANH_NGUYENSOAI_TK"] = (
    "Hệ số máu và sát thương Nguyên Soái Tống Kim",
    "Nhân vào máu (TKT_MAU_NGUYENSOAI) và sát thương của Nguyên Soái hai phe trong Tống Kim. 1 = giữ nguyên, 2 = gấp đôi. Đang chạy thật.\nChốt khi nạp lib_server.lua, cần khởi động lại máy chủ.",
    "Đặt 0 là Nguyên Soái 0 máu, chết ngay khi xuất hiện và trận kết thúc lập tức. Đặt 10 là không ai hạ nổi. Tăng từng bước và thử một trận trước.",
    1, {"k": 0, "dv": "x"})
VN["GLB_MAX_DONG_THUOCTINH"] = (
    "Trần số dòng thuộc tính đồ rơi (chưa nối)",
    "Dự kiến là trần số dòng thuộc tính của trang bị rơi từ quái, nhưng hiện không tệp script nào đọc khoá này; trần thật đang nằm cứng trong mã C++. Đổi ở đây không có tác dụng.",
    "Đổi không có tác dụng gì cho tới khi có script hoặc C++ đọc khoá này. Đừng trông chờ đồ rơi đổi số dòng.",
    0)

# --- CHUNG: BOT tu dong (bot_auto.lua, doc moi phut) -------------------------------------------
VN["BOT_TU_GOI"] = (
    "Bật hệ bot tự động",
    "1 = mỗi lần máy chủ lên, script tự gọi bot và phân việc (Dã Tẩu, bày sạp, Tống Kim, đánh quái) theo các khoá BOT_* bên dưới. 0 = tắt toàn bộ hệ bot tự động (bot đang có vẫn tồn tại tới khi thoát).\n"
    "Có hiệu lực trong vòng 1 phút.",
    "Tắt là server vắng bot, chợ không có sạp bot, Tống Kim không có bot. Bật lại sau khi tắt lâu có thể mất vài phút để xếp hàng sinh bot.",
    1)
VN["BOT_SO_LUONG"] = (
    "Số bot gọi khi máy chủ lên",
    "Tổng số bot tự động được sinh sau khi máy chủ lên. Trần cứng của engine là 1000 (PB_MAX_BOTS); đặt lớn hơn cũng chỉ sinh được 1000. Mặc định 1000.\nCó hiệu lực trong vòng 1 phút.",
    "Sinh hàng trăm bot cùng lúc là gánh nặng lên Goddess và băng thông; vụ sập 15:05 ngày 04/09 xảy ra khi 783 bot đăng nhập dồn. Tăng số bot phải đi cùng BOT_CHO_PHUT và BOT_GOI_GIAN đủ lớn. Không vượt 1000.",
    2, {"mn": 0, "mx": 1000})
VN["BOT_CHO_PHUT"] = (
    "Chờ bao lâu sau khi máy chủ lên mới gọi bot",
    "Số phút chờ sau khi máy chủ lên rồi mới bắt đầu sinh bot, để Goddess (máy chủ dữ liệu) sẵn sàng. Mặc định 2.\nCó hiệu lực trong vòng 1 phút.",
    "Đặt 0 là bot đăng nhập khi Goddess chưa xong, dễ lỗi nạp dữ liệu nhân vật bot. Giữ từ 1 phút trở lên.",
    1)
VN["BOT_GOI_GIAN"] = (
    "Giãn cách tối thiểu giữa hai đợt sinh bot",
    "Tối thiểu bao nhiêu phút giữa hai lần xếp hàng đợi sinh bot. Mặc định 10.\nCó hiệu lực trong vòng 1 phút.",
    "Đặt 0 là các đợt sinh bot chồng lên nhau, đăng nhập dồn và có thể sập máy chủ như ngày 04/09. Giữ từ 5 phút trở lên.",
    2, {"dv": "phut"})
VN["BOT_DA_TAU"] = (
    "Số bot đi làm Dã Tẩu",
    "Số bot được phân đi làm nhiệm vụ Dã Tẩu. Chỉ chọn được bot cấp từ 70 trở lên nên số thật có thể ít hơn. Mặc định 200.\nCó hiệu lực trong vòng 1 phút.",
    "Nhiều bot Dã Tẩu là nhiều bot chạy khắp bản đồ, tăng tải đường đi và gói tin. Không nên vượt số bot thật đang có.",
    1, {"dv": "bot"})
VN["BOT_BAN_SAP"] = (
    "Số bot về thành bày sạp",
    "Số bot về thành bày sạp bán hàng (theo lưới sạp SimCity, tránh quanh NPC). Mặc định 200.\nCó hiệu lực trong vòng 1 phút.",
    "Nhiều sạp là thành chật, người chơi khó bấm NPC. Số sạp bị giới hạn bởi số ô sạp có sẵn trên bản đồ.",
    1, {"dv": "bot"})
VN["BOT_SAP_NGUONG"] = (
    "Ngưỡng phần trăm sạp còn lại để đặt lại sạp",
    "Khi số sạp bot còn bày dưới số phần trăm này so với BOT_BAN_SAP thì script dẹp hết sạp cũ và bày lại. Mặc định 60.\nCó hiệu lực trong vòng 1 phút.",
    "Đặt 100 là bày lại liên tục (sạp nhấp nháy, tốn gói tin). Đặt 0 là không bao giờ bày lại.",
    0, {"dv": "%", "mn": 0, "mx": 100})
VN["BOT_SAP_GIAN"] = (
    "Giãn cách tối thiểu giữa hai lần đặt lại sạp",
    "Tối thiểu bao nhiêu phút giữa hai lần dẹp và bày lại sạp bot. Mặc định 15.\nCó hiệu lực trong vòng 1 phút.",
    "Đặt 0 kết hợp ngưỡng cao là sạp bị dẹp/bày lại mỗi phút, người chơi đang xem sạp bị đóng.",
    0, {"dv": "phut"})
VN["BOT_TK_TRAN"] = (
    "Trần số bot mỗi trận Tống Kim",
    "Số bot tối đa tham gia một trận Tống Kim (chia hai phe). 0 = không giới hạn. Mặc định 500.\nCó hiệu lực trong vòng 1 phút.",
    "Quá nhiều bot trong một bản đồ Tống Kim là máy chủ phát gói tới hàng trăm người mỗi hành động; đây là gốc của hiện tượng 'không thấy địch/chiêu' ngày 03-04/09. Giữ dưới 500.",
    1, {"dv": "bot"})
VN["BOT_TK_TU_DONG"] = (
    "Bot tự vào Tống Kim theo giờ",
    "1 = bot tự báo danh và vào Tống Kim theo lịch TK_LICH. 0 = bot không tham gia Tống Kim.\nCó hiệu lực trong vòng 1 phút.",
    "Tắt là Tống Kim chỉ còn người thật, trận có thể không đủ người.",
    0)
VN["BOT_BAO_PHUT"] = (
    "Nhịp in trạng thái [BotAuto] ra log",
    "Cứ bao nhiêu phút in một dòng trạng thái [BotAuto] vào logs\\hethong.log. 0 = không in. Mặc định 10.\nCó hiệu lực trong vòng 1 phút.",
    "Chỉ ảnh hưởng log; đặt 1 làm log dài nhanh.",
    0)
VN["BOT_VAO_PHAI"] = (
    "Bot tự vào môn phái",
    "1 = bot chưa có phái sẽ tự vào phái. 0 = không (mặc định, vì bot đã trên cấp 100 và có phái sẵn).\nCó hiệu lực trong vòng 1 phút.",
    "Bật khi bot đã có phái không hại gì; chỉ tốn vài lần kiểm tra. Bot mới sinh chưa có phái mà tắt thì bot không có kỹ năng phái.",
    0)
VN["BOT_TU_DANH"] = (
    "Bot tự bật đánh quái",
    "1 = script tự bật chế độ đánh quái cho bot (bot ra bản đồ luyện cấp). 0 = bot đứng yên ở thành sau khi sinh.\nCó hiệu lực trong vòng 1 phút.",
    "Tắt là toàn bộ bot đứng yên ở thành, trông như server chết. Bật với số bot lớn là tải đường đi và gói tin tăng.",
    1)
VN["BOT_DANH_GIAN"] = (
    "Nhịp gọi lại lệnh bật đánh quái cho bot",
    "Cứ bao nhiêu phút gọi lại lệnh bật đánh quái (để bot mới sinh hoặc bot vừa xong việc khác cũng được bật). Mặc định 5.\nCó hiệu lực trong vòng 1 phút.",
    "Đặt 0 hoặc 1 là gọi mỗi phút, tốn vài chục mili giây mỗi lần với 1000 bot; không hại nhưng vô ích.",
    0, {"dv": "phut"})
