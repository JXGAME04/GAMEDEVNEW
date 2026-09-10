# BÀN GIAO — ANDROID: NGỰA, TÊN TỆP GBK, DUNG LƯỢNG DỮ LIỆU, BẢN CHO ĐIỆN THOẠI (11/09 đêm)

> Sau B2 của WAuto (`BANGIAO_WAUTO_MOBILE_B2_1109.md`), chủ giao 5 việc: icon Auto hai kiếm chéo của VNKU; ngựa không hiện hình
> trên mobile; bản đồ nhỏ lệch khi vào Tống Kim; phân tích giảm dung lượng dữ liệu; bản cho điện thoại cùng mạng.
> Bản PC không đổi: hai chỗ sửa mã đều rào `JX_ANDROID`, x64 4/4 + SDL 6/6 dựng lại 0 lỗi.

---

## 1. Icon Auto + nút nhặt — xong (`B2 h`, `B2 i`, `android/va_nguon_android_wauto8.py`)

Chủ gửi ảnh mẫu: vòng tròn hai kiếm chéo vàng, dưới có nhãn "Auto", nằm trong cột icon bên phải dưới "Trao đổi". Rà **596 ảnh vuông nhỏ
của kho VNKU, 23 ảnh `UiToolsControlBar`, APK gốc `VNKU-27-07.apk` (557 mục, 0 mục "auto") và dự án USVOLAM (57 ảnh, 0 "auto")**: không
có sẵn tệp đúng như mẫu (kho chỉ có `pk.spr` kiếm chéo không chữ, `bat_auto.spr` là nút dài "Bật Auto"). Nên ghép từ chính nút
`MinMapSmall\bat_auto.spr` của kho: hình tròn kiếm chéo (trái nút) + chữ "Auto" cắt từ "Bật Auto" → `auto_m.spr` 48×56, khung 0 xám =
auto tắt, khung 1 vàng = auto bật (`anh_wauto_vnku.py` `icon()`). Đặt ở `[WAuto]` (868,300) trong cột phải dưới "Chạy"; chạm = mở/đóng khung.
Đã bỏ hai nút lớn "Bật Auto" / "Thiết lập" thử giữa chừng (lớp `Player_WAutoBat` vẫn dịch, không đặt trong ini).

**Nút nhặt (bàn tay)** — chủ: "bấm vào tự chạy tới nhặt, ưu tiên hơn tự đánh": `UiToolsControlBar\nut_nhat.spr` (89×89, trắng / vàng) →
`nhat_m.spr` 48×48 tại `[NhatDo]` (868,362), lớp `Player_NhatDo` → `JxWAuto_NhatNgay(6000)`: 6 giây "đợt nhặt" — bên gửi WAuto phát nhịp
kể cả khi auto đang tắt, với cấu hình tạm `bFight=0, bOnPK=0` (không đánh), `bPickUp=1, bFollowPick=1` (chạy tới nhặt), tầm nhặt ≥ 800,
Tống Kim / Dã Tẩu nghỉ; gỡ khoá mục tiêu lúc bắt đầu; hết đợt mà auto vốn tắt thì gửi `PRT_TICKSTART(0)` im lặng để bộ não `ATYPE_CLEAR`.
Đo 00:03 (APK `wauto-b2i`, MD5 khớp): chạm bàn tay → `[WAUTO] NHAT NGAY 6000 ms`, `jx_auto.log` 6 dòng `[AUTO-PASS] fight=0 pick=1
fpick=1 pvis=800` đúng 6 giây rồi về cấu hình thật; icon Auto / khung mở đóng / Bật-Tắt trong khung vẫn đúng (ảnh `ld/nut_*.png`).
Chủ thử (00:10): "kích vào chưa tự chạy tới nhặt" → hai cổng trong `ATYPE_PICKUP` (CoreShell.cpp) chặn khi nhân vật **không ở thế
chiến đấu**: `if(!m_FightMode && !bCityPick) return 0` và cổng chạy-tới `PICK-FOLLOW-GATE` đòi `m_FightMode`. Sửa (`B2 i d/e`,
`va_nguon_android_wauto9.py`): đợt nhặt ép `bCityPick=1`; cổng chạy-tới nhận thêm `bCityPick` (rào `JX_ANDROID`, PC giữ dòng cũ).
Đo 00:22 (`wauto-b2ie`): ngoài thế chiến đấu vẫn có `PICK2-SCAN pickvision=800 vision=800` (quét 800 rồi chạy tới) — trong thành
không có đồ rơi nên `followobj=0`; chủ thử ở bãi quái có đồ rơi.

## 2. Ngựa không hiện hình — gốc và sửa (`android/va_nguon_android_ngua1.py`)

`jx_rep3.log` ghi ảnh thiếu nhiều nhất: `\spr\npcres\woman\..\man\MA_HH_002_RD01.spr` ×8.878 (và `_HR01`, `_HM01`, `_HA01` — ảnh
ngựa các hướng). Nhân vật nữ xin ảnh ngựa qua đường dẫn tương đối `\woman\..\man\`. Trên PC, `g_GetFullPath` → `RemoveTwoPointPath`
(`Engine/KFilePath.cpp`) rút gọn `\..\` rồi mới băm id tìm trong pak (`spr.pak` có id `30c8b46d` = `\spr\npcres\man\ma_hh_002_rd01.spr`).
Bản Android dịch không có `WIN32` (chỉ `JX_POSIX`) nên hàm này chỉ tìm `/../` (nhánh Linux) → không rút gọn → id `33ef6bd0` không có
trong pak → ảnh null → ngựa tàng hình (ảnh chéo của nhân vật nữ cũng vậy). **Sửa**: thêm nhánh tìm `\..\` và `\.\` khi `JX_ANDROID`
(WIN32 và Linux server giữ nguyên). APK `android/apk/jx1mobile-1109-ngua1.apk`. **Đo 23:55** (MD5 khớp, khởi động lại): sau 70 s
`jx_rep3.log` không còn dòng `woman\..\man` nào (trước ×8.878), còn lại chỉ `enemy141_stb.spr` khung 2/4 (quái thiếu khung, việc khác).
Chưa chụp được ngựa vì nhân vật "quá mệt mỏi, không thể tiếp tục lên ngựa" — chủ lên ngựa thử là thấy.

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

**Bảng bỏ được ngay** (`android/phan_tich_du_lieu.py`, đo 00:00 sau khi sửa tên GBK; kịch bản dời sang `D:\jx1_android_data_bo\` để lấy lại
được: `android/rut_gon_du_lieu.py`, `--thu` để xem trước):

| Mục | Gì | Bỏ được |
|---|---|---|
| A | 12 tệp `jx_*.log` ở gốc (212 MB) + 230 tệp sao lưu `.truoc_*`, `.moi`, `.bak`, `__tmp__` (59 MB: `goldequip.txt.truoc_*` ×5, `platinaequip.txt.truoc_*`…) | **271 MB** |
| B | 46 bản đồ trong `maps2\` không có trong `settings\maplist.ini` (1.006 bản đồ khai báo, 326 có thư mục ảnh): 58.432 tệp — ví dụ `Thanh Long Sơn` 31 MB, `qichengdazhan` 31 MB, `Trường Bạch Sơn` 21 MB… | **493 MB** |
| C | pak bị che 100 % theo thứ tự tìm: `updatejx01.pak` 89 MB, `updatejx02.pak` 86 MB, `updatejx13.pak` 260 MB (còn < 64 KB dùng) | **435 MB** |
| D | pak nằm trong `data\` nhưng **không có trong `package.ini`** (game không bao giờ mở): `sprgame.pak` 145 MB, `vlngaothe1.pak` 145 MB, `vltkcache.pak` 2,6 MB, `serverlistfree.pak`, `update05.pak`, `maps_vuhon_tieudao.pak.truoc_*` 3,3 MB | **296 MB** |
| | **Tổng bỏ ngay: ≈ 1.235 MB → còn ≈ 9.490 MB** | |
| E | Đóng lại 12 pak bị che một phần (updatejx16 58 MB, updatejx08 28 MB, spr.pak 21 MB…) chỉ giữ mục còn thắng (`pakwrite.py`) | thêm **457 MB → ≈ 9.035 MB** |
| — | 4 dòng `package.ini` trỏ tới pak không tồn tại (`sprvlngaothe2`, `settings`, `ui`, `script`): xoá dòng cho sạch (không đổi dung lượng) | |

**Phải giữ** (không bỏ được bằng cách tĩnh): `spr\npcres` 1.141 MB (ảnh nhân vật / NPC, ghi đè pak), `spr\Ui3` 81 MB, `maps\*.fp` 350 MB
(1.006 bản đồ khai báo đều có), `maps2` 326 bản đồ còn lại 279 MB, `settings` 78 MB, các pak còn lại ≈ 6.400 MB (updatejx14/15/16, spr,
resource, update01/03, sprvuhontieudao…). Trong số này cái nào thực sự **được chơi** chỉ biết bằng nhật ký mở tệp (bước dưới).

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

## 5. Bản cho điện thoại cùng mạng — nay tải qua màn hình tải (mục 7), không chép tay nữa

APK debug chạy được trên điện thoại (Android 7+, arm64, Vulkan): **`android/apk/jx1mobile-1209-tai.apk`** (MD5 `0d50f50d…`). Máy chủ game
lấy từ `settings\serverlist.ini` (IP LAN 192.168.200.128/130/133). Các bước:
1. Trên PC: `python android\may_chu_tai_du_lieu.py` (phục vụ `D:\jx1_android_data_dt`, cổng 8765; mở tường lửa:
   `netsh advfirewall firewall add rule name=jx1tai dir=in action=allow protocol=TCP localport=8765`).
2. Cài APK lên điện thoại (`adb install -r …` hoặc chép tệp .apk sang). Mở game → màn hình tải tự tải ~9,4 GB từ
   `http://10.0.0.140:8765/` (IP đặt ở `res/values/strings.xml` `may_chu_tai`; đổi không cần build lại bằng tệp
   `/storage/emulated/0/Android/data/vn.jx1.mobile/files/tai_du_lieu.txt` dòng 1 = URL). Wi-Fi 5 GHz thường 30–60 MB/s → 3–5 phút;
   tắt giữa chừng mở lại tải tiếp (`.part` + HTTP Range), lần sau chỉ tải tệp đổi.
Cần ~10 GB trống. Sau khi rút gọn theo nhật ký (mục 7) gói còn 2–4 GB.

## 6. Việc đang chờ
- Nghiệm thu ngựa + icon trên máy ảo: xem ngựa hiện, `jx_rep3.log` không còn `woman\..\man`, số ảnh null giảm sau khi đổi tên 1.136 tệp.
- Bản đồ nhỏ Tống Kim: xem lại ở trận tới sau khi dữ liệu đã sửa.
- Chủ chơi đủ kịch bản với APK 1209 để gom `jx_tep_dung.log` rồi đóng gói `--chi-dung` (mục 7).

## 7. Màn hình tải dữ liệu (ống tiến độ như VNKU) + máy chủ tải + gói điện thoại — xong (`12/09 TAI`, `12/09 TEPDUNG`)

**Chủ yêu cầu:** "rút gọn tối đa", "phần đăng nhập có ống load dữ liệu như VNKU", "người chơi tải về nhanh nhất không tốn thời gian".

**APK mở bằng `TaiDuLieuActivity`** (Java, `android/gradle-project/app/src/main/java/vn/jx1/mobile/TaiDuLieuActivity.java`; layout
`res/layout/tai_du_lieu.xml`; ảnh VNKU `res/drawable-nodpi/nen_cap_nhat.png` = khung `UiUpdateNow\main.png` che chữ gốc bằng hoa văn,
`nut_cap_nhat.png` = `btn_update_f00.png` "Cập nhật ngay", `nut_trong.png` cho nút "Thử lại"; ống vàng `res/drawable/thanh_tai.xml`).
`AndroidManifest.xml`: LAUNCHER chuyển sang activity này, `JxActivity` giữ nguyên (singleInstance) và được mở sau khi tải xong. Luồng:
1. Có `config.ini` ở thư mục **ngoài** app (máy ảo `/mnt/shared/Misc`, `jx_data_dir.txt`…, cùng thứ tự `JxAndroidMain.cpp`) → vào game
   ngay, không tải gì. Tệp `tai_du_lieu.txt` dòng 2 = `ep` → bỏ qua dữ liệu ngoài, ép tải vào thư mục app (để thử trên máy ảo).
2. Tải `manifest.txt` (mỗi dòng `cỡ \t md5 \t đường dẫn`), so với `da_tai.txt` (md5 đã tải) + cỡ tệp trên máy → **chỉ tải tệp mới/đổi**.
3. Tải **4 luồng song song**, tệp to trước, ghi `.part`, **HTTP Range tải tiếp** khi mở lại; ống tiến độ + MB/s + còn bao lâu; nút
   "Cập nhật ngay" tự bấm sau 2 s. Xong: xoá pak trong `data/` không còn trong manifest (gói đổi tên / rút gọn) rồi mở `JxActivity`.
4. Không nối được máy chủ: đã có dữ liệu → vào game luôn; chưa có → báo lỗi + nút Thử lại.

**Máy chủ tải** `android/may_chu_tai_du_lieu.py [--thu-muc D:\jx1_android_data_dt] [--cong 8765] [--chi-manifest]`: sinh `manifest.txt`
(md5 đệm trong `manifest_cache.txt` theo cỡ + mtime → chạy lại không băm lại 9 GB), phục vụ HTTP đa luồng có Range; bỏ qua `userdata\`,
`apdata\`, `*.log`, `*.part`. Chạy nền được (`Start-Process python …`).

**Gói điện thoại** `android/dong_goi_du_lieu_dien_thoai.py [--nguon D:\jx1_android_data] [--dich D:\jx1_android_data_dt] [--co-goi 512]
[--chi-dung log…]` (dùng `android/pak_ghi.py`, `ReverseTools/pak_vltk/pakdump.py`): gom **tệp rời + 33 pak cũ thành một danh sách phẳng
đúng thứ tự tìm của game** (rời thắng pak, pak trước thắng pak sau) → cùng id chỉ giữ bản đầu (bỏ mục bị che, không mất gì) → ghi lại
thành `data\mobile_NN.pak` ≤ 512 MB (mục pak cũ chép nguyên byte, giữ nén UCL; `XPackFile` đọc như cũ). Giữ rời vì game mở bằng `fopen`
không qua pak: `config.ini`, `package.ini` (mới), `settings\serverlist.ini`, `settings\datau_toado.txt` (Dã Tẩu), `ui\uitoado_macdinh.ini`,
`userdata\`, `apdata\`. Id tính từ byte GBK của tên (`ten_sang_l1`: tên trên đĩa là cp1252 của byte GBK; `name2id` gốc chỉ đúng với Latin-1).
Kết quả: 228.944 mục / 8.413 MB (bỏ mục bị che: 9.432 → **8.417 MB**, 17 gói `mobile_01..17.pak` + 5 tệp rời), ghi 211 s; chạy lại sau khi sửa lớp ghi đè (`chuan_bi_du_lieu.ps1`) rồi máy chủ tự sinh manifest mới.

**Đo trên máy ảo LDPlayer** (mạng nội bộ PC, APK 1209 với `ep`): 37 tệp / 9.431 MB tải xong **45 s (~215 MB/s)**, game mở từ thư mục app,
vào thẳng menu chính; `anh_null` giống bản thường (4 nút menu Trung Quốc `登入界面\…` vốn đã null từ trước). Bộ rút gọn theo nhật ký
menu (`--chi-dung`, 440 id → **15 MB**) cũng tải + vào menu được, pak cũ trong `data/` bị xoá đúng.

**"Rút gọn tối đa" — bước còn lại cần chủ:** APK 1209 đã ghi nhật ký tệp dùng `jx_tep_dung.log` ở thư mục dữ liệu (`[Client] TepDung=0`
để tắt): chủ chơi đủ kịch bản (đăng nhập, mỗi thành, Tống Kim, Dã Tẩu, luyện công, 10 phái, Bảo vật, chợ…) trên máy ảo
(`D:\jx1_android_data\jx_tep_dung.log`) và/hoặc điện thoại (thư mục app) → gộp log nhiều người/nhiều ngày →
`python android\dong_goi_du_lieu_dien_thoai.py --chi-dung D:\jx1_android_data\jx_tep_dung.log <log khác…>` → máy chủ sinh manifest mới →
điện thoại chỉ tải phần đổi. Thiếu mục → ảnh trống (`anh_null`), không sập; nếu **thêm** mục sau này nên đóng thành pak mới đặt **đầu**
`package.ini` thay vì đóng gói lại toàn bộ (đóng gói lại làm md5 của nhiều gói đổi → điện thoại tải lại nhiều).

## 8. Nút kỹ năng hiện ở menu / đăng nhập — chủ: "các nút kỹ năng bị ra ngoài phần vào game" (`12/09 KYNANG AN`)

`UiShell.cpp` vẽ `JxKyNang_Ve()` + `JxCan_Ve()` (bảng nút kỹ năng, cần điều khiển, vòng chọn, mũi tên hướng đi, icon NPC) mỗi khung
không xét trạng thái → ở menu chính (ảnh 4:3 chỉ phủ 800 px) các nút nằm trên dải đen bên phải. Sửa trong
`Sources/S3Client/Platform/JxCanDieuKhien.cpp` (tệp chỉ Android, ngoài vcxproj; `android/va_nguon_android_kynang_an1.py`):
`KyNang_TrongGame()` = `KUiToolsControlBar::GetSelf() != NULL` (thanh công cụ mở trong `UiStartGame`, huỷ khi rời thế giới) — chưa vào
thế giới thì không vẽ và không nhận chạm (`JxCan_TrongVung`, `JxKyNang_TrungNut`, `JxKyNang_ChamBangChon` trả "không"). APK
`android/apk/jx1mobile-1209-tai-b.apk`. Chủ kiểm: menu / đăng nhập / chọn nhân vật không còn nút; vào thế giới nút và cần hiện lại như cũ.

**Lưu ý máy ảo:** thư mục app (`/storage/emulated/0/Android/data/vn.jx1.mobile/files/`) được `JxAndroidMain.cpp` ưu tiên TRƯỚC
`/mnt/shared/Misc` — thử tải trên máy ảo xong phải xoá `data/`, `config.ini`, `package.ini`, `settings/`, `da_tai.txt`, `tai_du_lieu.txt`
trong đó, nếu không máy ảo chạy bằng gói cũ và lớp ghi đè `android\du_lieu_ghi_de` hết tác dụng (đã dọn sau khi đo).
