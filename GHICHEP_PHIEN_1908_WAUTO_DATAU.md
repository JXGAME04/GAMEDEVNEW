# GHI CHÉP PHIÊN 19/08/2026 — AUTO DÃ TẨU (WAuto/client) + LỆNH BÀI ADMIN

> Soạn theo yêu cầu "note toàn bộ lại cho tôi". Đây là **nhật ký phiên**: mỗi việc ghi
> *triệu chứng anh báo → nguyên nhân THẬT (kèm chỗ trong mã) → cách sửa → commit*.
>
> Tài liệu tra cứu chi tiết vẫn là `BANGIAO_AUTO_DATAU_WAUTO.md` (mục 7.2, 8.6-8.8, 12) và
> `THAMCHIEU_AUTO_DATAU.md`. Phần hệ **bot KPlayer** do phiên khác làm, ghi ở
> `BANGIAO_PHIEN_1908.md` — **không lẫn với tệp này**.

---

## 1 · Trạng thái cuối phiên (đọc trước khi test)

| Tệp | Đường dẫn | Giờ | Ghi chú |
|---|---|---|---|
| `CoreClient.dll` | `E:\SourceTuanLe\...\bin\client\` | **19:11** | engine Dã Tẩu tới `e40f40ea` (mục 2.9-2.10); bản lùi `CoreClient_cu_1908_tg.dll` (17:50) |
| `Game.exe` | như trên | **19:11** | thêm `GDCNI_UI_ACT uParam=7` (AutoPick cửa sổ thưởng); bản lùi `Game_cu_1908_tg.exe` (06:49) |
| `settings\datau_toado.txt` | `...\bin\client\settings\` | **17:10** | 80 KB — bảng tọa độ quái 204 map, engine nạp **lúc chạy** |
| `WAuto.exe` | **`E:\Src_Auto_Ngoai\`** (gốc) | **11:39** | UI mới; ⚠️ post-build gọi `pwsh.exe` không có trên máy ⇒ **luôn chép tay** ra gốc |
| `script\item\datau_admin.lua` | `...\bin\server\script\item\` | **17:59** | công cụ Dã Tẩu trong lệnh bài admin |

- `re_pe_crt.py` **PASS** mọi lần deploy (CoreClient = CRT-tĩnh, Game.exe = UCRT-RELEASE,
  engine/Represent2 = UCRT-DEBUG).
- **Client**: đóng game mở lại là ăn bản mới (WAuto không phải mở lại nếu chỉ DLL đổi).
- **Server**: sửa Lua **không cần restart** — `Include` = `lua_dofile` không cache
  (`ScriptFuns.cpp:1964`) và `main()` của lệnh bài tự `dofile` lại chính nó mỗi lần dùng.
- ⚠️ **GameServer.exe đang TẮT** suốt phiên ⇒ phần lệnh bài **chưa chạy thử lần nào**.

---

## 2 · Mười việc đã làm, theo thứ tự anh yêu cầu

### 2.1 "Auto vẫn không tự làm dã tẩu, chỉ đứng yên" → `32e60788`
`KSubWorld::HaveTarget(int& x, int& y)` là hàm **XUẤT** target hiện tại ra tham số, không
phải hàm kiểm tra. `DT_WalkTo` truyền thẳng đích vào ⇒ đích bị ghi đè thành `(0,0)` ⇒
`FindPath(0,0)` bị chặn (`nX<=0` → -1) ⇒ **mọi pha di chuyển tê liệt**, in đúng một dòng
`[DaTau] uu tien...` rồi đứng im, không watchdog nào nổ. Sửa theo khuôn `ATYPE_MOVE`
nhánh `bAroundPoint` (hứng target ra biến nháp, truyền đích riêng vào FindPath).
Kèm `0d3c6629`: bọc `#ifdef _SERVER` quanh 2 lệnh `PB_LogNgoai` làm hỏng build client.

### 2.2 Ba việc sau khi auto chạy được → `34c79152`
- **Nhận nhiệm vụ xong đóng khung thoại**: thêm `CoreDataChanged(GDCNI_UI_ACT,1,0)`.
- **Loại 4 tự chạy tìm quái**: neo bám vị trí hiện tại + quét NpcSet tìm quái ngoài tầm +
  đảo 8 hướng (về sau thay bằng bảng cụm quái thật, mục 2.5).
- **Túi đầy → thả máy cho Hậu cần bán rác** (`DTP_SELLJUNK`), ≥8 ô trống tự quay lại.
  Cơ chế nền: `m_FightMode` là trạng thái "đang ở bãi" do **server** quản.

### 2.3 "Thiết kế lại WAuto cho gọn, có note từng tính năng" → `0c97e240`
Lưới nút tab 3×3, mỗi tab có khung tiêu đề + 14 kẻ phân mục, **~190 tooltip tiếng Việt
giải thích từng ô**, dialog 160×354, sửa 2 lỗi hình học cũ. Nguồn UI **lần đầu được đưa
vào git** (`WAutoUI/`, `e999d591`).

### 2.4 Năm lỗi khi test thật buổi chiều → `f6f550c5` + `6bde16a1`
| Anh báo | Nguyên nhân thật | Sửa |
|---|---|---|
| Bảng thưởng hiện mà không chọn | `seasonnpc.lua` hàm `Prise`: **một** cửa sổ, 3 thưởng gán vào 3 nút; bấm nút **không được ánh xạ** thì `Prise_Chon` **mở lại cửa sổ**, bấm nhầm nhóm thì server dùng reward-id nhóm kia (=0) | `DTP_REWARD` **xoay nút**: nút anh chọn → mở lại thì nút kế → hết nhóm này sang nhóm kia → quá 6 lần bỏ qua; bắt `uFinSeq` ở **mọi pha** |
| Chọn "Hủy" thì hủy luôn loại đang bật | `DT_Skip` hủy vô điều kiện | chỉ hủy khi loại **bị TẮT**; loại đang bật mà kẹt thì treo |
| Chưa xong đã tự phù về | (a) id map đọc từ **văn bản** nhiệm vụ lệch với map Xa Phu thả (map nhiều tầng); (b) `DTP_IDLE` sau mỗi lần hết treo coi map nhiệm vụ là "không ở thành" | (a) **tin máy chủ**: Xa Phu thả ra khỏi thành = đúng map; (b) IDLE đang giữ loại 4 đúng map thì vào thẳng FARM |
| Đi trả nhiệm vụ phải lên ngựa | `DT_WalkTo` không lên ngựa | thêm `DT_Ride` (khuôn `case PA_RIDE`) cho mọi chặng đi đường, **trừ** lúc farm |
| Lên nhầm map | map lạ không có Xa Phu → `DT_Skip` bỏ luôn nhiệm vụ | phù về thành (`nDTBackXaFu`) rồi ra **thẳng Xa Phu** |

### 2.5 "Quét NPC trong map nhiệm vụ bị sai" → `b6049e03` → `15b242c3` → `4c264409` → `16de2b3a`
Client **chỉ thấy NPC đã đồng bộ** (~2 màn hình); chỗ Xa Phu thả thường không có con quái
nào trong tầm đó. Giải pháp: bóc **file add NPC của server trong pak**
(`\maps\<map>\v_NNN\NNN_Region_S.dat`) → sinh bảng tọa độ cụm quái.

Bộ sinh `ReverseTools/gen_datau_spots.py` tự cài lại 3 thứ bằng Python: định dạng pak
`PACK`, hàm băm tên tệp `KPakList::FileNameToId`, giải nén `ucl_nrv2b_decompress_8`.

**Kết quả cuối: 204 map · 63.301 quái · 3.365 dòng cụm · 153 map có quái.**
Xuất ra `bin\client\settings\datau_toado.txt` — **engine nạp lúc chạy**, sửa tệp là đổi
được chỗ đánh quái, **không cần dựng lại DLL**. `KDaTauSpots.h` chỉ còn là bản dự phòng.

**Năm cạm bẫy đã trả giá** (ghi kỹ ở bàn giao 7.2):
1. chỉ số region **cộng offset** → phải quét 0-383 (0-255 mất 34% quái của map 25);
2. hàm băm chỉ 31 bit → đụng id tệp khác; lọc bằng **NPC phải nằm trong ô region của
   chính nó** — trước khi lọc **33 map toàn tọa độ bịa**;
3. chỉ đọc pak có trong `bin\server\package.ini` (`maps_error.pak` server **không nạp**);
4. **đừng** lọc region theo cỡ ≤2100 byte (mất 16 quái ở map 1);
5. **neo cụm vào vị trí quái THẬT**, tâm trung bình rơi vào **ô vật cản 10,1%** (map 53:
   21%) mà engine chỉ né vật cản **một lần** mỗi lệnh đi.

### 2.6 "Đã tìm hết map nhiệm vụ chưa?" — trả lời dứt điểm
Bảng quyết định map loại 4 là **`settings\task\tasklink_findmaps.txt`** (cột `MapID`), không
phải `map_index.lua`.

| Nguồn | Số map | Trạng thái |
|---|---|---|
| `tasklink_findmaps.txt` đang chạy | **14** | bảng cũ khớp 100% |
| `tasklink_findmaps.txt.goc` | **104** | bản gốc, đã bị cắt hôm 17/08 |
| `map_index.lua` (`TL_MAPTRAPINDEX`) | **204** | chỉ là bảng **tra tọa độ** |

⇒ Bảng txt nay phủ **cả 204 map** nên phục hồi `.goc` cũng không phải sinh lại. Muốn nhiều
map nhiệm vụ thật thì **phục hồi tệp `.goc` trên server**, không phải sửa auto.

Cũng đã kiểm chứng: **quái sinh từ pak** (`gamesetting.ini NotAddNpcNormal=1` +
`KRegion.cpp:468` là `!= kind_dialoger` ⇒ giữ quái, bỏ NPC thoại) — ⚠️ **chú thích trong mã
mô tả NGƯỢC**, đừng sửa theo chú thích.

### 2.7 Hai tính năng mới trong lệnh bài admin → `e81d33bf` → `d72ace99` → `9cee6585` → `e61bb31e`
Menu **"Da Tau: xoa phat huy + them luot huy"** (`script\item\datau_admin.lua`, ASCII;
lệnh bài chỉ bị chèn 2 dòng vá byte-safe nên 172 byte TCVN3 giữ nguyên):

| Mục | Ghi vào |
|---|---|
| Xóa phạt — làm tiếp được ngay | `1036=0` + ghi lại `1046` cho khớp (chữa luôn ca bị cấm hủy) |
| Thêm 10 lượt hủy | byte 4 của `1020` **và** `1046`, kẹp trần **100** |
| Đặt lượt hủy = 0 | như trên |
| Đặt số nhiệm vụ hôm nay = 0 | `2420=0` (cố ý **không** đụng `2797`) |
| Chọn nhân vật khác / bỏ chọn | `FindPlayer` + `SetTaskTemp(TMP_INDEX_PLAYER)` rồi đổi `PlayerIndex`, xong **trả lại** |

### 2.8 "Sa mạc 3 báo không đọc được map/số lượng rồi treo" → `ae1129f8`
Câu nhiệm vụ ghép tên map từ **cột `TaskInfo1`**: `"Sa Mạc sơn␣␣động 3"` (hai dấu cách),
còn bảng của auto lấy từ `map_index.lua`: `"Sơn Động Sa mạc tầng 3"` — **khác hẳn thứ tự
chữ** ⇒ không khớp ⇒ `nDTMapId=0` ⇒ treo. 11 map kia trùng tên nên chạy tốt; chỉ 3 map sa
mạc (225/226/227) dính. Sửa **tận gốc ở bộ sinh**: danh sách map và tên đều đọc từ
`tasklink_findmaps.txt` (**bỏ luôn danh sách 14 map chép tay**), tên nào lệch thì phát thêm
một dòng — 14 map → 17 dòng, **không phải sửa C++**.

### 2.9 "Nhận thưởng chưa tự kích vào phần thưởng, chạy thẳng script nên bảng thưởng vẫn hiện" → `e40f40ea`
Nút của `KUiDaTau`/`KUiDaTau1` khi người chơi bấm làm 2 việc: `Hide()` **rồi mới** gửi
`GOI_ADD_UI_CMD_SCRIPT` (UiQuestDT.cpp:87-117). `DTP_REWARD` cũ chỉ gọi `SendUiCmdScript`
= nửa sau, thiếu `Hide()`; server **không có gói đóng** cửa sổ này (khác give-box có
`S2C_GIVE_BOX nType==2`) ⇒ cửa sổ trơ mãi. Sửa: engine gọi
`CoreDataChanged(GDCNI_UI_ACT, 7, nhóm*10+nút)` → `GameSpaceChangedNotify` → hàm mới
`KUiDaTau[1]::AutoPick` phát `WND_N_BUTTON_CLICK` vào đúng nút = **bấm thật, tự đóng**.
AutoPick trả 0 (cửa sổ nhóm đó không mở — 3 lần thử chéo nhóm) thì UI ẩn nốt cửa sổ thừa
rồi engine gửi thẳng script dự phòng như cũ. Vòng xoay 6 nút + `uFinSeq` giữ nguyên.
Nút map thứ tự `DT_FIN3`/`DT_FIN4`: 30-32 = exp/tiền/ngẫu nhiên · 40-42 = điểm/may mắn/vật phẩm.
(⚠️ chú thích trong mã ghi nhầm "(20/08)" — thật ra tối 19/08.)

### 2.10 "Làm lại các dòng thông báo: có dấu, dễ hiểu, có màu" → `e40f40ea`
54 câu chuyển hết sang tiếng Việt **TCVN3 octal** (tệp .cpp vẫn thuần ASCII) + thẻ
`<color=...>`. Vì sao thẻ màu ăn được trong chat: `FilterTextColor` chỉ **lọc mã màu thô
0x02/0x03 trước**, sau đó `TEncodeText` (Text.cpp:487) mới dịch thẻ `<color=...>` thành mã
— đúng đường server vẫn dùng. Bảng màu: Cyan tiến trình · Green xong bước · Yellow cần
người chơi xử lý · Orange bỏ qua/thiếu đồ · Red lỗi bất thường · AYellow mốc thưởng ·
Gray ghi chú máy tự thêm (`DT_Hold` → "(tạm nghỉ N phút)"; `DT_Skip` nhánh hủy →
"(hủy nhiệm vụ - loại này đang tắt)"). Người gửi đổi `[DaTau]` → **`[Dã Tẩu]`** TCVN3.
**Hai luật chuỗi** (TEncodeText ghép cặp byte >0x80): trước mỗi thẻ `<` phải là ASCII
(dấu cách); ký tự **cuối chuỗi** phải ASCII. TCVN3 **không có nguyên âm HOA có dấu**
(chỉ Ă Â Đ Ê Ô Ơ Ư trần) ⇒ cấm viết "TẮT", dùng thường + tô màu.
Marker ASCII `[DaTau]` còn đúng 1 chỗ (g_DebugLog) để grep binary phân biệt bản cũ/mới:
bản mới `grep -c "<color=" CoreClient.dll` ra hàng trăm.

### 2.11 ĐỢT 20/08 (tối): 11 mục người dùng → `c9de7a7b` — chi tiết ở BANGIAO mục 13
Một câu mỗi mục: hộp giao đồ sửa tận gốc (server đòi mỗi gói move = 1 CLICK Down==Up —
`DT_ClickItem` 3 gói như người kéo thật) · Phúc Duyên kéo từ rương chỉ cần mật khẩu ·
túi đầy TỰ bán rác tại chỗ (hết vòng phù-về-lặp; có lá chắn không bán item nhiệm vụ) ·
bỏ tự-mua Thổ Địa Phù, ưu tiên phù vô hạn (6,1,437) rồi kéo rương · đang treo T2/T3 nhặt
được đồ đạt là tự về trả · đủ 40 treo 60' tự hỏi lại (chống lệch múi giờ server) · FARM T4
ưu tiên tọa độ tab Di chuyển đúng map · Lệnh bài hoàn thành (6,1,4818): client tự dùng khi
loại tắt + `lbhtdatau.lua` VIẾT LẠI cho tasklink (bản cũ nói với hệ chết 87-98; CẦN RESTART
server) · thiếu đồ loại 2 đi chợ MUA Ở SẠP (quét m_BaiTan, timeout 2,5s né sạp trang trí
SimCity, tự tính ô trống vì server bỏ qua lỗi đặt = mất tiền+món, mua món rẻ nhất ≤ trần)
+ nhảy đủ 10 thành/thôn qua menu Xa Phu "Những thành thị đã đi qua" · UI tab Dã Tẩu 2 cột
hết cụt chữ, "Khoe vật phẩm"→"Tìm trang bị", 3 điều khiển mới, ipc 6876→6888 + di trú .dat
HAI BẬC offsetof (không đè lựa chọn cũ). Deploy 21:32-21:35 (build ở D chép tay — CÂY NGUỒN E
BỎ theo chốt phiên F11). **PHẢN BIỆN đa-agent (6 mũi soi + 14 vòng bác bỏ): 8 lỗi thật đã sửa
ở `838960a9`** (nặng nhất: kéo đồ từ rương ngoài thành bị server drop im lặng; di trú .dat
`<` làm người dùng cũ mất 2 ô mới; bán rác bỏ qua công tắc bSellItem; livelock rút tiền sạp) —
bảng đầy đủ ở BANGIAO mục 13.12. Deploy lại CoreClient + WAuto 22:23; Game.exe dùng bản 22:13
của phiên F11 (build sau commit ipc nên đã 6888).

---

## 3 · Bản đồ biến đếm Dã Tẩu (đắt nhất phiên này — đừng đoán lại)

| Task | Ý nghĩa | Lưu ý sinh tử |
|---|---|---|
| **1020 byte 4** | số lượt hủy còn lại | **trần THẬT là 127**, không phải 254 — `LuaGetByte` dịch phải trên **int có dấu** nên byte ≥128 đọc ra **số âm** → `Task_Cancel` **xóa sạch chuỗi nhiệm vụ** |
| **1046** | bản sao chống gian lận | lệch với byte 4 = **cấm hủy vĩnh viễn** + ghi log gian lận ⇒ đổi một cái phải đổi **cả hai** (riêng `=0` luôn cho qua) |
| **1036** | số lần hủy đầu chuỗi; `==10` là **đang bị phạt** | đặt `=0` là gỡ xong |
| **1029** | mốc thời gian phạt (tự tha sau ~10 phút) | 🔴 **đừng reset riêng 1029** mà để `1036=10` → nhánh else gọi `Task_Confirm()` **xóa sạch tiến độ chuỗi** |
| **2419 / 2420 / 2797** | ngày / số nhiệm vụ trong ngày (trần 40) / số lần hủy trong ngày | `2797` chỉ dùng tính thưởng mốc 30 và 40, **không chặn gì**; reset `2420` = phát **lại** thưởng mốc 30 |
| 1021 / 1031 / 1032 / 1025 | loại nhiệm vụ / map / (byte2 = số cuộn cần) / đã có | client **không đọc được** id ≥256 (`nTaskId` là BYTE) |

**Ba luật khi thêm mục vào lệnh bài:**
1. `GetTask/SetTask` bám biến `PlayerIndex` ⇒ không đổi thì **chỉ sửa được chính GM**.
2. Đệm menu **512 byte dùng chung** cho tiêu đề + mọi nhãn, **cắt âm thầm** — phải đo bằng
   **chuỗi lúc chạy** (bản đầu của tôi ~648 byte = mất mục cuối; rút còn 448). Nhãn **cấm
   `|`**, tên hàm gọi lại **≤31 ký tự**.
3. `SyncTaskValue` ở JX1 là **hàm rỗng** — gọi không lỗi nhưng vô ích.

---

## 4 · Cạm bẫy công cụ đã mắc trong phiên (đừng lặp lại)

- **Bash heredoc rút `\\` thành `\`** — dính **2 lần** trong phiên này. Mọi script có
  backslash **phải đi qua tệp** (Write tool rồi chạy).
- **Ghi tệp bằng `open(p,"w")` rồi lỗi encode** = tệp bị **cắt trắng 0 byte** (mất tệp nháp
  một lần). Luôn kiểm dữ liệu ASCII **trước khi** mở tệp để ghi.
- **`git add -A <thư mục>`** quét nhầm `KPlayerBot.cpp` của phiên song song và một tệp
  `.pyc` (commit `4c264409`, đã ghi chú ở `1071dac1`). **Thêm từng tệp theo tên.**
- **Sửa .cpp/.h TCVN3 chỉ bằng python latin-1**, kiểm `check_encoding.py` FFFD=0 (533 byte
  cao của `CoreShell.cpp` giữ nguyên suốt phiên).
- **Vá hai pha**: kiểm mọi mốc trước, ghi sau — nhờ vậy hai lần thụt lề lệch không làm hỏng
  tệp nào.

---

## 5 · Việc còn nợ / chưa kiểm chứng

1. **Lệnh bài admin chưa chạy thử** — GameServer tắt suốt phiên. Cần thử cả bước
   "Chọn nhân vật khác".
2. **Auto Dã Tẩu sau bản 19:11 chưa test** — đặc biệt: nhiệm vụ **sa mạc** (2.8), đi tới
   **cụm quái thật** (2.5), **bấm rương thưởng có tự đóng cửa sổ không** (2.9), và
   **thông báo màu hiển thị đúng font** (2.10). Nhớ khởi động lại game (Game.exe cũng đổi).
3. **Bug có sẵn của hệ gốc, chưa sửa**: người chơi tự tích lượt hủy quá 127 bằng nút thưởng
   "may mắn" sẽ bị **xóa sạch chuỗi nhiệm vụ** ở lần hủy kế tiếp. Muốn sửa thì kẹp trần
   trong `SelectAward_Cancel` (seasonnpc.lua) và `tl_settaskstate`.
4. **Sáu việc phía bot server** anh nhắn rồi bảo dừng: bot cấp ≥80 mới làm Dã Tẩu, lên ngựa
   trong thành, bot bán sạp ngồi chỗ trống quanh NPC (không cưỡi ngựa, không kẹt kiến trúc),
   sạp bán trang sức `nMagicLevel` 1-5, bot map 79 ra ngoài map, xóa SimCity khỏi lệnh bài.
5. Mua **ngựa** (loại 1) và loại 5 PK/Tống Kim vẫn bỏ qua — giới hạn từ đầu.

---

## 6 · Chạy lại các bộ sinh + dựng/deploy

```bash
# bảng dữ liệu + marker (khi server đổi lời thoại/bảng nhiệm vụ)
python D:/GAMEDEVNEW/ReverseTools/gen_datau_tables.py

# bảng tọa độ quái (khi server đổi map/pak) - ghi thẳng ra client + repo
python D:/GAMEDEVNEW/ReverseTools/gen_datau_spots.py

# dựng ở D để bắt lỗi, rồi dựng ở E (build = deploy)
MSBuild Sources/Core/Core.vcxproj "-p:Configuration=Client Release" -p:Platform=Win32

# game đang chạy thì thay DLL bằng ĐỔI TÊN, xong kiểm CRT
python D:/GAMEDEVNEW/ReverseTools/re_pe_crt.py E:/SourceTuanLe/.../bin/client
```

---

## 7 · Danh sách commit của phiên (chỉ phần WAuto/Dã Tẩu/lệnh bài)

| Giờ | Commit | Nội dung |
|---|---|---|
| 10:24 | `32e60788` | **fix đứng yên** — `DT_WalkTo`/`HaveTarget` |
| 10:24 | `0d3c6629` | bọc `#ifdef _SERVER` quanh `PB_LogNgoai` (build client hỏng) |
| 11:23 | `e999d591` | sao lưu mã nguồn UI WAuto vào git |
| 11:40 | `34c79152` | đóng thoại + T4 tìm quái + túi đầy về bán rác |
| 11:40 | `0c97e240` | **UI WAuto thiết kế lại** (~190 tooltip) |
| 16:19 | `b6049e03` | bảng cụm quái từ pak + phù về đi Xa Phu lại |
| 16:27 | `f6f550c5` | 2 cửa sổ thưởng + chỉ hủy loại tắt + không phù giữa chừng + lên ngựa |
| 16:32 | `6bde16a1` | rương thưởng **tự xoay nút** |
| 17:00 | `15b242c3` | quét **204 map** + xuất txt, engine **nạp lúc chạy** |
| 17:07 | `4c264409` | lọc rác do trùng mã băm + nâng đệm 8192 |
| 17:12 | `16de2b3a` | đọc đúng pak `package.ini` + **neo cụm vào quái thật** |
| 17:34 | `e81d33bf` | **lệnh bài**: xóa phạt + thêm lượt hủy |
| 17:36 | `d72ace99` | hạ trần lượt hủy 254 → 100 (trần thật 127) |
| 17:41 | `9cee6585` | bỏ ghi `1029` + cảnh báo mốc thưởng |
| 17:50 | `ae1129f8` | **fix sa mạc** — tên map lấy từ `TaskInfo1` |
| 18:00 | `e61bb31e` | lệnh bài v2: **sửa được cho người chơi khác** + rút gọn menu |
| 19:14 | `e40f40ea` | **bấm nút THẬT cửa sổ thưởng** (AutoPick, mục 2.9) + **54 thông báo có dấu, có màu** (mục 2.10); deploy E 19:11, re_pe_crt PASS |
| 21:44 | `c9de7a7b` | **ĐỢT 20/08: 11 mục** (mục 2.11 / BANGIAO mục 13); deploy 21:32-21:35, re_pe_crt PASS |

Tài liệu cập nhật kèm theo: `BANGIAO_AUTO_DATAU_WAUTO.md` mục **7.2** (bộ sinh tọa độ),
**8.6-8.8** (ba nhóm lỗi test thật), **12** (công cụ lệnh bài).
