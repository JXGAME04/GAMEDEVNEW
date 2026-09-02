# BÀN GIAO — PHÁI VŨ HỒN (id 11) & TIÊU DAO (id 12) — thi công đợt 1 (02/09/2026)

Marker mã/dữ liệu: `[VHTD 02/09]`. Tool: `D:\GAMEDEVNEW\ReverseTools\phai3\vhtd_thicong\`. Bản lưu mọi tệp bị sửa: `<tệp>.truoc_vhtd_0209` cạnh tệp.

Yêu cầu chủ (02/09): *"dựa vào client vltk - và linux để làm tiếp 2 phái còn lại luôn (ưu tiên lấy dữ liệu client vltk … phần không có của 2 phái bạn phải dựa vào client vltk để làm thêm cho chuẩn) — làm xong chạy phản biện phần nào dễ bị lỗi"*.

Nguồn: **client VLTK Level Up** (`C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky`, trích tại `ReverseTools\phai3\vltk_raw\`) là nguồn chính vì **Linux `D:\ServerLinux` KHÔNG có hai phái này** (skills.txt Linux thiếu, faction_def Linux mã Tiêu Dao cũ); Linux chỉ dùng để đối chiếu 5 thuộc tính engine chung (`autocastskill 0x08097420`, `invincibility 0x080961B0`, `forbit_attack 0x08096150`, `melee_returnres_p 0x08096390`, `anti_lightingres_p 0x08096B50`, map `addphysicsdamage_p` 10/11/12 → ô 6/7/8 tại `0x0809A7F0`).

---

## 1. Tóm tắt kết quả

| Lớp | Trạng thái | Ghi chú |
|---|---|---|
| Kỹ năng (62 dòng skills.txt, 27 đạn, Lua `wuhuntang.lua`/`xiaoyao.lua`, `advancedskill.lua` +`xy_adskill`) | **XONG** server = client | chép nguyên byte VLTK, áp luật "bỏ (Dương)" của chủ (mục 3.1) |
| Engine 15 thuộc tính mới (310–324) + 5 handler theo Linux | **XONG, build sạch 3 binary** | CoreServer x64 / CoreClient Win32 / Game.exe — chờ swap |
| Script môn phái (17 tệp sửa + 3 tệp mới) | **XONG** | `vhtd_faction_script.py`; `t71` = 0 lỗi gọi nil thân chunk |
| NPC (458 hàng VLTK), bản đồ 1042–1046 + 1057, FactionInfo.ini [11]/[12] | **XONG** (tác tử) | `vhtd_npc_map.py`; server pak `8=maps_vuhon_tieudao.pak`, client `37=` |
| Sprite kỹ năng/trạng thái 5.818 mục | **XONG** (tác tử) | `sprvuhontieudao.pak` 371,9 MB, client `package.ini 36=`; bảng trạng thái +20 dòng |
| Vật phẩm 10 món (4967–4976) | **XONG** | `vhtd_items.py`; 4970 (Bí kíp 150) script VLTK không có ở JX1 → 0 |
| UI bảng kỹ năng phái 11/12 | **XONG** | `vhtd_ui.py` + `khung_wh.spr`/`khung_xy.spr` |
| Mô tả kỹ năng MagicDesc.ini | **XONG** | 5 dòng VLTK + 10 dòng tự viết TCVN3 |
| **Vũ khí Đao Thuẫn/Thuẫn Đao (particular 7/8) + Cầm (range 3)** | **XONG** dữ liệu (mục 8) + **giao thức WeaponType WORD** (mục 12.1, chủ quyết 02/09) | `vhtd_weapons.py`: 40 hàng item + res + objdata + ClientWeaponSkill; không có vũ khí thì kỹ năng Vũ Hồn (EqtLimit 7/8) và Cầm Pháp (103) bị từ chối ở `KSkills.cpp:280` |
| NPC môn phái theo **vị trí Linux** (6 thôn Vũ Hồn, 8 thôn Tiêu Dao, 17 NPC map 1042, 9 NPC map 1057) + Thuyền Phu về thành + lựa chọn "Đến phái" | **XONG** (mục 12.3) | `vhtd_faction_script.py --chi-npc`; `npc_chao.lua` / `thuyenphu_*.lua` |
| **Bẫy cũ Hoa Sơn:** `maps_hoason2013.pak` header 16 byte bị `XPackFile::Open` từ chối | **ĐÃ SỬA** (mục 7) | pak mới đã đặt tại `bin\server\Pak\`, bản cũ `.truoc_vhtd_0209`; hiệu lực khi restart |

---

## 2. Dữ liệu kỹ năng — `vhtd_skills.py`

- **skills.txt** (server + client, header 114 cột trùng VLTK): +62 dòng id **1963–1992** (Vũ Hồn), **2114–2143** (Tiêu Dao), **1733/1734** (bản id cũ của 1972 Đoạt Cơ / 1974 Trường Anh Huy Xích trong dữ liệu VLTK — giữ để đủ bộ, không dùng; đòn thường thuẫn dùng 53, cầm dùng 2135). Client còn hơn server 2 dòng cũ 1561/1562 (có từ trước).
- **missles.txt**: +27 dòng đạn 523,525–529,531,574–580,637–649 → `MAX_MISSLESTYLE 450 → 700` (`SkillDef.h:10`).
- **Lua**: `script\skill\wuhuntang.lua` (26.301 byte), `xiaoyao.lua` (19.467) chép nguyên byte + đổi khoá yan; `advancedskill.lua` chèn hàm `LineWith8Byte` và bảng `SKILLS.xy_adskill` (tiến giai Tiêu Dao 2132/2133/2134) — không đụng `sl/tw/gb_adskill`.
- **magic_level_exp.txt** +10 dòng: 1967, 1969, 1983, 1984, 1985, 2124, 2127, 2129, 2141, 2142 (ngưỡng tính từ `SkillExpFunc` trong Lua VLTK). **1970** (Huy Sư Diệt Lỗ thức 2) khai `skill_skillexp_v` nhưng bảng Lua `huishi_mielu_2` không có khoá → `GetSkillLevelData` trả rỗng (không lỗi) → không thêm.
- **MagicDesc.ini** +15 khoá (5 chép VLTK: lightingdamage_p, addlightingmagic_p, special_point_base, lifereplenish_dec_p, unravel_effect; 10 tự viết: special_point_add, cost_sp, autocastskill, lock_life, cast_when_buff_removed, reset_bufftime, resume_life_p, hidebodyunlock, invincibility, forbit_attack). JX1 đã có sẵn 4 khoá **rỗng** (autocastskill, lock_life, cast_when_buff_removed, resume_life_p) → tool điền tại chỗ.

### 2.1 Bộ kỹ năng (từ `tbSkillID` client VLTK)

| | Vũ Hồn (11) | Tiêu Dao (12) |
|---|---|---|
| 10 | 1972 Đoạt Cơ (thuẫn), 1974 (đao) | 2114 (kiếm), 2136 Tùy Ý Khúc (cầm) |
| 20 | 1964 Vũ Hồn Thuẫn Pháp, 1975 Vũ Hồn Đao Pháp | 2115 Tiêu Dao Kiếm Pháp, 2137 Tiêu Dao Cầm Pháp |
| 30 | 1976 (Nộ — `special_point_base`) | 2116 (Âm Luật — `special_point_base`) |
| 40 | 1965, 1977 | 2118, 2138 |
| 50 | 1963, 1979 | 2120, 2140 |
| 60 | 1980 Cổ Tướng Bách Chiến Hãm | 2121 |
| 70 | **1982 Vũ Mục Di Thư (trấn phái)**, 1971, 1981 | **2123 Thiên Lý Độc Hành (trấn phái)**, 2122 Hiệp Cốt Nhu Tình |
| 90 | 1967 Trấn Biên Thùy, 1983 Khát Ẩm Hung Nô Huyết | 2124 Kiếm Quyết Phù Vân, 2141 Cao Sơn Lưu Thủy |
| 120 | 1984 Trung Vu Lưu Phong | 2127 Ăn Uống No Say |
| 150 | 1969 Huy Sư Diệt Lỗ, 1985 Trừ Gian Diệt Nịnh | 2129 Thập Bộ Nhất Sát, 2142 Mai Hoa Tam Lộng |
| Tiến giai | 1986 | 2132 |

### 2.2 Luật "bỏ (Dương)" (quyết định chủ 02/09 đợt g, áp cho 2 phái)
Chiêu có cả nhánh yan + thường cùng giá trị → bỏ cột yan; chỉ có yan → đổi tên sang thường (khoá Lua đổi theo): 1968/1970 bỏ attackspeed_yan_v+castspeed_yan_v; 1971 bỏ lifemax_yan_p; 1980 bỏ attackspeed_yan_v, allres_yan_p→allres_p, fasthitrecover_yan_v→fasthitrecover_v; 2122 lifemax_yan_v→lifemax_v; 2123 lifemax_yan_p→lifemax_p, fasthitrecover_yan_v→fasthitrecover_v, allres_yan_p→allres_p, anti_lightingres_yan_p→anti_lightingres_p. **Giữ `sorbdamage_yan_p`** (JX1 `sorbdamage_p` là phần trăm ×10).

---

## 3. Engine — `vhtd_engine_patch1.py` + `vhtd_engine_patch1b.py`

| Thuộc tính (enum) | Handler / nơi xử lý | Nguồn |
|---|---|---|
| 310 `autocastskill` | `HS_AutoSkillModify(m_CastSkill)`; `KNpc::HS_AutoCastTick()` mỗi khung trong `ProcessState` (server): `Cast(id, cấp)` lên chính mình, hồi chiêu `wait`, tỷ lệ | Linux 0x08097420 / 0x0808BEC0 |
| 311/312 `special_point_base/add` | bộ đếm tầng `m_HSSp[4]` {khoá, số tầng, trần}; base đặt trần (không reset số tầng khi tính lại thuộc tính), add cộng byte thấp v2 | VLTK (Nộ 1976 / Âm Luật 2116) |
| 313 `cost_sp` | `KSkill::m_nCostSpKey/m_nCostSp` (đọc ở `ParseString2MagicAttrib`, không áp lên mục tiêu); `DoSkill` server: thiếu tầng → `goto Exit`, đủ → trừ | VLTK |
| 314 `lock_life` | `m_nHSLockLife/Mode`; `CalcDamage` + khe chí tử: chế độ 1 không cho máu tụt dưới giá trị | VLTK 1982 |
| 315 `cast_when_buff_removed` | `HS_OnStateRemoved` khi node hết hạn (node lưu giá trị đảo dấu): cast {id, cấp (-1 → cấp của chiêu tham chiếu)} | VLTK 1982→1991 |
| 316 `reset_bufftime` | immediate; `ModifyAttrib` chặn → `HS_ResetBuffTime(id)` trả `m_LeftTime` về `nValue[1]` thuộc tính trạng thái đầu | VLTK 1985→1988 |
| 317 `resume_life_p` | hồi ngay % sinh lực tối đa khi áp (bỏ qua khi gỡ) | VLTK 1991 |
| 318 `lifereplenish_dec_p` | `m_CurrentLifeReplenishPercent -= v0` | VLTK 1988 |
| 319 `unravel_effect` | chỉ lưu `m_nHSUnravel` (cơ chế "Tá lực" chưa rõ) | VLTK 2122 |
| 320 `lightingdamage_p` | `m_DamageAttribs[12]`; `AppendSkillEffect`: (nội công cơ bản + nội công Lôi × (100+addlightingmagic_p)/100) × p/100 × (100+nAddDamageP)/100 | **thiết kế theo mô tả VLTK** "Sát thương Lôi: #d1-%" (không có server chuẩn) |
| 321 `addlightingmagic_p` | `m_nHSAddLightMagicP` | VLTK 2137/2123 |
| 322 `hidebodyunlock` | `m_HideState.nTime = v1` (như `magic_hide`) | VLTK 2130 |
| 323 `invincibility` | `m_bHSInvincible` → `ReceiveDamage` return TRUE | Linux [0x147b] |
| 324 `forbit_attack` | `m_bHSForbidAttack` → `DoSkill` server chặn | Linux [0x1478] |
| 275 `melee_returnres_p` (đã có enum, chưa handler) | `m_nHSMeleeReturnResP` cộng vào kháng phản đòn khi `bIsMelee` | Linux [0x1264] |
| 200 `anti_lightingres_p` (đã có enum, chưa handler) | dùng chung `AntiLightingResYanP` | Linux 0x08096B50 |

Khác: `MAX_MELEE_WEAPON_VHTD 9` — `m_CurrentMeleeEnhance[10]` (ô 7/8 = Đao Thuẫn/Thuẫn Đao); `AddPhysicsDamageP` mã 10/11/12 → ô 6/7/8 (Linux), vòng ALL/MELEE_ALL 0..8; `KSkills.cpp` mô tả `[WeaponLimit]` case 7/8/103 (+ 3 dòng `gamesetting.ini` client); `UiSkillsNew.cpp` bảng ô phái 11/12 + nút đóng. Reset các trường mới ở khởi tạo, `RestoreNpcBaseInfo`, `KPlayer::UpdataCurData` (**số tầng Nộ/Âm Luật GIỮ**).

**Giới hạn biết trước:** cửa `cost_sp`/`forbit_attack` chỉ ở server → client vẫn diễn hoạt cast khi thiếu tầng (server không áp hiệu ứng). `HS_ResetBuffTime` dùng thời lượng gốc từ thuộc tính trạng thái đầu của chiêu buff.

---

## 4. Script môn phái — `vhtd_faction_script.py` (khuôn Hoa Sơn 01/09)

`factionhead.lua` (FACTION_INFO[12] `{3,"武魂堂",1,90,…}` Hậu Quân, [13] `{4,"逍遥派",3,98,…}` Tiêu Dao Cư Sĩ — rank 90/98 có trong `ranksetting.txt`; SKILLNORMAL/SKILL90/SKILL120AR/SKILL150 [12]/[13]), `hocvocong.lua` (bảng riêng + menu `hvcwh`/`hvcxy`, test 90 `skillvuhon`/`skilltieudao`, FACTION_TO_SKILL150), `lib_faction.lua`, `skillbook.lua` ([11]/[12] + sách 90: 4968/4969/4975/4976), `lvl120skillbook.lua`, `lenhbaitanthu.lua` (menu 13 phái; nhánh 12 `SetSeries(3)`, 13 `SetSeries(4)`), `skillfaction.lua`, `nangskillkieumoi5x/9x/tp.lua`, `daithanhbk90/120.lua`, `hotrotest.lua` (hệ Hoả → +11, Thổ → +12), `skills_table.lua` (`add_wh`/`add_xy`).
Mới: `npcthon\npcmonphai\vuhon.lua`, `tieudao.lua` (khuôn `hoason.lua`, `gianhapmonphai(11/12)`, hệ 3/4, camp 0, cấp ≥ 10), `global\vhtd\npc_vhtd.lua` (16 NPC nhập môn ở 8 thôn: Vũ Hồn Hậu Quân = mẫu **2467**, Tiêu Dao Mật Sứ = mẫu **2607**; toạ độ Tiêu Dao lấy Linux script, Vũ Hồn cạnh Đệ Tử Hoa Sơn x−4/y+2 — **giả định, kiểm trong game**), `startgame.lua` Include + `add_npc_vhtd()`.
Khôi phục về trước đợt: `vhtd_khoiphuc_script.py`. **Chưa làm:** NPC chưởng môn trong map 1042/1057, lối về phái, nhiệm vụ môn phái VLTK (`wuhun2020`), Đại Nghệ Đầu Sư (tín vật 4967/4974 hiện không script — đổi phái vẫn qua Lệnh Bài Tân Thủ).

---

## 5. NPC / bản đồ / FactionInfo — `vhtd_npc_map.py` (tác tử) & sprite — `vhtd_spr_pak.py` (tác tử)

- `npcs.txt` + `NpcName=id.txt` (server+client) +458 hàng VLTK = id engine **2185–2642** (id = số dòng − 2; Nhạc Lôi 2468, Văn Bán Sơn 2608, Hậu Quân Vũ Hồn 2467, Tiêu Dao Cư Sĩ 2607). `npc_res_kind_file_name.txt` client +67 kiểu.
- `bin\server\Pak\maps_vuhon_tieudao.pak` (1344 mục, md5 71e54798) + `bin\client\data\maps_vuhon_tieudao.pak` (673 mục, md5 c5cd9c21), `WorldSet_GameServer.ini` World927..932 = 1042..1046, 1057; `MapList.ini` 6 khối; **đường dẫn phải là `\maps\map_publish\…`** (cách `..\map_publish` bị `RemoveTwoPointPath`+`FindElemFile` băm sai).
- `FactionInfo.ini` + `门派设定.ini` (4 tệp) `[11] 武魂堂/火/正派/wuhun/Vũ Hồn`, `[12] 逍遥派/土/中立/xiaoyao/Tiêu Dao`.
- `sprvuhontieudao.pak` 5.818 mục (md5 0f33e71c), `package.ini` client `36=`; `state_magic_table_name.txt` 224 → 244 dòng (Status236 thiếu spr ở cả VLTK).
- Còn thiếu: 9 kiểu NPC không nối (boss140–142, passerby465–468/382, obj_herb_006); script NPC `npclevelscript\cang_npc.lua`, `npc_newsj.lua`; 27 mã kỹ năng NPC (1604…2091) — không thuộc phạm vi hai phái người chơi.

---

## 6. Vật phẩm — `vhtd_items.py` (magicscript.txt server = client, 4978 dòng)

| JX1 | VLTK | Tên | Script |
|---|---|---|---|
| 4967 | 5103 | Vũ Hồn-Tín Vật | 0 |
| 4968/4969 | 5106/5107 | sách 90 Trấn Biên Thùy / Khát Ẩm Hung Nô Huyết | `skillbook.lua` |
| 4970 | 5215 | Bí Kíp Tiêu Dao 150 | **0** (`dachengmiji_150.lua` không có ở JX1; 150 học qua NPC `hocvocong`) |
| 4971–4973 | 5216–5218 | Bí Kíp Tiêu Dao 150 Lv21–23 | 0 (VLTK cũng noscript) |
| 4974 | 5230 | Tín vật Tiêu Dao Phái | 0 |
| 4975/4976 | 5231/5232 | sách 90 Kiếm Quyết Phù Vân / Cao Sơn Lưu Thủy | `skillbook.lua` |

---

## 7. Bẫy phát hiện: pak bản đồ Hoa Sơn 987 chưa từng nạp được

`hs_map_pak.py` (01/09) ghi header **16 byte**; `XPackFileHeader` = 32 byte (`cSignature[4], uCount, uIndexTableOffset, uDataOffset, uCrc32, cReserved[12]`) và `XPackFile::Open` (`XPackFile.cpp:101`) từ chối `uIndexTableOffset < 32` ⇒ server không mở `maps_hoason2013.pak` (không có log). Đã sửa tool (header 32, `bytes(12)` reserved), tạo lại `bin\server\Pak\maps_hoason2013.pak` (651.870 byte, md5 64f05d22, đọc lại khớp 309/309), bản cũ `.truoc_vhtd_0209`. Hiệu lực sau restart GameServer → kiểm map 987 Hoa Sơn.

---

## 8. Vũ khí Đao Thuẫn / Thuẫn Đao / Cầm — `vhtd_weapons.py` (tác tử khảo sát, đã áp `--ghi-that --obj-cam=484`)

Đo trên dữ liệu VLTK: `meleeweapon.txt` ParticularType 6 = Triền Thủ, **7 = "Tinh Chế Đao Thuẫn"**, **8 = "Tinh Chế Thuẫn Đao"** (10 hàng/loại, yêu cầu phái đầu 11 qua `magic_requiremenpai`), `rangeweapon.txt` **P3 = "Mộc Cầm"** (DetailType 1, phái 12). Kỹ năng: EqtLimit **7/8** (Vũ Hồn) và **103** (= 3 + `MAX_MELEEWEAPON_PARTICULARTYPE_NUM` 100, `KSkills.cpp:270`) — cửa `KSkills.cpp:280` so đúng, không cần đổi. Mã 11/12 chỉ là `nValue[2]` của `addphysicsdamage_p` trong Lua (đã map 10/11/12 → ô 6/7/8 theo Linux).

Chỉ số engine phải giữ: `KItemGenerator.CPP:302/478/1415` record = `P*10+L-1` (P6 60..69 **bắt buộc đi kèm** để P7 70..79, P8 80..89 đúng vị trí); `KItemChangeRes.cpp:86-93` MeleeRes id 62..71→2, 72..81→270, 82..91→271, RangeRes hàng dữ liệu 30..39→278 (JX1 có hàng thừa đầu bảng nên id lệch 1 so VLTK); `KObjSet.cpp:340/356` ObjData DataID = số hàng → nối 463..484 liên tục (480 Đao Thuẫn `obj_wq_010`, 484 Cổ Cầm `obj_wq_011`, còn lại Tống Kim/Thanh Minh đi kèm); `ClientWeaponSkill.txt` +3: (0,7)→53, (0,8)→53 (đòn thường vật lý cận chiến, như đơn đao), (1,3)→2135 (đòn thường cầm VLTK, EqtLimit 103). 1733/1734 KHÔNG phải đòn thường — là bản id cũ của 1972/1974 (giữ trong skills.txt, không dùng).

Đã ghi (server = client): meleeweapon +30 (P6/P7/P8), rangeweapon +10 (P3), MeleeRes sửa 30 id, RangeRes sửa 10 hàng, ObjData +22, ClientWeaponSkill +3; bản lưu `.truoc_vhtd_0209`.

Đồ họa: client JX1 đã có động tác/res 268 "Vũ Hồn Tân Thủ-Đao" (SwordShieldW*), 269 "-Thuẫn" (ShieldSwordW*), 276 "逍遥琴1" (GuqinW*) và thân 156-157 "Bộ Vũ Hồn 1/2"; mọi truy cập NpcRes có kiểm biên → không crash. **Spr động tác ĐÃ XÁC MINH** (`vhtd_spr_pak2.py --kiem`, danh sách `vhtd_spr_pak2_danhsach.tsv`): 16 hàng × 75 động tác = 576 tên khác nhau, sinh đúng theo `KNpcResNode::Init` → 462 có sẵn trong pak JX1 (updatejx15 288, spr.pak 104, updatejx16 70); **toàn bộ 378 tên engine thật sự chạm tới** (qua `WeaponActionTab1/2`, 62/hàng thân) **đều có**, 9 mẫu giải mã SPR đúng → không cần pak mới. 114 tên thiếu ở cả JX1 và VLTK đều là ô `_000_` (vũ khí trống) của cột động tác không dùng (+ `FM_RW_144_WK06`, 4 mã lạ RN65/RN66/IJ64/IJ65 gõ sai trong bảng VLTK) — quy ước VLTK, không thay thế. Bảng đặc lệ VLTK (`PartFileNameSpecial`) engine JX1 không có → chỉ thẩm mỹ.

### 8.1 CHỜ CHỦ QUYẾT — trường đồng bộ `WeaponType` là BYTE
`KProtocol.h:39, 435` `BYTE WeaponType`, `KNpc.cpp:6134, 6339` ép `(BYTE)m_WeaponType`. Res vũ khí thuẫn/cầm = 268/269/276 (> 255) ⇒ **người chơi khác** nhận 12/13/20 → thấy sai hình vũ khí (máy mình vẫn đúng). JX1 hiện mọi res vũ khí ≤ 248 nên chưa lộ; hàng < 256 của bảng NpcRes đã dùng hết (Thiên Mệnh tới 259). Sửa = đổi `BYTE → WORD` ở 2 chỗ + bỏ ép kiểu = **đổi giao thức** (client + server swap cùng lúc; WAuto nếu đọc gói này cũng phải xem lại) → **KHÔNG tự làm**, chờ chủ quyết. Cùng lý do: `goldequipres.txt` Vũ Hồn 7059-7338 đang trỏ res 237-248 (Thiên Mệnh) thay vì 262/263.

---

## 9. Kiểm đã chạy
- Build sạch: `CoreServer.dll` (Server Release|x64), `CoreClient.dll` (Client Release|Win32), `Game.exe` (Release|Win32) — sau khi sửa lỗi client (khai báo HS_* nằm trong `#ifdef _SERVER`, patch1b).
- Dữ liệu server = client: skills (trừ 2 dòng cũ), missles, Lua, exp, MagicDesc, magicscript.
- `t71_quet_goi_nil_thanchunk.py`: 0; 18 tệp script: không `EF BF BD`, ngoặc `{}` cân, CRLF (startgame.lua có 5 LF lẻ từ trước).
- Mã hoá: mọi chữ Việt tự viết dùng bộ `vn_to_octal.unicode_to_tcvn3_bytes` (skill swordonline-dev) — bảng tự gõ ban đầu SAI vị trí dấu, đã thay và ghi lại (gamesetting.ini, MagicDesc, script).

## 9.1 PHẢN BIỆN (tác tử đối kháng, chỉ đọc) → vá `vhtd_engine_patch1c.py` (marker `[VHTD 02/09c]`)

| # | Mức | Phát hiện (tệp:dòng trước vá) | Vá |
|---|---|---|---|
| A1 | **Lỗi chắc chắn** | `HS_OnStateRemoved` (`KNpc.cpp:3419-3426`): `cast_when_buff_removed = {1991,-1,1984}` — cấp −1 lấy cấp của 1984 (kỹ năng **120**), chưa học → `GetCurrentLevel` = 0 → mã cũ ép cấp 1 ⇒ mọi người có 1982 (cấp 70) được 1991 = `resume_life_p` hồi **30 % máu miễn phí** | chưa học kỹ năng tham chiếu → **không cast** (`continue`) |
| A2 | **Lỗi chắc chắn** | `HS_AutoCastTick()` đặt trong khối `if (!(m_LoopFrames % GAME_UPDATE_TIME))` (`KNpc.cpp:1119`, = 10 khung) → chu kỳ 18 khung (2117 Âm Luật, 2134 Phất Y) thành 20; hộ thuẫn 17 khung hở 3 khung/chu kỳ. Khai thác: `UpdataCurData` → `HS_ResetVhtd` (memset `m_CastSkill`) → `ReCalcState` tạo mục mới `dwNextCastTime = now` ⇒ đổi trang bị/lên cấp = **+tầng Nộ/Âm Luật tức thì** | tick chuyển ra **mỗi khung** (đúng Linux 0x0808BEC0); `AutoCastSkill`: mục **mới** tạo → lần đầu sau `wait` khung, mục cũ giữ đồng hồ |
| B1 | Nghi ngờ | `ReceiveDamage` bất tử `return TRUE` = "trúng" → `KMissle.cpp:1266-1311`, `KSkills.cpp:2427-2456` vẫn áp trạng thái (choáng/độc) lên người bất tử | đòn của **địch** → `return FALSE` (trượt: không sát thương, không trạng thái); buff tự thân/phe ta (2130/2131/1989/2117 đi qua `ReceiveDamage(self)`) vẫn qua |
| B2 | Nghi ngờ (cân bằng) | `lock_life` chỉ kẹp khi máu **đang trên** ngưỡng; máu đã ≤ khoá (100..3400 tuyệt đối của 1982) thì buff vô tác dụng | theo nghĩa chữ chế độ 1 "không thể giảm dưới X": máu ≤ khoá → sát thương 0 (cả `CalcDamage` và khe chí tử). **Chủ cân nhắc** nếu muốn khác |
| B3 | Nghi ngờ | `KNpc::Cast(int,int)` (`KNpc.cpp:5788`) không kiểm NULL `GetSkill` — nay gọi bằng id từ dữ liệu | thêm `if (!pOrdinSkill) return;` |
| B4 | Nghi ngờ | `DoSkill` ép `(KSkill*)pSkill` — `KThiefSkill` (style 13) không kế thừa KSkill (dữ liệu hiện không có style 13) | chỉ ép khi `eStyle != SKILL_SS_Thief` |
| C | Ghi chú | desync `cost_sp`/`forbit_attack` (client vẫn diễn hoạt + trừ nội lực cục bộ, server im lặng) — đã khai mục 3; `HS_ResetBuffTime` đúng nạn nhân/đúng dấu, chỉ lệch đồng hồ icon client; `lightingdamage_p` ô [12] không đôi sát thương, **tooltip client chiêu cầm không hiện dòng Lôi** (`KPlayer.cpp ~10035` chỉ xét `_v`); duyệt `m_StateSkillList` khi cast không UAF; `#ifdef _SERVER` nhất quán; bảng tên/enum 326 = 325+1 khớp; Lua: menu 9/13 mục đúng, Say ≤ 390/512 byte, `doiphai1` nCurFac = nsel+1 khớp hệ; `Include(...npcmonphai\FactionHelper.lua)` 1 gạch chéo sao chép từ hoason.lua — vô hại (`gianhapmonphai` ở factionhead.lua:21); `addskillexp1`/`skill_skillexp_v` không có trong KMagicDesc → bỏ qua im lặng (quy ước Hoa Sơn: exp qua magic_level_exp.txt) | không đổi |

## 12. ĐỢT 2 (02/09 ~06:20) — 3 lệnh chủ: "đổi sang WORD" · "băng sát Hoa Sơn / choáng Tiêu Dao, đối chiếu Linux" · "Linux có sẵn vị trí NPC môn phái"

### 12.1 Giao thức `WeaponType` BYTE → WORD — `vhtd_engine_patch2.py` (marker `[VHTD 02/09d]`)
`KProtocol.h:39` (`PLAYER_SYNC`, `s2c_syncplayer`) và `:435` (`PLAYER_NORMAL_SYNC`, `s2c_syncplayermin`) `BYTE WeaponType` → `WORD`; `KNpc.cpp:6293/6498` bỏ ép `(BYTE)`. `#pragma pack(1)` → mỗi gói dài thêm 1 byte; bảng độ dài `KProtocol.cpp:21-22` dùng `sizeof` nên tự cập nhật; client `KProtocolProcess.cpp:2780/2852` gán từ struct, không đổi mã. **Kiểm phạm vi:** S3Client không tham chiếu 2 struct này (Game.exe không cần build lại); WAuto cây dự án `E:\Src_Auto_Ngoai\WAuto\WAuto` dùng IPC bộ nhớ chung, **không có** `WeaponType` (cây bẫy `J:\CayChay\...` mới có `KProtocolProcess.cpp` đọc gói — không dùng); `MultiServer\ClientClone` có tham chiếu nhưng không có trong cây chạy thật; DB không lưu (`KPlayerDBFuns.cpp:1004` đã comment). `KSimCity.cpp:974` bot Lua `sc_Byte(...)` vẫn kẹp byte (bot cầm thuẫn sẽ sai hình — chỉ bot). ⇒ **CoreServer + CoreClient phải swap CÙNG LÚC** (client cũ đọc gói mới = lệch 1 byte từ trường `MantleType` trở đi).

### 12.2 Băng sát Hoa Sơn / choáng Tiêu Dao — đối chiếu Linux: KHÔNG cần sửa
- **Hoa Sơn (hệ Thủy):** 11 kỹ năng mang `colddamage_v` (1347, 1351, 1355, 1360, 1361, 1362, 1363, 1368, 1372, 1382, 1383) + 1358 `addcoldmagic_v`/`coldenhance_p` — bảng Lua `huashan.lua` JX1 = Linux **11/11 giống hệt**. Engine: `KSkills.cpp:2691` nạp ô damage[10]; `AppendSkillEffect` cộng nội công băng + `coldenhance` vào thời gian đông (chuẩn Linux 0x0807C9C0/0x0807CA27, đợt g); `ReceiveDamage` `KNpc.cpp:4574-4582` đóng băng = `v1 × (100 − freezetimereduce)/100`, kẹp giảm tối đa 77 (Linux `FreezeTimeReduceMax`), không làm mới khi đang đông — **đã đối chiếu Linux ở đợt 01/09 (KIỂM TOÁN THUỘC TÍNH)**. Tooltip dùng khoá `colddamage_v` ("Băng sát") có sẵn.
- **Tiêu Dao (choáng):** 8 kỹ năng `stun_p` — 2120 (5→15 %), 2124/2125/2126 (5→20 %), 2129 (5→20 %), 2138 Lạc Nhạn Bình Sa (5→90 %), 2141 Cao Sơn Lưu Thủy (3→60 %), 2143 (5 %); thụ động 2116 Âm Luật `do_stun_p` (1→20 %); 2130 `frozen_action`; trấn phái 1982 Vũ Hồn `anti_do_stun_p` + `stuntimereduce_p`. Engine `KNpc.cpp:4718-4750` `[CHOANG 01/09]`: xác suất = v0 × (100 − ignorenegativestate)/100 + do_stun_p(kẻ đánh) − anti_do_stun_p(nạn nhân); thời lượng = v1 − v1 × (stuntimereduce − anti_stuntimereduce)/100, > 74 → /4 — **theo Linux 0x0808A8C5-0x0808A941 / 0x0808A95F-0x0808A9D1** (đối chiếu 01/09). Linux không có kỹ năng Tiêu Dao để so số liệu; số liệu lấy client VLTK.
- Kiểm 75 thuộc tính của 62 kỹ năng mới: mọi tên đều có handler (`ProcessFunc`) hoặc xử lý trong `KSkills.cpp` (addskilldamage1-9 theo dải `KSkills.cpp:2512`); chỉ `skill_desc` (mô tả client), `addskillexp1`/`skill_skillexp_v` (không enum — quy ước Hoa Sơn) không có → đúng dự kiến.

### 12.3 NPC môn phái theo vị trí Linux — `vhtd_faction_script.py --chi-npc`
Nguồn: `D:\ServerLinux\server1\script\wumumenpai\npc_wumumenpai.lua:110-167` (map 1042 + 6 thôn 53/20/99/100/101/121), `xiaoyao\npc\npc_xiaoyao.lua:6-23` (map **1046 Linux** + 8 thôn). Linux `xiaoyao.wor` md5 = `.wor` trong `maps_vuhon_tieudao.pak` → cùng bản đồ; JX1 đặt Tiêu Dao ở **1057** (`MapList.ini: 1057=map_publish\xiaoyao`; 1043–1046 JX1 là bản sao Vũ Hồn "Luyện Võ Trường") → toạ độ Linux 1046 dùng nguyên cho 1057. id NPC Linux = id JX1 (npcs.txt VLTK) cho toàn khối — tool in tên đối chiếu từng id (2473 "Hàn Khởi Phụng", 2483 "Nhạc Phi Y Quán Chúng", 2490 "Giáo Quan Luyện Võ" khác chính tả, cùng NPC). **Bỏ:** Linux 2476/2477 Cát Khang/Thư Nhuệ (JX1 là Nam/Nữ Đệ Tử Vũ Hồn), 2501 Đệ Tử Khả Nghi ×5, 2495 Thi Thể (NPC nhiệm vụ `wuhun2020` JX1 không có).
`npc_vhtd.lua` 40 NPC: 6 "Vũ Hồn Đường Hậu Quân" (2467) + 8 "Tiêu Dao Mật Sứ" (2607) ở thôn; map 1042: Nhạc Lôi 2468 (menu phái = `vuhon.lua`), 13 NPC chào (`global\vhtd\npc_chao.lua`), 2 Thuyền Phu 1846 (`thuyenphu_vuhon.lua` port Linux: Ba Lăng Huyện 53 (1794,3157) / Lâm An 176 (1607,2553), `SetFightState(1)`), Rương 625 (`ruongchua.lua`); map 1057: Văn Bán Sơn 2608 (menu phái = `tieudao.lua`), Lý Sấu Ngọc/Hướng Nam Chi/Liễu Tam Biến/Bùi Linh Lung/Cầm Đồng/Kiếm Đồng chào, Thuyền Phu (`thuyenphu_tieudao.lua`: Tây Sơn Thôn 175 (1712,3125) / Lâm An), Rương 625.
`vuhon.lua`/`tieudao.lua`: thêm lựa chọn **"Đến Vũ Hồn Đường/go_vuhon"** → `NewWorld(1042,1625,3130)` và **"Đến Tiêu Dao phái/go_tieudao"** → `NewWorld(1057,1641,3288)` (toạ độ Linux `hauquan_vuhon.lua`/`detu_xiaoyao.lua`) ở menu nhập môn (3 mục) và 3 menu thành viên (5/4/5 mục). Chưa port: nhiệm vụ nhập môn Linux (`SetTask(4406/4464)`, `nt_setTask(137,71)`), danh hiệu qua `AddNote`.
Tool nay **idempotent thật** (chạy lại = toàn `[=]`; phần chèn-sau kiểm "phần thêm đã có"); `--chi-npc` chỉ sinh lại NPC.

## 10. CHECKLIST SWAP (chủ chạy `ChayGameServer.bat` / `ChoiGame.bat`) — điền md5 ở mục 11
1. `bin\server\CoreServer.dll.moi` — swap cùng 2.
2. `bin\client\CoreClient.dll.moi` + 3. `bin\client\Game.exe.moi` — swap cùng lúc (enum thuộc tính 310–325 phải khớp 2 bên; Game.exe có bảng ô kỹ năng 11/12).
4. Dữ liệu đã ghi thẳng, đọc khi khởi động: server `skills.txt`, `missles.txt`, `script\**`, `settings\npcs.txt`, `magicscript.txt`, `MagicDesc.ini`, `magic_level_exp.txt`, `package.ini` (`8=maps_vuhon_tieudao.pak`), `Pak\maps_hoason2013.pak` (mới), `Pak\maps_vuhon_tieudao.pak`, `Maps\WorldSet_GameServer.ini`, `MapList.ini`, `faction\FactionInfo.ini`; client: các tệp cùng tên + `package.ini` (`36=`, `37=`), `data\sprvuhontieudao.pak`, `data\maps_vuhon_tieudao.pak`, `ui\Ui3\UiSkill*.ini`, `Spr\Ui3\UiSkills\khung_wh.spr`/`khung_xy.spr`, `settings\gamesetting.ini`, `NpcRes\*`.
5. Nghiệm thu: (a) log server không ScriptError khi boot; nhân vật hệ Hoả/Thổ chưa phái gặp "Vũ Hồn Hậu Quân"/"Tiêu Dao Mật Sứ" ở Ba Lăng Huyện (53) → gia nhập → bảng kỹ năng F? hiện khung mới; (b) học kỹ năng qua NPC `hocvocong` "Học võ công môn phái Vũ Hồn/Tiêu Dao"; (c) mô tả kỹ năng không còn `<color>` thô; (d) **kỹ năng Vũ Hồn/Cầm Pháp chỉ dùng được sau khi có vũ khí thuẫn/cầm (mục 8)**; kỹ năng Tiêu Dao Kiếm Pháp (EqtLimit 0) dùng kiếm được ngay; (e) map 987 Hoa Sơn vào được (mục 7).

## 11. Bộ .moi (Core build 06:17 sau patch2 WORD; Game.exe 05:09 — không tham chiếu PLAYER_SYNC nên không build lại)

| Tệp | Byte | md5 (8) | Giờ |
|---|---|---|---|
| `server\CoreServer.dll.moi` | 18.251.776 | `0a0cc352` | 06:19 |
| `client\CoreClient.dll.moi` | 2.443.776 | `f52ddc8e` | 06:19 |
| `client\Game.exe.moi` | 1.374.208 | `359536c5` | 05:20 |

HEAD `6a63af68`+ (superset đợt g `d715746b` + bot đợt d). **Swap 3 tệp cùng lúc — bắt buộc vì gói PLAYER_SYNC/PLAYER_NORMAL_SYNC đổi kích cỡ (+1 byte).** Goddess/WAuto không đổi.
