# -*- coding: utf-8 -*-
"""Gop api_gap_raw.json + bsig.json + chu thich TAY -> 04_api_gap.json / 04_api_gap.md"""
import io, os, sys, json

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
HERE = os.path.dirname(os.path.abspath(__file__))

raw = json.load(io.open(os.path.join(HERE, "api_gap_raw.json"), encoding="utf-8"))
bsig = json.load(io.open(os.path.join(HERE, "bsig.json"), encoding="utf-8"))

# ------------------------------------------------------------------ nhom D: KHONG phai ham engine
NHOM_D = {
    "ContriValueEntryLogic": (
        "**Hàm script, không phải engine.** `script/tong/contribution_entry.lua:190` gán "
        "`ContriValueEntryLogic = ContriValueEntryLogic_HEAD;` (thân hàm ở dòng 55). Bộ quét ban "
        "đầu chỉ bắt `function <tên>(` và `<tên> = function(` nên không thấy kiểu gán bí danh. "
        "Chuỗi `ContriValueEntryLogic` cũng không có trong ELF (`re_disasm --find` = *khong thay chuoi*). "
        "→ Chỉ cần port tệp `tong/contribution_entry.lua`."),
    "JudgePLAddTitle": (
        "**Lỗi của chính script JX2 gốc.** `battles/battlehead.lua:781` và `:783` gọi "
        "`JudgePLAddTitle()` nhưng cả cây Linux chỉ định nghĩa `bt_JudgePLAddTitle` "
        "(battlehead.lua:844). Không có trong ELF. → Khi port phải sửa thành "
        "`bt_JudgePLAddTitle()` hoặc bỏ đi, nếu không Lua báo *attempt to call global*."),
    "Require": (
        "**Hàm của client, không có ở máy chủ.** `protocol.lua:109` chỉ gọi trong nhánh "
        "`MODEL_GAMECLIENT == 1`; nhánh máy chủ (dòng 106) dùng `DynamicExecuteByPlayer`. "
        "Không có trong ELF máy chủ Linux. → **Không cần viết** cho GameServer."),
}

# ------------------------------------------------------------------ chu thich TAY nhom B
# do_tin_cay: CHẮC / KHÁ CHẮC / ĐOÁN
MAN = {
"Add120SkillExp": ("Add120SkillExp(nExp) → 1 số",
    "Cộng `nExp` vào **độ tu luyện kỹ năng cấp 120** của nhân vật hiện tại (lấy qua biến toàn cục "
    "`PlayerIndex`). Thân hàm có `lua_gettop` + 1 tham số số, có 3 nhánh `lua_pushnumber` "
    "(0 hoặc giá trị mới).",
    "KHÁ CHẮC"),
"AddStatData": ("AddStatData(szTên [, nSốLượng]) → 0 giá trị",
    "Cộng `nSốLượng` (mặc định 1) vào **bộ đếm thống kê** mang tên `szTên` của máy chủ. Thân hàm: "
    "`lua_gettop` phải bằng 1 hoặc 2 (`lea eax,[eax-1]; cmp eax,1; jbe`), `lua_tostring(L,1)`, "
    "nếu top==2 thì `lua_tonumber(L,2)` ngược lại dùng 1, rồi gọi `0x081D0420` trên đối tượng "
    "toàn cục `0x0978C0A0` với (tên, số, 0). Chuỗi `pcszName` nằm trong thân hàm. Phong Lăng Độ "
    "dùng để đếm số người chết từng thuyền (`fld_death.lua:30/32/34`).",
    "CHẮC"),
"BT_GetBattleParam": ("BT_GetBattleParam(nChỉSố) → 1 chuỗi",
    "Trả về tham số cấu hình thứ `nChỉSố` (đếm từ 1) của trận chiến hiện hành, dạng chuỗi "
    "`\"npcid,level,count\"` — `battlehead.lua:632` tách bằng `getNpcInfo(str)` ra 3 số. "
    "Thân hàm: lấy id trận qua `0x08106A40`, đọc vector chuỗi tại `[battle+0x4EF5C .. +0x4EF60]` "
    "bước 4 byte, rồi `lua_pushstring`.",
    "KHÁ CHẮC"),
"DropItemEx": ("DropItemEx(nSubWorldIdx, nMpsX, nMpsY, nBelonger, nVersion, szRandSeed, nQuality, "
    "nItemGenre, nItemDetailType, nItemParticularType, nItemLevel, nItemSeries, nItemLuck, "
    "nMagicLevel1..nMagicLevel6) → 1 số (chỉ số vật phẩm rơi, 0 = thất bại)",
    "Thả xuống đất một vật phẩm **đã xác định đầy đủ thuộc tính**. Chữ ký lấy từ **chính script**: "
    "`lib/droptemplet.lua:59` có dòng chú thích liệt kê đủ 19 tham số, và lời gọi thật ở dòng "
    "74–93 khớp đúng thứ tự đó. Lưu ý `szRandSeed` truyền bằng **chuỗi** "
    "(`format(\"%u\", ...)`) vì giá trị vượt phạm vi số nguyên có dấu.",
    "CHẮC"),
"File_Create": ("File_Create(szTênTệp) → 1 số",
    "Tạo tệp rỗng (dùng cho tệp .ini chưa tồn tại). `lib/file.lua:8` gọi ngay sau khi "
    "`IniFile_Load` trả 0.",
    "CHẮC"),
"FormatTime2Date": ("FormatTime2Date(nThờiGianUnix) → 1 số dạng YYYYMMDD",
    "Đổi mốc thời gian Unix sang số nguyên ngày. Thân hàm chứa chuỗi định dạng `%04d%02d%02d`. "
    "Kết quả đưa thẳng cho `ITEM_SetExpiredTime` (`.../qianqiu_yinglie/head.lua:200-201`).",
    "CHẮC"),
"GetFirstPlayerAtServer": ("GetFirstPlayerAtServer() → 1 số (PlayerIndex, 0 = hết)",
    "Bắt đầu duyệt **toàn bộ người chơi đang online** trên GameServer này. Thân hàm gọi "
    "`0x080C3EE0` trên đối tượng danh sách `0x08BAF320` rồi `lua_pushnumber`. Không tham số.",
    "CHẮC"),
"GetNextPlayerAtServer": ("GetNextPlayerAtServer() → 1 số (PlayerIndex, 0 = hết)",
    "Bước kế tiếp của vòng duyệt ở trên (`0x080C3F10`, cùng đối tượng). Cặp đôi với "
    "`GetFirstPlayerAtServer`; xem `missions/boss/bigboss.lua:289-294`.",
    "CHẮC"),
"GetGlodEqIndex": ("GetGlodEqIndex(nItemIndex) → 1 số (chỉ số hàng trong bảng trang bị HOÀNG KIM, 0 nếu không phải)",
    "Là một **thunk**: nạp hằng số 1 rồi nhảy vào hàm chung `0x080FEEB0`. Hàm chung: "
    "`lua_tonumber(L,1)` = chỉ số vật phẩm, kiểm `0 < idx < g_nItemCount` (`[0x0830CA5C]`), lấy "
    "bản ghi kích thước `0x368` từ bảng `[0x0830D300]`, **so sánh trường `[+4]` (= phẩm chất "
    "vật phẩm) với hằng số**; khớp thì trả chỉ số hàng, không khớp trả `0.0`. Hằng số 1 = hoàng "
    "kim, khớp với script `if nQuality == 1 then`.",
    "CHẮC"),
"GetPlatinaEquipIndex": ("GetPlatinaEquipIndex(nItemIndex) → 1 số (chỉ số hàng trong bảng trang bị BẠCH KIM, 0 nếu không phải)",
    "Cùng hàm chung `0x080FEEB0` như `GetGlodEqIndex` nhưng hằng số = 4 (bạch kim; script kiểm "
    "`if nQuality == 4`).",
    "CHẮC"),
"GetItemQuality": ("GetItemQuality(nItemIndex) → 1 số (0 = thường, 1 = hoàng kim, 4 = bạch kim)",
    "Đọc trường `[+4]` của bản ghi vật phẩm (bảng `[0x0830D300]`, bước `0x368`). Trả 0 khi chỉ số "
    "không hợp lệ.",
    "CHẮC"),
"GetPlatinaLevel": ("GetPlatinaLevel(nItemIndex) → 1 giá trị: số, hoặc **nil** nếu vật phẩm không phải bạch kim",
    "Kiểm `[+4] == 4`; nếu không thì gọi `lua_pushnil` (`0x08232E70`) mà **vẫn trả 1 giá trị**. "
    "Script bọc bằng `tostring(...)` nên nil vẫn an toàn (`lib/log.lua:43`).",
    "CHẮC"),
"GetItemAllParams": ("GetItemAllParams(nItemIndex) → 1 **bảng Lua** {nMagLvl1..nMagLvl6}",
    "Tạo bảng mới (`lua_newtable` = `0x08232BE0`) rồi đổ 6 giá trị liên tiếp từ bản ghi vật phẩm "
    "ở độ lệch `+0x1E0`, bước 4 byte. Trả 0 giá trị nếu chỉ số không hợp lệ. Script đọc "
    "`arynMagLvl[1..6]` (`lib/log.lua:46,50`).",
    "KHÁ CHẮC"),
"GetItemGenTime": ("GetItemGenTime(nItemIndex) → 1 số (mốc thời gian tạo vật phẩm)",
    "Đọc trường thời gian sinh của bản ghi vật phẩm. Script in bằng `%0.0f` vì giá trị vượt phạm "
    "vi 32-bit có dấu (`lib/log.lua:51`).",
    "KHÁ CHẮC"),
"ITEM_GetItemRandSeed": ("ITEM_GetItemRandSeed(nItemIndex) → 1 số (hạt giống ngẫu nhiên, không dấu)",
    "Đọc RandSeed của vật phẩm; script in bằng `%0.0f`. Cặp đôi với tham số `szRandSeed` của "
    "`DropItemEx`.",
    "KHÁ CHẮC"),
"ITEM_GetExpiredTime": ("ITEM_GetExpiredTime(nItemIndex) → 1 số (nil hoặc ≤ 0 = không có hạn)",
    "Đọc hạn sử dụng còn lại của vật phẩm. `activitysys/activity.lua:314-317` coi nil hoặc ≤ 0 là "
    "\"không có hạn\".",
    "KHÁ CHẮC"),
"ITEM_SetExpiredTime": ("ITEM_SetExpiredTime(nItemIndex, nGiáTrị [, nKiểu]) → 1 số",
    "Đặt hạn sử dụng cho vật phẩm. **CẢNH BÁO**: hai chỗ gọi trong bao đóng dùng **hai đơn vị "
    "khác nhau** — `event/.../qianqiu_yinglie/head.lua:201` truyền YYYYMMDD (kết quả "
    "`FormatTime2Date`), còn `missions/fengling_ferry/bossdeath.lua:35` truyền **số phút** còn "
    "lại trong ngày. Thân hàm đọc 3 tham số số và có 4 lần `lua_pushnumber`, nhiều khả năng tham "
    "số thứ 3 (có mặc định) chọn đơn vị. **Phải đối chiếu lại trước khi hiện thực.**",
    "KHÁ CHẮC (đơn vị của tham số 2: CHƯA XÁC MINH)"),
"ITEM_SetLeftUsageTime": ("ITEM_SetLeftUsageTime(nItemIndex, nThờiGian) → 1 số",
    "Đặt thời gian sử dụng còn lại của vật phẩm. `lib/droptemplet.lua:135,140`.",
    "KHÁ CHẮC"),
"ITEM_DropRateItem": ("ITEM_DropRateItem(nNpcIndex, nSốLần, szĐườngDẫnIni, nThamSố4, nThamSố5, nNgũHành) → 0 giá trị",
    "Thả vật phẩm theo **bảng tỉ lệ trong tệp .ini** (ví dụ "
    "`\\\\settings\\\\droprate\\\\boss\\\\bosstask_lev90.ini`) tại vị trí NPC. Thân hàm: "
    "`lua_gettop` + 6 tham số (số, số, chuỗi, số, số, số), không đẩy giá trị trả về. Chỗ gọi duy "
    "nhất: `task/tollgate/killer/kill_level.lua:69` → "
    "`ITEM_DropRateItem(nNpcIndex, 8, ini, 0, 10, nseries)`.",
    "KHÁ CHẮC"),
"IniFile_SetData": ("IniFile_SetData(szTệp, szSection, szKey, szValue) → 0 giá trị",
    "Ghi một khoá vào ảnh .ini **đã nạp trong bộ nhớ** (chưa ghi xuống đĩa). Cặp đôi với "
    "`IniFile_GetData` / `IniFile_Load` mà JX1 **đã có** (ScriptFuns.cpp:15288–15289).",
    "CHẮC"),
"IniFile_Save": ("IniFile_Save(szTệpNguồn, szTệpĐích) → 1 số",
    "Ghi ảnh .ini trong bộ nhớ xuống đĩa. `lib/file.lua:24` gọi với cùng một tên cho cả hai tham số.",
    "CHẮC"),
"GetNpcAroundPlayerList": ("GetNpcAroundPlayerList(nNpcIndex, nBánKính [, nThamSố3]) → **2 giá trị**: (bảng PlayerIndex, nSốLượng)",
    "Liệt kê người chơi trong bán kính `nBánKính` quanh NPC. Thân hàm có `lua_gettop` + 3 tham số "
    "số và trả 2 giá trị (`mov eax,2; ret`). Chỗ gọi: `missions/boss/bigboss.lua:218` → "
    "`local tbRoundPlayer, nCount = GetNpcAroundPlayerList(nNpcIndex, 20)` (chỉ dùng 2 tham số ⇒ "
    "tham số 3 có mặc định).",
    "KHÁ CHẮC"),
"NpcDropMoney": ("NpcDropMoney(nNpcIndex, nSốTiền, nBelongerIndex) → 0 giá trị",
    "Thả tiền tại vị trí NPC, quyền sở hữu thuộc về `nBelongerIndex`. `lib/droptemplet.lua:159` "
    "dùng cho Kim Nguyên Bảo (biến `nAddJxb`).",
    "CHẮC"),
"NPCINFO_GetSeries": ("NPCINFO_GetSeries(nNpcIndex) → 1 số (ngũ hành của NPC)",
    "Trả hệ ngũ hành của NPC. `task/tollgate/killer/kill_level.lua:37` lấy rồi truyền xuống "
    "`ITEM_DropRateItem` làm tham số thứ 6.",
    "CHẮC"),
"NpcName2Replace": ("NpcName2Replace(szTênNpc) → 1 chuỗi",
    "Đổi tên NPC qua **bảng thay thế** (bản địa hoá / bí danh). Thân hàm: `lua_tostring(L,1)`, "
    "2 nhánh `lua_pushstring`. Chỗ gọi có kiểm tồn tại trước: "
    "`missions/challengeoftime/npc/transfer.lua:10` → `if NpcName2Replace then ... end` ⇒ **có thể "
    "tạm bỏ qua**, không bắt buộc khai báo.",
    "KHÁ CHẮC"),
"SetItemBindState": ("SetItemBindState(nItemIndex, nBindState) → 0 hoặc 1 giá trị",
    "Đặt trạng thái **khoá (binding)** của vật phẩm; `-2` = khoá "
    "(`battles/battlehead.lua:1317` → `SetItemBindState(ItemIdx, -2);` kèm chú thích *绑定*).",
    "CHẮC"),
"GetRoomItems": ("GetRoomItems(nRoomType) → 1 **bảng** chỉ số vật phẩm",
    "Trả danh sách chỉ số vật phẩm trong một \"phòng\" (túi / trang bị / rương) theo `nRoomType`. "
    "`lib/composeex.lua:191` — nhánh `else` của trường hợp ô giao dịch (nhánh kia dùng "
    "`GetGiveItemUnit` mà JX1 đã có).",
    "KHÁ CHẮC"),
"Tm2Time": ("Tm2Time(nNăm, nTháng, nNgày, nGiờ, nPhút, nGiây) → 1 số (thời gian Unix)",
    "Hàm **ngược** của `Time2Tm` (JX1 đã có — ScriptFuns.cpp:14891). 6 tham số số, có `lua_gettop` "
    "để xử lý thiếu tham số. `activitysys/functionlib.lua:364,380`.",
    "CHẮC"),
"TrimString": ("TrimString() → 0 giá trị",
    "Cắt khoảng trắng và TAB ở **hai đầu** của chuỗi trong **bộ đệm chuỗi dùng chung** (biến toàn "
    "cục `0x09780D54` = con trỏ, `0x09780D58` = độ dài) — đúng bộ đệm mà `PushString` / "
    "`AppendString` / `PopString` dùng. Không tham số, không trả giá trị. Cách dùng: "
    "`PushString(str); TrimString(); return PopString()` (`lib/string.lua:162-164`). JX1 **đã có** "
    "`AppendString` (15218) và `ReplaceString` (15219) ⇒ viết `TrimString` rất rẻ.",
    "CHẮC"),
"ST_IsTransLife": ("ST_IsTransLife() → 1 số (0 / 1)",
    "Nhân vật hiện tại **đã chuyển sinh hay chưa**. Thân hàm đọc đúng một byte tại "
    "`player+0x86B8` rồi `setne`. **Chú ý**: `ST_GetTransLifeCount` (`0x081C1100`, JX1 đã có, ánh "
    "xạ tới `LuaGetPlayerReBornValue`) đọc **chính byte đó**. ⇒ Trong JX1 chỉ cần "
    "`Lua_PushNumber(L, Player[i].m_cReBorn.GetReBornValue() > 0 ? 1 : 0)`.",
    "CHẮC"),
"ST_DoTransLife": ("ST_DoTransLife() → 1 số (0/1 thành công)",
    "Thực hiện chuyển sinh cho nhân vật hiện tại (gọi `0x080B2180` trên bản ghi người chơi, "
    "`setne` → 0/1). Không tham số. `task/metempsychosis/translife_6.lua:102`.",
    "KHÁ CHẮC"),
"ST_LevelUp": ("ST_LevelUp(nSốCấp) → 0 giá trị",
    "Nâng cấp độ nhân vật thêm `nSốCấp` (dùng sau chuyển sinh). "
    "`vng_event/thapnienlenhbai/lenhbai_def.lua:378` → `ST_LevelUp(nLevel - GetLevel())`.",
    "KHÁ CHẮC"),
"PET_GetUpgradePoint": ("PET_GetUpgradePoint() → 1 số",
    "Đọc **điểm thăng cấp** thú cưng của nhân vật hiện tại (một trường trong bản ghi người chơi; "
    "mảng gốc `[0x08BAEE60]`, bước `0x8788`). Không tham số.", "KHÁ CHẮC"),
"PET_SetUpgradePoint": ("PET_SetUpgradePoint(nGiáTrị) → 0 giá trị",
    "Đặt điểm thăng cấp thú cưng rồi đồng bộ (gọi hàm thông báo `0x081D4CE0`).", "KHÁ CHẮC"),
"PET_GetGrownPoint": ("PET_GetGrownPoint() → 1 số",
    "Đọc **điểm tăng trưởng** thú cưng; trường tại `player+0x873C`.", "KHÁ CHẮC"),
"PET_SetGrownPoint": ("PET_SetGrownPoint(nGiáTrị) → 0 giá trị",
    "Ghi `player+0x873C` rồi đồng bộ (`0x081D4CE0`).", "KHÁ CHẮC"),
"PET_GetTamePoint": ("PET_GetTamePoint() → 1 số",
    "Đọc **điểm tu luyện / thuần hoá** thú cưng (trường kề bên, `player+0x8738`).", "KHÁ CHẮC"),
"PET_SetTamePoint": ("PET_SetTamePoint(nGiáTrị) → 0 giá trị",
    "Ghi điểm tu luyện thú cưng rồi đồng bộ.", "KHÁ CHẮC"),
"IsDisabledUseHeart": ("IsDisabledUseHeart() → 1 số (0/1)  ·  **tham số bị bỏ qua**",
    "Trả 1 nếu nhân vật hiện tại **bị cấm** dùng \"Tâm Tâm Tương Ánh Phù\". Thân hàm **không đọc "
    "tham số Lua nào cả**: nó lấy biến toàn cục `PlayerIndex` (`0x08107860`), gọi `0x080A8C80` lấy "
    "cờ trạng thái rồi `test eax, 0x200000`. Script vẫn truyền `PlayerIndex` "
    "(`item/heart_head.lua:116`) nhưng engine bỏ qua.",
    "CHẮC"),
"QueryWiseManForSB": ("QueryWiseManForSB(szCallbackOK, szCallbackFail, nThờiGianChờ, szTênNhânVật) → 0 giá trị",
    "Hỏi máy chủ trung tâm (WiseMan / Bishop) về **một nhân vật khác** rồi gọi lại hàm Lua "
    "`szCallbackOK` / `szCallbackFail`. Thân hàm: `lua_gettop`, 4 tham số (chuỗi, chuỗi, số, "
    "chuỗi), chứa chuỗi định dạng `|-|%d` và `|-|%s` (ghép gói tin). "
    "`item/heart_head.lua:129,132`.",
    "KHÁ CHẮC"),
"SendScriptData": ("SendScriptData(nProtocolId, nHandle) → 1 số",
    "Gửi gói dữ liệu script (handle của objbuffer) **xuống client** theo mã giao thức "
    "`nProtocolId`. `script_protocol/protocol_def_gs.lua:193` trong `ScriptProtocol:SendData`. Là "
    "mặt kia của hệ `OB_*` mà JX1 đã có (`OB_Create`, `OB_PushInt`…). Thân hàm có `lua_gettop` + "
    "2 tham số số, 3 lần `lua_pushnumber`, trả 1 giá trị.",
    "KHÁ CHẮC"),
"OpenProgressBar": ("OpenProgressBar(szTiêuĐề, nSốKhung, nCờSựKiện, bDesc, szHàmOnTime, szHàmOnBreak) → 0 giá trị",
    "Mở **thanh tiến trình** trên client; chạy hết thì gọi hàm Lua `szHàmOnTime`, bị ngắt thì gọi "
    "`szHàmOnBreak`. Thân hàm đọc tới 7 tham số (chuỗi, số, số, số, chuỗi, chuỗi, chuỗi) nhưng "
    "chỗ gọi thật chỉ dùng 6 (`lib/progressbar.lua:95`) ⇒ tham số 7 có mặc định. `nCờSựKiện` là "
    "mặt nạ bit dựng bằng `SetBit` (`progressbar.lua:86`).",
    "KHÁ CHẮC"),
}

# ------------------------------------------------------------------ nhom C
NHOM_C = {
"BT_GetGameData": ("ScriptFuns.cpp:14305 → LuaBT_GetDataStub (ScriptFuns.cpp:2265)",
    "**Tên giống, nghĩa khác.** JX1 có đăng ký nhưng thân hàm là STUB: "
    "`Lua_PushNumber(L, 0); return 1;` — **luôn trả 0**. Bản JX2 (`0x081C6230`) đọc kho dữ liệu "
    "trận chiến thật. Phong Lăng Độ và Vượt Ải đều kéo `battles/battlehead.lua` vào bao đóng và "
    "đọc rất nhiều trường qua `BT_GetGameData` ⇒ mọi nhánh đó sẽ chạy sai **một cách im lặng**."),
"BT_GetData": ("ScriptFuns.cpp:14306 → LuaBT_GetDataStub (ScriptFuns.cpp:2265)",
    "Như trên, **cùng một stub**. Ví dụ `battlehead.lua:769-771` so sánh "
    "`BT_GetData(PL_BATTLEPOINT)` ⇒ mọi người chơi đều ra 0."),
"TM_SetTimer": ("ScriptFuns.cpp:14303 → LuaTM_SetTimer (ScriptFuns.cpp:2251)",
    "**Stub có chủ đích**: `Lua_PushNumber(L, 0); return 1;` — không tạo hẹn giờ. Bản JX2 "
    "(`0x081C23C0`) tạo timer thật."),
"TM_GetRestCount": ("ScriptFuns.cpp:14304 → LuaTM_GetRestCount (ScriptFuns.cpp:2257)",
    "**Stub**: `Lua_PushNil(L); return 1;` — trả nil để hệ storm tự tắt (ghi chú ngay trên hàm, "
    "ScriptFuns.cpp:2247-2249). Bản JX2 tại `0x081C21A0`."),
"curpack": ("ScriptFuns.cpp:14299 → LuaCurPack (ScriptFuns.cpp:2186)",
    "**Stub** trả 0. Ghi chú trong nguồn: JX1 mỗi tệp .lua một `Lua_State` riêng nên khái niệm "
    "\"pack\" của máy ảo bản Linux không tồn tại."),
"usepack": ("ScriptFuns.cpp:14300 → LuaUsePack (ScriptFuns.cpp:2192)",
    "**Stub** trả 0, cùng lý do với `curpack`."),
"SetNpcScript / SetNpcDeathScript": ("ScriptFuns.cpp:14500 / :15190",
    "**CHƯA XÁC MINH.** `jx_linux_y.luamap.full.txt` dòng 152–153 gán **cả hai tên vào cùng một "
    "địa chỉ `0x08101500`**. JX1 lại tách thành hai hàm C khác nhau (`LuaSetNpcActionScript` và "
    "`LuaSetNpcDeathScript`). Hoặc công cụ lập luamap gán trùng, hoặc bản JX2 dùng một hàm chung. "
    "Phải kiểm lại bằng tay trước khi tin rằng hai tên này khác nghĩa."),
"JoinMission": ("KHÔNG có trong bảng đăng ký JX1 — luamap 0x08137E40",
    "**Cảnh báo trùng tên.** Engine JX2 có hàm `JoinMission` (luamap dòng 917), **nhưng** bao "
    "đóng của Vượt Ải lại tự định nghĩa một hàm Lua cùng tên ở "
    "`missions/challengeoftime/mission_match.lua:93` → `function JoinMission(RoleIndex, camp)`. "
    "Trong Lua, bản định nghĩa sau sẽ **đè lên** hàm engine. Chỗ gọi "
    "`missions/challengeoftime/npc/dragonboat_main.lua:163` → `JoinMission(MISSION_MATCH, 1)` "
    "truyền **mission id** chứ không phải RoleIndex ⇒ rất có thể dòng này muốn gọi hàm **engine**. "
    "Phải xác minh khi port, nếu không sẽ sai âm thầm."),
}

NONE_ENGINE = set(NHOM_D.keys())

# ------------------------------------------------------------------ dung JSON
out = {}
for n, v in sorted(raw.items()):
    if n in NONE_ENGINE:
        nhom, sig, mota, tin = "D", None, NHOM_D[n], "CHẮC"
    elif v["nhom"] == "B":
        m = MAN.get(n)
        nhom = "B"
        sig, mota, tin = (m if m else (None, None, "CHƯA PHÂN TÍCH"))
    else:
        nhom = "A"
        sig = mota = None
        tin = None
    e = {
        "nhom": nhom,
        "dia_chi": v["dia_chi_linux"],
        "chu_ky": sig,
        "mo_ta": mota,
        "do_tin_cay": tin,
        "dang_ky_jx1": v["dang_ky_jx1"],
        "so_lan_goi": v["so_lan_goi"],
        "dung_boi": v["dung_boi"],
        "trong_tep_loi": v["trong_seed"],
        "vi_tri_goi": v["vi_tri_goi"],
    }
    if bsig.get(n):
        e["disasm"] = bsig[n]
    out[n] = e
for k, (reg, txt) in NHOM_C.items():
    if k not in out:
        out[k] = {"nhom": "C", "dia_chi": None, "chu_ky": None,
                  "mo_ta": txt, "do_tin_cay": "CẢNH BÁO", "dang_ky_jx1": reg,
                  "so_lan_goi": 0, "dung_boi": [], "trong_tep_loi": [], "vi_tri_goi": []}
    out[k]["canh_bao_nhom_C"] = txt

json.dump(out, io.open(os.path.join(HERE, "04_api_gap.json"), "w", encoding="utf-8"),
          ensure_ascii=False, indent=1, sort_keys=True)

A = sorted(n for n, v in out.items() if v["nhom"] == "A")
B = sorted(n for n, v in out.items() if v["nhom"] == "B")
Dd = sorted(n for n, v in out.items() if v["nhom"] == "D")
print("A=%d  B=%d  C=%d  D=%d  | tong ten engine quet duoc = %d"
      % (len(A), len(B), len(NHOM_C), len(Dd), len(A) + len(B) + len(Dd)))
print("B:", ", ".join(B))
