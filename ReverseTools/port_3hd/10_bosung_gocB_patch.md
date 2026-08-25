# 10 — BỔ SUNG VÒNG 2: quét lại 3 hoạt động theo **GỐC DỮ LIỆU B** (`D:\ServerLinux\Patch`)

> Vòng 1 chỉ đọc gốc A (`D:\ServerLinux\server1`). Báo cáo này tra **cả hai gốc** cho mọi
> đường dẫn dữ liệu mà 3 tính năng đọc, và rà lại mọi khẳng định “thiếu / không có / chặn tiến độ”.
> **Không sửa** bất kỳ tệp nào của vòng 1 (00…09) và không đụng `Sources`, `bin`, `E:\SourceTuanLe`.

---

## 0. KẾT LUẬN NGẮN

| Câu hỏi | Trả lời |
|---|---|
| **CHẶN #1 (thiếu bảng toạ độ spawn Phong Lăng Độ) còn đúng không?** | 🟢 **KHÔNG CÒN ĐÚNG — ĐÃ BỊ LẬT.** Tệp có thật, **893 byte, 63 điểm**, ở `D:\ServerLinux\Patch\settings\maps\中原北区\渡船\渡船刷怪点.txt`. Nội dung đầy đủ ở §3. Không phải soạn lại, không phải đoán. |
| Còn đường dẫn dữ liệu nào của 3 tính năng thật sự thiếu ở **cả hai gốc** không? | ❌ **Không.** 38/38 đường dẫn `\settings\…` trong bao đóng đều tra được; 37 có ở **cả A lẫn B trùng byte**, 1 (chính là bảng trên) **chỉ có ở B**. |
| Trong 846 tệp chỉ-có-ở-B, còn tệp nào thuộc 3 tính năng mà vòng 1 chưa nhắc? | 🟡 **Có 4 tệp cận biên** (§6): 3 bảng boss bờ Bắc/bờ Nam Phong Lăng Độ + `great_night\风陵渡.txt`. |
| 55 tệp script lệch kích thước A↔B có tệp nào thuộc 3 tính năng? | 🟡 **2 tệp**: `tong\log.lua`, `tong\tong_header.lua` (§7). **B không phải lúc nào cũng mới hơn.** |
| Có sai sót nào khác của vòng 1 phát hiện thêm? | 🔴 **Có 2**: (a) `08_hai_goc_dulieu.md` mô tả sai bản chất gốc B; (b) `09_bang_toado_patch.md` khẳng định sai cho **72/106 dòng** (§5). |

---

## 1. 🔴 ĐÍNH CHÍNH BẢN CHẤT GỐC B — nó **KHÔNG** phải “lớp cập nhật của máy chủ”

`08_hai_goc_dulieu.md:6` ghi `B (lop cap nhat)`. Thực tế **B là cây CLIENT tiếng Việt** (thư mục
patch/hot-fix ghi đè lên pak của client), bằng chứng ở chính thư mục gốc:

| Bằng chứng | Ý nghĩa |
|---|---|
| `D:\ServerLinux\Patch\game_y.exe` (1.083.450 B), `engine.dll`, `represent2.dll`, `represent3.dll` | nhị phân **client** JX1 |
| `D:\ServerLinux\Patch\package.ini` → `Path=\data`, liệt kê **31 pak client** (`slistcache.pak`, `update01…04.pak`, `updatejx01…15.pak`, `spr.pak`, `ui.pak`, `sound.pak`…) | danh sách pak của **client**, không phải máy chủ |
| `D:\ServerLinux\Patch\{spr, ui, music, users, vng_script}\`, `vauto.exe`, `vhook.dll`, `autoupdate.trk`, `versionlist.ini` | trình tự cập nhật của **client** + bộ VAuto |
| Ngược lại `D:\ServerLinux\server1\` có `jx_linux_y` (ELF), `libheaven.so`, `librainbow.so`, `S2SSyncRelayD`, `pak\`, `Logs\` | đây mới là **máy chủ Linux** |

**Vì sao vẫn dùng được:** VNG đẩy **cùng một cây `settings\` / `script\`** vào gói cập nhật client, nên
gói client mang theo cả dữ liệu vốn chỉ máy chủ dùng (bảng rơi đồ boss, bảng điểm sinh quái…).
⇒ Nội dung vẫn là **dữ liệu gốc thật**, chỉ là **nơi cất khác**. Khi trích dẫn phải gọi đúng tên:
*“cây client `D:\ServerLinux\Patch`”*, không gọi *“lớp cập nhật máy chủ”*.

**Hệ quả phụ đáng ngại (CHƯA XÁC MINH được đầu-cuối):** bản dump máy chủ `server1` mà dự án có
**thật sự không có** tệp `渡船刷怪点.txt`. Nếu bản Linux chạy live cũng thiếu, thì
`TabFile_Load` thất bại → `GetTabFileHeight` in `"Load TabFileError!"` và trả **0**
(`script\lib\file.lua:38-44`) → `fld_head.lua:177` gọi `random(0)`. Tức khâu sinh Thuỷ tặc của
Phong Lăng Độ **có thể đã hỏng** trên chính bản dump này. Điều đó **không ảnh hưởng** việc port
(ta đã có dữ liệu), nhưng giải thích vì sao vòng 1 quét gốc A thấy trống.

---

## 2. BẢNG TRA **CẢ HAI GỐC** CHO MỌI ĐƯỜNG DẪN DỮ LIỆU CỦA 3 TÍNH NĂNG

Phương pháp: lấy **toàn bộ 137 tệp script** trong `closure3.json` (satthu 67 · phonglangdo 82 · vuotai 103,
có trùng nhau), quét **mọi chuỗi trong dấu nháy** chứa `settings` / `.txt` / `.ini` / `.tab`,
gỡ escape `\\`→`\` của Lua, rồi tra vào **bảng kiểm kê tự dựng lại** của cả hai gốc
(giải mã tên thư mục chữ Hán bằng `mbcs`→`gbk`, xem §8).

Kiểm kê tự dựng khớp **chính xác** số liệu vòng 1 ⇒ nền so sánh đáng tin:

| Nhánh | A | B | chỉ A | **chỉ B** | có cả hai nhưng lệch byte |
|---|---|---|---|---|---|
| `settings\` | 2388 | 1754 | 1101 | **467** | 4 |
| `script\` | 5145 | 634 | 4838 | **327** | 55 |
| `maps\` | 747 | 52 | 747 | **52** | 0 (giao = ∅) |

### 2.1 Kết quả tra — 38 đường dẫn `\settings\…`

| Đường dẫn | A? | B? | Byte | Tính năng | Nơi đọc | KẾT LUẬN |
|---|---|---|---|---|---|---|
| `\settings\maps\中原北区\渡船\渡船刷怪点.txt` | ❌ | ✅ | **893** | phonglangdo | `fld_head.lua:18,135,175-181`, `mission.lua:21`, `fld_smalltimer.lua:36,43,50,59` | 🟢 **CÓ DỮ LIỆU** (chỉ ở B) — xem §3 |
| `\settings\droprate\boss\bosstask_lev20.ini` | ✅ | ✅ | 6233 | satthu | `kill_level.lua:47` (đã comment) | có |
| `\settings\droprate\boss\bosstask_lev30.ini` | ✅ | ✅ | 6237 | satthu | `kill_level.lua:50` (đã comment) | có |
| `\settings\droprate\boss\bosstask_lev40.ini` | ✅ | ✅ | 6252 | satthu | `kill_level.lua:53` (đã comment) | có |
| `\settings\droprate\boss\bosstask_lev50.ini` | ✅ | ✅ | 6252 | satthu | `kill_level.lua:56` (đã comment) | có |
| `\settings\droprate\boss\bosstask_lev60.ini` | ✅ | ✅ | 6255 | satthu | `kill_level.lua:59` (đã comment) | có |
| `\settings\droprate\boss\bosstask_lev70.ini` | ✅ | ✅ | 6251 | satthu | `kill_level.lua:62` (đã comment) | có |
| `\settings\droprate\boss\bosstask_lev80.ini` | ✅ | ✅ | 6262 | satthu | `kill_level.lua:65` (đã comment) | có |
| `\settings\droprate\boss\bosstask_lev90.ini` | ✅ | ✅ | 6214 | satthu | `kill_level.lua:69` (**đang chạy**) | có |
| `\settings\task\tollgate\killer\killer.txt` | ✅ | ✅ | 14033 | satthu | `nieshichen.lua:54` | có |
| `settings\npcs.txt` | ✅ | ✅ | 739550 | satthu | `class\ktabfile.lua:69` | có |
| `\settings\maps\challengeoftime\lineup8.txt` | ✅ | ✅ | 99 | vuotai | `missions\challengeoftime\npc.lua:12` | có |
| `\settings\maps\challengeoftime\lineup16.txt` | ✅ | ✅ | 187 | vuotai | `npc.lua:13` | có |
| `\settings\maps\challengeoftime\lineup20.txt` | ✅ | ✅ | 231 | vuotai | `npc.lua:14` | có |
| `\settings\maps\challengeoftime\lineup24.txt` | ✅ | ✅ | 275 | vuotai | `npc.lua:15` | có |
| `\settings\maps\challengeoftime\lineup32.txt` | ✅ | ✅ | 363 | vuotai | `npc.lua:16` | có |
| `\settings\maps\challengeoftime\lineup40.txt` | ✅ | ✅ | 451 | vuotai | `npc.lua:17` | có |
| `\settings\maps\challengeoftime\lineup56.txt` | ✅ | ✅ | 627 | vuotai | `npc.lua:18` | có |
| `\settings\maps\liandandong\npc_3.txt` | ✅ | ✅ | 713 | vuotai | `chuangguang30.lua:216` | có |
| `\settings\event\thapnienlenhbai\taskthapnien.txt` | ✅ | ✅ | 3053 | vuotai | `lenhbai_def.lua:11` | có |
| `\settings\forbitheart.txt` | ✅ | ✅ | 228 | vuotai | `item\heart_head.lua:11` | có |
| `\settings\item_detail.txt` | ✅ | ✅ | 6913 | vuotai | `item\forbiditem.lua:69` | có |
| `\settings\item_type.txt` | ✅ | ✅ | 634 | vuotai | `item\forbiditem.lua:48` | có |
| `\settings\map_type.txt` | ✅ | ✅ | 1869 | vuotai | `item\forbiditem.lua:99` | có |
| `\settings\npc\player\level_exp.txt` | ✅ | ✅ | 7646 | cả 3 | `task\task_addplayerexp.lua:9` | có |
| `\settings\progressconfig.txt` | ✅ | ✅ | 1336 | phonglangdo | `lib\progressbar.lua:9` | có |
| `\settings\task\dailytask\talk.txt` | ✅ | ✅ | 200 | phonglangdo, vuotai | `config\32\talkdailytask.lua:7` | có |
| `\settings\task\dailytask\talk_pos.txt` | ✅ | ✅ | 3259 | phonglangdo, vuotai | `config\32\talkdailytask.lua:7` | có |
| `\settings\task\metempsychosis\translife.txt` | ✅ | ✅ | 2214 | cả 3 | `task\metempsychosis\task_head.lua:19` | có |
| `\settings\tong\tong_level_data.txt` | ✅ | ✅ | 286 | satthu, vuotai | `tong\tong_setting.lua:10` | có |
| `\settings\tong\tongstunt_setting.txt` | ✅ | ✅ | 1315 | satthu, vuotai | `tong\tong_setting.lua:43` | có |
| `\settings\activitysys\awardtable\<id>.txt` *(ghép lúc chạy)* | ✅ | ✅ | — | cả 3 | `activitysys\functionlib.lua:69-70` | thư mục `activitysys\` **92 tệp ở cả A lẫn B, trùng byte 100%** |
| `\settings\maps\yandibaozang\trap\{a,b,c}` + `…\clear\{a,b,c}` *(tiền tố, ghép lúc chạy)* | — | — | — | vuotai | `missions\yandibaozang\head.lua:212-214` | tiền tố, không phải tệp — thư mục `maps\yandibaozang\` có đủ ở cả hai |

**Đối chiếu thư mục con liên quan (toàn bộ, không chỉ tệp được nhắc tên):**

| Thư mục `settings\` | A | B | Lệch |
|---|---|---|---|
| `activitysys\` | 92 | 92 | 0 |
| `maps\challengeoftime\` | 7 | 7 | 0 |
| `task\tollgate\` | 3 | 3 | 0 |
| `missions\` | 36 | 36 | 0 |
| `droprate\boss\` | 60 | 40 | B **thiếu 20** tệp tên tiếng Việt (`caibang.ini`, `thieulam.ini`, `bosshoangkim.ini`…) — **không** tệp nào được 3 tính năng đọc |

⇒ **Ngoài đúng một tệp, không có gì bị bỏ sót ở gốc B.**

---

## 3. 🟢 BẢNG TOẠ ĐỘ SINH QUÁI PHONG LĂNG ĐỘ — NỘI DUNG ĐẦY ĐỦ

**Đường dẫn thật trên đĩa:**
`D:\ServerLinux\Patch\settings\maps\中原北区\渡船\渡船刷怪点.txt` — **893 byte, 65 dòng
(1 tiêu đề + 63 dòng dữ liệu + 1 dòng trống cuối)**. Mã hoá ASCII thuần, phân cách **TAB**, xuống dòng CRLF.
(Tên thư mục = `中原北区` “Trung Nguyên Bắc khu” / `渡船` “Độ thuyền”; tên tệp = `渡船刷怪点`
“điểm refresh quái của thuyền đò”.)

### 3.1 Giải thích từng cột

| Cột | Tiêu đề | Kiểu | Đơn vị | Giải thích |
|---|---|---|---|---|
| 1 | `XPOS` | số nguyên | **fine / “pixel” = ô × 32** | Hoành độ. **100 % (63/63) giá trị chia hết cho 32** ⇒ bảng do trình biên tập bản đồ sinh ra, canh đúng tâm ô. |
| 2 | `YPOS` | số nguyên | **fine / “pixel” = ô × 32** | Tung độ, cùng quy ước. |

Không có cột `map_ID`: bảng dùng chung cho **cả 3 bản đồ thuyền 337/338/339**
(`fld_head.lua:12 boatMAPS = {337,338,339}`), map lấy từ `SubWorld` của mission đang chạy.

### 3.2 Cách script đọc — **hai đơn vị khác nhau trong cùng một tệp**

```lua
-- fld_head.lua:175-181
function fld_getadata(file)
	local totalcount = GetTabFileHeight(file);   -- = TabFile_GetRowCount - 1 = 63
	id = random(totalcount);                     -- 1..63
	x = GetTabFileData(file, id + 1, 1);         -- dòng 2..64 (dòng 1 là tiêu đề)
	y = GetTabFileData(file, id + 1, 2);
	return x,y
end
```

| Nơi dùng | Xử lý giá trị | Đơn vị hàm nhận |
|---|---|---|
| `fld_head.lua:135-139` — người chơi lên thuyền | `posx = floor(posx/32)` rồi `NewWorld(boatmapid, posx, posy)` | **NewWorld nhận Ô** |
| `mission.lua:21-23` — sinh 30 Thuỷ tặc (npc 724) | dùng **nguyên giá trị** `AddNpc(724, 95, SubWorld, posx, posy, 0, "Thủy Tặc ")` | **AddNpc nhận FINE** |
| `fld_smalltimer.lua:36,43,59` — 3 lượt Thuỷ tặc đầu lĩnh (npc 725) | dùng **nguyên giá trị** | FINE |
| `fld_smalltimer.lua:50` — 2 Thuỷ Tặc Đại Đầu Lĩnh (npc 1692) | dùng **nguyên giá trị** | FINE |

🔴 **Bẫy port số 1:** cùng một cặp `(x,y)` lấy ra được dùng **hai đơn vị khác nhau**. Chép sai
một chỗ là người chơi rơi ra ngoài bản đồ hoặc quái sinh chồng một điểm.
🟢 Chỉ số dòng **không lệch**: `GetTabFileHeight` đã trừ tiêu đề (63), còn `TabFile_GetCell`
đánh số **từ 1 và tính cả dòng tiêu đề**, nên `id+1 ∈ [2,64]` phủ **đúng** 63 dòng dữ liệu.

### 3.3 Kiểm chứng độc lập: 63 điểm rơi đúng vào region có thật của bản đồ thuyền JX1

Client JX1 có sẵn dữ liệu địa hình bến thuyền tại
`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\maps2\中原北区\渡船\` (+ `渡船.wor` 113 B, `渡船24.jpg`),
gồm **21 tệp** `v_098…v_101\{097…104}_Region_C.dat`.

Quy ước region suy ra được: **1 region = 16 ô ngang × 32 ô dọc** (`regionX = ôX/16`, `regionY = ôY/32`).
Đối chiếu 63 điểm:

| Cách chia thử | Số region cần | Nằm trọn trong 21 region có thật? |
|---|---|---|
| **16 × 32** | **15** | ✅ **ĐÚNG 15/15** |
| 32 × 32 | 9 | ❌ |
| 16 × 16 | 24 | ❌ |
| 64 × 32 | 6 | ❌ |

⇒ **Không một điểm nào rơi ra ngoài vùng địa hình có thật.** Đây là bằng chứng mạnh nhất rằng
bảng này là **bảng gốc thật của Phong Lăng Độ**, không phải trùng tên.

### 3.4 Phạm vi

| | fine (giá trị trong tệp) | ô (fine / 32) |
|---|---|---|
| X | 50400 … 53152 | **1575 … 1661** |
| Y | 101184 … 104096 | **3162 … 3253** |
| số điểm | 63 | 63 ô **không trùng nhau** |

So với ước lượng của vòng 1 (`02_phonglangdo.md:245` — “ô 1570..1670, 3160..3260”):
**bao trùm đúng**, nhưng vòng 1 đoán từ `boss.lua:28,35` (`1636*32, 3221*32`) — mà **cả 6 hàm
`bossben1A…3B` trong `boss.lua` đều bị comment ở `thuytacdaulinh()` (`boss.lua:19-24`)**, tức
vòng 1 đã lấy tham chiếu từ **mã chết**. Nay có số thật, không cần suy đoán nữa.

### 3.5 NỘI DUNG ĐẦY ĐỦ (63 dòng dữ liệu)

Nguyên văn (TAB phân cách), kèm 2 cột **ô** do báo cáo này tính thêm (`fine / 32`):

| # | XPOS (fine) | YPOS (fine) | ôX | ôY | # | XPOS (fine) | YPOS (fine) | ôX | ôY |
|--:|--:|--:|--:|--:|--:|--:|--:|--:|--:|
| 1 | 50400 | 102240 | 1575 | 3195 | 33 | 51680 | 103136 | 1615 | 3223 |
| 2 | 50560 | 102048 | 1580 | 3189 | 34 | 51872 | 102912 | 1621 | 3216 |
| 3 | 50752 | 101856 | 1586 | 3183 | 35 | 51936 | 102432 | 1623 | 3201 |
| 4 | 51008 | 101792 | 1594 | 3181 | 36 | 52128 | 102272 | 1629 | 3196 |
| 5 | 51072 | 101536 | 1596 | 3173 | 37 | 52288 | 102112 | 1634 | 3191 |
| 6 | 51232 | 101344 | 1601 | 3167 | 38 | 51680 | 103552 | 1615 | 3236 |
| 7 | 51424 | 101184 | 1607 | 3162 | 39 | 51872 | 103360 | 1621 | 3230 |
| 8 | 50656 | 102496 | 1583 | 3203 | 40 | 52064 | 103136 | 1627 | 3223 |
| 9 | 50848 | 102272 | 1589 | 3196 | 41 | 52160 | 102688 | 1630 | 3209 |
| 10 | 51040 | 102080 | 1595 | 3190 | 42 | 52352 | 102496 | 1636 | 3203 |
| 11 | 51264 | 101728 | 1602 | 3179 | 43 | 52480 | 102304 | 1640 | 3197 |
| 12 | 51424 | 101568 | 1607 | 3174 | 44 | 51872 | 103712 | 1621 | 3241 |
| 13 | 51616 | 101408 | 1613 | 3169 | 45 | 52032 | 103520 | 1626 | 3235 |
| 14 | 50848 | 102688 | 1589 | 3209 | 46 | 52256 | 103296 | 1633 | 3228 |
| 15 | 51040 | 102496 | 1595 | 3203 | 47 | 52384 | 102848 | 1637 | 3214 |
| 16 | 51264 | 102336 | 1602 | 3198 | 48 | 52544 | 102656 | 1642 | 3208 |
| 17 | 51424 | 101984 | 1607 | 3187 | 49 | 52672 | 102528 | 1646 | 3204 |
| 18 | 51584 | 101760 | 1612 | 3180 | 50 | 52032 | 103904 | 1626 | 3247 |
| 19 | 51744 | 101600 | 1617 | 3175 | 51 | 52224 | 103712 | 1632 | 3241 |
| 20 | 51104 | 102944 | 1597 | 3217 | 52 | 52416 | 103520 | 1638 | 3235 |
| 21 | 51264 | 102752 | 1602 | 3211 | 53 | 52544 | 103040 | 1642 | 3220 |
| 22 | 51488 | 102528 | 1609 | 3204 | 54 | 52704 | 102848 | 1647 | 3214 |
| 23 | 51616 | 102144 | 1613 | 3192 | 55 | 52864 | 102688 | 1652 | 3209 |
| 24 | 51776 | 101952 | 1618 | 3186 | 56 | 52224 | 104096 | 1632 | 3253 |
| 25 | 51936 | 101760 | 1623 | 3180 | 57 | 52416 | 103936 | 1638 | 3248 |
| 26 | 51296 | 103168 | 1603 | 3224 | 58 | 52544 | 103776 | 1642 | 3243 |
| 27 | 51488 | 102976 | 1609 | 3218 | 59 | 52736 | 103200 | 1648 | 3225 |
| 28 | 51680 | 102720 | 1615 | 3210 | 60 | 52896 | 103040 | 1653 | 3220 |
| 29 | 51776 | 102272 | 1618 | 3196 | 61 | 53056 | 102848 | 1658 | 3214 |
| 30 | 51936 | 102112 | 1623 | 3191 | 62 | 53024 | 103168 | 1657 | 3224 |
| 31 | 52096 | 101920 | 1628 | 3185 | 63 | 53152 | 103072 | 1661 | 3221 |
| 32 | 51488 | 103360 | 1609 | 3230 |  |  |  |  |  |

```
# nguyên văn 65 dòng (tiêu đề + 63 dòng + 1 dòng trống cuối), phân cách TAB, CRLF
XPOS	YPOS
50400	102240
50560	102048
50752	101856
51008	101792
51072	101536
51232	101344
51424	101184
50656	102496
50848	102272
51040	102080
51264	101728
51424	101568
51616	101408
50848	102688
51040	102496
51264	102336
51424	101984
51584	101760
51744	101600
51104	102944
51264	102752
51488	102528
51616	102144
51776	101952
51936	101760
51296	103168
51488	102976
51680	102720
51776	102272
51936	102112
52096	101920
51488	103360
51680	103136
51872	102912
51936	102432
52128	102272
52288	102112
51680	103552
51872	103360
52064	103136
52160	102688
52352	102496
52480	102304
51872	103712
52032	103520
52256	103296
52384	102848
52544	102656
52672	102528
52032	103904
52224	103712
52416	103520
52544	103040
52704	102848
52864	102688
52224	104096
52416	103936
52544	103776
52736	103200
52896	103040
53056	102848
53024	103168
53152	103072
```

---

### 3.6 So với bảng Phong Lăng Độ **đang chạy** của JX1 — hai bảng **gần như không trùng nhau**

| Bảng | Nguồn | Số điểm | Đơn vị lưu | Phạm vi ô | Chia hết 32 |
|---|---|---|---|---|---|
| **GỐC THẬT (Linux, cây client B)** | `settings\maps\中原北区\渡船\渡船刷怪点.txt` | **63** | fine | X 1575–1661 · Y 3162–3253 | **126/126 (100 %)** |
| JX1 đang chạy | `bin\server\script\tinhnang\phonglangdo\lib_phonglangdo.lua:28-68` `TAB_QUAIVUOTAIPLD` *[đã sửa theo đối chất — trước ghi `28-106`, nhưng 70-77 là `TAB_THULINH`, 79-86 là `TAB_NPCCHUCNANG`, 88-106 là `TAB_TIME_PLD`]* | **30 bật + 9 bị comment** | **ô** (nhân 32 lúc `AddNpcEx3`, `:163`) | X 1582–1652 · Y 3167–3249 | — |
| JX1 tệp mồ côi | `bin\server\script\header\phonglangdo.lua:2-45` `NPC_PHONGLANG` | 44 | fine | X 1572–1656 · Y 3167–3252 | **7/88 (8 %)** |
| JX1 boss mồ côi | `header\phonglangdo.lua:47-50` `BOSS_PHONGLANG` | 2 | fine | X 1613–1642 · Y 3189–3216 | **0/4** |

🔴 **Phát hiện:** hai bảng của JX1 **không canh ô** (chỉ 8 % và 0 % chia hết 32) ⇒ chúng là
**số soạn tay / rải ngẫu nhiên**, không phải trích từ trình biên tập bản đồ. Đối chiếu theo ô:

- `TAB_QUAIVUOTAIPLD` (30 ô) ∩ bảng gốc (63 ô) = **1 ô** duy nhất `(1621, 3230)`.
- `NPC_PHONGLANG` (44 điểm) ∩ bảng gốc = **2 ô**: `(1601, 3167)`, `(1603, 3224)`.

⇒ Khi port, **thay hẳn** bằng bảng gốc 63 điểm sẽ khôi phục đúng bố cục quái bản gốc.

🔴 **Bẫy port số 2 — đơn vị ngược nhau giữa hai cây:**
- Bản Linux: bảng lưu **fine**, `AddNpc` dùng thẳng, `NewWorld` phải **chia 32**.
- JX1 đang chạy: bảng lưu **ô**, `AddNpcEx3` **nhân 32** (`lib_phonglangdo.lua:163`).

⇒ Nếu bê thẳng 63 cặp fine vào chỗ `TAB_QUAIVUOTAIPLD` thì `AddNpcEx3` sẽ nhân 32 lần nữa
(≈ 1,6 triệu) → quái sinh ngoài bản đồ. **Phải chia 32 trước** (cột “ô” ở bảng §3.5 đã tính sẵn).

---

## 4. RÀ LẠI TOÀN BỘ KHẲNG ĐỊNH “THIẾU / KHÔNG CÓ / CHẶN” CỦA VÒNG 1

| # | Khẳng định vòng 1 | Sự thật (vòng 2) | Bằng chứng |
|---|---|---|---|
| 1 | `05_dulieu.md:30` — “**CHẶN TIẾN ĐỘ: 1 hạng mục cứng** — Thiếu bảng toạ độ spawn Phong Lăng Độ ở **cả hai** cây và **mọi** pak” | 🔴 **SAI**. Có trên đĩa, 893 B. (“cả hai cây” của vòng 1 = Linux-A + JX1; họ chưa biết gốc B.) | `D:\ServerLinux\Patch\settings\maps\中原北区\渡船\渡船刷怪点.txt` |
| 2 | `05_dulieu.md:486` — bảng §5.5 ghi `渡船刷怪点.txt` : LINUX = ❌ **KHÔNG CÓ** | 🔴 **SAI ở cột LINUX** (đúng ở cột JX1) | như trên |
| 3 | `05_dulieu.md:515-545` — “CHẶN #1 (CỨNG)… Phải **soạn lại bảng toạ độ**” | 🔴 **KHÔNG CÒN**. Không phải soạn lại. | §3 |
| 4 | `05_dulieu.md:536` — “băm lại **44 pak** (514.459 mục) → vẫn KHÔNG CÓ ⇒ **CHẶN #1 vẫn đứng vững**” | 🟡 **Phép băm đúng, kết luận sai** — tệp nằm **ngoài pak**, dưới dạng tệp rời trong thư mục patch của client. Băm pak không bao giờ tìm ra. | quét byte toàn `D:\ServerLinux` (§8) |
| 5 | `05_dulieu.md:534` — “`settings\maps\` của bản Linux **không có thư mục tên chữ Hán nào** (chỉ 33 thư mục ASCII)” | 🟡 **Đúng cho A, sai cho “bản Linux”**. B có **4 khu chữ Hán** (`东北区`, `江南区`, `西北北区`, `中原北区`) → **9 thư mục con, 20 tệp** *[đã sửa theo đối chất — trước ghi 13/22]*. ⚠️ Con số “33 thư mục ASCII” của vòng 1 cũng **sai**: `D:\ServerLinux\server1\settings\maps` chỉ có **28 thư mục** (+1 tệp `damahuboss.txt` = 29 mục). | liệt kê `D:\ServerLinux\Patch\settings\maps` và `…\server1\settings\maps` |
| 6 | `02_phonglangdo.md:233-245` — “§4.3 Bảng điểm sinh quái — **THIẾU TRONG BẢN TRÍCH XUẤT**… phụ thuộc **BẮT BUỘC còn thiếu**” | 🔴 **BỊ LẬT** | §3 |
| 7 | `02_phonglangdo.md:238` — “Thư mục `settings\maps\中原北区\` **KHÔNG có** trong `D:\ServerLinux\server1`” | 🟢 **ĐÚNG** (câu chữ chính xác vì có nêu rõ `server1`) — chỉ là kết luận rút ra từ nó thì sai | — |
| 8 | `02_phonglangdo.md:658` — ước lượng “ô 1570..1670, 3160..3260” **được xác nhận bao trùm đúng vùng thật** | 🟢 **ĐÚNG** — vùng thật là 1575–1661 / 3162–3253 | §3.4 |
| 9 | `02_phonglangdo.md:650-656` (B4) — “✅ TÌM ĐƯỢC dữ liệu bù… `NPC_PHONGLANG` 44 điểm” | 🟡 **Đúng là có, nhưng đó KHÔNG phải dữ liệu gốc** — chỉ 8 % canh ô, trùng bảng gốc 2/63 ô. Dùng bảng gốc thay vì bảng bù. | §3.6 |
| 10 | `05_dulieu.md:546` — “CHẶN #2 (MỀM) — thiếu 7 tệp `lineup*.txt` **ở cây JX1**” | 🟢 **VẪN ĐÚNG** — 7 tệp có đủ ở **cả A lẫn B** (99–627 B, trùng byte), thiếu ở JX1 | §2.1 |
| 11 | `05_dulieu.md:568` — “CHẶN #3 (MỀM) — 16 dòng vật phẩm phải soạn thêm” | 🟢 **KHÔNG ĐỔI** — gốc B không có bảng item nào liên quan (chỉ 5 tệp `item\00x\装备版本说明.txt` = ghi chú phiên bản) | §6 |
| 12 | `05_dulieu.md:712` (BS-2) — “`GetTabFileData/GetTabFileHeight` không tồn tại ở JX1 ⇒ phải chép `script\lib\file.lua`” | 🟢 **KHÔNG ĐỔI** — `script\lib\file.lua` chỉ có ở A, **không có** ở B | kiểm kê §2 |
| 13 | `05_dulieu.md:578` — “`settings\droprate\boss\` của JX1 **chỉ có `helianpiaodroprate.ini`** ⇒ phải chép `bosstask_lev*.ini`” | 🟢 **KHÔNG ĐỔI** — 8 tệp `bosstask_lev20…90.ini` có ở **cả A lẫn B, trùng byte**, chép từ đâu cũng được | §2.1 |
| 14 | `09_bang_toado_patch.md:3-4` — “Những tệp này **KHÔNG có** trong `D:\ServerLinux\server1\settings`” (áp cho cả 106 dòng) | 🔴 **SAI cho 72/106 dòng** | §5 |

---

## 5. 🔴 SAI SÓT CỦA `09_bang_toado_patch.md`

Câu mở đầu của 09 khẳng định **toàn bộ** 106 tệp trong bảng là chỉ-có-ở-B. Kiểm lại từng dòng:

| Phân loại | Số dòng |
|---|---|
| **Thật sự chỉ có ở B** | **34** |
| **Có ở CẢ HAI gốc, trùng byte** (khẳng định của 09 là sai) | **72** |
| Tổng | 106 |

Trong 72 dòng bị nói sai có nhiều tệp quan trọng của chính 3 tính năng:
`droprate\boss\bosstask_lev20…90.ini` (8), toàn bộ 22 tệp `droprate\boss\*droprate.ini`,
`task\tollgate\killer\killer.txt`, **cả 7 tệp `maps\challengeoftime\lineup*.txt`**,
`killer.ini`, `goldboss.txt`, `trigger_challengeoftime.lua`, `missions\boss\libai\boxpos.txt`,
10 tệp `maps\newboss\*boss.txt`, `maps\citydefence\{junior,senior}boss.txt`, `maps\damahuboss.txt`…

⚠️ **Hệ quả thực tế:** bảng dữ liệu trong 09 vẫn **đúng nội dung** (đã giải mã ra
`src_utf8\_patch_settings\`), chỉ **sai xuất xứ**. Ai đọc 09 rồi kết luận “vòng 1 bỏ sót
`bosstask_lev90.ini`” là **hiểu nhầm** — tệp đó vốn nằm sẵn ở gốc A và vòng 1 đã dùng đúng
(`05_dulieu.md:571`). Chỉ **34 dòng** mới thật sự là đóng góp riêng của gốc B.

**34 tệp settings thật sự chỉ có ở B (đầy đủ)** *[đã sửa theo đối chất]*: 16 tệp `battles\maps\boss\*`
(`boss点2/4.txt`, `后营2/4(.trap).txt`, `大营2/4…`, `帅旗2/4.txt`)
+ **16 tệp** `maps\{东北区,江南区,西北北区,中原北区}\…\*boss.txt`
+ `maps\中原北区\渡船\渡船刷怪点.txt` + `maps\great_night\风陵渡.txt` = **34**.
⚠️ Bản trước liệt kê nhầm `droprate\goldennpc\{各种类droprate文字描述, 各迷宫所使用droprate对照}.txt`
và `西山屿\npc.txt` — 3 tệp này **chỉ-có-ở-B thật**, nhưng **không nằm trong 106 dòng của `09`**,
nên không thuộc 34 dòng đang xét; ngược lại bản trước **bỏ quên** `maps\great_night\风陵渡.txt`
(là dòng 82 của `09`). Tổng của cách liệt kê cũ cũng là 36 ≠ 34.

---

## 6. QUÉT 846 TỆP CHỈ-CÓ-Ở-B: CÒN GÌ THUỘC 3 TÍNH NĂNG?

Phân bố 467 tệp `settings\` chỉ-có-ở-B:

| Nhóm | Số tệp | Liên quan 3 tính năng? |
|---|---|---|
| `npcres\` + `npcres_simple\` (bảng tài nguyên đồ hoạ nhân vật) | **174** (87 + 87) *[đã sửa theo đối chất — trước ghi 352, khiến tổng bảng thành 642 ≠ 467]* | ❌ dữ liệu **client**, không phải logic |
| `battles\maps\*` (bản đồ Tống Kim: barrack/boss/bridge/desert/forest/…) | **232**→ trong đó 16 thuộc `boss\` | ❌ Tống Kim, không phải 3 hoạt động |
| `maps\great_night\` | 23 | 🟡 **1 tệp** — xem dưới |
| `maps\` khu chữ Hán (`东北区`/`江南区`/`西北北区`/`中原北区`) | 20 | 🟢 **1 tệp cốt lõi + 3 tệp cận biên** |
| `maps\menglan_2006\` | 7 | ❌ |
| `item\00x\装备版本说明.txt` (5), `faction\门派设定.ini` (1), `droprate\goldennpc\*` (2), 3 tệp ở gốc `settings\` | **11** *[đã sửa theo đối chất — trước ghi 8]* | ❌ ghi chú/tài liệu |
| **TỔNG** | **467** = 174 + 232 + 23 + 20 + 7 + 11 | (đã cân) |

### Kết quả: **1 tệp trong phạm vi + 4 tệp cận biên nên lấy kèm**

| Tệp (chỉ có ở B) | Byte | Dòng | Quan hệ | Ai đọc |
|---|---|---|---|---|
| `maps\中原北区\渡船\渡船刷怪点.txt` | 893 | 63 dữ liệu | 🟢 **TRONG PHẠM VI** — Phong Lăng Độ | `fld_head.lua:18` (§3) |
| `maps\中原北区\风陵渡北岸\baiyingyingboss.txt` | 99 | 8 | 🟡 cận biên — boss **bờ Bắc Phong Lăng Độ**, cùng map 336 | `missions\boss\bossdown.lua:10`, `callboss_incity.lua:8` (npc 742 “Bạch Doanh Doanh”) |
| `maps\中原北区\风陵渡北岸\yanxiaoqianboss.txt` | 99 | 8 | 🟡 cận biên — cùng map 336 | `bossdown.lua:9`, `callboss_incity.lua:10` (npc 744 “Yến Hiểu Thiến”) |
| `maps\中原北区\风陵渡南岸\herenwoboss.txt` | 143 | 12 | 🟡 cận biên — **bờ Nam**, cũng map 336 | `bossdown.lua:11`, `callboss_incity.lua:7` (npc 741) |
| `maps\great_night\风陵渡.txt` | 1520 | **100** | 🟡 cận biên — 100 toạ độ **ô** hợp lệ trên **map 336** | hệ `great_night` |

⚠️ Bốn tệp cận biên **không** nằm trong `closure3.json` (chúng thuộc hệ “Boss giáng lâm”
`missions\boss\` và hệ `great_night`), nên **vòng 1 không sai khi bỏ qua**. Nhưng chúng cho
**toạ độ hợp lệ đã kiểm chứng trên đúng bản đồ 336** — rất đáng lấy khi dựng lại bờ Bắc/bờ Nam.

**Nội dung 3 bảng boss (đầy đủ, cột `XPOS`/`YPOS`, đơn vị **Ô** — khác `渡船刷怪点.txt`!):**

```
风陵渡北岸\baiyingyingboss.txt (8 điểm)   风陵渡北岸\yanxiaoqianboss.txt (8 điểm)
  1110  3016                                1252  2698
  1110  2920                                1247  2546
  1109  2811                                1298  2568
  1114  2600                                1290  2745
  1173  2580                                1378  2811
  1190  2712                                1483  2674
  1198  2787                                1516  2531
  1239  2860                                1560  2688

风陵渡南岸\herenwoboss.txt (12 điểm)
  1565 2992 · 1580 3106 · 1586 3180 · 1443 3149 · 1406 3073 · 1360 3182
  1259 3137 · 1304 3027 · 1222 3033 · 1213 3110 · 1106 3128 · 1347 3122
```

🔴 **Bẫy port số 3:** các bảng `*boss.txt` này lưu **Ô** (1110, 3016 — không chia hết 32),
còn `渡船刷怪点.txt` lưu **fine**. Cùng thư mục cha, khác đơn vị. Phải kiểm từng bảng.

**Bổ sung về `maps\great_night\风陵渡.txt`:** cột `map_ID / nPosX / nPosY`, 100 dòng, **toàn bộ map 336**,
ô **X 1370–1522, Y 2984–3152** *[đã sửa theo đối chất — trước ghi “X 1370–1435, Y 3079–3152”,
đó chỉ là phạm vi của ~51 dòng đầu; dòng 52 trở đi mở rộng tới X 1522 và xuống Y 2984]*.
JX1 **đã có** tệp cùng tên tại
`E:\…\bin\server\settings\maps\great_night\风陵渡.txt` nhưng **chỉ 303 byte = 1 tiêu đề + 19 dòng dữ liệu**
*[đã sửa theo đối chất — trước ghi 18 dòng]* — tức **bản JX1 bị cắt cụt, 19/100 dòng**,
và 19 dòng đó **trùng khít** 19 dòng đầu bản Linux (đã so từng dòng).
⇒ Nếu cần đủ, lấy bản 1520 byte ở gốc B.

---

## 7. 55 TỆP SCRIPT LỆCH KÍCH THƯỚC A↔B — CÓ 2 TỆP THUỘC 3 TÍNH NĂNG

Trong 327 tệp script chỉ-có-ở-B: **0 tệp** thuộc bao đóng 3 tính năng (toàn `skill\`, `partner\`, `tong\workshop\`).
Trong 137 tệp script của bao đóng, chỉ **5 tệp** cũng xuất hiện ở B, và **2 trong đó lệch**:

| Tệp | A | B | Bên nào “mới hơn” | Nội dung khác biệt |
|---|---|---|---|---|
| `tong\tong_header.lua` | 6722 B / 171 dòng | **7009 B / 187 dòng** | 🟢 **B mới hơn** | B **thêm** `TONGTSK_TriumphDrum_LastUseDay = 1211`, `TONGTSK_TriumphDrum_BeUsed = 1212`, `TONGTSK_VNG_CUSTOM2/3/4 = 1214/1215/1216`; sửa chuỗi hiển thị `TB_CITYID_STUNTID` (thêm dấu cách đầu) và `TB_STUNT_PAUSESTATE` (`"më "` → `"Khai më"`) |
| `tong\log.lua` | **1651 B / 54 dòng** | 1419 B / 50 dòng | 🔴 **A mới hơn / đầy đủ hơn** | B **THIẾU** hàm `cTongLog:WriteTongMoneyChangeLog(...)` mà **A có ở `log.lua:32`** và **`tong\tong_mix.lua:65,77,157` đang gọi** |
| `tong\contribution_entry.lua` | 7488 | 7488 | trùng byte | — |
| `tong\tong_award_head.lua` | 4595 | 4595 | trùng byte | — |
| `tong\tong_setting.lua` | 5610 | 5610 | trùng byte | — |

🔴 **Kết luận quan trọng:** **gốc B KHÔNG phải “bản mới hơn” một cách nhất quán.**
Cùng thư mục `tong\`, một tệp B mới hơn (thêm 5 hằng số), một tệp B lại **thiếu hàm** so với A.
⇒ **Cấm chép đè nguyên thư mục B lên A.** Phải đối chiếu **từng tệp**.
⇒ Với 3 hoạt động này, **ưu tiên gốc A cho `script\`**; gốc B chỉ dùng để:
(a) lấy `settings\maps\中原北区\渡船\渡船刷怪点.txt` và 4 tệp §6;
(b) tham chiếu thêm 5 hằng số task id 1211/1212/1214/1215/1216 của `tong_header.lua`
(⚠️ **kiểm trùng task id với JX1 trước khi dùng** — xem cảnh báo trùng task id đã biết của dự án).

---

## 8. PHƯƠNG PHÁP & BẪY KỸ THUẬT (để vòng sau khỏi vấp lại)

1. **Tên thư mục chữ Hán trên đĩa là byte GBK thô.** Windows trả về chuỗi đã giải mã theo
   ANSI codepage → mojibake (`ÖÐÔ­±±Çø`). Cách khôi phục **đúng**:
   `ten.encode('mbcs').decode('gbk')`.
   ⚠️ **Đừng dùng `cp1258`** *[cơ chế đã sửa theo đối chất]*: không phải vì “cp1258 không ánh xạ được
   `0xD0`” — `cp1258` **giải mã** `0xD0` bình thường (ra `Đ` U+0110) và khứ hồi `d6d0d4adb1b1c7f8`
   vẫn đúng. Lỗi thật là **ANSI codepage của máy này là 1252**, nên Windows trả tên đã giải mã theo
   cp1252 (`0xD0` → `Ð` U+00D0); đem chuỗi đó **encode ngược bằng cp1258** thì hỏng vì
   `U+00D0` **không có trong cp1258** (`'Ð'.encode('cp1258')` → `UnicodeEncodeError`).
   ⇒ Quy tắc đúng: **encode lại bằng đúng ACP đang dùng** (`mbcs`), đừng đoán codepage.
   (Kiểm chứng: `ctypes.windll.kernel32.GetACP()` = **1252**.)
2. **`os.path.join` với chuỗi Unicode tiếng Trung sẽ báo không tìm thấy tệp** dù tệp có thật.
   Phải **duyệt cây bằng `os.walk` rồi giải mã tên để so sánh**, không dựng đường dẫn từ chuỗi Unicode.
3. **Chuỗi đường dẫn trong Lua có escape `\\`.** Regex bắt chuỗi phải `replace('\\\\','\\')`
   trước khi tra tệp — nếu không thì **mọi** đường dẫn đều “không tìm thấy” (lỗi này đã xảy ra
   ở lần chạy đầu của chính vòng 2 này).
4. **Bash heredoc rút `\\` thành `\`** → mọi script Python có backslash phải ghi bằng công cụ Write
   rồi mới chạy (đúng như ký ức dự án đã ghi).
5. **Băm pak không thay được quét đĩa.** Vòng 1 băm 44 pak / 514.459 mục và MISS, vì tệp là
   **tệp rời trong thư mục patch của client**. Luôn quét byte toàn cây **trước**, băm pak **sau**.
6. **Kiểm chứng toạ độ bằng region có thật** là phép thử rẻ và mạnh: đọc tên thư mục
   `maps2\<khu>\<bản đồ>\v_<regionY>\<regionX>_Region_C.dat`, suy `region = 16 ô × 32 ô`,
   rồi kiểm mọi điểm có rơi vào region tồn tại không (§3.3).

---

## 9. VIỆC CẦN LÀM TIẾP (đề xuất, chưa thực hiện)

| # | Việc | Ghi chú |
|---|---|---|
| 1 | Đưa 63 điểm §3.5 vào JX1, **dạng Ô**, thay `TAB_QUAIVUOTAIPLD` (`lib_phonglangdo.lua:28-68`) *[đã sửa theo đối chất — KHÔNG phải 28-106, đó là 4 bảng khác nhau]* | vì `AddNpcEx3` tự nhân 32 |
| 1b | **Thay luôn `TAB_THULINH`** (`lib_phonglangdo.lua:70-77`, 6 điểm soạn tay, 0/12 chia hết 32, chỉ trùng bảng gốc **1/63 ô**) | bản Linux lấy boss đầu lĩnh từ **cùng** `渡船刷怪点.txt` (`fld_smalltimer.lua:36,43,50,59`) — xem đối chất |
| 1c | Mở lại 2 nhánh **map 338/339** đang bị comment (`lib_phonglangdo.lua:168,173` cho quái, `:215,222` cho boss) | không mở thì thuyền 2/3 trống, khác hẳn bản gốc (mỗi map chạy 1 mission, 30 quái/chuyến) |
| 2 | Bỏ / hợp nhất `header\phonglangdo.lua` (tệp mồ côi, bảng 44 điểm soạn tay) | vòng 1 đã nêu ở B6 |
| 3 | Chép nguyên tệp gốc + 4 tệp §6 vào kho dữ liệu port | đường dẫn đầy đủ ở §2.1, §6 |
| 4 | Bù `maps\great_night\风陵渡.txt` từ 18 → 100 dòng nếu hệ `great_night` được port | §6 |
| 5 | Đối chiếu 5 task id mới của `tong_header.lua` (1211,1212,1214,1215,1216) với bảng task id JX1 | §7 |
| 6 | Kiểm xem `map_type.txt` / `waypoint.txt` của JX1 đã khai 337/338/339 chưa | `WorldSet2.txt` đã có: `336 phong lang do`, `337/338/339 thuyen PLD 1/2/3`; `bin\server\Maps\` mới chỉ có `336_srv.fp` |

---

*Vòng 2 — chỉ phân tích, không sửa mã. Mọi khẳng định đều kèm đường dẫn tệp hoặc `tệp:dòng`.
Chỗ chưa dò tới được ghi rõ “CHƯA XÁC MINH”.*

---
---

## ĐỐI CHẤT (tác tử độc lập)

> Người kiểm chứng **không** viết báo cáo này. Mặc định coi mọi khẳng định là **SAI cho tới khi
> nguồn gốc chứng minh ngược lại**. Toàn bộ số liệu dưới đây được **dựng lại từ đầu**, không đọc
> lại kết quả trung gian của vòng 2: kiểm kê A/B tự đi `os.walk` + MD5 từng tệp, bảng toạ độ đọc
> byte thô, bảng JX1 phân tích lại bằng regex trên tệp gốc. Không sửa `Sources`, `bin`,
> `E:\SourceTuanLe`; không đụng `00_`…`09_`.

**Tổng kết: kiểm 24 khẳng định — 8 SAI, 4 ĐÚNG-MỘT-PHẦN, 12 ĐÚNG. Tìm thêm 4 chỗ vòng 2 bỏ sót.**
Lõi của báo cáo (CHẶN #1 bị lật, bảng 63 điểm, số liệu kiểm kê A/B, §7) **đứng vững hoàn toàn**;
mọi lỗi tìm được đều nằm ở **các bảng đếm phụ và trích dẫn dòng**.

### A. Bảng đối chất

| # | Khẳng định (mục) | Bằng chứng gốc (đã tự dựng lại) | KẾT LUẬN | Sửa lại thành |
|--:|---|---|---|---|
| 1 | §0/§3: tệp `Patch\settings\maps\中原北区\渡船\渡船刷怪点.txt` có thật, **893 byte, 65 dòng, 63 điểm** | Đọc byte thô: `len=893`, `CRLF=64`, kết thúc bằng CRLF, 1 tiêu đề `XPOS\tYPOS` + 63 dòng dữ liệu | ✅ **ĐÚNG** | — |
| 2 | §3.5: khối “nguyên văn 63 dòng” và bảng ô kèm theo | So **từng byte** 63 cặp trong báo cáo với tệp gốc: `VERBATIM MATCH = True`; 63/63 dòng bảng md khớp, 0 lỗi cột `ôX/ôY` | ✅ **ĐÚNG** (chép chính xác tuyệt đối) | — |
| 3 | §3.1/§3.6: **126/126 (100 %)** giá trị chia hết 32; 63 ô **không trùng nhau** | Tính lại: `divisible-by-32 = 126/126`; `unique cells = 63/63` | ✅ **ĐÚNG** | — |
| 4 | §3.4: fine X 50400…53152 / Y 101184…104096 ⇒ ô X 1575–1661 / Y 3162–3253 | min/max tính lại trùng khít | ✅ **ĐÚNG** | — |
| 5 | §3.3: **21 tệp** region, quy ước **1 region = 16 ô × 32 ô**, 63 điểm rơi trọn **15/15** region có thật; 3 cách chia khác đều ❌ | `bin\client\maps2\中原北区\渡船\` = `v_098`(3) + `v_099`(7) + `v_100`(7) + `v_101`(4) = **21**. 15 region cần: `v_<ôY/32>\<ôX/16>_Region_C.dat` — **cả 15 đều tồn tại**. Chia 32×32 → cần tệp `049…051` (không có); 16×16 → cần `v_197…v_203` (không có); 64×32 → cần `024/025` (không có) | ✅ **ĐÚNG** (bổ sung: trục là `v_ = regionY`, tên tệp = `regionX`; nếu hoán vị 2 trục thì thiếu hẳn `v_102` ⇒ phép thử này còn phân biệt được cả chiều trục) | — |
| 6 | §2: kiểm kê `settings` A=2388 B=1754 chỉ-B=467 lệch=4 · `script` 5145/634/327/55 · `maps` 747/52/0 | Tự đi `os.walk` + MD5: **settings A=2388 B=1754 onlyA=1101 onlyB=467 both=1287 differ=4**; **script 5145/634/4838/327/307/55**; **maps 747/52/747/52/0** | ✅ **ĐÚNG** (khớp 100 %, kể cả ô “giao = ∅” của `maps`) | — |
| 7 | §2.1: 30 tệp `settings` cụ thể (8 `bosstask_lev*`, `killer.txt`, `npcs.txt` 739 550 B, 7 `lineup*`, …) có ở **cả A lẫn B, trùng byte**, đúng từng con số byte | Tra 30/30 đường dẫn: có ở A **và** B, **MD5 bằng nhau**, kích thước khớp từng byte (`errors: 0`) | ✅ **ĐÚNG** | — |
| 8 | §0/§2: “**38/38** đường dẫn `\settings\…` tra được; **37** có ở cả A lẫn B **trùng byte**, 1 chỉ có ở B” | Trích lại độc lập từ 137 tệp closure: **40 chuỗi dữ liệu**, **38** nằm dưới `\settings\` ✔. Nhưng trong 38 đó chỉ **30 là tệp thật**; 7 mục là **tiền tố thư mục** `maps\yandibaozang\trap\{a,b,c}` + `…\clear\{a,b,c}` và 1 là tiền tố `activitysys\awardtable\` — chúng **không phải tệp** nên không thể “trùng byte” | 🟡 **ĐÚNG MỘT PHẦN** | “38/38 **chuỗi** tra được; **30 tệp thật** có ở cả A lẫn B và **trùng byte**, **1 tệp chỉ có ở B**, **7 mục còn lại là tiền tố thư mục** (`yandibaozang` ×6 + `activitysys\awardtable\`), tra ở mức thư mục thì đủ ở cả hai gốc” |
| 9 | §5: `09_bang_toado_patch.md` có **106 dòng**, **72** dòng bị nói sai (thật ra có ở cả A), **34** dòng mới thật sự chỉ-có-ở-B; 72 dòng đó **trùng byte** A↔B | Parse 106 dòng bảng của `09`, tra vào kiểm kê: **inA = 72**, **onlyB = 34**, và **72/72 MD5 bằng nhau**; kích thước byte của `09` khớp đĩa **106/106** | ✅ **ĐÚNG** (con số chính xác tuyệt đối) | — |
| 10 | §5: liệt kê “34 tệp đầy đủ” = 16 `battles\maps\boss\*` + 2 `droprate\goldennpc\*` + 16 `*boss.txt` + `渡船刷怪点.txt` + `西山屿\npc.txt` | `09` **không** có dòng nào cho `goldennpc\各种类droprate文字描述.txt`, `各迷宫所使用droprate对照.txt` hay `西山屿\npc.txt` (grep = 0); ngược lại **có** dòng 82 = `maps\great_night\风陵渡.txt` mà bản liệt kê bỏ quên. Cách đếm cũ ra 16+2+16+1+1 = **36 ≠ 34** | 🔴 **SAI** | 16 `battles\maps\boss\*` + **16** `maps\{东北区,江南区,西北北区,中原北区}\…\*boss.txt` + `渡船刷怪点.txt` + **`great_night\风陵渡.txt`** = **34**. (3 tệp kia chỉ-có-ở-B thật nhưng **không nằm trong 106 dòng của `09`**.) — **đã sửa trong thân bài** |
| 11 | §6: phân bố 467 tệp chỉ-có-ở-B: `npcres\`+`npcres_simple\` = **352** | Đếm lại: `npcres` = **87**, `npcres_simple` = **87** ⇒ **174**. Tổng các dòng của bảng cũ = 352+232+23+20+7+8 = **642 ≠ 467** (bảng không cân) | 🔴 **SAI** | **174** (87+87). Tổng cân: 174+232+23+20+7+**11** = **467** — **đã sửa trong thân bài** |
| 12 | §6: nhóm “ghi chú/tài liệu” (`item\00x`, `faction`, …) = **8** | Đếm lại phần dư: `item\00{0..4}` 5 + `faction` 1 + `droprate\goldennpc` 2 + 3 tệp ở gốc `settings\` = **11** | 🔴 **SAI** | **11** — **đã sửa trong thân bài** |
| 13 | §6: `great_night\风陵渡.txt` = **1520 B, 100 dòng**, toàn bộ map 336, ô **X 1370–1435, Y 3079–3152** | Đọc thật: **1520 B**, 1 tiêu đề + **100 dòng dữ liệu**, **100/100 dòng có `map_ID = 336`** ✔. Nhưng phạm vi thật là **X 1370–1522** (dòng 100 = `1521`, dòng 101 = `1522`) và **Y 2984–3152** (dòng 52 = `2984`) | 🔴 **SAI (phần phạm vi)** | “ô **X 1370–1522, Y 2984–3152**”. Phạm vi cũ chỉ đúng cho ~51 dòng đầu — **đã sửa trong thân bài** |
| 14 | §6: bản JX1 của tệp trên **“303 byte = 18 dòng”**, “18/100 dòng”, 18 dòng đầu trùng khít | `E:\…\bin\server\settings\maps\great_night\风陵渡.txt` = **303 B**, tách dòng ra **20 dòng** = 1 tiêu đề + **19 dòng dữ liệu** (không có dòng trống cuối). So từng dòng với bản Linux: **19/19 trùng khít** | 🔴 **SAI (lệch 1 dòng)** | “**303 byte = 1 tiêu đề + 19 dòng dữ liệu**, tức **19/100**, 19 dòng đó trùng khít” — **đã sửa trong thân bài** |
| 15 | §6: 3 bảng boss cận biên `baiyingyingboss.txt` (99 B, 8 điểm), `yanxiaoqianboss.txt` (99 B, 8), `herenwoboss.txt` (143 B, 12) + nội dung đầy đủ + nơi đọc `bossdown.lua:9,10,11` / `callboss_incity.lua:7,8,10` (npc 741/742/744, map 336) | Đọc byte thô: 99/8, 99/8, 143/12 ✔; **toàn bộ 28 cặp số chép đúng, đúng thứ tự**. `bossdown.lua:9` = `yanxiaoqian`, `:10` = `baiyingying`, `:11` = `herenwo` ✔. `callboss_incity.lua:7` = `[741]…336`, `:8` = `[742]…336`, `:10` = `[744]…336` ✔ | ✅ **ĐÚNG** (kể cả từng dòng số) | — |
| 16 | §3.2/§3.6: mọi trích dẫn mã Linux — `fld_head.lua:12` `boatMAPS={337,338,339}`, `:18` `npcthiefpos`, `:135-139` chia 32 + `NewWorld`, `:175-181` `fld_getadata` (`random` ở `:177`), `mission.lua:21-23` `AddNpc(724,…)` dùng nguyên giá trị, `fld_smalltimer.lua:36,43,50,59` | Đọc `src_utf8` (giữ nguyên số dòng) và đếm dòng: **tất cả khớp chính xác từng số dòng** | ✅ **ĐÚNG** | — |
| 17 | §3.4: “cả 6 hàm `bossben1A…3B` trong `boss.lua` đều bị comment ở `thuytacdaulinh()` (`boss.lua:19-24`)”; `boss.lua:28,35` = `1636*32, 3221*32` / `1632*32, 3221*32` | `boss.lua:18` `function thuytacdaulinh()`, **19-24 = 6 dòng `--bossbenXY()`**, `:25 end`; `:28` và `:35` đúng như trích | ✅ **ĐÚNG** | — |
| 18 | §3.6: `TAB_QUAIVUOTAIPLD` **30 bật + 9 comment**, X 1582–1652 / Y 3167–3249, ∩ bảng gốc = **1 ô (1621,3230)**; `NPC_PHONGLANG` **44 điểm, 7/88 chia hết 32**, ô 1572–1656 / 3167–3252, ∩ = **2 ô (1601,3167) & (1603,3224)**; `BOSS_PHONGLANG` **2 điểm, 0/4** | Parse lại 3 bảng: **30/9** ✔, phạm vi ✔, ∩ = `[(1621,3230)]` ✔; **44** ✔, **7/88** ✔, phạm vi ✔, ∩ = `[(1601,3167),(1603,3224)]` ✔; **2 điểm, 0/4**, ô (1613,3189)/(1642,3216) ✔ | ✅ **ĐÚNG** (mọi con số) | — |
| 19 | §3.6/§9: bảng JX1 nằm ở `lib_phonglangdo.lua:28-106` | `TAB_QUAIVUOTAIPLD` mở ở dòng **28**, đóng ở dòng **68**. Dòng **70-77** = `TAB_THULINH`, **79-86** = `TAB_NPCCHUCNANG`, **88-106** = `TAB_TIME_PLD` | 🔴 **SAI** | `lib_phonglangdo.lua:**28-68**`. ⚠️ Làm theo “28-106” sẽ **xoá nhầm 3 bảng khác** (boss đầu lĩnh, NPC thuyền phu, lịch giờ) — **đã sửa trong thân bài (§3.6 và §9 #1)** |
| 20 | §7: `tong\log.lua` A=1651 B=1419, B **thiếu** `WriteTongMoneyChangeLog` (A có ở `:32`), `tong_mix.lua:65,77,157` đang gọi; `tong_header.lua` A=6722/171 dòng, B=7009/187, B thêm 1211/1212/1214/1215/1216; 3 tệp `tong\*` còn lại trùng byte | Đọc thật: 1651/1419 ✔; A `log.lua:32` = `function cTongLog:WriteTongMoneyChangeLog(...)`, **B = 0 kết quả** ✔; `tong_mix.lua` gọi ở **65, 77, 157** ✔; 6722/171 và 7009/187 ✔; diff dòng cho đúng 5 hằng số **1211,1212,1214,1215,1216** (+1 dòng `--TONGTSK_VNG_CUSTOM1 = 1213` bị comment) và các chuỗi `Khai më` ✔; `contribution_entry/tong_award_head/tong_setting` MD5 bằng nhau ✔ | ✅ **ĐÚNG** (từng chi tiết) | (có thể thêm: B còn khai `1213` nhưng **đã comment**) |
| 21 | §7: trong 137 tệp closure chỉ **5 tệp** cũng có ở B, **2** lệch | Giao độc lập closure(137) × kiểm kê B: đúng **5** tệp (`contribution_entry`, `log`, `tong_award_head`, `tong_header`, `tong_setting`), **2 lệch**. (`tong_mix.lua` **cũng lệch A↔B** nhưng **không** thuộc closure ⇒ không phản bác) | ✅ **ĐÚNG** | — |
| 22 | §2.1/§4 #12: `script\lib\file.lua` **chỉ có ở A**; `settings\droprate\boss\` A=60 B=40; `activitysys\` 92/92 trùng byte 100 % | `lib\file.lua`: A=True, B=False ✔. `droprate\boss\` **A=60 B=40** ✔. `activitysys\` A=92 B=92, **cùng tập tên và mọi MD5 bằng nhau** ✔. `file.lua:38-44` đúng là `GetTabFileHeight` → `print("Load TabFileError!")` → `return 0` ✔ | ✅ **ĐÚNG** | — |
| 23 | §1: `Patch` là **cây CLIENT**: `game_y.exe` 1 083 450 B, `package.ini` `Path=\data` liệt kê **31 pak** | `game_y.exe` = **1 083 450 B** ✔; `package.ini` có `Path=\\data` (hai backslash) và **31 mục** `0..30`, nhưng `slistcache.pak` **lặp ở mục 0 và 1** ⇒ chỉ **30 pak khác nhau**; danh sách gồm `spr/ui/sound/skills/resource/maps/settings/script/font.pak` + `vauto.exe`, `spr\`, `ui\`, `music\` ở gốc | 🟡 **ĐÚNG MỘT PHẦN** (kết luận “B là cây client” **ĐÚNG chắc chắn**) | “`Path=\\data`, **31 mục ⇒ 30 pak khác nhau** (`slistcache.pak` khai 2 lần)” |
| 24 | §9 #6: “`WorldSet2.txt` đã có `336 phong lang do`, `337/338/339 thuyen PLD 1/2/3`; `bin\server\Maps\` mới chỉ có `336_srv.fp`” | `WorldSet2.txt` **không nằm ở phía máy chủ** — nó là `bin\client\maps2\WorldSet2.txt`, dòng **29, 78, 79, 80** ✔. Phía máy chủ khai ở `bin\server\Maps\WorldSet_GameServer.ini:321-324` (`World316=336 … World319=339`) ✔. `bin\server\Maps\` có 93 tệp, dải 33x **chỉ có `336_srv.fp`** ✔ | 🟡 **ĐÚNG MỘT PHẦN** (thiếu đường dẫn ⇒ dễ hiểu nhầm là tệp máy chủ) | Ghi rõ 2 nguồn: **client** `bin\client\maps2\WorldSet2.txt:29,78-80` và **server** `bin\server\Maps\WorldSet_GameServer.ini:321-324`. Xem thêm “Bỏ sót #4” |

### B. Bỏ sót của chính vòng 2

**Bỏ sót #1 — §4 rà 14 khẳng định của vòng 1 nhưng *bỏ trắng đúng 2 câu bị chính dữ liệu mới bác bỏ*.**
Cả hai nằm ngay trong đoạn CHẶN #1 mà vòng 2 đang lật:

| Câu của vòng 1 | Sự thật đo được | Vì sao quan trọng |
|---|---|---|
| `05_dulieu.md:544` — “Map 337/338/339 **chỉ có 4 region** ⇒ **vùng hợp lệ rất nhỏ**, phải kiểm bằng `.wor`/`Region_S.dat` thật” | Bản đồ `渡船` có **21 region** trên đĩa client, và 63 điểm gốc **trải trên 15 region** | Đây là căn cứ khiến vòng 1 tin “phải soạn tay một bảng nhỏ”. Vòng 2 có sẵn số liệu bác bỏ (§3.3) nhưng **không đưa vào §4** ⇒ con số “4 region” vẫn còn nguyên trong `05` và sẽ tiếp tục dẫn người port đi sai |
| `05_dulieu.md:541` — “vùng chơi nằm quanh ô **(1630…1650, 3220…3235)**” | Vùng thật: ô **X 1575–1661, Y 3162–3253** — rộng gấp ~4 lần mỗi chiều, và **59/63 điểm nằm ngoài** khung đoán đó (chỉ 4 điểm lọt vào) | §4 #8 chỉ rà ước lượng ở `02_phonglangdo.md:658` (đúng) mà **bỏ qua** ước lượng hẹp hơn nhiều ở `05_dulieu.md:541` (sai) |

**Bỏ sót #2 — bảng boss `TAB_THULINH` của JX1 chưa hề được đối chiếu.**
Vòng 2 kết luận “thay hẳn `TAB_QUAIVUOTAIPLD`”, nhưng **không nhắc một chữ nào** tới `TAB_THULINH`
(`lib_phonglangdo.lua:70-77`, 6 điểm) — trong khi ở bản Linux **cả 3 lượt Thuỷ tặc đầu lĩnh (725) lẫn
2 Thuỷ Tặc Đại Đầu Lĩnh (1692) đều lấy toạ độ từ *cùng* `渡船刷怪点.txt`**
(`fld_smalltimer.lua:36,43,50,59` — chính §3.2 của báo cáo đã liệt kê).
Đo thật: `TAB_THULINH` có **0/12 giá trị chia hết 32** và chỉ trùng bảng gốc **1/63 ô** (`(1589,3196)`)
⇒ cũng là **số soạn tay**, y hệt lý do vòng 2 đòi thay `TAB_QUAIVUOTAIPLD`.
⇒ Đã bổ sung việc **1b** vào §9.

**Bỏ sót #3 — port bảng toạ độ *không đủ* để khôi phục bản gốc: JX1 chỉ sinh quái trên map 337.**
`lib_phonglangdo.lua:163` gọi `AddNpcEx3(…, **337**, …)`, còn hai nhánh map **338** (`:168`) và
**339** (`:173`) **đang bị comment**; tương tự với boss ở `:208` (337) / `:215` (338) / `:222` (339).
Bản Linux thì mỗi bản đồ thuyền chạy **một mission riêng** và sinh 30 Thuỷ tặc theo `SubWorld` của
mission đó (`mission.lua:20-24`, `fld_head.lua:12 boatMAPS={337,338,339}`).
⇒ Nếu chỉ thay bảng toạ độ như §9 #1, **thuyền 2 và thuyền 3 vẫn trống**. Đã bổ sung việc **1c**.

**Bỏ sót #4 — §9 #6 nêu lại chuyện `_srv.fp` mà vòng 1 đã đóng, và không dùng bằng chứng mạnh hơn sẵn có.**
Vòng 1 đã kết luận rõ ở `05_dulieu.md:579`: “❌ ~~Thiếu `_srv.fp`~~ — chỉ là **cache A\*** cho bot,
engine tự sinh (`KSubWorld.cpp:1928`)”, và `map_can_them.csv` của chính vòng 1 ghi cho 337/338/339:
`srv_fp_cache = "-(cache A*, tu sinh)"`, `wor_trong_pak_JX1_server = maps.pak`,
`wor_trong_pak_JX1_client = update01.pak`. Nêu lại “`bin\server\Maps\` mới chỉ có `336_srv.fp`” mà
không dẫn kết luận đó **dễ bị đọc thành một điểm chặn mới**.
Ngược lại, cùng tệp `map_client_pak_check.csv` của vòng 1 lại là **bằng chứng độc lập mạnh hơn** cho
chính §3.3: nó chứng minh **337, 338, 339 đều trỏ tới `中原北区\渡船`** — tức bản đồ mà vòng 2 dùng để
kiểm 63 điểm **đúng là** bản đồ thuyền, không phải trùng tên. Vòng 2 tự dựng phép thử region mà
**không dùng** mối nối này ⇒ mất một tầng xác nhận rẻ tiền.

### C. Những chỗ đã kiểm và **không** bắt được lỗi (để khỏi kiểm lại)

- Toàn bộ **63 cặp số** của §3.5 (cả khối nguyên văn lẫn bảng markdown, cả cột `ô`): **chép đúng 100 %**.
- Toàn bộ **28 cặp số** của 3 bảng boss §6: **chép đúng 100 %**, đúng thứ tự dòng.
- Mọi con số kiểm kê A/B ở §2 (2388/1754/467/4 · 5145/634/327/55 · 747/52/0): **tái lập chính xác**.
- Mọi con số ở §5 (106 / 72 / 34) và tính chất “72 dòng đó trùng byte”: **tái lập chính xác**.
- Mọi trích dẫn `tệp:dòng` phía Linux (`fld_head`, `mission`, `fld_smalltimer`, `boss`, `log`,
  `tong_header`, `tong_mix`, `file.lua`, `bossdown`, `callboss_incity`): **khớp từng số dòng**.
- Kết luận **“B là cây client”** và **“cấm chép đè nguyên thư mục B lên A”**: **vững**.

*Đối chất — chỉ đọc, không sửa mã nguồn. Mọi số liệu ở đây đều tự đo lại từ tệp gốc/nhị phân;
chỗ nào không đo được đã ghi “ĐÚNG MỘT PHẦN” kèm lý do.*
