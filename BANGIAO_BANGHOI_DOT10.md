# BÀN GIAO ĐỢT 10 — HỆ BANG HỘI JX2

> Lập 14/08/2026, cuối phiên đợt 9. **Phiên sau đọc hết file này trước khi gõ dòng mã nào.**
> Chủ game chốt: *"ở phiên sau phải chạy lại điều tra làm lại cho đúng chuẩn Linux"* —
> **không được sửa theo trí nhớ, phải điều tra lại từ bản Linux.**

---

## 0. VIỆC ĐẦU TIÊN: VÁ HAI LỖI TÔI VỪA GÂY RA

Hai lỗi chủ game báo cuối phiên (**mất danh sách thành viên ở các tab** và **rác hiển thị ở các tab**)
là **hồi quy do commit `3d1ddf3` của tôi**, không phải lỗi cũ.

**Gốc — đã xác minh bằng grep, không phải suy đoán:**

`RepositionRows()` trong `Sources/S3Client/Ui/UiCase/UiTongJX2.cpp`:

| Dòng | Nhánh | Đặt gì |
|---|---|---|
| 1028-1029 | `TJX2_UI_PAGE_TONGLIST` | `m_MList[i].SetPosition(205, …)` + `SetSize(150, 20)` |
| 1030-1031 | `TJX2_UI_PAGE_TONGLIST` | `m_RowDim[i].SetPosition(365, …)` + `SetSize(140, 20)` |
| 641, 708 | `LoadScheme` (**chạy đúng 1 lần**) | `m_MList[i].SetPosition(341, 68 + i*24)` |

⇒ **Không nhánh nào khác trả `m_MList` / `m_RowDim` về vị trí và kích thước gốc.**
Người chơi ghé tab "Xem tin Bang khác" **một lần** là:
- `m_MList` kẹt ở x=205 rộng 150 → danh sách thành viên ở mọi tab khác vẽ sai chỗ, bị cắt cụt = **"mất danh sách thành viên"**;
- `m_RowDim` kẹt rộng 140 → dòng người offline bị cắt = **"rác"**.

Kẹt **vĩnh viễn** cho tới khi khởi động lại `Game.exe`, vì `LoadScheme` chỉ chạy lần đầu.

**Cách vá:** mọi nhánh trong `RepositionRows` phải tự đặt **cả vị trí lẫn kích thước** cho
`m_Row`, `m_MList`, `m_RowDim`, `m_BtnRowSel` — không nhánh nào được để control ở trạng thái
nhánh khác đặt. Giá trị gốc: `m_MList`/`m_RowDim` tại `(341, 68 + i*24)`, cỡ theo `[Row0]`.

> 🔴 **ĐÂY LÀ LẦN THỨ HAI dính đúng cái bẫy này.** Lần đầu ở đợt 7 (nhánh Chiêu mộ đổi
> `SetSize` mà không nhánh nào trả lại), đã ghi vào bộ nhớ dự án là bẫy số 7 — **vẫn tái phạm**.
> Phiên sau: sau **mỗi** lần đụng `RepositionRows`, phải grep lại
> `SetPosition|SetSize` cho cả 4 mảng và đối chiếu **từng nhánh**.

---

## 1. VIỆC CHỦ GAME GIAO CHO PHIÊN SAU

### 1.1 Phải chạy lại ĐIỀU TRA trước khi làm
Chủ game nhấn mạnh hai lần: **100% dựa theo phân tích bản Linux, không đoán.**
Không được thi công theo `KEHOACH_BANGHOI_DOT9.md` mà không kiểm lại — bản đó có phần
đã cũ so với hiện trạng mã.

### 1.2 Danh sách việc

| # | Việc | Trạng thái |
|---|---|---|
| 1 | **Vá hồi quy mục 0** (mất danh sách thành viên + rác) | 🔴 gấp nhất |
| 2 | **"Xem tin Bang khác" làm CHUẨN LINUX** — bỏ hẳn bản hiện tại | Bản gốc **KHÔNG phải bảng cột** mà là **4 thẻ 2×2** |
| 3 | **Đổi phe: đổi màu tên thành viên** — hiện trừ tiền nhưng màu không đổi | Chưa đúng chuẩn |
| 4 | **Panel thông tin thành viên** làm lại đúng chuẩn Linux | Chưa |
| 5 | **Các nút ".."** vẫn hiển thị sai | Chưa |
| 6 | **Nhật ký** vẫn tiếng Việt không dấu | Chưa |
| 7 | **LIÊN MINH bang hội** — chủ game chốt **LÀM** | Chưa bắt đầu |
| 8 | **Tính năng "các nút 4 5"** — chủ game chốt **LÀM** | ⚠️ xem ghi chú bên dưới |

⚠️ **"các nút 4 5" — PHẢI HỎI LẠI CHỦ GAME cho rõ trước khi làm.**
Trong phiên, các cụm nút được đánh số khác nhau ở các chỗ khác nhau. Hai cách hiểu:
- **trang con 3 và 4** của tab "Sử dụng chức năng" (trước đó chủ game nói *"nút số 3 4 tạm thời chưa làm"*, nay nói *"làm luôn tính năng các nút 4 5"*) — trang con 3 = **liên minh**, trang con 4 = **đại thần quốc gia**;
- hoặc **nút số 4 và 5** trong một cụm cụ thể nào đó.
Vì chủ game đồng thời chốt *"làm luôn phần liên minh"*, khả năng cao là **trang con liên minh + đại thần**.
**Không tự suy — hỏi một câu rồi mới làm.**

---

## 2. LIÊN MINH BANG HỘI — KHỐI LƯỢNG THẬT

Đã điều tra (34 agent, có phản biện). **0% ở CẢ HAI phía**, không có gì tái dùng được:

- `grep "Union"` toàn bộ `Sources/MultiServer/S3Relay` = **0 kết quả**.
- `TONG_JX2_INFO_SYNC` (`Sources/Core/Src/KProtocol.h`) **không có trường liên minh nào**.
- Phía ta chỉ có duy nhất `TONG_GetUnionID` đọc field 10 — **không ai ghi field đó**.

**Phải dựng mới:**
1. Lớp `KTongUnion` ở Relay + **bảng CSDL riêng**;
2. **6 mã lệnh JX2** (tạo / xin vào / duyệt / rời / đuổi bang / giải tán);
3. Gói đồng bộ liên minh + **thêm trường vào `TONG_JX2_INFO_SYNC`** ⇒ **đổi gói tin, phải vá CẢ HAI bản `KProtocol.h` và build client + server + relay cùng lúc**;
4. 5 nút UI ở trang con + màn hình chọn liên minh;
5. Hai luật bản gốc: **rời liên minh phải chờ 3 ngày** mới vào được liên minh mới; số bang tối đa theo `[LevelUnionNum]` trong `tongset.ini` (cấp 0-40 → 3 bang, 41-49 → 5, 50-69 → 6, 70-89 → 7, 90-100 → 8);
6. Chỉ **Minh chủ** được công thành.

**Ước lượng 6–10 ngày công. Rủi ro cao** vì đụng vào `KTongControl.cpp` = đường lưu bang hội thật
(nguy cơ mất dữ liệu bang). **Phải xoá 3 thư mục DB ở `bin\multiserver\` trước khi test.**

Tài liệu tra cứu: `D:\GAMEDEVNEW\BANGHOI_JX2_PHANTICH.md` **mục 10** (`KTongUnion`, thuần Relay).

---

## 3. BẪY ĐÃ BIẾT — ĐỌC TRƯỚC KHI SỬA

1. 🔴 **HAI TÚI TIỀN.** Ngân quỹ bang có túi JX1 `CTongControl::m_dwMoney` và túi JX2 `field 3/4` (64-bit). Hệ JX2 chỉ nạp/tiêu túi JX2; **túi JX1 luôn = 0**. Mọi đường JX1 **có kiểm tiền** đều từ chối dù bang đầy tiền. Đã gặp ở **đổi phe** (ba cổng: client `KPlayerTong.cpp:1886`, GS `:1968`, relay `KTongSet.cpp:669`). ⇒ Tái dùng đường JX1 thì **luôn kiểm riêng nguồn tiền trước**.
2. 🔴 **`SetSize`/`SetPosition` trong `RepositionRows` không tự trả lại** — xem mục 0.
3. 🔴 **`KWndText` KHÔNG đọc khoá `Image=`** — phải có mảng `KWndImage` chạy song song (mẫu `m_InfoBg`, `m_FunTxtBg`).
4. 🔴 **`Trans=1` mà không có `Image=`** ⇒ nút **vô hình với chuột** (`PtInWindow` bị ghi đè bằng alpha của ảnh rỗng).
5. 🔴 **`AddChild` nối vào CUỐI; vẽ theo thứ tự đó; hit-test quét NGƯỢC** ⇒ control thêm sau nằm trên, **nuốt chuột**. Lớp chữ phủ lên vùng bấm phải `Enable(false)`.
6. 🔴 **Section KHÔNG có khoá `Label=` ⇒ sprite đã nung chữ sẵn** — `SetLabel` thêm là **chồng chữ**. Ngược lại section **có** `Label=` thì đừng đè bằng bản tự chế.
7. ⚠️ **Font game là font TỈ LỆ** — căn cột bằng `%-18s` không bao giờ thẳng hàng; muốn có cột phải dùng **control riêng cho từng cột**.
8. ⚠️ **`GetCurSchemePath` trả về THƯ MỤC** — phải tự nối `\UiTongJX2.ini` (mẫu: `LoadScheme`). Quên thì `Ini.Load` thất bại **im lặng**.
9. ⚠️ `KWndLabeledButton` **cắt nhãn hai lần**: `Buff[32]` lúc Init, rồi `m_Width*2/FontSize` lúc vẽ. Nút rộng 20-25px chỉ hiện 3-4 ký tự ⇒ ra `".."`.
10. ⚠️ Trang Phường thợ có **HAI file thiết kế**; file thứ hai tên bắt đầu bằng 作坊 nên `CLIENT_BANGHOI_CONTROLS.md` **bỏ sót**. Luôn `ls` thư mục .ini gốc, đừng tin mỗi tài liệu trung gian.
11. ⚠️ Nguồn là **ANSI/TCVN3** — bắt buộc sửa bằng skill `swordonline-dev` (`safe_edit.py`). Chuỗi tiếng Việt dán vào mã phải ở dạng **octal escape**, sinh bằng `vn_to_octal.py`, **đọc byte thô từ bản gốc, không gõ tay**.

---

## 4. ĐƯỜNG DẪN & BUILD

| Vai trò | Đường dẫn |
|---|---|
| Nguồn sửa (canonical) | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\SOURCESUPDATE_KINHMACH_ONLTEST0608` |
| Chạy thử | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin` |
| **Client chủ game THẬT SỰ chạy** | `J:\CayChay\pakgame\volamngaothe\PATCHFULL_NGAOTHE_MK_123456\` |
| Bản mẫu JX2 Linux | `D:\ServerLinux` (`Patch\ui\ui3_1024` = bản thiết kế gốc) |
| Kho git | `JXGAME04/GAMEDEVNEW.git`, đẩy `master:main` |

**4 cấu hình build (đều tự deploy sang `bin\`):**
- Core `"Server Release|x64"` → `bin\server\CoreServer.dll`
- Core `"Client Release|Win32"` → `bin\client\CoreClient.dll`
- S3Client `"Release|Win32"` → `bin\client\Game.exe`
- S3Relay `"Release|Win32"` → `bin\multiserver\S3Relay.exe`

⛔ Cấu hình `"Server Release|Win32"` **hỏng sẵn** (`ScriptFuns.cpp` thiếu include dir) — đừng dùng.
⛔ **Sau mỗi lần build client phải chép thêm** `Game.exe` + `CoreClient.dll` + `Ui\Ui3\UiTongJX2.ini`
sang cây **PATCHFULL** — quên là chủ game test bản cũ (đã mất trắng một đợt test vì lỗi này).

---

## 5. ĐÃ LÀM XONG ĐỢT 9 (để khỏi làm lại)

- Đổi phe: dựng op JX2 riêng (COP 27 + TOP 16), kiểm/trừ **túi JX2**, relay ghi `m_nCamp` qua `JX2_SetCamp` mới. **Còn thiếu: đổi màu tên thành viên theo phe.**
- Màu danh sách thành viên theo bảng gốc `[Fun_MemberList]`: online `236,238,111`, offline `0,134,132`, đang chọn `34,228,36`.
- 4 nút trang con + 4 nút mục con Nhật ký chạy kiểu **radio**.
- Phân quyền: bỏ `return` im lặng, hiện hộp báo; nút "Chọn tất cả" bật/tắt được.
- Bỏ gọi xin lại trang với 3 lệnh mang chuỗi ⇒ lưu tin chiêu mộ thấy ngay.
- Tên 5 chức vụ nguyên văn bản Linux (Đệ tử / Ẩn sỹ / Trưởng Lão).
- Bỏ `SetLabel` ở 3 chỗ sprite đã nung chữ; trả 4 nhãn mục con Nhật ký về nguyên văn.
- Vá lỗi `Ini.Load` nạp thư mục (làm mọi hộp xác nhận rơi về câu mặc định).

Commit cuối: `3d1ddf3`. **Lưu ý `3d1ddf3` chính là commit gây hồi quy mục 0.**
