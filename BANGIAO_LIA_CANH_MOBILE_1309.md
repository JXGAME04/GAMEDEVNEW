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


---

# BƯỚC 3 — THAM SỐ CAMERA THEO MAP (như `cameraInit` của game 3D) — 13/09 22:16

## 1. Bản đã lên dt_v4

| | |
|---|---|
| APK | **109132210** (`jx1mobile.apk`, md5 `4ffb3c99d7c40f1e16e9f571fabecb65`), commit `e013bafe` trên `claude/game-3d-data-analysis-19600d`, đã **gộp `mobile-0809` 8580129f** (XOANEN + NENTRUOC b + đo `[PGND-R]` của phiên đo nhịp) và FF-push lên `mobile-0809` |
| Dữ liệu | `settings/camera_mobile.ini` (mới) + `config.ini` `[Cham]` thêm `ZoomNho/ZoomTocDo/ZoomChu` — máy chủ 8765 khởi động lại 22:16 (apk.txt + manifest mới) |
| Lưu ý | 21:55 tôi ghi `config.ini`/`camera_mobile.ini` vào dt_v4 **sau** khi manifest được sinh → điện thoại báo "Tải tệp cập nhật bị lỗi"; phiên đo nhịp sinh lại manifest. **Từ nay: đổi tệp trong dt_v4 → `may_chu_tai_du_lieu.py --chi-manifest` (hoặc restart) ngay.** apk.txt chỉ sinh lúc máy chủ khởi động → đổi APK phải restart. |

## 2. Cách dùng (giống game 3D: mỗi cảnh một bộ tham số)

- Vào map / đổi map: **1 s** sau khi Core có map, đọc `settings\camera_mobile.ini` mục `[Map_<id>]` (id = số thứ tự trong `maplist.ini`: 1 Phượng Tường, 11 Thành Đô, 37 Biện Kinh, 44 Tống Kim, 78 Tương Dương, 80 Dương Châu…) rồi **zoom trôi dần** tới đích (`ZoomTocDo` %/s, mặc định 100 → 20 % mất 0,2 s), chữ vàng **"Nhìn rộng NNN%"** ở 1/5 màn trong 1,5 s.
- Đích = zoom người chơi **đã chụm** (nhớ ở `UserData\CameraMobile.ini`, `ZoomNho=1`) — nếu chưa chụm bao giờ thì `ZoomMacDinh` của map; luôn kẹp theo `ZoomToiDa` của map; map `ZoomCanh=0` → về 100 %.
- Đang chụm thì không trôi; chụm xong ghi nhớ ngay. Xoá `UserData\CameraMobile.ini` (hay `ZoomNho=0`) để quay về mặc định theo map.
- `[Map_<id>]` cũng ghi đè `LiaXaNgang/LiaXaDoc/LiaCanh/ZoomCanh`; map `LiaCanh=0` thì chụm vẫn zoom nhưng không lia (trước bước 3 chụm phụ thuộc lia).
- Bản mặc định trong tệp: thành lớn (1, 11, 37, 78, 80) `ZoomMacDinh=120`; Tống Kim (44) `ZoomToiDa=125` (đông người, giữ GPU); còn lại 100 / 150. Chủ chỉnh số trong tệp rồi `--chi-manifest`, không cần dựng lại.

## 3. Khoá `[Cham]` thêm (config.ini)

| Khoá | Mặc định | Ý nghĩa |
|---|---|---|
| `ZoomNho` | 1 | nhớ zoom người chơi qua `UserData\CameraMobile.ini` (thắng `ZoomMacDinh` của map) |
| `ZoomTocDo` | 100 | %/giây khi trôi tới zoom của map (kẹp 10..1000) |
| `ZoomChu` | 1 | chữ "Nhìn rộng NNN%" 1,5 s khi zoom đổi |

## 4. Đã làm gì trong mã (kịch bản `android/va_nguon_camera_map_1309.py`, idempotent, mọi dòng trong `JX_MOBILE`)

| Tệp | Việc |
|---|---|
| `JxLiaCanh.cpp` | `Camera_DocMap()` (đọc `KUiSceneTimeInfoOften` qua `GSMOI_SCENE_TIME_INFO_OFTEN` → `nSceneId`, `GetPrivateProfileInt` mục map / `[MacDinh]` / config), tách `ZoomAp()` (áp thật) khỏi `JxLia_ZoomDat()` (người chơi đặt → huỷ trôi + nhớ), trôi trong `JxLia_Nhip` (`s_nZoomDich`, `s_fZoomTroi`), `Camera_GhiNho()` khi thả chụm, `JxLia_Ve()` chữ báo (`OutputText` cỡ 12, vàng viền đen như `PerfHud_Chu`, y = 1/5 màn — đặt 1/3 màn trùng nhãn tên trên đầu nhân vật, ảnh máy ảo không thấy), `TrenBanDo()` dùng chung cho lia một ngón và chụm; `JxLia_Nhip` không còn thoát sớm khi `LiaCanh=0` (vẫn đọc map + trôi); `JxLia_DatLai` đặt `s_nCanDocMap=1`. Chuỗi "Nhìn rộng" là TCVN3 thật (kịch bản mã hoá bằng `vn_edit.vn`, không gõ tay byte) |
| `JxLiaCanh.h` | `JxLia_Ve()` |
| `Ui/UiShell.cpp` | `#include JxLiaCanh.h` + `JxLia_Ve()` ngay trước `JxCan_Ve()` (trong khối `JX_MOBILE` của `UiPaint`) |
| `android/du_lieu_ghi_de/settings/camera_mobile.ini` | mới; `config.ini` thêm 3 khoá |

Không đụng: `Core`, `Represent3`, `KSdlApp`, `Wnds.cpp`, `ios/JxIosMain.cpp` (không thêm ký hiệu Rep3 mới).

## 5. Chứng minh

- `ios/kiem_android_tuongduong.py --pc`: **ĐẠT** (3 tệp, Windows biên dịch y hệt HEAD). MSBuild `S3Client.vcxproj /t:ClCompile Ui\UiShell.cpp` Release|Win32: 0 lỗi, `UiShell.obj` 21:59.
- `check_encoding.py`: `UiShell.cpp` 22 byte cao trước = sau; `JxLiaCanh.cpp` 0 → 2 (đúng hai chữ ì, ộ), FFFD 0, CRLF đều.
- Máy ảo (LDPlayer, 3 lần đăng nhập, nhân vật chủ auto Dã Tẩu tự chạy lại sau mỗi lần):
  - Tương Dương (78): `[CAMERA] map 78: zoom mac dinh 120%… dang 100% -> troi toi 120%` → `[ZOOM] 105/110/115/120%` trong 0,17 s; ảnh: thế giới rộng ra, giao diện nguyên.
  - Auto dịch sang Khoả Lang động (75): `dang 120% -> troi toi 100%` (không có `[Map_75]`), chữ "Nhìn rộng 105%" hiện đúng vị trí, TCVN3 đúng (ảnh u22).
  - Nhớ zoom: `UserData\CameraMobile.ini` `Zoom=135` → `[CAMERA] theo map: … da nho 135%` → trôi 100→135 % với `ZoomTocDo=5` (kẹp 10 %/s → 3,5 s), đổi map giữ 135 %.
- Chưa thử được trên máy ảo: chụm thật (ghi nhớ khi thả chụm) — chủ thử trên Fold 7.

## 6. Chủ thử trên Fold 7

1. Mở lại app nhận **109132210**. Vào Phượng Tường / Tương Dương / Thành Đô: 1 s sau thấy thế giới **tự rộng ra 120 %** kèm chữ "Nhìn rộng 120%"; ra khỏi thành (map thường) → tự về 100 %.
2. Chụm hai ngón đặt 140 % → từ đó mọi map giữ 140 % (đã nhớ), riêng Tống Kim bị kẹp 125 %. Thoát app mở lại vẫn 140 %.
3. Muốn về mặc định theo map: xoá `UserData\CameraMobile.ini` trên máy (hoặc tôi đặt `ZoomNho=0`).
4. Muốn đổi số cho map nào: nói tôi id/tên map + %, tôi sửa `camera_mobile.ini` + `--chi-manifest`, không cần bản mới.

## 7. Sự cố chủ báo 22:13 "Giờ quay một lần có màng đen bị đen"

- Lúc đó điện thoại chạy **109132150** (bản của phiên đo nhịp, apk.txt 21:50, phiên log 22:10:16), **không có** mã lia/zoom/camera. Log phiên đó: `[PGND-R] vung (96,94) #38: xoa 156,1 ms + ghep 0,0 (0 luot, 0 anh)` và `#40: xoa 20,7 + ghep 0,0 (0 luot, 0 anh)` — vùng nền bị xoá đen trên GPU (XOANEN) mà không ghép ảnh → nghi là "màng đen". Đã chuyển cho phiên đo nhịp (chủ XOANEN/NENTRUOC) kèm dấu vết.
- Bản 109132210 gộp đúng mã đó nên nếu còn màng đen thì vẫn là lỗi ấy, không phải lia/zoom (tắt thử: `[Cham] ZoomCanh=0 LiaCanh=0`).

## 8. Còn lại

- Nút bật/tắt lia/zoom trong trình chỉnh giao diện (phương án §7) — chưa làm.
- Chữ neo thế giới nhỏ theo zoom (bản chất cách C) — chủ quyết sau khi thử.
- Nếu chủ muốn nhớ zoom **theo từng map** thay vì một số chung: thêm dòng `Zoom_<id>=` vào `CameraMobile.ini` (30 phút).

**Cập nhật 22:24 (phiên đo nhịp xác nhận):** màng đen là lỗi NENTRUOC của họ: lúc vào/quay lại map, 8 vùng nền KỀ BÊN (đang trên màn hình) bị hoãn ghép chờ luồng nền, luồng nền ngập 322 tệp spr → quanh nhân vật đen tới ~1,5 s. Họ sửa trên `mobile-0809` (vùng kề bên ghép ngay, chỉ hoãn vùng xa; bỏ đọc ngược GPU 156 ms) và sẽ nhắn trước khi chép APK. Bản 109132210 vẫn còn lỗi ấy tới lúc đó.

---

# HƯỚNG 2 — CÔNG TẮC NGƯỜI CHƠI TRONG CÀI ĐẶT (chủ 22:5x: "làm 2 đi") — 13/09 22:55

Chủ hỏi "tôi chưa quay camera được 90 độ": bản 2D không quay được (nền vẽ sẵn một góc, nhà/vách nằm trong ảnh nền); đưa hai hướng, chủ chọn **hướng 2** = giữ mức 1 và làm cho tiện dùng.

## 1. Bản đã lên dt_v4

| | |
|---|---|
| APK | **109132248** (`jx1mobile.apk`), gộp `mobile-0809` hiện tại + công tắc; commit trên `claude/game-3d-data-analysis-19600d`, FF-push `mobile-0809` |
| Dữ liệu | tệp rời mới `ui/ui3/uioptions2.ini` (lớp ghi đè `android/du_lieu_ghi_de/ui/ui3/`, sinh bằng `android/sinh_uioptions2_camera.py`) + `--chi-manifest` |

## 2. Cách dùng

Cài đặt (icon bánh răng) → **TÙY CHỌN** → bấm **Tối ưu** → cửa sổ "Tối ưu" nay có 7 công tắc, 3 mục mới ở hàng 3–4:
- **Lia cảnh**: tắt = kéo một ngón không lia (chụm hai ngón vẫn nhìn rộng nhưng không lia theo tâm).
- **Nhìn rộng**: tắt = chụm không tác dụng, thế giới trôi về 100 %; bật lại = trôi về zoom đã nhớ / mặc định của map.
- **Lia về nhanh**: bật = thả tay chờ 1/3 thời gian (333 ms thay vì 1 s), trôi về nhanh gấp đôi.

Lưu theo tài khoản ở `UserData\<tài khoản>\uiautoconfig.ini` mục `[Options2]` (`LiaCanh`, `NhinRong`, `LiaVeNhanh`), đọc lúc vào thế giới như các mục Giảm Player… Mặc định: lia bật, nhìn rộng bật, về nhanh tắt. Hiệu lực = config.ini / map (`camera_mobile.ini`) **và** công tắc: map tắt hoặc người chơi tắt đều tắt.

## 3. Đã làm gì trong mã (`android/va_nguon_camera_tuychon_1309.py`, idempotent; mọi dòng trong `JX_MOBILE`; PC y hệt: `kiem --pc` ĐẠT)

| Tệp | Việc |
|---|---|
| `Ui/UiCase/UiOptions2.h` | enum thêm `OPTION_I_LIA / NHINRONG / VENHANH`; `MAX_TOGGLE_BTN_COUNT` 7 **chỉ trong lớp KUiOptions2** (typedef kiểm bằng OPTION_INDEX_COUNT2), cuối header trả về 4; `PaintWindow()` |
| `Ui/UiCase/UiOptions2.cpp` | tên lưu `LiaCanh/NhinRong/LiaVeNhanh`; `ToggleOption` → `JxLia_DatTuyChon`; `LoadSetting` mặc định bật hai mục đầu + áp `JxLia_DatTuyChon` lúc vào thế giới; `PaintWindow` vẽ nền mờ (`JxNhip_VeNen`) sau các hàng vì `main2.spr` trong suốt ở vùng trên |
| `Platform/JxLiaCanh.h/.cpp` | `JxLia_DatTuyChon(lia, zoom, veNhanh)`; `s_nNcLia/s_nNcZoom/s_nNcVeNhanh`; `ChoVeMs()/VeTocDo()/VeEm()`; tắt lia → `KetThuc`; tắt zoom → `s_nZoomDich=100`; bật lại → `s_nCanDocMap=1` |
| `ui/ui3/uioptions2.ini` (lớp ghi đè) | `[ToggleBtn]/[ToggleStatus] Top 163 → 101` (4 hàng vừa vùng trống), `[ToggleOptionsName] 4=Lia cảnh 5=Nhìn rộng 6=Lia về nhanh` (TCVN3 mã hoá bằng `vn_edit.vn`) |

**Bẫy đã dính (22:46, app sập khi mở Cài đặt):** `MAX_TOGGLE_BTN_COUNT` được `#define` bên trong lớp ở cả `UiOptions.h` lẫn `UiOptions2.h`; `UiOptions.cpp` include cả hai → đổi macro trong `UiOptions2.h` làm các vòng `for` của `KUiOptions` chạy tới 7 trên mảng 4. Sửa: cuối `UiOptions2.h` trả macro về 4, `UiOptions2.cpp` đặt lại 7 sau include cuối.

## 4. Chứng minh (máy ảo, bản 109132248)

- Ảnh `tc1`: cửa sổ Tối ưu 7 công tắc + nền mờ; `tc2`: bấm tắt Lia cảnh, Nhìn rộng → ô trống; log `[CAMERA] tuy chon nguoi choi: lia=0 nhin rong=1` rồi `lia=0 nhin rong=0`.
- Tắt lia → kéo 220 px: không có dòng `[LIA]`; bật lại → `[LIA] bat dau tai 520,470 … nha tay … ve xong`.
- Về nhanh: về xong sau 0,15 s (thường 0,3 s); tắt lại → `uiautoconfig.ini [Options2] LiaCanh=1 NhinRong=1 LiaVeNhanh=0`.
- Không sập (pid giữ nguyên qua 3 lần mở Cài đặt).

## 5. Chủ thử trên Fold 7

1. Mở lại app nhận **109132248**. Cài đặt → TÙY CHỌN → Tối ưu: thấy 3 công tắc mới.
2. Tắt "Lia cảnh" → kéo một ngón không lia; bật lại → lia như cũ. Tắt "Nhìn rộng" → về 100 %, chụm không ăn; bật lại → về mức cũ.
3. Bật "Lia về nhanh" → thả tay về gần như ngay.


---

# LẮC NHẸ CAMERA KHI LIA (trái/phải + lên/xuống) — 14/09 00:4x

Chủ hỏi quay 90°, rồi 45°, 15°, và góc camera cúi/ngẩng. Tôi gửi mô phỏng trên ảnh (`mo_phong_quay90.png`, `mo_phong_45_25d.png`, `mo_phong_15_5_cui.png`): nền vẽ sẵn 2:1 làm nhà đổ nghiêng gấp đôi góc xoay; chủ chốt *"làm 2 đi"* (lắc nhẹ) và *"làm xoay qua trái và phải, trên dưới cần vậy cũng oke"*, 15° là ảnh thử để nhìn rõ, mặc định 5°.

## 1. Bản đã lên dt_v4

| | |
|---|---|
| APK | **109140035** dựng xong, thử máy ảo; **chưa chép dt_v4**: phiên đo nhịp gộp vào bản [TAI 14/09] của họ và giao MỘT lần (chủ dặn họ "test luôn một lần"), họ sẽ báo "bản X đã lên" ghi rõ có camera lắc. Kịch bản `android/va_nguon_lac_1409.py` + `va_nguon_lac_1409_b.py` (b gồm cả lắc dọc và giữ lề 2 s) |
| Dữ liệu | `config.ini` `[Cham]` thêm `LiaLacDo=5`, `LiaLacDoc=6`, `LiaLacLe=112`, `LiaLacThu=0` (cả lớp ghi đè, máy ảo, dt_v4 + `--chi-manifest`) |
| Soi chéo | phiên đo nhịp duyệt, 3 điểm chốt: lề RT chỉ lúc đang lia + 2 s sau (K=2 chỉ nghỉ lúc đó), RT là `D3DUSAGE_RENDERTARGET` (không bản CPU, cấp lại 2 lần/cử chỉ), góc ≠ 0 luôn đi kèm lề > 1000 (gửi lề trước góc) |

## 2. Cách dùng

- Kéo một ngón **ngang**: ngoài lia, cả cảnh xoay dần theo tay, tới hết tầm (LiaXaNgang) là 5° (phép "quay mặt đất" 2:1: dọc nghiêng khoảng 10°). Thả tay: trôi về 0 cùng độ lệch.
- Kéo **dọc**: cảnh co/giãn dọc tới 6 % như camera cúi/ngẩng (kéo xuống = dẹt hơn, kéo lên = cao hơn).
- Chụm hai ngón vẫn lia theo tâm nên cũng lắc. Tắt "Lia cảnh" trong Cài đặt > Tối ưu thì hết lắc; đổi số: `LiaLacDo` (độ, âm = đổi chiều, 0 = tắt), `LiaLacDoc` (%, 0 = tắt).
- Không tốn gì thêm khi không lia; khi lia, ảnh đệm thế giới rộng thêm 12 % (`LiaLacLe`) để góc khung không hở lúc xoay (RT 1164×676 ở khung 1040×604; ở 120 % là 1398×812).

## 3. Cơ chế (chỉ `JX_MOBILE`, PC y hệt: `kiem --pc` ĐẠT, cl Win32 + x64 `KRepresentShell3.cpp` 0 lỗi)

| Chỗ | Việc |
|---|---|
| `Represent3/KRepresentShell3.h/.cpp` `JxTheGioi` | lệnh **6** đặt góc (0,01°, kẹp ±15°), **7** hỏi, **8** lề RT (phần nghìn, 1000..1500), **9** co giãn dọc (phần nghìn, 850..1150), **10** hỏi. `m_nTgZoomRt = zoom × lề` = tỉ lệ RT thật (lệnh 0 cấp RT, lệnh 1 lùi gốc, lệnh 2 trả khung đều theo ZoomRt). Blit: 4 đỉnh = góc RT thu `1000/zoom` quanh tâm khung rồi `M = R(θ)·diag(1,k)`: `x' = c·x − 2s·k·y`, `y' = s·x/2 + c·k·y`; góc 0, k 1, lề 1000 → đúng 4 đỉnh cũ. Hai cửa toạ độ: `CoordinateTransform/X` (thế giới → khung) thêm `M`, `ViewPortCoordToSpaceCoord` (khung → thế giới, Core chọn vật thể/đi đường) thêm `M⁻¹` (`x = c·x + 2s·y`, `y = (−s·x/2 + c·y)/k`). Lọc tuyến tính khi có góc/k. `#include <math.h>` trong `JX_MOBILE`. |
| `Platform/JxLiaCanh.cpp` | `ApXoay()` gọi cuối `ApLech()` (mỗi khung khi đang lia) và trong `KetThuc()`: góc = `LiaLacDo × lệchX/lệch ngang tối đa`, k = `1 + LiaLacDoc% × lệchY/lệch dọc tối đa`; lề RT + vùng truy vấn vật thể × lề khi đang lia, trả về khi hết. `LiaLacThu` = gỡ lỗi giữ góc cố định 3 s sau khi vào thế giới (máy ảo). Sửa quirk `LiaThu` (kéo giả lập): `s_uNhaLuc = uNay` để nhịp chờ về không bị âm. |

Không đụng: `Core`, `Wnds.cpp`, `D3D9onGPU`, `ios/JxIosMain.cpp` (không thêm ký hiệu). Phiên đo nhịp đã được gửi tóm tắt để soi chéo trước khi push (vùng `[TG]` của họ).

## 4. Chứng minh (máy ảo)

- `LiaLacThu=5`: `[LAC] LiaLacThu: giu goc 5 do, le RT 112%`, `[TG] render target the gioi 1164x676` (và 1224/1282/1340 khi đang trôi zoom 105..115 % ở Tương Dương) → RT có lề đúng, không sập, HUD đứng yên (`lac1.png`).
- `LiaLacThu=15` (chỉ để nhìn rõ): cảnh nghiêng rõ, nhân vật và nhãn nghiêng theo, không hở góc (`lac15.png`) — chủ thấy "bóp méo" ở mức này, nên giữ mặc định 5°.
- `LiaThu=600,0` (kéo giả lập ngang): `[LAC] le RT 1000 -> 1120` lúc bắt đầu, `1120 -> 1000, goc -4` lúc về xong; ảnh `keo8.png` lúc đang về: cảnh dịch + xoay nhẹ. Kéo dọc `LiaThu=0,400`: xem `doc*.png`.
- Auto của nhân vật đang chạy nên "về" xảy ra ngay (nhân vật đi), không thử được nhịp chờ 1 s trên máy ảo; Fold 7 thử thật.

## 5. Chủ thử trên Fold 7

1. Mở lại app nhận **109140030**. Kéo ngang chậm: cảnh xoay dần theo tay, thả ra trôi về. Kéo dọc: cảnh cúi/ngẩng nhẹ.
2. Chạm đất / NPC trong lúc cảnh đang nghiêng (trước khi về hết) phải đi đúng chỗ, mở đúng thoại (cửa toạ độ đã đổi).
3. Thấy mạnh/yếu quá: nói tôi số độ (`LiaLacDo`) và % dọc (`LiaLacDoc`); muốn đổi chiều xoay thì để số âm.
4. Tống Kim đông: kéo log về, tôi xem `[TG]` viec/khung khi lia (RT rộng thêm 12 %).

## 6. Chủ hỏi thêm: "mổ nhị phân có lấy được phần vật phẩm rơi trên đất không?" (chưa làm, chỉ phân tích)

Từ siêu dữ liệu IL2CPP (`D:\game3gTQ_mo\meta_pc_res.txt`), bảng tài nguyên (`bundle_all.tsv`) và Lua (`pc_textassets`):
- Vật rơi là `StillObject : EntityObj` (lớp 759): `SetDropData(ownId, ownProtectLeftTime, playAnim, dropType, dropData, dropQuality, dropItemWX, dropItemType)`; trường `mDropPrefabAnim` (hoạt ảnh lúc rơi: bung ra rồi nảy), `mDropLightObj` (cột sáng), `mOwnerOnlyID/mOwnerProtectTime` (bảo hộ chủ sở hữu), `mDropQuality`, `mDropItemWX` (ngũ hành). `Global.dropStillLightObj[]` = prefab cột sáng theo phẩm chất.
- Tài nguyên: `cmn_droplight_blue / purple / gold / xgold / wgold` (cột sáng 5 mức), `cmn_drop_flash / _2 / _3 / _tong` (loé sáng lúc chạm đất), `cmn_drop_script_hong/zi/lv/lan/bai/huang` (sách/bí kíp rơi màu đỏ/tím/lục/lam/trắng/vàng), `cmn_drop_exp`, `cmn_drop_gj`.
- Lua `lua_scnobj_create.lua` `InitStill`: gói mạng `{loại, dữ liệu, phẩm chất, id chủ, số lượng, có hoạt ảnh, ngũ hành, giây bảo hộ}` → tên hiện màu theo phẩm chất (`GetItemColorNoEffectStr`), thêm ` x N` nếu chồng, phát `drop_sound` khi rơi mới; tiền đồng/vàng/vàng khoá là loại riêng. Nhặt: lỗi "quá xa", "còn bảo hộ", "túi đầy", "chỉ đội trưởng", "bang hạn chế" (`lua_text_item.lua`).
- JX1 hiện có: sprite vật rơi + tên (mobile luôn bật, `GOI_SHOW_OBJ_NAME`), màu tên theo phẩm chất, ảnh rơi riêng lúc mới rơi (`KObj.cpp` `m_nDropState`/`m_cImageDrop`), bảo hộ chủ sở hữu. **Chưa có:** cột sáng theo phẩm chất, loé lúc chạm đất, âm rơi, ` x N`.
- Mức chép được (chưa làm, chờ chủ chọn): (1) cột sáng dưới vật quý theo màu phẩm chất + loé lúc mới rơi (vẽ thêm 1 quad cộng sáng ở `KObj` mobile, ảnh từ kho VNKU hoặc vẽ dải gradient), ~1 ngày; (2) âm thanh rơi + ` x N` số lượng, nửa ngày; (3) bảng "nhặt tất cả" kiểu 3D, cần xem giao thức nhặt hiện có, chưa ước.


---

# VẬT PHẨM RƠI: CỘT SÁNG THEO PHẨM CHẤT + LOÉ LÚC CHẠM ĐẤT (mức 1) — 14/09 01:2x

Chủ: *"Làm mức 1 trước: hiệu ứng ánh sáng tuỳ theo loại vật phẩm"* (sau khi đọc §6 mổ game 3D).

## 1. Bản

| | |
|---|---|
| Mã | `Core/Src/KObj.cpp` (chỉ `JX_MOBILE`, trong `#ifndef _SERVER`), kịch bản `android/va_nguon_vatroi_1409.py`; PC y hệt (`kiem --pc` ĐẠT, byte cao 1466 = 1466) |
| Ảnh | `spr/vatroi/cotsang.spr` (1 khung 48×120, trắng, neo chân cột) + `spr/vatroi/loe.spr` (6 khung 64×32 vòng elip 2:1 dãn ra), sinh bằng `android/anh_vatroi_cotsang.py` (PIL → SPR bảng màu 1 màu + alpha 31 mức — SAI, alpha SPR là 8 bit, sửa 14/09 08:4x xem mục HÀO QUANG §3); lớp ghi đè, máy ảo, dt_v4 (đã `--chi-manifest`) |
| Config | `config.ini` mục mới `[VatRoi]`: `CotSang=1`, `CotSangTu=1`, `CotSangAlpha=170`, `Loe=1` (lớp ghi đè, máy ảo, dt_v4) |
| APK | 109140105 thử máy ảo; giao theo bản gộp của phiên đo nhịp hoặc tôi chép + `--chi-manifest` (xem cuối) |

## 2. Cách hiện

- Vật phẩm nằm đất có **cột sáng** dưới chân, màu = màu tên vật phẩm (= phẩm chất JX1: `normal_item` 0 trắng, `green_item` 1 xanh, `broken_item` 2 đồ hỏng, `gold_item` 3 hoàng kim, `purple_item` 4 đỏ tím, `platinum_item` 5 bạch kim). Mặc định từ xanh trở lên (`CotSangTu=1`), đồ hỏng không bao giờ; `CotSangTu=0` để đồ trắng cũng có. Cột thở nhẹ chu kỳ 1,6 s (alpha × 0,67..1,0).
- Lúc vật **vừa chạm đất** (hết hoạt ảnh rơi `m_nDropState` 1 → 0): **loé** vòng sáng dãn ra 6 khung × 70 ms, cùng màu. Vật có sẵn khi vào map (không rơi trước mắt) không loé.
- Tiền (`Obj_Kind_Money`) không có hiệu ứng.

## 3. Cơ chế

- `VatRoi_VeDuoi(colorID, màu tên, dropState, x, y)` gọi trong `KObj::Draw` ngay trước `switch(m_nKind)` (cột vẽ TRƯỚC ảnh vật), `VatRoi_VeTren(index, màu, x, y)` sau switch (loé vẽ SAU ảnh). Vẽ bằng `KRUImage` tĩnh dùng lại (`uImage` cache), `IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST` = nhân màu (Represent3 `SetSpriteBlend`: rgb ≠ 0 → nhân) với `Color = (alpha<<24)|rgb tên`, `RUIMAGE_RENDER_FLAG_REF_SPOT` + `IMAGE_IS_POSITION_INIT` nên neo (cx, cy) của SPR đặt chân cột đúng điểm đặt vật (như ảnh vật phẩm).
- `VatRoi_ChamDat(index)` trong `KObj::Activate` (nhánh client) ngay chỗ `m_nDropState = 0` sau `m_cImageDrop.CheckEnd()`; bảng phụ `s_uVrLoeLuc[MAX_OBJECT]` theo `m_nIndex`, không đổi lớp `KObj`, không đổi giao thức.
- Đọc `config.ini [VatRoi]` một lần lúc vẽ vật đầu tiên (`GetPrivateProfileInt`, log `[VATROI] cot sang=… loe=…`).

## 4. Thử

- Máy ảo 109140105, `CotSangTu=0`: `[VATROI] cot sang=1 (tu mau 0, alpha 170) loe=1` lúc vật rơi đầu tiên được vẽ (Sa Mạc Mê Cung, auto đánh quái), không sập; ảnh `vr_*.png` / `vs*.png`.
- Bẫy khi vá: `#endif` của khối mobile + dòng trống → bản Windows dư một dòng trống, `kiem --pc` HỎNG → bỏ dòng trống (kịch bản có mục sửa cho cây đã vá).

## 5. Chủ thử trên Fold 7

1. Đánh quái cho rơi đồ: đồ xanh/hoàng kim/tím/bạch kim có cột sáng màu tương ứng dưới chân, lúc rơi có vòng loé; đồ trắng không có (muốn có: `CotSangTu=0`).
2. Thấy cột to/nhỏ, đậm/nhạt: đổi `CotSangAlpha` (30..255) hoặc nói tôi đổi cỡ ảnh (`anh_vatroi_cotsang.py`).
3. Tống Kim đông đồ rơi: xem fps; mỗi vật thêm 1 quad, không tốn đáng kể.


---

# HÀO QUANG DƯỚI CHÂN + CÔNG TẮC HIỆU ỨNG + SỬA CỘT SÁNG VẬT RƠI — 14/09 08:4x

Chủ (14/09 sáng): *"hãy làm và có nút tắt mở khi cần tắt hoặc mở"* (sau §10.2 mổ game 3D: `halo_npc_purple/gold/pink`, `halo_boss_red`) và *"vật phẩm ném ra có hiệu ứng ánh sáng chưa hoạt động - tôi để đồ trong hành trang bạn ném ra để test"*.

## 1. Bản

| | |
|---|---|
| Mã | `Core/Src/KNpc.cpp` (khối static trước `KNpc::Paint` + gọi `HaoQuang_Ve(this)` ngay trước `m_DataRes.Draw`), `Core/Src/KObj.cpp` (cờ `s_nVrBat` + đọc `[VatRoi]` ngay đầu `KObj::Draw`), `S3Client/Ui/UiCase/UiOptions2.h/.cpp` (`MAX_TOGGLE_BTN_COUNT` 7 → 10 chỉ trong lớp `KUiOptions2`, 3 mục enum `OPTION_I_HQQUAI / HQTRANGBI / SANGVATROI`) — tất cả chỉ `JX_MOBILE`; PC y hệt (`kiem --pc` ĐẠT cả 4 tệp). Kịch bản `android/va_nguon_haoquang_1409.py` rồi `android/va_nguon_haoquang_1409_b.py` (chẩn đoán + khoá `ThuAnh`) |
| Ảnh | `spr/haoquang/vongquai.spr` (12 khung 128×64, neo (64,32) = tâm vòng) + `vongboss.spr` (16 khung 176×88): ảnh TRẮNG sinh bằng `android/anh_haoquang_vong.py` (vẽ khung tròn 2× rồi ép 2:1), nhuộm màu lúc vẽ bằng `IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST`; vòng trang bị dùng ảnh có sẵn `vongtronvang / vongtrontim / vongtronxanh.spr` (30 khung, giữ màu gốc) |
| Giao diện | Cài đặt > TÙY CHỌN > Tối ưu: **10 công tắc**, 5 hàng × 2 cột từ Top=70 (`android/sinh_uioptions2_haoquang.py` → `ui/ui3/uioptions2.ini`): thêm **Vòng quái**, **Vòng đồ mặc**, **Sáng vật rơi** (tên ≤ 12 ký tự vì nút rộng 72 px, "Vòng trang bị" bị cắt). Lưu `userdata/<acc>/uiautoconfig.ini [Options2] HaoQuangQuai / HaoQuangTrangBi / SangVatRoi`, mặc định BẬT; áp ngay khi bấm và lúc vào thế giới (`JxHaoQuang_DatBat`, `JxVatRoi_DatBat`) |
| Config | `config.ini [HaoQuang]`: `Alpha=220` (độ đậm vòng quái), `TrangBiTu=3` (phẩm chất tối thiểu có vòng mình: 3 hoàng kim / 4 đỏ tím / 5 bạch kim), `Thu=0` (gỡ lỗi: 1 = mọi quái thường vòng xanh, 2 = + ép mình hoàng kim, 3 = + cả NPC thoại), `ThuAnh=0` (1 = vòng quái dùng vongtrondo.spr thật, 2 = vongquai không nhuộm, 3 = nhuộm trắng). `[VatRoi] CotSangTu` **1 → 0** (xem §3) |
| APK | máy ảo 109140801 → 109140801 → 109140830 (5 lần vá), dt_v4 = **109140830** |

## 2. Cách hiện

- **Quái** theo `m_Type` (`BOSS_STATE`, máy chủ đẩy qua `NpcSync.NpcEnchant` — cùng trường JX1 dùng tô màu tên quái ở `PaintInfo`): `boss_blue` → vòng nhỏ **xanh dương** (0x6E78FF), `boss_muter` → xanh lá; `boss_gold` → vòng to **hoàng kim** (0xFFD94E), `boss_event` → **tím** (0xE65AFF), `boss_war` → **đỏ** (0xFF3C28). Vòng quay: vạch trong quay xuôi, nút sáng quay ngược, boss thêm 3 cung ngoài; 70 ms/khung, lặp kín.
- **Nhân vật của mình**: phẩm chất cao nhất trong đồ đang mặc (`m_ItemList.GetEquipment(i)` → `Item.GetColorItem()`, bỏ đồ hỏng) ≥ `TrangBiTu` → hoàng kim `vongtronvang`, đỏ tím `vongtrontim`, bạch kim `vongtronxanh`. **Người khác không có**: client không biết đồ họ mặc, muốn có phải đổi giao thức → không làm.
- Chết / hồi sinh không vẽ; "Mất Npc" bật thì ẩn luôn vòng (đi theo `bPaintBody`). Vẽ tại điểm đặt chân (`GetNpcRes()->GetPos`, `REF_SPOT`), trước bóng + thân nên nằm dưới quái, trên nền.

## 3. Lỗi gốc tìm được: ảnh SPR sinh bằng PIL ghi alpha SAI → cột sáng vật rơi (VATROI 14/09 01:2x) chưa từng hiện

- `Represent3/TextureRes.cpp` `RenderToA8R8G8B8` / `RenderToIndexAlpha`: byte thứ hai của mỗi run RLE là **alpha 8 bit** (`a << 24`), **không phải 0..31** như tôi ghi ở mục VẬT PHẨM RƠI (§1 "alpha 31 mức" — SAI). `cotsang.spr`, `loe.spr` (và `vongquai/vongboss` lúc đầu) ghi alpha ≤ 31 → tối đa 12 % → gần như trong suốt. Ảnh thật: `vongtronxanh` run `1a ff`, nút `nut_vp_*.spr` run `01 88` = alpha 255 / 136.
- Đã sửa `anh_vatroi_cotsang.py` + `anh_haoquang_vong.py` (alpha 0..255, `q = 0 if a < 8 else a`), sinh lại 4 ảnh vào lớp ghi đè, máy ảo, dt_v4. Sau sửa vòng hiện ngay (ảnh `vong_a.png`), trước đó `[HAOQUANG] anh ... param 1 (khung 12 128x64)` = ảnh nạp được nhưng không thấy.
- Lý do thứ hai chủ "ném ra không thấy": máy chủ **chỉ cho ném đồ trắng / xanh** (`KPlayer::ThrowAwayItem`: hoàng kim, đỏ tím, bạch kim, nhiệm vụ, đồ khoá / bảo hiểm → "Vật phẩm này không thể vứt bỏ được"), mà mặc định cũ `CotSangTu=1` loại đồ trắng → ném đồ trắng cũng không có cột. Nay `CotSangTu=0` (game 3D cũng có cột trắng cho đồ trắng `dg_xw_cmn_white`); muốn như cũ đặt lại 1, hoặc tắt hẳn bằng công tắc "Sáng vật rơi".
- Bẫy phụ: `VatRoi_DocCfg` trước chỉ chạy khi vẽ vật phẩm đầu tiên → `[VatRoi] Thu=2` (cột dưới mọi vật thể) không bao giờ bật được; nay đọc một lần ngay đầu hook trong `KObj::Draw`.
- Bẫy khi vá (lặp lại lần 3): khối `#endif` + dòng trống → `kiem --pc` HỎNG; kịch bản có mục sửa cho cây đã vá.

## 4. Thử máy ảo (Ba Lăng Huyện, tài khoản hinod1)

- Cửa sổ Tối ưu 10 công tắc nằm gọn trong nền `main2.spr`, nút Đóng không đụng (`tuychon3.png`); tắt "Vòng đồ mặc" → vòng vàng mất ngay, bật lại → hiện (`hq2/hq3.png`).
- Vòng vàng dưới nhân vật (Thu=2 ép hoàng kim) bám theo khi cưỡi ngựa; Thu=3: vòng xanh quay dưới mọi NPC trong thành (`vong_a.png`, `vong_zoom.png`).
- Ném "Chung Nhũ Nguyên Thạch" / "Đại Phong Đao" / "Phá Thiên Chùy" (tap ô → dải nút → Ném): log `[VATROI] cot sang=1 ... loe=1` mỗi lần; vật rơi đúng dưới chân ngựa nên chưa chụp được cột rõ trên máy ảo → chủ xem trên Fold 7 (cùng đường vẽ + cùng lỗi alpha đã sửa như vòng).
- Trang thông tin nhân vật / hành trang: JX1 đã có sẵn **viền phẩm chất động** (`KWndObjectBox::PaintWindow` → `DrawBorder2` xanh / hoàng kim / tím / bạch kim) và hiện đúng trên mobile (`trangbi1.png`) → không làm thêm.

## 5. Chưa làm

- **Đao quang** màu khi vung vũ khí (§10.1): phải vẽ sprite vệt sáng 8 hướng × từng loại vũ khí × vài khung, 2–3 ngày → chờ chủ chốt có làm không.
- Vòng hào quang trang bị của **người khác**: cần máy chủ gửi phẩm chất (đổi giao thức) → không làm.
- Máy ảo chưa gặp quái `boss_blue`/`boss_gold` thật (trong thành); màu vòng lấy đúng bảng màu tên quái nên chỉ còn xem cỡ vòng có hợp quái to/nhỏ.

## 6. Chủ thử trên Fold 7

1. Ra map có quái xanh (tinh anh) / boss hoàng kim: vòng dưới chân đúng màu tên quái; Tống Kim boss chiến trường vòng đỏ. Vòng to/nhỏ, đậm/nhạt: `[HaoQuang] Alpha` hoặc nói tôi đổi cỡ ảnh.
2. Mặc đồ hoàng kim / đỏ tím / bạch kim: vòng dưới chân mình. Cài đặt > TÙY CHỌN > Tối ưu: tắt/bật "Vòng quái", "Vòng đồ mặc", "Sáng vật rơi".
3. Ném đồ trắng / xanh: cột sáng màu tên + vòng loé lúc chạm đất. Không muốn đồ trắng sáng: `[VatRoi] CotSangTu=1`.

## 7. Giao bản

- dt_v4: `jx1mobile.apk` = bản cuối trong mục này, `config.ini` thêm `[HaoQuang]` + `CotSangTu=0`, `ui/ui3/uioptions2.ini` 10 công tắc, `spr/haoquang/vongquai.spr + vongboss.spr`, `spr/vatroi/cotsang.spr + loe.spr` sinh lại; sau đó `--chi-manifest`. Phiên đo nhịp đã xác nhận không giữ bản chưa đẩy.

## 8. Sửa theo chủ 14/09 09:0x: vòng đồ mặc kiểu 3D + đồ rơi dựng thẳng lên (bản **109140856**)

Chủ: *"Vòng hào quang đồ mặc phải làm giống như 3d, không dùng vòng sáng có sẵn"* và *"Đồ ném ra đã có màu theo loại đồ nhưng nó không giống như 3d: đồ rớt ra sẽ dựng thẳng lên theo cột sáng"*.

| | |
|---|---|
| Vòng đồ mặc | bỏ `vongtronvang/tim/xanh.spr`; ảnh mới **`spr/haoquang/vongnguoi.spr`** (16 khung 144×72, neo (72,36), trắng, phẳng: hai vòng đồng tâm + 6 cánh hoa quay xuôi + 12 nút quay ngược + quầng giữa — kiểu khác vòng quái), nhuộm theo phẩm chất: hoàng kim `0xFFD94E`, đỏ tím `0xE65AFF`, bạch kim `0xC8F0FF`, xanh `0x5AE65A` (chỉ khi `[HaoQuang] TrangBiTu<=1`). Kịch bản `android/va_nguon_haoquang_1409_c.py` (chỉ số ảnh 6). |
| Đồ rơi dựng lên | `Core/Src/KObj.cpp` `VatRoi_DoCao()`: icon vật phẩm (24×24, góc trái trên tại điểm đặt) được nhấc lên **12 px** (tâm icon = chân cột) **+ `NoiCao` (8) px dựng lên trong 350 ms sau khi chạm đất** (ease-out 2t−t²) **+ nhấp ±3 px** chu kỳ 1,4 s (sóng tam giác). Chân cột sáng và tâm vòng loé dời sang TÂM icon (ảnh sinh lại: `cotsang.spr` neo (12,100), `loe.spr` neo (20,20)). Bảng `s_uVrNoiLuc[MAX_OBJECT]` ghi lúc chạm đất. Kịch bản `android/va_nguon_vatroi_1409_c.py`. |
| Config | `[VatRoi] Noi=1` (0 = nằm phẳng như cũ), `NoiCao=8` (0..40) — lớp ghi đè, máy ảo, dt_v4 |
| Nhặt đồ | không đổi: `KObjSet::SearchObjAt` xét theo toạ độ bản đồ dọc một đoạn cao 120 px, không theo ô ảnh |

- Máy ảo: vòng vàng kiểu 3D dưới chân nhân vật (`nguoi_zoom.png`), bám theo ngựa; ném "Thôn Nhật Trảm": log `[VATROI] ... noi=1 cao 8`, vật rơi dưới bụng ngựa nên máy ảo chưa chụp rõ icon dựng → chủ xem Fold 7. `kiem --pc` ĐẠT.
- Ba ảnh vòng + hai ảnh cột/loé sinh lại; dt_v4 đã chép + `--chi-manifest`.

## 9. Sửa theo chủ 14/09 09:2x: vật phẩm nằm đất vẽ bằng ICON ĐỨNG trong cột sáng (bản **109140913**)

Chủ: *"Đồ ném ra đã nằm trôi nổi nhưng chưa dựng thẳng lên theo cột sáng - vẫn nằm bẹp nhưng chỉ trôi nổi lên thôi"*. Đúng: ảnh "nằm đất" của vật phẩm (`objdata.txt` ImageName) là ảnh vẽ theo góc nằm, nhấc lên vẫn thấy bẹp. Nay vẽ **icon đứng** (ảnh trong hành trang) lơ lửng trên chân cột — như mô hình đứng trong cột sáng của game 3D.

- **Lấy icon từ vật thể trên đất**: client chỉ biết `m_nGenre / m_nDetailType / m_nParticularType` + **tên** (`OBJ_ADD_SYNC.m_szName`), không biết level → `VatRoi_TimIcon`: trang bị thử `ItemGen.GetEquipmentCommonAttrib(detail, particular, level 1..10, series 0)` khớp tên → `GetImageName()`; thuốc `GetMedicineCommonAttrib(detail, level 1..5)`. Kích thước icon qua `g_pRepresent->GetImageParam` (chưa nạp thì khung sau hỏi lại). Nhớ theo `m_nID` của vật thể (bảng `s_szVrIcon[MAX_OBJECT]`...). Loại khác (nhiệm vụ, nguyên liệu, tiền) không tra được → nhấc ảnh nằm đất lên như mục 8.
- **Vẽ**: `VatRoi_DungIcon` thay `m_Image` (tên icon, khung 0, xoá `uImage` khi đổi tên) đặt đáy icon tại tâm ảnh nằm đất (x+12, y+12) = chân cột, nhấc lên `VatRoi_DoCao − 12` (dựng lên 350 ms + nhấp ±3 px). Vũ khí 1×4 (24×96) đứng thẳng cao trong cột. Nhặt đồ không đổi.
- Kịch bản `android/va_nguon_vatroi_1409_d.py` (sau bản c). `[VatRoi] Thu=1` ghi log `[VATROI] icon dung vat <id> '<tên>' (genre/detail/particular) -> <ảnh> WxH`.
- Máy ảo: ném "Liệt Thiên Triền Thủ" (trang bị xanh, 0/0/6) → log `-> \spr\item\equip\closeweapon\obj-glove04.spr 24x48`; đi ra xa thấy icon găng đứng trên chân cột, cột xanh nhạt phía sau (`dung4_zoom.png`). `kiem --pc` ĐẠT.

## 10. Sửa theo chủ 14/09 09:5x (ảnh Fold 7): icon co giãn vừa cột sáng, đặt đúng điểm đặt, tên nằm trên icon (bản **109140935**)

Chủ: *"tên trang bị bị lệch và hình ảnh quá to, làm nhỏ lại cho bằng cột sáng"*.

- **Gốc lệch**: ảnh nằm đất của vật phẩm (`objdata.txt` `\spr\obj\item\obj_wq_*.spr`) neo **đáy dưới** (`ImageCgXpos/Ypos` ≈ 60/105) nên điểm đặt của vật thể là (x, y) — bản c/d giả định icon 24×24 góc trái trên nên đặt cột, loé, icon ở (x+12, y+12) → lệch 12 px so với tên (`DrawInfo` căn giữa x). Nay cột (`cotsang.spr` neo (24,112)), loé (`loe.spr` neo (32,32)) và icon đều tại (x, y).
- **Co giãn**: `VatRoi_VeIconDung` vẽ icon bằng `RU_T_IMAGE_STRETCH` ở toạ độ màn hình: đổi (x, y) qua `g_pRepresent->CoordinateTransform` (cùng hàm Represent3 dùng cho mọi ảnh thế giới, gồm lia/zoom/lắc) rồi kéo về khung `min(1, IconRong/w, IconCao/h)` — `[VatRoi] IconRong=28 IconCao=44`: 1×1 giữ nguyên, 2×2 (50×50) → 28×28, 2×4 (48×96) → 22×44, 1×4 (24×102) → 10×44. Vẽ ở nhánh `default` của `switch` (SAU cột sáng, TRƯỚC loé); `m_Image` (ảnh nằm đất) không vẽ khi có icon. Hạn chế: `DrawImage2DStretch` cắt theo cỡ màn hình nên khi "Nhìn rộng" > 100 % icon ở dải lề ngoài cỡ màn hình có thể bị cắt (cột vẫn vẽ).
- **Tên**: `DrawInfo` cộng `VatRoi_CaoTen` = độ lơ lửng + chiều cao icon đã co − 12 vào `nHeightOff` (`OBJ_SHOW_NAME_Y_OFF` 48) → tên nằm ngay trên icon.
- `VatRoi_DoCao` nay trả độ LƠ LỬNG (0..NoiCao+3), −1 = tắt; không cộng 12 nữa. Kịch bản `android/va_nguon_vatroi_1409_e.py` (sau bản d); ảnh `anh_vatroi_cotsang.py` sinh lại (lớp ghi đè, máy ảo, dt_v4).
- Máy ảo: ném "Kim Cô Bổng" (1×4, 24×102) và "Phá Thiên Chùy" (2×2, 50×50 → 28×28): búa đứng nhỏ trong cột tím, tên phía trên (`chuy3_zoom2.png`). `kiem --pc` ĐẠT.
