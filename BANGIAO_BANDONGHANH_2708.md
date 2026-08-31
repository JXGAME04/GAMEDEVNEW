# BÀN GIAO — THI CÔNG BẠN ĐỒNG HÀNH (JX1)

> Bắt đầu 27/08/2026 sau khi chủ game chốt: **"đúng làm theo bản gốc bạn cứ làm"** (7 câu hỏi
> mục 8 của `PHANTICH_BANDONGHANH_2708.md` đều chốt THEO BẢN GỐC).
> **CẬP NHẬT 27/08 tối:** đã swap + khởi động lại 2 lần + **bộ test tự động PASS 17/17**
> (mục 8). G3/G5 đã thi công (b06→b08). G4 client đã thi công (mục 9) — build đợt 3,
> **chưa nghiệm thu bằng mắt trong game**.

---

## 0. TRẠNG THÁI 7 GIAI ĐOẠN

| GĐ | Việc | Trạng thái |
|---|---|---|
| G1 | Dữ liệu tĩnh: 5 bản đồ + 15 bảng + 75 script kỹ năng + 5 ini UI + 24 ảnh | ✅ **XONG, đã kiểm** |
| G2 | Engine server: module KPlayerPartner + 51 hàm Lua + AI + hôn mê + nhịp | ✅ **CHẠY THẬT, test tự động 17/17 PASS** (mục 8) |
| G3 | Túi đồng hành (room 14) + lưu item | ✅ b06 (14 mối KItemList/GameDataDef) — server nhận đồ pos_partnerbag |
| G4 | Client: giao thức + 4 cửa sổ + phím tắt | ✅ **THI CÔNG XONG** (b09/b10, mục 9) — chờ nghiệm thu bằng mắt |
| G5 | Port 98 tệp script gameplay + item wiring + hook giết quái + driver tế đàn | ✅ b07/b08 (mục 8.2) |
| G6 | Hệ nhiệm vụ ngẫu nhiên TASKSYS | 🟡 **compat theo đúng bản VN gốc** (VNG TẮT hệ này cho region vn — mục 11); TASKSYS đầy đủ = giai đoạn 2 |

Nhị phân mới đặt CẠNH bản chạy (chưa đè):

| Tệp (đợt 3 — **ĐANG CHẠY THẬT từ 27/08 22:45**) | Cỡ | md5(8) |
|---|---|---|
| `bin\server\CoreServer.dll` (= `.moi_2708_bdh_g4`) | 18.166.272 | `e6ddb0d3` |
| `bin\client\CoreClient.dll` (= `.moi_2708_bdh_g4`) | 2.408.960 | `8058c929` |
| `bin\client\Game.exe` (= `.moi_2708_bdh_g4`) | 1.318.912 | `f9d81e06` |

Bản trước đợt G4 giữ ở `.truoc_bdh_g4`; dấu vết chuỗi đã kiểm: CoreServer chứa
`partner_talk.lua`, Game.exe chứa tên ini GBK 同伴属性 + `partnerbar`.

> 🔴 **PHẢI SWAP CẢ BA CÙNG LÚC** rồi khởi động lại: `MAX_TASK` 4600→5200 đổi layout
> `KPlayer` dùng chung (đúng tiền lệ đợt kinh mạch). Swap bằng RENAME khi server tắt.
> 🔴 **CẤM chạy xen kẽ binary cũ/mới trên cùng roledb** (binary cũ nuốt task id ≥ 4600 khi lưu).

Bộ công cụ: `D:\GAMEDEVNEW\ReverseTools\bandonghanh\` (b01→b05, chạy lại được, có sao lưu
`.truoc_bdh_2708`).

---

## 1. G1 — DỮ LIỆU ĐÃ VÀO CHỖ (bộ vá b01/b02/b03)

### 1.1 Bản đồ (b01_rut_bando.py) — rút từ pak bằng bộ đọc ĐÚNG 16 byte/entry
| Map | Nguồn | Region (có vật cản 2048B) | .wor | 24.jpg |
|---|---|---|---|---|
| 512 Vân Trung Đạo | `E:\jx1m_cdn\data\image2.pak` | 385 (348) | 117 B | 177 KB |
| 513 Bách Hoa Cốc | image2.pak | 224 (184) | 117 B | 125 KB |
| 514 Sơn Tặc Động | image2.pak | 378 (324) | 111 B | 540 KB |
| 515 Tử Hà Lĩnh | image2.pak | 359 (335) | 123 B | 108 KB |
| 539 TCM Cấm Địa | `client\data\update03.pak` | 499 (371) | 111 B | 65 KB |

Ghi vào: client `maps\...\v_YYY\XXX_Region_C.dat`; server cùng cây, đổi tên `_S`. `.wor`+`.jpg`
đặt CẠNH thư mục map (đúng khuôn `KSubWorld.cpp:1847/2028` `%s.wor`). rect trong .wor khớp
lưới region đã rút. MapList/WorldSet/RevivePos **không phải sửa** (JX1 đã khai sẵn từ trước;
World491–494=512–515, World518=539; RevivePos bản gốc cũng không có mục riêng).

### 1.2 Bảng + script (b03_chep_bang_script.py) — 107 tệp
- Server: `settings\partner\` (18 tệp) + `settings\task\partner\` (3) + `partner_task_def.txt`
  + `script\partner\` (4 hook) + `script\skill\partner\` (75 tệp — **cây `D:\ServerLinux` RỖNG**,
  nguồn thật `J:\jx-thiendieu-x64`, đã kiểm **75/75 trùng byte** với pak client VLTK).
- Client: 6 bảng client đọc (partner_bag.ini, aptitude_mode, feature, allproblem, index_taskid,
  reward_allprize) — đúng bộ mà client VLTK chứa trong pak.
- `skills.txt` JX1 **đã trỏ sẵn** `\script\skill\partner\...` (cột LvlSetScript/LevelUpScript).

### 1.3 Giao diện (b02_rut_anh_ini.py)
- 5 ini `同伴*.ini` chép loose vào `client\Ui\Ui3\` (bản Patch ui3_1024, quickbar đã Việt hoá).
- **24 ảnh** rút từ image2.pak → `client\spr\UI3\同伴界面\...` + icon `同伴任务系统\同伴教育.spr`
  (3 ảnh còn lại có sẵn trong update03.pak). 24/24 magic `SPR\0` chuẩn.

### 1.4 🔴 HAI BẪY CÔNG CỤ ĐÃ BẮT TRONG PHIÊN (đã sửa trong b01/b02)
1. **Index pak là 16 byte/entry** (`XPackIndexInfo{uId,uOffset,lSize,lCompressSizeFlag}`,
   XPackFile.cpp:26-34). Mọi bộ dò trước đọc bước 12 byte → chỉ khớp ~1/4 entry ⇒ từng kết luận
   sai "14 ảnh + 4 bản đồ không tồn tại". Chuẩn đúng = `pakdump.entries`.
2. **Cờ nén 0x20** ("Load Pak VNG", XPackFile.cpp:258) = UCL nrv2b như 0x01 — mask `&0x0F`
   nuốt mất, 2 spr từng bị ghi ra còn nén.

---

## 2. G2 — ENGINE SERVER (đã build)

### 2.1 Tệp MỚI (thay trọn stub 2003)
- `Sources\Core\Src\KPlayerPartner.h` — hằng số + sơ đồ task-value + 11 struct bảng + class
  `KPartnerSys` + móc (`KPartner_Breathe`/`Partner_ProcessAI`/`Partner_OnNpcDeath`).
- `Sources\Core\Src\KPlayerPartner.cpp` (~1.500 dòng, toàn bộ trong `#ifdef _SERVER`) —
  nạp 11 bảng, sinh chỉ số, AI 4 tính cách, gọi/thu/hôn mê/theo chủ qua map, thân mật,
  sự kiện thoại, exp/lên cấp/đổi thời kỳ, 4 loại kỹ năng, **51 hàm Lua**, log
  `Logs\KSG_PartnerLog_YYYYMMDD.txt`.

### 2.2 Sơ đồ lưu (KHÔNG đổi định dạng blob roledb)
- Toàn bộ trạng thái ghi vào **task value 4600–5099**: khối chung 4600–4619 (cur/callout/
  baglevel/lastcall/switch) + 3 khối con 160 ô tại 4620/4780/4940 (offset trong
  `KPlayerPartner.h`: GenInfo 9 số, GenTime, RandSeed, level, exp, emotion, punish, tên 16B,
  attrib init+inc ×10000, 30 ô kỹ năng pack `id*1000+level`, 81 biến task/con tại +77).
- `MAX_TASK` 4600→**5200** (`KPlayerTask.h:23`) — sửa luôn **lỗi đang chạy**: task 5100
  (`npc_chuyensinh.lua:37,45`) trước giờ bị `SetSaveVal` nuốt im lặng.
- Trần **3 con** đúng binary gốc (`PARTNER_MAX_COUNT`); UI 5 thẻ vẫn vẽ được.

### 2.3 Mối nối engine (b04_daynoi_engine.py — 10 mối, sao lưu `.truoc_bdh_2708`)
| Tệp | Mối |
|---|---|
| `KNpc.h` | +`m_nPartnerOwner`/`m_nPartnerNo` (cạnh `m_nTmpCamp`, chỉ `_SERVER`) |
| `KPlayerTask.h` | MAX_TASK 5200 (MAX_TEMP_TASK giữ 4600 — tiết kiệm 6 MB RAM) |
| `KPlayer.h` | include + `KPartnerSys m_cPartner;` (`#ifdef _SERVER`, cạnh `m_cMeridian`) |
| `KPlayerSet.cpp` | `m_cPartner.Init(i)` trong khối init `_SERVER` |
| `KNpcAI.cpp` | nhánh `kind_partner` → `Partner_ProcessAI` (TRƯỚC nhánh vận tiêu, giữ điều tiết `m_NextAITime`) |
| `KNpc.cpp` OnDeath | nhánh `else {//Not Finish}` của `kind_partner` → `Partner_OnNpcDeath` (hôn mê 300s) |
| `KNpc.cpp:10193` | vá bug sẵn có: `ChangeWorld` ghi `Player[0].m_nPrePayMoney` vô điều kiện → bọc `IsPlayer()` |
| `CoreServerShell.cpp` | `KPartner_Breathe()` cạnh `PB_Breathe()` (18 nhịp/s, tự hạ 2 lần/s) |

### 2.4 Đăng ký Lua (b05_dangky_lua.py)
- 46 extern + đăng ký mới chèn **trước `#else`** cuối khối `_SERVER` của `GameScriptFuns[]`.
- 2 stub cũ `PARTNER_GetCurPartner/GetSettingIdx` (KJx2WarInfra.cpp:1028-1038) → trỏ bản thật
  (`...2`).
- `GetPartnerBagLevel/SetPartnerBagLevel`: bản stub vùng chung GIỮ NGUYÊN cho client; server
  đăng ký ĐÈ ở cuối (Lua lấy bản đăng ký sau cùng).
- `IncludeLib("PARTNER")` giữ noop.lua — hàm C là global mọi state, đúng thiết kế.

### 2.5 Kết quả build (MSBuild VS2022 Community, cây `D:\GAMEDEVNEW`)
```
Core.vcxproj  "Server Release|x64"   EXIT=0  -> CoreServer.dll 18.164.736 B (48 nhãn PARTNER_*)
Core.vcxproj  "Client Release|Win32" EXIT=0  -> CoreClient.dll  2.406.912 B (0 nhãn - đúng, server-only)
S3Client.vcxproj "Release|Win32"     EXIT=0  -> Game.exe        1.299.968 B
```
PostBuild của vcxproj chỉ chép về `D:\GAMEDEVNEW\bin\` (KHÔNG phải junction) — **server chạy
thật ở E:\ không bị đụng** trong lúc build.

### 2.6 Bốn GIẢ ĐỊNH có chủ đích (binary Linux strip — ghi để nghiệm thu soi)
1. **`level_exp.txt` = ngưỡng exp CỘNG DỒN** để lên cấp kế (lv99=lv100 chặn trần khớp cách đọc
   này). Nếu test thấy lên cấp quá nhanh/chậm → đổi sang "chi phí từng cấp" chỉ 1 hàm `AddExp`.
2. Công thức chỉ số: `coef = uniform[apt_min, apt_max]` (roll bằng LCG từ RandSeed) rồi
   `init/inc = min + coef×(max−min)`; giá trị tại cấp L = `init + inc×(L−1)`. Đúng dạng bảng,
   sai số làm tròn có thể khác bản gốc.
3. Ánh xạ chỉ số→NPC: strength→`m_PhysicsDamage[min=max]`, hitrate→`m_AttackRating`,
   defence→`m_Defend`, speed→`m_Run/WalkSpeed`, life→`m_LifeMax`; **luck chưa áp** (chưa có
   trường NPC tương ứng — binary có khoá `lucky_v_partner`, xử ở G5 qua skill state nếu cần).
4. Hôn mê: sau hoạt ảnh chết server GỠ npc (client giữ xác theo 12 câu lệnh gác `kind_partner`
   sẵn có); phạt 300s + cooldown 30s + hồi 2%/5s + emotion giảm 1/2h đúng `partner_setting.ini`.

---

## 3. CÒN LẠI — G3 (túi room 14)

14 điểm sửa đã tra sẵn tệp:dòng (báo cáo agent lưu trữ, tóm tắt):
`GameDataDef.h:314` +`room_partnerbag` trước `room_num`; `:333` +`pos_partnerbag` trước
`pos_num` (=25); +hằng W/H (6×10 theo `partner_bag.ini`); `KItemList.cpp` Init:1040 ·
AddKIL:318 · Remove:724 · eContainer:851/3869 · ExchangeItem:3371+2465 · tìm chỗ:539 ·
FindNumberInAll:960 (⚠ antihack) · ClearAll:4178; `KCore.cpp:892` PositionToRoom;
`GameDataDef.h:749` +`UOC_PARTNER_BAG` cuối enum; whitelist kéo-thả
`KProtocolProcess.cpp:5588+`. `ITEM_SYNC.m_btPlace` là BYTE — pos 25 vừa, KHÔNG đổi giao thức.
Kích hoạt ô theo `GetPartnerBagLevel` (bậc = cấp pet/10, `partner_skillbook_h.lua:54`).

## 4. CÒN LẠI — G4 (client)

Toàn bộ công thức nằm trong báo cáo agent client (transcript 27/08), tóm tắt:
- 4 lớp `UiPartner{Attr,Skill,Bag,QuickBar}` theo khuôn `UiTrembleItem`/`UiDiceItem`
  (KWndImage + LoadScheme KIniFile + Wnd_AddWindow + 4 chỗ đăng ký UiBase/UiShell +
  v33-style vá vcxproj). Túi dùng `KWndObjectMatrix` (HUnits/VUnits), kỹ năng dùng
  `KWndObjectBox` mảng; tooltip tự chạy qua `HoldObject`.
- Dữ liệu hiển thị đọc **`GDI_TASK_SAVE_VALUE`** (id≥256 tự sync qua kênh
  `PLAYER_SCRIPTACTION_SYNC UI_TASKVALUE` — `KPlayerTask.cpp:97-126`, client cũ bỏ qua êm)
  ⇒ phần LỚN panel không cần giao thức mới.
- Thao tác cần c2s mới: khe trống **c2s 173+**, **s2c 215+**; đuôi bảng `g_nProtocolSize`:
  client sau dòng 168, server sau dòng 283 (`KProtocol.cpp`); GOI_*/GDCNI_* chỉ thêm CUỐI enum.
- Phím tắt: thêm tên vào `l_WindowList[]` CUỐI (`ShortcutKey.cpp:141-175`) + case trong
  `LuaOpenWindow` + `\Ui\autoexec.lua` (⚠ B đã bị túi chiếm — rà lại trước khi gán).
- Thoại sự kiện hiện đầu NPC: đường `KPlayerChat::NpcChat` → `SetChatInfo` có sẵn 100%.

## 5. CÒN LẠI — G5 (script gameplay)

- Port ~60 tệp theo bộ sinh (khuôn v09 Viêm Đế): education/master/rewind/reward/train/trap/
  item + `partner_head.lua` (bảng NPC spawn — NPC template ĐÃ CÓ SẴN trong npcs.txt, trùng tên
  100%).
- Sửa **17 lỗi làm chết chức năng** của mã gốc (danh sách mục 9 PHANTICH; giữ nguyên phần mã
  chết/di sản).
- Bẫy riêng đã tra: `GetGameTime` trong `\script\partner\` trả FRAME (thêm `\\script\\partner\\`
  + `\\script\\task\\partner\\` vào `szJx2[]` `KSortScript.cpp:115-131` để trả GIÂY như Linux);
  `AddMapTrap` nhận PIXEL (exit.txt để nguyên, enter.txt ×32); `partner_addtrainnpc` cần
  `IncludeLib("FILE")` (`GetIniFileData` = hàm Lua trong `scriptjx2\lib\file.lua:29`);
  `Ladder_NewLadder` CÓ SẴN (id 10188 hợp lệ, top 10, âm điểm = xếp theo thời gian);
  `gb_Get/SetTask` CÓ SẴN + bền qua restart (`jx2league.txt`).
- `AddPlayerEvent/RemovePlayerEvent` **JX1 KHÔNG có** → làm bằng hook giết quái cạnh
  `KNpc.cpp:1680` (khuôn `danhquai.lua`) + đếm bằng task value, gọi `reward_killfinish` từ Lua.
  Bảng quái/số con của 381 cuộn nằm trong cột content `index_taskid.txt` — bước port sẽ dựng
  bảng `taskid → (map, npc template, số con)` từ `D:\ServerLinux\server1\settings\task\` (soi
  thêm thư mục event của Linux khi làm).
- Driver tế đàn giờ chẵn (viết mới ~1 tệp, khuôn `ydbz_driver`).
- `GetItemMagicLevel` chưa có — đổi script sang `GetItemMagicAttrib` (không cần C).
- Dịch: bảng thoại talk 4×50 câu + 118 câu hỏi + partner_event 26 sự kiện đang là tiếng Trung
  (bản gốc VNG cũng vậy) — port nguyên trạng trước, dịch TCVN3 là bước đánh bóng riêng.

## 6. NGHIỆM THU G2 (khi được phép restart)

1. Swap 3 nhị phân (RENAME) + khởi động lại server, phát client mới.
2. Console phải thấy `[Partner] Da nap bang cau hinh dong hanh (event=26)` ở lần Breathe đầu.
3. Test bằng script GM có sẵn: chép `functions_partner.lua` (đã port G1, chỉnh menu) hoặc gọi
   thẳng qua lệnh bài admin:
   - `PARTNER_AddFightPartner(5,0,1,5,5,5,5,5,5)` → trả 1; `PARTNER_Count()` → 1.
   - `PARTNER_CallOutCurPartner(1)` → NPC "DongHanh1" (template 1060 Kim Phong tiểu đồng)
     xuất hiện cạnh chủ, đi theo, quái đánh nó, nó đánh quái khi chủ bật chiến đấu.
   - Đánh chết pet → thông báo hôn mê (partner_action.lua), 300s không gọi lại được.
   - Qua map → pet tự theo (Breathe ChangeWorld). Relog → pet còn nguyên (task value).
   - `Logs\KSG_PartnerLog_*.txt` có dòng AddFightPartner.
4. Soát chuyển sinh: `GetTask(5100)` giờ lưu được (lỗi cũ tự khỏi).

## 7. TỆP ĐÃ ĐỤNG (nguồn `D:\GAMEDEVNEW\Sources`)

| Tệp | Kiểu | Sao lưu |
|---|---|---|
| `Core\Src\KPlayerPartner.h/.cpp` | THAY TRỌN (stub 2003 → module thật) | git |
| `Core\Src\KNpc.h` `KNpc.cpp` `KNpcAI.cpp` `KPlayer.h` `KPlayerSet.cpp` `KPlayerTask.h` `CoreServerShell.cpp` `ScriptFuns.cpp` | vá b04/b05 | `.truoc_bdh_2708` |
| Cây server E: `settings\partner\*` `settings\task\partner*` `script\partner\*` `script\skill\partner\*` `maps\...` | thêm mới (b01/b03) | — |
| Cây client E: `maps\...` `Ui\Ui3\同伴*.ini` `spr\UI3\同伴界面\*` `settings\partner\*` `settings\task\partner\*` | thêm mới (b01/b02/b03) | — |
| 3 nhị phân `.moi_2708_bdh` | đặt cạnh | — |


---

## 8. NGHIỆM THU TỰ ĐỘNG 27/08 TỐI — 17/17 PASS

Bộ test: `bin\server\script\partner\partner_test_bdh.lua` (timerserver RunTime gọi mỗi phút,
bot KPlayer thật `PB_AddBot(1,1)`, log `bin\server\bdh_test.log`). Chạy 2 đợt (2 lần restart).

### 8.1 Kết quả

| Nhóm | Kiểm | Kết quả |
|---|---|---|
| API | AddFightPartner(5,0,1,5×6) → GetCurPartner/CallOut/state | ✅ PASS |
| Tên | SetName/GetName "BDHTest" (4 int ↔ 16 byte) | ✅ PASS |
| Sinh chỉ số | GetSettingIdx=1060 (Kim Phong kỳ 1), GetSeries=0 | ✅ PASS |
| Cấp | AddExp 5000 → **lv 7** (bảng level_exp CỘNG DỒN — giả định 2.6.1 **ĐÃ XÁC NHẬN**; đợt 2 thêm 5000 nữa → tổng 10000 → **lv 10** đúng ngưỡng 8900) | ✅ PASS |
| TaskValue riêng | Set/GetTaskValue(2)=7 | ✅ PASS |
| Kỹ năng | AddSkill(594 lv3) → GetSkillInfo | ✅ PASS |
| Thuộc tính | GetAttribs: life=18311 str=250.6 hit=550.9 def=147.4 spd=10.26 luck=3.86 (đúng dải attrib_range hệ Kim apt 5) | ✅ PASS |
| Thân mật | Emotion khởi tạo = 50 | ✅ PASS |
| **LƯU BỀN** | PB_SaveAll → ClearBot → relog: Count/Level/Name/Skill/Series **giữ nguyên qua roledb**; đợt 2 còn giữ qua RESTART server | ✅ PASS 5/5 ×2 |
| Bản đồ | `MAPCHECK 512=491 513=492 514=493 515=494 539=518` — 5 map partner NẠP thật | ✅ |

FAIL duy nhất đợt 2: `AddExp5000->Level lv 7 -> 10 (mong 7)` — **cosmetic**: test chạy lại trên
pet cũ đã có 5000 exp, cộng thêm 5000 → lv10 đúng toán; không phải lỗi.

Fix phát sinh khi test: pet triệu hồi ra bị máu template (~1%) → `CallOut` đặt
`m_CurrentLife = m_LifeMax` sau ApplyToNpc (đã vào build đợt 2).

### 8.2 G5 đã vào (b07_port_gameplay.py + b08_noi_item_script.py)

- 435 item map tên Linux→JX1 (lệch −1 dải 446..902; riêng sách 704: 1063→**1064**); 98 tệp
  `script\task\partner\**` + npclevelscript + item script + `event_killnpc.txt` (server+client,
  marker `.bdh_nanned` chống nắn kép); syncheck 100 tệp = 0 lỗi.
- `magicscript.txt` cột 9: 833..837→resist, 838..847→ultimate, 848..883+900+1064→learnt,
  446..826→reward_partner, 828→reward_education (CẢ server + client).
- JX1 không có `AddPlayerEvent` → `bdh_compat.lua` + `bdh_killhook.lua` móc
  `danhquai.lua OnDeathMonsterDaTau` (KNpc.cpp:1680 gọi cho MỌI npc chết bởi người chơi):
  GetTask(1237)=event 6..386 → đếm task 1236 → đủ KillCount gọi `reward_killfinish`.
- Tế đàn: key bảng gb_* đổi GBK→ASCII `changgemen_jitan_bdh`; driver
  `train\bdh_jitan_driver.lua` (giờ chẵn bật cờ 1..5) do timerserver gọi.

---

## 9. G4 CLIENT — ĐÃ THI CÔNG (b09_core_protocol.py + b10a/b10b)

### 9.1 Giao thức (b09 — 20 mối, marker `[BDH-G4]`, sao lưu `.truoc_bdh_g4`)

- `Headers\KProtocolDef.h`: **`c2s_partnerop`** thêm CUỐI enum (trước `_c2s_begin_relay=250`).
- `Headers\KPartnerProtocol.h` (MỚI): `PARTNER_OP_DATA {BYTE op; int nParam; char szName[32]}`
  + enum op 1..8 (CALLOUT/SELECT/TALK/ATTACK/FOLLOW/RENAME/FORGETSKILL/DELETE) — tách tệp riêng
  y khuôn `KDiceProtocol.h` (2 bản KProtocol.h đụng guard), include từ CẢ HAI bản KProtocol.h.
- `KProtocol.cpp`: bảng size + `SendClientPartnerOp`; `KProtocolProcess.cpp`: đăng ký +
  handler `c2sPartnerOp` (validate như c2sDiceItem; RENAME lọc ký tự điều khiển;
  TALK/FORGET/DELETE → `Partner_RunTalkScript` = mở `partner_talk.lua main` đúng khuôn
  CallFunction không phá ActionScriptID).
- `CoreShell.h/.cpp`: `GOI_PARTNER_OP` (cuối enum GOI) + `GDI_ITEM_IN_PARTNER_BAG` (cuối GDI,
  liệt kê đồ pos_partnerbag) + 2 case `UOC_PARTNER_BAG→pos_partnerbag` trong GOI_SWITCH_OBJECT.
- `KPlayerPartner.h/.cpp`: `PTG_FIGHTMODE` (4606; 0=chủ động đánh, 1=chỉ theo — AI đọc trong
  Partner_ProcessAI) + `Partner_RunTalkScript`.
- 🔴 Bẫy đã dính & sửa: patch đầu **cắt cụt KProtocolDef.h** (quên nối `s[j:]`) → C1070
  mismatched #if/#endif; khôi phục từ `.truoc_bdh_g4` và vá lại (b09 đã sửa).

### 9.2 Bốn cửa sổ (b10a sinh 10 tệp `S3Client\Ui\UiCase\UiPartner*`)

| Cửa sổ | Ini (đã Việt hóa, `\Ui\Ui3\`) | Nội dung |
|---|---|---|
| KUiPartnerAttr | 同伴属性.ini | 5 thẻ dọc; tên (KWndEdit32, nút 选定 = lưu tên + chọn đương nhiệm); 24 trường: exp tổng, 5 kháng (=cấp skill 549..553), cấp, tính cách (Dũng mãnh/Vệ chủ/Lưu manh/Nhu nhược), 6 thuộc tính init+inc×(lv−1) + tiềm năng (=inc), ngộ tính (TB 6 tư chất), thân mật /100 |
| KUiPartnerSkill | 同伴技能.ini | 5 hàng × (3 icon bẩm sinh SK_GIVEN + 6 icon tự học SK_LEARNT) qua HoldObject(CGOG_SKILL_FIGHT) |
| KUiPartnerBag | 同伴背包.ini | 6×10 UOC_PARTNER_BAG; kéo-thả GOI_SWITCH_OBJECT; chuột phải trả về hành trang; dòng "Cấp túi: %d" |
| KUiPartnerBar | 同伴快捷栏.ini | 9 nút: Chara/Skill/Item toggle 3 cửa sổ; Talk/Attack/Follow/Call/Select gửi GOI_PARTNER_OP; **tự hiện khi login nếu PTG_VERSION>0** (móc task value notify) |

Dữ liệu attr/skill đọc từ **bản sao task value phía client** (`GDI_TASK_SAVE_VALUE`) — login đã
tự đồng bộ từng ô qua kênh UI_TASKVALUE (`LoadPlayerTaskList → SetSaveVal → SyncTaskValueToClient`).

### 9.3 Đăng ký (b10b) + phím

- `GameSpaceChangedNotify.cpp`: nhánh UOC_PARTNER_BAG (item notify) + `UiPartner_OnTaskValueChanged`
  trong case GDCNI_TASK_VALUE_UPDATE. `UiShell.cpp`: đóng 4 cửa sổ khi logout.
- `ShortcutKey.cpp`: l_WindowList 31..39 + case (31-34 toggle cửa sổ, 35-39 = UiPartner_HotKey
  gửi GOI). `S3Client.vcxproj(.filters)`: 5 cặp tệp.
- `bin\client\Ui\autoexec.lua` (data, `.truoc_bdh_g4`): **Y** attr · **I** skill · **U** túi ·
  **P** thanh nhanh · **L** thoại · **B** gọi/thu · **F** chủ động đánh · **G** chỉ theo ·
  **N** đổi con — đúng tip trong ini gốc. Nếu vướng phím đơn khi chơi → xóa dòng tương ứng.

### 9.4 Điểm CHƯA làm trong UI (ghi rõ, không tự chế)

- `m_editEnergy`/`m_editFire` (元气/火候): bản gốc mobile chưa rõ nguồn dữ liệu → hiện `-`.
- Ô túi khóa theo cấp không tô màu tím (server vẫn CHẶN đặt — Partner_BagCellActive); Remind
  checkbox chưa nối; 3 cột icon cạnh thẻ ở trang ATTR bỏ trống; lưới tự học hiện 6/16 ô đầu;
  exp hiện tổng thô (chưa hiện ngưỡng cấp kế — bảng level_exp chỉ ở server).
- Nút 遗忘 (quên kỹ năng) + 解散 (giải tán) → mở menu đối thoại partner_talk (bản gốc xử qua
  thoại), không xóa trực tiếp.

---

## 10. SAU NGHIỆM THU — GỠ HOOK TEST

Các dòng nhãn **"GO SAU KHI NGHIEM THU"**:
1. `bin\server\script\timerserver.lua`: 2 dòng Include + 2 dòng call (BDH_TestTick giữ hay gỡ
   tùy — `bdh_jitan_driver` PHẢI GIỮ, đó là driver tế đàn thật).
2. `bin\server\script\partner\partner_test_bdh.lua`: xóa cả tệp (hoặc giữ làm bộ thử hồi quy).
3. `danhquai.lua` hook `BDH_OnKillNpc` **PHẢI GIỮ** (đường sự kiện giết quái thật).

---

## 11. G6 TASKSYS — PHÁT HIỆN QUYẾT ĐỊNH + COMPAT

### 11.1 Bản VN gốc TẮT hệ nhiệm vụ ngẫu nhiên đồng hành

`D:\ServerLinux\server1\script	ask\systemandom.lua:36-40` (trigger 12 phút/lần):

```lua
local szRegion = GetProductRegion();
if (szRegion == "vn") then	--越南没有随机任务 ("Việt Nam không có nhiệm vụ ngẫu nhiên")
    do return end
end;
```

⇒ Trên server VN gốc, biến 1301–1309 mãi = 0, nhánh "đồng hành giao nhiệm vụ" trong
`partner_talk.lua` không bao giờ kích hoạt. **Duy nhất** tệp partner phụ thuộc TASKSYS là
`partner_talk.lua` (nút Đối thoại — dòng 42 gọi `GetTaskStatus(TaskName(...))` vô điều kiện,
thiếu hàm là chết thoại).

### 11.2 Compat đã đặt (`bin\server\script	ask\system	ask_main.lua` — tệp MỚI)

- ID_RANDOMTASK 1301 … ID_RANDOMTASKNUM 1309 (đúng số `taskandom	ask_head.lua:50-66`).
- `get/setRandomTaskState` THẬT (đọc/ghi task 1303); các hàm khung TASKSYS
  (TaskName/TaskNo/GetTaskStatus/SetTaskStatus/CloseTask/ApplyTask/GetTaskText/
  checkTaskBookEnable/checkRandomTaskEnable/initRandomTask/_confirmRandomTask) trả
  "không có nhiệm vụ" — đúng trạng thái vĩnh viễn của region vn; TaskTip→Msg2Player;
  SyncTaskValue→noop (JX1 SetTask tự đồng bộ id ≥ 256); WriteTaskLog→noop.
- Kết quả: **thoại tâm sự theo độ thân chạy trọn** — test tự động nạp `partner_talk.lua`
  trong ngữ cảnh bot, triệu pet rồi chạy `main()` end-to-end KHÔNG lỗi (b1–b20 + dostring,
  xem bdh_test.log); 5 bảng talk 51 hàng/bảng nạp đủ, Describe gửi được.
- 🔴 Bẫy đã dính khi test: `call(f,{},"x")` trả **0 giá trị** khi hàm không `return` gì ⇒
  `local ok = call(...)` nhận nil DÙ THÀNH CÔNG — 3 lượt "FAIL Talk.main" đầu là báo giả;
  xác minh lỗi thật phải dùng `_ERRORMESSAGE` + `dostring`. (`call(...,"x")` cũng NUỐT lỗi
  không ghi ScriptError.log.)
- Nội dung 4 bảng thoại tâm sự (`talk\man_m` v.v.) còn **tiếng Trung GBK** như bản gốc
  (typo nguyên bản dòng 86 `"ffriendly_man_w"` giữ nguyên) — dịch TCVN3 là bước đánh bóng
  riêng nếu chủ game muốn.

### 11.3 TASKSYS đầy đủ = giai đoạn 2 (nếu muốn BẬT nhiệm vụ ngẫu nhiên như bản CN)

Khối lượng đã đo: C-API TASKSYS phải dịch ngược từ binary Linux (TaskName/TaskNo/
GetTaskStatus/SetTaskStatus/CloseTask/GetTaskText/TaskEvent/EnumEventList…), framework Lua
`task\system\` (task_head 920 + task_main 852 + tools/function/string/random ≈ 2.5k dòng),
bảng `settings	askandomate_*.txt` (6 bảng) + định nghĩa nhiệm vụ, item Mật đồ
(id Linux 904 — NGOÀI dải item partner đã port), `taskandom	ask_head.lua` + treasure_head
(đào bảo 577 dòng), client bảng nhiệm vụ (bản gốc dùng Lua UI mobile `taskui_partner_random.lua`
— JX1 cần nối vào TaskGuide F12). Các hệ Linux khác (activitysys/event/giftcode) cũng dùng
TASKSYS — port chung một thể sẽ lợi.

---

## 12. BỔ SUNG 28/08 ĐÊM — MENU ADMIN + ICON MÀN HÌNH CHÍNH (p11 + p12)

- **Lệnh bài admin** thêm mục "Bộ test Đồng hành" (`BDH_Root`, tệp mới
  `script\item\bdh_admin.lua`): cấp thú Kim/ngẫu nhiên, gọi-thu, +10000 exp, thăng cấp,
  thân mật 100, túi cấp 10, dạy kỹ năng, xem thông tin, xóa con, chạy FULL test tự động.
  Đã ĐO trần 512 byte: menu chính 493/512, submenu 271/512 (bài học tràn 551/512 ngày 27/08).
  `lenhbaiadmin.lua` tự dofile mỗi lần bấm ⇒ ăn ngay không cần restart.
- **Icon đồng hành trên UiPlayerBar**, ngay TRÊN icon bầu cua (neo cùng cột phải 1024−30):
  ảnh nút GỐC Linux `同伴的基本属性.spr` (pak không có icon main-UI riêng — đã dò 24 tên);
  bấm = mở/đóng thanh nhanh + cửa sổ thuộc tính. Tệp: UiPlayerBar.ini (+[PartnerIcon]) +
  UiPlayerBar.h/.cpp (5 mối `[BDH-G4]`, sao lưu `.truoc_bdh_icon`).
- **Trang thuộc tính hiển thị ĐỦ theo dữ liệu có thật**: exp dạng "hiện tại/ngưỡng lên cấp"
  (chép `level_exp.txt` sang client, khuôn đọc y KPartnerTables::Load) + 3 cột icon kỹ năng
  bẩm sinh cạnh 5 thẻ (KongfuIcon/ToughIcon/MissleIcon của ini gốc). 2 ô 元气/火候 (Energy/
  Fire) hiện `-` — ĐÃ SOÁT KỸ: 81 biến partner_task_def + toàn bộ script partner Linux
  KHÔNG có nguồn dữ liệu này (thuần client mobile), không bịa số.
- Game.exe đợt 4 `cf90bf22` đã swap kiểu RENAME (bản trước đang chạy giữ ở
  `Game.exe.dangchay_2808`) — người chơi phải THOÁT GAME VÀO LẠI mới ăn.

---

## 13. ĐỢT 28/08 SÁNG — FIX "PET BỊ NGƯỜI CHƠI ĐÁNH" + VIỆT HÓA GIAO DIỆN (p13–p15)

### 13.1 Pet bị người chơi khác đánh — GỐC + FIX (p13_fix_relation.py)

GỐC: quan hệ đánh/không-đánh do bảng 6 chiều `m_RelationTable` quyết
(`KNpcSet::GenOneRelation` — SINH BẰNG CODE, không có nhánh `kind_partner`) ⇒ pet rơi
vào luật NPC thường: trong PK/chế độ chiến đấu thành mục tiêu hợp lệ.
FIX ở TẦNG BẢNG (một luật cho MỌI đường: chọn mục tiêu client, đạn/AOE
KNpc.cpp:4130 + KMissle, AI quái, AI pet sPartnerPickTarget):
pet - pet = none; pet - người chơi = **ally** (kể cả PK — không thể đánh/bị đánh);
pet - NPC theo phe thường (quái vẫn đánh pet, pet chết thì hôn mê như gốc).
`KNpcSet.cpp` compile CẢ 2 phía ⇒ đợt này swap CoreServer `18d22890` +
CoreClient `ad7d2c51` + Game.exe `a4a7e6af` (bản trước `.truoc_relation`).

### 13.2 Giao diện "sai" = bộ ảnh đang là bản CHỮ TRUNG — đã Việt hóa (p15_vietsub_spr.py)

- Dò trọn client VLTK (Level Up Games\Vo Lam Truyen Ky): pak KHÔNG có 14 ảnh 界面
  (hệ chưa từng bật trên client này); CÓ 5 ini (chỉ khác comment với bản đang dùng) +
  **2 ảnh túi bản VIỆT "Hành trang/Đóng"** trong update03 → đã rút vào client JX1
  (`\spr\Ui3\同伴背包\`) — trước đó client THIẾU HẲN nền túi.
- **Giải mã format SPR thật** (khác KSprite.h 2003): sau palette có **4 byte đệm**, bảng
  SPROFFS{Offset,Length} với Length = 8 + RLE, frame header chỉ 8 byte {W,H,offX,offY};
  RLE token [run][alpha 0..31]. Bộ đọc/ghi trong p15 (class Spr) + spr2png.py.
- **14 ảnh đã Việt hóa** (đè loose, backup `.truoc_vietsub`, marker `.da_viet`):
  2 title ("THUỘC TÍNH/KỸ NĂNG ĐỒNG HÀNH"), 2 nền 411×376 (mọi nhãn: Tên/Kinh nghiệm/
  Tinh lực/Sát thương/K.thường→K.độc/Cấp/Ngũ hành/Tính cách/Thân mật/Sinh lực→May mắn/
  Tư chất×6/Võ công/Kháng/Ra chiêu), 5 nút (Lưu/Giải tán/Đóng/Chuyển trang/Quên kỹ năng),
  5 thẻ dọc (Đ-H-số).

### 13.3 Đọc lại NGHĨA trường từ ảnh nền gốc — sửa dữ liệu attr (b10a regen)

- Cột `m_editXxxPotential` = **资质 TƯ CHẤT** (apt 1..10) — trước điền nhầm mức tăng/cấp.
- `m_editWuxing` = **五行 NGŨ HÀNH** (tên hệ Kim/Mộc/Thủy/Hỏa/Thổ) — trước điền TB tư chất.
- 3 cột icon trang attr = **Võ công (SK_GIVEN) / Kháng (549..553) / Ra chiêu (SK_LEARNT)**
  của CON ĐANG XEM (cột dọc theo YDistance) — trước hiểu nhầm "3 icon x 5 thẻ".
- `m_editEnergy` = 精力 (Tinh lực), `m_editFire` = 杀伤 (Sát thương) — nhãn đã rõ nhưng
  server Linux KHÔNG lưu 2 số này (soát 81 biến + script) → vẫn `-`, chờ nguồn.

---

## 14. ĐỢT 28/08 TRƯA — HỆ "BẠN ĐỒNG HÀNH" BẢN PC (PETSYS) — HỆ ĐÚNG CHỦ GAME MUỐN

Ảnh chụp chủ game đưa = cửa sổ **pet.ini** của client VLTK Level Up (một cửa sổ 650×450:
Thông Tin/Trang Bị/Kỹ Năng/Thao Tác + Đúc lại). Đây là hệ **petsys** — server Linux có
`script\petsys` (13 lua) + `settings\petsys` (3 bảng), SONG SONG với hệ partner mobile.
Nguồn 100%: lua+bảng Linux, `pet.ini` + bộ ảnh `spr\Ui3\pet\*` rút từ pak private
updatejx14/15, danh mục control từ `game_y.exe` (class UiPet).

### 14.1 Đã thi công (p16→p22 trong ReverseTools\bandonghanh)
- Server: `KPlayerPet.cpp/h` — C-API PET_* (25 hàm, thay lib C "PET" strip của Linux);
  lưu task value **5110..5131**; summon NPC kind_partner `m_nPartnerNo=100` (AI follow-only
  `Pet_ProcessAI`, theo map `Pet_Breathe`); AURA = 4 skill state **1600..1603** (sinh từ
  `pet_skill_def.txt`, p19a) cast lên CHỦ 40s, re-cast 30s; kênh lệnh = `c2s_partnerop`
  op `PARTNER_OP_PETSYS=30`, nParam = PET_OPERATION 1..7 → `Pet_RunProtocol` →
  `PetSys_Protocol(n)` (protocol_process_gs.lua — wrapper đặt ĐẦU file + Include đủ 7 module).
- Port 13 lua petsys (7/7 lib phụ thuộc CÓ SẴN trên JX1) + 3 bảng (server+client);
  6 item MỚI id **4874..4879** (Thiệp/Thuốc tăng trưởng/4 trái cây — bảng private dùng
  3453.. nhưng JX1 đã chiếm id đó nên thêm cuối bảng, đã nắn id trong lua);
  npcs.txt nối tới 2185 (2 template nangongjue/linhaotian từ Linux, server+client).
- Client: `UiPet.h/cpp` đúng 100% pet.ini (12 nhãn+12 giá trị, 8 nút op, Skill_1 icon aura,
  khung Equip, **AppearanceImg = hình pet đứng** `\spr\npcres\<nhóm>\<res>\<res>_st01.spr`
  tra cột 12 NpcResType của npcs.txt); phím **Y**/icon PlayerBar → UiPet; "petmain" case 40.
- Admin: lệnh bài → Bộ test Đồng hành → **"Hệ PET bản PC (mới)"** (cấp đồ/tạo nhanh/điểm/op).

### 14.2 Sự cố & luật mới (đã ghi memory `feedback-khong-tu-tat-gameserver`)
- 🔴 Test tick mỗi phút gọi `PB_ClearBot()` → **xóa cả bot chủ game tự dựng** ⇒ đã TẮT
  3 test tick; cấm ClearBot toàn cục trong test định kỳ.
- 🔴 Cấm tự tắt/restart GameServer khi chưa được phép; cấm tự cấp đồ/pet cho người thật
  (BDH_GmTick đã vô hiệu — quy trình gốc: item Thiệp Đồng Hành).
- 🔴 Bẫy bash nuốt `\n` tái diễn (probe jitan) → "unfinished string" làm `RunTime`
  dofile chết mỗi phút ⇒ MỌI sửa script không ăn trong ~15 phút; đã sửa.

### 14.3 Hai lỗi tồn & cách xử đợt này
- 4 ô ĐIỂM (5112..5115): C get/set no-op khó hiểu trong khi ô 5111/5116+ sống (Lua
  SetTask 5112 hoạt động) — nghi **obj trộn** sau lần build FAIL giữa chừng ⇒ viết lại
  12 hàm điểm TƯỜNG MINH số trần + **-t:Rebuild** cả 3 nhị phân.
- Nút bấm "không hiện gì": gốc = `protocol_process_gs.lua` phiên nạp CŨ (thiếu Include
  module ⇒ dispatch rơi im); đã vá wrapper+Include; sau restart mọi nút có thoại phản hồi
  từ script gốc (ALREADY_SUMMON/NOT_ENOUGH_POINT/hộp đổi tên/Say xác nhận…).

## 15. DOT 28/08 TOI - GOC THAT "MOI NUT PETSYS IM" + BO VA p27

Trieu chung: bam 8 nut UiPet -> op TOI server (petops.log ghi op=2/5/7)
nhung KHONG thong bao, KHONG loi script. Ba goc xep chong:

1. **Include KHONG guard** (ScriptFuns.cpp:2051 = lua_dofile thang):
   7 module petsys deu Include head.lua -> `PetSys = {}` bi RESET 8 lan
   trong state protocol_process_gs -> bang cuoi chi con TransferExp +
   ProtocolProcess; Summon/ChangeName/LevelUpDlg/... = nil -> dispatch im.
   FIX: head.lua tu guard `if PetSys == nil then ... end` (p27-A).
2. **Callback "label/#PetSys:Fn()" khong chay tren JX1**:
   KPlayer::ExecuteScript (KPlayer.cpp:7106) chi parse `Fn(1,2)` = ham
   GLOBAL PHANG + tham so SO (atoi); ten co `:` -> lua_getglobal fail.
   FIX: delete/feature doi sang CreateNewSayEx cua dailogsys (bang
   {label, fn, {self,...}}; callback #g_DailogBack(i) phang) (p27-F/G).
3. **Callback theo Npc[player].m_ActionScriptID**: Pet_RunProtocol cu goi
   pScript->CallFunction tran -> KHONG set ActionScriptID -> Say chon muc
   + AskClientForString (hop nhap ten) roi vao script cu/0.
   FIX C: Player[idx].ExecuteScript(path, "PetSys_Protocol", op) - tu set
   ActionScriptID (KPlayer.cpp:7092) + inject PlayerIndex/PlayerID (p27-K).

Kem 4 loi chac-chan-no khi chay tiep:
- **TSK_COUNT_TRANSFER_EXP = 1 ma task 1 JX1 = RANK MON PHAI**
  (quanly.lua SetTask(1,GetRank())) -> bam Thuan duong la PHA rank!
  Doi 7 hang task -> 5132..5138 (p27-C); o loai-aura 3061 -> 5124
  (= PET_TV_SKILL0, petcard/levelup/bdh_admin) (p27-D).
- `unpack` khong ton tai trong state petsys (chi o lib\common.lua khong
  duoc Include) -> feature.lua + feed.lua chet -> them vao jx1_compat (p27-B).
- feed.lua `if not PET_IsCreate()` sai vi 0 la truthy -> `~= 1` (p27-E).
- petcard.lua goi PetSys:Summon() nhung khong Include summon.lua (p27-H).

Xac minh tinh da lam: %GLOBAL upvalue HOP LE Lua4 (lparser.c:233 - chup
gia tri luc tao closure; lang.lua nap truoc -> hang song); tbLog/
tbSecurityLock co san (lib\log.lua + global\securitylock); TaskSay la
Lua (task_string.lua); client co UI hop nhap (s2c_inputbox nType=1 ->
GDCNI_OPEN_INPUT, duong Lien Dau); AskClientForString/ST_CheckTextFilter/
FileName2Id/GetWorldPos deu co binding.

**Trang thai**: script an ngay qua "Nap lai script" (lenh bai admin -
da them petcard/feed vao reload); C build sach, dat canh:
`bin\server\CoreServer.dll.moi_2808_petsys_menu` (md5 4b11bf00).
**Voi binary cu**: Talk/thoai/hop nhap DA HIEN (guard fix du); bam CHON
muc menu + callback nhap ten can SWAP binary moi + restart.
Output build server = `Sources\Core\x64\ServerRelease\` (KHONG space;
thu muc "Server Release" co space la ban cu 24/08 - dung lay nham).

Con mo sau swap: MapList summon (head.lua) van la map id LINUX
{1,11,37,...} can soat theo map JX1; go PLOG (p26) sau nghiem thu;
he trang bi pet (Duc lai + COMPANIONEQUIP) lam sau khi 8 nut chuan.
Bo va: ReverseTools\bandonghanh\p27_goc_include_reset.py (11 muc, chay
lai duoc; kem va tay levelup 3061->5124).

## 16. DAC TA MO RONG PETSYS BAN PRIVATE VLTK (chua thi cong - nguon da rut du)

Nguon: client VLTK Level Up, da rut ra_pet/client_common.lua (tu
slistcache.pak, path \script\petsys\common.lua) + client_ui.lua
(= 90bccb02). Ban PRIVATE khac ban Linux:

- MAX_LEVEL = 130 (Linux chi 20); PET_LEVEL_STEP 10; MIN_LEVEL 150 (level
  nguoi + chuyen sinh 2 de tao pet - nhu Linux).
- **Op 8 = PET_OPERATION_XIUZHEN_POINT** (nut Tu Chan): doi CHAN NGUYEN ->
  diem Tu chan; ZHENYUAN_TO_XIUZHEN_POINT_RATE=200, VALUE=20000
  (JX1 co he chan nguyen: lib\awardtype\zhenyuan_jx1.lua).
- **4 KY NANG CHIEN DAU pet (ext skill)**: mo tu pet level 21
  (EXT_SKILL_OPEN_PET_LEVEL), moi 5 cap hoc 1 (EXT_SKILL_GET_NEW_LEVEL),
  toi da 4 (EXT_SKILL_MAX_COUNT); hoc bang item Bi kip PET_MIJI_ITEM
  {6,1,4808}; bang skill tbPetSkillIDList = 1670..1687 (18 skill).
  GetExtSkillCount() = (lv-21)/5+1 cap 4. UiPet da co san 4 o m_ExtSkill.
- **Nguon diem THANG CAP (UpgradePoint)**: 8 su kien EVENT_LIST bit-per-day
  (TSK_EVENT_FINISHED/DATE reset MOI NGAY): Tong Kim 3000 diem tich luy,
  Vuot ai 10, Viem De ai 6, Trong cay 5 hat Thien Loc, 1 nhiem vu Tin Su,
  Do nang dong 40, 10 nhiem vu Da Tau, Kiem Gia Me Cung ai 2.
  Server PHAI goi FinishEvent(idx) tu tung hoat dong (chua noi) ->
  doi bit -> cong UpgradePoint (co che cong diem tu bit: chua ro, can mo
  them server private hoac tu thiet ke: du 1 su kien +1 diem?).
  Client tooltip UpgradePoint hien bang 8 dong xanh/do (OnTipsShow).
- **HE TRANG BI PET (nut Duc lai)** - 4 kenh ScriptProtocol (client_ui.lua):
  1. C2S_OPEN_COMPANIONEQUIP (khong tham so) -> server tra
     s2c_OpenRebuildCompanionEquipUI() -> client mo cua so duc.
  2. C2S_TYR_REBUILD_COMPANIONEQUIP (nItemId) -> server random thuoc tinh
     THU -> s2c_TryRebuild(nCount, tbAttrib) -> client hien bang tam
     (CompanionEquip_UpdateTmpAttrib).
  3. C2S_DO_REBUILD_COMPANIONEQUIP (nItemId, nRet 0/1) -> ap/huy ->
     s2c_DoRebuild() xoa bang tam.
  4. C2S_ASKCONSUME_COMPANIONEQUIP (nItemId) ->
     s2c_SetCompanionEquipCostTip(szCost) - tip gia.
  JX1 khong co ScriptProtocol registry -> di duong c2s_partnerop
  PARTNER_OP_PETSYS voi op 10..13, nParam dong goi (itemId*2+nRet cho DO).
  \script\petsys\companionequip.lua KHONG co trong pak (logic server
  private) -> phan server TU THIET KE khop protocol client 100%.
- Tooltip 4 diem (OnTipsShow client_ui.lua): UpgradePoint=bang 8 su kien;
  GrownPoint='trong cay nhan tao/mia/bap/khoai cho pet an';
  TamePoint='truyen exp, 25 lan/ngay'; XiuzhenPoint='150 (thuc te RATE=200)
  chan nguyen doi 1 diem tu chan'.
- Thu tu lam sau nghiem thu 8 nut: (a) op8 Xiuzhen; (b) FinishEvent noi 8
  hoat dong; (c) ext skill (item 4808 + bang 1670..1687 + cast trong
  Pet_ProcessAI); (d) trang bi pet (equip slots + duc lai).

### 15b. (23:53 dem) GOC THAT "diem khong an" = 6 STUB DE HAM THAT

Probe `sau set PET_Get=0 GetTask5114=0 muon=1` (binary chu tu build, restart
23:40) chung minh C-set khong an du nguon dung. Soi bang dang ky:
**ScriptFuns.cpp:15740-15745 co 6 dong `{"PET_(Get|Set)(Grown|Tame|Upgrade)Point",
LuaHD3_PET_Stub}`** (thoi port hoat dong, stub push 0/nuot set, KJx2WarInfra.cpp:2082)
dang ky SAU khoi PETSYS that (15565-15570) -> setglobal sau DE truoc -> 6 ten
tro stub. Khop mau '5112..5114 chet, 5110/5111/5116+ song' tu dau -> nghi an
'obj tron' hom qua la OAN. Da XOA 6 stub (giu ham LuaHD3_PET_Stub), build:
**`CoreServer.dll.moi_2808_hetstub` (md5 51298755)** dat canh CHO SWAP.
Luu y: dem chuoi 'PET_SetTamePoint' trong binary KHONG phan biet duoc
(string pooling /GF gop literal trung) - doi chieu bang md5.
Bonus: lenhbai_def.lua:390-392 (su kien thap nien VNG) goi PET_Set* de CONG
DIEM PET - go stub xong he nay hoat dong that (dung nguon diem ban private).
Trang thai khac tu log: daily tu luyen 25/25 (reset qua 0h theo %y%m%d);
CaiBang xu=0 (can cap xu test doi ten/ngoai quan); map 225/379 cam trieu dung.

## 17. 29/08 SANG - NGHIEM THU DOT 1 (chu: "da oke") + 5 fix cuoi

Da chay tron: 8 nut co ban + diem hien dung tren cua so (77/0/22/0),
doi ten (MeoMeo), Tu Chan op8 doi 200 chan nguyen/lan (952220->951620,
diem 0->3), menu chon/callback song, xu tru dung nguon.

Cac goc tim ra + fix trong dot nay:
1. **Diem khong hien tren UI du client nhan du** (do 3 tang p33: sv gui ok,
   cl nhan ok, updatedata doc ok) -> Game.exe 16:18 la ban build THIEU;
   build lai tron bo la hien. (Probe C da GO sau nghiem thu - p37.)
2. **"Xu" cua game = task 251 (TASKVALUE_STATTASK_XU, hien o HANH TRANG
   qua GDI_PLAYER_HOLD_FKCOIN)** - KHONG phai ExtPoint. jx1_compat
   GetCashCoin/PayCoin doi sang GetTask/SetTask(251); lang doi chu
   'tien dong' -> 'xu'.
3. **Hinh pet**: client JX1 thieu sach bo anh 21 loai pet - p35 rut tron
   tu pak VLTK ve spr
pcres\<nhom>\<res>\ (st/wlk/bat/die/at1/at2)
   + ban sao _st01.spr cho khung UiPet. Bang npcres JX1 chi map res->
   thu muc, ten file theo suffix chuan.
4. **Icon ky nang aura khong hien**: 4 dong skills.txt (id 1600..1603)
   cot SkillIcon THIEU duoi .spr (skill thuong co) - va ca server+client.
5. **Nut Tu Chan (op 8 ban private)**: p36 - xiuzhen.lua moi (doi 200
   chan nguyen = task 362 TASK_CHANGNGUYENDAN lay 1 diem, tran 20000),
   common +PET_OPERATION_XIUZHEN_POINT=8, head map [8], UiPet op 0->8.

Quy trinh swap MOI (chu yeu cau 'out game la tu cap nhat'):
- bin\client\ChoiGame.bat + bin\server\ChayGameServer.bat: tu doi
  <file>.moi -> ten that (ban cu .truoc) roi mo game/server.
- Tu nay build dat ten CO DINH: Game.exe.moi / CoreClient.dll.moi /
  CoreServer.dll.moi / WAuto.dll.moi.
- Dang cho .moi: ban sach da GO log C (sv eaeb269f, cl 446e91a0,
  game d8316f4c) - chu restart lan toi la an.

Con lai (dot ke tiep): he TRANG BI pet (6 o + nut Duc lai, 4 kenh
COMPANIONEQUIP - thiet ke server theo client, muc 16); 4 ky nang chien
dau ext skill (item Bi kip 4808, bang 1670..1687 - kiem skills.txt JX1
co chua, thieu thi port tu VLTK); go PLOG lua sau khi xong trang bi.

## 18. 29/08 TRUA - DICH NGUOC AI LINUX + TRAN 130 + SKILL HOAN CHINH + TRANG BI V1

Chu nhac LUAT: 'lam giong 100% linux chu khong tu bien' -> thay het tham so
tu che bang so THAT dich nguoc tu jx_linux_y (re_disasm.py + luamap co san):
- KPet::Summon 0x081D52F0 -> CreateNpc 0x081D5180: KHONG set toc/mau/AI
  bang code - moi chi so theo BANG npcs.txt (JX1 bang gan giong Linux,
  template pet co skill san).
- KPet follow 0x081D4F80 (goi tu PLAYER TICK moi frame, caller 0x080B7104,
  wrapper check kind==6 kind_pet): dist^2<=46224 (~6.7 o) DUNG;
  >562499 (~23.4 o) SetPos VE DUNG TOA DO CHU; giua: WALK toi diem
  cheo-sau chu 100mps cung phia pet dang dung (khong random).
  -> Pet_ProcessAI don npc mo coi; follow+fight chay trong Pet_Breathe
  (CoreServerShell moi frame ~18fps = dung nhip Linux).
- Pet Linux GOC KHONG DANH; tinh nang danh giu theo yeu cau chu (khuon
  partner: mode 22 vision 480, moi ~18 frame, skill tu bang npcs).
TRAN CAP: pet_skill_def.txt = ban PRIVATE VLTK 130 cap (pak); levelup.txt
noi 21..130 dung quy luat tuyen tinh do tu bang goc (Up=80(n-5), Grown=x2,
Tame=250(n-5)); MAX_LEVEL common + PET_MAX_LEVEL C = 130.
SKILL 18 con: goc 'hoc ma khong co gi' = JX1 THIEU \script\skill\petskill.lua
(LvlSetScript cua 18 dong) -> rut tu pak VLTK, ghep khung chuan JX1;
o luu SkillId*100+Level, hoc trung = +1 cap (max 5 theo bang);
icon o 36x36; menu admin vao thang PET; 5 cua so UiPartner* cu bi chan mo.
TRANG BI V1 (khong co nguon server private - thiet ke toi gian khop UI,
muc 16): 6 item 4881..4886 dung-la-deo (tra do cu), pct 80..120 duc lai
(op 10, 5 xu/lan, menu hop thoai server), bonus HP/MP ap khi summon
(5157/5158); 6 o cua so hien anh item; nut Duc lai -> SendOp(10).
Binary .moi cuoi: CoreServer addb0811, Game 53f5856e.

## 19. 29/08 CHIEU - TIM RA TRON NGUON HE TRANG BI DONG HANH (VLTK)

Chu: "toi nho 100% la ban linux co item cua pet chu khong can phai che"
-> DUNG. Truoc do toi quet SOT vi tim chuoi chu THUONG 'ong hanh' trong
khi ten item viet HOA 'Dong Hanh'. Quet lai + do chuoi trong game_y.exe
ra tron bo nguon:

1. **Bang item trang bi**: slistcache.pak entry 0xdf37e2dc (20 mon):
   Genre 12 / DetailType 0..9 = VI TRI / ParticularType 0..1 = BO.
   Bo 0 'Bich Huyet', bo 1 'Kim Lan' (chuoi con bo 3 'Dan Tam').
   10 vi tri: Nhan(vu khi), Chien Y, Gioi(nhan), Ho Uyen, Ho Than Phu,
   Thuc Yeu(dai), Lien(day chuyen), Chien Ngoa(giay), Yeu Truy, Quan(non).
2. **\settings\companionequip\suitattrib.txt** (thuoc tinh BO theo bac):
   bac0: 233=5000, 308=30, 311=1 | bac1: 7500/40/2 | bac2: 10000/50/3.
   233 = sinh luc toi da; 311 = cap 'Van Khoi Long Tuong' (chuoi
   G_STR_COMPANION_SUIT_ATTRIB: cap cao hon doi phuong -> sat thuong
   phong dai 10%); 308 CHUA ro (JX1 chi co 305 attrib - ma 308/311
   NGOAI DAI, ban private mo rong).
3. **Item lien quan** (bang VLTK 004): 5063 Ket Tinh Dong Hanh,
   5257 Ket Tinh (Cao), 5064/65/66 Ruong trang bi 1/2/3,
   5067 Chia Khoa Ruong Dong Hanh.
4. **Cua so duc lai**: ini 720a151f = 'Duc lai trang bi Dong Hanh':
   TxtCost 'Tieu hao: Dong Hanh Ket Tinh x%d', ListOldSkill/ListNewSkill
   (2 danh sach thuoc tinh cu/moi), nut 'Trang bi tay luyen' /
   'Giu lai' / 'Tu bo', o objEquip dat trang bi.
5. **Giao thuc** (protocol_def client): 8 ma COMPANIONEQUIP
   (OPEN / ASKCONSUME / TYR_REBUILD / S2C_TRYREBUILD / DO_REBUILD /
   S2C_REBUILD / SETCOST_TIP / DOREBUILD).
6. **UI goc pet_main.ini**: 10 o EquipWeapon/Helm/Armor/Belt/Boot/
   Amulet/Ring/Cuff/Pendant/Sachet + btnViewEquip1/2 (lat 2 trang).

DA PORT (p62/p63): 26 item vao bang JX1 - JX1 chi co 7 ItemGenre
(khong co 12) nen dua vao magicscript giu nguyen ten/anh goc; vi tri +
bo luu o bang phu script\petsys\petequip_def.lua (sinh tu dong).
Nan id: 4907..4926 = 20 mon, 4927..4932 = ket tinh/ruong/chia khoa
(4881..4906 DA CO item JX1 khac - phai dung day trong tu 4907).
suitattrib.txt chep vao settings\petsys\.

CON LAI (dot sau): logic server (deo/thao 10 o, thuoc tinh bo, mo ruong,
duc lai 2 danh sach) + UI 10 o & cua so duc. Logic server ban private
KHONG co trong pak -> thiet ke theo dung giao thuc + bang tren.

## 20. TONG KET 30-31/08 - TRANG THAI DE PHIEN SAU TIEP TUC

### 20.1 He dang chay (petsys ban PC/VLTK)
- Cua so `UiPet` (`\Ui\Ui3\pet_main.ini`): 4 diem, ten/cap, 6 thuoc tinh,
  10 O TRANG BI (hang tren, x 328 + k*30, y 82, 24x24), 5 o ky nang
  (x 376..520 buoc 36, y 167, 26x26), khung anh pet (40,303 260x120),
  nut Duc lai (235,390 62x24), 8 nut thao tac.
- 8 thao tac: 1 Xoa / 2 Goi / 3 Thu / 4 Thang cap / 5 Doi ten /
  6 Doi ngoai quan / 7 Tu luyen / 8 Tu Chan; + op 10 = menu TRANG BI.
- Tran cap 130 (VLTK), levelup noi 21..130 theo quy luat bang goc.
- 4 vong sang 1600..1603 + 18 ky nang bi kiep 1670..1687 (buff CHU).
- Trang bi: 20 mon (2 bo Bich Huyet/Kim Lan) x 10 vi tri; moi mon 3
  thuoc tinh {ma, min, max}; deo -> roll gia tri; Duc lai ton 1 Ket
  Tinh Dong Hanh -> roll lai, chon Giu lai / Tu bo.
- CHU CHOT 30/08: PET TU DANH (phan THEM ngoai ban goc) + thuoc tinh
  trang bi & bo cong cho PET; vong sang + bi kiep buff CHU.

### 20.2 O luu (task value)
```
5110 create   5111 level    5112 upgrade  5113 grown   5114 tame
5115 xiuzhen  5116 feature  5117 summon   5118..5123 6 thuoc tinh
5124..5127 4 o skill aura   5128..5131 ten pet (16 byte)
5132..5138 hang task he (transfer/trai cay/su kien)
5139..5142 4 ky nang bi kiep da hoc (id tran)
5143..5152 10 o trang bi (ParticularType)   5163 bo*100 + so mon
5170..5199 3 gia tri thuoc tinh da roll cua tung mon
251 = XU (hanh trang)   362 = chan nguyen   MAX_TASK = 5200
```

### 20.3 Item (bang magicscript, id JX1)
- 4874 Thiep / 4875 Thuoc tang truong (stack 200) / 4876..4879 4 trai
- 4880 Bi kiep ban dong hanh (VLTK 4808 - JX1 da co item khac nen nan)
- 4890..4906 = 17 ban ghi GIU CHO (bat buoc, xem 20.5)
- 4907..4926 = 20 mon trang bi | 4927/4928 Ket tinh (+Cao)
- 4929..4931 Ruong 1/2/3 | 4932 Chia khoa ruong

### 20.4 Tep da them
- server: script\petsys\{petequip, petequip_def, petbox, bikip,
  xiuzhen, jx1_compat}.lua; script\skill\petskill.lua;
  script\skill\petsys\aura.lua; settings\petsys\{suitattrib,
  equipattrib, attribname, pet_skill_def, levelup, feature}.txt
- client: ban sao bang item/skills + settings\petsys\{equipattrib,
  attribname}.txt + anh spr\item\companionequip\* (20),
  spr\Ui3\pet\{face,extskill}\*
- C: KPlayerPet.cpp/.h (40 ham PET_*), UiPet.cpp/.h

### 20.5 BAY DA CAN THAT (doc truoc khi sua!)
1. **Bang magicscript: RECORD INDEX = ma particular**
   (KItemGenerator.CPP:1660 `const int i = nParticularType;
   GetMagicScript(i)`). Xoa/them dong giua bang la LECH TOAN BO ->
   item vo hinh, dinh chuot, khong nhan duoc. Chi duoc THEM O CUOI,
   thieu ma nao phai chen ban ghi GIU CHO.
2. **Nhan menu KHONG duoc chua dau '/'** - LuaSelectUI cat ten ham tai
   dau '/' DAU TIEN (ScriptFuns.cpp:747).
3. **AddItem CAN 7 THAM SO** (ScriptFuns.cpp:4932) - goi 6 thi khong
   them item nao (mat do khi tra ve tui).
4. **Say/SayEx tran 512 BYTE** cho tieu de + moi nhan cong lai.
5. **`<enter>` la tag ENGINE TU CHEN khi wrap** (KItem.cpp:2662) -
   viet tay se hien tho.
6. **HoldObject(uGenre, uId, nDataW, nDataH)** - tham so 3/4 la KICH
   THUOC, khong phai cap; muon tooltip skill hien dung cap phai
   SetSkillLevelDirectlyUsingId cho nguoi choi.
7. **Z-ORDER: control AddChild SAU nam TREN** - khung anh pet trum
   len nut Duc lai lam nut khong bam duoc.
8. **pos_hand khong nam trong luoi o** -> ConsumeEquiproomItem khong
   voi toi; dung PET_ClearHand() (duyet m_ItemList).
9. **Build song song voi phien khac** -> loi PCH C1853; build bang
   `-p:IntDir=x64\SRpet\ -p:OutDir=x64\SRpetOut\` (output cuoi van
   ve x64\ServerRelease qua post-build).
10. **TCVN3 khong co chu HOA co dau**; **quet chuoi phai thu ca HOA
    lan thuong** (tim 'ong hanh' chu thuong da lam SOT ca he trang bi).
11. `Include` khong guard -> head.lua phai tu guard `if PetSys == nil`.
12. Callback `"nhan/#Obj:Fn()"` KHONG chay - dung CreateNewSayEx.

### 20.6 Binary moi nhat (CHO CHU SWAP)
- `bin\server\CoreServer.dll.moi` = **b2526657**
- `bin\client\Game.exe.moi` = **8886fac8**
- Swap bang `ChayGameServer.bat` / `ChoiGame.bat` (tu doi ten .moi).

### 20.7 CON TON DONG (viec phien sau)
1. Chu chua swap 2 binary tren -> ky nang van hien cap 0, nut Duc lai
   chua bam duoc, do ket tay chua go duoc.
2. Bo 3 **Dan Tam** co chuoi ten nhung KHONG co item trong bang goc.
3. Ma thuoc tinh **308/311** (Van Khoi Long Tuong) NGOAI DAI 305
   attrib cua JX1 -> muon co phai them attrib moi vao engine.
4. **8 hoat dong cap diem Thang cap** chua noi (Linux:
   `activitysys\config\44\extend.lua` goi FinishEvent + 
   PET_AddUpgradePoint(1) moi hoat dong/ngay). Ham C da co.
5. Cach LEN CAP ky nang bi kiep: nguon VLTK khong mo ta -> hien moi o
   hoc 1 ky nang, chua co nang cap.
6. Cua so **Duc lai rieng** (ini goc 720a151f: 2 danh sach cu/moi +
   3 nut) chua dung - hien dung menu hoi thoai server.
7. Go PLOG + petops.log sau khi nghiem thu.
8. He partner mobile (UiPartner*) da bi CHAN mo - neu can dung lai thi
   go `return NULL` trong 4 tep UiPartner*.cpp.
