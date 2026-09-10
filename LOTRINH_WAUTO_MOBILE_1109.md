# LỘ TRÌNH: ĐƯA **TRỌN BỘ WAuto** VÀO BẢN MOBILE (11/09)

> Nhánh `mobile-0809` (worktree `D:\GAMEDEVNEW_wt_mobile`).
> Đọc kèm: `BANGIAO_ANDROID_PHA4_0809.md` (§2.4, §9), `KEHOACH_GIAODIEN_MOBILE_0909.md`,
> `LOTRINH_MOBILE_0909.md` (§9 nói WAuto ngoài không dùng được — mục đó nay được thay bằng tệp này).
>
> **Chủ game chốt 11/09:**
> 1. Lấy **trọn bộ** WAuto vào trong game, không phải bản rút gọn.
> 2. Ảnh lấy từ kho VNKU.
> 3. Tính năng nào bản mobile không dùng thì **để nguyên, KHÔNG xoá mã** — client PC dùng chung một bản nguồn.
> 4. **Dựng khung Auto + icon Auto TRƯỚC**, chạy được rồi mới làm **từng tab một**.

---

## 0. LUẬT — ĐỌC LẠI TRƯỚC MỖI ĐỢT

### 0.1. BẢN PC ĐANG CHẠY KHÔNG ĐƯỢC ĐỔI MỘT HÀNH VI NÀO

Đây là luật số một. Mọi luật khác phục vụ nó.

| Cách | Làm thế nào |
|---|---|
| **Tệp nguồn mới, CHỈ Android** (không có gì cho `GameSDL.exe` dùng) | Đặt trong `Sources/S3Client/Platform/`, **không khai trong vcxproj nào**, thêm thẳng vào `add_library(main SHARED ...)` trong `android/CMakeLists.txt`. Không cấu hình Windows nào biết tới tệp. Mẫu có sẵn: `JxPerfHudAndroid.cpp` (vá 77) và `JxWAutoNoiBo.cpp` (vá 83, B0). **Ưu tiên cách này** |
| **Tệp nguồn mới có dùng cho cả `GameSDL.exe`** | Đặt trong `Sources/S3Client/Ui/UiCase/` hoặc `Platform/`, khai trong `S3Client.vcxproj` kèm đúng dòng `ExcludedFromBuild` với điều kiện khác `ReleaseSDL|x64` là `true`, rồi `python android/gen_lists.py`. Client PC phát hành là cấu hình `Release|x64` nên **không bao giờ biên dịch tệp đó**. Mẫu có sẵn: `Platform\JxCanDieuKhien.cpp` |
| **Sửa tệp nguồn ĐANG CÓ** (dùng chung với PC) | Bắt buộc rào `#ifdef JX_ANDROID`. Không rào là phạm luật, kể cả khi thấy chắc chắn vô hại |
| **Danh sách dựng Android** | Chạy `python android/gen_lists.py` sinh lại `android/lists/*.cmake`. **Không sửa tay tệp lists.** Bộ sinh đọc cấu hình `ReleaseSDL|x64` của từng vcxproj |
| **Nghiệm thu mỗi đợt** | Dựng lại **cả hai** chuỗi Windows, **chạy tách rời**, cả hai **0 lỗi**: `ReverseTools\mobile_x64\build_chuoi_x64.ps1` và `ReverseTools\mobile_x64\build_chuoi_sdl.ps1`. Chưa xanh cả hai thì đợt đó chưa xong |

> **Bẫy cần nhớ:** cấu hình `ReleaseSDL|x64` là **`GameSDL.exe` trên Windows**, không phải riêng Android.
> Tệp đánh dấu như trên **vẫn vào** `GameSDL.exe`. Nên phần nào chỉ đúng cho điện thoại thì bên trong
> tệp vẫn phải rào `#ifdef JX_ANDROID`, và `build_chuoi_sdl.ps1` vẫn phải xanh.

### 0.2. KHÔNG XOÁ TÍNH NĂNG WAuto NÀO

Thẻ **Đ.nhập** và thẻ **Ác chính** bản mobile không dùng (một máy chỉ một nhân vật), nhưng:

- **Không xoá** trường nào trong `autoData` (`Sources/Core/Src/ipc_shared.h`). Xoá một trường ở giữa struct
  là lệch offset mọi trường phía sau, **nát tệp `APdata\<ID>.dat` của mọi người chơi PC**. Cảnh báo này đã
  ghi sẵn ở 5 chỗ trong chính header đó.
- **Không xoá** hàm engine nào trong `CoreShell.cpp` (`AC_*`, `DT_*`, `TK_*`, `LD_*`, `CT_*`, `ST_*`, `HD_*`).
- **Không xoá** thư mục `WAutoUI/` — WAuto.exe vẫn là công cụ của bản PC.
- Bản mobile chỉ **không dựng hai thẻ đó trong giao diện**. Trường vẫn nằm trong struct, vẫn về 0 khi nạp mặc định.

Bộ auto cũ `fkauto` (`Player.m_cAuto`, `KPlayerAuto.cpp` 4.828 dòng, `UiAutoPlay.cpp` 4.606 dòng) cũng vậy:
chủ nói không dùng, nhưng **để nguyên**. Nó vốn đã bị tắt sẵn — `UiShell.cpp:215` đã ghi chú dòng
`Player_AutoPlay::RegisterSelfClass();` và toàn bộ thân `Player_AutoPlay::OnButtonClick()` đã bị ghi chú.
Việc duy nhất phải làm là **đừng mở nó**, và bảo đảm bảng WAuto mới với `fkauto` loại trừ nhau.

### 0.3. NGUỒN TCVN3 CHỈ SỬA BẰNG PYTHON `latin-1`

Mọi tệp `.cpp` / `.h` / `.ini` trong cây game là TCVN3. Công cụ soạn thảo thường làm hỏng hết chữ Việt.
Sửa bằng python đọc ghi `latin-1`, giữ CRLF, viết thành `android/va_nguon_android_NN.py` chạy lại được
nhiều lần mà vô hại.

Nhãn chữ của WAuto nằm trong `WAutoUI/WAuto.rc` dạng **UTF-16**, phải chuyển sang **TCVN3** khi đưa vào
tệp bố cục. Dùng hàm `vn()` trong `C:\Users\nguye\.claude\skills\swordonline-dev\scripts\vn_edit.py`.
**TCVN3 không mã hoá được chữ HOA có dấu** — nhãn phải viết kiểu "Chiến đấu", không viết "CHIẾN ĐẤU".

### 0.4. KÝ HIỆU TOÀN CỤC TRÊN ELF

Trên Android mọi thư viện là `.so`, bộ nạp động chỉ giữ **một** định nghĩa cho mỗi tên toàn cục trong cả
tiến trình. Hai biến trùng tên ở hai thư viện là **sai lặng lẽ, không báo lỗi lúc dựng**. Đã trả giá một
lần với `GameScriptFuns` (§9.1 bản bàn giao pha 4).

Luật: **mọi biến và hàm toàn cục mới của bảng WAuto phải `static`**, hoặc mang tiền tố riêng `g_WA...`.
Rà lại sau mỗi đợt bằng `llvm-nm -D --defined-only` trên các `.so` arm64 rồi lọc tên trùng.

Nền hiện tại đo ngày 11/09: **6 ký hiệu dữ liệu trùng**, năm cái cố ý, một cái là `l_Time`
(`Core/Src/ImgRef.cpp:7` và `S3Client/Ui/Elem/UiImage.cpp:13`) — vô hại vì cả hai đều ghi `SDL_GetTicks()`,
nhưng nên đặt `static` khi tiện tay. **Số 6 này là mốc: sau mỗi đợt không được tăng.**

### 0.5. DỮ LIỆU: TỆP CHỈ-ANDROID NẰM Ở LỚP GHI ĐÈ

`D:\jx1_android_data` là thư mục **SINH RA** bởi `android/chuan_bi_du_lieu.ps1` từ cây client PC. Sửa thẳng
vào đó là mất khi sinh lại. Ảnh `.spr` và tệp bố cục riêng của bảng WAuto mobile phải đặt trong
**`android/du_lieu_ghi_de/`**, đúng cây con:

```
android/du_lieu_ghi_de/spr/uinew/uiautonew/          anh bang auto (thu nho tu VNKU)
android/du_lieu_ghi_de/spr/uinew/uitoolscontrolbar/  icon Auto tren thanh cong cu
android/du_lieu_ghi_de/ui/ui3/                       bo cuc bang auto
```

Tên thư mục và tên tệp **hạ chữ thường**, vì `JxPathPosix` hạ chữ thường phần tương đối.

### 0.6. HAI KHO CHỈ ĐỌC

`C:\Users\nguye\Downloads\NHACTAI` (ảnh VNKU) và `D:\USVOLAM` (nguồn tham khảo): **chỉ đọc.**
Không ghi, không sửa, không xoá gì trong hai cây đó. Chép ra chỗ khác rồi mới xử lý.

### 0.7. HAI LUẬT CŨ VẪN GIỮ

- Không dựng, không đo khi đang có trận Tống Kim.
- Không đụng cây chạy thật `E:\SourceTuanLe\...\bin`.

---

## 1. ĐỨNG Ở ĐÂU — đo thật 11/09

**Bộ não WAuto đã nằm sẵn trong bản Android.** Không phải port, không phải viết lại.

| Phần | Ở đâu | Trạng thái Android |
|---|---|---|
| 180 hàm máy auto (`DT_/TK_/LD_/CT_/ST_/HD_/AC_`), trải ~12.800 dòng | `Sources/Core/Src/CoreShell.cpp` | đã dịch, nằm trong `libCoreClient.so` |
| Bộ điều phối mỗi vòng `ExtAutoLoop` | `Sources/S3Client/S3Client.cpp:843` | đã dịch, nằm trong `libmain.so` |
| Bộ nhận lệnh `ProcIpcCommand` | `Sources/S3Client/S3Client.cpp:1252` | **đã được gọi mỗi khung hình** (`:1466`) |
| Dòng trạng thái auto đang làm gì | `CoreShell.cpp` hàm `WA_HoatDong()` | đã dịch, trả chuỗi TCVN3 dùng được ngay |
| Danh sách chiêu cho hộp chọn | `KProtocolProcess.cpp:2090` `GetAllSkillByType()` | gọi thẳng được, không cần đi qua IPC |
| **Bảng điều khiển** (7.675 dòng hộp thoại Win32) | `WAutoUI/WAuto.cpp` | **chưa có gì** |

Kiểm chứng: chuỗi `[AUTO-PASS]`, `[AUTO-END]`, `[HD-GATE]` có trong `libmain.so` arm64 đã dựng;
chuỗi `ac chinh` có trong `libCoreClient.so`.

**Đường ống đã chạy được trong một tiến trình.** Lớp giả lập Posix biến vùng nhớ chung thành `calloc`
(`KPosixWin32.cpp:879`), còn tín hiệu là **điều kiện pthread thật** (`:148`, `:173`). Nghĩa là một bên gửi
nằm ngay trong game chạy qua đúng đường cũ, **không phải sửa một dòng nào ở bên nhận**.

**WAuto không đọc ghi bộ nhớ game.** Đếm trong nguồn: 0 lần `ReadProcessMemory`, 0 lần `WriteProcessMemory`,
0 lần `SendInput`. Nó chỉ gửi một cấu trúc `autoData` **7.644 byte** mỗi 54 ms. Không có phần hack nào phải port.

---

## 2. THỨ TỰ LÀM — theo đúng chỉ đạo của chủ

```
B0   duong ong trong game          (khong co giao dien - de nut BAT o B1 co cai de bat)
B1   KHUNG AUTO + ICON AUTO        <-- chu yeu cau lam TRUOC
B2   bo sinh bo cuc + lop trang chung
B3..B17   tung tab mot, moi tab mot dot
B18  sau bang con dac biet
B19  don, do, ban giao
```

**B0 không phải việc giao diện.** Nó là khoảng 400 dòng làm cho máy auto chạy được khi có ai bảo nó chạy.
Không có B0 thì nút BẬT ở B1 là nút chết. Làm B0 xong, chủ đã cầm điện thoại lên thấy nhân vật tự đánh,
tự nhặt, tự uống thuốc, trước khi bỏ một đồng nào cho phần vỏ.

---

## 3. CHI TIẾT TỪNG BƯỚC

### B0 — Đường ống trong game (~400 dòng, không giao diện)

> **Tiến độ: XONG 11/09** — vá `android/va_nguon_android_83.py` / `84.py` / `85.py` (tạo `Platform/JxWAutoNoiBo.h/.cpp`, vá
> `S3Client.cpp`, `CoreShell.cpp`, `KCore.cpp`, `android/CMakeLists.txt`, `du_lieu_ghi_de/config.ini`) + công cụ
> `android/wauto_dat.py`. Đo 3 lần trên LDPlayer: bên nhận `ExtAutoLoop` ghi `[AUTO-PASS]`/`[HD-GATE]`/`[FIGHT-*]` đúng tham số,
> tệp `APdata\<id>.dat` 7.644 byte sạch, hai chuỗi Windows 0 lỗi. Chi tiết: `BANGIAO_WAUTO_MOBILE_B0_1109.md`.

**Tệp mới:** `Sources/S3Client/Platform/JxWAutoNoiBo.cpp` và `.h`, đánh dấu `ExcludedFromBuild` ngoài
`ReleaseSDL|x64`, bên trong rào `#ifdef JX_ANDROID`.

| Việc | Chi tiết |
|---|---|
| Bên gửi trong tiến trình | Nạp `IPCGameLoop` (`CmdID = PRT_GAMELOOP`, `Size = sizeof(IPCGameLoop)`) vào `g_pState` rồi `SetEvent(g_hEventRecv)`. Cứ 54 ms một lần, gọi từ nhịp vòng lặp. `SHARED_SIZE` là 65.536 byte, `autoData` là 7.644 byte, thừa chỗ |
| Nạp và ghi cấu hình | `autoData` ghi thô ra `<thư mục ghi được>\userdata\wauto_<tên nhân vật>.dat`, y như WAuto ghi `APdata\<ID>.dat`. Có bản mặc định khi tệp chưa có |
| Loại trừ với `fkauto` | Bảng WAuto bật thì không mở `KUiAutoPlay`, và ngược lại. Một dòng kiểm ở mỗi bên |
| Công tắc tạm | `config.ini [WAuto] Bat=1` để thử trước khi có giao diện |

**Nghiệm thu B0:** đặt sẵn một tệp `.dat` bật Chiến đấu, Nhặt đồ, Phục hồi; mở game trên LDPlayer;
nhân vật **tự đánh, tự nhặt, tự uống thuốc**. Nhật ký `[AUTO-PASS]` và `[HD-GATE]` hiện trong logcat.
Hai chuỗi Windows dựng lại 0 lỗi.

### B1 — KHUNG AUTO + ICON AUTO (chủ yêu cầu làm trước)

Đây là bước quyết định trông có ra bản mobile không. Chưa có tab nào, chỉ có vỏ chạy được.

**Ảnh lấy từ kho VNKU** — đo thật 11/09, tất cả **chỉ đọc**, chép ra rồi mới xử lý:

| Ảnh nguồn | Cỡ gốc | Dùng làm | Xử lý |
|---|---|---|---|
| `Spr\UiNew\UiToolsControlBar\nut_de_auto.spr` | 151×151, 30 khung | **icon Auto** trên thanh công cụ | thu nhỏ về **44×44** cho khớp `pk_m.spr` đang dùng |
| `Spr\UiNew\UiAutoNew\khung.spr` | 1313×788, 1 khung | **khung bảng auto** | thu nhỏ về khoảng **980×588**, vừa khung vẽ 1040×604 |
| `Spr\UiNew\MinMapSmall\bat_auto.spr` và `tat_auto.spr` | 285×112, 2 khung | nút **BẬT** và **TẮT** auto | thu theo bề rộng khung |
| `Spr\UiNew\MinMapSmall\thietlapauto.spr` | 285×98, 2 khung | nút **Thiết lập** | như trên |
| `Spr\UiNew\UiAutoNew\tick_chon.spr` | 36×36, 2 khung | ô tick | **KHÔNG thu nhỏ** — 36 px là vừa tay trong hàng 48 px |
| `Spr\UiNew\UiAutoNew\nut_pop.spr` | 476×62, 2 khung | nút thả xuống | thu theo bề rộng cột |
| `Spr\UiNew\UiAutoNew\btn_*.spr` (10 nút) | 352×96 | thêm, xoá, lên, xuống, lấy | thu theo hàng 48 px |
| `Spr\UiNew\UiAutoNew\AutoGetCoord\main.spr` và `xong.spr` | 504×860 | bảng bắt toạ độ, dùng ở B18 | để dành |

Công cụ thu nhỏ đã có: `android/bo_cuc_vnku_mobile.py` đọc `.spr`, thu nhỏ bằng Pillow, ghi lại `.spr`
nhiều khung bảng màu chung qua `ReverseTools/tongkim_chat/ghi_spr.py`. Viết một kịch bản anh em
`android/anh_wauto_vnku.py` theo đúng mẫu đó, **ghi vào lớp ghi đè**, không ghi vào `D:\jx1_android_data`.

**Móc icon vào thanh công cụ** — chỗ móc đã có sẵn, chỉ đang bị ghi chú:

| Chỗ | Làm gì |
|---|---|
| `android/du_lieu_ghi_de/ui/ui3/uitoolscontrolbar.ini` | thêm `Button6=WAuto` vào mục `[Main]` và một mục `[WAuto]` trỏ ảnh icon vừa thu nhỏ. Dòng `; Button11=AutoPlay` **để nguyên đang ghi chú** |
| `Sources/S3Client/Ui/UiShell.cpp:215` | thêm `Player_WAuto::RegisterSelfClass();` **trong `#ifdef JX_ANDROID`**. Dòng `Player_AutoPlay` bên cạnh **giữ nguyên đang ghi chú** |
| `Sources/S3Client/Ui/UiShell.h` | lớp `Player_WAuto : public KWndButton` mới, rào `#ifdef JX_ANDROID`, nhấn là mở hoặc đóng bảng WAuto |

**Lớp cửa sổ mới:** `Sources/S3Client/Ui/UiCase/UiWAuto.cpp` và `.h` — lớp `KUiWAuto : public KWndWindow`
theo đúng khuôn `KUiAutoPlay` đã chạy trên Android: `OpenWindow()`, `GetIfVisible()`, `CloseWindow()`,
`LoadScheme()` đọc `ui\ui3\uiwauto.ini`, đăng ký `SetFitFlags(...)` để tự neo theo màn hình.

**Nội dung khung ở B1**, chưa có tab nào:

- Dải nút nhóm 4 cái: Điều khiển, Hậu cần, Cài đặt, Hoạt động — đúng bảng `s_aNhomTab` của WAuto.
- Dải nút tab con 8 chỗ, để trống.
- Nút **BẬT** và **TẮT** auto, nối thẳng vào công tắc của B0.
- **Dòng trạng thái** ở chân bảng: gọi `WA_HoatDong()` mỗi nhịp, in chuỗi TCVN3 có sẵn, ví dụ
  "Sát Thủ: tới Nhiếp Thị Trấn". Đây là thứ cho chủ thấy máy đang làm gì ngay từ B1, chưa cần một tab nào.
- Nút đóng.

**Nghiệm thu B1:** chạm icon Auto trên thanh công cụ thì bảng hiện đúng khung VNKU, không tràn màn,
không che thanh dưới. Chạm BẬT thì nhân vật bắt đầu tự đánh theo cấu hình từ tệp của B0, dòng trạng thái chạy.
Chạm TẮT thì đứng im. Đổi độ phân giải vẫn đúng chỗ. Hai chuỗi Windows 0 lỗi. Có ảnh chụp đối chiếu.

### B2 — Bộ sinh bố cục và lớp trang chung (~1.300 dòng C++ và ~800 dòng Python)

Đây là bước làm cho 15 tab sau **rẻ đi rất nhiều**. Lý do: trong 243 điều khiển thực sự nối vào `autoData`,
có **231 cái, tức 95 %, đọc ghi theo đúng ba khuôn mẫu lặp lại** trong hàm `SaveRoleData()` của WAuto:
104 ô tick, 75 ô nhập, 52 hộp chọn. Máy đọc được.

| Việc | Sản phẩm |
|---|---|
| `android/sinh_bang_wauto.py` | đọc `WAutoUI/WAuto.rc`, `Resource.h` và hàm `SaveRoleData`, sinh bảng gồm thẻ, mục bố cục, loại widget, trường trong `autoData`, danh sách lựa chọn |
| `android/sinh_bocuc_wauto.py` | từ bảng đó sinh `ui\ui3\uiwauto_<tab>.ini`: **2 cột, bước hàng 48 px**, ô tick 36 px |
| `UiWAutoTrang.cpp` | một lớp `KWndPage` chạy theo bảng: dựng widget, đọc ghi thẳng vào `autoData` theo offset, dựng `KPopupMenuData` từ danh sách lựa chọn |
| `android/nhan_wauto_tcvn3.py` | chuyển 119 nhãn từ UTF-16 sang TCVN3 bằng `vn_edit.vn()`, soi lại chữ hoa có dấu |

Bố cục đã đo là làm được: tab dày nhất là **Chiêu KH, 16 hàng**. Hai cột nhân 8 hàng nhân 48 px bằng
**384 px**, lọt thoải mái vùng nội dung khoảng 940×470 của khung 980×588.

**Nghiệm thu B2:** sinh thử tab **PK** (9 hàng, 23 điều khiển, nhẹ nhất) hoàn toàn bằng máy; mở ra đúng;
tick vào ghi đúng trường trong `autoData`; tắt game mở lại vẫn nhớ.

### B3 đến B17 — Từng tab một, mỗi tab một đợt

Mỗi đợt làm đúng bảy việc, không nhảy cóc:

1. Chạy bộ sinh cho tab đó, ra tệp bố cục.
2. Soi lại nhãn TCVN3, nhất là chữ hoa có dấu.
3. Nối những điều khiển bộ sinh không bắt được, xem cột "tay" ở bảng §4.
4. Thu nhỏ ảnh riêng của tab nếu có.
5. Dựng, chạy LDPlayer, tick từng ô, đối chiếu với WAuto trên PC cùng cấu hình.
6. Dựng lại hai chuỗi Windows, 0 lỗi.
7. Rà ký hiệu trùng theo §0.4, số phải vẫn là 6.

### B18 — Sáu bảng con đặc biệt

Đây là 12 điều khiển bộ sinh **không** làm được, vì chúng là mảng chứ không phải một số:

| Bảng con | Trường trong `autoData` | Ảnh VNKU sẵn có |
|---|---|---|
| Không nhặt theo tên | `szNOPName[60][80]` | `btn_them`, `btn_xoa`, `btn_xoahet` |
| Lọc series | `nFtMagic[40][2]` | `CloseSeriesBtn.spr` |
| Danh sách mời và nhận tổ đội | `szIJPtName[24][32]` | `btn_them`, `btn_xoa` |
| Toạ độ đi tuần | `sMoveCoord[24]` | **`AutoGetCoord\main.spr`, `xong.spr` và `AutoGetCoord.ini` — có sẵn cả bố cục** |
| Sáu khe chiêu kết hợp | `nComboSkill[6]`, `nComboDelay[6]` | `btn_len`, `btn_xuong` |
| Danh sách bạn Liên đấu | `szLDPtName[8][32]` | `btn_them`, `btn_xoa` |

Bảng bắt toạ độ của VNKU là món hời: có sẵn cả ảnh lẫn tệp bố cục, chỉ phải nối vào.

### B19 — Dọn và bàn giao

- Gỡ công tắc tạm `config.ini [WAuto] Bat`.
- Gỡ nhật ký chẩn đoán thêm trong đợt này.
- Viết `BANGIAO_WAUTO_MOBILE_<ngày>.md`.
- Chạy đủ 15 tab một lượt trên LDPlayer, chụp ảnh.
- Chủ chạy thử trên **điện thoại thật**.

---

## 4. BẢNG 15 TAB — số đo thật, dùng để xếp lịch

Đo từ `WAuto.rc` ngày 11/09. Cột **hàng** là số hàng sau khi gộp các điều khiển nằm cùng dòng; đây là con số
quyết định chiều cao trang. Cột **tay** là phần bộ sinh không bắt được.

| Thứ tự làm | Tab | Nhóm | Điều khiển | Hàng | tick | ô nhập | chọn | nút | chữ | tay |
|---|---|---|---|---|---|---|---|---|---|---|
| B3 | **Chiến đấu** | Điều khiển | 33 | 13 | 4 | 6 | 12 | 0 | 11 | 0 |
| B4 | **Phục hồi** | Hậu cần | 38 | 14 | 19 | 11 | 1 | 0 | 7 | 0 |
| B5 | **Nhặt đồ** | Điều khiển | 25 | 11 | 10 | 5 | 2 | 5 | 2 | 2 bảng con |
| B6 | **Di chuyển** | Điều khiển | 23 | 14 | 6 | 1 | 1 | 9 | 5 | 1 bảng con |
| B7 | **Hậu cần** | Hậu cần | 30 | 11 | 14 | 7 | 7 | 0 | 2 | 0 |
| B8 | **PK** | Điều khiển | 23 | 9 | 11 | 3 | 3 | 1 | 5 | 0 |
| B9 | **Cơ bản** | Cài đặt | 38 | 14 | 8 | 4 | 3 | 4 | 18 | bỏ khối đăng nhập |
| B10 | **Tổ đội** | Hậu cần | 20 | 11 | 3 | 3 | 3 | 7 | 3 | 1 bảng con |
| B11 | **Chiêu KH** | Điều khiển | 34 | **16** | 4 | 9 | 8 | 0 | 13 | 1 bảng con |
| B12 | **Dã Tẩu** | Hoạt động | 23 | 14 | 11 | 2 | 4 | 0 | 6 | 0 |
| B13 | **Tống Kim** | Hoạt động | 27 | 14 | 8 | 3 | 6 | 0 | 10 | 0 |
| B14 | **CTC** | Hoạt động | 27 | 11 | 3 | 6 | 6 | 0 | 12 | 0 |
| B15 | **Liên đấu** | Hoạt động | 32 | 15 | 7 | 9 | 3 | 2 | 10 | 1 bảng con |
| B16 | **Sát thủ** | Hoạt động | 11 | 7 | 3 | 2 | 2 | 0 | 4 | 0 |
| B17 | **H.động** | Hoạt động | 26 | 12 | 4 | 9 | 2 | 0 | 11 | 0 |
| | **Cộng** | | **410** | | 115 | 80 | 63 | 28 | 119 | 6 bảng con |

Thứ tự trên xếp theo **giá trị dùng hằng ngày**, không theo thứ tự hiện trên màn. Thứ tự **hiện** giữ đúng
bảng `s_aNhomTab` của WAuto để người chơi PC chuyển sang không phải học lại:

| Nhóm | Tab con theo thứ tự |
|---|---|
| Điều khiển | Chiến đấu, Chiêu KH, PK, Di chuyển, Nhặt đồ |
| Hậu cần | Hậu cần, Phục hồi, Tổ đội |
| Cài đặt | Cơ bản |
| Hoạt động | Dã Tẩu, Tống Kim, CTC, Liên đấu, Sát thủ, H.động |

**Hai tab BỎ.** Mã vẫn giữ nguyên, chỉ không dựng giao diện:

| Tab | Điều khiển | Vì sao bỏ |
|---|---|---|
| Ác chính | 9 | Cần WAuto nhìn nhiều cửa sổ cùng lúc để bơm vị trí chủ tướng. Một điện thoại chỉ một nhân vật |
| Đ.nhập | khoảng 10, dùng chung dải mã của tab 0 | Gọi `CreateProcessA("game.exe")` và quản lý nhiều tài khoản. Bản Android đã có tự đăng nhập riêng, §6.2 bản bàn giao pha 4 |

---

## 5. BẪY ĐÃ BIẾT

| # | Bẫy | Cách tránh |
|---|---|---|
| 1 | **Khung VNKU to hơn khung vẽ.** `khung.spr` 1313×788, khung vẽ điện thoại 1040×604 | Thiết kế bảng thẳng ở khoảng 980×588. Thu nhỏ **ảnh khung**, KHÔNG thu nhỏ `tick_chon.spr`, vì 36 px là cỡ vừa tay |
| 2 | **Nền trang VNKU lệch tỉ lệ 5/3.** `tab_chiendau.spr` là 2048×945 nhưng bố cục ghi 1228×563 | Đọc cỡ thật từ đầu tệp `.spr` rồi mới tính hệ số, đừng tin số trong tệp bố cục |
| 3 | **80 ô nhập cần bàn phím số.** Bố cục VNKU dùng khoá `NumKey=1`, `KWndEdit` bản này **chưa đọc khoá đó** | Thêm `NumKey` vào `WndEdit.cpp`, rào `JX_ANDROID`, nối vào đường bật bàn phím mềm của SDL |
| 4 | **TCVN3 không có chữ HOA có dấu** | Nhãn viết "Chiến đấu", không viết "CHIẾN ĐẤU". Soi lại sau mỗi lần sinh |
| 5 | **`autoData` không được đổi thứ tự trường** | Trường mới **chỉ thêm ở CUỐI struct**, trước constructor. Cảnh báo đã ghi ở 5 chỗ trong `ipc_shared.h` |
| 6 | **Ký hiệu toàn cục trùng tên sai lặng lẽ trên ELF** | Mọi toàn cục mới đặt `static`. Rà theo §0.4, số phải vẫn là 6 |
| 7 | **`am force-stop` rồi `sleep 2` không khởi động lại thật** | Phải `sleep 4` và kiểm `pidof vn.jx1.mobile` trước và sau, §6.3 bản bàn giao pha 4 |
| 8 | **APK đóng gói `.so` cũ** | Kiểm bằng cách tìm một chuỗi mới ngay trong `.so` lấy ra từ APK, §9.5 bản bàn giao pha 4 |
| 9 | **Hai bộ auto cùng điều khiển một nhân vật** | Bảng WAuto và `KUiAutoPlay` loại trừ nhau. `fkauto` vốn đã tắt sẵn ở `UiShell.cpp:215`, giữ nguyên |

---

## 6. SỐ ĐO THAM CHIẾU (đo 11/09, dùng để ước công)

| Thứ | Số |
|---|---|
| `sizeof(autoData)` đóng gói `pack(1)` | 7.644 byte |
| `SHARED_SIZE` | 65.536 byte |
| Nhịp gửi của WAuto (`GAMELOOPINTV`) | 54 ms |
| Hàm máy auto trong `CoreShell.cpp` | 180 hàm, trải khoảng 12.800 dòng |
| `WAutoUI/WAuto.cpp` | 7.675 dòng UTF-16, trong đó khoảng 4.900 là vỏ Win32 |
| Điều khiển trong `WAuto.rc` | 501, trong đó 500 có mã `IDC_` |
| Điều khiển **có nối** vào `autoData` | 243 |
| trong đó **máy móc hoá được** | 231, tức 95 % |
| Điều khiển **chỉ trang trí** (nhãn, khung, vạch) | khoảng 257 |
| Điều khiển **giữ lại** cho mobile | 410 có nhãn thẻ, cộng khoảng 30 dùng chung |
| Ảnh gốc cần từ kho VNKU | **24**, vì ô tick dùng lại 58 lần và nút thả xuống 25 lần |
| Mục bố cục VNKU **không cần ảnh** | 214 trên 348 |
| Mốc hiệu chuẩn: `UiAutoPlay` đã chạy trên Android | 191 widget, 118 hàm, 5.286 dòng, tức **27 dòng cho một widget** |
| Ước công đường viết tay | khoảng 11.000 dòng C++ và 2.900 dòng bố cục |
| **Ước công đường sinh bảng, chọn đường này** | **khoảng 3.200 dòng C++ viết tay, 1.100 dòng Python, 2.900 dòng bố cục do máy sinh** |
| Ký hiệu dữ liệu trùng giữa các `.so`, mốc không được tăng | 6 |
