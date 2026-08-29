# PHÂN TÍCH & DỊCH NGƯỢC TÍNH NĂNG: BẠN ĐỒNG HÀNH (同伴 / PARTNER)

> Ngày 27/08/2026. Làm theo `HUONGDAN_DICHNGUOC_TINHNANG_LINUX.md` (bước 1→2 của quy trình 8 bước).
> **CHƯA SỬA MỘT DÒNG NÀO** — đây là báo cáo đo đạc. Mọi con số đều có `tệp:dòng` làm bằng chứng.
> Yêu cầu chủ game: **lấy 100% tính năng từ bản Linux, không tự chế, không đoán mò.**
>
> Đo bằng: `recon_tinhnang.py` (3 lượt) + 4 agent đọc trọn 11.400 dòng script đã giải mã +
> mổ chuỗi binary `jx_linux_y` + dò 1.082 pak trên 3 ổ đĩa + đối chiếu bảng JX1 từng dòng.
> Tệp giải mã sẵn (138 tệp UTF-8) nằm ở scratchpad phiên này, thư mục `bdh\`.

---

## 0. TÓM TẮT MỘT TRANG

| | Hiện trạng |
|---|---|
| Nguồn Linux | `D:\ServerLinux\server1` — **131 tệp Lua** nhắc partner, lõi ~60 tệp/**11.400 dòng sống**; 11 bảng `settings\partner\` + 4 bảng `settings\task\partner*`; **51 hàm engine `PARTNER_*`** trong binary `jx_linux_y` |
| Bản Linux có chạy không | ✅ Có — đủ script sống, đủ bảng, binary có log `KSG_PartnerLog` |
| Phần lõi nằm ở đâu | **C++ engine** (binary Linux, ĐÃ STRIP ký hiệu hàm): mảng 3 đồng hành/người chơi, sinh chỉ số từ GenInfo+RandSeed, AI 4 tính cách, thân mật độ, túi riêng, 28 sự kiện thoại, lưu roledb |
| JX1 đã có sẵn (đo từng dòng) | ✅ **npcs.txt: TOÀN BỘ ~60 template** (945–958, 974, 1039–1066, 1043–1065 = 15 hình dạng đồng hành, 1048–1055, 1078–1079, 1112–1116…) thẳng hàng tên với Linux · ✅ **skills.txt: 84/84 kỹ năng đồng hành (549–630, 660, 704) trùng tên từng dòng** · ✅ MapList.ini **đã khai 512–515 + 539 đúng đường dẫn** và WorldSet **đang nạp** (World491–494=512–515, World518=539) · ✅ 3/5 ini giao diện + `taskui_partner_random.lua` có sẵn trong pak client (update01/03) · ✅ `IncludeLib` đã nhận tên `"PARTNER"/"FILESYS"/"TITLE"/"RELAYLADDER"` (đang trỏ noop) · ✅ lang VN có sẵn 3 chuỗi `MSG_PARTNER_*` (bản Linux `lang\vn\stringtable_core.txt:1017-1019`) |
| JX1 thiếu (cốt lõi) | ❌ **Toàn bộ module engine** server + client (KPlayerPartner của JX1 chỉ là nháp 92 dòng năm 2003, 0/51 hàm Lua) · ❌ dữ liệu bản đồ trên đĩa (đã TÌM RA trong pak — §6) · ❌ 11 bảng `settings\partner` · ❌ ~60 tệp script · ❌ 2/5 ini UI + 14 ảnh nút · ❌ hệ nhiệm vụ ngẫu nhiên TASKSYS (`task_main.lua`) |
| Có phải build engine không | ⚠️ **CÓ — đây là port engine lớn nhất từ trước tới nay**: mọi tính năng trước chỉ port Lua; Bạn Đồng Hành phần lõi nằm trong C++ cả server LẪN client (cửa sổ UI + giao thức mới) |
| Rủi ro lớn nhất | Lưu dữ liệu đồng hành vào roledb (đụng Gate 2 — có phương án không đổi cấu trúc blob, §7.3) · binary Linux đã strip nên công thức nội bộ phải tái lập từ bảng số + đo hành vi |

**Bức tranh:** VLTK1 bản VNG (chính là cây `D:\ServerLinux`) có tính năng Bạn Đồng Hành trọn vẹn từ 2009.
JX1 của ta dùng **cùng bảng dữ liệu đời VNG** (npcs/skills/maplist đã chứa sẵn mọi thứ) nhưng **engine
là bản 2005 chưa từng có module partner**. Việc port = viết module C++ partner cho cả 2 đầu, theo đúng
ngữ nghĩa đã dịch ngược ở tài liệu này, rồi chép script + bảng như các đợt trước.

---

## 1. TÍNH NĂNG LÀ GÌ (rút từ 11.400 dòng script)

Người chơi sở hữu tối đa **3 bạn đồng hành** (binary: `KPartnerArray<KPlayerPartner,3>`; script chặn
ở **5** — `swordking_people.lua:216` `PARTNER_Count()==5`; UI vẽ 5 thẻ — xem §8 câu hỏi 1). Đồng hành
là NPC đi theo chủ, đánh nhau cùng chủ, có:

- **Ngũ hành** (0 Kim·1 Mộc·2 Thủy·3 Hỏa·4 Thổ) + **ngoại hình** (5 mẫu × 3 thời kỳ: Tiểu đồng→lv1,
  Thiếu niên→lv60, Thành niên→lv100 — `partner_levelup_server.lua:3` `FEATURE_PERIOD={1,60,100}`).
- **Tính cách** (1 Dũng mãnh·2 Vệ chủ·3 Lưu manh·4 Nhu nhược) = 4 chế độ AI thật (`character.txt`:
  AIMode 21/22/23/24 — đánh gần nhất / đánh kẻ hại chủ / đánh con ít máu nhất / đánh ngẫu nhiên).
- **6 tư chất** (aptitude 1–10: Sinh lực·Lực·Chính xác·Né·Tốc độ·May mắn) quyết định dải chỉ số
  gốc + tăng trưởng mỗi cấp (`aptitude_range.txt` × `attrib_range.txt`, nội suy tuyến tính).
- **Cấp 1–100** (`level_exp.txt`, trần 194.495.000 exp), lên cấp học võ công theo hệ
  (`partner_levelup_server.lua`: lv30/60/80), roll lại tăng trưởng ở lv 10/30/50/90
  (`PARTNER_ReGenAttribsInc`).
- **Kỹ năng 4 loại** (`init_skill.ini` chú thích): 0 kháng · 1 tiên thiên · 2 tự học (tối đa theo cấp:
  bảng `{1,2,3,5,7,9,11,13,15,16}` ô, `partner_skillbook_h.lua:54`) · 3 tuyệt kỹ (chỉ 1). Học bằng
  **sách** (item `6,1,834..884/901/1063` → skill 549–621/660/704, bảng `skill_requirement.txt`).
- **Độ thân mật** 0–100 (khởi tạo 50, tự giảm mỗi 2 giờ khi >50 — `partner_setting.ini`), quyết định
  6 mức câu thoại khi trò chuyện (4 bảng × 50 câu × 6 cột = 1.200 câu, `partner_talk.lua`).
- **Túi riêng 6×10=60 ô** mở dần theo cấp túi 0–10 (`partner_bag.ini`; binary:
  `Get/SetPartnerBagLevel`, room item số **14** — `gamedatadef.lua:40 room_partnerbag=14`).
- **Hôn mê khi chết** (không chết hẳn): 300s không được gọi lại, hồi 2%/5s
  (`partner_setting.ini [DEATH_PUNISH]/[REVIVE]`; `partner_action.lua OnDeath`).
- **28 sự kiện tự thoại** (`partner_event.ini`: gọi ra, máu ít, chủ bị PK, rơi đồ vàng, mang >50 vạn
  lượng…, mỗi sự kiện 5 câu nam + 5 câu nữ, gọi chủ là 哥哥/姐姐).

### 5 nhánh chơi (mỗi nhánh 1 báo cáo chi tiết riêng đã đọc trọn):

1. **Thu nhận + Giáo dục** (map 512 Vân Trung Đạo): nhận miễn phí 1 con/ngày ở Kiếm Hoàng đệ tử
   (tư chất 5 đều), hoặc làm chuỗi giáo dục 5 nhánh nhỏ (Lư Thanh→Đường Ảnh→Bạch Sát→Hắc Sát→Thu Y
   Thủy, có hỏi đáp 118 câu + oẳn tù tì + 12 con giáp + đánh bao cát/cọc gỗ) rồi được tặng con mới:
   làm dưới 900 giây → 6 tư chất tổng 30 (`genRandNumArray(30,6,1,7)`), chậm hơn → mode 2;
   kèm bảng xếp hạng thời gian (`Ladder_NewLadder(10188, tên, -giây, 1)`).
2. **Tu luyện bằng cuộn** (mua ở Trường Ca Môn nhân tại 7 thành, shop 110–130): **381 cuộn**
   (`6,1,447..827` ↔ task id 6..386, `taskid = particular − 441`) giết 50/100/150 quái;
   engine đếm bằng `AddPlayerEvent(taskid)`; thưởng exp theo `reward_allprize.txt` (người = đồng hành);
   5→10→20 lượt/ngày theo danh hiệu Thanh Long Võ Sĩ/Kiếm Sát/Võ Tuyệt/Long Thần Chi Kiếm
   (title 77–80, thăng theo số NGÀY làm đủ lượt: 5/10/20/30 ngày).
3. **Cốt truyện** ("master" = 主线, KHÔNG phải sư phụ; NPC Long Ngũ): 4 tuyến + 1 ẩn —
   Thị Lang Chi Tử (514, PC≥70/pet≥10) · Khống Xà Nhân (PC≥80/pet≥20) · Châu Bảo Thương Nhân
   (515, PC≥90/pet≥30, có đường chuộc 30 triệu lượng) · Dị Tộc Võ Sĩ (513, PC≥100/pet≥40) ·
   Ẩn-Kiếm Hoàng (Bách Hoa Cốc, cần xong 4 tuyến + 15 ngày tu luyện → chọn 1/2 sách tuyệt kỹ
   theo hệ `6,1,839..848`, thưởng 5tr exp người + 1tr exp pet).
4. **Tu luyện thiên** ("rewind" — bản làm lại hằng ngày của cốt truyện 1/3/4, 3 lượt/ngày/đồng hành,
   trạng thái lưu HOÀN TOÀN trên biến task của đồng hành 30–46/55–57/61–66).
5. **Khu tu luyện Trường Ca Môn Cấm Địa** (map 539): vào theo 22 điểm `enter.txt`, 440 quái lv95
   (1112/1113/1114) spawn theo 9 tệp toạ độ, **5 Tế Đàn** (NPC 1116) mỗi giờ chẵn thức tỉnh 1 lần
   toàn server (global task `gb_*Task("长歌门祭坛", i)`): 40% rơi sách xịn / 20% boss 1115 /
   20% exp to (3k→3tr theo cấp) / 20% không gì; 259 trap viền `exit.txt` thoát về 27 điểm 7 thành.
   **Thiếu trong cây script: bộ timer bật tế đàn mỗi giờ chẵn** — phải tự viết (ghi rõ ở §8).

Ngoài ra: **nhiệm vụ ngẫu nhiên do đồng hành tự giao** khi trò chuyện (đi qua hệ TASKSYS
`task_main.lua`, task người chơi 1301–1306) + item "Mật đồ nhiệm vụ" (`random_taskbook.lua`,
taskID nằm trong magic level 1 của item, 8 lượt/ngày).

---

## 2. KIẾN TRÚC 4 TẦNG — VÀ TẦNG NÀO THIẾU GÌ

```
┌──────────── TẦNG 1: ENGINE SERVER (C++) ───────────────── JX1: ❌ PHẢI VIẾT MỚI
│ KPartner/KFightPartner/KPlayerPartner + KPartnerArray<,3> (tên lớp từ RTTI binary)
│ 51 hàm Lua PARTNER_* · nạp 11 bảng settings\partner · AI 4 tính cách (AIMode 21-24)
│ · sinh chỉ số GenInfo(9 số)+GenTime+RandSeed · exp/lên cấp/đổi thời kỳ · thân mật
│ (tự giảm 2h) · hồi máu 2%/5s · hôn mê 300s · cooldown gọi 30s · 28 sự kiện thoại
│ · túi room 14 + bag level · 81 biến task/đồng hành · log KSG_PartnerLog
│ · lưu/nạp roledb (log: "Level, GenInfo ×9, GenTime %u, RandSeed %u")
│ · gọi script: partner_talk.lua, partner_levelup_server.lua, partner_action.lua(OnDeath)
│ · PARTNER_OFF theo map (maplist 945/946_NewWorldParam=PARTNER_OFF)
├──────────── TẦNG 2: ENGINE CLIENT (C++) ───────────────── JX1: ❌ PHẢI VIẾT MỚI
│ 4 cửa sổ (thuộc tính Y · kỹ năng I · túi U · thanh phím tắt) + nút gọi/đối thoại/
│ chế độ đánh/đi theo/chọn pet (hotkey Y I U L F G B N — ini đã Việt hoá sẵn)
│ · nhận sync dữ liệu partner · client đọc bảng: partner_bag.ini, aptitude_mode.txt,
│ feature.txt, allproblem/index_taskid/reward_allprize (đo từ pak VLTK)
│ · partner_levelup_client.lua (hook client) · taskui_partner_random.lua (bảng nhiệm vụ)
├──────────── TẦNG 3: SCRIPT LUA (server) ───────────────── JX1: ❌ chép + nắn (như mọi đợt)
│ ~60 tệp/11.400 dòng: education/master/rewind/reward/train/trap/talk/skillbook/item
│ + phụ thuộc ngoài: lib\mem.lua (inherit) · task\system\task_main.lua + TASKSYS ·
│ newtask_head.lua (CÓ SẴN JX1) · gb_taskfuncs (CÓ SẴN) · titlefuncs (CÓ SẴN) ·
│ tong_award_head (tongaward_partner_*) · great_night event · task_debug.lua
├──────────── TẦNG 4: BẢNG DỮ LIỆU + TÀI NGUYÊN ─────────── JX1: phần lớn ĐÃ TÌM RA NGUỒN
│ 11 bảng settings\partner (engine đọc) + 4 bảng task (Lua đọc) + 5 ini UI + ảnh +
│ 5 bản đồ + 74 tệp script kỹ năng partner (skill\partner\*) + bảng skills/npcs (CÓ SẴN)
└──────────────────────────────────────────────────────────
```

---

## 3. CHỮ KÝ 51 HÀM `PARTNER_*` (suy từ MỌI call-site + bảng cấu hình)

> Nguồn: tổng hợp 4 báo cáo agent, mỗi chữ ký có ≥1 call-site làm bằng chứng.
> Index đồng hành **1-based**; `PARTNER_Count()` trả **−1 khi lỗi hệ thống** (script có nhánh riêng).

### 3.1 Nhóm truy vấn (client cũng cần cho UI)
```
idx, state = PARTNER_GetCurPartner()      -- state: 0 chưa gọi ra, 1 đang gọi ra
n          = PARTNER_Count()              -- 0..5, -1 = lỗi
s          = PARTNER_GetName(idx)
n          = PARTNER_GetLevel(idx)
n          = PARTNER_GetSeries(idx)       -- 0..4 (Kim Mộc Thủy Hỏa Thổ)
n          = PARTNER_GetEmotionDegree(idx)-- 0..100
n          = PARTNER_GetSettingIdx(idx)   -- npc template id hiện tại (1043-1065, để vẽ ảnh)
lv, exp    = PARTNER_GetSkillInfo(idx, skillId)
tb|nil     = PARTNER_GetAllSkill(idx, nType)   -- nType 2 hoặc 3; getn() được
v          = PARTNER_GetTaskValue(idx, nVarId) -- 81 biến/con, bảng partner_task_def.txt
n          = PARTNER_GetExp(idx)               -- (không thấy call-site; UI dùng)
tb/6 số    = PARTNER_GetAptitudes(idx)         -- (UI cột tiềm năng)
tb         = PARTNER_GetAttribs(idx)           -- (UI)
tb         = PARTNER_GetAttribsInc(idx)        -- (UI, tăng trưởng/cấp)
n          = PARTNER_GetCharacter(idx)         -- 1..4
tb/5 số    = PARTNER_GetResists(idx)           -- 5 kháng
n          = PARTNER_GetEssentialFeatureID(idx)-- 1..5 (feature.txt)
n          = PARTNER_GetEndure(idx)            -- thể lực/độ bền (chưa thấy call-site)
...        = PARTNER_GetGenData(idx)           -- GenInfo gốc (log xoá partner in 9 số)
n          = PARTNER_GetBirthday(idx)          -- GenTime
```

### 3.2 Nhóm thao tác
```
idx = PARTNER_AddFightPartner(nFeature, nSeries, nCharacter, nAptitudeMode)      -- dạng 4 tham số
idx = PARTNER_AddFightPartner(nFeature, nSeries, nCharacter, a1,a2,a3,a4,a5,a6)  -- dạng 9 tham số
      -- nFeature 1..5 (feature.txt), nSeries 0..4, nCharacter 1..4 (character.txt)
      -- a1..a6 = tư chất 1..10 (Sinh lực, Lực, Chính xác, Né, Tốc, May mắn)
      -- dạng 4: tham số 4 = APTITUDE_MODE_ID trong aptitude_mode.txt (mode 1 đỉnh bậc 6,
      --         mode 2 đỉnh bậc 4) — script gốc truyền 2 ở nhánh "làm chậm >900s"
      -- CẶP CHUẨN trong script gốc: (1,4) (2,2) (3,3) (4,1) (5,0); nam = 1/3/5, nữ = 2/4
      -- (công thức 5-nSeries của script GM chỉ đúng 3/5 cặp — xem §9 lỗi 17)
PARTNER_RemovePartner(idx)
PARTNER_SetCurPartner(idx)                 -- script truyền PARTNER_Count() = con mới nhất
PARTNER_CallOutCurPartner([1])             -- 0/1 tham số; 1 = gọi ra
PARTNER_SetTaskValue(idx, nVarId, v)
PARTNER_AddExp(idx, nExp [, nFlag])        -- nFlag luôn =1 khi có (cờ hiện thông báo?)
PARTNER_LevelUp(idx)                       -- (không thấy call-site; chuỗi MSG_PARTNER_LEVEL_UP)
PARTNER_AddSkill(idx, nType, nSkillId, nLevel [, nExp])  -- nType 0/1/2/3
PARTNER_RemoveSkill / PARTNER_RemoveAllSkill             -- (nút "Quên" trên UI)
PARTNER_SetStandbySkill(...)               -- kỹ năng trực chiến (chưa thấy call-site)
PARTNER_SetName(idx, sz)
PARTNER_SetEmotionDegree(idx, v) / PARTNER_AddEmotionDegree(idx, ±1)
PARTNER_SetAttribs / PARTNER_SetResists / PARTNER_ReGenAttribsInc(idx)  -- ReGen ở lv 10/30/50/90
PARTNER_AddLifeAptitude / AddStrengthAptitude / AddDefenceAptitude / AddSpeedAptitude
  / AddLuckAptitude / AddHitTargetRateAptitude (idx, n?)  -- tăng tư chất lẻ (item sau này)
PARTNER_ChangeCharacter(idx, n)
PARTNER_ChangeFeature(idx, n) / PARTNER_ChangeFeatureOfPeriod(idx, nPeriod 1..3)
PARTNER_AddState(...)                      -- buff trạng thái lên đồng hành
PARTNER_SetCallOutSwitch(n)                -- khoá/mở gọi đồng hành (map PARTNER_OFF dùng)
```

### 3.3 Hook engine → script (phải giữ đúng tên)
| Hook | Tệp Linux | Chữ ký |
|---|---|---|
| lên cấp (server) | `\script\partner\partner_levelup_server.lua` (binary có nhúng đường dẫn) | `main(nPartnerIdx, nPartnerLevel)` |
| lên cấp (client) | `\script\partner\partner_levelup_client.lua` | `main(nPartnerIdx, nPartnerLevel)` |
| hôn mê | `\script\partner\partner_action.lua` | `OnDeath(nLastDamageIdx)` |
| trò chuyện (nút L) | `\script\partner\partner_talk.lua` (binary có nhúng) | `main()` |

### 3.4 Chuỗi hệ thống trong binary (đã có bản dịch VN sẵn)
- `PlayerCount(%d)= Online(%d)+Offline(%d), PartnerCount(%d)= CallOut(%d)+UnCallOut(%d)` (thống kê)
- `(%s:%s) Request Remove Partner(Level: %d; Gen Info: %d..%d ×9)，GenTime: %u, RandSeed: %u` (log xoá)
- `[PartnerDataError:TaskValue] [%s:%s] [PartnerName:%s] [TaskIndex:%d]` (lỗi dữ liệu)
- `MSG_PARTNER_LEVEL_UP/GET_EXP/DEC_EXP` — `lang\vn\stringtable_core.txt:1017-1019` có sẵn tiếng Việt
- 11 khoá sự kiện: PartnerCallout, PartnerBloodLess, PartnerBloodFallQuickly, PartnerKill{Animal,Boss,Player,Partner}, PartnerLevelUp, PartnerBeDrawBack, PartnerCastToHost, HostKillPartner (+ KillPartnerPK, lucky_v_partner)

---

## 4. BẢNG DỮ LIỆU — 11 BẢNG ENGINE ĐỌC + 4 BẢNG LUA ĐỌC

> **Engine đọc trực tiếp** (grep toàn cây Lua: KHÔNG tệp nào mở các bảng này): `partner_setting.ini`,
> `partner_bag.ini`, `partner_event.ini`, `init_skill.ini`, `aptitude_mode.txt`, `aptitude_range.txt`,
> `attrib_range.txt`, `character.txt`, `feature.txt`, `resist.txt`, `level_exp.txt` — binary có nhúng
> cả 11 đường dẫn. **Lua đọc**: `skill_requirement.txt`, `talk\*.txt` (5 tệp), `problem\partner_allproblem.txt`,
> `reward\index_taskid.txt`, `reward\reward_allprize.txt`.

Số liệu chốt (đã đọc nguyên văn, chi tiết trong báo cáo agent D):

| Bảng | Nội dung chốt |
|---|---|
| `partner_setting.ini` | emotion 0–100/init 50/giảm 1 mỗi 7200s khi >50 · hôn mê 300s · cooldown gọi 30s · hồi 200/10000 mỗi 5s |
| `partner_bag.ini` | túi bậc 0–10: 0,0 / 2,2 / 2,3 / 3,3 / 4,4 / 5,5 / 6,6 / 6,7 / 6,8 / 6,9 / 6,10 (cột×hàng); màu ô khoá 0x190000A0 |
| `init_skill.ini` | mỗi hệ 1 kỹ năng tiên thiên khởi tạo: Kim 554 · Mộc 560 · Thủy 566 · Hỏa 572 · Thổ 578 |
| `character.txt` | 4 tính cách = AIMode 21/22/23/24, Vision/Active 500 hoặc 400, ForceSync 800, AIMaxTime 6, AIParam1=100 (+20,50 / +20,60,30 cho kiểu 3/4) |
| `feature.txt` | 5 hình dạng × 3 thời kỳ = npc 1056–1060 / 1043–1047 / 1061–1065, SEX 0/1/0/1/0 |
| `resist.txt` | mỗi hệ +0.25 một kháng, −0.25 một kháng (init + increment): Kim +độc−hỏa · Mộc +lôi−vật · Thủy +hỏa−lôi · Hỏa +vật−băng · Thổ +băng−độc |
| `aptitude_range.txt` | tư chất 1–10 → hệ số [min,max] ⊂ [0..1], bậc n ≈ [0.01+0.1(n−1), +0.12], chồng lấn ~0.04 |
| `attrib_range.txt` | dải INIT + INCREMENT 6 thuộc tính theo hệ (Kim trâu nhất LIFE 14000–20000, Thủy yếu nhất 8500–14500; SPEED init=10 mọi hệ; LUCK 0–8/0–0.24 mọi hệ). Công thức: `giá trị = INIT_MIN + hệ_số_tư_chất × (INIT_MAX−INIT_MIN)`, tăng/cấp tương tự với INCREMENT |
| `aptitude_mode.txt` | 2 mode trọng số bốc tư chất (62 cột): mode 1 "sơ khởi" đỉnh bậc 6 (48,7%), mode 2 "nhiệm vụ sơ nhập" đỉnh bậc 4 (52%) — là tham số 4 của `AddFightPartner` dạng 4 tham số |
| `level_exp.txt` | exp 1→100: 100, 500, 1100 … 194.495.000 (lv99=lv100 = trần) |
| `partner_event.ini` | 26 sự kiện + 3 khoá global (HostName_Man=哥哥, HostName_Woman=姐姐, Format=<color=green>); mỗi sự kiện Odds/Param1/DialogM1-5/DialogF1-5; 6 sự kiện Kill* đang Odds=0; `partner_event_old.ini` giống hệt 100% |
| `skill_requirement.txt` | 53 dòng: sách 834–838→skill 549–553 (Base1/Inc11) · 839–848→558..583 (Base80/Inc2, khoá hệ) · 849–884→584–621 (Base10/Inc10) · 901→660 · 1063→704. Công thức cấp yêu cầu: `Base + (cấp_sách−1)×Inc` |
| `talk\*.txt` | 4 bảng 51 dòng × 6 cột thân mật (0-10/11-19/20-49/50-84/85-95/96-100) theo cặp giới tính chủ×pet; `partner_sex.txt` 17 npc→giới tính |
| `index_taskid.txt` | 381 cuộn: `6,1,447..827` → taskid 6..386 (**taskid = particular − 441**), level gate = cấp pet yêu cầu + 10, bộ 3 dòng = 50/100/150 quái |
| `reward_allprize.txt` | taskid 6..386: PeopleExp = PartnerExp, bộ 3 = ×1/×2/×3 (3250/6500/9750 … 69350/138700/208050); tra theo **số dòng** `EventID−4` — cấm đảo thứ tự dòng |
| `partner_allproblem.txt` | 118 câu hỏi (1–100 kiến thức hệ đồng hành, 101–118 phương ngữ TQ — phải viết lại khi Việt hoá); đáp án hard-code trong `partner_problem.lua:26-31` (A 28 câu, B 42, C 48) |
| `partner_task_def.txt` | tài liệu 81 biến task/đồng hành (không được engine nạp): 1 giáo dục · 2 tu luyện · 3-6+14 cốt truyện · 7-13 đếm vật phẩm · 11 đếm sách kháng (trần 20) · 16-29 đếm quái · 30-46 rewind · 50-57 bitmask thưởng · 61-66 ngày/lượt rewind · 68 đếm ngày tu luyện · 80-81 tàng bảo đồ |

---

## 5. TÀI NGUYÊN CLIENT — ĐÃ DÒ 1.082 PAK, KẾT QUẢ CHỐT

### 5.1 Giao diện (ini) — nguồn `D:\ServerLinux\Patch\ui\ui3_1024\` (bản 800 cũng có)
| Tệp | Trong pak client JX1? | Ghi chú |
|---|---|---|
| `同伴属性.ini` (thuộc tính, 4.244B) | ❌ (chỉ có trong pak VLTK update03) | chép loose vào `client\Ui\Ui3\` như đã làm với `投色子.ini` |
| `同伴技能.ini` (kỹ năng) | ❌ (pak VLTK có) | như trên |
| `同伴技能树.ini` (cây kỹ năng) | ✅ update01/update03.pak | |
| `同伴背包.ini` (túi) | ✅ update03.pak | bản 1024.pak VLTK mới hơn (742B, "Túi hành trang tử mẫu" — đã Việt hoá) |
| `同伴快捷栏.ini` (phím tắt) | ✅ update01.pak | **đã Việt hoá sẵn cả Tips** (Y/I/U/L/F/G/B/N) |
| `\ui\taskui_partner_random.lua` | ✅ update01.pak | client task-panel, dùng TASKSYS + task 1301–1306 |

Bố cục đã đọc nguyên văn: cửa sổ 444×376, 5 thẻ `BtnPartner_0..4`, trang thuộc tính có 26 ô edit
(Name/Exp/Energy/Fire/5 kháng Pufang-Bingfang-Leifang-Huofang-Dufang/Level/Character/6 thuộc tính ×
[giá trị + tiềm năng]/Wuxing/QinHe), nút Forget/Save(选定)/Delete(解散)/Switch(切换)/Close; trang kỹ
năng thêm lưới `SkillsIcon` bước 39px; túi 6×10 ô 170×280; thanh phím tắt 220×28 với 9 nút.

### 5.2 Ảnh (26 đường dẫn rút từ 5 ini) — **12/26 đã tìm thấy**
- ✅ 3 ảnh trong `update03.pak` (cả JX1 lẫn VLTK): `同伴背包界面.spr`, `同伴背包界面-关闭.spr`, `同伴快捷栏\同伴的物品栏.spr`
- ✅ 9 ảnh trong **`E:\jx1m_cdn\data\image2.pak`** (pak mobile, định dạng PACK chuẩn PC): 标签1, 同伴-技能, 同伴属性title, 同伴快捷栏\{同伴的基本属性, 同伴的武功技能, 同伴提示, 与同伴对话, 召唤同伴}, …
- ❌ **14 ảnh chưa thấy ở bất kỳ pak nào trên 3 ổ** (đã quét 1.082 pak): 标签2/3/4/5*, 关闭, 解散, 切换, 选定, 遗忘, 同伴-人物 (nền trang thuộc tính!), 同伴技能title*, 仅跟随, 同伴选择, 主动攻击, 主界面按钮条改副本. (*một phần nằm trong 9 ảnh image2 — danh sách chính xác trong `scratchpad\allpak_hits.txt` + `mobilepak_hits.txt`)
- Phương án: (a) xin/tải client CN gốc đời 2009 có pak đầy đủ; (b) tạm mượn nút cùng họ trong pak
  (关闭/选定… có bản tương đương ở các UI khác — cách `v23` từng làm với xúc xắc, sau tìm ra ảnh gốc thì
  hoàn nguyên); (c) vẽ lại 14 ảnh (đa số là nút chữ 2-3 ký tự + nền panel).
- Icon nhiệm vụ `\spr\Ui3\同伴任务系统\任务图标\同伴教育.spr` (partner_head.lua:745): ❌ chưa thấy — cùng phương án.

### 5.3 Bản đồ — **CẢ 5 ĐÃ TÌM RA NGUỒN, KHÔNG CÒN CHẶN** ✅
| Map | Thư mục (MapList **JX1 đã khai đúng y Linux**) | Nguồn dữ liệu region tìm thấy |
|---|---|---|
| 512 Vân Trung Đạo | `特殊用地\任务用地\同伴教育任务\云中道` | `E:\jx1m_cdn\data\image2.pak` — 97 region, X96–119 Y94–117 |
| 513 Bách Hoa Cốc | `…同伴剧情任务\百花谷` | image2.pak — 54 region, X85–101 Y87–101 |
| 514 Sơn Tặc Động | `…同伴剧情任务\山贼洞` | image2.pak — 92 region, X87–145 Y97–129 |
| 515 Tử Hà Lĩnh | `…同伴剧情任务\紫霞岭` | image2.pak — 80 region, X80–100 Y91–113 |
| 539 TCM Cấm Địa | `特殊用地\长歌门禁地` | `update03.pak` (cả client JX1 LẪN VLTK) — 115 region, X96–118 Y97–118 |

- WorldSet_GameServer.ini **đang nạp sẵn**: `World491..494 = 512..515`, `World518 = 539`
  (hiện server bỏ qua vì thiếu dữ liệu trên đĩa — không có `512_srv.fp`).
- Quy trình chép giống Viêm Đế: rút region từ pak → client `maps\`; bản server đổi tên
  `_Region_C.dat → _Region_S.dat` (định dạng OBSTACLE trùng — đã chứng minh đợt Viêm Đế,
  `KRegion.cpp:407`); `.fp` tự sinh lần chạy đầu.
- Còn phải rút thêm: `.wor` + ảnh nền + ảnh nhỏ `24.jpg` của từng map (dò theo tên trong cùng pak,
  chưa chạy — cùng cách hash; nếu .wor không có trong pak thì tra bảng `v_XXX` để tự dựng .wor
  theo mẫu Viêm Đế `rect=`).
- Toạ độ script nằm trong vùng phủ: 512 spawn quanh ô 1545–1886×3132–3745 (= region X96–118
  ×32 ≈ đúng dải) — khớp.

### 5.4 Kịch bản kỹ năng partner (74 tệp `script\skill\partner\`)
Có ở cả `D:\ServerLinux\server1\script\skill\partner\` (7 thư mục: gold/wood/water/fire/earth/
resistance/expansion) lẫn bản sao `J:\jx-thiendieu-x64\bin\server\`. Định dạng: bảng `SKILLS` với
thuộc tính nội suy 2 điểm (`{cấp,giá trị}`) — **đúng định dạng LvlSetScript mà JX1 đang dùng**
(skills.txt cột `LvlSetScript`/`LevelUpScript` trỏ tới). Tên tệp GBK — chép giữ nguyên byte.
**JX1 skills.txt đã trỏ sẵn các cột script này** (bảng trùng đời VNG) → chỉ cần chép cây tệp.

---

## 6. PHÍA JX1 — ĐO TỪNG LỚP

### 6.1 Đã có sẵn (không phải làm)
| Thứ | Bằng chứng |
|---|---|
| ~60 NPC template (đủ mọi id script cần: 945–958, 974, 1039–1042, 1048–1055, 1066, 1078–1079, 1112–1116, 324/372/378/413/415/626, 960–964; **15 hình dạng đồng hành 1043–1047/1056–1065**) | so tên từng dòng npcs.txt hai bên — trùng 100% (scratchpad `npc_compare.txt`) |
| 84/84 kỹ năng 549–630+660+704 trong skills.txt, trùng tên từng SkillId | scratchpad `skill_compare2.txt` |
| MapList khai 512–515+539 đúng đường dẫn; WorldSet nạp World491–494/518 | `MapList.ini:3885-3894,3996`; `WorldSet_GameServer.ini:496-499,523` |
| `IncludeLib` nhận "PARTNER"/"FILESYS"/"TITLE"/"RELAYLADDER"/"SETTING" (trỏ noop.lua) | `ScriptFuns.cpp:2511-2533` |
| `newtask_head.lua` (nt_getTask/nt_setTask), `gb_taskfuncs.lua`, `titlefuncs.lua`, `great_night\huangzhizhang\event.lua` | recon mục 3: "co san o JX1" |
| 3/5 ini UI + taskui lua trong pak client | §5.1 |
| Task id trống: 182–191 (TaskTemp hỏi đáp), 1226–1272 (cần rà từng id — recon báo dải 1228/1229/1232/1250/1261/1269 TRỐNG), 1301–1306 | recon mục 5 |
| Chuỗi MSG_PARTNER_* bản VN | `D:\ServerLinux\server1\lang\vn\stringtable_core.txt:1017-1019` (chép về) |

### 6.2 Phải làm — theo tầng
**A. Engine server (khối lượng chính):**
1. Lớp `KPlayerPartner` (dữ liệu 1 con: GenInfo 9 số = feature/series/character/6 tư chất, GenTime,
   RandSeed, level, exp, name[16], emotion, taskvalues[81], skills 4 loại, standby skill, trạng thái
   callout) + `KPartnerArray<,3>` trong KPlayer + bộ nạp 11 bảng settings.
2. Sinh chỉ số: tư chất → hệ số (aptitude_range) → roll INIT/INCREMENT trong attrib_range bằng
   RandSeed; ReGenAttribsInc ở lv 10/30/50/90; kháng theo resist.txt + skill 549–553.
   ⚠️ Binary strip — công thức nội suy CHÍNH XÁC (làm tròn, roll theo seed) phải tái lập từ bảng +
   đo trên server Linux đang chạy (tạo pet GM, đọc chỉ số thật nhiều mẫu) — ghi §8 câu 5.
3. Partner NPC runtime: spawn NPC theo feature+period, đi theo chủ (đã có AI follow trong engine —
   tái dùng `PlayerAI`/bot follow), 4 AIMode 21–24, hồi máu 2%/5s, hôn mê 300s + OnDeath script,
   qua map cùng chủ, PARTNER_OFF theo map, cooldown gọi 30s.
4. Exp/level: PARTNER_AddExp (+MSG_PARTNER_GET_EXP), level_exp.txt, lên cấp gọi
   `partner_levelup_server.lua main(idx, lv)` (script tự AddSkill võ công + đổi thời kỳ).
5. Kỹ năng partner: gắn skill cho NPC partner qua bảng skills.txt sẵn có (Attrib 1006/1008 engine
   đã hiểu); loại 2 (hỗ trợ) áp buff lên chủ/pet — đi qua hệ skill state có sẵn.
6. Thân mật: tick giảm theo EMOTION_DEG_STEP; 28 sự kiện thoại partner_event.ini (phát chat bubble
   qua kênh chat thường).
7. Túi đồng hành: room 14 trong KItemList (JX1 hiện `room_num=13` — thêm `room_partnerbag`,
   xem 7.3) + bag level theo cấp.
8. 51 hàm Lua đăng ký vào ScriptFuns.cpp + `WriteCompoundLog`-style log riêng KSG_PartnerLog.
9. Lưu/nạp roledb — §7.3.

**B. Engine client:**
1. 4 lớp cửa sổ (UiPartnerAttr/Skill/Bag/Quickbar) đọc 5 ini trên + hotkey Y/I/U/L/F/G/B/N.
2. Giao thức mới (JX1 tự cấp số — KHÔNG cần khớp số của Linux vì hai đầu đều của ta):
   s2c sync danh sách + chi tiết partner + exp/emotion/bag; c2s: gọi/thu, chọn con, đổi tên,
   quên skill, chuyển chế độ đánh/theo, thao tác túi, bấm trò chuyện (mở partner_talk trên server).
   Làm đúng bài chèn lưới `kiem_luoi_giaothuc.py` như đợt xúc xắc (BANGIAO_VIEMDE mục 11).
3. Client copy bảng: partner_bag.ini, aptitude_mode.txt, feature.txt, allproblem/index_taskid/
   reward_allprize (VLTK client đọc đúng các bảng này — đã đo pak).

**C. Script + bảng:** chép 60 tệp qua bộ sinh (nắn TCVN3, sửa bẫy #1–13 + 30 lỗi gốc §9),
11+4 bảng settings, 74 tệp skill, dựng `partner_allproblem` bản Việt.

**D. Bản đồ + tài nguyên:** rút 438 region + .wor + ảnh từ image2.pak/update03.pak, chép 2 ini UI
loose, xử 14 ảnh thiếu.

### 6.3 Khối lượng ước lượng (so mốc cũ)
| Phần | Cỡ | So sánh |
|---|---|---|
| Engine server partner | **lớn** — module mới ~3–5 nghìn dòng C++ | lớn hơn hệ xúc xắc nhiều; cùng cỡ port bang hội JX2 |
| Engine client UI + protocol | vừa–lớn (~4 cửa sổ + 1 bộ giao thức) | cỡ UI lò rèn 7 khung đã làm |
| Script port | vừa (60 tệp, có bộ công cụ sẵn) | cỡ Viêm Đế (26 tệp) × 2 |
| Bảng + bản đồ + ảnh | nhỏ (thuần chép/rút pak) | như Viêm Đế bước 1 |
| Hệ TASKSYS (nhiệm vụ ngẫu nhiên) | vừa — CÓ THỂ TÁCH GIAI ĐOẠN 2 | riêng một hệ con |

---

## 7. BA QUYẾT ĐỊNH KIẾN TRÚC (đề xuất, chờ duyệt khi thi công)

### 7.1 Trần số đồng hành: theo binary = 3 hay theo script/UI = 5?
Binary Linux: `KPartnerArray<KPlayerPartner,3>` (RTTI). Script chặn `PARTNER_Count()==5`
(`swordking_people.lua:216`), UI vẽ 5 thẻ. → Bản Linux đang chạy trần **3** (template param),
script 5 là dư địa. **Đề xuất: làm mảng 5, mặc định cấu hình 3** — giữ nguyên hành vi Linux,
mở rộng không phải build lại.

### 7.2 Giao thức: số hiệu tự cấp của JX1
Số hiệu gói của Linux không trích được (binary strip; client CN không có trên đĩa) và **không cần**:
cả server lẫn client đích đều build từ cây ta. Chỉ cần ĐỦ ngữ nghĩa dữ liệu (danh sách §6.2-B).
Tuân thủ đúng quy trình chèn lưới đã chốt (`g_nProtocolSize` + thứ tự handler).

### 7.3 Lưu roledb: dùng vùng TASK VALUE, không đổi cấu trúc blob (Gate 2 an toàn)
Blob `TRoleData` của JX1 có các đoạn cố định (BaseInfo/Skill/Task/Item) — thêm ĐOẠN MỚI là đổi
định dạng blob (đụng Goddess/Bishop, rủi ro cao). Phương án đề xuất:
- **Toàn bộ trạng thái partner ghi vào dải task value dành riêng** (mỗi ô 1 int, đã có tiền lệ
  MAX_TASK 4200→4600 đợt kinh mạch; cần ~180 ô/con × 5 = ~900 ô → nới MAX_TASK 4600→5600, đổi
  layout KPlayer ⇒ build đồng bộ 3 nhị phân như đợt kinh mạch — ĐÃ TỪNG LÀM, có quy trình).
  Đoạn task trong blob là danh sách (id,value) biến thiên ⇒ **không đổi định dạng blob**.
- Túi đồng hành = room mới trong item list: mỗi bản ghi item đã mang (room,x,y) ⇒ thêm room
  **không đổi định dạng bản ghi**; binary cũ nạp save mới sẽ rơi item room 14 — chỉ một chiều,
  chấp nhận như mọi lần nâng.
- GenInfo+RandSeed lưu nguyên vẹn để chỉ số tái lập đúng như Linux (partner "sinh lại" từ seed).

---

## 8. NHỮNG CHỖ CẦN CHỦ GAME QUYẾT

1. **Trần đồng hành 3 hay 5?** (§7.1 — đề xuất mặc định 3 đúng Linux, engine chứa 5).
2. **118 câu hỏi giáo dục**: 1–100 dịch từ bản CN; **101–118 là câu đố phương ngữ Trung Quốc**
   (vô nghĩa với người Việt) — thay bằng 18 câu đố Việt tự soạn? (đổi nội dung, GIỮ cơ chế).
3. **14 ảnh nút UI chưa tìm thấy**: chờ client CN gốc / mượn nút cùng họ / vẽ lại? (§5.2).
4. **Hệ nhiệm vụ ngẫu nhiên TASKSYS** (đồng hành tự giao nhiệm vụ + Mật đồ 1301–1306): port ngay
   cùng đợt hay để giai đoạn 2? (Không ảnh hưởng nuôi/đánh/tu luyện/cốt truyện.)
5. **Cách chốt công thức chỉ số**: binary strip ⇒ đề xuất đo thật trên server Linux đang chạy
   (GM tạo pet các tổ hợp tư chất, đọc chỉ số nhiều mẫu để khớp công thức nội suy + cách làm tròn).
   Chủ game có cho phép bật server Linux đo không? (Nếu không: nội suy tuyến tính theo bảng —
   đúng dạng nhưng có thể lệch làm tròn.)
6. **Timer tế đàn mỗi giờ chẵn** không có trong cây script Linux (chỉ có mô tả trong thoại NPC) —
   phải tự viết driver 1 tệp (giống `ydbz_driver`). Xác nhận cách hiểu "mỗi giờ chẵn thức tỉnh,
   dùng 1 lần, toàn server"?
7. **partner_event.ini**: 6 sự kiện Kill* bản Linux đang Odds=0 (tắt) — giữ nguyên 0 đúng bản gốc?

---

## 9. LỖI TRONG MÃ GỐC (30 lỗi đã bắt khi đọc — PHẢI xử khi port)

> Nguyên tắc: port giữ HÀNH VI thật của bản Linux; lỗi làm CHẾT chức năng thì sửa, lỗi chỉ là
> mã thừa/chết thì giữ nguyên trạng (đánh dấu). Danh sách đầy đủ vị trí `tệp:dòng`:

**Làm hỏng chức năng (SỬA):**
1. `partner_problem.lua:35-47` — `partner_edu` không `return` nhưng 3 nơi so `== 10`
   (`educationpeople.lua:286,407,1028`) ⇒ nhánh 1231=17 không bao giờ chạy.
2. `educationpeople.lua:1006` gọi `longba_getprize2` — **chưa hề định nghĩa**; `longba_getprize1`
   định nghĩa 2 lần (`:1019`, `:1033`) — bản sau đè bản trước ⇒ mất chặng 5 câu hỏi + 4000 exp.
3. `swordking_people.lua:48` — `Uworld1229 = nt_getTask(1228)` (copy-paste sai) ⇒ điều kiện hoàn
   thành không kiểm nhánh Bạch Sát.
4. `partner_talk.lua:86` — `"ffriendly_man_w"` thừa `f` ⇒ nam×pet nữ emotion 11–19 lỗi bảng.
5. `partner_talk.lua:98,125,152,179` — cột `friend8495` không bao giờ được đọc (dùng nhầm
   `friend4984`).
6. `mastertask.lua:176` — `PARTNER_GetTaskValue(i,3,0)` phải là `SetTaskValue` ⇒ huỷ nhiệm vụ bản
   cũ vô hiệu.
7. `mastertask.lua:263` + `swordking_people.lua:215` — `Msg2player` (thường) không tồn tại.
8. `partner_jitan.lua:123` — `partneridx <= TB[i][2]` phải là `partnerlvl` ⇒ mọi pet chỉ nhận
   3.000 exp.
9. `functions_partner.lua:123` — `nPartnerIdx` chưa khai (phải là `partner_index`).
10. `main_03:497` + `main_03:141` — `PayMasterAward(3,8)` gọi 2 chỗ, bit chống trùng nuốt phần
    thưởng cuối ở Long Ngũ.
11. `task_award.lua:573` — `if nDoubleMode~=0 or nDoubleMode~=nil` luôn true; kèm
    `partner_reward.lua:157` nhân thẳng `greatnight_huang_event(5)` ⇒ sự kiện tắt trả 0 làm exp=0.
12. `swordking_people.lua:189` — chia 0 nếu `Uworld1234==0`.
13. `partner_reward2/3` thiếu `nt_setTask(1246,...)` khởi tạo ⇒ tier 4/5 có thể hiện "còn −x lần".
14. `getrealpara.lua` trả nil ngầm khi item lạ ⇒ so sánh `< nil` nổ runtime.
15. `partner_event.ini:322` — `DialogF4` khai 2 lần (mất 1 câu, đổi thành F5).
16. `main_04:289` — Boss_05 ghi đè biến 28 của Boss_04 (thiếu biến riêng; giữ nguyên hành vi gốc
    hay cấp biến mới → hỏi khi thi công, mặc định GIỮ NGUYÊN).
17. Ánh xạ feature↔series: bảng chuẩn (1,4)(2,2)(3,3)(4,1)(5,0) ≠ công thức `5-nSeries` của
    `functions_partner.lua:142` (script GM đời sau) — **dùng bảng chuẩn**.

**Mã chết/di sản (GIỮ, đánh dấu):** `partner_master_lib.lua` mồ côi; ~9 hàm bản cũ trong
`mastertask.lua` (dead); `doTaskEntity` đã bỏ logic tổ đội (comment gốc); biến pet 33 dùng đôi
(REWIND_004 + REWIND_ITEM_001); `partner_task.lua` ≡ `partner_task_boss.lua` (diff rỗng);
`skillbook_select(3)` comment nói 10% code là 30% (làm theo CODE); `swordking_people.lua` đè
`partner_checkdo` của `educationpeople.lua` (thứ tự Include quyết định — giữ đúng thứ tự gốc);
7/12 tên con giáp trong `educationpeople.lua:12` hỏng mã hoá (dịch lại khi nắn TCVN3);
`enter.txt` đơn vị Ô còn `exit.txt` + 9 tệp khu đơn vị PIXEL (kiểm chữ ký AddMapTrap của JX1
trước khi chép); `AddNpc` bảng `partner_master_people` KHÔNG nhân 32 (toạ độ đã pixel) còn 2 bảng
kia nhân 32 — ba hàm nạp khác nhau trong `partner_head.lua:693-724`.

---

## 10. THỨ TỰ THI CÔNG ĐỀ NGHỊ (7 giai đoạn, nghiệm thu từng bước)

1. **G0 — Chốt §8** với chủ game + rà lại dải task id 1226–1272/1301–1306 từng số một.
2. **G1 — Dữ liệu tĩnh**: rút 5 bản đồ + .wor + ảnh từ pak; chép 15 bảng settings (+ dịch);
   chép 74 tệp skill; chép 2 ini UI loose; khai `settings\partner\`. Server boot phải nạp được
   World491–494/518 (tự sinh .fp) — nghiệm thu bằng vào map bằng lệnh GM.
3. **G2 — Engine server lõi**: KPlayerPartner + nạp bảng + sinh chỉ số + NPC theo chủ + AI 4 kiểu
   + exp/level + hôn mê/hồi máu + emotion + 51 hàm Lua + log. Nghiệm thu bằng script GM
   (`functions_partner.lua` chính là bộ test có sẵn của dev cũ).
4. **G3 — Lưu roledb** (§7.3) + túi room 14 + bag level. Nghiệm thu: relog giữ nguyên pet.
5. **G4 — Client**: 4 cửa sổ + quickbar + giao thức + bảng client. Nghiệm thu trên client thật.
6. **G5 — Script gameplay**: port 60 tệp theo bộ sinh (sửa 17 lỗi nhóm SỬA ở §9), partner_event,
   partner_talk, sách kỹ năng, tế đàn + driver giờ chẵn, cuộn tu luyện + title 77–80.
7. **G6 — TASKSYS** (nếu duyệt): task_main.lua + nhiệm vụ ngẫu nhiên + Mật đồ + bảng F12.

Mỗi giai đoạn: `syncheck` + `check_encoding` + chép gương + commit kèm bằng chứng `tệp:dòng`.

---

## 11. PHỤ LỤC — LỆNH ĐO & TỆP TRUNG GIAN

```
python D:\GAMEDEVNEW\ReverseTools\port_3hd\recon_tinhnang.py "partner"
python D:\GAMEDEVNEW\ReverseTools\port_3hd\recon_tinhnang.py "task\partner"
```
- 138 tệp giải mã UTF-8: `scratchpad\bdh\` (server) + `scratchpad\bdh_client\` (rút từ pak VLTK).
- Chuỗi binary: `scratchpad\bin_partner_strings.txt` (91 chuỗi).
- So NPC/skill: `scratchpad\npc_compare.txt`, `skill_compare2.txt`.
- Dò pak: `probe_spr.py` / `probe_all_paks.py` / `probe_mobile_pak.py` / `probe_maps*.py`
  → kết quả `allpak_hits.txt`, `mobilepak_hits.txt`, `map512_allpak.txt`.
- 4 báo cáo agent (education / master / reward / train+settings) nằm trong transcript phiên 27/08.
- Bẫy đã né: hash pak PHẢI dùng bản char-có-dấu `ReverseTools\viemde\pak_id.py`; pak mobile "PAKX"
  index XOR 0xA7; heredoc bash nuốt `\\` (viết script dò bằng Write rồi mới chạy).
```
