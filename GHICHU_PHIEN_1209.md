# GHI CHÚ PHIÊN LÀM VIỆC 12/09 — BẢN MOBILE

> Đọc kèm `KEHOACH_GIAODIEN_MOBILE_1209.md` (kế hoạch giao diện sắp làm).
> Tệp này ghi lại **ràng buộc của chủ, kho tham khảo, quy trình dựng/đẩy, việc đã làm và các bẫy đã vấp**
> trong phiên 12/09, để phiên sau không phải dò lại.

---

## 1. Ràng buộc cố định của chủ game

1. **Chỉ sửa bản mobile, bản PC không được đổi.** Mọi thay đổi hành vi nằm trong `#ifdef JX_ANDROID`
   (mã mới của phần bố cục nên dùng `JX_MOBILE` để iOS dùng lại).
2. **Không xoá mã của tính năng WAuto** mà bản mobile không dùng — client PC dùng chung một bản nguồn.
3. **Mỗi lần dựng bản mới phải gộp thêm commit mới của phiên làm việc kia** (`origin/main`) cho đồng bộ.
4. **Gộp vào `mobile-0809` chỉ bằng `gan_ff.sh`** (fast-forward, chờ cây mobile sạch) — cẩn thận tránh
   mất tính năng. Không bao giờ ép ghi.
5. **`D:\USVOLAM` và `C:\Users\nguye\Downloads\NHACTAI` CHỈ ĐỌC.** Không sửa gì bên đó.
6. **`E:\SourceTuanLe\...\bin` không bao giờ được ghi vào.**
7. **Nguồn có byte cao (TCVN3) chỉ được vá bằng kịch bản python latin-1**, kiểm số byte ≥ 0x80 trước và
   sau khi vá phải bằng nhau. Không sửa tay, không để công cụ tự đổi bảng mã.
8. **Tệp nguồn chỉ-Android phải khai trong `android/CMakeLists.txt`** hoặc rào `#ifdef JX_ANDROID`.
9. **Cả hai chuỗi Windows phải dựng 0 lỗi** trước khi giao.
10. **Sau `adb install` phải kiểm md5 của bản đã cài** trên máy.
11. **Không bao giờ dùng `git stash` trần** — kho stash dùng chung với các worktree khác.

---

## 2. Kho tham khảo (CHỈ ĐỌC)

### 2.1 `D:\USVOLAM` — dự án Jx1 mobile bằng Cocos của cùng chủ

| Đường dẫn | Nội dung |
|---|---|
| `Jx1mClientMobile\Classes\gamecore` | `KPlayer.cpp`, `KProtocolProcess.cpp`… bản mobile của lõi |
| `Jx1mClientMobile\Classes\gameui\KuiAutoPlay.cpp` | **Mẫu ném 2 pha**: `ThrowItem` + `JX1M_ThrowPhase2` (đã dùng cho `[VATPHAM 12/09 f]`) |
| `Jx1mClientMobile\Classes\vn\gamescence` | `KgameWorldVN.cpp` — vòng chạm, chọn NPC |
| `Jx1mClientMobile\Resources\ui` | ảnh rời của bản Cocos (ít) |
| `jx1m_phone\VoLamHaiNgoai_phone_r196.apk` | **apk bản chạy thật** — ảnh giao diện nằm trong `assets/ui/...`, đọc bằng `zipfile`, đừng giải nén ra đĩa (C: từng đầy) |
| `Headers\KProtocol.h` | cấu trúc gói, đối chiếu khi cần |

**Bẫy:** client USVOLAM băm tên tệp bằng `g_FileName2Id`, **không phải** `name2id` của JX1 → không lấy
thẳng ảnh trong pak của nó sang được; phải lấy PNG trong `assets/`.

### 2.2 `C:\Users\nguye\Downloads\NHACTAI` — kho ảnh và dữ liệu tham khảo

| Thư mục | Nội dung |
|---|---|
| `VNKU_ui\png\Spr\...` | **ảnh PNG đã tách** của bản VNKU, cây thư mục giống `spr\` của game |
| `VNKU_ui\spr\Spr\...` | ảnh `.spr` gốc tương ứng |
| `VNKU_ui\png\_khong_ro_ten\` | ảnh chưa rõ tên, đặt theo `uid<hex>_<so>` — **icon bông hoa Auto** nằm ở đây: `uid2DA73294_0284.png` 18x20 (trùng `uidE776E440_1757.png`) |
| `VNKU_ui_ini\ini\Ui\Ui3\` | **ini giao diện của VNKU** — bằng chứng VNKU dựng trên chính hệ cửa sổ JX1; `MinMapSmall.ini` có `[AutoBtn]` 75x75 3 khung, `[AutoStart]`, `[AutoSettings]` |
| `VNKU_effect` | ảnh hiệu ứng |
| `VNKU_extract`, `VNKU_pak` | pak gốc (`vnku_ui.pak` header 32 + index cuối tệp) |
| `GHM_auto\` | **bộ ảnh auto của bản GHM/USVOLAM**: `start_auto.png`, `config_auto.png` (đĩa đỏ viền vàng, bánh răng vàng), `Ui_auto.png` 700x550 (nền khung Thiết lập Auto) |
| `R69_khaosat`, `PAKX_DAC_TA_MA_HOA.md` | khảo sát + đặc tả mã hoá pak |

**Công cụ đọc kho:** `android/bo_cuc_vnku_mobile.py` (`doc_spr`, `ghi_spr_nhieu_khung`, `ghi_moi_noi`,
hằng `VNKU`). `ghi_moi_noi` ghi **cùng lúc** vào `android/du_lieu_ghi_de\` và `D:\jx1_android_data\`.

---

## 3. Cây dữ liệu và bộ tải

| Nơi | Vai trò |
|---|---|
| `D:\jx1_android_data` | **cây dữ liệu đang chạy** của máy ảo. LDPlayer gắn nó vào `/mnt/shared/Misc`, game `chdir` vào đó |
| `android\du_lieu_ghi_de\` | **lớp ghi đè chỉ-Android** trong git: ini giao diện, ảnh VNKU, bố cục mặc định, `config.ini`. `chuan_bi_du_lieu.ps1` chép đè sau cùng |
| `D:\jx1_android_data_dt_v4` | **bộ tải cho điện thoại** (pak phẳng `mobile_NN.pak` + tệp rời + `jx1mobile.apk` + `manifest.txt`) |
| `android\may_chu_tai_du_lieu.py` | máy chủ HTTP cổng **8765**, tự sinh `manifest.txt` và `apk.txt` **lúc khởi động** → đổi tệp xong phải **khởi động lại máy chủ** |
| `android\dong_goi_du_lieu_dien_thoai.py` | đóng gói; `du_lieu_ghi_de` được giữ **dạng tệp rời** trong gói nên đổi giao diện chỉ cần chép tệp + sinh lại manifest, không phải đóng gói lại 7 GB |

**Quan trọng:** `manifest.txt` **bỏ qua** `userdata\` và `apdata\` → điện thoại **không** tải bố cục
riêng từ máy chủ, mỗi máy giữ tệp của nó.

**Bẫy mới (phiên D3D11 báo):** `KPakFile::Open` với `m_nPakFileMode = 0` mở tệp **trên đĩa trước** rồi
mới tìm trong pak → tệp rời trong gói sẽ **che** bản nằm trong pak.

---

## 4. Quy trình dựng và đẩy

```
scratchpad\build_chuoi_sdl_wt.ps1  -From core|rep3|s3client|common   # chuỗi SDL x64
scratchpad\build_chuoi_x64_wt.ps1  -From engine|core|rep3|s3client   # chuỗi Release x64 (bản PC)
scratchpad\build_apk_<ky_tu>.ps1                                     # APK -> android\apk\jx1mobile-1209-tai-<ky tu>.apk
scratchpad\gan_ff.sh [giay]                                          # chờ cây mobile sạch rồi fast-forward mobile-0809
git push origin mobile-0809
```

- `JAVA_HOME` = `C:\Program Files\Eclipse Adoptium\jdk-17.0.19.10-hotspot`.
- Sau khi dựng: chép APK vào `D:\jx1_android_data_dt_v4\jx1mobile.apk`, xoá `manifest.txt`, **khởi động
  lại máy chủ 8765** thì điện thoại mới thấy bản mới (`apk.txt` sinh lại lúc đó).
- `versionCode` = `(yy-25)*1e8 + MMddHHmm`, sinh mỗi lần dựng.

### Chẩn đoán trên máy ảo

| Khoá config | Tác dụng |
|---|---|
| `[Ui] NhatKyBoCuc=2` | ghi `bocuc_thuc.txt` = **toạ độ thật** của mọi cửa sổ (đường duy nhất đáng tin) |
| `[Ui] SuaToaDo=1` | bật chế độ Sửa giao diện; thông báo hiện `[<rộng>x<cao>]` khung vẽ |
| `[Resolution] TheoManHinh=0` + `Width/Height` | **ép thẳng khung vẽ** — cách duy nhất thử được khung 2520 px của Fold 7 vì LDPlayer chặn bề ngang 2080 px |
| `[Cham] NhatKyGiu=1` | nhật ký chẩn đoán "giữ ngón lâu trên ô vật phẩm" ra logcat |

`g_DebugLog` ra **logcat** (`adb logcat -d | grep ...`), không ra tệp.

---

## 5. Việc đã làm trong phiên (theo thứ tự)

| Commit | Nội dung |
|---|---|
| `[VEVATPHAM 12/09]` … `[HANHTRANG 12/09 b]` | ảnh vật phẩm nằm chính giữa ô; trả hành trang về bản gốc |
| `[ICON3X 12/09]`, `[VATPHAM 12/09]`→`[d]` | icon đấu giá/thư to gấp 3; **dải nút thao tác vật phẩm** (Dùng/Rao/Ném/Đính/Tháo/Tách/Đóng) đặt dưới bảng chú giải có sẵn |
| `[KYNANG 12/09 O + ICON]`, `[TRON]` | ô kỹ năng **theo từng nhân vật** (trước dùng chung 1 tệp nên nhân vật mới gỡ không được); icon to bằng ô; cắt tròn |
| `[VATPHAM 12/09 e]` | bốn ô phím 1-4: ô đen vốn **vẽ sẵn trong ảnh nền** thanh dưới → cắt ra `o_phim_nen.spr`, mỗi ô tự vẽ nền |
| `[VATPHAM 12/09 f+g]` | **ném 2 pha** (mẫu USVOLAM); giữ ngón lâu trên ô vật phẩm = nhấc lên tay |
| `[DPG 12/09 1024]` | **khung vẽ rơi đúng 1024 → game nhảy sang bộ giao diện 1024x768** (51 chỗ so `SCREEN_WIDTH == 1024`) → mất ô gõ chat. Sửa: không bao giờ để rộng = 1024 |
| `[UITOADO 12/09 CHUDAT]` | hai bảng `DAT_TAY`/`TILE_MOBILE` từng **ghi đè bố cục chủ đặt** → nay chỉ là giá trị dự phòng |
| `[AUTO 12/09 HOA]` | icon Auto = **bông hoa đỏ** `uid2DA73294_0284.png` kho VNKU |
| `[OPHIM 12/09]` | ô phím 1-4: lấy món ra được, bỏ chú giải, thêm nút **Gắn phím**, bỏ nút Dùng ở món không dùng được |
| `[MOBILE 12/09]`→`[d]` | gộp `main` 4 lần: lớp ảnh cảnh client 2.0, phi phong, vật cảnh động, NPC trang trí, A* góc hẹp |
| `[UITOADO 12/09 KHUNG]` | thông báo Sửa giao diện hiện khung vẽ — chủ đọc ra **1440x616** |
| `[UITOADO 12/09 LE]`, `[LE b]`, **`[LE x]`** | vá lề trái **SAI**, đã **gỡ hẳn** — xem §6 |

---

## 6. Bẫy đã vấp — đọc trước khi sửa bố cục

1. **Không vá bố cục khi chưa có số đo từ máy thật của chủ.** Máy ảo chặn 2080 px, và máy ảo dùng tệp
   bố cục riêng khác máy chủ → "đo trên máy ảo thấy đúng" không chứng minh được gì. Ngày 12/09 tôi đoán
   4 lần, sai cả 4.
2. **Sửa vị trí phải làm MỘT LƯỢT sau khi cây cửa sổ dựng xong, cha trước con.** Bản vá `[LE]` đọc toạ
   độ tuyệt đối rồi đẩy ô trong lúc duyệt cây → cha chưa đặt xong → đẩy sai chiều.
3. **`sinh_bocuc_rong.py --chuan` đọc `D:\jx1_android_data\userdata\UiToaDo.ini`** (bố cục chủ đặt).
   Chép tệp mặc định **đã sinh** đè lên đó rồi chạy lại là **trôi bố cục** (đo được: `SpringGame`
   751,-1 neo phải-trên → 2,335 neo giữa-dưới; `KUiItem|Main` 575,40 → 640,12). Đã có chặn
   `[UITOADO 12/09 CHONGTROI]`, đừng gỡ.
4. **Tệp bố cục riêng của người chơi ĐÈ LÊN mặc định** → đổi tệp mặc định không tới được máy đã chơi.
5. **`GetObjectAt` của ô vật phẩm nhận toạ độ TUYỆT ĐỐI** (nó tự trừ `m_nAbsoluteLeft`), đừng gửi toạ
   độ tương đối.
6. **Chen `case` vào giữa hai `case` đang rơi tiếp nhau là cắt đứt đường rơi** — từng làm hỏng
   `WM_MOUSEHOVER` của ô vật phẩm.
7. **`KItem::m_CommonAttrib.szScript` luôn rỗng ở bản client** (chỉ có bên máy chủ) → không dùng để
   đoán món có dùng được không; lọc theo **loại món**.
8. **Ô phím 1-4 và ô kỹ năng có đường chạm riêng** (`[ANDROID 11/09 OSO]`: chạm = dùng, giữ lâu = nhấc
   lên tay). Đừng áp luật chạm chung cho chúng.
9. **Dải 25–45 điểm sát mép trên là vùng vuốt thanh trạng thái của Android** — nút đặt ở `Top = 0` bấm
   không ăn trên máy thật, máy ảo thì vẫn ăn.
10. **Kịch bản python có `\U`, `\d`** trong chuỗi đường dẫn Windows → viết ra tệp `.py` trong scratchpad
    rồi chạy, hoặc dùng `chr(92)`; đừng nhét thẳng vào heredoc.
11. **Đọc log trước khi kết luận.** Log máy chủ 8765 ghi rõ điện thoại đã tải gì lúc mấy giờ.

---

## 7. Số đo giữ lại

- Khung vẽ của chủ (**Galaxy Z Fold 7, màn ngoài**): **1440x616**.
- 30 dòng máy đang bán → **27 khung vẽ** khác nhau; quét hết: **0 cửa sổ ra ngoài khung**.
- Ở khung 1440x616, nằm **đúng `x = 0`**: `KSysMsgCentrePad|SysRoom` (+3 nút cuộn),
  `KUiPlayerBar|HideChat`; `KUiMsgCentrePad|Main` ở `x = 2`.
- `CDevGpu::Letterbox` phóng bằng `min` hai chiều rồi căn giữa → **không méo hình**.
- SDL **3.2.14**, đã có `SDL_GetWindowSafeArea`.
- `JX_ANDROID`: **172 chỗ / 59 tệp**.
- Tầng vẽ chỉ có shader **SPIR-V** → iOS cần bản **MSL**.
- Dữ liệu điện thoại: bảng `npc_res_kind_file_name.txt` chỉ có `critter008`, thư mục
  `spr\npcres\critter` **rỗng** → động vật trang trí chưa thể hiện trên mobile.
