# PHÂN TÍCH 07/09 rạng sáng — "Mất đồ" CaiBang: gốc thật khác bàn giao 06/09 đêm

Đọc lại `BANGIAO_MATDO_CAIBANG_0609.md` + đo lại `role_history`, `bot.log`, `hethong.log` và mã. **Chỉ phân tích, chưa sửa gì.**

## 1. Kết luận

Bàn giao trước chẩn đúng triệu chứng (bản lưu có nhiều bản ghi "đang mặc" cùng một ô) nhưng **sai cơ chế**. Các bản ghi dư không phải hai món thật bị kéo chồng ô. Chúng là **"bản ghi ma"**: entry trong `m_Items` của CaiBang trỏ vào chỉ số `Item[]` đã bị giải phóng rồi cấp lại cho món của bot. Rớt bản ghi ma khi vào lại không phải mất đồ. Đồ thật mất **trước đó**, lúc chỉ số bị giải phóng.

Gốc thật nằm ở `KItemList::InsertEquipment` ([KItemList.cpp:4677](Sources/Core/Src/KItemList.cpp:4677)): khi túi đầy, lấy `nIndex = m_Hand`, gọi `Remove(nIndex)` **không xét kết quả**, rồi `ObjSet.Add(...)` với `m_nItemID = nIndex`. `m_Hand` cũ của bot trỏ vào chỉ số đã được cấp cho người chơi → vật rơi trỏ thẳng vào item của người chơi. Vật rơi hết hạn → `KObj::Release` ([KObj.cpp:75](Sources/Core/Src/KObj.cpp:75)) `ItemSet.Remove` = huỷ item của chủ thật. Ai nhặt → `AddKIL(Object.m_nItemDataID)` ([KPlayer.cpp:5076](Sources/Core/Src/KPlayer.cpp:5076)) = hai danh sách chung một item. Đây chính là lỗi bot "ném đồ" mà đợt 4 bot nội (`80079cc1`) đã né bằng cách không cho bot gọi `InsertEquipment` nữa; hàm gốc vẫn còn nguyên.

## 2. Bằng chứng

### 2.1 Dữ liệu `role_history` (đếm theo vị trí)

| Giờ | id | đang mặc | rương | túi | ghi chú |
|---|---|---|---|---|---|
| 19:03:00 | 693958 | 14 | 31 | 35 | bình thường, 14 ô đúng loại |
| 19:09:31 | 694957 | 14 | 29 | 39 | ô 5 = MŨ (dt7), ô 10 = GIÁP (dt2); bao tay + ngựa thật biến mất |
| 19:10:31 | 694958 | 14 | 18 | 38 | ô 4 = 2 BÙA (genre 6), ô 10 = bản ghi toàn 0; giày thật biến mất |
| 19:12:31 | 694959 | 14 | 10 | 38 | ô 4 = mũ, ô 5 = vũ khí |
| 20:45:49 | 694963 | 17 | 2 | 27 | 14 thật + ma: giáp@5, mũ@4, rỗng@10, vũ khí@13, rỗng@7 |
| 20:51:19 | 695964 | 15 | 2 | 33 | vào lại → 3 ma trùng ô bị bỏ |

Món thật mất trong 19:09→19:33: bao tay (ô 5), ngựa (ô 10), giày (ô 4), nhẫn (ô 7), ấn (ô 13), 29 món rương, vài món túi. Sau 20:45 quét toàn bộ (`matdo_quet.py CaiBang 1`): chỉ còn rớt bản ghi ma khi vào lại và auto bán rác ở hàng (x,0) như cả ngày. **Không còn mất thật.**

### 2.2 Chứng minh chung item với bot

Bản ghi ma "vũ khí dt0 cấp 7 hệ 4, seed 108862496" ở ô 13 của CaiBang (20:45:49) có **đúng seed** với món trong túi bot `CaoLong452` (role_history id 695415, 20:47:46, túi (0,6)). Seed sinh ngẫu nhiên từng món, không thể trùng tự nhiên.

Tra chéo (chạy ~130 s cho một ngày):

```python
import struct, pymysql
seed = 108862496
c = pymysql.connect(host='127.0.0.1', port=3306, user='root', password='123456', database='jx1_role')
cur = c.cursor()
cur.execute("SELECT id, role_name, saved_at FROM role_history WHERE saved_at BETWEEN %s AND %s "
            "AND LOCATE(UNHEX(%s), role_blob) > 0",
            ('2026-09-06 15:00:00', '2026-09-07 00:00:00', struct.pack('<i', seed).hex()))
print(cur.fetchall())
```

### 2.3 Dòng thời gian (hethong.log dòng `[MAIL] X dang nhap` = giờ đăng nhập; bot.log)

| Giờ | Sự kiện |
|---|---|
| 19:01 | máy chủ khởi động lại với bot đợt 3 `4424915b`; `[BotAuto] 0/1000 bot → sinh lại` |
| 19:01:30 | CaiBang đăng nhập (item cấp chỉ số mới) |
| 19:03:35 | `[BotMach] nâng mạch ... 1000/1000 bot đổi` |
| 19:04:01 | `[BotNoi] ... tháo vũ khí (đường NỘI CÔNG)` |
| 19:04:10 → 19:08 | **1.532 dòng** `[BotVuKhi] ... phát lại vũ khí nhập môn (lần 1..5)` = 1.532 lần `InsertEquipment` ném `Item[m_Hand]` |
| 19:05:31, 19:14:36 | CaiBang vào lại (chỉ số mới = chỉ số vừa được giải phóng) |
| 19:09 → 19:33 | CaiBang mất đồ từng đợt |
| 19:33 | tắt máy chủ swap đợt 4 `80079cc1` (bỏ InsertEquipment cho bot); boot 20:45 |
| 20:45 → | không còn mất thật |

Thứ tự bản lưu đảo ngược sau mỗi lần đăng nhập vì `KLinkArray::Insert` chèn đầu ([KLinkArray.cpp:44](Sources/Engine/Src/KLinkArray.cpp:44)); nhìn nhóm "đang mặc" nằm đầu hay cuối blob biết được có relogin.

## 3. Vì sao cơ chế "Equip đè ô" không phải gốc

Đường Src ≠ Des trong `ExchangeItem` nhánh `pos_equip` ([KItemList.cpp:2677](Sources/Core/Src/KItemList.cpp:2677)) chỉ chạy khi client gửi hai ô khác nhau. Giao diện thật luôn gán `Drop.Region.v = Pick.Region.v` cùng một ô ([UiEquip.cpp:329](Sources/S3Client/Ui/UiCase/UiEquip.cpp:329)); auto (`KPlayerAuto.cpp`, `KPlayer.cpp`) luôn gọi `MoveItem(P, P)`. Bốn nơi gọi `Equip` còn lại (`AddKIL`, `ExchangeItem` cùng ô, `SwitchEquipSet`, bot `pb_MacVaoNguoi`) đều đã dọn ô đích trước. Các bản ghi ma còn là bùa (genre 6) "đang mặc" và bản ghi toàn 0, thứ mà `Equip` không bao giờ tạo ra được.

## 4. Đánh giá bản vá `e06a12b6` (đã lên máy: `CoreServer.dll` b43c85e8 23:15, GameServer chạy 23:21)

- **H1, H2** (Equip gỡ món ở ô đích, xoá `m_Hand`): vô hại, coi như gia cố chống gói tin lạ. Không chạm gốc.
- **H3, H4** (`AddKIL` trùng ô → tìm chỗ khác thay vì bỏ): **rủi ro nhân bản đồ**. Một item có hai entry thật (lưới rách, đã ghi nhận ở [KPlayer.cpp:11178](Sources/Core/Src/KPlayer.cpp:11178) `[XEPDO 28/08]`) hoặc bản ghi ma trước đây bị bỏ lúc nạp; nay sinh thành hai món thật. Với trường hợp CaiBang, H3 sẽ đưa giáp/mũ/vũ khí của bot vào túi CaiBang thay vì bỏ.

## 5. Đề xuất (chưa làm)

1. **Chặn gốc ở `InsertEquipment`**: chỉ ném khi `Remove(nIndex)` trả TRUE; nếu không thì `m_Hand = 0` và không `ObjSet.Add`. Bảo vệ cả người chơi (script phát thưởng lúc túi đầy cũng đi qua đây).
2. **H3/H4**: chỉ cứu khi trong danh sách chưa có món cùng (genre, dt, pt, cấp, hệ, seed, goldid); trùng thì bỏ và `g_DebugLog` (hết "mất im lặng"). Hoặc lùi H3/H4.
3. **Khôi phục CaiBang** từ role_history id **693958** (19:03:00, trước bão: 14 mặc + 31 rương + 35 túi). Phải tắt nhân vật, mất đồ nhặt sau mốc đó. Quyết định của chủ.
4. Đợt bot kế tiếp: mọi chỗ bot bỏ/đổi đồ chỉ dùng `RemoveItemIdx` (có `FindSame`), không bao giờ `ObjSet.Add`/`ItemSet.Remove` với chỉ số chưa xác minh sở hữu.
