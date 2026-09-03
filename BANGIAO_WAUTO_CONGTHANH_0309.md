# BÀN GIAO — WAuto TỰ THAM GIA CÔNG THÀNH CHIẾN (03/09/2026)

> Chủ game giao: *"phân tích tính năng công thành chiến, viết thêm WAuto tự tham gia công thành —
> dựa vào mã nguồn auto mẫu Thái Lan và đọc tính năng công thành ở bản dự án để viết WAuto tự
> tham gia công thành đúng các bước; **không tự động nhảy map, phải tới đúng NPC để đăng ký**"*.
>
> Đọc mục **1** (swap) và **2** (câu hỏi chờ chủ) là đủ để tiếp tục. Mục 4 là luồng thật đã đo,
> mục 6 là bẫy.

---

## 1. Trạng thái — CHỜ SWAP + CHỜ CHỦ TEST (chưa test thật lần nào)

**Bốn** tệp `.moi`: ba ở `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client` + `CoreServer.dll.moi` ở `...\bin\server`.
**Phải swap cả bốn cùng lúc**: ba tệp client vì `struct autoData` (IPC WAuto ↔ Game.exe ↔ CoreClient.dll) thêm 12 trường ở cuối;
CoreServer vì vá S13 (giữ-chờ lệnh) nằm ở **cả hai bên** — chỉ lên một bên thì hai bên lệch cách xử lý lệnh.

| Tệp | md5 | cỡ (byte) | Nội dung |
|---|---|---|---|
| `CoreClient.dll.moi` | `f2ad5ca3` | `2.507.776` | **S13i/j** (phiên wauto-6a, 16:42: main `5c8b8736` + vá S13i/j — ⚠️ **nguồn S13i/j CHƯA có commit trên main lúc ghi dòng này**, wauto-6a phải commit + push trước khi chủ swap) — tập cha của MAIL 3 `e4ac910a` và của `5b56367c` (S13e) **đang chạy**; vẫn chứa máy `CT_Process` + bảng `KCongThanhTables.h` (chuỗi "[Công Thành]" TCVN3 đếm = 1), không đổi autoData/ExtAuto. Lịch sử: 96c3085d → 9976e63f → 5b56367c → 62730ed9 → f94b5100 → e4ac910a (MAIL 3) → f2ad5ca3 (S13i/j). |
| `Game.exe.moi` | `24762253` | `1.399.808` | **MAIL đợt 3** (wauto-d9, main `5c8b8736`): cửa sổ Hộp thư `UiMail.cpp`, struct `KMailUiAward` dùng chung CoreClient + Game.exe; vẫn có cổng máy CT trong `S3Client.cpp` (chuỗi "[HD-GATE] nCT=" đếm = 1) — tập cha của `d3d626ba` **đang chạy**. Phải lên CÙNG `CoreClient.dll.moi e4ac910a`. |
| `WAuto.exe.moi` | `46fdc93f` | `413.696` | tab thứ 15 **"Công Thành"** (nhóm *Sự kiện*) |
| `CoreServer.dll.moi` (**bin\server**) | `bad8e293` | `18.298.368` | **S13i/j** (wauto-6a, 16:42: main `5c8b8736` + S13i/j máy chủ: bỏ chiêu vào xác `[S13-XAC]`, hết mất khung khi NPC đổi vùng — ⚠️ nguồn chưa commit lúc ghi) — tập cha của MAIL 3 `cca51fdf` và của `7b3423c2` **đang chạy**. Ba tệp (kèm `Game.exe.moi 24762253` của MAIL 3, giữ nguyên) phải lên CÙNG LÚC. |

### Checklist swap (4 `.moi` cùng lúc — S13 hai bên phải cùng lên)

1. Thoát **hẳn** `Game.exe` **và** `WAuto.exe` (Task Manager không còn tiến trình nào).
2. Tắt `GameServer.exe` rồi chạy `bin\server\ChayGameServer.bat` — nó tự đổi `CoreServer.dll.moi` → `CoreServer.dll` (bản cũ giữ `.truoc`) và mở lại server.
3. Chạy `bin\client\ChoiGame.bat` — nó tự đổi `CoreClient.dll.moi` → `CoreClient.dll` và `Game.exe.moi` → `Game.exe`.
4. `ChoiGame.bat` **KHÔNG** đổi `WAuto.exe.moi`: đổi tay `WAuto.exe` cũ → `.truoc`, rồi `WAuto.exe.moi` → `WAuto.exe`.
5. Mở WAuto → nhóm **Sự kiện** → phải thấy tab **"Công Thành"** (tab thứ 3 của nhóm). Không thấy = bước 4 chưa xong.
6. Cấu hình cũ `APdata\<ID>.dat` **vẫn dùng được** — `LoadRoleData` di trú theo `offsetof(autoData, bCongThanh)`, tính năng mặc định **TẮT**.
7. Restart mà chưa làm bước 1-4 thì vẫn chạy bản cũ. Kiểm nhanh: md5 `CoreClient.dll` = `f2ad5ca3…`, `Game.exe` = `24762253…`, `CoreServer.dll` = `bad8e293…`.

Build lại (đúng thứ tự, **tắt post-build** để không đè `bin\client` đang chạy):
```
MSBuild D:\GAMEDEVNEW\Sources\Core\Core.vcxproj -p:Configuration="Client Release" -p:Platform=Win32 -p:SolutionDir=D:\GAMEDEVNEW\Sources\\ -p:PostBuildEventUseInBuild=false -m:1
copy Sources\Core\ClientRelease\CoreClient.lib Sources\lib\release\CoreClient.lib
MSBuild D:\GAMEDEVNEW\Sources\S3Client\S3Client.vcxproj -p:Configuration=Release -p:Platform=Win32 -p:SolutionDir=D:\GAMEDEVNEW\Sources\\ -p:VcpkgEnableManifest=false -p:PostBuildEventUseInBuild=false -m:1
MSBuild E:\Src_Auto_Ngoai\WAuto\WAuto\WAuto.vcxproj -p:Configuration=Release -p:Platform=Win32
```
Kết quả: `Sources\Core\ClientRelease\CoreClient.dll` · `Sources\S3Client\Release\Game.exe` · `E:\...\WAuto\Release\WAuto.exe`.
Máy chủ (S13): `MSBuild Core.vcxproj -p:Configuration="Server Release" -p:Platform=x64` cùng cờ → `Sources\Core\x64\ServerRelease\CoreServer.dll`.

---

## 2. CÂU HỎI CHỜ CHỦ GAME QUYẾT (đã chọn mặc định an toàn, đổi được bằng ô cấu hình / 1 dòng)

| # | Câu | Mặc định đang chạy |
|---|---|---|
| Q1 | **Công Thành vs Tống Kim trùng giờ** (TK 20h50 nằm trong trận CTC 20h00–21h30): máy nào ưu tiên? | Hai máy **loại trừ nhau**, máy nào đang cầm lái thì giữ; cả hai rảnh thì **Công Thành trước**. **Chủ trả lời 03/09 chiều:** *"tống kim đang mở test nên chưa cố định giờ, sau này sẽ không trùng giờ CTC"* → giữ nguyên, không đổi. |
| Q2 | Bên công **có nên nhắm cổng/trụ** hay chỉ PK người? | Combo *Bên công* = "Phá cổng rồi hạ Long trụ" (đủ 3 trụ = thắng). Combo *Bên thủ* = "Canh Long trụ, chiếm lại nếu mất". Đều có lựa chọn "Chỉ đánh người (tuần tra)". |
| Q3 | Người **không thuộc 2 bang** nhưng có *Thành chiến lệnh bài* (event item 354–367, bang chủ mua ở Sứ Giả) | Combo *Phe*: "Tự theo bang hội" (mặc định) / "Bên công" / "Bên thủ". Ép phe khi có lệnh bài; không có thì máy chủ từ chối "Thân phận nghĩa sĩ chưa phù hợp" → auto bỏ lượt. |
| Q4 | Hết trận về đâu? | Combo *Hết trận về*: 7 thành (qua "Những thành thị đã đi qua" của Xa Phu hậu phương) hoặc **"Điểm lưu (Rời khỏi đấu trường)"** (mặc định). |
| Q5 | **Lôi đài bang hội** 20h ngày báo danh (Sứ Giả map 53 → map 213–220, ≤16/phe, cấp ≥90) có làm auto không? | **Chưa làm** — máy này chỉ lo **trận công thành map 221**. Khuôn Sứ Giả → NPC camper "Ta thích hợp với điều kiện…" (marker `CTM_OPT_LOIDAI` đã trích sẵn). |
| Q6 | **Đấu thầu 18h** (bang chủ, nhập phí ≥ 1.000.000) có tự làm không? | **Không** — quyết định tiền của bang chủ, để bấm tay. |
| Q7 | Mua thuốc công thành ở *Tùy Quân dược Y* | **ĐÃ LÀM (r3, chủ giao 03/09 chiều)** — xem mục 4a. Ô "Mua Ngũ Hoa Ngọc Lộ ở Tùy Quân dược Y trước khi ra trận" (mặc định BẬT, 10 bình, cấp 5). |
| Q8 | Nhân vật đang ở **map luyện công không có Xa Phu** | **Chủ chốt 03/09 chiều:** *"nhân vật ở map luyện công thì phù về thành"* → giữ như đang làm (dùng phù về thành rồi đi bộ tới Xa Phu). |

---

## 3. Đã làm gì

| Việc | Ở đâu |
|---|---|
| Phân tích luồng CTC người chơi (mục 4) từ script SỐNG + dữ liệu map | `bin\server\script\...`, `bin\server\maps\特殊用地\剑门关vn`, pak Linux |
| Bộ sinh bảng `KCongThanhTables.h` (marker byte-exact + toạ độ + **ô trap đọc từ Region_S.dat**) | `ReverseTools\gen_congthanh_tables.py` (+ cache `congthanh_trap_cache.json`) |
| Máy trạng thái `CT_Process` (9 pha) | `Sources\Core\Src\CoreShell.cpp` khối `AUTO CONG THANH CHIEN` (sau `HET AUTO TONG KIM`), `case ATYPE_CONGTHANH` |
| Bộ vá idempotent 9 tệp (`--thu` = chỉ kiểm neo) | `ReverseTools\goi_va_wauto_congthanh.py` + `ReverseTools\wauto_ct\ct_block.cpp.txt` |
| 15 trường cấu hình cuối `autoData` + 26 trường `ExtAuto` + `ATYPE_CONGTHANH` | `ipc_shared.h` (3 bản), `KPlayer.h`, `CoreShell.h` |
| Cổng gọi máy CT trước Tống Kim, gộp `nBS` | `S3Client.cpp` `ExtAutoLoop` |
| Tab "Công Thành" (16 ô, tooltip, lưu/nạp/di trú/mặc định) | `WAuto.cpp`, `WAuto.rc`, `Resource.h` (E:\ + mirror `WAutoUI`) |

**Học từ auto Thái Lan** (`Form1.cs` tab *CTC › Thất thành*, `Class83.cs`, `Class104.cs`, `FormXaphuCT.cs`):
nó có 2 đường vào — *"Đi xa phu vào CTC"* (bảng toạ độ Xa Phu công thành 7 thành, nút "Lấy vị trí xa phu CTC")
và *"Dùng THP để vào, click menu dòng"*. Chủ chốt **chỉ giữ đường Xa Phu**; WAuto đã có sẵn bảng trạm Xa Phu
theo map (`g_MoveStation`, 15 map thành/thôn) nên không cần người chơi "lấy vị trí". Auto Thái đọc bộ nhớ
client bằng shellcode; WAuto là mã trong client nên gọi thẳng `DialogNpc`/`FindPath`.

---

## 4. LUỒNG THẬT ĐÃ ĐO (script sống + Region_S.dat) — máy CT bám đúng từng bước

```
CTP_OFF ──(cửa sổ giờ 20:00−3' .. +95' theo giờ máy chủ  HOẶC loa "… công thành chiến chính thức bắt đầu!")──►
CTP_GO       đi bộ tới Xa Phu (trạm g_MoveStation của map; map không có Xa Phu → phù về thành → đi tiếp;
             combo "Thành báo danh" 0..6 → LD_DiThanh tới thành đó trước)                     ─► DialogNpc
CTP_XAPHU    menu Xa Phu: "Đi Chiến trường công thành/GoCityWar"
             ├─ không có thoại tiếp = chưa khai chiến → 60 s hỏi lại (tới hết cửa sổ)
             └─ thoại "… bên công<X>, bên thủ<Y> …": đọc X/Y, so tên bang mình → "Bên công"/"Bên thủ"
                • "Thân phận nghĩa sĩ chưa phù hợp" → bỏ lượt hôm nay
             → NewWorld 223 (công) / 222 (thủ)
CTP_HAUPHUONG map 222/223: đi tới Ô TRAP TẬP KẾT (32 ô, 2 đoạn — zhongzhuan_map\trap.lua) → JoinCamp
             ├─ "Phe ta hiện đang tập hợp" (MS_STATE 0) → 30 s đạp lại
             ├─ "…gia nhập bang hội quá ngắn" (<5 ngày) / "…không có lệnh bài" → CTP_END
             └─ "…đã đầy" (200/phe) → 60 s thử lại
             → NewWorld 221 doanh (thủ 1533,3211 / công 1903,3608), FightState 0, PK khoá
CTP_DOANH    map 221, FightMode 0: đạp trap cửa doanh (thủ ctrap1/1b/1c 79 ô; công ctrap2/2b/2c 75 ô)
             → SetPos ra trận (1571,3263)/(1866,3547) + FightState 1
CTP_FIGHT    trả 2 → máy PK (tab PK + Chiêu KH) đánh mục tiêu uNpcID:
             1) người khác CAMP trong "Tầm nhìn PK", có đường nhìn (TK_ThayDuoc)
             2) bên công: cổng (NPC 532) gần nhất còn sống → tới sát mà không thấy = cổng vỡ →
                Long trụ thủ giữ (NPC 528) gần nhất; thấy NPC 530 = đã chiếm → trụ khác
                bên thủ: thấy NPC 530 → đánh chiếm lại; không thì đứng canh 1 trụ (đổi trụ mỗi 2')
             chết → hồi sinh về doanh (FightState 0) → CTP_DOANH → đạp trap ra lại
             loa "Chung cuộc" hoặc >100' → đạp trap RA (trap1/trap2) về hậu phương
CTP_END      map 222/223: Xa Phu hậu phương (chefu.lua tại (1612,3164)/(1636,3191)):
             "Rời khỏi đấu trường" → điểm lưu  |  "Những thành thị đã đi qua" → thành chọn
             không thấy Xa Phu sau 8 s → dùng phù về thành (hậu phương KHÔNG cấm phù)
CTP_VETHANH  LD_DiThanh tới thành chọn (nếu khác) → CTP_DONE (khoá theo ngày máy chủ)
```
Mọi pha đi đường/thoại kẹt quá **3 phút** → bỏ lượt, trả máy. Relog giữa trận: đang ở 221/222/223 → nhận
việc ngay theo map + FightMode (không cần đúng giờ).

### 4a. (r3) Mua thuốc ở Tùy Quân dược Y — trong `CTP_DOANH`, trước khi đạp trap ra trận

- **Sự thật đo được:** `chengzhan_map\yaoshang.lua` chỉ gọi `Sale(53)` = mở **cửa sổ shop 53** (không phải thoại chọn dòng như
  Quân Y Tống Kim). `buysell.txt` dòng 53 = goods 1–15, 21–25 = **thuốc thường** cấp 1–5: máu (1/0), nội lực (1/1), Ngũ Hoa
  Ngọc Lộ máu+nội (1/2, cấp 5 = 3000 lượng), giải độc (1/4). 4 "Hộp thuốc Công Thành Chiến" 1307–1310 và viên 1303–1306 là
  đồ JX2 **không có script** (`magicscript.txt Script=0`, `tb_CW_MEDICINE` không ai gọi) → bỏ qua.
- 8 NPC "Tùy Quân dược Y" (`head.lua DoctorPos`, 4 quanh mỗi doanh) do `mission.lua` AddNpc lúc mở trận → `g_CTDuocY[8]`,
  tên hạ chữ ASCII `CTM_NPC_DUOCY` để `DT_FindNpcName` khớp.
- Máy: mỗi mạng một lần (`nCTMua`, đặt lại khi hồi sinh): đứng 3 s chờ NPC đồng bộ → tới dược Y gần nhất → `DialogNpc` → shop mở
  (`CoreDataChanged(GDCNI_UI_ACT, 2)`) → mua từng bình `item_medicine` detail 2 đúng **cấp** đã chọn (`SendClientCmdBuy`, 250 ms/bình)
  tới khi túi có đủ **số bình**; túi đầy / hết tiền / không có hàng / quá 60 s → đóng shop (`GDCNI_UI_ACT, 3`) ra trận luôn.
  Uống thuốc do **tab Phục hồi** sẵn có (máy PUMPLIFE/PUMPMANA nhận detail 2). Ô cấu hình: `bCTMua`, `nCTSoBinh`, `nCTCapBinh`.

### Sự thật máy chủ đã kiểm chứng (tệp:dòng)

- Lịch: `timerserver_ctc.lua` TB_CTC6 — **20h ngày đánh** mỗi thành (PT T5 · TĐ T3 · ĐL T4 · BK T7 · TD T6 · DC T2 · LA CN) → mỗi ngày trong tuần có 1 thành; `citywar_global\timer.lua` poll **5 phút** → `OpenMission(7)` + `RunMission(7)` (MS_STATE 2 ngay, loa toàn máy chủ `mission.lua:114`). Trận **90 phút** (`head.lua GAMETIME`).
- Lối vào duy nhất của người chơi: **Xa Phu thành** (`global\npcchucnang\xaphu.lua:23`) → `station_ctc.lua GoCityWar/GoCityWarAttack/Defend` (kiểm bang == `GetCityWarBothSides` hoặc lệnh bài `CardTab`) → 222/223.
- Trap tập kết `zhongzhuan_map\trap.lua`: `GetJoinTongTime() >= 7200` phút (**5 ngày trong bang**) → `camper.lua JoinCamp`: `LeaveTeam`, trần 200/phe, `SetCurCamp`, `SetPKFlag(1)+ForbidChangePK`, `SetTempRevPos` doanh, `NewWorld(221)`, `SetFightState(0)`.
- Ra trận `chengzhan_map\ctrap1/2*.lua`: FightState 0 → `SetPos` + `SetFightState(1)`; `trap1/trap2.lua` = ra khỏi trận về 222/223 (**nằm sát doanh** — bảng `g_CTTrapRa*` để tránh).
- Thắng: `symboldeath.lua` đổi chủ trụ (528↔530), `CheckWin` 3/3 → công thắng; `totaltimer.lua` hết giờ công giữ ≥2 → công thắng. `doordeath.lua` cổng chết → `ClearObstacle` rào 7 ô chéo sau cổng (rào = `AddObstacleObj` **server-only, client không biết** → luật "cổng chưa vỡ không nhắm trụ").
- Kết: `camper.lua GameOver` → `LeaveGame`: `NewWorld(OuterPos 222/223 (1613,3185))`, PK tắt, camp gốc.
- Client **không nạp trap**: `KRegion::GetTrap` trả 0 ngoài `_SERVER` (KRegion.cpp:954), `Region_C.dat` chỉ NPC/Obj (KRegion.cpp:337-341) ⇒ ô trap phải trích offline.
- `KMapSuKien.h:80` đã liệt kê 221/222/223 (loại 7) → `ATYPE_MAPSUKIEN` chặn Dã Tẩu/di chuyển/phù tự do trên 3 map này.

---

## 5. Bản đồ mã

| Thứ | Ở đâu |
|---|---|
| Enum pha `CTPhase`, hằng `CT_HANPHA`… | `CoreShell.cpp` khối `AUTO CONG THANH CHIEN` (grep) |
| Cửa sổ giờ / loa | `CT_TrongCua`, `CT_NgayServer`, đầu `CT_Process` (`CTM_NEWS_KHAICHIEN` / `CTM_NEWS_CHUNGCUOC`) |
| Chọn phe từ thoại | `CT_LayTenBang` + `CTM_SAY_BENCONG/BENTHU` (case `CTP_XAPHU`) |
| Đạp trap (lùi ra – vào lại vì trap chỉ kích khi id ô đổi, `KNpc::CheckTrap`) | `CT_DapTrap` |
| Mục tiêu người / cổng / trụ | `CT_ChonDichNguoi`, `CT_ChonMucTieu`, `CT_TuanTra`, `CT_TimNpcTai` |
| Loại trừ với Tống Kim | đầu `CT_Process` (`ea.nTKHold`), đầu `TK_Process` (`ea.nCTHold`), `S3Client.cpp` (`nCT` trước `nTK`) |
| Bảng số liệu | `KCongThanhTables.h` — **sinh lại bằng `gen_congthanh_tables.py`, đừng gõ tay** |
| Cấu hình | `ipc_shared.h` 12 trường `bCongThanh… bCTLoa`; WAuto ID `IDC_*_15_*` 598–622, `IDC_INDEX_END` 640; khối ID ngoài dải dời **600–615 → 700–715** |

### Log để kiểm khi test

| Lọc kênh chat | Cho biết |
|---|---|
| `[Công Thành]` | mọi bước: tới giờ / chưa có trận / đăng ký bên … / đã tới hậu phương / trận chưa mở cửa / đã vào doanh / đã ra trận / cổng đã vỡ / rời hậu phương / về thành |
| `[HD-GATE] nCT=` (AutoLog) | máy CT trả 0/1/2 mỗi 5 s cùng các máy khác |
| `[PK-COMBO]` / `[PK-EMIT]` | máy PK đánh mục tiêu do CT giao (cổng/trụ = NPC 532/528/530) |

---

## 6. BẪY đã đo — đừng dẫm lại

1. 🔴 **Trap chỉ kích khi id ô ĐỔI** (`KNpc.cpp:11129 if (m_TrapScriptID == dwTrap) return;`): đứng yên hay đi dọc dải trap 2 ô rộng không kích lại. `CT_DapTrap` lùi ra 3 ô (ô không phải trap, không vật cản) rồi vào lại.
2. 🔴 **Ô trap ra-khỏi-trận nằm SÁT doanh**: `trap1.lua` (1522–1531,3198–3208) cạnh doanh thủ (1533,3211); `trap2.lua` (1904–1913,3614–3624) cạnh doanh công (1903,3608). Đạp nhầm = bay về hậu phương; máy tự vào lại (tối đa 3 lần) nhưng đừng đặt điểm đi qua đó.
3. 🔴 **Rào sau cổng client không thấy** (`AddObstacleObj` server): A* client vẽ đường xuyên rào → máy chủ giật về. Bên công chỉ nhắm trụ sau khi một cổng vỡ; nếu thấy "giật tại chỗ" trước rào thì nhìn `[S9-BOMUCTIEU]`.
4. 🔴 **Hai dòng "tab 14 …" trùng nhau làm bộ vá bỏ hunk**: `Tep.them` nhận biết "đã áp" bằng dòng đầu của khối → hai khối cùng dòng đầu thì khối sau bị bỏ. Đã đổi sang tham số `dau` riêng. Tương tự `Tep.thay` khi `moi` chứa `cu` (thêm phần tử vào cuối danh sách) → áp 2 lần; đã sửa thành "có `moi` là thôi".
5. 🔴 **Bash tool rút `\\` thành `\`** (bẫy đã ghi): script có backslash phải ghi bằng Write tool rồi chạy, không heredoc.
6. `WAuto.cpp`/`.rc` UTF-16LE **CRLF**; đọc bằng `newline=''` (universal newline biến CRLF→LF làm neo có `\r\n` trượt).
7. `IDC_INDEX_END` là biên của vòng ẩn `ShowTab`; đẩy nó qua 600 thì phải dời khối ID ngoài dải (hộp thoại rời, nút tab, dòng trạng thái) — đã dời 600–615 → 700–715.
8. Post-build của `Core.vcxproj`/`S3Client.vcxproj` **chép đè thẳng `bin\client`** → build với `-p:PostBuildEventUseInBuild=false` rồi tự đặt `.moi`; `S3Client` link `Sources\lib\release\CoreClient.lib` — chép tay sau khi build Core.
10. 🔴 **Hunk "thêm phần tử vào cuối danh sách" bị áp chồng khi đợt sau đổi giá trị** (r3): hunk cũ `"…335" → "…335, 246"`
   có `cu` là tiền tố của bản r3 `"…335, 270"` → chạy lại tool là thành `"…335, 246, 270"` (thừa phần tử, lỗi biên dịch). Mọi hunk
   kiểu này phải gác bằng **trạng thái cuối** (`if "335, 246" not in s and "335, 270" not in s`), không dựa vào `moi in s`.
9. Map 222/223 (`特殊用地\剑门关中转`) **không có trên đĩa máy chủ này** — trích từ `D:\ServerLinux\server1\pak\maps.pak` (giải nén UCL NRV2B thuần Python trong `gen_congthanh_tables.py`, đã kiểm bằng map 221 khớp 100% bản trên đĩa). Cache ở `ReverseTools\congthanh_trap_cache.json`.

---

## 7. Nghiệm thu (theo thứ tự)

1. Swap 4 tệp `.moi` (mục 1, cả `CoreServer.dll.moi` phía server) → mở WAuto → nhóm Sự kiện → tab **Công Thành** hiện đủ 13 ô; chỉnh vài ô, đóng mở lại → giá trị còn nguyên; tab cũ (Tống Kim, Liên đấu, Chiêu KH…) không đổi.
2. Nhân vật **trong bang tham chiến ≥ 5 ngày**, bật "Bật auto Công Thành Chiến", tick auto ở dòng nhân vật, đứng ở map luyện công. Tới **19:57** (giờ máy chủ): chat `[Công Thành] Tới giờ Công Thành Chiến - đi tới Xa Phu đăng ký tham chiến.` → nhân vật phù về thành (nếu map không có Xa Phu) → **đi bộ** tới Xa Phu → mở thoại.
3. Chưa khai chiến: `Chưa có trận công thành nào đang mở - 60 giây hỏi lại Xa Phu.` lặp tới khi loa `… công thành chiến chính thức bắt đầu!` → chọn "Đi Chiến trường công thành" → `Đăng ký bên công/thủ …` → sang 222/223.
4. Hậu phương: đi tới cửa tập kết → (nếu "Phe ta hiện đang tập hợp" thì 30 s sau đạp lại) → `Đã vào doanh trại` → (r3) đứng 3 s → tới Tùy Quân dược Y → cửa sổ shop mở → mua tới đủ số bình → `Đã đủ thuốc - ra cửa doanh.` → đi tới cửa doanh → `Đã ra trận - đánh theo cấu hình tab PK.`
5. Trong trận: có địch thì đánh; không thì bên công chạy tới cổng gần nhất đánh cổng (NPC 532), cổng vỡ → `Cổng đã vỡ - tiến vào đánh Long trụ.`; bên thủ đứng cạnh trụ. Chết → tự hồi sinh → đạp trap ra lại.
6. Hết trận (server kéo về hậu phương) → `Hết trận Công Thành - rời hậu phương.` → Xa Phu hậu phương → về điểm lưu / thành chọn → `… trả máy lại cho auto cũ.` → auto cũ chạy tiếp.
7. Nhân vật **không có bang** → `Chưa có bang hội - không tự tham gia…`, không đi. Bang không tham chiến → `Bang của mình không tham chiến trận này (công: X / thủ: Y)`.
8. **HỎNG cần báo:** kẹt lặp thoại Xa Phu; đứng trên trap mà không vào; đi lạc vào rào/đứng giật; hết trận không rời hậu phương; Tống Kim 20h50 cướp máy giữa trận.

---

## 8. Chưa làm / nợ

- Lôi đài bang hội (Q5), đấu thầu 18h (Q6), dụng cụ công thành (Kim Nguyên Bảo). (Q7 mua thuốc: đã làm r3.)
- Chưa đo thật trên máy chủ (không có trận trong phiên này). Cần 1 trận thật để chỉnh: bán kính tìm cổng/trụ (8 ô), ngưỡng "cổng vỡ" (12 ô), nhịp đạp trap.
- Phản biện tác tử **chưa chạy** (luật 01-02/09: chỉ chạy khi thật cần) — nên chạy 1 vòng chọn lọc trên `CT_ChonMucTieu` + `CT_DapTrap` trước khi thả cho người chơi đông.

*Ghi 03/09/2026.*
