# TỐC ĐỘ ĐÁNH — MỔ TOÀN BỘ BẢN LINUX (`jx_linux_y` + client `game_y`) SO VỚI DỰ ÁN — 07/09/2026

Yêu cầu chủ game: "đào sâu hơn phần tốc độ đánh. Tôi muốn bạn đào toàn bộ phần tốc độ đánh linux".

Đây là phần mở rộng của `PHANTICH_TOCDODANH_MAYMAN_LINUX_0709.md` (mục 2‑3). Công cụ như các đợt trước (`ReverseTools/mo_nhi_phan_0609/`, `dis_all.txt`, `fnfull.py`, `dis_client.txt`, `linux_magic_handlers.txt`). **Chỉ phân tích, chưa đổi mã hay dữ liệu.**

## 0. Kết luận nhanh

| # | Kết luận | Ảnh hưởng thực tế với người chơi |
|---|---|---|
| 1 | **Công thức khung đòn giống nhau**: `khung = KhungGốc × 100 / (100 + tốc độ)`, đòn trúng ở **60 %** khung, kết thúc khi đủ khung. Đánh thường, chiêu ngoại (IsPhysical) và ảo ảnh/nhảy dùng `AttackFrame` + tốc độ đánh; chiêu nội dùng `CastFrame` + tốc độ xuất chiêu — **cách chọn giống hệt JX1** (`IsPhysical`). | — |
| 2 | **Khung gốc người chơi lệch**: Linux nạp cả `AttackFrame=18` **và** `CastFrame=18` từ `basevalue.ini`; JX1 chỉ nạp `AttackFrame` (ini 17), còn `m_CastFrame` của người chơi **kẹt ở 20** (giá trị khởi tạo `KNpc.cpp:404`). | Cùng tốc độ 0: đánh thường/ngoại JX1 **nhanh hơn Linux 5,6 %** (17 vs 18 khung), chiêu nội JX1 **chậm hơn Linux 11 %** (20 vs 18 khung). Đây là lệch **lớn nhất và đang có tác dụng**. |
| 3 | Linux có **kênh Dương** riêng (`attackspeed_yan_v`, `castspeed_yan_v`), hiệu lực = **max(Âm, Dương)**; JX1 gộp Dương vào Âm (**cộng dồn**). | Dữ liệu JX1 hiện không có dòng `skills.txt` nào dùng `*_yan_v` ⇒ chưa có tác dụng; nhưng 8 dòng Linux có (Ngũ Độc Kỳ Kinh 75, Thiên Ma Giải Thể 150, Khi Hàn Ngạo Tuyết 175, Phi Tốc hoàn 261, lệnh bài 492/496, 1309, Kiếm Tông Tổng Quyết 1349) — xem mục 7. |
| 4 | Linux chặn `≤ 0 → 1` ở **cả 4 chỗ dùng tốc độ đánh** (kể cả client); JX1 chỉ chặn ở `DoSkill`, 3 chỗ còn lại chia thẳng. Ngược lại đường **nội công** của Linux (`DoMagic`, đuôi `DoSkill`) **không** chặn mẫu số. | Chỉ xảy ra khi tốc độ ≤ −100 (Linux có kỹ năng thử `kynang200.lua` +3000 %). Hiện JX1 min = −50 ⇒ ngủ. |
| 5 | **Dữ liệu buff tốc độ** (bảng Lua 90 của các phái): trong dải cấp JX1 đạt được (≤ MaxLevel 20/30) **giống nhau**; Linux có thêm điểm bảng cấp 33‑47 (kỹ năng vượt cấp). Lệch thật: **Ngũ Độc Kỳ Kinh (75) JX1 không có tốc độ** (Linux +1→80 % xuất chiêu, +32 % Dương); **Khi Hàn Ngạo Tuyết (175)** JX1 thiếu 2 dòng giảm tốc Dương và thời gian dài gấp đôi (45‑120 s vs 20‑60 s); **1309** JX1 khai `attackspeed_v` nhưng Lua chỉ có `_yan_v` ⇒ vô hiệu; `tianwang_zhanyi` bị chú thích `attackspeed_v` ở JX1 nhưng `skills.txt` hai bản đều **không** khai ⇒ chết ở cả hai. | Xem mục 7. |
| 6 | Đính chính đợt trước: bộ nạp `KSkill` Linux dùng con trỏ `this+4` nên các offset đọc từ loader phải **+4**: `+0x2c = CharAnimId` (không phải IsPhysical), `+0x30 = IsPhysical`. Nhánh "style 14 tức thời" thực ra là `CharAnimId == cdo_none (14)` ⇒ khung 0 — **JX1 cũng vậy** (`ClientDoing == cdo_none`). Bảng ảo `vt+0x2c = IsTargetEnemy` ⇒ gói SK120 H8 (`SetStateSkillEffect` chỉ tung ign% khi chiêu nhắm địch) **đúng như Linux**. | — |

## 1. Đường đi đầy đủ của tốc độ đánh trong Linux

```
Lua bảng kỹ năng / vật phẩm  (attackspeed_v = {{cấp, %}, {cấp, thời gian}})
        │  tên → chỉ số (115 attackspeed_v, 116 castspeed_v, 239 attackspeed_yan_v, 240 castspeed_yan_v — hai bản giống)
        ▼
Handler thuộc tính (KNpcAttribModify)             +0x1a34 Âm  ┐
  0x08098CF0 attackspeed_v      → +0x1a34 += v0          ├─ max(Âm, Dương) = tốc độ đánh hiệu lực
  0x08095FE0 attackspeed_yan_v  → +0x1a38 += v0   +0x1a38 Dương ┘
  0x08098CB0 castspeed_v        → +0x1a3c ; 0x08096000 castspeed_yan_v → +0x1a40
        │  (gốc: +0x1628/+0x162c = 0; NPC cộng thêm cột AttackSpeed/CastSpeed của npcgold 0x0809DE9D)
        ▼
Khi bắt đầu hành động  (6 chỗ server, mục 4)      s = max(Âm, Dương) + 100 ; nếu s ≤ 0 → 1
  nTotalFrame(+0x22c) = KhungGốc × 100 / s ; nếu ≤ 0 → 1 ; nCurrentFrame(+0x230) = 0
        │   KhungGốc: +0x1918 AttackFrame (đánh thường / chiêu ngoại / ảo ảnh / nhảy)
        │             +0x191c CastFrame   (chiêu nội, DoMagic)
        ▼
Mỗi tick (18 tick/s)  OnAttack 0x08085020 / OnSkill 0x08084E00 / OnMagic 0x08085180
  ++nCurrentFrame ; nếu == nTotalFrame×60/100 → Cast (đòn trúng, tạo đạn)
  nếu ≥ nTotalFrame → nCurrentFrame = 0, DoStand (nhận lệnh mới)   ; nTotalFrame == 0 → trúng ngay
        ▼
Đồng bộ client: gói 0x4a/0x4b gửi 4 trường riêng (0x0807BF20 / 0x080810C0), gói thuộc tính bản thân gửi max (0x0809FBD0)
Client game_y tính lại y hệt (+0x11648/+0x1164c, +0x1a10 AttackFrame, +0x1a14 CastFrame) để phát hoạt ảnh.
```

Thời gian một đòn = `nTotalFrame / 18` giây. Ví dụ với `AttackFrame = 18`: tốc độ 0 → 18 khung = 1,00 s; +65 % (Tuyết Ảnh cấp 20) → 10 khung = 0,56 s; +100 % → 9 khung; +122 % (Tuyết Ảnh cấp 44 Linux) → 8 khung. **Không có trần** tốc độ ở bất kỳ đâu (server lẫn client), chỉ có sàn 1 khung.

## 2. Thuộc tính và trường (Linux)

| Thuộc tính | Chỉ số (2 bản) | Handler Linux | Trường KNpc | JX1 |
|---|---|---|---|---|
| `attackspeed_v` | 115 | `0x08098CF0` | `+0x1a34` (Âm) | `AttackSpeedV` → `m_CurrentAttackSpeed` |
| `attackspeed_yan_v` | 239 | `0x08095FE0` | `+0x1a38` (Dương) | **cùng handler** với Âm (`KNpcAttribModify.cpp:81‑84`) ⇒ cộng dồn |
| `castspeed_v` | 116 | `0x08098CB0` | `+0x1a3c` | `CastSpeedV` → `m_CurrentCastSpeed` |
| `castspeed_yan_v` | 240 | `0x08096000` | `+0x1a40` | cùng handler với Âm |
| gốc | — | `0x080A801A` (người chơi), `0x0807E00F` (NPC mẫu) | `+0x1628/+0x162c` = 0; tính lại thuộc tính chép gốc vào **cả hai** kênh (`0x0807EEFB`, `0x080829DD`) | `BASE_ATTACK_SPEED 0` |
| bảng cấp NPC (npcgold) | — | `0x0809DE9D` | cột `AttackSpeed/CastSpeed` (+0x28/+0x2c của bản ghi) cộng vào cả hai kênh | `KNpcGold.cpp:135/246` giống |

Luật **max(Âm, Dương)** (mẫu `cmp/cmovge`) áp cho mọi chỗ tính khung và cho gói thuộc tính bản thân. Cùng họ Âm/Dương: `lifemax`, `manamax`, `movespeed`, 5 kháng (`+0x19ec…`), `+0x118c m_CurrentLifeMax = max(+0x1a14, +0x1a18)`.

## 3. Khung gốc

| Nguồn | Linux | JX1 |
|---|---|---|
| `settings/npc/player/basevalue.ini` `[Common]` | `AttackFrame=18`, `CastFrame=18` | `AttackFrame=17`, `CastFrame=17` (2 bản sao: `settings/npc/player/` và `settings/player/`) |
| Nạp vào NpcSet | `0x080A0185` "AttackFrame" → `NpcSet+0x1434` (= `0x8badef4`), "CastFrame" → `0x8badef8` | `KNpcSet::LoadPlayerBaseValue` (`KNpcSet.cpp:107`) **chỉ đọc `AttackFrame`** (mặc định 20); không có `CastFrame` |
| Áp cho người chơi | `0x08085E3F/0x08085E4A` (khởi tạo NPC người chơi) và `0x08165C58/0x08165C64` (nạp nhân vật): `+0x1918 = 18`, `+0x191c = 18` | `KNpc.cpp:1008/6326` `m_AttackFrame = 17`; **`m_CastFrame` không gán** ⇒ giữ 20 của hàm dựng (`KNpc.cpp:404`); `GetNpcCopyFromTemplate` bỏ qua khung cho `kind_player` (`KNpc.cpp:9339‑9342`) |
| NPC (`npcs.txt` cột `AttackSpeed`/`CastSpeed` = số khung) | mẫu `+0x54/+0x58`, mặc định 20 | `KNpcTemplate.cpp:86‑87` mặc định 20 — giống; 10 mẫu chung lệch số khung (JX1 12 vs Linux 18: Bạch Doanh Doanh, Liễu Thanh Thanh, Ngạo Thiên Tướng Quân, Doãn Thanh Vân, Thập Phương Câu Diệt…) |

Hệ quả (tốc độ = 0, 18 tick/s): đánh thường/chiêu ngoại Linux 18 khung (1,00 s) — JX1 17 khung (0,94 s); chiêu nội Linux 18 khung — JX1 **20 khung (1,11 s)**. Với +65 %: Linux 10/10, JX1 10 (ngoại) / 12 (nội).

## 4. Sáu chỗ tính khung ở server Linux ↔ JX1

| Hành động | Linux | Công thức Linux | `m_Doing` | JX1 |
|---|---|---|---|---|
| Đánh thường `DoAttack` | `0x08078790` (`0x080787A9`) | `s = max(atk, yan)+100; s≤0→1; f = AttackFrame×100/s; f≤0→1` | 7 `do_attack` | `KNpc.cpp:3171` **không chặn**, tổng Âm+Dương |
| Phép thường `DoMagic` (AI NPC) | `0x08078850` | `f = CastFrame×100/(max(cast, castyan)+100)`; **không chặn mẫu**, kết quả ≤0→1 | 6 `do_magic` | JX1 không có hàm riêng |
| Đuôi `DoSkill` — chiêu **ngoại** (`IsPhysical`) | `0x08088150` → `0x08088221` | như DoAttack (chặn cả hai); `CharAnimId == 14 (cdo_none)` → khung 0 | 7 | `KNpc.cpp:3111‑3131`: chặn + **làm chẵn** (JX1 riêng) |
| Đuôi `DoSkill` — chiêu **nội** | `0x0808818D` | `CastFrame×100/(max+100)` **không chặn gì**; `cdo_none` → 0 | 6 | `KNpc.cpp:3133‑3146`: có chặn + làm chẵn |
| Ảo ảnh `DoBlurAttack` (MeleeType 8) | `0x08084930` (`0x08084954`) | như DoAttack | 14 `do_special1` | `KNpc.cpp:3268` không chặn |
| Nhảy đánh `DoJumpAttack` (MeleeType 10) | `0x080807E0` (`0x080808A8`) | như DoAttack | 20 `do_jumpattack` | `KNpc.cpp:9124` không chặn |

`KNpc::DoSkill` chính = `0x08088350` (gọi đuôi tại `0x0808862D`); `CastMeleeSkill` = `0x08087F70` với bảng nhảy `0x8254ac0` (MeleeType 8→ảo ảnh, 9→nhảy, 10→nhảy đánh, 11→`0x8084a10`, 12→`0x8084b40`, 13→`0x8084c90`) — cùng cấu trúc `KNpc.cpp:3176‑3235`. Sau khi đặt khung, đuôi `DoSkill` còn tung `m_CurrentClearAllCD (+0x1384) > rand(100)` ⇒ `KSkillList::ClearAllCD` (chỉ số 291 `clearallcd`, JX1 không có — ghi nhận đợt SK120). Trạng thái `do_runattack (18)` đang chạy thì trả lại phần công kích tạm (`+0x14b0` trừ khỏi `+0x128c`) trước khi đặt khung — JX1 tương đương.

## 5. Nhịp đánh (tick) — giống JX1

`OnAttack 0x08085020`, `OnSkill 0x08084E00` (do_special1), `OnMagic 0x08085180`:

```
++nCurrentFrame
if (nCurrentFrame < nTotalFrame) { if (nCurrentFrame == nTotalFrame*60/100) → trúng đòn; return }
nCurrentFrame = 0
if (nTotalFrame != 0) { DoStand(); cờ +0x194c = 1; return }
→ nTotalFrame == 0 (chiêu không hoạt ảnh): trúng đòn ngay
trúng đòn = GetActiveSkill → kiểm m_SkillList (0x80e45c0) → KSkill::Cast 0x80ea920(launcher, param1, param2) → 0x80847b0 (đặt thời điểm được dùng tiếp)
```

`imul esi, esi, 0x3c` + chia 100 = 60 % = `ATTACKACTION_EFFECT_PERCENT 60` (`KNpc.cpp:63`), `WaitForFrame/IsReachFrame` (`KNpc.cpp:6270‑6288`) y hệt. `OnMagic` có nhánh riêng cho style 13 (`0x80faba0`).

## 6. Client `game_y` (VA = 0x401000 + offset)

| Hàm | Địa chỉ | Nội dung |
|---|---|---|
| `DoAttack` | `0x005938A0` (`0x005938C6`) | `max(+0x11648, +0x1164c)+100`, chặn ≤0→1, `+0x1a10 AttackFrame`, kết quả ≤0→1, `m_ClientDoing` = cdo_attack/attack1 ngẫu nhiên, `m_Doing(+0xf8) = 7` |
| `DoBlurAttack` | `0x00593930` (`0x0059395F`) | `m_Doing == 14` → thôi; `m_ClientDoing(+0xfc) = KSkill+0x2c (CharAnimId)`; công thức như trên; `m_Doing = 14` |
| Đặt hành động + khung | `0x00593800` | tham số `(nDoing, mẫu số)`: 6 → `+0x1a14 CastFrame`, cdo_magic (11); 7 → `+0x1a10`, cdo_attack ngẫu nhiên; `khung = Frame×100/mẫu`, ≤0→1 |
| `DoJumpAttack` | `0x00595A9B` | như DoAttack |
| Bộ nạp KSkill client | `0x006434E9…` | cùng thứ tự cột và cùng offset (this+4) với server |

Client và server cùng công thức nên hoạt ảnh và thời điểm đòn trùng nhau. JX1 client dùng chung mã `KNpc.cpp` (CoreClient.dll) ⇒ trong JX1 client‑server cũng đồng bộ với nhau (cùng lệch so với Linux).

## 7. Dữ liệu tốc độ (skills.txt + Lua) — Linux vs JX1

### 7.1 `skills.txt` (LvlSetting khai tên thuộc tính; giá trị nằm trong Lua)

Linux 26 dòng, JX1 30 dòng. Dòng có ở cả hai và **giống**: 48 Tâm Nhãn, 109 Tuyết Ảnh, 161 Lưỡng Nghi Tâm Pháp, 166 Thái Cực Thần Công, 252 Phật Pháp Vô Biên, 273 Như Lai Thiên Diệp, 275 Sương Ngạo Côn Lôn, 360 Tiêu Diêu Công, 401, 511, 590/591/613/614 (bạn đồng hành), 625, 664. Khác:

| Id | Linux khai | JX1 khai | Nhận xét |
|---|---|---|---|
| 75 Ngũ Độc Kỳ Kinh | `castspeed_v`, `castspeed_yan_v`, `attackspeed_yan_v` | **không** | Lua Linux `wudu_qijing`: `castspeed_v {1,1}→{40,80}`, `castspeed_yan_v {1,32}→{40,32}`, `attackspeed_yan_v {1,32}→{40,32}`; JX1 `wudu.lua:156` không có 3 dòng này ⇒ **Ngũ Độc JX1 không được tốc độ từ buff này** |
| 150 Thiên Ma Giải Thể | + `attackspeed_yan_v` | chỉ `attackspeed_v`, `castspeed_v` | Lua Linux `attackspeed_yan_v {1,0},{38,0},{39,60},{40,60}` — chỉ có từ cấp 39 (vượt MaxLevel 30) ⇒ trong dải cấp thường không lệch |
| 175 Khi Hàn Ngạo Tuyết (giảm tốc địch) | `castspeed_v`, `attackspeed_yan_v`, `castspeed_yan_v` | chỉ `castspeed_v` | Linux `castspeed_v {1,-6}…{30,-50}` **20‑60 s**, `attackspeed_yan_v {1,-1}…{30,-16}`, `castspeed_yan_v …{30,-12}`; JX1 cùng giá trị `castspeed_v` nhưng **45‑120 s** và thiếu 2 dòng Dương |
| 261 Phi Tốc hoàn | `attackspeed_v` + `_yan_v` | chỉ `attackspeed_v` | Lua trả cùng giá trị cho cả hai ⇒ max = như JX1 |
| 492 lệnh bài Tống Kim, 496 tăng công kích | 4 tên (Âm + Dương) | 2 tên (Âm) | Lua cùng giá trị cho Âm/Dương ⇒ max = không lệch; JX1 dùng cộng dồn nên **không được** thêm Dương vào JX1 nếu chưa làm luật max |
| 1309 加攻速 | `attackspeed_yan_v`, `castspeed_yan_v` | `attackspeed_v`, `castspeed_v` | Lua JX1 `sjbattle_state.lua:78‑79` chỉ có `_yan_v` ⇒ JX1 khai tên không có trong bảng ⇒ **vô hiệu** (dùng ở 4 tệp script) |
| 1120 Kích Công Trợ Lực | `castspeed_v` 50, `castspeed_yan_v` 50 | `castspeed_v` ×2 (Lua 10/10) | VN chỉnh riêng |
| 1349 Kiếm Tông Tổng Quyết | `attackspeed_yan_v` + `attackspeed_v` (cùng 6→32) | chỉ `attackspeed_v` | max = như JX1 |
| 1429/1430/1435 Tật Phong Đan / Hổ Tâm Đan | 1435 `attackspeed_v` | 1429 `attackspeed_v`, 1430 `castspeed_v`×2 | thiết kế vật phẩm khác nhau |
| 1968/1970/1980 (Vũ Hồn), 2123 (Tiêu Dao) | không có | có | phái JX1 tự thêm |

### 7.2 Bảng Lua buff 90 các phái (`script/skill/*.lua` Linux ↔ `script/nhanvat/kynang/*.lua` JX1)

| Phái / bảng | Linux | JX1 | Lệch trong dải cấp ≤ MaxLevel? |
|---|---|---|---|
| Thúy Yên `cuiyan` (109, MaxLevel 20) | `{1,12},{20,65},{23,73},{25,90},{28,99},{42,111},{43,119},{44,122}` (cả attack/cast), 120‑180 s | `{1,12},{20,65}` | không (cấp > 20 chỉ có khi được cộng cấp; JX1 `Link()` ngoại suy tuyến tính) |
| Nga My `emei` (252, 30) | `{1,12},{30,65},{33,70},{35,90},{38,95},{41,100}` | `{1,12},{30,65}` (bản `baiming/emei.lua` = Linux) | không |
| Cái Bang `gaibang` (360, 20) | `{1,6},{20,65},{25,90},{31,108},{32,118},{33,121}` | `{1,6},{20,65}` | không |
| Côn Lôn `kunlun` (275, 30) | cast `{1,25},{30,65},{34,73},{35,90},{36,92}`; attack `{1,25},{20,65},{33,92},{35,118},{38,124},{39,126}` | `{1,25},{30,65}` / `{1,25},{20,65}` | không |
| Thiếu Lâm `shaolin` (273, 30) | `{1,35},{30,65},{35,70}…{47,108}`, 120‑360 s | `{1,35},{30,65}`, 120‑360 s | không |
| Thiên Nhẫn `tianren` (150, 30) | attack `{1,26},{30,102},{33,109},{35,134},{38,138},{41,145},{42,163},{43,165}`; cast `{1,26},{30,81},{33,86},{35,101},{36,103}`; + Dương 60 từ cấp 39 | `{1,26},{30,102}` / `{1,26},{30,81}` | không |
| Võ Đang `wudang` (166, 30) | attack `{1,21},{30,65},{33,69},{35,90},{38,94},{41,98}`; cast `…{35,81},{41,90},{44,94}` | `{1,21},{30,65}` ×2 | không |
| Đường Môn `tangmen` (48, 30) | `{1,29},{30,106},{33,113},{34,116},{35,149},{38,156},{39,159},{40,162}` | giống hệt | không |
| Ngũ Độc `wudu_qijing` (75, 30) | có 3 dòng tốc độ (7.1) | **không có** | **CÓ** |
| Côn Lôn `qihan_aoxue` (175) | 3 dòng, 20‑60 s | 1 dòng, 45‑120 s | **CÓ** (thời gian + Dương) |
| Thiên Vương `tianwang_zhanyi` | `attackspeed_v {1,5},{30,65}` | dòng bị chú thích `--` | `skills.txt` hai bản **không khai** `attackspeed_v` cho chiêu này ⇒ chết ở cả hai, không lệch |
| Hoa Sơn `huashan` 1349, Tiêu Dao `xiaoyao`, Vũ Hồn `wuhuntang` | giống JX1 | | không |
| Linux riêng | `kynang200.lua` +100→3000 % (thử nghiệm), `skillenhance.lua` `zhangemei1.attackspeed_yan_v −17→−38` (debuff ngoại Dương), `cuiyan/xueying.lua`, `emei/fofa-wubian.lua`, `tianren/tianmo-jieti.lua` (bản hàm) | | |

Vật phẩm/trang bị: thư mục `settings/item/` hai bản **không** tham chiếu thuộc tính bằng tên lẫn chỉ số 115/239 ⇒ tốc độ trên trang bị đi qua Lua vật phẩm (`script/item/…`, ví dụ `feisu-wan.lua`, `jxf_pkitems.lua`) và `MagicDesc.ini` chỉ để hiển thị.

### 7.3 Cột thời gian giữa hai lần dùng (giới hạn nhịp thứ hai)

29 ô lệch trên id chung (`TimePerCast/TimePerCastOnHorse/WaitTime`): 392 Thúc Phược Chú Linux 27 khung — JX1 0; 1406/1539/1541/1543/1544/1545 boss Linux có hồi chiêu 180‑1080 khung, JX1 0; 40/224/327/336 `WaitTime` 3/6 vs 5/8; 1521/1522/1526/1601/1603 `WaitTime` 0 vs 2‑5; 1584‑1591 TOPTK 2‑5 vs 0. Cột `WeaponSkill` (=1 cho 8 chiêu "Công kích vật lý" 1/2/53/229‑232/418) Linux nạp vào `KSkill+0x34`, JX1 không đọc — không dính tốc độ.

## 8. Bản đồ `KSkill` Linux (đính chính) và bảng ảo

Bộ nạp `KSkill::LoadSkillData` = `0x080E9200`, `ebx = this + 4` (ghi `SkillName` tại `ebx+0`, `GetSkillName` trả `this+4`). Offset thật = offset trong loader + 4:

| Trường | Offset thật | Trường | Offset thật |
|---|---|---|---|
| `SkillId` | `+0x24` | `TargetOnly` | `+0x74` |
| `SkillStyle` | `+0x28` | `TargetEnemy` | `+0x78` |
| `CharAnimId` | `+0x2c` | `TargetOther` | `+0x7c` |
| `IsPhysical` | `+0x30` | `TargetAlly` | `+0x80` |
| `WeaponSkill` | `+0x34` | `TargetObj` | `+0x84` |
| `IsAura` | `+0x38` | `TargetNoNpc` | `+0x8c` |
| `IsMelee` | `+0x3c` | `ReqLevel` (word) | `+0x6c` |
| `IsUseAR` | `+0x40` | `CharClass` | `+0x70` |
| `IsExpSkill` | `+0x44` | `SkillCostType` / `CostValue` | `+0x9c` / `+0x98` |
| `AttackRadius` | `+0x48` | `TimePerCast` / `OnHorse` | `+0xa0` / `+0xa4` |
| `WaitTime` | `+0x4c` | `DoHurt` | `+0x5c` |
| `Param1` | `+0x50` | `MeleeType` | `+0xc8` (jump table CastMeleeSkill) |

Bảng ảo (`0x082587a8`): `+0x08 GetSkillId [+0x24]`, `+0x0c GetSkillName`, `+0x10 GetSkillStyle [+0x28]`, `+0x14 LoadSkillLevelData`, `+0x18 CanCastSkill`, `+0x1c GetSkillCostType [+0x9c]`, `+0x20 GetSkillCost [+0x98]`, `+0x24 IsExp [+0x44]`, `+0x28 IsTargetOnly [+0x74]`, **`+0x2c IsTargetEnemy [+0x78]`**, `+0x30 IsTargetAlly [+0x80]`, `+0x34 IsTargetOther [+0x7c]`, `+0x38 IsTargetObj [+0x84]`, `+0x3c IsTargetSelf [+0x88]`, `+0x40 IsTargetNoNpc [+0x8c]`, `+0x48 GetAttackRadius [+0x48]`, `+0x4c IsAura [+0x38]`, `+0x50 [+0x34] (WeaponSkill)`, `+0x58 GetDelayPerCast(bNgựa)`, `+0x5c GetReqLevel`. Thứ tự khác `Skill.h` JX1 (JX1: TargetOnly, Enemy, Ally, Obj, NoNpc, Self, Aura, Physical, Exp…).

⇒ `SetStateSkillEffect 0x08086C9C` gọi `vt+0x2c` = **IsTargetEnemy** trước khi tung `ignorenegativestate_p` — gói SK120 H8 đã làm đúng. `KSkillList::AddSkillExp 0x080E5EED` gọi `vt+0x24` = **IsExp** — gói SKEXP đúng.

## 9. Việc chủ quyết (chưa làm gì)

| # | Đề xuất | Lý do | Phạm vi |
|---|---|---|---|
| A | Nạp `CastFrame` cho người chơi từ `BaseValue.ini` (`KNpcSet::LoadPlayerBaseValue` + `KNpc.cpp:1008/6326` gán `m_CastFrame`) | JX1 chiêu nội đang 20 khung, Linux 18 — lệch 11 % đang có tác dụng | server + client (cùng mã Core) |
| B | Chọn số khung gốc: giữ 17/17 của JX1 hay đổi 18/18 như Linux | 17 làm ngoại công nhanh hơn Linux 5,6 % | chỉ ini (không build) — nếu làm A thì ini `CastFrame` bắt đầu có hiệu lực |
| C | Kênh Dương riêng + luật max ở 6 chỗ + đồng bộ | để chép được dữ liệu Linux có `_yan_v` (75, 175, 150…) mà không cộng dồn | server + client |
| D | Chặn `≤ 0 → 1` ở `KNpc.cpp:3171/3268/9124` | phòng chia 0 khi có debuff mạnh | server + client |
| E | Dữ liệu: Ngũ Độc Kỳ Kinh 75 (thêm 3 dòng như Linux — cần C nếu muốn phần Dương), Khi Hàn Ngạo Tuyết 175 (thời gian 20‑60 s như Linux, 2 dòng Dương cần C), 1309 (đổi tên khai cho khớp Lua hoặc ngược lại) | lệch thật đang có tác dụng | skills.txt + Lua server/client |
| F | 392 Thúc Phược Chú `TimePerCast` 0 → 27 như Linux; boss 1539‑1545 hồi chiêu | nhịp dùng chiêu | skills.txt |

Không đề xuất đổi: công thức khung, 60 %, cách chọn `IsPhysical`, làm chẵn khung (vô hại), bảng buff 90 (giống trong dải cấp).
