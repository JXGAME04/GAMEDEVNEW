# BÀN GIAO 30/08 — TỔNG KẾT, MỌI VIỆC ĐÃ XONG

Ba tệp chi tiết: [GO_HECU](BANGIAO_GO_HECU_3008.md) ·
[5VIEC](BANGIAO_5VIEC_3008.md) · [DONSCRIPT](BANGIAO_DONSCRIPT_3008.md).
Tệp này là bản chốt cuối ngày.

---

## 1. Đã làm xong

| # | Việc | Kết quả |
|---|---|---|
| 1 | Bỏ tính năng cũ trùng bản port Linux/JX2 | 7 hệ, 35 tệp / 4.921 dòng |
| 2 | Lôi Đài Hỗn Chiến — bỏ hẳn | 216 dòng + 3 tệp + lối thoát cho người kẹt |
| 3 | Hai rương hậu phương 222/223 | **để nguyên** theo lệnh |
| 4 | Long Huyết Hoàn | port từ Linux, đã dùng được; sửa thêm 6 lỗi phản biện bắt |
| 5 | Rương Dã Tẩu 1341 | truy ra là dòng thừa, **để nguyên** (bảng tra theo chỉ số dòng) |
| 6 | Dọn triệt để Dã Tẩu cũ | chặng A xong; chặng B đã vá C++ + dựng DLL |
| 7 | Dọn script không dùng | 117 tệp nữa, mỗi cụm kiểm đủ 8 đường nạp |
| 8 | Mìn `storm_clear()` | **đã chặn** |
| 9 | Bảng "bù hoạt động" | **đã tắt 5 dòng chết** |

**Tổng: 186 tệp / 10.807 dòng** đưa ra khỏi cây, **127 mục nhật ký hoàn tác**.
Cây còn **2.948 tệp `.lua`**.

### Bốn lỗi ĐANG XẢY RA đã vá

1. **Ba bẫy Công Thành cũ** trên bản đồ 221 búng người chơi lùi giữa trận Công
   Thành JX2 (`lib_ctc.lua:278-280` → `chancong_1/2/3.lua` đọc mission 2 đã chết).
2. **Nhặt cuộn Dã Tẩu cướp menu người chơi** — `KPlayer.cpp:4993/4995` để
   `bGlobal` mặc định, ghi đè `m_ActionScriptID` (`:7129`) là biến điều phối nút
   bấm (`:7621`/`:7626`). Log có 25 lần, mới nhất 30/08 11:10.
3. **Long Huyết Hoàn vô dụng** — chạy trên bộ đếm của hệ Vượt Ải đã gỡ.
4. **Ba công tắc không điều khiển gì**: `GLB_MANH_BOSS_SATTHU`,
   `BDH_THOIHAN_DANHHIEU_NGAY` (bị `partner_reward2.lua:15` ghi đè), và nhóm
   `BAT_*_VIET`.

### `storm_clear()` — đã chặn

`event\storm\function.lua:392`. Trước: gọi thiếu tham số là **xoá trắng task
1661→trần** của nhân vật (nuốt cả 2641/2642/4018 Long Huyết Hoàn và tasklink Dã
Tẩu). Nay phải gọi rõ `storm_clear(-1)`; gọi thiếu thì báo và thoát. Nhánh theo
`gameid` giữ nguyên.

### Bảng "bù hoạt động" — đã tắt 5 dòng chết

`lib_ham.lua:296`. Trong 6 dòng, **5 dòng đếm bằng task không ai ghi** (PLD 315,
VA 43, Hằng Ngày 345, Vip 377, Dã Tẩu 87). Hai hàm chạy **mỗi ngày cho mọi nhân
vật** cộng "nợ ảo" vào task 383-397 rồi nằm đó vĩnh viễn — trong khi **không nơi
nào phát thưởng** (`lenhbaitanthu.lua:107` đã comment). Đã giữ lại đúng dòng Vận
Tiêu (dòng duy nhất có hệ ghi thật) và đánh số lại thành `[1]`.

> Đánh số lại là **bắt buộc**: hai hàm duyệt bằng `while TB_BU_HD[i]` — hổng một
> số là dừng ngay.

---

## 2. Việc tôi CHỌN KHÔNG LÀM — và lý do

**Quét dọn hàng loạt ~1.000 tệp bẫy/obj bản đồ Trung Quốc.**

Bộ quét cuối cùng ra con số 1.048 tệp "mồ côi". Tôi **không dời**, vì trong đúng
buổi hôm nay công cụ của tôi đã cho **hai câu trả lời sai** về chính câu hỏi này,
và **cả hai lần đều sai theo hướng "tuyên bố tệp đang sống là chết"**:

1. **Thiếu đường nạp thứ 8** — NPC/bẫy gắn bằng dữ liệu bản đồ trong `Pak\*.pak`
   (khối đã nén). Thiếu nó thì báo **1.675/3.065 tệp là chết**.
2. **Biểu thức cắt cụt tên tiếng Trung** — `[\w\-]+\.lua` gặp ký tự như `·`
   (byte GBK) là dừng, nên `Â·ÈË_³Ì´óÎª.lua` bị đọc thành `ÈË_³Ì´óÎª.lua`, tra
   không khớp. Sửa xong thì chỉ mục bắt thêm **1.639 tên**.

Ngoài ra bộ khảo sát nền cũng xếp **9 cây thư mục Trung Quốc (1.285 tệp) là "an
toàn"** — tôi đối chiếu thì **454 tệp trong đó đang được bản đồ dùng**.

### Phép thử dứt điểm

Sau hai lần sai, tôi không tin bộ lọc nào nữa. Làm phép kiểm **thô bạo nhất**:
lấy 10 tệp ngẫu nhiên trong nhóm "mồ côi", tìm **nguyên byte tên tệp** trên toàn
bộ máy chủ — `script\`, `settings\`, `Maps\` (5.601 tệp), mã nguồn C++, và **cả
5 tệp pak đã giải nén** (154.607 mục).

**Kết quả: 4/10 tệp mẫu THẬT SỰ có trong `maps.pak`.**

```
ÄÏÔÀÕò-¸æÊ¾ÅÆ1.lua              !! CÓ trong maps.pak
Â·±ê_ÁúÈª´åtoÂÞÏüÉ½.lua          !! CÓ trong maps.pak
¼ñÊ°_cyl20_´óÂüÍÓÂÞ»¨.lua        !! CÓ trong maps.pak
sign_tolongmenzhen.lua           !! CÓ trong maps.pak
```

Tức bộ lọc của tôi **sai khoảng 40%** trên nhóm này — nếu dời 1.064 tệp thì có
thể đã dời ~400 tệp đang sống.

Lợi ích gần bằng không (các tệp này nằm im, không tốn gì lúc chạy), rủi ro là
hàng trăm biển chỉ đường / bẫy dịch chuyển chết lặng. **Dứt khoát không làm.**

Muốn dọn tiếp thì làm **từng cụm có tên tuổi**, đọc thật xem là tính năng gì —
đúng cách đã dùng cho 186 tệp hôm nay. Anh chỉ cụm nào tôi làm cụm đó.

**Sắp xếp lại cấu trúc thư mục** cũng vậy — đường dẫn ghi cứng ở C++ và trong dữ
liệu bản đồ đã nén; đổi chỗ tệp là loại thay đổi dễ hỏng nhất trong cây này, mà
lợi ích chỉ là dễ đọc. Tôi đã dọn `New folder` (17 tệp `.lua.bak` rác) và để
nguyên 237 bản sao lưu khác — **đó là lịch sử của anh, không phải rác của tôi.**

---

## 3. Nghiệm thu

| Phép kiểm | Kết quả |
|---|---|
| Cú pháp Lua 4.0.1 (`syncheck.exe` dựng từ `Sources\Library\LuaLib`) | **1.550/1.550** tệp tên ASCII đạt |
| 1.398 tệp tên tiếng Trung | cân bằng từ khoá lệch **0** |
| Lời gọi hàm nil | **0** |
| Tham chiếu treo | **2**, cả hai cố ý (`hd3_driver.lua:49-50` — tên NPC cần xoá) |

---

## 4. VIỆC CUỐI CÙNG — chỉ anh làm được

**Thay `CoreServer.dll`.** Tôi không tự tắt máy chủ.

1. Báo người chơi, tắt cụm theo quy trình của anh.
2. Xác nhận `bin\server\CoreServer.dll.moi` — 18.205.184 byte,
   md5 `5f879a7e3dfe31b7def75f9605196beb`.
3. Chạy `ChayGameServer.bat` (tự đổi tên `.moi`).
4. **Thoát hẳn client rồi vào lại**, không dùng nút đổi nhân vật.
5. Kiểm:
   - Giết quái → nhiệm vụ cuộn Bạn Đồng Hành vẫn đếm.
   - Mua Long Huyết Hoàn (Kỳ Trân Các, 10 xu) → dùng phải cộng lượt Thách Đấu
     Thời Gian; dùng lúc đang đánh nhau phải bị chặn.
   - Mở menu NPC, đi ngang một cuộn Dã Tẩu, bấm tiếp → nút phải **vẫn ăn**.
   - NPC Lôi Đài Hỗn Chiến ở Ba Lăng Huyện đã biến mất.
   - Vào bản đồ Công Thành 221 → không còn bị búng lùi ở 3 điểm cũ.
6. Ai kẹt ở bản đồ 210: thoát game vào lại là tự về Ba Lăng Huyện.
7. Xong báo tôi để dời nốt bộ chuyển tiếp `tinhnang\datau\danhquai.lua` — lúc đó
   thư mục `tinhnang\datau` mới biến mất hẳn.

> ⚠️ **DLL này gồm 4 tệp nguồn khác bản đang chạy**: `KNpc.cpp` + `KPlayer.cpp`
> (hai bản vá của tôi) và `KItem.h`/`KItem.cpp` (**mô tả Phi Phong** — không
> thuộc đợt này, khớp đợt port 29/08 vẫn chờ swap). Muốn bản chỉ có hai bản vá
> của tôi thì nói, tôi hoàn `KItem.*` rồi dựng lại.

---

## 5. Hoàn tác

`doi_tep.hoan_tac("<đường dẫn>", ghi=True)`, hoặc chép ngược từ
`bin\server\_dara\script\<đường dẫn cũ>`. Tệp sửa tại chỗ: chép đè bản `.truoc_*`
nằm cạnh. DLL: đổi tên `CoreServer.dll.truoc` về.
