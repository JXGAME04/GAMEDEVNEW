# KỸ NĂNG 150 & "SKILL 180" — MỔ NHỊ PHÂN LINUX (`jx_linux_y`) SO VỚI DỰ ÁN — 07/09/2026

Yêu cầu chủ game: (1) "kiểm tra toàn bộ kỹ năng 150 của các phái — dự án đang lỗi dame skill 150 — so với bản Linux để fix"; (2) "mổ nhị phân bản Linux để lấy bộ skill 180 các phái".

Công cụ: `ReverseTools/mo_nhi_phan_0609/` (`dis_all.txt` toàn ELF, `fnfull.py`, `linux_magicnames.txt` 336 tên thuộc tính theo enum Linux, `audit_*`). Dữ liệu đối chiếu: `D:\ServerLinux\server1\settings\skills.txt` + `script\skill\*.lua` (Linux) và cây chạy thật `E:\...\TESTLOFFF_ONLINE\bin\{server,client}` (JX1).

## 1. Kết luận nhanh

| | Kết quả |
|---|---|
| **Skill 150** | JX1 **hạ bảng sát thương** của 6 phái (Thiên Vương −25 %, Ngũ Độc −30 %, Thiên Nhẫn −27 %, Võ Đang −30 %, Côn Lôn −20 %, **Thúy Yên nội −66 %**), **bỏ hẳn** băng của Thiên Vương chuỳ, choáng của Võ Đang kiếm phụ, `skill_misslenum_v` Cái Bang, `anti_block_rate` + sự kiện 1201 của Thiếu Lâm; **cắt cấp tối đa 26 → 20** (bảng exp cấp 21‑25 vẫn còn ở cả hai bản); bật `IsUseAR` cho Hào Hùng Trảm (trượt theo chính xác). Tất cả đã **đưa về Linux** ở cây chạy thật (server + client), mục 2‑3. Engine: 5 khác biệt công thức sát thương (mục 4) — **chưa đổi, chờ chủ chọn** vì ảnh hưởng mọi kỹ năng, không riêng 150. |
| **Skill 180** | **Không tồn tại** trong bản Linux: `skills.txt` Linux không có dòng nào ReqLevel 180, script Linux không có bảng `*180*`, nhị phân server/client không có chuỗi nào "180"+kỹ năng, bảng tên thuộc tính không có nhóm 180. Thứ duy nhất tên "Skill 180" là NPC của nhà vận hành VN (`script\global\thinh\npc\npc_vocong.lua`: "Vào Phái Võ Học Skill 180") — nó chỉ phát bộ 90/120/150 + 10 kỹ năng phụ 120 (1220‑1229, JX1 đã có) và `AddMagic(1901, 20) ---- level 180` với **id 1901 không có trong skills.txt** (lệnh chết). Chi tiết mục 5. |

## 2. Dữ liệu 150 đã sửa (server **và** client, sao lưu `*.truoc_sk150_0709`; gương `serverscript_live` commit cùng đợt)

### 2.1 `settings/skills.txt` — 29 dòng, 70 ô: chép nguyên cột cơ chế từ Linux (giữ tên/mô tả/icon/âm thanh của JX1)

| Cột | Dòng | JX1 → Linux |
|---|---|---|
| `MaxLevel` | 1055‑1063, 1065‑1067, 1069‑1071, 1073‑1076, 1078‑1081 (24 chiêu chính) | 20 → **26** (`magic_level_exp.txt` hai bản đều đã có LEVEL21‑25 = 90k/120k/150k/200k/250k/300k; `KSkillList::IncreaseLevel` không chặn; `MAX_TRAIN_SKILLEXPLEVEL` 20 chỉ nằm trong điều kiện `&&` sai nên vô hiệu) |
| `ShowAddition` | 25 dòng | 0 → 1 (client hiện dòng "tăng thêm" trong tooltip) |
| `MslsGenerateData` | 1055 4→5, 1058 4→5, 1059 3→4, 1060 2→3 | khoảng cách **khung** giữa các viên (`KSkills.cpp:2346`), không phải số viên |
| `IsUseAR` | 1058 Hào Hùng Trảm, 1084 đạn của nó | 1 → **0**: không còn trượt theo chính xác/né tránh |
| `AttackRadius` / `DoHurt` | 1062 Băng Vũ Lạc Tinh 400/80 → 480/90; 1066 Hình Tiêu Cốt Lập DoHurt 90 → 100; 1095 Hồn Ảnh Tông Sanh 0 → 10 | |
| `LvlSetting` bổ sung | 1055 `anti_block_rate`; 1056 `skill_eventskilllevel`/`skill_startevent`/`skill_showevent`/`skill_desc`/`anti_block_rate` (→ **1201 Vi Đà Hộ Pháp được bắn lại**, JX1 trước không bao giờ bắn); 1059 & 1087 `skill_attackradius`; 1081 `seriesdamage_p` (lặp, vô hại); 1097 Truy Tâm Táy Mệnh `skill_eventskilllevel`/`skill_collideevent`/`skill_showevent` | |
| `LvlSetting` bỏ | 1083 Long Thủ Bát Nhã Thủ `stun_p` (JX1 tự thêm, Linux không có) | |

### 2.2 Bảng Lua (8 tệp, 26 khoá; khoá `skill_skillexp_v`/`addskillexp1`/`skill_desc` giữ JX1 vì engine JX1 tính exp bằng `magic_level_exp.txt` + `addskillexp1`)

| Tệp / bảng | Khoá | JX1 (cấp 20 / 26) | Linux (cấp 20 / 26) |
|---|---|---|---|
| tianwang `daotianwang150` (1058 Hào Hùng Trảm) | `physicsenhance_p` | 405 / 810 | **545 / 992** |
| tianwang `chuitianwang150` (1059 Tung Hoành Bát Hoang) | `physicsenhance_p` | 455 / 896 | **655 / 1156** |
| | `colddamage_v` | **bị chú thích** (không băng) | min 60/87, max 300/439 |
| tianwang `qiangtianwang150` (1060 Bá Vương Tạm Kim) | `physicsenhance_p`; `deadlystrike_p` | 460 / 856; 30 / 41 | **640 / 1028; 40 / 54** |
| wudu `daowudu150` (1067 U Hồn Phệ Ảnh) | `physicsenhance_p` | 208 / 411 | **308 / 541** |
| wudu `zhangwudu150` (1066 Hình Tiêu Cốt Lập) | `poisondamage_v` | 215 / 422 | **315 / 652** |
| cuiyan `neicuiyan150` (1065 Thủy Anh Man Tú) | `physicsdamage_v` (gốc nội) | 430 / 988 | **1280 / 1768** |
| | `missle_speed_v` | 20→24 | 24→28 |
| cuiyan `neicuiyan150_2` (1102 tầng 2) | `colddamage_v` max | 720 / 1035 | **920 / 1235** |
| cuiyan `daocuiyan150` (1063 Băng Tước Hoạt Kỳ) | `missle_speed_v` | 24 | 28 |
| tianren `moren150` (1076 Tật Hoả Liệu Nguyên) | `firedamage_v`; `fatallystrike_p` | 760 / 1098; 30 | **1000 / 1500**; 25 |
| wudang `qiwudang150` (1078 Tạo Hóa Thái Thanh) | `lightingdamage_v` max | 420 / 852 (min 6→11) | **600 / 1200** (min 6→100) |
| wudang `jianwudang150_2` (1107 Kiếm Minh Thương Khung) | `stun_p` | 24 / 34 % | **34 / 47 %** |
| wudang `jianwudang150_3` (1105 Kiếm Vũ Diệu Nguyệt) | `stun_p` | bị chú thích | 24 / 34 % (dòng 1105 hiện không tham chiếu — chỉ đồng bộ bảng) |
| kunlun `jiankunlun150` (1106 Tạo Hoá… đạn Côn Lôn kiếm) | `lightingdamage_v` | 1130 / 2040 | **1430 / 2430** |
| kunlun `jiankunlun150fu` (1081 Thiên Lôi Chấn Nhạc) | `lightingdamage_v` | 250 / 356 | **350 / 486** |
| gaibang `zhanggaibang150` (1073 Thời Thặng Lục Long) | `skill_misslenum_v` | bị chú thích | 1 → 2 (c12) → 3 (c20) (dòng 1073 không tham chiếu — đồng bộ bảng) |
| shaolin `gunshaolin150` (1056 Vi Đà Hiến Xử) | `anti_block_rate`, `skill_eventskilllevel`, `skill_showevent`, `skill_startevent`→1201 (+`missle_missrate` đã thêm đợt SK120) | thiếu | có |
| shaolin `quanshaolin150` (1055) | `anti_block_rate` 3 → 10 % | thiếu | có |
| shaolin `daoshaolin150` (1057 Tam Giới Quy Thiền) | `skill_showevent` (mở chú thích), `skill_startevent` (bỏ chữ `--double x` lạc trong bảng) | | |

Không đổi (giống nhau hoặc JX1-only vô hại): Thiếu Lâm/Đường Môn/Nga My/Cái Bang bổng bảng sát thương giống Linux; `addskillexp1` (JX1); các bản `npc`/`附属` 1146‑1168 giống.

Kiểm: `check_encoding.py` high‑byte không đổi (chú thích GBK trong dòng Linux chèn vào đã thay bằng `--[SK150 07/09] nhu Linux`), FFFD = 0; `kiem_54.py` 8 tệp 0 lỗi; client byte‑một với server (trừ `kunlun.lua` dòng 176 lệch sẵn của phiên khác); rà lại bằng bộ trích ngoặc: **0 khoá / 0 ô còn lệch** trong họ 150.

## 3. Điều chủ sẽ thấy sau restart (server + client)

1. Sát thương 150 của Thiên Vương/Ngũ Độc/Thiên Nhẫn/Võ Đang/Côn Lôn/Thúy Yên tăng 25‑65 % ở cấp 20 (bảng trên); Thiên Vương chuỳ có thêm băng 60‑300.
2. Kỹ năng 150 lên được cấp **21‑26** bằng exp kỹ năng (cấp 26 ≈ gấp đôi cấp 20: Vi Đà Hiến Xử `physicsenhance_p` 425 → 866). Khung kỹ năng client hiện tối đa 26.
3. Hào Hùng Trảm không còn "trượt"; 1201 Vi Đà Hộ Pháp (Thiếu Lâm côn) xuất hiện khi dùng 1056 (tỷ lệ 1 → 50 % theo cấp, `missle_missrate` 99 → 50).
4. Long Thủ Bát Nhã Thủ (1083) hết choáng.

Lùi: đổi tên `*.truoc_sk150_0709` về tên gốc (9 tệp server + 9 tệp client).

## 4. Engine — 5 khác biệt công thức sát thương (CHƯA ĐỔI, chờ chủ chọn)

Đối chiếu `KNpc::AppendSkillEffect` (JX1 `KNpc.cpp:5190‑5546`) với Linux `0x0807CE70` + 5 helper (`0x0807C700` vật lý, `0x0807C950` băng, `0x0807CD30` hoả, `0x0807CA90` lôi, `0x0807CBD0` độc). Bố cục ô sát thương Linux: 0 `seriesdamage_p`, 1 `attackrating`, 2 `ignoredefense`, 3 vật lý, 4 băng, 5 hoả, 6 lôi, 7 độc, 8 phép, 9‑11 hút, 12 `knockback_p`, 13 chí mạng, 14 chí tử, 15 choáng, 16‑17 `addskillexp1/2` (JX1: 3 series, 9‑13 hệ, 14 choáng, 15 ignoreneg, 16 randmove — chỉ nội bộ, không cần đổi).

| # | Khác biệt | Linux | JX1 | Ảnh hưởng | Đề xuất |
|---|---|---|---|---|---|
| **a** | **Nội công cộng vào chiêu phi vật lý** | mọi hệ băng/hoả/lôi/độc/phép phi‑vật‑lý: `min/max += NộiCông hệ đó (vũ khí/trang bị) + KPlayer::m_nCurEngergy` (hàm `0x080A7C70` trả `[KPlayer+0x5954]`, cùng thứ tự Sức mạnh/Thân pháp/Sinh khí/**Nội công**/May mắn như `KPlayer.h:709‑713`) | `KPlayer.cpp:9521 nMagicBase = 0` — cố ý tắt ("để 0 là không sử dụng"), chỉ cộng nội công hệ; sau đó vá tay `lightingdamage_p` theo nội lực tối đa và nhân cứng vài chiêu 90 (mục e) | Nhân vật nội công 150 có ~400‑800 Nội công ⇒ Linux cộng thẳng +400‑800 vào min/max **mọi** chiêu phi vật lý (90/120/150). Đây là khác biệt lớn nhất còn lại cho "dame nội công yếu" | Chọn 1 trong 2: **A1** như Linux (cộng `m_nCurEngergy`) — nên đi cùng bỏ mục e; **A2** giữ |
| b | `physicsenhance_p` × tăng sát thương kỹ năng | `(gốc+add)·(100+p)/100·(100+tăng)/100` | `gốc·(100 + p·(100+tăng)/100)/100` — tăng chỉ nhân phần `p` | chỉ khi có `skill_enhance`/`addskilldamage`: JX1 thấp hơn 4 % (p=425) → 9 % (p=150) | đổi như Linux (1 dòng) |
| c | Hoả sát (`m_CurrentFireEnhance`) | chỉ nhân vào **max** của chiêu; **không** nhân vào hoả vũ khí | nhân cả min lẫn max và cả hoả vũ khí | JX1 **mạnh hơn** Linux cho mọi chiêu hoả | giữ (có lợi người chơi) hoặc như Linux |
| d | `seriesdamage_p` cấp NPC (trang bị/trạng thái) | handler idx 75 cộng vào `+0x13ec`, ô 0 = NPC + chiêu | không có handler cấp NPC (`KSkills.cpp:2659` chỉ ô chiêu) | chỉ khi có trang bị mang `seriesdamage_p` | thêm nếu có vật phẩm dùng |
| e | Nhân cứng của JX1 | không có | 337/380 băng ×3, 362/357 hoả ×2, 359 hoả vật lý ×2, 365/375 lôi ×3, 372 ×2,8, 368 ×1,2; `DamePecentToLevel` +(cấp−100)/5 % cho phi vật lý; `+= m_PhysicsMagic` (=0) | các chiêu 90 mạnh gấp 2‑3 lần Linux ⇒ so với 150 (đã về Linux) càng thấy "150 yếu" | bỏ **cùng lúc** với A1, hoặc giữ cả hai |

Không phải khác biệt: băng/lôi/độc/hút/chí mạng/chí tử/choáng/`attackrating_p`/hệ số loại vũ khí (`0x080B0D50` = `m_CurrentMeleeEnhance[loại]`/`Range`/`Hand`) — trùng JX1.

## 5. "Skill 180" — bằng chứng

| Nguồn | Kết quả |
|---|---|
| `settings/skills.txt` Linux (1 606 dòng) | 0 dòng `ReqLevel = 180`; 0 `LvlData` chứa "180"; 64 id chỉ‑Linux đều là boss/Cổ Tháp/Huyết Chiến/Chí Tôn Ấn Giám (1618‑1628, ReqLevel 120)/Hero Tower/sự kiện — xem `audit_skills_diff.txt` |
| `script/` Linux (`.lua`) | không có bảng `*180*`; chuỗi "Skill 180" chỉ ở `global/thinh/npc/npc_vocong.lua` (NPC nhà vận hành VN) và `updateconfig/tasklist.lua` ("Task Kiểm Tra Skill 180" = task 5002) |
| `npc_vocong.lua` | `tbFaction[..].tbSkill` = {90, 90, 90, 120, 150, 150, 150, 122x} mỗi phái (ví dụ Thiếu Lâm {318, 319, 321, 709, 1055, 1056, 1057, 1220}), cộng `AddMagic(1901,20) ---- level 180` — **1901 không có trong skills.txt** cả hai bản (chỉ là id vật phẩm/nhiệm vụ ở script khác) |
| Nhị phân `jx_linux_y` | 0 chuỗi "180"+skill/级; bảng 336 tên thuộc tính không có nhóm 180; 3 lần dùng hằng 0x76D (1901) là tham số tác vụ, không phải kỹ năng |
| Client `game_y_unpacked.bin` | 0 chuỗi |

Kết luận: bản Linux này không chứa "bộ skill 180"; NPC "Skill 180" chỉ là tên gọi của nhà vận hành cho bộ 90/120/150 (+1220‑1229 đã có ở JX1). Nếu chủ muốn "180" thật, phải **thiết kế mới** (không có nguồn để dịch ngược); nội dung Linux có mà JX1 chưa có gần nhất là **Chí Tôn Ấn Giám** 1618‑1628 (tăng cường kỹ năng 120 theo phái, cần 6 thuộc tính mới `dec_pskill_cdtime`, `dec_percasttime`, `enhance_709_auto`, `movedistanc_710_enhance`, `validtiem_1366_enhance`, `daoxutian_enhance`) và các kỹ năng Cổ Tháp/Huyết Chiến (1605‑1616).

## 6. Việc chủ cần làm

1. Chạy `ChayGameServer.bat` + `ChoiGame.bat` (chỉ dữ liệu; `.moi` CoreServer 1a33f617 của đợt SK120 vẫn đang chờ swap — đi cùng lần này).
2. Kiểm mục 3.
3. Chọn phương án mục 4 (a/e đi cặp; b; c; d) — tôi thi công ngay khi có quyết định.


---

# PHẦN 7 (07/09, sau khi chủ chọn) — CÔNG THỨC LUYỆN KỸ NĂNG 90/120/150 + 2 công thức sát thương đã đổi

Quyết định của chủ: (a) giữ nguyên nội công (A2); (b) `physicsenhance_p` đổi như Linux; (c) hoả sát như Linux; (d) rà công thức luyện kỹ năng 90/120/150 vì "một số phái luyện rất lâu, một số rất nhanh".

## 7.1 Gốc "phái nhanh / phái chậm" — JX1 cộng exp theo TỪNG HỆ sát thương

JX1 (`KNpc.cpp` `CalcDamage` ~4590, đã bỏ): mỗi lần `CalcDamage` có sát thương > 0 ⇒ `AddSkillExp90(Skill90Rate × hệ số)` cho kỹ năng đang dùng (`m_ActiveSkillID`, kể cả 150) và `AddSkillExp120(...)` cho **mọi** kỹ năng kinh nghiệm không nhắm địch. `CalcDamage` được gọi **riêng cho từng hệ** (vật lý, băng, hoả, lôi, độc — `KNpc.cpp:4905‑4941`), cho **từng mục tiêu**, từng viên đạn, và **cả từng nhịp độc** ⇒ với `Skill90Rate = 10` (gamesetting.ini):

| Kiểu chiêu | exp / lần đánh (JX1 cũ) |
|---|---|
| 1 hệ, 1 mục tiêu (Thiếu Lâm côn, Thúy Yên đao, Đường Môn phi tiêu) | 10 |
| 2‑3 hệ (Thiên Vương chuỳ, Nga My kiếm, Côn Lôn đao…) | 20‑30 |
| AoE 5 mục tiêu (Cái Bang, Thiên Vương) | 50‑150 |
| Độc DoT (Ngũ Độc): mỗi nhịp độc lại +10 | hàng trăm |

Thêm 2 lỗi phụ: đọc hệ số x2 của `Player[CLIENT_PLAYER_INDEX]` (người chơi ngẫu nhiên ở khe 1) thay vì của người đánh; chặn ở `MAX_TRAIN_SKILLEXPLEVEL = 20` nên kỹ năng 150 **không bao giờ lên 21‑26** dù đã mở MaxLevel 26.

## 7.2 Luật Linux (mổ `jx_linux_y`)

| Mục | Linux | Địa chỉ |
|---|---|---|
| Ngưỡng lên cấp | `skill_skillexp_v` (idx 8) đọc từ Lua, `GetSkillNextExp` trả `KSkill+0x11c` ở **cấp hiện tại**; **không** đọc `magic_level_exp.txt` (không có chuỗi `LEVEL%d`) | `0x0812AC50` |
| So với JX1 | JX1 dùng `magic_level_exp.txt`: tính lại 62 kỹ năng có cả hai nguồn (kể cả `SkillExpFunc(6312, 1.15…)`) ⇒ **62/62 trùng** ⇒ ngưỡng không phải nguyên nhân | |
| Exp khi đánh (90) | trong `ReceiveDamage`, sau khe choáng: duyệt ô sát thương 16/17 `addskillexp1/2` của chiêu: `v0 > 0`, `rand(100) > 59 ⇒ bỏ` (**60 %**), cờ `v2 & 2` ⇒ cộng cho **nạn nhân** nếu là người chơi, không thì cho **người phát** (phải là người chơi/đồ đệ). `KSkillList::AddSkillExp`: chỉ kỹ năng kinh nghiệm, cấp < MaxLevel, `exp += v1` (cờ 0), đủ ngưỡng ⇒ lên cấp | `0x0808AA3C‑0x0808AA97`, `0x080E5D90` |
| Dữ liệu 90 | `addskillexp1 = {{1,0(=chính chiêu) hoặc id chính},{1,1},{20,1}}` ⇒ **1 exp / lần trúng / mục tiêu**, không nhân theo hệ; 2 chiêu `{20,10}` (qianfo_qianye Nga My, wusuo_kunlun Côn Lôn) | 47 bảng / 14 tệp Lua |
| 120 | không có exp khi đánh. `Add120SkillExp(nExp)` (`0x080A9B50`): cộng cho **1 kỹ năng 120 đã chọn** (task 2463), có **trần ngày** (task 2464/2465, trần toàn cục `0x8fbf4a0`), `add120skillexpenhance_p` (+%); gọi từ `task_award_extend.lua` (½ exp nhiệm vụ) và tu luyện ngoại tuyến | `0x0811C710` |
| 150 | **không có exp khi đánh** (không dòng nào có `addskillexp`); tu luyện qua NPC tốn exp nhân vật (`event/skillexp_150/skillexp_150_main.lua`, `vng_feature/skill_150_training.lua`, giới hạn ngày) và vật phẩm Thiên Sơn Thánh Thủy 30314 (50 exp) / 30449 (6 000 exp) | script VN |

## 7.3 Đã thi công — commit `[SKEXP 07/09]` origin/main, `CoreServer.dll` **fd2d4893** (18 480 640) đặt vào khe `.moi` (thay 1a33f617 của đợt SK120, cùng nguồn), CoreClient build riêng (xem trạng thái cuối)

Engine (`ReverseTools/goi_va_sk150_luyen_0709.py`, 16 hunk):

| # | Tệp | Thay đổi |
|---|---|---|
| H1‑H2 | `KSkills.h/.cpp` | `m_AddSkillExp[2]` đọc thẳng tên `LvlSetting` `addskillexp1/2` trong `LoadSkillLevelData` (JX1 không có tên này trong `KMagicDesc`; **không** thêm vào enum để không đổi số thứ tự thuộc tính đang đồng bộ client) |
| H3 | `KNpc.h/.cpp` | `CongExpKyNangKhiTrung(skill, level, victim)` = luật Linux 7.2 (60 %, cờ 2, chỉ người chơi, bỏ qua bot SimCity); `exp = v1 × Skill90Rate × x2 của chính người nhận` (`Skill90Rate = 1` ⇒ đúng Linux; ini hiện 10) |
| H4 | `KPlayer.h/.cpp` | `AddSkillExpKhiTrung`: chỉ IsExp, chặn theo **MaxLevel thật** (150 lên được 26), đồng bộ client |
| H5 | `KMissle.cpp` | gọi H3 ngay sau khi `ReceiveDamage` trả TRUE (mỗi lần trúng mỗi mục tiêu) |
| H6 | `KNpc.cpp` | **bỏ** hai khối `AddSkillExp120/90` theo hệ; `AddSkillExp120/90` cũ giữ nhưng không còn ai gọi; `Skill120Rate` không còn dùng |
| H7 | `KNpc.cpp` + `KPlayer.cpp` (tooltip) | `physicsenhance_p`: `(gốc+add)(100+p)/100 × (100+tăng)/100`; hoả sát chỉ nhân **max**, hoả vũ khí không nhân — cùng công thức ở server và bảng hiển thị client |

Dữ liệu (server + client, sao lưu `*.truoc_skexp_0709`; gương commit cùng):

| Nhóm | Sửa |
|---|---|
| 90 | 24 bảng có ở cả hai bản chép nguyên dòng Linux (`v1 = 1`, riêng qianfo_qianye/wusuo_kunlun `1 → 10`); các bảng Hoa Sơn/Vũ Hồn/Tiêu Dao (JX1) đã sẵn dạng `{0},{1,1}` — giữ |
| 150 | Linux không có exp khi đánh (7.2). **Dự án giữ luyện bằng đánh** nhưng theo đúng luật Linux: 40 bảng họ 150 (chính + tầng 2/3 + đạn con) thêm `addskillexp1 = {{1,<id chính>},…},{{1,1},{20,1}}` và 54 dòng `skills.txt` thêm `LvlSetting addskillexp1` (4 dòng đạn con 1094/1096/1098/1131 hết ô trống — không cộng, không ảnh hưởng vì chiêu chính có) ⇒ mỗi lần trúng = 1 × Skill90Rate, 60 %, không nhân hệ. Nếu chủ muốn **đúng hệt Linux** (NPC tu luyện tốn exp nhân vật + vật phẩm) thì port `skillexp_150_main.lua` + item 30314/30449 — việc riêng |
| 120 | như Linux: không exp khi đánh (khối cũ đã bỏ); `Add120SkillExp` Lua của JX1 (nhiệm vụ) giữ nguyên — JX1 cộng cho **mọi** kỹ năng 120 không nhắm địch, Linux cộng 1 kỹ năng đã chọn có trần ngày (cần NPC chọn kỹ năng — chưa port) |

Kiểm: `kiem_54.py` 0 lỗi 14 tệp; mã hoá không đổi; client byte‑một (trừ `kunlun.lua` dòng 176 sẵn khác). Sự cố trong lúc làm: bộ chèn coi `SKILLS` là bảng ⇒ chèn dòng lạc dưới `SKILLS={` và bước sửa xoá nhầm dòng gốc của Hoa Sơn/Vũ Hồn/Tiêu Dao ⇒ đã khôi phục 3 tệp từ sao lưu, rà lại từng dòng theo bảng chủ: 0 vấn đề.

## 7.4 Kết quả mong đợi sau restart

| | Trước | Sau |
|---|---|---|
| Thiếu Lâm côn (1 hệ, 1 mục tiêu), Skill90Rate 10 | 10 exp/đòn | 6 exp/đòn (10 × 60 %) |
| Thiên Vương chuỳ (2 hệ) | 20 | 6 |
| Cái Bang AoE 5 mục tiêu | 50 | 30 (5 mục tiêu × 6) — Linux cũng theo mục tiêu |
| Ngũ Độc độc DoT | +10 mỗi nhịp độc | chỉ lúc trúng |
| Kỹ năng 150 cấp 21‑26 | không lên được | lên được (exp 90k … 300k / cấp) |
| Kỹ năng 120 | +10 mỗi hệ mỗi đòn cho mọi buff (vô nghĩa với ngưỡng 17,8 triệu) | chỉ qua nhiệm vụ (`Add120SkillExp`) |

Muốn đúng Linux 100 % về tốc độ: đặt `Skill90Rate = 1` trong gamesetting.ini (1 exp/đòn, 60 %).
