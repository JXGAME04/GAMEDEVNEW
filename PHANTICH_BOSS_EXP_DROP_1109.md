# PHÂN TÍCH: hệ thống BOSS + tỉ lệ EXP / rơi đồ / chia đồ (11/09/2026)

Chủ yêu cầu: *"đọc mã nguồn hiện tại hệ thống boss có các loại nào để phân tích · tỉ lệ exp, tỉ lệ vật phẩm, tỉ lệ chia item"*.

Tài liệu CHỈ đọc — **không sửa gì**. Mọi con số lấy từ mã đang chạy: `D:\GAMEDEVNEW\Sources` (origin/main) và cây chạy thật
`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` (script + settings). Nhịp máy chủ **18 khung/giây** (mọi mốc "tick" quy ra giây theo đó).

---

## 1. CÓ NHỮNG LOẠI BOSS NÀO

### 1.1 Tầng engine — "boss xanh / boss vàng" gắn vào quái thường (C++)

`enum BOSS_STATE` (`GameDataDef.h:178-187`): `boss_none=0, boss_blue=1, boss_event=2, boss_gold=3, boss_war=4, boss_muter=5`.
Thực tế `KNpcGold::GetGoldType()` (`KNpcGold.cpp:251-260`) chỉ trả **0 / 1 (xanh) / 3 (vàng)**: loại nội bộ `m_nGoldType < 16` ⇒ xanh, `>= 16` ⇒ vàng
(`defNPC_GOLD_TYE = 16`).

| Việc | Mã | Chi tiết |
|---|---|---|
| Quay boss lúc NPC sinh ra | `KNpc.cpp:10661` → `KNpcGold::RandChangeGold` (`KNpcGold.cpp:149-160`) | `if (g_Random(500000) >= nzAutoGoldenNpc) return;` ⇒ **P(boss) = AutoGoldenNpc / 500.000** cho **mỗi lần một con quái được tạo** |
| Tỉ lệ theo TỪNG BẢN ĐỒ | `MapList.ini` `<id>_AutoGoldenNpc`, `<id>_GoldenType`, `<id>_GoldenDropRate`; đọc ở `KSubWorld.cpp:2138-2140` | 165 bản đồ có khai. Ví dụ map 1 = 2000 ⇒ **0,4 %**; map 2 = 1000 ⇒ **0,2 %** |
| Loại boss | `nzGoldenType` của map; nếu ≤ 0 thì `g_Random(số mẫu)`; script ép được bằng `SetNpcBoss(idx, n)` | mẫu trong `settings\npc\NpcGoldTemplate.txt` |
| Boss mạnh hơn ở đâu | `NpcGoldTemplate.txt` | hệ số nhân cho Exp, Life, LifeReplenish, AttackRating, Defense, Min/MaxDamage, **Treasure**, tốc độ, kỹ năng thứ 5, kháng 5 hệ |

### 1.2 Boss Hoàng Kim theo lịch (script `tinhnang\boss_hoangkim`, lịch gọi từ `timerserver.lua:801-817`)

| Loại | NPC | Giờ (`lib_bosshk.lua:64-88`) | Máu | Script chết / rơi đồ |
|---|---|---|---|---|
| **Tiểu Hoàng Kim** | Diệu Như 513, Liễu Thanh Thanh 523, Trương Tông Chính 511 | 15:00, 20:00 | `BHK_MAU_BOSS_TIEU` = 18.000.000 | `deathhktieu.lua` / `drophktieu.lua` |
| **Đại Hoàng Kim** | Huyền Giác Đại Sư 1365, Đường Phi Yến 1366, Từ Đại Nhạc 1367 | 11:00, 17:30 | `BHK_MAU_BOSS_DAI` = **1** ⚠️ (xem §5) | `deathhkdai.lua` / `drophkdai.lua` |
| **Boss máy chủ** ("Boss Cửu Thiên", npc 2009/2010) | map 53 | 12:00, 19:30 | như trên | `deathhksv.lua` / `drophksv.lua` (+ 10 **rương** npc 1828 rơi quanh xác) |
| **Boss Phong Lăng Độ** | npc 2009/2010, map 336 | gọi tay `addnpcbosspld` | như trên | `deathbosspld.lua` / `dropbosspld.lua` |

Thuộc tính chung: sống 120 phút rồi tự biến mất (`BHK_PHUT_BOSS_TON_TAI`), né tránh 5.000 (PLD ×2), hồi máu 300/nhịp, **giảm sát thương 70 %**
(`SetNpcDamageReduction`), kháng 95 % cả 5 hệ, `SetNpcBoss(idx, 5)` khi hồi sinh.

### 1.3 Boss phụ bản / hoạt động / nhiệm vụ (script riêng, không qua hệ Hoàng Kim)

- **Phụ bản**: `phuban\boss\bigboss.lua` (BigBoss — có bảng thưởng riêng cho người kết liễu, thưởng toàn server, drop chung),
  `phuban\fengling_ferry\` (Phong Lăng Độ: `boss.lua`, `bossdeath.lua`, `bigbossdeath.lua`), `phuban\yandibaozang\` (Viêm Đế bảo tạng),
  `phuban\challengeoftime\` (Vượt ải / Thí luyện), `phuban\bairenleitai\` (Bách nhân lôi đài), `phuban\tongcastle\`.
- **Chiến trường**: Nguyên soái Tống Kim (`tong_kim_tcap\nguyensoai.lua`), trụ/boss Công thành (`congthanhchien\death_tru.lua`).
- **Nhiệm vụ**: `nhiemvu\tollgate\messenger\` — boss tiêu xa / kho báu / sơn thần miếu chia theo cấp 60-79, 80-89, 90;
  `nhiemvu\tollgate\killer\` (sát thủ), `tinhnang\3hoatdong\` (Bắc Đẩu, săn boss sát thủ).
- **NPC cấp/nhiệm vụ**: `npclevelscript\` — `makeboss.lua`, `task_killboss.lua`, `randomtask_boss2.lua`, `partner_task_boss.lua`,
  `dragonboat_boss1/2.lua`, `highchallenge_boss.lua`, `lowchallenge_boss.lua`.
- **Triệu hồi bằng vật phẩm**: `vatpham\bosscharm.lua` — danh sách 20+ "Golden Boss" (562 Đạo Thanh Chân Nhân … 747 Tuyền Cơ Tử), chỉ dùng được ở
  một số bản đồ, cấm ở 58 bản đồ đặc biệt.

---

## 2. TỈ LỆ EXP

### 2.1 Exp khi giết quái — chia theo SÁT THƯƠNG, không phải ai kết liễu (`KNpcDeathCalcExp.cpp`)

- Chỉ áp cho NPC `kind_normal`. Mỗi con quái nhớ **tối đa 3 "khe"** người đánh (`defMAX_CALC_EXP_NUM = 3`); ai trong tổ đội thì **quy về đội trưởng**
  (một đội = một khe). Khe hết hạn sau `defMAX_CALC_EXP_TIME = 1200` nhịp ≈ **66 giây** không đánh tiếp.
- Khi quái chết: mỗi khe nhận **`exp = sát_thương_của_khe × m_Experience(quái) / máu_tối_đa`**. Ai đánh nhiều hơn được nhiều hơn; người đánh phát cuối
  chỉ quyết định "chủ đồ rơi", không quyết định exp.
- Điều kiện: phải trong bán kính `PLAYER_SHARE_EXP_DISTANCE = 768` (`GameDataDef.h:61`). Nếu người kết liễu không thuộc đội đó thì **người đứng gần xác
  nhất** của đội nhận thay.

### 2.2 Chia trong tổ đội (`KPlayer::AddExp`, `KPlayer.cpp:2383-2528`)

Đếm thành viên **cùng bản đồ + trong 768** rồi chia theo `gamesetting.ini [Exp]`:

| Số người | 2 | 3 | 4 | 5 | 6 | 7 | 8 | khác |
|---|---|---|---|---|---|---|---|---|
| % exp mỗi người | 80 | 70 | 60 | 55 | 55 | 50 | 50 | 60 |

Riêng **người trực tiếp gây sát thương** nhận **100 %** (không nhân %). Đội trưởng được cộng thêm exp thống lĩnh (`AddLeadExp`).

### 2.3 Hệ số nhân khi cộng vào người (`KPlayer::AddSelfExp`, `KPlayer.cpp:2530-2647`)

Thứ tự tính:

1. **Phạt chênh cấp**: cấp mình − cấp quái ≤ `ChenhCapMax = 9` ⇒ nguyên; hơn ⇒ **chia `ChiaKhiChenh = 10`**; quái **cao cấp hơn mình** ⇒ chỉ **1 exp**;
   từ `MienTruCap = 90` trở lên (cả người lẫn quái) ⇒ **bỏ hết phạt**.
2. **Bùa/trạng thái tăng exp** `m_CurrentExpEnhance` (%).
3. **Hệ số theo cấp** (`[Exp]`): `HeSo1 = 80` (cấp < 50), `HeSo2 = 70` (< 80), **`HeSo3 = 280` (< 140 — đang chi phối 8x-13x)**, `HeSo4 = 100` (≥ 140).
   VIP cộng thêm `VipCong = 20`. Chuyển sinh ≥ 3 lần và đứng map 341: `CsDuoi = 160` (< 140) / `CsTren = 50` (≥ 140).
4. **`ExpRate`** (`[ServerConfig]`) — hiện **= 1**.

⇒ Công thức thực tế: `exp nhận = (sát thương/máu × exp quái) × %tổ_đội × hệ_số_cấp(+VIP) × ExpRate × (1 + bùa)`.

### 2.4 Exp do script thưởng (boss, hoạt động)

Các script dùng `AddSumExp(...)` — **cộng thẳng, không qua hệ số §2.3** — với hằng số nhân `EXP_RATE = GLB_TILE_EXP = **20**`
(`lib_server.lua:28`, cấu hình `ch_chung.lua:126`; đây là "nút chính" chỉnh tốc độ lên cấp toàn server).

| Nguồn | Người kết liễu + tổ đội (trong bán kính 200) | Người lân cận khác đội |
|---|---|---|
| Boss Đại Hoàng Kim | 25.000.000 × 20 = **500 triệu** | 10.000.000 × 20 = **200 triệu** |
| Boss máy chủ | 20.000.000 × 20 = **400 triệu** | 5.000.000 × 20 = **100 triệu** |
| Boss Phong Lăng Độ | (cấu hình `BHK_EXP_LANCAN_BOSS_PLD` 20.000.000 × 20) | — |
| Rương boss máy chủ | `cấp^3 × 2` mỗi rương, tối đa **4 rương/ngày/người** | — |

⚠️ `BHK_EXP_NPC_BOSS_DAI` và `BHK_EXP_NPC_BOSS_TIEU` đều **= 0** ⇒ bản thân con boss Hoàng Kim **không cho exp theo sát thương**; toàn bộ exp đến từ
bảng trên, nên người vào đánh muộn/ít vẫn nhận đủ, còn người đánh nhiều không được thêm.

---

## 3. TỈ LỆ RƠI VẬT PHẨM

### 3.1 Đường đi

`NPC chết` → `KNpc::Die` (`KNpc.cpp:2192-2195`) → chạy Lua **`DropRate(npcIdx, người_đánh_phát_cuối)`** trong tệp `DropRateScript` của con NPC đó.
- Quái thường: `gamesetting.ini [NPC] NormalDropRate = \script\global\LuaNpcMonsters\Droprate_normal.lua` (gán cho mọi NPC lúc khởi tạo, `KNpcSet.cpp:522`).
- Boss: script riêng truyền vào `AddNpcNew(..., DROPRATEHK*)`.
- **Nhánh rơi đồ của C++ đã tắt**: `KNpc::DeathPunish` chỉ rơi khi `!g_NotAddNpcNormal`, mà `gamesetting.ini` đặt `NotAddNpcNormal = 1`
  ⇒ **100 % rơi đồ hiện do Lua quyết định**.

### 3.2 Quái thường (`Droprate_normal.lua`, cấu hình `ch_drop.lua`)

| Việc | Phép quay | Tỉ lệ |
|---|---|---|
| Rơi tiền | `random(0,30) == 2` | **1/31 ≈ 3,2 %**, số tiền theo bậc cấp (30…502) × `DRQ_HESO_TIEN = 1` |
| Rơi đồ | `random(0,30) < 4` (trừ giá trị 2) | **3/31 ≈ 9,7 %**, quay **1 món** từ `\settings\droprate\npcdroprate<bậc>0.ini` |
| Đồ sự kiện | quái bậc > 7 (cấp ≥ 80) và `random(0,4) < 2` | **40 %** gọi `dropeventmap` |

### 3.3 Boss xanh (cùng tệp, nhánh `GetNpcBoss() == 1`)

- Quay **8 món** (`DRQ_QUAY_BOSSXANH`) từ bảng **goldennpc** cùng bậc (bảng này `MinItemLevel` 8 thay vì 7 ⇒ đồ cấp cao hơn).
- Tiền: `random(0,10) > 8` ⇒ **2/11 ≈ 18 %**, số tiền 100…632.

### 3.4 Boss Hoàng Kim (4 script `drop*.lua`)

Mỗi con, khi chết:
1. `DropRateItem(npc, **10**, "\settings\droprate\goldennpc\npcdroprate90.ini", 1, cấp, hệ)` — **10 món**.
2. Một lần `random(1,100)` cho đồ đặc biệt:

| Món | Tiểu HK | Đại HK / máy chủ / PLD |
|---|---|---|
| Vô Lâm Mật Tịch (6,1,26) | `== 10` ⇒ **1 %** | `< 10` ⇒ **9 %** |
| Tẩy Tuỷ Kinh, THBT, 3 loại thuỷ tinh, Bàn Nhược, Tam Tâm, Thiết La Hán, Hoa Hồng | mỗi món `== n` ⇒ **1 %** | như trái |
| Túi bí kíp (4815) | `> 90` ⇒ **10 %** | `== 40` ⇒ **1 %** |
| Lệnh bài Dã Tẩu (4818) | `< 70` ⇒ **69 %** | — |
| Ngựa 9x (hạn 7 ngày) | — | `> 90` ⇒ **10 %** |

3. Gọi thêm các hàm sự kiện: `dropeventboss`, `dropntiendong`, `dropeventHMD`, `dropmanhhkmp`, `droptrangbihkmp` (boss máy chủ),
   `droptrangbihiepcotnhutinh` (PLD) — tỉ lệ nằm trong `lib_sukien.lua` (mẫu số `SKD_RAND*` trong `ch_drop.lua`).
4. Boss máy chủ còn sinh **10 rương** quanh xác, mỗi rương sống 5 phút.

### 3.5 Bảng `.ini` quyết định ra món gì (`KNpc::DropRateItem`, `KNpc.cpp:10983+`)

- Cấu trúc: `[Main] Count, RandRange, MagicRate, MoneyRate, MoneyScale, MinItemLevel/Scale, MaxItemLevel/Scale` + N mục `[i] Genre/Detail/Particular/RandRate`.
- Mỗi lần quay: `g_Random(RandRange)` rồi cộng dồn `RandRate` để chọn mục ⇒ **P(ra món i) = RandRate_i / RandRange**.
  Ví dụ `npcdroprate90.ini`: `RandRange = 200.000`, 58 mục × `RandRate = 30` ≈ 1.740 ⇒ **mỗi vòng quay chỉ ~0,87 % trúng**; engine lặp tới khi đủ số món
  yêu cầu (trần 2.000.000 vòng chống treo). Nghĩa là "10 món" của boss là **10 món thật**, chỉ tốn nhiều vòng quay.
- **Cấp đồ**: `may_mắn/10 %` ra `MaxItemLevel`; thêm `may_mắn/2 + 50 %` ra ngẫu nhiên trong khoảng; còn lại `MinItemLevel`.
- **Số dòng thuộc tính** (`nSlMagicTT`, 0…6) theo "may mắn dòng", **cấp mỗi dòng** (1…10) theo "tổng may mắn" — đều lấy từ `m_nCurLucky` của **người đánh
  phát cuối**; boss xanh dự tính +5/+10, boss vàng +10/+20, VIP +150/+200 (⚠️ xem §5.2).

---

## 4. "CHIA ITEM" — AI NHẶT ĐƯỢC

Dự án **không có cơ chế chia lượt (round-robin / bốc thăm)**. Luật thực tế:

| Việc | Mã | Nội dung |
|---|---|---|
| Ai là chủ món đồ | `KNpc::DropRateItem` `:11524`, `DropItemFromLuaScript` `:10387-10395` | **người gây sát thương phát cuối** (tham số thứ 2 của `DropRate`) |
| Giữ trong bao lâu | `KObj.h:44` `OBJ_BELONG_TIME = 600` nhịp ≈ **33 giây**; `KObj.cpp:683-691` trừ mỗi nhịp, hết thì `m_nBelong = -1` | sau đó **ai cũng nhặt được** |
| Đặt thời gian riêng | `SetItemBelong2(idx, nTime)` (script truyền `nTimeBelong`), `SetEntireBelong` = giữ tới khi món biến mất | |
| Không tổ đội | `KPlayer::ServerPickUpItem` `:4926-4943` | **chỉ chủ** nhặt được, người khác báo "không thể nhặt" |
| Có tổ đội | `:4944-4980` | **cả đội nhặt được**, TRỪ đồ nhiệm vụ (`item_task`) và đồ khoáng (`item_mine`) — hai loại này chỉ chủ |
| Tiền | `:4946-4958` | chủ hoặc người **cùng đội** |
| Khoảng cách | `:5007` | phải trong `PLAYER_PICKUP_SERVER_DISTANCE` |

Thưởng boss Hoàng Kim thì **không rơi ra đất**: `deathhkdai/hksv.lua` duyệt toàn bộ người chơi, ai cùng tổ đội **và** trong bán kính 200 thì
`AddSumExp` + `AddItemSL` cộng thẳng vào túi; người lân cận khác đội chỉ nhận exp thấp hơn.

---

## 5. ĐIỂM ĐÁNG NGỜ THẤY KHI ĐỌC (chưa sửa — chờ chủ quyết)

1. **Máu boss Đại / máy chủ / PLD = 1**. `ch_chung.lua:72` `BHK_MAU_BOSS_DAI = 1` (ghi chú ngay trong tệp: *"!! đang là 1, ghi chú nói mặc định
   18.000.000"*). Nếu bảng web (gcfg) không đè giá trị này thì ba loại boss đó chỉ có **1 máu** — một đòn là chết, trong khi vẫn giảm sát thương 70 %
   và kháng 95 %. Cần chủ xác nhận trên web trước khi kết luận.
2. **Lỗi trong `KNpc::DropRateItem` (`KNpc.cpp:11058-11077`)** — hai tầng if/else nối nhau:
   nhánh boss xanh/vàng cộng may mắn (+5/+10, +10/+20) xong thì khối VIP ngay dưới có `else nLuckySoDong = nTotalLucky = nLuck;` **ghi đè lại**,
   nên **phần cộng may mắn của boss xanh/vàng không bao giờ có tác dụng**. Ngoài ra điều kiện VIP đọc `Player[CLIENT_PLAYER_INDEX]` = **Player[1]**
   (một khe người chơi cố định trên máy chủ) chứ không phải người nhận đồ ⇒ VIP của người khác quyết định số dòng đồ của tất cả.
3. **`printf("Debug: Lucky: %d - Dong: %d\n", …)`** (`KNpc.cpp:11079`) chạy **mỗi món rơi** — ghi console trong luồng chính, nên bỏ.
4. **Quái cao cấp hơn mình chỉ cho 1 exp** (`KPlayer.cpp:2580-2582`): `nSubLevel < 0 ⇒ nGetExp = 1`, kể cả chênh 1 cấp, trong khi chênh ngược lại tới 9
   cấp vẫn ăn đủ. Với mốc `MienTruCap = 90` thì chỉ ảnh hưởng người dưới cấp 90.
5. **Tên rương boss máy chủ sai**: `drophksv.lua:77` dùng biến `NpcIndex` (thiếu `n`) ⇒ Lua trả nil, tên rương thành "Rương của " + rỗng — cùng loại lỗi
   gõ thiếu tiền tố đã gặp ở `DropNpcMoney` trước đây.
6. `KNpcDeathCalcExp` chỉ nhớ **3 khe**: đánh boss đông người (≥ 4 đội) thì đội thứ 4 trở đi **không nhận exp sát thương** nào — với boss Hoàng Kim thì
   không sao (exp phát bằng script), nhưng với boss phụ bản/quái thường đông người thì có.

---

## 6. Bảng nút chỉnh nhanh (khi chủ muốn đổi tỉ lệ)

| Muốn đổi | Sửa ở đâu | Đang là |
|---|---|---|
| Tốc độ lên cấp toàn server (exp script: boss, nhiệm vụ, hoạt động) | `ch_chung.lua` `GLB_TILE_EXP` | 20 |
| Exp đánh quái theo cấp | `gamesetting.ini [Exp] HeSo1..4`, `VipCong` | 80 / 70 / **280** / 100, VIP +20 |
| Hệ số exp toàn cục | `gamesetting.ini [ServerConfig] ExpRate` | 1 |
| % exp chia tổ đội | `[Exp] ToDoi2..8`, `ToDoiKhac` | 80/70/60/55/55/50/50, khác 60 |
| Tỉ lệ rơi đồ quái thường | `ch_drop.lua` `DRQ_MAU_QUAI`, `DRQ_NGUONG_ITEM`, `DRQ_NGUONG_TIEN`, `DRQ_QUAY_QUAI` | 30 / 4 / 2 / 1 |
| Số món boss xanh rơi | `ch_drop.lua` `DRQ_QUAY_BOSSXANH` | 8 |
| Tỉ lệ hoá boss xanh theo bản đồ | `MapList.ini` `<id>_AutoGoldenNpc` (trên 500.000) | 165 bản đồ, vd 2000 = 0,4 % |
| Món gì rơi ra + tỉ lệ từng món | `settings\droprate\**\*.ini` (`RandRate / RandRange`) | 49 tệp |
| Thưởng exp boss Hoàng Kim | `ch_thuong.lua` `BHK_EXP_GIET_BOSS_DAI`, `BHK_EXP_LANCAN_*` | 25tr / 10tr (×20) |
| Máu, giờ, số boss Hoàng Kim | `ch_chung.lua` `BHK_MAU_*`, `lib_bosshk.lua` `TAB_TIME_BOSS_*` | xem §1.2 |
| Thời gian giữ đồ cho chủ | `KObj.h:44 OBJ_BELONG_TIME` (C++, phải dựng lại CoreServer) | 600 nhịp ≈ 33 giây |
