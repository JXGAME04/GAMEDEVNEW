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

## 13. ĐỢT 3 (02/09 ~07:10) — 4 lỗi chủ test sau swap: NPC sai hình · chưa nhận vũ khí ở NPC test · bảng kỹ năng Tiêu Dao trống · kỹ năng Vũ Hồn thiếu thuộc tính

| # | Lỗi chủ báo | Gốc (đã đo) | Vá |
|---|---|---|---|
| 1 | **Kỹ năng Vũ Hồn không hiện thuộc tính** | Client `ScriptError.log`: `ScriptError 4:[1] (\script\skill\wuhuntang.lua)` ×6 = `KLuaScript::ExecuteCode` lỗi khi chạy **thân chunk**. Client nạp Lua kỹ năng bằng `KLuaScript Script; Script.Init()` (`KSkills.cpp:2210-2222`, nhánh `#ifndef _SERVER`) → ctor mặc định `lua_open(100)` (`KLuaScript.cpp:6`) và `Init()` không đổi stack ⇒ **stack Lua 4 chỉ 100 ô**. `wuhuntang.lua` (client VLTK) khai `SKILLS={ 31 bảng lồng 5 tầng }` **một constructor** → "stack Overflow" (tái hiện bằng `lua4.exe -s100`, host build từ `Sources\Library\LuaLib\src`); server dùng `g_ScriptSet` (`Init()` → 1024 ô) nên không lỗi. `huashan.lua`/`xiaoyao.lua` vừa đủ 100. | `vhtd_lua_split.py`: tách `SKILLS={}` + `SKILLS.<bảng>={...}` (server+client, bản lưu `.truoc_vhtd_split_0209`); kiểm lua4: chạy chính + `dofile` ở stack 100 đều OK, giá trị `GetSkillLevelData` mọi (bảng, thuộc tính) cấp 1/10/20 giống 100 % (xiaoyao so trực tiếp; wuhuntang gốc không chạy được ở dofile). |
| 2 | **Bảng kỹ năng Tiêu Dao trống** | `SkillDef.h:4 MAX_SKILL 2000` — id Tiêu Dao **2114–2143 ≥ 2000** → `KSkillManager` (`m_SkillInfo[MAX_SKILL]`, `m_pOrdinSkill[MAX_SKILL][64]`) bỏ qua ⇒ `AddMagic` thất bại, client không có kỹ năng để vẽ. Thêm: `MAX_FIGHTSKILL_SORTLIST 50` (`KSkillList.cpp:636`) + `FIGHT_SKILL_COUNT/UI_MAX 50` — nhân vật test học nhiều phái > 50 chiêu → chiêu học sau không vào `GDI_FIGHT_SKILLS`. | `vhtd_engine_patch3.py`: `MAX_SKILL 2300`; `MAX_FIGHTSKILL_SORTLIST 100`; `UiSkillsNew.h`/`UiSkills.h` `FIGHT_SKILL_COUNT`/`FIGHT_SKILL_UI_MAX` 100 → build lại **3 binary** (Game.exe đổi). |
| 3 | **Chưa nhận được vũ khí 2 phái ở NPC test Ba Lăng Huyện** | `header\testgame.lua` `selvk` (menu "Nhận trang bị Xanh → Vũ khí hệ X"): vòng cận chiến `for i=0,5` (P0–5), tầm xa `for i=0,2` (P0–2). | `vhtd_testgame_vukhi.py`: cận chiến `0,8` (+P6 Triền Thủ, P7 Đao Thuẫn, P8 Thuẫn Đao), tầm xa `0,3` (+Mộc Cầm), cùng hệ chọn, cấp 10, cùng dòng thuộc tính. Bản lưu `.truoc_vhtd_0209`. |
| 4 | **NPC Vũ Hồn/Tiêu Dao sai hình** | Tác tử kiểm bằng uid (`vhtd_npc_spr.py`, `vhtd_npc_spr_danhsach.tsv`): 227 kiểu NpcResType của 460 NPC → 865 tệp: JX1 có 829, thêm VLTK 4, thiếu cả hai 32 (NPC không spawn). **2467 passerby410 / 2607 boss202: bảng kind/normal_res/spr_info đủ, spr có trong updatejx15/16 và giống hệt VLTK** → dữ liệu hiện tại không còn lỗi. Đồng thời sửa 7 hàng `npc_normal_res_file.txt` hỏng byte GBK (passerby341_*/342/344), boss166/167 `ResFilePath`, thêm kind passerby465–468 (pak `sprvuhontieudao2.pak` 4 tệp, md5 67f57bdb, `package.ini 38=`). Khả năng: chủ test trên tiến trình client cũ / chưa nạp lại bảng. | Không sửa thêm; **cần chủ tả rõ** sau khi khởi động lại client: NPC nào, hiện thành hình gì. |
| + | ScriptError server `tieudao.lua`/`npc_chao.lua cFuncName:(OnRevive)` | Engine gọi `OnRevive` khi spawn (`KNpc.cpp:9352`, `ScriptFuns.cpp:7346`); thiếu hàm → lỗi 4 và `KNpc::ExecuteScript` đặt `m_ActionScriptID = 0` | thêm `function OnRevive() end` vào 5 script mới (tool `--chi-npc`). |

Kiểm đợt 3: build sạch 3 binary; `cmp` server = client cho `wuhuntang.lua`/`xiaoyao.lua`/`testgame.lua`... ; lua4 (engine Lua 4 build riêng, `scratchpad\lua4\lua4.exe`, cách build ghi trong memory) chạy 4 tệp skill ở stack 100 OK; `t71` = 0.

## 10. CHECKLIST SWAP (chủ chạy `ChayGameServer.bat` / `ChoiGame.bat`) — điền md5 ở mục 11
1. `bin\server\CoreServer.dll.moi` — swap cùng 2.
2. `bin\client\CoreClient.dll.moi` + 3. `bin\client\Game.exe.moi` — swap cùng lúc (enum thuộc tính 310–325 phải khớp 2 bên; Game.exe có bảng ô kỹ năng 11/12).
4. Dữ liệu đã ghi thẳng, đọc khi khởi động: server `skills.txt`, `missles.txt`, `script\**`, `settings\npcs.txt`, `magicscript.txt`, `MagicDesc.ini`, `magic_level_exp.txt`, `package.ini` (`8=maps_vuhon_tieudao.pak`), `Pak\maps_hoason2013.pak` (mới), `Pak\maps_vuhon_tieudao.pak`, `Maps\WorldSet_GameServer.ini`, `MapList.ini`, `faction\FactionInfo.ini`; client: các tệp cùng tên + `package.ini` (`36=`, `37=`), `data\sprvuhontieudao.pak`, `data\maps_vuhon_tieudao.pak`, `ui\Ui3\UiSkill*.ini`, `Spr\Ui3\UiSkills\khung_wh.spr`/`khung_xy.spr`, `settings\gamesetting.ini`, `NpcRes\*`.
5. Nghiệm thu: (a) log server không ScriptError khi boot; nhân vật hệ Hoả/Thổ chưa phái gặp "Vũ Hồn Hậu Quân"/"Tiêu Dao Mật Sứ" ở Ba Lăng Huyện (53) → gia nhập → bảng kỹ năng F? hiện khung mới; (b) học kỹ năng qua NPC `hocvocong` "Học võ công môn phái Vũ Hồn/Tiêu Dao"; (c) mô tả kỹ năng không còn `<color>` thô; (d) **kỹ năng Vũ Hồn/Cầm Pháp chỉ dùng được sau khi có vũ khí thuẫn/cầm (mục 8)**; kỹ năng Tiêu Dao Kiếm Pháp (EqtLimit 0) dùng kiếm được ngay; (e) map 987 Hoa Sơn vào được (mục 7).

## 11. Bộ .moi (build 06:58–06:59 sau patch3; đủ 3 tệp, Game.exe ĐỔI)

| Tệp | Byte | md5 (8) | Giờ |
|---|---|---|---|
| `server\CoreServer.dll.moi` | 18.252.288 | `6084cd4e` | 07:02 |
| `client\CoreClient.dll.moi (đã swap, đang chạy)` | 2.444.288 | `c25890a5` | 07:02 |
| `client\Game.exe.moi (đã swap, đang chạy)` | 1.374.720 | `1ba35427` | 07:02 |

**Swap 3 tệp cùng lúc** (WORD giao thức + MAX_SKILL 2300 hai bên + Game.exe bảng 100 ô). Sau swap: khởi động lại client để nạp lại `skills.txt`/Lua/`npcs.txt`/bảng NpcRes/pak 36–38.

## 14. ĐỢT 4 (02/09 ~09:00–09:50) — 14 mục chủ giao sáng 02/09 (phiên tài khoản trước dừng sau mục "skill 150 test" = commit `33a0530e`)

Tool: `ReverseTools\phai3\vhtd_thicong\vhtd_engine_patch4.py` (16 tệp nguồn, 33 hunk, marker `[VHTD 02/09g]`, có `--kiem`) + `vhtd_data_patch4.py` (4 tệp dữ liệu ở E:\, bản lưu `.truoc_vhtd_0209g`). Bằng chứng đo trên `bin\server\jx_auto_server.log(.1)` (3 phiên máy chủ pid 27612/30268/42328; nhân vật test 91477 học cả Vũ Hồn + Tiêu Dao, 91468 Hoa Sơn, bù nhìn 90645).

### 14.1 Từng mục chủ nêu → gốc đo được → xử lý

| # | Chủ nêu | Gốc (đo / đọc mã) | Xử lý |
|---|---|---|---|
| 1 | Kỹ năng Vũ Hồn "trừ HP liên tục"; cả 2 phái "tự mất máu theo thời gian" | **Không có sát thương tự gây**: 0 dòng `E2-CALC-FINAL` có attacker = nạn nhân với sát thương > 0. 1547/1561 sát thương lên 91477 là của NPC **90645** (bù nhìn test, sinh lực 288.000.000) phóng **164 Bạc Cập Nhi Phục / 165 Vô Ngã Vô Kiếm** (độc, type 3, 44/nhịp) → sàn độc = 1 nên máu về đúng **1** lặp lại (95→1, 327→1, 48→1…). Chi phí mọi chiêu mới = nội lực (`costtype=0` = `attrib_mana_v`; log 1967 nội lực 1484→1454). | Không sửa engine. **Kiểm lại ở chỗ không có bù nhìn** (hoặc bù nhìn không mang 164/165). |
| 2 | Một số skill Vũ Hồn và cầm Tiêu Dao "hay miss"; hiệu ứng bay nửa chừng bị mất | `KMissle::CheckCollision` nhánh **CollidRange > 1** (đạn VLTK: CollidRange = DmgRange 2..4, chép nguyên client): `FindNpc` thấy NPC cách tới CollidRange ô, nhưng `ProcessCollision()` chỉ quét `DmgRange/2` ô quanh **đạn** → không ai bị đánh mà `DoCollision` (ColVanish 1) tan luôn. Đo: **Trấn Biên Thùy 1967 phóng 87 trúng 2** (86 dòng `S4-MSL-END early lasthit=0`); Quảng Lăng Tán 2140: 96 tan sớm/15 hết tuổi; Cao Sơn Lưu Thủy 2141: 33 trúng/48 phóng. Client chạy cùng `CheckCollision` → đạn tan xa mục tiêu = "bay nửa chừng mất". Đạn cũ JX1 (< 500) bay đều CollidRange 1 nên không lộ. Linux có 26 đạn Line CollidRange > 1 nhưng hàm CheckCollision Linux **không định vị được** (không có `cmp [+0x40],1`). | `KMissle.cpp`: đạn **id ≥ 500** và NPC nằm ngoài ô quanh đạn → `ProcessCollision(launcher, ô NPC, IsRangeDmg ? DmgRange : 1, quan hệ, NPC)` = sát thương **tại NPC** (đơn: chỉ nó; diện rộng: DmgRange quanh nó) rồi mới `DoCollision`. Đạn < 500 giữ nguyên. |
| 3 | Tiêu Dao cầm "tăng điểm kỹ năng không thấy dame" | `lightingdamage_p` (320) nhân `m_PhysicsMagic` (KPlayer.cpp:9497 `nMagicBase = 0` — JX1 tắt nội công cơ bản) + `m_CurrentLightMagic` (Mộc Cầm P3 chỉ có weapondamage 28/29 + durability 31, **không có dòng nội công**) → sát thương Lôi luôn **0** dù trúng (HIT 2136 = 15, 2140 = 43). Giá trị VLTK: 2136 1→16 %, 2141 1→39 %, 2138 1→10 %. Mổ `game_y.exe` (không nén; bảng tên thuộc tính tại 0x10fe2e0, lightingdamage_p = 48) **không** tìm được điểm so sánh (client dùng bảng nhảy); Linux không có phái này. | `KNpc.cpp` AppendSkillEffect: gốc = **nội lực tối đa** (+ nội công cơ bản + nội công Lôi vũ khí) × (100 + `addlightingmagic_p`)/100 × p/100 — **GIẢ ĐỊNH** theo thiết kế Tiêu Dao VLTK (hộ thuẫn 2139/2134 = "x lần nội lực tối đa"). Ví dụ nội lực 4.000: Tùy Ý Khúc cấp 20 = 640/đòn, Cao Sơn Lưu Thủy cấp 20 = 1.000 (×1,43 với Cầm Pháp cấp 20). **Chủ quyết** nếu muốn hệ số khác. |
| 4 | Hoa Sơn: 3 kiếm "phải dí đối phương"; đứng 3–5 phút mới xuất hiện; buff chưa hiển thị hiệu ứng | (a) Server `CastAutoSkillAt` đúng, đạn 418 MoveKind 5 (Follow) dí được — nhưng **client** `s2cDirectlyCastSkill` (KProtocolProcess.cpp) đổi (−1, ID) thành **toạ độ** trước khi `Cast` → `nTargetId = 0` → đạn client bay thẳng. (b) Tỷ lệ 1364: **VLTK = Linux** = `15*18*256 + 1..3` → **1 %/đòn bị đánh (cấp 1) → 3 % (cấp 20), hồi 15 s**; đo 91468: 4 lần/≈40 phút với ~10 đòn/phút = đúng cỡ dữ liệu. (c) Mọi `PreCastSpr` 3 phái đều có và pak đã nạp (Hoa Sơn `updatejx14.pak`, Vũ Hồn `updatejx15.pak`, Tiêu Dao dùng `\spr\skill\昆仑\kl_16_魔法施法.spr` trong `skills.pak`); VLTK cũng `StateSpecialId 0` cho buff Hoa Sơn → không có hào quang bền, chỉ hiệu ứng lúc xuất chiêu. | (a) `KProtocolProcess.cpp`: MapX == −1 → `Cast(nIdx, -1, chỉ số NPC)` như server (áp cho mọi `s2c_castskilldirectly`). (b) Không sửa (dữ liệu chuẩn); muốn dày hơn: byte thấp v2 của `autoreplyskill` trong `huashan.lua` (chủ quyết). (c) Không sửa — **cần chủ tả rõ** thiếu gì (icon? hào quang? hoạt ảnh?). |
| 5 | Tiêu Dao chưa có skill cơ bản (mang cầm → đàn bay ra) | `ClientWeaponSkill` (1,3)→2135 đúng, nhưng `SetLeftSkill` cần `GetCurrentLevel(2135) > 0` = nhân vật phải **học** 2135 (VLTK cho 1/2/2135 `MaxLevel 0`; `KSkillList::Add` không kiểm MaxLevel). | `factionhead.lua` SKILLNORMAL[13] + `{2135,1}`; `skills_table.lua` add_xy(10) + `AddMagic(2135, 1)` (NPC test `skilltieudao` → add_xy(150) cũng cấp). |
| 6 | Tạp Đạp Lưu Tinh "skill lướt kèm hiệu ứng" | Form 13 MoveWithBlur **đã chạy** qua `KNpc::CastMeleeSkill → DoBlurMove` (như Mê Ảnh Tung 710): lướt tối đa `skill_param1_v` = 280, cần `cost_sp` 10 tầng Âm Luật (cấp 1; cấp 30: 4). Thiếu chiêu con **2119 "Khởi"** (đạn 638 đứng yên = hiệu ứng lúc bắt đầu). Hãm Sơn Kích 1977 (form 11 RunAndAttack) cũng đã chạy (5 phóng, 7 trúng 1978). | `KNpc.cpp` DoBlurMove bước 0: `Cast(chiêu con, m_DesX, m_DesY)` cả client lẫn server (đạn 638 CollidRange 0, không thuộc tính → vô hại). |
| 7 | Nộ / Âm Luật chưa hiện trên mini icon góc trái | Không có đường đồng bộ `m_HSSp` xuống client; 2117 Âm Luật là thuộc tính tức thời (không có node trạng thái). | Gói mới **`s2c_syncvhtd`** (`S2C_SYNC_VHTD` 12 byte: kind 0 = {key 1976/2116, tầng, trần}; kind 1 = khiên) chỉ gửi cho chính người chơi khi tầng đổi / đặt trần; client `m_HSSp` **chỉ nhận** (handler SpecialPointBase/Add bỏ qua ở client). `GDI_HS_SP` + `UiPlayerBar`: sau các icon trạng thái thêm ô icon (StatePos.ini [1976] "Nộ" / [2116] "Âm Luật"), chữ số = tầng, tooltip tầng/trần. |
| 8 | Lạc Nhạn Bình Sa: "ống máu thứ 2" | Khiên tĩnh `staticmagicshield_p` (2139 Tiêu Dao Vũ, 2134 Phất Y_Thuẫn) chỉ tồn tại server (`m_CurrentStaticMagicShieldP`), client không biết. | `m_nHSShieldMax` (bể lúc áp) + `HS_SyncShield()` khi áp / hấp thu / vỡ / gỡ → client; `KUiPlayerRuntimeInfo.nShield/nShieldFull`; lớp UI **`Player_Shield`** (UiShell.cpp) + `UiHeaderControlBar.ini` `Button6=Shield`, `[Shield] ClassType=Player_Shield` (ảnh thanh nội lực, cao 4 px đè nửa dưới thanh sinh lực, **ẩn khi không có khiên**). |
| 9 | Skill 120 Vũ Hồn (1984 Trung Vu Lưu Phong) đúng chưa | `attackrating_p` 10→200 %, `skill_skillexp_v`; 1982 `cast_when_buff_removed {1991, −1, 1984}` → 1991 `resume_life_p` 30→71 % theo cấp 1984 (phản biện A1 đợt 1: chưa học → không hồi). **Giống VLTK từng byte.** | Không sửa. |
| 10 | Thêm skill 150 các phái để test | Đã làm ở commit `33a0530e` (`skillvuhon/skilltieudao/skillhoason` → add_*(150) + Hoa Sơn 90/120/150 trực tiếp). | Không đổi. |
| 11 | Một số kỹ năng "có time thi triển mà hiện tại chưa có" | VLTK: không có cột / thuộc tính / chuỗi nào về thanh thi triển (không có 读条/吟唱; `WaitTime`/`PreCastSpr`/`CharAnimId` giống JX1 và đã có). | **Cần chủ nêu tên kỹ năng cụ thể** để so hoạt ảnh. |
| 12 | Kiểm hiệu ứng / thuộc tính / đạn / phạm vi mỗi phái | Đạn 27 dòng = VLTK 100 % (header 57 cột trùng); 62 kỹ năng đã đối chiếu cột chức năng đợt 2. Lệch còn lại là ngữ nghĩa engine (mục 2, 3). | Như mục 2, 3. |

### 14.2 Đã kiểm
- Build sạch 3 binary (0 lỗi): `Core.vcxproj` Server Release x64, Client Release Win32, `S3Client.vcxproj` Release Win32 (MSBuild gọi thẳng .vcxproj, `SolutionDir=D:\GAMEDEVNEW\Sources\`); các tệp sửa đều được biên dịch lại.
- `ReverseTools\viemde\kiem_luoi_giaothuc.py`: nửa client khớp 150, ô cuối = 218 (`s2c_syncvhtd`); 4 + 3 lệch là chú thích sai cũ (đã biết từ 26/08).
- Mã hoá: byte cao mọi tệp nguồn không đổi (tool assert), 0 `EF BF BD`; `t71` = 0 gọi nil thân chunk; lua4 nạp `skills_table.lua` (chỉ nil `Include` do chạy ngoài engine).
- Bẫy tool: `KProtocolProcess.cpp`, `CoreShell.h` là **LF**, `CoreShell.cpp` trộn (35 CRLF / 21.393 LF) → tool vá dò NL theo đa số.

### 14.3 Chưa làm / chờ chủ
1. Hệ số gốc Lôi % (mục 3) — đang giả định nội lực tối đa.
2. Tỷ lệ 3 kiếm Hoa Sơn (mục 4b) — dữ liệu 1–3 % / 15 s.
3. "Time thi triển" (mục 11) và "buff Hoa Sơn chưa hiển thị" (mục 4c) — cần mô tả cụ thể.
4. Đạn Hoa Sơn 419–424 (Stand, CollidRange 5 = DmgRange 5) cùng ngữ nghĩa mục 2 nhưng id < 500 nên **chưa** đổi (chủ chưa báo miss Hoa Sơn) — muốn áp thì hạ ngưỡng `m_nMissleId >= 500` trong `KMissle.cpp`.
5. Nhân vật Tiêu Dao đã tạo trước đợt này: bấm NPC test "Học skill 90 → Tiêu Dao" (add_xy(150)) để nhận 2135.

### 14.4 CHECKLIST SWAP đợt 4 (chủ chạy `ChayGameServer.bat` / `ChoiGame.bat`; giao thức đổi → 3 tệp CÙNG LÚC)
1. `bin\server\CoreServer.dll.moi` — 18.253.312 byte, md5 `bfa9c6bc` (09:49).
2. `bin\client\CoreClient.dll.moi` — 2.445.824 byte, md5 `e364ca09` (09:49).
3. `bin\client\Game.exe.moi` — 1.376.768 byte, md5 `5db988fc` (09:49) — có `Player_Shield` + ô icon tầng.
4. Dữ liệu đã ghi thẳng (đọc khi khởi động): server `script\header\factionhead.lua`, `script\global\skills_table.lua`; client `ui\StatePos.ini` (+21 mục, BuffCount 152), `ui\Ui3\UiHeaderControlBar.ini` ([Shield]).
5. Nghiệm thu: (a) Tiêu Dao cầm: Tùy Ý Khúc / Quảng Lăng Tán / Cao Sơn Lưu Thủy trúng và có số sát thương Lôi tăng theo cấp và theo nội lực tối đa; (b) Vũ Hồn: Trấn Biên Thùy / Huy Sư Diệt Lỗ trúng (log `S4-MSL-END ... lasthit>0`); (c) góc trái: icon Nộ / Âm Luật kèm số tầng (Âm Luật +1 mỗi giây tới 10; Nộ +2 mỗi 3 s tới 5); (d) Lạc Nhạn Bình Sa / Phất Y: thanh xanh mỏng đè dưới thanh máu, giảm khi bị đánh, ẩn khi hết; (e) Tạp Đạp Lưu Tinh: đủ 10 tầng Âm Luật → lướt 280 + hiệu ứng "Khởi" tại chỗ; (f) Tiêu Dao cầm đánh thường bắn đạn 643; (g) Hoa Sơn 1364: 3 kiếm trên client bay bám mục tiêu; (h) đứng xa bù nhìn: máu không tụt.



## 15. ĐỢT 5 + 6 (02/09 ~10:30–13:00) — 10 mục chủ test sau swap đợt 4 (2 ảnh: icon buff to/sai, "thuộc tính lạ") + 6 ý bổ sung ~11:40 (2 ảnh tooltip VLTK)

Tool: `ReverseTools\phai3\vhtd_thicong\vhtd_engine_patch5.py` (marker `[VHTD 02/09h]`, 4 hunk) + `vhtd_data_patch5.py` (dữ liệu + sinh icon) + `vhtd_data_patch6.py` (đồng bộ VLTK) + `oct2raw_tcvn3.py` / `bo_bom_tcvn3.py` (chuỗi thông báo TCVN3 thô). So dữ liệu: `phai3\tools\vltk_diff_skills.py`, `vltk_dump_skills_lua.py`, `vltk_find_text.py`. Build 11:54 (CoreServer/CoreClient; Game.exe KHÔNG đổi).

### 15.1 Gốc từng mục (đợt 5)

| # | Chủ báo | Gốc đo được | Vá |
|---|---|---|---|
| 10 | "NPC môn phái Tiêu Dao hiển thị sai hình, tự di chuyển đánh người như quái trong thành" | `settings\npcs.txt` dòng 2186 (`nangongjue`, thêm đợt 1) có **103 cột** trong bảng 87 cột. `KTabFile` đọc đúng 87 ô mỗi dòng → 16 ô thừa tràn sang dòng sau → **mọi template id > 2184 lệch +1** (`InitNpcBaseData` row = id+2): sứ giả Tiêu Dao 2607 nhận dòng quái "Trương Tông Chính" (AI quái, hình quái). Cũng là gốc "máu tụt về 1" đợt 4 (bù nhìn độc 90645/90646 đứng đúng ô sứ giả) và "NPC sai hình" đợt 3. | Cắt dòng 2186 về 87 cột (server + client). Luật: thêm dòng bảng tab phải đếm cột bằng tool — KTabFile KHÔNG báo lỗi. |
| 1 | "icon quá to và sai icon không giống client VLTK" | StatePos đợt 4 trỏ icon **kỹ năng** 36×36 (`\spr\Ui\技能图标`) vào ô buff `[BuffImage] 34×24`. VLTK dùng icon **trạng thái** 24×24 ở `\spr\Ui\状态图标\<tên GBK>.spr`, có sẵn cho 1966/1968/1971/1973/1982/1987; 15 trạng thái còn lại VLTK không có icon riêng. | 7 mục trỏ icon trạng thái VLTK; 15 mục (Nộ 1976/1989/1991, Âm Luật 2116, 2128/2130/2131/2133/2134/2139, Hoa Sơn 1366/1376/1380/1381, 1988) sinh 24×24 từ icon kỹ năng → `bin\client\data\sprvuhontieudao3.pak` (11 tệp, `package.ini 39=`), đường dẫn `\spr\Ui\状态图标\vhtd\<tên>.spr`. |
| 1b | icon thuốc "00s" rác | `GDI_NPC_STATE_SKILL` (CoreShell.cpp, 2 vòng) liệt kê cả node `m_LeftTime == 0` (2117 Âm Luật tức thời). | Lọc `pNode->m_LeftTime != 0`. |
| 3 | "skill trừ Âm Luật/Nộ khi không đủ phải không dùng được và có thông báo" | Chỉ server chặn (`Cost` + `HS_SpGet`) và chặn IM LẶNG; client `KNpc::DoSkill` không có gate → vẫn diễn hoạt / tự lướt rồi bị kéo về. | Client `#ifndef _SERVER`: thiếu tầng → `goto Exit` + thông báo "Không đủ N tầng Nộ / Âm Luật" (số tầng client có qua `s2c_syncvhtd` đợt 4). |
| 2 | "Tạp Đạp Lưu Tinh có skill con kèm theo phải có sát thương" | Chiêu con 2119 "Khởi" VLTK = đạn 638 đứng yên, KHÔNG có bảng sát thương; đợt 4 phóng ở bước 0. | `DoBlurMove` phóng ở bước 2 (điểm đến) `Cast(m_Index,-1,m_Index)`; dữ liệu 2119: MisslesForm 7, TargetEnemy 1, sát thương = bảng `wugoushuangxue` (2114) — **GIẢ ĐỊNH**; đạn 638 CollidRange/DmgRange 2, DmgInterval 20. |
| 4a | "kỹ năng cầm không hiện lực tay ở bảng thông tin" | Tooltip chỉ in dòng `%` của `lightingdamage_p`. | `KSkill::GetDescAboutLevel` thêm "Sát thương Lôi (theo nội lực tối đa): X" đúng công thức `AppendSkillEffect`. |
| 4b | "đứng gần, đạn cầm bay tới quái gần tới thì mất, không dính sát thương" | Đợt 4 đã vá nhánh `CollidRange > 1`; sau swap: 1967 trúng 6/10, 2136 giết bù nhìn 200 máu. Còn miss khi đứng sát = `m_ulDamageInterval` (`[COLL-DMG-COOLDOWN]`), không phải mất đạn. | Không vá thêm; test lại sau swap, nếu còn gửi log `S4-MSL-END` + `COLL-DMG-COOLDOWN` đúng lượt. |
| 5 | "Tá lực ở Hiệp Cốt Nhu Tình là gì?" | `unravel_effect` (319). VLTK `game_y.exe` chỉ có tên + mô tả "Tá lực: N điểm"; không có site nào đọc giá trị; Linux không có phái. | Engine chỉ lưu + mô tả. Chờ chủ định nghĩa cơ chế. |
| 6+9 | "thuộc tính mới đã làm đủ chưa? nhiều thuộc tính lạ" | Bảng 15.2. "Thuộc tính lạ" = dòng `MagicDesc.ini` tôi tự đặt cho 10 thuộc tính mà VLTK **để trống** (VLTK mô tả bằng hàm `skill_desc=function(level)` trong Lua từng kỹ năng; engine JX1 đã in `m_szMagicSkillDesc`). | Đợt 6: để trống 10 dòng như VLTK (cost_sp, special_point_add, reset_bufftime, hidebodyunlock, invincibility, forbit_attack, autocastskill, lock_life, cast_when_buff_removed, resume_life_p); sửa "Giới hạn tồn"→"tầng". |
| 7 | "chưa thấy kỹ năng 150 các phái mới trong bảng kỹ năng" | Ô bảng đã có (`[Skill_11_4]`/`[Skill_11_11]`/`[Skill_12_4]`/`[Skill_12_10]`), nhân vật test học TRƯỚC commit `33a0530e` (`add_*(150)`). | Bấm lại NPC test "Vũ Hồn 90 / Tiêu Dao 90 / Hoa Sơn 90". |
| 8 | "skill 120 đã hoạt động đúng chưa?" | Vũ Hồn 1984 bị động (cộng khi học); Tiêu Dao 2127 → 2128 qua `autoattackskill`; Hoa Sơn 1370. Log chưa có lần phóng nào (nhân vật chưa học 120). | Học 120 ở NPC test, đánh 5–10 đòn, gửi log. |
| — | `wuhuntang.lua` chuỗi `Di Th<color>` | Thiếu dấu cách trước `<color>` → TEncodeText nuốt `<`. | Thêm dấu cách (server + client). |

### 15.2 15 thuộc tính mới (310–324) — trạng thái

| id | tên | xử lý ở | đã kiểm |
|---|---|---|---|
| 310 | autocastskill | KNpc.cpp (HS_AutoSkill, Hoa Sơn đợt f) | 1364 → 1363 tự phóng, client bám mục tiêu |
| 311 | special_point_base | KNpcAttribModify.cpp + s2c_syncvhtd | Nộ/Âm Luật đếm tầng, icon góc trái |
| 312 | special_point_add | KNpcAttribModify.cpp | +tầng, giới hạn trần |
| 313 | cost_sp | KSkills.cpp + KNpc::DoSkill (server đợt c, **client đợt 5**) | trừ tầng khi phóng; thiếu → chặn + thông báo |
| 314 | lock_life | KNpcAttribModify.cpp | chưa có log (buff Vũ Hồn 1987) |
| 315 | cast_when_buff_removed | KNpc.cpp | chưa có log |
| 316 | reset_bufftime | KNpc.cpp + KSkills.cpp | chưa có log |
| 317 | resume_life_p | KNpcAttribModify.cpp | chưa có log |
| 318 | lifereplenish_dec_p | KNpcAttribModify.cpp | chưa có log |
| 319 | unravel_effect | chỉ lưu + mô tả | **chưa có cơ chế** (VLTK cũng không) |
| 320 | lightingdamage_p | KNpc.cpp (gốc = nội lực tối đa, đợt 4) + tooltip đợt 5 | 2136 giết bù nhìn 200 máu |
| 321 | addlightingmagic_p | KNpcAttribModify.cpp | vào công thức 320 |
| 322 | hidebodyunlock | KNpcAttribModify.cpp | chưa có log |
| 323 | invincibility | KNpcAttribModify.cpp | chưa có log |
| 324 | forbit_attack | KNpcAttribModify.cpp | chưa có log |

Các thuộc tính VLTK khác mà 3 phái dùng (`_yan_`, enhancehit(effect)_rate, skill_misslenum_v, anti_do_hurt_p, anti_enhancehit_rate, skill_mintimepercast_v, skill_desc…) đều có sẵn enum + handler trong JX1 (`KNpcAttribModify.cpp` `ProcessFunc[magic_lifemax_yan_p] = LifeMaxP`…).

### 15.3 ĐỢT 6 — "kiểm tra thuộc tính các phái mới xem đúng như bản VLTK không? tôi thấy sai hơi nhiều" / "phạm vi kỹ năng nhỏ hơn bản gốc"

- **Nguồn so:** client `C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky`. `KPakList::FindElemFile` duyệt `package.ini` theo thứ tự và lấy bản ĐẦU TIÊN → bản mới nhất của skills.txt / missles.txt / huashan-wuhuntang-xiaoyao.lua / magicdesc.ini nằm ở `slistcache.pak` (19/08 15:49) = `ReverseTools\phai3\vltk_raw\*` (md5 trùng; các bản trong update*.pak cũ hơn).
- **Kết quả đo:** `missles.txt` trùng 100 %. `skills.txt` lệch 14/79 dòng (13 dòng lấy VLTK, giữ 2119). **Lua Hoa Sơn lệch 52 mục** vì đợt 1 lấy từ Linux `huashan2013` (khác đời với client VLTK): `physicsenhance_p` (vd Cửu Kiếm Hợp Nhất JX1 3→322 %, VLTK 30→1500 %), `jinyan_hengkong.skill_attackradius` 384/416 → **512**, `pishi_poyu` (1382) AttackRadius 380 → **520** + `skill_misslenum_v` 1→4 đạn + `fatallystrike_p` = chính là "**vòng tròn nhỏ hơn bản gốc**"; `jiujian_start.autoattackskill` 15 s / 1–5 % → **5 s / 1–40 %** (VLTK 10 % ở cấp 10, 20 % cấp 20, 40 % cấp 34+); thiếu `lifemax_yan_p` / `anti_sorbdamage_yan_p` / `enhancehit(effect)_rate` / `anti_block_rate` / `fire2medamage_p` / `me2firedamage_p` / `fatallystrikeres_p` / `anti_hitrecover` / `attackspeed_yan_v`; `zixia_xiaoguo.lifereplenish_v` 300→6100 vs VLTK 500→10000; `huanyan_yunyan` buff 1080/3240 khung vs VLTK 5400. Vũ Hồn/Tiêu Dao Lua đã đúng VLTK (chỉ khác tên `_yan_` do đợt 1 đổi sang tên thường — engine JX1 có handler `_yan_`).
- **Đã làm (vhtd_data_patch6.py):** (1) skills.txt 13 dòng theo tên cột: 1369/1384 MaxLevel 27; 1374 manamax_yan_p; 1376 +lifemax_yan_p/anti_sorbdamage_yan_p/enhancehiteffect_rate/enhancehit_rate; 1378 bỏ meleedamagereturnmana_p (Linux thêm); 1381 +lifemax_yan_p; 1382 AttackRadius 520, ChildSkillNum 4, +fatallystrike_p/skill_misslenum_v; 1968/1970 +attackspeed_yan_v/castspeed_yan_v; 1971 +lifemax_yan_p; 1980 +attackspeed_yan_v, allres_yan_p, fasthitrecover_yan_v; 2122 lifemax_yan_v; 2123 lifemax_yan_p/fasthitrecover_yan_v/allres_yan_p/anti_lightingres_yan_p. (2) `huashan.lua` = bản VLTK + tách 35 bảng `SKILLS.x=` (kiểm lua4 `-s100` dofile như client + so GetSkillLevelData 600 giá trị cấp 1/10/19 = trùng). (3) MagicDesc bỏ "(Dương)" ở mọi `_yan_` (chủ quyết đợt g) + để trống 10 thuộc tính VLTK không mô tả.
- **Thỉnh Anh Đề Nhuệ Lữ (1965) "có thời gian khôi phục chiêu thức và khi buff lên sẽ duy trì vài giây":** dữ liệu VLTK (= JX1 hiện tại): `sorbdamage_yan_p`/`anti_do_hurt_p`/`anti_enhancehit_rate` đều **36 khung = 2 giây**, tốn 4 tầng Nộ, `skill_desc` = "Điều kiện thi triển: 4 tầng [Nộ]". Ảnh chủ gửi có thêm "Thời gian khôi phục chiêu thức: 10 giây", "Sinh lực tối đa +30 %", "Nội lực tối đa +12 %", "Kháng tất cả +9 %" — bản VLTK trong máy KHÔNG có các dòng này.
- **Ảnh tooltip 2 ("Đoạt Mệnh Liên Hoàn Kiếm" / "Thái Nhạc Thanh Phong"):** = kỹ năng **1364 "Đoạt Mệnh Liên Hoàn Tam Tiên Kiếm"** (Hoa Sơn 90, kiếm bay = 1363 "Thái Nhạc Tam Thanh Phong") của chính client Level Up — dữ liệu 1363 cấp 20/21 trùng ảnh (566 %/603 % vật lý, băng sát 70/75, chí mạng 20 %). Ba dòng "Đòn đánh có 10 % tỷ lệ xuất… / Số lượng kiếm xuất ra: 3/9 kiếm / Thời gian hồi chiêu: 5 giây" **không nằm trong dữ liệu** (không có ở skills.txt, Lua `skill_desc`, magicdesc, stringtable — quét 34 pak + 327 tệp rời) mà do mã client VLTK ghép từ `autoreplyskill` → **đợt 6b port vào `KSkill::GetDescAboutLevel`** (`vhtd_engine_patch6.py`): với autoreply/autoattack/autorescue/autodeath/autocast in 3 dòng (tỷ lệ = byte thấp v3, số kiếm = ChildSkillNum kỹ năng sự kiện, hồi chiêu = v3>>8 / 18 giây), bỏ câu MagicDesc chung; khối "Tầng thứ 2: … [cấp N]" engine đã có. Nhãn đỏ "Độc Cô Cửu Kiếm" trong ảnh chưa rõ nguồn (có thể là tên bộ võ công) → chưa in. **Giá trị:** dữ liệu Level Up/Linux cho 1364 = hồi chiêu 15 s, tỷ lệ 1→3 % (khớp log đợt f "3–5 phút mới thấy kiếm"); ảnh = 5 s / 10 % → `vhtd_data_patch7.py` đặt 1364 = 5 s, 1→10 % (cấp 1→20) — **GIẢ ĐỊNH theo ảnh**, chủ đổi lại nếu không muốn (dòng `autoreplyskill` trong `SKILLS.duoming_start`, huashan.lua cả 2 phía).
- **Ảnh tooltip 1 (Thỉnh Anh Đề Nhuệ Lữ):** các dòng "Thời gian khôi phục chiêu thức 10 giây / Sinh lực tối đa +30 % / Nội lực tối đa +12 % / Kháng tất cả +9 %" không có trong dữ liệu Level Up (1965 chỉ có sorbdamage/anti_do_hurt/anti_enhancehit 2 giây + 4 tầng Nộ) → bản khác; cần chủ chỉ nguồn nếu muốn theo ảnh.
- **Icon trạng thái (bổ sung đợt 6b):** VLTK `slistcache.pak \ui\StatePos.ini` (212 buff) có icon THẬT cho 1965 (chưa có), 1970 (chưa có, debuff), 1973 (đang trỏ nhầm 智勇超伦 → 减攻击状态图标), 1988 (撼山击), 2128 (啖炙持觞buff), 2133 (深藏身名buff), 2139 (落雁平沙buff) — 4 icon đợt 5 tự sinh bị thay bằng bản thật; SPR có sẵn trong updatejx15/16.pak client đang nạp. BuffCount 152 → 154. Còn tự sinh (VLTK không có): 1976/1989/1991 Nộ, 2116 Âm Luật, 2130/2131/2134, 1366/1376/1380/1381.
- **Thông báo "đúng font chữ":** mọi chuỗi tôi thêm đã đổi từ escape bát phân sang TCVN3 thô (`oct2raw_tcvn3.py`, 18 escape trong KNpc.cpp/KSkills.cpp/KItem.cpp). Đo thêm bằng `cl.exe` 14.44: **tệp có BOM UTF-8 (`KNpc.cpp` từ trước 18/08) làm MSVC nuốt chuỗi TCVN3 thô (C4828) và biến ế/ó/í/ề/ũ/ừ/ý/ị thành '?' nếu bọc UTF-8 (C4566)** → `bo_bom_tcvn3.py` giải bọc 15 literal + cắt BOM; build lại: mọi chuỗi TCVN3 trong DLL trùng byte với bản đợt 5 (kể cả "Thiều Lâm" ở bảng phái từng hỏng ngầm). Các tệp còn BOM (BauCua.cpp, KProtocol.h, MouseHover.cpp, SpringGame.cpp, UiGamble.cpp, UiMeridian.cpp) không có literal thô → để yên; **luật:** trước khi thêm chữ Việt vào .cpp/.h phải `head -c3` kiểm BOM.

### 15.4 Định dạng đo được (dùng lại cho mọi việc sinh SPR)
- SPR thường: `SPRHEAD` 32 byte (`"SPR\0"`, WORD Width, Height, CenterX, CenterY, Frames, Colors, Directions, Interval, Reserved[6]) + KPAL24 × Colors + `SPROFFS{DWORD Offset, Length}` × Frames + mỗi khung `WORD w, h, ox, oy` + RLE (count, alpha[, count chỉ số]). Alpha "mờ" trong tệp VNG = **0xF6 (246)**. `ReverseTools\pak_vltk\spr2png.py` giả định header 28 byte → sai; bộ giải/mã đúng: `vhtd_data_patch5.py` (`spr_to_image` / `encode_spr`).
- Mục pak cờ 0x01 (UCL) và 0x20 (VNG) = nén NRV2B **cả blob** → `ucl.nrv2b_decompress_8(blob, size)`; cờ 0x10 = nén theo khung (`giai_spr_khung`).

### 15.5 CHECKLIST SWAP đợt 5+6 (chủ chạy `ChayGameServer.bat` / `ChoiGame.bat`; 2 tệp CÙNG LÚC, Game.exe giữ `5db988fc`)
1. `bin\server\CoreServer.dll.moi` — 18.253.312 byte, md5 `a4e813d3` (12:27).
2. `bin\client\CoreClient.dll.moi` — 2.448.384 byte, md5 `42c8b9fc` (12:27).
3. Dữ liệu đã ghi thẳng (đọc khi khởi động): `settings\npcs.txt` (server `b033b7e5` / client `28a98db6`), `settings\skills.txt` (server `a79be5ba` / client `e8e4b388`), `settings\missles.txt` (`f108af8d`), `script\skill\huashan.lua` (`53c90880` cả hai, bản VLTK tách bảng + 1364 5 s/10 %; bản cũ `.truoc_vhtd_patch6_0209`), `script\skill\wuhuntang.lua` (`64821f96`), `settings\MagicDesc.ini` (`65a11739`); client `ui\StatePos.ini` (`8ca86c8d`, BuffCount 154), `data\sprvuhontieudao3.pak` (18.807 byte, `f6e45f7f`), `package.ini` (`01e85ffa`, thêm `39=`).
4. Nghiệm thu: (a) thành Tiêu Dao: sứ giả đúng hình, đứng yên, không đánh người, đứng cạnh máu không tụt; (b) góc trái: icon buff 24×24 đúng ô, hết icon "00s"; Thỉnh Anh Đề Nhuệ Lữ / Huy Sư Diệt Lỗ / Chiết Kích / Hãm Sơn Kích / Túy Tửu / Phất Y / Tiêu Dao Vũ dùng icon VLTK thật; (c) Nộ/Âm Luật 0 → bấm Tạp Đạp Lưu Tinh / Hãm Sơn Kích: đứng yên + "Không đủ N tầng ..."; (d) đủ tầng Âm Luật (**6** tầng ở cấp 20 — xem đính chính mục 19): lướt → nổ "Khởi" tại điểm đến + sát thương quanh 2 ô; (e) tooltip Tùy Ý Khúc có "Sát thương Lôi (theo nội lực tối đa): X"; (f) Hoa Sơn: tooltip Đoạt Mệnh Liên Hoàn Tam Tiên Kiếm cấp 20 = "Đòn đánh có 10% tỷ lệ xuất Thái Nhạc Tam Thanh Phong / Số lượng kiếm xuất ra: 3/9 kiếm / Thời gian hồi chiêu: 5 giây / Tầng thứ 2: … [cấp 20] 566 %…"; Cửu Kiếm Hợp Nhất tương tự (Độc Cô Cửu Kiếm 9/9 kiếm); Phách Thạch Phá Ngọc vòng 520, 4 đạn; Kim Nhạn Hoành Không 512; nhân vật Hoa Sơn cần **học lại** ở NPC test (bảng cấp 27); (g) tooltip 3 phái: hết dòng lạ, không còn "(Dương)"; (h) học 120/150 ở NPC test rồi mở bảng kỹ năng.


## 16. ĐỢT 7 (02/09 ~13:15–15:30) — 11 mục chủ test sau swap đợt 5+6 + "log hết các phái mới"

Tool: `vhtd_engine_patch7.py` (marker `[VHTD 02/09k]`: KMissle.cpp 18 hunk, KNpc.cpp, KPlayer.cpp, KProtocolProcess.cpp) + `vhtd_data_patch8.py`; sửa tay `huashan.lua`; sửa tool `vhtd_data_patch7.py` (kiểm lua4 bắt buộc) và `vhtd_data_patch5.py` (main guard).

### 16.1 Gốc từng mục

| # | Chủ báo | Gốc đo được | Vá |
|---|---|---|---|
| 1 | "skill Hoa Sơn mất hết sạch thuộc tính" | `vhtd_data_patch7.py` chèn chú thích `-- [VHTD…]` vào GIỮA dòng `autoreplyskill={{…}}},` → `--` nuốt dấu `},` → `huashan.lua` lỗi cú pháp "`}' expected (line 269)" → bảng SKILLS không nạp → mọi kỹ năng Hoa Sơn 0 thuộc tính. | Bỏ chú thích (md5 `f5be47ce` cả 2 phía, lua4 -s100 nạp 35 bảng). Tool: cấm chú thích inline + bắt buộc kiểm lua4 trước khi ghi. **Server phải khởi động lại** mới nạp lại Lua. |
| 2+8 | "skill cầm Tiêu Dao vẫn đánh hụt (4-5 lần mới trúng)", "các skill phái mới miss nhiều" | (a) Khối vá đợt 4 (`if (m_nMissleId >= 500 …)`) so sánh **chỉ số instance** trong `Missle[]` chứ không phải id đạn (KMissleSet::Add ghi đè `m_nMissleId = nFreeIndex`) → vá chỉ có tác dụng NGẪU NHIÊN tùy ô trống → lúc trúng lúc hụt. (b) JX1 `ProcessCollision` quét ±DmgRange/2 (xem mục 5). (c) Các tag log cũ (`MSL-BARRIER`, `MSL-Z-LOW`, `DENY-ALLY`, `COLL-CLIENTSEND`…) là `AUTOLOG_EVERY` đặt TRƯỚC `if` → in bất kể nhánh có chạy → không dùng đếm tag làm bằng chứng được. | Kiểu đạn lấy qua `KSkill::GetChildSkillId()` của kỹ năng tạo đạn (`VhMissleType`); ngữ nghĩa VLTK cho đạn id ≥ 400; log toàn bộ `VH-*` (16.2). |
| 5+7 | "các skill có vòng tròn nhỏ hơn nhiều so với client VLTK", "bạn tính sai missle" | Mổ `game_y.exe` (capstone; KMissle vùng 0x6f8000–0x6fe000, offset thành viên trùng Linux): **`ProcessCollision` VLTK (0x6fb6c0)** gọi hàm tìm NPC trong **BÁN KÍNH nRange ô** (0x6fa840 → KSubWorld tìm theo ô tuyệt đối), không chia đôi; JX1 quét `±nRange/2` → diện tích chỉ **¼**. **`CheckCollision` VLTK (0x6fba60)**: `CollidRange > 1` → lấy NPC còn sống trong bán kính CollidRange → `DmgRange == 1` thì đánh đúng con đó (ProcessCollision tại NPC, range 1), ngược lại `ProcessCollision()` tại đạn với bán kính DmgRange. VLTK cũng đọc CollidRange/DmgRange bằng GetInteger (3.5 → 3). Dữ liệu missles.txt 3 phái đã trùng VLTK 100 % nên khác biệt hoàn toàn do engine. | Đạn id ≥ 400 (sự kiện 2013 400–411, Hoa Sơn 412–441, 3 phái ≥ 500): `ProcessCollision` quét ±nRange, `CheckCollision` theo VLTK. Đạn < 400 giữ JX1 (cân bằng cũ). |
| 3 | "lực tay khi bỏ kỹ năng cầm hiện 0/0" | Bảng trạng thái (`UiStatus.cpp` `m_LeftDamage`) ← `KPlayer::GetEchoDamage` chỉ cộng sát thương vật lý/hệ dạng `_v`; cầm dùng `lightingdamage_p` (ô [12]) → 0/0. | Thêm nhánh `lightingdamage_p`: (nội lực tối đa + nội công + nội công Lôi) × (100+addlightingmagic_p)/100 × % (đúng công thức đợt 4). |
| 4 | "ống máu Lạc Nhạn Bình Sa nằm ở thanh máu dưới tên, không phải trên ống máu" | Đợt 4 vẽ `Player_Shield` trên thanh sinh lực góc trái. | `KNpc::PaintLife` (client): thanh xanh (70,170,255) ngay trên thanh máu dưới tên của chính mình khi còn khiên; gỡ `Button6=Shield` khỏi `UiHeaderControlBar.ini`. |
| 6 | "Thỉnh Anh Đề Nhuệ Lữ buff lên tồn tại hình ảnh 2-3 giây rồi biến mất" | Đúng dữ liệu VLTK: 3 thuộc tính 36 khung = 2 giây, tốn 4 tầng Nộ. | Giữ. |
| 9 | "dựa vào missle và skill.txt bản VLTK phân tích fix lại" | Đã so toàn bộ: missles.txt 3 phái trùng; VLTK đổi 24 đạn **cổ điển** (34, 57, 94, 284, 291–297, 318, 319, 326–331, 335, 336, 342, 345, 348, 363, 372, 413, 426) và 151 dòng skills.txt cổ điển (MaxLevel 20→27 Cái Bang 1055–1059, AttackRadius, ChildSkillNum, SkillStyle…). Chỉ đồng bộ **426** (Phách Thạch Phá Ngọc: CollidRange 1→3, LifeTime 15→24, Speed 25→30, ColVanish 0→1). | 23 đạn + 151 kỹ năng cổ điển KHÔNG đổi (cân bằng cũ) — chủ quyết nếu muốn theo VLTK toàn bộ. |
| 11 | "Vũ Mục Di Thư trước hiển thị màu đúng, giờ lạc màu" | Đợt 5 chuyển sang icon **trạng thái** thật của VLTK (`状态图标\武穆遗志.spr` 24×24, 255 màu, alpha đủ) thay icon **kỹ năng** 36×36 đợt 4 → khác artwork (render kiểm `scratchpad\icon24\wumu_cmp.png`: nền đỏ cam, người ở giữa). | Giữ icon VLTK; muốn màu cũ thì trỏ lại `\spr\Ui\技能图标\武穆遗志.spr` trong StatePos (dữ liệu). |
| — | Sự cố tool 13:40 | `render_wumu.py` `import vhtd_data_patch5` → tool không có `if __name__` nên TỰ CHẠY LẠI patch 5 → ghi đè 5 dòng icon StatePos về bản tự sinh (pak không ghi được vì client đang mở). | Chạy lại patch7 (idempotent) trả icon VLTK; thêm main guard cho patch5. |
| — | 2124 Kiếm Quyết Phù Vân 0 hit | Đạn 641 "_空" CollidRange 0 / DmgRange 0 = đạn rỗng cả ở VLTK (`nRange <= 0 → return 0`); sát thương do 2125/2126 (639/640). | Bình thường. |

### 16.2 Log toàn bộ 3 phái (`VHLOG` = `AUTOLOG_IDX(launcher)`, chỉ kỹ năng 1363–1384 / 1965–1991 / 2114–2143)
- Server `jx_auto_server.log` (nhân vật `config.ini [AutoLog] Name=CaiBang`), client `jx_auto.log` (mọi launcher):
  `[VH-MSL-NEW]` (đạn ra: kiểu, move, speed, life, col/dmg, follow, vị trí, mục tiêu) · `[VH-MSL-TICK]` mỗi khung (vị trí, z, khoảng cách ô tới mục tiêu, barrier) · `[VH-COL-IN]` / `[VH-COL-FOUND]` / `[VH-COL-NONE]` (kiểm va chạm) · `[VH-SCAN-IN]` / `[VH-SCAN-NPC]` / `[VH-SCAN-OUT]` (quét sát thương, bán kính thật) · `[VH-DMG-IN]` / `[VH-DMG-TRY]` / `[VH-DMG-HIT] life a -> b` / `[VH-DMG-SKIP]` · `[VH-END-BARRIER]` / `[VH-END-COLFAIL]` / `[VH-END-REGION]` · `[VH-MSL-END]` · client `[VH-CL-CAST-DIRECT]` / `[VH-CL-CAST-NET]` (nhận lệnh phóng).
- Cách đọc "không hiện hình kỹ năng": client có `[VH-CL-CAST-*]` mà không có `[VH-MSL-NEW]` = không tạo đạn (lỗi Cast); có NEW mà END ngay khung đầu = tan sớm (xem `VH-END-*`); có NEW không có `[VH-MSL-TICK]` = không bay.

### 16.3 CHECKLIST SWAP đợt 7 (chủ chạy `ChayGameServer.bat` / `ChoiGame.bat`; 2 tệp CÙNG LÚC, Game.exe giữ `5db988fc`)
1. `bin\server\CoreServer.dll.moi` — 18.258.432 byte, md5 `6c6ad9b5` (13:45).
2. `bin\client\CoreClient.dll.moi` — 2.454.016 byte, md5 `768b9ad6` (13:45).
3. Dữ liệu đã ghi thẳng: `script\skill\huashan.lua` (`f5be47ce` cả hai — **cần restart server**), `settings\missles.txt` (`4dc8f06f` cả hai, đạn 426), client `ui\Ui3\UiHeaderControlBar.ini` (`c367e20a`, bỏ Button6=Shield), `ui\StatePos.ini` (`8ca86c8d`, đã phục hồi).
4. Nghiệm thu: (a) Hoa Sơn tooltip có thuộc tính lại; (b) Thập Bộ Nhất Sát / Lạc Nhạn Bình Sa / Dương Quan Tam Điệp: quái trong cả vòng tròn (bán kính 8 / 8 / 3 ô) đều mất máu; (c) cầm: Tùy Ý Khúc/Quảng Lăng Tán trúng liên tục, log `[VH-DMG-HIT]`; (d) bảng trạng thái với kỹ năng cầm: Lực tay ≠ 0/0; (e) Lạc Nhạn Bình Sa: thanh xanh dưới tên, không còn trên ống máu; (f) gửi `jx_auto_server.log` + `jx_auto.log` sau 5–10 phút test để tôi đọc `VH-*`.


## 17. ĐỢT 8 (02/09 ~14:45–19:30) — 2 ảnh Thỉnh Anh Đề Nhuệ Lữ, "kéo log về xem lỗi thì fix", 3 mục Hoa Sơn, sự cố "mất đường dẫn spr bày bán"

Tool: `vhtd_engine_patch8.py` (`[VHTD 02/09m]`), `vhtd_engine_patch9.py` (`[VHTD 02/09n]`), `unwrap_cp1258.py` (`[VHTD 02/09p]`), `vhtd_engine_patch10.py` (`[VHTD 02/09q]`, 8 hunk). Điều tra log bằng 6 tác tử đọc `server_1443.log` (41 MB) + `client_1438_prev.log` (67 MB) → 23 phát hiện có bằng chứng; theo luật chủ, chỉ phản biện phần có nguy cơ.

### 17.1 Sáu lỗi "đánh hụt" tìm ra từ log (đều đã vá, chỉ áp cho đạn/kỹ năng thời VLTK)

| | Gốc | Bằng chứng | Vá |
|---|---|---|---|
| **A. Ô đạn bị trễ** | `KMissle::CheckBeyondRegion` chuẩn hoá offset chỉ lùi/tiến **đúng một ô mỗi khung** (`if/else if`). Đạn VLTK nhanh (528 = 55 px/khung, 529 = 45, 580 = 60, 641 = 80) vượt 1 ô (32 px) mỗi khung → dư ~0,7 ô dồn lại, sau 6–7 khung ô hiện tại **trễ 3–6 ô** so với vị trí thật. Toạ độ pixel vẫn đúng (nên log nhìn bình thường) nhưng vòng quét va chạm quét **phía sau** đạn. | 1969 phóng 103, **48 lần (47 %)** trượt đúng kiểu này; offset chưa chuẩn hoá lên tới 167936 = 5 ô. | Đổi `if/else if` → `while` cho đạn VLTK. Đạn cổ điển ≤ 1 ô/khung nên `while` chạy đúng 1 vòng = y hệt cũ. |
| **B. Bộ đếm sát thương kế thừa** | `m_ulNextCalDamageTime` chỉ đặt 0 ở hàm khởi tạo; `Release` phía server không đặt lại → đạn mới sinh vào ô cũ **kế thừa khoá sát thương** của đạn trước (17 khung = 0,94 s với 645/646/647; 21 khung với 527) → bị khoá ngay từ khung đầu. | 1967: **24/65** lần phóng bị chặn kiểu này (đúng mô tả "4–5 lần mới trúng"). | Đặt lại 0 (cả `m_nHitCount`) trong `KMissleSet::Add`. Đây là khởi tạo, không đổi hành vi chiêu cũ. |
| **C. Xác che mục tiêu** | Nhánh VLTK gọi `FindNpc` không truyền mục tiêu → trả **con đầu tiên** của ô; nếu là xác thì `continue` bỏ **luôn cả ô** → mục tiêu còn sống đứng chung ô không bao giờ được xét. | 1969 bắn 4 lần liên tiếp vào con 30000 máu đứng trên xác, cả 4 lần `VH-COL-NONE`. | Truyền mục tiêu đang bám (hoặc mốc `MAX_NPC`) để `FindNpc` dùng nhánh sẵn có: ưu tiên mục tiêu, và **con sống hơn xác**. |
| **D. Hồi quy bản vá đợt 7 của tôi** | `nRangeX = VLTK ? nRange : nRange/2` áp cả cho lời gọi `nRange = 1` (đánh **đúng một** NPC đã chọn) → quét 3×3 thay vì 1 ô → đạn đơn mục tiêu đánh lan con đứng cạnh. | Chính chú thích đợt 7 ghi "DmgRange == 1 → đánh ĐÚNG NPC này". | Chỉ bỏ `/2` khi `nRange > 1`. |
| **E. Xác ăn lượt quét** | Vòng quét sát thương đếm xác vào `nRet`/`m_nHitCount` và gọi sát thương lên xác → đạn giới hạn số mục tiêu hết lượt vì xác. | — | Đạn VLTK bỏ qua `do_death`/`do_revive` (log `[VH-SCAN-XAC]`). |
| **F. Auto-skill bắn vào xác** | `AttackSkill`/`ReplySkill`/`CastAutoSkillAt` chỉ loại `do_death`/`do_revive`, **không loại máu ≤ 0** — nạn nhân vừa bị đòn cuối về 0 máu (chưa kịp đổi trạng thái) vẫn được chọn. | 1368: **130/234** kiếm bay hết đời không trúng ai; 1969: 19 lần phóng vào mục tiêu đã chết; 1970: 39/93. | Loại luôn mục tiêu hết máu ở cả 3 chỗ. |

### 17.2 Ba mục còn lại của đợt 8

| Mục | Gốc | Vá |
|---|---|---|
| **Vòng khiên Thỉnh Anh Đề Nhuệ Lữ nhỏ, không duy trì** | Đạn 525 (1965) **không có ảnh** — vô hình, sống đủ 20 s, chỉ áp buff. Vòng khiên thật là đạn **526** do kỹ năng sự kiện **1966** tạo. `KSkill::Cast`/`OnMissleEvent` đòi `EventSkillLevel > 0` nhưng dữ liệu VLTK ghi **−1** (= cấp của chính kỹ năng) → 1966 chưa bao giờ phóng: log 0 dòng 1966/526 cả server lẫn client. Cùng lỗi ở 1376 → 1377. Vòng nhỏ chủ thấy là ảnh thi triển. | `VhEventLevel()`: −1 → cấp kỹ năng, chỉ cho kỹ năng id ≥ 1347 (23 kỹ năng cổ điển giữ nguyên). 526 lặp ảnh nên vòng khiên tồn tại đủ thời gian buff. |
| **"Mất đường dẫn các spr bày bán"** (lỗi của tôi, đợt 6) | `KNpc.cpp` lưu mọi chuỗi kiểu **cp1258→UTF-8** kèm BOM (trình biên dịch giải ngược đúng). Đợt 6 tôi cắt BOM nhưng chỉ giải bọc 15 chuỗi chữ Việt → **10 chuỗi GBK** còn bọc thành byte đôi: `\Spr\Ui3\摆摊\摆摊头顶条－中/右/左.spr` (biển sạp hàng), `mag_spe_眩晕.spr`, `男主角/女主角/人物名称`. | `unwrap_cp1258.py` giải bọc toàn bộ đúng ánh xạ cp1258. Bản mới: 3 đường dẫn sạp hàng + `mag_spe_` đúng GBK, **0** chuỗi `.spr` còn dấu vết bọc. Đã ghi luật vào bộ nhớ. |
| **Thần Quang Toàn Nhiễu không kích nổ Ma Vân Kiếm Khí** | 1384 có thuộc tính kích nổ trỏ đạn 419/428, nhưng `KNpc::DetonateMissles` so `m_nMissleId` — **chỉ số ô đạn**, không phải kiểu đạn → không bao giờ khớp 419 (cùng bẫy với bản vá đợt 4). | So kiểu đạn qua kỹ năng tạo đạn. Kích nổ → đạn tan → sự kiện tan của 1380 → 1411 Kiếm Khí Vô Định (băng sát + đóng băng). Log `[HS-DETONATE]`. |

### 17.3 Đã kiểm và thấy bình thường (tác tử đối chiếu log)
- 2124 → 2125 sinh đủ 4 đạn mỗi lần phóng (183 = 46×4−1, một đạn chết vì địa hình); 2125 trúng ~2 lần/lần phóng — đúng thiết kế.
- 2141 Cao Sơn Lưu Thủy 51/55 trúng; 2138 Lạc Nhạn Bình Sa 2/2, sự kiện 2139 nổ đủ; 2143 Dương Quan Tam Điệp 44/44 đạn sống trọn đời.
- Nhịp tự phóng 1364 → 1363 (10 %, hồi 5 s) và 1369 → 1368 (20 %, hồi 5 s) đo trong log **khớp** dữ liệu đợt 7: 8 lần / 572 s và 27 lần, khoảng cách nhỏ nhất đúng 5,0 s.
- Client thấy đủ kiếm: 237 đạn 418 = toàn bộ lần phóng của server nằm trong cửa sổ log.
- 1965 sống trọn 365/365 khung (20 s) cả hai phía; 1972/1974/1978/1973 đều đủ đời, sự kiện con nổ đúng.

### 17.4 Còn lại — chờ chủ quyết (không tự sửa)
1. **2129 Thập Bộ Nhất Sát**: đạn 642 hồi sát thương **1 khung**, đời 10 khung, bán kính 8 ô → mỗi quái trong vòng tròn ăn **10 đòn**. Đây là dữ liệu VLTK y hệt; muốn giảm thì đặt trần số đòn mỗi mục tiêu (cần thêm mã) hoặc sửa dữ liệu.
2. **Kỹ năng của chính mình đôi khi không hiện hình** (1979 rõ nhất, 4/10 lần): client tự đoán trước và **tự từ chối** vì "quá xa" theo vị trí client, trong khi server chấp nhận; gói phát lại của server chỉ vẽ cho **người khác**, không vẽ cho chính mình. Sửa được ở client nhưng đụng luồng vẽ chung → cần chủ duyệt.
3. **Đạn đường thẳng có cờ tan-khi-chạm (527/528/645/647)** biến mất ngay khung đầu khi chạm, nên nhìn như "không có hiệu ứng". Sửa được kiểu hoãn tan phần hình ở client.
4. **2142 Mai Hoa Tam Lộng**: hai nốt bên lệch 16,9° nên quá 220 px là trượt mục tiêu đơn — đúng dữ liệu VLTK; muốn khác thì sửa dữ liệu.
5. **Huyền Nhãn Vân Yên (bóng mờ)** và **lực tay nội công 60k**: workflow điều tra bị dừng giữa chừng do hết hạn mức phiên, chưa có kết luận có bằng chứng. Số liệu thô đã có: 1382 dùng băng sát VLTK `{25: 14400, 40: 36000}` cộng dồn `addskilldamage1` từ nhiều buff (+60 % mỗi cái) và `manatoskill_enhance` 100 % khi đầy nội lực → 60k là **cộng dồn dữ liệu**, không phải công thức hiển thị sai; ngoại công tính theo lực tay vũ khí × phần trăm nên ~10k. Sẽ xác nhận ở đợt sau.

### 17.5 CHECKLIST SWAP đợt 8 (chủ chạy `ChayGameServer.bat` / `ChoiGame.bat`; 2 tệp CÙNG LÚC, Game.exe giữ `5db988fc`)
1. `bin\server\CoreServer.dll.moi` — 18.277.376 byte, md5 `244a3a18` (19:11).
2. `bin\client\CoreClient.dll.moi` — 2.455.040 byte, md5 `f73cd480` (19:11).
3. Không có tệp dữ liệu nào đổi ở đợt này.
4. Nghiệm thu: (a) Thỉnh Anh Đề Nhuệ Lữ: vòng khiên to hiện đủ thời gian buff; (b) sạp hàng "bày bán": biển hiện lại; (c) Ma Vân Kiếm Khí rồi Thần Quang Toàn Nhiễu: khí trường nổ; (d) Kinh Đào Phách Ngạn / Trấn Biên Chuỳ / Trừ Gian Diệt Nịnh: bắn trúng liên tục, hết cảnh "đạn bay qua mà không mất máu"; (e) kiếm tự phóng Hoa Sơn không còn bay vào xác; (f) gửi lại log sau 5–10 phút để tôi đo lại tỷ lệ trúng.


## 18. ĐỢT 9 (02/09 ~19:15–20:00) — 3 mục Hoa Sơn chủ giao

Tool: `vhtd_engine_patch11.py` (`[VHTD 02/09r]`), `vhtd_data_patch9.py` (`[VHTD 02/09r]`).

### 18.1 Bóng mờ Huyền Nhãn Vân Yên (1358) — ĐÃ VÁ

**Gốc:** engine JX1 **đã có đủ** hệ vẽ bóng mờ theo khung — `KNpcRes::Draw` mỗi khung hạ một ảnh mờ khi cờ `m_nBlurState` bật, `KNpcBlur::ChangeAlpha` làm nhạt dần, `KNpcRes::SetBlur` bật/tắt (chiêu lướt Tạp Đạp Lưu Tinh và các chiêu `NeedShadow` đang dùng đúng hệ này). Nhưng thuộc tính `walkrunshadow` chỉ đi tới `m_WalkRun.nTime` rồi đồng bộ qua cờ `STATE_WALKRUN` sang client, và **không một dòng nào đọc để vẽ**: cả cây nguồn chỉ có 4 chỗ nhắc `m_WalkRun` (đặt, giảm mỗi khung, đóng gói đồng bộ, nhận gói) — không có chỗ vẽ.

**Vá:** trong `KNpc::Activate` (client, chạy mỗi khung), khi nhân vật **đang đi hoặc chạy** thì bật/tắt bóng mờ theo `m_WalkRun.nTime`. Chỉ chạm ở trạng thái đi/chạy nên không đụng bóng mờ của chiêu `NeedShadow` (đặt trong `DoSkill`) và các chỗ tắt sẵn có. `walkrunshadow` là thuộc tính **duy nhất kỹ năng 1358 dùng** trong toàn bộ bảng kỹ năng → không chiêu cổ điển nào bị ảnh hưởng.

**Kèm theo (dữ liệu):** hàng 1358 còn **thiếu 5 thuộc tính** so VLTK (đợt 6 chỉ đồng bộ 13 hàng khác): `me2firedamage_p`, `fire2medamage_p`, `anti_block_rate`, `fasthitrecover_yan_v`, `fatallystrikeres_p`, và `lifemax_p` → `lifemax_yan_p`. Đã đồng bộ. Cùng dịp sửa nốt 2 hàng Hoa Sơn còn lệch: **1349** Kiếm Tông Tổng Quyết (thiếu `attackspeed_yan_v`, `addcolddamage_v`, `anti_hitrecover`) và **1385** Thần Quang Toàn Nhiễu 2 (`sorbdamage_p` → `sorbdamage_yan_p`). Không đụng 2119 (giả định chờ chủ duyệt) và các chiêu BOSS/danh hiệu.

### 18.2 Lực tay nội công 60k so ngoại công 10k — KHÔNG phải lỗi hiển thị

Đã dò cả hai đường: công thức hiển thị (`KPlayer::GetEchoDamage`) và công thức sát thương thật của máy chủ (`KNpc::AppendSkillEffect`). Hai bên **khớp nhau**; chênh lệch nằm ở **dữ liệu**.

**Nội công** — Phách Thạch Phá Ngọc (1382) dùng **số tuyệt đối** lấy nguyên từ VLTK (bảng `pishi_poyu`, chú thích ghi rõ "neihuashan150"): băng sát 11.433 ở cấp 20, 14.400 ở cấp 25, 21.600 ở cấp 30. Rồi nhân với tổng phần trăm cộng thêm:

| Cấp | Không buff | 3 buff Hoa Sơn | 4 buff | 4 buff + Vũ Hồn 1965 |
|---|---|---|---|---|
| 20 | 11.433 | 43.445 | 50.305 | 57.165 |
| 25 | 14.400 | 56.448 | 65.664 | 74.880 |
| 30 | 21.600 | 87.912 | 102.816 | 117.720 |

Phần trăm cộng thêm đến từ `addskilldamage1` trỏ vào 1382 của **bốn** bảng (Thanh Vân Tống Sảng 1372, Long Huyền Kiếm Khí 1376, Ma Vân Kiếm Khí 1380, Thần Quang Toàn Nhiễu 2 1385), mỗi cái **+60 %** ở cấp 20 và **+69 %** ở cấp 30, cộng `manatoskill_enhance` của Khí Quán Trường Hồng 1379 (**+100 %** cấp 20, +136 % cấp 30) khi đầy nội lực. Bảng Vũ Hồn `qingyin_tiruilv1` (Thỉnh Anh Đề Nhuệ Lữ 1965) **cũng** trỏ +60 % vào 1382 — điều này có sẵn trong dữ liệu VLTK, chỉ ảnh hưởng nhân vật test học nhiều phái.

**Ngoại công** — Kim Nhạn Hoành Không (1351) dùng **phần trăm nhân lực tay vũ khí**: 880 % ở cấp 20, 1328 % ở cấp 30.

| Cấp | Lực tay vũ khí 800 | 1.000 | 1.500 |
|---|---|---|---|
| 20 | 7.840 | 9.800 | 14.700 |
| 30 | 11.424 | 14.280 | 21.420 |

**Kết luận:** nội công là số cố định theo thang cấp 150 của VLTK, ngoại công là phần trăm nhân lực tay vũ khí **của máy chủ này**. Vũ khí ở đây khoảng 1.000 lực tay nên ngoại công ra ~10k, còn nội công giữ nguyên 60k. Ở VLTK gốc, vũ khí cấp 150 mạnh hơn nhiều nên hai bên cân nhau.

**Hai điểm phụ đo được** (đều làm bảng hiển thị **thấp hơn** sát thương thật, không phải cao hơn): bảng hiển thị bỏ qua hệ số theo cấp trên 100 mà máy chủ có áp; và `manatoskill_enhance` bảng chỉ cộng khi nội lực **đầy tuyệt đối**, còn máy chủ cộng theo tỷ lệ nội lực hiện có.

**Ba cần gạt nếu chủ muốn kéo nội công xuống** (tôi **không tự sửa**, đây là cân bằng):
1. **Dùng lại dòng VLTK cũ đã bị chú thích ngay trong `huashan.lua`**: `{1,200},{40,8000}` thay cho `{1,160},{25,14400},{40,36000}` → cấp 20 còn **17.600** (4 buff), cấp 25 còn 22.800, cấp 30 còn 28.560. Đây là chính con số VLTK từng dùng trước khi họ nâng lên.
2. **Bớt số buff cộng phần trăm**: bỏ `addskilldamage1` ở một hoặc hai trong bốn bảng.
3. **Đổi cách tính**: hiện `GetAddSkillDamage` cộng phần trăm của một chiêu **chỉ cần đã học**, không cần đang bật buff. Đây là mã dùng chung cho **mọi phái** từ gốc JX1 (Thiếu Lâm, Cái Bang… đều dựa vào), đổi sẽ thay cân bằng toàn máy chủ nên tôi khuyên không đụng.

### 18.3 Thập Bộ Nhất Sát (2129) — giữ đúng VLTK, không đổi
Đã đối chiếu từng cột: đạn 642 (bán kính 8 ô, hồi sát thương 1 khung, đời 10 khung) và hàng kỹ năng 2129 **trùng 100 %** dữ liệu VLTK. Theo yêu cầu "làm đúng dữ liệu VLTK" → giữ nguyên, mỗi quái trong vòng tròn ăn 10 đòn đúng như bản gốc.


> **CẢNH BÁO BUILD SONG SONG (lần 2 trong ngày):** bản build đầu của đợt này (19:30, `eaa0390c` / `21f903c7`) **ĐÃ BỊ NUỐT** phần dở dang của luồng công việc khác — họ sửa 7 tệp trong cùng cây `D:\GAMEDEVNEW` lúc 19:25–19:27 (FUSCHAT / Vân Cương 6 ô, gồm `CoreShell.cpp`, `GameDataDef.h`, `KItemDice.cpp`, `KPlayerBot.cpp`). **KHÔNG swap hai tệp đó.** Bản trong bảng dưới build lại từ `git worktree` tại đúng commit `65076c82`, không chứa mã của họ (đã đối chiếu: bản cây chung khác bản cây sạch, `CoreClient` lớn hơn 512 byte). Luật mới: `git status` trước mỗi lần build; có tệp sửa dở không phải của mình thì build ở worktree riêng.

### 18.4 CHECKLIST SWAP đợt 9 — **3 TỆP CÙNG LÚC** (chủ chạy `ChayGameServer.bat` / `ChoiGame.bat`)

> **BẮT BUỘC 3 TỆP, KHÔNG PHẢI 2.** Phiên `wauto-6e` đang làm `[FUSCHAT 02/09]` ("trang bị dung luyện khi post lên kênh chat không hiện thông tin dung luyện") đã đổi cấu trúc `ChatItem` trong `GameDataDef.h`: thêm 6 ô Vân Cương (`m_nFusionP[6]` + `m_uFusionSeed[6]`), **105 → 153 byte**, và `NUM_INFO_ITEM_CHAT` **43 → 44**. `ChatItem` nằm trong gói `s2c_diceitem` nên máy chủ, client và Game.exe **phải cùng một cỡ**. Swap lệch = mọi link vật phẩm trong chat thành chữ thô và gói xúc xắc tách sai.
>
> Hai bản `ce444da3` / `d1ec5e41` tôi build lúc 19:47 lấy ở commit `65076c82` — **trước** phần FUSCHAT — nên **KHÔNG dùng nữa**. Bộ đúng do phiên `wauto-6e` build lại cả 3 tệp từ HEAD `431f2e50` (HEAD đã gồm cả hai luồng: đợt 9 Hoa Sơn của tôi và FUSCHAT của họ).

1. `bin\server\CoreServer.dll.moi` — 18.277.376 byte, md5 `2f9bb7de798b` (19:45).
2. `bin\client\CoreClient.dll.moi` — 2.456.576 byte, md5 `227895cf1ac6` (19:53) — **bản thay thế**,
   phiên `wauto-16` build lại tại `7cab3e26` (WAuto Hậu cần "phú về bán rác thì đứng yên ở thành")
   trong cây chính, `-t:Rebuild -m:1`. `7cab3e26` là con cháu của `431f2e50` nên **vẫn mang đủ
   FUSCHAT** — đã đối chiếu `git show 7cab3e26:GameDataDef.h`: `NUM_INFO_ITEM_CHAT` = 44 +
   `m_nFusionP`/`m_uFusionSeed` ⇒ `ChatItem` vẫn 153 byte, bộ 3 tệp **không lệch**.
   *(bản `a9f4f516e6c1` 19:45 đã bị thay, không dùng nữa)*
3. `bin\client\Game.exe.moi` — 1.377.792 byte, md5 `f6a2229c290a` (19:45).
   *(tệp 1 và 3 do phiên `wauto-6e` build từ **cây sạch** `git worktree D:\GAMEDEVNEW_wt_fuschat` tại
   HEAD `431f2e50`, `-t:Rebuild -m:1` từng dự án một. `7cab3e26` chỉ đổi `CoreShell.cpp` — tệp bị
   `ExcludedFromBuild` ở cả 4 cấu hình Server (`Core.vcxproj:705-710`) và không nằm trong Game.exe —
   nên **không phải build lại 2 tệp này**.
   Bản đang chạy trước swap: CoreServer `244a3a18085d` · CoreClient `f73cd48037e0` · Game.exe `5db988fc529f`.)*
4. Dữ liệu đã ghi thẳng: `settings\skills.txt` (server `734b92fd` / client `d7f9d0b3`) — 3 hàng 1349/1358/1385; bản cũ giữ ở `.truoc_vhtd_patch9_0209`. **Cần khởi động lại máy chủ** để nạp lại bảng kỹ năng.
5. Nghiệm thu phần đợt 9: (a) bật Huyền Nhãn Vân Yên rồi chạy → có vệt bóng mờ phía sau, tắt buff thì hết; (b) tooltip 1358 có thêm các dòng kháng đỡ / giảm sát thương hệ Hỏa / kháng chí mạng; (c) Kiếm Tông Tổng Quyết có thêm tốc đánh và băng sát; (d) phần FUSCHAT: post trang bị đã dung luyện lên kênh chat → hiện đủ thông tin Vân Cương.


## 19. ĐỢT 10 (02/09 ~20:00) — "Tạp Đạp Lưu Tinh giảm yêu cầu Âm Luật lại, bản gốc client VLTK đâu phải 10"

### 19.1 Kết quả đo — **không phải lỗi, và dữ liệu ta đã đúng VLTK**

Chi phí Âm Luật của 2118 **giảm dần theo cấp kỹ năng** (chạy bằng bộ Lua 4 của chính engine):

| Cấp kỹ năng | 1 | 5 | 10 | 15 | **20** (tối đa) | 25 | 30+ |
|---|---|---|---|---|---|---|---|
| Tầng Âm Luật cần | 10 | 9 | 8 | 7 | **6** | 5 | 4 |

- **Dữ liệu ta trùng VLTK 100 %** — so từng byte khối `cost_sp` của `xiaoyao.lua` ta với `vltk_raw/script__skill__xiaoyao.lua`: khít. Bản gốc VLTK **cũng** dùng `{{1,10},{30,4},{31,4}}` và **cũng** tự chú thích sẵn dòng `--{{1,8},{20,3},{21,3}}`. Vậy **10 đúng là con số của VLTK, nhưng ở cấp 1**.
- Nhân vật test đang để 2118 ở **cấp 20** (log ghi `slotskill=2118 lv=20 addlv=0 curlv=20` cả server lẫn client) → engine chỉ đòi **6 tầng**.
- Truy hết đường mã: cổng chặn (`KNpc.cpp:2677` server / `:2683` client) và lệnh trừ tầng (`:2699`) dùng **cùng một** đối tượng kỹ năng của cấp hiện tại; mỗi cấp là một đối tượng riêng; `KSkills.cpp:2212` xoá `m_nCostSp = 0` ngay đầu hàm nạp nên bản sao từ cấp 1 **không thể** mang số 10 sang (hỏng thì ra 0 = mất chặn, chứ không ra 10). `skills.txt` chỉ có một dòng 2118; chiêu con 2119 không khai `cost_sp`. Hai tệp `xiaoyao.lua` server/client giống hệt (`bda7f656`).

**Số 10 đến từ đâu:** (a) **tooltip của kỹ năng chưa cộng điểm** — `CoreShell.cpp:1130` lấy cấp gốc, cấp 0 thì `KSkills.cpp:2863` vẽ mô tả theo **cấp 1** nên in "10 tầng Âm Luật"; đường học chuẩn thêm kỹ năng ở cấp 0 (`hocvocong.lua:736` = `{2118,0}`), NPC test cũng vậy. (b) Trùng hợp: **trần tích Âm Luật cũng đúng bằng 10 tầng**. (c) **Ghi chú sai của chính tôi** trong nghiệm thu đợt 5 — đã sửa ở mục 15.

### 19.2 Nếu chủ vẫn muốn hạ — công cụ đã sẵn, **chưa chạy**

`ReverseTools\phai3htd_thiconghtd_data_patch10_costsp.py` — bật dòng VLTK đã chú thích sẵn `{{1,8},{20,3},{21,3}}`:

| Cấp | 1 | 10 | **20** | 30 |
|---|---|---|---|---|
| Hiện tại | 10 | 8 | **6** | 4 |
| Sau khi đổi | 8 | 5 | **3** | 3 |

- **Không phải build, không phải swap .moi** — thuần dữ liệu Lua. Nhưng sửa **cả hai** tệp server + client (đang giống hệt nhau), rồi **restart máy chủ và người chơi thoát vào lại** (đối tượng kỹ năng cache theo cấp).
- **Không đụng phái khác**: chỉ 2118 dùng bảng `sataliuxing`. Thập Bộ Nhất Sát (2129) cố định 3, Lạc Nhạn Bình Sa (2138) cố định 8, ba chiêu Vũ Hồn dùng bảng riêng.
- **Rủi ro cân bằng**: ba chiêu Tiêu Dao ăn chung kho 10 tầng (+1 mỗi lần tích). Hạ 2118 xuống 3 là **ngang Thập Bộ Nhất Sát**, mà 2118 là chiêu **lướt 280 đơn vị** → gần như lướt liên tục.
- Chạy thử: `python vhtd_data_patch10_costsp.py --kiem` (đã chạy, ra đúng bảng trên). Ghi thật: bỏ `--kiem`. Trả lại như cũ: `--hoan-tac`. Công cụ tự nạp lại bằng lua4 trước khi ghi nên không thể làm hỏng tệp.

### 19.3 ~~Một lệch thiết kế của chính VLTK~~ — **ĐÍNH CHÍNH: tôi kết luận sai**
Tôi từng viết rằng `MaxLevel` của 2118 là 20 trong khi đường cong chi phí vẽ cho 30 cấp, nên "người chơi vĩnh viễn không chạm được mức 4 tầng". **Sai.** Engine có thuộc tính `allskill_v` (số 139) từ **trang bị cộng cấp mọi kỹ năng**: `KSkillList` cộng `m_nAllSkillV` vào `AddLevel`, và `CurrentSkillLevel = SkillLevel + AddLevel`. Mọi công thức đều dùng `CurrentSkillLevel`. Kiểm lại **toàn bộ 33 kỹ năng Hoa Sơn đều có đường cong vượt `MaxLevel`** — đó là thiết kế bình thường của VLTK, phần vượt dành cho trang bị cộng cấp. Không có gì phải sửa.


## 20. ĐỢT 11 (02/09 ~20:30) — 4 mục chủ giao kèm ảnh tooltip Kiếm Tông Tổng Quyết

### 20.1 "Kiếm Tông Tổng Quyết dư thuộc tính phải không?" — **ĐÚNG, và là hồi quy do tôi gây ra**

Ảnh chủ gửi có **hai dòng "Tốc độ đánh – ngoại công: +7%" giống hệt nhau**. Gốc:
- Hàng 1349 khai **cả** `attackspeed_yan_v` (biến thể "Dương") **lẫn** `attackspeed_v` — **đúng như VLTK**.
- Nhưng engine ta ánh xạ hai cái vào **cùng một hàm xử lý** (`KNpcAttribModify.cpp:82` cho `attackspeed_yan_v` → `AttackSpeedV`; tương tự `lifemax_yan_p` → `LifeMaxP`…). VLTK có hệ âm/dương nên hai kho riêng; ta thì **cộng đôi cùng một chỉ số**.
- Đợt 6 tôi còn bỏ hậu tố "(Dương)" trong bảng mô tả (theo quyết định của chủ ở đợt g) nên hai dòng không còn phân biệt được.

**Đã vá (dữ liệu):** gỡ biến thể "Dương" ở **8 kỹ năng** mà đợt 6/đợt 9 của tôi đã thêm vào trong khi bản thường đã có sẵn — 1349 (tốc đánh), 1376 / 1381 / 1385 / 1971 (sinh lực tối đa), 1968 / 1970 (tốc đánh + tốc nội công), 1980 (tốc đánh). Từ nay mỗi chỉ số cộng **một lần**, tooltip còn một dòng.
**Không đụng** 92 / 208 Phật Tâm Từ Hữu — chiêu cổ điển vốn đã khai cả hai từ trước, sửa là đổi cân bằng cũ.

### 20.2 "Thượng Tùng Nghênh Khách lực tay chỉ 1196 trong khi kỹ năng cấp thấp hơn trên 3k"

Không phải lỗi — đây là **kỹ năng hỗ trợ**, sát thương riêng thấp theo đúng thiết kế VLTK. Bảng `physicsenhance_p` ở cấp 20:

| Kỹ năng | Cấp yêu cầu | physicsenhance_p | Lực tay ước tính (vũ khí ~1.000) |
|---|---|---|---|
| 1347 Bạch Hồng Quán Nhật | 10 | 75 % | ~1.750 |
| 1351 Kim Nhạn Hoành Không | 30 | **880 %** | ~9.800 |
| 1355 Thiên Thân Đảo Huyền | 50 | 133 % | ~2.330 |
| **1360 Thượng Tùng Nghênh Khách** | **60** | **20 %** | **~1.200** (chủ thấy 1196 ✓) |
| 1363 Thái Nhạc Tam Thanh Phong | 90 | 566 % | ~6.660 |
| 1368 Độc Cô Cửu Kiếm | 150 | 600 % | ~7.000 |

Giá trị của 1360 nằm ở chỗ khác: nó cộng **+112 %** cho Thái Nhạc Tam Thanh Phong và **+50 %** cho Độc Cô Cửu Kiếm (`addskilldamage1/2` trong bảng `cangsong_yingke`). Ô "Lực tay" chỉ hiện sát thương của **kỹ năng đang chọn ở tay trái**, nên nó không bao giờ phản ánh phần 1360 cộng cho hai chiêu kia. Muốn thấy hiệu quả thật thì so sát thương của Thái Nhạc Tam Thanh Phong khi có và không có 1360.

Số liệu này trùng VLTK từng cột. Nếu chủ muốn 1360 tự đánh mạnh hơn thì đó là đổi cân bằng, cần chủ quyết.

### 20.3 Hai mục engine đang điều tra
- **Ma Vân Kiếm Khí không nổ, không biến mất khi kích hoạt Thần Quang Toàn Nhiễu**: hướng đã lộ — cả hàm kích nổ lẫn chỗ gọi nó đều nằm trong khối **chỉ biên dịch phía máy chủ**, nên client không kích nổ bản sao khí trường của nó: khí trường client sống tiếp đủ 20 giây (không biến mất) và sự kiện nổ 1411 không bắn trên client (không có hiệu ứng). Đang xác minh và tìm neo vá chính xác.
- **Bóng mờ ngắn và giật + thiếu vòng sáng xanh dưới chân**: engine chỉ hạ được một ảnh mờ mỗi chu kỳ làm nhạt (`NowGetBlur` chỉ đúng khi bộ đếm về 0) nên vệt thưa và không đều; chiêu lướt thì hạ nhiều ảnh cùng lúc theo khoảng cách nên mượt. Vòng sáng dưới chân chưa rõ nguồn (`StateSpecialId` của 1358 bằng 0 ở **cả** ta và VLTK). Đang tìm trong pak.


## 21. ĐỢT 12 (02/09 ~21:00) — "Lực tay nội công Hoa Sơn: full kỹ năng + full mạch cũng không hơn 20k"

### 21.1 GỐC — tìm ra bằng cách mổ chính nhị phân client VLTK

Hàm hiển thị "lực tay" của client VLTK nằm ở **`0x006EC360`** (tương đương `KPlayer::GetEchoDamage` của ta). Đọc mã máy:

```
006ec3bc  lea  ecx, [ebp + 0x114]        ; mảng 5 ô kỹ năng (0x114, 0x118, 0x11c, 0x120, 0x124)
006ec3d8  mov  edx, [esp + 0x20]         ; ĐẦU VÒNG LẶP
006ec3dc  mov  edi, [edx]                ;   lấy id kỹ năng của ô này
006ec3de  test edi, edi
006ec3e4  jne  0x6ec3f8                  ;   ô trống -> dùng kỹ năng đang cầm
          ... tính sát thương cho kỹ năng đó ...
006ec890  mov  eax, 0x66666667           ; CHIA 5
006ec895  imul edi
006ec89b  sar  edx, 1
006ec8a4  add  [eax], ecx                ;   *nMin += dmgMin / 5
006ec8a6  mov  eax, 0x66666667           ;   *nMax += dmgMax / 5
006ec8c1  mov  eax, 0x66666667           ;   *nAR  += ar / 5
006ec8dd  add  eax, 1
006ec8e0  cmp  eax, 5
006ec8e7  jl   0x6ec3d8                  ; LẶP ĐỦ 5 LẦN
```

**VLTK cộng sát thương chia 5, lặp qua 5 ô kỹ năng** — tức ô "Lực tay" của VLTK là **sát thương TRUNG BÌNH của cả thanh 5 kỹ năng**, không phải sát thương của một chiêu. Engine của ta hiển thị **trọn vẹn sát thương của DUY NHẤT kỹ năng tay trái**. Hai con số **không so sánh trực tiếp được**.

`KPlayer` của ta **không có** mảng 5 ô này (chỉ có `m_nLeftSkillID` / `m_nRightSkillID`), tức đây là tính năng VLTK có mà máy chủ ta không có.

### 21.2 Những gì đã loại trừ (không phải lỗi bên ta)
- **Hệ số cộng thêm**: mổ nhị phân cho thấy VLTK áp `addskilldamage` **y hệt ta** — tra bảng ánh xạ nằm trong `KSkillList + 0xF14`, bảo trì tăng dần, **không** kiểm tra "buff đang bật", công thức `v × (100 + P) / 100`. Giả thuyết "VLTK không cộng phần này" đã **bị bác**.
- **Dữ liệu**: `skills.txt` hàng 1372 / 1376 / 1379 / 1380 / 1382 trùng **114/114 cột** với VLTK; `huashan.lua` cũng trùng, và trùng luôn bản Linux.
- **Cấp tối đa**: đã đính chính ở mục 19.3 — đường cong vượt cấp là để dành cho trang bị cộng cấp kỹ năng.

### 21.3 Ba cần gạt để đạt mục tiêu ≤ 20.000 (đây là **cân bằng**, chờ chủ quyết — tôi không tự sửa)

Bảng dưới tính theo công thức của ta: băng sát gốc × (100 + 3 buff + phần nội lực đầy) / 100.

| Cấp kỹ năng | Băng sát gốc | Hệ số | **Hiện nay** | **Dùng dòng VLTK cũ** |
|---|---|---|---|---|
| 20 | 11.433 | +280 % | 43.445 | **15.200** |
| 25 | 14.400 | +310 % | 59.040 | **20.500** |
| 30 | 21.600 | +343 % | 95.688 | 26.580 |
| 40 | 36.000 | +346 % | 160.560 | 35.680 |

1. **Bật lại dòng VLTK đã tự chú thích trong `huashan.lua`** (`{1,200},{40,8000}` thay cho `{1,160},{25,14400},{40,36000}`). Đạt mục tiêu ≤ 20k tới khoảng cấp 25. **Thuần dữ liệu, không build, không swap `.moi`**, nhưng phải sửa **cả hai** tệp server + client rồi khởi động lại máy chủ. Đây là con số chính VLTK từng dùng trước khi họ nâng lên — **tôi khuyên dùng cách này** nếu chủ muốn hạ sát thương thật.
2. **Chia hiển thị cho 5 để giống cách VLTK trình bày** — số sẽ về ~8.700. Nhưng chỉ đổi **con số hiện ra**, sát thương thật giữ nguyên, và bảng sẽ không còn phản ánh đúng sát thương. Cần sửa mã + build + swap.
3. **Bớt buff cộng phần trăm**: gỡ `addskilldamage1 → 1382` ở một hoặc hai trong ba bảng (1372 / 1376 / 1380). Lệch khỏi VLTK.

### 21.4 Cần chủ xác nhận
Trong client VLTK, khi chủ **bật cả ba khí trường** (Thanh Vân Tống Sảng, Long Huyền Kiếm Khí, Ma Vân Kiếm Khí) và **nội lực đầy**, con số lực tay có nhảy lên không, hay vẫn quanh 10-20k? Nếu vẫn quanh đó thì xác nhận VLTK đang chia 5; nếu nhảy lên gấp mấy lần thì cần đo lại.


---

## 22. ĐỢT 12 (02/09 ~21:00) — HẠ LỰC TAY NỘI CÔNG HOA SƠN: ĐÃ THI CÔNG CÁCH 1 + CÁCH 3

Chủ chốt: *"phần để phái hoa sơn nội - ngoài đúng là của nó, đường nội công của phái hoa sơn khi tăng full hết kỹ năng full mạch cũng không hơn 20k lực tay"* → sau đó *"trước mắt làm 1 - 3 trước"*.

Công cụ: `ReverseTools\phai3\vhtd_thicong\vhtd_data_patch12_luctay.py`, marker `[VHTD 02/09v]`.
**Thuần dữ liệu Lua — KHÔNG build, KHÔNG swap `.moi`.** Chỉ cần khởi động lại GameServer.

### 22.1 Vì sao bỏ cách 2

Cách 2 (chia con số hiển thị cho 5 cho giống cách VLTK trình bày) **loại trừ lẫn nhau** với cách 1 và cách 3. Cách 1 và 3 hạ **sát thương thật** để ô lực tay đọc ra khoảng 10-18k. Cách 2 chỉ hạ **con số hiển thị**, sát thương thật giữ nguyên. Áp cả ba thì ô lực tay còn khoảng 2-3k, thấp xa mục tiêu 20k của chủ. Nên hiểu *"làm 1 - 3"* là **cách 1 và cách 3**.

### 22.2 Cách 1 — trả băng gốc 1382 về dòng VLTK cũ

Trong `SKILLS.pishi_poyu.colddamage_v` (Phách Thạch Phá Ngọc, chú thích gốc ghi "neihuashan150"), đảo chỗ hai cặp dòng: chú thích dòng đang dùng, bật lại dòng **chính VLTK đã tự chú thích sẵn**.

```
truoc:  --[1]={{1,200},{40,8000},{41,8000}},
        [1]={{1,160},{25,12000*1.2},{40,24000*1.5}},
sau:    [1]={{1,200},{40,8000},{41,8000}},
        --[1]={{1,160},{25,12000*1.2},{40,24000*1.5}},
```

Băng gốc theo cấp: **11.433 → 4.000** (cấp 20) · 14.400 → 5.000 (cấp 25) · 21.600 → 6.000 (cấp 30).

**Bối cảnh quan trọng — đây cũng là sửa hồi quy của chính tôi.** Bản vá đợt 6 của tôi thay **nguyên tệp** `huashan.lua` bằng bản client VLTK, kéo băng gốc 1382 lên **9,5 lần** so với bản `D:\ServerLinux` mà máy chủ vốn chạy. Đo lại từ các bản lưu:

| Mốc | Băng gốc cấp 20 | Cấp 30 |
|---|---|---|
| `huashan.lua.truoc_vhtd_patch6_0209` (bản Linux, 03:57) | **1.200** | 3.072 |
| Sau đợt 6/7 (bản VLTK, đến 13:17) | **11.433** | 21.600 |
| Sau đợt 12 (dòng VLTK cũ) | **4.000** | 6.000 |

Con số mới nằm giữa hai bản, giữ được nguồn gốc VLTK mà không kéo phái lên 9,5 lần.

### 22.3 Cách 3 — bớt buff cộng phần trăm vào 1382

Toàn bộ chỉ có **ba nguồn thật** bơm `addskilldamage1` vào 1382 (bảng thứ tư `moyun_jianqi1` là **dữ liệu mồ côi** — đã kiểm cả `skills.txt` server lẫn client, **không hàng nào** trỏ tới nó):

| Bảng | Kỹ năng | Xử lý |
|---|---|---|
| `qingfeng_songshuang` | 1372 Thanh Vân Tống Sảng | **chú thích (bỏ)** |
| `longxuan_jianqi1` | 1376 Long Huyền Kiếm Khí | **chú thích (bỏ)** |
| `shenguang_xuanrao` | 1380 Ma Vân Kiếm Khí | **giữ lại** |

Giữ 1380 vì đây là chiêu cấp cao nhất và gắn với cơ chế kích nổ đang làm dở. `manatoskill_enhance` của 1379 Khí Quán Trường Hồng (+100% khi đầy nội lực) **giữ nguyên**.

Cách chú thích: thêm `--` ngay sau phần thụt lề của **từng dòng** trong khối. Không dùng lại kiểu nối `--` giữa dòng đã làm hỏng tệp ở đợt 7.

### 22.4 Kết quả đo bằng lua4

`MaxLevel` của cả 1372/1376/1380/1382/1383 đều là **20**, nên không có trang bị cộng cấp thì trần là dòng đầu.

| Cấp 1382 | Băng gốc | Hệ số | Lực tay |
|---|---|---|---|
| **20 (trần)** | 4.000 | 260% | **10.400** |
| 25 | 5.000 | 282% | 14.148 |
| 30 | 6.000 | 305% | 18.355 |

Cả ba mốc đều **dưới 20.000**, và mốc trần 10.400 khớp đúng con số *"hơn 10k 1 tí"* chủ đo trong client VLTK.

### 22.5 Đính chính hồ sơ đợt 9 của tôi

Hồ sơ cũ ghi **bốn** bảng bơm vào 1382 và có kể `qingyin_tiruilv1` (Vũ Hồn 1965). Đo lại: chỉ **ba** bảng thật, `moyun_jianqi1` mồ côi, và `1965` cũng **không** được hàng `skills.txt` nào dùng làm `LvlData`. Bảng lực tay ở mục 18.2 vì thế cao hơn thực tế ở hai cột cuối.

### 22.6 Một lỗi thật phát hiện kèm — CHƯA VÁ, chờ chủ quyết

Khí Quán Trường Hồng (1379) lệch giữa hai đường:
- `KPlayer.cpp:9924` (bảng **hiển thị**) chỉ cộng khi nội lực **đầy 100%**.
- `KNpc.cpp:4944` (**sát thương thật**) cộng **theo tỷ lệ** nội lực hiện có.

Hậu quả: nội lực chưa đầy thì ô lực tay hiện **thấp hơn** sát thương thật. Client VLTK gốc cộng cả hai trường **không kèm điều kiện nội lực** nào, nên lệch này là của ta. Vá được nhưng phải sửa mã + build + swap, và sẽ **nâng** con số hiển thị lúc nội lực vơi — nên để chủ quyết sau khi nghiệm thu đợt 12.

### 22.7 CHECKLIST NGHIỆM THU đợt 12

1. **Không** có `.moi` nào trong đợt này. Không swap, không build.
2. Đã ghi thẳng hai tệp, cùng md5 `e42d4f3b`:
   `bin\server\script\skill\huashan.lua` và `bin\client\script\skill\huashan.lua`.
   Bản cũ giữ ở `huashan.lua.truoc_luctay_0209`. Hoàn tác: `python vhtd_data_patch12_luctay.py --hoan-tac`.
3. **Khởi động lại GameServer** (`ChayGameServer.bat`), người chơi **thoát vào lại** (đối tượng kỹ năng cache theo cấp).
4. Nghiệm thu:
   (a) Hoa Sơn nội, đủ kỹ năng, đầy nội lực → ô lực tay khoảng **10.400**, không còn 60k.
   (b) Tooltip Thanh Vân Tống Sảng và Long Huyền Kiếm Khí **không còn** dòng hỗ trợ sát thương; Ma Vân Kiếm Khí **vẫn còn**.
   (c) Ngoại công không đổi (khoảng 10k) → hai đường cân nhau.
