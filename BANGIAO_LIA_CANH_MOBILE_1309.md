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


---

# BƯỚC 2 (13/09 tối, sau khi chủ thử xong bước 1): ZOOM NHÌN RỘNG RA BẰNG HAI NGÓN — bản 109132104

> Chủ: *"oke được rồi tôi test được làm bước tiếp theo"*. Làm theo **cách C** của `PHANTICH_ZOOM_CANH_1209.md` (đã cân nhắc lại 12/09): thế giới vẽ vào ảnh đệm **to hơn khung** rồi **thu nhỏ** khi blit, giao diện giữ nguyên; tái dùng đúng đường `[TG]` `Rep3_JxTheGioi` đã có.

## 1. Bản đã lên dt_v4

| | |
|---|---|
| APK | `android/apk/jx1mobile-1309-zoom-b.apk` → `D:\jx1_android_data_dt_v4\jx1mobile.apk`, versionCode **109132104**, md5 `6bcf743211e0c128a3ad678a60cda11c`; 8765 khởi động lại |
| Lùi | `[Cham] ZoomCanh=0` (tắt chụm) + restart 8765; lia bước 1 vẫn còn. Lùi cả hai: `LiaCanh=0`. Bản trước: 109132032 (lia, chưa zoom) |
| Config dt_v4 | thêm khối `[Cham] Zoom*` (mục 3) |

## 2. Cách dùng

- **Hai ngón đặt lên bản đồ** (cả hai ngoài giao diện / cần / nút kỹ năng) rồi **chụm lại = nhìn rộng ra**, giãn ra = về 1:1; tối đa **150 %** (rộng gấp 1,5 lần mỗi chiều). Vừa chụm vừa kéo thì cảnh **lia theo tâm hai ngón**; nhả một ngón = hết chụm, 1 s sau lia tự về (zoom **giữ nguyên** tới khi đổi, kể cả qua map).
- Nhân vật, quái, tên, thanh máu nhỏ theo tỉ lệ (vì nằm trong ảnh đệm); bảng, nút, chữ giao diện **giữ nguyên cỡ**. Chạm chọn NPC / chạm đất đi đúng chỗ ở mọi mức zoom (một "cửa" đổi toạ độ cho cả vẽ lẫn chạm).

## 3. Khoá `[Cham]` thêm

```
ZoomCanh=1        ; 0 = tat chum hai ngon
ZoomToiDa=150     ; % nhin rong toi da (150 = 2,25 lan dien tich -> GPU + nen dat theo ti le)
ZoomBuoc=5        ; nac zoom, %
ZoomMacDinh=100   ; zoom luc vao game
ZoomThu=0         ; go loi: % zoom tu dat 3 s sau khi vao the gioi (may ao khong co hai ngon)
```

## 4. Đã làm gì trong mã (kịch bản `android/va_nguon_zoom_1309.py`, idempotent; mọi dòng trong rào `JX_MOBILE`)

| Tệp | Việc |
|---|---|
| `Represent3/KRepresentShell3.h` | thành viên `m_nTgZoom` (phần nghìn), `m_nZoomDx/Dy` (gốc RT lùi so với gốc khung), `m_nTgKhungW/H`, `m_nTgLeftKhung/TopKhung` — trong khối `JX_MOBILE` của `[TG]` |
| `Represent3/KRepresentShell3.cpp` | `JxTheGioi`: lệnh **4** đặt zoom / **5** hỏi; lệnh 0: zoom > 1000 → luôn vẽ RT (K = 1), RT = khung × zoom (cấp lại khi đổi cỡ); lệnh 1: lưu gốc + cỡ khung, **lùi `m_nLeft/m_nTop` = Dx/Dy** để tiêu điểm nằm giữa RT, tạm đặt `g_nScreenWidth/Height` = cỡ RT (cull/cắt trong shell theo RT); lệnh 2: trả lại cỡ khung + gốc khung, **đích blit = cỡ khung** (trước là cỡ RT → vẽ 1:1 rồi cắt — lỗi tôi bắt được bằng ảnh: nhân vật lệch đúng 130 px = (1300−1040)/2), lọc **tuyến tính** khi thu nhỏ; `CoordinateTransform` + `CoordinateTransformX` (nhánh 2D, chỉ khi không đang vẽ RT): `(x − gốc khung + Dx)·1000/zoom`; `ViewPortCoordToSpaceCoord` (nhánh 2D): `x·zoom/1000 − Dx + gốc khung` |
| `KSdlApp.h/.cpp` | `m_nLiaChum`; nhớ ngón 1 ngoài chế độ sửa; ngón 2 đặt lên bản đồ khi ngón 1 đang CHO/LIA/KEO và cả hai điểm được lia → huỷ chuột giả lập ngón 1 (KEO thì gửi `WM_LBUTTONUP`), `JxLia_ChumBatDau/Keo/Nha`; ngón nào nhả trước là hết chụm |
| `JxLiaCanh.h/.cpp` | `JxLia_ChumDuoc/ChumBatDau/ChumKeo/ChumNha`, `JxLia_ZoomDat/ZoomLay`: zoom = zoom0 · d0/d làm tròn `ZoomBuoc`, kẹp 100..`ZoomToiDa`; áp = `Rep3_JxTheGioi(4, zoom·10)` (GetProcAddress như `Wnds.cpp`) + `g_pCoreShell->SetRepresentAreaSize(khung × zoom)` (vùng truy vấn vật thể, không thì người ở rìa không được vẽ); lia theo tâm hai ngón; giới hạn lia nhân theo zoom; đổi map áp lại vùng truy vấn (Rep3 giữ zoom); `ZoomThu` gỡ lỗi |

Không đụng: `Core`, `Wnds.cpp`, `D3D9onGPU` (lớp GPU tự đặt viewport = cỡ RT khi `SetRenderTarget`, đã đọc `CDevGpu::SetRenderTarget`).

## 5. Chứng minh

- `ios/kiem_android_tuongduong.py --pc`: **DAT — Windows biên dịch y hệt** (mọi dòng mới trong `JX_MOBILE`).
- MSBuild `Represent3.vcxproj` `/t:ClCompile KRepresentShell3.cpp` Release|Win32 **và** Release|x64: 0 lỗi, `.obj` dịch lại sau vá (21:04). `/t:Rebuild` Win32 dừng ở **LNK1181 `d3dx9.lib`** (máy này không có DirectX SDK — lỗi môi trường, không phải mã; máy chủ dựng bản PC ở cây thật của chủ).
- `check_encoding.py`: `KRepresentShell3.cpp` 3780 / `.h` 5392 byte cao trước = sau.
- Máy ảo (`ZoomThu=125`, không có hai ngón): log `[ZOOM] zoom 1000 -> 1250` + `[TG] render target the gioi 1300x756`; ảnh: **nhân vật đúng giữa màn**, thế giới nhỏ 80 %, thấy rộng hơn 25 % mỗi chiều, tên/thanh máu đúng chỗ, giao diện nguyên cỡ, không mép đen; lia 200 px ở zoom → lệch 195, về sau 1 s; chạm đất ở vùng mới lộ → nhân vật đi tới; `[TG]` việc/khung thế giới 0,7 ms, 60 fps đều.
- Chi phí lý thuyết trên Fold 7: 150 % = 2,25 lần điểm ảnh lớp thế giới + 1 blit toàn màn (GPU 61–63 % ở 120 Hz K=1 hôm 13/09 → có thể chạm trần ở Tống Kim đông khi zoom 150 %); `[TG]` K=2 tắt khi zoom nên không có "xen khung". Cần đo `[MAU]` GPU % và `[PGND]` khi chủ zoom 150 % ở chỗ đông.

## 6. Chủ thử trên Fold 7

1. Mở lại app nhận **109132104**. Vào thành: hai ngón chụm lại → thế giới nhỏ dần, thấy rộng ra; giãn → về 1:1. Bảng, nút không đổi.
2. Ở 150 %: chạm NPC ở rìa → mở đúng thoại; chạm đất → đi đúng chỗ; kéo một ngón lia rồi thả 1 s về.
3. Qua cổng đổi map: zoom giữ, người ở rìa vẫn được vẽ (nếu thấy người/quái biến mất ở mép khi zoom → báo tôi, đó là vùng truy vấn).
4. Tống Kim đông ở 150 %: xem fps/nóng; kéo log về, tôi đọc `[TG]`, `[MAU]` (GPU %, W), `[PGND]`.
5. Không thích chữ nhỏ theo zoom → đó là bản chất cách C; muốn chữ giữ cỡ phải làm cách A (35 chỗ chữ neo thế giới, mã dùng chung PC) — chủ quyết sau khi thử.
