# BÁO CÁO LỖ HỔNG KINH TẾ (29/08/2026)

> Tài liệu này chỉ nói về **những thứ đang mở trên máy chủ đang chạy**. Mọi điều dưới đây tôi đã
> tự mở tệp đọc tận dòng để kiểm chứng, không nghe lại của ai.
>
> **Tôi chưa tự sửa con số nào** — cả hai việc dưới đây đều là quyết định của chủ, không phải lỗi
> kỹ thuật thuần. Nhưng tôi đã đưa mọi con số ra tệp cấu hình, nên chủ đổi **một dòng** là xong,
> không cần chờ tôi.

---

## 🔴 1. Hai NPC phát đồ GM đứng giữa làng tân thủ

**Ai cũng bấm được. Không kiểm tài khoản GM. Không giới hạn số lần.**

Ở làng xuất phát Ba Lăng Huyện (map 53) có hai NPC tên **"Hỗ Trợ Test"** và **"Hỗ Trợ Test Game"**,
đứng cách nhau vài bước — toạ độ (1632, 3179) và (1619, 3176).

Mỗi lần bấm:

| Thứ | Số lượng | Nguồn |
|---|---|---|
| Lượng | **1.000.000.000** | `hotrotest.lua:400` — `Earn(1000000000)` |
| Xu | **100.000** | `hotrotest.lua:249, 360` |
| Kinh nghiệm | **1.000.000.000** | `hotrotest.lua:394-395` |
| Cấp độ | lên thẳng **200** | `hotrotest.lua:404` |
| Đồ Hoàng Kim, ngựa | có | menu trang 2 |

Tôi đã đọc hết **451 dòng** `hotrotest.lua`: **không một lần nào gọi `admincheck()`**, và **không
có bộ đếm ngày** ở bất kỳ đường phát đồ nào. Nghĩa là bấm bao nhiêu lần thì nhân bấy nhiêu lần.

Đối chiếu: NPC tân thủ thật (`hotrotanthu.lua`) **có** chốt phát một lần và có khoá vật phẩm.

### Vì sao nó mở

`script\startgame.lua:69`:

```lua
if(SERVER_TEST == 1) then
    -- gắn hotrotest.lua  ("Hỗ Trợ Test")      <-- ĐANG CHẠY NHÁNH NÀY
else
    -- gắn hotrotanthu.lua ("Hỗ Trợ Tân Thủ")  <-- nhánh đúng cho máy chủ thật
end
```

Và `script\lib\lib_server.lua:5` đang đặt `SERVER_TEST = 1`.

### Cách tắt

Đổi một số trong `script\lib\lib_server.lua:5` thành `0` rồi khởi động lại. Tôi đã kiểm: tắt đi
**không làm hỏng gì** — nhánh `else` gắn đúng NPC tân thủ thật.

**Tôi không tự tắt** vì rất có thể chủ đang dùng chính hai NPC này để kiểm thử những thứ chúng ta
vừa làm. Chủ quyết.

---

## 🔴 2. Vận Tiêu trả gấp hơn 10 lần con số báo cho người chơi

Hoạt động này **đang mở**: `startgame.lua:220` gọi `addnpcvantieu()` và dòng đó **không bị
comment**. (Chỉ phần loa thông báo ở `timerserver.lua:128` bị tắt — nên rất dễ tưởng nhầm là hoạt
động đã đóng.)

Đọc `script\event\event_vantieu\tieudau.lua:246-280`:

```lua
for k=1,1000 do AddOwnExp(50000) end     --    50.000.000
AddSumExp(500000000)                     --   500.000.000
Msg2Player("... nhận được 50.000.000 kinh nghiệm")   -- báo 50 triệu
```

Phần nền đã là **550 triệu** trong khi báo 50 triệu. Cộng thêm theo loại tiêu:

| Loại tiêu | Cộng thật | Thông báo cho người chơi | Gấp |
|---|---|---|---|
| Tiêu đồng | **650.000.000** | 60.000.000 | 10,8× |
| Tiêu bạc | **850.000.000** | 80.000.000 | 10,6× |
| Tiêu vàng | **1.050.000.000** | 100.000.000 | 10,5× |

Kèm theo **mỗi chuyến**: 300–500 Hộ Mạch Đơn, 50–100 Chân Nguyên Đơn, 2–10 rương trang bị xanh,
10 Lệnh Bài Boss.

Trần 20 lượt/ngày ⇒ **tối đa 21 tỷ kinh nghiệm một người một ngày**.

### Điểm đáng lo hơn con số

`AddSumExp` đi qua `DirectAddExp`, **không chịu bất kỳ hệ số nào** — không phạt chênh cấp, không
bùa kinh nghiệm, không `ExpRate`, không nhóm `[Exp]` mà chúng ta vừa làm. Nghĩa là chủ chỉnh tỉ lệ
kinh nghiệm bao nhiêu cũng **không chạm được** vào cục exp này; nó đứng ngoài mọi cần gạt điều
tiết.

Bốn con số `500tr / 100tr / 300tr / 500tr` trông đúng như **bị thừa một số 0** so với thông báo —
nhưng đó là suy đoán của tôi, chủ mới là người biết ý định ban đầu.

### Cách chỉnh

Mọi con số nay nằm ở `script\cauhinh\ch_thuong.lua`, khoá bắt đầu bằng `VT_`. **Giá trị mặc định
đang bằng đúng số cũ**, nên bản vá tự nó không đổi gì. Muốn đưa về đúng thông báo thì sửa:

```
VT_EXP_NEN_SUM = 50000000     (thay vì 500000000)
VT_EXP_DONG    = 10000000     (thay vì 100000000)
VT_EXP_BAC     = 30000000     (thay vì 300000000)
VT_EXP_VANG    = 50000000     (thay vì 500000000)
```

rồi khởi động lại. Nếu chủ muốn giữ nguyên mức hiện tại thì **không phải làm gì**.

---

## Các cảnh báo khác — đã kiểm chứng, không cần gấp

Tôi cho 8 nhóm kiểm chứng độc lập 10 cảnh báo. Kết quả: **1 cái đang ngủ, 7 cái đúng một phần**
(số dòng sai hoặc hậu quả nhẹ hơn mô tả). Hai cái nặng là hai mục trên.

| Cảnh báo | Kết luận | Ghi chú |
|---|---|---|
| Lôi Đài Hỗn Chiến ăn 10 triệu exp/mạng | **đang ngủ** | NPC báo danh còn sống và vẫn ném người chơi vào map 210, nhưng hoạt động chưa mở. **Phải vá trước khi bật** |
| Lịch Tống Kim Lua ≠ C++ (23:46 vs 13:58) | đúng | Bảng cho auto/bot đã cũ so với script sống — người dùng auto chờ nhầm giờ |
| Lôi Đài Bang Hội thu 1 triệu lượng cho trận không mở | đúng một phần | Đường vào NPC hẹp hơn cảnh báo mô tả |
| Trần bù lượt Vượt Ải lệch cấu hình | đúng | Số nhỏ, sửa lúc nào cũng được |
| Hai tệp trùng tên biến toàn cục | đúng số, sai lý do | Mỗi tệp Lua là một vùng riêng nên không giẫm nhau như lo ngại |
| Cửa sổ chốt quán quân sai dải phút | đúng một phần | Chỉ nổ khi bật lại hoạt động |
| `MSTIME_VUOT_AI_BD` dùng hai vai | đang ngủ | Hàm vượt ải cũ đã tắt |

---

## Việc đã làm kèm trong đợt này

Ngoài hai lỗ hổng trên, tôi đã nối **83 khoá cấu hình** nữa, tất cả **giữ nguyên giá trị đang
chạy**:

| Nhóm | Số khoá | Nơi chỉnh | Đáng chú ý |
|---|---|---|---|
| Bật/tắt hoạt động | 20 | `ch_lich.lua` | Trước đây phải sửa mã nguồn mới bật/tắt được |
| Rơi đồ sự kiện | 27 | `ch_drop.lua` | Có chỗ đang là **99%** rơi 10 món, **79%** rơi mảnh đồ phổ |
| Rơi đồ quái thường | 13 | `ch_drop.lua` | Chi phối mọi bản đồ |
| Trần exp bảo rương | 4 | `ch_exp.lua` | Trần ẩn 50/80/100 triệu mỗi ngày |
| Thưởng Vận Tiêu | 19 | `ch_thuong.lua` | Mục 2 ở trên |

Và **vá một lỗi thật**: `Droprate_normal.lua:115` gọi `DropNpcMoney(NpcIndex, ...)` — biến thiếu
một chữ `n`, không được gán ở đâu trong cả cây script. Máy nhận số 0 nên lấy vị trí của phần tử
đầu mảng NPC thay vì con quái vừa chết, khiến tiền rơi ở chỗ khác hẳn và người chơi không bao giờ
nhặt được. Vá xong quái thường bắt đầu rơi tiền đúng chỗ (khoảng 3,2% mỗi lần chết) — muốn tắt thì
đặt `DRQ_NGUONG_TIEN = -1`.
