> **Cập nhật 01/09 tối:** Hoa Sơn đã THI CÔNG đợt 1 — xem `BANGIAO_HOASON_THICONG_0109.md` (checklist swap, rủi ro, tool).

# BÀN GIAO 01/09/2026 — MỔ 3 PHÁI HOA SƠN · VŨ HỒN · TIÊU DAO (Linux + client VLTK) ĐỂ VIẾT VÀO JX1

> Phân tích, **chưa sửa một byte nào** trong mã nguồn hay dữ liệu chạy thật.
> Mọi tệp trích được nằm ở `D:\GAMEDEVNEW\ReverseTools\phai3\` (xem mục 1).
> Tác tử khảo sát mã JX1 đã đếm **~78 điểm khoá cứng 10 phái** (mục 7).

---

## 0. TRẢ LỜI NGẮN 5 CÂU CHỦ HỎI

| Câu hỏi | Trả lời |
|---|---|
| **Linux `D:\ServerLinux` có đủ 3 phái không?** | **KHÔNG đủ để lấy làm chuẩn.** Có `script\faction_def.lua` khai 13 phái (10 Hoa Sơn · 11 Vũ Hồn · 12 Tiêu Dao) và 3 script kỹ năng, **nhưng** `settings\skills.txt` (cả `server1` lẫn `Patch`, 1.629 dòng, id lớn nhất 1667) **chỉ có Hoa Sơn** (1347–1384). Vũ Hồn (1963–1992) và Tiêu Dao (2114–2143) **không có dòng nào**. `faction_def.lua` Linux còn trỏ Tiêu Dao vào mã cũ 1881–1911 (không tồn tại ở bất kỳ bảng nào). `faction\factionskill.txt` chỉ tới phái 10. `ranksetting.txt` Linux dừng ở 90 (thiếu 98 của Tiêu Dao). `npcs.txt` Linux 2.355 dòng, script custom của người vận hành gọi NPC 2469–2544 **vượt bảng**. ⇒ Linux đang ở trạng thái *làm dở* 2 phái mới. |
| **Client VLTK Level Up có đủ 3 phái không?** | **CÓ, 100%.** `slistcache.pak` chứa `settings\skills.txt` **1.915 dòng** (id tới 2181) đủ cả 3 phái, `settings\faction_settings.ini` 13 phái, `script\faction_def.lua` với `bFactionIsOpen = 1` cho Tiêu Dao, 3 script kỹ năng bản mới nhất, 64 bảng `npcres` có **27 động tác mới** (thuẫn/cầm), `goldequip.txt` 7.947 dòng đủ bộ trang bị, `npcs.txt` 2.645 dòng đủ NPC, `maplist.ini` đủ bản đồ 1042/1046/1057. |
| **Hệ kỹ năng chuẩn lấy ở đâu?** | Lấy **từ client VLTK** (mục 3). Riêng Hoa Sơn: JX1 **đã có sẵn** 38 dòng skills.txt + `script\skill\huashan.lua` nhưng bản `.lua` của JX1 là bản bị cắt xén (`--*` chú thích 30 thuộc tính) và 6 ô lệch với chuẩn → thay bằng bản client. |
| **Trang bị mới của Vũ Hồn / Tiêu Dao?** | Vũ Hồn dùng **thuẫn**: vũ khí cận chiến `ParticularType 7` (Đao Thuẫn) và `8` (Thuẫn Đao), ảnh `obj-shield01..07.spr`, ô vật thể rơi `objdata 480/481`. Tiêu Dao dùng **cầm**: vũ khí tầm xa `DetailType 1 / ParticularType 3` (Mộc Cầm), ảnh `obj-lyre01..05.spr`. Bộ hoàng kim: Vũ Hồn 15 bộ (JX1 **đã có** 14 bộ ở đúng dòng 7059–7338), Tiêu Dao 9 bộ (JX1 **chưa có**, dòng 7370–7539 của VLTK trùng vùng JX1 đang dùng cho Phi Phong/Vô Danh Giới Chỉ), Nghĩa Hiệp 3 bộ (7808–7867, chưa có). Chi tiết mục 4. |
| **Hình ảnh mới?** | 27 động tác nhân vật mới (`SwordShieldW*`, `ShieldSwordW*`, `GuqinW*`) + cơ chế bảng **đặc lệ (特例)** mà engine JX1 chưa đọc. Client JX1 đã có bảng `NpcRes` với 27 cột mới và pak `updatejx15/16` chứa icon/hiệu ứng, **nhưng còn thiếu 522 tệp `.spr` động tác nhân vật, 6.085 tệp `.spr` của các hàng trang bị mới, 29 ảnh vật phẩm** (mục 5). |

---

## 1. NGUỒN VÀ TỆP ĐÃ TRÍCH — `D:\GAMEDEVNEW\ReverseTools\phai3\`

| Thư mục | Nội dung |
|---|---|
| `vltk_raw\` (37 tệp) | **Byte nguyên bản** rút từ `slistcache.pak` client VLTK: `settings__skills.txt`, `settings__faction_settings.ini`, `script__faction_def.lua`, `script__skill__{huashan,wuhuntang,xiaoyao}.lua`, `script__global__skills_table.lua`, `settings__item__004__{goldequip,platinaequip,magicscript,meleeweapon,rangeweapon,rangeres,horseres,magicattrib,magicattrib_ge,mask,horse,clothes,...}.txt`, `settings__npcs.txt`, `settings__maplist.ini`, `settings__missles.txt`, `settings__ranksetting.txt`, `settings__factiontitle.txt`, `settings__magicdesc.ini`, `settings__task__150skill__factioninfo.txt`, `settings__task__faction_task_def.txt`. Thêm `settings__item__004__magicscript__serverlist_moinhat.txt` (bản mới hơn từ `serverlist.pak`). |
| `vltk_raw\` (bổ sung) | `settings__clientweaponskill__update01.txt`, `settings__item__meleeres.txt` (93 hàng, bản `slistcache`), `settings__item__004__meleeres__update01.txt`, `settings__item__goldequipres.txt` (7.947 hàng), `settings__item__004__goldequipres__update01.txt`, `settings__item__rangeres.txt`, `settings__obj__objdata.txt` (486 hàng), `script__skill__advancedskill.lua` (có `xy_adskill`), `maps__huashanpai2013.wor`. |
| `vltk_raw\npcres\` (64 tệp) | Toàn bộ bảng tài nguyên nhân vật client VLTK: 男/女主角 × {头部,发型,肩膀,躯体,左手,右手,左手武器,右手武器,马前/中/后,披风} + `信息`, `未骑马关联表`, `骑马关联表`, **`…关联表特例`**, **`部件列表特例`**, `贴图顺序表new`, `人物类型.txt`, `动作编号表.txt`, `状态图形对照表.txt`, `普通npc资源.txt`. |
| `vltk_doc\` | Bản **đọc được (UTF‑8)** của các `.lua`/`.ini` ở trên + `stringtable.txt` (bảng chuỗi giao diện `59A637DE`). Script client VLTK vốn là **UTF‑8** (khác Linux VNI/TCVN+GBK) — khi port sang JX1 phải chuyển `skill_desc` UTF‑8 → TCVN3 bằng `vn_to_octal`. |
| `phantich\` | `vltk_pak_index_all.tsv` (uid của **190.677** mục trong 38 pak VLTK, quét xong 01/09 20:21), `vltk_vitri_tai_san.txt` (từng tệp ảnh 3 phái: pak VLTK nào chứa / không ai có), `ten_tep_uid_tim_duoc.txt`, `jx1_server_pak_index.tsv`, `linux_server_maps_pak_index.tsv`. Kết quả đo: `vltk_skills_3phai.tsv` / `vltk_skills_tieudao.tsv` (dòng skills.txt đủ 114 cột), `skill_ids_can_them.txt` (61 id), `thieu_trong_pak_jx1.txt` (tệp ảnh JX1 chưa có), `npcres_new_sprites_vltk.txt` (1.008 spr động tác mới), `npcres_newrow_sprites.txt` (6.870), `npcres_changed_sprites.txt` (213), `missle_paths.txt`, `equip_paths.txt`, `npc_faction_sprites.txt`, `jx1_client_pak_index.tsv` (200.098 uid của 37 pak client JX1), `linux_skills_3phai.tsv`, `lin_skill_*.txt`, `jx1_skill_huashan.txt`. |
| `tools\` | `dec.py` (giải mã Linux), `dec_u8.py` (giải mã client UTF‑8‑trước), `dec_grep.py`, `lin_skills_dump.py`, `pak_dumpall.py` (giải nén trọn 1 pak), `scan_all.py` (khám phá tên tệp trong pak). |

Cách rút thêm tệp từ pak client: `python tools\pak_dumpall.py "<pak>" <thư mục>` rồi tra uid bằng `pakdump.name2id` (char CÓ DẤU, tên GBK giữ byte latin‑1 — bẫy đã ghi 26/08).

Bản Linux dùng làm **đối chứng script server** (không phải bảng): `server1\script\faction_def.lua`, `script\skill\{huashan,wuhuntang,xiaoyao,skillenhance}.lua`, `script\global\huashan2013\` (79 tệp, 3.706 dòng), `script\xiaoyao\npc\` (9 tệp, 455 dòng), `script\wumumenpai\` (41 tệp, 1.285 dòng — bản đồ Vũ Mục môn phái), `script\global\vuhon\` (3 tệp), `script\missions\huashanqunzhan\` (26 tệp, Hoa Sơn quần chiến), `maps\map_publish\{wumumenpai,xiaoyao}\` + `*.wor` (dữ liệu bản đồ server 1042/1046/1057).

---

## 2. ĐỊNH NGHĨA 3 PHÁI (chuẩn = `faction_settings.ini` + `faction_def.lua` client)

| | Hoa Sơn | Vũ Hồn | Tiêu Dao |
|---|---|---|---|
| Id | **10** | **11** | **12** |
| `Name` | `huashan` | `wuhun` | `xiaoyao` |
| ShowName | Hoa Sơn phái | Vũ Hồn | Phái Tiêu Dao |
| Hệ (`Series`) | 2 Thủy (`水`) | 3 Hỏa (`火`) | 4 Thổ (`土`) |
| Camp | 3 trung lập | 1 chính phái | 3 trung lập |
| Task phái / 90 / JMDZ | 3481 / 3486 / – | 4406 / 4407 / 4405 (=30·256) | 4464 / 4465 / 4463 (=40·256) |
| Task 137 (xuất sư) giá trị | 71 | 71 | 71 |
| Rank (`ranksetting`) | 89 Thái Học Các Chủ | 90 Hậu Quân | 98 Tiên Đảo Cư Sĩ |
| Kỹ năng tiến giai | 1370 Hạo Nhiên Chi Khí | 1986 Tận Trung Báo Quốc | 2132 Mai Danh Ẩn Tính |
| Bản đồ phái / toạ độ | 987 (1346,3128) | 1042 (1530,3251) | 1057 (1584,3251) |
| Trường phái (`tbSchools`) | 24 khí tông · 25 kiếm tông | 26 thuẫn pháp · 27 đao pháp | 28 kiếm pháp · 29 cầm pháp |
| Bí kíp 90 (item) | 4062 kiếm · 4063 khí | 5106 thuẫn · 5107 đao | 5231 kiếm · 5232 cầm |
| Bí kíp 150 | 4325–4327 (Lv21–23) ⚠ trùng id JX1 | (chưa thấy) | 5215 + 5216–5218 (Lv21–23) |
| Tín vật đổi phái | 4328 Hoa Sơn Kỳ Thư ⚠ trùng id JX1 | 5103 Vũ Hồn‑Tín Vật | 5230 Tín vật Tiêu Dao Phái |
| Bước nhiệm vụ 150 (`factioninfo.txt`) | 9 | 3 | 6 |
| CharClass trong skills.txt | 2 | 4 | 5 |

Tổng theo hệ sau khi thêm: Kim 2 · Mộc 2 · **Thủy 3 · Hỏa 3 · Thổ 3** — công thức `id = hệ×2 + thứ_tự` của JX1 sụp đổ (mục 7).

Task id 3481/3486/4405–4407/4463–4465: **JX1 chưa script nào dùng** (grep = 0) → dùng được ngay. Rank 82–105: JX1 `RankSetting.txt` dừng ở 81 → thêm 24 dòng (đã có trong `vltk_raw\settings__ranksetting.txt`).

`factioninfo.txt` client còn dòng **"Nguyệt Ca Đảo, FactionId 13"** (phái thứ 14, `yuegedao`) — chưa có trong `faction_settings.ini`/`faction_def.lua` của bản client này; ghi nhận, ngoài phạm vi.

---

## 3. HỆ KỸ NĂNG

### 3.1 Bảng `skills.txt` — 114 cột, **cùng khuôn** JX1 (header trùng 100%), mã hoá **TCVN3** ⇒ chép dòng nguyên byte được.

Cần thêm vào JX1 (server **và** client) **61 dòng** (`phantich\skill_ids_can_them.txt`), là bao đóng theo `ChildSkillId/CollidSkillId/FlySkillId` + id nhắc trong 3 script:

- **Vũ Hồn**: 1963–1992 (30) + 1733/1734 (kỹ năng NPC đệ tử Vũ Hồn).
- **Tiêu Dao**: 2114–2143 (30).
- Hoa Sơn 1347–1384: JX1 **đã có**; lệch chuẩn ở `ReqLevel` 1363/1364/1382/1383 (JX1 80, chuẩn 90) và `MaxLevel` 1369/1384 (JX1 26, chuẩn 27); 8 dòng tên Trung Quốc bị mã hoá hỏng (chỉ cosmetic).

Kỹ năng theo cấp (client `faction_def.lua`):

| Cấp | Vũ Hồn | Tiêu Dao |
|---|---|---|
| 10 | 1972 Đoạt Trại Dương Kỳ · 1974 Trường Anh Huy Xích | 2114 Ngô Câu Sương Tuyết (kiếm) · 2136 Tùy Ý Khúc (cầm) |
| 20 | 1964 Vũ Hồn Thuẫn Pháp · 1975 Vũ Hồn Đao Pháp | 2115 Tiêu Dao Kiếm Pháp · 2137 Tiêu Dao Cầm Pháp |
| 30 | 1976 Công Trung Bỉnh Tính (nguồn **Nộ**) | 2116 Di Cung Hoán Vũ (nguồn **Âm Luật**) |
| 40 | 1965 Thỉnh Anh Đề Nhuệ Lữ · 1977 Hám Sơn Kích | 2118 Táp Đạp Lưu Tinh · 2138 Lạc Nhạn Bình Sa |
| 50 | 1963 Triều Thiên Khuyết · 1979 Nộ Phách Thiên Nhai | 2120 Ỷ Kiếm Phất Thiên · 2140 Quảng Lăng Tán |
| 60 | 1980 Cô Tương Bách Chiến Hàm | 2121 Ngân Yên Bạch Mã |
| 70 | 1982 Vũ Mục Di Thư (trấn phái) · 1971 Trí Dũng Siêu Luân · 1981 Cơ Xan Lỗ Nhục | 2123 Thiên Lý Độc Hành (trấn phái) · 2122 Hiệp Cốt Nhu Tình (xuất sư) |
| 90 | 1967 Trấn Biên Thùy (thuẫn) · 1983 Khát Ẩm Hung Nô Huyết (đao) | 2124 Kiếm Quyết Phù Vân · 2141 Cao Sơn Lưu Thủy |
| 120 | 1984 Trung Vu Lưu Phong | 2127 Ăn Uống No Say |
| 150 | 1969 Huy Sư Diệt Lỗ (thuẫn) · 1985 Trừ Gian Diệt Nịnh (đao) | 2129 Thập Bộ Nhất Sát · 2142 Mai Hoa Tam Lộng |
| tiến giai | 1986 Tận Trung Báo Quốc | 2132 Mai Danh Ẩn Tính (+2133 Phất Y, 2134 Phất Y_Thuẫn — `advancedskill.lua`) |
| phụ trợ/con | 1966, 1968, 1970, 1973, 1978, 1987 Đấu Dũng Đương Tiên, 1988, **1989 Nộ**, 1990, 1991, 1992 Phí Huyết Nhẫn | **2117 Âm Luật**, 2119, 2125, 2126, 2128 Túy Tửu, 2130, 2131, **2135 Công kích vật lý gần (đánh thường cầm, `EqtLimit 103`, `WeaponSkill 1`)**, 2139 Tiêu Dao Vũ, 2143 Dương Quan Tam Điệp |

Hoa Sơn: 10:{1347,1372} 20:{1349,1374} 30:{1350,1375} 40:{1351,1376} 50:{1354,1378} 60:{1355,1379} 70:{1358,1360,1380} 90:{1364 kiếm,1382 khí} 120:{1365} 150:{1369 kiếm,1384 khí}.

### 3.2 Script cấp độ (`LvlSetScript`) — lấy bản client

| Tệp | Dòng | Khối `SKILLS` | So với Linux | So với JX1 |
|---|---|---|---|---|
| `script\skill\huashan.lua` | 831 | 34 | Linux cũ hơn (thiếu `addcolddamage_v`, `anti_hitrecover`, số khác ~50 dòng) | JX1 là bản **bị cắt** (`--*` 30 thuộc tính, đổi hằng) → **thay** |
| `script\skill\wuhuntang.lua` | 765 | 25 | 29 dòng khác | JX1 không có |
| `script\skill\xiaoyao.lua` | 620 | 19 | 235 dòng khác (Linux còn mã 1881+) | JX1 không có |

Kỹ năng tiến giai Tiêu Dao 2132–2134 nằm ở `script\skill\advancedskill.lua` khối **`xy_adskill`** (client VLTK **có**: `autoreplyskill` → 2133 Phất Y 30 s, `autocastskill` → 2134 mỗi 1 s, `staticmagicshield_p` 100→1050 % nội lực, `fastwalkrun_p` 5→10 s); JX1 `advancedskill.lua` (343 dòng) chỉ có `sl/tw/gb_adskill` → thay bằng bản client (`vltk_doc\script__skill__advancedskill.lua`).

`skills_table.lua` client có `add_hs/add_wh/add_xy(lvl)` (duyệt `tbFacDef.tbSkills`, cấp ≤ 70) — cần `Include faction_def.lua`; JX1 `skills_table.lua` (1.320 dòng) chưa có 3 hàm này lẫn `tbFacDef`.

### 3.3 Thuộc tính (magic attribute) 3 script dùng mà **engine JX1 chưa có** — đối chiếu `Core\Src\KMagicAttrib.h` (309 tên) và chuỗi trong `game_y.exe`

| Tên | Dùng ở | Ý nghĩa (từ chú thích gốc) | Linux 2021 có? |
|---|---|---|---|
| `special_point_base` | 1976 Công Trung Bỉnh Tính, 2116 Di Cung Hoán Vũ, `zhizunyinjian` (ấn chí tôn) | {id khoá, -1 nếu là buff/0 nếu chủ động, **trần số tầng**}: Nộ tối đa 5, Âm Luật tối đa 10 (+2 khi có ấn chí tôn) | không |
| `special_point_add` | 1989 Nộ (+2 tầng / lần), 2117 Âm Luật (+1), 1990 (đã bỏ: `max*256+add`) | cộng tầng vào bộ đếm của kỹ năng khoá | không |
| `cost_sp` | Hám Sơn Kích 2 tầng, Vũ Mục Di Thư 1, `qingyin_tiruilv1` 4; Táp Đạp Lưu Tinh 10→4 (cấp 30), Lạc Nhạn Bình Sa, Thập Bộ Nhất Sát | **điều kiện thi triển** = có đủ tầng, thi triển thì trừ | không |
| `autocastskill` | 1976 → tự thi triển 1989 mỗi **3 s** (54 khung), 2116 → 2117 mỗi **1 s** | {id*256+cấp, 1=tính CD/-1 không, khung*256+xác suất%} — thụ động tự phóng kỹ năng con theo chu kỳ | có |
| `hidebodyunlock` · `invincibility` · `forbit_attack` (+`frozen_action` đã có) | buff 2130 Thập Bộ Nhất Sát | ẩn thân/bất tử/cấm đánh trong 10 khung, cấm đánh 7 | có (hidebody: không) |
| `lock_life` | 1982 Vũ Mục Di Thư | {giá trị khoá 100→3400, thời gian 18→72 khung, chế độ 1 = không thấp hơn} | không |
| `cast_when_buff_removed` | 1982 | khi buff hết → phóng 1991 với cấp của 1984 | không |
| `reset_bufftime` | 1984 Trung Vu Lưu Phong | đặt lại thời gian buff 1988 | không |
| `resume_life_p` | 1991 (buff hồi máu Trung Vu Lưu Phong) | hồi 30→71 % mỗi khung trong 1 khung | không |
| `lifereplenish_dec_p` | 1988 (hậu quả Hám Sơn Kích) | giảm hiệu suất hồi sinh lực 1→30 % trong 18→162 khung | không |
| `unravel_effect` | 2121 Ngân Yên Bạch Mã | "Tá lực" 7→300 điểm | không |
| `lightingdamage_p` · `addlightingmagic_p` | Tùy Ý Khúc, Lạc Nhạn Bình Sa, Quảng Lăng Tán, Cầm Pháp, Ngân Yên Bạch Mã | sát thương Lôi % / nội công Lôi % | không |
| `addskillexp1` · `skill_skillexp_v` | các kỹ năng 90 (Hoa Sơn, Vũ Hồn, Tiêu Dao) | độ thuần thục kỹ năng (JX1 có kỹ năng 90 hệ thuần thục — kiểm lại xem JX1 gọi tên khác) | có |

Mô tả cho client (`magicdesc.ini`, 5 dòng có chữ): `lightingdamage_p=Sát thương Lôi: #d1-%` · `addlightingmagic_p=Sát thương Lôi hệ Nội công: #d1+%` · `special_point_base=Giới hạn tồn của kỹ năng #l1 <color=orange>#d3+ tầng<color>` · `lifereplenish_dec_p=Hiệu suất hồi sinh lực giảm: #d1+%` · `unravel_effect=Tá lực: #d1+ điểm`.

⚠ `KMagicDesc.cpp` JX1 đang 311 tên (id tới 310), `magic_normal_end` = 306 — thêm tên mới **phải nối đuôi** và ánh xạ chỉ số bằng **TÊN**, không chép số (bẫy đã ghi ở kinh mạch 27/08). Kỹ năng "Nộ/Âm Luật" hiển thị bằng buff 1989/2117 có `StateSpecialId` → cần dòng bảng trạng thái (3.5).

### 3.4 Ràng buộc vũ khí (`EqtLimit`, `KSkills.cpp:253-280`)
-2 mọi vũ khí · -1 tay không · **7 Đao Thuẫn · 8 Thuẫn Đao** (Vũ Hồn) · **103 = 100 + 3 Mộc Cầm** (Tiêu Dao cầm) · 0 kiếm (Tiêu Dao kiếm, Hoa Sơn). JX1 cộng `MAX_MELEEWEAPON_PARTICULARTYPE_NUM` cho vũ khí tầm xa — phải bằng 100 mới khớp 103 (kiểm hằng số trong `GameDataDef.h`).

### 3.5 Đạn, hiệu ứng trạng thái, âm thanh
- `missles.txt`: 26 dòng JX1 chưa có — **523,525–529,531** (Vũ Hồn), **575–580** (Vũ Hồn), **637–649** (Tiêu Dao); Hoa Sơn 412–428 đã có. JX1 `missles.txt` id tới 441 → **kiểm cách nạp (theo hàng hay theo cột `MissleId`)** trước khi chép.
- `StateSpecialId` cần: 234 Vũ Mục Di Thư (ảo ảnh 280 khung 8 hướng), 235 Tận Trung Báo Quốc, 239/240 debuff giảm công/giảm tốc đánh, 242 Đãn Chích Trì Thương, 243 Tiêu Dao Vũ; Hoa Sơn 166/171 (JX1 đã có). JX1 `state_magic_table_name.txt` 225 dòng (Status1..223) → nối tới **243** (ảnh `\spr\skill\wuhuntang\武穆遗志虚影出现.spr`, `精忠报国.spr`, `减速状态.spr`, `\spr\skill\xiaoyao\啖炙持觞.spr`, `逍遥羽.spr`).
- Âm thanh: `sound_k001..k010.wav` đủ; Tiêu Dao `SF_XY_*.wav` đủ trong pak JX1; **thiếu duy nhất** `\sound\skill\刀剑刺中声.wav`.

---

## 4. VŨ KHÍ VÀ TRANG BỊ MỚI

### 4.1 Loại vũ khí
| | Vũ Hồn | Tiêu Dao |
|---|---|---|
| Bảng | `meleeweapon.txt` **`DetailType 0 / ParticularType 7`** "Tinh Chế Đao Thuẫn" 1–10 và **`/ 8`** "Tinh Chế Thuẫn Đao" 1–10 (20 dòng, JX1 chưa có; Linux cũng chưa có — Linux chỉ tới P5) | `rangeweapon.txt` **`DetailType 1 / ParticularType 3`** "Mộc Cầm" 1–10 (10 dòng, JX1 chưa có) |
| Ảnh vật phẩm | `\spr\item\equip\closeweapon\obj-shield01..06yh.spr`, `\spr\item\equip\weapon\obj-shield07.spr` | `obj-lyre01..04.spr`, `\spr\item\equip\weapon\obj-lyre05.spr` |
| Vật thể rơi (`objdata.txt` cột "对应物件索引") | **480 "Đao Thuẫn"** (`obj_wq_010.spr`) / 481 — VLTK `objdata.txt` **486 hàng** (đã trích), JX1 464, Linux 479 → nối 465–486 | 9 / 10 (dùng lại kiếm/đao) |
| Tài nguyên tra `MeleeRes.txt`/`RangeRes.txt` (`KItemChangeRes::GetWeaponRes`, hàng = `parti*10+level+2`) | VLTK `settings\item\meleeres.txt` **93 hàng**: hàng 72–81 → bộ phận **270** (武魂新手武器刀), 82–91 → **271** (…盾). JX1 `MeleeRes.txt` 220 hàng, hàng 72–91 đang là mục vũ khí hoàng kim cũ (腾龙…) không còn ai tra theo `parti*10+level` → **thay 20 hàng đó** theo VLTK | `rangeres.txt` VLTK hàng 32–41 → **278** (逍遥琴3) |
| Bộ phận nhân vật (`男主角右手武器.txt` VLTK 310 hàng, JX1 281) | hàng **260–271**: 武魂武器刀1/盾1, 刀2/盾2, 月阙刀/盾, 日曜刀/盾, 新手刀/盾, 英豪刀/盾 (**JX1 đã có đúng 260–271**, tên Việt "Vũ Hồn‑Đao 1"…); **305/306** 义侠之武魂盾/刀 (JX1 chưa) | **272–275** 逍遥剑1–4, **276–279** 逍遥琴1–4 (**JX1 đã có**, tên hỏng mã); **307/308** 义侠之逍遥琴/剑 (chưa) |
| `goldequipres.txt` (vũ khí hoàng kim → hàng bộ phận) | VLTK: Thanh Câu Trí Công Thuẫn 7064→**263**, Phấn Chiến Đao 7074→**262**, Nghĩa Hiệp thuẫn/đao 7833/7843→**307/308**. ⚠ **JX1 `goldequipres.txt` (7.379 hàng) gán các món này vào 237–248 = vũ khí "Thiên Mệnh…" — SAI**, phải chép lại 7059–7338 từ VLTK | Tiêu Dao cầm 7375/7395→**279**, Nghĩa Hiệp cầm 7853→**309** |
| Kỹ năng đánh thường | `clientweaponskill.txt` VLTK (`update01.pak`) **giống hệt JX1** (32 loại, chưa có thuẫn/cầm) → ánh xạ đánh thường cho thuẫn nằm trong engine/bảng khác, chưa xác định | 2135 "Công kích vật lý gần" (`WeaponSkill 1`, `EqtLimit 103`) |

### 4.2 Bộ hoàng kim (`goldequip.txt`, id = **số hàng**; VLTK 7.947 hàng, JX1 7.400)
JX1 và VLTK **lệch hàng từ dòng 160** (296 hàng khác nhau trong 7.400) nhưng vùng phái **trùng hàng**:

| Bộ | Hàng VLTK | JX1 |
|---|---|---|
| Hoa Sơn khí tông + kiếm tông: Thanh Câu → Đằng Long (10 cấp × 2 tông × 10 món) | 4634–4833 | **đã có, cùng hàng** (chỉ khác cột 57/58 dung luyện JX1 tự thêm, và ngoặc kép cột mô tả) |
| Hoa Sơn Xích Lân Kiêm Gia / Tinh Sương / Nguyệt Khuyết / Diệu Dương / Anh Hào / Thiên Mệnh | 5637–5656, 5610–5629, 5901–5920, 6193–6212, 6514–6533, 7039–7058 | đã có, cùng hàng |
| **Vũ Hồn** Thanh Câu → Anh Hào (14 bộ thuẫn/đao, 280 hàng) | 7059–7338 | **đã có, cùng hàng** (2 ô chỉ số thuộc tính lệch: hàng 7324 cột 50 VLTK 5577 vs JX1 4913) |
| **Tiêu Dao** cầm: Bạch Hổ, Xích Lân, Minh Phượng, Tinh Sương, Nguyệt Khuyết, Diệu Dương, Anh Hào, Đằng Long (8 bộ × 20 = 160 hàng + 20) | 7370–7539 | **CHƯA CÓ** — vùng 7363–7399 JX1 đang chứa Phi Phong Kình Lôi/Chí Tôn/Tiềm Long/Siêu Thần, Long Đảm, Thổ Hổ Ấn, Vô Danh Giới Chỉ ⇒ **phải nối sau 7399, id đổi**, mọi script gọi id Tiêu Dao phải ánh xạ theo TÊN |
| Nghĩa Hiệp: Hoa Sơn Kiếm 7808–7827 · Vũ Hồn Thuẫn/Đao 7828–7847 · Tiêu Dao Cầm 7848–7867 | 60 hàng | chưa có |

Chỉ số `magicattrib_ge` mà 241 hàng thiếu dùng: 230 chỉ số, **222 trùng hệt** JX1, 8 lệch (963 tham số 1080 vs 372; 1302/1303/1305/2215/2216/2220 chỉ khác chữ mô tả; **5837 thuộc tính 308 vs 305**) → ánh xạ bằng nội dung, không chép số.

`platinaequip.txt` VLTK và JX1 **trùng 100%** (5.338 hàng) — không có bộ bạch kim riêng cho 3 phái.

### 4.3 Vật phẩm `magicscript.txt` (khoá `6,1,P`) — JX1 P lớn nhất 4937, chưa có P ≥ 5000
- Vũ Hồn: **5096–5107** (nhiệm vụ + tín vật 5103 + bí kíp 90: 5106/5107).
- Tiêu Dao: **5215–5232** (bí kíp 150: 5215; Lv21–23: 5216–5218; tín vật 5230; bí kíp 90: 5231/5232), 5280 rương trang sức, 30620/30658/30671.
- Hoa Sơn: 4062/4063 JX1 **có nhưng tên hỏng mã** (chép lại từ VLTK); 30339 Lệnh bài Hoa Sơn, 30341 Tín Vật Phong Thanh Dương chưa có; ⚠ **4325–4328 trùng id** JX1 đang dùng cho "Nhược Thủy Văn Tinh" (Phi Phong) → cấp id mới cho Mật tịch 21–23 và **Hoa Sơn Kỳ Thư** (script đổi phái `daiyitoushi` đòi 4328).
- Ảnh vật phẩm: `wuhunxinwu.spr`, `xiaoyaoxinwu.spr`, `xiaoyaodan.spr`, `obj_item_lection.spr`, `21miji..23miji.spr` **đã có** trong pak JX1 (`updatejx15/16`).

---

## 5. HÌNH ẢNH — ĐÃ ĐO TRONG 37 PAK CLIENT JX1 (`phantich\thieu_trong_pak_jx1.txt`)

Client JX1 (`bin\client\data`) có `updatejx14/15/16.pak` **khác cỡ** bản VLTK (1,90/1,60/0,86 GB so với 1,72/1,46/1,41 GB) — cùng dòng nhưng không cùng bản.

| Nhóm | Cần | JX1 có | VLTK có | **Chỉ VLTK có (= phải chép)** | Không pak nào có (bảng VLTK trỏ tệp không tồn tại → không phải thiếu) |
|---|---|---|---|---|---|
| Icon kỹ năng + hiệu ứng thi triển + âm thanh (3 phái) | 78 | 78 | 78 | 0 | 0 |
| Đạn/hiệu ứng trúng (`missles.txt`) | 70 | 69 | 69 | 0 | 1 (`刀剑刺中声.wav`) |
| Ảnh vật phẩm bộ hoàng kim 3 phái | 188 | 159 | 187 | **28** (bộ Nghĩa Hiệp `*119/121-124/145-147.spr`, `obj-lyre05`, `obj-shield07`, `obj-sword55/56`) | 1 (`horse044.spr`) |
| Spr **động tác mới** nhân vật nam (27 cột) | 434 | 319 | 386 | **67** | 48 |
| Spr động tác mới nhân vật nữ | 574 | 167 | 196 | **29** | 378 (bảng nữ trỏ `FM_RW_160_MG*.spr` không tồn tại) |
| Spr của **hàng bảng mới** (VLTK 310/195 hàng vs JX1 281/165) | 6.870 | 785 | 6.575 | **5.790** | 295 |
| Spr ô **đổi** ở hàng cũ | 213 | 119 | 119 | 0 | 94 |
| Spr NPC 3 phái (`passerby407/410/430a/432`, `boss169/173/202/203/115`) | 40 | 40 | 40 | 0 | 0 |

⇒ **Tổng phải chép từ VLTK: 5.914 tệp, tất cả nằm trong `updatejx16.pak` của VLTK** (danh sách từng tệp + pak: `phantich\vltk_vitri_tai_san.txt`). Đối chứng dương tính đã chạy (4/4 tệp có thật). Đường dẫn = `\spr\npcres\man|woman\<tên>.spr` (ghép trong `KNpcResNode::ComposePathAndName`). Quét 38 pak VLTK xong 20:21 (190.677 uid, `phantich\vltk_pak_index_all.tsv`).

Bảng `NpcRes` client JX1 (`bin\client\settings\NpcRes\`) **đã có** 27 cột động tác mới (`action_file_name.txt` 76 dòng) nhưng còn **thiếu 29 hàng vũ khí và 30 hàng thân/đầu/tóc** so với VLTK, và **chưa có 3 bảng đặc lệ** (`…关联表特例`, `部件列表特例`) — engine JX1 (`npc_res_kind_file_name.txt` 19 cột) chưa có cột `PartFileNameSpecial / WeaponActionTab1Special / WeaponActionTab2Special` (VLTK 22 cột). Cơ chế đặc lệ: với vũ khí hàng 260–271/305/306 (thuẫn) và 276–279/307 (cầm), khi thân người thuộc dải `156:160,190:191` (thuẫn) hoặc `161:164,192:193` (cầm) thì dùng bảng liên kết đặc lệ và `部件列表特例` (chỉ vẽ đầu/tóc + trang bị, **không vẽ vũ khí/ngựa/phi phong** — thân đã vẽ gộp thuẫn/cầm). `贴图顺序表new.txt` cho vũ khí 276–279 khi cưỡi ngựa.

---

## 6. NPC · BẢN ĐỒ · NHIỆM VỤ · GIA NHẬP

- **`npcs.txt`**: id = số hàng. JX1 2.187 hàng (hàng **2036–2184 trống "0"**, 286 hàng còn lại khác tên do dịch khác). VLTK 2.645 hàng: Hoa Sơn 2087–2119 rơi đúng vùng trống của JX1; Vũ Hồn (2469 Nhạc Lôi, 2478–2481 đệ tử, 2481 Y Sư …) và Tiêu Dao (2609 Văn Bán Sơn, 2628 …) nằm **≥ 2187** ⇒ nối 458 hàng VLTK vào JX1 **giữ nguyên id** (server + client). Kèm 14 kiểu tài nguyên mới trong `人物类型.txt`/`普通npc资源.txt` (ảnh đã có đủ 40/40).
- ⚠ Script custom Linux (`global\vuhon\npc_vuhon.lua`, `xiaoyao\npc\npc_xiaoyao.lua`, `wumumenpai\npc_wumumenpai.lua`) gọi id NPC 2469–2544 theo **một bảng khác** (Linux `npcs.txt` chỉ 2.355 hàng; ở VLTK 2524–2544 là "Kim Trừng Ác…") → **ánh xạ lại theo TÊN** khi port, không dùng số.
- **Bản đồ** (đã đo bằng uid, `KSubWorld.cpp:1884/1995` ghép `\maps\` + mục MapList; region `v_%03d\%03d_Region_S.dat` server, lùi về `_Region_C.dat`):
  - **987 Hoa Sơn 2013** (`西北南区\华山派2013`, rect 74–99 × 87–105 = 494 ô): client JX1 **đã có** `.wor` + 376 `_region_c.dat` trong `updatejx14.pak`; **server JX1 có 0** (không có `987_srv.fp`); Linux `server1\pak\maps.pak` có `.wor` + **308 `_region_s.dat`** → đóng vào pak server JX1 (như `maps_banghuichengbao.pak` 22/08).
  - **1042–1046 Vũ Mục môn phái** (`map_publish\wumumenpai`, rect 91–114 × 91–104): client JX1 **đã có đủ 286/286** `_region_c.dat` + `.wor` trong `updatejx15.pak` **theo đường dẫn VLTK `\map_publish\…` (không có `\maps\`)**; server: Linux tệp rời 286 `_region_c.dat` (`D:\ServerLinux\server1\maps\map_publish\wumumenpai\`) → đóng pak server.
  - **1057 Tiêu Dao** (`map_publish\xiaoyao`, rect 92–119 × 92–112): client JX1 **đã có đủ 385/385** trong `updatejx16.pak`; server: Linux tệp rời 385.
  - Vì JX1 luôn ghép `\maps\` mà `g_GetPackPath` chạy `RemoveAllPointPath` (`KFilePath.cpp:34-84`, xử lý `\..\`), có thể khai `1042=..\map_publish\wumumenpai` trong `MapList.ini` để trỏ đúng uid VLTK **không cần băm lại tên**; hoặc đóng lại pak dưới `\maps\map_publish\…`. Cả `MapList.ini` server lẫn client JX1 đều chưa có 1042–1046/1057 (VLTK: 1043–1045 cũng trỏ `wumumenpai`).
- **Gia nhập phái** (Linux, bản custom, dùng làm mẫu): `huashan2013\hoason.lua` (hệ Thủy, cấp ≥10, `SetFaction("huashan")`, `SetLastFactionNumber(10)`, `add_hs`), `wumumenpai\vuhonduonghauquan.lua` / `global\vuhon\hauquan_vuhon.lua` (`SetSeries(3)` rồi `SetFaction("wuhun")`, rank 90), `xiaoyao\npc\detu_xiaoyao.lua` (hệ Thổ, `SetFaction("xiaoyao")`, rank 98, `SetTask(4464,10*256)`, `add_xy`) — ⚠ bản Tiêu Dao Linux còn `AddMagic(1881…)` mã cũ và gọi `add_xy` **không tồn tại** trong `skills_table.lua` Linux ⇒ sai; lấy `add_xy` từ client. Rank Linux gán lung tung (82/90/97/98/89) — dùng 89/90/98 theo `faction_def`.
- **Học kỹ năng**: Linux `task\newtask\education\knowmagic.lua` chỉ có mục Hoa Sơn (`Uworld1000_huashan` → kiếm tông/khí tông) — Vũ Hồn/Tiêu Dao **không có** ở cả hai nguồn (server VLTK không có trong tay) ⇒ tự viết theo mẫu Hoa Sơn + `AddFacSkill(nFacId, nLv)` của `faction_def.lua`. Nhiệm vụ 150 (`task\150skilltask\`) Linux có thư mục từng phái cũ + `yuegedao`, **không có** huashan/wuhun/xiaoyao; JX1 không có hệ này.
- **Đổi phái** ("Đại nghệ đầu sư"): đã mổ 01/09 (`BANGIAO_DOITEN_DOIPHAI_LINUX_0109.md`) — Hoa Sơn cần 4328 + 5 chuyển sinh; Vũ Hồn 5103; Tiêu Dao 5230.
- NPC/quái Vũ Mục môn phái (`wumumenpai`, 41 tệp) và Hoa Sơn 2013 (79 tệp) + Hoa Sơn quần chiến (26 tệp) là khối script port sau, theo quy trình `HUONGDAN_DICHNGUOC_TINHNANG_LINUX.md` (chạy `recon_tinhnang.py` từng thư mục).

---

## 7. ENGINE JX1 PHẢI SỬA (tác tử đo 78 điểm; nêu điểm chết)

**A. Số phái** (`KFaction.h:15-16` `FACTIONS_PRR_SERIES 2`, `MAX_FACTION = 2×series_num`):
1. `KFaction.cpp:37-57` — id phái = "ô trống đầu tiên của hệ", KHÔNG phải số mục ini. Lên 13 phái chia 5 hệ không đều ⇒ **id đảo lộn, nhân vật cũ đổi phái ngầm** (DB `BaseInfo.nSect`). Viết lại `nArrayPos = i`, `m_nSeries` đọc từ key `Series` (chuỗi GBK `金木水火土` — mục mới phải dùng đúng byte).
2. `KPlayerFaction.cpp:80` chặn `nFactionID ∉ [hệ×2, hệ×2+2)` — cửa vào duy nhất của `AddFaction` ⇒ phái mới **im lặng không vào được**.
3. `KLadder.h:6` `MAX_FAC` (thiếu ngoặc) + `KLadder.cpp` 5 chỗ; `KProtocol.h:2222-2229` `TGAME_STAT_DATA` 5 mảng `[11]` ↔ `Goddess2\src\DBBackup.h:38-42` (kiểm `sizeof` ở `KLadder.cpp:19`) ⇒ **Core và Goddess đổi cùng lúc**; lỗi sẵn có `DBBackup.cpp:498/512/527` (`nSect<=10` tràn `[11]`, bỏ sót phái 0).
4. Mảng tên/chặn cứng: `KNpc.cpp:11774-11800` (`FactionName[10]`, `<= 9`), `ScriptFuns.cpp:14335-14355` (`s_szFaction[11]` đã có "Hoa Sơn", `nNo < 11`), `UiTongJX2.cpp:120-125`, `KPlayerBot.cpp:111-150, 1287, 2578-2612, 2737, 5892` (bảng NPC/vũ khí/giới tính theo phái), `KSimCity.cpp:1092`.
5. DB: `BYTE nSect/nFirstSect` chứa 12 OK; gói tin BYTE OK; `Lua GetLastFactionNumber` JX1 = cur (Linux = last) — bẫy đã ghi 01/09.

**B. Vũ khí**: `GameDataDef.h:411-413` `MAX_MELEE_WEAPON 6`, `HAND_PARTICULAR 6`, `MAX_RANGE_WEAPON 3`; `KNpcAttribModify.cpp:377-380` `WEAPON_ALL/RANGE_ALL/MELEE_ALL/NONE = 6/7/8/9` **nằm trong dữ liệu item** ⇒ thêm particular 7/8 phải dời hằng đặc biệt (đề nghị ≥100) + migrate `requirement` trong bảng item; `m_CurrentMeleeEnhance[MAX_MELEE_WEAPON+1]` tràn nếu không nới. `KSkills.cpp:261-276` `HAND_PARTICULAR`. `MeleeRes.txt` hàng 72–91 xung đột (mục 4.1).

**C. Tài nguyên nhân vật**: `CEquipStyleTable` cấp phát động (`KNpcResNode.h:135-152`) — số loại vũ khí/động tác do bảng quyết định (tốt); **thiếu** cơ chế đặc lệ (3 cột `*Special` của `人物类型.txt`, dải `a:b,c:d`) → thêm vào `KNpcResNode` nạp + `KNpcRes::Draw` chọn bảng theo thân người.

**D. Thuộc tính kỹ năng**: 13 tên mới (mục 3.3) → `KMagicAttrib.h` enum + `KMagicDesc.cpp` bảng tên + handler trong `KNpcAttribModify.cpp` (+ hệ **bộ đếm tầng** theo khoá kỹ năng: base/add/cost, tự phóng kỹ năng theo chu kỳ, khoá sinh lực, phóng kỹ năng khi buff hết, đặt lại thời gian buff, ẩn thân/bất tử/cấm đánh).

**E. Client UI**: `UiSkillsNew.cpp:57-60,546,764-787` đọc `Ui\Ui3\UiSkillNew.ini` mục `[MainN]`/`[MainN1024]`/`[Skill_N_i]`/`RemainPoint_N` — có 0..9, cần **10..12** + 3 ảnh khung `khung_*.spr`. `UiSkillNew.ini` là tệp **riêng của JX1** (chỉ có trong `sprgame/vlngaothe` pak), client VLTK không có; cửa sổ kỹ năng VLTK = `1024.pak` uid `279CE281` (khung `\Spr\Ui3\技能\技能.spr` + 2 nút Chiến đấu/Sinh hoạt, trang kỹ năng nằm ở tệp khác chưa dò được tên) → **không dùng lại nguyên xi, phải tự vẽ khung cho 3 phái**. `1024.pak` uid `29AC0C15` = bảng xếp hạng client có mục "Thập đại cao thủ" Hoa Sơn/Vũ Hồn/Tiêu Dao (ID ladder 15/16/17) → mở rộng `KLadder` enum + `UiRankData`; `UiTeamManager2.cpp:82-122` icon phái trong tổ đội: thư mục `\spr\Ui4\主界面\组队预览\` — VLTK có **`icon_zd_wht.spr` (Vũ Hồn)** và **`icon_zd_xy.spr` (Tiêu Dao)**, và **client JX1 đã có sẵn cả hai** (`updatejx15.pak`/`updatejx16.pak`) + `icon_zd_hsp.spr` Hoa Sơn → chỉ thêm 2 `case`; tên phái trên đầu nhân vật `KNpc.cpp`. Ảnh trang kỹ năng của VLTK (`\Spr\Ui\技能\技能面板改－战斗技能分页.spr`, `…－一页/二页.spr`) có trong danh sách tên khám phá nhưng không nằm trong pak nào đang có; toàn bộ 21.759 tên tệp khám phá được: `phantich\vltk_ten_tep_kham_pha.txt`.

**F. Dữ liệu**: `FactionInfo.ini` `[10][11][12]`; `skills.txt` +61; `missles.txt` +26; `magicscript.txt` +31 (+ sửa 4062/4063, đổi id 4325–4328); `goldequip.txt` +241 (nối đuôi, đổi id) + `goldequipres.txt` + `magicattrib_ge` ánh xạ; `meleeweapon.txt` +20, `rangeweapon.txt` +10, `MeleeRes/RangeRes/objdata` +; `npcs.txt` +458 hàng & lấp 2036–2184; 64 bảng `NpcRes` + 3 bảng đặc lệ; bảng trạng thái tới Status243; `RankSetting.txt` +24; `MagicDesc.ini` +5; `ClientWeaponSkill.txt` (+thuẫn, +cầm); `MapList.ini` +5 và dữ liệu map; script (`faction_def.lua`, `skills_table.lua`, 3 script kỹ năng, NPC gia nhập/học, spawn NPC).

---

## 8. ĐỀ NGHỊ THI CÔNG THEO ĐỢT

| Đợt | Việc | Build? |
|---|---|---|
| 0 | Chốt với chủ 5 câu ở mục 9 | – |
| 1 | Engine 13 phái: A1–A5 + F ini; bộ test admin "vào phái 10/11/12" | Core (server+client) + Goddess |
| 2 | **Hoa Sơn trọn**: thay `huashan.lua`, sửa 6 ô skills.txt, `add_hs`, NPC 2087–2119 + script gia nhập/học, map 987 kiểm, item 4062/4063 + đổi id 4325–4328, bảng trạng thái, UI khung kỹ năng 10 | chỉ Lua/data (+client ini) |
| 3 | Vũ khí thuẫn/cầm: B + C (đặc lệ) + bảng NpcRes/MeleeRes/RangeRes/objdata + 522+6.085 spr từ pak VLTK | Core client |
| 4 | Thuộc tính mới D (bộ đếm Nộ/Âm Luật, autocast, lock_life…) | Core server+client |
| 5 | Vũ Hồn: 32 dòng skills + missles + wuhuntang.lua + NPC/map 1042 + item 5096–5107 + bộ Nghĩa Hiệp + UI 11 | data/Lua |
| 6 | Tiêu Dao: 30 dòng skills + missles + xiaoyao.lua + advancedskill + NPC/map 1057 + item 5215–5232 + 9 bộ cầm (nối đuôi goldequip) + UI 12 | data/Lua |
| 7 | Nhiệm vụ phái (Linux custom 79+41+9 tệp), đổi phái, 150 skill, bot 3 phái (`KPlayerBot` bảng NPC/vũ khí) | Lua + Core |

---

## 9. CHỜ CHỦ QUYẾT

1. **Id bộ hoàng kim Tiêu Dao/Nghĩa Hiệp** phải khác VLTK (nối sau 7399) — chấp nhận, hay dời Phi Phong/Vô Danh Giới Chỉ (7363–7399) sang chỗ khác để khớp VLTK 100%?
2. **Id vật phẩm Hoa Sơn 4325–4328** đang bị Phi Phong dùng — cấp id mới cho Hoa Sơn (đề nghị) hay đổi Phi Phong?
3. **Hằng vũ khí đặc biệt 6/7/8/9** (`WEAPON_ALL…`) dời sang ≥100 = phải nắn `requirement` trong bảng item hiện có — đồng ý?
4. Nguồn **nhiệm vụ Vũ Hồn/Tiêu Dao**: chỉ có bản custom của người vận hành Linux (không phải VNG). Port nguyên bản đó, hay chỉ làm gia nhập + học kỹ năng + đổi phái trước?
5. UI bảng kỹ năng: giữ khuôn `UiSkillNew.ini` của JX1 (phải vẽ 3 khung mới) hay dựng lại cửa sổ cây kỹ năng của VLTK?

---

## 10. BẪY VÀ GHI CHÚ ĐÃ TRẢ GIÁ TRONG PHIÊN

- Script client VLTK là **UTF‑8**; giải bằng `dec2` ra mojibake ("Mỏằ—i") → dùng `tools\dec_u8.py`.
- `faction_def.lua` Linux và client **khác mã kỹ năng Tiêu Dao** (1881+ vs 2114+) — chỉ tin client.
- `goldequip` cột 5 "对应物件索引" là **vật thể rơi trên đất** (`objdata.txt`), không phải hàng bảng vũ khí nhân vật; hàng vũ khí nhân vật lấy qua `goldequipres.txt` (`人物部件编号`).
- Bảng `NpcRes` JX1 tên tiếng Việt (TCVN3) còn VLTK tên Trung (GBK) → so **theo hàng**, đừng so theo tên.
- Linux `skillenhance.lua` (1.545 dòng, kỹ năng 150 cường hoá của 3 phái) **không tệp nào Include** — mồ côi, chưa cần port.
- `strings` không có trên máy — trích chuỗi nhị phân bằng Python (đã làm cho `jx_linux_y`, `game_y.exe`, `engine.dll`).
- Bash heredoc nuốt `\\` → mọi script Python có backslash phải ghi bằng Write rồi chạy (bẫy 17/08).
