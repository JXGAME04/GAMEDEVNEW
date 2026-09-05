# GIAO KÈO — CẤU HÌNH GAME chỉnh từ WEB ADMIN (04/09/2026)

Hai bên: **web admin** (phiên `webver5-eb`, model `loi/CauHinhGame.php` + `giaodien/admin_cauhinhgame.php`, tab `?muc=cauhinhgame` "Cấu hình game" trong nhóm Hệ thống) và **máy chủ game** (phiên `wauto-c9`, C++ `KCauHinhWeb.cpp` + Lua `script\cauhinh_web\cfgw_driver.lua`). Tài liệu này là nguồn duy nhất về tên bảng / cột / ý nghĩa. Đổi gì phải sửa ở đây trước.

Chủ (04/09 ~18:0x): *"viết lại config kết nối lên web — chỉnh exp, exp kỹ năng, tỉ lệ rớt item, vạn — nói chung là config toàn bộ lên web để chỉnh cho dễ"* và *"thời gian hoạt động nữa đẩy lên web luôn"*.

## 1. Cách chạy

1. Máy chủ khởi động → tạo 3 bảng (`CREATE TABLE IF NOT EXISTS`, khuôn `KChienLenh.cpp`) → **khai** mọi khoá cấu hình đang có kèm mô tả/nhóm/kiểu/khoảng/đơn vị: 27 khoá C++ (`gamesetting.ini` `[ServerConfig]` + `[Exp]`) do C++ tự khai; ~330 khoá script (`script\cauhinh\ch_*.lua` 5 bảng `tbCFG_*` + `script\header\cauhinh_hoatdong.lua` bảng `tbCHD` = lịch/giờ/số người hoạt động) do `cfgw_driver.lua` khai qua `CFGW_Khai(...)`, mô tả lấy tự động từ chú thích trong script (`ReverseTools\cauhinh_web\sinh_cfgw_meta.py` → `cfgw_meta.lua`). Khai = `INSERT IGNORE` (web đã sửa thì giữ), rồi làm mới `v_macdinh`/mô tả/nhóm/thứ tự.
2. Web hiện bảng `gcfg` theo `nhom`, `thu_tu`; admin sửa `v`; sau **mỗi lần lưu** web tăng `gcfg_config['cfg_version']`.
3. Máy chủ dò `cfg_version` mỗi **30 giây** (`CFGW_Tick30`, một `SELECT` theo khoá chính 0,08 ms). Khác → nạp lại toàn bộ `gcfg` → kiểm kiểu/khoảng → **áp**:
   - khoá C++ (`ServerConfig.*`, `Exp.*`): gán thẳng biến toàn cục — **ăn ngay**, không restart;
   - khoá script: giữ trong bộ nhớ; `G_CFG(khoá, mặc_định)` (`ch_lib.lua`) và `HD_CFG` (`cauhinh_hoatdong.lua`) hỏi `CFGW_Get(khoá)` **trước** rồi mới đến bảng tệp → mọi `*_CFG` (TK_CFG, HD_CFG, SKD_CFG, BDH_CFG, DT_CFG, CTC_CFG, VT_CFG, TBH_CFG, BHK_CFG, GLB_CFG…) đều ăn theo. Chỗ đọc lúc chạy (rớt đồ, lịch tick, công tắc `BAT_*` trong `timerserver.lua`) ăn trong ≤ 30 s; giá trị chốt lúc nạp tệp ăn khi `timerserver.lua` tự `dofile` (≤ 1 phút); khoá chỉ đọc một lần lúc boot (đánh `ap_dung = 2`) cần restart.
4. Giá trị hỏng (không phải số, ngoài khoảng, giờ sai) → máy chủ **giữ giá trị cũ**, ghi `gcfg_log` (level 2, kèm `k`) và ghi `v_ap` = giá trị đang thực dùng. Không bao giờ sập vì dữ liệu web.
5. Web **không** `CREATE` / `ALTER` / `DELETE`. Chưa thấy bảng = máy chủ chưa cập nhật bản mới → web hiện "máy chủ chưa hỗ trợ".

## 2. Bảng `gcfg` — máy chủ tạo, web đọc/ghi `v`

```sql
CREATE TABLE IF NOT EXISTS gcfg (
  k          VARBINARY(64)  NOT NULL PRIMARY KEY,  -- tên khoá y nguyên script/ini (ASCII): GLB_TILE_EXP, DRQ_HESO_TIEN, TW_GIO_KHAICHIEN, Exp.HeSo3, ServerConfig.ExpRate…
  v          VARBINARY(255) NOT NULL DEFAULT '',   -- giá trị muốn dùng (WEB SỬA CỘT NÀY)
  v_ap       VARBINARY(255) NOT NULL DEFAULT '',   -- giá trị máy chủ ĐANG THỰC DÙNG (máy chủ ghi sau mỗi lần nạp)
  v_macdinh  VARBINARY(255) NOT NULL DEFAULT '',   -- giá trị trong tệp lúc máy chủ khai (nút "về mặc định" = copy sang v)
  nhom       VARBINARY(32)  NOT NULL DEFAULT '',   -- mã nhóm (bảng tên ở mục 4)
  kieu       TINYINT        NOT NULL DEFAULT 0,    -- 0 số nguyên | 1 số thực | 2 chuỗi TCVN3 | 3 bật/tắt 0/1 | 4 giờ HHMM (730, 2000) | 5 khoá cũ, script không còn dùng
  min_v      DOUBLE         NOT NULL DEFAULT 0,    -- khoảng hợp lệ; min = max = 0 → không kiểm
  max_v      DOUBLE         NOT NULL DEFAULT 0,
  don_vi     VARBINARY(16)  NOT NULL DEFAULT '',   -- ASCII không dấu: x, %, cap, phut, giay, gio, lan, o, xu, luong, diem, map, HHMM
  mota       VARBINARY(255) NOT NULL DEFAULT '',   -- mô tả: byte TCVN3 thô hoặc ASCII không dấu (web hiện qua ChuViet::sangUtf8)
  nguon      VARBINARY(64)  NOT NULL DEFAULT '',   -- ch_chung.lua / ch_drop.lua / cauhinh_hoatdong.lua / gamesetting.ini…
  ap_dung    TINYINT        NOT NULL DEFAULT 0,    -- 0 ăn ngay (≤ 30 s) | 1 ≤ 1,5 phút | 2 cần khởi động lại (nhãn đỏ)
  thu_tu     INT            NOT NULL DEFAULT 0,    -- thứ tự khai trong script → thứ tự hiện trong nhóm
  updated_at INT            NOT NULL DEFAULT 0,
  updated_by VARBINARY(32)  NOT NULL DEFAULT '',   -- tên đăng nhập quản trị (ASCII); máy chủ khai = 'server'
  KEY idx_nhom (nhom, thu_tu)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS gcfg_config (      -- y hệt st_config
  k VARBINARY(32) NOT NULL PRIMARY KEY, v VARBINARY(255) NOT NULL DEFAULT '', updated_at INT NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=latin1;         -- dòng 'cfg_version' máy chủ chèn sẵn = '0'

CREATE TABLE IF NOT EXISTS gcfg_log (          -- MÁY CHỦ ghi, WEB đọc
  id INT AUTO_INCREMENT PRIMARY KEY, ts INT NOT NULL DEFAULT 0, level TINYINT NOT NULL DEFAULT 0,  -- 0 tin, 1 cảnh báo, 2 lỗi
  k VARBINARY(64) NOT NULL DEFAULT '',          -- khoá liên quan ('' = chung)
  msg VARBINARY(255) NOT NULL DEFAULT ''
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
```

🔴 **Luật chữ**: mọi cột `VARBINARY` là **byte TCVN3 thô** hoặc ASCII không dấu; **cấm** ghi UTF-8 (gõ thẳng phpMyAdmin thì game hiện rác). Khoá `k` ASCII thuần. TCVN3 không có chữ HOA mang dấu thanh.

**Định dạng `v`:** kieu 0 số nguyên thập phân (âm được nếu `min_v < 0`); kieu 1 số thực dấu chấm `1.5`; kieu 3 `"0"`/`"1"`; kieu 4 giờ ghi **y như script** (`730`, `2000`, không thêm số 0 đầu; web hiện 07:30, nhận cả `0730`); kieu 2 chuỗi TCVN3 ≤ 255 byte.

## 3. Giao thức

- **Web lưu** (theo nhóm, chỉ khoá đổi): `UPDATE gcfg SET v=?, updated_at=UNIX_TIMESTAMP(), updated_by=? WHERE k=?` … rồi **một lần** `UPDATE gcfg_config SET v=v+1, updated_at=UNIX_TIMESTAMP() WHERE k='cfg_version'` (không có dòng thì `INSERT`). "Về mặc định" = copy `v_macdinh → v` rồi cũng tăng `cfg_version`. Web kiểm kiểu/khoảng trước; máy chủ kiểm lại.
- **Máy chủ** mỗi 30 s: đọc `cfg_version`; đổi → `SELECT k, v, v_ap, v_macdinh, kieu, min_v, max_v FROM gcfg` → kiểm → áp → ghi `v_ap` cho khoá có `v_ap ≠ v` (Post, bất đồng bộ) → ghi `gcfg_log` level 0 "da nap cfg_version=N: X khoa, Y loi, Z khoa C++ doi"; mỗi khoá C++ đổi thêm một dòng level 0 "`<cũ> -> <mới> (ap ngay)`" với `k`.
- **Web hiện trạng thái từng khoá**: `v = v_ap` → đang dùng; `v ≠ v_ap` và `updated_at` < 90 s → "đang chờ máy chủ"; quá đó → đỏ "máy chủ từ chối, đang dùng `v_ap`" (đối chiếu `gcfg_log` theo `k`). Nhãn `ap_dung`: 0 "≤ 30 giây", 1 "≤ 1,5 phút", 2 "cần khởi động lại" (đỏ). Nhóm `HETHONG` hỏi lại trước khi lưu. Khoá `kieu = 5` gom vào "khoá cũ" ẩn mặc định. Web đọc `gcfg_log` 50 dòng mới nhất, tô đỏ level 2, đếm level 2 trong 24 h làm banner.
- **Máy chủ ghi `v`?** Không bao giờ. Máy chủ chỉ ghi `v_ap`, `v_macdinh`, metadata, `kieu = 5`, và `gcfg_log`.

## 4. Nhóm (`nhom`) — web giữ bảng tên, không cần cột

| mã | tên | nguồn khoá |
|---|---|---|
| `EXP` | Kinh nghiệm | `ServerConfig.ExpRate`, `Exp.*` (23 khoá `[Exp]`: `HeSo1..4`, `MocCap1..3`, `VipCong`, `Cs*`, `ChenhCapMax`, `ChiaKhiChenh`, `MienTruCap`, `ToDoi2..8`, `ToDoiKhac`), `GLB_TILE_EXP`, `BRXP_*` |
| `EXP_KYNANG` | Exp kỹ năng | `ServerConfig.Skill90Rate`, `ServerConfig.Skill120Rate` |
| `TIEN` | Tiền (vạn/lượng) | `ServerConfig.MoneyRate`, `GLB_TILE_TIEN` |
| `ROTDO` | Rớt đồ | `DRQ_*` (xác suất quái thường/boss xanh, hệ số tiền rơi), `SKD_*` (rớt sự kiện) |
| `LICH` | Lịch hoạt động | `ch_lich.lua`: `CTC_CHUKY_*`, `CTLD_*`… |
| `HOATDONG` | Hoạt động (bật/tắt, giờ, số người) | `tbCHD`: `BAT_*`, `*_GIO_*`, `*_PHUT_*`, `*_NGUOI_*`… |
| `TONGKIM` | Tống Kim | `TKT_*` |
| `CONGTHANH` | Công thành chiến | `CTC_*` |
| `LOIDAI` | Lôi đài | `CTLD_*` |
| `BANGCHIEN` / `BACHNHAN` / `TYVO` / `THANHBAO` / `VIEMDE` | Bang chiến / Bách nhân / Tỷ võ / Thành bảo / Viêm Đế | `TW_*` / `BR_*` / `BW_*` / `TC_*`, `TS_*` / `YDBZ_*` |
| `BANDONGHANH` | Bạn đồng hành | `BDH_*` |
| `DATAU` | Dã Tẩu | `DT_*` |
| `BOSS` | Boss Hoàng Kim | `BHK_*` |
| `VANTIEU` | Vận tiêu | `VT_*` |
| `TRONGBANG` | Trống bang hội | `TBH_*` |
| `THUONG` | Thưởng hoạt động | còn lại của `ch_thuong.lua` |
| `CHUNG` | Chung | `GLB_*` còn lại |
| `HETHONG` | Hệ thống — cần khởi động lại, hỏi lại trước khi lưu | `GLB_CHE_DO_TEST`, `GLB_GIO_MO_SERVER`, `CH_NAPLAI_PHUT` |

**Không lên web đợt này:** danh mục vật phẩm + `RandRate` trong 49 tệp `settings\droprate\**\*.ini` (máy chủ nạp lại mỗi lần rớt, không cần restart — đợt 2 nếu chủ muốn); nhóm `[Log]` của `gamesetting.ini`; khoá script có giá trị `nil` hoặc bảng con (13 + 10 khoá, ví dụ `TW_MUA9`, `TW_THUONG`).

## 5. Phía máy chủ (wauto-c9)

- `Sources\Core\Src\KCauHinhWeb.cpp/.h` (chỉ `_SERVER`): DDL, khai khoá (`sKhai`), nạp lại (`CauHinhWeb_TuNapLai`), áp C++ (`sApCpp`, khoảng kẹp = bảng `aKep` của `KCore.cpp:844`), ghi ngược `gcfg_log`/`v_ap`. Hàm Lua `CFGW_Get / CFGW_Khai / CFGW_Tick / CFGW_Reload / CFGW_Info` đăng ký ở `ScriptFuns.cpp` (nhánh máy chủ).
- `script\cauhinh_web\cfgw_driver.lua` (Include từ `timerserver.lua`, chạy lại mỗi phút): khai tất cả bảng, nhịp 30 s `AddTimer` (GLB 9004, khuôn `AucWeb_Tick`), bọc `call(..., "x")` để lỗi Lua không giết timer. An toàn với `CoreServer.dll` cũ (mọi chỗ kiểm `CFGW_* == nil`).
- `script\cauhinh_web\cfgw_meta.lua` — **sinh tự động**, đừng sửa tay: `python ReverseTools\cauhinh_web\sinh_cfgw_meta.py <bin\server\script>`; báo cáo `ReverseTools\cauhinh_web\cfgw_meta_baocao.txt` (khoá `ap_dung = 2`, khoá bỏ qua).
- Vá: `ch_lib.lua` `G_CFG` và `cauhinh_hoatdong.lua` `HD_CFG` hỏi `CFGW_Get` trước; `timerserver.lua` thêm một dòng Include.
- Gương trong repo: `serverscript_jx2\cauhinh_web\script\...` (bản chạy thật là `bin\server\script`).

## 6. Phía web (webver5-eb)

Trang "Cấu hình game": lọc theo nhóm + ô tìm khoá/mô tả; sửa `v` tại chỗ theo `kieu/min_v/max_v/don_vi` (bật/tắt = công tắc, HHMM = ô giờ); "về mặc định"; trạng thái `v`/`v_ap`; khối "Nhật ký máy chủ" từ `gcfg_log`; nhãn `ap_dung`; nhật ký quản trị (khoá, cũ → mới, ai); tăng `cfg_version` sau mỗi lần lưu. Model riêng, danh sách trắng riêng, không đụng `Thu::ghi()`.

## 7. Nhị phân + nghiệm thu

| tệp | md5 | ghi chú |
|---|---|---|
| `bin\server\CoreServer.dll.moi` | **0e3634a6328d77ee7eb40ae0fa186fd6** (18.424.832) | = bản đang chạy 5af51667 (S13l + TKBOT/VATCAN) + CFGW; tắt GameServer → `ChayGameServer.bat` |
| `bin\client\CoreClient.dll` | 790fa976dc0ca7d95d8d71bc6e16ca29 | đã live 19:43 (chủ swap cặp 5af51667/790fa976); CFGW chỉ máy chủ → **không cần swap client** lần này |

Nghiệm thu: sau restart, `SELECT COUNT(*) FROM gcfg` ≈ 350; `gcfg_log` có dòng "da nap cfg_version=0: N khoa, 0 loi"; sửa `ServerConfig.ExpRate` trên web → ≤ 30 s có dòng "`<cũ> -> <mới> (ap ngay)`" và exp nhận được đổi ngay; sửa `BAT_TONGKIM` / `TW_GIO_KHAICHIEN` → ≤ 1,5 phút `timerserver` đọc giá trị mới (kiểm `CFGW_Info()` qua lệnh GM hoặc `gcfg.v_ap`). Gõ chữ vào ô số → `gcfg_log` level 2 kèm `k`, `v_ap` giữ số cũ.
