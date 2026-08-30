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

## 🔴 2. Tống Kim đang chạy **cấu hình thử nghiệm** — người chơi chỉ có 1 phút báo danh

Đây là thứ ảnh hưởng **mỗi ngày, toàn máy chủ**, và tôi suýt bỏ sót.

`script\tinhnang\tong_kim_tcap\lib_tktc.lua` được lưu lúc **28/08 23:45:56**, và khung giờ đầu
tiên trong bảng lịch là **`{23,46,...}`** — tức đặt để nổ sau đúng **4 giây**. Đó là dấu vết của
một lần chỉnh để thử, rồi bị bỏ quên.

Ba con số thời gian đều lệch hẳn với chú thích nằm ngay cạnh chúng:

| Dòng | Đang chạy | Chú thích ngay cạnh ghi |
|---|---|---|
| `:56` `TIME_BD_TK` | **1 phút** báo danh | "10 phut báo danh" |
| `:57` `TIME_KT_TK` | **30 phút** cả trận | "70 phut chiến đấu" |
| `:58` `TIME_NS_TK` | **1 phút** đến lúc ra Nguyên Soái | "30 phut ... nguyên soái" |

Nghĩa là người chơi chỉ có **đúng một phút** để báo danh Tống Kim thay vì mười phút. Tống Kim là
hoạt động cày chính hằng ngày nên việc này chạm tới tất cả mọi người.

Bảng lịch hiện tại: `{23,46,19,1}` · `{17,50,18,3}` · `{20,50,21,4}` · `{22,50,23,5}` — ba khung
sau bình thường, riêng khung đầu là dấu vết test.

**Tôi không tự đổi** vì không biết số đúng: riêng khung giờ đầu đã từng có **ba giá trị khác nhau**
(13h23, 13h58, 23h46), và chú thích lại nói một đằng.

Mọi con số nay nằm ở `script\cauhinh\ch_lich.lua`, khoá `TK_`, **mặc định bằng đúng giá trị đang
chạy**. Anh chỉ cần sửa, ví dụ:

```
TK_PHUT_BAODANH = 10
TK_PHUT_TRAN    = 70
TK_LICH         = {{17,50,18,3}, {20,50,21,4}, {22,50,23,5}},
```

rồi khởi động lại.

---

## 🔴 3. Vận Tiêu trả gấp hơn 10 lần con số báo cho người chơi

Hoạt động này **đang mở**: `startgame.lua:220` gọi `addnpcvantieu()` và dòng đó **không bị
comment**. (Chỉ phần loa thông báo ở `timerserver.lua:128` bị tắt — nên rất dễ tưởng nhầm là hoạt
động đã đóng.)

> **Đính chính quan trọng, để anh khỏi lo quá mức:** bộ kiểm chứng soi nhật ký 410 ngày và
> **không thấy lượt lĩnh thưởng nào từ 17/05/2024**, dù người chơi vẫn mua Hộ Tiêu Lệnh đến tận
> 12/2025. Nhiều khả năng có một cổng chặn ở `tieusu.lua:36-40` (phải giết đủ 3 boss đúng thứ tự)
> khiến chưa ai đi hết được chuỗi. Nên đây là **mìn đã cài, ngòi còn nguyên** chứ chưa phải đang
> chảy máu — nhưng chỉ cần ai nới cổng boss đó mà không biết chuyện dòng 250 là nó nổ ngay hôm sau.

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

## ⚠️ Đính chính (30/08) — hai chỗ tôi báo sai

**1. Công Thành Quan: tôi báo "lỗi đang xảy ra", thực ra NPC đó không tồn tại.**

Hôm qua tôi báo `congthanhquan.lua:24` đặt ngoặc sai làm mất mục báo danh nửa tiếng mỗi giờ, và
gọi đó là lỗi đang xảy ra. Kiểm lại kỹ hơn: **cả ba nơi tạo NPC Công Thành Quan đều đã bị
comment** — `balanghuyen.lua:79` và `lib_ctc.lua:243-244`. Không có NPC nào chạy tệp đó, nên lỗi
đặt ngoặc **không ảnh hưởng ai**. Bản vá vẫn giữ (nó đúng về logic, sẽ có tác dụng nếu anh bật lại
NPC), nhưng nó **không gấp** như tôi nói.

Kéo theo: cảnh báo "Lôi Đài Bang Hội thu 1.000.000 lượng" cũng **không còn đường sống** — đường thu
tiền duy nhất đi qua chính NPC đó.

**2. Tôi vá sai một chỗ và đã hoàn tác.**

Tôi đổi biến `count` thành số `1` trong `citywar_function.lua`, kết luận rằng "biến không được khai
ở đâu". Sai — hàm chứa nó khai là `function take_tong_award(count)`, tức **`count` là tham số của
hàm**, hoàn toàn hợp lệ. Tôi đã không đọc dòng khai hàm trước khi kết luận. Đã hoàn tác.

Chỗ thứ hai thì kết luận đúng và giữ nguyên bản vá: hàm `take_tong_resaward()` **không có tham số
nào**, nên `count` ở đó thật sự chưa khai.

---

## Các cảnh báo khác — đã kiểm chứng, không cần gấp

Tôi cho 8 nhóm kiểm chứng độc lập 10 cảnh báo. Kết quả: **1 cái đang ngủ, 7 cái đúng một phần**
(số dòng sai hoặc hậu quả nhẹ hơn mô tả). Hai cái nặng là hai mục trên.

| Cảnh báo | Kết luận | Ghi chú |
|---|---|---|
| **NPC Lôi Đài Hỗn Chiến nhốt người chơi** | **đang xảy ra** | Người cấp ≥90 bấm vào NPC ở Ba Lăng Huyện lúc 16h/22h bị **ném vào bản đồ 210 trống**, chỉ thoát được bằng cách thoát game đăng nhập lại. Không mất đồ, nhưng là lỗi khó chịu |
| Lôi Đài Hỗn Chiến — sáu điểm hở exp | **đang ngủ** | Xem mục riêng ngay dưới. **Phải xử lý trước khi bật** |
| Lịch Tống Kim Lua ≠ C++ (23:46 vs 13:58) | đúng | Bảng cho auto/bot đã cũ so với script sống — người dùng auto chờ nhầm giờ |
| Lôi Đài Bang Hội thu 1 triệu lượng cho trận không mở | đúng một phần | Đường vào NPC hẹp hơn cảnh báo mô tả |
| Trần bù lượt Vượt Ải lệch cấu hình | đúng | Số nhỏ, sửa lúc nào cũng được |
| Hai tệp trùng tên biến toàn cục | **cảnh báo SAI** | Ba con số đúng, nhưng lý do sai: mỗi tệp Lua có vùng riêng nên **không thể giẫm nhau**. Hơn nữa cả hai tệp đều không còn NPC nào gọi |
| Trần bù lượt Vượt Ải "lệch cấu hình" | **cảnh báo SAI** | Số 2 là đúng — nó đếm hệ Vượt Ải **cũ** (2 lượt/ngày); số 1 thuộc một hệ Vượt Ải **khác hoàn toàn**. **Đừng sửa theo cảnh báo này** |
| Cửa sổ chốt quán quân sai dải phút | đúng một phần | Chỉ nổ khi bật lại hoạt động |
| `MSTIME_VUOT_AI_BD` dùng hai vai | đang ngủ | Hàm vượt ải cũ đã tắt |

---

## Lôi Đài Hỗn Chiến — sáu điểm hở, phải xử lý trước khi bật

Hoạt động này **đang tắt**, nên chưa mất gì. Nhưng nếu anh bật lên mà chưa xử lý, đây là những gì
sẽ xảy ra. Em tự đọc `timerserver.lua` để xác nhận từng điểm:

| # | Điểm hở | Vị trí |
|---|---|---|
| 1 | **Chỉ cần có mặt là được 50 triệu exp + 100 Hộ Mạch Đơn.** Lúc phút 52, mọi người đang đứng ở bản đồ 210 đều được cộng — không cần đánh ai | `:481-482` |
| 2 | **Một người báo danh đơn là thành quán quân**, ăn 500 triệu exp + 5 mảnh Hoàng Kim. Điều kiện thắng là `count == 1`, không đòi hỏi chiến đấu | `:511`, `:535-539` |
| 3 | **Cửa sổ chốt quán quân mở trước khai chiến 40 phút** — từ phút 12 trong khi trận bắt đầu phút 52 | `:497` vs `:469` |
| 4 | **Trần 4 mạng bị xoá mỗi lần khai chiến** bởi `SetTaskTemp(1,0)` | `:479` |
| 5 | **Chết không do người chơi thì exp rơi vào chính người vừa chết** | `bigiet.lua:6-8` |
| 6 | **"Tổng dame" dùng để xếp hạng là số giả** — không nơi nào ghi giá trị đó, nên mọi người đều bằng 0 và quán quân là người đầu bảng ngẫu nhiên | `:579`, `:583` |

Bốn con số (50 triệu, 100 Hộ Mạch Đơn, 500 triệu, 5 mảnh) nay nằm ở `ch_thuong.lua` khoá `LDHC_`,
giữ nguyên giá trị. Nhưng **sáu điểm trên là lỗi thiết kế chứ không phải con số** — sửa chúng là
quyết định của anh, em không tự làm.

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
