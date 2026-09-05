# -*- coding: utf-8 -*-
"""Nhom EXP / EXP_KYNANG / TIEN: khoa C++ (gamesetting.ini [ServerConfig] + [Exp], an NGAY khi
sua tren web) va khoa script (GLB_TILE_*, BRXP_*)."""

VN = {}

# ---------------------------------------------------------------------------------------------
# [ServerConfig] - KPlayer.cpp:2610 AddSelfExp, KNpc.cpp:9411 tien roi, KNpc.cpp:4570 exp ky nang
# ---------------------------------------------------------------------------------------------
VN["ServerConfig.ExpRate"] = (
    "Hệ số nhân kinh nghiệm toàn máy chủ (C++)",
    "Hệ số nhân cuối cùng cho mọi kinh nghiệm đánh quái, boss và nhiệm vụ cộng qua máy chủ (hàm AddSelfExp).\n"
    "Công thức: exp nhận = exp gốc (sau khi chia tổ đội, phạt chênh cấp, buff x2) x ExpRate x (hệ số theo mốc cấp + VIP). "
    "Ví dụ ExpRate = 1 và hệ số cấp 80 thì mỗi 100 exp gốc thành 8.000 exp; ExpRate = 2 là gấp đôi toàn bộ.\n"
    "Đây là khoá C++, sửa trên web có hiệu lực trong 30 giây, không cần khởi động lại. Muốn chỉnh nhịp lên cấp thì ưu tiên chỉnh khoá này thay vì các hệ số theo cấp.",
    "Đây là hệ số nhân trực tiếp, không phải phần trăm: 1 sang 2 là gấp đôi exp của cả server ngay lập tức và exp đã cộng cho người chơi không thu hồi được. "
    "Gõ nhầm thêm một số 0 (10 thay vì 1) sẽ làm cả server lên cấp 200 trong vài giờ. Chỉ tăng từng bước nhỏ, đổi xong theo dõi 10 phút. Máy chủ từ chối giá trị ngoài 1..100000.",
    2)
VN["ServerConfig.MoneyRate"] = (
    "Hệ số nhân tiền quái rơi (C++)",
    "Nhân vào số tiền (lượng) rơi ra mỗi khi quái chết. Tiền gốc = exp của quái x MoneyScale của bảng rớt / 100, rồi nhân với khoá này.\n"
    "1 = giữ nguyên; 2 = mọi quái rơi gấp đôi tiền. Khoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Tăng khoá này là tăng lạm phát của toàn server: tiền do bot và người cày đổ vào thị trường sẽ nhân lên tương ứng và không rút lại được. "
    "Chỉ tăng từng bước (1 sang 2), theo dõi giá chợ vài ngày rồi mới cân nhắc tiếp. Không đặt 0 (máy chủ từ chối, khoảng cho phép 1..100000).",
    2)
VN["ServerConfig.Skill90Rate"] = (
    "Tốc độ luyện kỹ năng 90",
    "Mỗi lần giết quái, exp kỹ năng 90 đang luyện được cộng = Skill90Rate x hệ số tăng cường đang có trên nhân vật (buff luyện kỹ năng). "
    "0 = tắt hẳn việc luyện kỹ năng 90 (không cộng gì). Bot SimCity không được cộng.\n"
    "Bản gốc dùng giá trị từ 100 trở lên. Khoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Đặt 0 làm người chơi tưởng lỗi game vì kỹ năng 90 không lên nữa. Đặt quá cao (hàng nghìn) thì kỹ năng 90 đầy trong vài phút, mất ý nghĩa cày. Khoảng cho phép 0..100000.",
    1)
VN["ServerConfig.Skill120Rate"] = (
    "Tốc độ luyện kỹ năng 120",
    "Giống Skill90Rate nhưng cho kỹ năng 120: mỗi lần giết quái cộng Skill120Rate x hệ số tăng cường đang có. 0 = tắt hẳn việc luyện kỹ năng 120.\n"
    "Bản gốc dùng giá trị từ 100 trở lên. Khoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Đặt 0 thì kỹ năng 120 không bao giờ lên. Đặt quá cao làm mất giá trị của kỹ năng 120 là mục tiêu dài hạn của người chơi. Khoảng cho phép 0..100000.",
    1)

# ---------------------------------------------------------------------------------------------
# [Exp] - KPlayer.cpp:2533-2585
# ---------------------------------------------------------------------------------------------
_MOC = {1: ("thứ nhất", "HeSo1", "50"), 2: ("thứ hai", "HeSo2", "80"), 3: ("thứ ba", "HeSo3", "140")}
for _i, (_tt, _hs, _md) in _MOC.items():
    VN["Exp.MocCap%d" % _i] = (
        "Mốc cấp %s của bảng hệ số exp" % _tt,
        "Nhân vật có cấp nhỏ hơn mốc này (và không rơi vào mốc thấp hơn) dùng hệ số exp %s. Ba mốc phải tăng dần: MocCap1 < MocCap2 < MocCap3; "
        "từ MocCap3 trở lên dùng HeSo4. Mặc định gốc %s.\nKhoá C++, sửa trên web có hiệu lực trong 30 giây." % (_hs, _md),
        "Ba mốc không tăng dần thì có khoảng cấp bị xếp sai hệ số (ví dụ MocCap2 nhỏ hơn MocCap1 làm HeSo2 không bao giờ được dùng). Khoảng cho phép 1..200. "
        "Đổi mốc là đổi tốc độ lên cấp của cả một dải cấp, hãy đối chiếu với bốn hệ số bên dưới trước khi lưu.",
        1)
_HESO = {
    1: ("thứ nhất", "cấp dưới MocCap1 (tân thủ)", "80"),
    2: ("thứ hai", "cấp từ MocCap1 đến dưới MocCap2", "70"),
    3: ("thứ ba", "cấp từ MocCap2 đến dưới MocCap3 (đang là dải 80 đến 139)", "280"),
    4: ("thứ tư", "cấp từ MocCap3 trở lên", "100"),
}
for _i, (_tt, _dai, _md) in _HESO.items():
    VN["Exp.HeSo%d" % _i] = (
        "Hệ số exp %s (%s)" % (_tt, _dai.split(" (")[0]),
        "Hệ số nhân exp cho %s. Công thức: exp nhận = exp gốc x ExpRate x (HeSo%d + VipCong). Mặc định gốc %s, nghĩa là nhân %s lần.\n"
        "Không phải phần trăm: 100 là nhân 100 lần chứ không phải giữ nguyên. Khoá C++, sửa trên web có hiệu lực trong 30 giây." % (_dai, _i, _md, _md),
        "Hệ số này nhân thẳng vào exp, nhân thêm với ExpRate: tăng gấp đôi ở đây là gấp đôi exp cho cả dải cấp. Đặt quá nhỏ (1..5) thì dải cấp đó gần như không lên nổi, người chơi bỏ game. "
        "Khoảng cho phép 1..100000. Đổi xong nhớ theo dõi log exp vài phút.",
        2)
VN["Exp.VipCong"] = (
    "Hệ số exp cộng thêm cho VIP",
    "Nhân vật đang có trạng thái VIP (cờ VIP = 2) được cộng thêm số này vào hệ số exp theo cấp trước khi nhân: (HeSoN + VipCong). "
    "Ví dụ HeSo3 = 280, VipCong = 20 thì VIP nhận 300 thay vì 280 (hơn khoảng 7%). 0 = VIP không hơn gì người thường.\nKhoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Đặt lớn hơn hệ số theo cấp (ví dụ 280) là VIP nhận gấp đôi người thường, gây mất cân bằng và khiếu nại. Khoảng cho phép 0..100000.",
    1)
VN["Exp.CsMap"] = (
    "Bản đồ áp dụng luật chuyển sinh",
    "Mã bản đồ mà nhân vật đã chuyển sinh nhiều lần (hơn CsLanToiThieu) dùng hệ số riêng CsDuoi / CsTren thay cho hệ số theo cấp. Mặc định 341. "
    "0 = không bản đồ nào áp dụng.\nKhoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Gõ sai mã bản đồ thì luật chuyển sinh âm thầm không chạy ở đâu cả (không có báo lỗi). Mã phải là bản đồ có thật trong MapList.ini. Khoảng cho phép 0..100000.",
    1)
VN["Exp.CsLanToiThieu"] = (
    "Số lần chuyển sinh tối thiểu để ăn hệ số chuyển sinh",
    "Nhân vật có số lần chuyển sinh lớn hơn số này (và đang đứng ở bản đồ CsMap) mới dùng hệ số CsDuoi / CsTren. Mặc định 3, tức là từ lần chuyển sinh thứ 4 trở đi.\n"
    "Khoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Đặt 0 thì ai chuyển sinh 1 lần cũng ăn hệ số chuyển sinh. Đặt quá cao thì không ai đạt tới và hai hệ số CsDuoi / CsTren vô nghĩa. Khoảng cho phép 0..100.",
    1)
VN["Exp.CsDuoi"] = (
    "Hệ số exp chuyển sinh, cấp dưới MocCap3",
    "Thay cho HeSo1..HeSo3 khi nhân vật đủ điều kiện chuyển sinh (xem CsLanToiThieu, CsMap) và cấp nhỏ hơn MocCap3. Mặc định 160. Vẫn được cộng VipCong và nhân ExpRate.\n"
    "Khoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Là hệ số nhân thẳng vào exp (không phải phần trăm). Đặt cao hơn nhiều so với HeSo3 thì người chuyển sinh lên cấp quá nhanh so với người thường ở cùng bản đồ. Khoảng cho phép 1..100000.",
    2)
VN["Exp.CsTren"] = (
    "Hệ số exp chuyển sinh, cấp từ MocCap3 trở lên",
    "Thay cho HeSo4 khi nhân vật đủ điều kiện chuyển sinh và cấp từ MocCap3 trở lên. Mặc định 50, nghĩa là người chuyển sinh cấp cao lên chậm hơn hệ số thường (100) tại bản đồ CsMap.\n"
    "Khoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Là hệ số nhân thẳng vào exp. Đặt cao hơn HeSo4 là đảo ngược ý đồ hãm tốc độ cấp cao. Khoảng cho phép 1..100000.",
    2)
VN["Exp.ChenhCapMax"] = (
    "Chênh cấp tối đa vẫn nhận đủ exp",
    "Khi nhân vật cao cấp hơn quái: chênh lệch (cấp nhân vật trừ cấp quái) nhỏ hơn hoặc bằng số này thì nhận đủ exp; chênh nhiều hơn thì exp bị chia cho ChiaKhiChenh. Mặc định 9.\n"
    "Nhân vật thấp cấp hơn quái chỉ nhận 1 điểm exp gốc (rồi mới nhân hệ số), trừ khi cả hai đều từ MienTruCap trở lên. Khoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Đặt quá lớn (200) là bỏ hẳn phạt chênh cấp: cấp 150 cày quái cấp 20 vẫn ăn đủ exp, mất ý nghĩa bản đồ theo cấp. Khoảng cho phép 0..200.",
    1)
VN["Exp.ChiaKhiChenh"] = (
    "Mức chia exp khi chênh cấp quá mức",
    "Khi chênh cấp vượt ChenhCapMax, exp gốc bị chia cho số này. Mặc định 10, tức là còn 10%. Đặt 1 là không phạt.\nKhoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Đặt 1 là bỏ phạt chênh cấp hoàn toàn. Đặt rất lớn (1000) thì đánh quái thấp cấp gần như không có exp, người chơi tưởng lỗi. Khoảng cho phép 1..100000.",
    1)
VN["Exp.MienTruCap"] = (
    "Cấp miễn phạt chênh cấp",
    "Khi cả nhân vật lẫn quái đều từ cấp này trở lên thì bỏ mọi phạt chênh cấp, nhận đủ exp gốc. Mặc định 90 (cấp 90 trở lên đánh boss cấp 95 vẫn đủ exp).\n"
    "Khoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Đặt thấp (ví dụ 1) là bỏ phạt chênh cấp cho mọi cấp. Đặt cao hơn cấp tối đa thì không ai được miễn. Khoảng cho phép 1..200.",
    1)
for _n in range(2, 9):
    VN["Exp.ToDoi%d" % _n] = (
        "Phần trăm exp mỗi người khi tổ đội %d người" % _n,
        "Khi tổ đội có %d người cùng hưởng, mỗi thành viên không trực tiếp giết quái nhận số phần trăm này của exp gốc (người giết nhận nguyên 100%%). "
        "Ví dụ 80 = mỗi người nhận 80%%. Sau đó mới nhân ExpRate và hệ số cấp.\nKhoá C++, sửa trên web có hiệu lực trong 30 giây." % _n,
        "Đặt 100 cho mọi cỡ đội là đi đội %d người ai cũng ăn đủ như đánh một mình, exp toàn server tăng vọt vì bot và người chỉ cần lập đội. "
        "Đặt quá thấp (1..10) thì không ai muốn lập đội. Khoảng cho phép 1..100." % _n,
        1)
VN["Exp.ToDoiKhac"] = (
    "Phần trăm exp mỗi người khi số người tổ đội ngoài 2..8",
    "Dùng khi số người hưởng exp không rơi vào 2..8 (trường hợp hiếm, ví dụ đội lớn hơn 8 do mở rộng). Mặc định 60. Cách tính giống ToDoi2..ToDoi8.\n"
    "Khoá C++, sửa trên web có hiệu lực trong 30 giây.",
    "Bình thường ít khi dùng tới; đặt 100 hoặc 1 đều ít ảnh hưởng nhưng vẫn nên giữ cùng mức với ToDoi8. Khoảng cho phép 1..100.",
    0)

# ---------------------------------------------------------------------------------------------
# script: GLB_TILE_* (lib_server.lua EXP_RATE / MONEY_RATE), BRXP_* (bao ruong)
# ---------------------------------------------------------------------------------------------
VN["GLB_TILE_EXP"] = (
    "Hệ số nhân exp thưởng của script (EXP_RATE)",
    "Hệ số EXP_RATE trong script (lib_server.lua): nhân vào exp thưởng của nhiệm vụ, sự kiện, hoạt động nào có nhân với EXP_RATE. "
    "không ảnh hưởng exp đánh quái (phần đó do ServerConfig.ExpRate và nhóm Exp.* của C++ quyết định). Mặc định 20.\n"
    "Giá trị được chốt khi nạp lib_server.lua nên đổi trên web chỉ có hiệu lực sau khi khởi động lại máy chủ.",
    "Là hệ số nhân thẳng: 20 sang 40 là gấp đôi exp thưởng của mọi nhiệm vụ / sự kiện dùng EXP_RATE. Exp đã phát không thu hồi được. Đặt 0 làm mọi thưởng đó về 0. "
    "Cần khởi động lại máy chủ mới có hiệu lực.",
    2)
VN["GLB_TILE_TIEN"] = (
    "Hệ số nhân tiền thưởng của script (MONEY_RATE)",
    "Hệ số MONEY_RATE trong script (lib_server.lua): nhân vào tiền thưởng của thư viện câu hỏi, sự kiện Hoa Đăng và các script khác có dùng MONEY_RATE. "
    "Không ảnh hưởng tiền quái rơi (do ServerConfig.MoneyRate). Mặc định 1.\nChốt khi nạp lib_server.lua nên cần khởi động lại máy chủ.",
    "Tăng ở đây là bơm tiền vào server qua sự kiện, gây lạm phát không rút lại được. Đặt 0 làm các thưởng tiền đó về 0. Cần khởi động lại máy chủ mới có hiệu lực.",
    2)
VN["BRXP_MOC_CS"] = (
    "Mốc chuyển sinh để chọn trần exp bảo rương",
    "Số lần chuyển sinh nhỏ hơn hoặc bằng mốc này dùng trần BRXP_TRAN_CS4; đúng bằng mốc + 1 dùng BRXP_TRAN_CS5; cao hơn nữa dùng BRXP_TRAN_CS6. Mặc định 4.\n"
    "Có hiệu lực trong vòng 1,5 phút (script đọc mỗi lần mở rương).",
    "Đặt sai mốc là người chuyển sinh cao bị áp trần thấp hoặc ngược lại. Khoảng hợp lý 0..10.",
    1)
for _k, _tt, _md in (("BRXP_TRAN_CS4", "chuyển sinh thấp (tới mốc)", "50"), ("BRXP_TRAN_CS5", "chuyển sinh bằng mốc + 1", "80"), ("BRXP_TRAN_CS6", "chuyển sinh cao hơn", "100")):
    VN[_k] = (
        "Trần exp mỗi ngày từ bảo rương, %s" % _tt,
        "Tổng exp một nhân vật (%s) được nhận trong một ngày từ tất cả chín loại bảo rương, tính bằng triệu exp (%s = %s triệu). Vượt trần thì mở rương không cộng exp nữa.\n"
        "Có hiệu lực trong vòng 1,5 phút." % (_tt, _md, _md),
        "Đơn vị là triệu: gõ 50000000 nghĩa là 50 nghìn tỷ, coi như bỏ trần. Đặt 0 là mở rương không có exp. Đây là trần kinh tế ẩn chống bơm exp qua rương, hãy giữ ba mức tăng dần.",
        2)
