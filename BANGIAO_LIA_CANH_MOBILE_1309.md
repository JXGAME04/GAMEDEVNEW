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
- **Co giãn**: `VatRoi_VeIconDung` vẽ icon bằng `RU_T_IMAGE_STRETCH` ở toạ độ màn hình: đổi (x, y) qua `g_pRepresent->CoordinateTransform` (cùng hàm Represent3 dùng cho mọi ảnh thế giới, gồm lia/zoom/lắc) rồi kéo về khung `min(1, IconRong/w, IconCao/h)` — `[VatRoi] IconRong=28 IconCao=44`: 1×1 giữ nguyên, 2×2 (50×50) → 28×28, 2×4 (48×96) → 22×44, 1×4 (24×102) → 10×44. Vẽ ở nhánh `default` của `switch` (SAU cột sáng, TRƯỚC loé); `m_Image` (ảnh nằm đất) không vẽ khi có icon. Không bị cắt khi "Nhìn rộng": phiên đo nhịp kiểm (KRepresentShell3 `JxTheGioi` lệnh 1 đặt `g_nScreenWidth/Height` = cỡ RT gồm zoom + lề trong suốt lúc vẽ thế giới, lệnh 2 mới trả lại) nên `DrawImage2DStretch` cắt theo cỡ RT, icon ở dải lề vẫn vẽ đủ.
- **Tên**: `DrawInfo` cộng `VatRoi_CaoTen` = độ lơ lửng + chiều cao icon đã co − 12 vào `nHeightOff` (`OBJ_SHOW_NAME_Y_OFF` 48) → tên nằm ngay trên icon.
- `VatRoi_DoCao` nay trả độ LƠ LỬNG (0..NoiCao+3), −1 = tắt; không cộng 12 nữa. Kịch bản `android/va_nguon_vatroi_1409_e.py` (sau bản d); ảnh `anh_vatroi_cotsang.py` sinh lại (lớp ghi đè, máy ảo, dt_v4).
- Máy ảo: ném "Kim Cô Bổng" (1×4, 24×102) và "Phá Thiên Chùy" (2×2, 50×50 → 28×28): búa đứng nhỏ trong cột tím, tên phía trên (`chuy3_zoom2.png`). `kiem --pc` ĐẠT.

## 11. Chủ 14/09 10:2x: "bỏ vòng đồ ở player đi" (bản **109140955**)

- Gỡ hẳn vòng hào quang đồ mặc của nhân vật mình: `KNpc.cpp` cắt nhánh `kind_player` trong `HaoQuang_Ve`, bỏ `s_nHqTrangBi / TrangBiTu`, `JxHaoQuang_DatBat(int nQuai)`, bảng ảnh 7 → 6 (bỏ `vongnguoi.spr`, xoá tệp ở lớp ghi đè, máy ảo, dt_v4; `anh_haoquang_vong.py` không sinh nữa, giữ hàm `vong_nguoi` để tham khảo).
- Cài đặt > Tối ưu còn **9 công tắc**: bỏ "Vòng đồ mặc" (`OPTION_I_HQTRANGBI`, `MAX_TOGGLE_BTN_COUNT` 10 → 9, `uioptions2.ini` "8=Sáng vật rơi"; `sinh_uioptions2_haoquang.py` 2 tên). Khoá đã lưu `HaoQuangTrangBi` trong `uiautoconfig.ini [Options2]` của tài khoản cũ bị bỏ qua, không lỗi.
- `config.ini [HaoQuang]` viết lại: chỉ còn `Alpha`, `Thu` (1 mọi quái thường, 3 cả NPC thoại), `ThuAnh`; bỏ `TrangBiTu`.
- Vòng quái/boss và cột sáng + icon đứng của vật rơi giữ nguyên. Kịch bản `android/va_nguon_haoquang_1409_d.py` (chạy sau a/b/c; nhận thêm tham số thư mục dt_v4 để sửa dữ liệu). `kiem --pc` ĐẠT; máy ảo: vào thế giới không sập, cửa sổ Tối ưu 9 nút.


---

# ZOOM KIỂU 3D: CHỤM NGÓN + ĐỘ NHẠY + PHÓNG TO CÓ LỌC NÉT — 14/09 11:4x (bản **109141129**)

Chủ: *"làm 1-3"* sau mục 11 `PHANTICH_KIEMVONG_GIANGHO_3D_1309.md` (mổ zoom bản 3D).

## 1. Bản

| | |
|---|---|
| Mã | `S3Client/Platform/JxLiaCanh.cpp/.h` (chụm kiểu 3D, phóng to, công tắc), `S3Client/Ui/UiCase/UiOptions2.h/.cpp` (12 công tắc), `Represent/Represent3/KRepresentShell3.cpp/.h` (phóng to + lọc nét, khối [TG]) — tất cả chỉ `JX_MOBILE`; `kiem --pc` ĐẠT. Kịch bản `android/va_nguon_zoom3d_1409.py` (S3Client) + `android/va_nguon_zoom3d_1409_rep3.py` (Represent3, phiên đo nhịp soi chéo), `android/sinh_uioptions2_zoom.py` (ini) |
| Config | `config.ini [Cham]`: `ZoomKieu=1`, `ZoomNhay=10`, `ZoomChongRung=5`, `ZoomMuot=16`, `ZoomToiThieu=80`; `[Client] Rep3ZoomNet=1` |

## 2. Mục 1 — chụm hai ngón như GameCamera 3D (`ZoomKieu=1`)

- Mỗi lần hai ngón dời: `Δpx = khoảng cách hai ngón − lần chấp nhận trước`; `|Δpx| < ZoomChongRung` (5 px) bỏ qua; `zoom_đích −= Δpx × ZoomNhay/100` (hai ngón dang ra = phóng to, chụm lại = nhìn rộng), kẹp `[ZoomToiThieu, ZoomToiDa]`. 3D: `mWheelDistance += −0,01·Δpx × fWheelSpeed`.
- Mỗi khung (`JxLia_Nhip`, cả trong lúc chụm và lúc trôi theo map): `zoom += (đích − zoom) × clamp01(Δt × ZoomMuot)`; 16 = `fDistanceSpeed` của 3D (95 % sau ~0,18 s). Áp thật theo nấc `ZoomBuoc` (5 %) như cũ. `ZoomKieu=0` trả về cách cũ (tỉ lệ khoảng cách + trôi `ZoomTocDo` %/s).
- Nhả hai ngón: nhớ `zoom_đích` vào `UserData\CameraMobile.ini` như trước.

## 3. Mục 2 — công tắc độ nhạy (Cài đặt > TÙY CHỌN > Tối ưu, 12 nút, 6 hàng từ Top=60 cách 27)

| Công tắc | Tác dụng | Mặc định |
|---|---|---|
| Zoom nhanh | `ZoomNhay` × 2 | tắt |
| Zoom chậm | `ZoomNhay` × 0,5 (bật cái này tự tắt cái kia; cả hai tắt = vừa) | tắt |
| Lắc camera | bật/tắt camera lắc nhẹ khi lia (`[LAC 14/09]`) | bật |

Lưu `uiautoconfig.ini [Options2] ZoomNhanh / ZoomCham / LacCamera`; áp ngay và lúc vào thế giới (`JxLia_DatNhay`). JX1 không có xoay camera nên không có "Xoay nhanh/chậm" như thanh của bản 3D.

## 4. Mục 3 — phóng to (zoom < 100 %) có lọc nét

- `ZoomToiThieu=80` (phóng to tối đa 1,25×; đặt 100 = tắt phóng to, 50 = 2× như biên cứng của Represent3). Chữ giữa màn: "Phóng to 125%" khi < 100, "Nhìn rộng NNN%" khi > 100.
- Represent3 (`JxTheGioi`): lệnh 4 kẹp dưới 1000 → 500; lệnh 0: zoom < 1000 vẫn đi đường RT (RT = cỡ khung, K=1); ba cửa toạ độ (`CoordinateTransform`, `ViewPortCoordToSpaceCoord`, sampler blit) điều kiện `> 1000` → `!= 1000` (công thức `1000/m_nTgZoom` đã tổng quát). Chạm chọn NPC ở 80 % đúng (máy ảo bấm NPC "Kim Quốc Quân" mở đúng thoại).
- **Lọc nét** (`Rep3ZoomNet=1`): lệnh 2 blit hai bước: RT → RT2 (cỡ 2×, sampler POINT = nhân đôi điểm sắc) → khung (LINEAR). Kết quả kiểu "sharp bilinear": viền điểm giữ nét, chỉ mép điểm hoà; so với blit thẳng LINEAR (`Rep3ZoomNet=0`, ảnh `pt_sosanh.png` trái/phải) rõ hơn thấy được ở 1,25×. RT2 2080×1208 BGRA8 (~10 MB) tạo lần đầu khi phóng to, huỷ cùng `JxTheGioiHuy`; toạ độ quad theo viewport của render target (không đụng `g_nScreenWidth`). Chi phí: thêm một quad toàn khung/khung khi phóng to.
- Vùng truy vấn vật thể (`SetRepresentAreaSize`) = khung × zoom (80 % → 832×483) khớp phần nhìn thấy.

## 5. Thử máy ảo

- `[Cham] ZoomThu=80`: vào map trôi 100 → 95 → 90 → 85 → 80 trong ~0,12 s (đuổi hàm mũ, nấc 5 %), log `[ZOOM3D] RT2 2080x1208 cho phong to co loc net`; hình đúng tâm, UI không đổi (`pt_c.png`, phóng `pt_c_zoom.png`); chạm NPC đúng (`pt_npc.png`); cửa sổ Tối ưu 12 nút (`pt_tuychon.png`), bật/tắt lưu đúng.
- Chụm hai ngón thật chưa thử được trên máy ảo (adb không giả lập hai ngón) → chủ thử Fold 7: dang/chụm ngón, cảm giác mượt, độ nhạy ba mức.
- Lưu ý: tài khoản đang tắt "Lia cảnh"/"Nhìn rộng" thì zoom không chạy (đúng thiết kế) — máy ảo hôm nay hai công tắc này đang tắt, tôi đã bật lại.

## 6. Chủ thử trên Fold 7

1. Dang hai ngón = phóng to (tới 125 %), chụm = nhìn rộng (tới ZoomToiDa của map); nhả ngón zoom dừng êm. Muốn phóng to hơn: `ZoomToiThieu=70` (1,43×) hoặc 50 (2×) — càng to càng thấy hạt.
2. Cài đặt > Tối ưu: Zoom nhanh / Zoom chậm / Lắc camera.
3. Thấy ảnh phóng to bị nhoè thì so `Rep3ZoomNet=0`; thấy hạt thô thì đặt 0 để mềm hơn.
4. Fold 7 nóng khi phóng to: chi phí = 1 khung RT + 1 RT2 (4× điểm ảnh của RT2 chỉ là blit) — báo tôi nếu fps tụt để đo.

## 7. Ghi chú soi chéo của phiên đo nhịp (đã OK, không chặn)

- Khi zoom < 100 % thì `bPhongTo` ép K=1 nên chế độ [TG] K=2 (thế giới 60 Hz khi màn 120 Hz quá tải) tắt trong lúc phóng to, giống lúc nhìn rộng.
- RT2 = 2 × RT; khi vừa lắc (lề RT tới 1,5× khung) vừa phóng to thì RT2 tới ~3× mỗi chiều (~20–35 MB BGRA8), chấp nhận được; muốn gọn thì RT2 = 2 × cỡ khung là đủ vì phóng to chỉ lấy phần giữa.
- Bước hai xét `m_nTgZoom < 1000` còn đường RT to xét `m_nTgZoomRt`: zoom 90 % × lề 1,12 = 1008 thì đi cả RT to lẫn RT2, hình vẫn đúng, chỉ tốn thêm; chưa cần sửa.
- `CDevGpu::GetRenderTarget` có AddRef nên `SAFE_RELEASE(pDichCu)` cân; viewport tự bằng RT2 khi `SetRenderTarget`; RT2 không cần Clear vì quad phủ kín.



---

# TGCAP: RT THẾ GIỚI CẤP MỘT LẦN, ZOOM/LỀ CHỈ ĐỔI PHẦN DÙNG — 14/09 13:2x (bản **109141317**)

Phiên đo nhịp đo log Fold 7 12:41 (bản 109141129, chủ thử zoom/lắc): chụm ngón zoom 1200→800 = 5 bước zoom + RT2 cấp lại 3 lần → **khung 97 ms**; mỗi lần lắc lề RT 1000→1120 (và trả về) cấp lại RT → khung 20–30 ms. Gốc: lệnh 0 cấp lại RT mỗi khi cỡ (khung × zoom × lề) đổi, RT2 = 2 × RT cũng cấp lại theo; `[TAI-DO]` đo tạo texture lớn 5–54 ms + driver cam kết bộ nhớ ở lần nộp đầu. Chi tiết ở `PHANTICH_LOG_FOLD7_1309.md` mục 14/09 12:41.

## 1. Bản

| | |
|---|---|
| Mã | `Represent/Represent3/KRepresentShell3.cpp/.h` (khối [TG], JX_MOBILE; phiên đo nhịp soi chéo OK) + `S3Client/Platform/JxLiaCanh.cpp` (một dòng lệnh 11); `kiem --pc` ĐẠT. Kịch bản `android/va_nguon_tgcap_1409.py` (a–d, idempotent) |
| Config `[Client]` | `TheGioiRTCap=1700` (‰ khung; = ZoomToiDa 150 % × LiaLacLe 112 %; mã mặc định 1500), `TheGioiRT2Cap=1150` (RT2 = 2 × khung × ‰, ≥ LiaLacLe), `TheGioiRTSan=1` (cấp sẵn + chạm ở khung đầu vào thế giới; 0 = cấp khi cần) |

## 2. Cơ chế

- **RT**: texture thật có cỡ **cấp** = khung × `TheGioiRTCap`; `m_nTgW/H` giờ là phần **dùng** (khung × zoom × lề) đổi mỗi bước; chỉ cấp lại khi khung đổi (gập/mở, xoay: `m_nTgCapKhungW/H` ≠ `g_nScreenWidth/Height`) hoặc phần dùng vượt cấp (cấp mới theo nấc 250 ‰, kẹp 3500). Thế giới vẽ vào góc trái trên của texture: `CDevGpu::SetRenderTarget` đặt viewport = cả texture (VS chia theo cấp) còn `g_nScreenWidth/Height` = phần dùng ở lệnh 1 nên Core cắt theo phần dùng.
- **Blit**: uv = dùng/cấp ở cả ba chỗ (RT→RT2, RT→khung, RT2→khung). Mép phần dùng không lem: thu nhỏ/1:1 mẫu cuối ≤ dùng − 0,5 texel, RT→RT2 là POINT, phóng to/lắc thì mép nằm ngoài khung.
- **RT2** (phóng to có lọc nét): cấp = 2 × khung × `TheGioiRT2Cap`, phần dùng = 2 × RT dùng; không thể cố định 2 × khung vì zoom 950 × lề 1120 = 1064 ‰ > khung.
- **Cấp sẵn + chạm** (`TheGioiRTSan=1`, khi `TheGioiRT=1`): khung đầu vào thế giới cấp RT rồi đổi đích + Clear + trả đích (lớp SDL_GPU mở pass rỗng load-op CLEAR, XOANEN 13/09) để driver cam kết bộ nhớ ngay trong màn nạp; RT2 cấp sẵn + chạm khi S3Client báo `ZoomToiThieu < 100` qua **lệnh 11** (`JxLiaCanh Camera_DocMap`, đến sau khung đầu ~1 s vì chờ có map) và `Rep3ZoomNet=1`. Fold 7: RT 1768×1591 ≈ 11 MB + RT2 ≈ 21 MB RAM thường trực. Log: `[TGCAP] cham san RT WxH [+ RT2] (zoom toi thieu N)`, `[TGCAP] zoom toi thieu 1000 -> 800 (lenh 11)`.
- Chi phí đổi lại: mỗi khung đi đường RT tô/lưu cả texture cấp (Adreno resolve mọi bin trong render area): +~8 MB/khung khi RT bật do quá tải mà không zoom — phiên đo nhịp và tôi cùng cho là nhỏ; Clear cả texture rẻ trên tile GPU.

## 3. Thử máy ảo (1040×604)

| Lượt | Kết quả |
|---|---|
| ZoomThu=80 | RT cấp 1 lần `cap 1768x1026 (dung 1040x604, muc 1700)`; RT2 1 lần; zoom 1000→950→…→800 không cấp lại; lắc lề 1120 không cấp lại |
| ZoomThu=150 | 9 bước zoom 1000→1500 + lắc lề 1120 (dùng 1747 ≤ cấp 1768) không cấp lại |
| A/B 100 %: `TheGioiRTEp=1` (qua RT cấp 1768) so `TheGioiRTEp=0` (vẽ thẳng) | ảnh thế giới trùng nhau sau khi bù 1 px (nhân vật đứng lệch 1 px giữa hai lần đăng nhập, bảng tên cũng lệch đúng 1 px như thế giới) → uv dùng/cấp đúng |
| ZoomThu=80 bản cuối | `cham san RT` ở khung đầu, `zoom toi thieu 1000 -> 800 (lenh 11)`, `RT2 cap 2392x1390 (dung 2080x1208)` + `cham san RT + RT2` trước khi zoom, sau đó zoom 1000→800 không cấp lại; không sập |

Chưa đo được trên máy ảo: chi phí thật (LDPlayer không có [TAI-DO]); chủ thử Fold 7: chụm/dang ngón + lia liên tục, xem `[VE-GIAT]`/khung dài trong log 8765.

## 4. Còn lại

- Cấp sẵn khối atlas trong màn nạp (đề xuất của phiên đo nhịp, việc của họ).
- Nếu chủ đổi `ZoomToiDa` > 150 hoặc `LiaLacLe` > 112: nâng `TheGioiRTCap` tương ứng, không thì lần đầu vượt sẽ cấp lại một lần (nấc 250 ‰).


---

# THANH KÉO ZOOM + CHỮ THẾ GIỚI SẮC NÉT KHI ZOOM — 14/09 14:0x (bản **109141405**)

Chủ 14/09 13:2x: *"Làm nút kéo điều chỉnh zoom cho khỏi ảnh hưởng tới lúc kích vào màn chơi. Và lúc zoom rộng chữ bị mờ đi không nhìn rõ."*

## 1. Bản

| | |
|---|---|
| Mã mới | `S3Client/Ui/UiCase/UiZoomThanh.cpp/.h` (`KUiZoomThanh`, chỉ JX_MOBILE, thêm vào `android/CMakeLists.txt` + `ios/CMakeLists.txt`) |
| Mã sửa | `Platform/JxLiaCanh.cpp/.h` (công tắc chụm + API thanh kéo), `Platform/KSdlApp.cpp` (vuốt dọc trên thanh = kéo nút), `Ui/UiCase/UiOptions2.h/.cpp` (13 công tắc), `Ui/UiShell.cpp` (mở/đóng), `Ui/Elem/UiToaDoMobile.inc` (khoá trình chỉnh), `Represent/Represent3/KRepresentShell3.cpp/.h` (khối [TG], [CHUNET]; phiên đo nhịp soi chéo). `kiem --pc` ĐẠT |
| Dữ liệu (lớp ghi đè + máy ảo + dt_v4) | `ui\ui3\uizoomthanh.ini` (mới), `ui\ui3\uioptions2.ini` (13 tên, Top 58, cách 23: `android/sinh_uioptions2_chum.py`), `ui\uitoado_danhsach.ini` (+`KUiZoomThanh|Main=Thanh zoom,1`), `spr\ui3\uizoomthanh\thanh.spr` + `nut.spr` (`android/anh_zoomthanh.py`, SPR 8 bit bảng màu xám), `config.ini [Client] TheGioiRTChu=1` |
| Kịch bản | `android/va_nguon_zoomthanh_1409.py` (A–H, idempotent) |

## 2. Thanh kéo zoom (`KUiZoomThanh`)

- Thanh dọc 28×160 ở mép trái (ini `[Main] Left=4 Top=330`, neo mép như mọi ô HUD); dấu **+** trên = phóng to (`ZoomToiThieu`, 80 %), dấu **−** dưới = nhìn rộng (`ZoomToiDa` của map, 150 %). Nút = `KWndScrollBar` dọc (`Type=1`), giá trị 0..(ToiDa−ToiThieu)/ZoomBuoc, mã đặt lại khoảng theo map mỗi khung.
- Bấm vào thanh trên/dưới nút = một nấc 5 %; kéo nút = liên tục. Zoom trôi mượt tới đích (`JxLia_ZoomDatMuot`: đặt `s_nZoomDich`, `JxLia_Nhip` đuổi hàm mũ như chụm kiểu 3D) và nhớ vào `UserData\CameraMobile.ini` như chụm.
- Nút tự chạy theo zoom khi map đổi mức / chụm (trừ 700 ms sau khi người chơi vừa kéo). Map hoặc người chơi tắt zoom ("Nhìn rộng") → thanh không vẽ và **chạm xuyên qua** (`PtInWindow` trả 0).
- `KSdlApp`: vuốt DỌC trên giao diện vốn là CUỘN danh sách (`CHAM_CUON`) nên kéo nút không được; thêm `JxZoomThanh_TaiDiem(x, y)`: điểm đặt ngón trên thanh → rơi xuống `CHAM_KEO` (giữ chuột trái rồi rê). Bấm liên tiếp < 400 ms trên thanh không thành nháy đúp (KWndScrollBar không nhận DBLCLK).
- Trình chỉnh giao diện: khoá `KUiZoomThanh|Main` (không cho to nhỏ) → người chơi kéo thanh sang chỗ khác / giấu.

## 3. Công tắc "Chụm zoom" (Cài đặt > TÙY CHỌN > Tối ưu, hàng 7)

Mặc định **TẮT**: hai ngón đặt lên bản đồ không còn thành chụm zoom (gốc "ảnh hưởng tới lúc kích vào màn chơi": ngón 1 đang chờ/lia/kéo trên bản đồ + ngón 2 chạm bản đồ = chụm, huỷ cả cú chạm của ngón 1 — hay xảy ra khi chơi hai ngón cái). Bật lại thì chụm như 109141317 (Zoom nhanh/chậm vẫn áp cho chụm). `JxLia_ChumDuoc` trả false khi tắt; lưu `uiautoconfig.ini [Options2] ChumZoom`.

## 4. Chữ thế giới sắc nét khi zoom ([CHUNET], Represent3 khối [TG])

- Gốc: tên NPC/người chơi, chat trên đầu, tên vật rơi, nền mờ sau tên, thanh máu đều vẽ vào ảnh RT rồi bị thu nhỏ (150 % = chữ 12 px còn 8 px, nhoè) hoặc phóng to.
- Sửa: khi đang vẽ RT và có zoom/lắc (`TgChuXep()`), `OutputText` / `OutputVNText` / `OutputRichText` và `DrawPrimitives(RU_T_SHADOW, toạ độ thế giới)` không vẽ mà xếp hàng (`s_TgChu`, tham số gốc: toạ độ thế giới + nZ); lệnh 2 sau blit gọi lại y hệt (`TgChuVe()`), `CoordinateTransform` đi qua cửa toạ độ (`m_nTgTrangThai == 0`) nên đúng chỗ, **cỡ thật** như nhãn game 3D. Khung chỉ blit (K=2) vẽ lại hàng của khung trước. Zoom 100 % không lắc: không xếp, y hệt cũ. Chữ giờ nằm trên mọi thân sprite (không bị cây che).
- `[Client] TheGioiRTChu=0` tắt (chữ lại theo ảnh RT). Log `[CHUNET] lop chu the gioi ve sau blit khi zoom / lac: 1`.

## 5. Thử máy ảo (1040×604, bản cuối)

| Việc | Kết quả |
|---|---|
| Vào thế giới | thanh hiện mép trái, nút ở 100 %; `[ZOOMTHANH] tuy chon: chum hai ngon zoom=0` |
| Bấm dưới nút | 105 % (một nấc), `[ZOOM] zoom 1000 -> 1050` |
| Kéo nút xuống / lên (swipe 1,2 s) | 150 % / 80 %, log từng nấc; trước khi vá KSdlApp thì vuốt dọc thành cuộn (nút không đi) |
| Hai chạm nhanh (250 ms) dưới nút | +5 % + 5 % (không mất cú thứ hai) |
| 150 % | tên NPC/người chơi/mục tiêu cỡ thật, rõ; nền mờ đi cùng chữ (`zt_150.png`) |
| 80 % | thế giới to 1,25×, chữ vẫn cỡ thật (`zt_h80.png`) |
| Tối ưu | 7 hàng, 13 công tắc, "Chụm zoom" tắt, nút Đóng không đè (`zt_i_toiuu.png`) |
| Bấm bản đồ | thanh không chắn (chỉ 28 px mép trái); chụm tắt nên không mất chạm |

Chưa thử được trên máy ảo: cảm giác kéo bằng ngón thật, chat trên đầu (OutputRichText) lúc zoom — chủ xem trên Fold 7.

## 6. Còn lại

- Đao quang (mục 10 báo cáo 3D) và bốn đề xuất tối ưu mục 12 vẫn chờ chủ chọn.
- Nếu chủ muốn thanh nằm ngang hoặc chỗ khác: đổi `uizoomthanh.ini` (Type=0 + ảnh ngang) hoặc kéo trong "Chỉnh giao diện".


---

# THANH KÉO ZOOM (b–e) + LỚP CHỮ NEO CHÂN (b–d) — 14/09 15:2x (bản **109141520**), sau ba lượt chủ thử Fold 7

Chủ: *"chưa thấy chỗ zoom kéo"* → *"zoom chưa cố định theo kích cỡ màn hình nên màn hình rộng bị che bởi kênh chat"* → *"khi zoom rộng ra thì tên bang hội với danh hiệu bị lệch"* → *"bị lỗi hở quá xa với thanh máu"*, *"không có nút tắt thanh zoom?"* → *"chụm zoom là gì? những tính năng nào không dùng dọn sạch phần cài đặt"*, *"ngôi sao chuyển sinh màu xanh chưa cố định theo tên nên bị lệch"*.

## 1. Thanh kéo zoom: nằm ngang, neo giữa mép trên qua bảng bố cục

- Gốc lỗi "bị kênh chat che": `KUiZoomThanh` không gọi `SetFitFlags` và không có trong bảng bố cục nên đứng ở toạ độ tuyệt đối (4,330) mọi màn; mọi ô HUD khác được đặt qua `ui\uitoado_macdinh.ini` (họ máy tính bảng) / `uitoado_macdinh_rong.ini` (họ điện thoại, tỉ lệ dài) với neo NeoX/NeoY.
- Sửa: thanh **nằm ngang 140×28** ngay dưới hàng icon menu, khoá `KUiZoomThanh|Main = 442,96,1000,0,1,0` (NeoX=1 = căn giữa) trong **cả hai** tệp bố cục (lớp ghi đè + máy ảo + dt_v4); `uizoomthanh.ini` `[Thanh] Type=0`, ảnh `thanh_ngang.spr` (dấu − trái = nhìn rộng, + phải = phóng to). Mã đọc `Type` để đảo chiều (`m_bNgang`); thanh dọc vẫn dùng được (Type=1). Vuốt ngang trên giao diện vốn là kéo (`CHAM_KEO`) nên không cần ngoại lệ KSdlApp (ngoại lệ dọc vẫn giữ).
- Công tắc **"Thanh zoom"** (Tối ưu, mặc định BẬT): `JxZoomThanh_DatBat` → thanh không vẽ, chạm xuyên qua.

## 2. Tối ưu còn 11 công tắc (6 hàng, Top 60 cách 27)

Bỏ **Zoom nhanh / Zoom chậm / Chụm zoom** (chủ hỏi "chụm zoom là gì": chụm = hai ngón dang/khép để zoom; nó nuốt cú chạm của ngón 1 khi chơi hai ngón cái nên đã tắt, thanh kéo thay thế). Chụm hai ngón và độ nhạy chỉ còn qua config `[Cham] ZoomChum=0/1`, `ZoomNhay` (không có trên dt_v4, mã mặc định 0). `OPTION_INDEX_COUNT2` = 11, `android/sinh_uioptions2_don.py` viết lại `[ToggleOptionsName]` (9 = Lắc camera, 10 = Thanh zoom).

## 3. Lớp chữ thế giới: neo chân nhóm, chiều cao co theo zoom, icon đi cùng

- **(b) lệch tên/bang/danh hiệu**: Core căn giữa mỗi dòng quanh `nMpsX` theo bề rộng chữ và xếp dòng theo độ cao ở toạ độ 1:1; qua cửa zoom các độ lệch đó bị thu phóng trong khi chữ giữ cỡ thật → mỗi dòng lệch một kiểu. Sửa trong `TgChuVe()`: gom các phần tử **liên tiếp cùng tâm x (±2) và gần nhau theo y (150 đơn vị)** thành một nhóm = một NPC (thanh máu hai đoạn kề nhau gộp tâm); neo nhóm = (tâm x, y chân = y lớn nhất của dòng chữ) đi qua `CoordinateTransform` (zoom/lắc); mỗi phần tử vẽ ở neo + độ lệch 1:1 không thu phóng, chữ với `TEXT_IN_SINGLE_PLANE_COORD`, bóng/icon với `bSinglePlaneCoord = TRUE`.
- **(c) "hở quá xa với thanh máu"**: chiều cao neo (`nHeightOff` = z nhỏ nhất của nhóm = chiều cao nhân vật) phải **co theo zoom** như thân sprite; chỉ phần xếp dòng (z − zMin, độ lệch y) giữ cỡ thật.
- **(d) ngôi sao chuyển sinh / icon cạnh tên**: `KNpc::PaintInfo` (mobile) gọi `DrawPrimitives(…, RU_T_IMAGE, 2)` cho 6 chỗ icon (RURank, RUIconImage ×2, RUIconImageR ×3; `#ifdef JX_MOBILE/#else` giữ nguyên dòng PC); Represent3 hiểu 2 = toạ độ thế giới + lớp thông tin → xếp hàng loại 4, vẽ sau blit cùng nhóm chữ ở cỡ thật, lệch 1:1 so với tâm nhóm (icon gia nhập nhóm với biên ±200; nhóm mở bằng icon lấy tâm từ dòng chữ đầu tiên).
- Chưa đổi: Bloom/ảnh nền; hạt mưa tuyết (KWeather, cả lô) không xếp.

## 4. Thử máy ảo (1040×604, bản cuối)

| | |
|---|---|
| Thanh | ngang giữa mép trên (x 440–580, y 96–124), kéo hai chiều, bấm nấc, tắt/bật qua công tắc |
| 80 % / 150 % | chữ + thanh máu bám sát đầu (không hở), tên/bang/danh hiệu căn giữa thẳng cột |
| Tống Kim 150 % (auto của chủ đưa vào) | ~200 nhãn cỡ thật, 55 fps, không sập |
| Tối ưu | 11 công tắc 6 hàng, "Thanh zoom" hàng cuối |

## 5. Còn lại

- Đề xuất (d) mục 12 (nhìn rộng ≥ 130 % vẽ RT nấc thấp rồi blit) — chủ đã chọn, làm tiếp ngay sau bản này (log Fold 7 14:47: GPU nghẽn lúc đông khi zoom).
- Chat trên đầu lúc zoom chưa thử được trên máy ảo (OutputRichText đi cùng nhóm).


---

# TGNAC: NHÌN RỘNG VẼ THU NHỎ THẲNG VÀO VÙNG CỠ KHUNG (đề xuất (d) mục 12, chủ chọn) — 14/09 15:4x (bản **109141532**)

Gốc (phiên đo nhịp, log Fold 7 14:47 bản 109141405): chủ để zoom bật suốt, 44/67 khung giật là trình chiếu 26–29 ms với `nop` 6–17 ms (CPU chờ GPU khi nộp) ở các đoạn đông; GPU 65 % (p90 76, max 85) — đường RT khi nhìn rộng vẽ thế giới 1:1 vào RT to (150 % = 2,25× điểm ảnh) rồi thu nhỏ lúc blit.

## 1. Bản

| | |
|---|---|
| Mã | `Represent/Represent3/D3D9onGPUi.h` + `D3D9onGPUDev.cpp` (lớp GPU: viewport lô-gic + viewport ép gắn RT + ép lọc palette; hàm xuất `Rep3Gpu_VpLogic` / `Rep3Gpu_VpEp` / `Rep3Gpu_PalLin`), `KRepresentShell3.cpp/.h` (khối [TG]); tất cả JX_MOBILE, `kiem --pc` ĐẠT; phiên đo nhịp soi chéo. Kịch bản `android/va_nguon_tgnac_1409.py` |
| Config `[Client]` | `TheGioiRTNac=1100` (‰; zoom ≥ mức này thì vẽ thu nhỏ; 0 = cách cũ). Lớp ghi đè + máy ảo; dt_v4 không thêm (mã mặc định 1100) |

## 2. Cơ chế

- Lệnh 0: khi nhìn rộng và `m_nTgZoom ≥ TheGioiRTNac`: vùng điểm ảnh thật `m_nTgPxW/H` = lô-gic × 1000/zoom = **khung × lề** (thay vì khung × zoom × lề); RT cấp/kiểm theo vùng này (TGCAP). `m_nTgW/H` (lô-gic = khung × zoom × lề) giữ nguyên nên cull của Core, cửa toạ độ, CHUNET không đổi.
- Lệnh 1: `SetViewport(vùng thật)` + `Rep3Gpu_VpLogic(m_nTgW, m_nTgH)`: VS chia toạ độ theo viewport LÔ-GIC (Core vẫn vẽ toạ độ 1:1) trong khi viewport thật nhỏ hơn → GPU tự thu nhỏ (hệ số 1000/zoom). `Rep3Gpu_PalLin(1)`: ép cờ `st0b[3]` cho ps lọc palette tuyến tính (sampler thật vẫn NEAREST vì texture là chỉ số bảng màu) → sprite thu nhỏ mượt như blit LINEAR cũ. `Rep3Gpu_VpEp(m_pTgSurf, vùng)`: viewport ép **gắn với texture RT** — ghép nền đất giữa pha đổi render target rồi `SetRenderTarget` quay lại RT (viewport bị đặt lại = cả texture → góc trên trái đen ở lần thử đầu); nay quay lại RT là giữ vùng ép, và viewport lô-gic chỉ có hiệu lực khi đích = RT đó (ghép vùng nền vào texture vùng vẫn theo cỡ thật).
- Lệnh 2: trả cả ba về 0 trước blit; uv blit = vùng thật / cấp; đích = khung × lề nên blit **1:1** (LINEAR ở 1:1 = không mờ thêm).
- Phóng to (< 100 %) và K=2 không đổi. Chất lượng: thu nhỏ bilinear tại chỗ ≈ thu nhỏ bilinear lúc blit (cùng 2×2 tap) — ảnh so `tgnac_sosanh.png`.

## 3. Thử máy ảo

| | |
|---|---|
| Lần đầu | góc trên trái đen: ghép nền đất đổi target giữa pha → sửa bằng viewport ép gắn RT (b) |
| Bản cuối | 110 % → 150 %: `[TGNAC] zoom 1100: ve thu nho vao vung khung x le (vung 1040x604, lo-gic 1144x664)`; ảnh đầy đủ, lia/zoom bình thường, chữ CHUNET đúng chỗ, 59–62 fps |
| So cũ/mới 150 % | cùng cảnh, khác biệt nhìn không ra (`tgnac_sosanh.png`) |

Chi phí thật (GPU %, W, khung dài) phải đo trên Fold 7 (log 8765): mong GPU khi nhìn rộng lúc đông xuống ~1× thay vì 2,25×.

## 4. Còn lại

- RT cấp vẫn theo `TheGioiRTCap=1700` (11 MB); với TGNAC vùng dùng ≤ khung × 1,12 nên có thể hạ cấp xuống 1200 ‰ để bớt store/khung — chờ số đo Fold 7.
- Nếu chủ thấy mờ hơn ở 150 %: đặt `TheGioiRTNac=0` so lại.

## 5. 16:0x chủ thử 109141557: "npc với player bị mờ" → TẮT mặc định

Đường cũ vẽ sprite POINT 1:1 vào ảnh đệm to rồi thu nhỏ **cả ảnh** một lần lúc ghép = siêu lấy mẫu (giữ chi tiết). TGNAC vẽ thẳng ở cỡ nhỏ nên **mỗi sprite** bị thu nhỏ ngay lúc quét, lấy mẫu bảng màu tuyến tính 4 điểm, không mipmap → thân NPC và người chơi mất chi tiết; chữ vẫn nét vì CHUNET vẽ sau ghép, khớp đúng điều chủ kêu. Tỉ lệ tròn khít (1144 × 1000/1100 = 1040) nên **không** phải lỗi lệch nửa điểm ảnh — đây là bản chất cách vẽ: muốn thấy rộng hơn trên cùng số điểm ảnh thì hoặc vẽ thừa rồi lọc (nét, tốn GPU), hoặc vẽ thẳng (rẻ, mờ).

Chốt theo luật chủ (không giảm trải nghiệm): `TheGioiRTNac` **mặc định 0 = tắt**, giữ khoá cho máy yếu; thêm `TheGioiRTNacLoc` (1 = lọc như cũ, 0 = không ép lọc: nét hơn, răng cưa) để so sau. A/B chỉ cần đổi config, không phải dựng lại. Phiên đo nhịp đo trên máy ảo (RMS Laplacian, hộp 0,250–700,560, cùng cảnh, `ZoomThu=125`; hai số đầu đo bằng **một** gói cài 109141557, chỉ đổi config):

| Đường vẽ | Độ nét |
|---|---|
| TGNAC bật (`TheGioiRTNac=1100`) | 35,8 |
| TGNAC tắt (`=0`) | 52,9 |
| Bản sạch 109141607 (mặc định) | 53,2 |

Mất 32 % chi tiết khi bật, khớp lời chủ. Phiên đo nhịp còn **xoá hẳn khoá khỏi config máy ảo** rồi chạy 109141607: log `[TGNAC] nhin rong tu 0 phan nghin`, tức mặc định 0 trong mã chạy đúng, không phụ thuộc config; vào thế giới + zoom 125 % không sập, 60 fps. Bản đưa lên: **109141607** (mã 868d88e4 + [KYNANG] của phiên ô kỹ năng). GPU khi đông sẽ tìm cách khác (giảm hiệu ứng / số người vẽ) thay vì đổi độ nét.

**Bài học (chốt với phiên đo nhịp):** đừng lấy độ phân giải thế giới làm nút điều chỉnh hiệu năng — chủ nhận ra trong vài phút. Muốn lấy lại GPU khi nhìn rộng thì hoặc chỉ thu một phần (vẽ ở khung × 1,1 rồi ghép xuống, vẫn còn siêu lấy mẫu), hoặc giảm số đối tượng / hiệu ứng vẽ lúc đông. Mọi thay đổi ảnh hưởng hình ảnh nên có khoá kiểu `TheGioiRTNac` để A/B bằng một gói cài.

**Công cụ đo** (`android/do_net_ab.py`, chốt 14/09): đo RMS Laplacian trên hộp cố định của ảnh chụp máy ảo, tự đổi một khoá config giữa hai lượt rồi trả config về cũ. `python android/do_net_ab.py --khoa TheGioiRTNac --gia-tri 0 1100` = hai lượt, in bảng; `--anh a.png b.png` = tính trên ảnh có sẵn. Đo chéo trên ảnh chụp của tôi (cảnh khác, cùng ZoomThu=125) ra **34,9 khi bật / 55,6 đường cũ**, khớp kết luận của phiên đo nhịp (35,8 / 52,9) tuy hai phép đo viết độc lập. Luật từ nay: thay đổi nào đụng hình ảnh thì chạy phép này trước khi lên dt_v4.

---

# SỐ LƯỢNG VẬT PHẨM KHÔNG HIỆN TRONG HÀNH TRANG / RƯƠNG — 14/09 16:3x

Chủ 16:2x: *"item trong hành trang - rương đồ không hiện số lượng như trước (item có số lượng thường hiện số ngay ở item luôn)"*, và *"cần tìm ra đúng nguyên nhân mới fix"*.

## 1. Nguyên nhân (chứng minh, không đoán)

`KItem::PaintItem` vẽ số lượng ở **cuối hàm**. Bản vá [VEVATPHAM 12/09 e] thêm nhánh mobile "vẽ nguyên cỡ, đặt khung ảnh chính giữa ô" (điều kiện `!g_nJxKeoAnhVatPham && oCo > 0`) — nhánh này vẽ ảnh xong rồi **`return;` ngay**, nên khối vẽ số không bao giờ chạy. Chú thích cũ ghi *"(số lượng món chồng ô phím tắt không vẽ, giống bản gốc)"*: người viết tưởng nhánh chỉ dùng cho ô phím tắt, thật ra nó là đường **mặc định** của mọi ô to hơn ảnh món (hành trang 31×31 và 44×44, ảnh 26 px) → hành trang, rương, cửa hàng đều mất số từ 12/09.

Bằng chứng: nhật ký `[SOLUONG]` cắm tạm ở đầu hàm, máy ảo 16:3x với hành trang mở:

```
[SOLUONG] mon 6 o ve 581,127 | chong 1 so 500 | ophim 0 | o that 581,127 31x31 | keo 0 | \spr\item\songjinescript.spr
```

Món có số lượng 500, không phải ô phím tắt, đi đúng nhánh `keo 0` (return sớm) — màn hình không có chữ số nào.

**Hai bẫy khi đo** (ghi để lần sau khỏi mất 3 lượt dựng): (a) ngân sách nhật ký `[Ui] NhatKyVatPham` bị ô phím tắt rồi cửa sổ Trang bị tiêu hết trong ~7 giây vì chúng vẽ lại mỗi khung — phải lọc `IsStack()` mới bắt được món trong hành trang; (b) log đặt ở cuối hàm thì nằm **sau** chính cái `return` cần soi, đo mãi không ra.

## 2. Sửa

Vẽ số ngay trong nhánh đó, trước `return`, đặt theo **ô thật** (`g_nJxVeVatPhamX/Y/W/H`) chứ không theo lưới 27 px của bản PC — ô to nhỏ thế nào số cũng nằm góc dưới phải ô; vẫn bỏ qua ô phím tắt (`!ispos_immediacy`) như bản gốc. Chỉ trong nhánh mobile, `kiem --pc` ĐẠT. Kịch bản `android/va_nguon_soluong_1409_c.py` (chẩn đoán: `_1409.py`, `_1409_b.py`).

## 3. Thử máy ảo

Hành trang hiện 500 / 367 / 6 / 2 ở góc dưới phải từng ô, chữ vàng đọc rõ trên nền ô tối; ô phím tắt vẫn chỉ một số (46) do thanh người chơi vẽ, không bị vẽ đè hai lần. Rương và cửa hàng dùng chung `KWndObjectMatrix` → cùng đường vẽ, chủ kiểm lại khi mở rương.

**Nhánh kéo ảnh** (`[Ui] KeoAnhVatPham=1`, mặc định tắt) không `return` nên rơi xuống khối cuối hàm; phiên đo nhịp soi ra là nó vẫn dùng lưới 27 px. Đã cho khối đó dùng ô thật khi có (`g_nJxVeVatPham* > 0`), phần của bản PC giữ **nguyên văn** trong nhánh `#else` để `kiem --pc` ĐẠT. Thử máy ảo với khoá bật: số cũng nằm đúng góc dưới phải (2 / 8 / 6 / 500 / 367), rồi gỡ khoá khỏi config máy ảo.

---

# [GONMAN 14/09] DỌN MÀN CHƠI: BỎ NÚT "CHỈNH GIAO DIỆN", BỎ "Ô ĐÃ GIẤU", NÚT HAI MŨI TÊN = ẨN HẾT NÚT KỸ NĂNG

Chủ 20:0x: *"tôi muốn bỏ chỉnh giao diện ở màn hình đi / bỏ luôn ô đã giấu ở chỉnh giao diện / nút như trên hình làm lại thành nút ẩn hết các nút kỹ năng"* (ảnh: nút tròn hai mũi tên).

| Việc | Làm gì | Ghi chú |
|---|---|---|
| Bỏ nút nổi "Chỉnh giao diện" | `UiToaDoMobile.inc`: không vẽ và không nhận chạm ở màn chơi (rào `JX_MOBILE`, bản PC giữ nguyên) | Vào trình chỉnh bằng **Cài đặt > TÙY CHỌN > Chỉnh giao diện** (`UiOptions.cpp:248, 320`) — đã thử, mở đúng |
| Bỏ "Ô đã giấu" | `SuaGd_DanhSachNutA`: hàng nút A còn **Đổi ô - Gốc ô - Hoàn tác** | Ô nào **đang** giấu vẫn giấu; muốn hiện lại thì bấm **Xoá hết** (trả bố cục về mặc định) |
| Nút hai mũi tên | `JxCanDieuKhien.cpp`: chạm = **ẩn / hiện cả cụm nút kỹ năng** (nút chính + 8 ô phụ); nút này vẫn hiện để bấm lại; chạm cũng thoát chế độ gắn | Đang ẩn thì chạm vào vùng đó **rơi thẳng xuống bản đồ**. Không nhớ qua lần chơi sau: mở lại game là hiện lại |

**Gắn kỹ năng vào ô sau khi đổi nút**: vẫn còn đường cũ — bảng chọn kỹ năng tự bật chế độ gắn (`JxCanDieuKhien.cpp:1888/1910/1920`), gắn xong tự thoát (`1575`). Chạm nút hai mũi tên trong lúc đang gắn thì thoát chế độ gắn, giữ đúng đường huỷ như trước.

`kiem --pc` ĐẠT (mọi thay đổi trong rào `JX_MOBILE`, **chú thích cũng phải nằm trong rào** — lần đầu để chú thích ngoài rào nên bộ kiểm báo HỎNG). Kịch bản `android/va_nguon_gonman_1409.py`. Máy ảo: chữ "Chỉnh giao diện" biến mất khỏi màn chơi; chạm nút hai mũi tên ẩn hết 9 nút, chạm lần nữa hiện lại (nhật ký `[KYNANG] an het nut ky nang = 1 / = 0`); trình chỉnh mở từ Cài đặt, hàng nút chỉ còn ba cái.

## [GONMAN 14/09 b] Ô vuông vàng và thông báo khi gắn kỹ năng

Chủ 20:2x sau khi thử 109142015: *"bỏ vẽ ô vuông màu vàng ở ô ẩn kỹ năng mới vừa làm - và vẽ lại ô vuông màu vàng như trên hình cho đẹp"*, *"tắt mấy thông báo khi chọn kỹ năng vào ô kỹ năng đi"*.

| Chỗ | Trước | Nay |
|---|---|---|
| Nút hai mũi tên (nay là nút ẩn) | chế độ gắn tô **ô vuông vàng** đè lên nút + hai dòng hướng dẫn giữa màn | bỏ hẳn cả hai |
| Ô đang chọn để gắn | ô vuông vàng đè lên nút tròn, nhìn chỏi | **vòng tròn sáng** `effect_skill.spr` (ảnh của nút đang cầm), vừa khít nút; thiếu ảnh mới lùi về ô vuông |
| Gắn xong | báo "Đã gắn vào ô chính" / "Đã gắn vào ô phụ N" | không báo nữa (ô sáng lên là đủ thấy) |

**Giữ lại** các thông báo nói LÝ DO không làm được: "Ô chính chỉ thay được, không gỡ được", "Kỹ năng này chưa nằm ở ô nào", "Hết ô trống: chạm ô phụ muốn thay", "Ô chính vẽ kỹ năng cơ bản" — bỏ nốt thì người chơi chạm mà không hiểu vì sao không ăn.

`kiem --pc` ĐẠT. Máy ảo: vào chế độ gắn, nút hai mũi tên sạch, ô đang chọn là vòng tròn sáng; chọn kỹ năng trong bảng thì gắn xong không còn chữ nào giữa màn (nhật ký vẫn ghi `[KYNANG] gan ky nang 155 vao o 7`). Kịch bản `android/va_nguon_gonman_1409_b.py`.

**Bẫy khi vá**: bộ bảo vệ "số byte cao không đổi" của kịch bản vá chặn đúng khi mình CỐ Ý xoá chuỗi tiếng Việt — phải đổi phép kiểm thành "giảm đúng bằng phần xoá" (`cao(s) - cao(cu) + cao(moi)`), đừng bỏ phép kiểm.

## [KMTRON + KNHOTRO 14/09] Icon Kinh Mạch tròn, chặn kỹ năng hỗ trợ xuống ô kỹ năng

Chủ 21:0x: *"icon kinh mạch ở màng hình đang hình vuông tôi muốn bạn làm lại hình tròn"*, *"fix lại các kỹ năng nào thuộc dạng hỗ trợ (không dùng được) thì không cho bỏ xuống ô kỹ năng"*.

### 1. Icon Kinh Mạch tròn

`android/anh_kinhmach_tron.py` đọc thẳng `spr/UiNew/UiToolsControlBar/kinh_mach_m.spr` (48×48, 2 khung), cắt tròn bán kính 23 với biên mềm 1,5 px, thêm vòng vàng `#F0D070` dày 2 px và vòng trong tối `#2A1E0A` dày 1 px cho khớp các icon tròn bên cạnh, rồi ghi đè **đúng tên - đúng cỡ - đúng số khung** nên không phải đụng ini nào. Ghi vào cả lớp ghi đè `android/du_lieu_ghi_de` lẫn `D:\jx1_android_data`; chạy lại được (bốn góc đã trong suốt thì bỏ qua).

Bẫy đã tránh: alpha trong RLE của SPR là **8 bit** (xem `spr-alpha-8bit-sinh-anh-pil`), bảng màu ghi lại bằng `quantize(255, FASTOCTREE)` chứ không giữ bảng gốc vì hai vòng viền là màu mới.

### 2. Chặn kỹ năng hỗ trợ

`KyNang_DungDuoc()` trong `JxCanDieuKhien.cpp`, gọi ở **đầu** `JxKyNang_GanKyNang` nên chặn cả hai đường: chạm thẳng vào kỹ năng trong bảng, và bấm "Phím chính" / "Phím phụ" của bảng chọn.

**Dùng được = có trong bảng đánh TRÁI (`GDI_LEFT_ENABLE_SKILLS`) HOẶC bảng đánh PHẢI (`GDI_RIGHT_ENABLE_SKILLS`).** Hai hàm Core đó (`KSkillList.cpp:700`, `:767`) là chỗ duy nhất đòi `GetSkillLRInfo()` phải là `BothSkill` / `LeftOnlySkill` / `RightOnlySkill`; kỹ năng hỗ trợ - nội công có `LRSkill = 3 = NoneSkill` ("không tay nào") nên rơi khỏi cả hai. Hai hàm cũng đòi **đã học** (`SkillLevel > 0`) và **đủ cấp nhân vật** (`GetSkillReqLevel`).

**KHÔNG được dùng `GDI_FIGHT_SKILLS` làm phép thử** — đây là lỗi của bản a: `GetSkillSortList` (`KSkillList.cpp:638`) chỉ loại kỹ năng GỐC (`IsBase()` = `m_nAttrib <= 1`), còn kỹ năng **bị động** (`SKILL_SS_PassivityNpcState`) vẫn nằm nguyên trong bảng. Bản a nhận cả ba bảng nên "Thê Vân Tung" (id 160, `Property` = "hỗ trợ chiến đấu - bị động") vẫn gắn được như cũ.

`KyNang_DonOChet` **vẫn giữ cả ba bảng**: ở đó thà lọt còn hơn xoá nhầm ô của người chơi, ngược chiều với chỗ này. Bảng PHẢI ở `KyNang_DungDuoc` đọc lại **mỗi lần** (không nhớ) vì đây là việc theo cú chạm chứ không phải mỗi khung, mà bảng cũ sai ngay sau khi đổi phái / học thêm chiêu.

Thiếu bảng thì **không chặn**: `g_pCoreShell == NULL`, `uId == 0`, hay `s_nKNCo1 <= 1` (vừa vào game / đang chuyển phái) đều trả `true`.

### 3. Thử máy ảo (nhân vật Võ Đang cấp 120, bản `109142107`)

| id | Tên | Property | LRSkill | Kết quả |
|---|---|---|---|---|
| 151 | Võ Đang Kiếm pháp | Hỗ trợ bị động | 3 | **chặn** (đã thử) |
| 152 | Võ Đang Quyền Pháp | Hỗ trợ bị động | 3 | **chặn** (đã thử) |
| 160 | Thê Vân Tung | hỗ trợ chiến đấu - bị động | 3 | **chặn** (đã thử, cả "Phím chính" lẫn "Phím phụ") |
| 166 | Thái Cực Thần Công | Hỗ trợ bị động | 3 | **chặn** (tính từ bảng) |
| 1078/1079 | Tạo Hoá Thái Thanh / Kiếm Thuỷ Tinh Hà | Công kích | 0 | **chặn** vì `ReqLevel` 150 > cấp 120 |
| 153 | Nộ Lôi Chưởng | Công kích nội công | 0 | gắn được (đã thử) |
| 159 | Thất Tinh Trận | Vòng tròn hỗ trợ công kích | 2 | gắn được (đã thử) |
| 155, 157, 158, 164, 165, 267, 365, 368 | | Công kích / hỗ trợ chủ động | 0 hoặc 2 | gắn được |

Thông báo khi bị chặn: **"Kỹ năng hỗ trợ: không gắn vào ô được"** (đã soi ảnh, chữ đúng, không vỡ font). Nhật ký: `[KYNANG] ky nang %u khong dung duoc (khong co trong bang danh trai lan phai) -> khong gan vao o`.

`kiem --pc` ĐẠT. Kịch bản `android/va_nguon_kynang_hotro_1409.py` (bản a) + `android/va_nguon_kynang_hotro_1409_b.py` (sửa lại phép thử).

### 4. Còn lại

Kỹ năng cấp 150 bị chặn ở nhân vật cấp 120 là **đúng ý "không dùng được"**, nhưng nếu chủ muốn cho gắn sẵn để lên cấp là dùng được ngay thì bỏ điều kiện cấp phải đổi cách hỏi Core (hai bảng trái/phải đã lọc sẵn theo cấp, không tách ra được) — lúc đó mới cần thêm một đường hỏi mới.

## [GONMAN 14/09 c] Tắt nốt thông báo khi bỏ kỹ năng vào ô, và một lỗi bản b tự gây ra

Chủ 21:3x: *"bạn chưa tắt thông báo khi bỏ kỹ năng vào ô kỹ năng"*.

### 1. Lỗi bản b tự gây ra, nặng hơn cả chuyện thông báo

Khi gỡ dòng báo "Đã gắn vào ô phụ N", bản b xoá **cả khối** trong nhánh "còn ô trống" của `KyNang_ChamBangChon`, kể cả ba dòng **làm việc**:

```
s_nKNCheDoGan = 1;   s_nKNOChon = i;   JxKyNang_GanKyNang(o.uGenre, o.uId);
```

Hậu quả: bấm **"Phím phụ"** trong bảng chọn lúc chưa chạm ô nào thì **không gắn được gì, im lặng**. Đã khôi phục ba dòng, chỉ bỏ dòng báo.

**Bài học**: kịch bản vá thay cả khối bằng một dòng chú thích thì phải đọc lại khối cũ xem trong đó có gì ngoài dòng định gỡ. Bộ bảo vệ "số byte cao" không bắt được vì ba dòng kia là ASCII.

### 2. Hai thông báo còn lại trên đường BỎ KỸ NĂNG VÀO Ô

| Thông báo | Nay |
|---|---|
| "Hết ô trống: chạm ô phụ muốn thay" | bỏ; vẫn vào chế độ gắn để người chơi chạm ô muốn thay |
| "Kỹ năng hỗ trợ: không gắn vào ô được" | bỏ; vẫn chặn, `g_DebugLog` vẫn ghi lý do |

**Giữ lại** thông báo của đường **GỠ** kỹ năng: "Đã gỡ khỏi ô phụ", "Ô chính chỉ thay được, không gỡ được", "Kỹ năng này chưa nằm ở ô nào", "Ô chính về kỹ năng cơ bản". Chủ nói về lúc **bỏ vào** ô, không phải lúc gỡ. Lần trước tôi tự giữ lại nhóm "lý do" và chủ nhắc lại lần hai, nên lần này bỏ hết phần bỏ-vào.

### 3. Thử máy ảo (bản `109142140`, nhân vật Võ Đang 120, trong Tống Kim)

- chạm "Võ Đang Kiếm pháp" (bị động): không gắn, **không còn chữ nào** giữa màn; nhật ký `[KYNANG] ky nang 151 khong dung duoc ...`
- gỡ một ô rồi bấm "Phím phụ" cho kỹ năng khác: `[KYNANG] gan ky nang 164 vao o 5` — đường bản b làm hỏng đã chạy lại
- gắn xong không có chữ nào hiện
- chuỗi "Hết ô trống" đã **biến mất khỏi `libmain.so`** cả hai ABI

`kiem --pc` ĐẠT. Kịch bản `android/va_nguon_gonman_1409_c.py`.

### 4. Một chỗ chủ nên biết: kỹ năng cấp 0 cũng bị chặn

Hai bảng trái/phải của Core đòi `SkillLevel > 0`, nên kỹ năng **đã có trong bảng võ công nhưng chưa cộng điểm** (ví dụ "Thương Hải Minh Nguyệt", "Đẳng cấp hiện thời: 0") cũng không gắn xuống ô được. Trước [KNHOTRO] thì gắn được. Đúng nghĩa "chưa dùng được", nhưng là thay đổi ngoài câu chữ của chủ — nếu chủ muốn cho gắn sẵn kỹ năng cấp 0 thì phải thêm một đường hỏi Core mới, hai bảng kia không tách điều kiện ra được.
