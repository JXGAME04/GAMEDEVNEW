# BÀN GIAO 13/09 — LIA CẢNH MOBILE BƯỚC 1a (một ngón kéo trên bản đồ = lia, thả 1 s tự về)

> Chủ 13/09: *"Bạn thiết kế sao giống bản 3D để tiện sử dụng - bạn làm các bước tôi test"* (sau khi chọn mức 1 của
> `PHANTICH_KIEMVONG_GIANGHO_3D_1309.md` và đọc `PHUONGAN_LIA_CANH_MOBILE_1309.md`). Nhánh `claude/game-3d-data-analysis-19600d`
> = `origin/mobile-0809` (`9fcdc3d4`, có NENTRUOC 109131908) + việc này. PC không đổi hành vi.

## 1. Bản đã lên dt_v4

| | |
|---|---|
| APK | `android/apk/jx1mobile-1309-lia-b.apk` → `D:\jx1_android_data_dt_v4\jx1mobile.apk`, versionCode **109132032**, md5 `3f2032afb861f12e5d6f472944cc4e21`, 20 625 583 B; máy chủ 8765 khởi động lại (apk.txt + manifest sinh lại) |
| Dựng từ | worktree này qua junction `D:\GAMEDEVNEW_wt_lia` (chép `vcpkg_installed`, `SDL3-src/SDL3-3.2.30` từ `wt_bg` — cùng hint `JX_DEM_SUBOPTIMAL`/`JX_SWAPCHAIN_W` và cùng cỡ `libSDL3.so` với bản 109131908 đang chạy), `gradlew assembleDebug` 2 m 33 s (lần đầu) |
| Bản trước để lùi | 109131908 (`eec40e37…`) — hoặc nhanh hơn: `[Cham] LiaCanh=0` trong `config.ini` dt_v4 + khởi động lại 8765, không cần APK |
| Config dt_v4 | thêm khối `[Cham] Lia*` (mục 3), `LiaNhatKy=1` để tôi đọc `[LIA]` trong log điện thoại |
| Máy ảo | đã cài 109132032, config `D:\jx1_android_data\config.ini` có khối Lia (`LiaNhatKy=1`); WAuto của chủ tự chạy lại sau khi cài (đang báo danh Tống Kim lúc 10:35 giờ máy ảo) |

## 2. Cách dùng (giống game 3D)

- **Một ngón kéo trên bản đồ** (chỗ không có bảng, không phải vùng cần bên trái, không phải nút kỹ năng) = **lia khung nhìn theo tay 1:1**;
  kéo sang phải là xem phần bên trái, kéo lên là xem phần bên dưới. Giới hạn 60 % cỡ khung mỗi chiều và không ra ngoài biên bản đồ.
- **Thả tay: chờ 1 s rồi khung tự trôi về nhân vật** (tốc độ tối thiểu 700 px/s, có êm cuối). Nhân vật bước đi (chạm đi, auto đi), cầm cần, đổi map → về ngay.
- **Chạm** (không kéo) vẫn là bấm chuột trái như cũ; **giữ 400 ms** vẫn là chuột phải; **cần điều khiển** bên trái vẫn thế; **nút kỹ năng**, **kéo trên giao diện** (cuộn danh sách, kéo vật phẩm) không đổi.
- Điều **mất đi** khi bật: "kéo trên bản đồ = giữ chuột trái rồi rê đi liên tục" (kiểu PC). Đi bằng cần hoặc chạm điểm đến. Muốn lại như cũ: `LiaCanh=0`.

## 3. Khoá `config.ini` `[Cham]` (đã có trong `android/du_lieu_ghi_de/config.ini`, dt_v4, máy ảo)

```
LiaCanh=1        ; 0 = tat han
LiaXaNgang=60    ; lech toi da ngang, % be rong khung ve
LiaXaDoc=60      ; lech toi da doc, % chieu cao khung ve
LiaNguong=3      ; tay di it hon bao nhieu px thi chua tinh
LiaChoVeMs=1000  ; nha tay bao lau thi bat dau ve
LiaVeTocDo=700   ; toc do ve toi thieu, px/giay
LiaVeEm=3        ; he so em cuoi (0 = di deu)
LiaVeKhiDi=1     ; nhan vat buoc di / cam can -> ve ngay
LiaNhatKy=0      ; 1 = ghi [LIA]
LiaThu=0,0       ; go loi: dx,dy -> 3 s sau khi vao the gioi tu lia roi tu ve
```

## 4. Đã làm gì trong mã (chỉ `JX_MOBILE`; kịch bản `android/va_nguon_lia_1309.py` idempotent, giữ byte TCVN3)

| Tệp | Việc |
|---|---|
| `Sources/S3Client/Platform/JxLiaCanh.h/.cpp` (mới, 330 dòng) | trạng thái `KHONG → KEO → CHO_VE → VE`; dịch tiêu điểm qua `g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP=1, GSMOI_SCENE_MAP_FOCUS_OFFSET)` (cơ chế có sẵn của JX1, bản PC dùng cho Ctrl+chuột phải kéo bản đồ nhỏ); `JxLia_Nhip` áp lại lệch **mỗi khung** (bẫy `FollowWithMap`: cờ bật thì khung không theo nhân vật); kẹp theo % khung + `nFocusMin/Max`; Y thế giới = 2 × px; phát hiện nhân vật đi bằng `nOrigFocus` đổi giữa hai nhịp; kết thúc = `FOLLOW=0` rồi `OFFSET=0` (thứ tự của `KUiMiniMap::MapMoveBack`) |
| `KSdlApp.h` | `CHAM_LIA` |
| `KSdlApp.cpp` | include; `JxLia_Nhip()` trong vòng lặp cạnh `JxCan_Nhip()`; trong nhánh "đã xê dịch > 12 px": sau nhánh CẦN, trước nhánh CUỘN: `JxLia_DuocBatDau(x0,y0)` → `CHAM_LIA`; `MOUSE_MOTION` khi `CHAM_LIA` → `JxLia_Keo`; nhả → `JxLia_Nha` |
| `Ui/GameSpaceChangedNotify.cpp` | `case GDCNI_SWITCHING_MAPMODE` (đổi map thật) → `JxLia_DatLai()` (rào `JX_MOBILE`) |
| `android/lists/s3client.cmake` | thêm `JxLiaCanh.cpp` (Android + iOS dùng chung list) |
| `android/du_lieu_ghi_de/config.ini` | khối `[Cham] Lia*` |

Không đụng: `Represent3`, `Core`, `Wnds.cpp`, `S3Client.vcxproj` (PC/GameSDL không dịch tệp mới; `KSdlApp` chỉ có trong ReleaseSDL và mọi dòng thêm nằm trong `JX_MOBILE`).

## 5. Chứng minh

- `python ios/kiem_android_tuongduong.py --pc`: `KSdlApp.cpp/.h` **giống hệt** với bộ macro Windows; `GameSpaceChangedNotify.cpp` chỉ khác **một dòng `#include`** của header rỗng ngoài `JX_MOBILE` (công cụ báo KHÁC, đã xem tay).
- MSBuild `S3Client.vcxproj` Release|Win32 `/t:ClCompile /p:SelectedFiles=Ui\GameSpaceChangedNotify.cpp`: **0 Error(s)**, 16 s.
- `check_encoding.py` trước/sau: `GameSpaceChangedNotify.cpp` 325 byte cao = 325, FFFD 0; các tệp khác thuần ASCII.
- Máy ảo LDPlayer (1040×604, TestBot, bản a rồi bản b), `adb shell input swipe`:
  - kéo 200 px sang trái: `[LIA] bat dau tai 620,350` → `nha tay: lech 196,0 -> cho 1000 ms` → `bat dau ve (het cho)` → `ket thuc (ve xong)`; ảnh chụp: cảnh dịch 200 px, **tên/thanh máu/cờ đứng đúng trên đầu nhân vật**, giao diện đứng yên; 3 s sau về đúng chỗ cũ.
  - kéo rồi **chạm đất ở vùng vừa lộ ra**: nhân vật đi tới đúng chỗ chạm (chạm theo toạ độ đã lia), `bat dau ve (nhan vat di)` sau 0,39 s.
  - kéo khi có **hộp thoại NPC** đang mở đúng chỗ đặt ngón: không lia (giao diện ăn chạm trước) — đúng luật.
  - bản b (700 px/s, êm 3): 138 px về trong 0,19 s.
- WAuto trên máy ảo tự chạy lại sau khi cài, không phải bật tay.

## 6. Chủ thử trên Fold 7 (mục 8 phương án, rút gọn)

1. Mở lại app nhận bản 109132032. Vào thành: kéo một ngón giữa màn 4 hướng → cảnh theo tay, tên/thanh máu người khác đúng chỗ.
2. Thả tay 1 s → trôi về, không giật. Muốn về chậm/nhanh hơn: `LiaVeTocDo`, `LiaVeEm`; muốn chờ lâu hơn: `LiaChoVeMs`.
3. Đang lia, chạm NPC ở rìa → mở đúng thoại; chạm đất → đi tới đúng chỗ.
4. Kéo tới mép: dừng ở 60 % màn, không mép đen (nếu thấy mép đen báo tôi kèm map).
5. Qua cổng đổi map / Tống Kim đông ở 120 Hz (K=2): lia không xé hình; kéo log về, tôi đọc `[LIA]` + `[PGND]` xem lia có làm dựng nền tăng không.

## 7. Còn lại

- Bước 2: **zoom nhìn rộng ra** bằng hai ngón (cách C, thế giới vẽ vào RT to hơn rồi blit thu nhỏ, tái dùng đường `[TG]`) — chỉ sau khi đo mép đen và `[PGND]` trên Fold 7.
- Bước 3: tham số theo map (như `cameraInit` của họ) + nút bật/tắt lia trong trình chỉnh giao diện.
- Bẫy gặp trong phiên: heredoc Bash nuốt `\` và `'` → viết tệp bằng công cụ Write rồi đổi CRLF; robocopy rc=1 là thành công; gradle lần hai báo "3 s" nhưng vẫn dịch lại tệp đổi (kiểm `.o` mtime); máy ảo mở về **menu** sau khi cài (không tự đăng nhập) → tôi bấm: Bắt đầu → chọn máy chủ 2 cấp → Xác nhận (mật khẩu đã ghi nhớ) → chọn nhân vật.
