# KẾ HOẠCH ĐỢT E — CÔNG THÀNH CHIẾN (dựng lại bản gốc Linux JX2, nhánh VN)

**BẢN 2 — 14/08/2026, đã qua phản biện kế hoạch** (5 phiên: fact-check V1-V8 + 4 lăng kính,
57 phát hiện, 10 blocker — toàn bộ đã hấp thụ vào bản này). Spec kỹ thuật 41 hàm + tham số
vận hành gốc: xem `DIEUTRA_CONGTHANH_BINARY.md` (cùng thư mục). Bản 1 sai ở đâu thì bản này
đã sửa tại chỗ — KHÔNG đọc bản 1 nữa.

Lệnh chủ game: "Làm luôn tính năng công thành E / Phần nay phải chạy phản biện điều tra cho
kỹ / Phải làm đúng bản gốc linux bỏ phần có sẵn mã nguồn củ".

## 0. BA QUYẾT ĐỊNH KIẾN TRÚC LỚN (chốt sau phản biện)

**Q1 — Theo NHÁNH VN của bản gốc**: cây ta `GetProductRegion()` cố định "vn". Ở nhánh VN:
lôi đài 213-220 là hệ CHẾT (engine gốc không đăng ký hàm báo danh lôi đài; safeshow undefined);
khiêu chiến giả chọn bằng **BỐC THĂM 19h** giữa các bang nộp Khiêu chiến lệnh nhiều nhất
(`GetRandomChallenger`, relay citywar_head.lua:62-97); **thành VÔ CHỦ → bổ nhiệm Thái Thú NGAY
không cần đánh** (citywar_head.lua:131-135 — đây là đường khai trương 7 thành, binary s3relay_y
CÓ GetCityOwner, kết luận "relay bug chết pha 2" của điều tra trước là SAI). Nhóm hàm Arena vẫn
đăng ký đủ cho script nạp sạch nhưng để idle như gốc.

**Q2 — Mọi kho chia sẻ phải nằm ở C++**: engine ta nạp MỖI file .lua một Lua state riêng
(KSortScript.cpp:100-113) → store bằng bảng Lua (jx2compat) là per-state, VÔ HÌNH giữa NPC ↔
đồng hồ ↔ callback. Do đó League (508/509 + type 4 cityinfo + cờ báo danh), Ladder, GlbValue,
ObjBuffer, Title, CityWar state — TẤT CẢ là store C++ toàn cục, đăng ký hàm C cho mọi state,
persist file (khuôn tmp + MoveFileEx replace; TỰ VIẾT writer text, KHÔNG dùng KIniFile::Save
— nó cắt trắng file rồi mới ghi và vứt comment). jx2compat (bin\server\scriptjx2\jx2compat.lua)
giữ nguyên cho gb_task/gb_module (mod 500/502) — các state citywar không include nó.

**Q3 — Phân bổ tài nguyên tránh đâm hệ sống** (bảng cố định, các đợt sau tra ở đây):
| Tài nguyên | Gốc JX2 | Ta dùng | Lý do |
|---|---|---|---|
| Mission id | city=6, arena=5, global=8 | **city=7, arena=9, global=8** | 5=Kiếm Môn Quan, 6=Lôi Đài BH đang sống; 7/8/9 trống (mission08.lua stub 0 caller). Sửa hằng MISSIONID trong script gốc + row missions.txt + điểm OpenMission trong citywar_global\timer.lua — lệch-gốc-bắt-buộc, hành vi giữ nguyên |
| Timer id | 12/13 city, 16/17 arena, 18 global | **giữ nguyên văn**; DỜI PUBG 12/13 → 14/15 | citywar hardcode nhiều chỗ, PUBG chỉ giương 2 điểm (timerserver.lua:105, timertask\pubg.lua:61) + 2 row TimerTask.txt |
| Tong field | 1053/1054 CLAIMWAR_DATE/TIMES + field 48 | **y gốc 1053/1054 + 48**; id MỚI tự chế ≥1300 | 1100-1107 = log quỹ Lua đợt C, 1101-1103 = tuyệt kỹ C++ đợt B — CẤM đụng |
| Task người chơi | ArenaCredits 3179; active title 1122 | ArenaCredits remap **id <3000 còn trống** (quét lúc làm); 1122 giữ | MAX_TASK=3000 — 3179 im lặng vô hiệu (KPlayerTask.cpp:69-81) |
| Bảng danh hiệu | settings\playertitle.txt | **settings\playertitle_jx2.txt** | tên gốc ĐÈ PlayerTitle.txt quân hàm TK đang sống (CoreUseNameDef.h:149, FS không phân biệt hoa thường) |
| WorldSet | — | GHI ĐÈ vùng chết **World044..048 = 221,222,223,37,176; Count=49** | CẤM "nâng Count" — World044-118 là 75 dòng di sản trùng map sống (78/1/2/11/227/53...); index World000-043 giữ nguyên tuyệt đối. Map 220 ĐÃ nạp (World022); 213-219 không nạp (arena idle) |
| Protocol client | 0xAC/0xA3/0x97 | **KHÔNG protocol mới**: UI id mới append enum UIInfo (KPlayer.h:44-57) qua s2c_scriptaction (khuôn LuaOpenTongJX2 ScriptFuns.cpp:6965) + **page 8** SGDI_TONG_JX2VIEW (4-bit page, mới dùng 0-7) + **COP 38** trong TONG_JX2OP_COMMAND | tái dùng đường sẵn có, không đụng bảng size protocol. Khối define mới thêm GIỐNG HỆT vào CẢ 2 KProtocol.h (Core/Src + Headers) cùng commit — 2 file này KHÁC nhau toàn cục, chỉ KHỐI JX2 thêm vào là phải giống, CẤM chép đè nguyên file |

## 1. KIẾN TRÚC

### 1.1 Đồng hồ 5 pha tại GS (relay ta không có Lua, không sửa relay)
`CTC_JX2_Tick(nDyfW, nHr, nMi)` trong timerserver.lua — **nhận tham số TÙY CHỌN để GM ép pha
test** (engine không có cơ chế ép giờ; GetLocalDate = localtime thật). Điều kiện thực tế:
RunTime chỉ chạy ~1 lần/phút tại giây 0 và CÓ THỂ MISS → mọi mốc so bằng CỬA SỔ `>=` + guard
"ngày+pha đã chạy" kiểu g_nTongMaintainDay (biến g_CTC_* CHỈ gán trong hàm — timerserver tự
dofile lại mỗi tick, top-level bị reset); đặt SAU các call sẵn có trong RunTime để lỗi không
kéo sập Tống Kim/bảo trì bang; **deploy timerserver.lua có hiệu lực TỨC THÌ không cần restart**
— chỉ thả file ở thời điểm cutover.

5 pha (port nguyên ngữ nghĩa cw_*_fun của relay gốc, chạy trong state timerserver qua Include):
- **0h00**: clearCityWarLeague (dọn member 508/509) + checkCityWarLeague (TẠO league nếu chưa có
  — thiếu là báo danh chết im); cũng gọi 1 lần lúc boot.
- **18h00** (thành X theo TB_CITYWAR_ARRANGE): set cờ báo danh league-task(508, city, 1) = 1 +
  Ladder_ClearLadder(10261) + AddLocalNews.
- **19h00**: GetRandomChallenger (max Khiêu chiến lệnh, hòa thì random) → thành vô chủ:
  citywar_appointviceroy NGAY; có chủ: citywar_appointchallenger + thông báo; không ai: thông báo
  thái bình; MỌI nhánh: hạ cờ về 0. (Callback gốc gate SubWorldID2Idx(2) — map 2 đang nạp World008, giữ.)
- **20h00** (thành Y): mở trận — OpenMission(7) map 221 theo khuôn citywar_global\timer.lua
  (poll HaveBeginWar 5 phút — giữ nguyên cơ chế gốc qua timer 18 + StartGlbMSTimer).
- Kết trận (≤21h30, GAMETIME 90'): mission scripts tự gọi NotifyWarResult.

### 1.2 State + persist (KCityWarJX2, Core, _SERVER)
- `Sources\Core\Src\KCityWarJX2.{h,cpp}` — struct SẠCH theo DIEUTRA_CONGTHANH_BINARY.md §1
  (CẤM sao offset binary 32-bit). Loader đọc `settings\citywar.ini` bằng KIniFile (an toàn:
  value giữ nguyên byte TCVN3, key phải ASCII — đã kiểm KIniFile.cpp:492-501,823-825).
- Persist: tong field 48 + 1053/1054 (relay giữ, sống qua restart) + mirror
  `settings\citywar_state.ini` (city → owner/master/tax/priceparam/ngày chiếm; ghi theo SỰ KIỆN,
  khuôn tmp+rename); kho League + Ladder persist cùng khuôn. Boot: đọc mirror → dựng state +
  đồng bộ KSubWorld + re-grant Title Thái Thú.
- Đồng bộ thuế: khi đổi chủ/thuế → cập nhật `m_CityOwnTong/m_CityTax` cho **MỌI instance**
  subworld có map id đó (3/7 thành nạp 2 subworld — SearchWorld chỉ trả cái đầu, CẤM dùng đơn lẻ);
  guard index<0 (map chưa nạp) + strncpy 31+NUL + clamp thuế 0..MaxExchangeTax trước khi gán BYTE.
- De-hardcode map 78 ở CẢ BA: LuaSetThueTongOwnCity/LuaSetViewTongOwnCity (ScriptFuns.cpp:7002-7028)
  VÀ KPlayer::GetCityOwnTong (KPlayer.cpp:6370-6377 — trả lời client, thiếu là client chỉ thấy Tương Dương).

### 1.3 Script gốc: đặt Ở ĐƯỜNG THẬT (tự được nạp lúc boot — LoadAllScript quét đĩa đệ quy;
trap/NPC trong map data gọi theo hash đường dẫn nên KHÔNG remap được; V8 xác nhận không cần cờ
môi trường nào — _RELAY_/_GAMESERVER_ là local hardcode từng bản file). Nguồn:
- `citywar_city\` = **D:\ServerLinux\server1 GỐC là CHUẨN — KHÔNG dùng zip** (đảo E-PB4-15,
  chốt 14/08 bằng quét ScriptID trap nhúng trong 292 region vn: map vn gọi ĐÚNG 8 trap gốc
  ctrap1/1b/1c/2/2b/2c + trap1/trap2 tại đúng cụm tọa độ gốc, KHÔNG gọi tuyen* nào; zip là bản
  ai đó dời tọa độ cho nửa map `剑门关` cũ — CS_CampPos1 zip (1280,2912)=cell v091/f080 nằm
  NGOÀI rect vn). File mồ côi bản chế (zhongzhuan_map\yaoshang.lua) giữ + ngắt kích.
- `citywar_global\` +7 file, `citywar_arena\` +7 file, `misc\league_cityinfo.lua`,
  `task\system\task_string.lua`, `lib\objbuffer_head.lua`, eventhead.lua... từ D:\ServerLinux\server1.
- **2 bản ladder.lua khác nhau**: `missions\citywar_global\ladder.lua` (GS-side, được nạp) và
  `mission\citywar_global\ladder.lua` (số ít — bản relay, chỉ là TARGET của LG_ApplyDoScript).
  LG_ApplyDoScript của ta = thực thi CỤC BỘ đồng bộ tương đương AddOneInRelay
  (LG_ApplyAppendMemberTask 508 + Ladder_NewLadder 10261).
- `station.lua`: GIỮ bản JX1 đang sống. Port riêng 3 hàm GoCityWar/GoCityWarAttack/GoCityWarDefend
  + bảng CardTab vào file mới cạnh citywar_global, nối vào menu trạm dịch/xa phu; vá TV_VALUE
  (nil→0, GIỮ hành vi từ-chối như gốc, CẤM "sửa đúng ý" thành TV_TASKVALUE — sẽ MỞ đường vào
  không có ở gốc) đặt tại file port này. chefu.lua (zip) include station.lua → nạp bản JX1: test
  thoại xa phu 222/223.
- 4 item xe công thành ({gongchengche,toushiche,leigushibing,yaoqishibing}.lua): mine.txt:32-35
  ĐANG trỏ sẵn → chép file là item SỐNG DẬY với bất kỳ ai đang giữ — đọc guard gốc trước, chỉ
  chép ở E5 (sau khi hàm guard tồn tại).
- Shim/vá tối thiểu (ghi chú trong file): `add_citybonus_task` = get+set (gốc KHÔNG định nghĩa
  — nil-call đường thưởng tuần), `CITYINFO_LEAGUETASK_COUNT` = 4 (gốc thiếu), safeshow KHÔNG
  tồn tại trong gốc (chỉ cần nếu kích arena — bỏ), TaskSay KHÔNG phải bug (hàm Lua thật trong
  task_string.lua — KHÔNG alias sang Say).

### 1.4 IncludeLib bổ sung (LuaIncludeLib bỏ qua IM LẶNG module lạ — ScriptFuns.cpp:1947-1985):
FILESYS, SETTING, BATTLE, RELAYLADDER, TITLE, **LEAGUE** (5 call site — trỏ file no-op vì LG_ đã
là hàm C), **PARTNER** (task_string.lua — stub PARTNER_GetCurPartner/GetSettingIdx/GetName trả
0/""). Kèm ReplaceString + GetLastDiagNpc/GetNpcSettingIdx/GetNpcName nếu ta chưa có.

### 1.5 Kinh tế + danh hiệu
- Báo danh = nộp ITEM Khiêu chiến lệnh (6/1/1499), KHÔNG trừ tiền quỹ (SignUpFee 1M thuộc đường
  đấu giá chết ở nhánh VN — bỏ khỏi scope); hoàn lệnh 19h-24h khi thua bốc thăm
  (checkIsTakeQingtongDing). Điều kiện bang: cấp ≥18 (MinTongLevel), nhân số ≥37 (MinTongCrowNumber
  — chỉnh citywar.ini theo gốc 37, hiện ta 60: ghi bàn giao cho chủ game).
- Thái Thú: title id 152+cityid từ playertitle_jx2.txt; AppointViceroy CẤP cho tân + **THU của
  cựu** (Title_RemoveTitle) — kể cả đường đổi chủ qua NotifyWarResult; boot re-grant từ mirror.
- ⚠ Đăng ký Title_* sẽ ĐÁNH THỨC nhánh chết songjin_shophead.lua:227 (2 bản) đang gọi
  Title_AddTitle/SetTask(1122) — comment 2 call site đó trong cùng đợt E6, ghi bàn giao.

## 2. GIAI ĐOẠN (mỗi giai đoạn: làm → build 4 đích → TEST theo mục riêng → phản biện → commit đích danh → push)

**E1 — Ngắt hệ cũ + nền dữ liệu** (lệnh "bỏ mã cũ" thi hành NGAY để các giai đoạn sau an toàn):
- Ngắt đường kích CTC cũ còn SỐNG: balanghuyen.lua:79 (NPC 373 báo danh — đường sống duy nhất,
  đồng hồ :70 đã comment sẵn), playerlogin.lua:23 + :122-131 (ghi ngược thuế map 78 mỗi login),
  lệnh CTC trong lenhbaiadmin.lua/lenhbaitanthu.lua. GIỮ: bia tuongduong.lua:60 (AddObj →
  citybulletin.lua — là mount point của hệ mới, file sẽ được thay ruột ở E5), Include lib_ctc
  timerserver.lua:11 (gỡ sẽ nil-call dây chuyền — bài học jxsv-r168), row mission 2, task01/02.
- WorldSet: ghi đè World044..048 = 221,222,223,37,176; Count=49; sửa comment World022 (=220).
  ⚠ 37/176 là 2 map CHƯA TỪNG mở trên server ta — kiểm NPC/spawn tối thiểu, ghi bàn giao.
- TimerTask.txt: 12→citywar_city\timer.lua, 13→citywar_city\totaltimer.lua, 14/15=pubg (dời),
  16/17=citywar_arena, 18=citywar_global + sửa 2 điểm giương PUBG (timerserver.lua:105 12→14,
  timertask\pubg.lua:61 13→15). Row 12/13/16/17/18 trỏ script chưa tồn tại = ngủ, vô hại.
- MapList server + client: `221=特殊用地\剑门关vn` (bỏ ý NOTONGCLAIMWAR — engine ta không parse
  NewWorldParam, text chết). Client KHÔNG cần chép data (剑门关vn nằm sẵn update03.pak; maps2\ là
  kho trích, client không đọc — 'maps2' 0 hit mọi binary).
- Server map data: viết tool trích XPack (hash lookup3 co-slash+signed; sinh danh sách đường
  v_YYY\XXX từ rect trong .wor) → trích `剑门关vn` (.wor + 509 region) từ
  D:\ServerLinux\server1\pak\maps.pak ra **bin\server\Maps\特殊用地\剑门关vn\** (loose THẮNG pak
  — KPakFile mode 0 đọc đĩa trước, g_SetPakFileMode không ai gọi → KHÔNG cần repack).
- citywar.ini: merge 4 key thiếu (StartSetTaxTime=22, EndSetTaxTime=23, WarCycleValue=7,
  SupplyLineBuildScale=30) + sửa **3** giá trị (MaxExchangeTax 15→20, MaxPriceParam 15→20,
  MinTongCrowNumber 60→37) + làm sạch CR thừa dòng AreaName03 — tool byte-level.
- npcs.txt dòng 627: tên "Rương chứa đồ" theo gốc (byte TCVN3) — LƯU Ý mọi rương dùng template
  625 toàn server sẽ hiện tên (trước rỗng) — ghi bàn giao.
- Chép settings\playertitle_jx2.txt (đổi tên từ playertitle.txt gốc — CẤM đè PlayerTitle.txt).
- TEST E1: boot sạch; GM goto 221/222/223 + soát 9 tọa độ trận (head.lua:81-94); goto 37/176;
  PUBG vẫn chạy với timer 14/15; thuế Tương Dương cũ ngừng cập nhật (chấp nhận — ghi bàn giao).

**E2 — Engine C++ nền** (khuôn 3 tầng như KTongJX2: PCH #include "KCore.h" TRƯỚC #ifdef _SERVER;
extern + đăng ký TRONG khối #ifdef _SERVER của ScriptFuns.cpp; Core.vcxproj ExcludedFromBuild
4 cấu hình Client + .filters; build Client Win32 NGAY sau khi thêm file để bắt C2065 sớm):
- OB_* 13 hàm (pool 4KB, lô 16) · Ladder_* 3 hàm (store + persist; id>10000; trả ("",0,-1,0);
  int64 so sánh phía C++) · **LG_* engine-side** (store namespace theo nMod + persist 508/509/538
  + type 4; LG_ApplySetLeagueTask dạng 6 tham số PHẢI gọi callback đồng bộ — khóa
  league_cityinfo không thì kẹt vĩnh viễn; LG_ApplyDoScript cục bộ; đủ danh sách:
  LG_GetMemberCount/GetMemberInfo/ApplyDoScript/LGM_ApplyRemoveMember/LGM_FreeMemberObj + arity
  đã ghi ở điều tra) · Set/GetGlbValue · StartGlbMSTimer/StopGlbMSTimer · ArenaCredits remap ·
  alias SetMissionV→SetMission, SetMissionS... · nhóm hàm §7 DIEUTRA (SetNpcDeathScript,
  AddObstacleObj, SetPKFlag, SetSiegeVoitureParam...).
- **Lập PHULUC_HAM_CONGTHANH.md**: bảng đích danh ~163 hàm (sẵn/alias/mới/stub — giai đoạn đăng
  ký từng hàm); phản biện các giai đoạn sau tick theo bảng này.
- TEST E2: script GM round-trip OB/Ladder/LG (2 state khác nhau phải thấy CÙNG kho); restart
  giữ kho.

**E3 — KCityWarJX2 nhóm CITY** (§1.2 + DIEUTRA §1): 12+ hàm City, mirror, boot-load, sync đa
instance, de-hardcode 3 điểm map 78, GetCityOwner/GetCityAreaName/GetCityArea phục vụ cả 9+ hệ
ngoài công thành. TEST: GM AppointViceroy → restart → mirror + field 48 + title giữ đúng; client
hiện chủ thành từng thành (s2c_returncityowntong).

**E4 — Nhóm ARENA idle + Title_**: 11 hàm Arena (đăng ký đủ, idle nhánh VN) + 7 hàm Title_ +
playertitle_jx2 loader + BT_* 19 hàm (mapping task-value theo điều tra GS-flow). TEST: nạp thử
titlefuncs.lua; Title_GetTitleTab trả bảng/nil chuẩn.

**E5 — Chép script gốc + lib + station-port + item + shim** (§1.3-1.4; sửa MISSIONID 6→7 /
5→9 + row missions.txt 7/8/9 + điểm OpenMission trong citywar_global\timer.lua; spawn
citybulletin cho các thành đang nạp; boot gọi buildAllCityInfoLeague + checkCityWarLeague).
TEST: boot SẠCH ScriptError.log; NPC ArenaMain thoại được; nộp N Khiêu chiến lệnh →
LG_GetMemberTask(508) tăng N (xuyên state + xuyên restart).

**E6 — Vòng đời trận + tick 5 pha + chuỗi VN**: CTC_JX2_Tick (§1.1) + NotifyWarResult đầy đủ
(công thắng: đổi chủ+master; MỌI nhánh: clear challenger + state thường — thiếu là poll 5' mở
lại trận VÔ HẠN; CITY_OCCUPY_R theo bản server1: đường war-result tự xóa OccupyCityDay chủ cũ;
top-10 3M exp; AddLocalNews) + Title Thái Thú cấp/thu + comment 2 call site songjin + **bảng
mapping 31 chuỗi G_CITYWAR_*** (stringtable_relay.txt:123-153 → octal TCVN3, rải đúng hàm:
WARNING_* nhóm báo danh ở E5/E6, WAR_RESULT/CHANGE_CITYMASTER ở NotifyWarResult, TAX_SET ở E7).
TEST: ép pha bằng CTC_JX2_Tick(dyfW,hr,mi) — 2 bang test chạy trọn: 18h mở → nộp lệnh → 19h bốc
thăm (cả 3 nhánh: vô chủ / có chủ / thái bình) → 20h trận map 221 (trụ/cổng/xe/rương/mật đạo) →
kết trận cả 2 chiều thắng/thua → không mở lại trận → thưởng + title đúng.

**E7 — Client + thuế**: UI id mới (enum UIInfo) + page 8 SGDI_TONG_JX2VIEW + COP 38 đặt thuế
(enforce cửa sổ **22h-23h** StartSetTaxTime/EndSetTaxTime + MaxExchangeTax; chuỗi
G_CITYWAR_WARNING_SETTAXTIME/TAXALREADYSET); cửa sổ `KUiCityManage` khuôn UiTongGrant; khối
protocol thêm GIỐNG HỆT 2 file KProtocol.h; build client Win32 + PATCHFULL. TEST: Thái Thú mở
UI tại NPC, đặt thuế trong/ngoài khung giờ, thuế trừ đúng ở giao dịch cả 2 instance map.

**E8 — Cutover + vận hành**: thả timerserver.lua (hiệu lực tức thì) đồng bộ với restart
GS+client; chạy trọn 1 lịch thật; `BANGIAO_CONGTHANH_DOTE.md` với **checklist vận hành đầy đủ**:
lịch tuần từng thành (PT T4/T5 · TĐ T2/T3 · ĐL T3/T4 · BK T6/T7 · TD T5/T6 · DC CN/T2 ·
LA T7/CN, báo danh 18-19h, đánh 20h, 90 phút), điều kiện (cấp 18, 37 người, Khiêu chiến lệnh),
thưởng T2 9-18h (BK/LA 300 / thành thường 200 huyền tinh 5, bang chủ phần dư 18-20h), thuế
22-23h, dụng cụ đổi item 343, 2 map 37/176 lần đầu mở, tên rương 625 đổi toàn server, thuế bia
Tương Dương cũ đã thay bằng hệ mới.

## 3. BẢNG BUG GỐC — CÁCH XỬ (giữ hành vi gốc trừ khi CHẾT script; mọi vá ghi chú trong file)
| Bug | Xử |
|---|---|
| TV_VALUE undefined (station GoCityWar) | nil→0 tại file port — GIỮ hành vi từ chối; CẤM đổi thành TV_TASKVALUE |
| add_citybonus_task + CITYINFO_LEAGUETASK_COUNT undefined | shim get+set + define =4 (đường thưởng tuần) |
| safeshow undefined | KHÔNG tồn tại trong gốc; chỉ cần nếu kích arena — bỏ |
| `Tong2,Tong1 = GetCityWarBothSides(...)` đảo tại city\mission.lua:86 | GIỮ NGUYÊN |
| AppointViceroy nhận TÊN nhưng sevencity truyền id | sevencity NGOÀI scope — không chép |
| GetPlayerRev | implement đúng **2 giá trị trả** |
| TaskSay | KHÔNG phải bug — hàm Lua task_string.lua; cần PARTNER/ReplaceString (§1.4) |

## 4. RỦI RO CÒN LẠI
1. 37/176 mở lần đầu: nội dung NPC 2 thành có thể trống — kiểm ở E1, báo chủ game.
2. Trap wrapper Fong Kiều trên map 221 sống từ E1 đến E5 (GM dẫm sẽ gọi hệ cũ/lỗi) — không có
   đường vào tự nhiên cho người chơi (NPC báo danh cũ đã cắt, GoCityWar chưa có); chấp nhận.
3. Kho league đổi từ RAM-Lua sang C++ — mọi hệ khác đang dùng mod 500/502 KHÔNG đổi đường
   (state của chúng vẫn dùng bản Lua per-state như cũ); nếu sau này cần hợp nhất thì làm đợt riêng.
4. citywar.ini vẫn là file "chết" với relay/client ta — chỉ GS đọc; đổi giá trị phải restart GS.
5. Chuỗi VN nào thiếu trong stringtable sẽ lộ khi test E6 — bảng mapping là living doc.

## 5. QUY TRÌNH CỐ ĐỊNH (không thương lượng)
`core.autocrlf=false` · CẤM đẩy bin/ (mirror repo: serverscript_jx2\ + serverdata_jx2\) · commit
đích danh từng file · TCVN3 chỉ qua safe_edit/python bytes · chuỗi VN mới = octal TCVN3 · build
4 đích trước commit · sửa client → PATCHFULL · mỗi giai đoạn có phản biện riêng · claim mới phải
kèm file:line · GS+client phát hành cùng lúc khi protocol/hành vi đổi.
