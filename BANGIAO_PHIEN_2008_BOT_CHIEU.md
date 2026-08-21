# BÀN GIAO PHIÊN 20/08 (09:30 → 19:20) — HỆ BOT KPlayer

> **ĐỌC TỆP NÀY TRƯỚC KHI GÕ dòng nào cho hệ bot.**
> Phiên trước: `BANGIAO_PHIEN_2008_BOT.md` (sáng 20/08) → `BANGIAO_PHIEN_1908_CHIEU.md` → `BANGIAO_PHIEN_1808.md`.
> Phiên này gồm **5 đợt vá** (`91bd422d` → `b91831f5`), tất cả build xanh + push + đã chạy thật.

---

## 1 · TRẠNG THÁI LÚC BÀN GIAO (20/08 19:20)

| Thứ | Trạng thái |
|---|---|
| Commit của phiên này | `91bd422d` (11:12) · `aad1e03c` (11:40) · `8a3c882b` (12:00) · `20a84d0d` (16:19) · `b91831f5` (18:18) — **đã push main** |
| `CoreServer.dll` đang chạy | build **18:41** của phiên song song (cây dùng chung) — **đã kiểm marker: có đủ cả 5 đợt bot** (`di bo lach sang`, `NHAY (van cuoi)`, `BotNgoai`, `bot_bai_go`) |
| GameServer | **ĐANG CHẠY** từ 18:33, ổn định |
| Bản lùi DLL | `.bak_2008_1620` (15:39, bản trước waypoint+BotNgoai) · `.bak_2008_truoc_waypoint` (18:01) · `.giu_1619_codoBotNgoai` (16:19) |
| **Đổi CẤU HÌNH cây E (KHÔNG nằm trong repo)** | `Pak\maps_client.pak` (bản sao `maps_error.pak` = `maps.pak` của client) + dòng `5=maps_client.pak` trong `package.ini`. Đã lưu `package.ini.bak_2008`. **Gỡ dòng đó là về nguyên trạng, không cần build.** |
| Lua LIVE đã sửa | `script/global/station.lua`: thêm bảng `BOT_BAI` (39 dòng) + hàm `bot_bai_go`. **Đã mirror `serverscript_jx2/jx1_edits/station.lua`** (giữ luật 2 bản giống hệt) |
| kMagic lưới A* | **05** (`0x53465005`). Cache `Maps\*_srv.fp` đã tính lại lúc 16:04. Đổi tiếp kMagic ⇒ boot chậm thêm ~60 giây |

### Số đo thật (bot.log, 1000 bot)

| Chỉ số | Sáng 20/08 (trước phiên) | Sau phiên |
|---|---|---|
| Bot **ngoài cửa sổ lưới** | không đo được | **0** — mọi mẫu, suốt nhiều giờ |
| Bot trong ô lưới chặn | không đo được | **2-4 / 1000** (0,2-0,4%), thoáng qua |
| `[BotCuu]` (bot kẹt phải cứu) | 40 lượt / 5 phút | **2 lượt / 9 phút** |
| `[BotA*]` lỗi tìm đường | 37 | **3** |
| `[BotHoang]` thất bại | 277/838 = 33% | 128/840 = **15%** |
| Bãi mốc 80 | 4 map lẫn lộn 70+80, 116-134 con | **4 map đúng mốc 80**: 320(138) 224(137) 181(134) 198(130) |
| `KHONG VAO DUOC` (bãi hỏng) | — | **0** (cả 39 bãi vào được) |
| Lách đám đông: teleport / tổng | 100% teleport | **852 nhảy / 2790 lượt = 30,5%** |
| Nhịp bot | 564-625 ms/10s | 650-880 ms/10s (~11 ms trong ngân sách 55 ms/khung — **không quá tải**) |

---

## 2 · NĂM ĐỢT VÁ

| Commit | Nội dung |
|---|---|
| `91bd422d` | **Sửa GỐC lưới A\*** (tách `bCoTep`/`bCoObs`, kMagic 04) + `pb_CatDoan` chặn chạy xuyên map + bỏ follow khi đang ra bãi + lách chọn ô gần đích + `nRaBaiThu` đếm theo giây |
| `aad1e03c` | **Bảng 39 bãi chính thống** theo đúng mốc cấp + `BOT_BAI`/`bot_bai_go` trong Lua + **tản đàn theo toạ độ NPC đã add** (`pb_LayCum`) + 15 lỗi phản biện |
| `8a3c882b` | **Nạp lưới vật cản từ pak CLIENT** cho vùng server thiếu (kMagic 05) + `[BotCuu]` nhánh T1b "ngoài hạn cửa sổ lưới" |
| `20a84d0d` | **Bộ đếm `[BotNgoai]`** đo THẬT theo từng map + cứu bot ngoài lưới trên MỌI bản đồ + `map=` cho `[BotLach]` |
| `b91831f5` | **Đóng băng waypoint** + **lách đi bộ thay `SetPos`** + 5 lỗi phản biện |

---

## 3 · TRI THỨC ĐẮT TIỀN TÌM RA PHIÊN NÀY

### 3.1 🔴🔴 Luật 19/08 "region thiếu dữ liệu vật cản = VẬT CẢN" **sơn đặc cả đất trống CÓ THẬT**

Tệp `%03d_Region_S.dat` gói dữ liệu region theo dạng combin. **Đoạn vật cản là `int[16][32]` = 2048 byte, nên tệp CÓ vật cản luôn ≥ 2100 byte.** Luật 19/08 gộp hai ca khác hẳn nhau:

- **không mở được tệp nào** → region ngoài bản đồ → chặn là ĐÚNG
- **mở được tệp mà tệp không có đoạn vật cản** (149-308 byte) → region **CÓ THẬT, chỉ là đất trống** → chặn là SAI

Đo thật: **không một region nào trong 8 map đã kiểm bị chặn bởi dữ liệu vật cản THẬT** — tất cả đều do luật này. Bị sơn oan: map 319: 45 region, 320: 25, map 1: 7, map 53: 4, map 11: 3.

Và đúng hai ô rơi vào đó là **điểm đáp CHÍNH THỐNG của script Xa Phu**: Lâm Du Quan `319 (1630,3592)`, Chân núi Trường Bạch `320 (1146,3130)` — người chơi thật cũng đáp xuống đấy. Bot đáp xuống = `FindPathServer` trả **−4 (xuất phát kẹt)** vĩnh viễn → đứng im → `[BotCuu]` gom cả đám về một chỗ.

**Fix**: tách `bCoTep` (mở được tệp) khỏi `bCoObs` (đọc được đoạn vật cản).

### 3.2 🔴🔴 Bảng bãi 19/08 chép NHẦM NGUỒN — mốc 80 có 4 map chứ không phải 2

`TRAIN_ARRAY1/2` trong `shenxingfu.lua` **chỉ là menu phụ "luyện công tân thủ"** (2 map/bậc). Bảng THẬT người chơi dùng là **`tab_lv20map … tab_lv90map`** trong CÙNG tệp (d.433-679), qua menu `chondiadiem1` → *"Bản đồ luyện công từ 20 đến 90"*:

```
mốc 20: 2 map | 30: 2 | 40: 2 | 50: 2 | 60: 3 | 70: 3 | 80: 4 | 90: 13
```

`s_bai` mới = **hợp hai nguồn = 39 bãi** (10:1 20:3 30:4 40:4 50:4 60:3 70:3 **80:4** 90:13), đã kiểm 39/39 toạ độ đáp được. **Bỏ mảng `s_baiLc`**, thay bằng macro `PB_BAI_LUA(i) = i+1`.

> ⚠️ **`s_bai` (C++) và `BOT_BAI` (station.lua) PHẢI khớp thứ tự tuyệt đối.** Lệch một dòng là bot đi sai map. Có script đối chiếu máy trong scratchpad phiên này — nên viết lại nếu sửa bảng.

### 3.3 🔑 Toạ độ "NPC đã add trong map" = `KNpc::m_OriginX / m_OriginY`

KNpc.h:402, public, đơn vị MPS tuyệt đối. Đặt **đúng một lần** ở `KNpcSet.cpp:515 AddNpcSet1` từ `Npc_S.dat`, và là **chính toạ độ engine dùng để HỒI SINH quái** (`KNpc::Revive` KNpc.cpp:8392) + kéo quái về khi lạc (`KNpcAI::KeepActiveRange` KNpcAI.cpp:1182).

- **KHÁC HẲN `GetMpsPos()`** — quái lang thang quanh điểm gốc mỗi nhịp AI (`KNpcAI::CommonAction`). Lấy vị trí hiện tại làm đích roam = bot "di chuyển bậy".
- Quét phải bao **cả `m_Region[].m_NpcList` LẪN `m_NoneRegionNpcList`** — quái **đang chết** nằm ở danh sách thứ hai (`KNpc::DoRevive` đẩy sang VOID_REGION), điểm sinh vẫn nguyên.
- `KNpc::ChangeWorld` **ghi đè** `m_OriginX` — quái bị script đẩy sang map khác thì mất điểm sinh gốc.

### 3.4 🔴 Client có `_Region_C.dat` vật cản thật cho vùng server thiếu

Đo: **743 region trên 10 map**. Bản sao `maps.pak` của client **đã nằm sẵn** ở `bin\server\Pak\maps_error.pak` (md5 trùng tuyệt đối), chỉ chưa khai vào `package.ini`.

🔑 **Nạp nó CHỈ đổi lưới bot, KHÔNG đổi va chạm người chơi**: `KRegion::LoadObject` (đường va chạm thật) chỉ lùi về `%03d_OBSTACLE.DAT`, **không bao giờ đọc `_Region_C.dat`**; còn `ProcLoadPathGrid` thì có. Và pak client có **đúng 0** tệp `_OBSTACLE.DAT`. Kết quả thật: "region KHONG co TEP" tụt map 11 754→613, map 21 2396→2110, map 19 438→309.

⚠️ **Câu hỏi còn mở**: pak client mở thêm cả region **rìa** mà server không hề có tệp (map 319 hàng r118/r119 đáy bản đồ: 51 region từ chặn hết → mở trắng). Phân biệt: region "đất trống có thật" trong lòng map **có `_Region_S.dat` stub của server**; region rìa **chỉ có bản client**. Đo log: mới 1 bot đứng ở vùng client-only ⇒ chưa phải nguyên nhân chính. Nếu cần siết, luật đúng là: **`_Region_S.dat` của server mới định nghĩa map có region đó; `_Region_C.dat` client chỉ được cấp DỮ LIỆU vật cản, không được "khai sinh" region.**

### 3.5 🔴 "Đứng đông bot thì FPS không giảm mà di chuyển khựng" = **bão gói tin**, không phải server yếu

Đo: nhịp bot 650-880 ms/10s, engine 1200 ms/10s ⇒ ~11 ms trong ngân sách 55 ms/khung. **Server thừa 80%.**

Hai triệu chứng, hai nguyên nhân:
- **"khựng 1 nhịp rồi mới đi tiếp"** = `PB_WalkTo` tính đích bằng `BlockNearestMps` **từ vị trí bot mỗi nhịp** ⇒ đích **trượt theo bot**. Bot chạy **18-20 MPS/khung (~340 MPS/giây)** ⇒ ~7 gói `do_run`/giây/bot. Client nhận `s2c_npcwalk` là **dựng lại đường đi của NPC đó từ đầu**. Đứng giữa 130 bot ≈ **900 gói/giây**. → **Đóng băng waypoint** (`wpX/wpY`, chốt 1 lần mỗi block).
- **"tốc biến tới một đoạn"** = teleport thật. `[BotLach]` gọi `SetPos`, quãng nhảy **trung bình 6,4 ô = 205 MPS**, dồn đúng chỗ đông. → phát `do_run` đi bộ; **giữ `SetPos` làm ván cuối** cứ 2 lần đi bộ mới 1 lần nhảy.

---

## 4 · CẠM BẪY ĐÃ MẮC / SUÝT MẮC (đừng lặp lại)

### 4.1 🔴🔴 BẪY ĐO ĐẠC — mất 2 vòng vì nó

Các dòng log sẵn có (`[BotLach]`, `[BotKet]`, `[BotDanh]`) **KHÔNG in bản đồ**. Khi đối chiếu vị trí bot với lưới tôi đoán map theo dòng gần nhất có ghi map, và **cách đoán đó SAI**: ba bot đứng CHUNG một toạ độ `o(3537,6231)` bị gán ba map khác nhau. Tôi đã báo "29 bot trong ô chặn / 23 bot ngoài lưới" — **toàn bộ là rác**. Số thật sau khi có bộ đếm: **0 ngoài lưới**.

> **Log chẩn đoán PHẢI mang map id. Muốn sửa gốc thì ĐO, đừng suy luận từ dữ liệu đoán.**

### 4.2 🔴🔴 QUY TRÁCH NHIỆM CRASH TRONG CÂY DÙNG CHUNG

`D:\GAMEDEVNEW` dùng chung với phiên khác (hôm nay: MySQL + auto Dã Tẩu). Build của tôi lúc 16:19 là bản **đầu tiên** mang commit `b8d363bd` (15:44, MySQL đợt 5 cắm `KGameKV::GetFileToDisk` vào **`g_InitCore`**) ra chạy thật ⇒ GameServer **boot lặp 12 lần, chết ~55 giây sau khi nạp xong lưới bản đồ cuối**.

🔑 **Cách quy trách nhiệm**: tôi có `[BotNgoai]` chạy **ngay lần gọi `PB_Breathe` đầu tiên**. `grep -ac "BotNgoai" bot.log` = **0** ⇒ vòng game chưa hề quay ⇒ **toàn bộ code bot không thể là thủ phạm**. **Luôn để một dấu log chạy sớm trong đợt vá của mình.**

Kèm: `mysql_core.log` là log MỚI, ghi **SAU** cả `bot.log` — **luôn đọc nó khi chẩn đoán boot**. Và `git log --date=format:"%H:%M"` cho thấy ngay phiên kia vừa commit gì.

### 4.3 🔴 Bốn lỗi "tự bắn vào chân" phản biện bắt được

Ba vòng phản biện độc lập bắt **13 + 46 + 5 = 64 điểm**. Bốn cái sẽ làm **TỆ HƠN bản cũ**:

| Lỗi | Vì sao chết người |
|---|---|
| Đóng băng waypoint **mà giữ ngưỡng "lệch > 48 MPS mới phát"** | hai block A* kề nhau **dùng chung một cạnh**, rect lại **thụt 16 MPS**, nên waypoint mới thường chỉ cách waypoint cũ **32-64 MPS — đúng ngay trên ngưỡng 48**. Rơi dưới 48 là **lệnh DUY NHẤT của chặng bị nuốt** ⇒ **tự tạo ra đúng cái "khựng 1 nhịp" đang đi chữa, ở MỌI ranh block** |
| Lệnh lách đi bộ **bị ghi đè ngay trong CÙNG NHỊP** | khối lách **không `return`** và còn `Reset` cả 5 lộ trình ⇒ `PB_WalkTo` của nhánh kế tính A* mới rồi phát `do_run` khác đè lên (**`m_Command` chỉ có MỘT khe**). Bot kẹt phải chờ 24 giây thay vì 8 |
| `pb_CatDoan` trả 0 → `PB_WalkTo` báo thua sau 1 giây | ngưỡng đó **trị vì TRƯỚC** B5a (3 giây, đánh thức bot bị `m_ProcessAI=0` nuốt lệnh) và B5b (tìm lại đường **có tính NPC làm vật cản**) — hai thứ chữa bệnh kẹt thật |
| Bãi KHÔNG VÀO ĐƯỢC thành **hố đen** | nó luôn có 0 bot ⇒ luôn thắng phép "ít bot nhất" ⇒ cả mốc cấp kẹt vòng lặp sống |

Các lỗi khác đáng nhớ: `pb_CatDoan` **không loại trừ ô bot đang đứng** (bước mẫu 16 MPS = nửa ô nên mẫu đầu vẫn trong ô đó) ⇒ bot đứng trên ô bị sơn sẽ **tê liệt vĩnh viễn**; `CellObsSrv` trả −1 cho **cả** "chưa nạp lưới" **lẫn** "ngoài cửa sổ lưới" ⇒ phải hỏi `CoLuoiSrv()` chứ không hỏi ô của bot; lọc cụm bằng `NpcSet.GetRelation` rồi **chốt vĩnh viễn** ⇒ một bot **tà phái** hỏi trước là cả map bị đánh dấu "không có quái" mãi mãi (`GenOneRelation` xét **camp TRƯỚC kind**) — phải lọc bằng `m_Kind == kind_normal`; `new` + `if (!p)` là **mã chết** (MSVC ném `bad_alloc`) ⇒ dùng mảng tĩnh; nhánh **tự hồi sinh** chỉ `Reset` `b.walk` ⇒ `b.chase` giữ waypoint **chốt từ chỗ chết**.

### 4.4 Cạm bẫy vận hành (nhắc lại)

- Build: `MSBuild Core.vcxproj -p:Configuration="Server Release" -p:Platform=x64 -p:SolutionDir="D:\GAMEDEVNEW\Sources\"`. **Phải build CẢ `Client Release|Win32`** — `KSubWorld.cpp/.h` là tệp dùng chung.
- `KPlayerBot.cpp/.h` **ASCII/LF** (Edit tool OK). `KSubWorld.cpp/.h`, `station.lua` = **TCVN3 → `safe_edit.py` BẮT BUỘC**. Kiểm `check_encoding.py` trước+sau, high-byte phải không đổi (KSubWorld.cpp 198, KSubWorld.h 68, station.lua 384).
- Đoạn nhiều dòng có tab/CRLF: **sinh old.txt/new.txt bằng python trích byte nguyên văn từ chính tệp nguồn**, rồi `safe_edit --old-file/--new-file`. Đừng dùng heredoc — **Bash tool rút `\\` thành `\`**.
- Deploy = **rename bản cũ** rồi copy + md5 + **grep chuỗi mới trong DLL**.
- Thêm trường vào `PB_Bot` thì **BẮT BUỘC khởi tạo ở khối tái dùng khe** (~dòng 960-1010).

---

## 5 · VIỆC PHIÊN SAU

### 5.1 Nghiệm thu (server đang chạy, chỉ cần đọc log)

```bash
cd "E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
grep -a "\[BotNgoai\] tk:" bot.log | tail -20      # NGOAI cua so luoi phai = 0
grep -a "\[BotLach\]" bot.log | grep -oa "di bo lach sang\|NHAY (van cuoi)" | sort | uniq -c
grep -a "\[BotCum\]" bot.log | tail -30            # moi map bai mot dong, khong map nao rong
grep -ac "KHONG VAO DUOC" bot.log                  # phai = 0
grep -a "\[BotBai\]" bot.log | grep -o "(map [0-9]*" | sort | uniq -c | sort -rn
grep -a "\[BotPerf\]\|\[SvPerf\]" bot.log | tail -6
```

### 5.2 Việc còn dang dở (theo thứ tự giá trị)

1. **Bot kẹt trong ô lưới chặn trên map KHÔNG PHẢI bãi** — 25/465 lượt `[BotLach]` cho thấy bot đứng trong ô chặn, dồn ở **map 80, 78, 181, 20** tại vài toạ độ cố định (map 80 `o(1739,2982)` lặp 4 lần). Đó là **map nhiệm vụ / thành Dã Tẩu**, mà nhánh `[BotCuu]` "kẹt trong ô chặn" **chỉ chạy khi bot đứng đúng map bãi** (cần điểm neo để thả về). **Sửa: mở nhánh đó ra mọi bản đồ, đúng khuôn đã làm cho nhánh "ngoài lưới"** (quét xoắn ốc, lấy ô đầu tiên mà cả lưới lẫn engine đều báo đi được). ~20 dòng.
2. **Nếu vẫn còn khựng**: gắn bộ đếm gói `do_run` (số lần `SendCommand(do_run)` /10 giây, in cạnh `[BotPerf]`) để **đo trực tiếp** thay vì ước lượng 7 gói/giây/bot.
3. **Region rìa do pak client mở** (mục 3.4) — siết nếu chủ game thấy bot ra rìa đáy bản đồ.
4. `BO TAY` (teleport dự phòng khi không tới được Xa Phu) còn ~20 lượt/9 phút. Chủ game đã cấm "bay tự chế" nên nên truy nốt.
5. **Mốc 10 chỉ có 1 bãi (Hoa Sơn)** ⇒ bot cấp 10-19 vẫn dồn một map. Đó là thiết kế của chính game; muốn tản phải thêm map ngoài bảng chính thống — **cần chủ game quyết**.
6. `pb_TimNpcNho` cache **−1 vĩnh viễn** cho map không tìm thấy NPC (có sẵn từ đợt Dã Tẩu, chưa đụng).
7. `pb_CumGan` so toạ độ gốc THÔ với tâm cụm ĐÃ nhích ⇒ ngưỡng 1280 thực tế lệch ±128 MPS (vô hại).
8. `PB_BAI_TRAN 120` giờ vô dụng (giữ làm mốc đọc log).

### 5.3 Nợ cũ chưa đụng (từ phiên trước)

- 3 dòng Sa Mạc hỏng của Thần Hành Phù cho NGƯỜI CHƠI (`shenxingfu.lua` TRAIN_ARRAY2 waypoint 226/227/228).
- Bot nhiễm skill Thiên Vương lưu trong khe 18/08 không tự sạch.
- `KNpc.cpp:8241` `pnMagicLevel[6]` đọc ngoài biên (đường rớt đồ QUÁI — ảnh hưởng người thật).
- Nợ engine: `Mps2Map` chia số âm, `ServeJump` trôi offset.
- Goddess `RemoveLogProc` checkpoint 1h→10p đã sửa nguồn, chưa build/deploy.

---

## 6 · CÔNG CỤ TỰ DỰNG PHIÊN NÀY (viết lại nếu cần)

Nằm ở scratchpad phiên `199e9b59`, **nên chép vào `ReverseTools/` nếu dùng lại**:

| Script | Việc |
|---|---|
| `fpdump.py` / `reg2.py` | vẽ lưới A* từ cache `Maps\<id>_srv.fp` (header `SFP`+ver2+`int[7]`+ver+size, node 20 byte, `obs` ở offset 16) |
| `paklook.py` / `pak4.py` | tra `maps.pak` bằng `FileNameToId` — **không cần build, không cần unpack** |
| `checkbai.py` / `vethanh.py` | kiểm mọi toạ độ đáp (39 bãi + 15 điểm về thành) trên lưới |
| `khop.py` | **đối chiếu máy `s_bai` (C++) với `BOT_BAI` (Lua)** — lệch một dòng là bot đi sai map |
| `vitri3.py` | phân loại vị trí bot từ log (cần `[BotLach]` có `map=`) |

**Công thức `FileNameToId`** (KPakList.cpp:72) — tên đã hạ chữ hoa ASCII, dạng `\maps\<đường-dẫn-MapList.ini>\v_%03d\%03d_Region_S.dat`:

```
id = 0; i = 0
for c in name:  i++;  id = ((id + i*(signed char)c) & 0xffffffff) % 0x8000000b * 0xffffffef   (mod 2^32)
return id ^ 0x12345678
```

Đường dẫn map lấy từ `settings\MapList.ini` khoá `<id>=` (**bytes GBK, giữ nguyên byte, đừng decode**). `package.ini` cho biết pak nào thật sự được nạp.
