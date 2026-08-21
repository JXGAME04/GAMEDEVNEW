# PHÂN TÍCH DỊCH NGƯỢC — LÔI ĐÀI · TÍN SỨ · HOẠT ĐỘNG BANG HỘI (bản Linux → dự án JX1)

> Phiên **21/08/2026**. **CHỈ PHÂN TÍCH, KHÔNG SỬA GÌ.**
> Nguồn: `D:\ServerLinux\server1` (GameServer Linux JX2/Kiếm Thế VNG — bản tham chiếu)
> Đích: `D:\GAMEDEVNEW\Sources` (mã nguồn) + `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` (cây chạy thật)
> Mọi con số dưới đây đều **đo thật** bằng công cụ, có ghi `tệp:dòng` để đối chiếu lại.

---

## 0. TÓM TẮT ĐIỀU HÀNH — đọc 1 phút

Ba nhóm tính năng người dùng hỏi tương ứng **9 cây script** bên bản Linux. Trạng thái dự án:

| # | Tên Việt | Cây bản Linux | Quy mô | Dự án hiện có gì | Hàm engine còn thiếu |
|---|---|---|---|---|---|
| L1 | **Lôi đài tỷ võ** (nhiều người, 1v1→8v8) | `missions/bw` | 9 tệp / 854 dòng | ❌ chưa có. Map 209 đang bị **Lôi Đài Hỗn Chiến** tự viết chiếm | **6** |
| L2 | **Bách Nhân Lôi Đài** (5 đài, lôi chủ giữ đài) | `missions/bairenleitai` | 10 tệp / 1.368 dòng | ❌ chưa có | **4** |
| L3 | **Cảnh Kỹ Trường** (đấu xếp hạng 1v1, danh hiệu) | `missions/arena` | 9 tệp / 1.096 dòng | ❌ chưa có | **8** |
| L4 | **Lôi Đài Bang Hội** | `missions/citywar_arena` | 10 tệp / 560 dòng | ✅ **ĐÃ PORT** (đợt E) nhưng **đang IDLE**; song song có bản tự viết `tinhnang/loidai` **đã bị tắt** | **0** |
| T1 | **Tín Sứ** (Dịch Quan → 3 ải → trả nhiệm vụ) | `task/tollgate/messenger` + `item/messenger` | 66 tệp / 5.992 dòng | ❌ chưa có. Map 395 đang bị **Thiên Bảo Khố** tự viết chiếm (ĐANG CHẠY) | **3** (lõi thật sự **0**) |
| B1 | **3 Hoạt động Phường bang hội** (Chiêu mộ đệ tử · Niên Thú · Thu thập vật tư) | `missions/tong` | 41 tệp / 4.533 dòng | ❌ chưa có — **9 dòng `Include` đã bị stub 15/08** | **17** |
| B2 | **Cổ Tháp Bang Hội** (6 ải boss, phó bản) | `missions/tong_guta` | 40 tệp / 4.686 dòng | ❌ chưa có | **35** |
| B3 | **Bang Hội Thành Bảo** (lãnh địa, vệ binh, Thần Mộc Lệnh) | `missions/tongcastle` | 9 tệp / 1.835 dòng | ❌ chưa có | **17** |
| B4 | **Bang Chiến** (liên minh bang, 150 người, xếp quân hàm) | `missions/tongwar` + `event/tongwar` + `tongpkmessage` | 24 tệp / 3.117 dòng | ❌ chưa có | **16** |

**Hợp nhất: 69 hàm engine Lua còn thiếu** (mục 5). Trong đó **~49 hàm đã có hàm gần tương đương** trong dự án (chỉ cần bọc/mở rộng), **20 hàm không có gì tương đương** — đó mới là phần đắt.

**Ba kết luận quan trọng nhất:**

1. **Nền tảng bang hội của bản Linux ĐÃ được port rồi.** `scriptjx2\tong_vn` = bản `script\tong` của Linux (66/67 tệp), engine có **127 `TONG_` + 27 `TONGM_` + 30 `TWS_`**. Cái còn thiếu **không phải hệ bang hội**, mà là **các HOẠT ĐỘNG treo trên nó** (`missions/tong*`).
2. **Tín Sứ gần như miễn phí.** Lõi nhiệm vụ **không thiếu hàm engine nào**; 3 hàm ở bảng trên đều nằm ngoài lõi (2 hàm chỉ dùng cho 5 lệnh bài phụ, 1 hàm có `if ... then` bảo vệ). **Dữ liệu 3 bản đồ ải đã nằm sẵn trong `maps.pak` của dự án** (192/127/99 tệp region server).
3. **Có 2 va chạm tài nguyên phải quyết trước khi gõ:** map **209** (Lôi đài tỷ võ ↔ Lôi Đài Hỗn Chiến đang chạy) và map **996** (Cổ Tháp ↔ PUBG đang chạy). Map **395** thì trùng khái niệm với Thiên Bảo Khố tự viết đang chạy.

---

## 1. NGUỒN SỰ THẬT VÀ CÁCH TÁI LẬP SỐ LIỆU

### 1.1 Mã hoá — bẫy đầu tiên
Cây script Linux **trộn 2 bảng mã trong cùng một tệp**: chú thích Trung là **GBK**, chuỗi hiển thị Việt là **TCVN3**.
Nhận dạng đúng phải làm **theo TỪNG DÒNG**: dòng mà **mọi byte cao đều nằm trong bảng 74 byte TCVN3** *và* run byte cao liên tiếp ≤ 3 → TCVN3; còn lại → GBK.
(Nhận dạng theo cả tệp là sai: `missions/citywar_arena/camper.lua` cả tệp bị đoán là GBK trong khi phần lớn chuỗi thoại là TCVN3.)

### 1.2 Công cụ dùng trong phiên (đã đặt vào `ReverseTools/`)
| Tệp | Việc |
|---|---|
| `re_lua_api_gap.py` (có sẵn) | bản gốc — đối chiếu API 1 cây |
| **`gbktool.py`** (mới) | đọc/tìm kiếm cây script trộn GBK+TCVN3, tự nhận dạng theo dòng |
| **`api_gap2.py`** (mới) | đối chiếu API 1 cây, phân 5 nhóm `OK / TREE / LIB / ENG / ???` |
| **`dep_gap.py`** (mới) | bao đóng phụ thuộc `Include()` + đánh dấu tệp dự án đã có |
| **`pakcheck.py` / `pakscan.py` / `regionscan.py`** (mới) | băm `FileNameToId` để tra tài nguyên trong `.pak/.mps` |

```bash
python ReverseTools/api_gap2.py "TIN SU" task/tollgate/messenger item/messenger
python ReverseTools/regionscan.py "E:/.../bin/server/Pak" '特殊用地\leitai'
```

### 1.3 🔴 Hai bẫy của công cụ cũ đã sửa trong phiên này (nếu lặp lại sẽ đếm SAI)

1. **`function Lop:Ten(` KHÔNG che hàm toàn cục `Ten`.**
   `lib/player.lua:365` có `function Player:ForbidEnmity(flag) return CallPlayerFunction(self.m_PlayerIndex, ForbidEnmity, flag) end` — thân hàm gọi **chính hàm engine cùng tên**. Bản `re_lua_api_gap.py` cắt tiền tố `Player:` nên xếp `ForbidEnmity` vào nhóm "script tự định nghĩa" ⇒ **giấu mất 5 hàm engine thiếu**. Chỉ `function Ten(` trần mới là định nghĩa toàn cục.
2. **`FileNameToId` phải chặn 32 bit TRƯỚC phép chia dư.**
   C: `id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;` với `id` là `unsigned long` 32 bit. Thiếu `& 0xFFFFFFFF` trước `%` là tra pak ra **0 kết quả** cho cả những map chắc chắn tồn tại (đã dính bẫy này trong phiên, mất 20 phút). Ngoài ra:
   - Đường dẫn tra pak = `\maps\` + giá trị trong `MapList.ini` (đã **rút `\\` về `\`**) + `.wor`; **KHÔNG hạ chữ thường bằng `.lower()` của Python** (nó phá byte GBK — bản thân hàm băm đã tự hạ A–Z rồi).
   - Region: `<đường dẫn map>\v_%03d\%03d_Region_S.dat` với `%03d` là **chỉ số region TUYỆT ĐỐI** (`m_nRegionBeginY+h`, `m_nRegionBeginX+w`, `KSubWorld.cpp:258`) — có map bắt đầu từ ~106, quét dải 0..90 là kết luận nhầm "map trống".

---

## 2. BẢN ĐỒ TÊN — tên Việt ↔ thư mục Linux

"Lôi đài" trong bản Linux **không phải một tính năng** mà là **năm** thứ khác nhau. Đây là bảng đối chiếu chốt (trích từ chuỗi TCVN3 trong chính script):

| Tên hiện trong game | Thư mục | Ghi chú |
|---|---|---|
| "Lôi đài thi đấu" / "Lôi đài nhiều người" | `missions/bw` (比武 = tỷ võ) | `bwmanager.lua:26,49` — 🔴 **ĐÃ KHOÁ CỨNG**, xem dưới |
| **"Lôi Đài Hoàng Thành Tư"** = Bách Nhân Lôi Đài | `missions/bairenleitai` (百人擂台) | `npc_enter.lua:19`; `activitysys/config/9` là **bộ khởi động** của nó (`config.lua:12,45`), KHÔNG phải tính năng riêng |
| **"Lôi đài bang hội"** | `missions/citywar_arena` | `camper.lua:44` — thuộc hệ Công Thành Chiến |
| "Lôi Đài Hoa Sơn Đại Chiến" | `missions/huashanqunzhan` (26 tệp/758 dòng) | ngoài phạm vi yêu cầu, chỉ ghi nhận |
| "Cảnh Kỹ Trường" | `missions/arena` | **KHÔNG gọi là lôi đài** nhưng là đấu trường 1v1 xếp hạng — `npc/officer.lua:79` |

> ✏️ **Đính chính 21/08 (sau khi đọc kỹ):** bản đầu của tài liệu này đếm "lôi đài" là **5 thứ** và tách
> `activitysys/config/9` thành một mục riêng. Sai — nó là **vỏ khởi động của `bairenleitai`**
> (`config/9/config.lua:12` Include `hundred_arena.lua`, `:45` Include `npc_enter.lua`). Đúng là **4 thứ**.

> 🔴 **HAI TÍNH NĂNG ĐANG CHẾT trên chính bản Linux** (phát hiện 21/08, workflow 13 tác tử):
> 1. `missions/bw` — dòng **đầu tiên** của `main()` là `bwmanager.lua:7`
>    `do Talk(1, "", "Chức năng đã đóng.") return end` (**không có `--`**) ⇒ toàn bộ cây là mã chết.
> 2. `missions/arena` — thiếu **5 mảnh**: giao thức báo danh đã comment 2 nơi; `apply_signup`,
>    `on_player_enter_map`/`on_player_leave_map`/`on_begin_battle` **không tồn tại**;
>    **lớp `tbMember` (giữ điểm rank) KHÔNG TỒN TẠI** dù `rule.lua:101` gọi; `common.lua` không ai Include.
>
> Chi tiết cách hoạt động + điều kiện tham gia của cả 9 hoạt động: **`DIEUKIEN_THAMGIA_9_HOATDONG.md`**.

**"Tín sứ" = 信使** → `task/tollgate/messenger` + `item/messenger` (NPC **Dịch Quan**).
⚠ Đừng nhầm với `event/tongwar/npc_shizhe.lua` (使者) — cái đó là **"Sứ giả"** của Bang Chiến, không phải Tín Sứ.

---

## 3. CƠ CHẾ HOẠT ĐỘNG TỪNG TÍNH NĂNG

### 3.1 L1 — LÔI ĐÀI TỶ VÕ (`missions/bw`)

**Ý tưởng:** hai đội tự do đấu nhau công khai giữa thành, ai cũng vào xem được.

```
NPC "Công Bình Tử" (bwmanager.lua) đứng ở thành
  │ Bang/nhóm A báo danh → chọn cỡ trận 1v1 … 8v8  (SignUpFinal(n), bwmanager.lua:49)
  │ Bang/nhóm B báo danh vế còn lại
  │ Mỗi đội trưởng nhận 1 SỐ THỨ TỰ ngẫu nhiên (BW_KEY, 0..100.000.000)
  ├─ 2 phút báo danh (GO_TIME = 6 nhịp × TIMER_1 20s)
  │    đội viên nhập ĐÚNG số thứ tự của đội trưởng mới được vào  (OnEnterKey)
  ├─ vào map 209/210/211, camp 1 = "bên Giáp", camp 2 = "bên Ất", camp 3 = KHÁN GIẢ
  │    khán giả bị ChangeOwnFeature(0,0,-1,…) = TÀNG HÌNH  (bwhead.lua:158)
  │    hai bên vào 2 kênh chat riêng (CreateChannel/EnterChannel)
  ├─ đánh tối đa 12 phút (TIMER_2 = 12 × 3 × TIMER_1)
  └─ hết người một bên HOẶC hết giờ → GameOver() trả về vị trí lưu ở task 300/301/302
```

| Hằng số | Giá trị | Nguồn |
|---|---|---|
| Mission ID | **4** | `bwhead.lua:40` |
| Timer task | **10, 11** | `settings/task/timertask.txt` dòng 10-11 bản Linux |
| Map | **209 / 210 / 211** | `bwhead.lua:8` |
| Trần người | 100 | `bwhead.lua:21` |
| Task lưu vị trí | 300 / 301 / 302 | `bwhead.lua:23-25` |
| Điểm hồi sinh | thành 80 (Dương Châu) / 78 (Tương Dương) / 6 | `bwhead.lua:42-49` |

**Hàm engine thiếu (6):** `CreateChannel`, `EnterChannel`, `LeaveChannel`, `DeleteChannel`, `ForbidEnmity`, `ChangeOwnFeature`.
4 hàm kênh chat chỉ phục vụ **chat riêng theo phe** — bỏ đi thì trận vẫn chạy; đó là con đường "bản tối thiểu".

---

### 3.2 L2 — BÁCH NHÂN LÔI ĐÀI (`missions/bairenleitai`)

**Ý tưởng:** một map (960) có **5 đài**; ai bước lên đài trống thì thành **Lôi Chủ**; người khác bước lên là **khiêu chiến**; thắng thì lên bậc, thua thì bị đá xuống.

```
Trap ô vuông trên mỗi đài  →  HundredArena:InTrap(index)      (hundred_arena.lua:465)
   nFightState = 0 (đài trống)
      → người bước vào thành LÔI CHỦ, nGrade = 1, SetTmpCamp(1)
        hẹn giờ 30 giây chờ người khiêu chiến (tbTimerInfo.nApply)
   nFightState = 1 (có lôi chủ, chưa có người thách)
      → người thứ hai thành VISITOR, SetTmpCamp(2), đếm ngược 3 giây
      → hết 30 giây mà không ai lên: hệ thống thả NPC "Cao thủ <10 phái>"
        (npcId 1786-1795, cấp 90, ngũ hành theo phái) làm đối thủ  (ArenaField:CallFightNpc)
   nFightState = 2 → đánh, tối đa 3 phút (tbTimerInfo.nFight)
      thắng → nGrade + 1, giữ đài; thua → ArenaField:Leave đá ra 1 trong 4 góc
```

| Hằng số | Giá trị | Nguồn |
|---|---|---|
| Map | **960** (`特殊用地\leitai`) | `hundred_arena.lua:23` |
| Số đài | 5, toạ độ In/Out/Npc/Revival đủ | `head.lua:10-79` |
| Chờ lôi chủ / chuẩn bị / đánh / điều phối | 30s / 3s / 3 phút / 1 phút | `head.lua:82-88` |
| Ở lì tối đa / hiệu ứng nhân đôi | 90 phút / 30 phút | `head.lua:192-193` |
| Task lưu vị trí ra | 2323 / 2324 / 2325 | `head.lua:188-190` |
| Task trần kinh nghiệm ngày | 2709 | `hundred_arena.lua:32` |
| NPC phụ | Cổ Thủ (1571, đánh trống buff), Xử Vụ Hương, tiệm thuốc | `head.lua:99-185` |

**Hàm engine thiếu (4):** `Msg2Map` (×19), `SetTmpCamp`, `AddMapTrap`, `AddNpcEx`.
Thư viện phải port kèm: `lib/timerlist.lua`, `lib/player.lua`, `lib/remoteexc.lua`, `missions/basemission/lib.lua`, `lib/file.lua`.
Dữ liệu bảng toạ độ: `\settings\maps\missions\bairenleitai\*.txt` — **11 tệp, dự án CHƯA CÓ**.

---

### 3.3 L3 — CẢNH KỸ TRƯỜNG (`missions/arena`)

**Ý tưởng:** báo danh **từ bản đồ nhỏ góc phải màn hình**, hệ thống ghép cặp 1v1 vào **phó bản riêng**, tính điểm ELO + danh hiệu tuần.

```
Báo danh (client, chuột phải bản đồ nhỏ)  →  protocol.lua  →  ghép cặp
   → PreApplyDungeonMap(975) sinh 1 BẢN SAO map 975 cho riêng cặp đó
   → OnEnterMap: SetTmpCamp(1|2), SetPKFlag(1), khoá tổ đội/bày sạp/thù hằn
   → 60 giây chuẩn bị (READY_TIME) → đếm ngược 5 → 5 → đánh 5 phút (FIGHT_TIME)
   → hạ gục HOẶC hết giờ so ST_GetDamageCounter (ai CHỊU ít sát thương hơn thì thắng)
   → tbPlayer:SetResult(...) đổi TSK_RANK theo ELO, ghi WriteLog, trả về vị trí cũ
```

| Task cá nhân | Ý nghĩa |
|---|---|
| 3172 | Rank (điểm ELO) |
| 3173 / 3174 / 3175 | thắng / hoà / thua |
| 3176 / 3177 | tuần hiện tại / số trận trong tuần |

9 danh hiệu theo mốc rank (`common.lua:13-24`): 2600+ **Độc Cô Cầu Bại**, 2400 Võ Lâm Chí Tôn, 2200 Nhất Đại Tông sư, 2000 Tuyệt Thế Cao Thủ, 1800 Vang Danh Giang Hồ, 1600 Hiệp Danh Viễn Bá, 1400 Võ Lâm Tân Tú, 1200 Mới nhập giang hồ, dưới nữa Nhập Môn Đệ Tử → id danh hiệu 237-245 (`npc/officer.lua:8-19`).
Đủ **20 trận/tuần** → thưởng 100.000.000 exp (`officer.lua:41`, phần trao danh hiệu đã bị VNG chú thích tắt). Shop huy chương = `Sale(175, 16)`.

**Hàm engine thiếu (8):** `Msg2Map`, `RemoteExecute`, `ForbidEnmity`, `SetTmpCamp`, `SetDeathType`, `AddTimer`, `ClearMapNpcWithName`, **`PreApplyDungeonMap`**.
🔴 `PreApplyDungeonMap` kéo theo **cả khung phó bản `missions/basemission/dungeon.lua` + `settings/dungeonmap.ini`** — dự án **chưa có gì tương đương**. Đây là tính năng **đắt nhất trong nhóm lôi đài**.

---

### 3.4 L4 — LÔI ĐÀI BANG HỘI (`missions/citywar_arena`) — ĐÃ CÓ, ĐANG NGỦ

Bản gốc: **bang chủ** hai bang gặp NPC quản lý, **mỗi bên đặt cược 1000 vạn lượng**, 10 phút cho thành viên vào đấu trường, đánh 25 phút; **giết địch +3 điểm, bị giết −1 điểm**; bên nhiều điểm hơn thắng → **thu lại tiền cược + 800 vạn thưởng + 12.000 điểm kinh nghiệm bang**; bên thua **mất cược và −14.000 exp bang** (`camper.lua:44` — nguyên văn thoại).

| | Bản gốc Linux | Dự án |
|---|---|---|
| Script | `missions/citywar_arena/*` | ✅ **trùng 9/10 tệp từng byte** |
| Mission ID | 5 | **9** (`head.lua:36`, đổi ở đợt E vì 5 là Kiếm Môn Quan) |
| Timer task | 16, 17 | ✅ đã khai trong `settings/TimerTask.txt` |
| Map | 213-220 (`特殊用地\帮会擂台`) | ✅ có trong `WorldSet` + `maps.pak` (52 tệp region) |

**Vướng: 3 điểm nối dây đang bị chú thích tắt** (kết luận đợt E: *"Lôi đài 213-220: đăng ký đủ, idle như nhánh VN gốc — bật = đợt riêng"*).

| Điểm | Trạng thái |
|---|---|
| `script/startgame/thon/balanghuyen.lua:80` | `-- AddNpcNew(373,…,"\script\tinhnang\loidai\vebinhdautruong.lua",…,"Lôi Đài Bang Hội")` **đã comment** |
| `script/timerserver.lua:76` | `-- sukien_loidaibanghoi(nDyfW,nHr,nMi)` **đã comment** |
| `script/item/lenhbaiadmin.lua:25` | `-- Include(".../loidai/lib_loidai.lua")` **đã comment** |

**Phát hiện đáng giá:** `script/tinhnang/loidai/lib_loidai.lua` (bản tự viết 2021 của Fong Kiều) **chính là bản chép tay của `citywar_arena/head.lua`** — trùng nguyên `MS_TONG1Name=10 / MS_TONG1VALUE=13 / MS_ARENAID=15 / PKWINBONUS=3 / LOSEBONUS=1 / WIN_TONGEXP=1200 / LOSE_TONGEXP=1400 / MAX_MEMBER_COUNT=16 / WORLDIDXBEGIN=213 / CampPos1={1536,3223} / CampPos2={1563,3195}`. Khác biệt: bản tự viết **ghi danh sách bang ra tệp `.lua` trên đĩa** (`SaveData("script/tinhnang/loidai/danhsach_bang.lua")`) thay vì dùng League, và gắn kết quả vào Công Thành Chiến ("lôi đài tranh quyền công thành").
⇒ **Không cần viết mới gì. Chỉ cần quyết dùng nhánh nào rồi bật dây.** Đây là việc rẻ nhất trong cả tài liệu.

---

### 3.5 T1 — TÍN SỨ (`task/tollgate/messenger`)

**Ý tưởng gốc:** nhận thư ở Dịch Quan thành A → vượt **một trong ba ải** → giao thư ở Dịch Quan thành B.

```
NPC DỊCH QUAN (posthouse.lua) — chỉ ở THÀNH ĐÔ (11) và ĐẠI LÝ (162)
  ├─ "Ta bằng lòng!"                     → messenger_ido   (yêu cầu cấp ≥ 120)
  │     kiểm số lần trong ngày: check_daily_task_count()  (posthouse.lua:1089)
  │        < 2 lần   → cho nhận thường
  │        = 2 lần   → phải có "Thiên Khố Bảo Lệnh" (6,1,2813) mới được lần 3
  │        ≥ 3 lần   → hết lượt
  │     đặt task 1204 = tuyến đi, task 1203 = 10 (đã nhận, chưa vào ải)
  ├─ "Ta đến để giao nhiệm vụ Tín sứ"    → messenger_finishtask → messenger_treasureprize
  ├─ "Ta muốn xóa bỏ nhiệm vụ Tín sứ"    → messenger_losemytask
  └─ "Ta muốn biết về nhiệm vụ Tín sứ!"  → messenger_what

XA PHU đứng cạnh (wagoner.lua) → messenger_storewagoner → NewWorld(395, 1417, 3207)

TRONG ẢI (map 395, Thiên Bảo Khố):
  NPC cửa vào (messenger_turerukou.lua)
    "Bắt đầu nhiệm vụ" → ture_try_starttask
        · tổ đội thì CHỈ ĐỘI TRƯỞNG bấm được, cả đội cùng nhận 1 mã
        · get_task_order(): bốc NGẪU NHIÊN 5 số trong 9 rương → mã 5 chữ số
        · task 1203 = 20 (đang làm), task 1201 = mã, task 1202 = tiến độ mở rương
        · SetPos(1414,3191) + bất tử 3 giây (SetProtectTime + AddSkillState 963)
    Người chơi phải MỞ 5 BẢO RƯƠNG ĐÚNG THỨ TỰ  (messenger_baoxiangtask.lua)
        add_task_step(n): task1202 = task1202*10 + n; sai thứ tự = hỏng lượt
  NPC cửa ra "Tiêu Trấn" (qianbaoku/messenger_turenpc.lua) → task 1203 = 30
  Rời ải: ture_movecity → về Dịch trạm thành đích (citygo[i][7..9])

TRẢ NHIỆM VỤ (messenger_treasureprize, posthouse.lua:478)
  · cần 5 ô trống hành trang
  · lần đầu trong ngày: +3 "Hành Hiệp Lệnh" (6,1,2566)
  · mỗi lần: +2 "Tín Sứ Bảo Rương" (6,1,2812)
  · tối đa 2 lần/ngày kinh mạch (task daily 3073) +2 nguyên liệu (6,1,30229)
  · bắn sự kiện EventSys "Messenger":OnFinish  →  hệ thành tựu / hoạt động
```

**Bảng biến nhiệm vụ (task) — dùng cho auto/WAuto sau này:**

| Task | Ý nghĩa |
|---|---|
| 1201 | ải Phong Chi Kỵ (bản chạy: bị tái dụng làm **mã thứ tự mở rương**) |
| 1202 | ải Sơn Thần Miếu (bản chạy: **tiến độ mở rương**) |
| 1203 | ải Thiên Bảo Khố — **0 chưa nhận · 10 đã nhận · 20/21 đang làm · 25/30 xong** |
| 1204 | tuyến đi (chỉ số trong bảng `citygo`) |
| 1205 | điểm tích luỹ Tín Sứ · 1206 danh hiệu (1 Mộc → 5 Ngự Tứ) |
| 1211 | thời điểm bắt đầu · 1218 số lần hoàn thành · 1221-1224 bộ đếm ngày |

**Năm lệnh bài Tín Sứ** (`item/messenger/toll_*xinshi.lua`): Mộc/Đồng/Ngân/Kim/Ngự Tứ — phải có **task 1206 ≥ 1/2/3/4/5**, mỗi cái dùng **5 lần** thì hỏng, hồi 10/15/20/25/30 điểm nội lực trong 1 giờ + kháng ngũ hành (`AddSkillState 542-546` + `631-635`).

🔴 **Cực kỳ quan trọng — bản VNG đang chạy đã BỊ CẮT rất nhiều so với bản gốc:**

| Bản gốc thiết kế | Bản Linux đang chạy |
|---|---|
| 7 thành × 6 tuyến = 42 tuyến | **2 tuyến** (Thành Đô ↔ Đại Lý), `posthouse.lua:87-91`, 40 dòng còn lại đã comment |
| 3 ải: Phong Chi Kỵ · Sơn Thần Miếu · Thiên Bảo Khố | **chỉ Thiên Bảo Khố**, `wagoner.lua:25-44` comment 2 ải |
| 3 mức bản đồ theo cấp (60-89 / 90-119 / 120+) → map 387-395 | **chỉ map 395**, ép cấp ≥ 120 |
| Đồng hồ 7200 giây/ngày, phạt quá giờ | **toàn bộ `messenger_timeer.lua` bị comment** = không giới hạn giờ |
| Nâng danh hiệu Tín Sứ bằng điểm | `messenger_getlevel` **comment** — task 1206 không bao giờ tăng ⇒ **5 lệnh bài thành đồ chết** |

⇒ **Quyết định bắt buộc trước khi port:** phục dựng bản GỐC ĐẦY ĐỦ (3 ải, 42 tuyến, 3 mức cấp — dữ liệu map **đã có sẵn**), hay chép đúng hiện trạng VNG (1 ải, 2 tuyến)? Đây là câu hỏi phải hỏi chủ game, không phải quyết định kỹ thuật.

**Hàm engine thiếu:**
| Hàm | Ở đâu | Có bắt buộc không |
|---|---|---|
| `SetSpecItemParam` | 5 tệp `item/messenger/*` | ❌ chỉ cho 5 lệnh bài phụ (đang là đồ chết) |
| `ConsumeEquiproomItem` | `posthouse.lua:503` | ⚠ chỉ khi bật lượt thứ 3 bằng Thiên Khố Bảo Lệnh; dự án đã có `ConsumeItem` + `CalcEquiproomItemCount` |
| `NpcName2Replace` | `messenger_turenpc.lua:23` | ❌ đã có `if NpcName2Replace then` bảo vệ |

**⇒ Lõi Tín Sứ: 0 hàm engine phải viết.**
⚠ `SetItemParam` của dự án (`ScriptFuns.cpp:5171`) **chỉ xử lý `nKind == 1` (số lượng chồng)** — nó KHÔNG phải ô tham số tự do như `SetSpecItemParam` của JX2. Đừng tưởng là có sẵn.

**🔴 Va chạm:** `script/tinhnang/thienbaokho` (Ken Nguyen, 7 tệp) **ĐANG CHẠY** (`startgame.lua:106 addnpcthienbaokho()`), sinh NPC **trên chính map 395**: Dịch Quan (377), **"Tiêu Trấn"** (đúng tên NPC cửa ra bản gốc), 12 "Bảo Rương" (1828) và 12 "Bảo khố thủ hộ giả" (845). Port Tín Sứ = **thay thế** tính năng đang chạy, phải gỡ dây cũ trước.

---

### 3.6 B1 — BA HOẠT ĐỘNG PHƯỜNG BANG HỘI (`missions/tong`)

**Đây là món đang HỎNG NGAY LÚC NÀY.** `scriptjx2/tong_vn/workshop/ws_huodong.lua:274-283` có 9 dòng `Include` trỏ vào `\script\missions\tong\...` — cây đó **không tồn tại**, và ghi chú 15/08 ngay trong tệp:

> *"cay \script\missions\tong\ KHONG TON TAI trong ban port nen 9 dong Include duoi day deu hong IM LANG → moi ham cua 3 hoat dong thanh nil → kich vao Tong quan Hoat dong phuong la bao loi script (ke ca bam nut Huy). KHONG chep cay mission tu ban goc sang: JX1 thieu >30 ham engine JX2…"*

Con số thật **không phải >30 mà là 17** (mục 5) — ghi chú cũ ước lượng bi quan.

**Cổng vào chung** (`missions/tong/tong_head.lua:30` `festival_tongjudge`):
```
NPC "Tổng quản Hoạt động phường" (tong/npc/huodong_zongguan.lua — ĐÃ CÓ trong tong_vn)
  → TWS_GetLevel(bangID, phườngID) phải trong 1..10  (chưa xây phường thì từ chối)
  → mỗi ngày tối đa 20 lượt/bang, đếm bằng TWS_ApplySetTaskValue(HUODONG_TASKID)
  → kiểm điểm cống hiến cá nhân: 800 (chiêu mộ đệ tử) / 500 (niên thú) / 300 (thu thập)
  → vào SÂN CHUẨN BỊ; < 5 người thì huỷ (cancelgame)
  → chaos(): xáo trộn và chia NHÓM 10 NGƯỜI  →  gogamemap(): trừ cống hiến rồi đẩy vào map
```

| Hoạt động | Mission chuẩn bị / chính | Map | Timer | Số người | Lượt/ngày |
|---|---|---|---|---|---|
| Chiêu mộ đệ tử (`tong_disciple`) | **37 / 38** | 821 / 822 | 65,66,67 | 10 mỗi sân, 120 tổng | 5 |
| Niên Thú Tết (`tong_springfestival`) | **39 / 40** | 823 / 824 | 68,69,70 | 10 mỗi sân, 100 tổng | 5 |
| Thu thập vật tư (`collectgoods`) | **44 / 45** | 827 / 828 | 75,76,77 | 10 mỗi sân, 120 tổng | 5 |

Lối chơi `collectgoods`: 15 giây thả một đợt "tinh linh"/địa thử, người chơi đập; **1 người ngẫu nhiên được gắn quả bom** (`collg_settobomber`: `AddTempMagic(736,10)` + `SetImmedSkill` + trạng thái 705) nổ 30 giây/lần; ghi điểm bằng task 2404, xếp hạng bằng task 2408.

**17 hàm engine thiếu:** `ForbitSkill` `SetMoveSpeed` `ForbitAura` `ForbidEnmity` `FileName2Id` `AddMapTrap` `SetSpecItemParam` `IniFile_Load` `IniFile_GetData` `SetAForbitSkill` `AddTempMagic` `SetImmedSkill` `ClearMapObj` `ClearMapNpcWithName` `TaskNo` `ChangeOwnFeature` `GetMapNpcWithName`.

---

### 3.7 B2 — CỔ THÁP BANG HỘI (`missions/tong_guta`)

**Phó bản 6 ải boss dành riêng cho một bang**, có bảng xếp hạng và **giao diện client riêng**.

- Map mẫu **996** (`特殊用地\guta`), sinh bản sao qua `PreApplyDungeonMap` (`guta_config.lua:7,14`).
- **25 ≤ người ≤ 60**, cho phép tối đa **15 "người hỗ trợ"** ngoài bang (`MIN/MAX_HELPER_PLAYER`).
- Điểm: vào trận 200 · qua ải còn sống 300 · kết liễu boss ải cuối 1000 · người hỗ trợ 150.
- 6 ải, mỗi ải một cơ chế riêng (mô tả nguyên văn trong `guta_config.lua:39-71`): Dung Hạ (Ngũ Thánh — để 3 loại cùng lúc là thua), Chiêm Bình (tứ đại thần thú), Vô Tình + Lãnh Huyết (2 boss không được đứng gần nhau), Hoàn Nhan Quân Sĩ (Hắc Y Nhân theo ngũ hành tương khắc), Quách Nham (3 giai đoạn Phong/Hoả/Thần Thí + thu thập Thanh Thủy Mẫu Hoa), Trương Phiền (3 pha).
- NPC báo danh "Kinh Thanh" (2273) đặt tại Thành Đô (11, 3100, 5050).
- Chạy **liên GameServer**: `guta_relay.lua` dùng `RemoteExecute`, `NewCommonShareData`, `IsGameServerReady`, `GameServerId2ConnectIdx`, `GetHostPlayerCount`.
- Chia đồ bằng **roll xúc xắc** (`ApplyItemDice` / `AddDiceItemInfo` / `GetItemDiceItemInfo`).

**35 hàm engine thiếu — nhiều nhất trong tất cả.** Và **2 chặn cứng ngoài mã nguồn**:

1. 🔴 **KHÔNG CÓ DỮ LIỆU BẢN ĐỒ.** `\maps\特殊用地\guta.wor` **không có trong pak Linux (95.366 mục) LẪN pak dự án (87.245 mục)**. Tính năng này chưa từng được VNG mở trên cây server này.
2. 🔴 **Map 996 đang bị PUBG chiếm** (`MapList.ini: 996=pubg\bienkinhpubg`, có tệp rời `Maps/pubg/bienkinhpubg.wor`).
3. Cần thêm **giao diện client** (`GuTaUi_Open/SetData/AppendRankData/ClearLevelInfo/ClearRankData`) — dự án không có.

**⇒ Khuyến nghị: LOẠI Cổ Tháp khỏi phạm vi** cho tới khi có bộ art bản đồ. Đây là đề mục duy nhất trong tài liệu này bị chặn bởi thứ không nằm trong mã nguồn.

---

### 3.8 B3 — BANG HỘI THÀNH BẢO (`missions/tongcastle`)

Lãnh địa riêng của bang trên map **984** (`特殊用地\banghuichengbao`, bản gốc gán cho Lâm An 176).

- Khung `Game`/`Dungeon` (`game.lua`), loại map `"Bang hội thành bảo"`, **cấm** đồ TRANSFER/MATE/CALLNPC.
- Bốn hướng Đông/Nam/Tây/Bắc, mỗi hướng có bảng trap + bảng vật cản riêng (`\settings\maps\tongcastle\*.txt` — **11 tệp, dự án CHƯA CÓ**).
- **Vệ binh tối đa 100** (`GUARDLIMIT`), người dẫn đường trong/ngoài, **Thần Mộc Lệnh** (`shenmuling.lua`), cây thần bị đánh (`treedeath.lua` dùng `NPCINFO_Get/SetNpcCurrentLife`).
- Có bộ đếm thời gian ở lì (`TSK_STAY_TIME = 4056`).

**17 hàm engine thiếu.** 🔴 `\maps\特殊用地\banghuichengbao.wor` **CÓ trong pak Linux, KHÔNG có trong pak dự án** ⇒ phải trích từ pak Linux sang (đúng quy trình đợt E đã làm cho Kiếm Môn Quan), và **map 984 chưa khai trong `WorldSet_GameServer.ini`**.

---

### 3.9 B4 — BANG CHIẾN (`missions/tongwar` + `event/tongwar`)

Trận **liên minh bang hội** quy mô lớn, chạy trên **hệ League** (giống Liên Đấu).

```
event/tongwar/npc_shizhe.lua  ("Sứ giả", 363 dòng)
   → bang chủ đăng ký, lập LIÊN MINH (LG type 10)   headinfo.lua:10
   → mỗi liên minh giữ LGTASK: số bang, thắng/thua/hoà, tổng điểm, id thành, phe, map, hạng
missions/tongwar/head.lua
   → 30 phút báo danh (RUNGAME_TIME) rồi tự chuyển sang pha đánh
   → tối đa 150 người/phe, tối thiểu 5 (MAX/MIN_MEMBERCOUNT)
   → đánh 90 phút (TIMER_2 = 15*6*60*18)
   → giết người +75 điểm, chuỗi liên trảm tối đa +150 (BONUS_KILLPLAYER / BONUS_MAXSERIESKILL)
   → quân hàm 6 bậc theo điểm tích luỹ: 0 / 10.000 / 20.000 / 40.000 / 60.000 / 80.000
        Binh Sĩ · Hiệu Úy (+20% sinh lực) · Thống Lĩnh (+30% HP, +5% thủ)
        · Phó Tướng (+40%, +10%) · Đại Tướng (+50%, +15%) · Nguyên Soái
        (áp bằng RANK_SKILL = 661; hệ số PK chéo bậc ở RANK_PKBONUS)
   → chết quá TONGWAR_MAXDEATH = 10 lần thì bị loại lượt
```

| | |
|---|---|
| Mission ID | **33** |
| Timer task | **61, 62** |
| Map | báo danh 605-607, chiến trường 608-613 (3 bộ) |
| Task cá nhân | 2369-2378 (điểm, số chết, số giết, liên trảm…), 1737 (cờ đã nhận thưởng) |
| Thưởng | Quả Đại Hoàng Kim (30438), Hoàng Chân Đơn (2264), Hỗn Nguyên Chân Đơn (30301), Cuồng Lan 5% (4862), Phi Phong Cấp Phù Quang (3477) — `headinfo.lua:147-154` |

**16 hàm engine thiếu.** 🟢 **Phần đắt nhất đã có sẵn**: hệ League `LG_`/`LGM_` (28 hàm) + `Ladder_*` + persist `jx2league.txt`/`jx2ladder.txt` đã được port ở đợt E và dùng thật cho Liên Đấu.
Trong 16 hàm đó, 4 hàm kênh chat (`Create/Enter/Leave/DeleteChannel`) chỉ để chat theo phe — bỏ được.

---

## 4. TRẠNG THÁI TÀI NGUYÊN (đo thật, không suy đoán)

### 4.1 Bản đồ — hầu như đã sẵn sàng

`WorldSet_GameServer.ini` của dự án đang nạp **909 map**, và `settings/MapList.ini` **về cơ bản chính là MapList của bản Linux**.

| Map | Dùng cho | WorldSet | Dữ liệu trong pak dự án |
|---|---|---|---|
| 209-211 | Lôi đài tỷ võ | ✅ | ✅ `中原南区\演武场一` — 99 tệp `_Region_S` |
| 213-220 | Lôi đài bang hội | ✅ | ✅ `特殊用地\帮会擂台` — 52 tệp |
| 387-389 | Tín Sứ · Phong Chi Kỵ | ✅ | ✅ 127 tệp |
| 390-392 | Tín Sứ · Sơn Thần Miếu | ✅ | ✅ 99 tệp |
| 393-395 | Tín Sứ · Thiên Bảo Khố | ✅ | ✅ 192 tệp |
| 605-607 | Bang chiến (báo danh) | ✅ | ✅ 336 tệp |
| 608-613 | Bang chiến (chiến trường) | ✅ | ✅ 105 tệp |
| 821-828 | 3 hoạt động phường | ✅ | ✅ 40-123 tệp mỗi map |
| 960 | Bách Nhân Lôi Đài | ✅ | ✅ `特殊用地\leitai` — 222 tệp |
| 975 | Cảnh Kỹ Trường | ✅ | ✅ trong `jingjichang.mps` — 32 tệp |
| **984** | Thành Bảo bang hội | ❌ **chưa khai** | ❌ **thiếu** (có ở pak Linux → trích được) |
| **996** | Cổ Tháp | ⚠ **đang là PUBG** | ❌ **không có ở CẢ HAI pak** |

Dải ID còn trống để dời map nếu cần: **598-604 · 871-874 · 886-916 · 951-958 · 978-985 · 987-990 · 996-999**.

### 4.2 Bảng cấu hình (`settings\`)

| Đường dẫn | Bản Linux | Dự án |
|---|---|---|
| `settings/tong/` + `settings/tong/workshop/` | 8 + 9 tệp | ✅ **đã có đủ** |
| `settings/task/tollgate/messenger/messenger_tollprize.txt` + `tollgate_allprize.txt` | 2 tệp | ❌ thiếu |
| `settings/maps/missions/bairenleitai/` | 11 tệp | ❌ thiếu |
| `settings/missions/arena/` (readypos.txt, battlepos.txt) | 2 tệp | ❌ thiếu |
| `settings/maps/tongcastle/` | 11 tệp | ❌ thiếu |
| `settings/dungeonmap.ini`, `settings/forbititem.ini` | có | ❌ thiếu |

### 4.3 Khe Mission và Timer

`settings/task/missions.txt` **tra theo SỐ DÒNG** — cấm xoá dòng, chỉ được nối thêm.
Hiện dự án có **26 dòng**; khe **13-23** đang là `mission_trong.lua` (dùng lại được ngay).

| Cần | Linux dùng | Dự án |
|---|---|---|
| Lôi đài tỷ võ | mission **4** | ❌ **kẹt** — `mission04.lua` đang sống ⇒ phải dời sang khe 13-23 |
| Bang chiến | mission **33** | phải nối `missions.txt` tới dòng 33 |
| 3 hoạt động phường | mission **37/38, 39/40, 44/45** | phải nối tới dòng 45 |
| Lôi đài bang hội | mission 5 | ✅ đã dời sang **9** |

`settings/TimerTask.txt` **tra theo KHOÁ**, thừa thiếu dòng đều được. Dự án đang dùng 1-10, 12-18, 50-52.
Các khe cần thêm **đều còn trống**: **11** (bw), **61-62** (bang chiến), **65-70** và **75-77** (3 hoạt động phường).

---

## 5. BẢNG 69 HÀM ENGINE CÒN THIẾU

Cột "Nhóm": `TS` tín sứ · `LD1` lôi đài tỷ võ · `LD2` bách nhân · `LD3` cảnh kỹ trường · `BH1` hoạt động phường · `BH2` cổ tháp · `BH3` thành bảo · `BH4` bang chiến.
Cột "Hàm gần nhất đã có" = gợi ý điểm bám, **chưa kiểm chứng ngữ nghĩa** — phải đọc thân hàm trước khi tin.

### 5.1 Nhóm A — đã có hàm gần tương đương (bọc/mở rộng, rẻ)

| Hàm thiếu | Nhóm | Hàm gần nhất dự án đã có |
|---|---|---|
| `AddMapTrap` | BH1 BH3 BH4 LD2 | `AddTrap` |
| `AddNpcEx` | BH2 BH3 LD2 | `AddNpc`, `AddMSNpc` |
| `AddTempMagic` | BH1 | `AddMagic`, `AddMagicPoint` |
| `ChangeOwnFeature` | BH1 LD1 | `ChangeNpcFeature`, `RestoreOwnFeature` |
| `ClearMapNpcWithName` | BH1 LD3 | `ClearMapNpc` |
| `ClearMapObj` / `ClearMapTrap` | BH1 BH4 | `ClearMapNpc`, `ClearObstacleObj` |
| `ClearNpcDamageState` | BH2 | `SetNpcDamage` |
| `ConsumeEquiproomItem` | BH2 TS | `ConsumeItem` + `CalcEquiproomItemCount` |
| `DisabledUseHeart` | BH2 | `DisabledUseTownP`, `DisabledStall` |
| `FindFreeRoomByWH` | BH2 | `CountFreeRoomByWH` |
| `ForbitAura` / `ForbitSkill` | BH1 | `ForbitTrade`, `ForbitStamina` |
| `GetAllEquipment` | BH4 | `GetEquipCount` |
| `GetAroundNpcList` | BH3 | `FindAroundNpc` |
| `GetHostPlayerCount` | BH2 | `GetPlayerCount` |
| `GetItemQuality` / `GetItemStackCount` / `SetItemStackCount` | BH2 BH3 | `GetItemCount`, `GetItemCountEx` |
| `GetMapNpcWithName` | BH1 | `GetNpcName` + `ClearMapNpc` |
| `GetNpcCurLife` / `GetNpcCurMaxLife` / `SetNpcCurLife` | BH2 | `GetNpcLife`, `SetNpcLife` |
| `GetNpcId` | BH2 BH3 | `GetNpcIdx`, `GetNpcID` |
| `GetTmpCamp` / `SetTmpCamp` | BH3 LD2 LD3 | `GetCamp`, `SetCamp`, `SetCurCamp` |
| `IniFile_Load` / `IniFile_GetData` | BH1 BH4 | `TabFile_Load`, `BT_GetData` |
| `Msg2Map` | BH2 BH3 LD2 LD3 | `Msg2Region`, `Msg2SubWorld` |
| `NpcCastSkill` | BH2 BH3 | `CastNpcSkill`, `CastSkill` |
| `NpcSetHide` | BH2 | `SetNpcSeries` (cùng họ cờ NPC) |
| `PIdx2NpcIdx` | BH2 | `PIdx2MSDIdx` |
| `RemoveNpcSkillState` | BH2 | `RemoveNpcSkill`, `AddNpcSkillState` |
| `SetDeathType` | BH2 BH4 LD3 | `SetDeathScript` |
| `SetImmedSkill` | BH1 | `SetNpcSkill` |
| `SetItemBindState` | BH2 | (có `AddItem` tham số bind) |
| `SetMoveSpeed` | BH1 | `SetPlayerSpeed` |
| `SetNpcActiveRegion` | BH2 | `SetNpcActiveRange` |
| `SetSpecItemParam` | BH1 TS | `SetItemParam` ⚠ **chỉ nhận `nKind==1`** |
| `SubWorldIdx2MapCopy` | BH2 | `SubWorldIdx2ID` |
| `TaskNo` / `Tm2Time` | BH1 BH4 | `TaskName` / `TaskTime` |
| `AddTimer` / `DelTimer` | BH2 BH3 LD3 | `SetTimer`, `TM_SetTimer` |
| `ITEM_SetExpiredTime` | BH2 | `AddTimeItem` |

### 5.2 Nhóm B — KHÔNG có gì tương đương (phải viết mới)

| Hàm | Nhóm | Ghi chú |
|---|---|---|
| `CreateChannel` `EnterChannel` `LeaveChannel` `DeleteChannel` | LD1 BH4 | hệ kênh chat theo phe — **bỏ được** ở bản tối thiểu |
| **`PreApplyDungeonMap`** | BH2 LD3 | khung phó bản (bản sao map động) — kéo theo `basemission/dungeon.lua` + `dungeonmap.ini` |
| `RemoteExecute` | BH2 BH3 LD3 | gọi script liên GameServer. Dự án có `GlobalExecute` + `LG_ApplyDoScript` (đợt E) — có thể bọc lại |
| `IsGameServerReady` `GameServerId2ConnectIdx` `ConnectIdx2GameServerId` | BH2 | quản lý cụm GS |
| `ApplyItemDice` `AddDiceItemInfo` `GetItemDiceItemInfo` | BH2 | hệ roll xúc xắc chia đồ |
| `NPCINFO_GetNpcCurrentLife` `NPCINFO_SetNpcCurrentLife` | BH3 | họ `NPCINFO_` dự án có **0 hàm** |
| `GetNpcAroundNpcList` | BH3 | quét NPC quanh NPC (khác `FindAroundNpc` quanh người) |
| `ForbidEnmity` | BH1 BH2 BH4 LD1 LD3 | khoá thù hằn — **5/8 tính năng đều cần** |
| `SetAForbitSkill` | BH1 | cấm 1 chiêu cụ thể |
| `FileName2Id` | BH1 BH4 | băm tên tệp ra id (engine đã có hàm C `KPakList::FileNameToId`, chỉ chưa mở ra Lua) |
| `GetGlodEqIndex` | BH2 | tra chỉ số trang bị hoàng kim |
| `KillNpcWithIdx` | BH2 | |
| `MakeDateTime` | BH3 | |
| `NpcName2Replace` | BH3 TS | đổi tên NPC hiển thị; **cả 2 chỗ gọi đều có `if … then` bảo vệ** |
| `ST_SyncMiniMapObj` | BH4 | đồng bộ ký hiệu bản đồ nhỏ (client) |
| `GetMapInfoFile` | BH4 | |
| `TaskNo` | BH1 | |

---

## 6. XUNG ĐỘT VÀ RỦI RO — phải quyết trước khi gõ dòng mã đầu tiên

| # | Vấn đề | Chi tiết |
|---|---|---|
| 1 | 🔴 **Map 209** | `missions/bw` cần 209-211. Dự án đang chạy **Lôi Đài Hỗn Chiến** trên đúng map 209 (`tinhnang/loidaihonchien/mainloidai.lua:46 NewWorld(209,1628,3213)`, NPC 240 ở Ba Lăng Huyện, 16h và 22h, cấp ≥ 90). Hai thứ không sống chung được. |
| 2 | 🔴 **Map 395** | Tín Sứ dùng 395. Dự án đang chạy **Thiên Bảo Khố** tự viết trên đúng map đó, sinh 26 NPC lúc `startgame`. |
| 3 | 🔴 **Map 996** | Cổ Tháp cần 996, dự án đã gán cho PUBG. Cộng thêm việc **không có dữ liệu map** ⇒ đề nghị bỏ. |
| 4 | 🟡 **Mission 4** | Lôi đài tỷ võ cần mission 4, đang bị `mission04.lua` chiếm. Dời sang khe 13-23 (đúng cách đợt E đã dời 5→9). |
| 5 | 🟡 **`missions.txt` tra theo dòng** | Muốn dùng mission 33/37/44 phải **nối đủ dòng 27→45**, không được nhảy cóc. |
| 6 | 🟡 **Ba bản chép của cùng một tính năng** | Lôi đài bang hội hiện tồn tại **3 bản** trong cây dự án: `missions/citywar_arena` (bản gốc, ngủ), `tinhnang/loidai` (tự viết, đã tắt), `header/loidai.lua` (**mã chết, không ai `Include`** — đúng kiểu `header/liendau.lua` đã gặp ở phiên Liên Đấu). Đừng lấy `header/loidai.lua` làm điểm khởi đầu. |
| 7 | 🟡 **Bản VNG đã cắt xén** | Không chỉ Tín Sứ. Trước khi port bất kỳ cây nào, phải soi xem bản Linux có comment tắt phần nào không — chép nguyên xi có thể ra một tính năng "chạy mà không dùng được". |
| 8 | 🟡 **Mỗi tệp `.lua` một `lua_State`** | Luật engine của dự án (khác Linux dùng 1 state + pack). Mọi thứ Linux "đăng ký toàn cục lúc boot" phải kéo về từng state bằng `Include`, kèm guard chống đệ quy — đúng bài học của phiên Liên Đấu. |
| 9 | 🟡 **`missions/tong_leitai`** | `settings/task/missions.txt` bản Linux dòng 50 trỏ `\script\missions\tong_leitai\battle\mission.lua` — **thư mục đó không tồn tại**. Là mục chết, đừng đi tìm. |

---

## 7. ĐỀ XUẤT THỨ TỰ THI CÔNG (theo tỉ lệ giá trị / công sức)

| Bậc | Việc | Vì sao | Chặn |
|---|---|---|---|
| **1** | **Bật Lôi Đài Bang Hội** (`citywar_arena`) | Script + map + timer + mission **đã sẵn sàng 100%**, **0 hàm engine**. Chỉ gỡ 3 dòng comment và quyết bỏ bản tự viết. | quyết định vận hành |
| **2** | **Tín Sứ** | **0 hàm engine cho lõi**; dữ liệu 3 map đầy đủ trong pak dự án; task/NPC/thoại rõ ràng. | gỡ Thiên Bảo Khố tự viết; chốt "bản gốc đầy đủ hay bản VNG rút gọn" |
| **3** | **3 Hoạt động Phường** (`missions/tong`) | **Đang gây lỗi script thật** khi bấm NPC Tổng quản; nền bang hội + `settings/tong` đã có; 17 hàm mà **15 có hàm gần tương đương**. | nối `missions.txt` tới dòng 45 |
| **4** | **Bang Chiến** (`tongwar`) | Hệ League đắt nhất **đã có sẵn**; map đủ; 16 hàm, bỏ 4 hàm kênh chat còn 12. | khe mission 33 |
| **5** | **Bách Nhân Lôi Đài** | Chỉ **4 hàm**; map 960 đủ dữ liệu (222 region). | thiếu 11 tệp `settings/maps/missions/bairenleitai` |
| **6** | **Lôi đài tỷ võ** (`bw`) | 6 hàm, 4 trong đó bỏ được. | **va chạm map 209** với Lôi Đài Hỗn Chiến |
| **7** | **Thành Bảo bang hội** | 17 hàm, cần trích map 984 từ pak Linux + 11 tệp settings. | map chưa khai trong WorldSet |
| **8** | **Cảnh Kỹ Trường** | Cần dựng cả **khung phó bản** — hạ tầng mới, dùng lại được cho nhiều thứ về sau. | `PreApplyDungeonMap` |
| **—** | ~~Cổ Tháp Bang Hội~~ | **ĐỀ NGHỊ BỎ** | **không có dữ liệu bản đồ ở cả hai pak** + 35 hàm + cần UI client mới |

---

## 8. VIỆC CÒN PHẢI XÁC MINH (chưa làm trong phiên này)

1. **Ngữ nghĩa từng hàm ở nhóm A mục 5.1** — mới đối chiếu bằng TÊN, chưa đọc thân hàm bản Linux. Trước khi viết phải dịch ngược từng hàm ở `jx_linux_y` theo địa chỉ trong `ReverseTools/jx_linux_y.luamap.full.txt` (cách làm y như đợt Công Thành: `DIEUTRA_CONGTHANH_BINARY.md`).
2. **ID vật phẩm** — bảng vật phẩm sống của dự án lệch bản Linux (bài học Liên Đấu: *"bảng ta ≈ Linux TRỪ 1"*, đối chiếu bằng **TÊN** qua `magicscriptTQ`). Mọi id trong tài liệu này (2812, 2813, 2566, 30229, 30438, 2264, 30301, 4862, 3477, 1786-1795, 1828, 845…) **phải tra lại theo tên**, chưa kiểm.
3. **Va chạm task id** — dải task bản Linux dùng (1201-1224, 2323-2325, 2369-2378, 2392-2409, 2709, 3172-3177, 4056) chưa đối chiếu với bảng task đang dùng của dự án (`script/lib/lib_task.lua`). `MAX_TASK` hiện là 4200 nên **task 4056 vừa đủ lọt**, nhưng phải kiểm trùng.
4. **Va chạm skill id** — 542-546, 631-635, 661, 705, 736, 963, 976 chưa đối chiếu.
5. **Phía client** — Cảnh Kỹ Trường báo danh **từ bản đồ nhỏ**, Cổ Tháp có **cửa sổ riêng**; hai thứ này cần gói giao thức + UI mới, chưa khảo sát.
6. **Bảng `buysell.txt`** — Cảnh Kỹ Trường gọi `Sale(175,16)`, dự án chỉ có 101 shop (bài học Liên Đấu). Chưa kiểm.

---

## 9. TỆP THAM CHIẾU LIÊN QUAN

- `BANGIAO_CONGTHANH_DOTE.md` — đợt E, nguồn của `citywar_arena` đã port + câu *"Lôi đài 213-220: đăng ký đủ, idle"*
- `BANGIAO_BANGHOI_DOT12.md` — nền bang hội JX2 (`scriptjx2\tong_vn`, TWS_/TONG_/TONGM_)
- `BANGIAO_LIENDAU_THICONG.md` — khuôn mẫu port 1 hệ trọn vẹn (bao gồm cách xử lý 1-state-mỗi-tệp, missions.txt tra theo dòng, `dw`/`GlobalExecute`)
- `PHULUC_HAM_CONGTHANH.md` — mẫu bảng đích danh hàm engine, tick khi phản biện
- `ReverseTools/README.md` — cách chạy bộ công cụ dịch ngược
