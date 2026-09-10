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
1. Có `config.ini` ở thư mục **ngoài** app (máy ảo `/mnt/shared/Misc`, `jx_data_dir.txt`…, cùng thứ tự `JxAndroidMain.cpp`) → mở
   `JxActivity` ngay trong `onCreate`, **không hiện gì** (chủ 01:00: "giữ lại các bước đăng nhập như trước" — menu, đăng nhập, chọn
   nhân vật của game giữ nguyên; màn hình này chỉ hiện khi phải tải/cập nhật). Tệp `tai_du_lieu.txt` dòng 2 = `ep` → ép tải (để thử).
2. Tải `manifest.txt` (mỗi dòng `cỡ \t md5 \t đường dẫn`), so với `da_tai.txt` (md5 đã tải) + cỡ tệp trên máy → **chỉ tải tệp mới/đổi**;
   đủ rồi thì vào game ngay (đã có dữ liệu: chờ máy chủ tối đa 3 s, không có mạng cũng vào game).
3. Có tệp mới → **tự tải ngay** (chủ: "có bản cập nhật mới sẽ tự cập nhật"), không chờ bấm nút: 4 luồng song song, tệp to trước, ghi
   `.part`, **HTTP Range tải tiếp** khi mở lại; ống tiến độ + MB/s + còn bao lâu. Xong: xoá pak trong `data/` không còn trong manifest
   (gói đổi tên / rút gọn) rồi mở `JxActivity`. Nút "Thử lại" chỉ hiện khi lỗi.
4. Lần đầu chưa có dữ liệu mà không nối được máy chủ → báo lỗi + nút Thử lại. APK bản này: `android/apk/jx1mobile-1209-tai-c.apk`
   (chưa cài lên máy ảo lúc 01:10 vì chủ đang chơi; `tai-b` đang cài chỉ khác ở chỗ hiện chữ 0,3 s và chờ nút 2 s).

**Điện thoại của chủ (01:15, 10.0.0.127, Wi‑Fi):** 8.417 MB tải xong **2 phút 42 giây (~52 MB/s)**, 24 yêu cầu, 0 lỗi máy chủ; 01:18:40
mở lại kiểm tra manifest và tải lại đúng 1 gói `mobile_16.pak` (chắc tệp đó lỗi ở lượt đầu → "Thử lại" chỉ tải phần thiếu). Chủ hỏi
"tải 5 phút có cách nào nhanh hơn, tốn ít data hơn": đường mạng đã ~52 MB/s, chỉ còn cách **bớt byte**:

| Cách | Còn bao nhiêu | Trạng thái |
|---|---|---|
| Nén truyền (gzip) cả gói | 85 % (6,4 GB từ pak cũ đã nén UCL sẵn, chỉ 2 GB tệp rời nén được ~47–63 %) | không đáng làm riêng |
| **Nén tệp rời ngay trong pak** bằng UCL (`--nen-roi 7`, `android/ucl_nen.c` dịch từ `Engine/Src/ucl` + tự giải nén kiểm từng mục, engine đọc như pak cũ) | đo 01:50: tệp rời 2.024 → **985 MB** (43.493/43.959 tệp nén được), cả gói 8.417 → **7.379 MB, 15 gói**, 348 s; kiểm giải nén Python 240 mục ngẫu nhiên (40/gói có UCL) đúng | `D:\jx1_android_data_dt_nen`; máy chủ chuyển sang phục vụ bộ này sau khi chủ cài `tai-f` (APK xoá gói cũ TRƯỚC khi tải để đủ chỗ) |
| Rút gọn theo nhật ký tệp dùng (`--chi-dung`) | **≈ 1,8–2,5 GB** | chờ chủ chơi đủ kịch bản |
| "Tải bổ sung khi thiếu" (gói đầu 300–500 MB, thiếu gì tải lúc chơi) | ~20 s vào game, tổng data = phần thực dùng | đợt sau, cần duyệt |
| Chặn tải qua 4G: mạng tính phí thì báo và chờ Wi‑Fi (nút "Tải bằng 4G" nếu vẫn muốn) | không tốn data di động | APK `jx1mobile-1209-tai-d.apk` |

**Số đo để ước lượng rút gọn** (`--chi-dem --chi-dung`, 01:05): chủ chơi ~1 giờ trong thành Tương Dương với APK 1209 → nhật ký 1.155 id
→ **70 MB** (bản đồ + ảnh bản đồ 30, nhân vật/NPC quanh đó 12, bảng vật phẩm/npcres 12, nhạc 5, kỹ năng 1…). Cả gói 8.417 MB gồm: ảnh
nhân vật/NPC/quái ≈ 6.000 MB (`spr\npcres` rời 1.141 + updatejx14/15/16 ≈ 4.150 + spr.pak 880), bản đồ ≈ 1.000 MB cho 1.006 bản đồ
(`maps` 350, `maps2` 300, maps.pak 80…), UI/vật phẩm/kỹ năng/âm thanh ≈ 700 MB. Ước phần **thực dùng của cả máy chủ sau vài tuần**:
bản đồ ~100/1.006 (100–150 MB), nhân vật thường gặp 800–1.200 MB, quái/NPC 300–500, kỹ năng 200–300, UI/bảng/nhạc ~300 → **≈ 1,8–2,5 GB**
(giảm 70–80 %). Chính xác đến đâu tuỳ nhật ký thu được; muốn cắt mạnh hơn (gói đầu 300–500 MB) cần thêm "tải bổ sung khi thiếu"
(game xin tệp không có → tải từ máy chủ lúc chơi, bước sau).

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

## 9. Điện thoại màn rộng "không ra full màn" + tọa độ UI chủ chỉnh chưa vào gói (`12/09 MANHINH`)

Ảnh chủ gửi 01:30 (điện thoại ~21:9): thế giới vẽ kín màn nhưng cột icon phải, bản đồ nhỏ, cụm nút kỹ năng, thanh chat nằm ở vị trí
của máy ảo 1040×604; dải bên phải trống. Gốc: `KSdlApp.cpp` chọn khung vẽ theo màn hình (`ChieuCaoMucTieu=640` → điện thoại
2400×1080 ⇒ hệ số 1,75 ⇒ khung vẽ ~1371×617, máy ảo 1040×604), còn bố cục `UiToaDo` (`ui\uitoado_macdinh.ini` + `userdata\UiToaDo.ini`,
kể cả `KyNang0..8`, `KyNangGan` của cụm kỹ năng) là **toạ độ tuyệt đối** theo 1040×604.
- Sửa `Sources/S3Client/Ui/Elem/UiToaDo.cpp` (chỉ `JX_ANDROID`, `android/va_nguon_android_uitoado_manhinh1.py`): tệp bố cục có dòng
  `ManHinh=W,H` trong `[Pos]` (khung vẽ lúc lưu; game ghi khi lưu; thiếu = 1040×604). Lúc nạp, ô có X ≥ 70 % rộng thiết kế được cộng
  thêm `SCREEN_WIDTH − W` (neo mép phải), Y ≥ 70 % cao cộng `SCREEN_HEIGHT − H` (neo mép dưới); còn lại giữ nguyên (thanh chat, hộp
  thoại giữa, cột trái). Tắt bằng `config.ini [Ui] NeoTheoMep=0`. Máy ảo 1040×604 không đổi gì.
- Chủ: "chưa up toạ độ UI mới ở bản PC lên": bố cục chủ chỉnh trên máy ảo nằm ở `userdata\UiToaDo.ini` (09/09 23:58), không nằm trong
  gói (userdata là dữ liệu người chơi). Đã lấy nguyên tệp đó làm **mặc định của game**: `android/du_lieu_ghi_de/ui/uitoado_macdinh.ini`
  (+ `D:\jx1_android_data\ui\`), thêm `ManHinh=1040,604` và `KUiToolsControlBar|NhatDo=868,362`. Đóng gói lại là điện thoại tải 2 KB.
- Chưa thử được bố cục màn rộng trên máy ảo (LDPlayer 1040×604 nên phép neo không tác dụng; đổi độ phân giải máy ảo là của chủ);
  chủ thử trên điện thoại với APK `jx1mobile-1209-tai-e.apk`: cột icon phải, bản đồ nhỏ, cụm kỹ năng phải sát mép phải.

**Chủ 02:00 (sau `tai-f`): "UI chưa đi theo kích thước màn hình, phím 1234 / icon phía trên vẫn chỗ cũ, ô hành trang và icon quá nhỏ,
UI bị bóp nhỏ."** Phép neo 70 % chỉ đẩy cột phải; hàng icon trên (`KUiPlayerBar|Status/Items/Skills/Faction/Team/Friend/Options`,
`KUiToolsControlBar|Sit`, ini `Top=47`) và thanh chat (`KUiPlayerBar|Main` vẽ `khung_chat_mobile.spr`) không có mục trong bố cục nên
đứng yên. Làm **bố cục mặc định riêng cho màn rộng** (`12/09 RONG`): `android/sinh_bocuc_rong.py` sinh
`ui\uitoado_macdinh_rong.ini` (ManHinh=1371,617) từ bố cục máy ảo: nhóm phải +331 sát mép (bản đồ nhỏ, Bảo Vật, cột icon, cụm kỹ
năng lùi 40 px, Rec, bảng nhiệm vụ), `KUiPlayerBar|Main` +165 → thanh chat + ô phím tắt vào giữa, hàng icon trên **phóng 1,3 lần, giãn
80 px** từ x=495 (8 icon tới 1116, cạnh Bảo Vật 1142), cột icon phải **phóng 1,3 lần, giãn 65 px** (NhatDo 422, đáy 496 < 617), PK 1,1;
hộp thoại giữa +165; HP/MP, chat trái giữ. `UiToaDo.cpp` chọn tệp này khi `SCREEN_WIDTH*10 >= SCREEN_HEIGHT*19` (có tệp), sau đó
`userdata\UiToaDo.ini` đè lên như cũ. Máy ảo (1,72) không đổi. APK `jx1mobile-1209-tai-g.apk`.
**Vì sao "bị bóp nhỏ"**: máy ảo 1040×604 hiện 1:1 trên màn PC (icon 47 px ≈ 12 mm), điện thoại 2400×1080 vẽ khung 1371×617 phóng
1,75 (47 px ≈ 5,6 mm). Muốn to hơn phải giảm số dòng lô-gic (`[Resolution] ChieuCaoMucTieu`, 640 → 480 = phóng 2,25, to thêm 29 %)
nhưng bố cục phải (bản đồ nhỏ 180 + cột 5 icon 250 + cụm kỹ năng 160 = 590 px) không còn chỗ dọc và hộp thoại PC cao 450–550 px tràn
màn → chỉ làm được khi bố trí lại cột icon (hàng ngang dưới bản đồ nhỏ) và thu hộp thoại; **ô hành trang** to hơn cần ảnh + ini
mới (kho VNKU có `UiItem`), là một đợt riêng như B2 của WAuto. Phóng từng icon bằng TiLe (đã làm) là cách an toàn trong khung hiện tại.

## 10. Bố cục theo NEO cho mọi cỡ điện thoại (`12/09 NEO`) — chủ 02:20: "điện thoại nhiều kích cỡ, chỉnh từng cỡ không hợp lý"

Công nghệ các engine mobile (Cocos, Unity) dùng: **độ phân giải thiết kế + neo (anchor) từng ô** trái/giữa/phải, trên/giữa/dưới;
màn nào cũng chỉ dịch ô theo neo, không chỉnh từng cỡ. Đưa vào `UiToaDo.cpp` (`android/va_nguon_android_uitoado_neo3.py`, thay khối
MANHINH, chỉ `JX_ANDROID`):
- Dòng bố cục có 6 trường `Left,Top,TiLe,Co,NeoX,NeoY` (neo 0/1/2; thiếu → tự suy: < 35 % = trái/trên, ≥ 65 % = phải/dưới, giữa).
  Bản PC đọc 4 trường đầu như cũ (trường 5-6 bị bỏ qua).
- Nạp tệp: `Left += (SW − W) × (neo − neo cha) / 2` với `W,H` = `ManHinh` của tệp; **con của cửa sổ cha toàn màn** (`<lớp>|Main` có trong
  bảng) dịch theo neo của nó TRỪ neo của cha (toạ độ con tương đối cha). Bảng giữ toạ độ khung vẽ thật; ghi tệp = 6 trường +
  `ManHinh=SW,SH` → tệp người chơi trên điện thoại tự nhất quán, kéo thả trong chế độ sửa vẫn giữ neo.
- Hai tệp mặc định (cả hai đã có neo, `android/sinh_bocuc_rong.py`): `uitoado_macdinh.ini` (thiết kế 1040×604, `--chuan` chỉ gán neo,
  dùng khi tỉ lệ < 1,9: máy ảo, 16:9) và `uitoado_macdinh_rong.ini` (1371×617, tỉ lệ ≥ 1,9: 19,5:9 … 21:9; icon 1,3 lần, giãn, ô phím
  tắt 1-4 `KUiPlayerBar|Item_0..3` = khối 2×2 bên trái cột icon phải, phóng 1,25). Điện thoại 1337×617 hay 1440×617 tự dịch theo neo.
- Ô phím 1-4 ("UI đặt item dùng nhanh"): con của `KUiPlayerBar` (ini `Item_0..3` 932/972×205/245, 36 px) — trước không có mục nên đứng
  yên; nay có mục + neo phải. APK `jx1mobile-1209-tai-h.apk`.

## 11. Làm lại hành trang cho mobile (`HANHTRANG 12/09`) — chủ 02:20: "làm lại hành trang, ô to hơn"

Hành trang gốc (`ui\ui3\uiitem.ini`): cửa sổ 214×454, `[ItemBox]` 168×280 lưới 6×10 → ô **28 px** (điện thoại 1,75× ≈ 49 px vật lý
≈ 2,9 mm, không chạm nổi). Lưới là `KWndObjContainer` (ô = Width/HUnits), ảnh vật phẩm vẽ qua `CoreShell::DrawGameObj(x,y,w,h)` →
`KItem::PaintItem` ở cỡ gốc 26 px (chỉ căn giữa, không phóng).
- `android/anh_hanhtrang_vnku.py`: khung mới **316×592, ô 44 px** (`ItemBox` 264×440, 6×10, viền 2 → 77 px vật lý ≈ 4,8 mm, to
  gấp 1,6), tiêu đề + hoa văn + hàng tiền từ kho VNKU `UiItem\main.png`, lưới vẽ lại; nút 96×35 hai khung (thường/bấm) từ VNKU:
  Lời rao = `quang_ba`, Định giá = `btn_tham_dinh`, Rao bán = `btn_trung_bay`, Gửi tiền, Trang bị, Đóng. Ghi vào lớp ghi đè
  `android/du_lieu_ghi_de/spr/ui3/uiitem/*.spr` + `ui/ui3/uiitem.ini` (cùng tên mục/lớp `KUiItem`, không đổi C++ giao diện).
- Icon phóng theo ô (`android/va_nguon_android_hanhtrang1.py`, Core, chỉ `JX_ANDROID`): `CoreDrawGameObj` ô lớn hơn ảnh gốc (và không ở
  chế độ thu nhỏ) → đặt cờ `g_nJxVeVatPhamW/H`, `KItem::PaintItem` vẽ kéo `RU_T_IMAGE_STRETCH` theo ô rồi xoá cờ. Túi đồ/cửa hàng/rương
  ô 26–28 px không đổi (Width − viền < 26).
- Đã đưa lên máy chủ tải dạng tệp rời (tệp rời thắng pak): điện thoại tải ~250 KB. Máy ảo (`D:\jx1_android_data`) cũng có → mở lại
  game là thấy; icon chỉ phóng khi cài APK `jx1mobile-1209-tai-i`. Vị trí cửa sổ: ini `Left=400,Top=10` (neo giữa trên màn rộng).
- Chưa làm: ô phím 1-4 kiểu VNKU (`o_phim1..4`), rương/cửa hàng/trang bị cùng cỡ ô (làm tiếp cùng cách nếu chủ ưng hành trang).

## 12. Phần đăng nhập trên điện thoại (`DANGNHAP 12/09`) — chủ 03:10

`android/va_nguon_android_dangnhap1.py` (rào `JX_ANDROID`, bản PC không đổi):
- **Vừa màn hình mọi máy**: nền menu/đăng nhập là `KUiLoginBackGround` (ảnh 800×600 `Init_Login.jpg`, trước chỉ chiếm 800 px bên trái).
  `PaintWindow` mới vẽ hai lớp: bản kéo toàn màn làm nền tối + bản thật vừa chiều cao, căn giữa (không méo, không cắt). Các bảng
  `KUiInit|Main`, `KUiLogin|Main`, `KUiConnectInfo|Main`, `KUiSelNativePlace|Main` (800×600) và `KUiSelServer|Main` (281×421) neo
  GIỮA trong cả hai tệp bố cục (`sinh_bocuc_rong.py`, `GIUA_TRUOC_GAME`) → máy ảo 1040 cũng vào giữa (x=120), màn rộng x=285.
- **Bàn phím hiện ngay khi sang ô mật khẩu**: IME Android tự đóng sau phím Enter dù ô kế đã nhận tiêu điểm →
  `JxSdl_BanPhimAo(bBat, nMatKhau)` hẹn 0,3 s, `JxSdl_BanPhimNhip()` trong vòng lặp đóng rồi mở lại bàn phím nếu vẫn có ô nhập.
- **Không tự viết hoa chữ đầu**: `SDL_StartTextInputWithProperties` với `SDL_CAPITALIZE_NONE`, không tự sửa chữ, ô mật khẩu kiểu
  `TEXT_PASSWORD_HIDDEN` (KWndEdit truyền cờ `WNDEDIT_ES_MASK_CHARACTER`). Phòng hờ: `KUiLogin::GetInputInfo` hạ chữ đầu tài khoản
  xuống chữ thường trước khi gửi (quy chế game). APK `jx1mobile-1209-tai-j.apk`. Chưa thử được trên máy ảo (chủ đang trong game);
  chủ kiểm trên điện thoại: menu và bảng đăng nhập ở giữa, nền kín màn, gõ tài khoản → Enter → bàn phím còn/hiện ngay ở ô mật khẩu,
  chữ đầu không tự hoa.
- **Chủ (tai-k): "bấm vào ô tài khoản bàn phím hiện rồi tắt đi, không nhập được"** → lỗi do chính phép tắt+mở lại 0,3 s ở trên: trên
  Android `SDL_StopTextInput` rồi `SDL_StartTextInput` liền nhau làm bàn phím mất luôn (cũng là gốc của lỗi cũ "sang ô mật khẩu không
  hiện bàn phím": KILL_FOCUS tắt, SET_FOCUS mở ngay sau). Sửa `DANGNHAP 12/09 b` (`va_nguon_android_dangnhap2.py`): KILL_FOCUS chỉ
  **hẹn tắt sau 0,2 s**, SET_FOCUS đến trước thì huỷ hẹn (bàn phím giữ nguyên, chỉ đổi kiểu ô nếu khác); không còn tắt+mở lại.
  APK `jx1mobile-1209-tai-l.apk` (tự cập nhật từ tai-k).

## 13. Tự tải và cài APK mới trong game (`CAPNHAT 12/09`) — chủ 03:40: "tải APK mới mà không cần up lên Drive"

- Cách nhanh không cần gì thêm: điện thoại cùng Wi‑Fi mở `http://10.0.0.140:8765/jx1mobile.apk` (máy chủ tải phục vụ APK ở gốc).
- Tự cập nhật (như VNKU): `build.gradle` sinh `versionCode = (yy−25)×10⁸ + MMddHHmm` mỗi lần dựng (tăng dần), `versionName 1.MMddHHmm`;
  máy chủ tải lúc khởi động đọc `versionCode` của `jx1mobile.apk` bằng `aapt` (SDK build-tools) → `apk.txt` = `<versionCode> <md5> <cỡ> jx1mobile.apk`.
  `TaiDuLieuActivity.kiemApk()` (chỉ khi dữ liệu nằm trong thư mục app, tức điện thoại): mã trên máy chủ lớn hơn bản đang cài → tự tải
  APK về `files/capnhat/jx1mobile.apk` (ống tiến độ) → mở trình cài đặt qua `content://vn.jx1.mobile.tep/jx1mobile.apk`
  (`JxTepProvider`, ContentProvider tối giản, không cần androidx; quyền `REQUEST_INSTALL_PACKAGES`, Android 8+ hỏi cho phép "cài ứng
  dụng không rõ nguồn" một lần). Huỷ cài → nút "Vào game (bản cũ)". Máy ảo (dữ liệu ngoài) không kiểm APK (cài bằng adb).
- Quy trình phát bản mới cho chủ: dựng APK → chép thành `D:\jx1_android_data_dt_nen\jx1mobile.apk` → khởi động lại
  `may_chu_tai_du_lieu.py` (sinh `apk.txt` + manifest) → điện thoại mở game là tự tải + hỏi cài. APK đầu tiên có cơ chế này:
  `jx1mobile-1209-tai-k.apk` (từ bản này trở đi không cần gửi tệp nữa).

**Lưu ý máy ảo:** thư mục app (`/storage/emulated/0/Android/data/vn.jx1.mobile/files/`) được `JxAndroidMain.cpp` ưu tiên TRƯỚC
`/mnt/shared/Misc` — thử tải trên máy ảo xong phải xoá `data/`, `config.ini`, `package.ini`, `settings/`, `da_tai.txt`, `tai_du_lieu.txt`
trong đó, nếu không máy ảo chạy bằng gói cũ và lớp ghi đè `android\du_lieu_ghi_de` hết tác dụng (đã dọn sau khi đo).
