# BÀN GIAO — AUTO SĂN BOSS SÁT THỦ + GHÉP SÁT THỦ GIẢN (WAuto tab 13)

> 25/08/2026. Theo yêu cầu chủ game: *"viết thêm tính năng tự săn boss sát thủ - ghép
> sát thủ lệnh"*, *"phải làm theo từng bước 1: nhận nhiệm vụ rồi di chuyển tới xa phu
> rồi lên map nhiệm vụ - đến toạ độ boss sát thủ đánh boss xong phải chạy phần nhặt đồ
> theo config người chơi - rồi dùng phù về thành tới nhận nhiệm vụ tiếp cứ vậy"*.
>
> Nguồn tham khảo: `AUTO_SATTHU_SPEC.md` (đặc tả) + `PHANTICH_SOURCE_AUTO_THAILAN.md`
> (`D:\Source_ANTITHAILAN` — `Class103.cs` là phần săn boss + tự ghép Sát Thủ Giản).
>
> **⚠️ ĐÃ BUILD, ĐẶT BINARY CẠNH BẢN ĐANG CHẠY, CHƯA SWAP, CHƯA TEST.**

---

## 1. Vòng chơi auto làm — đúng thứ tự chủ game yêu cầu

```
[1] về thành có NPC 769 "Nhiếp Thí Trần" (dùng phù về thành; hết phù thì đi Xa phu)
      |
      |-- (nếu đủ 5 Sát Thủ lệnh CÙNG CẤP và còn ô trống) --> GHÉP Sát Thủ Giản trước
      |
[2] mở thoại NPC -> "Nhiệm vụ cấp 90" -> bấm ĐÚNG dòng tên con boss muốn đánh
      (server ghi số hiệu 141..160 vào task 1082)
      |
[3] ra XA PHU -> "Lên bản đồ luyện công (20 - 90)" -> "Mốc 90" -> tên bản đồ nhiệm vụ
      |
[4] chờ chuyển map -> đi bộ từ điểm đáp xuống tới Ô BOSS (bảng KSatThuBossPos.h)
      |
[5] tìm ĐÚNG con boss mang tên trong bảng rồi giao mục tiêu cho máy PK (tab PK) đánh
      |
[6] boss chết (task 1082 về 0, task 1193 +1) -> NHẢ MÁY cho bộ NHẶT ĐỒ của người chơi
      (tab Nhặt đồ + danh sách Lọc) làm việc tại chỗ
      |
[7] dùng phù về thành -> quay lại [1]. Hết lượt/ngày thì nghỉ tới ngày mai.
```

Không tự chế cơ chế nào mới: nhặt đồ, ăn thuốc, bán rác, cất rương, ngưỡng túi đầy…
vẫn là các tab có sẵn của WAuto.

---

## 2. Giao diện — tab mới **"Sát thủ"** (tab thứ 13)

Hàng nút tab thứ 3 nay có 5 nút (Dã Tẩu · Tống Kim · Liên đấu · H.động · **Sát thủ**);
4 nút cũ chỉ bị thu hẹp lại, nhãn giữ nguyên.

| Ô | Ý nghĩa | Mặc định |
|---|---|---|
| Bật auto săn Boss Sát Thủ (cần cấp 90) | công tắc chính | tắt |
| Số lượt mỗi ngày (tối đa 8) | máy chủ chỉ cho 8 lượt/ngày (task 1193) | 8 |
| Chọn boss | Lần lượt 20 con / Ngẫu nhiên / Cố định 1 con | Lần lượt |
| Boss cố định | 20 con nhóm cấp 90 (chỉ dùng khi chọn "Cố định") | Giả Giới Nhân |
| Nghỉ giữa 2 lượt (phút) | 0 = làm liên tục; >0 = nhả máy cho auto thường | 0 |
| Tự ghép 5 Sát Thủ lệnh -> 1 Sát Thủ Giản | ghép ngay tại NPC | bật |
| Boss chưa hồi sinh thì chờ tại chỗ | tắt = huỷ nhiệm vụ đổi con khác (không mất lượt) | bật |

Chân cửa sổ WAuto hiện dòng *"Sát Thủ: …"* cho biết auto đang ở bước nào.

---

## 3. Tệp đã sửa

| Tệp | Việc |
|---|---|
| `ReverseTools/gen_satthu_boss_table.py` | sinh thêm bảng `s_szST3BossTen[]` (tên 160 boss) |
| `Sources/Core/Src/KSatThuBossPos.h` | **sinh lại** — thêm bảng tên boss (2 tệp sinh kia byte y hệt) |
| `Sources/Core/Src/ipc_shared.h` | 7 ô cấu hình mới, **thêm ở CUỐI struct** (`bSatThu` … `nSTNghi`) |
| `Sources/Core/Src/KPlayer.h` | 18 trường trạng thái `ExtAuto` (`nSTPhase` … `uSTVongT`) |
| `Sources/Core/Src/CoreShell.h` | `ATYPE_SATTHU` (thêm CUỐI enum, không xê dịch số cũ) |
| `Sources/Core/Src/CoreShell.cpp` | **máy chính `ST_Process` + 10 hàm phụ (~640 dòng)** + dòng trạng thái |
| `Sources/S3Client/S3Client.cpp` | nối vào `ExtAutoLoop` (ưu tiên thấp nhất trong 4 máy hoạt động) |
| `E:\Src_Auto_Ngoai\WAuto\WAuto\Resource.h` / `.rc` / `.cpp` | tab 13 + 13 control + nạp/lưu cấu hình + tương thích `.dat` cũ |

---

## 4. Những chỗ then chốt (đọc trước khi sửa tiếp)

1. **Đọc trạng thái từ task, KHÔNG đoán theo thoại**
   `1082` = số hiệu boss đang truy nã (0 = chưa nhận) · `1193` = số lượt đã giết hôm nay ·
   `1192` = ngày server ghi. Server đồng bộ 3 task này xuống client qua `nt_setTask ->
   SyncTaskValue`. Boss chết = `1082` về 0 (`lib_killlevel.lua : SetMemberTask`).

2. **Tìm boss theo TÊN, tìm NPC theo TEMPLATE**
   NPC 769 tìm bằng `m_NpcSettingIdx == 769` (tên TCVN3 có byte cao, `g_StrLower` không
   an toàn để so tên NPC); còn con boss thì phải tìm theo ĐÚNG TÊN trong `killer.txt`
   (đã kiểm: **0 cặp tên lồng nhau** trong cả 160 con) — giết nhầm con khác là nhiệm vụ
   không tính.

3. **Menu Xa Phu khớp từng byte**
   `"Lên bản đồ luyện công (20 - 90)"` → `"Mốc 90"` → `"Mạc Cao Quật [90]"`. Cả 20 boss
   nhóm cấp 90 ĐỀU có trong menu ⇒ auto **không bao giờ phải thuê xe 1000 lượng**
   (nhánh `st3_goboss` vẫn giữ làm dự phòng). `xp_go` gọi `SetFightState(1)` nên lên map
   là đã ở chế độ đánh — máy PK dùng được ngay.

4. **Ghép Sát Thủ Giản = hộp giao vật phẩm, mỗi nhịp bỏ MỘT món**
   Một gói `c2s_playermoveitem` phải có `Down == Up` (server từ chối gói kéo), nên "kéo"
   đồ = 2 cú click — dùng đúng khuôn `DTP_GIVEBOX` của Dã Tẩu. Server kiểm theo thứ tự:
   lẫn món khác → lỗi; đủ 5 nhưng khác cấp → lỗi; >5 → "để quá nhiều"; <5 → "để quá ít".
   🔴 `nieshichen.lua` **không gọi `EndGiveBox`** ⇒ hộp không tự đóng, phải tự đóng bằng
   `GDCNI_END_AFFAIR_BOX` (`ST_DongHop`), không thì khung hộp treo mãi và
   `Wnd_GameSpaceHandleInput(false)` chặn tay người chơi.

5. **Huỷ nhiệm vụ không cần về thành** — server đã có sẵn nhánh `SendUiCmdScript(6,
   "st3_quit")` (nút *Bỏ nhiệm vụ* của bảng F11) gọi thẳng `nieshichen.cancel()`.

6. **Trả 2 chứ không phải 1 khi đứng chờ boss hồi sinh** — trả 1 là chặn cả
   `ATYPE_PICKUP` lẫn `ATYPE_PKFIGHT`: đứng im 10 phút giữa ổ quái mà không đánh trả
   được. Trả 2 (không giao `uNpcID`) thì máy PK tự chọn mục tiêu theo đúng tab PK.

7. **Bốn đồng hồ canh kẹt**: 4 phút/pha đi đường · **8 phút riêng cho pha đi bộ trong
   map** (hầm Sa Mạc Mê Cung / Mạc Cao Quật đi rất lâu) · 10 phút chờ hồi sinh
   (`ReviveFrame 16200`, JX1 chia đôi ≈ 7,5 phút/lần) · 45 phút/vòng. Đang đánh thật thì
   gia hạn đồng hồ vòng.

8. **Túi đầy thì NHẢ MÁY** — hết một lượt mà túi đầy theo đúng ngưỡng tab Cơ bản
   (`bCheckTPIBox` / `nTPiboxSel`) thì nghỉ 3 phút cho chu trình Hậu cần có sẵn (về
   thành, bán rác theo bộ lọc, cất rương) chạy, xong mới săn tiếp.

9. **Tệp `APdata\<ID>.dat` cũ** vẫn dùng được: gói ngắn hơn struct được chép vào bản
   sao đã xoá trắng (`S3Client.cpp:1162`) nên `bSatThu = 0`, và WAuto đặt mặc định cho
   khối mới qua `offsetof(autoData, bSatThu)`.

---

## 5. Học được gì từ source auto Thái Lan

`Class103.cs` (56 KB) là phần săn boss sát thủ + tự ghép của họ. Ba thứ đã áp dụng:

* **Bắt các câu trả lời chốt của server thay vì đếm bước**: `"khinh k"` (khinh kẻ bại
  trận = hết 8 lượt/ngày), `"ngươi thắng"` (đã nhận nhiệm vụ), `", hoàn thành nh"` (boss
  chết). Bản này dùng đúng các marker đó, cộng thêm task 1082/1193 làm nguồn chắc chắn.
* **Công tắc `TuGhepSTG`** — họ cũng để người chơi bật/tắt ghép, và chỉ ghép lệnh **cấp
  90** (`!= 90 continue`). Bản này gộp theo cấp có nhiều lệnh nhất, ưu tiên cấp cao.
* **Đếm số lệnh trước rồi mới mở giao diện** (`smethod_4 >= 5`) — tránh mở hộp rồi loay
  hoay. Bản này làm y vậy (`ST_DemLenh`).

Khác biệt cố ý: họ dò địa chỉ hàm client bằng AOB rồi bắn shellcode từ ngoài vào; mình
gọi thẳng hàm C++ trong CoreClient nên không cần bảng địa chỉ, không sợ đổi build.

---

## 6. Binary đã đặt sẵn (CHƯA SWAP)

Lúc thi công có **2 Game.exe + 1 WAuto.exe đang chạy** (chủ đang test bản vá C37 của
phiên khác) nên **không swap**. Ba tệp mới nằm cạnh bản đang chạy:

```
E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\
    CoreClient.dll.moi_2508_satthu    (md5 9ff22bad3381…)
    Game.exe.moi_2508_satthu          (md5 fe2cc45a5351…)
    WAuto.exe.moi_2508_satthu         (md5 fa48276838e4…)
```

**Cách swap** (thoát hết Game.exe + WAuto.exe trước): đổi tên bản cũ thành
`*.cu_2508_truoc_satthu` rồi đổi 3 tệp `.moi_2508_satthu` thành tên chuẩn.
**Phải swap CẢ BA** — `CoreClient.dll` chứa máy, `Game.exe` chứa chỗ gọi máy,
`WAuto.exe` chứa tab bật/tắt. Thiếu tệp nào thì tính năng chỉ nằm im (không hỏng gì,
nhờ hàng rào kích thước gói ở `S3Client.cpp:1162`).

Không cần restart máy chủ: toàn bộ phần mới nằm ở **client**, script server giữ nguyên
(chỉ `hd3_st_goboss.lua` được sinh lại nhưng byte y hệt bản cũ).

---

## 7. Việc còn lại / cần chủ game xác nhận khi test

1. Bấm đúng dòng tên boss trong bảng 20 con (trang 1 = 141..150, phải bấm *Trang kế* mới
   thấy 151..160) — cần nhìn thật xem có bấm trúng không.
2. Ghép Sát Thủ Giản: xem 5 lệnh có vào hộp đủ không, và hộp có tự đóng sau khi ghép.
3. Con boss ở hầm sâu (Sa Mạc Mê Cung 1 / Mạc Cao Quật): xem 8 phút đi bộ có đủ không.
4. Nhặt đồ sau khi hạ boss: xác nhận bộ lọc tab Nhặt đồ được tôn trọng.
5. Nếu chủ muốn auto **không** ăn hết 8 lượt (để dành cho tay), giảm ô *Số lượt mỗi ngày*.
