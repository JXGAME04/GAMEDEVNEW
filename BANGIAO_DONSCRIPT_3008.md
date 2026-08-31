# BÀN GIAO 30/08 (đợt 3) — DỌN SCRIPT KHÔNG DÙNG ĐẾN

Tiếp [BANGIAO_GO_HECU_3008.md](BANGIAO_GO_HECU_3008.md) và
[BANGIAO_5VIEC_3008.md](BANGIAO_5VIEC_3008.md). Đây là nửa còn lại của yêu cầu
gốc: *"sắp xếp lại script rồi bỏ những tính năng đang không dùng đến"*.

---

## 1. Kết quả đo — và vì sao KHÔNG nên quét dọn hàng loạt

Tôi định quét toàn cây tìm tệp mồ côi rồi dọn một lượt. **Không làm được an
toàn**, và đây là lý do — đáng ghi lại vì lần sau ai làm cũng sẽ vấp:

Một tệp `.lua` có thể được nạp bằng **ít nhất 8 đường**:

| | Đường nạp | Grep thấy? |
|---|---|---|
| a | `Include` / `dofile` / `ReLoadScript` / `DynamicExecute` | ✅ |
| b | Gắn NPC: `AddNpc` / `AddNpcNew` / `AddNpcEx1` | ✅ |
| c | `SetDeathScript` / `AddTrapEx1` / `OpenGiveBox` / `SetNpcDropScript` | ✅ |
| d | Bảng `settings\`: `TimerTask.txt`, `magicscript.txt`, `npc\`, `task\` | ✅ |
| e | **Quy ước tên theo số hiệu**: `missionNN.lua`, `taskNN.lua` | ❌ |
| f | C++ gọi cứng đường dẫn | ✅ |
| g | Bảng chuỗi trong C++ header (`KHoatDongTables.h`, `KDaTauTables.h`) | ✅ |
| h | **Dữ liệu bản đồ trong `bin\server\Pak\*.pak`** | ❌ |

Đường **(h)** là chỗ tôi suýt sai nặng. Lần quét đầu tôi báo **1.675/3.065 tệp
mồ côi** — con số vô lý, và tôi dừng lại đúng lúc thay vì dọn theo nó.

Đào ra thì: NPC và bẫy đặt sẵn trong dữ liệu bản đồ, **đường dẫn script của
chúng nằm trong khối đã nén** — grep văn bản không bao giờ thấy. Trích được
**2.864 tên script** từ đó, con số mồ côi tụt xuống 1.205.

Còn một bẫy nữa trong chính công cụ đọc pak: `ReverseTools\pak_vltk\pakdump.py`
chỉ biết **zlib**, mà pak này nén bằng **UCL NRV2B**. Khi zlib thất bại nó trả
về **nguyên khối đã nén**, và tôi trích ra 703 cái tên rác kiểu
`-=8a¶ô»ü°.lua`. Phải gọi `ucl.nrv2b_decompress_8` mới ra tên thật.

**Kết luận:** con số 1.205 vẫn không đủ tin để xoá hàng loạt (còn quy ước theo
số hiệu, và những tệp chỉ được nhắc ở dòng đã comment). Cách đúng là **kiểm từng
ứng viên có tên tuổi** — đọc thật xem nó là tính năng gì — như đã làm cho 7 hệ
cũ sáng nay.

---

## 2. Đã dọn — 32 tệp / 1.763 dòng, mỗi cái kiểm đủ 8 đường

| Mục | Quy mô | Là gì, vì sao chết |
|---|---|---|
| `header\loidai.lua` | 161 dòng | Hệ **"Lôi Đài CBT"** cũ — khác Lôi Đài Hỗn Chiến và khác Lôi Đài bang hội. Dòng 9 trỏ `\script\feature\loidaicbt\xaphu.lua` mà **thư mục `script\feature` không tồn tại**. Bản sao sống của `BW_COMPETEMAP` đang dùng nằm ở `missions\bw\bwhead.lua:9`. |
| `startgame\khuvucbang\` | 17 tệp, 834 dòng | Lãnh địa / khu vực bang hội **bản cũ** (kèm `bossbanghoi1-5.lua`). Bản thay: `scriptjx2\tong_vn` (JX2) + `startgame\tongjx2npc.lua`. |
| `event\demhuyhoang\` | 3 tệp, 208 dòng | Đêm Huy Hoàng **bản 1** |
| `event\event_demhuyhoang\` | 6 tệp, 248 dòng | Đêm Huy Hoàng **bản 2** — bản thứ ba mới là bản đang chạy: `event\event_huyhoang_dungdb` (`timerserver.lua:26`) |
| `global\thanh\npc\add_npc.lua` | 244 dòng | Bảng `tbNpcSatThu` — Boss Sát Thủ "nửa port" map 995. Hai hàm `add_npc_thanh` / `add_boss` **đo được: 0 nơi gọi**. |
| `item\ib\{hoangkim,bachngan,thanhdong,hanthiet}.lua` | 4 tệp, 68 dòng | Thẻ **Liên Đấu cũ**. Bảng vật phẩm có 13 dòng trỏ vào `item\ib\` nhưng **không dòng nào trỏ 4 tệp này** — đợt port WLLS 20/08 đã đổi sang `leaguematch\item\honour.lua`. |

Mỗi mục: **0** đường dẫn trong script/settings, **0** chỗ sống trong C++ (những
chỗ C++ bắt được đều là comment và khớp nhầm chuỗi con), **0** tên trong dữ liệu
bản đồ.

**Nghiệm thu:** 1.636/1.636 tệp tên ASCII đạt cú pháp Lua 4.0.1; 1.398 tệp tên
tiếng Trung cân bằng từ khoá đúng; **0 lời gọi hàm nil**; 2 tham chiếu treo còn
lại là cố ý (danh sách tên NPC cần xoá trong `hd3_driver.lua:49-50`).

---

## 3. Tự kiểm lại bản vá C++ rủi ro nhất

Bản vá `bGlobal = false` ở `KPlayer.cpp:4993/4995` chỉ đúng **nếu** hàm `PickUp`
không cần ngữ cảnh hội thoại. Đã đọc cả hai tệp:

- `item\tasklink_goods.lua:84-87` và `item\tasklink_goods_secret.lua:85-88`:
  `PickUp` chỉ gọi `TLG_ChiaToDoi()` → `AddMapValues()` → `nt_getTask` /
  `nt_setTask` / `Msg2Player`.
- **Không có** `Say`, `SayEx`, `OpenGiveBox`, không mở menu nào.

Nên bản vá không lấy đi thứ gì. Thêm một dấu hiệu tốt: `main()` của chính hai
tệp đó cũng **trừ vật phẩm trước rồi mới chia thưởng**, kèm chú thích rằng đợt
phản biện 17/08 đã bắt được exploit khi trừ nằm sau — đúng khuôn tôi vừa áp cho
Long Huyết Hoàn.

---

## 4. Chưa động vào — chờ anh quyết

1. **`storm_clear()`** (`event\storm\function.lua:392-400`) — gọi không tham số
   là xoá trắng task 1661→5199. Không nơi nào gọi, nhưng nạp sẵn trong 4 trạng
   thái Lua. Vá bằng cách bắt nó đòi tham số xác nhận: 3 dòng.
2. **Bảng "bù hoạt động"** `lib_ham.lua:296-305` — 5/6 bộ đếm chết. Nối lại là
   mở một dòng phần thưởng mới nên để anh quyết.
3. **~1.200 tệp** còn trong diện nghi ngờ. Muốn dọn tiếp thì phải làm từng cụm
   có tên tuổi như mục 2, không quét ào. Tôi làm tiếp được nếu anh muốn.
4. **Sắp xếp lại cấu trúc thư mục** — tôi cố ý **chưa đụng**. Đường dẫn ghi cứng
   ở khắp nơi kể cả trong C++ và trong dữ liệu bản đồ đã nén; đổi chỗ tệp là
   loại thay đổi dễ hỏng nhất trong cây này, và lợi ích chỉ là dễ đọc.

---

## 5. Hoàn tác

`doi_tep.hoan_tac("<đường dẫn>", ghi=True)`, hoặc chép ngược từ
`bin\server\_dara\script\<đường dẫn cũ>`. Nhật ký: `_dara\NHATKY_DOI.txt`.

**Tổng đã đưa ra khỏi cây trong cả ngày: 74 tệp.**

## 6. Công cụ mới của đợt này

`t57_quet_mocoi_toancay.py` (quét 8 đường nạp) ·
`t58_duong_nap_thu8_mapdata.py` (giải nén pak bằng UCL, trích tên script trong
dữ liệu bản đồ) · `t59_kiem_ungvien.py` (kiểm kỹ từng ứng viên) ·
`t60_doi_ungvien_chet.py` (dời).
