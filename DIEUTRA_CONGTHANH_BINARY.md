# ĐẶC TẢ 41 HÀM ENGINE CÔNG THÀNH (dịch ngược `jx_linux_y` 14/08/2026)

File này cố định kết quả phiên dịch ngược binary để E2-E6 có spec trên đĩa (KEHOACH_CONGTHANH_DOTE.md tham chiếu tới đây).
**QUY TẮC VÀNG: struct C++ của ta là struct SẠCH theo chuẩn cây ta (char[32], int, BOOL...).
TUYỆT ĐỐI không sao chép offset/stride/byte-layout của binary Linux 32-bit (0x72, +0x11...) —
các con số đó chỉ là BẰNG CHỨNG ngữ nghĩa, không phải khuôn dữ liệu.**

Quy ước: engine JX2 = Lua 4.0, hàm `int Lua<Name>(lua_State*)`; "trả N" = số giá trị push cho Lua.

## 1. NHÓM CITY — state 7 thành

Bản gốc: mảng cố định 7 phần tử (index 1..7), KHÔNG phải map động. Mỗi phần tử:
`btState (0..5)`, `szOwnerTong[32]`, `szMaster[32]` (tên NHÂN VẬT Thái Thú), `szChallengerTong[32]`,
`nTax`, `nPriceParam` (−1 → hiển thị 10). Toàn cục: `bEnable`, `nCurArenaTargetCity`.
Cấu hình nạp từ `\settings\citywar.ini` ([CityArea] AreaIncludesNN = map id; [InitCityMaster] seed, mặc định tắt).
Người ghi owner = AppointViceroy/AppointChallenger + NotifyWarResult.

| Hàm | In | Out | Chi tiết bắt buộc |
|---|---|---|---|
| `GetCityOwner(nCityID)` | 1..7 | **2**: szOwnerTong, szMaster | fail → `"",""`. Giá trị 2 là tên NHÂN VẬT (citybulletin.lua:15 so với GetName()) |
| `GetCityWarBothSides(nCityID)` | 1..7 | **2**: szChallenger, szDefender | thứ tự (CÔNG, THỦ); chỉ trả khi CẢ HAI non-empty, ngược lại `"",""` |
| `GetCityAreaName(nCityID)` | 1..7 | 1 chuỗi | sai id → chuỗi mặc định |
| `GetCityArea()` | — | 1 số 0..7 | khu của MAP người chơi đang đứng (tra AreaIncludes theo subworld) |
| `GetCitySummary(nCityID)` | 1..7 | 1 chuỗi | sprintf(tên thành) + mô tả theo btState (6 nhánh) |
| `GetAllCitySummary()` / `SyncCitySummary(id...)` | 0 / 1..7 id | **0** | gốc gửi gói 0xAC cho client JX2 — client ta không hiểu → **no-op có chủ đích** |
| `OpenCityManageUI(nCityID)` | 1..7 | 0 | gốc gửi 0xA3 [BYTE 1][DWORD tax1][DWORD tax2] — ta thay bằng UI riêng (xem kế hoạch E7); điều kiện gốc: đang đối thoại đúng NPC |
| `HaveBeginWar(nCityID)` | 1..7 | 1 BOOL | |
| `NotifyWarResult(nCityID, bAttackerWin)` | ≥2 | 0 | ta xử lý TRỰC TIẾP: đổi chủ (nếu công thắng) + LUÔN clear challenger + state về thường + CITY_OCCUPY_R + thưởng + thông báo |
| `AppointViceroy(szCityName, szTongName)` | 2 CHUỖI (TÊN THÀNH, không phải id) | 0 | kèm cấp/thu Title Thái Thú (id 152+cityid) |
| `AppointChallenger(szCityName, szTongName)` | 2 chuỗi | 0 | |
| `IsSigningUp(nCityID)` | 1 | 1 BOOL | |
| `GetSignUpTongName` / `NumOfSignUpTongs` | | | kho báo danh (league 508) |
| `TONG_GetOccupyCityDay(nTongID)` | 1 | 1 số | nối tong field 48 (đã có sẵn getter/setter C++) |
| `TONG_ApplySetOccupyCityDay(nTongID, nDay)` | 2 | 0 | |
| `DisabledChatCity(nFlag)` / `IsDisabledChatCity()` | 1 / 0 | 1 / 1 | cờ bit trên NGƯỜI CHƠI (không phải thành); 0 call site Lua → stub an toàn |
| `GetCityCount()` | — | 1 (=7) | ⚠ trùng tên JX1 (trạm dịch, ScriptFuns.cpp:8869) → nếu cần thì đăng ký `GetCityAreaCount`; 0 call site → có thể bỏ |
| `GetCity()` | — | — | JX2 vốn là STUB RỖNG, 0 call site → GIỮ NGUYÊN bản JX1 (khác hẳn) |

## 2. NHÓM ARENA — 8 khe lôi đài (map 213-220, ArenaID 0..7 = WorldID − 213)

⚠ PHÁT HIỆN PHẢN BIỆN: nhánh VN của bản gốc KHÔNG chạy lôi đài (engine không đăng ký
SignUpCityWarArena; chọn khiêu chiến giả bằng BỐC THĂM 19h phía relay). Nhóm này đăng ký ĐỦ
để script nạp không lỗi nhưng để idle như gốc VN.

| Hàm | In | Out | Fail |
|---|---|---|---|
| `IsArenaBegin(nArenaID)` | 0..7 | 1 (0/1) | "đã bắt đầu" = khe có đủ 2 phe |
| `GetArenaBothSides(nArenaID)` | 0..7 | **2** chuỗi (giáp, ất) | `"",""` |
| `GetArenaCityArea(nArenaID)` | ≤7 | 1 = CityID | |
| `GetArenaLevel()` | — | 1 (0 = chưa/xong) | fail → **0 giá trị** |
| `GetArenaTargetCity()` | — | 1 CityID (0 nếu không) | |
| `GetArenaTotalLevel()` / `GetArenaTotalLevelByCity(n)` | 0 / 1 | 1 | fail → 0 giá trị |
| `GetArenaSchedule(nCityID)` | 1..7 | 1 chuỗi bảng lịch | stub "" được (call site Say) |
| `GetArenaInfoByCity(nCityID[,nLevel])` | ≥1 | 1 chuỗi | call site duy nhất đang bị comment |
| `NotifyArenaResult(nArenaID, bSide1Win)` | **≥2** | 0 | |
| `GetCityWarTongCamp(szTongName)` | 1 CHUỖI | 1 số / nil | |

`Get/Set/Add/ReduceArenaCredits`: bản chất = đọc/ghi **task value id 3179** của người chơi.
⚠ MAX_TASK ta = 3000 (KPlayerTask.h:13) → 3179 im lặng vô hiệu. **Remap sang id < 3000 còn
trống** (quét trước khi chốt); 0 call site Lua nên chỉ cần alias đúng ngữ nghĩa.

## 3. NHÓM LADDER — bảng xếp hạng (gốc: gói 52B lên relay; ta: store C++ tại GS + persist)

Ràng buộc phải giữ y nguyên:
- `nLadderId > 10000` bắt buộc (cả New lẫn Clear).
- `Ladder_NewLadder(id, szName, nValue [,nType][,nSect][,nGender])` — ≥3 tham số, szName non-empty,
  nValue là int64 (Lua double chính xác tới 2^53 — đủ), trả 0.
- `Ladder_ClearLadder(id)` — trả 0.
- `Ladder_GetLadderInfo(id, nRank)` — ≥2 tham số, nRank 1..10, trả **4**: szName, nValue, nSect, nGender;
  ô trống → `("", 0, -1, 0)` — **CHUỖI RỖNG, KHÔNG nil** (ladderfunlib.lua:20 kiểm `~= nil` luôn đúng).
- Entry: szName[32] + int64 value + int nSect (mặc định −1) + BYTE gender + BYTE type (cờ ngữ cảnh, không ảnh hưởng xếp hạng).
- `ShowLadder` → dùng `LuaShowLadder` JX1 sẵn có (ScriptFuns.cpp:11041), KHÔNG port.
- Citywar dùng ladder id 10261 (điểm Khiêu chiến lệnh, reset 18h mỗi phiên báo danh).

## 4. NHÓM TITLE — hệ danh hiệu ĐỘC LẬP (không đụng quân hàm Tống Kim JX1)

⚠ JX1 đã có `Get/SetPlayerTitle...` = BYTE quân hàm TK trên Npc — hệ KHÁC, để nguyên.
⚠ Bảng cấu hình gốc `settings\playertitle.txt` **ĐÂM TÊN FILE** với `settings\PlayerTitle.txt`
quân hàm TK đang được engine đọc (CoreUseNameDef.h:149, Windows không phân biệt hoa thường)
→ ta đặt **`settings\playertitle_jx2.txt`**. 7 dòng Thái Thú = id 153-159 (= 152 + cityid).

State per-player: map<titleId, {nTimeType, nTime}> + nActiveTitle (+ thời hạn sống của title active).
KHÔNG đổi save format — active-id persist qua task 1122 (chính script gốc làm, titlefuncs.lua),
ownership Thái Thú cấp lại từ mirror lúc boot; danh hiệu event khác không sống qua restart (giới hạn ghi nhận).

| Hàm | In | Out | Chi tiết |
|---|---|---|---|
| `Title_AddTitle(nId, nTimeType, nTime)` | **≥3**, nId>0 | 0 | nTimeType GIỐNG AddSkillState: 0=vĩnh viễn, 1=FRAME (giây×18), 2=mốc tuyệt đối dạng số `MMDDHHMM` |
| `Title_ActiveTitle(nId)` | 1 (0 = tắt hết) | 1 số | tắt title cũ (gỡ 5 skill-state phụ) rồi bật mới |
| `Title_RemoveTitle(nId)` | 1 | 0 | |
| `Title_GetTitleInfo(nId)` | 1 | **2**: nTimeType, nTime | không sở hữu → `(0,0)`; title active trả thời hạn "sống" |
| `Title_GetTitleName(nId)` | 1 | 1 chuỗi | không có → `""` |
| `Title_GetActiveTitle()` | — | 1 số | 0 nếu không |
| `Title_GetTitleTab()` | — | 1 BẢNG các id / **0 giá trị (nil)** khi không có gì | Lua 4: `lua_newtable` + Lua_SetTable (tiền lệ ScriptFuns.cpp:7781) |

## 5. NHÓM OB — ObjBuffer (pool buffer chia sẻ giữa các Lua state)

Pool ĐỘNG: map<int handle, KObjBuffer*> + free-list, grow theo LÔ 16, buffer 4096 byte/handle,
handle = số nguyên tăng dần (>0), thread-safe. Spec ngữ nghĩa đầy đủ nằm trong
`script\lib\objbuffer_head.lua` gốc (PushObject/PopObject theo type byte 1=NUMBER 2=STRING 3=TABLE 4=NIL).

Bộ hàm BẮT BUỘC đủ 13 (thiếu 1 là chết cả ObjBuffer):
`OB_Create()→handle(0=fail)`, `OB_Release(h)→1`, `OB_IsEmpty(h)→1 (1=rỗng/invalid, handle≤0 invalid)`,
`OB_Clear`, `OB_Append`, `OB_Copy`, `OB_PushByte/PopByte`, `OB_PushInt/PopInt`,
`OB_PushDouble/PopDouble`, `OB_PushString/PopString`.

## 6. GÓI TIN CLIENT JX2 (chỉ để biết — ta KHÔNG dùng)

- 0xAC = city summary (809B, 7 thành) → ta no-op.
- 0xA3 sub 1 = mở UI quản thành [DWORD tax1][DWORD tax2] → ta thay bằng đường JX1: UI id mới
  (enum UIInfo, KPlayer.h:44-57) qua s2c_scriptaction + page 8 SGDI_TONG_JX2VIEW + COP 38.
- 0x97 = ShowLadder → JX1 s2c_ladderlist sẵn có.

## 7. HÀM ENGINE THIẾU PHÁT HIỆN THÊM (fact-check + phản biện, phải có trong E2-E4)

- `StartGlbMSTimer(nMissionID, nTimerID, nIntervalFrames)` — 3 THAM SỐ, timer LẶP VÔ HẠN
  (18 frame = 1 giây; citywar dùng (8, 18, 5*60*18) = 5 phút; đáo hạn chạy OnTimer() của
  script TimerTask.txt[nTimerID]) / `StopGlbMSTimer(nMissionID, nTimerID)` — 2 tham số;
  kèm `OpenGlbMission(id)` (autoexec gốc :142 — gọi InitMission() của missions.txt[id]).
- `SetGlbValue/GetGlbValue` (id 840-846 league_cityinfo dùng) — kho global value engine, chưa đăng ký.
- `SetMissionV/SetMissionS` — JX1 chỉ có `SetMission` + `GetMissionV/GetMissionS` → alias.
- `SetNpcDeathScript` (11 call site — trụ/cổng chết; THIẾU LÀ TRẬN KHÔNG PHÂN THẮNG BẠI),
  `AddObstacleObj/ClearObstacleObj/ClearMapObj/ClearMapNpc`, `SetPKFlag/ForbidChangePK`,
  `GetItemCountEx/DelItemEx`, `GetTongMaster`, `GetJoinTongTime`, `Sub/AddTongExp`,
  `GiveItemUI/GetGiveItemUnit`, `RemoveItemByIndex`, `SearchPlayer`, `DisabledUseTownP`,
  `LeaveChannel`, `RestoreOwnFeature`, `GetItemLife/GetItemProp`, `GetBonus`, `RevID2WXY`,
  `SetSiegeVoitureParam`, `SetMangonelParam`, `ConsumeEquiproomItem`, `NW_GetSealInfo`,
  `GetPlayerRev` (trả **2** giá trị), `GetLoop`.
  (Danh mục đích danh đầy đủ 163 hàm lập ở E2 — phụ lục PHULUC_HAM_CONGTHANH.md, tick từng hàm.)

## 8. LỊCH + THAM SỐ VẬN HÀNH GỐC (từ relay + ini)

- 5 pha/ngày: **0h00** dọn + tạo league 508/509 · **18h00** mở báo danh thành X (set league task 1=1,
  reset ladder 10261, AddLocalNews) · **19h00** chốt (bốc thăm GetRandomChallenger giữa các bang
  nộp Khiêu chiến lệnh nhiều nhất; thành VÔ CHỦ → AppointViceroy NGAY; có chủ → AppointChallenger;
  không ai báo danh → thông báo thái bình; hạ cờ về 0) · **20h00** khai chiến thành Y (mission map 221,
  90 phút → 21h30) · kết trận → NotifyWarResult.
- `TB_CITYWAR_ARRANGE` (thứ báo danh / thứ đánh): PT=T4/T5 · TĐ=T2/T3 · ĐL=T3/T4 · BK=T6/T7 ·
  TD=T5/T6 · DC=CN/T2 · LA=T7/CN.
- Kinh tế báo danh: nộp ITEM Khiêu chiến lệnh (6/1/1499) — KHÔNG trừ tiền quỹ (đường đấu giá tiền
  1M là code chết ở bản VN); hoàn lệnh 19h-24h khi thua bốc thăm.
- citywar.ini: MinTongLevel=18, MaxExchangeTax=20, MaxPriceParam=20, MinTongCrowNumber=37,
  StartSetTaxTime=22, EndSetTaxTime=23 (Thái Thú chỉ đổi thuế 22h-23h), WarCycleValue=7,
  SupplyLineBuildScale=30.
- Thưởng tuần bang chiếm thành: Thứ Hai 9h-18h mỗi thành viên 1 huyền tinh 5 (Đô thành BK/LA pool
  300 viên, thành thường 200), bang chủ nhận phần dư 18h-20h; top-10 trận nhận 3M exp; dụng cụ
  công thành đổi bằng Kim Nguyên Bảo (item 343 → 4 item 6-1-29..32).
