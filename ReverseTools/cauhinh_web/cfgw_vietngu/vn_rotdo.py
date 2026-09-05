# -*- coding: utf-8 -*-
"""Nhom ROTDO: DRQ_* (Droprate_normal.lua - quai thuong moi ban do) va SKD_* (lib_sukien.lua -
rot do su kien). Cach doc ti le: random(1, MAU) so voi NGUONG."""

VN = {}

_CACH_DOC = ("Cách đọc tỉ lệ của cả nhóm: máy chủ bốc một số ngẫu nhiên từ 1 đến mẫu số rồi so với ngưỡng; "
             "mẫu số càng lớn tỉ lệ càng nhỏ, ngưỡng dịch đi một đơn vị là tỉ lệ đổi 1/mẫu.")

VN["DRQ_HESO_TIEN"] = (
    "Hệ số nhân tiền quái thường rơi (script)",
    "Nhân thêm vào số tiền do script Droprate_normal.lua tính cho quái thường, độc lập với ServerConfig.MoneyRate của C++ (hai hệ số nhân chồng nhau). 1 = giữ nguyên.\n"
    "Có hiệu lực trong vòng 1,5 phút.",
    "Nhân chồng với MoneyRate: đặt 2 ở đây và 2 ở MoneyRate là tiền rơi gấp 4. Gây lạm phát không rút lại được; chỉ tăng từng bước nhỏ.",
    2)
VN["DRQ_CAP_BAC_TOITHIEU"] = (
    "Cấp quái xếp vào bậc rớt đồ 1",
    "Quái có cấp dưới số này dùng bảng rớt đồ bậc 1 (bảng thấp nhất). Từ cấp này trở lên, bậc được tính theo cấp quái. Mặc định 10.\nCó hiệu lực trong vòng 1,5 phút.",
    "Đặt quá cao (ví dụ 60) là quái cấp 59 vẫn rơi đồ bậc tân thủ. Phải nhỏ hơn DRQ_CAP_BAC_TOIDA.",
    1)
VN["DRQ_CAP_BAC_TOIDA"] = (
    "Cấp quái bốc ngẫu nhiên bậc 9 đến 10",
    "Quái có cấp trên số này không tính bậc theo cấp nữa mà bốc ngẫu nhiên bảng bậc 9 hoặc 10 (hai bảng cao nhất). Mặc định 100.\nCó hiệu lực trong vòng 1,5 phút.",
    "Hạ thấp là quái trung cấp cũng rơi đồ bậc cao nhất, phá giá trang bị. Phải lớn hơn DRQ_CAP_BAC_TOITHIEU.",
    1)
VN["DRQ_QUAY_BOSSXANH"] = (
    "Số món boss xanh rơi mỗi lần chết",
    "Mỗi lần boss xanh (boss thường trên bản đồ) chết, script quay bảng rớt đồ đúng số lần này, mỗi lần một món. Mặc định 8.\nCó hiệu lực trong vòng 1,5 phút.",
    "Tăng lên vài chục là mỗi boss xanh xả cả đống đồ, bot và người săn boss làm ngập chợ. Đặt 0 là boss xanh không rơi gì.",
    1)
VN["DRQ_MAU_BOSSXANH"] = (
    "Mẫu số bốc tiền của boss xanh",
    "Mẫu số của phép bốc tiền khi boss xanh chết: bốc từ 1 đến số này, lớn hơn DRQ_NGUONG_TIEN_BOSSXANH thì rơi tiền. Mặc định 10 (với ngưỡng 8 là khoảng 20%%).\n%s\nCó hiệu lực trong vòng 1,5 phút." % _CACH_DOC,
    "Đặt nhỏ hơn hoặc bằng ngưỡng thì boss xanh không bao giờ rơi tiền. Đặt 0 làm hàm random lỗi trong script (ScriptError). Giữ từ 1 trở lên và lớn hơn ngưỡng.",
    2, {"dv": ""})
VN["DRQ_NGUONG_TIEN_BOSSXANH"] = (
    "Ngưỡng rơi tiền của boss xanh",
    "Số bốc được (1 đến DRQ_MAU_BOSSXANH) phải lớn hơn ngưỡng này thì boss xanh mới rơi tiền. Mặc định 8 với mẫu 10 nghĩa là bốc được 9 hoặc 10 mới rơi (khoảng 20%%).\n%s\nCó hiệu lực trong vòng 1,5 phút." % _CACH_DOC,
    "Đặt 0 là boss xanh luôn rơi tiền; đặt bằng hoặc lớn hơn mẫu số là không bao giờ rơi. Cả hai đều đổi kinh tế server.",
    2, {"dv": ""})
VN["DRQ_MAU_QUAI"] = (
    "Mẫu số bốc rớt của quái thường",
    "Mẫu số dùng chung cho quái thường: bốc từ 1 đến số này, đúng bằng DRQ_NGUONG_TIEN thì rơi tiền, nhỏ hơn DRQ_NGUONG_ITEM thì quay bảng rớt đồ. Mặc định 30.\n%s\nCó hiệu lực trong vòng 1,5 phút." % _CACH_DOC,
    "Đây là nút chỉnh tỉ lệ rớt của mọi quái thường trên mọi bản đồ. Hạ từ 30 xuống 10 là tỉ lệ rớt đồ tăng gấp ba. Đặt 0 làm script lỗi (ScriptError) và quái không rơi gì.",
    2, {"dv": ""})
VN["DRQ_NGUONG_TIEN"] = (
    "Số trúng để quái thường rơi tiền",
    "Số bốc được (1 đến DRQ_MAU_QUAI) đúng bằng số này thì quái thường rơi tiền. Mặc định 2 với mẫu 30 nghĩa là khoảng 3,2%% số quái chết rơi tiền. Đặt -1 là tắt hẳn rơi tiền.\n%s\nCó hiệu lực trong vòng 1,5 phút." % _CACH_DOC,
    "Đây là xác suất trúng một số, không phải ngưỡng: đổi 2 thành 10 không tăng tỉ lệ (vẫn 1/mẫu). Muốn tăng tỉ lệ tiền phải giảm DRQ_MAU_QUAI. Đặt lớn hơn mẫu số là không bao giờ rơi tiền. Đặt -1 để tắt.",
    2, {"dv": ""})
VN["DRQ_NGUONG_ITEM"] = (
    "Ngưỡng quay bảng rớt đồ của quái thường",
    "Số bốc được (1 đến DRQ_MAU_QUAI) nhỏ hơn số này (và không trúng nhánh tiền) thì quay bảng rớt đồ theo bậc. Mặc định 4 với mẫu 30: các số 1, 3 là rớt đồ (2 đã dành cho tiền), tức khoảng 2/31.\n%s\nCó hiệu lực trong vòng 1,5 phút." % _CACH_DOC,
    "Đây là nút tỉ lệ rớt đồ của toàn bộ quái thường: 4 lên 8 là tỉ lệ tăng gấp đôi trên cả server. Đặt 0 hoặc 1 là quái thường không rớt đồ. Đặt bằng mẫu số là quái nào cũng rớt.",
    2)
VN["DRQ_QUAY_QUAI"] = (
    "Số món quái thường rơi mỗi lần trúng",
    "Khi quái thường trúng nhánh rớt đồ, script quay bảng đúng số lần này. Mặc định 1 (một món mỗi lần trúng).\nCó hiệu lực trong vòng 1,5 phút.",
    "Đặt 2 là mỗi lần trúng rơi hai món, tương đương tăng gấp đôi sản lượng đồ toàn server. Đặt 0 là không rơi.",
    2)
VN["DRQ_MAU_SUKIEN"] = (
    "Mẫu số bốc nhánh rớt sự kiện",
    "Mẫu số của phép bốc quyết định có gọi nhánh rớt sự kiện (dropeventmap) hay không sau khi quái thường (từ bậc DRQ_CAP_SUKIEN) chết. Mặc định 4.\n%s\nCó hiệu lực trong vòng 1,5 phút." % _CACH_DOC,
    "Đặt 0 làm script lỗi. Đặt nhỏ hơn hoặc bằng ngưỡng sự kiện là mọi quái đủ bậc đều gọi nhánh sự kiện, đồ sự kiện ngập server.",
    2, {"dv": ""})
VN["DRQ_CAP_SUKIEN"] = (
    "Bậc quái tối thiểu để có rớt sự kiện",
    "Chỉ quái thuộc bậc rớt đồ từ số này trở lên (bậc tính theo cấp quái, 1 đến 10) mới có cơ hội gọi nhánh rớt sự kiện. Mặc định 7.\nCó hiệu lực trong vòng 1,5 phút.",
    "Hạ xuống 1 là quái tân thủ cũng rơi đồ sự kiện, bot cày bản đồ thấp thu đồ sự kiện hàng loạt. Khoảng hợp lý 1..10.",
    1, {"dv": "bac"})
VN["DRQ_NGUONG_SUKIEN"] = (
    "Ngưỡng gọi nhánh rớt sự kiện",
    "Số bốc được (1 đến DRQ_MAU_SUKIEN) nhỏ hơn số này thì gọi dropeventmap (rớt sự kiện trên bản đồ). Mặc định 2 với mẫu 4 (chú thích trong mã ghi khoảng 40%%). Đặt -1 là tắt hẳn nhánh sự kiện.\n%s\nCó hiệu lực trong vòng 1,5 phút." % _CACH_DOC,
    "Đặt lớn hơn mẫu số là 100% quái đủ bậc rơi đồ sự kiện. Đặt 0 hoặc 1 là không bao giờ. Đặt -1 để tắt hẳn khi hết sự kiện.",
    2)

# --- SKD_* : lib_sukien.lua -------------------------------------------------------------------
_MAU = {
    "SKD_RANDMAP": ("nhánh rớt sự kiện bản đồ, Hiệp Cốt Nhu Tình, Huy Hoàng Đơn, mảnh HKMP", "100"),
    "SKD_RANDPLD": ("nhánh Phong Lăng Độ (hàm thần rồng)", "100"),
    "SKD_RANDOTHER": ("nhánh boss sự kiện (dropeventboss) và Tiền Đồng (dropntiendong)", "10"),
    "SKD_RANDTK": ("nhánh Tống Kim (hàm thần rồng)", "100"),
    "SKD_RANDVA": ("nhánh Vượt ải (hàm thần rồng)", "100"),
    "SKD_DROPMDTB": ("nhánh nhiệm vụ dropnvdt / dropnvdt01", "95"),
}
for _k, (_nh, _md) in _MAU.items():
    VN[_k] = (
        "Mẫu số bốc của %s" % _nh.split(" (")[0].split(",")[0],
        "Mẫu số của phép bốc dùng cho %s: máy chủ bốc từ 1 đến số này rồi so với ngưỡng của từng nhánh. Mặc định %s.\n%s\n"
        "Chốt khi nạp lib_sukien.lua nên cần khởi động lại máy chủ." % (_nh, _md, _CACH_DOC),
        "Giảm mẫu số là tăng tỉ lệ của tất cả các nhánh dùng chung mẫu này. Đặt 0 làm script lỗi khi rớt đồ. Đặt nhỏ hơn ngưỡng là nhánh đó không bao giờ rơi. Cần khởi động lại máy chủ.",
        2)
VN["SKD_MAP_NGUONG"] = (
    "Ngưỡng rớt vật phẩm sự kiện bản đồ",
    "Số bốc (1 đến SKD_RANDMAP) phải lớn hơn ngưỡng này thì rơi vật phẩm SKD_MAP_ITEM. Mặc định 80 với mẫu 100 = 20%.\nCó hiệu lực trong vòng 1,5 phút.",
    "Đặt 0 là 100% rơi, đặt từ mẫu số trở lên là không bao giờ rơi. Đây là vật phẩm sự kiện, đặt thấp làm ngập server.",
    2, {"dv": ""})
VN["SKD_MAP_ITEM"] = (
    "Mã vật phẩm rớt sự kiện bản đồ",
    "Mã vật phẩm (id trong bảng vật phẩm) rơi ra khi trúng nhánh sự kiện bản đồ. Mặc định 4854.\nCó hiệu lực trong vòng 1,5 phút.",
    "Mã không tồn tại làm script lỗi (ScriptError) hoặc rơi vật phẩm sai. Chỉ đổi khi chắc chắn mã mới có trong bảng vật phẩm của cả máy chủ lẫn client.",
    2, {"dv": ""})
VN["SKD_HCNT_NGUONG"] = (
    "Ngưỡng rớt trang bị Hiệp Cốt Nhu Tình",
    "Số bốc (1 đến SKD_RANDMAP) phải lớn hơn ngưỡng này thì rơi một trang bị Hiệp Cốt Nhu Tình trong dải mã SKD_HCNT_MA_MIN..MAX. Mặc định 50 với mẫu 100 = 50%.\nCó hiệu lực trong vòng 1,5 phút.",
    "Đặt 0 là luôn rơi. Trang bị này có giá trị cao, hạ ngưỡng là phá giá.",
    2)
VN["SKD_HCNT_MA_MIN"] = (
    "Dải mã trang bị Hiệp Cốt Nhu Tình, mã đầu",
    "Mã vật phẩm nhỏ nhất của dải bốc ngẫu nhiên trang bị Hiệp Cốt Nhu Tình. Mặc định 185 (dải 185..192).\nCó hiệu lực trong vòng 1,5 phút.",
    "Mã đầu lớn hơn mã cuối hoặc trỏ vào vật phẩm không tồn tại làm rơi đồ sai hoặc lỗi script. Đổi cả cặp MIN/MAX cùng lúc.",
    2)
VN["SKD_HCNT_MA_MAX"] = (
    "Dải mã trang bị Hiệp Cốt Nhu Tình, mã cuối",
    "Mã vật phẩm lớn nhất của dải bốc ngẫu nhiên trang bị Hiệp Cốt Nhu Tình. Mặc định 192.\nCó hiệu lực trong vòng 1,5 phút.",
    "Phải lớn hơn hoặc bằng SKD_HCNT_MA_MIN và mọi mã trong dải phải là vật phẩm có thật.",
    2)
VN["SKD_PUBG_MA_MIN"] = (
    "Dải mã vật phẩm Sinh Tồn (PUBG), mã đầu",
    "Mã vật phẩm nhỏ nhất của dải thưởng hoạt động Sinh Tồn. Mặc định 4851 (dải 4851..4853). Chốt khi nạp script, cần khởi động lại máy chủ.",
    "Sai dải là thưởng Sinh Tồn rơi vật phẩm khác. Đổi cả cặp MIN/MAX và khởi động lại máy chủ.",
    2)
VN["SKD_PUBG_MA_MAX"] = (
    "Dải mã vật phẩm Sinh Tồn (PUBG), mã cuối",
    "Mã vật phẩm lớn nhất của dải thưởng hoạt động Sinh Tồn. Mặc định 4853. Cần khởi động lại máy chủ.",
    "Phải lớn hơn hoặc bằng SKD_PUBG_MA_MIN.",
    2)
VN["SKD_HKMP_MA_MIN"] = (
    "Dải mã trang bị Hoàng Kim Môn Phái rơi, mã đầu",
    "Mã nhỏ nhất của dải trang bị Hoàng Kim Môn Phái có thể rơi (bảng goldequip). Mặc định 0 (dải 0..139). Cần khởi động lại máy chủ.",
    "Sai dải là rơi trang bị HKMP không đúng bộ. Đổi cả cặp MIN/MAX.",
    2)
VN["SKD_HKMP_MA_MAX"] = (
    "Dải mã trang bị Hoàng Kim Môn Phái rơi, mã cuối",
    "Mã lớn nhất của dải trang bị Hoàng Kim Môn Phái có thể rơi. Mặc định 139. Cần khởi động lại máy chủ.",
    "Phải lớn hơn hoặc bằng SKD_HKMP_MA_MIN và không vượt quá số dòng thật của bảng goldequip.",
    2)
VN["SKD_HKMP_HAN_NGAY"] = (
    "Hạn dùng trang bị HKMP rơi ra",
    "Trang bị Hoàng Kim Môn Phái rơi từ sự kiện có hạn dùng bằng số ngày này rồi tự biến mất. Mặc định 7. Cần khởi động lại máy chủ.",
    "Đặt 0 có thể thành vĩnh viễn tuỳ hàm tạo vật phẩm, làm trang bị HKMP miễn phí tràn server. Giữ từ 1 ngày trở lên.",
    2)
VN["SKD_HMD_NGUONG"] = (
    "Ngưỡng rớt Huy Hoàng Đơn",
    "Số bốc (1 đến SKD_RANDMAP) phải lớn hơn ngưỡng này thì rơi Huy Hoàng Đơn. Mặc định 1 với mẫu 100 = 99%.\nCó hiệu lực trong vòng 1,5 phút.",
    "Đang gần như luôn rơi. Tăng ngưỡng để giảm tỉ lệ; đặt từ 100 trở lên là không bao giờ rơi.",
    2)
VN["SKD_HMD_SOLUONG"] = (
    "Số Huy Hoàng Đơn mỗi lần rơi",
    "Mỗi lần trúng, rơi ra đúng số Huy Hoàng Đơn này. Mặc định 10.\nCó hiệu lực trong vòng 1,5 phút.",
    "Nhân thẳng vào lượng Huy Hoàng Đơn đổ vào server. Đặt 0 là không rơi.",
    1, {"dv": "cai"})
VN["SKD_HMD_ITEM"] = (
    "Mã vật phẩm Huy Hoàng Đơn",
    "Mã vật phẩm của Huy Hoàng Đơn dùng trong nhánh rớt này. Mặc định 4844.\nCó hiệu lực trong vòng 1,5 phút.",
    "Chỉ đổi khi mã Huy Hoàng Đơn trong bảng vật phẩm thay đổi; mã sai làm rơi vật phẩm khác hoặc lỗi script.",
    2)
VN["SKD_MANHHKMP_NGUONG"] = (
    "Ngưỡng rớt mảnh đồ phổ HKMP",
    "Số bốc (1 đến SKD_RANDMAP) nhỏ hơn ngưỡng này thì rơi mảnh đồ phổ Hoàng Kim Môn Phái. Mặc định 80 với mẫu 100 = 79%. Chú ý chiều so sánh ngược với các ngưỡng khác.\nCó hiệu lực trong vòng 1,5 phút.",
    "Tăng ngưỡng là tăng tỉ lệ (ngược với SKD_HMD_NGUONG). Đặt 101 là luôn rơi, đặt 0 hoặc 1 là không bao giờ.",
    2)
VN["SKD_BOSS_NGUONG"] = (
    "Ngưỡng rớt trang bị boss sự kiện",
    "Số bốc (1 đến SKD_RANDOTHER) phải lớn hơn ngưỡng này thì boss sự kiện rơi một trang bị trong dải SKD_BOSS_MA_MIN..MAX. Mặc định 1 với mẫu 10 = 90%.\nCó hiệu lực trong vòng 1,5 phút.",
    "Đặt từ 10 trở lên (bằng mẫu) là boss sự kiện không rơi trang bị. Đặt 0 là luôn rơi.",
    2)
VN["SKD_BOSS_MA_MIN"] = (
    "Dải mã trang bị boss sự kiện, mã đầu",
    "Mã nhỏ nhất của dải trang bị boss sự kiện. Mặc định 753 (dải 753..770).\nCó hiệu lực trong vòng 1,5 phút.",
    "Sai dải là boss rơi vật phẩm không đúng bộ hoặc lỗi script. Đổi cả cặp MIN/MAX.",
    2)
VN["SKD_BOSS_MA_MAX"] = (
    "Dải mã trang bị boss sự kiện, mã cuối",
    "Mã lớn nhất của dải trang bị boss sự kiện. Mặc định 770.\nCó hiệu lực trong vòng 1,5 phút.",
    "Phải lớn hơn hoặc bằng SKD_BOSS_MA_MIN.",
    2)
VN["SKD_TIENDONG_NGUONG"] = (
    "Ngưỡng rớt Tiền Đồng",
    "Số bốc (1 đến SKD_RANDOTHER) phải lớn hơn ngưỡng này thì rơi Tiền Đồng. Mặc định 1 với mẫu 10 = 90%.\nCó hiệu lực trong vòng 1,5 phút.",
    "Đặt từ 10 trở lên là không rơi Tiền Đồng. Đặt 0 là luôn rơi.",
    2, {"dv": ""})
VN["SKD_TIENDONG_SL_MIN"] = (
    "Số Tiền Đồng mỗi lần rơi, ít nhất",
    "Mỗi lần trúng rơi ngẫu nhiên từ SL_MIN đến SL_MAX Tiền Đồng. Mặc định 1. Chốt khi nạp script, cần khởi động lại máy chủ.",
    "Phải nhỏ hơn hoặc bằng SKD_TIENDONG_SL_MAX, nếu không hàm random lỗi.",
    1, {"dv": "cai"})
VN["SKD_TIENDONG_SL_MAX"] = (
    "Số Tiền Đồng mỗi lần rơi, nhiều nhất",
    "Cận trên của số Tiền Đồng rơi mỗi lần. Mặc định 3. Cần khởi động lại máy chủ.",
    "Đặt cao là bơm Tiền Đồng vào server. Phải lớn hơn hoặc bằng SKD_TIENDONG_SL_MIN.",
    1, {"dv": "cai"})
VN["SKD_TIENDONG_ITEM"] = (
    "Mã vật phẩm Tiền Đồng",
    "Mã vật phẩm của Tiền Đồng dùng trong nhánh rớt này. Mặc định 4835.\nCó hiệu lực trong vòng 1,5 phút.",
    "Mã sai làm rơi vật phẩm khác hoặc lỗi script. Chỉ đổi khi bảng vật phẩm đổi.",
    2, {"dv": ""})
