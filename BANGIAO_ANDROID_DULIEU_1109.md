# BÀN GIAO — ANDROID: NGỰA, TÊN TỆP GBK, DUNG LƯỢNG DỮ LIỆU, BẢN CHO ĐIỆN THOẠI (11/09 đêm)

> Sau B2 của WAuto (`BANGIAO_WAUTO_MOBILE_B2_1109.md`), chủ giao 5 việc: icon Auto hai kiếm chéo của VNKU; ngựa không hiện hình
> trên mobile; bản đồ nhỏ lệch khi vào Tống Kim; phân tích giảm dung lượng dữ liệu; bản cho điện thoại cùng mạng.
> Bản PC không đổi: hai chỗ sửa mã đều rào `JX_ANDROID`, x64 4/4 + SDL 6/6 dựng lại 0 lỗi.

---

## 1. Icon Auto — xong

`Spr\UiNew\UiToolsControlBar\pk.spr` của kho VNKU (74×74, 3 khung hai kiếm chéo nền xanh lá / đỏ / vàng) → `auto_m.spr` 47×47:
khung 0 (xanh lá) = auto tắt, khung 1 (đỏ) = auto bật. Sinh bằng `android/anh_wauto_vnku.py` (`icon()`), ảnh đã chép vào
`D:\jx1_android_data`. Trong 596 ảnh vuông nhỏ của kho chỉ có `pk.spr` là hai kiếm chéo đúng nghĩa (ảnh rà: scratchpad `vnku_vuong_nho_*.png`).

## 2. Ngựa không hiện hình — gốc và sửa (`android/va_nguon_android_ngua1.py`)

`jx_rep3.log` ghi ảnh thiếu nhiều nhất: `\spr\npcres\woman\..\man\MA_HH_002_RD01.spr` ×8.878 (và `_HR01`, `_HM01`, `_HA01` — ảnh
ngựa các hướng). Nhân vật nữ xin ảnh ngựa qua đường dẫn tương đối `\woman\..\man\`. Trên PC, `g_GetFullPath` → `RemoveTwoPointPath`
(`Engine/KFilePath.cpp`) rút gọn `\..\` rồi mới băm id tìm trong pak (`spr.pak` có id `30c8b46d` = `\spr\npcres\man\ma_hh_002_rd01.spr`).
Bản Android dịch không có `WIN32` (chỉ `JX_POSIX`) nên hàm này chỉ tìm `/../` (nhánh Linux) → không rút gọn → id `33ef6bd0` không có
trong pak → ảnh null → ngựa tàng hình (ảnh chéo của nhân vật nữ cũng vậy). **Sửa**: thêm nhánh tìm `\..\` và `\.\` khi `JX_ANDROID`
(WIN32 và Linux server giữ nguyên). APK `android/apk/jx1mobile-1109-ngua1.apk`.

## 3. Tên tệp GBK bị hỏng khi sinh dữ liệu — 1.136 tệp không tìm thấy (`android/sua_ten_gbk_android.py`)

Trong lúc truy ngựa thấy `\spr\skill\<GBK>\mag_bz_bin1_<GBK>.spr` ×950 cũng null. Đối chiếu cây PC với `D:\jx1_android_data`:
`chuan_bi_du_lieu.ps1` hạ chữ thường bằng `.ToLowerInvariant()`; tên GBK khi Windows đọc kiểu cp1252 là các chữ Latin-1 hoa
(`Ö` `Ï` `É` `Í` `Ø`…) nên bị hạ thành `ö` `ï` `é`… → **byte trên đĩa đổi** (0xD6 → 0xF6). Game xin tệp bằng byte GBK gốc
(`JxPathPosix` đổi cp1252 → UTF-8 từ byte gốc) → không thấy. Đếm được **1.136 tên** bị đổi: spr 395 (ảnh vật phẩm, trang bị đồng
hành, hiệu ứng chiêu), maps2 474 (ảnh bản đồ nhỏ `<tên GBK>24.jpg`, `.wor`), script 123, settings 109 (`faction\门派设定.ini`,
`maps\great_night\*.txt`), ui 18 (`ui3\同伴背包.ini`, `录像操作界面.ini`…), maps 17.
**Đã sửa**: (a) `chuan_bi_du_lieu.ps1` chỉ hạ A-Z (hàm `HaAscii`); (b) `sua_ten_gbk_android.py` đổi tên lại dữ liệu đã sinh theo
cây PC (chạy 11/09 23:39: 1.136 tệp, kiểm lại 0 còn sót). Thư mục GBK không bị (đã đối chiếu byte).
Hệ quả có thể thấy ngay sau khi khởi động lại app: ảnh vật phẩm / hiệu ứng chiêu / bản đồ nhỏ của các bản đồ tên GBK (trong đó có
khu **Tống Kim**, `maps2\特殊用地\…`) hiện đủ như PC. **Nghi vấn "bản đồ nhỏ vào Tống Kim bị lệch"** rất có thể là hệ quả của mục này
(ảnh bản đồ nhỏ không tìm thấy nên vẽ sai) — cần xem lại ở trận Tống Kim tới; nếu vẫn lệch, chủ mô tả thêm (lệch ảnh, lệch chấm nhân
vật hay cửa sổ nhảy chỗ) vì mã bản đồ nhỏ (`UiMiniMap.cpp`, `ScenePlaceMapC.cpp`) không có nhánh riêng cho Android và ảnh chụp 23:27
trong trận thấy chấm nhân vật vẫn ở giữa ô bản đồ.

## 4. Dung lượng dữ liệu — số đo và cách giảm

`D:\jx1_android_data` = **10,7 GB**: `data` (pak) 7.878 MB / 42 tệp; `spr` rời 1.350 MB / 3.314 tệp; `maps2` 771 MB / 95.365 tệp;
`maps` 366 MB; gốc 244 MB (log!); `settings` 78 MB. Pak lớn: updatejx14 1.814 MB, updatejx15 1.521 MB, spr.pak 880 MB, updatejx16 818 MB,
sprvuhontieudao 372 MB, updatejx13 260 MB, resource 227 MB.

Phân tích pak theo đúng thứ tự tìm của `KPakList` (`package.ini` 0..39, id trùng thì pak đứng trước thắng; `ReverseTools/pak_vltk/pakdump.py`):
206.604 mục, **189.507 id duy nhất**; tổng tệp pak 7.582 MB, phần **còn được dùng 6.840 MB** → bỏ phần bị che chỉ tiết kiệm ~740 MB
(updatejx13 bị che 100 %, updatejx01/02 gần hết, slistcache…). `package.ini` còn kê 4 pak không tồn tại (sprvlngaothe2, settings, ui, script).

Muốn giảm mạnh phải theo hướng chủ nói — **"unpack ra chỉ lấy data cần"**:
1. Thêm nhật ký mở tệp trên Android (chỉ `JX_ANDROID`, trong `KFile::Open` + `KPakList::FindElemFile`): ghi mọi đường dẫn (tệp rời hoặc
   id trong pak) được xin trong phiên chơi. Chạy đủ kịch bản: đăng nhập, mỗi thành, Tống Kim, Dã Tẩu, Liên đấu, Công thành, các bản đồ
   luyện công, 10 môn phái (chiêu/hiệu ứng). Gộp danh sách → tập "cần".
2. Sinh dữ liệu điện thoại = tệp rời trong tập cần + **một pak mới** gồm các mục pak trong tập cần (giải nén rồi đóng lại bằng
   `pakwrite.py`, giữ id). Ước lượng: ảnh nhân vật/chiêu/NPC thực dùng thường 1,5–2,5 GB; cộng bản đồ đã đi → **3–4 GB** thay vì 10,7.
3. Bỏ ngay không cần nhật ký: log ở gốc (244 MB), pak bị che (740 MB), `sound/music` nếu bản thử không cần tiếng, bản đồ không có trong
   `MapList.ini` của máy chủ (`ReverseTools/pakscan.py` quét được).
Rủi ro của cách 1-2: tài nguyên chưa từng gặp trong phiên ghi (trang bị hiếm, boss lạ) sẽ thiếu → hiện "ảnh null" chứ không sập
(`Rep3AnhNullGhi` đếm và ghi log, có thể bổ sung sau). Việc này là một đợt riêng (~1 ngày làm + chạy phiên ghi); chủ duyệt thì làm.

## 5. Bản cho điện thoại cùng mạng

Không cần bản dựng riêng: APK debug chạy được trên điện thoại (Android 7+, arm64, Vulkan) — dùng **`android/apk/jx1mobile-1109-ngua1.apk`**.
Máy chủ lấy từ `settings\serverlist.ini` (đã là IP LAN 192.168.200.128/130/133) → điện thoại cùng mạng nối được. Các bước theo
`android/HUONG_DAN_TEST.md` §2-3: cài APK → mở một lần để Android tạo thư mục → chép dữ liệu:
```bat
adb install -r D:\GAMEDEVNEW_wt_mobile\android\apk\jx1mobile-1109-ngua1.apk
adb push D:\jx1_android_data\. /storage/emulated/0/Android/data/vn.jx1.mobile/files/
```
Cần ~11 GB trống và 30–60 phút chép qua dây (hoặc chép `D:\jx1_android_data` vào thẻ nhớ rồi dời vào đúng thư mục). Trước khi chép
nên xoá log ở gốc (`jx_*.log`, 244 MB). Sau khi mục 4 làm xong, gói điện thoại còn 3–4 GB.

## 6. Việc đang chờ
- Nghiệm thu ngựa + icon trên máy ảo (kịch bản `thu_ngua.sh` chạy sau Tống Kim 22:50): xem ngựa hiện, `jx_rep3.log` không còn
  `woman\..\man`, và số ảnh null giảm sau khi đổi tên 1.136 tệp.
- Gắn `1a04074c` + commit tên GBK vào `mobile-0809`.
- Bản đồ nhỏ Tống Kim: xem lại ở trận tới sau khi dữ liệu đã sửa.
