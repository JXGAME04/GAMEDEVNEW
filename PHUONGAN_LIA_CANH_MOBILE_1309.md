# PHƯƠNG ÁN "LIA CẢNH" CHO JX1 MOBILE — MỨC 1, LÀM TỪNG BƯỚC

> **Cập nhật 13/09 tối:** chủ chốt *"thiết kế sao giống bản 3D để tiện sử dụng, bạn làm các bước tôi test"* → bước 1a đã làm theo bản **MỘT ngón** (kéo trên bản đồ = lia, như game 3D một ngón kéo = quay; đi bằng cần hoặc chạm), không phải hai ngón như mục 0 dưới đây. Kết quả, bản APK và cách thử: `BANGIAO_LIA_CANH_MOBILE_1309.md`. Phần còn lại của tài liệu này giữ nguyên làm căn cứ kỹ thuật (mục 2 là phần đúng nhất).

> Chủ 13/09: *"Oke làm từng bước"* sau khi chọn mức 1: **chép 100 % cách điều khiển camera của Kiếm Võng Giang Hồ nhưng thay "quay" bằng "lia"**
> (một cử chỉ kéo là dịch khung nhìn sang chỗ khác, thả tay chờ 1 s rồi tự trôi về nhân vật). Zoom nhìn rộng ra là bước 2, tách riêng.
> Toàn bộ dưới đây đọc từ mã thật của worktree (HEAD `c6f70252`, cùng `D:\GAMEDEVNEW_wt_mobile`). Đối chiếu hành vi gốc: `PHANTICH_KIEMVONG_GIANGHO_3D_1309.md` mục 3.2, 4.

## 0. Tóm tắt cho chủ quyết

| | |
|---|---|
| Làm gì | Hai ngón đặt lên vùng bản đồ (không phải giao diện, không phải cần, không phải nút kỹ năng) rồi kéo = **lia cảnh** theo tay 1:1. Thả ra: chờ **1 s** rồi khung nhìn **trôi mượt về nhân vật**. Nhân vật bước đi / cầm cần / đổi map = về ngay. |
| Vì sao hai ngón | JX1 mobile đã dùng **một ngón kéo trên bản đồ = giữ chuột trái rê = đi liên tục** (`KSdlApp.cpp:1167`). Hai ngón hiện **chưa dùng cho gì** (ghi chú 09/09: "chạm hai ngón KHÔNG dùng làm lối tắt nào"). Bên game 3D một ngón quay được vì họ đi bằng cần; JX1 giữ nguyên thói quen đi bằng ngón. |
| Làm bằng gì | **Cơ chế có sẵn của JX1**: `KScenePlaceC::FollowMapMove` + `SetMapFocusPositionOffset` qua `g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP / GSMOI_SCENE_MAP_FOCUS_OFFSET)`. Bản PC đã dùng nó: **Ctrl + chuột phải kéo trên bản đồ nhỏ = lia cảnh, 3 s sau nhảy về** (`UiMiniMap.cpp:340-364, 461`). Tức là dịch **tiêu điểm** chứ không dịch phép vẽ → không đụng `DrawImage2DFlat`, không đụng 35 chỗ chữ neo thế giới, không lặp lại 4 lỗi zoom 12/09. |
| Đụng mã | 1 tệp mới `Sources/S3Client/Platform/JxLiaCanh.cpp` (~300 dòng, chỉ `JX_MOBILE`), ~40 dòng móc trong `KSdlApp.cpp/.h`, 1 dòng trong `GameSpaceChangedNotify.cpp`, khoá `[Cham]` trong `config.ini`. **PC không đổi một byte** (đúng luật 11/09), iOS tự có vì cùng đường SDL. |
| Lùi | `[Cham] LiaCanh=0` + khởi động lại 8765, không cần APK. |
| Rủi ro chính | Lia xa qua ranh vùng 512 → nạp vùng + dựng nền (`[PGND]` ~17 ms/vùng) như khi chạy bộ; chặn bằng giới hạn lia mặc định 60 % màn và đo `[PGND]` trước/sau. |
| Việc chủ | Duyệt mục 6 (khoá, số mặc định) rồi tôi làm bước 1; thử hai ngón trên Fold 7 theo mục 8 (máy ảo LDPlayer không giả lập được hai ngón, tôi tự thử bằng khoá gỡ lỗi `LiaThu`). |

## 1. Hành vi chép từ game 3D (và cái đổi)

| Của họ (`GameCamera` / `TouchControl`) | Bản JX1 mobile |
|---|---|
| Ngón đặt trên UI (`UICamera.Raycast`) → bỏ | Cả hai ngón phải **ngoài** `JxUi_CoGiaoDienTaiDiem`, ngoài `JxKyNang_TrungNut`, ngoài vùng cần `JxCan_TrongVung` |
| Ngón 1 kéo → `OnRotate` (yaw/pitch) | **Hai ngón** kéo → lia (dịch tiêu điểm); ngón 1 giữ nguyên nghĩa cũ |
| Ngưỡng 1 px bỏ rung tay | Ngưỡng `LiaNguong=3` px (màn điện thoại cỡ khung 616 cao) |
| Lọc mượt `dt=(delta+cũ)·0,2` | Lia theo tay **1:1** (lọc mượt chỉ hợp với quay); có khoá `LiaMuot` nếu chủ muốn |
| Hai ngón: `OnWheelChg((d−d0)·−0,01)` | Ghi lại khoảng cách hai ngón để **bước 2** (zoom); bước 1 không dùng |
| Thả: `bTouch=false; fTouchWaitTime=1; bFollowSmooth=true` | Thả: chờ `LiaChoVeMs=1000` rồi trôi về |
| Về: `MoveTowardsAngle` 40°/s | Về: dịch lệch về 0 với tốc độ `LiaVeTocDo` px/s, có êm cuối (`LiaVeEm`) |
| Vị trí luôn bám nhân vật, chỉ góc tự về | Đang lia mà nhân vật đi → khung vẫn bám nhân vật **kèm lệch**; thả tay → về **ngay** (không chờ 1 s) |
| Kẹp pitch, kẹp khoảng cách | Kẹp lệch theo `LiaXaNgang/LiaXaDoc` (% màn) **và** theo biên bản đồ (`nFocusMin/Max` của `GSMOI_SCENE_MAP_INFO`) |
| Tham số theo cảnh (`cameraInit`) | Bước 3: theo map trong `settings` |

## 2. Cơ chế có sẵn trong JX1 — bằng chứng đọc mã

### 2.1 Đường đi của "tiêu điểm"

- Nhân vật đi: `KNpcRes::SetPos(..., bFocus)` → `g_ScenePlace.SetFocusPosition(x,y,z)` (`KNpcRes.cpp:1345`).
- `KScenePlaceC::SetFocusPosition` (`KScenePlaceC.cpp:471`): **nếu `m_bFollowWithMap`** thì chỉ ghi `m_OrigFocusPosition` rồi return (khung nhìn KHÔNG đi theo nhân vật nữa cho tới lần gọi `SetMapFocusPositionOffset` kế). Nếu không: đặt `m_FocusPosition`, gọi `g_pRepresent->LookAt`, tính lại `m_RepresentArea` (vùng truy vấn vật thể) quanh tiêu điểm, chuyển vùng nạp (`m_Map.SetFocusPosition`, `ChangeLoadArea` khi qua ranh vùng), cập nhật thời tiết.
- `SetMapFocusPositionOffset(dx,dy)` (`:2070`) khi đang `FollowWithMap`: tắt cờ, `SetFocusPosition(m_OrigFocusPosition + (dx,dy))`, bật lại cờ. `FollowMapMove(0)` (`:2122`): về `SetFocusPosition(m_OrigFocusPosition)`.
- Giao diện gọi qua `g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, bật)` và `(GSMOI_SCENE_MAP_FOCUS_OFFSET, dx, dy)` (`CoreShell.cpp:25744-25747`). `GSMOI_SCENE_MAP_INFO` trả `nOrigFocusH/V` (vị trí nhân vật), `nFocusOffsetH/V`, `nFocusMin/Max` (biên bản đồ trừ nửa màn).

### 2.2 Vì sao mọi thứ tự đúng khi dịch tiêu điểm

`KRepresentShell3::LookAt` (`:2833`): `m_nLeft = nX − W/2; m_nTop = nY/2 − z·887/1024 − H/2`. Mọi phép vẽ và mọi phép chạm (`CoordinateTransform` và nghịch đảo) đều đi qua `m_nLeft/m_nTop`, nên sprite, tên, thanh máu, vòng chọn, icon NPC, chạm chọn NPC, `JxMucTieu_Khoa` **tự trùng** với cảnh đã lia. Vùng truy vấn vật thể (`m_RepresentArea`, biên 140/90/150) cũng đặt lại quanh tiêu điểm mới → người ở rìa không biến mất. Đây là khác biệt bản chất với 4 lỗi zoom (zoom sửa phép vẽ, lia chỉ đổi số đưa vào phép vẽ).

Đơn vị: X thế giới = px; **Y thế giới = 2 × px** (dòng `nY/2` trong LookAt, `(nY + …)*2` ở `KScenePlaceC.cpp:1722`). Vậy kéo tay `(dx, dy)` px → lệch tiêu điểm `(−dx, −2·dy)` (kéo sang phải = xem phần bên trái).

### 2.3 Chi phí và giới hạn

- Vùng đã nạp quanh nhân vật: 7×7 vùng (`SPWP_NUM_REGIONS_IN_PROCESS_AREA=49`), mỗi vùng **512 × 1024** đơn vị (`RWPP_AREGION_*`) = 512 × 512 px. Nửa dữ liệu sẵn ≈ 1792 px ngang, 1792 px dọc; màn rộng nhất cần nửa 684 × 308 px → **dư địa lia ≈ 1100 px ngang, 1480 px dọc** trước khi phải nạp thêm.
- Tiêu điểm qua ranh vùng (mỗi 512 px ngang / 512 px dọc): `ChangeLoadArea` + `ClearPreprocess` + cờ `SetLoadingStatus` như khi chạy bộ, kéo theo dựng nền `[PGND]` ~17 ms/vùng (đo 12/09). Vì vậy mặc định kẹp lia **60 % cỡ màn** (≈ 820 × 370 px trên Fold 7 gập, 800 × 370 trên điện thoại) → tối đa qua 1–2 ranh, y như đi bộ vài giây.
- Bẫy `FollowWithMap`: khi cờ bật, nhân vật đi mà khung không theo. Cách chặn: `JxLia_Nhip()` **mỗi khung** gọi lại `FOCUS_OFFSET` (lệch hiện tại) — `SetFocusPosition` tự return sớm nếu toạ độ không đổi nên gọi mỗi khung gần như miễn phí. Đây chính là lỗi 3 của zoom 12/09 ("LookAt chỉ chạy khi nhân vật đi") được giải bằng thiết kế, không phải bằng vá.
- Bản đồ nhỏ trên PC dùng cùng cờ (`MapScroll/MapMoveBack`); trên điện thoại không có Ctrl + chuột phải nên không tranh chấp; `JxLia_Nhip` đặt lại cờ mỗi khung nên có bị `MapMoveBack` tắt cũng bật lại.

## 3. Thiết kế mô-đun mới `JxLiaCanh.cpp` (theo mẫu `JxCanDieuKhien.cpp`)

```
extern "C":
  bool JxLia_Bat();                                  // [Cham] LiaCanh
  bool JxLia_DuocBatDau(int x1,int y1,int x2,int y2);// cả hai điểm ngoài UI / nút kỹ năng / vùng cần, đang trong game, không đang sửa giao diện
  void JxLia_BatDau(int x1,int y1,int x2,int y2);    // ghi tâm hai ngón + khoảng cách; FOLLOW_WITH_MAP=1; trạng thái LIA
  void JxLia_Keo(int x1,int y1,int x2,int y2);       // tâm mới − tâm cũ → cộng lệch (−dx, −2dy) sau ngưỡng; kẹp; (ghi d/d0 cho bước 2)
  void JxLia_Nha();                                  // → CHO_VE (mốc thời gian) hoặc VE ngay nếu nhân vật đang đi / cần đang cầm
  void JxLia_Nhip();                                 // mỗi vòng lặp (cạnh JxCan_Nhip): áp lệch; hết chờ → trôi về; về xong → FOLLOW=0, OFFSET=0
  void JxLia_DatLai();                               // đổi map / rời game / đăng nhập lại: tắt cờ, lệch = 0
  int  JxLia_DangLia();                              // cho KSdlApp / lớp khác hỏi
  void JxLia_Ve();                                   // (tuỳ chọn) vẽ mũi tên nhỏ chỉ về nhân vật khi đang lệch, dùng kho VNKU
```

Trạng thái: `KHONG → LIA → CHO_VE → VE → KHONG`. Trôi về: mỗi khung `buoc = max(LiaVeTocDo·dt, |lệch|·LiaVeEm·dt)`; `|lệch| < 2` → kết thúc. Phát hiện nhân vật đi: so `nOrigFocusH/V` (từ `GSMOI_SCENE_MAP_INFO`) giữa hai khung; cầm cần: `JxCan_DangCam()`.

Móc trong `KSdlApp::ChamSuKien` (đúng chỗ đã có nhánh **CHUM** của trình sửa giao diện, `KSdlApp.cpp:880-931`):

1. `FINGER_DOWN` ngón thứ hai (`m_nNgonDangDat==2`, không sửa giao diện, `JxLia_Bat()`), ngón 2 không trúng nút kỹ năng, không trúng cần, và `JxLia_DuocBatDau(ngón1, ngón2)`:
   - huỷ đường chuột giả lập của ngón 1: nếu `m_nCham==CHAM_KEO` → gửi `WM_LBUTTONUP` (ngừng đi); `m_nCham = CHAM_KHONG` (như nhánh CHUM đã làm);
   - `m_nLia = 1`, ghi `m_nNgon1/2`, gọi `JxLia_BatDau`; `return true`.
2. `FINGER_MOTION` khi `m_nLia`: cập nhật toạ độ ngón tương ứng, `JxLia_Keo(...)`; `return true` (cả hai ngón, để chuột giả lập không rê).
3. `FINGER_UP/CANCELED` một trong hai ngón khi `m_nLia`: `JxLia_Nha()`, `m_nLia=0`, `m_nNgon1=m_nNgon2=-1`, `m_nCham=CHAM_KHONG`; `return true`. (Ngón còn lại nếu vẫn đặt: coi như chạm mới chưa bắt đầu — giống CHUM.)
4. Chuột giả lập `MOUSE_MOTION/BUTTON_UP` tới trong lúc `m_nLia` → nuốt (`m_nCham` đã `CHAM_KHONG` nên nhánh hiện có tự nuốt).

Móc khác: `KSdlApp` vòng lặp (`:673`) thêm `JxLia_Nhip();` cạnh `JxCan_Nhip()`; `GameSpaceChangedNotify.cpp` `case GDCNI_SWITCHING_MAPMODE` (đổi map THẬT, `KScenePlaceC::OpenPlace`) thêm `JxLia_DatLai()`; `UiShell.cpp:383` thêm `JxLia_Ve()` trước `JxCan_Ve()` nếu chủ muốn mũi tên chỉ về.

Không đụng: `Represent3`, `Core` (dùng API sẵn), `Wnds.cpp`, mã dùng chung PC.

## 4. Tương tác với những thứ đang chạy

| Thứ | Ảnh hưởng | Xử lý |
|---|---|---|
| `[TG]` thế giới vẽ vào RT (K=2 khi 120 Hz đông) | Khung "chỉ blit" đặt `m_nLeft/m_nTop = gốc lúc vẽ RT` → lớp phủ trùng ảnh RT, lệch tay lia trễ 1 khung ở K=2 | Chấp nhận (8 ms); không cần sửa |
| `SinhHover` bơm `WM_MOUSEMOVE` mỗi khung | Con trỏ giả lập nằm ở chỗ chạm cũ; sau lia thì NPC dưới con trỏ đổi | Khi bắt đầu lia: `MsgProc(WM_MOUSEMOVE)` về tâm hai ngón một lần; khi về xong không làm gì thêm |
| WAuto / đánh tự động đang đi | Nhân vật đi → thả tay là về ngay | theo luật mục 1 |
| Cưỡi ngựa, chết, hồi sinh, `GDCNI_SWITCHING_SCENEPLACE` (nạp vùng) | không đổi map thật | không đặt lại; chỉ `GDCNI_SWITCHING_MAPMODE` mới đặt lại |
| Trình sửa giao diện `UiToaDo_DangSua` | đã chiếm hai ngón (CHUM) | `JxLia_DuocBatDau` trả 0 khi đang sửa |
| Bảng Auto/khung lớn đang mở | hai ngón trên khung = UI → không lia | tự nhiên |
| Bản đồ nhỏ PC (`MapMoveBack`) | có thể tắt cờ giữa chừng | `JxLia_Nhip` bật lại mỗi khung |

## 5. Làm từng bước

| Bước | Việc | Sản phẩm | Cách kiểm |
|---|---|---|---|
| **1a** (nửa buổi) | Viết `JxLiaCanh.cpp` + móc KSdlApp + khoá config + đặt lại khi đổi map; khoá gỡ lỗi `[Cham] LiaThu=dx,dy` (bật thì 3 s sau khi vào game tự lia (dx,dy) rồi tự về — để thử trên máy ảo không có hai ngón) | APK `jx1mobile-13xx-lia-a.apk` | Máy ảo: cảnh dịch đúng hướng, tên/thanh máu/vòng chọn trùng, chạm NPC ở rìa mở thoại, `[LIA]` log; `[PGND]` mỗi phút trước/sau lia 60 % |
| **1b** (chủ thử) | Fold 7 hai ngón: kéo 4 hướng, thả → 1 s → trôi về; đi bộ khi đang lia; đổi map; Tống Kim đông ở K=2 | ghi nhận chủ | như mục 8 |
| **1c** | Chỉnh số theo cảm giác chủ (tốc độ về, giới hạn, ngưỡng), tắt `LiaThu`, giao bản | bản lên dt_v4 + bàn giao | |
| 2 (sau) | Zoom nhìn rộng bằng **cách C** (thế giới vẽ vào RT to hơn khung rồi blit thu nhỏ có lọc tuyến tính, tái dùng đúng đường `[TG]` `Rep3_JxTheGioi`; hai ngón chụm/giãn) — chỉ sau khi đo mép đen, `[PGND]`, và liệt kê lớp phủ vẽ sau blit | phương án riêng | |
| 3 (sau) | Tham số theo map (như `cameraInit`) + nút bật/tắt trong trình chỉnh giao diện | | |

## 6. Khoá `config.ini` đề nghị (`[Cham]`, đọc lúc vào game như `JxCan`)

```
; [LIA 13/09] hai ngon keo tren ban do = lia canh; tha 1 s roi tu troi ve nhan vat (chep cam giac Kiem Vong Giang Ho)
LiaCanh=1          ; 0 = tat han (nut lui, khong can APK)
LiaXaNgang=60      ; lech toi da ngang, % be rong khung ve (60 % ~ 800 px)
LiaXaDoc=60        ; lech toi da doc, % chieu cao khung ve
LiaNguong=3        ; px tay di chuyen it hon thi bo (rung tay)
LiaChoVeMs=1000    ; tha tay bao lau thi bat dau ve (ho: 1 s)
LiaVeTocDo=700     ; toc do ve toi thieu, px/giay
LiaVeEm=3          ; he so em cuoi (moi giay ve 6 lan phan con lai; 0 = di deu)
LiaVeKhiDi=1       ; 1 = nhan vat buoc di / cam can -> ve ngay, khong cho
LiaMuiTen=1        ; ve mui ten nho chi ve phia nhan vat khi dang lech
LiaNhatKy=0        ; 1 = ghi [LIA] ra logcat / jx_rep3.log
LiaThu=0,0         ; GO LOI: dx,dy px -> 3 s sau khi vao game tu lia roi tu ve (thu tren may ao khong co hai ngon)
```

## 7. Ước lượng và điều kiện dừng

- Mã: ~300 dòng mới + ~40 dòng móc + 12 dòng config; dựng APK ~40 s (đã có junction `D:\GAMEDEVNEW_wt_bg`, SDL đã vá; nhớ luật **đồng bộ git mobile-0809 + kiểm SDL đã vá** trước khi dựng).
- Điều kiện dừng bước 1 (không giao): chạm NPC sau khi lia mở sai đối tượng; `[PGND]` tăng quá 2 lần khi lia 60 %; khung K=2 xé hình; giao diện đăng nhập/bảng bị ảnh hưởng (không thể vì không đụng phép vẽ, nhưng vẫn nhìn).

## 8. Danh sách thử trên Fold 7 (chủ)

1. Vào thành: hai ngón đặt giữa màn, kéo trái/phải/lên/xuống: cảnh theo tay, tên và thanh máu người khác đứng đúng trên đầu họ.
2. Thả tay: đứng yên 1 s rồi cảnh trôi về, không giật, dừng đúng nhân vật.
3. Đang lia, chạm một ngón vào NPC ở rìa: mở thoại đúng NPC đó.
4. Đang lia thì bấm cần đi: cảnh về ngay và bám nhân vật.
5. Kéo tới mép giới hạn: dừng lại, không có mép đen.
6. Qua cổng đổi map: không còn lệch.
7. Tống Kim đông (K=2 khi 120 Hz): lia không xé hình, fps không tụt rõ; kéo log về xem `[PGND]` và `[LIA]`.
