> ✅ **ĐÃ HOÀN TẤT E3→E7 (14/08/2026 tối)** — đọc `BANGIAO_CONGTHANH_DOTE.md` thay file này.

# BÀN GIAO GIỮA CHỪNG — ĐỢT E CÔNG THÀNH (dừng theo lệnh chủ game 14/08/2026 ~15h15)

Phiên sau ĐỌC FILE NÀY TRƯỚC, rồi đọc 3 tài liệu chuẩn: `KEHOACH_CONGTHANH_DOTE.md` (kế hoạch
bản 2 sau phản biện — LÀM THEO ĐÚNG NÓ), `DIEUTRA_CONGTHANH_BINARY.md` (đặc tả 41 hàm engine),
`DIEUTRA_LEAGUE_SPEC.md` (đặc tả League đầy đủ). Lệnh gốc của chủ game: "Làm luôn tính năng
công thành E / chạy phản biện điều tra cho kỹ / đúng bản gốc linux, bỏ mã nguồn cũ".

## I. ĐÃ LÀM XONG (9 commit `2dbaa20` → `8cb333e`, đã push gamedevnew/main)

| # | Việc | Commit | Ghi chú |
|---|---|---|---|
| 1 | Kế hoạch bản 2 + đặc tả binary (sau phản biện kế hoạch 5 phiên / 57 phát hiện / 10 blocker) | 2dbaa20 | |
| 2 | **E1** ngắt hệ CTC cũ + nền dữ liệu | fcdb7f4 | chi tiết dưới |
| 3 | **E2a** KJx2SharedStore: OB_* 13 hàm + Ladder_* 3 hàm (persist) + Set/GetGlbValue | bfe0817 | |
| 4 | Đảo nguồn chuẩn citywar_city: **GỐC D:\ServerLinux, KHÔNG dùng zip** | 3cd4048 | chứng minh bằng quét ScriptID trap trong region vn |
| 5 | **E2b** KJx2League: đủ 28 hàm LG_/LGM_ + LG_ApplyDoScript (chạy cục bộ) + OpenGlbMission + Start/StopGlbMSTimer (3 tham số, lặp vô hạn) + hook `KJx2GlbMission_Breathe` trong CoreServerShell::Breathe | f85a579 | spec làm chuẩn = DIEUTRA_LEAGUE_SPEC.md |
| 6 | Cố định spec League ra đĩa | cdf8b55 | |
| 7 | **Vá theo phản biện E1+E2**: blocker PUBG (điểm TẮT/ĐỌC timer chưa dời 12/13→14/15 — 6 điểm đã vá + quét sạch), ngắt wrapper infocenter.lua CTC cũ, Ladder rank ngoài dải FAIL như gốc | 8cb333e | |

E1 gồm: comment NPC 373 báo danh CTC cũ (balanghuyen.lua:79) + UpdateCityOwnTongFromLua
(playerlogin.lua:23); PUBG dời timer 12/13→14/15 (nay đã vá đủ CẢ điểm tắt/đọc); TimerTask.txt
12/13=citywar_city, 14/15=pubg, 16/17=citywar_arena, 18=citywar_global; WorldSet GHI ĐÈ
World044-048 = 221,222,223,37,176, Count=49 (KHÔNG đụng W000-043, KHÔNG nâng Count qua đuôi
di sản); MapList server+client `221=特殊用地\剑门关vn` (+PATCHFULL\settings\MapList.ini);
trích 292 region + .wor từ pak Linux ra `bin\server\Maps\特殊用地\剑门关vn\` (loose THẮNG pak;
verify 285/285 trùng byte pak ta + 7 cell ta thiếu nay có); citywar.ini về đúng gốc relay
(1506B: 3 giá trị + 4 key + CR thừa); npcs.txt:627 tên "Rương chứa đồ"; playertitle_jx2.txt.

**Binary đã build + deploy vào `bin\server`** (Coreserver.dll qua post-build, GameServer.exe
copy tay). Build PASS: Core Server x64 + GameServer x64 + Core Client Win32. **Chưa restart** —
chủ game restart khi muốn test (relay + client exe KHÔNG đổi; client chỉ đổi MapList.ini).

**Checklist test cho chủ game sau restart** (đã gửi trong chat): boot sạch ScriptError.log;
GM goto 221/222/223/37/176 (2 thành mới có thể trống NPC); PUBG trọn vòng đời (báo danh →
đếm ngược → kết thúc, không lặp); NPC báo danh công thành cũ ở Ba Lăng Huyện biến mất;
thuế bia Tương Dương ngừng cập nhật (chủ đích); rương 625 hiện tên toàn server; bang hội
đợt 12 vẫn chạy.

## II. CHƯA LÀM (E3 → E8 — thứ tự bắt buộc, mỗi giai đoạn: làm → build → phản biện → commit đích danh → push)

### E3 — KCityWarJX2 nhóm CITY (ĐANG DỞ: mới tra xong mỏ neo, CHƯA gõ mã)
Thiết kế đã chốt (làm theo, khỏi nghĩ lại):
- File mới `Sources\Core\Src\KJx2CityWar.{h,cpp}` (_SERVER, khuôn 3 tầng như KJx2League:
  PCH KCore.h trước #ifdef; extern+đăng ký trong khối _SERVER ScriptFuns.cpp; vcxproj
  ExcludedFromBuild 4 cấu hình Client).
- Struct SẠCH: `cities[8]` (dùng 1..7): nState, nMapId, szAreaName[64] (byte TCVN3 từ ini),
  szOwnerTong[64], szMaster[64], szChallenger[64], nTax, nPriceParam, nOccupyDate;
  state: 0=NORMAL, 1=WARDECIDED (đã có khiêu chiến giả), 2=ATWAR.
- Nạp config: KIniFile đọc `settings\citywar.ini` (AreaIncludesNN=map id, AreaNameNN=raw bytes,
  [CitySettings] các trần — lưu global cho E7). Mirror persist `settings\jx2citywar.txt`
  (khuôn tmp+MoveFileEx như jx2league.txt; dòng: `C id state tax pp date` + `O id <owner...>`
  + `M id <master...>` + `H id <challenger...>` — tên để CUỐI dòng).
- 17 hàm đăng ký: GetCityOwner(2 ret) · GetCityWarBothSides(2 ret CÔNG,THỦ — cả hai non-empty
  mới trả) · GetCityAreaName · GetCityArea (KHÔNG dùng player: đọc global "SubWorld" của state
  gọi — extern `GetSubWorldIndex(Lua_State*)` ScriptFuns.cpp:476 dùng được) · GetCitySummary
  (tên + Thái Thú, chuỗi VN octal TCVN3 tối giản, E6 làm giàu) · GetAllCitySummary/
  SyncCitySummary (no-op 0 giá trị) · OpenCityManageUI (no-op tới E7) · HaveBeginWar
  (state==ATWAR) · NotifyWarResult (thắng: owner=challenger + master + nOccupyDate=yyyymmdd;
  MỌI nhánh: challenger="" + state=NORMAL; save + sync; **KHÔNG đụng field 48 — script
  CITY_OCCUPY_R gốc tự gọi TONG_ApplySetOccupyCityDay**, tong_mix.lua:936 bản server1) ·
  AppointViceroy(szCityName, szTongName — TÊN THÀNH, tra city theo so byte szAreaName) ·
  AppointChallenger(tên thành, tên bang → state=WARDECIDED) · IsSigningUp (league 508 task 1
  ==1) · NumOfSignUpTongs / GetSignUpTongName (đếm/tên member league 508) · DisabledChatCity/
  IsDisabledChatCity (stub 1/0) · thêm hàm hạ tầng cho tick: `CTC_JX2_SetCityState(nCityID,
  nState)` (tick 20h flip ATWAR).
- Cần export thêm từ KJx2League.cpp 3 helper C: `KJx2League_GetLeagueTaskC(type,name,task)`,
  `KJx2League_GetMemberCountC(type,name)`, `KJx2League_GetMemberNameC(type,name,idx)`.
- De-hardcode 3 điểm map 78: 2 setter `ScriptFuns.cpp:7002-7028` (thêm guard SearchWorld==-1,
  strncpy 31+NUL, clamp tax 0..MaxExchangeTax, tham số city tùy chọn, **ghi MỌI instance**
  cùng map id — 3/7 thành nạp 2 subworld, SearchWorld chỉ trả cái đầu) +
  `KPlayer::GetCityOwnTong` KPlayer.cpp:6370-6377 (đổi sang subworld CỦA NGƯỜI CHƠI:
  `Npc[m_nNpcIndex].GetSubWorldIndex()` — KNpc.h:283/:784, idiom ScriptFuns.cpp:3354).
- Sync chủ/thuế → KSubWorld: owner ghi dạng đệm `" %s "` (khớp hiển thị client cũ), trống thì
  chuỗi rỗng (ghi bàn giao); vòng for i<MAX_SUBWORLD so m_SubWorldID.
- Mỏ neo đã tra sẵn: SearchWorld first-match KSubWorldSet.cpp:29-37; tong name→ID =
  `g_FileName2Id(name)` rồi `g_TongJX2.FindTong(id)` (KTongJX2.cpp:695); bang chủ = member
  btFigure==0 (KTongJX2.cpp:864-881, m_mapTong public); **TONG_Get/ApplySetOccupyCityDay field
  48 ĐÃ đăng ký sẵn** (KTongJX2.cpp:967/:1436) — không phải làm.
- TEST E3: GM script AppointViceroy → restart → mirror giữ; client hiện chủ thành từng thành.

### E4 — Arena idle + Title_ + BT_* + ArenaCredits
- 11 hàm Arena đăng ký đủ nhưng idle (nhánh VN lôi đài chết — DIEUTRA mục 2; đủ để script nạp).
- 7 hàm Title_ theo DIEUTRA mục 4: bảng `settings\playertitle_jx2.txt` (ĐÃ chép ở E1), state
  per-player theo TÊN (map<string> trong C++, không đổi save; active-id script gốc tự lưu
  task 1122); ⚠ khi đăng ký Title_ phải comment 2 call site songjin_shophead.lua:227 (2 bản)
  — nhánh chết sẽ sống dậy (phản biện F9).
- ArenaCredits: remap task id <3000 còn trống (quét trước khi chốt; 3179 vượt MAX_TASK=3000
  im lặng vô hiệu — KPlayerTask.cpp:69-81).
- BT_* 19 hàm: mapping task-value (đề xuất trong báo cáo GS-flow); danh mục hàm battle-infra
  còn thiếu xem DIEUTRA mục 7 (SetNpcDeathScript 11 call site — thiếu là trận không phân
  thắng bại; AddObstacleObj/Clear*, SetPKFlag/ForbidChangePK, SetSiegeVoitureParam,
  SetMangonelParam, GetItemCountEx/DelItemEx, GiveItemUI, RemoveItemByIndex, SetMissionV→
  SetMission alias, GetPlayerRev 2 giá trị, GetLoop, NW_GetSealInfo...). Kèm việc lập
  `PHULUC_HAM_CONGTHANH.md` (bảng đích danh ~163 hàm — phản biện E-PB4-10 đòi).

### E5 — Chép script gốc (NGUỒN = D:\ServerLinux\server1, **CẤM dùng citywar_city.zip**)
- citywar_city 27 file + citywar_global 7 file + citywar_arena 7 file (ghi đè bản Fong Kiều
  đường thật — hệ cũ đã ngắt, an toàn) + `script\mission\citywar_global\ladder.lua` (bản
  RELAY `_RELAY_=1` — target của LG_ApplyDoScript, KHÁC bản missions\) + misc\league_cityinfo
  + task\system\task_string.lua + lib\objbuffer_head.lua + eventhead.lua + 4 item xe công
  thành (mine.txt:32-35 ĐANG trỏ sẵn — item sống dậy, đọc guard trước).
- Sửa MISSIONID trong script chép: city 6→**7**, arena 5→**9**, global giữ **8** + missions.txt
  row 7/8/9 + điểm OpenMission(6)→(7) trong citywar_global\timer.lua (mission 5/6 bị Kiếm Môn
  Quan/Lôi Đài BH chiếm — bảng phân bổ trong KEHOACH §0-Q3).
- IncludeLib bổ sung (LuaIncludeLib nuốt im lặng module lạ — ScriptFuns.cpp:1947-1985):
  FILESYS/SETTING/BATTLE/RELAYLADDER/TITLE/**LEAGUE** (file no-op — LG_ đã là hàm C, đừng trỏ
  jx2compat kẻo Lua đè C!)/**PARTNER** (stub 3 hàm) + ReplaceString + GetLastDiagNpc/
  GetNpcSettingIdx/GetNpcName nếu thiếu.
- Shim ghi chú trong file: add_citybonus_task = get+set; CITYINFO_LEAGUETASK_COUNT=4;
  TV_VALUE nil→0 (đặt trong file port station — GIỮ hành vi từ chối, CẤM đổi thành
  TV_TASKVALUE); station.lua GIỮ bản JX1, port riêng 3 hàm GoCityWar* + CardTab.
- Boot: gọi buildAllCityInfoLeague + checkCityWarLeague (league type 4 + 508/509 — GS gốc
  KHÔNG có ai gọi, league do relay tạo 0h — ta phải tự gọi lúc boot + 0h).
- TEST E5: boot SẠCH; nộp N Khiêu chiến lệnh (item 6/1/1499) → LG_GetMemberTask(508) tăng N
  xuyên state + xuyên restart.

### E6 — Tick 5 pha + vòng đời trận + chuỗi VN
- `CTC_JX2_Tick(nDyfW,nHr,nMi — tham số TÙY CHỌN để GM ép pha test)` trong timerserver.lua:
  đặt SAU các call sẵn có; cửa sổ `>=` + guard ngày+pha kiểu g_nTongMaintainDay (biến CHỈ gán
  trong hàm — file tự dofile mỗi tick); 5 pha: 0h dọn+tạo league / 18h set cờ 508 task1=1 +
  Ladder_ClearLadder(10261) + AddLocalNews / 19h GetRandomChallenger (bốc thăm max điểm 508;
  VÔ CHỦ → AppointViceroy NGAY; có chủ → AppointChallenger; không ai → thái bình; MỌI nhánh hạ
  cờ về 0) / 20h CTC_JX2_SetCityState(ATWAR) + để citywar_global\timer.lua (timer 18, 5 phút)
  poll HaveBeginWar → OpenMission(7). Port ngữ nghĩa cw_*_fun (relay citywar_head.lua:62-145)
  + đầy đủ chuỗi thông báo (bảng 31 chuỗi G_CITYWAR_* mục 4 báo cáo data-agent → octal TCVN3).
- NotifyWarResult phía script: CITY_OCCUPY_R (bản server1 — dòng xóa OccupyCityDay chủ cũ do
  war-result đảm nhiệm), top-10 3M exp (BT_GetTopTenInfo), thông báo; Title Thái Thú
  152+cityid cấp cho tân + THU của cựu; sau trận thua HaveBeginWar phải =0 (không thì poll 5'
  mở lại trận VÔ HẠN — phản biện E-04).
- Deploy timerserver.lua có hiệu lực TỨC THÌ (dofile mỗi tick) — chỉ thả lúc cutover.

### E7 — Client + thuế (KHÔNG protocol mới)
UI id mới append enum UIInfo (KPlayer.h:44-57) qua s2c_scriptaction (khuôn LuaOpenTongJX2
ScriptFuns.cpp:6965) + **page 8** SGDI_TONG_JX2VIEW (KSOServer.cpp:4058-4082, page 4-bit) +
**COP 38** TONG_JX2OP_COMMAND (đặt thuế; enforce 22h-23h StartSetTaxTime/EndSetTaxTime +
MaxExchangeTax; chuỗi G_CITYWAR_WARNING_SETTAXTIME/TAXALREADYSET octal); cửa sổ KUiCityManage
khuôn UiTongGrant; khối protocol thêm GIỐNG HỆT vào CẢ 2 KProtocol.h (Core/Src + Headers —
2 file KHÁC nhau toàn cục, CẤM chép đè nguyên file); build client Win32 + PATCHFULL;
spawn citybulletin (bản gốc) cho các thành đang nạp (mount point Tương Dương =
tuongduong.lua:60 GIỮ NGUYÊN — file citybulletin sẽ được E5 thay ruột).

### E8 — Cutover + bàn giao vận hành
Chạy trọn lịch test ép pha; `BANGIAO_CONGTHANH_DOTE.md` cuối cùng với checklist vận hành
(lịch tuần TB_CITYWAR_ARRANGE: PT T4/T5 · TĐ T2/T3 · ĐL T3/T4 · BK T6/T7 · TD T5/T6 ·
DC CN/T2 · LA T7/CN; báo danh 18-19h nộp Khiêu chiến lệnh — KHÔNG trừ 1M; điều kiện bang
cấp ≥18, ≥37 người; trận 20h map 221, 90 phút; thưởng T2 9-18h huyền tinh 5 pool 300/200;
thuế 22-23h; dụng cụ đổi item 343); restart GS+client đồng bộ.

## III. VIỆC NGOÀI LỀ CÒN NỢ
1. **Phản biện KJx2League bằng agent độc lập** — phiên tự động bị chặn vì HẾT HẠN MỨC TUẦN
   (reset 19/08 9h sáng giờ Mỹ). Tôi đã tự rà theo danh mục bẫy (overload va_list, con trỏ
   sau callback, reentrancy Breathe, wrap GetTickCount, misuse handle, trùng đăng ký) — sạch;
   nhưng khi có hạn mức NÊN chạy bù 1 phiên review độc lập file này (lăng kính 3 trong
   workflow script `phanbien-e1-e2-congthanh-wf_08f21d7e-06e.js` còn nguyên, resume được).
2. Phản biện từng giai đoạn E3-E8 theo quy trình (bằng agent khi có hạn mức, không thì tự rà
   + ghi rõ trong commit).
3. Lỗi thấp chưa xử: LuaSetViewTongOwnCity còn strcpy không giới hạn (sẽ xử trong E3 khi
   de-hardcode); 2 thành 37/176 trống NPC (báo chủ game quyết).

## IV. BẪY PHIÊN SAU PHẢI NHỚ (ngoài các bẫy trong KEHOACH/memory)
- MSVC `va_list = char*`: gọi KLuaScript::CallFunction 4 đối với đối cuối `char*` resolve
  NHẦM overload private → ép `(void*)` (đã dính 1 lần, KJx2League.cpp có comment).
- Heredoc bash nuốt `\` — mọi patch script viết ra FILE rồi chạy, octal TCVN3 dựng bằng
  bytes([92]); patch script khai `# -*- coding: ascii -*-` thì KHÔNG được gõ ký tự Unicode
  (kể cả "…") — dùng utf-8 khi anchor có tiếng Việt.
- File GBK/TCVN3 chỉ vá byte-level với assert hb(); tên thư mục GBK trên đĩa =
  `bytes.decode('mbcs')` (ACP=1252).
- Repo có 2 remote: push `git push gamedevnew master:main` (origin là repo khác!).
- Mirror bắt buộc: bin sửa gì → chép vào serverdata_jx2\ / serverscript_jx2\jx1_edits\ /
  clientdata_jx2\ + PATCHFULL nếu là file client; commit đích danh.
- jx2league.txt / jx2ladder.txt (persist mới) nằm `bin\server\settings\` — sẽ tự sinh khi
  chạy; đừng commit vào repo.
